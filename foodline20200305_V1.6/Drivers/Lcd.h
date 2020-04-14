#ifndef MZSYS_LCD_HANDLER_H
#define MZSYS_LCD_HANDLER_H


#include "stm32f10x.h"
#include "stm32f10x_pwr.h"
#include "Debug.h"
#include "ucos_ii.h"
#include "PeripheralIO.h"


#define SPICS0  PBout(7)
#define SPICS1  PBout(6)
#define SPICS2  PBout(5)

#define SPIDAT  PBout(9)
#define SPIWR   PBout(8)


#define DRY_TEMP_PART_TEMP          1       // switch part temp
#define WET_TEMP_PHASE_LOSS         2       // switch phase loss
#define DRY_TEMP_TARGET_OVERLOAD    3       // switch overload
#define WET_TEMP_TARGET_VAL         4
#define BAKE_STAGE_TIME             5
#define BAKE_TOTLE_TIME             6
#define DRY_BAKE_CURVE_VAL          7       // constant rise temp
#define WET_BAKE_CURVE_VAL          8
#define BAKE_CURVE_STAGE_TIME       9       // 63 switch clean wet  65 switch help fire
#define BAKE_VOLTAGE_VALUE          10      // switch auto and high (auto low high)
#define BAKE_STAGE_SEQUENCE         11
#define LCD_SYTEM_TIME_YEAR         12      // switch self-define
#define LCD_SYTEM_TIME_MONTH        13      // switch down
#define LCD_SYTEM_TIME_DAY          14
#define LCD_SYTEM_TIME_HOUR         15      // switch mid
#define LCD_SYTEM_TIME_MIN          16      // switch up
#define LCD_LOAD_SMOKE_VOLUME       17

#define DISPLAY_LOGO_FRAME          18      // switch low (auto low high)
#define SWITCH_UP_DOWN_ROOM         19

#define CLEAR_ALL_CHARS             0x80
#define SHOW_ONLY_BAR               0x40
#define SHOW_CHARS_MASK             0x3f
#define SHOW_CHARS_SPEC             0xc0


#if USE_RTC_WITH_BATTERY == 0
#define SCREEN_GREEN    PCout(14)
#define SCREEN_RED      PCout(13)
#else
#define SCREEN_GREEN    PCout(13)
#endif


#define Lcd_Sector_1    0
#define Lcd_Sector_2    1
#define Lcd_Sector_3    2
#define Lcd_Sector_4    3
#define Lcd_Sector_5    4
#define Lcd_Sector_6    5
#define Lcd_Sector_7    6
#define Lcd_Sector_8    7



void Lcd_Init(void);
void Lcd_Display(u8 mode, u8 index, u8 frame);
void Lcd_WriteNum(u8 index, u8 num, u8 xorval, u8 clrs, u8 bar);

#endif

