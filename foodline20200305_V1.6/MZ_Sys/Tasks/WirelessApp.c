#include <stddef.h>
#include "System.h"
#include "Os_Wrap.h"
#include "Os_TaskPrio.h"
#include "WirelessComm.h"
#include "Wlan4GComm.h"
#include "AirIap.h"
#include "WirelessApp.h"
#include "FoodlineControl.h"



extern OS_MsgBoxHandle gControlSendNotify_Queue;

extern u32 gLocalDeviceId;
extern u32 gCtrlPlatformId;
extern u32 gNineSensorId;
//static u32 gRcDeviceId;
//static u32 gTimeCont;
extern bool NormalStopFlag;   //  位置  StateMachine.h
extern bool foodDownPlaceArrivalFlag;   //  位置  StateMachine.h
typedef struct
{
	u8 cmdState;
	u8 cmdSeqH;
	u8 cmdSeqL;
}RcCmdPara;

//static RcCmdPara gRcCmdPara;
//extern void ParaDeviceRcUpdate(u8* pbuf, u16 len);
void WirelessApp_Init(u8 use4g)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);       // reset io
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);                      // reset io
    
    Wireless_ResetIO(0);
    
    COM3_Init();
    
    WirelessComm_Init(USART3, use4g, 16);
    
    CreateWrelessTask(9, WIRELESSRCV_TASK_PRIORITY, 4, WIRELESSSND_TASK_PRIORITY, 4, WIRELESSRTRAN_TASK_PRIORITY);
    
    OSTimeDly(100);
    Wireless_ResetIO(1);
}


void SetLocalDeviceConfig(u8 *cfgbuf)
{
    u8 buffer[32];
    
    // do config actions here, max 31 bytes
    OS_MemCopy(buffer, cfgbuf, 31);
    
    if (WriteLocalConfig(buffer) == 0)
    {
        if (ReadLocalConfig(buffer) == 0)
            Sys_Soft_Reset();
    }
}


void WirelessDebug(u8 *pbuf)
{
    
}
#define HISTORY_MAX_NUM       3
#define HISTORY_MAX_SEQ_NUM   5

typedef struct SEQ_HISTORY
{
    u32 Srcaddr;
    u8  SeqArr[HISTORY_MAX_SEQ_NUM];
    u8  OldSeqIndex;
}SEQ_HISTORY_STRUCT;

static SEQ_HISTORY_STRUCT s_SeqHistory[HISTORY_MAX_NUM];
static u8 s_cOldestIndex = HISTORY_MAX_NUM;

static u8 FindSrcAddrIndex(u32 srcaddr)
{
    u8 i ;
    for(i = 0; i < HISTORY_MAX_NUM; i++)
    {
        if(s_SeqHistory[i].Srcaddr == srcaddr)
        {
            return i;
        }
    }
    if(++s_cOldestIndex >= HISTORY_MAX_NUM)
    {
        s_cOldestIndex = 0;
    }
    s_SeqHistory[s_cOldestIndex].Srcaddr = srcaddr;
    OS_MemSet(&s_SeqHistory[s_cOldestIndex].SeqArr,0xFF,sizeof(s_SeqHistory[s_cOldestIndex].SeqArr));
    return s_cOldestIndex;
}

static bool IsSeqInRecord(u8 srcaddrIndex,u8 seq)
{
    u8 i;
    if(srcaddrIndex >= HISTORY_MAX_NUM)
    {
        return FALSE;
    }
    for(i = 0; i < HISTORY_MAX_SEQ_NUM;i++)
    {
        if(s_SeqHistory[srcaddrIndex].SeqArr[i] == seq)
        {
            return TRUE;
        }
    }
    s_SeqHistory[srcaddrIndex].OldSeqIndex = (++s_SeqHistory[srcaddrIndex].OldSeqIndex >= HISTORY_MAX_SEQ_NUM)?0:s_SeqHistory[srcaddrIndex].OldSeqIndex;
    s_SeqHistory[srcaddrIndex].SeqArr[s_SeqHistory[srcaddrIndex].OldSeqIndex] = seq;
    return FALSE;
}

//static u16 recordRc[256];	// 调试用，需要删除
static u8 temp;
//static u16 tempValue;

