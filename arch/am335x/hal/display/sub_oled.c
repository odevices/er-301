/*
 * sub_oled.c
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

#define SSD1309_SET_COL_HI 0x10
#define SSD1309_SET_COL_LO 0x00
#define SSD1309_SET_LINE 0x40
#define SSD1309_SET_CONTRAST 0x81
#define SSD1309_SET_BRIGHTNESS 0x82
#define SSD1309_SET_LOOKUP 0x91
#define SSD1309_SET_SEG_REMAP0 0xA0
#define SSD1309_SET_SEG_REMAP1 0xA1
#define SSD1309_EON_OFF 0xA4
#define SSD1309_EON_ON 0xA5
#define SSD1309_DISP_NOR 0xA6
#define SSD1309_DISP_REV 0xA7
#define SSD1309_MULTIPLEX 0xA8
#define SSD1309_MASTER_CONFIG 0xAD
#define SSD1309_CHARGE_PUMP 0x8D
#define SSD1309_PUMP_OFF 0x10
#define SSD1309_PUMP_ON 0x14
#define SSD1309_DISP_OFF 0xAE
#define SSD1309_DISP_ON 0xAF
#define SSD1309_SET_PAGE 0xB0
#define SSD1309_SET_SCAN_FLIP 0xC0
#define SSD1309_SET_SCAN_NOR 0xC8
#define SSD1309_SET_OFFSET 0xD3
#define SSD1309_SET_RATIO_OSC 0xD5
#define SSD1309_SET_AREA_COLOR 0xD8
#define SSD1309_SET_CHARGE 0xD9
#define SSD1309_SET_PADS 0xDA
#define SSD1309_SET_VCOM 0xDB
#define SSD1309_NOP 0xE3
#define SSD1309_SCROLL_RIGHT 0x26
#define SSD1309_SCROLL_LEFT 0x27
#define SSD1309_SCROLL_VR 0x29
#define SSD1309_SCROLL_VL 0x2A
#define SSD1309_SCROLL_OFF 0x2E
#define SSD1309_SCROLL_ON 0x2F
#define SSD1309_SCROLL_ON 0x2F
#define SSD1309_VERT_SCROLL_A 0xA3
#define SSD1309_MEM_ADDRESSING 0x20
#define SSD1309_SET_COL_ADDR 0x21
#define SSD1309_SET_PAGE_ADDR 0x22

#define LCDC_LIDD_CS1_ADDR (0x20)
#define LCDC_LIDD_CS1_DATA (0x24)

static inline void Command(uint32_t addr)
{
  HW_WR_REG32(SOC_LCDC_0_REGS + LCDC_LIDD_CS1_ADDR, addr);
}

static inline void Data(uint32_t data)
{
  HW_WR_REG32(SOC_LCDC_0_REGS + LCDC_LIDD_CS1_DATA, data);
}

static inline void SetDisplayRAMBounds()
{
  // set column address
  Command(SSD1309_SET_COL_ADDR);
  Command(0);
  Command(127);

  // set pageaddress
  Command(SSD1309_SET_PAGE_ADDR);
  Command(0);
  Command(7);
}

void subFill(uint32_t fill)
{
  uint32_t nbytes = 128 * 8;
  while (nbytes > 0)
  {
    nbytes--;
    Data(fill);
  }
}

void subDisplayOn(void)
{
  Command(SSD1309_DISP_ON);
}

void subInit(void)
{
  // command lock
  Command(0xFD);
  Command(0x12);

  // display off
  Command(0xAE);

  // set display clock divide ratio
  Command(0xD5);
  Command(0xA0);

  // set multiplex ratio
  Command(0xA8);
  Command(0x3F);

  // set display offset
  Command(0xD3);
  Command(0x00);

  // set display start line
  Command(0x40);

  // set segment remap
  Command(0xA1);

  // set com output scan direction
  Command(0xC8);

  // set com pins hardware configuration
  Command(0xDA);
  Command(0x12);

  // set current control
  Command(0x81);
  //Command(0xCF);
  Command(128);

  // set pre-charge period
  Command(0xD9);
  Command(0x22);

  // set vcomh deselect level
  Command(0xDB);
  Command(0x34);

  // set entire display on/off
  Command(0xA4);

  // set normal display
  Command(0xA6);

  // set memory addressing mode
  Command(SSD1309_MEM_ADDRESSING);
  Command(0x00); // horizontal addressing

  SetDisplayRAMBounds();

  subFill(0);
}

void subDrawBox(void)
{
  uint32_t page, bit, row, col, cur = 0, data;
  while (1)
  {
    for (page = 0; page < 8; page++)
    {
      for (col = 0; col < 128; col++)
      {
        data = 0;
        for (bit = 0; bit < 8; bit++)
        {
          row = page * 8 + bit;
          if (row == 0 || row == 63 || col == 0 || col == 127 || cur == row * 128 + col)
          {
            data |= (1 << bit);
          }
        }
        Data(data);
      }
    }
    cur++;
    if (cur == 128 * 64)
      cur = 0;
  }
}
