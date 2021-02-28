/*
 * DeadbandFilter.h
 *
 *  Created on: 11 Jul 2016
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_FILTERS_DEADBANDFILTER_H_
#define APP_OBJECTS_FILTERS_DEADBANDFILTER_H_

#include <od/objects/Object.h>

namespace od
{

  class DeadbandFilter : public Object
  {
  public:
    DeadbandFilter();
    virtual ~DeadbandFilter();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
    Parameter mThreshold{"Threshold", 0.1f};
#endif

  private:
    float mLast = 0.0f;
  };

} /* namespace od */

#endif /* APP_OBJECTS_FILTERS_DEADBANDFILTER_H_ */