void WirelessApp_RcvMsg(u8 cmd, u16 seq, u8 *msg, u16 len, u32 saddr, u32 daddr, u32 srcid, u8 flag)
{
    DevicePara tempDevice, tempLocal, tempdaddr;
    u8 i,j;
	u8 deviceConfirmFlag = 0;
	
    INPUT_EVENT evt;
    tempDevice.id = 0;
    tempLocal.id = 0; 
    tempdaddr.id = 0;
    if (daddr == gLocalDeviceId)
    {
        ParaRefreshGet()->count = 1;
        if(cmd == 0x30)
        {
            if(len >= 7)
            {
                tempDevice.placeRc.rc1 = msg[0];
                tempDevice.placeRc.rc0 = msg[1];
                tempdaddr.placeRc.rc1 = msg[2];
                tempdaddr.placeRc.rc0 = msg[3];
                tempLocal.id = gLocalDeviceId;
            }
            else
                return;

            if(tempdaddr.placeNew.useID != tempLocal.placeNew.useID)
                return;
            if(DeviceControlParaGet()->rcDevice.placeNew.useID == tempDevice.placeNew.useID)
            {
                DeviceControlParaGet()->rcDevice.id = 0;
                if(DeviceControlParaGet()->maxTime[4] == msg[4])
                {
                    DeviceControlParaGet()->maxTime[1] = OSTimeGet();
                    DeviceControlParaGet()->maxTime[2] = DeviceControlParaGet()->maxTime[1] - DeviceControlParaGet()->maxTime[0];
                    if(DeviceControlParaGet()->maxTime[3] < DeviceControlParaGet()->maxTime[2])
                        DeviceControlParaGet()->maxTime[3] = DeviceControlParaGet()->maxTime[2];
                }
            }
            switch(msg[6])
            {
                case DEVICE_CMD_PARA_CONFIG:
                    if(tempDevice.place.type == DEVICE_NAME_FLINE ||tempDevice.place.type == DEVICE_NAME_VICE_FLINE)
                    {
                        if(len == FOODLINEDEVICE_RC_DATA_LEN+7)
                        {
                            temp = 7;
                            FoodlineDeviceParaGet()->shield.buf = msg[temp++];
                            FoodlineDeviceParaGet()->stateTime = msg[temp++];
                            FoodlineDeviceParaGet()->motorMintime = msg[temp++];
                            FoodlineDeviceParaGet()->overCurMax = msg[temp++];
                            FoodlineDeviceParaGet()->overCurMax = (FoodlineDeviceParaGet()->overCurMax<<8 | msg[temp++]);
                            FoodlineDeviceParaGet()->missPhaseMin = msg[temp++];
                            FoodlineDeviceParaGet()->missPhaseMin = (FoodlineDeviceParaGet()->missPhaseMin<<8 | msg[temp++]);
                            FoodlineDeviceParaGet()->curAlarmFilTime = msg[temp++];
                            FoodlineDeviceParaGet()->curCalib = msg[temp++];
                            FoodlineDeviceParaGet()->curCalib = (FoodlineDeviceParaGet()->curCalib<<8 | msg[temp++]);
                            FoodlineDeviceParaGet()->motorOnFilTime = msg[temp++];
                            FoodlineDeviceParaGet()->maleDevice.buf = msg[temp++];
                            FoodlineDeviceParaGet()->commSum = msg[temp++];
                        }
                    }
                    else if(tempDevice.place.type == DEVICE_NAME_IN_TOWERS || tempDevice.place.type == DEVICE_NAME_MAIN_VICE)
                    {
                        if(len == SWITCHVALVEPARA_RC_DATA_LEN+7)
                        {
                            temp = 7;
                            SwitchValveParaGet()->alarm.buf = msg[temp++];
                            SwitchValveParaGet()->stateTime = msg[temp++];
                            SwitchValveParaGet()->openOvertime = (msg[temp]<<8 | msg[temp+1]);
                        }
                    }
                    else if(tempDevice.place.type == DEVICE_NAME_TOWERS_OUT)
                    {
                        if(len == TOWERS_OUT_DEVICE_RC_DATA_LEN+7)
                        {
                            temp = 7;
                            TowersOutDeviceParaGet()->shield.buf = msg[temp++];
                            TowersOutDeviceParaGet()->stateTime = msg[temp++];
                            TowersOutDeviceParaGet()->motorMintime = msg[temp++];
                            TowersOutDeviceParaGet()->overCurMax = (msg[temp]<<8 | msg[temp+1]);
                            temp += 2;
                            TowersOutDeviceParaGet()->missPhaseMin = (msg[temp]<<8 | msg[temp+1]);
                            temp += 2;
                            TowersOutDeviceParaGet()->curAlarmFilTime = msg[temp++];
                            TowersOutDeviceParaGet()->curCalib = (msg[temp]<<8 | msg[temp+1]);
                            temp += 2;
                            TowersOutDeviceParaGet()->motorOnFilTime = msg[temp++];
                            TowersOutDeviceParaGet()->feedFilTime = msg[temp++];
                            TowersOutDeviceParaGet()->motorState = msg[temp++];
                        }
                    }
                    break;
                case DEVICE_CMD_PATH_CONFIG:
                    if(msg[7] == 1) // 设置成功
                    {
                        PathParameterGet()->alarm.path = 0;
                        PathParameterGet()->alarm.id = 0;
                    }
                    else if(msg[7] == 2) // 设置失败
                    {
                        PathParameterGet()->alarm.path = msg[5];
                        PathParameterGet()->alarm.id = (msg[8]<<8 | msg[9]);
                    }
                    
                    ParaRefreshGet()->count = 2;
                    break;
                case DEVICE_CMD_READ_PATH:
                    if(len - 7 > 0 && ReadPathGet()->path == msg[5])
                    {
                        u8 k, num;
                        num = (len - 7) / 2;
                        for(k = 0; k < num; k++)
                        {
                            ReadPathGet()->rcPara[k].placeRc.rc0 = msg[7+k*2];
                            ReadPathGet()->rcPara[k].placeRc.rc1 = msg[8+k*2];
                            if(ReadPathGet()->rcPara[k].placeNew.useID == 0)
                                break;
                        }
                    }
                    break;
                case DEVICE_CMD_FLINE_SET:
					
					ParaAnswer(saddr);
					if(IsSeqInRecord(FindSrcAddrIndex(saddr),msg[4]) == TRUE)
                    {
                        return;
                    }      
					evt.Info.b[0] = SEND_TYPE_CONTROL;
                    evt.Info.b[0] = msg[4];
                    evt.Num = 1;
                    OS_MsgBoxSend(gControlSendNotify_Queue, &evt, OS_NO_DELAY, FALSE);

                    if(tempDevice.place.type == DEVICE_NAME_FLINE || tempDevice.place.type == DEVICE_NAME_VICE_FLINE)
                        FoodlineControlGet()->rcAlarm = msg[7];
                    else if(tempDevice.place.type == DEVICE_NAME_IN_TOWERS || tempDevice.place.type == DEVICE_NAME_MAIN_VICE)
                        SwitchValveControlGet()->rcAlarm = msg[7];
                    else if(tempDevice.place.type == DEVICE_NAME_TOWERS_OUT)
                        TowersOutControlGet()->rcAlarm = msg[7];
                    break;
                case DEVICE_CMD_FLINE_READ:
					
					ParaAnswer(saddr);     
			
                    if(IsSeqInRecord(FindSrcAddrIndex(saddr),msg[4]) == TRUE)
                    {
                        return;
                    }
                
                    evt.Info.b[0] = SEND_TYPE_INQUIRE;         
                    evt.Info.b[0] = msg[4];
                    evt.Num = 1;
                    OS_MsgBoxSend(gControlSendNotify_Queue, &evt, OS_NO_DELAY, FALSE);

					for(j = 0; j < AREA_DEVICE_TOTAL_NUMBER; j++)
					{
						if(AllTheControlParaGet(j,0)->cDevice.placeNew.useID != 0)
						{
							for(i = 0; i < SING_LINK_DEVICE_TOTAL_NUMBER; i++)
							{
								if(AllTheControlParaGet(j,i)->cDevice.placeNew.useID != 0 )
								{
									if(AllTheControlParaGet(j,i)->cDevice.placeNew.useID == tempDevice.placeNew.useID)
									{
										deviceConfirmFlag = 1;
										break;
									}
								}
								else
									break;
							}
							if(deviceConfirmFlag)
							{
								deviceConfirmFlag = 0;
								break;
							}
						}
					}
                    if(tempDevice.place.type == DEVICE_NAME_FLINE || tempDevice.place.type == DEVICE_NAME_VICE_FLINE)
                    {
                        if(FOODLINECONTROL_RC_DATA_LEN == msg[7])
                        {
                            u8 index = 8;
                            if(PageXIdGet()->placeNew.useID == tempDevice.placeNew.useID)
                            {
                                FoodlineControlGet()->currentA = (msg[index]<<8 | msg[index+1]);
                                index += 2;
                                FoodlineControlGet()->currentB = (msg[index]<<8 | msg[index+1]);
                                index += 2;
                                FoodlineControlGet()->currentC = (msg[index]<<8 | msg[index+1]);
                                index += 2;
                                FoodlineControlGet()->state = msg[index++];
                                FoodlineControlGet()->sensorState.buf = msg[index++];
                                FoodlineControlGet()->sensorComm.buf = msg[index++];
                                FoodlineControlGet()->alarm.buf = msg[index++];
                                FoodlineControlGet()->manualAuto = msg[index++];
                            }
                            if(i != SING_LINK_DEVICE_TOTAL_NUMBER)
                            {                   								
                                AllTheControlParaGet(j,i)->stateByte = msg[14];
								AllTheControlParaGet(j,i)->onoff.buf = msg[15];
                                AllTheControlParaGet(j,i)->alarmByte = msg[17];
                                AllTheControlParaGet(j,i)->manualAuto = msg [18];

                            }
                        }
                    }				
                    else if(tempDevice.place.type == DEVICE_NAME_IN_TOWERS || tempDevice.place.type == DEVICE_NAME_MAIN_VICE)
                    {
                        if(SVALVECONTROL_RC_DATA_LEN == msg[7])
                        {
                            u8 index = 8;
                            if(PageXIdGet()->placeNew.useID == tempDevice.placeNew.useID)
                            {
                                SwitchValveControlGet()->onoff.buf = msg[index++];
                                SwitchValveControlGet()->tcock = msg[index++];
                                SwitchValveControlGet()->alarm.buf = msg[index++];
                                SwitchValveControlGet()->manualAuto = msg[index++];
                            }
                            if(i != SING_LINK_DEVICE_TOTAL_NUMBER)
                            {
                                AllTheControlParaGet(j,i)->onoff.buf = msg[8];
                                if(AllTheControlParaGet(j,i)->onoff.b.b0 == 1)
                                {
                                  //  foodUpPlaceArrivalFlag = TRUE;
                                }
                                if(AllTheControlParaGet(j,i)->onoff.b.b1 == 1)
                                {
                                  //  foodDownPlaceArrivalFlag = TRUE;
                                } 
                                AllTheControlParaGet(j,i)->stateByte = msg[9];
                                AllTheControlParaGet(j,i)->alarmByte = msg[10];
                                AllTheControlParaGet(j,i)->manualAuto = msg [11];
                            }
                        }                        
                    }
                    else if(tempDevice.place.type == DEVICE_NAME_TOWERS_OUT)
                    {
                        if(TOWERS_OUT_RC_DATA_LEN == msg[7])
                        {
                            u8 index = 8;
                            if(PageXIdGet()->placeNew.useID == tempDevice.placeNew.useID)
                            {
                                TowersOutControlGet()->currentA = (msg[index]<<8 | msg[index+1]);
                                index += 2;
                                TowersOutControlGet()->currentB = (msg[index]<<8 | msg[index+1]);
                                index += 2;
                                TowersOutControlGet()->currentC = (msg[index]<<8 | msg[index+1]);
                                index += 2;
                                TowersOutControlGet()->motorState = msg[index++];
                                TowersOutControlGet()->alarm.buf  = msg[index++];
                                TowersOutControlGet()->manualAuto = msg[index++];
                            }
                            if(i != SING_LINK_DEVICE_TOTAL_NUMBER)
                            {
//                                AllTheControlParaGet(i)->stateByte = TowersOutControlGet()->motorState;
//                                AllTheControlParaGet(i)->alarmByte = TowersOutControlGet()->alarm.buf;
                                AllTheControlParaGet(j,i)->stateByte = msg[14];
                                AllTheControlParaGet(j,i)->alarmByte = msg[15];
								AllTheControlParaGet(j,i)->onoff.buf = msg[15];								
                                AllTheControlParaGet(j,i)->manualAuto = msg [16];  
								if(AllTheControlParaGet((DEVICE_AREA_C - 1),1)->cDevice.placeNew.useID != 0
									&& AllTheControlParaGet((DEVICE_AREA_W - 1),1)->cDevice.placeNew.useID != 0)
								{
									AllTheControlParaGet((DEVICE_AREA_W - 1),1)->stateByte = AllTheControlParaGet((DEVICE_AREA_C - 1),1)->stateByte;						
									AllTheControlParaGet((DEVICE_AREA_W - 1),1)->alarmByte = AllTheControlParaGet((DEVICE_AREA_C - 1),1)->alarmByte;	
									AllTheControlParaGet((DEVICE_AREA_W - 1),1)->manualAuto = AllTheControlParaGet((DEVICE_AREA_C - 1),1)->manualAuto;	
								}
								if(AllTheControlParaGet((DEVICE_AREA_C - 1),2)->cDevice.placeNew.useID != 0
									&& AllTheControlParaGet((DEVICE_AREA_W - 1),2)->cDevice.placeNew.useID != 0)
								{
									AllTheControlParaGet((DEVICE_AREA_W - 1),2)->stateByte = AllTheControlParaGet((DEVICE_AREA_C - 1),1)->stateByte;						
									AllTheControlParaGet((DEVICE_AREA_W - 1),2)->alarmByte = AllTheControlParaGet((DEVICE_AREA_C - 1),1)->alarmByte;	
									AllTheControlParaGet((DEVICE_AREA_W - 1),2)->manualAuto = AllTheControlParaGet((DEVICE_AREA_C - 1),1)->manualAuto;	
								}
								if(AllTheControlParaGet((DEVICE_AREA_C - 1),3)->cDevice.placeNew.useID != 0
									&& AllTheControlParaGet((DEVICE_AREA_W - 1),3)->cDevice.placeNew.useID != 0)
								{
									AllTheControlParaGet((DEVICE_AREA_W - 1),3)->stateByte = AllTheControlParaGet((DEVICE_AREA_C - 1),1)->stateByte;						
									AllTheControlParaGet((DEVICE_AREA_W - 1),3)->alarmByte = AllTheControlParaGet((DEVICE_AREA_C - 1),1)->alarmByte;	
									AllTheControlParaGet((DEVICE_AREA_W - 1),3)->manualAuto = AllTheControlParaGet((DEVICE_AREA_C - 1),1)->manualAuto;	
								}

                            }
                        } 
                    }
						
										
                    break;
            }
        }
        
		if(cmd == 0x50)
		{
			u32 tempID;
			tempID = msg[0];
			tempID = tempID << 8;
			tempID += msg[1];
			tempID = tempID << 8;
			tempID += msg[2];
			tempID = tempID << 8;
			tempID += msg[3];
			if(tempID == saddr)
			{
				if(RcDeviceParaGet()->id != tempID)
				{
					RcDeviceParaGet()->id = tempID;
				}
			}
		}

        if(cmd == 0x31)
        {
            PathParameterGet()->alarm.next = 0;
        }

    }

    if (flag == FRAME_FLAG_RETRANS_TIMEOUT)
    {
        
    }
    else if (flag == FRAME_FLAG_PEER_MSG_ACK)
    {
        
    }
    else if (daddr == gLocalDeviceId)
    {
        // for terminal device
        switch (cmd)
            {
                case ASK_IAPVERSON:
                case ASK_IAPDATA:
                case ASK_IAPINFO:
                case ASK_CHIPGUID:
                case CONFIG_LOCAL_DEVICE_PARA:
                case READ_LOCAL_DEVICE_PARA:
                case CONFIG_LOCAL_DEVICE_ID:
                case ADJUST_NETWORK_TIME:
                case RESET_LOCAL_DEVICE:
                    RcvIapData(cmd, seq, msg, len, saddr, daddr, srcid, 0);
                    break;
                default: break;
            }
    }
}


void WirelessApp_ForwMsg(u8 *msg, u16 len, u32 saddr, u32 daddr, u8 flag, u8 ttl)
{
    // for middleware device, terminal or wireless module
    // flag: MIDWARE_DATA_MESSAGE or MIDWARE_DATA_WIRELESS
//    Wlan4G_SendData(msg, len, gLocalDeviceId, daddr, flag, 1);
}

void Wireless_ResetIO(u8 val)
{
     PBout(1) = val;                                             // reset io
}

