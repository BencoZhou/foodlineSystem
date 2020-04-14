#ifndef MZSYS_DISPATCHER_HEADER_H
#define MZSYS_DISPATCHER_HEADER_H  

#include "stm32f10x.h"
#include "Debug.h"


void Dispatcher_Init(void);
void Dispatcher_Task(void *Task_Parameters);

u32  LocalDeviceIdGet(void);
void LocalDeviceIdSet(u32 id);


#endif
