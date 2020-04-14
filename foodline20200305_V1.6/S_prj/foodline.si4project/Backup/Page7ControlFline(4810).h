/*****************************************************************
 * File: Page7ControlFline.h
 * Date: 2019/11/28 14:52
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _PAGE7CONTROLFLINE_H_
#define _PAGE7CONTROLFLINE_H_

#include "System.h"

#define PAGE7_FLINE_CONTROL								(0x0700)
#define PAGE7_FLINE_SENSOR_SELECT						(0x0701)
#define PAGE7_FLINE_OVERTIME							(0x0702)

#define PAGE7_FLINE_CURRENT_A							(0x0703)
#define PAGE7_FLINE_CURRENT_B							(0x0704)
#define PAGE7_FLINE_CURRENT_C							(0x0705)
#define PAGE7_FLINE_STATE								(0x0706)
#define PAGE7_FLINE_SENSOR1_STATE						(0x0707)
#define PAGE7_FLINE_SENSOR2_STATE						(0x0708)
#define PAGE7_FLINE_SENSOR3_STATE						(0x0709)
#define PAGE7_FLINE_SENSOR1_COMM						(0x070A)
#define PAGE7_FLINE_SENSOR2_COMM						(0x070B)
#define PAGE7_FLINE_SENSOR3_COMM						(0x070C)
#define PAGE7_FLINE_OPENALARM							(0x070D)
#define PAGE7_FLINE_REGULATINGALARM						(0x070E)
#define PAGE7_FLINE_CURRENTALARM						(0x070F)
#define PAGE7_FLINE_OPENPHASEALARM						(0x0710)
#define PAGE7_FLINE_STOP_MANUAL							(0x0711)
#define PAGE7_FLINE_RUNNING_MANUAL						(0x0712)
#define PAGE7_FLINE_RUNNING_AUTO						(0x0713)

#define PAGE7_FLINE_CONTROL_BUTTON						(0x0720)
#define PAGE7_FLINE_STATE_BUTTON						(0x0721)

void Page7ControlFlineInit(void);
void Page7ControlFlineProcess(u8 reg, u16 addr, u8 *pbuf, u8 len);
void Page7ControlFlineRefresh(void);

#endif // _PAGE7CONTROLFLINE_H_



