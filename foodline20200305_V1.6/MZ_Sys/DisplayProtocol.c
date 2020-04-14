/*****************************************************************
 * File: DisplayProtocol.c
 * Date: 2018/03/21 19:58
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
 ******************************************************************/
#include "DisplayProtocol.h"
#include "DisplayPara.h"
#include "System.h"
#include "Page1flinePathConfig.h"
#include "Page2PathConfig.h"
#include "Page3FlineDevice.h"
#include "Page5Revise.h"
#include "Page6ReadPathConfig.h"
#include "Page7ControlFline.h"
#include "Page8SwitchValve.h"
#include "Page9SwitchValveConfig.h"
#include "Page10TowersOut.h"
#include "Page11TowersOutConfig.h"
#include "Page12S1S2Control.h"
#include "Page13LinkConfig.h"
#include "Page14W1Control.h"
#include "Page15W2Control.h"
#include "Page17C1Control.h"
#include "Page18C2Control.h"
#include "Page19N1Control.h"
#include "Page20N2Control.h"
#include "Page16E1E2Control.h"

bool DisplayCommSetTime(SystemTime setTime);
// display
OS_MsgBoxHandle gDisplaySend_Queue;
OS_MsgBoxHandle gDisplayMsg_Queue;
OS_MsgBoxHandle gDisplayNotify_Queue;
u8 gDisplayMsgBody[WIRELESS_RCVBODY_COUNT][128];
u8 gDisplayMsgSend[WIRELESS_MSGSND_COUNT][128];
u8 gDisplayMsgBodyLen[WIRELESS_RCVBODY_COUNT];
u8 gDisplayMsgSendLen[WIRELESS_MSGSND_COUNT];

static u8 DisplaySndMsgs;
//static bool gIsVersion = FALSE;

void DisplayComm_Init(void)
{
    COM2_Init();    
    DisplaySndMsgs = 0;        
    OS_MemSet(gDisplayMsgBodyLen, 0, DISPLAY_RCVBODY_COUNT);
    OS_MemSet(gDisplayMsgSendLen, 0, DISPLAY_MSGSND_COUNT);
	OS_MemSet(gDisplayMsgBody, 0, sizeof(gDisplayMsgBody));
	OS_MemSet(gDisplayMsgSend, 0, sizeof(gDisplayMsgSend));
	DisplayParaInit();
}
// len:Display Screen send dataLen(reg, addr, wordLen, dataBuf);
// 大小					    1     2      1            2
void DisplayAnalyseCommPacket(u8 *pbuf, u8 len)
{
	u8 reg = pbuf[0];
	u16 addr = (pbuf[1]<<8) | pbuf[2];	
	DisplayParaDealProcess(reg, addr, pbuf+4, len-4);
}


void DisplayCommRecv_Task(void *Task_Parameters)
{
    INPUT_EVENT event;
    u8 *pbuf;
    u8 err, len;
    
    while (1)
    {
        err = OS_MsgBoxReceive(gDisplayMsg_Queue, &event, 0);
		
        if (err == OS_NO_ERR)
        {
            if ((event.Type == Input_PeripCtrl) && (event.Subtype == Perip_Display_Comm))
            {
                len = event.Info.b[0];
                pbuf = &(gDisplayMsgBody[event.Info.b[1]][0]);
                
                if (len > 0)
                {
                    DisplayAnalyseCommPacket(pbuf, len);
                }
                gDisplayMsgBodyLen[event.Info.b[1]] = 0;
            }
        }	
    }
}

void DisplaySendCommPacket(u8 *pbuf, u8 len)
{
    u8 i = 0;
    
    if ((len < 2) || (len >= 128))
        return;
    
    while (i < len)
    {
	    USART_SendData(USART2, pbuf[i++]);
	    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    }
}


