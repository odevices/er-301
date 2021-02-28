/*
 * ssd1322.c
 *
 *  Created on: 2 Nov 2015
 *      Author: clarkson
 */

#include <stdint.h>
#include <stdbool.h>
#include <ti/am335x/lcdc.h>
#include <ti/am335x/soc.h>
#include <ti/am335x/hw_types.h>
#include <ti/sysbios/knl/Task.h>
#include "oled.h"

#define SSD1322_CMD_COL 0x15
#define SSD1322_CMD_ROW 0x75
#define SSD1322_CMD_WRITE 0x5C
#define SSD1322_CMD_READ 0x5D
#define SSD1322_CMD_DISPON 0xAF
#define SSD1322_CMD_DISPOFF 0xAE
#define SSD1322_CMD_ENGREYSCALE 0x00
#define SSD1322_CMD_MODE 0xA0
#define SSD1322_CMD_SETSTART 0xA1
#define SSD1322_CMD_DISPOFFSET 0xA2
#define SSD1322_CMD_DISPNORM 0xA6
#define SSD1322_CMD_DISPINVERT 0xA7
#define SSD1322_CMD_DISPALLON 0xA5
#define SSD1322_CMD_DISPALLOFF 0xA4
#define SSD1322_CMD_VDDSEL 0xAB
#define SSD1322_CMD_PHASELEN 0xB1
#define SSD1322_CMD_SETCLKFREQ 0xB3
#define SSD1322_CMD_DISPENHA 0xB4
#define SSD1322_CMD_SETGPIO 0xB5
#define SSD1322_CMD_SECPRECHRG 0xB6
#define SSD1322_CMD_SETGRYTABLE 0xB8
#define SSD1322_CMD_DEFGRYTABLE 0xB9
#define SSD1322_CMD_PRECHRGVOL 0xBB
#define SSD1322_CMD_SETVCOMH 0xBE
#define SSD1322_CMD_CONTRSTCUR 0xC1
#define SSD1322_CMD_MSTCONTRST 0xC7
#define SSD1322_CMD_MUXRATIO 0xCA
#define SSD1322_CMD_DISPENHB 0xD1
#define SSD1322_CMD_COMLOCK 0xFD

static inline void Command(uint32_t addr)
{
  HW_WR_REG32(SOC_LCDC_0_REGS + LCDC_LIDD_CS0_ADDR, addr);
}

static inline void Data(uint32_t data)
{
  HW_WR_REG32(SOC_LCDC_0_REGS + LCDC_LIDD_CS0_DATA, data);
}

static inline void SetDisplayRAMBounds(uint32_t row0, uint32_t row1, uint32_t col0, uint32_t col1)
{
  Command(SSD1322_CMD_ROW);
  Data(row0);
  Data(row1);
  Command(SSD1322_CMD_COL);
  Data(28 + col0);
  Data(28 + col1);
}

void mainInit(void)
{
  uint8_t A, B;

  // command lock
  Command(0xFD);
  Data(0x12);

  // display off
  Command(0xAE);

  // set display clock divide ratio
  Command(0xB3);
  Data(0x91);
  //Data(0xFF);

  // set multiplex ratio
  Command(0xCA);
  Data(0x3F);

  // set display offset
  Command(0xA2);
  Data(0x00);

  // set display start line
  Command(0xA1);
  Data(0x00);

  // set remap & dual com line mode
  Command(0xA0);
  A = 0;
  A |= (1 << 2); // nibble remap
  A |= (1 << 4); // COM scan direction: bottom to top
  B = 1;         // bit 0 must be set
  B |= (1 << 4); // enable Dual COM mode
  Data(A);
  Data(B);

  // set gpio
  Command(0xB5);
  Data(0x00);

  // function selection
  Command(0xAB);
  Data(0x01); // enable internal VDD regulator

  // enable external vsl/display enhancement
  Command(0xB4);

  Data(0b10100010); // Internal VSL (default)
  //Data(0b10100000); // Enable external VSL

  //Data(0b10110101); // Normal Display Enhancement (default)
  Data(0b11111101); // Enhanced low GS display quality

  // set contrast current
  Command(0xC1);
  Data(0x9F);

  // master contrast current control
  Command(0xC7);
  //Data(0x0F);
  Data(15); // 0-15

#if 1
  // select default linear gray scale table
  Command(0xB9);
#else
  // custom gray scale table
  Command(0xB8);
  Data(10);  // GS1
  Data(20);  // GS2
  Data(40);  // GS3
  Data(60);  // GS4
  Data(80);  // GS5
  Data(90);  // GS6
  Data(100); // GS7
  Data(110); // GS8
  Data(120); // GS9
  Data(130); // GS10
  Data(140); // GS11
  Data(150); // GS12
  Data(160); // GS13
  Data(170); // GS14
  Data(180); // GS15

  // enable gray scale
  Command(0x0);
#endif

  // set phase length
  Command(0xB1);
  Data(0xE2);

  // enhance driving scheme capability
  Command(0xD1);
  Data(0x82);
  Data(0x20);

  // set pre-charge voltage
  Command(0xBB);
  Data(0x1F);

  // set second pre-charge period
  Command(0xB6);
  Data(0x08);

  // set vcomh deselect level
  Command(0xBE);
  Data(0x07);

  // set display mode
  //Command(0xA4); // entire display off
  //Command(0xA5); // entire display on
  Command(0xA6); // normal display

  mainFill(0);
}

