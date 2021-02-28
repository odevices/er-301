/*
 * PopReducer.h
 *
 *  Created on: 14 Sep 2016
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_ENV_POPREDUCER_H_
#define APP_OBJECTS_ENV_POPREDUCER_H_

#include <od/objects/Object.h>

namespace od
{

  class PopReducer : public Object
  {
  public:
    PopReducer();
    virtual ~PopReducer();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Inlet mTrigger{"Trigger"};
    Outlet mOutput{"Out"};
    Parameter mTime{"Time"};
#endif

  private:
    float mPhase = 0.0f;
  };

} /* namespace od */

#endif /* APP_OBJECTS_ENV_POPREDUCER_H_ */
