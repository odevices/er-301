#pragma once

#include <od/glue/Interpreter.h>
#include <od/glue/Expression.h>

namespace od
{

  class ExpressionInterpreter : public Interpreter
  {
  public:
    ExpressionInterpreter();
    virtual ~ExpressionInterpreter();

    // Not OK to call in the audio thread.
    bool compile(Expression &e);
    void remove(Expression &e);

    // OK to call in the audio thread.
    float value(Expression &e);
    float target(Expression &e);

    void enable()
    {
      mEnabled = true;
    }

    void disable()
    {
      mEnabled = false;
    }

#ifdef BUILDOPT_TESTING
    void test(int type);
#endif

  private:
    int mFunctionTable = 1;
    bool mEnabled = true;

    bool mPrepared = false;
    void prepare();
  };

} /* namespace od */
