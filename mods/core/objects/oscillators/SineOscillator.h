/*
 * SineOscillator.h
 *
 *  Created on: 15 Feb 2017
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_OSCILLATORS_SINEOSCILLATOR_H_
#define APP_OBJECTS_OSCILLATORS_SINEOSCILLATOR_H_

#include <od/objects/Object.h>

namespace od
{

  class SineOscillator : public Object
  {
  public:
    SineOscillator();
    virtual ~SineOscillator();

#ifndef SWIGLUA
    virtual void process();
    Inlet mVoltPerOctave{"V/Oct"};
    Inlet mSync{"Sync"};
    Outlet mOutput{"Out"};
    Inlet mFundamental{"Fundamental"};
    Inlet mPhase{"Phase"};
    Inlet mFeedback{"Feedback"};
    Parameter mInternalPhase{"Internal Phase", 0.0f};
#endif

  protected:
    float mLast[4] = {0, 0, 0, 0};
  };

} /* namespace od */

#endif /* APP_OBJECTS_OSCILLATORS_SINEOSCILLATOR_H_ */
