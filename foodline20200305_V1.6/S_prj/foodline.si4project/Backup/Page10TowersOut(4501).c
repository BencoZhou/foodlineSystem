/*****************************************************************
 * File: Page10TowersOut.c
 * Date: 2019/11/29 10:29
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Page10TowersOut.h"


static DevicePara gPage7DevicePara;
static u8 gPage7Seq; // ÐòºÅ£¬ÀÛ¼Ó
static bool gAction = FALSE;

void Page7ControlFlineRefresh(void)
{
	char str[12];
	u32 tempID;
	if(gAction)
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
}
