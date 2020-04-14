/*****************************************************************
 * File: Page12S1S2Control.h
 * Date: 2019/12/04 19:02
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _PAGE12S1S2CONTROL_H_
#define _PAGE12S1S2CONTROL_H_

#include "System.h"

#define PAGE12_TOWERSOUT_S1_10						(0x0C00)


#define PAGE12_SELECT								(0x00)
#define PAGE12_STATE								(0x10)
#define PAGE12_BUTTON								(0x30)
#define PAGE12_ALARM								(0x40)
#define PAGE12_STATE_MACHINE_STATE					(0x0C50)

#define PAGE12_S1_DELAYTIME							(0x0C22)
#define PAGE12_S2_DELAYTIME							(0x0C24)
#define PAGE12_START_BUTTON							(0x0C0D)
#define PAGE12_STOP_BUTTON							(0x0C0C)
#define PAGE12_EMERGENCY_STOP_BUTTON				(0x0C0B)
#define PAGE12_STOP_RENEW_BUTTON					(0x0C0A)

void Page12S1S2ControlInit(void);
void Page12S1S2ControlProcess(u8 reg, u16 addr, u8 *pbuf, u8 len);
void Page12S1S2ControlRefresh(void);

#endif // _PAGE12S1S2CONTROL_H_