void DisplayCommSend_Task(void *Task_Parameters)
{
    INPUT_EVENT event;
    u8 *pbuf;
    u8 len, index;
    u8 err, exit, cnt;
//    u8 seq;
    while (1)
    {
        err = OS_MsgBoxReceive(gDisplaySend_Queue, &event, OS_MAX_DELAY);
        if (err != OS_NO_ERR)
            continue;
        
        if ((event.Type != Input_PeripCtrl) || (event.Subtype != Perip_Display_Comm))
            continue;
        
        len = event.Info.b[0];
        index = event.Info.b[1];
        pbuf = &(gDisplayMsgSend[index][0]);
//        seq = event.Info.b[2];
        
        exit = 0;
        cnt = 0;
        while (cnt++ < 1)
        {
            DisplaySendCommPacket(pbuf, len);
            
//            while (1)
//            {
//                err = OS_MsgBoxReceive(gDisplayNotify_Queue, &event, 10);
//                if (err == OS_TIMEOUT)
//                    break;
//                
//                if ((err == OS_NO_ERR) && (event.Num == 1))
//                {
//                    if (event.Info.b[0] == seq)
//                    {
//                        exit = 1;
//                        break;
//                    }
//                }
//            }
            
            if (exit)
                break;
        }
        gDisplayMsgSendLen[index] = 0;
    }
}

void DisplayParaRefresh_Task(void *Task_Parameters)
{
//    static u32 gCount;
    OSTimeDly(300);
    
//	DisplayCommSetBacklight(AdminConfigGet()->screen_light);// 屏保亮度
    while(1)
    {
		switch(ScrInfGet()->currentPage)
		{
			case 1:
				Page1FlinePathConfigRefresh();
				break;
            case 2:
                Page2PathConfigRefresh();
                break;
            case 3:
                Page3FlineDeviceRefresh();
                break;
            case 5:
                Page5ReviseRefresh();
                break;
            case 6:
                Page6PathConfigRefresh();
                break;
            case 7:
                Page7ControlFlineRefresh();
                break;
            case 8:
                Page8SwitchValveRefresh();
                break;
            case 9:
                Page9SwitchValveConfigRefresh();
                break;
            case 10:
                Page10TowersOutRefresh();
                break;
            case 11:
                Page11TowersOutConfigRefresh();
                break;
            case 12:
                Page12S1S2ControlRefresh();
                break;
            case 13:
                Page13LinkConfigRefresh();
                break;      
            case 14:
                Page14W1ControlRefresh();
                break;   
            case 15:
                Page15W2ControlRefresh();
                break;   
            case 16:
                Page16E1E2ControlRefresh();
                break;   
            case 17:
                Page17C1ControlRefresh();
                break;   
            case 18:
                Page18C2ControlRefresh();
                break;   
            case 19:
                Page19N1ControlRefresh();
                break;   
            case 20:
                Page20N2ControlRefresh();
                break; 			
		}
//	    OSTimeDly(1000);	// 5s
		OSTimeDly(200);	// 1s

    }
}

// addr :目标地址；buf: 数据数组; bufLen: sizeof(buf)
bool DisplayCommTextSend(u16 addr, u8 *buf, u16 bufLen)
{
	if(buf == NULL)
		return FALSE;
	
	return(DisplayComm_SendData(CTRL_TYPE_WORD, bufLen+3, VARMEM_W, addr, buf));
}

bool DisplayCommIconSend(u16 addr, u16 index)
{
	u8 tempIconBuf[2] = {0};
	tempIconBuf[0] = index >> 8;
	tempIconBuf[1] = index & 0xFF;
	return(DisplayComm_SendData(CTRL_TYPE_WORD, 5, VARMEM_W, addr, tempIconBuf));
}

bool DisplayCommTextNumberSend(u16 addr, u32 number, u8 pointNum)
{
	u8 tempNumberBuf[8], count = 0;
	u32 tempN = number;
	while(tempN != 0)
	{
		tempN /= 10;
		count++;
	}
	switch(pointNum)
	{
		case 0:
			sprintf((char*)tempNumberBuf, "%d", number);
			break;
		case 1:
			sprintf((char*)tempNumberBuf, "%d.%d", number/10, number%10);
			break;
		case 2:
			sprintf((char*)tempNumberBuf, "%d.%d", number/100, number%100);
			break;
		case 3:
			sprintf((char*)tempNumberBuf, "%d.%d", number/1000, number%1000);
			break;
		default:
			sprintf((char*)tempNumberBuf, "%d", number);
			break;
	}
	if(pointNum > 0 )
		count++;
	
	return(DisplayComm_SendData(CTRL_TYPE_WORD, 3+count, VARMEM_W, addr, tempNumberBuf));
}

