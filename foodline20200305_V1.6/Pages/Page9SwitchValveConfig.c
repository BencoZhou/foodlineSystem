/*****************************************************************
 * File: Page9SwitchValveConfig.c
 * Date: 2019/11/29 9:04
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page9SwitchValveConfig.h"

static DevicePara gPage9DevicePara;
static SwitchValvePara *pSwitchValvePara;

//static u32 gDestDeviceID;
static u8 gPage9Seq; // 序号，累加


void Page9SwitchValveConfigInquiry(void)
{
	WirlessPara gWirlessParaP9;
    u8 index = 0;
	u8 path;
	path = PathTrav(gPage9DevicePara.id);
	if(path == PATH_NUMBERS)
		return;
    gWirlessParaP9.buffer[index++] = (LocalDeviceIdGet()>>8)&0xFF;
    gWirlessParaP9.buffer[index++] = LocalDeviceIdGet()&0xFF;
    gWirlessParaP9.buffer[index++] = (gPage9DevicePara.id>>8)&0xFF;
    gWirlessParaP9.buffer[index++] = gPage9DevicePara.id&0xFF;
    gWirlessParaP9.buffer[index++] = gPage9Seq++;
    gWirlessParaP9.buffer[index++] = path;
    gWirlessParaP9.buffer[index++] = 0x39;
    gWirlessParaP9.cmd = 0x30;
    gWirlessParaP9.len = index;
	gWirlessParaP9.buffer[5] |= 0x80;
    WirelessApp_SendData(gWirlessParaP9.cmd, FRAME_NEED_NO_ACK, gWirlessParaP9.buffer, gWirlessParaP9.len, LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id, PathParameterGet()->dPara[path][0].id, 0);
	    
//    ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id);
}

void Page9SwitchValveConfigSend(void)
{
	WirlessPara gWirlessParaP9;
    u8 index = 0;
	u8 path;
	path = PathTrav(gPage9DevicePara.id);
	if(path == PATH_NUMBERS)
		return;
    gWirlessParaP9.buffer[index++] = (LocalDeviceIdGet()>>8)&0xFF;
    gWirlessParaP9.buffer[index++] = LocalDeviceIdGet()&0xFF;
    gWirlessParaP9.buffer[index++] = (gPage9DevicePara.id>>8)&0xFF;
    gWirlessParaP9.buffer[index++] = gPage9DevicePara.id&0xFF;
    gWirlessParaP9.buffer[index++] = gPage9Seq++;
    gWirlessParaP9.buffer[index++] = path;
    gWirlessParaP9.buffer[index++] = 0x39;
    gWirlessParaP9.buffer[index++] = SwitchValveParaGet()->alarm.buf;
    gWirlessParaP9.buffer[index++] = SwitchValveParaGet()->stateTime;
    gWirlessParaP9.buffer[index++] = (SwitchValveParaGet()->openOvertime>>8)&0xFF;
    gWirlessParaP9.buffer[index++] = SwitchValveParaGet()->openOvertime&0xFF;
    gWirlessParaP9.cmd = 0x30;
    gWirlessParaP9.len = index;
    
	gWirlessParaP9.buffer[5] |= 0x80;
    WirelessApp_SendData(gWirlessParaP9.cmd, FRAME_NEED_NO_ACK, gWirlessParaP9.buffer, gWirlessParaP9.len, LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id, PathParameterGet()->dPara[path][0].id, 0);
	
	// 用局部变量  直接用WirelessApp_SendData(gWirlessPara.cmd, FRAME_NEED_NO_ACK, gWirlessPara.buffer, gWirlessPara.len, srcaddr, dstaddr, dstaddr, 0);
	//发送
//    ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id);
}


void Page9SwitchValveConfigInit(void)
{
    gPage9DevicePara.place.type = DEVICE_NAME_IN_TOWERS;    
    gPage9DevicePara.place.device = FOODLINE_STATE;
    pSwitchValvePara = SwitchValveParaGet();
    Page9SwitchValveConfigRefresh();
}

void Page9SwitchValveConfigProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{
    u32 data;
	if(len == 4)
		data = pbuf[0] << 24 | pbuf[1] << 16 | pbuf[2] << 8 | pbuf[3];
	else
		data = pbuf[0] << 8 | pbuf[1];

    if(addr == PAGE9_INQUIRY)
    {
        Page9SwitchValveConfigInquiry();
    }
    if(addr == PAGE9_SEND)
    {
        Page9SwitchValveConfigSend();
    }    
    if(addr == PAGE9_ALARM_OPEN_BUTTON)
    {
        if(pSwitchValvePara->alarm.b.b0 == 1)
            pSwitchValvePara->alarm.b.b0 = 0;
        else
            pSwitchValvePara->alarm.b.b0 = 1;
    }
    if(addr == PAGE9_ALARM_CLOSE_BUTTON)
    {
        if(pSwitchValvePara->alarm.b.b1 == 1)
            pSwitchValvePara->alarm.b.b1 = 0;
        else
            pSwitchValvePara->alarm.b.b1 = 1;
    }

    if(addr == PAGE9_STATETIME)
    {
        pSwitchValvePara->stateTime = data;
    }
    if(addr == PAGE9_OPEN_OVERTIME)
    {
        pSwitchValvePara->openOvertime = data;
    }
    
    if(addr == PAGE9_TYPE1)
    {
        gPage9DevicePara.place.type = data;
    }
    if(addr == PAGE9_AREA1)
    {
        gPage9DevicePara.place.area = data;
    }
    if(addr == PAGE9_MAIN_LINE1)
    {
        gPage9DevicePara.place.mainLine = data;
    }
    if(addr == PAGE9_VICE_LINE1)
    {
        gPage9DevicePara.place.viceLine = data;
    }
    Page9SwitchValveConfigRefresh();
}
 

void Page9SwitchValveConfigRefresh(void)
{
	char str[12];
	u32 tempID;
	DisplayCommIconSend(PAGE9_ALARM_OPEN_SHOW   , pSwitchValvePara->alarm.b.b0);
	DisplayCommIconSend(PAGE9_ALARM_CLOSE_SHOW  , pSwitchValvePara->alarm.b.b1);
	DisplayCommNumSend(PAGE9_STATETIME          , pSwitchValvePara->stateTime);
	DisplayCommNumSend(PAGE9_OPEN_OVERTIME      , pSwitchValvePara->openOvertime);
	OSTimeDly(3);
	DisplayCommNumSend(PAGE9_TYPE1      , gPage9DevicePara.place.type);
	DisplayCommNumSend(PAGE9_AREA1      , gPage9DevicePara.place.area);
	DisplayCommNumSend(PAGE9_MAIN_LINE1 , gPage9DevicePara.place.mainLine);
	DisplayCommNumSend(PAGE9_VICE_LINE1 , gPage9DevicePara.place.viceLine);
	tempID = gPage9DevicePara.id;
	sprintf(str, "0x%08X", tempID);
	DisplayCommTextSend(PAGE9_ID1,	(u8*)str, sizeof(str));
	OSTimeDly(3);
    PageXIdSet(gPage9DevicePara);
}

