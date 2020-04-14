#include "IrqPriority.h"
#include "Time.h"


void Tim2_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIME2_IRQn_Priority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
}

void Tim4_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIME4_IRQn_Priority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
}

void Tim5_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIME5_IRQn_Priority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
}



// uS_Base : 1 - us, 100 - 100us,  1 -> 900
// timer val = Val x uS_Base x 1us
// new val overwrite old val
// AutoReload single or circle
// val or uS_Base is 0, stop timer
void Tim2_Set(u16 Val, u16 uS_Base, bool AutoReload)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_ClocksTypeDef RCC_Clocks;

    RCC_GetClocksFreq(&RCC_Clocks);

    TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
    TIM_Cmd(TIM2, DISABLE);

    if ((Val == 0) || (uS_Base == 0)) return;

    if (uS_Base > 900) uS_Base = 900;
    
    // timer frequency : 72M/(prefreq+1)/preload
    TIM_TimeBaseStructure.TIM_Period = (Val-1);
    TIM_TimeBaseStructure.TIM_Prescaler = ((RCC_Clocks.SYSCLK_Frequency/1000000)*uS_Base - 1);

    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    if (AutoReload) TIM_SelectOnePulseMode(TIM2, TIM_OPMode_Repetitive);
    else TIM_SelectOnePulseMode(TIM2, TIM_OPMode_Single);
    
    TIM_ClearFlag(TIM2, TIM_IT_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
}


void Tim4_Set(u16 Val, u16 uS_Base, bool AutoReload)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_ClocksTypeDef RCC_Clocks;

    RCC_GetClocksFreq(&RCC_Clocks);

    TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
    TIM_Cmd(TIM4, DISABLE);

    if ((Val == 0) || (uS_Base == 0)) return;

    if (uS_Base > 900) uS_Base = 900;
    
    TIM_TimeBaseStructure.TIM_Period = (Val - 1);
    TIM_TimeBaseStructure.TIM_Prescaler = ((RCC_Clocks.SYSCLK_Frequency/1000000)*uS_Base-1);

    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    if (AutoReload) TIM_SelectOnePulseMode(TIM4, TIM_OPMode_Repetitive);
    else TIM_SelectOnePulseMode(TIM4, TIM_OPMode_Single);
    
    TIM_ClearFlag(TIM4, TIM_IT_Update);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM4, ENABLE);
}


void Tim5_Set(u16 Val, u16 uS_Base, bool AutoReload)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_ClocksTypeDef RCC_Clocks;

    RCC_GetClocksFreq(&RCC_Clocks);

    TIM_ITConfig(TIM5, TIM_IT_Update, DISABLE);
    TIM_Cmd(TIM5, DISABLE);

    if ((Val == 0) || (uS_Base == 0)) return;

    if (uS_Base > 900) uS_Base = 900;
    
    TIM_TimeBaseStructure.TIM_Period = (Val - 1);
    TIM_TimeBaseStructure.TIM_Prescaler = ((RCC_Clocks.SYSCLK_Frequency/1000000)*uS_Base-1);

    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    if (AutoReload) TIM_SelectOnePulseMode(TIM5, TIM_OPMode_Repetitive);
    else TIM_SelectOnePulseMode(TIM5, TIM_OPMode_Single);
    
    TIM_ClearFlag(TIM5, TIM_IT_Update);
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM5, ENABLE);
}


// TimId: select timer, Sys_Timer event triggered, IntParam include this id
// uS_Base 1: unit is us, 100: 100us, min 1, max 900
// Val: max 65535
// time: Val x uS_Base x 1us
// new timer setting will overwrite old setting
// AutoReload: once or circle
// val or uS_Base, any one set to 0, timer will stop
void MZ_TimSet(MZ_TIM_ID TimId, u16 Val, u16 uS_Base, bool AutoReload)
{
    switch (TimId)
    {
    case MZ_TIM_DS18B20:
        Tim2_Set(Val, uS_Base, AutoReload);
        break;
    case MZ_TIM_VOICEPLAY:
        Tim4_Set(Val, uS_Base, AutoReload);
        break;
    case MZ_TIM_500MS:
        Tim5_Set(Val, uS_Base, AutoReload);
        break;
    default:
        break;
    }
}
