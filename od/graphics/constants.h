#pragma once

namespace od
{

#define BLACK 0
#define GRAY1 1
#define GRAY2 2
#define GRAY3 3
#define GRAY4 4
#define GRAY5 5
#define GRAY6 6
#define GRAY7 7
#define GRAY8 8
#define GRAY9 9
#define GRAY10 10
#define GRAY11 11
#define GRAY12 12
#define GRAY13 13
#define GRAY14 14
#define WHITE 15

#define GRID6_LINE1 55
#define GRID6_LINE2 45
#define GRID6_LINE3 35
#define GRID6_LINE4 25
#define GRID6_LINE5 15
#define GRID6_LINE6 0

#define GRID6_CENTER1 60
#define GRID6_CENTER2 50
#define GRID6_CENTER3 40
#define GRID6_CENTER4 30
#define GRID6_CENTER5 20
#define GRID6_CENTER6 7

#define GRID5_LINE1 51
#define GRID5_LINE2 39
#define GRID5_LINE3 27
#define GRID5_LINE4 15
#define GRID5_LINE5 0

#define GRID5_CENTER1 57
#define GRID5_CENTER2 45
#define GRID5_CENTER3 33
#define GRID5_CENTER4 21
#define GRID5_CENTER5 7

#define GRID4_LINE1 48
#define GRID4_LINE2 32
#define GRID4_LINE3 16
#define GRID4_LINE4 0

#define GRID4_CENTER1 56
#define GRID4_CENTER2 40
#define GRID4_CENTER3 24
#define GRID4_CENTER4 7

#define BUTTON1_CENTER 20
#define BUTTON2_CENTER 63
#define BUTTON3_CENTER 106
#define BUTTON4_CENTER 149
#define BUTTON5_CENTER 192
#define BUTTON6_CENTER 235

#define COL1_LEFT 0
#define COL2_LEFT ((BUTTON1_CENTER + BUTTON2_CENTER) / 2)
#define COL3_LEFT ((BUTTON2_CENTER + BUTTON3_CENTER) / 2)
#define COL4_LEFT ((BUTTON3_CENTER + BUTTON4_CENTER) / 2)
#define COL5_LEFT ((BUTTON4_CENTER + BUTTON5_CENTER) / 2)
#define COL6_LEFT ((BUTTON5_CENTER + BUTTON6_CENTER) / 2)

#define TEXT_HORIZONTAL 0
#define TEXT_VERTICAL 1

    enum Justification
    {
        justifyLeft,
        justifyCenter,
        justifyRight
    };

    enum Alignment
    {
        alignTop,
        alignMiddle,
        alignBottom,
    };

    enum Scaling
    {
        linearScaling,
        octaveScaling,
        decibelScaling
    };

#define ALIGN_BASE 0
#define ALIGN_TOP 1
#define ALIGN_MIDDLE 2
#define ALIGN_BOTTOM 3

    enum CheckState
    {
        checkNo,
        checkYes,
        checkPartial
    };

    enum Direction
    {
        up,
        down,
        left,
        right
    };

    enum IndicatorType
    {
        upArrowIndicator,
        downArrowIndicator,
        leftArrowIndicator,
        rightArrowIndicator,
        roundIndicator,
    };

#define SECTION_PLY 42

#define MAIN_SCREEN_WIDTH 256
#define SUB_SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

    enum SectionStyle
    {
        sectionNoBorder,
        sectionSimple,
        sectionBegin,
        sectionMiddle,
        sectionEnd
    };

    enum SelectionDrawStyle {
      straight,
      box,
      beveledBox
    };    

} /* namespace od */
