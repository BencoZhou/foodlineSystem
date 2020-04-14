/*****************************************************************
 * File: StateMachine.h
 * Date: 2019/12/03 11:20
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_
#include "System.h"

typedef enum
{
	STATE_CHANGE_IDLE = 1,
	STATE_CHANGE_READY,
	STATE_CHANGE_RUNNING,
	STATE_CHANGE_SUSPEND,
	STATE_CHANGE_SUSPENDING,
}STATE_CHANGE;

void StateMachineInit(void);
void StateMachineProcess(void);

#endif // _STATEMACHINE_H_



