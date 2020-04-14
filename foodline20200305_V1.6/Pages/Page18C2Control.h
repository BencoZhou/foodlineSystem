/*****************************************************************
 * File: Page18C2ontrol.h
 * Date: 2019/12/04 19:02
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _PAGE18C2CONTROL_H_
#define _PAGE18C2CONTROL_H_

#include "System.h"

#define PAGE18_CONTROL_C2						(0x1200)


#define PAGE18_SELECT								(0x00)
#define PAGE18_STATE								(0x10)
#define PAGE18_BUTTON								(0x30)
#define PAGE18_ALARM								(0x40)
#define PAGE18_STATE_MACHINE_STATE					(0x12A0)


#define PAGE18_START_BUTTON							(0x12B0)
#define PAGE18_STOP_BUTTON							(0x12B1)
#define PAGE18_EMERGENCY_STOP_BUTTON				(0x12B2)
#define PAGE18_STOP_RENEW_BUTTON					(0x12B3)

void Page18C2ControlInit(void);
void Page18C2ControlProcess(u8 reg, u16 addr, u8 *pbuf, u8 len);
void Page18C2ControlRefresh(void);

#endif // _PAGE12S1S2CONTROL_H_



