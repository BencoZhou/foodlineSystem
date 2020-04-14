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
static u8 gCountdown;

void Page2PathConfigTurn(u8 table);
void Page2PathConfigTouchRefresh(void);

void Page2PathConfigInit(void)
{
	ScrInfGet()->currentTable = gP2Table;
    Page2PathConfigTurn(gP2Table);
    DisplayCommNumSend(PAGE2_PATH_CURRENT_PAGE, gP2Table+1);
    DisplayCommNumSend(PAGE2_PATH_TOTAL_PAGES, PAGE2_DEVICE_PAGES);
}

void Page2PathSendPath(void)
{
//    u8 index = 0;
//    WirlessParaGet()->buffer[index++] = (gflinePara[0].fullID>>8)&0xFF;
//    WirlessParaGet()->buffer[index++] = gflinePara[0].fullID&0xFF;
//    WirlessParaGet()->buffer[index++] = (gflinePara[4].fullID>>8)&0xFF;
//    WirlessParaGet()->buffer[index++] = gflinePara[4].fullID&0xFF;
//    WirlessParaGet()->buffer[index++] = gSeq++;
//    WirlessParaGet()->buffer[index++] = gPath;
//    WirlessParaGet()->buffer[index++] = 0x31;
////    WirlessParaGet()->buffer[index++] = gflinePara[0].fullID&0xFF;
////    WirlessParaGet()->buffer[index++] = (gflinePara[0].fullID>>8)&0xFF;
////    WirlessParaGet()->buffer[index++] = gflinePara[1].fullID&0xFF;
////    WirlessParaGet()->buffer[index++] = (gflinePara[1].fullID>>8)&0xFF;
////    WirlessParaGet()->buffer[index++] = gflinePara[2].fullID&0xFF;
////    WirlessParaGet()->buffer[index++] = (gflinePara[2].fullID>>8)&0xFF;
////    WirlessParaGet()->buffer[index++] = gflinePara[3].fullID&0xFF;
////    WirlessParaGet()->buffer[index++] = (gflinePara[3].fullID>>8)&0xFF;
////    WirlessParaGet()->buffer[index++] = gflinePara[4].fullID&0xFF;
////    WirlessParaGet()->buffer[index++] = (gflinePara[4].fullID>>8)&0xFF;
//    
//    ParaSettingSendData(gflinePara[0].fullID, gflinePara[1].fullID, index);
}

//void Page2IDConvert(u8 path, u8 per)
//{
//    if(path < PATH_NUMBERS && per < PATH_PER_INDEX)
//    {
//        PathParaGet()->id[path][per] &= 0xFFFF0000;
//        PathParaGet()->id[path][per] |= (PathParaGet()->type[path][per] << 13);
//        PathParaGet()->id[path][per] |= (PathParaGet()->area[path][per] << 10);
//        PathParaGet()->id[path][per] |= (PathParaGet()->mainLine[path][per] << 7);
//        PathParaGet()->id[path][per] |= (PathParaGet()->viceLine[path][per] & 0x7F);
//    }
//}

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
	// 数据录入
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
	// 翻页设置
	if(addr == PAGE2_PATH_LEFT_BUTTON && gP2Table > 0)// 左
	{
		gP2Table--;
		Page2PathConfigTurn(gP2Table);
	}
	else if(addr == PAGE2_PATH_RIGHT_BUTTON && gP2Table < 4)// 右
	{
		gP2Table++;
		Page2PathConfigTurn(gP2Table);
	}
    else if(addr == PAGE2_PATH_LEFT_BUTTON || addr == PAGE2_PATH_RIGHT_BUTTON)
    {
		Page2PathConfigTurn(gP2Table);
    }
    // 按钮
    if(addr == PAGE2_PATH_SEND)
    {
        ParaPathParaSave();
        Page2PathSendPath();
    }
    Page2PathConfigRefresh();
}

void Page2PathConfigRefresh(void)
{
	char str[12];
	u32 tempID, i;
    if(gCountdown == 0)
    {
        gCountdown = 2;
    }
    else
    {
        gCountdown--;
        return;
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
}



