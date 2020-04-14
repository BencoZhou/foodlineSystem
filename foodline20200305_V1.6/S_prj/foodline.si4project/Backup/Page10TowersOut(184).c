/*****************************************************************
 * File: Page10TowersOut.c
 * Date: 2019/11/29 10:29
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page10TowersOut.h"

static DevicePara gPage10DevicePara;
static u8 gPage10Seq; // 序号，累加
static bool gPage10Action = FALSE;


void Page10TowersOutInit(void)
{
    gPage10DevicePara.place.device = FOODLINE_STATE;
    TowersOutControlGet()->rcAlarm = 0;
    gPage10Action = FALSE;

    TowersOutControlGet()->currentA = 0;
    TowersOutControlGet()->currentB = 0;
}

void Page10ControlSend(void)
{
    u8 index = 0;
    u8 path;
    path = PathTrav(gPage10DevicePara.id);
    if(path == PATH_NUMBERS)
        return;
    WirlessParaGet()->buffer[index++] = (LocalDeviceIdGet()>>8)&0xFF;
    WirlessParaGet()->buffer[index++] = LocalDeviceIdGet()&0xFF;
    WirlessParaGet()->buffer[index++] = (gPage10DevicePara.id>>8)&0xFF; 
    WirlessParaGet()->buffer[index++] = gPage10DevicePara.id&0xFF;
    WirlessParaGet()->buffer[index++] = gPage10Seq++;
    WirlessParaGet()->buffer[index++] = path;
    WirlessParaGet()->buffer[index++] = 0x36;
    WirlessParaGet()->buffer[index++] = TowersOutControlGet()->control;
    WirlessParaGet()->buffer[index++] = (TowersOutControlGet()->overtime>>8)&0xFF; 
    WirlessParaGet()->buffer[index++] = TowersOutControlGet()->overtime&0xFF;
    
    WirlessParaGet()->cmd = 0x30;
    WirlessParaGet()->len = index;
    ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id);
}

void Page10ControlAsk(void)
{
    u8 index = 0;
    u8 path;
    path = PathTrav(gPage10DevicePara.id);
    if(path == PATH_NUMBERS)
        return;
    WirlessParaGet()->buffer[index++] = (LocalDeviceIdGet()>>8)&0xFF;
    WirlessParaGet()->buffer[index++] = LocalDeviceIdGet()&0xFF;
    WirlessParaGet()->buffer[index++] = (gPage10DevicePara.id>>8)&0xFF; 
    WirlessParaGet()->buffer[index++] = gPage10DevicePara.id&0xFF;
    WirlessParaGet()->buffer[index++] = gPage10Seq++;
    WirlessParaGet()->buffer[index++] = path;
    WirlessParaGet()->buffer[index++] = 0x38;
    
    WirlessParaGet()->cmd = 0x30;
    WirlessParaGet()->len = index;
    ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id);

    TowersOutControlGet()->currentA++;
}

void Page10TowersOutProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{
	u32 data;
	if(len == 4)
		data = pbuf[0] << 24 | pbuf[1] << 16 | pbuf[2] << 8 | pbuf[3];
	else
		data = pbuf[0] << 8 | pbuf[1];

	// 数据录入
    if(addr == PAGE10_TOWERS_CONTROL)
    {
        TowersOutControlGet()->control = data;
    }
    if(addr == PAGE10_TOWERS_OVERTIME)
    {
        TowersOutControlGet()->overtime = data;
    }
    if(addr == PAGE10_TYPE1)
    {
        gPage10DevicePara.place.type = data;
    }
    if(addr == PAGE10_AREA1)
    {
        gPage10DevicePara.place.area = data;
    }
    if(addr == PAGE10_MAIN_LINE1)
    {
        gPage10DevicePara.place.mainLine = data;
    }
    if(addr == PAGE10_VICE_LINE1)
    {
        gPage10DevicePara.place.viceLine = data;
    }
    // 按钮
    if(addr == PAGE10_TOWERS_CONTROL_BUTTON)
    {
        Page10ControlSend();
    }
    if(addr == PAGE10_TOWERS_STATE_BUTTON)
    {
        gPage10Action = !gPage10Action;
    }
}

void Page10TowersOutRefresh(void)
{
	char str[12];
	u32 tempID;
    if(gPage10Action)
        Page10ControlAsk();
    
	DisplayCommNumSend(PAGE10_TYPE1      , gPage10DevicePara.place.type);
	DisplayCommNumSend(PAGE10_AREA1      , gPage10DevicePara.place.area);
	DisplayCommNumSend(PAGE10_MAIN_LINE1 , gPage10DevicePara.place.mainLine);
	DisplayCommNumSend(PAGE10_VICE_LINE1 , gPage10DevicePara.place.viceLine);
	tempID = gPage10DevicePara.id;
	sprintf(str, "0x%08X", tempID);
	DisplayCommTextSend(PAGE10_ID1,	(u8*)str, sizeof(str));
	DisplayCommNumSend(PAGE10_TOWERS_RC_ALARM , TowersOutControlGet()->rcAlarm);
	DisplayCommNumSend(PAGE10_TOWERS_CURRENT_A , TowersOutControlGet()->currentA);
	DisplayCommNumSend(PAGE10_TOWERS_CURRENT_B , TowersOutControlGet()->currentB);
	DisplayCommNumSend(PAGE10_TOWERS_CURRENT_C , TowersOutControlGet()->currentC);
	DisplayCommNumSend(PAGE10_TOWERS_WORK_STATE, TowersOutControlGet()->motorState);
	OSTimeDly(3);    
	DisplayCommNumSend(PAGE10_TOWERS_ALARM0_STATE, TowersOutControlGet()->alarm.b.b0);
	DisplayCommNumSend(PAGE10_TOWERS_ALARM1_STATE, TowersOutControlGet()->alarm.b.b1);
	DisplayCommNumSend(PAGE10_TOWERS_ALARM2_STATE, TowersOutControlGet()->alarm.b.b2);
	DisplayCommNumSend(PAGE10_TOWERS_ALARM3_STATE, TowersOutControlGet()->alarm.b.b3);
	DisplayCommNumSend(PAGE10_TOWERS_ALARM4_STATE, TowersOutControlGet()->alarm.b.b4);
	DisplayCommNumSend(PAGE10_TOWERS_ALARM5_STATE, TowersOutControlGet()->alarm.b.b5);    
	DisplayCommNumSend(PAGE10_TOWERS_REAL_STATE  , TowersOutControlGet()->manualAuto);
	OSTimeDly(3);    
}

