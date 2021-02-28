#pragma once

#define BIQUAD_FLAT_Q 0.70710678f

namespace od
{

	class BiquadFilter
	{
	public:
		BiquadFilter();
		virtual ~BiquadFilter();

		void designLowPass(float cutoff, float Q = BIQUAD_FLAT_Q);
		void designHighPass(float cutoff, float Q = BIQUAD_FLAT_Q);
		void designBandPass(float cutoff, float Q = BIQUAD_FLAT_Q);

		inline void initState(float x)
		{
			mZ2 = x * (mA2 - mB2);
			mZ1 = x * (1.0f - mA0);
		}

		inline float push(float x)
		{
			float y = x * mA0 + mZ1;
			mZ1 = x * mA1 + mZ2 - mB1 * y;
			mZ2 = x * mA2 - mB2 * y;
			return y;
		}

		void filtfilt(float *buffer, int n);
		void filtfilt(float *buffer, int n, int stride);
		void filtfilt(float *dst, float *src, int n);
		void filtfilt(float *dst, float *src, int n, int stride);

	private:
		float mA0, mA1, mA2;
		float mB1, mB2; // mB0 is assumed to be 1.
		float mZ1, mZ2;
	};

} /* namespace od */
