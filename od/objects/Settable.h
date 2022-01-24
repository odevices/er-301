#pragma once

namespace od
{

  // Settable Interface
  class Settable
  {
  public:
    virtual ~Settable() = 0;
    virtual void hardSet(float x) = 0;
    virtual void softSet(float x) = 0;
  };

} // namespace od
