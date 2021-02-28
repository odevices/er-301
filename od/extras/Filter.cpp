/*
 * Filter.cpp
 *
 *  Created on: 20 Jun 2016
 *      Author: clarkson
 */

#include <od/extras/Filter.h>
#include <algorithm>

namespace od
{

	Filter::Filter()
	{
	}

	Filter::~Filter()
	{
	}

	void Filter::setCoefficients(std::vector<float> &a, std::vector<float> &b)
	{

		// reverse coefficients
		mB = b;
		std::reverse(mB.begin(), mB.end());
		mX.allocate(mB.size());
		mX.clear();

		if (a.size() > 1)
		{
			mA.resize(a.size() - 1);
			for (int i = 1; i < (int)a.size(); i++)
			{
				mA[mA.size() - i] = a[i] / a[0];
			}
			mY.allocate(mA.size());
			mY.clear();
			isFIR = false;
		}
		else
		{
			mY.deallocate();
			isFIR = true;
		}

		if (mA.size() > mB.size())
		{
			mDepth = mA.size();
		}
		else
		{
			mDepth = mB.size();
		}
	}

	float Filter::push(float x)
	{
		float *tmp;
		float y = 0;

		mX.push(x);

		tmp = mX.get(mB.size());
		for (int i = 0; i < (int)mB.size(); i++)
		{
			y += tmp[i] * mB[i];
		}

		if (!isFIR)
		{
			tmp = mY.get(mA.size());
			for (int i = 0; i < (int)mA.size(); i++)
			{
				y -= tmp[i] * mA[i];
			}
		}

		mY.push(y);
		return y;
	}

	void Filter::filtfilt(std::vector<float> &input, std::vector<float> &output)
	{
		int n = input.size();
		int i;

		mX.clear(input[0]);
		mY.clear(input[0]);

		for (i = 0; i < n; i++)
		{
			output[i] = push(input[i]);
		}

		mX.clear(input[n - 1]);
		mY.clear(input[n - 1]);

		for (i = n - 1; i >= 0; i--)
		{
			output[i] = push(output[i]);
		}
	}

	void Filter::filtfilt(std::vector<float> &x)
	{
		int n = x.size();
		int i;
		float y = x[n - 1];

		mX.clear(x[0]);
		mY.clear(x[0]);

		for (i = 0; i < n; i++)
		{
			x[i] = push(x[i]);
		}

		mX.clear(y);
		mY.clear(y);

		for (i = n - 1; i >= 0; i--)
		{
			x[i] = push(x[i]);
		}
	}

} /* namespace od */
