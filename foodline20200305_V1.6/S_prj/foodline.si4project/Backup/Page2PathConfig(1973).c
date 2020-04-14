/*****************************************************************
 * File: Page2PathConfig.c
 * Date: 2019/11/21 14:30
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page2PathConfig.h"
static u8 gP2Table, gIndex;
//static u8 gCountdown;
static u8 gPage2Seq; // ÐòºÅ£¬ÀÛ¼Ó

void Page2PathConfigTurn(u8 table);
void Page2PathConfigTouchRefresh(void);

void Page2PathConfigInit(void)
{
	ScrInfGet()->currentTable = gP2Table;
    Page2PathConfigTurn(gP2Table);
    DisplayCommNumSend(PAGE2_PATH_CURRENT_PAGE, gP2Table+1);
    DisplayCommNumSend(PAGE2_PATH_TOTAL_PAGES, PAGE2_DEVICE_PAGES);
    ParaRefreshGet()->count = 1;
}

void Page2PathSendPath(void)
{
    u8 index = 0, num, i;
    gIndex = PathParameterGet()->index;
    WirlessParaGet()->buffer[index++] = (LocalDeviceIdGet()>>8)&0xFF;
    WirlessParaGet()->buffer[index++] = LocalDeviceIdGet()&0xFF;
    for(num = 0; num < PATH_PER_INDEX; num++)
    {
        if(PathParameterGet()->dPara[gIndex][num].placeNew.useID == 0)
            break;
    }
    WirlessParaGet()->buffer[index++] = (PathParameterGet()->dPara[gIndex][num-1].id>>8)&0xFF; 
    WirlessParaGet()->buffer[index++] = PathParameterGet()->dPara[gIndex][num-1].id&0xFF;
    WirlessParaGet()->buffer[index++] = gPage2Seq++;
    WirlessParaGet()->buffer[index++] = gIndex;
    WirlessParaGet()->buffer[index++] = 0x31;
    WirlessParaGet()->buffer[index++] = LocalDeviceIdGet()&0xFF;
    WirlessParaGet()->buffer[index++] = (LocalDeviceIdGet()>>8)&0xFF;
    for(i = 0; i < num; i++)
    {
        WirlessParaGet()->buffer[index++] = PathParameterGet()->dPara[gIndex][i].id&0xFF;
        WirlessParaGet()->buffer[index++] = (PathParameterGet()->dPara[gIndex][i].id>>8)&0xFF; 
    }
    
    WirlessParaGet()->cmd = 0x30;
    WirlessParaGet()->len = index;
    ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[gIndex][0].id);
}

void Page2PathSavePath(void)
{
    u8 index = 0, num;
    gIndex = PathParameterGet()->index;
    WirlessParaGet()->buffer[index++] = (LocalDeviceIdGet()>>8)&0xFF;
    WirlessParaGet()->buffer[index++] = LocalDeviceIdGet()&0xFF;
    for(num = 0; num < PATH_PER_INDEX; num++)
    {
        if(PathParameterGet()->dPara[gIndex][num].placeNew.useID == 0)
            break;
    }
    WirlessParaGet()->buffer[index++] = (PathParameterGet()->dPara[gIndex][num-1].id>>8)&0xFF; 
    WirlessParaGet()->buffer[index++] = PathParameterGet()->dPara[gIndex][num-1].id&0xFF;
    WirlessParaGet()->buffer[index++] = gPage2Seq++;
    WirlessParaGet()->buffer[index++] = gIndex;
    WirlessParaGet()->buffer[index++] = 0x30;
    WirlessParaGet()->buffer[index++] = 1;
    
    WirlessParaGet()->cmd = 0x30;
    WirlessParaGet()->len = index;
    ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[gIndex][0].id);
}

void Page2PathConfigTurn(u8 table)
{
	switch(table)
	{
	case 0:
 		DisplayCommIconSend(PAGE2_PATH_LEFT_BUTTON,  ICO_HIDE);
 		DisplayCommIconSend(PAGE2_PATH_RIGHT_BUTTON, ICO_SHOW);
 		break;
 	case 4:
 		DisplayCommIconSend(PAGE2_PATH_LEFT_BUTTON,  ICO_SHOW);
 		DisplayCommIconSend(PAGE2_PATH_RIGHT_BUTTON, ICO_HIDE);
 		break;
 	default:
 		DisplayCommIconSend(PAGE2_PATH_LEFT_BUTTON,  ICO_SHOW);
 		DisplayCommIconSend(PAGE2_PATH_RIGHT_BUTTON, ICO_SHOW);
		break;
	}
    DisplayCommNumSend(PAGE2_PATH_CURRENT_PAGE, gP2Table+1);
}

void Page2PathConfigProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{
	u32 tempIndex, data;
    u16 tempAddr;
	if(len == 4)
		data = pbuf[0] << 24 | pbuf[1] << 16 | pbuf[2] << 8 | pbuf[3];
	else
		data = pbuf[0] << 8 | pbuf[1];

    tempIndex = (addr&0x00F0)>>4;
    tempAddr = addr&0xFF0F;
    gIndex = PathParameterGet()->index;
	// Êý¾ÝÂ¼Èë
    if(tempIndex < PAGE2_DEVICE_PER && gP2Table < PAGE2_DEVICE_PAGES)
    {
        if(tempAddr == PAGE2_PATH_TYPE1)
        {
            PathParameterGet()->dPara[gIndex][tempIndex+gP2Table*PAGE2_DEVICE_PER].place.type = data;
        }
        if(tempAddr == PAGE2_PATH_AREA1)
        {
            PathParameterGet()->dPara[gIndex][tempIndex+gP2Table*PAGE2_DEVICE_PER].place.area = data;
        }
        if(tempAddr == PAGE2_PATH_MAIN_LINE1)
        {
            PathParameterGet()->dPara[gIndex][tempIndex+gP2Table*PAGE2_DEVICE_PER].place.mainLine = data;
        }
        if(tempAddr == PAGE2_PATH_VICE_LINE1)
        {
            PathParameterGet()->dPara[gIndex][tempIndex+gP2Table*PAGE2_DEVICE_PER].place.viceLine = data;
        }
    }
    if(addr == PAGE2_PATH_INDEX)
    {
        if(PathParameterGet()->index != data)
        {
            PathParameterGet()->index = data;
        }
    }
	// ·­Ò³ÉèÖÃ
	if(addr == PAGE2_PATH_LEFT_BUTTON && gP2Table > 0)// ×ó
	{
		gP2Table--;
		Page2PathConfigTurn(gP2Table);
	}
	else if(addr == PAGE2_PATH_RIGHT_BUTTON && gP2Table < 4)// ÓÒ
	{
		gP2Table++;
		Page2PathConfigTurn(gP2Table);
	}
    else if(addr == PAGE2_PATH_LEFT_BUTTON || addr == PAGE2_PATH_RIGHT_BUTTON)
    {
		Page2PathConfigTurn(gP2Table);
    }
    // °´Å¥
    if(addr == PAGE2_PATH_SEND)
    {
        Page2PathSendPath();
        PathParameterGet()->alarm.next = 0xAA;
        PathParameterGet()->alarm.path = 0;
        PathParameterGet()->alarm.id = 0;
    }
    if(addr == PAGE2_PATH_SAVE)
    {
        ParaPathParaSave();
        Page2PathSavePath();
    }

    ParaRefreshGet()->count = 1;
}

void Page2PathConfigRefresh(void)
{
	char str[12];
	u32 tempID, i;
    if(ParaRefreshGet()->count == 0)
    {
        return;
    }
    else
    {
        ParaRefreshGet()->count--;
    }
    
    gIndex = PathParameterGet()->index;
    for(i = 0; i < 8; i++)
    {
        DisplayCommIconSend(PAGE2_PATH_TYPE1+i*0x10, PathParameterGet()->dPara[gIndex][i+gP2Table*PAGE2_DEVICE_PER].place.type);
        DisplayCommIconSend(PAGE2_PATH_AREA1+i*0x10, PathParameterGet()->dPara[gIndex][i+gP2Table*PAGE2_DEVICE_PER].place.area);
        DisplayCommNumSend(PAGE2_PATH_MAIN_LINE1+i*0x10, PathParameterGet()->dPara[gIndex][i+gP2Table*PAGE2_DEVICE_PER].place.mainLine);
        DisplayCommNumSend(PAGE2_PATH_VICE_LINE1+i*0x10, PathParameterGet()->dPara[gIndex][i+gP2Table*PAGE2_DEVICE_PER].place.viceLine);
    	tempID = PathParameterGet()->dPara[gIndex][i+gP2Table*PAGE2_DEVICE_PER].id;
    	sprintf(str, "0x%08X", tempID);
    	DisplayCommTextSend(PAGE2_PATH_ID1+i*8,	(u8*)str, sizeof(str));
	    OSTimeDly(3);
    }
    DisplayCommNumSend(PAGE2_PATH_INDEX, gIndex);
    // ´íÎóÂ·¾¶ºÍID
	tempID = PathParameterGet()->alarm.id;
	sprintf(str, "0x%08X", tempID);
	DisplayCommTextSend(PAGE2_PATH_ALARM_ID,	(u8*)str, sizeof(str));
    DisplayCommNumSend(PAGE2_PATH_ALARM_PATH, PathParameterGet()->alarm.path);
}



