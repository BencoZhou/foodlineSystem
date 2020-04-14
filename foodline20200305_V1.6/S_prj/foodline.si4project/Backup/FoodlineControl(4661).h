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


typedef struct
{
    bool isHaveDevice;  // 是否选择了设备
    bool isHaveAlarm;   // 被选择的设备是否有报警
    bool isClickStart;  // 点击启动按钮
    bool isClickStop;   // 点击停止按钮
}DeviceControlPara;

void ControlMsgBoxInit(void);
void DeviceStateInquire(void);
void DeviceControl(void);
DeviceControlPara *DeviceControlParaGet(void);
void ControlSend_Task(void *Task_Parameters);

#endif // _FOODLINECONTROL_H_



