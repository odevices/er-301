#pragma once

#include <od/glue/Interpreter.h>
#include <od/glue/Expression.h>
#include <hal/concurrency/Mutex.h>

namespace od
{

  class ExpressionInterpreter : public Interpreter
  {
  public:
    ExpressionInterpreter(bool init = true);
    virtual ~ExpressionInterpreter();

    // Not OK to call in the audio thread.
    bool compile(Expression &e);
    void remove(Expression &e);

    // OK to call in the audio thread.
    float value(Expression &e);
    float target(Expression &e);

#ifdef BUILDOPT_TESTING
    void test(int type);
#endif

  private:
    Mutex mMutex;
    int mFunctionTable = 1;
    bool mEnabled = true;

    void enable();
    void disable();
  };

} /* namespace od */
