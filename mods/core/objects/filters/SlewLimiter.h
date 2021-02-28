/*
 * SlewLimiter.h
 *
 *  Created on: 17 Dec 2017
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_FILTERS_SLEWLIMITER_H_
#define APP_OBJECTS_FILTERS_SLEWLIMITER_H_

#include <od/objects/Object.h>

namespace od
{

  class SlewLimiter : public Object
  {
  public:
    SlewLimiter();
    virtual ~SlewLimiter();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
    Parameter mTime{"Time", 0.0f};
    Option mDirection{"Direction", CHOICE_BOTH};
#endif

  private:
    float mPreviousValue = 0.0f;
  };

} /* namespace od */

#endif /* APP_OBJECTS_FILTERS_SLEWLIMITER_H_ */
