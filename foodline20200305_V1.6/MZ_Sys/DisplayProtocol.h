/*****************************************************************
 * File: DisplayProtocol.h
 * Date: 2018/03/21 19:58
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _DISPLAYPROTOCOL_H_
#define _DISPLAYPROTOCOL_H_
#include "System.h"
#include "ParaSettingLib.h"

#define ERROR_CODE					(0xFFFF)

#define HEAD_SIGN_H					(0xA5)	//帧头
#define HEAD_SIGN_L					(0x5A)
#define MIN_PROTOCOL_LENGTH			(7)
#define MAX_PROTOCOL_LENGTH			(128)

#define RECV_DISPLAY_HEAD_H			(0)
#define RECV_DISPLAY_HEAD_L			(1)
#define RECV_DISPLAY_CMD_LEN		(2)	// 一帧数据长度减去帧头和自身  (-3)
#define RECV_DISPLAY_REG			(3)
#define RECV_DISPLAY_ADDR_H			(4)
#define RECV_DISPLAY_ADDR_L			(5)
#define RECV_DISPLAY_DATA_LEN		(6)	// 数据区长度(单位word)

#define DISPLAY_RCVBODY_COUNT       (16)
#define DISPLAY_MSGSND_COUNT        (16)

typedef enum
{
	CTRL_TYPE_WORD = 0,
	CTRL_TYPE_BYTE,
	CTRL_TYPE_MAX,
}CTRL_TYPE;

typedef struct
{
	u8 hourHex;
	u8 minHex;
	u8 hourDeci;
	u8 minDeci;
}TimeConf;

void DisplayComm_Init(void);


void DisplayCommRecv_Task(void *Task_Parameters);
void DisplayCommSend_Task(void *Task_Parameters);
void DisplayParaRefresh_Task(void *Task_Parameters);
bool DisplayComm_SendData(CTRL_TYPE ctrl, u8 len, u8 reg, u16 dstaddr, u8 *msg);
bool DisplayCommTextSend(u16 addr, u8 *buf, u16 bufLen);
bool DisplayCommIconSend(u16 addr, u16 index);
bool DisplayCommTextNumberSend(u16 addr, u32 number, u8 pointNum);
bool DisplayCommNumSend(u16 addr, u16 number);
bool DisplayCommNumBufSend(u16 addr, u16 *buf, u16 len);
bool DisplayCommNum32Send(u16 addr, u32 number);
bool DisplayCommNum32BufSend(u16 addr, u32 *buf, u16 len);
bool DisplayCommSetBacklight(u8 scrsavLight);
bool DisplayCommReadBacklight(void);
bool DisplayCommSetColor(u16 pointerAddr, u16 color);
bool DisplayCommSetCurveColor(u16 pointerAddr, u16 color);
bool DisplayCommSetTP_LED(u8 isScrPro);// 1:enable screen saver;  0: disable
bool DisplayCommSetTP_LEDScrProLight(u8 scrProLight);// 屏保亮度
bool DisplayCommSetTP_LEDBackLight(u8 tpBacklight);
bool DisplayCommSetTP_LEDScrProTime(u8 scrProTime);
bool DisplayCommSetCONFIG_EN(u8 isEN);
void DisplayCommSetScreen(void);

bool DisplayCommTimeSend(u16 addr, u16 time);
bool DisplayCommCurveSend(u16 addr, u8 *buf, u16 bufLen);
bool DisplayCommCleanCurveSend(u8 channel);
bool DisplayCommPopup(u8 keyIndex);
bool DisplayCommJumpToPic(u8 picNo);
bool DisplayCommReadTime(bool isVersion);
//bool DisplayCommSetTime(SystemTime setTime);

#endif // _DISPLAYPROTOCOL_H_
