/*
 * TriangleOscillator.h
 *
 *  Created on: 29 May 2017
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_OSCILLATORS_TRIANGLEOSCILLATOR_H_
#define APP_OBJECTS_OSCILLATORS_TRIANGLEOSCILLATOR_H_

#include <od/objects/Object.h>

namespace od
{

  class TriangleOscillator : public Object
  {
  public:
    TriangleOscillator();
    virtual ~TriangleOscillator();

#ifndef SWIGLUA
    virtual void process();
    Inlet mVoltPerOctave{"V/Oct"};
    Inlet mSync{"Sync"};
    Outlet mOutput{"Out"};
    Inlet mFundamental{"Fundamental"};
    Parameter mPhase{"Phase", 0.0f};
#endif
  };

} /* namespace od */

#endif /* APP_OBJECTS_OSCILLATORS_TRIANGLEOSCILLATOR_H_ */
