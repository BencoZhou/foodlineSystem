/*****************************************************************
 * File: DisplayDriver.h
 * Date: 2018/03/22 14:48
 * Copyright@2015-2025: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _DISPLAYDRIVER_H_
#define _DISPLAYDRIVER_H_

#include "System.h"

typedef void (*RcDisplayISR)(u8);
typedef void (*TxDisplayISR)(void);
void RHDisplayUsartInit(RcDisplayISR, TxDisplayISR);
void RHDisplaySendData(u8 nData);

#endif // _DISPLAYDRIVER_H_
