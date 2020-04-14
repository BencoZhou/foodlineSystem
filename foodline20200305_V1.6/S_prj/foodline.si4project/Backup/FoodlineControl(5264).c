/*****************************************************************
 * File: FoodlineControl.c
 * Date: 2019/12/03 16:01
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "FoodlineControl.h"
// control
OS_MsgBoxHandle gControlSend_Queue;
u8 gControlMsgBody[CONTROL_MSGSND_COUNT][128];
u8 gControlMsgBodyLen[CONTROL_MSGSND_COUNT];
OS_MsgBoxHandle gControlNotify_Queue;
static u8 gFlineCtrlSeq; // 序号，累加
static u8 gSendIndex;

typedef struct
{
    DevicePara dPara[PATH_PER_INDEX];
    u8 index;
}ControlDevice;

static ControlDevice gControlDevice;
static DeviceControlPara gDeviceControlPara;

DeviceControlPara *DeviceControlParaGet(void)
{
    return &gDeviceControlPara;
}


void ControlMsgBoxInit(void)
{
    // Control
    gControlSend_Queue = OS_MsgBoxCreate("CsndEvt", sizeof(INPUT_EVENT), CONTROL_MSGSND_COUNT);
    if (gControlSend_Queue == NULL)
        SYSTEMINIT_Debug("gControlSend_Queue NULL\n\r");
    
    gControlNotify_Queue = OS_MsgBoxCreate("CsndEvt", sizeof(INPUT_EVENT), 12);
    if (gControlNotify_Queue == NULL)
        SYSTEMINIT_Debug("gControlSend_Queue NULL\n\r");
}


void ControlSend_Task(void *Task_Parameters)
{
    INPUT_EVENT event;
    u8 index = 0, place;
    u8 path, err;
    ControlMsgBoxInit();
    while (1)
    {
        err = OS_MsgBoxReceive(gControlSend_Queue, &event, OS_MAX_DELAY);
        if (err != OS_NO_ERR)
            continue;
        
        if ((event.Type != Input_PeripCtrl) || (event.Subtype != Perip_Foodline_control))
            continue;

        place = event.Info.b[0];
        path = PathTrav(S1S2ControlParaGet(place)->cDevice.id);
        if(path == PATH_NUMBERS)
            return;
        WirlessParaGet()->buffer[index++] = (LocalDeviceIdGet()>>8)&0xFF;
        WirlessParaGet()->buffer[index++] = LocalDeviceIdGet()&0xFF;
        WirlessParaGet()->buffer[index++] = (S1S2ControlParaGet(place)->cDevice.id>>8)&0xFF; 
        WirlessParaGet()->buffer[index++] = S1S2ControlParaGet(place)->cDevice.id&0xFF;
        WirlessParaGet()->buffer[index++] = gFlineCtrlSeq++;
        WirlessParaGet()->buffer[index++] = path;
        WirlessParaGet()->buffer[index++] = 0x38;
        
        WirlessParaGet()->cmd = 0x30;
        WirlessParaGet()->len = index;
        ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id);  
    }
}


// 查询被选择设备的状态
void DeviceStateInquire(void)
{
    INPUT_EVENT evt;
    u8 i;
    if(gSendIndex >= S1S2DEVICE_TOTAL_NUMBER)
        gSendIndex = 0;
    for(i = gSendIndex; i < S1S2DEVICE_TOTAL_NUMBER; i++)
    {
        if(S1S2ControlParaGet(i)->isSelect == TRUE)
        {
            break;
        }
    }
    if(i == S1S2DEVICE_TOTAL_NUMBER)
    {
        gSendIndex = 0;
        return;
    }
    gSendIndex = i+1;
    evt.Type = Input_PeripCtrl;
    evt.Subtype = Perip_Foodline_control;
    evt.Info.b[0] = i;
    if(OS_MsgBoxSend(gControlSend_Queue, &evt, OS_NO_DELAY, FALSE) != OS_ERR_MBOX_FULL)
    {
    }

}

// 开始控制
void DeviceControl(void)
{

}

