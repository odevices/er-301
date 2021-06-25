#include <od/glue/ExpressionInterpreter.h>
#include <od/objects/Parameter.h>
#include <hal/log.h>

extern "C"
{
#include "lauxlib.h"
#include "lualib.h"

  /*
** these libs are loaded by lua.c and are readily available to any Lua
** program
*/
  static const luaL_Reg loadedlibs[] = {
      {"_G", luaopen_base},
      {LUA_MATHLIBNAME, luaopen_math},
      {NULL, NULL}};

  static void openStandardlibs(lua_State *L)
  {
    const luaL_Reg *lib;
    /* "require" functions from 'loadedlibs' and set results to global table */
    for (lib = loadedlibs; lib->func; lib++)
    {
      luaL_requiref(L, lib->name, lib->func, 1);
      lua_pop(L, 1); /* remove lib */
    }
  }
} // extern "C"

namespace od
{

  ExpressionInterpreter::ExpressionInterpreter(bool init)
  {
    if (init)
    {
      logInfo("Initializing ExpressionInterpreter");
      Interpreter::init();

      lua_gc(L, LUA_GCGEN, 0, 0);

#ifndef BUILDOPT_LUA_USE_REALLOC
      const int multiplier = 2;
      mAllocator.add(16, 1500 * multiplier);
      mAllocator.add(32, 3500 * multiplier);
      mAllocator.add(64, 2500 * multiplier);
      mAllocator.add(128, 600 * multiplier);
      mAllocator.add(256, 200 * multiplier);
      mAllocator.add(512, 150 * multiplier);
      mAllocator.add(1024, 75 * multiplier);
      mAllocator.add(2048, 15 * multiplier);
#endif

      openStandardlibs(L);

      // A table to hold functions.  Pre-allocate 64 entries.
      // (Should never be popped and thus always at the bottom of the stack)
      lua_createtable(L, 64, 0);
      lua_setglobal(L, "TheFunctionTable");
      lua_getglobal(L, "TheFunctionTable");
      mFunctionTable = lua_gettop(L);
    }
  }

  ExpressionInterpreter::~ExpressionInterpreter()
  {
  }

  bool ExpressionInterpreter::compile(Expression &e)
  {
    std::string tmp("return ");
    tmp += e.mFunction;
    const char *buff = tmp.c_str();
    size_t sz = tmp.length();

    disable();

    // Compile the function string into a function object.
    if (luaL_loadbufferx(L, buff, sz, "expression", NULL))
    {
      onError("Failed to load expression string.");
      enable();
      return false;
    }

    // Execute the returned function object (we should get another function)
    if (lua_pcall(L, 0, 1, 0) != 0)
    {
      onError("Failed to execute expression string.");
      enable();
      return false;
    }

    if (lua_isnil(L, -1))
    {
      onError("Expression string compiled to nil.");
      enable();
      return false;
    }

    // Search for an empty spot in the function table.
    int key = 1;
    while (true)
    {
      lua_rawgeti(L, mFunctionTable, key);
      if (lua_isnil(L, -1))
      {
        break;
      }
      lua_pop(L, 1);
      key++;
    }
    lua_pop(L, 1);

    // Save the function object in the function table.
    lua_rawseti(L, mFunctionTable, key);
    e.mFunctionKey = key;
    enable();
    return true;
  }

  void ExpressionInterpreter::remove(Expression &e)
  {
    disable();
    lua_pushnil(L);
    lua_rawseti(L, mFunctionTable, e.mFunctionKey);
    enable();
  }

  float ExpressionInterpreter::value(Expression &e)
  {
    Lock lock(mMutex);
    if (mEnabled)
    {
      // Push function on the stack.
      lua_rawgeti(L, mFunctionTable, e.mFunctionKey);
      // Push arguments on the stack.
      for (Parameter *param : e.mParameters)
      {
        lua_pushnumber(L, param->value());
      }

      lua_pcall(L, e.mParameters.size(), 1, 0);
      // Ignoring potential error condition on purpose, since it will just be converted to a 0 and returned.

      e.mCachedValue = lua_tonumber(L, -1);
      lua_pop(L, 1);
    }
    return e.mCachedValue;
  }

