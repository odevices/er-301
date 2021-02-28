/*
 * ADSR.cpp
 *
 *  Created on: 9 Jun 2016
 *      Author: clarkson
 */

#include <od/extras/LookupTables.h>
#include <od/constants.h>
#include <core/objects/env/ADSR.h>
#include <od/config.h>
#include <hal/ops.h>

namespace od
{

	ADSR::ADSR()
	{
		addInput(mGateInput);
		addInput(mAttack);
		addInput(mDecay);
		addInput(mSustain);
		addInput(mRelease);
		addOutput(mOutput);

		mAttackStage = &LookupTables::ConvexParabolicRise;
		mDecayStage = &LookupTables::ConcaveParabolicFall;
		mReleaseStage = &LookupTables::ConcaveParabolicFall;
	}

	ADSR::~ADSR()
	{
	}

	inline float ADSR::nextLUT(float sustain)
	{
		switch (mStage)
		{
		case 0: // waiting for trigger
			break;
		case 1: // attack
			mPhase += mAttackPhaseChange;
			if (mPhase >= 1.0f)
			{
				mCapture = mCurrentValue;
				mPhase = 0.0f;
				mStage = 2;
			}
			else
			{
				// rise from mCapture to 1.0
				mCurrentValue = mCapture + (1.0f - mCapture) * mAttackStage->get(mPhase);
			}
			break;
		case 2: // decay down from capture to sustain
			mPhase += mDecayPhaseChange;
			if (mPhase >= 1.0f)
			{
				mCurrentValue = sustain;
				mPhase = 0.0f;
				mStage = 3;
			}
			else
			{
				// fall from mCapture to mSustain
				mCurrentValue = sustain + (mCapture - sustain) * mDecayStage->get(mPhase);
			}
			break;
		case 3: // sustain
			mCurrentValue = sustain;
			break;
		case 4: // release from capture to zero
			mPhase += mReleasePhaseChange;
			if (mPhase >= 1.0f)
			{
				mCurrentValue = 0.0f; // force to zero, possible discontinuity
				mCapture = 0.0f;
				mPhase = 0.0f;
				mStage = 0;
			}
			else
			{
				// fall from mCapture to 0.0f
				mCurrentValue = mCapture * mReleaseStage->get(mPhase);
			}
			break;
		}

		return mCurrentValue;
	}

	inline float ADSR::next(float sustain)
	{
		switch (mStage)
		{
		case 0: // waiting for trigger
			break;
		case 1: // attack
			mPhase += mAttackPhaseChange;
			if (mPhase >= 1.0f)
			{
				mCapture = mCurrentValue;
				mPhase = 0.0f;
				mStage = 2;
			}
			else
			{
				// rise from mCapture to 1.0
				mCurrentValue = mCapture + (1.0f - mCapture) * (1.0f - (mPhase - 1.0f) * (mPhase - 1.0f));
			}
			break;
		case 2: // decay down from capture to sustain
			mPhase += mDecayPhaseChange;
			if (mPhase >= 1.0f)
			{
				mCurrentValue = sustain;
				mPhase = 0.0f;
				mStage = 3;
			}
			else
			{
				// fall from mCapture to mSustain
				mCurrentValue = sustain + (mCapture - sustain) * (mPhase - 1.0f) * (mPhase - 1);
			}
			break;
		case 3: // sustain
			mCurrentValue = sustain;
			break;
		case 4: // release from capture to zero
			mPhase += mReleasePhaseChange;
			if (mPhase >= 1.0f)
			{
				mCurrentValue = 0.0f; // force to zero, possible discontinuity
				mCapture = 0.0f;
				mPhase = 0.0f;
				mStage = 0;
			}
			else
			{
				// fall from mCapture to 0.0f
				mCurrentValue = mCapture * (mPhase - 1.0f) * (mPhase - 1.0f);
			}
			break;
		}

		return mCurrentValue;
	}

	// [ADSR]: 1.0228% (27163 ticks, 376 Hz)
	void ADSR::process()
	{
		float *gate = mGateInput.buffer();
		float *out = mOutput.buffer();
		float *attack = mAttack.buffer();
		float *decay = mDecay.buffer();
		float *sustain = mSustain.buffer();
		float *release = mRelease.buffer();

		mAttackPhaseChange = globalConfig.samplePeriod / MAX(0.0001f, attack[0]);
		mDecayPhaseChange = globalConfig.samplePeriod / MAX(0.0001f, decay[0]);
		mReleasePhaseChange = globalConfig.samplePeriod / MAX(0.0001f, release[0]);

		for (int i = 0; i < FRAMELENGTH; i++)
		{
			if ((mStage == 0 || mStage == 4))
			{
				// envelope is inactive
				if (gate[i] > 0.5f)
				{
					// turn on
					mStage = 1;
					mPhase = 0.0f;
					mCapture = mCurrentValue;
				}
			}
			else
			{
				// envelope is active
				if (gate[i] < 0.5f)
				{
					// turn off
					mStage = 4;
					mPhase = 0.0f;
					mCapture = mCurrentValue;
				}
			}

			out[i] = next(sustain[i]);
		}
	}

} /* namespace od */
