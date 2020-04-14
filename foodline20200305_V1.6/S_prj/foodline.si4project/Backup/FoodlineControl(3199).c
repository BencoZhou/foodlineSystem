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

typedef struct
{
    DevicePara dPara[PATH_PER_INDEX];
    u8 index;
}ControlDevice;

static ControlDevice gControlDevice;
static DeviceControlPara gDeviceControlPara;

DeviceControlPara DeviceControlParaGet(void)
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
    u8 *pbuf;
    u8 len, index;
    u8 err, exit, cnt;
    u8 seq;
    while (1)
    {
        err = OS_MsgBoxReceive(gControlSend_Queue, &event, OS_MAX_DELAY);
        if (err != OS_NO_ERR)
            continue;
        
        if ((event.Type != Input_PeripCtrl) || (event.Subtype != Perip_Foodline_control))
            continue;
        
//        len = event.Info.b[0];
//        index = event.Info.b[1];
//        pbuf = &(gControlMsgBody[index][0]);
//        seq = event.Info.b[2];
        
        exit = 0;
        cnt = 0;
        while (cnt++ < 3)
        {
//            ParaSettingSendData(pbuf, len);
            
            while (1)
            {
                err = OS_MsgBoxReceive(gControlNotify_Queue, &event, 40);
                if (err == OS_TIMEOUT)
                    break;
                
                if ((err == OS_NO_ERR) && (event.Num == 1))
                {
                    if (event.Info.b[0] == seq)
                    {
                        exit = 1;
                        break;
                    }
                }
            }
            
            if (exit)
                break;
        }
        gDisplayMsgSendLen[index] = 0;
    }
}


// 查询被选择设备的状态
void DeviceStateInquire(void)
{

}

// 开始控制
void DeviceControl(void)
{

}
