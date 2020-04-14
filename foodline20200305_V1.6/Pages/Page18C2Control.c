/*****************************************************************
 * File: Page12S1S2Control.c
 * Date: 2019/12/04 19:02
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page18C2Control.h"
#include "FoodlineControl.h"
#include "StateMachine.h"

extern u8 AreaC1Num ;

void Page18C2ControlInit(void)
{
    
    Page18C2ControlRefresh();

}

void Page18C2ControlProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{
    u32 data;
//    u16 tempSelect, tempState, tempButton, tempAlarm;
    u16 tempEffect, tempIndex,areaIndex;   //areaIndex  需要得到代表区域的地址

	if(len == 4)
		data = pbuf[0] << 24 | pbuf[1] << 16 | pbuf[2] << 8 | pbuf[3];
	else
		data = pbuf[0] << 8 | pbuf[1];

    if((addr & 0xFF00) != 0x1200)
        return;

    tempEffect = addr&0xF0;
    tempIndex = addr&0x0F;
	tempIndex = tempIndex + AreaC1Num + 1;
	if(tempEffect >= 0x50 && tempEffect <= 0x90)
	{
		tempEffect -= 0x50;
		tempIndex += 0x10;
	}
    	
    if(tempIndex < SING_LINK_DEVICE_TOTAL_NUMBER)
    {
        switch(tempEffect)
        {
            case PAGE18_SELECT:
                break;
            case PAGE18_STATE:
                break;
            case PAGE18_BUTTON:
                if(AllTheControlParaGet((DEVICE_AREA_C - 1),tempIndex)->isSelect == TRUE)
                    AllTheControlParaGet((DEVICE_AREA_C - 1),tempIndex)->isSelect = FALSE;
                else
                    AllTheControlParaGet((DEVICE_AREA_C - 1),tempIndex)->isSelect = TRUE;


                break;
            case PAGE18_ALARM:
                break;
        }
    }
    if(addr == PAGE18_START_BUTTON)
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
			DeviceControlParaGet()->controlArea[(DEVICE_AREA_C - 1)] = TRUE;
        }
    }
    if(addr == PAGE18_STOP_BUTTON)
    {
        DeviceControlParaGet()->isClickStart = FALSE;
        DeviceControlParaGet()->isClickStop = TRUE;
        DeviceControlParaGet()->isClickShutdown = FALSE;
		DeviceControlParaGet()->controlStopArea[(DEVICE_AREA_C - 1)] = TRUE;
    }
    if(addr == PAGE18_EMERGENCY_STOP_BUTTON)
    {
        DeviceControlParaGet()->isClickStart = FALSE;
        DeviceControlParaGet()->isClickStop = FALSE;
        DeviceControlParaGet()->isClickShutdown = TRUE;
		DeviceControlParaGet()->controlShutdownArea[(DEVICE_AREA_C - 1)] = TRUE;
    }     
    if(addr == PAGE18_STOP_RENEW_BUTTON)
    {

        DeviceControlParaGet()->isClickShutdown = FALSE;
		DeviceControlParaGet()->controlShutdownArea[(DEVICE_AREA_C - 1)] = FALSE;
    }       
  
    Page18C2ControlRefresh();
}

void Page18C2ControlRefresh(void)
{
    u8 i;
    for(i = AreaC1Num + 1; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
    {
		if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cDevice.place.mainLine != 1&&
			AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cDevice.place.mainLine != 0)
		{
			if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cDevice.place.type == DEVICE_NAME_FLINE
				||AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cDevice.place.type == DEVICE_NAME_VICE_FLINE)
			{
				if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte == FOODLINE_PARA_CLOSE)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState = DEVICE_STATE_CLOSE;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte == FOODLINE_PARA_OPEN)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState = DEVICE_STATE_OPEN;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte == FOODLINE_PARA_JUSTSTART)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState = DEVICE_STATE_CLOSE;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte == FOODLINE_PARA_OVERTIME)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState = DEVICE_STATE_AREA;
				
				if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->isCommAlarm )         //
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 5;          // 通讯故障
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->alarmByte & 0x01)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 1;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->alarmByte & 0x02)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 2;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->alarmByte & 0x04)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 3;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->alarmByte & 0x08)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 4;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->manualAuto != AUTO_GEARS)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 6;                 
				else 
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 0;
			}
			else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cDevice.place.type == DEVICE_NAME_IN_TOWERS 
				|| AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)
			{
				if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte == SWITCH_VALVE_PARA_FREE)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState = DEVICE_STATE_CLOSE;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte == SWITCH_VALVE_PARA_OPEN)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState = DEVICE_STATE_OPEN;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte == SWITCH_VALVE_PARA_CLOSE)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState = DEVICE_STATE_CLOSE;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte == SWITCH_VALVE_PARA_OPENING)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState = DEVICE_STATE_OPEN;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte == SWITCH_VALVE_PARA_CLOSING)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState = DEVICE_STATE_OPEN;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte == SWITCH_VALVE_PARA_OPENALARM)
				{
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState = DEVICE_STATE_AREA;
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 1;   // 阀开异常
				}
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte == SWITCH_VALVE_PARA_CLOSEALARM)
				{
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState = DEVICE_STATE_AREA;
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 2;    // 阀关异常
				}
				
				if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->isCommAlarm )         //
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 3;          // 通讯故障            
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->alarmByte & 0x01)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 1;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->alarmByte & 0x02)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 2;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->manualAuto != AUTO_GEARS)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 4;                 
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte != SWITCH_VALVE_PARA_OPENALARM&&
						AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte != SWITCH_VALVE_PARA_CLOSEALARM )
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 0;
			}
			else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
			{
				if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte == TOWERSOUT_PARA_SOTP)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState = DEVICE_STATE_CLOSE;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte == TOWERSOUT_PARA_FOREWARD)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState = DEVICE_STATE_OPEN;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte == TOWERSOUT_PARA_REVERSAL)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState = DEVICE_STATE_OPEN;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte == TOWERSOUT_PARA_FOREWARDING)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState = DEVICE_STATE_OPEN;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte == TOWERSOUT_PARA_REVERSALING)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState = DEVICE_STATE_OPEN;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte == TOWERSOUT_PARA_FOREWARD_OVERTIME)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState = DEVICE_STATE_AREA;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->stateByte == TOWERSOUT_PARA_REVERSAL_OVERTIME)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState = DEVICE_STATE_AREA;

				if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->isCommAlarm )         //
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 7;          // 通讯故障            
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->alarmByte & 0x01)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 1;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->alarmByte & 0x02)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 2;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->alarmByte & 0x04)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 3;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->alarmByte & 0x08)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 4;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->alarmByte & 0x10)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 5;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->alarmByte & 0x20)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 6;
				else if(AllTheControlParaGet((DEVICE_AREA_C - 1),i)->manualAuto != AUTO_GEARS)
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 8;            
				else 
					AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm = 0;
			}
		
			if(i <= (AreaC1Num+0x10))
			{
				DisplayCommIconSend((PAGE18_CONTROL_C2|PAGE18_SELECT) + i - AreaC1Num - 1 , AllTheControlParaGet((DEVICE_AREA_C - 1),i)->isSelect);
				DisplayCommIconSend((PAGE18_CONTROL_C2|PAGE18_STATE) + i - AreaC1Num - 1  , AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState);
				DisplayCommIconSend((PAGE18_CONTROL_C2|PAGE18_ALARM) + i - AreaC1Num - 1  , AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm);
			}
			else
			{
				DisplayCommIconSend((PAGE18_CONTROL_C2|PAGE18_SELECT) + 0x40 + i - AreaC1Num - 1 , AllTheControlParaGet((DEVICE_AREA_C - 1),i)->isSelect);
				DisplayCommIconSend((PAGE18_CONTROL_C2|PAGE18_STATE) + 0x40 + i - AreaC1Num - 1  , AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cState);
				DisplayCommIconSend((PAGE18_CONTROL_C2|PAGE18_ALARM) + 0x40 + i - AreaC1Num - 1  , AllTheControlParaGet((DEVICE_AREA_C - 1),i)->cAlarm);			
			}
			DisplayCommIconSend(PAGE18_STATE_MACHINE_STATE     , DeviceControlParaGet()->stateMachineState);
			
			
			OSTimeDly(2);
		}
    }
	DisplayCommIconSend(PAGE18_START_BUTTON, (u8)DeviceControlParaGet()->isClickStart);

}

