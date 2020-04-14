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

}

static void IdleStateExit(void)
{

}

static void DeviceExistCal(void)
{
    u8 i;
    DeviceControlParaGet()->isHaveDevice = FALSE;
    for(i = 0; i < S1S2DEVICE_TOTAL_NUMBER; i++)
    {
        if(S1S2ControlParaGet(i)->isSelect == TRUE)
        {
            DeviceControlParaGet()->isHaveDevice = TRUE;
            break;
        }
    }
}

static void DeviceAlarmCal(void)
{
    u8 i;
    DeviceControlParaGet()->isHaveAlarm = FALSE;
    for(i = 0; i < S1S2DEVICE_TOTAL_NUMBER; i++)
    {
        if(S1S2ControlParaGet(i)->isCommAlarm == TRUE || S1S2ControlParaGet(i)->alarmByte > 0)
        {
            DeviceControlParaGet()->isHaveAlarm = TRUE;
            break;
        }
    }
}

static void DevicEemergencyStopCal(void)
{
    u8 index;
    DeviceControlParaGet()->isEemergencyStop = FALSE;
    for(index = 0; index < S1S2DEVICE_TOTAL_NUMBER; index++)
    {
        if(S1S2ControlParaGet(index)->isSelect == TRUE)
        {
            if(S1S2ControlParaGet(index)->cDevice.place.type == DEVICE_NAME_IN_TOWERS
                ||S1S2ControlParaGet(index)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)
            {
                if(S1S2ControlParaGet(index)->stateByte == SWITCH_VALVE_PARA_CLOSE)
                {                
                    DeviceControlParaGet()->isEemergencyStop = TRUE;
                }
            }
            else if(S1S2ControlParaGet(index)->cDevice.place.type == DEVICE_NAME_FLINE)
            {
                if(S1S2ControlParaGet(index)->stateByte == FOODLINE_PARA_CLOSE)
                {                    
                    DeviceControlParaGet()->isEemergencyStop = TRUE;
                }
            }
            else if(S1S2ControlParaGet(index)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
            {
                if(S1S2ControlParaGet(index)->stateByte == TOWERSOUT_PARA_SOTP)
                {                    
                    DeviceControlParaGet()->isEemergencyStop = TRUE;
                }
            }
        }

    }
}


static void IdleStateProcess(void)
{
    DeviceExistCal();
 	if(DeviceControlParaGet()->isHaveDevice)
 		StateChange(&gStateReady);
}

static void IdleStateInit(void)
{
	gStateIdle.state = STATE_CHANGE_IDLE;
	gStateIdle.Entry = IdleStateEntry;
	gStateIdle.Exit  = IdleStateExit;
	gStateIdle.Process = IdleStateProcess;
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
    DeviceStopAndInquire();
    DeviceExistCal();
    DeviceAlarmCal();
 	if(DeviceControlParaGet()->isHaveDevice == FALSE)
 		StateChange(&gStateIdle);
    else if(DeviceControlParaGet()->isHaveAlarm == TRUE)
 		StateChange(&gStateSuspend);
    else if(DeviceControlParaGet()->isClickStart == TRUE)
 		StateChange(&gStateRunning);
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
}

static void RunningStateExit(void)
{

}

static void RunningStateProcess(void)
{
    DeviceControlAndInquire();
    DeviceAlarmCal();
//    DevicEemergencyStopCal();
    if(DeviceControlParaGet()->isHaveAlarm == TRUE)
 		StateChange(&gStateSuspend);
    if(DeviceControlParaGet()->isClickStop == TRUE)
 		StateChange(&gStateReady);
    if(DeviceControlParaGet()->isClickShutdown == TRUE)
 		StateChange(&gStateReady);
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
}

static void SuspendStateExit(void)
{

}

static void SuspendStateProcess(void)
{
    DeviceStopAndInquire();
    DeviceAlarmCal();
    if(DeviceControlParaGet()->isHaveAlarm == FALSE)
 		StateChange(&gStateIdle);
 	if(DeviceControlParaGet()->isHaveDevice == FALSE)
 		StateChange(&gStateIdle);
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