bool DisplayCommNumSend(u16 addr, u16 number)
{
	u8 tempNumBuf[2];
	tempNumBuf[0] = number >> 8;
	tempNumBuf[1] = number & 0xFF;
	return(DisplayComm_SendData(CTRL_TYPE_WORD, 5, VARMEM_W, addr, tempNumBuf));
}

bool DisplayCommNumBufSend(u16 addr, u16 *buf, u16 len)
{
	u16 i, j;
	u8 tempNumBuf[40];
	for(i = 0, j = 0; j < len; i+=2, j++)
	{
		tempNumBuf[i] = (u8)(buf[j] >> 8);
		tempNumBuf[i+1] = (u8)(buf[j] & 0xFF);
	}
	return(DisplayComm_SendData(CTRL_TYPE_WORD, 3+len*2, VARMEM_W, addr, tempNumBuf));
}

bool DisplayCommNum32Send(u16 addr, u32 number)
{
	u8 tempNumBuf[4];
	tempNumBuf[0] = number >> 24;
	tempNumBuf[1] = number >> 16;
	tempNumBuf[2] = number >> 8;
	tempNumBuf[3] = number & 0xFF;
	return(DisplayComm_SendData(CTRL_TYPE_WORD, 7, VARMEM_W, addr, tempNumBuf));
}

bool DisplayCommNum32BufSend(u16 addr, u32 *buf, u16 len)
{
	u16 i, j;
	u8 tempNumBuf[80];
	for(i = 0, j = 0; j < len; i+=4, j++)
	{
		tempNumBuf[i] 	= (u8)(buf[j] >> 24);
		tempNumBuf[i+1] = (u8)(buf[j] >> 16);
		tempNumBuf[i+2] = (u8)(buf[j] >> 8);
		tempNumBuf[i+3] = (u8)(buf[j] & 0xFF);
	}
	return(DisplayComm_SendData(CTRL_TYPE_WORD, 3+len*4, VARMEM_W, addr, tempNumBuf));
}

