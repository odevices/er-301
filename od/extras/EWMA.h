/*
 * EWMA.h
 *
 *  Created on: Jun 21, 2016
 *      Author: clarkson
 */

#ifndef APP_UTILS_EWMA_H_
#define APP_UTILS_EWMA_H_

namespace od
{

    class EWMA
    {
    public:
        EWMA();
        virtual ~EWMA();

        // The time it takes to reach 63% of a step input.
        inline void setTimeConstantApproximate(float sampleRate, float secs)
        {
            // approximation of 1-exp(-1.0/(sampleRate * secs))
            if (secs > 0.00001f)
            {
                mAlpha = 1.0f / (sampleRate * secs);
            }
            else
            {
                mAlpha = 1.0f / (sampleRate * 0.00001f);
            }
        }

        // The time it takes to reach 63% of a step input.
        void setTimeConstantExact(float sampleRate, float secs);

        inline void setInitialState(float y)
        {
            mY = y;
        }

        inline float push(float x)
        {
            mY = mAlpha * x + (1.0f - mAlpha) * mY;
            return mY;
        }

        inline void pushInPlace(float *buffer, int n)
        {
            float *end = buffer + n;
            float tmp = 1.0f - mAlpha;
            while (buffer < end)
            {
                mY = mAlpha * (*buffer) + tmp * mY;
                *(buffer++) = mY;
            }
        }

        inline void push(float *dst, float *src, int n)
        {
            float *end = src + n;
            float tmp = 1.0f - mAlpha;
            while (src < end)
            {
                mY = mAlpha * (*(src++)) + tmp * mY;
                *(dst++) = mY;
            }
        }

        inline float get()
        {
            return mY;
        }

    protected:
        float mAlpha = 1.0f;
        float mY = 0.0f;
    };

} /* namespace od */

#endif /* APP_UTILS_EWMA_H_ */
