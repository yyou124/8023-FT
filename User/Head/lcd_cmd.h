/******************** (C) COPYRIGHT  上海芯北电子有限公司 ***********************
 * 文件�? ：LCDCMD.h
 * 描述    ：LCD驱动芯片 控制命令
 * 库版�? ：V2.1.0
 * 作�?   ：由德强
 *修改时间 �?017-07-04

*******************************************************************************/
#ifndef __lcd_cmd_H__
#define __lcd_cmd_H__
#include "stm8s.h"



#define countof(a) (sizeof(a) / sizeof(*(a)))
#define BU_WRITE_ADDRESS  0x7C
#define BU_READ_ADDRESS   0x7D

 /*//BR8236Cmond
enum
{
  BR_DISPLAY_UN = 0xC0, // lcd display off, 1/3Bias, Duty = 4COM
  BR_DISPLAY_EN = 0xC8, //lcd display on, 1/3Bias, 4COM
  BR_DISPLAY_EN_LAST = 0x60, //last command of BR_DISPLAY_ON
  BR_RAM_ADDRESS = 0x80, //RAM write begin address (00_0000)
  BR_RAM_ADDRESS_LAST = 0x00, // last command of BR_RAM_ADDRESS
  BR_FLASH_UN = 0xF0, //
  BR_NORMAL_CLK_OFF = 0xE0, //Normal mode, clock output, clk = 0
};
//cn55070 CMD
enum
{
  CN_NORMAL_DISPLAY_UN = 0xC3, //normal mode, 1/3Bias, 4COM, display off
  CN_NORMAL_DISPLAY_EN = 0xCB, //normal mode, 1/3Bias, 4COM, display on
  CN_NORMAL_DISPLAY_EN_LAST = 0x4B, //last command 0f CN_NORMAL_DISPLAY_ON
  CN_RAM_ADDRESS = 0x80, //RAM write begin address (00_0000)
  CN_RAM_ADDRESS_LAST =0x00, //last command of CN_RAM_ADDRESS
  CN_FLASH_UN = 0xF0, //
  CN_SLEEP_UN = 0xF4, //
};*/
//BU91R61CH-3BW CMD
enum
{
  ICSET_RST = 0xEA, //Software Reset
  ICSET_MSB = 0xE8, //Set MSB of RAM address (P2 of ICSET_MSB) = 0
  ICSET_MSB_ONE = 0xEC, //Set MSB of RAM address (P2 of ICSET_MSB) = 1
  MODESET_EN = 0xC8, //display ON, 1/3 bias, 1/4 duty, Direct Mode OFF
  MODESET_UN = 0xC0, //display OFF
  DISCTL_NORM = 0xA2, //
  BLKCTL_UN = 0xF0, //Blink Mode OFF
  APCTL = 0xF8, //set all pixel display normal
  APCTL_ON = 0xFA, //set all pixel on
  APCTL_OFF = 0xF9, //set all pixel off
  EVRSET_100 = 0xE0, //Constrast Setting 1.000*VDD
  ADSET = 0x80, //set RAM address P4-P0 = 0_0000
  ADSET_LAST = 0x00, //last command, set RAM address P4-P0 = 0_0000
  ADSET_CMD = 0x8C, //set RAM address P4-P0 = 0_1100
  BLKCTL_EN = 0XF3,
};

u8 CMD_INIT [6] = {
  ICSET_RST,
  BLKCTL_UN,
  DISCTL_NORM,
  ICSET_MSB,
  MODESET_UN,
  ADSET_LAST,
};

u8 LINE_TO_FRAME [1] = {
  0xA6,//L
  //ADSET_CMD, //
  //ADSET_CMD, //
};
u8 READ_DATA [3] = {
  ICSET_MSB,
  ADSET,
  ADSET,
};

