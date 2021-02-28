#include <od/glue/Expression.h>
#include <od/glue/ExpressionInterpreter.h>
#include <od/extras/Utils.h>
#include <od/objects/Parameter.h>

namespace od
{

  static ExpressionInterpreter &interpreter()
  {
    static ExpressionInterpreter expressionInterpreter;
    return expressionInterpreter;
  }

  Expression::Expression()
  {
  }

  Expression::~Expression()
  {
    interpreter().remove(*this);
    clearParameters();
  }

  bool Expression::setFunction(const std::string &f)
  {
    mFunction = trim_copy(f);
    if (startsWith(mFunction, "function"))
    {
      mOperation = Operation::function;
      return true;
    }
    else if (mFunction == "+")
    {
      mOperation = Operation::add;
      return true;
    }
    else if (mFunction == "-")
    {
      mOperation = Operation::subtract;
      return true;
    }
    else if (mFunction == "*")
    {
      mOperation = Operation::multiply;
      return true;
    }
    else if (mFunction == "/")
    {
      mOperation = Operation::divide;
      return true;
    }
    else if (mFunction == "negate")
    {
      mOperation = Operation::negate;
      return true;
    }
    else if (mFunction == "invert")
    {
      mOperation = Operation::invert;
      return true;
    }

    return false;
  }

  void Expression::clearParameters()
  {
    for (Parameter *p : mParameters)
    {
      p->release();
    }
    mParameters.clear();
  }

  void Expression::addParameter(Parameter *param)
  {
    param->attach();
    mParameters.push_back(param);
  }

  bool Expression::compile()
  {
    switch (mOperation)
    {
    case Operation::function:
      return interpreter().compile(*this);
    case Operation::add:
    case Operation::subtract:
    case Operation::multiply:
    case Operation::divide:
      return mParameters.size() == 2;
    case Operation::negate:
    case Operation::invert:
      return mParameters.size() == 1;
    default:
      return false;
    }
  }

  float Expression::value()
  {
    switch (mOperation)
    {
    case Operation::function:
      return interpreter().value(*this);
    case Operation::add:
      return mParameters[0]->value() + mParameters[1]->value();
    case Operation::subtract:
      return mParameters[0]->value() - mParameters[1]->value();
    case Operation::multiply:
      return mParameters[0]->value() * mParameters[1]->value();
    case Operation::divide:
      return mParameters[0]->value() / mParameters[1]->value();
    case Operation::negate:
      return -1.0f * mParameters[0]->value();
    case Operation::invert:
      return 1.0f / mParameters[0]->value();
    default:
      return 0.0f;
    }
  }

  float Expression::target()
  {
    switch (mOperation)
    {
    case Operation::function:
      return interpreter().target(*this);
    case Operation::add:
      return mParameters[0]->target() + mParameters[1]->target();
    case Operation::subtract:
      return mParameters[0]->target() - mParameters[1]->target();
    case Operation::multiply:
      return mParameters[0]->target() * mParameters[1]->target();
    case Operation::divide:
      return mParameters[0]->target() / mParameters[1]->target();
    case Operation::negate:
      return -1.0f * mParameters[0]->target();
    case Operation::invert:
      return 1.0f / mParameters[0]->target();
    default:
      return 0.0f;
    }
  }

  int Expression::roundValue()
  {
    return fastRound(value());
  }

  int Expression::roundTarget()
  {
    return fastRound(target());
  }

} /* namespace od */
