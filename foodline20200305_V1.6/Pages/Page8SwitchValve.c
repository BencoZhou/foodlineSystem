/*****************************************************************
 * File: Page8SwitchValve.c
 * Date: 2019/11/28 18:50
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page8SwitchValve.h"
#include "ParaSettingLib.h"

static DevicePara gPage8DevicePara;
static u8 gPage8Seq; // 序号，累加
static bool gP8Action = FALSE;


void Page8SwitchValveInit(void)
{
    gPage8DevicePara.place.type = DEVICE_NAME_IN_TOWERS;     
	gPage8DevicePara.place.device = FOODLINE_STATE;
	SwitchValveControlGet()->rcAlarm = 0;
	gP8Action = FALSE;

}

void Page8ControlSend(void)
{
	u8 index = 0;
	u8 path;
	path = PathTrav(gPage8DevicePara.id);
	if(path == PATH_NUMBERS)
		return;
	WirlessParaGet()->buffer[index++] = (LocalDeviceIdGet()>>8)&0xFF;
	WirlessParaGet()->buffer[index++] = LocalDeviceIdGet()&0xFF;
	WirlessParaGet()->buffer[index++] = (gPage8DevicePara.id>>8)&0xFF; 
	WirlessParaGet()->buffer[index++] = gPage8DevicePara.id&0xFF;
	WirlessParaGet()->buffer[index++] = gPage8Seq++;
	WirlessParaGet()->buffer[index++] = path;
	WirlessParaGet()->buffer[index++] = 0x36;
	WirlessParaGet()->buffer[index++] = SwitchValveControlGet()->control;

	WirlessParaGet()->cmd = 0x30;
	WirlessParaGet()->len = index;
	ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id);
}

void Page8ControlAsk(void)
{
	u8 index = 0;
	u8 path;
	path = PathTrav(gPage8DevicePara.id);
	if(path == PATH_NUMBERS)
		return;
	WirlessParaGet()->buffer[index++] = (LocalDeviceIdGet()>>8)&0xFF;
	WirlessParaGet()->buffer[index++] = LocalDeviceIdGet()&0xFF;
	WirlessParaGet()->buffer[index++] = (gPage8DevicePara.id>>8)&0xFF; 
	WirlessParaGet()->buffer[index++] = gPage8DevicePara.id&0xFF;
	WirlessParaGet()->buffer[index++] = gPage8Seq++;
	WirlessParaGet()->buffer[index++] = path;
	WirlessParaGet()->buffer[index++] = 0x38;

	WirlessParaGet()->cmd = 0x30;
	WirlessParaGet()->len = index;
	ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id);
}

void Page8SwitchValveProcess(u8 reg, u16 addr, u8 *pbuf, u8 len)
{
	u32 data;
	if(len == 4)
		data = pbuf[0] << 24 | pbuf[1] << 16 | pbuf[2] << 8 | pbuf[3];
	else
		data = pbuf[0] << 8 | pbuf[1];

	// 数据录入
	if(addr == PAGE8_SVALVE_CONTROL)
	{
		SwitchValveControlGet()->control = data;
	}
	if(addr == PAGE8_TYPE1)
	{
		gPage8DevicePara.place.type = data;
	}
	if(addr == PAGE8_AREA1)
	{
		gPage8DevicePara.place.area = data;
	}
	if(addr == PAGE8_MAIN_LINE1)
	{
		gPage8DevicePara.place.mainLine = data;
	}
	if(addr == PAGE8_VICE_LINE1)
	{
		gPage8DevicePara.place.viceLine = data;
	}
	// 按钮
	if(addr == PAGE8_SVALVE_CONTROL_BUTTON)
	{
		Page8ControlSend();
	}
	if(addr == PAGE8_SVALVE_STATE_BUTTON)
	{
		gP8Action = !gP8Action;
	}
}

void Page8SwitchValveRefresh(void)
{
	char str[12];
	u32 tempID;
	if(gP8Action)
		Page8ControlAsk();
    
      
    DisplayCommNumSend(PAGE8_SVALVE_RC_ALARM , SwitchValveControlGet()->rcAlarm);
    DisplayCommNumSend(PAGE8_TYPE1      , gPage8DevicePara.place.type);
    DisplayCommNumSend(PAGE8_AREA1      , gPage8DevicePara.place.area);
    DisplayCommNumSend(PAGE8_MAIN_LINE1 , gPage8DevicePara.place.mainLine);
    DisplayCommNumSend(PAGE8_VICE_LINE1 , gPage8DevicePara.place.viceLine);
    tempID = gPage8DevicePara.id;
    sprintf(str, "0x%08X", tempID);
    DisplayCommTextSend(PAGE8_ID1,	(u8*)str, sizeof(str));
    OSTimeDly(3);    
    
    DisplayCommNumSend(PAGE8_SVALVE_UP_SENSOR   , SwitchValveControlGet()->onoff.b.b0);
    DisplayCommNumSend(PAGE8_SVALVE_DOWN_SENSOR , SwitchValveControlGet()->onoff.b.b1);
    DisplayCommNumSend(PAGE8_SVALVE_TCOCK_OPEN  , SwitchValveControlGet()->onoff.b.b2);
    DisplayCommNumSend(PAGE8_SVALVE_TCOCK_CLOSE , SwitchValveControlGet()->onoff.b.b3);
    DisplayCommNumSend(PAGE8_SVALVE_TCOCK_STATE , SwitchValveControlGet()->tcock);
    DisplayCommNumSend(PAGE8_SVALVE_ALARM_OPEN  , SwitchValveControlGet()->alarm.b.b0);
    DisplayCommNumSend(PAGE8_SVALVE_ALARM_CLOSE , SwitchValveControlGet()->alarm.b.b1);
    DisplayCommNumSend(PAGE8_SVALVE_DEVICE_STATE, SwitchValveControlGet()->manualAuto);
    OSTimeDly(3);  
    
    PageXIdSet(gPage8DevicePara);
           
}
	

	


	


