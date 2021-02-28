/*
 * FareySequence.h
 *
 *  Created on: 9 Oct 2016
 *      Author: clarkson
 */

#ifndef APP_UTILS_FAREYSEQUENCE_H_
#define APP_UTILS_FAREYSEQUENCE_H_

#include <vector>

namespace od
{

	class FareySequence
	{
	public:
		FareySequence();
		virtual ~FareySequence();

		void generate(int n);
		void choose(int i);
		void search(float x);

		int mNumerator = 0;
		int mDenominator = 1;
		float mValue = 0;

	private:
		int mMaxDenonimator = 1;

		struct Entry
		{
			Entry(float num, float den) : num((int)num), den((int)den), value(num / den)
			{
			}
			int num;
			int den;
			float value;
		};

		std::vector<Entry> mTable;
	};

} /* namespace od */

#endif /* APP_UTILS_FAREYSEQUENCE_H_ */
