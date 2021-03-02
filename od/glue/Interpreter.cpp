#include <od/glue/Interpreter.h>
//#define BUILDOPT_VERBOSE
//#define BUILDOPT_DEBUG_LEVEL 10
#include <hal/log.h>
#include <hal/timing.h>
#include <string.h>
#include <stdlib.h>

extern "C"
{
#include "lauxlib.h"
#include "lualib.h"
}

namespace od
{

  static inline size_t nextPowerOfTwo(size_t v)
  {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
  }

  Interpreter::Interpreter()
  {
    luaL_set_writestring_function(writeStringCallback);
    luaL_set_writestringerror_function(writeStringErrorCallback);
  }

  Interpreter::~Interpreter()
  {
  }

  void Interpreter::init(void)
  {
    L = lua_newstate(Interpreter::reallocateCallback, this);
  }

  void Interpreter::onError(const char *format, ...)
  {
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    logError("%s", buffer);

    if (!lua_isnil(L, -1))
    {
      logError("%s", lua_tostring(L, -1));
      lua_pop(L, 1);
    }
  }

  bool Interpreter::executeFile(const char *filename)
  {
    if (luaL_loadfile(L, filename))
    {
      onError("Interpreter::executeFile(%s): Error loading file", filename);
      return false;
    }

    if (lua_pcall(L, 0, 0, 0))
    {
      onError("Interpreter::executeFile(%s): Error in pcall", filename);
      return false;
    }

    return true;
  }

  bool Interpreter::execute(const char *code)
  {
    logInfo("Executing: %s", code);
    if (luaL_loadstring(L, code))
    {
      onError("Interpreter::executeString: Failed to load string");
      return false;
    }

    int result = lua_pcall(L, 0, LUA_MULTRET, 0);
    switch (result)
    {
    case 0: // no error
      logDebug(1, "no error");
      return true;
    case LUA_ERRRUN:
      onError("Interpreter::executeString: Error in pcall, runtime error.");
      return false;
    case LUA_ERRMEM:
      onError(
          "Interpreter::executeString: Error in pcall, memory allocation error.");
      return false;
    case LUA_ERRERR:
      onError(
          "Interpreter::executeString: Error in pcall, error while running error handler.");
      return false;
    }

    return true;
  }

  void *Interpreter::reallocateCallback(void *ud, void *ptr, size_t osize, size_t nsize)
  {
#ifndef BUILDOPT_LUA_USE_REALLOC
    Interpreter *interp = (Interpreter *)ud;
    void *ptr2 = interp->reallocate(ptr, osize, nsize);
    if (nsize > 0 && ptr2 == NULL)
    {
      logWarn("Interpreter::realloc: out of memory (osize=%d, nsize=%d)", osize, nsize);
    }
    return ptr2;
#else
    return realloc(ptr, nsize);
#endif
  }

  void Interpreter::writeStringCallback(const char *buffer, size_t sz)
  {
    logWrite(buffer, sz);
  }

  void Interpreter::writeStringErrorCallback(const char *s1, const char *s2)
  {
    logError(s1, s2);
  }

  void Interpreter::printMemoryStats()
  {
#if INTERPRETER_RECORD_ALLOCATIONS
    logInfo("bytes\tcur\tmax\tcnt");
    for (auto const &kv : mRecords)
    {
      logInfo("%d:\t%d\t%d\t%d", kv.first, kv.second.current,
              kv.second.max, kv.second.count);
    }

    float totalSecs = ticks2secs(mTotalElapsed);
    logInfo("Total Count: %d ", mTotalCount);
    logInfo("Total Time: %0.3f secs ", totalSecs);
    logInfo("Avg Time per Allocation: %0.3f ms",
            1000 * totalSecs / mTotalCount);
    logInfo("Misses = %d ", mMisses);
#else
    logInfo("Allocation recording disabled.");
    logInfo("Misses = %d", mMisses);
#endif
  }

#ifndef BUILDOPT_LUA_USE_REALLOC

  void *Interpreter::getBuffer(size_t size)
  {
    void *ptr = mAllocator.get(size);
    if (ptr)
    {
      return ptr;
    }
    mMisses++;
    return ::malloc(size);
  }

  void Interpreter::releaseBuffer(void *ptr)
  {
    if (!mAllocator.release((uint8_t *)ptr))
    {
      // Belongs to malloc.
      ::free(ptr);
    }
  }

  void *Interpreter::allocate(size_t size)
  {
#if INTERPRETER_RECORD_ALLOCATIONS
    size_t v = nextPowerOfTwo(size);
    Record &rec = mRecords[v];
    rec.count++;
    rec.current++;
    if (rec.current > rec.max)
    {
      rec.max = rec.current;
    }
    void *result;
    volatile tick_t start = ticks();
    result = getBuffer(size);
    mTotalElapsed += ticks() - start;
    mTotalCount++;
    return result;
#else
    return getBuffer(size);
#endif
  }

