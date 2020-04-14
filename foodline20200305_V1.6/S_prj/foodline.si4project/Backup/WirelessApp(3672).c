#include <stddef.h>
#include "System.h"
#include "Os_Wrap.h"
#include "Os_TaskPrio.h"
#include "WirelessComm.h"
#include "Wlan4GComm.h"
#include "AirIap.h"
#include "WirelessApp.h"


extern u32 gLocalDeviceId;
extern u32 gCtrlPlatformId;
extern u32 gNineSensorId;
//static u32 gRcDeviceId;
//static u32 gTimeCont;

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

//static u16 recordRc[256];	// 调试用，需要删除
static u8 temp;
static u16 tempValue;

void WirelessApp_RcvMsg(u8 cmd, u16 seq, u8 *msg, u16 len, u32 saddr, u32 daddr, u32 srcid, u8 flag)
{
    DevicePara tempDevice, tempLocal, tempdaddr;
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
            switch(msg[6])
            {
                case DEVICE_CMD_PARA_CONFIG:
                    if(tempDevice.place.type == DEVICE_NAME_FLINE)
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
                    break;
                case DEVICE_CMD_PATH_CONFIG:
                    if(msg[7] == 1)
                    {
                    }
                    else if(msg[7] == 2)
                    {
                        PathParameterGet()->alarm.path = msg[5];
                        PathParameterGet()->alarm.id = (msg[8]<<8 | msg[9]);
                    }
                    break;
                case DEVICE_CMD_READ_PATH:
                    if(len - 7 > 0 && ReadPathGet()->path == msg[5])
                    {
                        u8 i, num;
                        num = (len - 7) / 2;
                        for(i = 0; i < num; i++)
                        {
                            ReadPathGet()->rcPara[i].placeRc.rc0 = msg[7+i*2];
                            ReadPathGet()->rcPara[i].placeRc.rc1 = msg[8+i*2];
                            if(ReadPathGet()->rcPara[i].placeNew.useID == 0)
                                break;
                        }
                    }
                    break;
                case DEVICE_CMD_FLINE_SET:
                    if(tempDevice.place.type == DEVICE_NAME_FLINE)
                        FoodlineControlGet()->rcAlarm = msg[7];
                    else if(tempDevice.place.type == DEVICE_NAME_IN_TOWERS || tempDevice.place.type == DEVICE_NAME_MAIN_VICE)
                        SwitchValveControlGet()->rcAlarm = msg[7];
                    else if(tempDevice.place.type == DEVICE_NAME_TOWERS_OUT)
                        TowersOutControlGet()->rcAlarm = msg[7];
                    break;
                case DEVICE_CMD_FLINE_READ:
                    if(tempDevice.place.type == DEVICE_NAME_FLINE)
                    {
                        if(FOODLINECONTROL_RC_DATA_LEN == msg[7])
                        {
                            u8 index = 8;
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
                    }
                    else if(tempDevice.place.type == DEVICE_NAME_IN_TOWERS || tempDevice.place.type == DEVICE_NAME_MAIN_VICE)
                    {
                        if(SVALVECONTROL_RC_DATA_LEN == msg[7])
                        {
                            u8 index = 8;
                            SwitchValveControlGet()->onoff.buf = msg[index++];
                            SwitchValveControlGet()->tcock = msg[index++];
                            SwitchValveControlGet()->alarm.buf = msg[index++];
                            SwitchValveControlGet()->manualAuto = msg[index++];
                        }                        
                    }
                    else if(tempDevice.place.type == DEVICE_NAME_TOWERS_OUT)
                    {
                        if(TOWERS_OUT_RC_DATA_LEN == msg[7])
                        {
                            u8 index = 8;
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

