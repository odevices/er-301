/*
 * MinMax.h
 *
 *  Created on: 29 Sep 2016
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_MEASUREMENT_MINMAX_H_
#define APP_OBJECTS_MEASUREMENT_MINMAX_H_

#include <od/objects/Object.h>

namespace od
{

  class MinMax : public Object
  {
  public:
    MinMax();
    virtual ~MinMax();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Parameter mMinimum{"Min"};
    Parameter mMaximum{"Max"};
    Parameter mCenter{"Center"};
#endif

  private:
    int mCount = 0;
    int mPeriod = 0;
    float mNextMin = 0;
    float mNextMax = 0;
  };

} /* namespace od */

#endif /* APP_OBJECTS_MEASUREMENT_MINMAX_H_ */
