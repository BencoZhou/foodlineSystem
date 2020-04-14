/*****************************************************************
 * File: Page12S1S2Control.c
 * Date: 2019/12/04 19:02
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page12S1S2Control.h"

void Page12S1S2ControlInit(void)
{
    
    Page12S1S2ControlRefresh();

}

void Page12S1S2ControlProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{
    u32 data;
//    u16 tempSelect, tempState, tempButton, tempAlarm;
    u16 tempEffect, tempIndex;
	if(len == 4)
		data = pbuf[0] << 24 | pbuf[1] << 16 | pbuf[2] << 8 | pbuf[3];
	else
		data = pbuf[0] << 8 | pbuf[1];

    if((addr & 0xFF00) != 0x0C00)
        return;

    tempEffect = addr&0xF0;
    tempIndex = addr&0x0F;
    if(tempIndex < S1S2DEVICE_TOTAL_NUMBER)
    {
        switch(tempEffect)
        {
            case PAGE12_SELECT:
                if(S1S2ControlParaGet(tempIndex)->isSelect == TRUE)
                    S1S2ControlParaGet(tempIndex)->isSelect = FALSE;
                else
                    S1S2ControlParaGet(tempIndex)->isSelect = TRUE;
                break;
            case PAGE12_STATE:
                break;
            case PAGE12_BUTTON:
                break;
            case PAGE12_ALARM:
                break;
        }
    }
}

void Page12S1S2ControlRefresh(void)
{
    u8 i;
    for(i = 0; i < S1S2DEVICE_TOTAL_NUMBER; i++)
    {
	    DisplayCommIconSend(PAGE12_TOWERSOUT_S1_10+i, S1S2ControlParaGet(i)->isSelect);
    }
	OSTimeDly(3);

}

