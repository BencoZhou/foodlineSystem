/*****************************************************************
 * File: FlineCtrl.h
 * Date: 2019/06/19 10:52
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _FLINECTRL_H_
#define _FLINECTRL_H_

#include "System.h"


typedef struct  
{
	bool isInit;
	bool isReady;
	u32 destAddrs[2];
    u32 rcTime;
}FlineCtrlMsg;

void FlineCtrlInit(void);
void FlineCtrlProcess(void);
FlineCtrlMsg *FlineCtrlMsgGet(void);
void FlineCtrlPower(void);

#endif // _FLINECTRL_H_

