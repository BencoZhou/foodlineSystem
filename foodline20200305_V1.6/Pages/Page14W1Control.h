/*****************************************************************
 * File: Page12S1S2Control.h
 * Date: 2019/12/04 19:02
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _PAGE14W1CONTROL_H_
#define _PAGE14W1CONTROL_H_

#include "System.h"

#define PAGE14_CONTROL_W1						(0x0E00)


#define PAGE14_SELECT								(0x00)
#define PAGE14_STATE								(0x10)
#define PAGE14_BUTTON								(0x30)
#define PAGE14_ALARM								(0x40)
#define PAGE14_STATE_MACHINE_STATE					(0x0EA0)

#define PAGE14_W1_DELAYTIME							(0x0E21)
#define PAGE14_START_BUTTON							(0x0EB0)
#define PAGE14_STOP_BUTTON							(0x0EB1)
#define PAGE14_EMERGENCY_STOP_BUTTON				(0x0EB2)
#define PAGE14_STOP_RENEW_BUTTON					(0x0EB3)

void Page14W1ControlInit(void);
void Page14W1ControlProcess(u8 reg, u16 addr, u8 *pbuf, u8 len);
void Page14W1ControlRefresh(void);

#endif // _PAGE12S1S2CONTROL_H_



