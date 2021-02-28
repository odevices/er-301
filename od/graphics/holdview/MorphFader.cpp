/*
 * MorphFader.cpp
 *
 *  Created on: 4 Apr 2019
 *      Author: clarkson
 */

#include <od/graphics/holdview/MorphFader.h>

namespace od
{

    MorphFader::MorphFader() : Graphic(0, 0, SECTION_PLY, SCREEN_HEIGHT), mReadout(0, 15, SECTION_PLY,
                                                                                   10)
    {
        own(mReadout);
        own(mNameLabel);

        addChild(&mReadout);
        addChild(&mNameLabel);
        mNameLabel.setCenter(mWidth / 2);
        mReadout.setUnits(ReadoutUnits::unitPercent);
        mReadout.useHardSet();

        mCursorState.orientation = CursorOrientation::cursorDown;
    }

    MorphFader::~MorphFader()
    {
        if (mpParamSetMorph)
        {
            mReadout.setParameter(NULL);
            mpParamSetMorph->release();
        }
    }

    void MorphFader::setParamSetMorph(ParamSetMorph *object)
    {
        if (mpParamSetMorph)
        {
            mReadout.setParameter(NULL);
            mpParamSetMorph->release();
        }
        mpParamSetMorph = object;
        if (mpParamSetMorph)
        {
            mpParamSetMorph->attach();
            mReadout.setParameter(&(object->mWeight));
        }
    }

    void MorphFader::draw(FrameBuffer &fb)
    {
        Graphic::draw(fb);
        int y0 = mReadout.mWorldBottom + mReadout.mHeight;
        int y1 = mWorldBottom + mHeight - 4;
        int H = y1 - y0;
        int W = mWidth - 10;
        float invW = 1.0f / W;
        int x0 = mWorldLeft + 5;

        float target = 0;
        if (mpParamSetMorph)
        {
            mpParamSetMorph->apply();
            target = mpParamSetMorph->mWeight.target();
        }

        int w = target * W;

        int x = x0, y = y0;
        for (; x < x0 + w; x++)
        {
            y = y0 + H * (x - x0) * invW;
            fb.vline(WHITE, x, y0, y);
        }

        mCursorState.x = x;
        mCursorState.y = y + 5;

        for (; x < x0 + W; x++)
        {
            y = y0 + H * (x - x0) * invW;
            fb.vline(GRAY5, x, y0, y);
        }
    }

    void MorphFader::setLabel(const std::string &text)
    {
        mNameLabel.setText(text);
        mNameLabel.setCenter(mWidth / 2);
    }

    const std::string &MorphFader::getLabel()
    {
        return mNameLabel.getText();
    }

    void MorphFader::setLabelCenter(int x)
    {
        mNameLabel.setCenter(x);
    }

    void MorphFader::showLabel()
    {
        mNameLabel.show();
    }

    void MorphFader::hideLabel()
    {
        mNameLabel.hide();
    }

    void MorphFader::encoder(int change, bool shifted, bool fine)
    {
        mReadout.encoder(change, shifted, fine);
    }

    void MorphFader::setMap(DialMap *map)
    {
        mReadout.setMap(map);
    }

    void MorphFader::setUnits(ReadoutUnits units)
    {
        mReadout.setUnits(units);
    }

    void MorphFader::setPrecision(int p)
    {
        mReadout.setPrecision(p);
    }

    void MorphFader::save()
    {
        mReadout.save();
    }

    void MorphFader::restore()
    {
        mReadout.restore();
    }

    void MorphFader::zero()
    {
        mReadout.zero();
    }

} /* namespace od */
