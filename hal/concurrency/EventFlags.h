#pragma once
#include <stdint.h>

namespace od
{
  class EventFlags
  {
  public:
    EventFlags();
    virtual ~EventFlags();

    void clear(uint32_t flags = 0xFFFFFFFF);
    void post(uint32_t flags);
    uint32_t getPosted();

    uint32_t waitForAny(uint32_t flags);
    uint32_t waitForAny(uint32_t flags, uint32_t timeout);

    uint32_t waitForAll(uint32_t flags);
    uint32_t waitForAll(uint32_t flags, uint32_t timeout);

    uint32_t wait(uint32_t allFlags, uint32_t anyFlags, uint32_t timeout);
    uint32_t wait(uint32_t allFlags, uint32_t anyFlags);

    enum Event : uint32_t
    {
      flag00 = 1U << 0,
      flag01 = 1U << 1,
      flag02 = 1U << 2,
      flag03 = 1U << 3,
      flag04 = 1U << 4,
      flag05 = 1U << 5,
      flag06 = 1U << 6,
      flag07 = 1U << 7,
      flag08 = 1U << 8,
      flag09 = 1U << 9,
      flag10 = 1U << 10,
      flag11 = 1U << 11,
      flag12 = 1U << 12,
      flag13 = 1U << 13,
      flag14 = 1U << 14,
      flag15 = 1U << 15,
      flag16 = 1U << 16,
      flag17 = 1U << 17,
      flag18 = 1U << 18,
      flag19 = 1U << 19,
      flag20 = 1U << 20,
      flag21 = 1U << 21,
      flag22 = 1U << 22,
      flag23 = 1U << 23,
      flag24 = 1U << 24,
      flag25 = 1U << 25,
      flag26 = 1U << 26,
      flag27 = 1U << 27,
      flag28 = 1U << 28,
      flag29 = 1U << 29,
      flag30 = 1U << 30,
      flag31 = 1U << 31,
    };

  private:
    void *mHandle;
  };
} // namespace od