/*
 * EncoderHysteresis.h
 *
 *  Created on: 28 Sep 2018
 *      Author: clarkson
 */

#ifndef APP_UTILS_ENCODERHYSTERESIS_H_
#define APP_UTILS_ENCODERHYSTERESIS_H_

namespace od
{

    class EncoderHysteresis
    {
    public:
        EncoderHysteresis();
        virtual ~EncoderHysteresis();

        void setThreshold(int threshold);
        void setReset(int reset);
        int onChange(int change);

    private:
        int mSum = 0;
        int mThreshold = 1;
        int mReset = 1;
    };

} /* namespace od */

#endif /* APP_UTILS_ENCODERHYSTERESIS_H_ */
