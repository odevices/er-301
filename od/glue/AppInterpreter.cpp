#include "AppInterpreter.h"
#include <hal/dir.h>
#include <set>
#include <vector>
#include <string>

extern "C"
{

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

  int luaopen_app(lua_State *L);

  static int dir_iter(lua_State *L)
  {
    char *fname;
    dir_t *d = (dir_t *)lua_touserdata(L, lua_upvalueindex(1));

    if (Dir_read(*d, &fname, 0))
    {
      lua_pushstring(L, fname);
      return 1;
    }
    else
      return 0; /* no more values to return */
  }

  static int l_dir(lua_State *L)
  {
    const char *path = luaL_checkstring(L, 1);

    /* create a userdatum to store a DIR address */
    dir_t *d = (dir_t *)lua_newuserdata(L, sizeof(dir_t));

    /* set its metatable */
    luaL_getmetatable(L, "LuaBook.dir");
    lua_setmetatable(L, -2);

    /* try to open the given directory */
    *d = Dir_open(path);
    if (*d == 0) /* error opening the directory? */
      luaL_error(L, "cannot open %s", path);

    /* creates and returns the iterator function
	 (its sole upvalue, the directory userdatum,
	 is already on the stack top */
    lua_pushcclosure(L, dir_iter, 1);
    return 1;
  }

  static int dir_gc(lua_State *L)
  {
    dir_t *d = (dir_t *)lua_touserdata(L, 1);
    if (*d)
      Dir_close(*d);
    return 0;
  }

  int luaopen_dir(lua_State *L)
  {
    luaL_newmetatable(L, "LuaBook.dir");

    /* set its __gc field */
    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, dir_gc);
    lua_settable(L, -3);

    /* register the `dir' function */
    lua_pushcfunction(L, l_dir);
    lua_setglobal(L, "dir");

    return 0;
  }

  static const luaL_Reg loadedlibs[] = {
      {"_G", luaopen_base},
      {LUA_LOADLIBNAME, luaopen_package},
      {LUA_COLIBNAME, luaopen_coroutine},
      {LUA_TABLIBNAME, luaopen_table},
      {LUA_IOLIBNAME, luaopen_io},
      // {LUA_OSLIBNAME, luaopen_os},
      {LUA_STRLIBNAME, luaopen_string},
      {LUA_MATHLIBNAME, luaopen_math},
      // {LUA_UTF8LIBNAME, luaopen_utf8},
      {LUA_DBLIBNAME, luaopen_debug},
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

// Some typemaps for SWIG

void stringVectorToLuaTable(lua_State *L, std::vector<std::string> &vec)
{
  int n = (int)vec.size();
  lua_createtable(L, n, 0);
  for (int i = 0; i < n; ++i)
  {
    lua_pushstring(L, vec[i].c_str());
    lua_rawseti(L, -2, i + 1); // -1 is the number, -2 is the table
  }
}

// const-version
void stringVectorToLuaTable(lua_State *L,
                            const std::vector<std::string> &vec)
{
  int n = (int)vec.size();
  lua_createtable(L, n, 0);
  for (int i = 0; i < n; ++i)
  {
    lua_pushstring(L, vec[i].c_str());
    lua_rawseti(L, -2, i + 1); // -1 is the number, -2 is the table
  }
}

void stringSetToLuaTable(lua_State *L, std::set<std::string> &set)
{
  int i = 0;
  lua_createtable(L, set.size(), 0);
  for (const std::string &x : set)
  {
    lua_pushstring(L, x.c_str());
    lua_rawseti(L, -2, i + 1); // -1 is the number, -2 is the table
    i++;
  }
}

// const-version
void stringSetToLuaTable(lua_State *L, const std::set<std::string> &set)
{
  int i = 0;
  lua_createtable(L, set.size(), 0);
  for (const std::string &x : set)
  {
    lua_pushstring(L, x.c_str());
    lua_rawseti(L, -2, i + 1); // -1 is the number, -2 is the table
    i++;
  }
}

namespace od
{

  AppInterpreter::AppInterpreter()
  {
  }

  AppInterpreter::~AppInterpreter()
  {
  }

  void AppInterpreter::init()
  {
    Interpreter::init();

    lua_gc(L, LUA_GCGEN, 0, 0);

#ifndef BUILDOPT_LUA_USE_REALLOC
    // Approx 7472kB
    const int multiplier = 1;
    mAllocator.add(16, 15000 * multiplier);
    mAllocator.add(32, 50000 * multiplier);
    mAllocator.add(64, 30000 * multiplier);
    mAllocator.add(128, 8000 * multiplier);
    mAllocator.add(256, 4000 * multiplier);
    mAllocator.add(512, 1500 * multiplier);
    mAllocator.add(1024, 750 * multiplier);
    mAllocator.add(2048, 150 * multiplier);
#endif

    openStandardlibs(L);
    luaopen_dir(L);
    // Defined by SWIG using app.cpp.swig
    luaopen_app(L);

#ifdef BUILDOPT_TESTING
    execute("app.TESTING = true");
#else
    execute("app.TESTING = false");
#endif
  }

} /* namespace od */

/*
 Maximums after loading 12x MIXER(PLAYER+VCA(ADSR))
 4: 48
 8: 1611
 16: 1618
 32: 12793
 64: 10056
 128: 1788
 256: 908
 512: 910
 1024: 493
 2048: 37
 4096: 22
 8192: 12
 16384: 3
 32768: 1
 65536: 3

 Stats with system heap (malloc, free)
 Total Count: 43893
 Total Time: 8.8869 secs
 Avg Time per Allocation: 0.2024 ms

 ** 42 units **

 Lua Allocations
 bytes   cur     max     cnt
 4:      49      52      61
 8:      1464    1519    2011
 16:     1938    2494    6333
 32:     18030   19739   32283
 64:     11301   12176   22512
 128:    2120    2199    5657
 256:    1132    1185    2863
 512:    847     884     2148
 1024:   400     401     602
 2048:   35      44      218
 4096:   2       24      132
 8192:   2       13      45
 16384:  2       13      38
 32768:  0       1       6
 65536:  0       1       6
 131072: 0       15      24
 Total Count: 74939
 Total Time: 0.1015 secs
 Avg Time per Allocation: 0.0013 ms

 ** 42 units after quicksaving **

 Lua Allocations
 bytes   cur     max     cnt
 4:      49      52      61
 8:      1513    1828    4124
 16:     1938    9476    14143
 32:     18087   27745   42850
 64:     11736   14829   26176
 128:    2117    4708    8356
 256:    1132    1775    3534
 512:    846     1061    2366
 1024:   401     440     644
 2048:   35      116     301
 4096:   2       61      193
 8192:   2       72      117
 16384:  1       53      116
 32768:  1       2       9
 65536:  0       2       8
 131072: 0       15      36
 262144: 0       1       2
 Total Count: 103036
 Total Time: 8.6551 secs
 Avg Time per Allocation: 0.0840 ms
 */