  void Interpreter::deallocate(void *ptr, size_t size)
  {
#if RECORD_ALLOCATIONS
    Record &rec = mRecords[nextPowerOfTwo(size)];
    rec.current--;
    volatile tick_t start = ticks();
    releaseBuffer(ptr);
    mTotalElapsed += ticks() - start;
#else
    releaseBuffer(ptr);
#endif
  }

  void *Interpreter::reallocate(void *ptr, size_t osize, size_t nsize)
  {
    if (nsize == 0)
    {
      if (ptr && osize > 0)
      {
        //debug_printf("Lua: freeing %d bytes.",osize);
        deallocate(ptr, osize);
      }
      return NULL;
    }
    else
    {
      if (ptr)
      {
        size_t asize = mAllocator.getActualSize((uint8_t *)ptr);
        if (asize == 0)
        {
          // Belongs to malloc.
          return ::realloc(ptr, nsize);
        }
        if (osize > nsize)
        {
          // shrinking
          return mAllocator.shrink((uint8_t *)ptr, nsize);
        }
        else if (osize < nsize)
        {
          // growing
          if (nsize <= asize)
          {
            // do nothing
            return ptr;
          }
          else
          {
            uint8_t *ptr2 = (uint8_t *)allocate(nsize);
            if (ptr2)
            {
              memcpy(ptr2, ptr, osize);
              deallocate(ptr, osize);
              return ptr2;
            }
            else
            {
              // out of memory
              return NULL;
            }
          }
        }
        else
        {
          // nothing to do
          return ptr;
        }
      }
      else
      {
        return allocate(nsize);
      }
    }
  }

#endif
  /*

  Results of measureSpeed() on AM335X.
  
 C-->Lua: 579 ticks per call 0.578949 us per call
 Lua-->C: 153 ticks per call 0.153261 us per call
 Lua-->C(swig): 193 ticks per call 0.193224 us per call
 Lua-->Lua(global): 289 ticks per call 0.289135 us per call
 Lua-->Lua(local): 243 ticks per call 0.243605 us per call
 C-->C: 4 ticks per call 0.004197 us per call
 Lua(z=y+x*z): 154 ticks per call 0.153860 us per call
 C(z=y+x*z): 27 ticks per call 0.027974 us per call
 
 */

#if 0


  bool Interpreter::genericCall(const char *func, const char *sig, ...)
  {
    va_list vl;
    int narg, nres;
    int top = lua_gettop(L);

    va_start(vl, sig);
    lua_getglobal(L, func);

    // push args
    for (narg = 0; *sig; narg++)
    {
      luaL_checkstack(L, 1, "too many arguments");
      switch (*sig++)
      {
      case 'g': // named global
        lua_getglobal(L, va_arg(vl, char *));
        break;
      case 'f': // float/double
        lua_pushnumber(L, (float)va_arg(vl, double));
        break;
      case 'd': // int
        lua_pushinteger(L, va_arg(vl, int));
        break;
      case 's': // string
        lua_pushstring(L, va_arg(vl, char *));
        break;
      case '>': // end of args
        goto endargs;
      default:
        logError("Interpreter::genericCall(%s): Invalid option (%c)",
                 func, *(sig - 1));
        goto failed;
      }
    }

  endargs:

    nres = strlen(sig);
    if (lua_pcall(L, narg, nres, 0) != 0)
    {
      onError("Error in '%s'", func);
      goto failed;
    }

    // retrieve results

    nres = -nres;
    while (*sig)
    {
      switch (*sig++)
      {
      case 'f':
      { // float
        int isnum;
        float n = lua_tonumberx(L, nres, &isnum);
        if (!isnum)
        {
          logError(
              "Interpreter::genericCall(%s): wrong result type (%c)",
              func, *(sig - 1));
          goto failed;
        }
        *va_arg(vl, float *) = n;
        lua_pop(L, 1);
        break;
      }
      case 'd':
      { // int
        int isnum;
        int n = lua_tointegerx(L, nres, &isnum);
        if (!isnum)
        {
          logError(
              "Interpreter::genericCall(%s): wrong result type (%c)",
              func, *(sig - 1));
          goto failed;
        }
        *va_arg(vl, int *) = n;
        lua_pop(L, 1);
        break;
      }
      case 's':
      { // string
        const char *s = lua_tostring(L, nres);
        if (s == NULL)
        {
          logError(
              "Interpreter::genericCall(%s): wrong result type (%c)",
              func, *(sig - 1));
          goto failed;
        }
        *va_arg(vl, const char **) = s;
        lua_pop(L, 1);
        break;
      }
      default:
        logError("Interpreter::genericCall(%s): Invalid option (%c)",
                 func, *(sig - 1));
        goto failed;
      }
      nres++;
    }

    va_end(vl);
    return true;

  failed:
    va_end(vl);
    lua_settop(L, top);
    return false;
  }

