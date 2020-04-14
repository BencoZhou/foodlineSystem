/*****************************************************************
 * File: StateMachine.c
 * Date: 2019/12/03 11:21
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "StateMachine.h"

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

static void IdleStateProcess(void)
{
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
}

static void ReadyStateExit(void)
{

}

static void ReadyStateProcess(void)
{
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
}

static void RunningStateExit(void)
{

}

static void RunningStateProcess(void)
{
    if(DeviceControlParaGet()->isHaveAlarm == TRUE)
 		StateChange(&gStateSuspend);
    if(DeviceControlParaGet()->isClickStop == TRUE)
 		StateChange(&gStateReady);
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
}

static void SuspendStateExit(void)
{

}

static void SuspendStateProcess(void)
{
    if(DeviceControlParaGet()->isHaveAlarm == FALSE)
 		StateChange(&gStateIdle);
 	if(DeviceControlParaGet()->isHaveDevice == FALSE)
 		StateChange(&gStateIdle);
}

static void SuspendStateInit(void)
{
	gStateRunning.state = STATE_CHANGE_SUSPEND;
	gStateRunning.Entry = SuspendStateEntry;
	gStateRunning.Exit  = SuspendStateExit;
	gStateRunning.Process = SuspendStateProcess;
}


