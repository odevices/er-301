/*
 * StereoConvolution.h
 *
 *  Created on: 14 Sep 2017
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_FILTERS_STEREOCONVOLUTION_H_
#define APP_OBJECTS_FILTERS_STEREOCONVOLUTION_H_

#include <od/objects/Object.h>
#include <od/extras/UPOLS.h>
#include <od/audio/Sample.h>

namespace od
{

  class StereoConvolution : public Object
  {
  public:
    StereoConvolution();
    virtual ~StereoConvolution();

#ifndef SWIGLUA
    virtual void process();
    Inlet mLeftInput{"Left In"};
    Inlet mRightInput{"Right In"};
    Outlet mLeftOutput{"Left Out"};
    Outlet mRightOutput{"Right Out"};
#endif

    void setSample(Sample *sample);

  private:
    Sample *mpSample = 0;
    UPOLS mLeftFilter;
    UPOLS mRightFilter;
  };

} /* namespace od */

#endif /* APP_OBJECTS_FILTERS_STEREOCONVOLUTION_H_ */
