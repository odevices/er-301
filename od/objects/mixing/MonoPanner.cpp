#include <od/objects/mixing/MonoPanner.h>
#include <od/config.h>

namespace od
{

    MonoPanner::MonoPanner()
    {
        addInput(mInput);
        addInput(mPanInput);
        addOutput(mLeftOutput);
        addOutput(mRightOutput);
    }

    MonoPanner::~MonoPanner()
    {
    }

    void MonoPanner::process()
    {
        float *in = mInput.buffer();
        float *left = mLeftOutput.buffer();
        float *right = mRightOutput.buffer();
        float *pan = mPanInput.buffer();

        // -1 to 1 pans left to right
        for (int i = 0; i < FRAMELENGTH; i++)
        {
            if (pan[i] < -1e-5f)
            {
                // reduce right
                left[i] = in[i];
                right[i] = in[i] * (1.0f + pan[i]);
            }
            else if (pan[i] > 1e-5f)
            {
                // reduce left
                left[i] = in[i] * (1.0f - pan[i]);
                right[i] = in[i];
            }
            else
            {
                left[i] = in[i];
                right[i] = in[i];
            }
        }
    }

} /* namespace od */
