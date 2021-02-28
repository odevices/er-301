#pragma once

#include <od/glue/Interpreter.h>

namespace od
{

  class AppInterpreter : public Interpreter
  {
  public:
    AppInterpreter();
    virtual ~AppInterpreter();

    void init();
  };

} /* namespace od */
