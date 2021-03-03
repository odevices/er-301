#pragma once

#include <emu/Knob.h>
#include <emu/Button.h>
#include <emu/Jack.h>
#include <emu/Toggle.h>
#include <emu/Led.h>
#include <emu/constants.h>

#include <hal/channels.h>
#include <hal/events.h>

#include <SDL2/SDL.h>
#include <SDL_FontCache.h>

#include <stdint.h>
#include <map>
#include <array>

namespace emu
{
  struct Window
  {
    Window();
    ~Window();

    void onResized(int w, int h);

    FC_Font *getFont(int size);
    void drawCircle(int x, int y, int r);
    void drawText(int x, int y, int size, const char *fmt, ...);
    void drawTextAligned(FC_AlignEnum align, int x, int y, int size, const char *fmt, ...);
    void renderMainFrame(uint8_t *frame);
    void renderSubFrame(uint8_t *frame);
    void setScale(float scale);
    void setPosition(int x, int y, int correction = 0);
    void getPosition(int &x, int &y);
    int getTitleBarHeight();
    void update(uint8_t *mainFrame, uint8_t *subFrame);

    SDL_Window *window = 0;
    SDL_Renderer *renderer = 0;
    SDL_Texture *mainTexture = 0;
    SDL_Texture *subTexture = 0;
    SDL_PixelFormat *pixelFormat = 0;

    std::map<int, FC_Font *> fontMap;
    SDL_Rect mainRect{
        .x = MAIN_X,
        .y = MAIN_Y,
        .w = MAIN_W,
        .h = MAIN_H};
    SDL_Rect subRect{
        .x = SUB_X,
        .y = SUB_Y,
        .w = SUB_W,
        .h = SUB_H};
    SDL_Rect mainOutlineRect{
        .x = MAIN_X - 1,
        .y = MAIN_Y - 1,
        .w = MAIN_W + 2,
        .h = MAIN_H + 2};
    SDL_Rect subOutlineRect{
        .x = SUB_X - 1,
        .y = SUB_Y - 1,
        .w = SUB_W + 2,
        .h = SUB_H + 2};
    SDL_Rect panelOutlineRect{
        .x = MARGIN / 2,
        .y = MARGIN / 2,
        .w = SCREEN_WIDTH - MARGIN,
        .h = SCREEN_HEIGHT - MARGIN};
    SDL_Rect storageOutlineRect{
        .x = MARGIN / 2,
        .y = T_STORAGE_Y - 20,
        .w = (MB2_X + MB3_X) / 2 - MARGIN / 2 + 12,
        .h = MARGIN / 2 + SCREEN_HEIGHT - MARGIN - T_STORAGE_Y + 20};
    SDL_Rect abcdOutlineRect{
        .x = LED_OUT1_X,
        .y = (J4_Y + J5_Y) / 2,
        .w = MARGIN / 2 + SCREEN_WIDTH - MARGIN - LED_OUT1_X,
        .h = MARGIN / 2 + SCREEN_HEIGHT - MARGIN - (J4_Y + J5_Y) / 2};

    std::array<Button, 19> buttons{
        Button{"M1(QS)", BUTTON_MAIN1, MB1_X, MB1_Y, B_GRAY, B_GRAY, B_GRAY, "Q"},
        Button{"M2", BUTTON_MAIN2, MB2_X, MB1_Y, B_GRAY, B_GRAY, B_GRAY, "W"},
        Button{"M3", BUTTON_MAIN3, MB3_X, MB1_Y, B_GRAY, B_GRAY, B_GRAY, "E"},
        Button{"M4", BUTTON_MAIN4, MB4_X, MB1_Y, B_GRAY, B_GRAY, B_GRAY, "R"},
        Button{"M5", BUTTON_MAIN5, MB5_X, MB1_Y, B_GRAY, B_GRAY, B_GRAY, "T"},
        Button{"M6", BUTTON_MAIN6, MB6_X, MB1_Y, B_GRAY, B_GRAY, B_GRAY, "Y"},
        Button{0, BUTTON_DIAL1, MB1_X, SB1_Y, B_RED, B_GREEN, B_BLUE, "A"},
        Button{"CANCEL", BUTTON_DIAL2, MB2_X, SB1_Y, B_RED, B_GREEN, B_BLUE, "S"},
        Button{"HOME", BUTTON_DIAL3, MB3_X, SB1_Y, B_RED, B_GREEN, B_BLUE, "D"},
        Button{"S1", BUTTON_SUB1, MB4_X, SB1_Y, B_GRAY, B_GRAY, B_GRAY, "F"},
        Button{"S2", BUTTON_SUB2, MB5_X, SB1_Y, B_GRAY, B_GRAY, B_GRAY, "G"},
        Button{"S3", BUTTON_SUB3, MB6_X, SB1_Y, B_GRAY, B_GRAY, B_GRAY, "H"},
        Button{"ENTER", BUTTON_ENTER, MB4_X, HB1_Y, B_RED, B_GREEN, B_BLUE, "V"},
        Button{"UP", BUTTON_UP, MB5_X, HB1_Y, B_RED, B_GREEN, B_BLUE, "B"},
        Button{"(SHIFT)", BUTTON_SHIFT, MB6_X, HB1_Y, B_RED, B_GREEN, B_BLUE, "N"},
        Button{0, BUTTON_SELECT1, JB1_X, JB1_Y, B_GRAY, B_GRAY, B_GRAY, "1"},
        Button{0, BUTTON_SELECT2, JB1_X, JB2_Y, B_GRAY, B_GRAY, B_GRAY, "2"},
        Button{0, BUTTON_SELECT3, JB1_X, JB3_Y, B_GRAY, B_GRAY, B_GRAY, "3"},
        Button{0, BUTTON_SELECT4, JB1_X, JB4_Y, B_GRAY, B_GRAY, B_GRAY, "4"},
    };

