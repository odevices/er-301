#pragma once

#include <hal/concurrency/Thread.h>

namespace od
{

  class Busy : public Thread
  {
  public:
    Busy();
    virtual ~Busy();

    void enable();
    void disable();

  private:
    const uint32_t onEnable = EventFlags::flag01;
    const uint32_t onDisable = EventFlags::flag02;
    bool mEnabled = false;
    virtual void run();
  };

} /* namespace od */
