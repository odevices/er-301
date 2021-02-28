#pragma once

#ifndef BUILDOPT_LUA_USE_REALLOC
#include <od/extras/MultiBufferPool.h>
#endif

#define INTERPRETER_RECORD_ALLOCATIONS 0
#if INTERPRETER_RECORD_ALLOCATIONS
#include <map>
#endif

#include <memory>
#include <string>

extern "C"
{
#include "lua.h"
}

namespace od
{

  class Interpreter
  {
  public:
    Interpreter();
    virtual ~Interpreter();

    void init(void);
    
    void printMemoryStats();
    void measureSpeed(void);

    bool execute(const char *code);
    template <typename... Args>
    bool execute(const std::string &format, Args... args)
    {
      int size = snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
      if (size <= 0)
      {
        return false;
      }
      std::unique_ptr<char[]> buf(new char[size]);
      snprintf(buf.get(), size, format.c_str(), args...);
      return execute(buf.get());
    }
    bool executeFile(const char *filename);

  protected:
    lua_State *L;

#ifndef BUILDOPT_LUA_USE_REALLOC
    MultiBufferPool<uint8_t> mAllocator;
    void *allocate(size_t size);
    void deallocate(void *ptr, size_t size);
    void *reallocate(void *ptr, size_t osize, size_t nsize);
#endif

    static void *reallocateCallback(void *ud, void *ptr, size_t osize, size_t nsize);
    static void writeStringCallback(const char *buffer, size_t sz);
    static void writeStringErrorCallback(const char *s1, const char *s2);

    void onError(const char *format, ...);

#if INTERPRETER_RECORD_ALLOCATIONS
    class Record
    {
    public:
      int count = 0;
      int current = 0;
      int max = 0;
    };
    std::map<size_t, Record> mRecords;
    int mTotalCount = 0;
    tick_t mTotalElapsed = 0;
#endif
    int mMisses = 0;

  private:
#ifndef BUILDOPT_LUA_USE_REALLOC
    void *getBuffer(size_t size);
    void releaseBuffer(void *ptr);
#endif

    bool genericCall(const char *func, const char *sig, ...);
  };

} /* namespace od */
