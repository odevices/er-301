/*
 * LookupTables.h
 *
 *  Created on: 10 Jun 2016
 *      Author: clarkson
 */

#ifndef APP_UTILS_LOOKUPTABLES_H_
#define APP_UTILS_LOOKUPTABLES_H_

#include <od/extras/LookupTable.h>

namespace od
{

	class LookupTables
	{
	public:
		static void initialize();

		static LookupTable Flat;
		static LookupTable FrameOfLinearRamp;
		static LookupTable FrameOfCosineRamp;
		static LookupTable ConcaveParabolicRise;
		static LookupTable ConcaveParabolicFall;
		static LookupTable ConvexParabolicRise;
		static LookupTable ConvexParabolicFall;
		static LookupTable ConvexExponentialRise;
		static LookupTable ConcaveExponentialRise;
		static LookupTable VoltPerOctave;
		static LookupTable ToDecibels;
		static LookupTable Sine;

		static float sin(float x);
	};

} /* namespace od */

#endif /* APP_UTILS_LOOKUPTABLES_H_ */
