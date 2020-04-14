/*****************************************************************
 * File: Page16E1E2Control.h
 * Date: 2019/12/04 19:02
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _PAGE16E1E2CONTROL_H_
#define _PAGE16E1E2CONTROL_H_

#include "System.h"

#define PAGE16_CONTROL_E						(0x1000)


#define PAGE16_SELECT								(0x00)
#define PAGE16_STATE								(0x10)
#define PAGE16_BUTTON								(0x30)
#define PAGE16_ALARM								(0x40)
#define PAGE16_STATE_MACHINE_STATE					(0x1050)

#define PAGE16_E1_DELAYTIME							(0x1021)
#define PAGE16_E2_DELAYTIME							(0x1023)
#define PAGE16_START_BUTTON							(0x100D)
#define PAGE16_STOP_BUTTON							(0x100C)
#define PAGE16_EMERGENCY_STOP_BUTTON				(0x100B)
#define PAGE16_STOP_RENEW_BUTTON					(0x100A)

void Page16E1E2ControlInit(void);
void Page16E1E2ControlProcess(u8 reg, u16 addr, u8 *pbuf, u8 len);
void Page16E1E2ControlRefresh(void);

#endif // _Page16E1E2CONTROL_H_



