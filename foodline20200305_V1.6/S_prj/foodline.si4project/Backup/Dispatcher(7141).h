#ifndef MZSYS_DISPATCHER_HEADER_H
#define MZSYS_DISPATCHER_HEADER_H  

#include "stm32f10x.h"
#include "Debug.h"


void Dispatcher_Init(void);
void Dispatcher_Task(void *Task_Parameters);

void NineSensorIdSet(u32 id);
void LocalDeviceIdSet(u32 id);


#endif
