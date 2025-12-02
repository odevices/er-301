#include <ladspa/GVerb.h>
#include <od/config.h>

namespace ladspa
{
  GVerb::GVerb() : ladspa::Adapter(ladspa_descriptor(0))
  {
    // Hello
  }

  GVerb::~GVerb()
  {
    // Goodbye
  }
} /* namespace ladspa */
