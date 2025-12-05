#pragma once

#include <ladspa/Adapter.h>

extern "C" const LADSPA_Descriptor *ladspa_descriptor(unsigned long index);

namespace ladspa
{
  class GVerb : public ladspa::Adapter
  {
  public:
    GVerb();
    virtual ~GVerb();
  };
} /* namespace ladspa */
