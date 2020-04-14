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
OS_MsgBoxHandle gControlNotify_Queue;
static u8 gFlineCtrlSeq; // 序号，累加
static u8 gSendIndex;
static u32 retryTime = 400;   // *5ms重发间隔时间，根据路径中设备个数确定

typedef enum
{
    SEND_TYPE_INQUIRE = 0,
    SEND_TYPE_CONTROL_START,
    SEND_TYPE_CONTROL_STOP,
}SEND_TYPE;

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
        index = 0;
        if(event.Info.b[1] == SEND_TYPE_INQUIRE)
        {
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
        else if(event.Info.b[1] == SEND_TYPE_CONTROL_START)
        {
        }
        else if(event.Info.b[1] == SEND_TYPE_CONTROL_STOP)
        {
        }
    }
}

void DeviceSendCmd(u8 index, SEND_TYPE type)
{
    INPUT_EVENT evt;
    evt.Type = Input_PeripCtrl;
    evt.Subtype = Perip_Foodline_control;
    evt.Info.b[0] = index;
    evt.Info.b[1] = type;
    DeviceControlParaGet()->rcDevice.id = S1S2ControlParaGet(index)->cDevice.id;
    if(OS_MsgBoxSend(gControlSend_Queue, &evt, OS_NO_DELAY, FALSE) != OS_ERR_MBOX_FULL)
    {
    }
}


// 查询被选择设备的状态
void DeviceStateInquire(void)
{
    INPUT_EVENT evt;
    u8 i;
    static u32 nowTime, lastTime;
    nowTime = OSTimeGet();
    if(DeviceControlParaGet()->rcDevice.id != 0 && nowTime - lastTime < retryTime)
        return;
    else if(DeviceControlParaGet()->rcDevice.id != 0)
    {
        lastTime = OSTimeGet();
//        DeviceSendCmd(gSendIndex-1, SEND_TYPE_INQUIRE);
        if(++S1S2ControlParaGet(gSendIndex-1)->sendTimes >= 3)
        {
            DeviceControlParaGet()->rcDevice.id = 0;
            S1S2ControlParaGet(gSendIndex-1)->isCommAlarm = TRUE;
            S1S2ControlParaGet(gSendIndex-1)->sendTimes = 0;
        }
        return;
    }
    if(gSendIndex >= S1S2DEVICE_TOTAL_NUMBER)
        gSendIndex = 0;
    for(i = gSendIndex; i < S1S2DEVICE_TOTAL_NUMBER; i++)
    {
        if(S1S2ControlParaGet(i)->isSelect == TRUE)
            break;
    }
    if(i == S1S2DEVICE_TOTAL_NUMBER)
    {
        gSendIndex = 0;
        return;
    }
    gSendIndex = i+1;
    lastTime = OSTimeGet();
    DeviceSendCmd(i, SEND_TYPE_INQUIRE);
}

// 开始控制
void DeviceControlStart(void)
{
        
}

void DeviceControlStop(void)
{

}


