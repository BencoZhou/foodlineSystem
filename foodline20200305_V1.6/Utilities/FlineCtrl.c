/*****************************************************************
 * File: FlineCtrl.c
 * Date: 2019/06/19 10:52
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "FlineCtrl.h"

#define FLINE_BUFFER_LEN                (87)
static FlineCtrlMsg gFlineCtrlMsg;
//static u8 gFlineBuf[FLINE_BUFFER_LEN];

// Ñ¯ÎÊËÇÎ¹Õ¾ÊÇ·ñÈ±ÁÏ
void FlineCtrlInit(void)
{
    gFlineCtrlMsg.destAddrs[0] = 0x0A504204;
    gFlineCtrlMsg.destAddrs[1] = 0x0A504205;
    gFlineCtrlMsg.isInit = TRUE;
    gFlineCtrlMsg.isReady = TRUE;
    gFlineCtrlMsg.rcTime = 0;
}

void FlineCtrlProcess(void)
{    
//	u8 len = 0;
    
//	memset(gFlineBuf, 0, FLINE_BUFFER_LEN);
//	WirelessApp_SendData(CMD_ASK_G4_RECORD, 
//							0, 
//							gFlineBuf, 
//							len, 
//							ParaSettingLibGet()->gPiggeryConf.localid,
//							gFlineCtrlMsg.destAddrs[0],
//							gFlineCtrlMsg.destAddrs[0],
//							0);
//    OSTimeDly(10);
//	WirelessApp_SendData(CMD_ASK_G4_RECORD, 
//							0, 
//							gFlineBuf, 
//							len, 
//							ParaSettingLibGet()->gPiggeryConf.localid,
//							gFlineCtrlMsg.destAddrs[1],
//							gFlineCtrlMsg.destAddrs[1],
//							0);
//    OSTimeDly(10);
}

void FlineCtrlPower(void)
{
    DeviceControlSet(ParaSettingLibGet()->gCommDevConf[COMM_DEV_CONF_INDEX_FOODLINE].id, DEVICE_SWITCH_ON, 0);
    gFlineCtrlMsg.rcTime = OSTimeGet();
//    gFlineCtrlMsg.isReady = FALSE;
}

FlineCtrlMsg *FlineCtrlMsgGet(void)
{
    return &gFlineCtrlMsg;
}

