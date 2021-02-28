#pragma once

#include <od/graphics/Graphic.h>
#include <od/graphics/controls/Readout.h>
#include <od/graphics/controls/SoftButton.h>
#include <od/objects/control/ParamSetMorph.h>

namespace od
{

    class MorphFader : public Graphic
    {
    public:
        MorphFader();
        virtual ~MorphFader();

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
#endif

        void setLabel(const std::string &text);
        const std::string &getLabel();
        void setLabelCenter(int x);
        void showLabel();
        void hideLabel();

        void encoder(int change, bool shifted, bool fine);
        void setParamSetMorph(ParamSetMorph *object);

        // readout encapsulation
        void setMap(DialMap *map);
        void setUnits(ReadoutUnits units);
        void setPrecision(int p);
        void save();
        void restore();
        void zero();

    protected:
        ParamSetMorph *mpParamSetMorph = 0;
        Readout mReadout;
        SoftButton mNameLabel{" ---- ", 1};
    };

} /* namespace od */
