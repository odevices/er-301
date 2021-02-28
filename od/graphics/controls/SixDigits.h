/*
 * SixDigits.h
 *
 *  Created on: 1 Oct 2018
 *      Author: clarkson
 */

#ifndef APP_GRAPHICS_CONTROLS_SIXDIGITS_H_
#define APP_GRAPHICS_CONTROLS_SIXDIGITS_H_

#include <od/graphics/Graphic.h>
#include <string>

namespace od
{

    class SixDigits : public Graphic
    {
    public:
        SixDigits();
        virtual ~SixDigits();

#ifndef SWIGLUA
        virtual void draw(FrameBuffer &fb);
#endif

        bool increment();
        bool decrement();
        bool encoder(int change, bool shifted, int threshold);

        void selectDigit(int i);
        void selectDigit();

        void selectDecimalPoint();
        void setDecimalPoint(int i);
        int getDecimalPoint();

        float getValue();
        int getIntegerPart();
        int getDecimalPart();
        bool isInteger();

        void setValue(float value);
        void setIntegerValue(int value);
        void zero();
        void zeroRight();
        void zeroLeft();

        void setMessage(const std::string &msg);
        void setMantissaFloor(int value);
        void setMantissaCeiling(int value);

    protected:
        std::string mMessage;
        int mMantissa = 0;
        int mDecimalPoint = 1;
        int mSelectedDigit = 1;
        int mEncoderSum = 0;
        int mMargin = 2;
        int mMantissaFloor = -999999;
        int mMantissaCeiling = 999999;
        bool mDecimalPointSelected = false;

        void drawDigit(FrameBuffer &fb, int i, int value, Color color, Justification justification);

        int mDigits[6] = {0, 0, 0, 0, 0, 0};
        Color mColors[6] = {WHITE, WHITE, WHITE, WHITE, WHITE, WHITE};
        void refresh();
    };

} /* namespace od */

#endif /* APP_GRAPHICS_CONTROLS_SIXDIGITS_H_ */
