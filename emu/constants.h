#pragma once

#include <hal/display.h>

namespace emu
{
  const int OVERSAMPLE = 4;

  const int SCREEN_WIDTH = 505;
  const int SCREEN_HEIGHT = 370;
  const int SCREEN_BRIGHTNESS = 15; // 1-16
  const float SCREEN_TINT = 0.85f;

  // status area
  const int STATUS_H = 24;

  // panel color
  const int P_GRAY = 184;

  // button colors
  const int B_GRAY = 98;
  const int B_RED = 81;
  const int B_GREEN = 124;
  const int B_BLUE = 245;

  // toggle colors
  const int T_GRAY = 25;

  // led colors
  const int RL_RED = 230;
  const int RL_GREEN = 0;
  const int RL_BLUE = 0;
  const int OL_RED = 220;
  const int OL_GREEN = 110;
  const int OL_BLUE = 0;

  const int ENCODER_SPEED = 5;

  // outer margin
  const int MARGIN = 16;

  // main display
  const int MAIN_X = MARGIN;
  const int MAIN_Y = MARGIN;
  const int MAIN_W = MAIN_HORIZONTAL_PIXELS;
  const int MAIN_H = MAIN_VERTICAL_PIXELS;

  // buttons
  const int B_SPACING = 6;   // button-button spacing
  const int DB_SPACING = 24; // button-display spacing
  const int BUTTON_W = MAIN_W / 6 - B_SPACING;
  const int BUTTON_H = BUTTON_W;

  // main buttons
  const int MB1_X = MAIN_X + B_SPACING / 2 + 2;
  const int MB1_Y = MAIN_Y + MAIN_H + DB_SPACING;
  const int MB2_X = MB1_X + BUTTON_W + B_SPACING;
  const int MB3_X = MB2_X + BUTTON_W + B_SPACING;
  const int MB4_X = MB3_X + BUTTON_W + B_SPACING;
  const int MB5_X = MB4_X + BUTTON_W + B_SPACING;
  const int MB6_X = MB5_X + BUTTON_W + B_SPACING;

  // knob
  const int KNOB_X = MAIN_X;
  const int KNOB_Y = MB1_Y + BUTTON_H + B_SPACING;
  const int KNOB_W = MAIN_W / 2 - B_SPACING;
  const int KNOB_H = 100;
  const int KNOB_R = 40;
  const double KNOB_SPEED = 360.0 * ENCODER_SPEED / 128.0;

  // sub display
  const int SUB_W = SUB_HORIZONTAL_PIXELS;
  const int SUB_H = SUB_VERTICAL_PIXELS;
  const int SUB_X = MAIN_X + MAIN_W / 2;
  const int SUB_Y = KNOB_Y + KNOB_H - SUB_H - 12;

  // sub buttons
  const int SB1_Y = SUB_Y + SUB_H + DB_SPACING;

  // hard buttons
  const int HB1_Y = SB1_Y + BUTTON_H + DB_SPACING;

  // jacks
  const int J_OUTER = 17;
  const int J_INNER = 8;
  const int J_HILITE = 10;

  const int J_DIVIDER = 50;
  const int J7_Y = HB1_Y + BUTTON_H / 2;
  const int J1_Y = MAIN_Y + J_OUTER;
  const int J_VSPACING = (J7_Y - J1_Y) / 6;
  const int J2_Y = J1_Y + J_VSPACING;
  const int J3_Y = J2_Y + J_VSPACING;
  const int J4_Y = J3_Y + J_VSPACING;
  const int J5_Y = J4_Y + J_VSPACING;
  const int J6_Y = J5_Y + J_VSPACING;
  const int J_HSPACING = 2 * (J_OUTER + J_INNER);
  const int J1_X = MAIN_X + MAIN_W + J_DIVIDER;
  const int J2_X = J1_X + J_HSPACING;
  const int J3_X = J2_X + J_HSPACING;
  const int J4_X = J3_X + J_HSPACING;

  const int JB1_X = J1_X - BUTTON_W / 2;
  const int JB1_Y = J1_Y - BUTTON_H / 2;
  const int JB2_Y = J2_Y - BUTTON_H / 2;
  const int JB3_Y = J3_Y - BUTTON_H / 2;
  const int JB4_Y = J4_Y - BUTTON_H / 2;

  // toggles
  const int TOGGLE_UP = 3;
  const int TOGGLE_MID = 2;
  const int TOGGLE_DOWN = 1;
  const int TOGGLE_W = BUTTON_W + B_SPACING;
  const int TOGGLE_H = BUTTON_H;
  const int T_R1 = 2;
  const int T_R2 = 6;
  const int T_MARGIN = 2 * T_R2 + 2;
  const int T_FONT = 11;
  const int T_SPACING = 12;
  const int T_STORAGE_X = MB1_X - 5;
  const int T_STORAGE_Y = J7_Y - TOGGLE_H / 2;
  const int T_MODE_X = MB3_X - 5;
  const int T_MODE_Y = T_STORAGE_Y;

  // leds
  const int LED_R = 6;
  const int LED_DIAL1_X = MB1_X + LED_R + 4;
  const int LED_DIAL1_Y = SUB_Y + SUB_H - 2 * LED_R;
  const int LED_DIAL2_X = LED_DIAL1_X + BUTTON_W / 2;
  const int LED_DIAL2_Y = LED_DIAL1_Y + BUTTON_H / 2;
  const int LED_IO_X = MB2_X + LED_R;
  const int LED_IO_Y = J7_Y - BUTTON_W / 4;
  const int LED_SAFE_Y = J7_Y + BUTTON_W / 4;
  const int LED_OUT1_X = (MAIN_X + MAIN_W + JB1_X) / 2;
  const int LED_OUT1_Y = JB1_Y + BUTTON_H / 2;
  const int LED_OUT2_Y = JB2_Y + BUTTON_H / 2;
  const int LED_OUT3_Y = JB3_Y + BUTTON_H / 2;
  const int LED_OUT4_Y = JB4_Y + BUTTON_H / 2;
  const int LED_LINK12_Y = (JB1_Y + JB2_Y) / 2 + BUTTON_W / 2;
  const int LED_LINK23_Y = (JB2_Y + JB3_Y) / 2 + BUTTON_W / 2;
  const int LED_LINK34_Y = (JB3_Y + JB4_Y) / 2 + BUTTON_W / 2;

} // namespace emu