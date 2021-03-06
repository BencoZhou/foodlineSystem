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

extern u8 AreaN1Num ;

extern OS_MsgBoxHandle gControlStartRe_Queue;
extern OS_MsgBoxHandle gControlStopRe_Queue;
extern u8 AreaW1Num ;

bool NormalStopFlag = FALSE;   // 正常停止标志位
bool foodDownPlaceArrivalFlag = FALSE;     // 副料线

static void InitDelayHalt(void);


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

static void StateShow(void);

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
	u8 i;
    DeviceControlParaGet()->stateMachineState[DEVICE_AREA_S - 1] = STATE_CHANGE_IDLE;
	for(i = 0; i < AREA_DEVICE_TOTAL_NUMBER; i++)
	{
		if( DeviceControlParaGet()->controlShutdownArea[i] == FALSE
			&& DeviceControlParaGet()->controlArea[i] == FALSE
			&& DeviceControlParaGet()->controlStopArea[i] == FALSE)
		{
			DeviceControlParaGet()->stateMachineState[i] = STATE_CHANGE_IDLE;
		}
	}

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
		if(AllTheControlParaGet(j,0)->cDevice.placeNew.useID != 0 )
		{
			for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
			{		   
				if(AllTheControlParaGet(j,i)->cDevice.placeNew.useID != 0 )
				{
					if(AllTheControlParaGet(j,i)->isSelect == TRUE)
					{
						DeviceControlParaGet()->isHaveDevice = TRUE;
						break;
					}
				}
				else
					break;		   
			}
		}
	}
	if(AllTheControlParaGet(areaIndex,0)->cDevice.placeNew.useID != 0 )    //刷选出有设备的区域
	{		
		if(AllTheControlParaGet(areaIndex,equipIndex)->cDevice.placeNew.useID != 0 )  //选出不为空的数据 
		{
			if(AllTheControlParaGet(areaIndex,equipIndex)->cDevice.place.type != DEVICE_NAME_CONTROL
				&& AllTheControlParaGet(areaIndex,equipIndex)->cDevice.place.type != 0)  //类型不是主机也不为空的才查询
			SendTypeInquire(areaIndex,equipIndex); 
			equipIndex = (++equipIndex >= SING_LINK_DEVICE_TOTAL_NUMBER)?0:equipIndex;			
		}
		else
		{
			areaIndex = (++areaIndex >= AREA_DEVICE_TOTAL_NUMBER)?0:areaIndex;	
			equipIndex = 0;
		}
	}
	else
	{
		areaIndex = (++areaIndex >= AREA_DEVICE_TOTAL_NUMBER)?0:areaIndex;	
		equipIndex = 0;
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
		if(AllTheControlParaGet(j,0)->cDevice.placeNew.useID != 0)
		{
			for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
			{
				if(AllTheControlParaGet(j,i)->cDevice.placeNew.useID != 0)
				{
					if(AllTheControlParaGet(j,i)->isSelect == TRUE)
					{
						DeviceControlParaGet()->isHaveDevice = TRUE;
						break;
					}
				}
				else
					break;
			}
		}
	}
