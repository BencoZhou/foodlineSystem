/*****************************************************************
 * File: Page12S1S2Control.c
 * Date: 2019/12/04 19:02
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page14W1Control.h"
#include "FoodlineControl.h"
#include "StateMachine.h"

u8 AreaW1Num = 0;

extern OS_MsgBoxHandle gControlStopRe_Queue;
extern OS_MsgBoxHandle gControlStartRe_Queue;


void Page14W1ControlInit(void)
{
    
    Page14W1ControlRefresh();

}

void Page14W1ControlProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{
    u32 data;
	INPUT_EVENT evt;
//    u16 tempSelect, tempState, tempButton, tempAlarm;
    u16 tempEffect, tempIndex,areaIndex;   //areaIndex  需要得到代表区域的地址

	if(len == 4)
		data = pbuf[0] << 24 | pbuf[1] << 16 | pbuf[2] << 8 | pbuf[3];
	else
		data = pbuf[0] << 8 | pbuf[1];

    if((addr & 0xFF00) != 0x0E00)
        return;

    tempEffect = addr&0xF0;
	tempIndex = addr&0x0F;
	tempIndex += 1;
	if(tempEffect >= 0x50 && tempEffect <= 0x90)
	{
		tempEffect -= 0x50;
		tempIndex += 0x10;
	}
    
    if(tempIndex < SING_LINK_DEVICE_TOTAL_NUMBER)
    {
        switch(tempEffect)
        {
            case PAGE14_SELECT:
                break;
            case PAGE14_STATE:
                break;
            case PAGE14_BUTTON:
                if(AllTheControlParaGet((DEVICE_AREA_W - 1),tempIndex)->isSelect == TRUE)
                    AllTheControlParaGet((DEVICE_AREA_W - 1),tempIndex)->isSelect = FALSE;
                else
				{
                    AllTheControlParaGet((DEVICE_AREA_W - 1),tempIndex)->isSelect = TRUE;					
				}

                break;
            case PAGE14_ALARM:
                break;
        }
    }
    if(addr == PAGE14_START_BUTTON)
    {
        DeviceControlParaGet()->isClickShutdown = FALSE;

        if(DeviceControlParaGet()->stateMachineState[DEVICE_AREA_W-1] == STATE_CHANGE_SUSPEND)
        {
            DeviceControlParaGet()->stateMachineState[DEVICE_AREA_W-1] = STATE_CHANGE_SUSPENDING;   
        }
        
        {

            DeviceControlParaGet()->isClickStart = TRUE;
			DeviceControlParaGet()->controlArea[(DEVICE_AREA_W - 1)] = TRUE;
			DeviceControlParaGet()->controlStopArea[(DEVICE_AREA_W - 1)] = FALSE;
			DeviceControlParaGet()->controlShutdownArea[(DEVICE_AREA_W - 1)] = FALSE;
			DeviceControlParaGet()->controlIndexMemory[(DEVICE_AREA_W - 1)] = 0;
			OS_MsgBoxSend(gControlStartRe_Queue, &evt, OS_NO_DELAY, FALSE);			
        }
    }
    if(addr == PAGE14_STOP_BUTTON)
    {		
        DeviceControlParaGet()->isClickStop = TRUE;
        DeviceControlParaGet()->isClickShutdown = FALSE;
		DeviceControlParaGet()->controlStopArea[(DEVICE_AREA_W - 1)] = TRUE;
		DeviceControlParaGet()->controlArea[(DEVICE_AREA_W - 1)] = FALSE;
		DeviceControlParaGet()->controlShutdownArea[(DEVICE_AREA_W - 1)] = FALSE;	
		OS_MsgBoxSend(gControlStopRe_Queue, &evt, OS_NO_DELAY, FALSE);	
		DeviceControlParaGet()->controlIndexMemory[(DEVICE_AREA_W - 1)]	= SING_LINK_DEVICE_TOTAL_NUMBER - 1;		
    }
    if(addr == PAGE14_EMERGENCY_STOP_BUTTON)
    {
        DeviceControlParaGet()->isClickStart = FALSE;
        DeviceControlParaGet()->isClickStop = FALSE;
        DeviceControlParaGet()->isClickShutdown = TRUE;
		DeviceControlParaGet()->controlShutdownArea[(DEVICE_AREA_W - 1)] = TRUE;
    }     
    if(addr == PAGE14_STOP_RENEW_BUTTON)
    {

        DeviceControlParaGet()->isClickShutdown = FALSE;
		DeviceControlParaGet()->controlShutdownArea[(DEVICE_AREA_W - 1)] = FALSE;
    }       
    if(addr == PAGE14_W1_DELAYTIME)
    {
        AllTheControlParaGet((DEVICE_AREA_W - 1),0x02)->time = data;
		AllTheControlParaGet((DEVICE_AREA_W - 1),0x02)->setStopTime = data;
		*FoodLineTimeGet(W1_FOOD_LINE_TIME) = data;
		ParaDelayParaSave();		
    }    
   

    Page14W1ControlRefresh();
}

void Page14W1ControlRefresh(void)
{
    u8 i,W1Num = 0;
    for(i = 1; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
    {
		if(AllTheControlParaGet((DEVICE_AREA_W - 1),i)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
		{
			AllTheControlParaGet((DEVICE_AREA_W - 1),i)->rotationDirection = TOWERSOUT_CONTROL_REVERSAL;   //控制绞龙反转
		}			
		if(AllTheControlParaGet((DEVICE_AREA_W - 1),i)->cDevice.place.mainLine != 2 &&
			AllTheControlParaGet((DEVICE_AREA_W - 1),i)->cDevice.place.mainLine != 0)
		{
			ControlPareState(DEVICE_AREA_W ,i);
			W1Num++;

			if(i <= 0x10)
			{
				DisplayCommIconSend((PAGE14_CONTROL_W1|PAGE14_SELECT) + (i-1)    , AllTheControlParaGet((DEVICE_AREA_W - 1),(i))->isSelect);
				DisplayCommIconSend((PAGE14_CONTROL_W1|PAGE14_STATE) + (i-1)    , AllTheControlParaGet((DEVICE_AREA_W - 1),(i))->cState);
				DisplayCommIconSend((PAGE14_CONTROL_W1|PAGE14_ALARM) + (i-1)     , AllTheControlParaGet((DEVICE_AREA_W - 1),(i))->cAlarm);
			}
			else    //控件数量大于0x0f时需要将地址进位0x40  = 0x50() - 0x10()
			{
				DisplayCommIconSend((PAGE14_CONTROL_W1|PAGE14_SELECT) + 0x40 + (i-1)     , AllTheControlParaGet((DEVICE_AREA_W - 1),(i))->isSelect);
				DisplayCommIconSend((PAGE14_CONTROL_W1|PAGE14_STATE) + 0x40 + (i-1)     , AllTheControlParaGet((DEVICE_AREA_W - 1),(i))->cState);
				DisplayCommIconSend((PAGE14_CONTROL_W1|PAGE14_ALARM) + 0x40 + (i-1)     , AllTheControlParaGet((DEVICE_AREA_W - 1),(i))->cAlarm);
			
			}
			DisplayCommIconSend(PAGE14_W1_DELAYTIME     , AllTheControlParaGet((DEVICE_AREA_W - 1),0x02)->time);
			DisplayCommIconSend(PAGE14_STATE_MACHINE_STATE     , DeviceControlParaGet()->stateMachineState[DEVICE_AREA_W-1]);
			OSTimeDly(2);
		}
		AreaW1Num = W1Num;
        
	    OSTimeDly(2);
    }
	DisplayCommIconSend(PAGE14_START_BUTTON, (u8)DeviceControlParaGet()->isClickStart);

}

