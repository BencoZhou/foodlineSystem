/*****************************************************************
 * File: PAGE17C2ontrol.h
 * Date: 2019/12/04 19:02
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _PAGE17C1CONTROL_H_
#define _PAGE17C1CONTROL_H_

#include "System.h"

#define PAGE17_CONTROL_C1						(0x1100)


#define PAGE17_SELECT								(0x00)
#define PAGE17_STATE								(0x10)
#define PAGE17_BUTTON								(0x30)
#define PAGE17_ALARM								(0x40)
#define PAGE17_STATE_MACHINE_STATE					(0x11A0)

#define PAGE17_C1_DELAYTIME							(0x1123)
#define PAGE17_C2_DELAYTIME							(0x112B)
#define PAGE17_START_BUTTON							(0x11B0)
#define PAGE17_STOP_BUTTON							(0x11B1)
#define PAGE17_EMERGENCY_STOP_BUTTON				(0x11B2)
#define PAGE17_STOP_RENEW_BUTTON					(0x11B3)

void Page17C1ControlInit(void);
void Page17C1ControlProcess(u8 reg, u16 addr, u8 *pbuf, u8 len);
void Page17C1ControlRefresh(void);

#endif // _PAGE12S1S2CONTROL_H_



