/*****************************************************************
 * File: Page5Revise.c
 * Date: 2019/11/27 14:49
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page5Revise.h"

static DevicePara gPage5NewDevicePara, gPage5LocalDevicePara;
static DevicePara *pPage5OldDevicePara;
//static u8 gPage5Seq; // ÐòºÅ£¬ÀÛ¼Ó

void Page5ReviseSend(void)
{
    u8 index = 0;
    WirlessParaGet()->cmd = 0xF6;
    WirlessParaGet()->buffer[index++] = gPage5NewDevicePara.id>>24;
    WirlessParaGet()->buffer[index++] = gPage5NewDevicePara.id>>16;
    WirlessParaGet()->buffer[index++] = gPage5NewDevicePara.id>>8;
    WirlessParaGet()->buffer[index++] = gPage5NewDevicePara.id&0xFF;
    WirlessParaGet()->len = index;
    
    ParaSettingSendData(gPage5LocalDevicePara.id, pPage5OldDevicePara->id);
}


void Page5ReviseInit(void)
{
    pPage5OldDevicePara = RcDeviceParaGet();
    pPage5OldDevicePara->place.device = FOODLINE_STATE;
    gPage5NewDevicePara.place.device = FOODLINE_STATE;
    gPage5LocalDevicePara.id = LocalDeviceIdGet();
}

void Page5ReviseProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{
    u32 data;
	if(len == 4)
		data = pbuf[0] << 24 | pbuf[1] << 16 | pbuf[2] << 8 | pbuf[3];
	else
		data = pbuf[0] << 8 | pbuf[1];

    if(addr == PAGE5_REVISE_OLD_TYPE)
    {
        pPage5OldDevicePara->place.type = data;
    }
    if(addr == PAGE5_REVISE_OLD_AREA)
    {
        pPage5OldDevicePara->place.area = data;
    }
    if(addr == PAGE5_REVISE_OLD_MAIN_LINE)
    {
        pPage5OldDevicePara->place.mainLine = data;
    }
    if(addr == PAGE5_REVISE_OLD_VICE_LINE)
    {
        pPage5OldDevicePara->place.viceLine = data;
    }
    
    if(addr == PAGE5_REVISE_NEW_TYPE)
    {
        gPage5NewDevicePara.place.type = data;
    }
    if(addr == PAGE5_REVISE_NEW_AREA)
    {
        gPage5NewDevicePara.place.area = data;
    }
    if(addr == PAGE5_REVISE_NEW_MAIN_LINE)
    {
        gPage5NewDevicePara.place.mainLine = data;
    }
    if(addr == PAGE5_REVISE_NEW_VICE_LINE)
    {
        gPage5NewDevicePara.place.viceLine = data;
    }
    
    if(addr == PAGE5_REVISE_LOCAL_TYPE)
    {
        gPage5LocalDevicePara.place.type = data;
    }
    if(addr == PAGE5_REVISE_LOCAL_AREA)
    {
        gPage5LocalDevicePara.place.area = data;
    }
    if(addr == PAGE5_REVISE_LOCAL_MAIN_LINE)
    {
        gPage5LocalDevicePara.place.mainLine = data;
    }
    if(addr == PAGE5_REVISE_LOCAL_VICE_LINE)
    {
        gPage5LocalDevicePara.place.viceLine = data;
    }

    if(addr == PAGE5_REVISE_SEND)
    {
        Page5ReviseSend();
    }
    if(addr == PAGE5_REVISE_RENAME)
    {
        LocalDeviceIdSet(gPage5LocalDevicePara.id);
    }

}

void Page5ReviseRefresh(void)
{
	char str[12];
	u32 tempID;
    
    if(pPage5OldDevicePara->id != RcDeviceParaGet()->id)
    {
        pPage5OldDevicePara->id = RcDeviceParaGet()->id;
    }
	DisplayCommNumSend(PAGE5_REVISE_OLD_TYPE      , pPage5OldDevicePara->place.type);
	DisplayCommNumSend(PAGE5_REVISE_OLD_AREA      , pPage5OldDevicePara->place.area);
	DisplayCommNumSend(PAGE5_REVISE_OLD_MAIN_LINE , pPage5OldDevicePara->place.mainLine);
	DisplayCommNumSend(PAGE5_REVISE_OLD_VICE_LINE , pPage5OldDevicePara->place.viceLine);
    tempID = pPage5OldDevicePara->id;
    sprintf(str, "0x%08X", tempID);
    DisplayCommTextSend(PAGE5_REVISE_OLD_ID,	(u8*)str, sizeof(str));
	OSTimeDly(3);
	DisplayCommNumSend(PAGE5_REVISE_NEW_TYPE      , gPage5NewDevicePara.place.type);
	DisplayCommNumSend(PAGE5_REVISE_NEW_AREA      , gPage5NewDevicePara.place.area);
	DisplayCommNumSend(PAGE5_REVISE_NEW_MAIN_LINE , gPage5NewDevicePara.place.mainLine);
	DisplayCommNumSend(PAGE5_REVISE_NEW_VICE_LINE , gPage5NewDevicePara.place.viceLine);
    tempID = gPage5NewDevicePara.id;
    sprintf(str, "0x%08X", tempID);
    DisplayCommTextSend(PAGE5_REVISE_NEW_ID,	(u8*)str, sizeof(str));
	OSTimeDly(3);
	DisplayCommNumSend(PAGE5_REVISE_LOCAL_TYPE      , gPage5LocalDevicePara.place.type);
	DisplayCommNumSend(PAGE5_REVISE_LOCAL_AREA      , gPage5LocalDevicePara.place.area);
	DisplayCommNumSend(PAGE5_REVISE_LOCAL_MAIN_LINE , gPage5LocalDevicePara.place.mainLine);
	DisplayCommNumSend(PAGE5_REVISE_LOCAL_VICE_LINE , gPage5LocalDevicePara.place.viceLine);
    tempID = gPage5LocalDevicePara.id;
    sprintf(str, "0x%08X", tempID);
    DisplayCommTextSend(PAGE5_REVISE_LOCAL_ID,	(u8*)str, sizeof(str));
	OSTimeDly(3);
}

