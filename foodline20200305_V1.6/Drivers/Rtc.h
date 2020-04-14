#ifndef MZ_RTC_HEADER_H
#define MZ_RTC_HEADER_H

#include "stm32f10x.h"
#include "ucos_ii.h"
#include "Debug.h"

#define DAY_SECONDS     86400
#define HOUR_SECONDS	3600

typedef struct
{
    u16 year;
    u8 mon;
    u8 day;
    u8 hour;
    u8 min;
    u8 sec;
    u8 week;
} RTC_TIME;

typedef enum
{
    RtcOp_CheckVal,
    RtcOp_SetTime,
    RtcOp_SetAlarm,
} RTC_OPERATE;


void RTC_SetUp(void);
u32 RTC_GetTime(RTC_TIME *pTime);
bool RTC_Adjust(RTC_TIME *pTime, RTC_OPERATE Op);
void RTC_SetTime(u8 year, u8 month, u8 day, u8 hour, u8 min, u8 sec);
void RTCTimeTodate(RTC_TIME *cTime, u32 conTime);
void rtcTimeConver(RTC_TIME *sTime, u32 rtcTime);


u16 TimeDeciConvertHex(u16 timeDeci);
u16 TimeHexConvertDeci(u16 timeHex);


#endif
