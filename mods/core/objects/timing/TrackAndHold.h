/*
 * TrackAndHold.h
 *
 *  Created on: 7 Jan 2017
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_TIMING_TRACKANDHOLD_H_
#define APP_OBJECTS_TIMING_TRACKANDHOLD_H_

#include <od/objects/Object.h>

// Track & Hold Choices
#define TRACKNHOLD_CHOICE_HIGH 1
#define TRACKNHOLD_CHOICE_LOW 2
#define TRACKNHOLD_CHOICE_MINMAX 3

namespace od
{

  class TrackAndHold : public Object
  {
  public:
    TrackAndHold();
    virtual ~TrackAndHold();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Inlet mTrack{"Track"};
    Outlet mOutput{"Out"};
    Option mFlavor{"Flavor", TRACKNHOLD_CHOICE_HIGH};
    Parameter mValue{"Value", 0.0f};
#endif
  };

} /* namespace od */

#endif /* APP_OBJECTS_TIMING_TRACKANDHOLD_H_ */
