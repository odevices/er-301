# lib54

## luaconf.h

### line 81 uncommented
#define LUA_32BITS

### line 172 added
#define LUA_PATH_DEFAULT "X:/?.lua;X:/?/init.lua"
#define LUA_CPATH_DEFAULT ""

### at the end
#define LUA_USE_DLOPEN

## lauxlib.h

Replaced the entire section:

/*
** {==================================================================
** "Abstraction Layer" for basic report of messages and errors
** ===================================================================
*/

typedef void (*writestring_t)(const char *buffer, size_t sz);
LUALIB_API void luaL_set_writestring_function(writestring_t f);
LUALIB_API void luaL_writestring(const char *buffer, size_t sz);

typedef void (*writestringerror_t)(const char *s1, const char *s2);
LUALIB_API void luaL_set_writestringerror_function(writestringerror_t f);
LUALIB_API void luaL_writestringerror(const char *s1, const char *s2);

/* print a string */
#if !defined(lua_writestring)
/*
#define lua_writestring(s,l)   fwrite((s), sizeof(char), (l), stdout)
*/
#define lua_writestring(s, l) luaL_writestring(s, l)
#endif

/* print a newline and flush the output */
#if !defined(lua_writeline)
#define lua_writeline() (lua_writestring("\n", 1), fflush(stdout))
#endif

/* print an error message */
#if !defined(lua_writestringerror)
/*
#define lua_writestringerror(s,p) \
        (fprintf(stderr, (s), (p)), fflush(stderr))
        */
#define lua_writestringerror(s, p) luaL_writestringerror(s, p)
#endif

## lauxlib.c

Added at the end:

static writestring_t writestring = NULL;
static writestringerror_t writestringerror = NULL;

LUALIB_API void luaL_set_writestring_function(writestring_t f)
{
  writestring = f;
}

LUALIB_API void luaL_set_writestringerror_function(writestringerror_t f)
{
  writestringerror = f;
}

LUALIB_API void luaL_writestring(const char *buffer, size_t sz)
{
  if (writestring)
  {
    writestring(buffer, sz);
  }
}

LUALIB_API void luaL_writestringerror(const char *s1, const char *s2)
{
  if (writestringerror)
  {
    writestringerror(s1, s2);
  }
}