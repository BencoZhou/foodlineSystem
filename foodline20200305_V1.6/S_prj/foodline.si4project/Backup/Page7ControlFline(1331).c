/*****************************************************************
 * File: Page7ControlFline.c
 * Date: 2019/11/28 14:52
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page7ControlFline.h"

static DevicePara gPage7DevicePara;


void Page7ControlFlineInit(void)
{
    gPage7DevicePara.place.device = FOODLINE_STATE;
}

void Page7ControlSend(void)
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

void Page7ControlFlineProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{
	u32 data;
	if(len == 4)
		data = pbuf[0] << 24 | pbuf[1] << 16 | pbuf[2] << 8 | pbuf[3];
	else
		data = pbuf[0] << 8 | pbuf[1];

	// 数据录入
    if(addr == PAGE7_FLINE_CONTROL)
    {
        FoodlineControlGet()->control = data;
    }
    if(addr == PAGE7_FLINE_SENSOR_SELECT)
    {
        FoodlineControlGet()->sensorSelect = data;
    }
    if(addr == PAGE7_FLINE_OVERTIME)
    {
        FoodlineControlGet()->overtime = data;
    }
    if(addr == PAGE7_TYPE1)
    {
        gPage7DevicePara.place.type = data;
    }
    if(addr == PAGE7_AREA1)
    {
        gPage7DevicePara.place.area = data;
    }
    if(addr == PAGE7_MAIN_LINE1)
    {
        gPage7DevicePara.place.mainLine = data;
    }
    if(addr == PAGE7_VICE_LINE1)
    {
        gPage7DevicePara.place.viceLine = data;
    }
    // 按钮
    if(addr == PAGE7_FLINE_CONTROL_BUTTON)
    {
        Page7ControlSend();
    }
}

void Page7ControlFlineRefresh(void)
{
	char str[12];
	u32 tempID;
	DisplayCommNumSend(PAGE7_TYPE1      , gPage7DevicePara.place.type);
	DisplayCommNumSend(PAGE7_AREA1      , gPage7DevicePara.place.area);
	DisplayCommNumSend(PAGE7_MAIN_LINE1 , gPage7DevicePara.place.mainLine);
	DisplayCommNumSend(PAGE7_VICE_LINE1 , gPage7DevicePara.place.viceLine);
	tempID = gPage7DevicePara.id;
	sprintf(str, "0x%08X", tempID);
	DisplayCommTextSend(PAGE3_ID1,	(u8*)str, sizeof(str));
	OSTimeDly(3);
}

