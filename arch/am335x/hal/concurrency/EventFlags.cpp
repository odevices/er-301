#include <hal/concurrency/EventFlags.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Event.h>

namespace od
{
  EventFlags::EventFlags()
  {
    mHandle = (void *)Event_create(NULL, NULL);
  }

  EventFlags::~EventFlags()
  {
    Event_delete((Event_Handle *)&mHandle);
  }

  void EventFlags::clear(uint32_t flags)
  {
    Event_pend((Event_Handle)mHandle, 0, flags, BIOS_NO_WAIT);
  }

  void EventFlags::post(uint32_t flags)
  {
    Event_post((Event_Handle)mHandle, flags);
  }

  uint32_t EventFlags::getPosted()
  {
    return Event_getPostedEvents((Event_Handle)mHandle);
  }

  uint32_t EventFlags::waitForAny(uint32_t flags)
  {
    return Event_pend((Event_Handle)mHandle, 0, flags, BIOS_WAIT_FOREVER);
  }

  uint32_t EventFlags::waitForAny(uint32_t flags, uint32_t timeout)
  {
    return Event_pend((Event_Handle)mHandle, 0, flags, timeout);
  }

  uint32_t EventFlags::waitForAll(uint32_t flags)
  {
    return Event_pend((Event_Handle)mHandle, flags, 0, BIOS_WAIT_FOREVER);
  }

  uint32_t EventFlags::waitForAll(uint32_t flags, uint32_t timeout)
  {
    return Event_pend((Event_Handle)mHandle, flags, 0, timeout);
  }

  uint32_t EventFlags::wait(uint32_t allFlags, uint32_t anyFlags)
  {
    return Event_pend((Event_Handle)mHandle, allFlags, anyFlags, BIOS_WAIT_FOREVER);
  }

  uint32_t EventFlags::wait(uint32_t allFlags, uint32_t anyFlags, uint32_t timeout)
  {
    return Event_pend((Event_Handle)mHandle, allFlags, anyFlags, timeout);
  }

} // namespace od