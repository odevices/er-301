/*
 * LinearRamp.h
 *
 *  Created on: Oct 22, 2016
 *      Author: clarkson
 */

#ifndef APP_UTILS_LINEARRAMP_H_
#define APP_UTILS_LINEARRAMP_H_

namespace od
{

    class LinearRamp
    {
    public:
        LinearRamp();
        virtual ~LinearRamp();

        void setLength(int n)
        {
            if (mLength != n)
            {
                mLength = n;
                mStepUnit = 1.0f / (n > 0 ? n : 1);
            }
        }

        void reset(float start, float stop)
        {
            mCount = mLength;
            mValue = start;
            mStep = (stop - start) * mStepUnit;
            mGoal = stop;
        }

        void reset(float stop)
        {
            mCount = mLength;
            mStep = (stop - mValue) * mStepUnit;
            mGoal = stop;
        }

        void set(float value)
        {
            mCount = 0;
            mGoal = value;
            mValue = value;
        }

        void conditionalReset(float stop)
        {
            float diff = stop - mValue;
            if (diff < 1e-10f && diff > -1e-10f)
            {
                mValue = mGoal = stop;
                mCount = 0;
                mStep = 0;
            }
            else
            {
                mStep = diff * mStepUnit;
                mCount = mLength;
                mGoal = stop;
            }
        }

        inline float step()
        {
            if (mCount > 0)
            {
                mCount--;
                if (mCount == 0)
                {
                    mValue = mGoal;
                }
                else
                {
                    mValue += mStep;
                }
                return mValue;
            }
            else
            {
                return mGoal;
            }
        }

        inline bool done()
        {
            return mCount == 0;
        }

        inline bool notFinished()
        {
            return mCount > 0;
        }

        void getInterpolatedFrame(float *out);

        int mLength = 0;
        int mCount = 0;
        float mStep = 0.0f;
        float mStepUnit = 1.0f;
        float mValue = 0.0f;
        float mGoal = 0.0f;
    };

} /* namespace od */

#endif /* APP_UTILS_LINEARRAMP_H_ */