u8 CMD_ALLOFF [] = {
  MODESET_EN,
  ADSET_LAST,
  0x00, //address 0x00 - 0x01 (SEG0 - SEG1)
  0x00, //address 0x02 - 0x03 (SEG2 - SEG3)
  0x00, //address 0x04 - 0x05 (SEG4 - SEG5)
  0x00, //address 0x06 - 0x07 (SEG6 - SEG7)
  0x00, //address 0x08 - 0x09 (SEG8 - SEG9)
  0x00, //address 0x0A - 0x0B (SEG10 - SEG11)
  0x00, //address 0x0C - 0x0D (SEG12 - SEG13)
  0x00, //address 0x0E - 0x0F (SEG14 - SEG15)
  0x00, //address 0x10 - 0x11 (SEG16 - SEG17)
  0x00, //address 0x12 - 0x13 (SEG18 - SEG19)
  0x00, //address 0x14 - 0x15 (SEG20 - SEG21)
  0x00, //address 0x16 - 0x17 (SEG22 - SEG23)
  0x00, //address 0x18 - 0x19 (SEG24 - SEG25)
  0x00, //address 0x1A - 0x1B (SEG26 - SEG27)
  0x00, //address 0x1C - 0x1D (SEG28 - SEG29)
  0x00, //address 0x1E - 0x1F (SEG30 - SEG31)
  0x00, //address 0x20 - 0x21 (SEG32 - SEG33)
  0x00, //address 0x22 - 0x23 (SEG34 - SEG35)
  0x00, //address 0x24 - 0x25 (SEG36 - SEG37)
  0x00, //address 0x26 - 0x27 (SEG38 - SEG39)
  0x00, //address 0x28 - 0x29 (SEG40 - SEG41)
  0x00, //address 0x2A - 0x2B (SEG42 - SEG43)
};

u8 CMD_ALLON [] = {
  MODESET_EN,
  ADSET_LAST,
  0xFF, //address 0x00 - 0x01 (SEG0 - SEG1)
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF,
  0xFF, //address 0x22 - 0x23 (SEG34 - SEG35)
  0xFF,
  0xFF,
  0xFF,
  0xFF, //address 0x2A - 0x2B (SEG42 - SEG43)
};

u8 CMD_ALL55 [] = {
    MODESET_EN,
    ADSET_LAST,
    0x55, //address 0x00 - 0x01 (SEG0 - SEG1)
    0x55, //address 0x02 - 0x03 (SEG2 - SEG3)
    0x55, //address 0x04 - 0x05 (SEG4 - SEG5)
    0x55, //address 0x06 - 0x07 (SEG6 - SEG7)
    0x55, //address 0x08 - 0x09 (SEG8 - SEG9)
    0x55, //address 0x0A - 0x0B (SEG10 - SEG11)
    0x55, //address 0x0C - 0x0D (SEG12 - SEG13)
    0x55, //address 0x0E - 0x0F (SEG14 - SEG15)
    0x55, //address 0x10 - 0x11 (SEG16 - SEG17)
    0x55, //address 0x12 - 0x13 (SEG18 - SEG19)
    0x55, //address 0x14 - 0x15 (SEG20 - SEG21)
    0x55, //address 0x16 - 0x17 (SEG22 - SEG23)
    0x55, //address 0x18 - 0x19 (SEG24 - SEG25)
    0x55, //address 0x1A - 0x1B (SEG26 - SEG27)
    0x55, //address 0x1C - 0x1D (SEG28 - SEG29)
    0x55, //address 0x1E - 0x1F (SEG30 - SEG31)
    0x55, //address 0x20 - 0x21 (SEG32 - SEG33)
    0x55, //address 0x22 - 0x23 (SEG34 - SEG35)
    0x55, //address 0x24 - 0x25 (SEG36 - SEG37)
    0x55, //address 0x26 - 0x27 (SEG38 - SEG39)
    0x55, //address 0x28 - 0x29 (SEG40 - SEG41)
    0x55, //address 0x2A - 0x2B (SEG42 - SEG43)
};


