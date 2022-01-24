#pragma once

#include <od/extras/ReferenceCounted.h>

namespace od
{

  // Followable Interface
  class Followable : public ReferenceCounted
  {
  public:
    virtual ~Followable() = 0;
    virtual float value() = 0;
    virtual float target() = 0;
    virtual int roundValue() = 0;
    virtual int roundTarget() = 0;
  };

} // namespace od
