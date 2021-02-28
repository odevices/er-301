/*
 * ZeroCrossingDetector.h
 *
 *  Created on: 2017/06/06
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_TIMING_ZEROCROSSINGDETECTOR_H_
#define APP_OBJECTS_TIMING_ZEROCROSSINGDETECTOR_H_

#include <od/objects/Object.h>

namespace od
{

  class ZeroCrossingDetector : public Object
  {
  public:
    ZeroCrossingDetector();
    virtual ~ZeroCrossingDetector();

#ifndef SWIGLUA
    virtual void process();
    Inlet mGateInput{"Gate In"};
    Inlet mAudioInput{"Audio In"};
    Outlet mGateOutput{"Gate Out"};
#endif

  protected:
    int mState = 0;
    float mLastValue = 0.0f;
  };

} /* namespace od */

#endif /* APP_OBJECTS_TIMING_ZEROCROSSINGDETECTOR_H_ */
