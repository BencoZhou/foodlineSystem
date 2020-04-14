/*****************************************************************
 * File: Page10TowersOut.h
 * Date: 2019/11/29 10:29
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _PAGE10TOWERSOUT_H_
#define _PAGE10TOWERSOUT_H_


#include "System.h"

#define PAGE10_TOWERS_CONTROL							(0x0A00)
#define PAGE10_TOWERS_OVERTIME							(0x0A01)
			
#define PAGE10_TOWERS_CURRENT_A							(0x0A02)
#define PAGE10_TOWERS_CURRENT_B							(0x0A03)
#define PAGE10_TOWERS_CURRENT_C							(0x0A04)
#define PAGE10_TOWERS_WORK_STATE						(0x0A05)
			
#define PAGE10_TOWERS_ALARM0_STATE						(0x0A06)
#define PAGE10_TOWERS_ALARM1_STATE						(0x0A07)
#define PAGE10_TOWERS_ALARM2_STATE						(0x0A08)
#define PAGE10_TOWERS_ALARM3_STATE						(0x0A09)
#define PAGE10_TOWERS_ALARM4_STATE						(0x0A0A)
#define PAGE10_TOWERS_ALARM5_STATE						(0x0A0B)
#define PAGE10_TOWERS_REAL_STATE						(0x0A0C)
			
#define PAGE10_TOWERS_RC_ALARM                          (0x0A0D)
			
#define PAGE10_TOWERS_CONTROL_BUTTON					(0x0A0E)
#define PAGE10_TOWERS_STATE_BUTTON						(0x0A0F)

#define PAGE10_TYPE1                                    (0x0A20)	// 
#define PAGE10_AREA1                                    (0x0A21)
#define PAGE10_MAIN_LINE1                               (0x0A22)
#define PAGE10_VICE_LINE1                               (0x0A23)
#define PAGE10_ID1                                      (0x0A30)	// id 

void Page10TowersOutInit(void);
void Page10TowersOutProcess(u8 reg, u16 addr, u8 *pbuf, u8 len);
void Page10TowersOutRefresh(void);

#endif // _PAGE10TOWERSOUT_H_



