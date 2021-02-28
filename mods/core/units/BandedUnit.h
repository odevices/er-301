#pragma once

#include <od/units/Unit.h>
#include <od/objects/measurement/Monitor.h>
#include <od/objects/mixing/Mixer.h>

namespace od
{

    class ChainBand;
    class BandedUnit : public Unit
    {
    public:
        BandedUnit(const std::string &name, int channelCount, int bandCount);
        virtual ~BandedUnit();

#ifndef SWIGLUA
        virtual void process();
#endif

        ChainBand *getBand(int index);

    protected:
        friend ChainBand;
        std::vector<Monitor *> mInputMonitors;
        std::vector<ChainBand *> mBands;
        std::vector<Mixer *> mMixers;
    };

} /* namespace od */