    std::array<Jack, 24> jacks{
        Jack{INPUT_G1, J2_X, J1_Y},
        Jack{INPUT_G2, J2_X, J2_Y},
        Jack{INPUT_G3, J2_X, J3_Y},
        Jack{INPUT_G4, J2_X, J4_Y},
        Jack{INPUT_IN1, J3_X, J1_Y},
        Jack{INPUT_IN2, J3_X, J2_Y},
        Jack{INPUT_IN3, J3_X, J3_Y},
        Jack{INPUT_IN4, J3_X, J4_Y},
        Jack{OUTPUT_1, J4_X, J1_Y},
        Jack{OUTPUT_2, J4_X, J2_Y},
        Jack{OUTPUT_3, J4_X, J3_Y},
        Jack{OUTPUT_4, J4_X, J4_Y},
        Jack{INPUT_A1, J1_X, J5_Y},
        Jack{INPUT_A2, J1_X, J6_Y},
        Jack{INPUT_A3, J1_X, J7_Y},
        Jack{INPUT_B1, J2_X, J5_Y},
        Jack{INPUT_B2, J2_X, J6_Y},
        Jack{INPUT_B3, J2_X, J7_Y},
        Jack{INPUT_C1, J3_X, J5_Y},
        Jack{INPUT_C2, J3_X, J6_Y},
        Jack{INPUT_C3, J3_X, J7_Y},
        Jack{INPUT_D1, J4_X, J5_Y},
        Jack{INPUT_D2, J4_X, J6_Y},
        Jack{INPUT_D3, J4_X, J7_Y}};

    std::array<Toggle, 2> toggles{
        Toggle{"STORAGE", TOGGLE_STORAGE_A, TOGGLE_STORAGE_B, T_STORAGE_X, T_STORAGE_Y, "Z", "user", "admin", "eject"},
        Toggle{"MODE", TOGGLE_MODE_A, TOGGLE_MODE_B, T_MODE_X, T_MODE_Y, "X", "hold", "edit", "scope"}};

    std::array<Led, 11> leds{
        RedLed{"fine", LED_DIAL1, LED_DIAL1_X, LED_DIAL1_Y, false},
        RedLed{"coarse", LED_DIAL2, LED_DIAL2_X, LED_DIAL2_Y, false},
        RedLed{"I/O", LED_IO, LED_IO_X, LED_IO_Y},
        RedLed{"safe", LED_SAFE, LED_IO_X, LED_SAFE_Y},
        OrangeLed{LED_OUT1, LED_OUT1_X, LED_OUT1_Y},
        OrangeLed{LED_OUT2, LED_OUT1_X, LED_OUT2_Y},
        OrangeLed{LED_OUT3, LED_OUT1_X, LED_OUT3_Y},
        OrangeLed{LED_OUT4, LED_OUT1_X, LED_OUT4_Y},
        RedLed{"linked", LED_LINK12, LED_OUT1_X, LED_LINK12_Y},
        RedLed{"linked", LED_LINK23, LED_OUT1_X, LED_LINK23_Y},
        RedLed{"linked", LED_LINK34, LED_OUT1_X, LED_LINK34_Y}};

    Knob knob;

    int width = SCREEN_WIDTH;
    int height = SCREEN_HEIGHT;
    double pauseDisplayTime = 0;
    float scale = 1.0f;
  };
}; // namespace emu