//    SendTypeInquire(areaIndex,equipIndex); 
//	
//  
//	if(equipIndex > SING_LINK_DEVICE_TOTAL_NUMBER)
//	{
//		areaIndex = (++areaIndex >= AREA_DEVICE_TOTAL_NUMBER)?0:areaIndex;
//		equipIndex = 0;		
//	}
//	else
//	{
//		equipIndex++;
//	}	
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
						DeviceControlParaGet()->controlShutdownArea[j] = TRUE;
						break;
					}
					if(AllTheControlParaGet(j,i)->isSelect == TRUE)
					{
						if( AllTheControlParaGet(j,i)->stateByte == SWITCH_VALVE_PARA_OPENALARM ||
							AllTheControlParaGet(j,i)->stateByte == SWITCH_VALVE_PARA_CLOSEALARM )
						{
							DeviceControlParaGet()->isHaveAlarm = TRUE;  
							DeviceControlParaGet()->controlShutdownArea[j] = TRUE;
							break;
						}     
					} 
					if(AllTheControlParaGet(j,i)->manualAuto != AUTO_GEARS)   //  不再自动状态则退出
					{
						//加入手动状态提示图标。
//						DeviceControlParaGet()->isClickStop  = TRUE;
//						DeviceControlParaGet()->controlStopArea[j] = TRUE;
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
				else if(AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_FLINE
					|| AllTheControlParaGet(areaIndex,index)->cDevice.place.type == DEVICE_NAME_VICE_FLINE)
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
//        DeviceControlParaGet()->isClickStart = FALSE;
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
//			DeviceControlParaGet()->isClickStop = FALSE;
			DeviceControlParaGet()->isClickStart = TRUE;
			NormalStopFlag = TRUE;
		}
	}
    
}
static void IdleStateProcess(void)
{
	u8 stopAreaNum = 0, delayAreaNum = 0, starAreaNum = 0;
	u8 i;
	StateShow();
    DeviceExistCal();
 	//if(DeviceControlParaGet()->isHaveDevice)
		
	if(DeviceControlParaGet()->isClickStop == TRUE)
 		StateChange(&gStateReady); 
	for(i = 0; i < AREA_DEVICE_TOTAL_NUMBER; i++)   // 检查个区域的开关延时状态
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
		DeviceControlParaGet()->isClickStop = TRUE;
//		DeviceControlParaGet()->isClickStart = FALSE;
		StateChange(&gStateReady); 
	}
	else if(starAreaNum)
	{
		DeviceControlParaGet()->isClickStart = TRUE;
//		DeviceControlParaGet()->isClickStop = FALSE;
		StateChange(&gStateRunning);
	}
    else if(DeviceControlParaGet()->isHaveAlarm == TRUE)
	{
 		StateChange(&gStateSuspend);
	}
	
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
		if(AllTheControlParaGet(j,0)->cDevice.placeNew.useID != 0 )
		for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
		{
			if(AllTheControlParaGet(j,i)->cDevice.placeNew.useID != 0 )
				AllTheControlParaGet(j,i)->isCommAlarm = TRUE;
			else
				break;
			
		}    
	}

}


static void ReadyStateEntry(void)
{
    INPUT_EVENT evt;
//    OS_MsgBoxSend(gControlStartRe_Queue, &evt, OS_NO_DELAY, FALSE);
    OS_MsgBoxSend(gControlStopRe_Queue, &evt, OS_NO_DELAY, FALSE);
}

static void ReadyStateExit(void)
{

}
static void StateShow(void)
{
	u8 i,j,deviceCloseNum = 0,deviceNum = 0,deviceStartNum = 0,selectNum = 0;
	for(j = 0; j < AREA_DEVICE_TOTAL_NUMBER; j++)    //  判断状态是否需要显示为准备提示
	{
		if(AllTheControlParaGet(j,0)->cDevice.placeNew.useID != 0 )
		{
			deviceNum = 0;
			deviceCloseNum = 0;
			deviceStartNum = 0;
			selectNum = 0;
			for(i = 0 ;i <SING_LINK_DEVICE_TOTAL_NUMBER; i++)
			{
				if(AllTheControlParaGet(j,i)->cDevice.place.type != DEVICE_NAME_CONTROL)  //非主机设备才进行判断
				{
					if(AllTheControlParaGet(j,i)->cDevice.placeNew.useID != 0) 
//						&& AllTheControlParaGet(j,i)->cDevice.place.type != 0)
					{
						if(AllTheControlParaGet(j,i)->cDevice.place.type != 0)
						{
							if(AllTheControlParaGet(j,i)->isSelect == TRUE)					
								selectNum++;
							
							if(AllTheControlParaGet(j,i)->cState == DEVICE_STATE_OPEN)													
								deviceStartNum++;
							else if(AllTheControlParaGet(j,i)->cState == DEVICE_STATE_CLOSE)
								deviceCloseNum++;
								
							deviceNum++;
						}
					}
					else 
					{
						if(selectNum == 0)
						{
							DeviceControlParaGet()->stateMachineState[j] = STATE_CHANGE_IDLE;      // 没有选择设备，则为待机状态																							
							DeviceControlParaGet()->controlArea[j] = FALSE;
							DeviceControlParaGet()->controlStopArea[j] = FALSE;
						}			
						else if(selectNum != 0 &&  (deviceCloseNum == deviceNum)) //有选择但都是关闭状态则是准备状态
						{
							DeviceControlParaGet()->stateMachineState[j] = STATE_CHANGE_READY;
						}					
						else if((selectNum != 0 && deviceStartNum != 0) || (DeviceControlParaGet()->controlArea[i] == TRUE))//有选择但有开启的状态则是运行状态
						{
//							if(DeviceControlParaGet()->controlArea[i] == TRUE)
								DeviceControlParaGet()->stateMachineState[j] = STATE_CHANGE_RUNNING;
//							else
//								DeviceControlParaGet()->stateMachineState[j] = STATE_CHANGE_READY;
						}
						break;
					}
				}
			}
		}
	}	
}
static void ReadyStateProcess(void)
{
    DeviceStopAndInquire();
    DeviceExistCalReady();
    DeviceAlarmCal();
    DeviceTriggerStartCondition();
	StateShow();
	
 	if(DeviceControlParaGet()->isHaveDevice == FALSE)
 		StateChange(&gStateIdle);
    else if(DeviceControlParaGet()->isHaveAlarm == TRUE)
 		StateChange(&gStateSuspend);
    else if(DeviceControlParaGet()->isClickStart == TRUE)
 		StateChange(&gStateRunning);
//	else if(DeviceControlParaGet()->isClickStop == TRUE)
// 		StateChange(&gStateReady);

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
//    OS_MsgBoxSend(gControlStopRe_Queue, &evt, OS_NO_DELAY, FALSE);
	InitDelayHalt();
}

