/*****************************************************************
 * File: PAGE19_C2ontrol.h
 * Date: 2019/12/04 19:02
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _PAGE19N1CONTROL_H_
#define _PAGE19N1CONTROL_H_

#include "System.h"

#define PAGE19_CONTROL_N1						(0x1300)


#define PAGE19_SELECT								(0x00)
#define PAGE19_STATE								(0x10)
#define PAGE19_BUTTON								(0x30)
#define PAGE19_ALARM								(0x40)
#define PAGE19_STATE_MACHINE_STATE					(0x13A0)

#define PAGE19_N1_DELAYTIME							(0x1324)
#define PAGE19_START_BUTTON							(0x13B0)
#define PAGE19_STOP_BUTTON							(0x13B1)
#define PAGE19_EMERGENCY_STOP_BUTTON				(0x13B2)
#define PAGE19_STOP_RENEW_BUTTON					(0x13B3)

void Page19N1ControlInit(void);
void Page19N1ControlProcess(u8 reg, u16 addr, u8 *pbuf, u8 len);
void Page19N1ControlRefresh(void);

#endif // _PAGE12S1S2CONTROL_H_



