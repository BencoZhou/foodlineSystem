/*****************************************************************
 * File: Page19N2ontrol.h
 * Date: 2019/12/04 19:02
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _PAGE20N2CONTROL_H_
#define _PAGE20N2CONTROL_H_

#include "System.h"

#define PAGE20_CONTROL_N2						(0x1400)


#define PAGE20_SELECT								(0x00)
#define PAGE20_STATE								(0x10)
#define PAGE20_BUTTON								(0x30)
#define PAGE20_ALARM								(0x40)
#define PAGE20_STATE_MACHINE_STATE					(0x14A0)

#define PAGE20_N2_DELAYTIME							(0x1420)
#define PAGE20_START_BUTTON							(0x14B0)
#define PAGE20_STOP_BUTTON							(0x14B1)
#define PAGE20_EMERGENCY_STOP_BUTTON				(0x14B2)
#define PAGE20_STOP_RENEW_BUTTON					(0x14B3)

void Page20N2ControlInit(void);
void Page20N2ControlProcess(u8 reg, u16 addr, u8 *pbuf, u8 len);
void Page20N2ControlRefresh(void);

#endif // _PAGE12S1S2CONTROL_H_



