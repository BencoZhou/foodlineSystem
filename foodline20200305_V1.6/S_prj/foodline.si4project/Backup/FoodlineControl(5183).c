/*****************************************************************
 * File: FoodlineControl.c
 * Date: 2019/12/03 16:01
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "FoodlineControl.h"
#include "StateMachine.h"
// control
OS_MsgBoxHandle gControlSend_Queue;
OS_MsgBoxHandle gControlRc_Queue;
static u8 gFlineCtrlSeq; // 序号，累加
static u8 gSendIndex;
static u32 retryTime = 1000;//400;   // *5ms重发间隔时间，根据路径中设备个数确定

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
    
    gControlRc_Queue = OS_MsgBoxCreate("Rc_Queue", sizeof(INPUT_EVENT), 12);
    if (gControlRc_Queue == NULL)
        SYSTEMINIT_Debug("gControlRc_Queue NULL\n\r");
}
void StateMachine_Task(void *Task_Parameters)
{
    INPUT_EVENT event;
    u8 err;
    StateMachineInit();
    while (1)
    {
        err = OS_MsgBoxReceive(gControlRc_Queue, &event, 2000);
//        if (err != OS_NO_ERR)
//            continue;
//        
//        if ((event.Type != Input_PeripCtrl) || (event.Subtype != Perip_Foodline_Rc))
//            continue;
        
        StateMachineProcess();
    }
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
        WirlessParaGet()->buffer[index++] = (LocalDeviceIdGet()>>8)&0xFF;
        WirlessParaGet()->buffer[index++] = LocalDeviceIdGet()&0xFF;
        WirlessParaGet()->buffer[index++] = (S1S2ControlParaGet(place)->cDevice.id>>8)&0xFF; 
        WirlessParaGet()->buffer[index++] = S1S2ControlParaGet(place)->cDevice.id&0xFF;
        WirlessParaGet()->buffer[index++] = gFlineCtrlSeq++;
        WirlessParaGet()->buffer[index++] = path;
        if(event.Info.b[1] == SEND_TYPE_INQUIRE)
        {
            WirlessParaGet()->buffer[index++] = DEVICE_CMD_FLINE_READ;
        }
        else if(event.Info.b[1] == SEND_TYPE_CONTROL_START)
        {
            	WirlessParaGet()->buffer[index++] = DEVICE_CMD_FLINE_SET;
            	WirlessParaGet()->buffer[index++] = 1;     
        }
        else if(event.Info.b[1] == SEND_TYPE_CONTROL_STOP)
        {
            	WirlessParaGet()->buffer[index++] = DEVICE_CMD_FLINE_SET;
            	WirlessParaGet()->buffer[index++] = 0;   
        }

    	WirlessParaGet()->cmd = 0x30;
    	WirlessParaGet()->len = index;
        DeviceControlParaGet()->maxTime[0] = OSTimeGet();
        DeviceControlParaGet()->maxTime[4] = gFlineCtrlSeq-1;
    	ParaSettingSendData(LocalDeviceIdGet(), PathParameterGet()->dPara[path][0].id);
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
        DeviceSendCmd(gSendIndex-1, SEND_TYPE_INQUIRE);
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
    s8 i;
    for(i = S1S2DEVICE_TOTAL_NUMBER-1; i >= 0; i--)
    {
        if(S1S2ControlParaGet(i)->isSelect == TRUE)
        {
            if(S1S2ControlParaGet(i)->cDevice.place.type == DEVICE_NAME_IN_TOWERS
                ||S1S2ControlParaGet(i)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)
            {
                if(S1S2ControlParaGet(i)->stateByte != SWITCH_VALVE_PARA_OPEN && S1S2ControlParaGet(i)->stateByte != SWITCH_VALVE_PARA_OPENING)
                {                    
                    DeviceSendCmd(i, SEND_TYPE_CONTROL_START);
                    break;
                }
            }
            else if(S1S2ControlParaGet(i)->cDevice.place.type == DEVICE_NAME_FLINE)
            {
                if(S1S2ControlParaGet(i)->stateByte != FOODLINE_PARA_OPEN)
                {                    
                    DeviceSendCmd(i, SEND_TYPE_CONTROL_START);
                    break;
                }
            }
            else if(S1S2ControlParaGet(i)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
            {
                if(S1S2ControlParaGet(i)->stateByte != TOWERSOUT_PARA_FOREWARD && S1S2ControlParaGet(i)->stateByte != TOWERSOUT_PARA_FOREWARDING)
                {                    
                    DeviceSendCmd(i, SEND_TYPE_CONTROL_START);
                    break;
                }
            }
        }
        else
        {
            if(S1S2ControlParaGet(i)->cDevice.place.type == DEVICE_NAME_IN_TOWERS
                ||S1S2ControlParaGet(i)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)
            {
                if(S1S2ControlParaGet(i)->stateByte != SWITCH_VALVE_PARA_CLOSE && S1S2ControlParaGet(i)->stateByte != SWITCH_VALVE_PARA_CLOSING)
                {                    
                    DeviceSendCmd(i, SEND_TYPE_CONTROL_STOP);
                    break;
                }
            }
            else if(S1S2ControlParaGet(i)->cDevice.place.type == DEVICE_NAME_FLINE)
            {
                if(S1S2ControlParaGet(i)->stateByte != FOODLINE_PARA_CLOSE)
                {                    
                    DeviceSendCmd(i, SEND_TYPE_CONTROL_STOP);
                    break;
                }
            }
            else if(S1S2ControlParaGet(i)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
            {
                if(S1S2ControlParaGet(i)->stateByte != TOWERSOUT_PARA_SOTP)
                {                    
                    DeviceSendCmd(i, SEND_TYPE_CONTROL_STOP);
                    break;
                }
            }
        }
    }
}

void DeviceControlStop(void)
{
    s8 i;
    for(i = 0; i < S1S2DEVICE_TOTAL_NUMBER; i++)
    {
        if(S1S2ControlParaGet(i)->isSelect == TRUE)
        {
            
        }
    }

}
// 强制关机
void DeviceShutdown(void)
{
    s8 i;
    for(i = 0; i < S1S2DEVICE_TOTAL_NUMBER; i++)
    {
        if(S1S2ControlParaGet(i)->cDevice.place.type == DEVICE_NAME_IN_TOWERS
            ||S1S2ControlParaGet(i)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)
        {
            if(S1S2ControlParaGet(i)->cState != 2 && S1S2ControlParaGet(i)->cState != 4)
            {                    
                DeviceSendCmd(i, SEND_TYPE_CONTROL_STOP);
                break;
            }
        }
        else if(S1S2ControlParaGet(i)->cDevice.place.type == DEVICE_NAME_FLINE)
        {
            if(S1S2ControlParaGet(i)->cState != 0)
            {                    
                DeviceSendCmd(i, SEND_TYPE_CONTROL_STOP);
                break;
            }
        }
        else if(S1S2ControlParaGet(i)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
        {
            if(S1S2ControlParaGet(i)->cState != 0)
            {                    
                DeviceSendCmd(i, SEND_TYPE_CONTROL_STOP);
                break;
            }
        }
    }
}


