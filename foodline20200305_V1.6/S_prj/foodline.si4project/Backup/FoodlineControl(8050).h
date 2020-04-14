/*****************************************************************
 * File: FoodlineControl.h
 * Date: 2019/12/03 16:00
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _FOODLINECONTROL_H_
#define _FOODLINECONTROL_H_

#include "System.h"

#define CONTROL_MSGSND_COUNT                (16)

typedef enum
{
    SEND_TYPE_INQUIRE = 0,
    SEND_TYPE_CONTROL_START,
    SEND_TYPE_CONTROL_STOP,
    SEND_TYPE_CONTROL,
}SEND_TYPE;

typedef struct
{
    bool isHaveDevice;  // 是否选择了设备
    bool isHaveAlarm;   // 被选择的设备是否有报警
    bool isClickStart;  // 点击启动按钮
    bool isClickStop;   // 点击停止按钮
    bool isClickShutdown;  // 点击强制停止按钮
    bool isClickReshutdown;   // 点击恢复停止按钮
    DevicePara  rcDevice;     // 刚发送数据设备
    u32  maxTime[10];       // 测试发送到接收时间长短
}DeviceControlPara;

void ControlMsgBoxInit(void);
void DeviceStateInquire(void);
void DeviceStopAndInquire(void);                // 全部强制停止
void DeviceControlAndInquire(void); // 启动
DeviceControlPara *DeviceControlParaGet(void);
//void InquireSend_Task(void *Task_Parameters);
void ControlSend_Task(void *Task_Parameters);
void StateMachine_Task(void *Task_Parameters);

#endif // _FOODLINECONTROL_H_



