/*****************************************************************
 * File: Page5Revise.c
 * Date: 2019/11/27 14:49
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page5Revise.h"


void Page5ReviseInit(void)
{

}

void Page5ReviseProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{

}

void Page5ReviseRefresh(void)
{
    	tempID = PathParameterGet()->dPara[gIndex][i+gP2Table*PAGE2_DEVICE_PER].id;
    	sprintf(str, "0x%08X", tempID);
    	DisplayCommTextSend(PAGE2_PATH_ID1+i*8,	(u8*)str, sizeof(str));
}
