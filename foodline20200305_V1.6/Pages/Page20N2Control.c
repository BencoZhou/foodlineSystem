/*****************************************************************
 * File: Page12S1S2Control.c
 * Date: 2019/12/04 19:02
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page20N2Control.h"
#include "FoodlineControl.h"
#include "StateMachine.h"

extern u8 AreaN1Num ;

void Page20N2ControlInit(void)
{
    
    Page20N2ControlRefresh();

}

void Page20N2ControlProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{
    u32 data;
//    u16 tempSelect, tempState, tempButton, tempAlarm;
    u16 tempEffect, tempIndex,areaIndex;   //areaIndex  需要得到代表区域的地址

	if(len == 4)
		data = pbuf[0] << 24 | pbuf[1] << 16 | pbuf[2] << 8 | pbuf[3];
	else
		data = pbuf[0] << 8 | pbuf[1];

    if((addr & 0xFF00) != 0x1400)
        return;

    tempEffect = addr&0xF0;
    tempIndex = addr&0x0F;
	tempIndex = tempIndex + AreaN1Num + 1;
    if(tempIndex < SING_LINK_DEVICE_TOTAL_NUMBER)
    {
        switch(tempEffect)
        {
            case PAGE20_SELECT:
                break;
            case PAGE20_STATE:
                break;
            case PAGE20_BUTTON:
                if(AllTheControlParaGet((DEVICE_AREA_N - 1),tempIndex)->isSelect == TRUE)
                    AllTheControlParaGet((DEVICE_AREA_N - 1),tempIndex)->isSelect = FALSE;
                else
                    AllTheControlParaGet((DEVICE_AREA_N - 1),tempIndex)->isSelect = TRUE;


                break;
            case PAGE20_ALARM:
                break;
        }
    }
    if(addr == PAGE20_START_BUTTON)
    {
        DeviceControlParaGet()->isClickShutdown = FALSE;

        if(DeviceControlParaGet()->stateMachineState == STATE_CHANGE_SUSPEND)
        {
            DeviceControlParaGet()->stateMachineState = STATE_CHANGE_SUSPENDING;   
        }
        else
        {
            DeviceControlParaGet()->isClickStart = TRUE;
            DeviceControlParaGet()->isClickStop = FALSE;
			DeviceControlParaGet()->controlArea[(DEVICE_AREA_N - 1)] = TRUE;
        }
    }
    if(addr == PAGE20_STOP_BUTTON)
    {
        DeviceControlParaGet()->isClickStart = FALSE;
        DeviceControlParaGet()->isClickStop = TRUE;
        DeviceControlParaGet()->isClickShutdown = FALSE;
		DeviceControlParaGet()->controlStopArea[(DEVICE_AREA_N - 1)] = TRUE;
    }
    if(addr == PAGE20_EMERGENCY_STOP_BUTTON)
    {
        DeviceControlParaGet()->isClickStart = FALSE;
        DeviceControlParaGet()->isClickStop = FALSE;
        DeviceControlParaGet()->isClickShutdown = TRUE;
		DeviceControlParaGet()->controlShutdownArea[(DEVICE_AREA_N - 1)] = TRUE;
    }     
    if(addr == PAGE20_STOP_RENEW_BUTTON)
    {

        DeviceControlParaGet()->isClickShutdown = FALSE;
		DeviceControlParaGet()->controlShutdownArea[(DEVICE_AREA_N - 1)] = FALSE;
    }       
    if(addr == PAGE20_N2_DELAYTIME)
    {
        AllTheControlParaGet((DEVICE_AREA_N - 1),(AreaN1Num + 0x01))->time = data;
		*FoodLineTimeGet(N2_FOOD_LINE_TIME) = data;
		ParaDelayParaSave();
    }    
   

    Page20N2ControlRefresh();
}

void Page20N2ControlRefresh(void)
{
    u8 i;
    for(i = AreaN1Num + 1; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
    {
		if(AllTheControlParaGet((DEVICE_AREA_N - 1),i)->cDevice.place.mainLine == 2&&
			AllTheControlParaGet((DEVICE_AREA_N - 1),i)->cDevice.place.mainLine != 0)
		{
			ControlPareState(DEVICE_AREA_N ,i);
			DisplayCommIconSend((PAGE20_CONTROL_N2|PAGE20_SELECT) + i - AreaN1Num -1   , AllTheControlParaGet((DEVICE_AREA_N - 1),i)->isSelect);
			DisplayCommIconSend((PAGE20_CONTROL_N2|PAGE20_STATE) + i- AreaN1Num - 1     , AllTheControlParaGet((DEVICE_AREA_N - 1),i)->cState);
			DisplayCommIconSend((PAGE20_CONTROL_N2|PAGE20_ALARM) + i- AreaN1Num - 1     , AllTheControlParaGet((DEVICE_AREA_N - 1),i)->cAlarm);	
			OSTimeDly(2);			
		}
        

        DisplayCommIconSend(PAGE20_N2_DELAYTIME     , AllTheControlParaGet((DEVICE_AREA_N - 1),0x01)->time);
        DisplayCommIconSend(PAGE20_STATE_MACHINE_STATE     , DeviceControlParaGet()->stateMachineState);
        
        
	    OSTimeDly(2);
    }
	DisplayCommIconSend(PAGE20_START_BUTTON, (u8)DeviceControlParaGet()->isClickStart);

}

