/*****************************************************************
 * File: DisplayPara.h
 * Date: 2018/03/25 15:15
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _DISPLAYPARA_H_
#define _DISPLAYPARA_H_

#include "System.h"

#define PAGE_SIGN											(0xF000)	// 后两字节表示在哪一页，第三个字节表示页面内的第几个按钮跳转 

// 变量存储地址
// 前两位表示所在页面
// 后两位为控件类型: 
// 0x00~0x3F 数据录入和数据变量显示；
// 0xF0~0x7F 按钮键值返回；
// 0x80~0xBF 变量图标；
// 0xC0~0xFF 文本。

#define BACKLIGHT_LEV_MAX								(100)
#define BACKLIGHT_LEV_MIN								(0)
#define BACKLIGHT_INCR									(10)

#define LOCAL_REG_W										(0x80)
#define LOCAL_REG_R										(0x81)
#define VARMEM_W										(0x82)
#define VARMEM_R										(0x83)
#define CURVE_W											(0x84)   

#define BACKLIGHT_ADDR									(0x01)
#define CURVE_ADDR										(0xEB)
#define READ_TIME_ADDR									(0x20)
#define SET_TIME_ADDR									(0x1F)
#define WORK_MODE_ADDR									(0x12)
#define SCREEN_PROTECTION_LIGHT_ADDR					(0x17)
#define START_BACKLIGHT_ADDR							(0x16)
#define SCREEN_PROTECTION_TIME_ADDR						(0x18)
#define SCREEN_CONFIG_EN_ADDR							(0x1D)
#define SCREEN_SET_BPS_ADDR								(0x11)
#define SCREEN_BPS_INIT									(0x07)
#define POPUP_ADDR                                      (0x4F)

#define JUMP_TO_PIC_ADDR								(0x03)

#define COLOR_BALCK										(0xBFFF)	// 背景色，用来隐藏
#define COLOR_EDIT										(0x0AD2)	// 还原隐藏的字体



void DisplayParaInit(void);
void DisplayParaDealProcess(u8 reg, u16 displayAddr, u8 *pbuf, u8 len);

#endif // _DISPLAYPARA_H_
