/*
 * SampleSaver.cpp
 *
 *  Created on: 24 Oct 2016
 *      Author: clarkson
 */

#include <od/audio/SampleSaver.h>
#include <od/audio/WavFileWriter.h>

namespace od
{

	SampleSaver::SampleSaver()
	{
	}

	SampleSaver::~SampleSaver()
	{
	}

	bool SampleSaver::set(Sample *sample, const char *filename)
	{
		if (mStatus == STATUS_WORKING || sample == 0 || filename == 0)
			return false;

		mpSample = sample;
		mpSample->attach();
		mFilename = filename;
		mStatus = STATUS_WORKING;
		return true;
	}

	void SampleSaver::work()
	{
		if (mpSample)
		{
			mStatus = save();
			mpSample->release();
			mpSample = NULL;
		}
	}

	int SampleSaver::save()
	{
		WavFileWriter writer(mpSample->mSampleRate, mpSample->mChannelCount,
												 wavFloat);
		uint32_t sw;
		float *buffer = mpSample->mpData;

		if (buffer == NULL)
		{
			return STATUS_SAMPLE_NOT_PREPARED;
		}

		mPercentDone = 0.0f;
		mSamplesWritten = 0;

		if (!writer.open(mFilename))
		{
			return STATUS_ERROR_OPENING_FILE;
		}

		mSamplesRemaining = mpSample->mSampleCount;

		while (mSamplesRemaining)
		{
			if (mCancelRequested)
			{
				return STATUS_CANCELED;
			}

			if (mSamplesRemaining < mSamplesPerBlock)
				sw = mSamplesRemaining;
			else
				sw = mSamplesPerBlock;

			if (writer.writeSamples(buffer, sw) != sw)
			{
				return STATUS_ERROR_WRITING_FILE;
			}
			else
			{
				buffer += sw * mpSample->mChannelCount;
				mSamplesWritten += sw;
				mSamplesRemaining -= sw;
				if (mpSample->mSampleCount < mSamplesPerBlock)
				{
					mPercentDone = 100.0f;
				}
				else
				{
					mPercentDone = 100.0f * (float)(mSamplesWritten / mSamplesPerBlock) / (float)(mpSample->mSampleCount / mSamplesPerBlock);
				}
			}
		}
		mpSample->mDirty = false;
		writer.close();
		return STATUS_FINISHED;
	}

} /* namespace od */
