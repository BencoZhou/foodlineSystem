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
static TowersOutDevicePara *pTowersOutDevicePara;

static u8 gPage11Seq; // ÐòºÅ£¬ÀÛ¼Ó

void Page11TowersOutConfigInquiry(void)
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

void Page11TowersOutConfigSend(void)
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
    WirlessParaGet()->buffer[index++] = pTowersOutDevicePara->shield.buf;
    WirlessParaGet()->buffer[index++] = pTowersOutDevicePara->stateTime;
    WirlessParaGet()->buffer[index++] = pTowersOutDevicePara->motorMintime;
    WirlessParaGet()->buffer[index++] = (pTowersOutDevicePara->overCurMax>>8);
    WirlessParaGet()->buffer[index++] = pTowersOutDevicePara->overCurMax;
    WirlessParaGet()->buffer[index++] = (pTowersOutDevicePara->missPhaseMin>>8);
    WirlessParaGet()->buffer[index++] = pTowersOutDevicePara->missPhaseMin;
    WirlessParaGet()->buffer[index++] = pTowersOutDevicePara->curAlarmFilTime;
    WirlessParaGet()->buffer[index++] = (pTowersOutDevicePara->curCalib>>8);
    WirlessParaGet()->buffer[index++] = pTowersOutDevicePara->curCalib;
    WirlessParaGet()->buffer[index++] = pTowersOutDevicePara->motorOnFilTime;
    WirlessParaGet()->buffer[index++] = pTowersOutDevicePara->feedFilTime;
    WirlessParaGet()->buffer[index++] = pTowersOutDevicePara->motorState;
    
    WirlessParaGet()->cmd = 0x30;
    WirlessParaGet()->len = index;
    
    ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id);
}


void Page11TowersOutConfigInit(void)
{
    gPage11DevicePara.place.type = DEVICE_NAME_TOWERS_OUT;    
    gPage11DevicePara.place.device = FOODLINE_STATE;
    pTowersOutDevicePara = TowersOutDeviceParaGet();
    Page11TowersOutConfigRefresh();
}

void Page11TowersOutConfigProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{
    u32 data;
	if(len == 4)
		data = pbuf[0] << 24 | pbuf[1] << 16 | pbuf[2] << 8 | pbuf[3];
	else
		data = pbuf[0] << 8 | pbuf[1];

    if(addr == PAGE11_INQUIRY)
    {
        Page11TowersOutConfigInquiry();
    }
    if(addr == PAGE11_SEND)
    {
        Page11TowersOutConfigSend();
    }
    
    if(addr == PAGE11_ALARM_FORWARD_MAX_BUTTON)
    {
        if(pTowersOutDevicePara->shield.b.b0 == 1)
            pTowersOutDevicePara->shield.b.b0 = 0;
        else
            pTowersOutDevicePara->shield.b.b0 = 1;
    }
    if(addr == PAGE11_ALARM_FORWARD_NONE_BUTTON)
    {
        if(pTowersOutDevicePara->shield.b.b1 == 1)
            pTowersOutDevicePara->shield.b.b1 = 0;
        else
            pTowersOutDevicePara->shield.b.b1 = 1;
    }
    if(addr == PAGE11_ALARM_REVERSE_MAX_BUTTON)
    {
        if(pTowersOutDevicePara->shield.b.b2 == 1)
            pTowersOutDevicePara->shield.b.b2 = 0;
        else
            pTowersOutDevicePara->shield.b.b2 = 1;
    }
    if(addr == PAGE11_ALARM_REVERSE_NONE_BUTTON)
    {
        if(pTowersOutDevicePara->shield.b.b3 == 1)
            pTowersOutDevicePara->shield.b.b3 = 0;
        else
            pTowersOutDevicePara->shield.b.b3 = 1;
    }
    if(addr == PAGE11_ALARM_OVERCURMAX_BUTTON)
    {
        if(pTowersOutDevicePara->shield.b.b4 == 1)
            pTowersOutDevicePara->shield.b.b4 = 0;
        else
            pTowersOutDevicePara->shield.b.b4 = 1;
    }
    if(addr == PAGE11_ALARM_MISSPHASEMIN_BUTTON)
    {
        if(pTowersOutDevicePara->shield.b.b5 == 1)
            pTowersOutDevicePara->shield.b.b5 = 0;
        else
            pTowersOutDevicePara->shield.b.b5 = 1;
    }

    if(addr == PAGE11_STATETIME)
    {
        pTowersOutDevicePara->stateTime = data;
    }
    if(addr == PAGE11_MOTORMINTIME)
    {
        pTowersOutDevicePara->motorMintime = data;
    }
    if(addr == PAGE11_OVERCURMAX)
    {
        pTowersOutDevicePara->overCurMax = data;
    }
    if(addr == PAGE11_MISSPHASEMIN)
    {
        pTowersOutDevicePara->missPhaseMin = data;
    }
    if(addr == PAGE11_CURALARMFILTIME)
    {
        pTowersOutDevicePara->curAlarmFilTime = data;
    }
    if(addr == PAGE11_CURCALIB)
    {
        pTowersOutDevicePara->curCalib = data;
    }
    if(addr == PAGE11_MOTORONFILTIME)
    {
        pTowersOutDevicePara->motorOnFilTime = data;
    }
    if(addr == PAGE11_FEEDFILTIME)
    {
        pTowersOutDevicePara->feedFilTime = data;
    }
    if(addr == PAGE11_MOTOR_STATE)
    {
        pTowersOutDevicePara->motorState = data;
    }   
    
    if(addr == PAGE11_TYPE1)
    {
        gPage11DevicePara.place.type = data;
    }
    if(addr == PAGE11_AREA1)
    {
        gPage11DevicePara.place.area = data;
    }
    if(addr == PAGE11_MAIN_LINE1)
    {
        gPage11DevicePara.place.mainLine = data;
    }
    if(addr == PAGE11_VICE_LINE1)
    {
        gPage11DevicePara.place.viceLine = data;
    }


    Page11TowersOutConfigRefresh();    
}
 

