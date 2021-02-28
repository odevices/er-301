/*
 * Energy.h
 *
 *  Created on: 20 Jun 2016
 *      Author: clarkson
 */

#ifndef APP_OBJECTS_FILTERS_ENERGY_H_
#define APP_OBJECTS_FILTERS_ENERGY_H_

#include <od/objects/Object.h>

namespace od
{

  class Energy : public Object
  {
  public:
    Energy();
    virtual ~Energy();

#ifndef SWIGLUA
    virtual void process();
    Inlet mInput{"In"};
    Outlet mOutput{"Out"};
#endif
  };

} /* namespace od */

#endif /* APP_OBJECTS_FILTERS_ENERGY_H_ */
