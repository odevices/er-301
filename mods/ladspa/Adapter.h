#pragma once

#include <od/objects/Object.h>
#include <ladspa/ladspa.h>
#include <vector>

namespace ladspa
{
  class Adapter : public od::Object
  {
  public:
    Adapter(const LADSPA_Descriptor *ladspaDesc);
    virtual ~Adapter();

#ifndef SWIGLUA
    virtual void process();
    const LADSPA_Descriptor *getDescriptor() const { return ladspaDesc; }
#endif

  private:
    const LADSPA_Descriptor *ladspaDesc;
    LADSPA_Handle ladspaHandle;

    std::vector<od::Inlet *> er301Inlets;
    std::vector<od::Outlet *> er301Outlets;
    std::vector<od::Parameter *> er301InputParams;
    std::vector<od::Parameter *> er301OutputParams;

    std::vector<LADSPA_Data> ladspaInputControlValues;
    std::vector<LADSPA_Data> ladspaOutputControlValues;

    void initializePorts();
  };

} /* namespace ladspa */
