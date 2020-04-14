/*****************************************************************
 * File: Page9SwitchValveConfig.h
 * Date: 2019/11/29 9:03
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _PAGE9SWITCHVALVECONFIG_H_
#define _PAGE9SWITCHVALVECONFIG_H_


#include "System.h"

#define PAGE9_ALARM_OPEN_SHOW						(0x0900)
#define PAGE9_ALARM_CLOSE_SHOW						(0x0901)
#define PAGE9_ALARM_OPEN_BUTTON						(0x0902)
#define PAGE9_ALARM_CLOSE_BUTTON					(0x0903)
#define PAGE9_STATETIME								(0x0904)
#define PAGE9_OPEN_OVERTIME							(0x0905)

#define PAGE9_TYPE1                                 (0x0920)	// 
#define PAGE9_AREA1                                 (0x0921)
#define PAGE9_MAIN_LINE1                            (0x0922)
#define PAGE9_VICE_LINE1                            (0x0923)
#define PAGE9_ID1                                   (0x0930)	// id 

#define PAGE9_INQUIRY                               (0x0906)
#define PAGE9_SEND                                  (0x0907)


void Page9SwitchValveConfigInit(void);
void Page9SwitchValveConfigProcess(u8 reg, u16 addr, u8 *pbuf, u8 len);
void Page9SwitchValveConfigRefresh(void);

#endif // _PAGE9SWITCHVALVECONFIG_H_

