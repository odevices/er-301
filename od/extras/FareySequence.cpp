/*
 * FareySequence.cpp
 *
 *  Created on: 9 Oct 2016
 *      Author: clarkson
 */

#include <od/extras/FareySequence.h>
#include <utility>
#include <math.h>

namespace od
{

#if 0
/**
 * Farey algorithm
 *
 * Translated from John D. Cook's Python implementation in
 * http://www.johndcook.com/blog/2010/10/20/best-rational-approximation/
 *
 * @param x A value between 0.0 and 1.0 to be approximated by two integers
 * @param eps The required precision such that abs(x-a/b) < eps. Eps > 0.
 * @param N The maximum size of the numerator allowed
 */
static std::pair<int, int> fareyApproximate(float x, float eps, int N) {
	int a(0);
	int b(1);
	int c(1);
	int d(1);
	float mediant(0.0f);

	while ((b <= N) and (d <= N)) {
		mediant = static_cast<float>(a + c) / static_cast<float>(b + d);
		if (fabs(x - mediant) < eps) {
			if (b + d <= N) {
				return std::pair<int, int>(a + c, b + d);
			} else if (d > b) {
				return std::pair<int, int>(c, d);
			} else {
				return std::pair<int, int>(a, b);
			}
		} else if (x > mediant) {
			a = a + c;
			b = b + d;
		} else {
			c = a + c;
			d = b + d;
		}
	}
	if (b > N) {
		return std::pair<int, int>(c, d);
	} else {
		return std::pair<int, int>(a, b);
	}
}
#endif

	FareySequence::FareySequence()
	{
	}

	FareySequence::~FareySequence()
	{
	}

	void FareySequence::choose(int i)
	{
	}

	void FareySequence::search(float x)
	{
	}

	// Reference: http://www.geeksforgeeks.org/farey-sequence/
	// Optimized function to print Farey sequence of order n
	void FareySequence::generate(int n)
	{
		// We know first two terms are 0/1 and 1/n
		float x1 = 0, y1 = 1, x2 = 1, y2 = n;

		mTable.clear();
		//printf("%.0f/%.0f %.0f/%.0f", x1, y1, x2, y2);
		mTable.emplace_back(x1, y1);
		mTable.emplace_back(x2, y2);

		float x, y = 0; // For next terms to be evaluated
		while (y != 1.0f)
		{
			// Using recurrence relation to find the next term
			x = floorf((y1 + n) / y2) * x2 - x1;
			y = floorf((y1 + n) / y2) * y2 - y1;

			// Print next term
			//printf(" %.0f/%.0f", x, y);
			mTable.emplace_back(x, y);

			// Update x1, y1, x2 and y2 for next iteration
			x1 = x2, x2 = x, y1 = y2, y2 = y;
		}
	}

} /* namespace od */
