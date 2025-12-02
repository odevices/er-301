#include <ladspa/Adapter.h>
#include <od/config.h>
#include <string>
#include <sstream>
#include <math.h>

namespace ladspa
{

  Adapter::Adapter(const LADSPA_Descriptor *ladspaDesc)
      : ladspaDesc(ladspaDesc), ladspaHandle(nullptr)
  {
    if (!ladspaDesc)
    {
      return;
    }

    if (ladspaDesc->Label)
    {
      mName = std::string(ladspaDesc->Label);
    }

    if (ladspaDesc->instantiate)
    {
      ladspaHandle = ladspaDesc->instantiate(ladspaDesc, globalConfig.sampleRate);
    }

    if (!ladspaHandle)
    {
      return;
    }

    initializePorts();

    if (ladspaDesc->activate)
    {
      ladspaDesc->activate(ladspaHandle);
    }
  }

  Adapter::~Adapter()
  {
    if (ladspaHandle && ladspaDesc)
    {
      if (ladspaDesc->deactivate)
      {
        ladspaDesc->deactivate(ladspaHandle);
      }

      if (ladspaDesc->cleanup)
      {
        ladspaDesc->cleanup(ladspaHandle);
      }
    }
  }

  void Adapter::initializePorts()
  {
    if (!ladspaDesc || !ladspaHandle)
    {
      return;
    }

    unsigned long ladspaPortCount = ladspaDesc->PortCount;

    unsigned long ladspaInputControlCount = 0;
    unsigned long ladspaOutputControlCount = 0;

    for (unsigned long i = 0; i < ladspaPortCount; i++)
    {
      LADSPA_PortDescriptor ladspaPortDesc = ladspaDesc->PortDescriptors[i];
      if (LADSPA_IS_PORT_CONTROL(ladspaPortDesc))
      {
        if (LADSPA_IS_PORT_INPUT(ladspaPortDesc))
        {
          ladspaInputControlCount++;
        }
        else if (LADSPA_IS_PORT_OUTPUT(ladspaPortDesc))
        {
          ladspaOutputControlCount++;
        }
      }
    }

    ladspaInputControlValues.reserve(ladspaInputControlCount);
    ladspaOutputControlValues.reserve(ladspaOutputControlCount);

    for (unsigned long i = 0; i < ladspaPortCount; i++)
    {
      LADSPA_PortDescriptor ladspaPortDesc = ladspaDesc->PortDescriptors[i];
      const char *ladspaPortName = ladspaDesc->PortNames[i];
      std::string er301Name = ladspaPortName ? std::string(ladspaPortName) : "";

      if (LADSPA_IS_PORT_AUDIO(ladspaPortDesc))
      {
        if (LADSPA_IS_PORT_INPUT(ladspaPortDesc))
        {
          od::Inlet *er301Inlet = new od::Inlet(er301Name);
          addInputFromHeap(er301Inlet);
          er301Inlets.push_back(er301Inlet);
        }
        else if (LADSPA_IS_PORT_OUTPUT(ladspaPortDesc))
        {
          od::Outlet *er301Outlet = new od::Outlet(er301Name);
          addOutputFromHeap(er301Outlet);
          er301Outlets.push_back(er301Outlet);
        }
      }
      else if (LADSPA_IS_PORT_CONTROL(ladspaPortDesc))
      {
        if (LADSPA_IS_PORT_INPUT(ladspaPortDesc))
        {
          // TODO: Factor this out.
          LADSPA_Data defaultValue = 0.0f;

          const LADSPA_PortRangeHint *hint = &ladspaDesc->PortRangeHints[i];

          if (LADSPA_IS_HINT_HAS_DEFAULT(hint->HintDescriptor))
          {
            if (LADSPA_IS_HINT_DEFAULT_0(hint->HintDescriptor))
            {
              defaultValue = 0.0f;
            }
            else if (LADSPA_IS_HINT_DEFAULT_1(hint->HintDescriptor))
            {
              defaultValue = 1.0f;
            }
            else if (LADSPA_IS_HINT_DEFAULT_100(hint->HintDescriptor))
            {
              defaultValue = 100.0f;
            }
            else if (LADSPA_IS_HINT_DEFAULT_440(hint->HintDescriptor))
            {
              defaultValue = 440.0f;
            }
            else if (LADSPA_IS_HINT_DEFAULT_MINIMUM(hint->HintDescriptor))
            {
              defaultValue = hint->LowerBound;
            }
            else if (LADSPA_IS_HINT_DEFAULT_MAXIMUM(hint->HintDescriptor))
            {
              defaultValue = hint->UpperBound;
            }
            else if (LADSPA_IS_HINT_DEFAULT_LOW(hint->HintDescriptor))
            {
              if (LADSPA_IS_HINT_LOGARITHMIC(hint->HintDescriptor))
              {
                defaultValue = exp(log(hint->LowerBound) * 0.75 + log(hint->UpperBound) * 0.25);
              }
              else
              {
                defaultValue = hint->LowerBound * 0.75f + hint->UpperBound * 0.25f;
              }
            }
            else if (LADSPA_IS_HINT_DEFAULT_MIDDLE(hint->HintDescriptor))
            {
              if (LADSPA_IS_HINT_LOGARITHMIC(hint->HintDescriptor))
              {
                defaultValue = exp(log(hint->LowerBound) * 0.5 + log(hint->UpperBound) * 0.5);
              }
              else
              {
                defaultValue = hint->LowerBound * 0.5f + hint->UpperBound * 0.5f;
              }
            }
            else if (LADSPA_IS_HINT_DEFAULT_HIGH(hint->HintDescriptor))
            {
              if (LADSPA_IS_HINT_LOGARITHMIC(hint->HintDescriptor))
              {
                defaultValue = exp(log(hint->LowerBound) * 0.25 + log(hint->UpperBound) * 0.75);
              }
              else
              {
                defaultValue = hint->LowerBound * 0.25f + hint->UpperBound * 0.75f;
              }
            }
          }

          od::Parameter *er301Param = new od::Parameter(er301Name, defaultValue);
          addParameterFromHeap(er301Param);
          er301InputParams.push_back(er301Param);
          ladspaInputControlValues.push_back(defaultValue);
          ladspaDesc->connect_port(ladspaHandle, i, &ladspaInputControlValues.back());
        }
        else if (LADSPA_IS_PORT_OUTPUT(ladspaPortDesc))
        {
          od::Parameter *er301Param = new od::Parameter(er301Name);
          addParameterFromHeap(er301Param);
          er301OutputParams.push_back(er301Param);
          ladspaOutputControlValues.push_back(0.0f);
          ladspaDesc->connect_port(ladspaHandle, i, &ladspaOutputControlValues.back());
        }
      }
    }
  }

