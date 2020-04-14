/*****************************************************************
 * File: Page12S1S2Control.c
 * Date: 2019/12/04 19:02
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page19N1Control.h"
#include "FoodlineControl.h"
#include "StateMachine.h"

u8 AreaN1Num ;

void Page19N1ControlInit(void)
{
    
    Page19N1ControlRefresh();

}

void Page19N1ControlProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{
    u32 data;
//    u16 tempSelect, tempState, tempButton, tempAlarm;
    u16 tempEffect, tempIndex,areaIndex;   //areaIndex  需要得到代表区域的地址

	if(len == 4)
		data = pbuf[0] << 24 | pbuf[1] << 16 | pbuf[2] << 8 | pbuf[3];
	else
		data = pbuf[0] << 8 | pbuf[1];

    if((addr & 0xFF00) != 0x1300)
        return;

    tempEffect = addr&0xF0;
    tempIndex = addr&0x0F;
	tempIndex += 1;
    if(tempIndex < SING_LINK_DEVICE_TOTAL_NUMBER)
    {
        switch(tempEffect)
        {
            case PAGE19_SELECT:
                break;
            case PAGE19_STATE:
                break;
            case PAGE19_BUTTON:
                if(AllTheControlParaGet((DEVICE_AREA_N - 1),tempIndex)->isSelect == TRUE)
                    AllTheControlParaGet((DEVICE_AREA_N - 1),tempIndex)->isSelect = FALSE;
                else
                    AllTheControlParaGet((DEVICE_AREA_N - 1),tempIndex)->isSelect = TRUE;


                break;
            case PAGE19_ALARM:
                break;
        }
    }
    if(addr == PAGE19_START_BUTTON)
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
    if(addr == PAGE19_STOP_BUTTON)
    {
        DeviceControlParaGet()->isClickStart = FALSE;
        DeviceControlParaGet()->isClickStop = TRUE;
        DeviceControlParaGet()->isClickShutdown = FALSE;
		DeviceControlParaGet()->controlStopArea[(DEVICE_AREA_N - 1)] = TRUE;
    }
    if(addr == PAGE19_EMERGENCY_STOP_BUTTON)
    {
        DeviceControlParaGet()->isClickStart = FALSE;
        DeviceControlParaGet()->isClickStop = FALSE;
        DeviceControlParaGet()->isClickShutdown = TRUE;
		DeviceControlParaGet()->controlShutdownArea[(DEVICE_AREA_N - 1)] = TRUE;
    }     
    if(addr == PAGE19_STOP_RENEW_BUTTON)
    {

        DeviceControlParaGet()->isClickShutdown = FALSE;
		DeviceControlParaGet()->controlShutdownArea[(DEVICE_AREA_N - 1)] = FALSE;
    }       
    if(addr == PAGE19_N1_DELAYTIME)
    {
        AllTheControlParaGet((DEVICE_AREA_N - 1), 0x04)->time = data;
		*FoodLineTimeGet(N1_FOOD_LINE_TIME) = data;
		ParaDelayParaSave();
    }    
    	
   

    Page19N1ControlRefresh();
}

void Page19N1ControlRefresh(void)
{
    u8 i, N1Num = 0;
    for(i = 1; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
    {
		if(AllTheControlParaGet((DEVICE_AREA_N - 1),i)->cDevice.place.mainLine == 1&&
			AllTheControlParaGet((DEVICE_AREA_N - 1),i)->cDevice.place.mainLine != 0)
		{
			ControlPareState(DEVICE_AREA_N ,i);
			N1Num++;
			DisplayCommIconSend((PAGE19_CONTROL_N1|PAGE19_SELECT) + i - 1     , AllTheControlParaGet((DEVICE_AREA_N - 1),i)->isSelect);
			DisplayCommIconSend((PAGE19_CONTROL_N1|PAGE19_STATE) + i - 1       , AllTheControlParaGet((DEVICE_AREA_N - 1),i)->cState);
			DisplayCommIconSend((PAGE19_CONTROL_N1|PAGE19_ALARM) + i - 1     , AllTheControlParaGet((DEVICE_AREA_N - 1),i)->cAlarm);
			OSTimeDly(2);			
		}
		
		AreaN1Num = N1Num;
        

        DisplayCommIconSend(PAGE19_N1_DELAYTIME     , AllTheControlParaGet((DEVICE_AREA_N - 1),0x05)->time);
        DisplayCommIconSend(PAGE19_STATE_MACHINE_STATE     , DeviceControlParaGet()->stateMachineState);
        OSTimeDly(2);
	   
    }
	DisplayCommIconSend(PAGE19_START_BUTTON, (u8)DeviceControlParaGet()->isClickStart);

}

