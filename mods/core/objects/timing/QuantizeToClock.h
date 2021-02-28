/*
 * QuantizeToClock.h
 *
 *  Created on: 25 Jan 2018
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_TIMING_QUANTIZETOCLOCK_H_
#define APP_OBJECTS_TIMING_QUANTIZETOCLOCK_H_

#include <od/objects/Object.h>

namespace od
{

  class QuantizeToClock : public Object
  {
  public:
    QuantizeToClock();
    virtual ~QuantizeToClock();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Inlet mClock{"Clock"};
    Outlet mOutput{"Out"};
#endif

  private:
    float mCurrentValue = 0.0f;
    bool mInputState = false;
    bool mGotRisingEdge = false;
    bool mGotFallingEdge = false;
    bool mClockState = false;
  };

} /* namespace od */

#endif /* APP_OBJECTS_TIMING_QUANTIZETOCLOCK_H_ */