  static int noopC(lua_State *L)
  {
    lua_pushinteger(L, 1);
    return 1;
  }
  
  void Interpreter::measureSpeed(void)
  {
    int i, res;
    tick_t start;
    tick_t elapsed;
    int tickspercall;
    float microsecspercall;

    lua_pushcfunction(L, noopC);
    lua_setglobal(L, "noopC");
    loadString(ramdisk_lookup_as_string("test/benchmark.lua"));

    while (1)
    {
      logInfo("----------------------------");

      // C--> Lua
      start = ticks();
      for (i = 0; i < 100000; i++)
      {
        if (!genericCall("noopLua", ">d", &res))
          break;
        if (!genericCall("noopLua", ">d", &res))
          break;
        if (!genericCall("noopLua", ">d", &res))
          break;
        if (!genericCall("noopLua", ">d", &res))
          break;
        if (!genericCall("noopLua", ">d", &res))
          break;
        if (!genericCall("noopLua", ">d", &res))
          break;
        if (!genericCall("noopLua", ">d", &res))
          break;
        if (!genericCall("noopLua", ">d", &res))
          break;
        if (!genericCall("noopLua", ">d", &res))
          break;
        if (!genericCall("noopLua", ">d", &res))
          break;
      }
      elapsed = ticks() - start;
      tickspercall = elapsed / 1000000;
      microsecspercall = ticks2secs(elapsed);
      logInfo("C-->Lua: %d ticks per call %f us per call",
              tickspercall, microsecspercall);

      // Lua --> C
      start = ticks();
      genericCall("benchmarkLocal", "gd>d", "noopC", 1000000, &res);
      elapsed = ticks() - start;
      tickspercall = elapsed / 1000000;
      microsecspercall = ticks2secs(elapsed);
      logInfo("Lua-->C: %d ticks per call %f us per call",
              tickspercall, microsecspercall);

      // Lua --> C via SWIG
      start = ticks();
      genericCall("benchmarkLocal", "gd>d", "noopSwig", 1000000, &res);
      elapsed = ticks() - start;
      tickspercall = elapsed / 1000000;
      microsecspercall = ticks2secs(elapsed);
      logInfo("Lua-->C(swig): %d ticks per call %f us per call",
              tickspercall, microsecspercall);

      // Lua --> Lua (global)
      start = ticks();
      genericCall("benchmarkGlobal", "d>d", 1000000, &res);
      elapsed = ticks() - start;
      tickspercall = elapsed / 1000000;
      microsecspercall = ticks2secs(elapsed);
      logInfo("Lua-->Lua(global): %d ticks per call %f us per call",
              tickspercall, microsecspercall);

      // Lua --> Lua (local)
      start = ticks();
      genericCall("benchmarkLocal", "gd>d", "noopLua", 1000000, &res);
      elapsed = ticks() - start;
      tickspercall = elapsed / 1000000;
      microsecspercall = ticks2secs(elapsed);
      logInfo("Lua-->Lua(local): %d ticks per call %f us per call",
              tickspercall, microsecspercall);

      // C --> C
      start = ticks();
      for (i = 0; i < 100000; i++)
      {
        noopSwig();
        noopSwig();
        noopSwig();
        noopSwig();
        noopSwig();
        noopSwig();
        noopSwig();
        noopSwig();
        noopSwig();
        noopSwig();
      }
      elapsed = ticks() - start;
      tickspercall = elapsed / 1000000;
      microsecspercall = ticks2secs(elapsed);
      logInfo("C-->C: %d ticks per call %f us per call", tickspercall,
              microsecspercall);

      // Lua (mult-accumulate)
      start = ticks();
      genericCall("benchmarkMulAcc", "d>d", 1000000, &res);
      elapsed = ticks() - start;
      tickspercall = elapsed / 1000000;
      microsecspercall = ticks2secs(elapsed);
      logInfo("Lua(z=y+x*z): %d ticks per call %f us per call",
              tickspercall, microsecspercall);

      // C (mult-accumulate)
      start = ticks();
      {
        float x = 0.99f;
        float y = 2.0f;
        float z = 0.0f;
        for (i = 0; i < 250000; i++)
        {
          z = y + x * z;
          z = y + x * z;
          z = y + x * z;
          z = y + x * z;
        }
        res = z;
      }
      elapsed = ticks() - start;
      tickspercall = elapsed / 1000000;
      microsecspercall = ticks2secs(elapsed);
      logInfo("C(z=y+x*z): %d ticks per call %f us per call",
              tickspercall, microsecspercall);

      Task_sleep(100);
    }
  }
#endif
} /* namespace od */