u8 CMD_ALLAA [] = {
    MODESET_EN,
    ADSET_LAST,
    0xAA, //address 0x00 - 0x01 (SEG0 - SEG1)
    0xAA, //address 0x02 - 0x03 (SEG2 - SEG3)
    0xAA, //address 0x04 - 0x05 (SEG4 - SEG5)
    0xAA, //address 0x06 - 0x07 (SEG6 - SEG7)
    0xAA, //address 0x08 - 0x09 (SEG8 - SEG9)
    0xAA, //address 0x0A - 0x0B (SEG10 - SEG11)
    0xAA, //address 0x0C - 0x0D (SEG12 - SEG13)
    0xAA, //address 0x0E - 0x0F (SEG14 - SEG15)
    0xAA, //address 0x10 - 0x11 (SEG16 - SEG17)
    0xAA, //address 0x12 - 0x13 (SEG18 - SEG19)
    0xAA, //address 0x14 - 0x15 (SEG20 - SEG21)
    0xAA, //address 0x16 - 0x17 (SEG22 - SEG23)
    0xAA, //address 0x18 - 0x19 (SEG24 - SEG25)
    0xAA, //address 0x1A - 0x1B (SEG26 - SEG27)
    0xAA, //address 0x1C - 0x1D (SEG28 - SEG29)
    0xAA, //address 0x1E - 0x1F (SEG30 - SEG31)
    0xAA, //address 0x20 - 0x21 (SEG32 - SEG33)
    0xAA, //address 0x22 - 0x23 (SEG34 - SEG35)
    0xAA, //address 0x24 - 0x25 (SEG36 - SEG37)
    0xAA, //address 0x26 - 0x27 (SEG38 - SEG39)
    0xAA, //address 0x28 - 0x29 (SEG40 - SEG41)
    0xAA, //address 0x2A - 0x2B (SEG42 - SEG43)
};
u8 CMD_ALL5A [] = {
    MODESET_EN,
    ADSET_LAST,
    0x5A, //address 0x00 - 0x01 (SEG0 - SEG1)
    0x5A, //address 0x02 - 0x03 (SEG2 - SEG3)
    0x5A, //address 0x04 - 0x05 (SEG4 - SEG5)
    0x5A, //address 0x06 - 0x07 (SEG6 - SEG7)
    0x5A, //address 0x08 - 0x09 (SEG8 - SEG9)
    0x5A, //address 0x0A - 0x0B (SEG10 - SEG11)
    0x5A, //address 0x0C - 0x0D (SEG12 - SEG13)
    0x5A, //address 0x0E - 0x0F (SEG14 - SEG15)
    0x5A, //address 0x10 - 0x11 (SEG16 - SEG17)
    0x5A, //address 0x12 - 0x13 (SEG18 - SEG19)
    0x5A, //address 0x14 - 0x15 (SEG20 - SEG21)
    0x5A, //address 0x16 - 0x17 (SEG22 - SEG23)
    0x5A, //address 0x18 - 0x19 (SEG24 - SEG25)
    0x5A, //address 0x1A - 0x1B (SEG26 - SEG27)
    0x5A, //address 0x1C - 0x1D (SEG28 - SEG29)
    0x5A, //address 0x1E - 0x1F (SEG30 - SEG31)
    0x5A, //address 0x20 - 0x21 (SEG32 - SEG33)
    0x5A, //address 0x22 - 0x23 (SEG34 - SEG35)
    0x5A, //address 0x24 - 0x25 (SEG36 - SEG37)
    0x5A, //address 0x26 - 0x27 (SEG38 - SEG39)
    0x5A, //address 0x28 - 0x29 (SEG40 - SEG41)
    0x5A, //address 0x2A - 0x2B (SEG42 - SEG43)
};
u8 CMD_ALLA5 [] = {
    MODESET_EN,
    ADSET_LAST,
    0xA5, //address 0x00 - 0x01 (SEG0 - SEG1)
    0xA5, //address 0x02 - 0x03 (SEG2 - SEG3)
    0xA5, //address 0x04 - 0x05 (SEG4 - SEG5)
    0xA5, //address 0x06 - 0x07 (SEG6 - SEG7)
    0xA5, //address 0x08 - 0x09 (SEG8 - SEG9)
    0xA5, //address 0x0A - 0x0B (SEG10 - SEG11)
    0xA5, //address 0x0C - 0x0D (SEG12 - SEG13)
    0xA5, //address 0x0E - 0x0F (SEG14 - SEG15)
    0xA5, //address 0x10 - 0x11 (SEG16 - SEG17)
    0xA5, //address 0x12 - 0x13 (SEG18 - SEG19)
    0xA5, //address 0x14 - 0x15 (SEG20 - SEG21)
    0xA5, //address 0x16 - 0x17 (SEG22 - SEG23)
    0xA5, //address 0x18 - 0x19 (SEG24 - SEG25)
    0xA5, //address 0x1A - 0x1B (SEG26 - SEG27)
    0xA5, //address 0x1C - 0x1D (SEG28 - SEG29)
    0xA5, //address 0x1E - 0x1F (SEG30 - SEG31)
    0xA5, //address 0x20 - 0x21 (SEG32 - SEG33)
    0xA5, //address 0x22 - 0x23 (SEG34 - SEG35)
    0xA5, //address 0x24 - 0x25 (SEG36 - SEG37)
    0xA5, //address 0x26 - 0x27 (SEG38 - SEG39)
    0xA5, //address 0x28 - 0x29 (SEG40 - SEG41)
    0xA5, //address 0x2A - 0x2B (SEG42 - SEG43)
};
#endif