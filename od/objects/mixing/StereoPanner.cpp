#include <od/objects/mixing/StereoPanner.h>
#include <od/config.h>

namespace od
{

    StereoPanner::StereoPanner()
    {
        addInput(mLeftInput);
        addInput(mRightInput);
        addInput(mPanInput);
        addOutput(mLeftOutput);
        addOutput(mRightOutput);
    }

    StereoPanner::~StereoPanner()
    {
    }

    void StereoPanner::process()
    {
        float *leftIn = mLeftInput.buffer();
        float *rightIn = mRightInput.buffer();
        float *leftOut = mLeftOutput.buffer();
        float *rightOut = mRightOutput.buffer();
        float *pan = mPanInput.buffer();

        // -1 to 1 pans left to right
        for (int i = 0; i < FRAMELENGTH; i++)
        {
            if (pan[i] < -1e-5f)
            {
                // reduce right
                leftOut[i] = leftIn[i];
                rightOut[i] = rightIn[i] * (1.0f + pan[i]);
            }
            else if (pan[i] > 1e-5f)
            {
                // reduce left
                leftOut[i] = leftIn[i] * (1.0f - pan[i]);
                rightOut[i] = rightIn[i];
            }
            else
            {
                leftOut[i] = leftIn[i];
                rightOut[i] = rightIn[i];
            }
        }
    }

} /* namespace od */
