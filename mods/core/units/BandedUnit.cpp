#include <core/units/ChainBand.h>
#include <core/units/BandedUnit.h>

namespace od
{

  BandedUnit::BandedUnit(const std::string &name, int channelCount,
                         int bandCount) : Unit(name, channelCount)
  {
    for (int i = 0; i < channelCount; i++)
    {
      Monitor *o = new Monitor();
      o->attach();
      addInput(i, o, "In");
      mInputMonitors.push_back(o);
    }

    for (int i = 0; i < bandCount; i++)
    {
      ChainBand *band = new ChainBand(name, channelCount, this);
      band->attach();
      mBands.push_back(band);
    }

    float gain = 1.0f / bandCount;
    for (int i = 0; i < channelCount; i++)
    {
      Mixer *mixer = new Mixer(bandCount);
      mixer->attach();
      mMixers.push_back(mixer);
      setOutput(i, mixer, "Out");

      for (int j = 0; j < bandCount; j++)
      {
        mixer->setInputGain(j, gain);
        Inlet *inlet = mixer->getInput(j);
        Outlet *outlet = mBands[j]->getOutput(i);
        inlet->connect(outlet);
      }
    }
  }

  BandedUnit::~BandedUnit()
  {
    for (Monitor *o : mInputMonitors)
    {
      o->release();
    }

    for (ChainBand *band : mBands)
    {
      band->release();
    }

    for (Mixer *mixer : mMixers)
    {
      mixer->release();
    }
  }

  void BandedUnit::process()
  {
    Unit::process();

    for (Monitor *o : mInputMonitors)
    {
      o->process();
    }

    for (ChainBand *band : mBands)
    {
      band->process();
    }

    for (Mixer *mixer : mMixers)
    {
      mixer->updateParameters();
      mixer->process();
    }
  }

  ChainBand *BandedUnit::getBand(int i)
  {
    if (i >= 0 && i < (int)mBands.size())
    {
      return mBands[i];
    }
    else
    {
      return 0;
    }
  }

} /* namespace od */