void Page11TowersOutConfigRefresh(void)
{
	char str[12];
	u32 tempID;
	DisplayCommIconSend(PAGE11_ALARM_FORWARD_MAX_SHOW , pTowersOutDevicePara->shield.b.b0);
	DisplayCommIconSend(PAGE11_ALARM_FORWARD_NONE_SHOW, pTowersOutDevicePara->shield.b.b1);
	DisplayCommIconSend(PAGE11_ALARM_REVERSE_MAX_SHOW , pTowersOutDevicePara->shield.b.b2);
	DisplayCommIconSend(PAGE11_ALARM_REVERSE_NONE_SHOW, pTowersOutDevicePara->shield.b.b3);
	DisplayCommIconSend(PAGE11_ALARM_OVERCURMAX_SHOW  , pTowersOutDevicePara->shield.b.b4);
	DisplayCommIconSend(PAGE11_ALARM_MISSPHASEMIN_SHOW, pTowersOutDevicePara->shield.b.b5);
	OSTimeDly(3);
	DisplayCommNumSend(PAGE11_STATETIME       , pTowersOutDevicePara->stateTime);
	DisplayCommNumSend(PAGE11_MOTORMINTIME    , pTowersOutDevicePara->motorMintime);
	DisplayCommNumSend(PAGE11_OVERCURMAX      , pTowersOutDevicePara->overCurMax);
	DisplayCommNumSend(PAGE11_MISSPHASEMIN    , pTowersOutDevicePara->missPhaseMin);
	DisplayCommNumSend(PAGE11_CURALARMFILTIME , pTowersOutDevicePara->curAlarmFilTime);
	DisplayCommNumSend(PAGE11_CURCALIB        , pTowersOutDevicePara->curCalib);
	DisplayCommNumSend(PAGE11_MOTORONFILTIME  , pTowersOutDevicePara->motorOnFilTime);
	DisplayCommNumSend(PAGE11_FEEDFILTIME     , pTowersOutDevicePara->feedFilTime);
	DisplayCommNumSend(PAGE11_MOTOR_STATE     , pTowersOutDevicePara->motorState);
	OSTimeDly(3);
	DisplayCommNumSend(PAGE11_TYPE1      , gPage11DevicePara.place.type);
	DisplayCommNumSend(PAGE11_AREA1      , gPage11DevicePara.place.area);
	DisplayCommNumSend(PAGE11_MAIN_LINE1 , gPage11DevicePara.place.mainLine);
	DisplayCommNumSend(PAGE11_VICE_LINE1 , gPage11DevicePara.place.viceLine);
	tempID = gPage11DevicePara.id;
	sprintf(str, "0x%08X", tempID);
	DisplayCommTextSend(PAGE11_ID1,	(u8*)str, sizeof(str));
	OSTimeDly(3);
    PageXIdSet(gPage11DevicePara);
}

