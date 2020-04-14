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
OS_MsgBoxHandle gControlStartRe_Queue;
OS_MsgBoxHandle gControlStopRe_Queue;
OS_MsgBoxHandle gControlSendSeq_Queue;

static u8 gFlineCtrlSeq; // 序号，累加
static u8 gSendIndex;
static u32 retryTime = 1000;//400;   // *5ms重发间隔时间，根据路径中设备个数确定

#define DELAY_RESEND                    (1000)


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
    
    gControlStartRe_Queue = OS_MsgBoxCreate("StartRe_Queue", sizeof(INPUT_EVENT), 12);
    if (gControlStartRe_Queue == NULL)
        SYSTEMINIT_Debug("gControlStartRe_Queue NULL\n\r");
    
    gControlStopRe_Queue = OS_MsgBoxCreate("StopRe_Queue", sizeof(INPUT_EVENT), 12);
    if (gControlStopRe_Queue == NULL)
        SYSTEMINIT_Debug("gControlStopRe_Queue NULL\n\r");
    
    gControlSendSeq_Queue = OS_MsgBoxCreate("SendSeq_Queue", sizeof(INPUT_EVENT), 12);
    if (gControlSendSeq_Queue == NULL)
        SYSTEMINIT_Debug("gControlSendSeq_Queue NULL\n\r");
}
void StateMachine_Task(void *Task_Parameters)
{
    StateMachineInit();
    while (1)
    {
        
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
            continue;
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

        if(S1S2ControlParaGet(place)->cDevice.place.type == DEVICE_NAME_FLINE)
        {
            WirlessParaGet()->buffer[index++] = 0;   
            WirlessParaGet()->buffer[index++] = 0;   
            WirlessParaGet()->buffer[index++] = 0;   
        }
        else if(S1S2ControlParaGet(place)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
        {
            WirlessParaGet()->buffer[index++] = 0;   
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

bool SingleDeviceStop(u8 index)
{
    if(S1S2ControlParaGet(index)->cDevice.place.type == DEVICE_NAME_IN_TOWERS
        ||S1S2ControlParaGet(index)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)
    {
        if(S1S2ControlParaGet(index)->stateByte != SWITCH_VALVE_PARA_CLOSE 
//            && S1S2ControlParaGet(index)->stateByte != SWITCH_VALVE_PARA_CLOSING
            )
        {                    
            DeviceSendCmd(index, SEND_TYPE_CONTROL_STOP);
            return TRUE;
        }
    }
    else if(S1S2ControlParaGet(index)->cDevice.place.type == DEVICE_NAME_FLINE)
    {
        if(S1S2ControlParaGet(index)->stateByte != FOODLINE_PARA_CLOSE)
        {                    
            DeviceSendCmd(index, SEND_TYPE_CONTROL_STOP);
            return TRUE;
        }
    }
    else if(S1S2ControlParaGet(index)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
    {
        if(S1S2ControlParaGet(index)->stateByte != TOWERSOUT_PARA_SOTP)
        {                    
            DeviceSendCmd(index, SEND_TYPE_CONTROL_STOP);
            return TRUE;
        }
    }

    return FALSE;
}

bool SingleDeviceControl(u8 index)
{
    if(S1S2ControlParaGet(index)->isSelect == TRUE)
    {
        if(S1S2ControlParaGet(index)->cDevice.place.type == DEVICE_NAME_IN_TOWERS
            ||S1S2ControlParaGet(index)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)
        {
            if(S1S2ControlParaGet(index)->stateByte != SWITCH_VALVE_PARA_OPEN 
//                && S1S2ControlParaGet(index)->stateByte != SWITCH_VALVE_PARA_OPENING
            )
            {                    
                DeviceSendCmd(index, SEND_TYPE_CONTROL_START);
                return TRUE;
            }
        }
        else if(S1S2ControlParaGet(index)->cDevice.place.type == DEVICE_NAME_FLINE)
        {
            if(S1S2ControlParaGet(index)->stateByte != FOODLINE_PARA_OPEN)
            {                    
                DeviceSendCmd(index, SEND_TYPE_CONTROL_START);
                return TRUE;
            }
        }
        else if(S1S2ControlParaGet(index)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
        {
            if(S1S2ControlParaGet(index)->stateByte != TOWERSOUT_PARA_FOREWARD 
//                && S1S2ControlParaGet(index)->stateByte != TOWERSOUT_PARA_FOREWARDING
            )
            {                    
                DeviceSendCmd(index, SEND_TYPE_CONTROL_START);
                return TRUE;
            }
        }
    }
    else
    {
        if(S1S2ControlParaGet(index)->cDevice.place.type == DEVICE_NAME_IN_TOWERS
            ||S1S2ControlParaGet(index)->cDevice.place.type == DEVICE_NAME_MAIN_VICE)
        {
            if(S1S2ControlParaGet(index)->stateByte != SWITCH_VALVE_PARA_CLOSE 
//                && S1S2ControlParaGet(index)->stateByte != SWITCH_VALVE_PARA_CLOSING
            )
            {                    
                DeviceSendCmd(index, SEND_TYPE_CONTROL_STOP);
                return TRUE;
            }
        }
        else if(S1S2ControlParaGet(index)->cDevice.place.type == DEVICE_NAME_FLINE)
        {
            if(S1S2ControlParaGet(index)->stateByte != FOODLINE_PARA_CLOSE)
            {                    
                DeviceSendCmd(index, SEND_TYPE_CONTROL_STOP);
                return TRUE;
            }
        }
        else if(S1S2ControlParaGet(index)->cDevice.place.type == DEVICE_NAME_TOWERS_OUT)
        {
            if(S1S2ControlParaGet(index)->stateByte != TOWERSOUT_PARA_SOTP)
            {                    
                DeviceSendCmd(index, SEND_TYPE_CONTROL_STOP);
                return TRUE;
            }
        }
    }

    return FALSE;
}

void DeviceControlAndInquire(void)
{
    u8 err;
    INPUT_EVENT event;
    static u8 index = S1S2DEVICE_TOTAL_NUMBER-1;
    static bool isFinish;
    
    err = OS_MsgBoxReceive(gControlStartRe_Queue, &event, OS_NO_DELAY);
    if (err == OS_NO_ERR)
    {
        index = S1S2DEVICE_TOTAL_NUMBER-1;
        isFinish = FALSE;
    }
    while(OS_MsgBoxReceive(gControlRc_Queue, &event, OS_NO_DELAY) == OS_NO_ERR);
    DeviceSendCmd(index, SEND_TYPE_INQUIRE);    
    err = OS_MsgBoxReceive(gControlRc_Queue, &event, DELAY_RESEND);
    if (err == OS_TIMEOUT)
        return;
    if(event.Info.b[0] == SEND_TYPE_INQUIRE)
    {
        if(isFinish == FALSE && SingleDeviceControl(index))
        {
            err = OS_MsgBoxReceive(gControlRc_Queue, &event, DELAY_RESEND);
            if (err == OS_TIMEOUT)
                return;
        }
        else
        {
            if(index == 0)
            {
                index = S1S2DEVICE_TOTAL_NUMBER-1;
                isFinish = TRUE;
            }
            else
            {
                index -= 1;
            }
        }
    }
}


void DeviceStopAndInquire(void)
{
    u8 err;
    INPUT_EVENT event;
    static u8 index = 0;
    
    err = OS_MsgBoxReceive(gControlStartRe_Queue, &event, OS_NO_DELAY);
    if (err == OS_NO_ERR)
    {
        index = 0;
    }
    while(OS_MsgBoxReceive(gControlRc_Queue, &event, OS_NO_DELAY) == OS_NO_ERR);
    DeviceSendCmd(index, SEND_TYPE_INQUIRE);   
    err = OS_MsgBoxReceive(gControlRc_Queue, &event, DELAY_RESEND);
    if (err == OS_TIMEOUT)
        return;
    if(event.Info.b[0] == SEND_TYPE_INQUIRE)
    {
        if(SingleDeviceStop(index))
        {
            err = OS_MsgBoxReceive(gControlRc_Queue, &event, DELAY_RESEND);
            if (err == OS_TIMEOUT)
                return;
        }
        else
        {
            if(index == S1S2DEVICE_TOTAL_NUMBER-1)
                index = 0;
            else
                index += 1;
        }
    }
}



// 查询被选择设备的状态
void DeviceStateInquire(void)
{
    INPUT_EVENT event;
    u8 i, err;
    static u32 nowTime, lastTime;
    nowTime = OSTimeGet();
    err = OS_MsgBoxReceive(gControlRc_Queue, &event, 200);
//    if (err == OS_TIMEOUT)
//        return;
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


