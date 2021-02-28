/*
 * Filter.h
 *
 *  Created on: 20 Jun 2016
 *      Author: clarkson
 */

#ifndef APP_UTILS_FILTER_H_
#define APP_UTILS_FILTER_H_

#include <od/extras/Fifo.h>
#include <vector>
#include <stdint.h>

namespace od
{

	class Filter
	{
	public:
		Filter();
		virtual ~Filter();

		void setCoefficients(std::vector<float> &a, std::vector<float> &b);
		float push(float x);

		// batch mode filtering
		void filtfilt(std::vector<float> &input, std::vector<float> &output);
		void filtfilt(std::vector<float> &x);

	protected:
		Fifo<float> mX;
		Fifo<float> mY;
		std::vector<float> mA;
		std::vector<float> mB;
		uint32_t mDepth;
		bool isFIR = false;
	};

} /* namespace od */

#endif /* APP_UTILS_FILTER_H_ */
