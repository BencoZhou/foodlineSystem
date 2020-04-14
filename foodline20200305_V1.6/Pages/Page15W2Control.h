/*****************************************************************
 * File: PAGE15W2ontrol.h
 * Date: 2019/12/04 19:02
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _PAGE15W2CONTROL_H_
#define _PAGE15W2CONTROL_H_

#include "System.h"

#define PAGE15_CONTROL_W2						(0x0F00)


#define PAGE15_SELECT								(0x00)
#define PAGE15_STATE								(0x10)
#define PAGE15_BUTTON								(0x30)
#define PAGE15_ALARM								(0x40)
#define PAGE15_STATE_MACHINE_STATE					(0x0FA0)

#define PAGE15_W2_DELAYTIME							(0x0F21)
#define PAGE15_START_BUTTON							(0x0FB0)
#define PAGE15_STOP_BUTTON							(0x0FB1)
#define PAGE15_EMERGENCY_STOP_BUTTON				(0x0FB2)
#define PAGE15_STOP_RENEW_BUTTON					(0x0FB3)

void Page15W2ControlInit(void);
void Page15W2ControlProcess(u8 reg, u16 addr, u8 *pbuf, u8 len);
void Page15W2ControlRefresh(void);

#endif // _PAGE12S1S2CONTROL_H_