static void RunningStateExit(void)
{

}

static void RunningStateProcess(void)
{
	StateShow();
    DeviceControlAndInquire();
    DeviceAlarmCal();
    DeviceNormalStopCondition();
//    DevicEemergencyStopCal();
    if(DeviceControlParaGet()->isHaveAlarm == TRUE)
 		StateChange(&gStateSuspend);
    else if(DeviceControlParaGet()->isClickStop == TRUE)
 		StateChange(&gStateReady);
    else if(DeviceControlParaGet()->isClickShutdown == TRUE)
 		StateChange(&gStateSuspend);
//    else if(DeviceControlParaGet()->isEemergencyStop == TRUE)
// 		StateChange(&gStateReady);
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
//    OS_MsgBoxSend(gControlStartRe_Queue, &evt, OS_NO_DELAY, FALSE);
//    OS_MsgBoxSend(gControlStopRe_Queue, &evt, OS_NO_DELAY, FALSE);
    
    DeviceControlParaGet()->isClickStart = FALSE;
    DeviceControlParaGet()->isClickStop = TRUE;
    DeviceControlParaGet()->isEemergencyStop = FALSE;
//    DeviceControlParaGet()->stateMachineState[DEVICE_AREA_S-1] = STATE_CHANGE_SUSPEND;

}

static void SuspendStateExit(void)
{
    DeviceControlParaGet()->isClickShutdown = FALSE;
}

