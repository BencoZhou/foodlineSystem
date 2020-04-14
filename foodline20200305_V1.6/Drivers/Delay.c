#include "IrqPriority.h"
#include "Delay.h"


const u32 fac_for_us = 9;     // for 72000000


void delay_us(u32 nus)
{
    u32 ticks, told, tnow, tcnt = 0;
    u32 reload = SysTick->LOAD;
    ticks = nus*fac_for_us;
    
    OSSchedLock();
    told = SysTick->VAL;
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
                tcnt += told-tnow;
            else
                tcnt += (reload-tnow+told);
            
            told = tnow;
            if (tcnt >= ticks)
                break;
        }
    }
    OSSchedUnlock();
}

