#include "stm32f10x_pwr.h"
#include "IrqPriority.h"
#include "Rtc.h"


#define RTC_START_YEAR  1912

const s32 Leap_Month_Seconds[13] = 
{
    0,
    DAY_SECONDS*31,
    DAY_SECONDS*(31+29),
    DAY_SECONDS*(31+29+31),
    DAY_SECONDS*(31+29+31+30),
    DAY_SECONDS*(31+29+31+30+31),
    DAY_SECONDS*(31+29+31+30+31+30),
    DAY_SECONDS*(31+29+31+30+31+30+31),
    DAY_SECONDS*(31+29+31+30+31+30+31+31),
    DAY_SECONDS*(31+29+31+30+31+30+31+31+30),
    DAY_SECONDS*(31+29+31+30+31+30+31+31+30+31),
    DAY_SECONDS*(31+29+31+30+31+30+31+31+30+31+30),
    DAY_SECONDS*(31+29+31+30+31+30+31+31+30+31+30+31),
};


const s32 Month_Seconds[13] = 
{
    0,
    DAY_SECONDS*31,
    DAY_SECONDS*(31+28),
    DAY_SECONDS*(31+28+31),
    DAY_SECONDS*(31+28+31+30),
    DAY_SECONDS*(31+28+31+30+31),
    DAY_SECONDS*(31+28+31+30+31+30),
    DAY_SECONDS*(31+28+31+30+31+30+31),
    DAY_SECONDS*(31+28+31+30+31+30+31+31),
    DAY_SECONDS*(31+28+31+30+31+30+31+31+30),
    DAY_SECONDS*(31+28+31+30+31+30+31+31+30+31),
    DAY_SECONDS*(31+28+31+30+31+30+31+31+30+31+30),
    DAY_SECONDS*(31+28+31+30+31+30+31+31+30+31+30+31),
};

#define RTCClockSource_LSE  // 32.768KHz 


/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : Configures the rtc.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Configuration(void)
{
    u32 rtcintcnt = 0x200000;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    BKP_DeInit();
    RCC_LSEConfig(RCC_LSE_ON);
    
    while ((RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) && (--rtcintcnt));
    if (rtcintcnt != 0)
    {
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        RCC_RTCCLKCmd(ENABLE);
        RTC_WaitForSynchro();
        RTC_WaitForLastTask();
        #if 0
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
        RTC_WaitForLastTask();
        #endif
        RTC_SetPrescaler(32767);
        RTC_WaitForLastTask();
        
        Debug("RTC LSE WORK OK!!!\n\r");
    }
    else
    {
        Debug("!!!RTC LSE NO WORK!!!\n\r");
        
        rtcintcnt = 0x200000;
        RCC_HSEConfig(RCC_HSE_ON);
        while ((RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET) && (--rtcintcnt));
        if (rtcintcnt == 0)
            return;
        
        RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);
        
        RCC_RTCCLKCmd(ENABLE);
        RTC_WaitForSynchro();
        RTC_WaitForLastTask();
        
        RTC_SetPrescaler(8000000/128-1);
        RTC_WaitForLastTask();
    }
}


