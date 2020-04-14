#ifndef MZSYS_TIME_HEADER_H
#define MZSYS_TIME_HEADER_H

#include "stm32f10x.h"
#include "Debug.h"

typedef enum
{
    MZ_TIM_DS18B20 = 0, // Tim2
    MZ_TIM_VOICEPLAY,   // Tim4
    MZ_TIM_500MS,       // Tim5
} MZ_TIM_ID;


void Tim2_Init(void);
void Tim4_Init(void);
void Tim5_Init(void);
void Tim2_Set(u16 Val, u16 uS_Base, bool AutoReload);
void Tim4_Set(u16 Val, u16 uS_Base, bool AutoReload);
void Tim5_Set(u16 Val, u16 uS_Base, bool AutoReload);
void MZ_TimSet(MZ_TIM_ID TimId, u16 Val, u16 uS_Base, bool AutoReload);

#endif
