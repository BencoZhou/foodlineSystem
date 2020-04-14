/*****************************************************************
 * File: Page8SwitchValve.h
 * Date: 2019/11/28 18:50
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _PAGE8SWITCHVALVE_H_
#define _PAGE8SWITCHVALVE_H_

#include "System.h"

#define PAGE8_SVALVE_CONTROL							(0x0800)
#define PAGE8_SVALVE_UP_SENSOR							(0x0801)
#define PAGE8_SVALVE_DOWN_SENSOR						(0x0802)
#define PAGE8_SVALVE_TCOCK_OPEN							(0x0803)
#define PAGE8_SVALVE_TCOCK_CLOSE						(0x0804)
#define PAGE8_SVALVE_TCOCK_STATE						(0x0805)
#define PAGE8_SVALVE_ALARM_OPEN							(0x0806)
#define PAGE8_SVALVE_ALARM_CLOSE						(0x0807)
#define PAGE8_SVALVE_DEVICE_STATE						(0x0808)

#define PAGE8_SVALVE_CONTROL_BUTTON						(0x0809)
#define PAGE8_SVALVE_STATE_BUTTON						(0x080A)

#define PAGE8_TYPE1                                     (0x0820)	// 
#define PAGE8_AREA1                                     (0x0821)
#define PAGE8_MAIN_LINE1                                (0x0822)
#define PAGE8_VICE_LINE1                                (0x0823)
#define PAGE8_ID1                                       (0x0830)	// id 

void Page8SwitchValveInit(void);
void Page8SwitchValveProcess(u8 reg, u16 addr, u8 *pbuf, u8 len);
void Page8SwitchValveRefresh(void);

#endif // _PAGE8SWITCHVALVE_H_

