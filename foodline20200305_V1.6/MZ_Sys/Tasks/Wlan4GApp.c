#include <stddef.h>
#include "Os_Wrap.h"
#include "Os_TaskPrio.h"
#include "WirelessComm.h"
#include "Wlan4GComm.h"
#include "Wlan4GApp.h"
#include "AirIap.h"
#include "CtrlOperate.h"
#include "SensorStat.h"
#include "ParaSettingLib.h"


extern u32 gUploadDeviceId;
extern u32 gLocalDeviceId;

extern u8 USimICCID[];


void Wlan4G_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);       // reset io
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    COM1_Init(115200);
    Wlan4GComm_Init(USART1);
    
    CreateW4GTask(2, WLAN4GLNK_TASK_PRIORITY, 9, WLAN4GPKT_TASK_PRIORITY, 4, WLAN4GSND_TASK_PRIORITY);
}


void Wlan4G_ResetIO(u8 val)
{
    PAout(0) = val;
}


void Wlan4G_Stat(u8 conn, u8 err)
{
    conn = conn;
    err = err;
}


void Wlan4G_Errinfo(u8 type, u8 *errmsg)
{
    
}


void Wlan4G_RecvMsg(u8 flag, u8 *buffer, u16 len, u32 srcid, u32 dstid)
{
    BIGMSG_EVENT evt;
    u8 *sndbuf;
    u32 dstaddr;
//	u8 tempCont = 0;
//	u8 spaceSize = 0; // 此设备所占字节数
//	u8 deviceBuf[64];
//    u8 msg[64];
//    u16 msgLen;
//    u16 tempTotalPiece;
    
    switch (flag)
    {
    case MIDWARE_DATA_WIRELESS:
        {
            sndbuf = evt.buffer+11;
            OS_MemCpy(sndbuf, buffer, len);
            
            if (dstid == gUploadDeviceId)
                WirelessComm_SendData(&evt, 2, len, gLocalDeviceId, gLocalDeviceId, 3);
            else
                WirelessComm_SendData(&evt, 2, len, gLocalDeviceId, dstid, 3);
        }
        break;
    
    case MIDWARE_DATA_MESSAGE:
        if (dstid == gUploadDeviceId)
        {
            dstaddr = ((u32)buffer[3] << 24) | ((u32)buffer[4] << 16) | ((u32)buffer[5] << 8) | buffer[6];
            if (dstaddr == gLocalDeviceId)
            {
                u32 srcaddr = ((u32)buffer[7] << 24) | ((u32)buffer[8] << 16) | ((u32)buffer[9] << 8) | buffer[10];
                switch (buffer[11])
                {
#if Board_NineSensor_4GGateway
                case ASK_CTRLPOLICY_VERSION:
                    RecvCtrlPolicyVer(buffer+12, len-14, srcaddr, dstaddr, srcid, 1);
                    break;
                case ASK_CTRLPOLICY_CONFIG:
                    RecvAskCtrlPolicy(buffer+12, len-14, srcaddr, dstaddr, srcid, 1);
                    break;
                case ASK_CTRLDEVS_STATUS:
                    RecvAskCtrlDevicesSt(buffer+12, len-14, srcaddr, dstaddr, srcid, 1);
                    break;
                case QUERY_NSENS_LOG_STATUS:
                    QueryLogEnvironmentStat(buffer+12, len-14, srcaddr, dstaddr, srcid, 1);
                    break;
                
                case ALARM_DEVICE_MASKCTRL:
                    RecvAlarmMaskCtrl(buffer[11], (buffer[12]<<8)|buffer[13], buffer+14, len-16, srcaddr, dstaddr, srcid, 1);
                    break;
                
                case ALARM_DEVICE_CONTROL:
                case ALARM_DEVICE_ASKSTATUS:
                    RecvAlarmStatus(buffer+12, len-14, srcaddr, dstaddr, srcid, 1);
                    break;
                
                #if 1
                case 0xFB:
                    {
                        u32 spiaddr = (buffer[12]<<24)|(buffer[13]<<16)|(buffer[14]<<8)|buffer[15];
                        StorageEraseSector(spiaddr & 0xfffff000);
                    }
                    break;
                #endif
#endif
                
                case ASK_IAPVERSON:
                case ASK_IAPDATA:
                case ASK_IAPINFO:
                case ASK_CHIPGUID:
                case ADJUST_NETWORK_TIME:
                case CONFIG_LOCAL_DEVICE_PARA:
                case CONFIG_LOCAL_DEVICE_ID:
                case RESET_LOCAL_DEVICE:
                    if ((buffer[2] & FRAME_CTRL_MASK) == FRAME_NEED_NO_ACK)
                    {
                        RcvIapData(buffer[11], 0, buffer+12, len-14, srcaddr, dstaddr, srcid, 1);
                    }
                    break;
                
                case READ_LOCAL_DEVICE_PARA:
                    if ((buffer[2] & FRAME_CTRL_MASK) == FRAME_NEED_ACK)
                    {
                        RcvIapData(buffer[11], (buffer[12]<<8)|buffer[13], buffer+14, len-16, srcaddr, dstaddr, srcid, 1);
                    }
                    break;
                
                case GET_WLAN4G_USIM_CCID:
                    if ((buffer[2] & FRAME_CTRL_MASK) == FRAME_NEED_ACK)
                    {
                        u8 retbuf[40];
                        retbuf[0] = 0x7e;
                        retbuf[1] = (USimICCID[0] == 0)?14:34;
                        retbuf[2] = FRAME_JUST_ACK;
                        retbuf[3] = (u8)(srcaddr>>24);
                        retbuf[4] = (u8)(srcaddr>>16);
                        retbuf[5] = (u8)(srcaddr>>8);
                        retbuf[6] = (u8)(srcaddr);
                        retbuf[7] = (u8)(dstaddr>>24);
                        retbuf[8] = (u8)(dstaddr>>16);
                        retbuf[9] = (u8)(dstaddr>>8);
                        retbuf[10] = (u8)(dstaddr);
                        retbuf[11] = GET_WLAN4G_USIM_CCID;
                        retbuf[12] = buffer[12];
                        retbuf[13] = buffer[13];
                        retbuf[14] = USimICCID[0];
                        if (USimICCID[0] == 0)
                        {
                            retbuf[15] = docrc8(0, retbuf+1, 14);
                            retbuf[16] = 0x7e;
                            Wlan4G_SendData(retbuf, 17, gUploadDeviceId, srcid, MIDWARE_DATA_MESSAGE, 1);
                        }
                        else
                        {
                            OS_MemCpy(retbuf+15, USimICCID+1, 20);
                            retbuf[35] = docrc8(0, retbuf+1, 34);
                            retbuf[36] = 0x7e;
                            Wlan4G_SendData(retbuf, 37, gUploadDeviceId, srcid, MIDWARE_DATA_MESSAGE, 1);
                        }
                    }
                    break;
                
#if Board_NineSensor_4GGateway || Board_WeatherStation_4G
                case 0xFD:
                    {
                        u8 retbuf[80];
                        u32 spiaddr = (buffer[12]<<24)|(buffer[13]<<16)|(buffer[14]<<8)|buffer[15];
                        StorageRead(spiaddr, 64, retbuf+12);
                        retbuf[0] = 0x7e;
                        retbuf[1] = 11+64;
                        retbuf[2] = FRAME_NEED_NO_ACK;
                        retbuf[3] = (u8)(srcaddr>>24);
                        retbuf[4] = (u8)(srcaddr>>16);
                        retbuf[5] = (u8)(srcaddr>>8);
                        retbuf[6] = (u8)(srcaddr);
                        retbuf[7] = (u8)(dstaddr>>24);
                        retbuf[8] = (u8)(dstaddr>>16);
                        retbuf[9] = (u8)(dstaddr>>8);
                        retbuf[10] = (u8)(dstaddr);
                        retbuf[11] = 0xFD;
                        retbuf[12+64] = docrc8(0, retbuf+1, 11+64);
                        retbuf[13+64] = 0x7e;
                        Wlan4G_SendData(retbuf, 14+64, gUploadDeviceId, srcid, MIDWARE_DATA_MESSAGE, 0);
                    }
                    break;
#endif
                
#if Board_WeatherStation_4G
                case ASK_OUTERSENSOR_LOG:
                    AskOutSensorLog(buffer+12, len-14, srcaddr, dstaddr, srcid, 1);
                    break;
#endif
                
                default:
                    break;
                }
            }
#if Board_NineSensor_4GGateway || Board_Middleware_4GGateway
            else
            {
                sndbuf = evt.buffer+10;
                OS_MemCpy(sndbuf, buffer, len);
                
                // forward
                WirelessComm_SendData(&evt, 0, len, gLocalDeviceId, dstaddr, 3);
            }
#endif
        }
        break;
    
    default:
        break;
    }
}



