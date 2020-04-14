/*****************************************************************
 * File: Page11TowersOutConfig.c
 * Date: 2019/11/29 11:59
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page11TowersOutConfig.h"

static DevicePara gPage11DevicePara;
static TowersOutDevicePara *PTowersOutDevicePara;

static u8 gPage11Seq; // ÐòºÅ£¬ÀÛ¼Ó

void Page11FoodlineDeviceInquiry(void)
{
    u8 index = 0;
	u8 path;
	path = PathTrav(gPage11DevicePara.id);
	if(path == PATH_NUMBERS)
		return;
    WirlessParaGet()->buffer[index++] = (LocalDeviceIdGet()>>8)&0xFF;
    WirlessParaGet()->buffer[index++] = LocalDeviceIdGet()&0xFF;
    WirlessParaGet()->buffer[index++] = (gPage11DevicePara.id>>8)&0xFF;
    WirlessParaGet()->buffer[index++] = gPage11DevicePara.id&0xFF;
    WirlessParaGet()->buffer[index++] = gPage11Seq++;
    WirlessParaGet()->buffer[index++] = path;
    WirlessParaGet()->buffer[index++] = 0x39;
    
    WirlessParaGet()->cmd = 0x30;
    WirlessParaGet()->len = index;
    
    ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id);
}

void Page11FoodlineDeviceSend(void)
{
    u8 index = 0;
	u8 path;
	path = PathTrav(gPage11DevicePara.id);
	if(path == PATH_NUMBERS)
		return;
    WirlessParaGet()->buffer[index++] = (LocalDeviceIdGet()>>8)&0xFF;
    WirlessParaGet()->buffer[index++] = LocalDeviceIdGet()&0xFF;
    WirlessParaGet()->buffer[index++] = (gPage11DevicePara.id>>8)&0xFF;
    WirlessParaGet()->buffer[index++] = gPage11DevicePara.id&0xFF;
    WirlessParaGet()->buffer[index++] = gPage11Seq++;
    WirlessParaGet()->buffer[index++] = path;
    WirlessParaGet()->buffer[index++] = 0x39;
    WirlessParaGet()->buffer[index++] = PTowersOutDevicePara->shield.buf;
    WirlessParaGet()->buffer[index++] = PTowersOutDevicePara->stateTime;
    WirlessParaGet()->buffer[index++] = PTowersOutDevicePara->motorMintime;
    WirlessParaGet()->buffer[index++] = (PTowersOutDevicePara->overCurMax>>8);
    WirlessParaGet()->buffer[index++] = PTowersOutDevicePara->overCurMax;
    WirlessParaGet()->buffer[index++] = (PTowersOutDevicePara->missPhaseMin>>8);
    WirlessParaGet()->buffer[index++] = PTowersOutDevicePara->missPhaseMin;
    WirlessParaGet()->buffer[index++] = PTowersOutDevicePara->curAlarmFilTime;
    WirlessParaGet()->buffer[index++] = (PTowersOutDevicePara->curCalib>>8);
    WirlessParaGet()->buffer[index++] = PTowersOutDevicePara->curCalib;
    WirlessParaGet()->buffer[index++] = PTowersOutDevicePara->motorOnFilTime;
    WirlessParaGet()->buffer[index++] = PTowersOutDevicePara->feedFilTime;
    WirlessParaGet()->buffer[index++] = PTowersOutDevicePara->motorState;
    
    WirlessParaGet()->cmd = 0x30;
    WirlessParaGet()->len = index;
    
    ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id);
}


void Page11FlineDeviceInit(void)
{
    gPage11DevicePara.place.device = FOODLINE_STATE;
    PTowersOutDevicePara = TowersOutDeviceParaGet();
    Page11FlineDeviceRefresh();
}

void Page11FlineDeviceProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{
    u32 data;
	if(len == 4)
		data = pbuf[0] << 24 | pbuf[1] << 16 | pbuf[2] << 8 | pbuf[3];
	else
		data = pbuf[0] << 8 | pbuf[1];

    if(addr == PAGE11_INQUIRY)
    {
        Page11FoodlineDeviceInquiry();
    }
    if(addr == PAGE11_SEND)
    {
        Page11FoodlineDeviceSend();
    }
    
    if(addr == PAGE11_ALARM_FORWARD_MAX_BUTTON)
    {
        if(PTowersOutDevicePara->shield.b.b0 == 1)
            PTowersOutDevicePara->shield.b.b0 = 0;
        else
            PTowersOutDevicePara->shield.b.b0 = 1;
    }
    if(addr == PAGE11_ALARM_FORWARD_NONE_BUTTON)
    {
        if(PTowersOutDevicePara->shield.b.b1 == 1)
            PTowersOutDevicePara->shield.b.b1 = 0;
        else
            PTowersOutDevicePara->shield.b.b1 = 1;
    }
    if(addr == PAGE11_ALARM_REVERSE_MAX_BUTTON)
    {
        if(PTowersOutDevicePara->shield.b.b2 == 1)
            PTowersOutDevicePara->shield.b.b2 = 0;
        else
            PTowersOutDevicePara->shield.b.b2 = 1;
    }
    if(addr == PAGE11_ALARM_REVERSE_NONE_BUTTON)
    {
        if(PTowersOutDevicePara->shield.b.b3 == 1)
            PTowersOutDevicePara->shield.b.b3 = 0;
        else
            PTowersOutDevicePara->shield.b.b3 = 1;
    }
    if(addr == PAGE11_ALARM_OVERCURMAX_BUTTON)
    {
        if(PTowersOutDevicePara->shield.b.b4 == 1)
            PTowersOutDevicePara->shield.b.b4 = 0;
        else
            PTowersOutDevicePara->shield.b.b4 = 1;
    }
    if(addr == PAGE11_ALARM_MISSPHASEMIN_BUTTON)
    {
        if(PTowersOutDevicePara->shield.b.b5 == 1)
            PTowersOutDevicePara->shield.b.b5 = 0;
        else
            PTowersOutDevicePara->shield.b.b5 = 1;
    }

    if(addr == PAGE11_STATETIME)
    {
        PTowersOutDevicePara->stateTime = data;
    }
    if(addr == PAGE11_MOTORMINTIME)
    {
        PTowersOutDevicePara->motorMintime = data;
    }
    if(addr == PAGE11_OVERCURMAX)
    {
        PTowersOutDevicePara->overCurMax = data;
    }
    if(addr == PAGE11_MISSPHASEMIN)
    {
        PTowersOutDevicePara->missPhaseMin = data;
    }
    if(addr == PAGE11_CURALARMFILTIME)
    {
        PTowersOutDevicePara->curAlarmFilTime = data;
    }
    if(addr == PAGE11_CURCALIB)
    {
        PTowersOutDevicePara->curCalib = data;
    }
    if(addr == PAGE11_MOTORONFILTIME)
    {
        PTowersOutDevicePara->motorOnFilTime = data;
    }
    if(addr == PAGE11_FEEDFILTIME)
    {
        PTowersOutDevicePara->feedFilTime = data;
    }
    if(addr == PAGE11_MOTOR_STATE)
    {
        PTowersOutDevicePara->motorState = data;
    }   
    
    if(addr == PAGE3_TYPE1)
    {
        gPage11DevicePara.place.type = data;
    }
    if(addr == PAGE3_AREA1)
    {
        gPage11DevicePara.place.area = data;
    }
    if(addr == PAGE3_MAIN_LINE1)
    {
        gPage11DevicePara.place.mainLine = data;
    }
    if(addr == PAGE3_VICE_LINE1)
    {
        gPage11DevicePara.place.viceLine = data;
    }


    Page11FlineDeviceRefresh();    
}
 

void Page11FlineDeviceRefresh(void)
{
	char str[12];
	u32 tempID;
	DisplayCommIconSend(PAGE3_OPENALARM_SHOW      , PTowersOutDevicePara->shield.b.b0);
	DisplayCommIconSend(PAGE3_REGULATINGALARM_SHOW, PTowersOutDevicePara->shield.b.b0);
	DisplayCommIconSend(PAGE3_CURRENTALARM_SHOW   , PTowersOutDevicePara->shield.b.b0);
	DisplayCommIconSend(PAGE3_OPENPHASEALARM_SHOW , PTowersOutDevicePara->shield.b.b0);
	DisplayCommIconSend(PAGE3_CURRENTALARM_SHOW   , PTowersOutDevicePara->shield.b.b0);
	DisplayCommIconSend(PAGE3_OPENPHASEALARM_SHOW , PTowersOutDevicePara->shield.b.b0);
	OSTimeDly(3);
	DisplayCommNumSend(PAGE3_STATETIME      , PTowersOutDevicePara->stateTime);
	DisplayCommNumSend(PAGE3_MOTORMINTIME   , PTowersOutDevicePara->motorMintime);
	DisplayCommNumSend(PAGE3_OVERCURMAX     , PTowersOutDevicePara->overCurMax);
	DisplayCommNumSend(PAGE3_MISSPHASEMIN   , PTowersOutDevicePara->missPhaseMin);
	DisplayCommNumSend(PAGE3_CURALARMFILTIME, PTowersOutDevicePara->curAlarmFilTime);
	DisplayCommNumSend(PAGE3_CURCALIB       , PTowersOutDevicePara->curCalib);
	DisplayCommNumSend(PAGE3_MOTORONFILTIME , PTowersOutDevicePara->motorOnFilTime);
	OSTimeDly(3);
	DisplayCommNumSend(PAGE11_TYPE1      , gPage11DevicePara.place.type);
	DisplayCommNumSend(PAGE11_AREA1      , gPage11DevicePara.place.area);
	DisplayCommNumSend(PAGE11_MAIN_LINE1 , gPage11DevicePara.place.mainLine);
	DisplayCommNumSend(PAGE11_VICE_LINE1 , gPage11DevicePara.place.viceLine);
	tempID = gPage11DevicePara.id;
	sprintf(str, "0x%08X", tempID);
	DisplayCommTextSend(PAGE11_ID1,	(u8*)str, sizeof(str));
	OSTimeDly(3);
}

