/*****************************************************************
 * File: Page6ReadPathConfig.c
 * Date: 2019/11/28 11:34
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page6ReadPathConfig.h"
static u8 gP6Table;
//static u8 gP6Index;
static u8 gPage6Seq; // ÐòºÅ£¬ÀÛ¼Ó

void Page6PathConfigTurn(u8 table);
void Page6PathConfigTouchRefresh(void);

void Page6PathConfigInit(void)
{
	ScrInfGet()->currentTable = gP6Table;
    Page6PathConfigTurn(gP6Table);
    DisplayCommNumSend(PAGE6_PATH_CURRENT_PAGE, gP6Table+1);
    DisplayCommNumSend(PAGE6_PATH_TOTAL_PAGES, PAGE6_DEVICE_PAGES);
    ReadPathGet()->readDevice.place.device = 0x3200;
    ParaRefreshGet()->count = 1;
}

void Page6PathSendPath(void)
{
    u8 index = 0;
    u8 path;
    path = PathTrav(ReadPathGet()->readDevice.id);
    if(path == PATH_NUMBERS)
        return;
    WirlessParaGet()->buffer[index++] = (LocalDeviceIdGet()>>8)&0xFF;
    WirlessParaGet()->buffer[index++] = LocalDeviceIdGet()&0xFF;
    WirlessParaGet()->buffer[index++] = (ReadPathGet()->readDevice.id>>8)&0xFF; 
    WirlessParaGet()->buffer[index++] = ReadPathGet()->readDevice.id&0xFF;
    WirlessParaGet()->buffer[index++] = gPage6Seq++;
    WirlessParaGet()->buffer[index++] = ReadPathGet()->path;
    WirlessParaGet()->buffer[index++] = 0x33;
    
    WirlessParaGet()->cmd = 0x30;
    WirlessParaGet()->len = index;
    ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id);
}

void Page6PathConfigTurn(u8 table)
{
	switch(table)
	{
	case 0:
 		DisplayCommIconSend(PAGE6_PATH_LEFT_BUTTON,  ICO_HIDE);
 		DisplayCommIconSend(PAGE6_PATH_RIGHT_BUTTON, ICO_SHOW);
 		break;
 	case 4:
 		DisplayCommIconSend(PAGE6_PATH_LEFT_BUTTON,  ICO_SHOW);
 		DisplayCommIconSend(PAGE6_PATH_RIGHT_BUTTON, ICO_HIDE);
 		break;
 	default:
 		DisplayCommIconSend(PAGE6_PATH_LEFT_BUTTON,  ICO_SHOW);
 		DisplayCommIconSend(PAGE6_PATH_RIGHT_BUTTON, ICO_SHOW);
		break;
	}
    DisplayCommNumSend(PAGE6_PATH_CURRENT_PAGE, gP6Table+1);
}

void Page6PathConfigProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{
	u32 data;
	if(len == 4)
		data = pbuf[0] << 24 | pbuf[1] << 16 | pbuf[2] << 8 | pbuf[3];
	else
		data = pbuf[0] << 8 | pbuf[1];

//    gP6Index = ReadPathGet()->path;
	// Êý¾ÝÂ¼Èë
    if(addr == PAGE6_READ_TYPE1)
    {
        ReadPathGet()->readDevice.place.type = data;
    }
    if(addr == PAGE6_READ_AREA1)
    {
        ReadPathGet()->readDevice.place.area = data;
    }
    if(addr == PAGE6_READ_MAIN_LINE1)
    {
        ReadPathGet()->readDevice.place.mainLine = data;
    }
    if(addr == PAGE6_READ_VICE_LINE1)
    {
        ReadPathGet()->readDevice.place.viceLine = data;
    }
    if(addr == PAGE6_PATH_INDEX)
    {
        if(ReadPathGet()->path != data)
        {
            ReadPathGet()->path = data;
        }
    }
	// ·­Ò³ÉèÖÃ
	if(addr == PAGE6_PATH_LEFT_BUTTON && gP6Table > 0)// ×ó
	{
		gP6Table--;
		Page6PathConfigTurn(gP6Table);
	}
	else if(addr == PAGE6_PATH_RIGHT_BUTTON && gP6Table < 4)// ÓÒ
	{
		gP6Table++;
		Page6PathConfigTurn(gP6Table);
	}
    else if(addr == PAGE6_PATH_LEFT_BUTTON || addr == PAGE6_PATH_RIGHT_BUTTON)
    {
		Page6PathConfigTurn(gP6Table);
    }
    // °´Å¥
    if(addr == PAGE6_PATH_READ)
    {
        memset(&ReadPathGet()->rcPara[0], 0, PATH_PER_INDEX*sizeof(DevicePara));
        Page6PathSendPath();
    }

    ParaRefreshGet()->count = 1;
}

void Page6PathConfigRefresh(void)
{
	char str[12];
	u32 tempID, i;
//    if(ParaRefreshGet()->count == 0)
//    {
//        return;
//    }
//    else
//    {
//        ParaRefreshGet()->count--;
//    }
    
    for(i = 0; i < 8; i++)
    {
        DisplayCommIconSend(PAGE6_PATH_TYPE1+i*0x10, ReadPathGet()->rcPara[i+gP6Table*PAGE6_DEVICE_PER].place.type);
        DisplayCommIconSend(PAGE6_PATH_AREA1+i*0x10, ReadPathGet()->rcPara[i+gP6Table*PAGE6_DEVICE_PER].place.area);
        DisplayCommNumSend(PAGE6_PATH_MAIN_LINE1+i*0x10, ReadPathGet()->rcPara[i+gP6Table*PAGE6_DEVICE_PER].place.mainLine);
        DisplayCommNumSend(PAGE6_PATH_VICE_LINE1+i*0x10, ReadPathGet()->rcPara[i+gP6Table*PAGE6_DEVICE_PER].place.viceLine);
    	tempID = ReadPathGet()->rcPara[i+gP6Table*PAGE6_DEVICE_PER].id;
    	sprintf(str, "0x%08X", tempID);
    	DisplayCommTextSend(PAGE6_PATH_ID1+i*8,	(u8*)str, sizeof(str));
	    OSTimeDly(3);
    }
    DisplayCommIconSend(PAGE6_READ_TYPE1, ReadPathGet()->readDevice.place.type);
    DisplayCommIconSend(PAGE6_READ_AREA1, ReadPathGet()->readDevice.place.area);
    DisplayCommNumSend(PAGE6_READ_MAIN_LINE1, ReadPathGet()->readDevice.place.mainLine);
    DisplayCommNumSend(PAGE6_READ_VICE_LINE1, ReadPathGet()->readDevice.place.viceLine);
	tempID = ReadPathGet()->readDevice.id;
	sprintf(str, "0x%08X", tempID);
	DisplayCommTextSend(PAGE6_READ_ID1,	(u8*)str, sizeof(str));
    OSTimeDly(3);
    DisplayCommNumSend(PAGE6_PATH_INDEX, ReadPathGet()->path);
}



