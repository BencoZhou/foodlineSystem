/*****************************************************************
 * File: ParaHistory.c
 * Date: 2018/11/21 10:01
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "ParaHistory.h"
#include "Rtc.h"

static HistoryPara gHistoryPara;
static LogTimeHdr gLogTimeHdr;
static RTC_TIME gTempTime;
static u32 gInitTime, gLastInitTime;

void ParaHistoryInit(void)
{
	gHistoryPara.isDoneFirstWr = FALSE;
	gInitTime = RTC_GetTime(&gTempTime);
	gInitTime -= DAY_SECONDS;	
	gInitTime += HOUR_SECONDS;
	memset(&gHistoryPara, 0, sizeof(HistoryPara));
}

void ParaHistoryFirstWr(u8 count)
{
	static u8 tempBuf[87], len, i;
	static u8 lastCount;

	if(count >= HISTORY_CURVE_SAVE_NUMBER)
	{
		gHistoryPara.isDoneFirstWr = TRUE;
		return;
	}

	if(lastCount != count)
	{
		lastCount = count;
		gInitTime += HOUR_SECONDS;
	}
	RTCTimeTodate(&gTempTime, gInitTime);
	gLogTimeHdr.date.t.year	= (gTempTime.year%100);
	gLogTimeHdr.date.t.mon	= gTempTime.mon;
	gLogTimeHdr.date.t.day	= gTempTime.day;
	gLogTimeHdr.hour		= gTempTime.hour;
	gLogTimeHdr.min			= 0;
	
	tempBuf[0] = 0xFF;
	tempBuf[1] = 0;
	len = sizeof(LogTimeHdr);
	memcpy(&tempBuf[2], &gLogTimeHdr, len);
	len += 2;
	if(count == 0)
	{
		for(i = 0; i < HISTORY_CURVE_SAVE_NUMBER; i++)
		{
			if(gLogTimeHdr.hour + i > 23)
				gHistoryPara.hour[i] = gLogTimeHdr.hour + i - 24;
			else
				gHistoryPara.hour[i] = gLogTimeHdr.hour + i;
		}
	}
	WirelessApp_SendData(CMD_HISTORY_DATA, 
							FRAME_NEED_NO_ACK, 
							tempBuf, 
							len, 
							ParaSettingLibGet()->gPiggeryConf.localid,							
							ParaSettingLibGet()->gPiggeryConf.ninesensor1,					
							ParaSettingLibGet()->gPiggeryConf.ninesensor1,
							0);

}

void ParaHistoryHourWr(void)
{	
	static u8 tempBuf[87], len;
	static RTC_TIME lastTempTime;
	gLastInitTime = RTC_GetTime(&lastTempTime);
	if(gInitTime < gLastInitTime && lastTempTime.hour != gLogTimeHdr.hour && lastTempTime.min > 2)
	{
		gInitTime = gLastInitTime;
		gLogTimeHdr.date.t.year	= (lastTempTime.year%100);
		gLogTimeHdr.date.t.mon	= lastTempTime.mon;
		gLogTimeHdr.date.t.day	= lastTempTime.day;
		gLogTimeHdr.hour		= lastTempTime.hour;
		gLogTimeHdr.min			= 0;
		
		tempBuf[0] = 0xFF;
		tempBuf[1] = 0;
		len = sizeof(LogTimeHdr);
		memcpy(&tempBuf[2], &gLogTimeHdr, len);
		len += 2;

		memcpy(&gHistoryPara.hour[0], &gHistoryPara.hour[1], HISTORY_CURVE_SAVE_NUMBER-1);
		gHistoryPara.hour[HISTORY_CURVE_SAVE_NUMBER-1] = gLogTimeHdr.hour;
		WirelessApp_SendData(CMD_HISTORY_DATA, 
								FRAME_NEED_NO_ACK, 
								tempBuf, 
								len, 
								ParaSettingLibGet()->gPiggeryConf.localid,							
								ParaSettingLibGet()->gPiggeryConf.ninesensor1,				
								ParaSettingLibGet()->gPiggeryConf.ninesensor1,
								0);
	}
}

void ParaHistoryRc(SensorStatVars *pSensorStatVars, OuterSensor *pOuterSensor)
{

}

void ParaHistoryDataProcess(bool isNeedInit)
{
//	static u8 sCnt = 0xFF;
//	if(isNeedInit == TRUE)
//		sCnt = 0xFF;
	if(gHistoryPara.isDoneFirstWr == FALSE)		
	{
		if((isNeedInit)||(gHistoryPara.dataRcCnt != gHistoryPara.dataSendCnt))
		{
			gHistoryPara.dataSendCnt = gHistoryPara.dataRcCnt;
	 		ParaHistoryFirstWr(gHistoryPara.dataSendCnt);
		}
	}
	else if(gHistoryPara.isDoneFirstWr == TRUE)
	{
		ParaHistoryHourWr();
	}
}

HistoryPara* ParaHistoryGet(void)
{
	return &gHistoryPara;
}


