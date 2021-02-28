#pragma once

#include <od/objects/Object.h>

namespace od
{

  class RoundRobin : public Object
  {
  public:
    RoundRobin(int n);
    virtual ~RoundRobin();

#ifndef SWIGLUA
    virtual void process();
    Inlet mNext{"Next"};
    Inlet mInput{"In"};
    Option mProcessingRate{"Processing Rate", PER_FRAME};
    Option mActiveOutput{"Active Output", 0};
    // Outputs are allocated dynamically.
#endif

    void next();
    void reset();

  protected:
    int mNumOutputs = 0;
    float **mBuffers = NULL;
  };

} /* namespace od */