void mainDisplayOn(void)
{
  SetDisplayRAMBounds(0, 63, 0, 63);
  Command(SSD1322_CMD_SETSTART);
  Data(0);
  Command(SSD1322_CMD_DISPON);
  Command(SSD1322_CMD_WRITE);
}

void mainFlip(void)
{
  static uint8_t state = 0;

  switch (state)
  {
  case 0:
    SetDisplayRAMBounds(0, 63, 0, 63);
    Command(SSD1322_CMD_SETSTART);
    Data(64);
    Command(SSD1322_CMD_WRITE);
    state = 1;
    break;
  case 1:
    SetDisplayRAMBounds(64, 127, 0, 63);
    Command(SSD1322_CMD_SETSTART);
    Data(0);
    Command(SSD1322_CMD_WRITE);
    state = 0;
    break;
  }
}

void mainFill(uint32_t fill)
{
  //uint32_t nbytes = 128*120*4;
  //SetDisplayRAMBounds(0,127,0,119);
  uint32_t nbytes = 256 * 64 / 2;
  SetDisplayRAMBounds(0, 63, 0, 63);
  Command(SSD1322_CMD_WRITE);
  while (nbytes > 0)
  {
    nbytes--;
    Data(fill);
  }
}

void mainDrawBox(void)
{
  uint8_t tmp = 0;
  while (1)
  {
    mainInit();
    mainDisplayOn();

    Command(0xA2);
    Data(tmp++);

    mainFill(0);
    Task_sleep(500);
    mainFill(1 << 0);
    Task_sleep(500);

    mainFill(0);
    Task_sleep(500);
    mainFill(1 << 1);
    Task_sleep(500);

    mainFill(0);
    Task_sleep(500);
    mainFill(1 << 2);
    Task_sleep(500);

    mainFill(0);
    Task_sleep(500);
    mainFill(1 << 3);
    Task_sleep(500);

    mainFill(0);
    Task_sleep(500);
    mainFill(1 << 4);
    Task_sleep(500);

    mainFill(0);
    Task_sleep(500);
    mainFill(1 << 5);
    Task_sleep(500);

    mainFill(0);
    Task_sleep(500);
    mainFill(1 << 6);
    Task_sleep(500);

    mainFill(0);
    Task_sleep(500);
    mainFill(1 << 7);
    Task_sleep(500);

    mainFill(0xFF);
    Task_sleep(500);
    mainFill(0xFF);
    Task_sleep(500);
  }
  uint32_t row, col, cur = 0;
  SetDisplayRAMBounds(0, 63, 0, 63);
  Command(SSD1322_CMD_WRITE);
  while (1)
  {
    for (row = 0; row < 64; row++)
    {
      for (col = 0; col < 64; col++)
      {
        if (cur == row * 63 + col)
        {
          Data(0xFF);
          Data(0xFF);
        }
        else if (row == 0 || row == 63)
        {
          Data(0xFF);
          Data(0xFF);
        }
        else if (col == 0)
        {
          Data(0xF0);
          Data(0x0);
        }
        else if (col == 63)
        {
          Data(0x00);
          Data(0x0F);
        }
        else
        {
          Data(0x0);
          Data(0x0);
        }
      }
    }
    cur++;
    if (cur == 64 * 64)
      cur = 0;
  }
}

static inline void Gradient(uint32_t cur)
{
  uint32_t row, seg, val, val0, val1, state;
  for (row = 0; row < 64; row++)
  {
    val0 = val1 = state = 0;
    for (seg = 0; seg < 256; seg++)
    {
      if (seg - cur < 0xF)
      {
        val = (0xF - (seg - cur));
      }
      else if (cur - seg < 0xF)
      {
        val = (0xF - (cur - seg));
      }
      else
      {
        val = 0x0;
      }
      switch (state)
      {
      case 0:
        val0 |= val << 4;
        break;
      case 1:
        val0 |= val;
        break;
      case 2:
        val1 |= val << 4;
        break;
      case 3:
        val1 |= val;
        break;
      }
      state++;
      if (state == 4)
      {
        state = 0;
        Data(val0);
        Data(val1);
        val0 = 0;
        val1 = 0;
      }
    }
  }
}

void mainTestAnimationDoubleBuffer(void)
{
  uint32_t cur = 0, dir = 1, frame = 0;
  SetDisplayRAMBounds(0, 127, 0, 63);
  while (1)
  {
    switch (frame)
    {
    case 0:
      Command(SSD1322_CMD_WRITE);
      Gradient(cur);
      Command(SSD1322_CMD_SETSTART);
      Data(0);
      frame = 1;
      break;
    case 1:
      Command(SSD1322_CMD_WRITE);
      Gradient(cur);
      Command(SSD1322_CMD_SETSTART);
      Data(64);
      frame = 0;
      break;
    }
    if (cur >= 255)
      dir = -3;
    if (cur <= 0)
      dir = 3;
    cur += dir;
    Task_sleep(10);
  }
}

void mainTestAnimation(void)
{
  uint32_t cur = 0, dir = 1;
  SetDisplayRAMBounds(0, 63, 0, 63);
  Command(SSD1322_CMD_SETSTART);
  Data(0);
  Command(SSD1322_CMD_WRITE);
  while (1)
  {
    Gradient(cur);
    if (cur >= 255)
      dir = -3;
    if (cur <= 0)
      dir = 3;
    cur += dir;
    Task_sleep(10);
  }
}