static void SuspendStateProcess(void)
{
		u8 i,j,deviceSuspendNum = 0;
    //DeviceStopAndInquire();
    DeviceShutdown();
    DeviceAlarmCal();
    if(DeviceControlParaGet()->isHaveAlarm == FALSE)
 		StateChange(&gStateIdle);
 	if(DeviceControlParaGet()->isHaveDevice == FALSE)
 		StateChange(&gStateIdle);
    if(DeviceControlParaGet()->isClickShutdown == FALSE)
 		StateChange(&gStateReady);    
	
	for(j = 0; j < AREA_DEVICE_TOTAL_NUMBER; j++)    //  判断状态是否需要显示为准备提示
	{
		if(AllTheControlParaGet(j,0)->cDevice.placeNew.useID != 0 )
		{
			
			if(DeviceControlParaGet()->controlShutdownArea[j] == TRUE)
			{
				DeviceControlParaGet()->stateMachineState[j] = STATE_CHANGE_SUSPEND;
			}

		}
	}	
	
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
static void InitDelayHalt(void)
{
    if(AllTheControlParaGet(DEVICE_AREA_S-1,0x03)->time == 0)
    {
        AllTheControlParaGet(DEVICE_AREA_S-1,0x03)->time = *FoodLineTimeGet(S1_FOOD_LINE_TIME);
		AllTheControlParaGet((DEVICE_AREA_S - 1),0x03)->setStopTime = *FoodLineTimeGet(S1_FOOD_LINE_TIME);
    }
    if(AllTheControlParaGet(DEVICE_AREA_S-1,0x05)->time == 0)
    {
        AllTheControlParaGet(DEVICE_AREA_S-1,0x05)->time = *FoodLineTimeGet(S2_FOOD_LINE_TIME);
		AllTheControlParaGet((DEVICE_AREA_S - 1),0x05)->setStopTime = *FoodLineTimeGet(S2_FOOD_LINE_TIME);
    }  
	
    if(AllTheControlParaGet(DEVICE_AREA_C-1,0x04)->time == 0)
    {
        AllTheControlParaGet((DEVICE_AREA_C - 1),0x04)->time = *FoodLineTimeGet(C1_FOOD_LINE_TIME);
		AllTheControlParaGet((DEVICE_AREA_C - 1),0x04)->setStopTime = *FoodLineTimeGet(C1_FOOD_LINE_TIME);
    }   		
    if(AllTheControlParaGet(DEVICE_AREA_C-1,0x0c)->time == 0)
    {
        AllTheControlParaGet((DEVICE_AREA_C - 1),0x0c)->time = *FoodLineTimeGet(C2_FOOD_LINE_TIME);
		AllTheControlParaGet((DEVICE_AREA_C - 1),0x0c)->setStopTime = *FoodLineTimeGet(C2_FOOD_LINE_TIME);
    }   	
	
    if(AllTheControlParaGet(DEVICE_AREA_W-1,0x02)->time == 0)
    {
        AllTheControlParaGet((DEVICE_AREA_W - 1),0x02)->time = *FoodLineTimeGet(W1_FOOD_LINE_TIME);
		AllTheControlParaGet((DEVICE_AREA_W - 1),0x02)->setStopTime = *FoodLineTimeGet(W1_FOOD_LINE_TIME);
    }   
	if(AllTheControlParaGet(DEVICE_AREA_W-1,(AreaW1Num + 0x02))->time == 0)
    {
        AllTheControlParaGet((DEVICE_AREA_W - 1),(AreaW1Num + 0x02))->time = *FoodLineTimeGet(W2_FOOD_LINE_TIME);
		AllTheControlParaGet((DEVICE_AREA_W - 1),(AreaW1Num + 0x02))->setStopTime = *FoodLineTimeGet(W2_FOOD_LINE_TIME);
    }
	
    if(AllTheControlParaGet(DEVICE_AREA_E-1,0x02)->time == 0)
    {
        AllTheControlParaGet((DEVICE_AREA_E - 1),0x02)->time = *FoodLineTimeGet(E1_FOOD_LINE_TIME);
		AllTheControlParaGet((DEVICE_AREA_E - 1),0x02)->setStopTime = *FoodLineTimeGet(E1_FOOD_LINE_TIME);
    }   
	if(AllTheControlParaGet(DEVICE_AREA_E-1,0x04)->time == 0)
    {
        AllTheControlParaGet((DEVICE_AREA_E - 1),0x04)->time = *FoodLineTimeGet(E2_FOOD_LINE_TIME);
		AllTheControlParaGet((DEVICE_AREA_E - 1),0x04)->setStopTime = *FoodLineTimeGet(E2_FOOD_LINE_TIME);
    }	

    if(AllTheControlParaGet(DEVICE_AREA_N-1,0x05)->time == 0)
    {
        AllTheControlParaGet((DEVICE_AREA_N - 1),0x05)->time = *FoodLineTimeGet(N1_FOOD_LINE_TIME);
		AllTheControlParaGet((DEVICE_AREA_N - 1),0x05)->setStopTime = *FoodLineTimeGet(N1_FOOD_LINE_TIME);
    }   
	if(AllTheControlParaGet(DEVICE_AREA_N-1,(AreaN1Num + 0x01))->time == 0)
    {
        AllTheControlParaGet((DEVICE_AREA_N - 1),(AreaN1Num + 0x01))->time = *FoodLineTimeGet(N2_FOOD_LINE_TIME);
		AllTheControlParaGet((DEVICE_AREA_N - 1),(AreaN1Num + 0x01))->setStopTime = *FoodLineTimeGet(N2_FOOD_LINE_TIME);
    }	
	
}
