#pragma once

#include <od/objects/Object.h>
#include <vector>

namespace od
{

  class Sequencer : public Object
  {
  public:
    Sequencer();
    virtual ~Sequencer();

#ifndef SWIGLUA
    virtual void process();
    Inlet mTrigger{"Trigger"};
    Inlet mReset{"Reset"};
    Outlet mOutput{"Out"};
    Parameter mStage{"Stage"};
    Option mProcessingRate{"Processing Rate", PER_FRAME};
#endif

    void clear();
    void remove(int i);
    void insert(int i, float value);
    void push_back(float value);
    void push_front(float value);
    int count();
    int get(int i);
    void set(int i, float value);

  protected:
    std::vector<float> mStages;
    void atFrameRate();
    void atSampleRate();
  };

} /* namespace od */
