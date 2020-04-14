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
#define PAGE12_TOWERSOUT_S1_20						(0x0C01)
#define PAGE12_CONTROLFLINE_S1_00					(0x0C02)
#define PAGE12_SWITCHVALVE							(0x0C03)
#define PAGE12_CONTROLFLINE_S2_00					(0x0C04)
#define PAGE12_INTOWERS_S2_10						(0x0C05)
#define PAGE12_INTOWERS_S2_20						(0x0C06)
#define PAGE12_INTOWERS_S2_30						(0x0C07)
#define PAGE12_INTOWERS_S2_40						(0x0C08)
#define PAGE12_INTOWERS_S2_50						(0x0C09)

#define PAGE12_SELECT								(0x00)
#define PAGE12_STATE								(0x10)
#define PAGE12_BUTTON								(0x30)
#define PAGE12_ALARM								(0x40)

#define PAGE12_S1_DELAYTIME							(0x0C0F)
#define PAGE12_S2_DELAYTIME							(0x0C0E)

void Page12S1S2ControlInit(void);
void Page12S1S2ControlProcess(u8 reg, u16 addr, u8 *pbuf, u8 len);
void Page12S1S2ControlRefresh(void);

#endif // _PAGE12S1S2CONTROL_H_



