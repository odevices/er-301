#pragma once

#include <od/graphics/Graphic.h>
#include <od/graphics/controls/SoftButton.h>
#include <od/graphics/controls/Readout.h>
#include <od/extras/LinearRamp.h>
#include <od/objects/timing/Comparator.h>

namespace od
{

    class ComparatorView : public Graphic
    {
    public:
        ComparatorView(int left, int bottom, int width, int height,
                       Comparator &edge);
        ComparatorView(int left, int bottom, int width, int height,
                       Comparator &edge, Parameter &param);
        virtual ~ComparatorView();

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
#endif

        void setLabel(const std::string &text);
        const std::string &getLabel()
        {
            return mNameLabel.getText();
        }
        void setUnits(ReadoutUnits unit);
        void setPrecision(int p)
        {
            mReadout.setPrecision(p);
        }
        void setIndicatorType(IndicatorType type)
        {
            mIndicatorType = type;
        }

        Parameter *getReadoutParameter()
        {
            return mReadout.getParameter();
        }

        Comparator *getComparator()
        {
            return &mComparator;
        }

    private:
        SoftButton mNameLabel{" ---- "};
        Comparator &mComparator;
        int mLastCount = 0;
        LinearRamp mFade;

        Parameter mRateParameter{"Rate"};
        Readout mReadout;
        bool mShowRate;

        IndicatorType mIndicatorType = roundIndicator;
    };

} /* namespace od */
