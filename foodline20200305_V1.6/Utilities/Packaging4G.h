/*****************************************************************
 * File: Packaging4G.h
 * Date: 2019/06/20 14:46
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _PACKAGING4G_H_
#define _PACKAGING4G_H_

#include "System.h"

void Wlan4G_SendDataPackaging(u8 cmd, u8 type, u8 *msg, u8 len, u32 srcaddr, u32 dstaddr, u16 ackseq);
void App_SendData(u8 cmd, u8 type, u8 *msg, u8 len, u32 srcaddr, u32 dstaddr, u16 ackseq);

#endif // _PACKAGING4G_H_

