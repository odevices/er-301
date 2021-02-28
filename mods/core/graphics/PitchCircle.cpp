#include <core/graphics/PitchCircle.h>
#include <hal/constants.h>
#include <math.h>

namespace od
{

    PitchCircle::PitchCircle(int left, int bottom, int width, int height) : Graphic(left, bottom, width, height)

    {
    }

    PitchCircle::~PitchCircle()
    {
        if (mpQuantizer)
        {
            mpQuantizer->release();
        }
    }

    void PitchCircle::draw(FrameBuffer &fb)
    {
        const int CURSOR = 3;
        int radius = MIN(mWidth / 2, mHeight / 2) - mMargin - CURSOR;
        int centerX = mWorldLeft + mWidth / 2;
        int centerY = mWorldBottom + mHeight / 2;
        fb.circle(GRAY5, centerX, centerY, radius);

        if (mpQuantizer)
        {
            float theta, x, y;
            float lastOutput = mpQuantizer->mLastOutput;
            float lastInput = mpQuantizer->mLastInput;
            float post = FULLSCALE_IN_VOLTS * mpQuantizer->mPostTranspose.value();
            float radius2 = 0.8f * radius;

            for (float pc : mpQuantizer->mPitchClasses)
            {
                if (pc > -1e-8f)
                {
                    theta = (pc + post) * M_PI * 2;
                    x = sinf(theta);
                    y = cosf(theta);
                    x = centerX + radius * x;
                    y = centerY + radius * y;
                    fb.line(mForeground, centerX, centerY, x, y);
                }
            }

            for (float pc : mpQuantizer->mPitchBoundaries)
            {
                if (pc > -1e-8f)
                {
                    pc += post;
                    theta = pc * M_PI * 2;
                    x = sinf(theta);
                    y = cosf(theta);
                    fb.line(GRAY3, centerX + radius2 * x, centerY + radius2 * y,
                            centerX + radius * x, centerY + radius * y);
                }
            }

            // root degree
            theta = post * M_PI * 2;
            x = sinf(theta);
            y = cosf(theta);
            x = centerX + radius * x;
            y = centerY + radius * y;
            fb.fillCircle(BLACK, x, y, CURSOR);
            fb.circle(mForeground, x, y, CURSOR);

            // last input
            theta = lastInput * (M_PI * 2 * FULLSCALE_IN_VOLTS);
            x = sinf(theta);
            y = cosf(theta);
            x = centerX + x * radius;
            y = centerY + y * radius;
            fb.line(GRAY7, centerX, centerY, x, y);
            fb.fillCircle(GRAY7, x, y, CURSOR);
            fb.circle(GRAY7, x, y, CURSOR);

            // last output
            theta = lastOutput * (M_PI * 2 * FULLSCALE_IN_VOLTS);
            x = sinf(theta);
            y = cosf(theta);
            x = centerX + x * radius;
            y = centerY + y * radius;
            fb.fillCircle(mForeground, x, y, CURSOR);
        }
    }

    void PitchCircle::setQuantizer(ScaleQuantizer *pQuantizer)
    {
        if (mpQuantizer)
        {
            mpQuantizer->release();
        }
        mpQuantizer = pQuantizer;
        if (mpQuantizer)
        {
            mpQuantizer->attach();
        }
    }

} /* namespace od */
