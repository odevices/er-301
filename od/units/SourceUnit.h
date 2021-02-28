#pragma once

#include <od/units/CustomUnit.h>

namespace od
{

    class SourceUnit : public CustomUnit
    {
    public:
        SourceUnit(const std::string &name, int channelCount);
        virtual ~SourceUnit();

#ifndef SWIGLUA
        virtual void process();
#endif

    protected:
        std::vector<Monitor *> mOutputMonitors;

        void connectUnits() override;
        void disconnectUnits() override;
    };

} /* namespace od */
