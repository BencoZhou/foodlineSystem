/*****************************************************************
 * File: StateMachine.c
 * Date: 2019/12/03 11:21
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "StateMachine.h"
#include "FoodlineControl.h"


extern OS_MsgBoxHandle gControlStartRe_Queue;
extern OS_MsgBoxHandle gControlStopRe_Queue;

bool NormalStopFlag = FALSE;   // 正常停止标志位
bool foodDownPlaceArrivalFlag = FALSE;     // 副料线


// 定义状态参数
typedef struct
{
	STATE_CHANGE state;		// 状态名称
	void (*Entry)(void);	// 状态入口
	void (*Exit)(void);
	void (*Process)(void);
}State;


static State gStateIdle, gStateReady, gStateRunning, gStateSuspend;
static State *gState = NULL;

static void StateChange(State *state)
{
	if(state == NULL)
		return;

	// 执行状态的退出
	if(gState->Exit)
		gState->Exit();

	gState = state;

	// 执行状态的切入
	if(gState->Entry)
		gState->Entry();
}

static void IdleStateEntry(void)
{
    DeviceControlParaGet()->stateMachineState = STATE_CHANGE_IDLE;
}

static void IdleStateExit(void)
{

}

static void DeviceExistCal(void)
{
    u8 i,j;
    static u8 equipIndex = 0 ,areaIndex = 0;
    DeviceControlParaGet()->isHaveDevice = FALSE;
	
	for(j = 0; j < AREA_DEVICE_TOTAL_NUMBER; j++)    //  有待优化， 这种查询方式太耗资源。
	{
		for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
		{
		   
			if(AllTheControlParaGet(j,i)->isSelect == TRUE)
			{
				DeviceControlParaGet()->isHaveDevice = TRUE;
				break;
			}
		   
		}
	}
    SendTypeInquire(areaIndex,equipIndex);   
    if(AllTheControlParaGet(areaIndex,0)->isCommAlarm == TRUE)
	{
		equipIndex = 0;		
	}
	else
	{
		if(equipIndex > SING_LINK_DEVICE_TOTAL_NUMBER)
		{
			areaIndex = (++areaIndex >= AREA_DEVICE_TOTAL_NUMBER)?0:areaIndex;
			equipIndex = 0;		
		}
		else
		{
			equipIndex++;
		}	
	}

    OSTimeDly(100);
}
static void DeviceExistCalReady(void)
{
    u8 i, j;
    static u8 equipIndex = 0,areaIndex = 0 ;
    DeviceControlParaGet()->isHaveDevice = FALSE;
	for(j = 0; j < AREA_DEVICE_TOTAL_NUMBER; j++)    //  有待优化， 这种查询方式太耗资源。
	{
		for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
		{
		   
			if(AllTheControlParaGet(j,i)->isSelect == TRUE)
			{
				DeviceControlParaGet()->isHaveDevice = TRUE;
				break;
			}
		}
	}
    SendTypeInquire(areaIndex,equipIndex);   
  
	if(equipIndex > SING_LINK_DEVICE_TOTAL_NUMBER)
	{
		areaIndex = (++areaIndex >= AREA_DEVICE_TOTAL_NUMBER)?0:areaIndex;
		equipIndex = 0;		
	}
	else
	{
		equipIndex++;
	}	
}

static void DeviceAlarmCal(void)
{
    u8 i,j;
    DeviceControlParaGet()->isHaveAlarm = FALSE;
	for(j = 0; j < AREA_DEVICE_TOTAL_NUMBER; j++)    //  有待优化， 这种查询方式太耗资源。
	{
		if(AllTheControlParaGet(j,0)->cDevice.place.type != 0)
		{
			for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
			{
				if(AllTheControlParaGet(j,i)->cDevice.place.type != DEVICE_NAME_CONTROL
					&&AllTheControlParaGet(j,i)->cDevice.place.type != 0)   //不判断主机设备
				{
					if(AllTheControlParaGet(j,i)->alarmByte > 0)
					{
						DeviceControlParaGet()->isHaveAlarm = TRUE;
						break;
					}
					if(AllTheControlParaGet(j,i)->isSelect == TRUE)
					{
						if( AllTheControlParaGet(j,i)->stateByte == SWITCH_VALVE_PARA_OPENALARM ||
							AllTheControlParaGet(j,i)->stateByte == SWITCH_VALVE_PARA_CLOSEALARM )
						{
							DeviceControlParaGet()->isHaveAlarm = TRUE;  
							break;
						}     
					} 
					if(AllTheControlParaGet(j,i)->manualAuto != AUTO_GEARS)   //  不再自动状态则退出
					{
						//加入手动状态提示图标。
						DeviceControlParaGet()->isHaveAlarm = TRUE;
						break;
					
					}    
				}			
			}
		}
	}
}

static void DevicEemergencyStopCal(void)
{
    u8 index,areaIndex = 0 ;
    DeviceControlParaGet()->isEemergencyStop = FALSE;
	for(areaIndex = 0; areaIndex < AREA_DEVICE_TOTAL_NUMBER; areaIndex++)    //  有待优化， 这种查询方式太耗资源。
	{
		for(index = 0; index < SING_LINK_DEVICE_TOTAL_NUMBER; index++)
		{
			if(AllTheControlParaGet(areaIndex,index)->isSelect == TRUE)
			{
				if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_IN_TOWERS
					||AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)
				{
					if(AllTheControlParaGet(areaIndex,index)->stateByte == SWITCH_VALVE_PARA_CLOSE)
					{                
						DeviceControlParaGet()->isEemergencyStop = TRUE;
					}
				}
				else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_FLINE)
				{
					if(AllTheControlParaGet(areaIndex,index)->stateByte == FOODLINE_PARA_CLOSE)
					{                    
						DeviceControlParaGet()->isEemergencyStop = TRUE;
					}
				}
				else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
				{
					if(AllTheControlParaGet(areaIndex,index)->stateByte == TOWERSOUT_PARA_SOTP)
					{                    
						DeviceControlParaGet()->isEemergencyStop = TRUE;
					}
				}
			}

		}
	}
}

static void DeviceNormalStopCondition(void)
{
   
    if(NormalStopFlag == TRUE)   //缺料满料引起的正常关闭标志位
    {
        DeviceControlParaGet()->isClickStop = TRUE;
        DeviceControlParaGet()->isClickStart = FALSE;
        NormalStopFlag = FALSE;
    }
    
}
static void DeviceTriggerStartCondition(void)
{
	u8 i,j;
	u8 stopAreaNum, delayAreaNum, starAreaNum;
	for(i = 0; i < AREA_DEVICE_TOTAL_NUMBER; i++)   // 检查个区域的快关延时状态
	{
		if(DeviceControlParaGet()->controlStopArea[i] == TRUE)
		{
			stopAreaNum++;
		}
		if(DeviceControlParaGet()->controlStopDelayFlag[i] == TRUE)
		{
			delayAreaNum++;
		}
		if(DeviceControlParaGet()->controlArea[i] == TRUE)
		{
			starAreaNum++;
		}
	}
	if(stopAreaNum == delayAreaNum)
	{
		if(starAreaNum != 0)
		{
			DeviceControlParaGet()->isClickStop = FALSE;
			DeviceControlParaGet()->isClickStart = TRUE;
			NormalStopFlag = TRUE;
		}
	}
    
}
static void IdleStateProcess(void)
{
	u8 stopAreaNum, delayAreaNum, starAreaNum;
	u8 i;
    DeviceExistCal();
 	if(DeviceControlParaGet()->isHaveDevice)
 		StateChange(&gStateReady); 
	for(i = 0; i < AREA_DEVICE_TOTAL_NUMBER; i++)   // 检查个区域的快关延时状态
	{
		if(DeviceControlParaGet()->controlStopArea[i] == TRUE)
		{
			stopAreaNum++;
		}
		if(DeviceControlParaGet()->controlStopDelayFlag[i] == TRUE)
		{
			delayAreaNum++;
		}
		if(DeviceControlParaGet()->controlArea[i] == TRUE)
		{
			starAreaNum++;
		}
	}	
	if(stopAreaNum || delayAreaNum)
	{
		StateChange(&gStateReady); 
	}
	else if(starAreaNum)
	{
		StateChange(&gStateRunning);
	}
//    else if(DeviceControlParaGet()->isHaveAlarm == TRUE)
// 		StateChange(&gStateSuspend);
}

static void IdleStateInit(void)
{
    u8 i, j;
	gStateIdle.state = STATE_CHANGE_IDLE;
	gStateIdle.Entry = IdleStateEntry;
	gStateIdle.Exit  = IdleStateExit;
	gStateIdle.Process = IdleStateProcess;
	for(j = 0; j < AREA_DEVICE_TOTAL_NUMBER; j++)    //  有待优化， 这种查询方式太耗资源。
	{
		for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
		{
			AllTheControlParaGet(j,i)->isCommAlarm = TRUE;
			
		}    
	}

}


static void ReadyStateEntry(void)
{
    INPUT_EVENT evt;
    OS_MsgBoxSend(gControlStartRe_Queue, &evt, OS_NO_DELAY, FALSE);
    OS_MsgBoxSend(gControlStopRe_Queue, &evt, OS_NO_DELAY, FALSE);
}

static void ReadyStateExit(void)
{

}

static void ReadyStateProcess(void)
{
    u8 i,j,deviceCloseNum = 0;
    DeviceStopAndInquire();
    DeviceExistCalReady();
    DeviceAlarmCal();
    DeviceTriggerStartCondition();
 	if(DeviceControlParaGet()->isHaveDevice == FALSE)
 		StateChange(&gStateIdle);
    else if(DeviceControlParaGet()->isHaveAlarm == TRUE)
 		StateChange(&gStateSuspend);
    else if(DeviceControlParaGet()->isClickStart == TRUE)
 		StateChange(&gStateRunning);
	for(j = 0; j < AREA_DEVICE_TOTAL_NUMBER; j++)    //  有待优化， 这种查询方式太耗资源。
	{
		for(i = 0 ;i <SING_LINK_DEVICE_TOTAL_NUMBER; i++)
		{
			if(AllTheControlParaGet(j,i)->cState == DEVICE_STATE_CLOSE)
				deviceCloseNum++;
		}
	}
    if(deviceCloseNum == SING_LINK_DEVICE_TOTAL_NUMBER)
    {
        DeviceControlParaGet()->stateMachineState = STATE_CHANGE_READY;
    }
}

static void ReadyStateInit(void)
{
	gStateReady.state = STATE_CHANGE_READY;
	gStateReady.Entry = ReadyStateEntry;
	gStateReady.Exit  = ReadyStateExit;
	gStateReady.Process = ReadyStateProcess;
}

static void RunningStateEntry(void)
{
    INPUT_EVENT evt;
    OS_MsgBoxSend(gControlStartRe_Queue, &evt, OS_NO_DELAY, FALSE);
    OS_MsgBoxSend(gControlStopRe_Queue, &evt, OS_NO_DELAY, FALSE);
    if(AllTheControlParaGet(DEVICE_AREA_S,0x02)->time == 0)
    {
        AllTheControlParaGet(DEVICE_AREA_S,0x02)->time = *FoodLineTimeGet(S1_FOOD_LINE_TIME);
    }
    if(AllTheControlParaGet(DEVICE_AREA_S,0x04)->time == 0)
    {
        AllTheControlParaGet(DEVICE_AREA_S,0x04)->time = *FoodLineTimeGet(S2_FOOD_LINE_TIME);
    }          
    DeviceControlParaGet()->stateMachineState = STATE_CHANGE_RUNNING;
}

static void RunningStateExit(void)
{

}

static void RunningStateProcess(void)
{
    DeviceControlParaGet()->stateMachineState = STATE_CHANGE_RUNNING;
    DeviceControlAndInquire();
    DeviceAlarmCal();
    DeviceNormalStopCondition();
//    DevicEemergencyStopCal();
    if(DeviceControlParaGet()->isHaveAlarm == TRUE)
 		StateChange(&gStateSuspend);
    if(DeviceControlParaGet()->isClickStop == TRUE)
 		StateChange(&gStateReady);
    if(DeviceControlParaGet()->isClickShutdown == TRUE)
 		StateChange(&gStateSuspend);
    if(DeviceControlParaGet()->isEemergencyStop == TRUE)
 		StateChange(&gStateSuspend);
}

static void RunningStateInit(void)
{
	gStateRunning.state = STATE_CHANGE_RUNNING;
	gStateRunning.Entry = RunningStateEntry;
	gStateRunning.Exit  = RunningStateExit;
	gStateRunning.Process = RunningStateProcess;
}


static void SuspendStateEntry(void)
{
    
    INPUT_EVENT evt;
    OS_MsgBoxSend(gControlStartRe_Queue, &evt, OS_NO_DELAY, FALSE);
    OS_MsgBoxSend(gControlStopRe_Queue, &evt, OS_NO_DELAY, FALSE);
    
    DeviceControlParaGet()->isClickStart = FALSE;
    DeviceControlParaGet()->isClickStop = TRUE;
    DeviceControlParaGet()->isEemergencyStop = FALSE;
    DeviceControlParaGet()->stateMachineState = STATE_CHANGE_SUSPEND;

}

static void SuspendStateExit(void)
{
    DeviceControlParaGet()->isClickShutdown = FALSE;
}

static void SuspendStateProcess(void)
{
    //DeviceStopAndInquire();
    DeviceShutdown();
    DeviceAlarmCal();
    if(DeviceControlParaGet()->isHaveAlarm == FALSE)
 		StateChange(&gStateIdle);
 	if(DeviceControlParaGet()->isHaveDevice == FALSE)
 		StateChange(&gStateIdle);
    if(DeviceControlParaGet()->isClickShutdown == FALSE)
 		StateChange(&gStateReady);    
    DeviceControlParaGet()->stateMachineState = STATE_CHANGE_SUSPEND;
}

static void SuspendStateInit(void)
{
	gStateSuspend.state = STATE_CHANGE_SUSPEND;
	gStateSuspend.Entry = SuspendStateEntry;
	gStateSuspend.Exit  = SuspendStateExit;
	gStateSuspend.Process = SuspendStateProcess;
}


void StateMachineInit(void)
{
	IdleStateInit();
	ReadyStateInit();
	RunningStateInit();
	SuspendStateInit();
	gState = &gStateIdle;
}

void StateMachineProcess(void)
{

	if(gState != NULL)
	{
		if(gState->Process)
			gState->Process(); 
	}
 
}

