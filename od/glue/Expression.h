#pragma once

#include <od/objects/Followable.h>
#include <string>
#include <vector>

namespace od
{

  class Parameter;
  class ExpressionInterpreter;
  class Expression : public Followable
  {
  public:
    Expression();
    virtual ~Expression();

    virtual float value();
    virtual float target();
    virtual int roundValue();
    virtual int roundTarget();

    bool setFunction(const std::string &f);
    const char *getFunction()
    {
      return mFunction.c_str();
    }
    void clearParameters();
    void addParameter(Parameter *param);
    bool compile();

  private:
    friend class ExpressionInterpreter;
    std::vector<Parameter *> mParameters;
    std::string mFunction;
    int mFunctionKey = 0;
    float mCachedValue = 0.0f;
    float mCachedTarget = 0.0f;
    enum Operation
    {
      function,
      add,
      subtract,
      multiply,
      divide,
      negate,
      invert,
    };

    Operation mOperation = Operation::function;
  };

} /* namespace od */
