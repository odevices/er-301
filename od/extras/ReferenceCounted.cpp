#include "ReferenceCounted.h"

#if TRACK_REFCOUNTED_OBJECTS
#include <typeinfo>
#include <sstream>
static int totalAttached = 0;
static int totalAttachedLua = 0;
static int totalObjects = 0;
static bool printingEnabled = false;
#endif

static unsigned int nextKey = 0;

#define LOCALCHECKS 0
#define VERBOSE_ATTACH_RELEASE 0

namespace od
{

#if TRACK_REFCOUNTED_OBJECTS
  void ReferenceCounted::enablePrinting()
  {
    printingEnabled = true;
  }
#endif

  ReferenceCounted::ReferenceCounted()
  {
    mInternalKey = nextKey++;
  }

  ReferenceCounted::~ReferenceCounted()
  {
#if TRACK_REFCOUNTED_OBJECTS
    if (mTrackingEnabled)
    {
      totalObjects--;

      if (printingEnabled)
      {
        logInfo("DEL:%4d %s", totalObjects, mTrackingName.c_str());
      }
    }
#endif
  }

#if TRACK_REFCOUNTED_OBJECTS
  void ReferenceCounted::fillTrackingName()
  {
    std::ostringstream oss;
    oss << demangle(typeid(*this).name()) << '(' << mInternalKey << ')';
    mTrackingName = oss.str();
  }
#endif

  void ReferenceCounted::enableTracking()
  {
#if TRACK_REFCOUNTED_OBJECTS
    if (!mTrackingEnabled)
    {
      mTrackingEnabled = true;
      fillTrackingName();
      totalObjects++;
      if (printingEnabled)
      {

        logInfo("NEW:%4d %s", totalObjects, mTrackingName.c_str());
      }
    }
#endif
  }

  void ReferenceCounted::attach()
  {
    if (mpOwner)
    {
      mpOwner->attach();
    }
    mRefCount++;
#if TRACK_REFCOUNTED_OBJECTS
    if (mTrackingEnabled)
    {
      totalAttached++;
#if VERBOSE_ATTACH_RELEASE
      logInfo("---:%4d:ATT %s ref=%d", totalAttached,
              mTrackingName.c_str(), mRefCount);
#endif
    }

#endif
  }

  void ReferenceCounted::release()
  {
#if LOCALCHECKS
    logAssert(mRefCount > 0);
#endif
    mRefCount--;
#if TRACK_REFCOUNTED_OBJECTS
    if (mTrackingEnabled)
    {
      totalAttached--;
#if VERBOSE_ATTACH_RELEASE
      logInfo("---:%4d:REL %s ref=%d", totalAttached,
              mTrackingName.c_str(), mRefCount);
#endif
    }
#endif
    if (mpOwner)
    {
      mpOwner->release();
    }
    else if (mRefCount == 0)
    {
      delete this;
    }
  }

  void ReferenceCounted::attachLua()
  {
    if (mpOwner)
    {
      mpOwner->attachLua();
    }
    mRefCount++;
#if TRACK_REFCOUNTED_OBJECTS
    if (mTrackingEnabled)
    {
      totalAttachedLua++;
#if VERBOSE_ATTACH_RELEASE
      logInfo("LUA:%4d:ATT %s ref=%d", totalAttachedLua,
              mTrackingName.c_str(), mRefCount);
#endif
    }
#endif
  }

  void ReferenceCounted::releaseLua()
  {
#if LOCALCHECKS
    logAssert(mRefCount > 0);
#endif
    mRefCount--;
#if TRACK_REFCOUNTED_OBJECTS
    if (mTrackingEnabled)
    {
      totalAttachedLua--;
#if VERBOSE_ATTACH_RELEASE
      logInfo("LUA:%4d:REL %s ref=%d", totalAttachedLua,
              mTrackingName.c_str(), mRefCount);
#endif
    }
#endif
    if (mpOwner)
    {
      mpOwner->releaseLua();
    }
    else if (mRefCount == 0)
    {
      delete this;
    }
  }

  void ReferenceCounted::setOwner(ReferenceCounted *owner)
  {
#if LOCALCHECKS
    logAssert(mpOwner == 0);
#endif
    mpOwner = owner;
    for (int i = 0; i < mRefCount; i++)
    {
      mpOwner->attach();
    }
  }

  void ReferenceCounted::clearOwner()
  {
#if LOCALCHECKS
    logAssert(mpOwner);
#endif
    if (mpOwner)
    {
      for (int i = 0; i < mRefCount; i++)
      {
        mpOwner->release();
      }
      mpOwner = 0;
      if (mRefCount == 0)
      {
        delete this;
      }
    }
  }

  void ReferenceCounted::own(ReferenceCounted *target)
  {
    target->setOwner(this);
  }

  void ReferenceCounted::disown(ReferenceCounted *target)
  {
#if LOCALCHECKS
    logAssert(target->mpOwner == this);
#endif
    target->clearOwner();
  }

  void ReferenceCounted::own(ReferenceCounted &target)
  {
    target.setOwner(this);
  }

  void ReferenceCounted::disown(ReferenceCounted &target)
  {
#if LOCALCHECKS
    logAssert(target.mpOwner == this);
#endif
    target.clearOwner();
  }

  bool ReferenceCounted::owned(ReferenceCounted *target)
  {
    return target->mpOwner == this;
  }

  bool ReferenceCounted::owned(ReferenceCounted &target)
  {
    return target.mpOwner == this;
  }

  int ReferenceCounted::refCount()
  {
    return mRefCount;
  }

  ReferenceCounted *ReferenceCounted::owner()
  {
    return mpOwner;
  }

  unsigned int ReferenceCounted::handle()
  {
    return mInternalKey;
  }

} /* namespace od */
