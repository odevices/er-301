/*
 * FreeVerb.cpp
 *
 *  Created on: 3 Nov 2017
 *      Author: clarkson
 */

#include <core/objects/freeverb/Freeverb.h>
#include <hal/ops.h>

namespace od
{

    Freeverb::Freeverb()
    {
        addInput(mLeftInput);
        addInput(mRightInput);
        addOutput(mLeftOutput);
        addOutput(mRightOutput);
        addParameter(mSize);
        addParameter(mDamp);
        addParameter(mWidth);
    }

    Freeverb::~Freeverb()
    {
    }

    void Freeverb::process()
    {
        float *leftIn = mLeftInput.buffer();
        float *rightIn = mRightInput.buffer();
        float *leftOut = mLeftOutput.buffer();
        float *rightOut = mRightOutput.buffer();
        float size = CLAMP(0, 1, mSize.value());
        float damp = CLAMP(0, 1, mDamp.value());
        float width = CLAMP(0, 1, mWidth.value());
        model.set(size, damp, width);
        model.process(leftIn, rightIn, leftOut, rightOut);
    }

} /* namespace od */
