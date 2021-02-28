#pragma once

#define TRACK_REFCOUNTED_OBJECTS 0
#if TRACK_REFCOUNTED_OBJECTS
#include <string>
#endif

namespace od
{

  // WARNING: only create these on the heap
  // If you must create on the stack or as a member of another class
  // then attach to it once and do not let go.
  class ReferenceCounted
  {
  public:
    ReferenceCounted();
    virtual ~ReferenceCounted();

#ifndef SWIGLUA
    void attach();
    void release();

    void attachLua();
    void releaseLua();

    // An object is deleted when it is both released and disowned.
    // Classes should 'own' their (ref'ed) member objects and never disown them.
    void own(ReferenceCounted *target);
    void disown(ReferenceCounted *target);
    void own(ReferenceCounted &target);
    void disown(ReferenceCounted &target);

    bool owned(ReferenceCounted *target);
    bool owned(ReferenceCounted &target);

    int refCount();
    ReferenceCounted *owner();

    bool mOnHeap = false;
#endif
    unsigned int handle();

  protected:
    int mRefCount = 0;
    ReferenceCounted *mpOwner = 0;

    void setOwner(ReferenceCounted *owner);
    void clearOwner();
    void enableTracking();

#if TRACK_REFCOUNTED_OBJECTS
  protected:
    bool mTrackingEnabled = false;
    std::string mTrackingName;
    virtual void fillTrackingName();

  public:
    static void enablePrinting();

#endif

  private:
    int mInternalKey = 0;
  };

} /* namespace od */
