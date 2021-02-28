/*
 * MonoConvolution.h
 *
 *  Created on: 13 Sep 2017
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_FILTERS_MONOCONVOLUTION_H_
#define APP_OBJECTS_FILTERS_MONOCONVOLUTION_H_

#include <od/objects/Object.h>
#include <od/extras/UPOLS.h>
#include <od/audio/Sample.h>

namespace od
{

  class MonoConvolution : public Object
  {
  public:
    MonoConvolution();
    virtual ~MonoConvolution();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
#endif

    void setSample(Sample *sample);

  private:
    Sample *mpSample = 0;
    UPOLS convolve;
  };

} /* namespace od */

#endif /* APP_OBJECTS_FILTERS_MONOCONVOLUTION_H_ */
