#include <od/graphics/controls/ComparatorView.h>

#define HATPITCH 5

namespace od
{

    ComparatorView::ComparatorView(int left, int bottom, int width, int height,
                                   Comparator &edge) : Graphic(left, bottom, width, height), mComparator(edge), mReadout(0, height - 10, width, 10), mShowRate(true)
    {
        mComparator.attach();
        mRateParameter.attach(); // don't let go
        own(mNameLabel);
        own(mReadout);

        addChild(&mNameLabel);
        mNameLabel.setCenter(mWidth / 2);
        addChild(&mReadout);
        mReadout.setParameter(&mRateParameter);
        mReadout.setUnits(ReadoutUnits::unitHertz);
        mReadout.setPrecision(1);

        mCursorState.orientation = cursorRight;
        mFade.setLength(GRAPHICS_REFRESH_RATE / 2);
    }

    ComparatorView::ComparatorView(int left, int bottom, int width, int height,
                                   Comparator &edge, Parameter &param) : Graphic(left, bottom, width, height), mComparator(edge), mReadout(0, height - 10, width, 10), mShowRate(false)
    {
        mComparator.attach();
        mRateParameter.attach(); // don't let go
        own(mNameLabel);
        own(mReadout);

        addChild(&mNameLabel);
        mNameLabel.setCenter(mWidth / 2);
        addChild(&mReadout);
        mReadout.setParameter(&param);
        mReadout.setUnits(ReadoutUnits::unitNone);

        mCursorState.orientation = cursorRight;
        mFade.setLength(GRAPHICS_REFRESH_RATE / 2);
    }

    ComparatorView::~ComparatorView()
    {
        mComparator.release();
    }

    void ComparatorView::setLabel(const std::string &text)
    {
        mNameLabel.setText(text);
        mNameLabel.setCenter(mWidth / 2);
    }

    void ComparatorView::setUnits(ReadoutUnits unit)
    {
        mReadout.setUnits(unit);
    }

    void ComparatorView::draw(FrameBuffer &fb)
    {
        Graphic::draw(fb);

        if (mIndicatorType == roundIndicator)
        {
            int i, x, y, r;
            int levelCount = mComparator.getLevelCount();

            x = mWorldLeft + mWidth / 2;
            y = mWorldBottom + GRID4_LINE2;
            // set the cursor
            mCursorState.x = mWorldLeft;
            mCursorState.y = y;
            r = mWidth / 4;

            if (levelCount > mLastCount)
            {
                mFade.reset(1.0f, 0.0f);
            }

            if (mComparator.isOutputInverted())
            {
                fb.circle(WHITE, x, y + r + 1, 3);
                y -= 3;
                mCursorState.y = y;

                if (mComparator.mState.value())
                {
                    fb.circle(GRAY3, x, y, r);
                }
                else
                {
                    fb.circle(WHITE, x, y, r);
                }

                int cost = r * WHITE * (1 - mFade.mValue);
                cost /= 2;
                for (i = r; i > 0; i -= 2)
                {
                    if (cost > WHITE)
                    {
                        fb.circle(WHITE, x, y, i);
                    }
                    else if (cost > 0)
                    {
                        fb.circle(cost, x, y, i);
                    }
                    cost -= WHITE;
                }
            }
            else
            {
                if (mComparator.mState.value())
                {
                    fb.circle(WHITE, x, y, r);
                }
                else
                {
                    fb.circle(GRAY3, x, y, r);
                }

                r += 2;
                int cost = r * WHITE * mFade.mValue;
                cost /= 2;
                for (i = 1; i < r; i += 2)
                {
                    cost -= WHITE;
                    if (cost > WHITE)
                    {
                        fb.circle(WHITE, x, y, i);
                    }
                    else if (cost > 0)
                    {
                        fb.circle(cost, x, y, i);
                    }
                }
            }

            if (mComparator.getRisingEdgeCount() > 2 && mComparator.getElapsed() > 0.25f)
            {
                if (mShowRate)
                {
                    // we have a good estimate now
                    if (mReadout.getUnits() == ReadoutUnits::unitHertz)
                    {
                        float hz = mComparator.getRateInHertz();
                        mRateParameter.hardSet(hz);
                    }
                    else
                    {
                        float secs = mComparator.getPeriodInSeconds();
                        mRateParameter.hardSet(secs);
                    }
                }
                mComparator.resetCounter();
                mLastCount = 0;
            }
            else
            {
                mLastCount = levelCount;
            }

            mFade.step();

            if (mComparator.isHeld())
            {
                int x0 = mWorldLeft + 10;
                int x1 = mWorldLeft + mWidth - 10;
                int y0 = mWorldBottom + mHeight - 20;
                int y1 = y0 + 5;

                if (mComparator.isRisingEdgeHeld())
                {
                    Color color = GRAY5 + (WHITE - GRAY5) * sTween;
                    fb.vline(color, x0, y0, y1);
                    fb.hline(color, x0, x, y1);
                }
                else
                {
                    fb.vline(GRAY5, x0, y0, y1);
                    fb.hline(GRAY5, x0, x, y1);
                }

                if (mComparator.isFallingEdgeHeld())
                {
                    Color color = GRAY5 + (WHITE - GRAY5) * sTween;
                    fb.vline(color, x, y0, y1);
                    fb.hline(color, x, x1, y0);
                }
                else
                {
                    fb.vline(GRAY5, x, y0, y1);
                    fb.hline(GRAY5, x, x1, y0);
                }
            }
        }
    }

} /* namespace od */
