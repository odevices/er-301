/*
 * BiquadFilter.cpp
 *
 *  Created on: 21 Jun 2016
 *      Author: clarkson
 */

#include <core/extras/BiquadFilter.h>
#include <math.h>

namespace od
{

	BiquadFilter::BiquadFilter()
	{
		// TODO Auto-generated constructor stub
	}

	BiquadFilter::~BiquadFilter()
	{
		// TODO Auto-generated destructor stub
	}

	void BiquadFilter::designLowPass(float cutoff, float Q)
	{
		float K = tanf(M_PI * cutoff);
		float norm = 1.0f / (1.0f + K / Q + K * K);
		mA0 = K * K * norm;
		mA1 = 2 * mA0;
		mA2 = mA0;
		mB1 = 2 * (K * K - 1) * norm;
		mB2 = (1 - K / Q + K * K) * norm;
	}

	void BiquadFilter::designHighPass(float cutoff, float Q)
	{
		float K = tanf(M_PI * cutoff);
		float norm = 1.0f / (1.0f + K / Q + K * K);
		mA0 = norm;
		mA1 = -2 * mA0;
		mA2 = mA0;
		mB1 = 2 * (K * K - 1) * norm;
		mB2 = (1 - K / Q + K * K) * norm;
	}

	void BiquadFilter::designBandPass(float cutoff, float Q)
	{
		float K = tanf(M_PI * cutoff);
		float norm = 1.0f / (1.0f + K / Q + K * K);
		mA0 = K / Q * norm;
		mA1 = 0;
		mA2 = -mA0;
		mB1 = 2 * (K * K - 1) * norm;
		mB2 = (1 - K / Q + K * K) * norm;
	}

	void BiquadFilter::filtfilt(float *dst, float *src, int n,
															int stride)
	{
		float *begin = dst;
		float *end = dst + n;

		// initialize internal state
		initState(*src);

		while (dst < end)
		{
			*dst = push(*src);
			dst += stride;
			src += stride;
		}

		// re-initialize internal state
		dst -= stride;
		initState(*dst);

		while (dst >= begin)
		{
			*dst = push(*dst);
			dst -= stride;
		}
	}

	void BiquadFilter::filtfilt(float *dst, float *src, int n)
	{
		int i;

		// initialize internal state
		initState(src[0]);

		for (i = 0; i < n; i++)
		{
			dst[i] = push(src[i]);
		}

		// re-initialize internal state
		i--;
		initState(dst[i]);

		for (; i >= 0; i--)
		{
			dst[i] = push(dst[i]);
		}
	}

	void BiquadFilter::filtfilt(float *buffer, int n)
	{
		int i;

		initState(buffer[0]);

		for (i = 0; i < n; i++)
		{
			buffer[i] = push(buffer[i]);
		}

		// re-initialize internal state
		i--;
		initState(buffer[i]);

		for (; i >= 0; i--)
		{
			buffer[i] = push(buffer[i]);
		}
	}

	void BiquadFilter::filtfilt(float *buffer, int n, int stride)
	{
		int i;

		initState(buffer[0]);

		for (i = 0; i < n; i += stride)
		{
			buffer[i] = push(buffer[i]);
		}

		// re-initialize internal state
		i -= stride;
		initState(buffer[i]);

		for (; i >= 0; i -= stride)
		{
			buffer[i] = push(buffer[i]);
		}
	}

} /* namespace od */
