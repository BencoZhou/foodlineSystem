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
    if (daddr == gLocalDeviceId)
    {
        if(cmd == 0x30)
        {
            switch(msg[6])
            {
                case DEVICE_CMD_PARA_CONFIG:
                    if(len >= 19)
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
                    break;
            }
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

