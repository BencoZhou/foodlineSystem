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
static u8 gPage7Seq; // 序号，累加
static bool gPage7Action = FALSE;


void Page7ControlFlineInit(void)
{
    gPage7DevicePara.place.type = DEVICE_NAME_FLINE;     
    gPage7DevicePara.place.device = FOODLINE_STATE;
    FoodlineControlGet()->rcAlarm = 0;
    gPage7Action = FALSE;
}

void Page7ControlSend(void)
{
	WirlessPara gWirlessParaP7;
    u8 index = 0;
    u8 path;
    path = PathTrav(gPage7DevicePara.id);
    if(path == PATH_NUMBERS)
        return;
    gWirlessParaP7.buffer[index++] = (LocalDeviceIdGet()>>8)&0xFF;
    gWirlessParaP7.buffer[index++] = LocalDeviceIdGet()&0xFF;
    gWirlessParaP7.buffer[index++] = (gPage7DevicePara.id>>8)&0xFF; 
    gWirlessParaP7.buffer[index++] = gPage7DevicePara.id&0xFF;
    gWirlessParaP7.buffer[index++] = gPage7Seq++;
    gWirlessParaP7.buffer[index++] = path;
    gWirlessParaP7.buffer[index++] = 0x36;
    gWirlessParaP7.buffer[index++] = FoodlineControlGet()->control;
    gWirlessParaP7.buffer[index++] = FoodlineControlGet()->sensorSelect;
    gWirlessParaP7.buffer[index++] = (FoodlineControlGet()->overtime>>8)&0xFF; 
    gWirlessParaP7.buffer[index++] = FoodlineControlGet()->overtime&0xFF;
    
    gWirlessParaP7.cmd = 0x30;
    gWirlessParaP7.len = index;
//    ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id);
	
	gWirlessParaP7.buffer[5] |= 0x80;
    WirelessApp_SendData(gWirlessParaP7.cmd, FRAME_NEED_NO_ACK, gWirlessParaP7.buffer, gWirlessParaP7.len, LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id, PathParameterGet()->dPara[path][0].id, 0);
		
}

void Page7ControlAsk(void)
{
    u8 index = 0;
    u8 path;
	WirlessPara gWirlessParaP7;
    path = PathTrav(gPage7DevicePara.id);
    if(path == PATH_NUMBERS)
        return;
    gWirlessParaP7.buffer[index++] = (LocalDeviceIdGet()>>8)&0xFF;
    gWirlessParaP7.buffer[index++] = LocalDeviceIdGet()&0xFF;
    gWirlessParaP7.buffer[index++] = (gPage7DevicePara.id>>8)&0xFF; 
    gWirlessParaP7.buffer[index++] = gPage7DevicePara.id&0xFF;
    gWirlessParaP7.buffer[index++] = gPage7Seq++;
    gWirlessParaP7.buffer[index++] = path;
    gWirlessParaP7.buffer[index++] = 0x38;
    
    gWirlessParaP7.cmd = 0x30;
    gWirlessParaP7.len = index;
	gWirlessParaP7.buffer[5] |= 0x80;
    WirelessApp_SendData(gWirlessParaP7.cmd, FRAME_NEED_NO_ACK, gWirlessParaP7.buffer, gWirlessParaP7.len, LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id, PathParameterGet()->dPara[path][0].id, 0);

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
    if(addr == PAGE7_FLINE_STATE_BUTTON)
    {
        gPage7Action = !gPage7Action;
    }
}

void Page7ControlFlineRefresh(void)
{
	char str[12];
	u32 tempID;
    if(gPage7Action)
        Page7ControlAsk();
    
	DisplayCommNumSend(PAGE7_TYPE1      , gPage7DevicePara.place.type);
	DisplayCommNumSend(PAGE7_AREA1      , gPage7DevicePara.place.area);
	DisplayCommNumSend(PAGE7_MAIN_LINE1 , gPage7DevicePara.place.mainLine);
	DisplayCommNumSend(PAGE7_VICE_LINE1 , gPage7DevicePara.place.viceLine);
	tempID = gPage7DevicePara.id;
	sprintf(str, "0x%08X", tempID);
	DisplayCommTextSend(PAGE7_ID1,	(u8*)str, sizeof(str));
	DisplayCommNumSend(PAGE7_FLINE_RC_ALARM , FoodlineControlGet()->rcAlarm);
	DisplayCommNumSend(PAGE7_FLINE_CURRENT_A , FoodlineControlGet()->currentA);
	DisplayCommNumSend(PAGE7_FLINE_CURRENT_B , FoodlineControlGet()->currentB);
	DisplayCommNumSend(PAGE7_FLINE_CURRENT_C , FoodlineControlGet()->currentC);
	DisplayCommNumSend(PAGE7_FLINE_STATE     , FoodlineControlGet()->state);
	OSTimeDly(3);    
	DisplayCommNumSend(PAGE7_FLINE_SENSOR1_STATE	 , FoodlineControlGet()->sensorState.b.b0);
	DisplayCommNumSend(PAGE7_FLINE_SENSOR2_STATE	 , FoodlineControlGet()->sensorState.b.b1);
	DisplayCommNumSend(PAGE7_FLINE_SENSOR3_STATE	 , FoodlineControlGet()->sensorState.b.b2);
	DisplayCommNumSend(PAGE7_FLINE_SENSOR1_COMM	 , FoodlineControlGet()->sensorComm.b.b0);
	DisplayCommNumSend(PAGE7_FLINE_SENSOR2_COMM	 , FoodlineControlGet()->sensorComm.b.b1);
	DisplayCommNumSend(PAGE7_FLINE_SENSOR3_COMM	 , FoodlineControlGet()->sensorComm.b.b2);
	DisplayCommNumSend(PAGE7_FLINE_OPENALARM		 , FoodlineControlGet()->alarm.b.b0);
	DisplayCommNumSend(PAGE7_FLINE_REGULATINGALARM , FoodlineControlGet()->alarm.b.b1);
	DisplayCommNumSend(PAGE7_FLINE_CURRENTALARM	 , FoodlineControlGet()->alarm.b.b2);
	DisplayCommNumSend(PAGE7_FLINE_OPENPHASEALARM	 , FoodlineControlGet()->alarm.b.b3);
	DisplayCommNumSend(PAGE7_FLINE_MANUAL_AUTO		 , FoodlineControlGet()->manualAuto);
	OSTimeDly(3);    
    PageXIdSet(gPage7DevicePara);
}