bool DisplayCommTimeSend(u16 addr, u16 time)
{
	u8 tempTimeBuf[2];
	u16 tempTime;
	tempTime = TimeDeciConvertHex(time);
	tempTimeBuf[0] = (tempTime >> 8);
	tempTimeBuf[1] = tempTime & 0xFF;
	return(DisplayComm_SendData(CTRL_TYPE_WORD, 5, VARMEM_W, addr, tempTimeBuf));
}
bool DisplayCommPopup(u8 keyIndex)
{
	u8 tempPopupBuf[1] = {0};
	tempPopupBuf[0] = keyIndex & 0xFF;
	return(DisplayComm_SendData(CTRL_TYPE_BYTE, 3, LOCAL_REG_W, POPUP_ADDR, tempPopupBuf));	
}
bool DisplayCommJumpToPic(u8 picNo)
{
	u8 tempPicNoBuf[2] = {0};
	tempPicNoBuf[1] = picNo & 0xFF;
	return(DisplayComm_SendData(CTRL_TYPE_BYTE, 4, LOCAL_REG_W, JUMP_TO_PIC_ADDR, tempPicNoBuf));	
}
// 等级: 0~100
bool DisplayCommSetBacklight(u8 scrsavLight)
{
	u8 tempBacklightBuf[1] = {0};
	tempBacklightBuf[0] = (scrsavLight/2+10) & 0xFF;
	return(DisplayComm_SendData(CTRL_TYPE_BYTE, 3, LOCAL_REG_W, BACKLIGHT_ADDR, tempBacklightBuf));	
}
bool DisplayCommReadBacklight(void)
{
	u8 tempBacklightRBuf[1] = {0};
	tempBacklightRBuf[0] = (10/2+10) & 0xFF;	
	return(DisplayComm_SendData(CTRL_TYPE_BYTE, 3, LOCAL_REG_R, BACKLIGHT_ADDR, tempBacklightRBuf));	
}
bool DisplayCommSetColor(u16 pointerAddr, u16 color)
{
	u8 tempColorBuf[2];
	tempColorBuf[0] = (color >> 8);
	tempColorBuf[1] = color & 0xFF;
	return(DisplayComm_SendData(CTRL_TYPE_WORD, 5, VARMEM_W, pointerAddr+3, tempColorBuf));	// 指针在偏移3位置
}
bool DisplayCommSetCurveColor(u16 pointerAddr, u16 color)
{
	u8 tempColorBuf[2];
	tempColorBuf[0] = (color >> 8);
	tempColorBuf[1] = color & 0xFF;
	return(DisplayComm_SendData(CTRL_TYPE_WORD, 5, VARMEM_W, pointerAddr+7, tempColorBuf));	// 指针在偏移3位置
}
bool DisplayCommSetScreenBps(u8 bps)
{
	u8 tempbpsBuf[1] = {0};
	tempbpsBuf[0] = bps & 0xFF;
	return(DisplayComm_SendData(CTRL_TYPE_BYTE, 3, LOCAL_REG_W, SCREEN_SET_BPS_ADDR, tempbpsBuf));	
}
bool DisplayCommSetTP_LED(u8 isScrPro)// 1:enable screen saver;  0: disable
{
	u8 tempScrProBuf[1] = {0};
	if(isScrPro)
		tempScrProBuf[0] = 0x20 | 0x08 | 0x04;
	else
		tempScrProBuf[0] = 0x08 | 0x04;
	return(DisplayComm_SendData(CTRL_TYPE_BYTE, 3, LOCAL_REG_W, WORK_MODE_ADDR, tempScrProBuf));	
}
bool DisplayCommSetTP_LEDBackLight(u8 tpBacklight)
{
	u8 temptpBacklightBuf[1] = {0};
	temptpBacklightBuf[0] = (tpBacklight/2+10) & 0xFF;
	return(DisplayComm_SendData(CTRL_TYPE_BYTE, 3, LOCAL_REG_W, START_BACKLIGHT_ADDR, temptpBacklightBuf));	
}
bool DisplayCommSetTP_LEDScrProLight(u8 scrProLight)// 屏保亮度
{
	u8 tempscrProLightBuf[1] = {0};
	tempscrProLightBuf[0] = (scrProLight/2+10) & 0xFF;
	return(DisplayComm_SendData(CTRL_TYPE_BYTE, 3, LOCAL_REG_W, SCREEN_PROTECTION_LIGHT_ADDR, tempscrProLightBuf));	
}
bool DisplayCommSetTP_LEDScrProTime(u8 scrProTime)
{

	u8 tempscrProTimeBuf[1] = {0};
	tempscrProTimeBuf[0] = scrProTime & 0xFF;
	return(DisplayComm_SendData(CTRL_TYPE_BYTE, 3, LOCAL_REG_W, SCREEN_PROTECTION_TIME_ADDR, tempscrProTimeBuf));	
}
bool DisplayCommSetCONFIG_EN(u8 isEN)
{
	u8 tempIsENBuf[1] = {0};
	if(isEN)
		tempIsENBuf[0] = 0x5A;
	else
		tempIsENBuf[0] = 0xA5;
	return(DisplayComm_SendData(CTRL_TYPE_BYTE, 3, LOCAL_REG_W, SCREEN_CONFIG_EN_ADDR, tempIsENBuf));
}
void DisplayCommSetScreen(void)
{
	OSTimeDly(5);
//	DisplayCommSetScreenBps(SCREEN_BPS_INIT);
////	DisplayCommSetTP_LED(AdminConfigGet()->scrsav_switch);
////	DisplayCommSetTP_LEDBackLight(AdminConfigGet()->screen_light);
//	DisplayCommSetTP_LEDScrProLight(AdminConfigGet()->scrsav_light);// 屏保亮度
////	DisplayCommSetTP_LEDScrProTime(AdminConfigGet()->scrsav_keep);
//    
//	DisplayCommSetCONFIG_EN(1); // 设置并保存
}