/*******************************************************************************
* Function Name  : RTC_Config
* Description    : called when power on, auto check if need to init RTC
*                  if need to init RTC again, call RTC_Configuration()
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_SetUp(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = RTC_Priority;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // battery power down, need to config rtc again
    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
    {
        RTC_TIME NowTime;
        
        Debug("RTC need configure!\n\r");
        
        RTC_Configuration();
        
        NowTime.year = 2017;
        NowTime.mon = 1;
        NowTime.day = 1;
        NowTime.hour = 0;
        NowTime.min = 0;
        NowTime.sec = 0;
        if (RTC_Adjust(&NowTime, RtcOp_SetTime) == TRUE)
        {
            Debug("RTC set sucess!\n\r");
        }
        else
        {
            Debug("RTC set error!\n\r");
        }
        
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
    }
    else
    {
#if USE_RTC_WITH_BATTERY == 0
        // in fact, no need
        RTC_Configuration();
#endif
        
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
        {
            Debug("System power on reset!\n\r");
        }
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
        {
            Debug("System reset!\n\r");
        }
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP, ENABLE);
        PWR_BackupAccessCmd(ENABLE);
        
        RTC_WaitForSynchro();
        RTC_WaitForLastTask();
        #if 0
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
        RTC_WaitForLastTask();
        #endif
    }
    RCC_ClearFlag();
}

void RTCTimeTodate(RTC_TIME *cTime, u32 conTime)
{
    u32 tmp, i;
    
    tmp = conTime % (DAY_SECONDS*366+DAY_SECONDS*365*3);
    if (tmp < DAY_SECONDS*366) cTime->year = RTC_START_YEAR+conTime/(DAY_SECONDS*366+DAY_SECONDS*365*3)*4+0;
    else if (tmp < DAY_SECONDS*366+DAY_SECONDS*365*1) cTime->year = RTC_START_YEAR+conTime/(DAY_SECONDS*366+DAY_SECONDS*365*3)*4+1;
    else if (tmp < DAY_SECONDS*366+DAY_SECONDS*365*2) cTime->year = RTC_START_YEAR+conTime/(DAY_SECONDS*366+DAY_SECONDS*365*3)*4+2;
    else cTime->year = RTC_START_YEAR+conTime/(DAY_SECONDS*366+DAY_SECONDS*365*3)*4+3;
    
    if (tmp < DAY_SECONDS*366)
    {
        for (i=1;i<13;i++)
        {
            if (tmp < Leap_Month_Seconds[i])
            {
                cTime->mon = i;
                tmp -= Leap_Month_Seconds[i-1];
                break;
            }
       }
    }
    else if (tmp < DAY_SECONDS*366+DAY_SECONDS*365*1)
    {
        tmp -= DAY_SECONDS*366;
        for (i=1;i<13;i++)
        {
            if (tmp < Month_Seconds[i])
            {
                cTime->mon = i;
                tmp -= Month_Seconds[i-1];
                break;
            }
        }
    }
    else if (tmp < DAY_SECONDS*366+DAY_SECONDS*365*2)
    {
        tmp -= DAY_SECONDS*366+DAY_SECONDS*365*1;
        for (i=1;i<13;i++)
        {
            if (tmp < Month_Seconds[i])
            {
                cTime->mon = i;
                tmp -= Month_Seconds[i-1];
                break;
            }
        }
    }
    else
    {
        tmp -= DAY_SECONDS*366+DAY_SECONDS*365*2;
        for (i=1;i<13;i++)
        {
            if (tmp < Month_Seconds[i])
            {
                cTime->mon = i;
                tmp -= Month_Seconds[i-1];
                break;
            }
        }
    }
    
    // 1912.1.1 Monday (0 : Monday)
    cTime->week = conTime/DAY_SECONDS%7;
    cTime->day = tmp/DAY_SECONDS+1;
    tmp = tmp % DAY_SECONDS;
    cTime->hour = tmp/3600;
    tmp = tmp % 3600;
    cTime->min = tmp/60;
    cTime->sec = tmp%60;
}

void rtcTimeConver(RTC_TIME *sTime, u32 rtcTime)
{
    u32 tmp, i;
    tmp = rtcTime % (DAY_SECONDS*366+DAY_SECONDS*365*3);
    if (tmp < DAY_SECONDS*366) sTime->year = RTC_START_YEAR+rtcTime/(DAY_SECONDS*366+DAY_SECONDS*365*3)*4+0;
    else if (tmp < DAY_SECONDS*366+DAY_SECONDS*365*1) sTime->year = RTC_START_YEAR+rtcTime/(DAY_SECONDS*366+DAY_SECONDS*365*3)*4+1;
    else if (tmp < DAY_SECONDS*366+DAY_SECONDS*365*2) sTime->year = RTC_START_YEAR+rtcTime/(DAY_SECONDS*366+DAY_SECONDS*365*3)*4+2;
    else sTime->year = RTC_START_YEAR+rtcTime/(DAY_SECONDS*366+DAY_SECONDS*365*3)*4+3;
    
    if (tmp < DAY_SECONDS*366)
    {
        for (i=1;i<13;i++)
        {
            if (tmp < Leap_Month_Seconds[i])
            {
                sTime->mon = i;
                tmp -= Leap_Month_Seconds[i-1];
                break;
            }
       }
    }
    else if (tmp < DAY_SECONDS*366+DAY_SECONDS*365*1)
    {
        tmp -= DAY_SECONDS*366;
        for (i=1;i<13;i++)
        {
            if (tmp < Month_Seconds[i])
            {
                sTime->mon = i;
                tmp -= Month_Seconds[i-1];
                break;
            }
        }
    }
    else if (tmp < DAY_SECONDS*366+DAY_SECONDS*365*2)
    {
        tmp -= DAY_SECONDS*366+DAY_SECONDS*365*1;
        for (i=1;i<13;i++)
        {
            if (tmp < Month_Seconds[i])
            {
                sTime->mon = i;
                tmp -= Month_Seconds[i-1];
                break;
            }
        }
    }
    else
    {
        tmp -= DAY_SECONDS*366+DAY_SECONDS*365*2;
        for (i=1;i<13;i++)
        {
            if (tmp < Month_Seconds[i])
            {
                sTime->mon = i;
                tmp -= Month_Seconds[i-1];
                break;
            }
        }
    }
    
    // 1912.1.1 Monday (0 : Monday)
    sTime->week = rtcTime/DAY_SECONDS%7;
    sTime->day = tmp/DAY_SECONDS+1;
    tmp = tmp % DAY_SECONDS;
    sTime->hour = tmp/3600;
    tmp = tmp % 3600;
    sTime->min = tmp/60;
    sTime->sec = tmp%60;
}


u32 RTC_GetTime(RTC_TIME *pTime)
{
    u32 NowTime;
    u32 tmp, i;
    
    RTC_WaitForLastTask();
    NowTime = RTC_GetCounter();
    RTC_WaitForLastTask();
    
    tmp = NowTime % (DAY_SECONDS*366+DAY_SECONDS*365*3);
    if (tmp < DAY_SECONDS*366) pTime->year = RTC_START_YEAR+NowTime/(DAY_SECONDS*366+DAY_SECONDS*365*3)*4+0;
    else if (tmp < DAY_SECONDS*366+DAY_SECONDS*365*1) pTime->year = RTC_START_YEAR+NowTime/(DAY_SECONDS*366+DAY_SECONDS*365*3)*4+1;
    else if (tmp < DAY_SECONDS*366+DAY_SECONDS*365*2) pTime->year = RTC_START_YEAR+NowTime/(DAY_SECONDS*366+DAY_SECONDS*365*3)*4+2;
    else pTime->year = RTC_START_YEAR+NowTime/(DAY_SECONDS*366+DAY_SECONDS*365*3)*4+3;
    
    if (tmp < DAY_SECONDS*366)
    {
        for (i=1;i<13;i++)
        {
            if (tmp < Leap_Month_Seconds[i])
            {
                pTime->mon = i;
                tmp -= Leap_Month_Seconds[i-1];
                break;
            }
       }
    }
    else if (tmp < DAY_SECONDS*366+DAY_SECONDS*365*1)
    {
        tmp -= DAY_SECONDS*366;
        for (i=1;i<13;i++)
        {
            if (tmp < Month_Seconds[i])
            {
                pTime->mon = i;
                tmp -= Month_Seconds[i-1];
                break;
            }
        }
    }
    else if (tmp < DAY_SECONDS*366+DAY_SECONDS*365*2)
    {
        tmp -= DAY_SECONDS*366+DAY_SECONDS*365*1;
        for (i=1;i<13;i++)
        {
            if (tmp < Month_Seconds[i])
            {
                pTime->mon = i;
                tmp -= Month_Seconds[i-1];
                break;
            }
        }
    }
    else
    {
        tmp -= DAY_SECONDS*366+DAY_SECONDS*365*2;
        for (i=1;i<13;i++)
        {
            if (tmp < Month_Seconds[i])
            {
                pTime->mon = i;
                tmp -= Month_Seconds[i-1];
                break;
            }
        }
    }
    
    // 1912.1.1 Monday (0 : Monday)
    pTime->week = NowTime/DAY_SECONDS%7;
    pTime->day = tmp/DAY_SECONDS+1;
    tmp = tmp % DAY_SECONDS;
    pTime->hour = tmp/3600;
    tmp = tmp % 3600;
    pTime->min = tmp/60;
    pTime->sec = tmp%60;
    
    return NowTime;
}

void RTC_SetTime(u8 year, u8 month, u8 day, u8 hour, u8 min, u8 sec)
{
	static RTC_TIME setTime;
	setTime.year = 2000 + TimeHexConvertDeci(year);
	setTime.mon  = TimeHexConvertDeci(month);
	setTime.day  = TimeHexConvertDeci(day);
	setTime.hour = TimeHexConvertDeci(hour);
	setTime.min  = TimeHexConvertDeci(min);
	setTime.sec  = TimeHexConvertDeci(sec);
	RTC_Adjust(&setTime, RtcOp_SetTime);
}


bool RTC_Adjust(RTC_TIME *pTime, RTC_OPERATE Op)
{
    u32 tmp1, tmp2;
    u8 Leap_Flag = 0;
    
    tmp1 = pTime->year-RTC_START_YEAR;
    // year check
    if ((tmp1 > 135) || (pTime->year < RTC_START_YEAR)) return FALSE;
    
    if (tmp1) tmp2 = (tmp1-1)/4+1;
    else tmp2 = 0;
    Leap_Flag = (tmp1%4)?0:1;
    // year -> seconds
    tmp1 = (tmp1*365+tmp2)*DAY_SECONDS;
    
    if ((pTime->mon < 1) || (pTime->mon > 12)) return FALSE;
    
    if (Leap_Flag)
    {
        if (pTime->day > ((Leap_Month_Seconds[pTime->mon]-Leap_Month_Seconds[pTime->mon-1])/DAY_SECONDS)) return FALSE;
        tmp2 = Leap_Month_Seconds[pTime->mon-1];
    }
    else
    {
        if (pTime->day > ((Month_Seconds[pTime->mon]-Month_Seconds[pTime->mon-1])/DAY_SECONDS)) return FALSE;
        tmp2 = Month_Seconds[pTime->mon-1];
    }
    
    if (pTime->hour > 23) return FALSE;
    if (pTime->min > 59) return FALSE;
    if (pTime->sec > 59) return FALSE;
    
    tmp1 += (tmp2+(pTime->day-1)*DAY_SECONDS);
    tmp1 += (pTime->hour*3600 + pTime->min*60 + pTime->sec);
    
    switch (Op)
    {
    case RtcOp_SetTime:
        RTC_WaitForLastTask(); 
        RTC_SetCounter(tmp1);    
        RTC_WaitForLastTask();
        break;
        
    case RtcOp_SetAlarm:
        RTC_WaitForLastTask(); 
        RTC_SetAlarm(tmp1);
        RTC_WaitForLastTask();
        break;
    }
    
    return TRUE;
}


u16 TimeDeciConvertHex(u16 timeDeci)
{
	static u16 timeHex, hour, min;
	hour = timeDeci/1000*16 + timeDeci/100%10;
	min = timeDeci%100/10*16 + timeDeci%10;
	if(min > 0x59)
	{
		min -= 0x60;
		hour++;
	}	
//	if(hour > 0x23)
//	{
//		hour -= 0x24;
//	}
	timeHex = (hour * 0x100) + min;
	return timeHex;
}

u16 TimeHexConvertDeci(u16 timeHex)
{
	static u16 timeDeci, hour, min;
	hour = timeHex/0x1000*10 + timeHex/0x100%0x10;
	min = timeHex%0x100/0x10*10 + timeHex%0x10;
	timeDeci = hour * 100 + min;
	return timeDeci;
	
}


