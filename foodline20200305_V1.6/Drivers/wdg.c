#include "stm32f10x_iwdg.h"
#include "wdg.h"


void IWDG_Init(u8 prer, u16 rlr)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);   // enable write to IWDG_PR and IWDG_RLR
    
    IWDG_SetPrescaler(prer);    // 64
    
    IWDG_SetReload(rlr);
    
    IWDG_ReloadCounter();
    
    IWDG_Enable();
}


void IWDG_Feed(void)
{
    IWDG_ReloadCounter();
}
