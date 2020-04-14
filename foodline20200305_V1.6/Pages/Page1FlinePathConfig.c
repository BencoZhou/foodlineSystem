/*****************************************************************
 * File: Page1flinePathConfig.c
 * Date: 2019/11/01 9:19
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page1flinePathConfig.h"

#define PATH_DEVICE_NUMBER                  (10)

//static u8 gP1Table;
static flinePara gflinePara[PATH_DEVICE_NUMBER];
static u8 gPath;
static u8 gSeq; // 序号，累加

void Page1Convert(flinePara *data)
{
    data->fullID = 0;
    data->fullID = data->deviceType<<13;
    data->fullID += data->letter<<10;
    data->fullID += data->number;
    data->fullID += 0x32000000;
}

void Page1FlinePathConfigInit(void)
{
//    u8 tempLen;
//	ScrInfGet()->currentTable = gP1Table;
//    tempLen = sizeof(gflinePara);
//    tempLen = sizeof(flinePara);
//    memset(&gflinePara[0], 0, PATH_DEVICE_NUMBER*sizeof(flinePara));
//	Page1FlinePathConfigRefresh();
}

void Page1FlineSendPath(void)
{
    u8 index = 0;
    WirlessParaGet()->buffer[index++] = (gflinePara[0].fullID>>8)&0xFF;
    WirlessParaGet()->buffer[index++] = gflinePara[0].fullID&0xFF;
    WirlessParaGet()->buffer[index++] = (gflinePara[4].fullID>>8)&0xFF;
    WirlessParaGet()->buffer[index++] = gflinePara[4].fullID&0xFF;
    WirlessParaGet()->buffer[index++] = gSeq++;
    WirlessParaGet()->buffer[index++] = gPath;
    WirlessParaGet()->buffer[index++] = 0x33;
//    WirlessParaGet()->buffer[index++] = gflinePara[0].fullID&0xFF;
//    WirlessParaGet()->buffer[index++] = (gflinePara[0].fullID>>8)&0xFF;
//    WirlessParaGet()->buffer[index++] = gflinePara[1].fullID&0xFF;
//    WirlessParaGet()->buffer[index++] = (gflinePara[1].fullID>>8)&0xFF;
//    WirlessParaGet()->buffer[index++] = gflinePara[2].fullID&0xFF;
//    WirlessParaGet()->buffer[index++] = (gflinePara[2].fullID>>8)&0xFF;
//    WirlessParaGet()->buffer[index++] = gflinePara[3].fullID&0xFF;
//    WirlessParaGet()->buffer[index++] = (gflinePara[3].fullID>>8)&0xFF;
//    WirlessParaGet()->buffer[index++] = gflinePara[4].fullID&0xFF;
//    WirlessParaGet()->buffer[index++] = (gflinePara[4].fullID>>8)&0xFF;
    WirlessParaGet()->cmd = 0x30;
    WirlessParaGet()->len = index;
    
    ParaSettingSendData(gflinePara[0].fullID, gflinePara[1].fullID);
}

void Page1FlinePathConfigProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{
    u32 temp, data;
	if(len == 4)
		data = pbuf[0] << 24 | pbuf[1] << 16 | pbuf[2] << 8 | pbuf[3];
	else
		data = pbuf[0] << 8 | pbuf[1];
    
    temp = addr&0xFF;
    if(temp < 0x1F)
    {
        if(temp%3 == 0)
            gflinePara[temp/3].deviceType = data;
        else if(temp%3 == 1)
            gflinePara[temp/3].letter = data;
        else if(temp%3 == 2)
            gflinePara[temp/3].number = data;
    }
	// 数据录入
	if(addr == PAGE1_FLINE_PATH)
	{
        gPath = data;
	}
    if(addr == PAGE1_FLINE_SEND_BUTTON)
    {
        Page1FlineSendPath();
    }

}

void Page1FlinePathConfigRefresh(void)
{
	char str[12];
	u32 tempID, i;
    memset(str, 0, sizeof(str));
    for(i = 0; i < PATH_DEVICE_NUMBER; i++)
    {
        Page1Convert(&gflinePara[i]);
        DisplayCommNumSend(PAGE1_FLINE_DEVICE_TYPE+i*3, gflinePara[i].deviceType);
        DisplayCommIconSend(PAGE1_FLINE_LETTER+i*3,     gflinePara[i].letter);
        DisplayCommNumSend(PAGE1_FLINE_NUMBER+i*3,      gflinePara[i].number);
    	tempID = gflinePara[i].fullID;
    	sprintf(str, "0x%08X", tempID);
    	DisplayCommTextSend(PAGE1_FLINE_FULLID+i*0x10,	(u8*)str, sizeof(str));
	    OSTimeDly(5);
    }
    
    DisplayCommNumSend(PAGE1_FLINE_PATH, gPath);
}

