/*****************************************************************
 * File: Page2PathConfig.h
 * Date: 2019/11/21 14:30
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/

#ifndef _PAGE2PATHCONFIG_H_
#define _PAGE2PATHCONFIG_H_
#include "System.h"

#define PAGE2_DEVICE_PAGES              (5)         // 5页，每页8个设备
#define PAGE2_DEVICE_PER                (8)         // 5页，每页8个设备

#define PAGE2_PATH_TYPE1				(0x0200)	// 
#define PAGE2_PATH_AREA1                (0x0201)
#define PAGE2_PATH_MAIN_LINE1           (0x0202)
#define PAGE2_PATH_VICE_LINE1           (0x0203)
#define PAGE2_PATH_INSERT1              (0x0204)
#define PAGE2_PATH_DELETE1              (0x0205)
#define PAGE2_PATH_ID1					(0x0280)	// id + 8

#define PAGE2_PATH_INDEX                (0x02FD)    // 第几条路径，0表示第1条
#define PAGE2_PATH_SEND                 (0x02FC)    // 发送按钮
#define PAGE2_PATH_LEFT_BUTTON          (0x02FE)
#define PAGE2_PATH_RIGHT_BUTTON         (0x02FF)

#define PAGE2_PATH_CURRENT_PAGE         (0x02FA)    // 当前页
#define PAGE2_PATH_TOTAL_PAGES          (0x02FB)    // 总页数

void Page2PathConfigInit(void);
void Page2PathConfigProcess(u8 reg, u16 addr, u8 *pbuf, u8 len);
void Page2PathConfigRefresh(void);

#endif // _PAGE2PATHCONFIG_H_


