/*****************************************************************
 * File: Page16E1E2ontrol.c
 * Date: 2019/12/04 19:02
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page16E1E2Control.h"
#include "FoodlineControl.h"
#include "StateMachine.h"

extern OS_MsgBoxHandle gControlStopRe_Queue;
extern OS_MsgBoxHandle gControlStartRe_Queue;		

extern AllTheControlPara gAllTheControlPara[AREA_DEVICE_TOTAL_NUMBER][SING_LINK_DEVICE_TOTAL_NUMBER];
void Page16E1E2ControlInit(void)
{
    
    Page16E1E2ControlRefresh();

}

void Page16E1E2ControlProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{
    u32 data;
	INPUT_EVENT evt;	
//    u16 tempSelect, tempState, tempButton, tempAlarm;
    u16 tempEffect, tempIndex,areaIndex;   //areaIndex  需要得到代表区域的地址

	if(len == 4)
		data = pbuf[0] << 24 | pbuf[1] << 16 | pbuf[2] << 8 | pbuf[3];
	else
		data = pbuf[0] << 8 | pbuf[1];

    if((addr & 0xFF00) != 0x1000)
        return;

    tempEffect = addr&0xF0;
    tempIndex = addr&0x0F;
 
    if(tempIndex < SING_LINK_DEVICE_TOTAL_NUMBER)
    {
        switch(tempEffect)
        {
            case PAGE16_SELECT:
                break;
            case PAGE16_STATE:
                break;
            case PAGE16_BUTTON:
                if(AllTheControlParaGet((DEVICE_AREA_E - 1),tempIndex)->isSelect == TRUE)
				{
                    AllTheControlParaGet((DEVICE_AREA_E - 1),tempIndex)->isSelect = FALSE;
				}
                else
				{
                    AllTheControlParaGet((DEVICE_AREA_E - 1),tempIndex)->isSelect = TRUE;					
				}
				
					
                break;
            case PAGE16_ALARM:
                break;
        }
    }
    if(addr == PAGE16_START_BUTTON)
    {
        DeviceControlParaGet()->isClickShutdown = FALSE;

        if(DeviceControlParaGet()->stateMachineState[DEVICE_AREA_E-1] == STATE_CHANGE_SUSPEND)
        {
            DeviceControlParaGet()->stateMachineState[DEVICE_AREA_E-1] = STATE_CHANGE_SUSPENDING;   
        }
        else
        {
            DeviceControlParaGet()->isClickStart = TRUE;
            DeviceControlParaGet()->isClickStop = FALSE;
			DeviceControlParaGet()->controlArea[(DEVICE_AREA_E - 1)] = TRUE;
        }
    }
    if(addr == PAGE16_STOP_BUTTON)
    {
		DeviceControlParaGet()->isClickStart = TRUE;
		DeviceControlParaGet()->controlArea[(DEVICE_AREA_E - 1)] = TRUE;
		DeviceControlParaGet()->controlStopArea[(DEVICE_AREA_E - 1)] = FALSE;
		DeviceControlParaGet()->controlShutdownArea[(DEVICE_AREA_E - 1)] = FALSE;
		DeviceControlParaGet()->controlIndexMemory[(DEVICE_AREA_E - 1)] = 0;
		OS_MsgBoxSend(gControlStartRe_Queue, &evt, OS_NO_DELAY, FALSE);				
    }
    if(addr == PAGE16_EMERGENCY_STOP_BUTTON)
    {
        DeviceControlParaGet()->isClickStop = TRUE;
        DeviceControlParaGet()->isClickShutdown = FALSE;
		DeviceControlParaGet()->controlStopArea[(DEVICE_AREA_E - 1)] = TRUE;
		DeviceControlParaGet()->controlArea[(DEVICE_AREA_E - 1)] = FALSE;
		DeviceControlParaGet()->controlShutdownArea[(DEVICE_AREA_E - 1)] = FALSE;	
		OS_MsgBoxSend(gControlStopRe_Queue, &evt, OS_NO_DELAY, FALSE);	
		DeviceControlParaGet()->controlIndexMemory[(DEVICE_AREA_E - 1)]	= SING_LINK_DEVICE_TOTAL_NUMBER - 1;	
    }     
    if(addr == PAGE16_STOP_RENEW_BUTTON)
    {

        DeviceControlParaGet()->isClickShutdown = FALSE;
		DeviceControlParaGet()->controlShutdownArea[(DEVICE_AREA_E - 1)] = FALSE;
    }       
    if(addr == PAGE16_E1_DELAYTIME)
    {
        AllTheControlParaGet((DEVICE_AREA_E - 1),0x02)->time = data;
		AllTheControlParaGet((DEVICE_AREA_E - 1),0x02)->setStopTime = data;
		*FoodLineTimeGet(E1_FOOD_LINE_TIME) = data;
		ParaDelayParaSave();			
    }    
    if(addr == PAGE16_E2_DELAYTIME)
    {
        AllTheControlParaGet((DEVICE_AREA_E - 1),0x04)->time = data;
		AllTheControlParaGet((DEVICE_AREA_E - 1),0x04)->setStopTime = data;
		*FoodLineTimeGet(E2_FOOD_LINE_TIME) = data;
		ParaDelayParaSave();			
    }    

    Page16E1E2ControlRefresh();
}

void Page16E1E2ControlRefresh(void)
{
    u8 i;
    for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
    {
		if(AllTheControlParaGet((DEVICE_AREA_E - 1),i)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
		{
			AllTheControlParaGet((DEVICE_AREA_E - 1),i)->rotationDirection = TOWERSOUT_CONTROL_FOREWARD;   //控制绞龙正转
		}			
		if(AllTheControlParaGet((DEVICE_AREA_E - 1),i)->cDevice.place.type != 0)
		{
			ControlPareState(DEVICE_AREA_E ,i);
				 
			DisplayCommIconSend((PAGE16_CONTROL_E|PAGE16_SELECT) + i    , AllTheControlParaGet((DEVICE_AREA_E - 1),i)->isSelect);
			DisplayCommIconSend((PAGE16_CONTROL_E|PAGE16_STATE) + i     , AllTheControlParaGet((DEVICE_AREA_E - 1),i)->cState); 
			DisplayCommIconSend((PAGE16_CONTROL_E|PAGE16_ALARM) + i     , AllTheControlParaGet((DEVICE_AREA_E - 1),i)->cAlarm);
			DisplayCommIconSend(PAGE16_E1_DELAYTIME     , AllTheControlParaGet((DEVICE_AREA_E - 1),0x02)->time);
			DisplayCommIconSend(PAGE16_E2_DELAYTIME     , AllTheControlParaGet((DEVICE_AREA_E - 1),0x04)->time);
			DisplayCommIconSend(PAGE16_STATE_MACHINE_STATE     , DeviceControlParaGet()->stateMachineState[DEVICE_AREA_E-1]);
			
			
			OSTimeDly(2);
		}
	}
	DisplayCommIconSend(PAGE16_START_BUTTON, (u8)DeviceControlParaGet()->isClickStart);

}

