#pragma once

#include <od/units/CustomUnit.h>

namespace od
{

    class EffectUnit : public CustomUnit
    {
    public:
        EffectUnit(const std::string &name, int channelCount);
        virtual ~EffectUnit();

#ifndef SWIGLUA
        virtual void process();
#endif

    protected:
        std::vector<Monitor *> mInputMonitors;
        std::vector<Monitor *> mOutputMonitors;

        void connectUnits() override;
        void disconnectUnits() override;
    };

} /* namespace od */
