/*****************************************************************
 * File: Page11TowersOutConfig.c
 * Date: 2019/11/29 11:59
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page11TowersOutConfig.h"

static DevicePara gPage3DevicePara;
static FoodlineDevicePara *pFoodlineDevicePara;

extern u32 gLocalDeviceId;  // 临时使用，需要删除
//static u32 gDestDeviceID;
static u8 gPage3Seq; // 序号，累加


//void Page3IDConvert(void)
//{
//    gPage3Device.id &= 0xFFFF0000;
//    gPage3Device.id |= (gPage3Device.type << 13);
//    gPage3Device.id |= (gPage3Device.area << 10);
//    gPage3Device.id |= (gPage3Device.mainLine << 7);
//    gPage3Device.id |= (gPage3Device.viceLine & 0x7F);
//    gPage3Device.id |= 0x32000000;
//}

void Page3FoodlineDeviceInquiry(void)
{
    u8 index = 0;
	u8 path;
	path = PathTrav(gPage3DevicePara.id);
	if(path == PATH_NUMBERS)
		return;
    WirlessParaGet()->buffer[index++] = (LocalDeviceIdGet()>>8)&0xFF;
    WirlessParaGet()->buffer[index++] = LocalDeviceIdGet()&0xFF;
    WirlessParaGet()->buffer[index++] = (gPage3DevicePara.id>>8)&0xFF;
    WirlessParaGet()->buffer[index++] = gPage3DevicePara.id&0xFF;
    WirlessParaGet()->buffer[index++] = gPage3Seq++;
    WirlessParaGet()->buffer[index++] = path;
    WirlessParaGet()->buffer[index++] = 0x39;
    WirlessParaGet()->cmd = 0x30;
    WirlessParaGet()->len = index;
    
    ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id);
}

void Page3FoodlineDeviceSend(void)
{
    u8 index = 0;
	u8 path;
	path = PathTrav(gPage3DevicePara.id);
	if(path == PATH_NUMBERS)
		return;
    WirlessParaGet()->buffer[index++] = (LocalDeviceIdGet()>>8)&0xFF;
    WirlessParaGet()->buffer[index++] = LocalDeviceIdGet()&0xFF;
    WirlessParaGet()->buffer[index++] = (gPage3DevicePara.id>>8)&0xFF;
    WirlessParaGet()->buffer[index++] = gPage3DevicePara.id&0xFF;
    WirlessParaGet()->buffer[index++] = gPage3Seq++;
    WirlessParaGet()->buffer[index++] = path;
    WirlessParaGet()->buffer[index++] = 0x39;
    WirlessParaGet()->buffer[index++] = pFoodlineDevicePara->shield.buf;
    WirlessParaGet()->buffer[index++] = pFoodlineDevicePara->stateTime;
    WirlessParaGet()->buffer[index++] = pFoodlineDevicePara->motorMintime;
    WirlessParaGet()->buffer[index++] = (pFoodlineDevicePara->overCurMax>>8);
    WirlessParaGet()->buffer[index++] = pFoodlineDevicePara->overCurMax;
    WirlessParaGet()->buffer[index++] = (pFoodlineDevicePara->missPhaseMin>>8);
    WirlessParaGet()->buffer[index++] = pFoodlineDevicePara->missPhaseMin;
    WirlessParaGet()->buffer[index++] = pFoodlineDevicePara->curAlarmFilTime;
    WirlessParaGet()->buffer[index++] = (pFoodlineDevicePara->curCalib>>8);
    WirlessParaGet()->buffer[index++] = pFoodlineDevicePara->curCalib;
    WirlessParaGet()->buffer[index++] = pFoodlineDevicePara->motorOnFilTime;
    WirlessParaGet()->buffer[index++] = pFoodlineDevicePara->maleDevice.buf;
    WirlessParaGet()->buffer[index++] = pFoodlineDevicePara->commSum;
    
    WirlessParaGet()->cmd = 0x30;
    WirlessParaGet()->len = index;
    
    ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id);
}


void Page3FlineDeviceInit(void)
{
    gPage3DevicePara.place.device = FOODLINE_STATE;
    pFoodlineDevicePara = FoodlineDeviceParaGet();
    Page3FlineDeviceRefresh();
}

void Page3FlineDeviceProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{
    u32 data;
	if(len == 4)
		data = pbuf[0] << 24 | pbuf[1] << 16 | pbuf[2] << 8 | pbuf[3];
	else
		data = pbuf[0] << 8 | pbuf[1];

    if(addr == PAGE3_INQUIRY)
    {
        Page3FoodlineDeviceInquiry();
    }
    if(addr == PAGE3_SEND)
    {
        Page3FoodlineDeviceSend();
    }
    if(addr == PAGE3_OPENALARM_BUTTON)
    {
        if(pFoodlineDevicePara->shield.alarm.openAlarm == 1)
            pFoodlineDevicePara->shield.alarm.openAlarm = 0;
        else
            pFoodlineDevicePara->shield.alarm.openAlarm = 1;
    }
    if(addr == PAGE3_REGULATINGALARM_BUTTON)
    {
        if(pFoodlineDevicePara->shield.alarm.regulatingAlarm == 1)
            pFoodlineDevicePara->shield.alarm.regulatingAlarm = 0;
        else
            pFoodlineDevicePara->shield.alarm.regulatingAlarm = 1;
    }
    if(addr == PAGE3_CURRENTALARM_BUTTON)
    {
        if(pFoodlineDevicePara->shield.alarm.currentAlarm == 1)
            pFoodlineDevicePara->shield.alarm.currentAlarm = 0;
        else
            pFoodlineDevicePara->shield.alarm.currentAlarm = 1;
    }
    if(addr == PAGE3_OPENPHASEALARM_BUTTON)
    {
        if(pFoodlineDevicePara->shield.alarm.openphaseAlarm == 1)
            pFoodlineDevicePara->shield.alarm.openphaseAlarm = 0;
        else
            pFoodlineDevicePara->shield.alarm.openphaseAlarm = 1;
    }

    if(addr == PAGE3_STATETIME)
    {
        pFoodlineDevicePara->stateTime = data;
    }
    if(addr == PAGE3_MOTORMINTIME)
    {
        pFoodlineDevicePara->motorMintime = data;
    }
    if(addr == PAGE3_OVERCURMAX)
    {
        pFoodlineDevicePara->overCurMax = data;
    }
    if(addr == PAGE3_MISSPHASEMIN)
    {
        pFoodlineDevicePara->missPhaseMin = data;
    }
    if(addr == PAGE3_CURALARMFILTIME)
    {
        pFoodlineDevicePara->curAlarmFilTime = data;
    }
    if(addr == PAGE3_CURCALIB)
    {
        pFoodlineDevicePara->curCalib = data;
    }
    if(addr == PAGE3_MOTORONFILTIME)
    {
        pFoodlineDevicePara->motorOnFilTime = data;
    }
 
    
    if(addr == PAGE3_CONTYPE_BUTTON)
    {
        if(pFoodlineDevicePara->maleDevice.device.conType == 1)
            pFoodlineDevicePara->maleDevice.device.conType = 0;
        else
            pFoodlineDevicePara->maleDevice.device.conType = 1;
    }
    if(addr == PAGE3_NUMBERTHREE_BUTTON)
    {
        if(pFoodlineDevicePara->maleDevice.device.numberThree == 1)
            pFoodlineDevicePara->maleDevice.device.numberThree = 0;
        else
            pFoodlineDevicePara->maleDevice.device.numberThree = 1;
    }
    if(addr == PAGE3_NUMBERTWO_BUTTON)
    {
        if(pFoodlineDevicePara->maleDevice.device.numberTwo == 1)
            pFoodlineDevicePara->maleDevice.device.numberTwo = 0;
        else
            pFoodlineDevicePara->maleDevice.device.numberTwo = 1;
    }
    if(addr == PAGE3_NUMBERONE_BUTTON)
    {
        if(pFoodlineDevicePara->maleDevice.device.numberOne == 1)
            pFoodlineDevicePara->maleDevice.device.numberOne = 0;
        else
            pFoodlineDevicePara->maleDevice.device.numberOne = 1;
    } 
    
    if(addr == PAGE3_TYPE1)
    {
        gPage3DevicePara.place.type = data;
    }
    if(addr == PAGE3_AREA1)
    {
        gPage3DevicePara.place.area = data;
    }
    if(addr == PAGE3_MAIN_LINE1)
    {
        gPage3DevicePara.place.mainLine = data;
    }
    if(addr == PAGE3_VICE_LINE1)
    {
        gPage3DevicePara.place.viceLine = data;
    }
    if(addr == PAGE3_COMMSUM)
    {
        pFoodlineDevicePara->commSum = data;
    }


    Page3FlineDeviceRefresh();

    
}
 

void Page3FlineDeviceRefresh(void)
{
	char str[12];
	u32 tempID;
	DisplayCommIconSend(PAGE3_OPENALARM_SHOW      , pFoodlineDevicePara->shield.alarm.openAlarm);
	DisplayCommIconSend(PAGE3_REGULATINGALARM_SHOW, pFoodlineDevicePara->shield.alarm.regulatingAlarm);
	DisplayCommIconSend(PAGE3_CURRENTALARM_SHOW   , pFoodlineDevicePara->shield.alarm.currentAlarm);
	DisplayCommIconSend(PAGE3_OPENPHASEALARM_SHOW , pFoodlineDevicePara->shield.alarm.openphaseAlarm);
	DisplayCommNumSend(PAGE3_COMMSUM      , pFoodlineDevicePara->commSum);
	OSTimeDly(3);
	DisplayCommNumSend(PAGE3_STATETIME      , pFoodlineDevicePara->stateTime);
	DisplayCommNumSend(PAGE3_MOTORMINTIME   , pFoodlineDevicePara->motorMintime);
	DisplayCommNumSend(PAGE3_OVERCURMAX     , pFoodlineDevicePara->overCurMax);
	DisplayCommNumSend(PAGE3_MISSPHASEMIN   , pFoodlineDevicePara->missPhaseMin);
	DisplayCommNumSend(PAGE3_CURALARMFILTIME, pFoodlineDevicePara->curAlarmFilTime);
	DisplayCommNumSend(PAGE3_CURCALIB       , pFoodlineDevicePara->curCalib);
	DisplayCommNumSend(PAGE3_MOTORONFILTIME , pFoodlineDevicePara->motorOnFilTime);
	OSTimeDly(3);
	DisplayCommNumSend(PAGE3_TYPE1      , gPage3DevicePara.place.type);
	DisplayCommNumSend(PAGE3_AREA1      , gPage3DevicePara.place.area);
	DisplayCommNumSend(PAGE3_MAIN_LINE1 , gPage3DevicePara.place.mainLine);
	DisplayCommNumSend(PAGE3_VICE_LINE1 , gPage3DevicePara.place.viceLine);
	DisplayCommIconSend(PAGE3_CONTYPE_SHOW    , pFoodlineDevicePara->maleDevice.device.conType);
	DisplayCommIconSend(PAGE3_NUMBERTHREE_SHOW, pFoodlineDevicePara->maleDevice.device.numberThree);
	DisplayCommIconSend(PAGE3_NUMBERTWO_SHOW  , pFoodlineDevicePara->maleDevice.device.numberTwo);
	DisplayCommIconSend(PAGE3_NUMBERONE_SHOW  , pFoodlineDevicePara->maleDevice.device.numberOne);
	tempID = gPage3DevicePara.id;
	sprintf(str, "0x%08X", tempID);
	DisplayCommTextSend(PAGE3_ID1,	(u8*)str, sizeof(str));
	OSTimeDly(3);
}

