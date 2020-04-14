/*****************************************************************
 * File: Page2PathConfig.h
 * Date: 2019/11/21 14:30
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/

#ifndef _PAGE13LINKCONFIG_H_
#define _PAGE13LINKCONFIG_H_
#include "System.h"

#define PAGE13_DEVICE_PAGES              (5)         // 5页，每页8个设备
#define PAGE13_DEVICE_PER                (8)         // 5页，每页8个设备


#define PAGE13_PREV_LINK_TYPE1				(0x0D06)	// 
#define PAGE13_PREV_LINK_AREA1				(0x0D07)	// 
#define PAGE13_PREV_LINK_MAIN_LINE1			(0x0D08)	// 
#define PAGE13_PREV_LINK_VICE_LINE1 		(0x0D09)	// 
#define PAGE13_LINK_TYPE1				(0x0D00)	// 
#define PAGE13_LINK_AREA1                (0x0D01)
#define PAGE13_LINK_MAIN_LINE1           (0x0D02)
#define PAGE13_LINK_VICE_LINE1           (0x0D03)
#define PAGE13_LINK_INSERT1              (0x0D04)
#define PAGE13_LINK_DELETE1              (0x0D05)

#define PAGE13_LINK_SAVE                 (0x0DF9)    // 保存并发送命令让执行器保存按钮

#define PAGE13_LINK_LEFT_BUTTON          (0x0DFE)
#define PAGE13_LINK_RIGHT_BUTTON         (0x0DFF)

#define PAGE13_LINK_CURRENT_PAGE         (0x0DFA)    // 当前页
#define PAGE13_LINK_TOTAL_PAGES          (0x0DFB)    // 总页数


#define PAGE13_LINK_HEARD_AREA           (0x0Dc0)


void Page13LinkConfigInit(void);
void Page13LinkConfigProcess(u8 reg, u16 addr, u8 *pbuf, u8 len);
void Page13LinkConfigRefresh(void);

#endif // _PAGE2LINKCONFIG_H_