  void Adapter::process()
  {
    if (!ladspaHandle || !ladspaDesc || !ladspaDesc->run)
    {
      return;
    }

    for (size_t i = 0; i < er301InputParams.size(); i++)
    {
      ladspaInputControlValues[i] = er301InputParams[i]->value();
    }

    unsigned long er301InletIndex = 0;
    unsigned long er301OutletIndex = 0;
    for (unsigned long i = 0; i < ladspaDesc->PortCount; i++)
    {
      LADSPA_PortDescriptor ladspaPortDesc = ladspaDesc->PortDescriptors[i];

      if (LADSPA_IS_PORT_AUDIO(ladspaPortDesc))
      {
        if (LADSPA_IS_PORT_INPUT(ladspaPortDesc))
        {
          float *er301InletBuffer = er301Inlets[er301InletIndex]->buffer();
          ladspaDesc->connect_port(ladspaHandle, i, er301InletBuffer);
          er301InletIndex++;
        }
        else if (LADSPA_IS_PORT_OUTPUT(ladspaPortDesc))
        {
          float *er301OutletBuffer = er301Outlets[er301OutletIndex]->buffer();
          ladspaDesc->connect_port(ladspaHandle, i, er301OutletBuffer);
          er301OutletIndex++;
        }
      }
    }

    ladspaDesc->run(ladspaHandle, FRAMELENGTH);

    for (size_t i = 0; i < er301OutputParams.size(); i++)
    {
      er301OutputParams[i]->hardSet(ladspaOutputControlValues[i]);
    }
  }
} /* namespace ladspa */