// addr:	0x01 通道0
//		0x02 通道1
//		0x04 通道2
//		...	...
//		0x03 通道0 + 通道1
//		0x07 通道0 + 通道1 + 通道2
bool DisplayCommCurveSend(u16 addr, u8 *buf, u16 bufLen)
{
	if(buf == NULL)
		return FALSE;
	
	return(DisplayComm_SendData(CTRL_TYPE_BYTE, bufLen+2, CURVE_W, addr, buf));
}
// channel:1~8代表0~7通道
bool DisplayCommCleanCurveSend(u8 channel)
{	
	u8 tempCurveChannelBuf[1] = {0};
	if(channel <= 8)
		tempCurveChannelBuf[0] = channel+0x55;
	else
		tempCurveChannelBuf[0] = 0x55;
	return(DisplayComm_SendData(CTRL_TYPE_BYTE, 3, LOCAL_REG_W, CURVE_ADDR, tempCurveChannelBuf));
}
//// time
//bool DisplayCommReadTime(bool isVersion)
//{
//	u8 tempTimeRBuf[1] = {0};
//	gIsVersion = isVersion;
//	tempTimeRBuf[0] = 0x07;
//	return(DisplayComm_SendData(CTRL_TYPE_BYTE, 3, LOCAL_REG_R, READ_TIME_ADDR, tempTimeRBuf));	
//}

bool DisplayCommSetTime(SystemTime setTime)
{
	u8 tempTimeTxBuf[8] = {0};
	tempTimeTxBuf[0] = 0x5A;
	tempTimeTxBuf[1] = setTime.year;
	tempTimeTxBuf[2] = setTime.mon;
	tempTimeTxBuf[3] = setTime.day;
	tempTimeTxBuf[4] = 0;
	tempTimeTxBuf[5] = setTime.hour;
	tempTimeTxBuf[6] = setTime.min;
	tempTimeTxBuf[7] = 0;
	return(DisplayComm_SendData(CTRL_TYPE_BYTE, 10, LOCAL_REG_W, SET_TIME_ADDR, tempTimeTxBuf));	
}

// ctrl: 
// 0: data channel,  1: reg channel, 
// len: data length
// reg: reg cmd
// dstaddr:dest addr. ctrl=0,2Byte;ctrl=1,1Byte
bool DisplayComm_SendData(CTRL_TYPE ctrl, u8 len, u8 reg, u16 dstaddr, u8 *msg)
{
    INPUT_EVENT evt;
    u8 *sndmsg;
	u8 DisplayIndex;
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register*/
		OS_CPU_SR  cpu_sr = 0;
#endif
    
    if (len <= 100 && msg != NULL)
    {
        if (gDisplayMsgSendLen[DisplaySndMsgs] == 0)
        {
            evt.Type = Input_PeripCtrl;
            evt.Subtype = Perip_Display_Comm;
            evt.Info.b[0] = len + 3;
			OS_ENTER_CRITICAL();
			DisplayIndex = DisplaySndMsgs;
			DisplaySndMsgs = (++DisplaySndMsgs)>= WIRELESS_MSGSND_COUNT?0:DisplaySndMsgs;
			OS_EXIT_CRITICAL();
            evt.Info.b[1] = DisplayIndex;
			evt.Info.b[2] = ctrl;
            
            if (OS_MsgBoxSend(gDisplaySend_Queue, &evt, OS_NO_DELAY, FALSE) != OS_ERR_MBOX_FULL)
            {
                sndmsg = &(gDisplayMsgSend[DisplayIndex][0]);
                memset(sndmsg, 0, 128);
                sndmsg[0] = HEAD_SIGN_H;
                sndmsg[1] = HEAD_SIGN_L;
                sndmsg[2] = len;
                sndmsg[3] = reg;
				if(ctrl == CTRL_TYPE_WORD)
				{
	                sndmsg[4] = (dstaddr >> 8) & 0xFF;
	                sndmsg[5] = dstaddr & 0xFF;
                    OS_MemCpy(sndmsg+6, msg, len-3);
				}
				else if(ctrl == CTRL_TYPE_BYTE)
				{
	                sndmsg[4] = dstaddr & 0xFF;
                    OS_MemCpy(sndmsg+5, msg, len-2);
				}
                
                gDisplayMsgSendLen[DisplayIndex] = len + 3;
                
                return TRUE;
            }
        }
    }
    
    return FALSE;
}