  float ExpressionInterpreter::target(Expression &e)
  {
    Lock lock(mMutex);
    if (mEnabled)
    {
      // Push function on the stack.
      lua_rawgeti(L, mFunctionTable, e.mFunctionKey);
      // Push arguments on the stack.
      for (Parameter *param : e.mParameters)
      {
        lua_pushnumber(L, param->target());
      }
      lua_pcall(L, e.mParameters.size(), 1, 0);
      // Ignoring potential error condition on purpose, since it will just be converted to a 0 and returned.

      e.mCachedTarget = lua_tonumber(L, -1);
      lua_pop(L, 1);
    }
    return e.mCachedTarget;
  }

  void ExpressionInterpreter::enable()
  {
    Lock lock(mMutex);
    mEnabled = true;
  }

  void ExpressionInterpreter::disable()
  {
    Lock lock(mMutex);
    mEnabled = true;
  }

  /*
ExpressionInterpreter::test(0):
Expression: [function(x) return math.sin(x) + 1 end]
Expression: sum = 1847927.0000
Expression: 1617 ticks per call 1.6169 us per call
Native: sum = 1847927.0000
Native: 484 ticks per call 0.4847 us per call

ExpressionInterpreter::test(1):
Expression: [function(x) return x*x end]
Expression: sum = 1000000.0000
Expression: 648 ticks per call 0.6485 us per call
Native: sum = 1000000.0000
Native: 41 ticks per call 0.0413 us per call

ExpressionInterpreter::test(2):
Expression: [*]
Expression: sum = 1000000.0000
Expression: 61 ticks per call 0.0610 us per call
Native: sum = 1000000.0000
Native: 41 ticks per call 0.0413 us per call
 */

#ifdef BUILDOPT_TEST

  void ExpressionInterpreter::test(int type)
  {
    logInfo("ExpressionInterpreter::test(%d): ", type);

    int i;
    tick_t start;
    tick_t elapsed;
    int tickspercall;
    float microsecspercall;
    Expression E;
    Parameter X("test");

    E.attach();
    X.attach();
    X.hardSet(1.0f);
    E.addParameter(&X);
    E.addParameter(&X);

    switch (type)
    {
    case 0:
      E.setFunction("function(x) return math.sin(x) + 1 end");
      break;
    case 1:
      E.setFunction("function(x) return x*x end");
      break;
    case 2:
      E.setFunction("*");
      break;
    }

    logInfo("Expression: [%s] ", E.getFunction());

    if (!E.compile())
    {
      logError("Failed to compile expression. ");
      return;
    }

    start = ticks();
    float sum = 0.0f;
    for (i = 0; i < 1000000; i++)
    {
      sum += E.value();
    }
    elapsed = ticks() - start;
    logInfo("Expression: sum = %f ", sum);
    tickspercall = elapsed / 1000000;
    microsecspercall = ticks2secs(elapsed);
    logInfo("Expression: %d ticks per call %f us per call",
            tickspercall, microsecspercall);

    start = ticks();
    sum = 0.0f;
    switch (type)
    {
    case 0:
      for (i = 0; i < 1000000; i++)
      {
        sum += sinf(X.value()) + 1;
      }
      break;
    case 1:
    case 2:
      for (i = 0; i < 1000000; i++)
      {
        sum += X.value() * X.value();
      }
      break;
    }

    elapsed = ticks() - start;
    logInfo("Native: sum = %f ", sum);
    tickspercall = elapsed / 1000000;
    microsecspercall = ticks2secs(elapsed);
    logInfo("Native: %d ticks per call %f us per call", tickspercall,
            microsecspercall);
  }
#endif

} /* namespace od */
