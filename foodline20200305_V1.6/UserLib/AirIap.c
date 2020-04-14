#include <stddef.h>
#include "Os_Wrap.h"
#include "Os_TaskPrio.h"
#include "Rtc.h"
#include "flash.h"
#include "Delay.h"
#include "AirIap.h"
#include "Wlan4GComm.h"
#include "WirelessComm.h"
#include "WirelessApp.h"


#if AIRIAP_DBG
#define AIRIAP_Debug    Debug
#else
#define AIRIAP_Debug(x,y...)
#endif



extern u32 gUploadDeviceId;
extern u32 gLocalDeviceId;
extern u32 gCtrlPlatformId;
extern u8 gLocalInfoFromRom;


// FLASH_SIZE : 0x80000         // 512K
// FLASH_PAGE_SIZE : 0x800      // 2K
// FLASH_PAGE_MASK : 0x7ff

// FLASH_START : 0x08000000


#define BOOT_SIZE       (FLASH_PAGE_SIZE*5)     // 0x2800
#define APP_SIZE        (FLASH_PAGE_SIZE*120)                // max: 240K, temp store

#define BIN_APPFILE1    (FLASH_START+BOOT_SIZE)
#define BIN_IAPFILE2    (BIN_APPFILE1+APP_SIZE)

// MAC_ADDR : (FLASH_START+FLASH_SIZE-FLASH_PAGE_SIZE)
// CFG_ADDR : (FLASH_START+FLASH_SIZE-FLASH_PAGE_SIZE*2)
// IAPINFO_ADDR : (FLASH_START+FLASH_SIZE-FLASH_PAGE_SIZE*3)


static u32 mac_address;
static u32 paras_address;
static u32 iapinfo_address;


#define AirIap_Type_Msg     1

#define AirIap_AskVer_Msg   1
#define AirIap_Data_Msg     2


typedef struct
{
    u8 iap_rdy;
    u8 type;
    u8 mver;
    u8 vver;
    u32 verlen;
    
    u8 vercrc8;
    u8 errcnt;
    u8 reserv;
    
    u8 romcrc8;
} IAP_RECORD;


typedef struct
{
    IAP_RECORD record;
    
    u32 iaptime;
    u32 nettime;
    
    u32 updatesvrid;
    u32 updatesvraddr;
    u8 fromsrc;
    u8 reserv[3];
} IAP_INFO;


typedef struct
{
    u32 wraddr;
    u8 len;
    u8 type;
    u8 mver;
    u8 vver;
    u8 data[64];
} IAP_DATA;



static OS_MsgBoxHandle gIapMsg_Queue;

static void *IapReq_Task_Handle = NULL;
static void *IapDat_Task_Handle = NULL;

static u32 chip_guid[3];

static IAP_INFO DefInfo;
static IAP_INFO IapInfo;
static IAP_DATA RcvIapDt;

static u32 last_rcvtick = 0;
static u32 call_time = 0;

static u32 IapAskaddr = 0;
static u8 IapAsklen = 0;

static u8 last_optimes = 0;

static u8 iap_en_rtc;
static u16 iap_loopasktime;
static u16 iap_reqdattime;


typedef void (*Wlan4GSendIap)(u8*, u16, u32, u32, u8, u8);
typedef void (*WirelessSendIap)(u8, u8, u8*, u8, u32, u32, u32, u16);

static Wlan4GSendIap funcwlan4g;
static WirelessSendIap funcwireless;


// *****************************IAP INFO ***********************

u8 EraseIapInfodata(void)
{
    u8 err;
    
    err = FlashErase(iapinfo_address);
    if (FLASH_OP_SUCCESS == err)
    {
        
    }
    return err;
}


u8 WriteIapInfo(IAP_RECORD *info)
{
    u16 i;
    u8 err;
    
    for (i=0;i<2048/(sizeof(IAP_RECORD));i++)
    {
        err = FlashCheckFF(iapinfo_address+i*sizeof(IAP_RECORD), 2);
        if (err == 0)   // all ff
        {
            err = FlashWriteWithCheck(iapinfo_address+i*sizeof(IAP_RECORD), (u8 *)info, sizeof(IAP_RECORD));
            if (err != FLASH_OP_SUCCESS)
                return FLASH_OP_ERR_WRITE;
            return FLASH_OP_SUCCESS;
        }
    }
    if (FlashErase(iapinfo_address) != FLASH_OP_SUCCESS)
        return FLASH_OP_ERR_ERASE;
    err = FlashWriteWithCheck(iapinfo_address, (u8*)info, sizeof(IAP_RECORD));
    if (err != FLASH_OP_SUCCESS)
        return FLASH_OP_ERR_WRITE;
    return FLASH_OP_SUCCESS;
}


u8 ReadIapInfo(IAP_RECORD *info)
{
    u8 buffer[sizeof(IAP_RECORD)];
    u16 i;
    u8 err, ok = 0;
    
    for (i=0;i<2048/(sizeof(IAP_RECORD));i++)
    {
        err = FlashReadWithCheck(iapinfo_address+i*sizeof(IAP_RECORD), buffer, sizeof(IAP_RECORD));
        if (err == FLASH_OP_SUCCESS)
        {
            ok = 1;
            OS_MemCopy((u8*)info, buffer, sizeof(IAP_RECORD));
        }
        else if (err == FLASH_OP_ERR_ALLF)
        {
            if (0 == i)
                return FLASH_OP_ERR_ALLF;
            
            break;
        }
        else
        {
            ok = 0;
        }
    }
    
    return (ok == 0)?FLASH_OP_ERR_READ:FLASH_OP_SUCCESS;
}


u8 WriteIapdata(u8 *buf, u32 addr, u8 len)
{
    u32 waddr = BIN_IAPFILE2+addr;
    u8 err;
    
    if ((waddr & FLASH_PAGE_MASK) == 0)
    {
        err = FlashErase(waddr);
        if (err != FLASH_OP_SUCCESS)
            return FLASH_OP_ERR_DRV;
    }
    
    err = FlashWrite(waddr, buf, len);
    return err;
}


u8 ReadIapdata(u8 *buf, u32 addr, u8 len)
{
    return FlashRead(BIN_IAPFILE2+addr, buf, len);
}

// *****************************END IAP INFO *******************


// *****************************LOCAL IAP INFO *****************

u8 WriteLocalConfig(u8 *buffer)
{
    u16 i;
    u8 err;
    
    for (i=0;i<2048/32;i++)
    {
        err = FlashCheckFF(mac_address+i*32, 2);
        if (err == 0)   // all ff
        {
            err = FlashWriteWithCheck(mac_address+i*32, buffer, 32);
            if (err != FLASH_OP_SUCCESS)
                return FLASH_OP_ERR_WRITE;
            return FLASH_OP_SUCCESS;
        }
    }
    if (FlashErase(mac_address) != FLASH_OP_SUCCESS)
        return FLASH_OP_ERR_ERASE;
    err = FlashWriteWithCheck(mac_address, buffer, 32);
    if (err != FLASH_OP_SUCCESS)
        return FLASH_OP_ERR_WRITE;
    return FLASH_OP_SUCCESS;
}


u8 ReadLocalConfig(u8 *retbuf)
{
    u8 buffer[32];
    u16 i;
    u8 err, ok = 0;
    
    for (i=0;i<2048/32;i++)
    {
        err = FlashReadWithCheck(mac_address+i*32, buffer, 32);
        if (err == FLASH_OP_SUCCESS)
        {
            // reserve last ok config
            ok = 1;
            OS_MemCopy(retbuf, buffer, 32);
        }
        else if (err == FLASH_OP_ERR_ALLF)
        {
            if (0 == i)
                return FLASH_OP_ERR_ALLF;
            
            break;
        }
        else
        {
            ok = 0;
        }
    }
    
    return (ok == 0)?FLASH_OP_ERR_READ:FLASH_OP_SUCCESS;
}

// *****************************END LOCAL IAP INFO *************

void AskVersion(u32 daddr, u32 saddr)
{
    u8 askv[18];
    askv[12] = DefInfo.record.mver;
    askv[13] = DefInfo.record.vver;
    askv[14] = gLocalInfoFromRom;
    askv[15] = 0;
    
    if (funcwlan4g != NULL)
    {
        askv[0] = 0x7e;
        askv[1] = 15;
        askv[2] = FRAME_NEED_NO_ACK;
        askv[3] = (u8)(daddr>>24);
        askv[4] = (u8)(daddr>>16);
        askv[5] = (u8)(daddr>>8);
        askv[6] = (u8)(daddr);
        askv[7] = (u8)(saddr>>24);
        askv[8] = (u8)(saddr>>16);
        askv[9] = (u8)(saddr>>8);
        askv[10] = (u8)(saddr);
        askv[11] = ASK_IAPVERSON;
        askv[16] = docrc8(0, askv+1, 15);
        askv[17] = 0x7e;
        
        // MIDWARE_DATA_MESSAGE
        funcwlan4g(askv, 18, gUploadDeviceId, daddr, MIDWARE_DATA_MESSAGE, 1);
    }
    else if (funcwireless != NULL)
    {
        // FRAME_NEED_NO_ACK
        funcwireless(ASK_IAPVERSON, FRAME_NEED_NO_ACK, askv+12, 4, saddr, daddr, daddr, 0);
    }
}


void RcvVersion(u8 *buf, u16 len, u32 svrid, u32 svraddr, u8 frmsrc)
{
    if (len == 15)
    {
        INPUT_EVENT event;
        
        IapInfo.record.type = buf[0];
        IapInfo.record.mver = buf[1];
        IapInfo.record.vver = buf[2];
        IapInfo.record.verlen = ((u32)buf[3] << 16) | ((u32)buf[4] << 8) | buf[5];
        IapInfo.record.vercrc8 = buf[6];
        IapInfo.iaptime = (((u32)buf[7])<<24)|(((u32)buf[8])<<16)|(((u32)buf[9])<<8)|((u32)buf[10]);
        IapInfo.nettime = (((u32)buf[11])<<24)|(((u32)buf[12])<<16)|(((u32)buf[13])<<8)|((u32)buf[14]);
        IapInfo.updatesvrid = svrid;
        IapInfo.updatesvraddr = svraddr;
        IapInfo.fromsrc = frmsrc;
        
        event.Type = AirIap_Type_Msg;
        event.Subtype = AirIap_AskVer_Msg;
        OS_MsgBoxSend(gIapMsg_Queue, &event, OS_NO_DELAY, FALSE);
    }
}


void AskVerData(u32 dstid, u32 daddr, u32 saddr, u32 waddr, u8 len, u8 frmsrc)
{
    u8 askbuf[18];
    askbuf[12] = (u8)(waddr>>16);
    askbuf[13] = (u8)(waddr>>8);
    askbuf[14] = (u8)(waddr);
    askbuf[15] = len;
    
    if (frmsrc && funcwlan4g != NULL)
    {
        askbuf[0] = 0x7e;
        askbuf[1] = 15;
        askbuf[2] = FRAME_NEED_NO_ACK;
        askbuf[3] = (u8)(daddr>>24);
        askbuf[4] = (u8)(daddr>>16);
        askbuf[5] = (u8)(daddr>>8);
        askbuf[6] = (u8)(daddr);
        askbuf[7] = (u8)(saddr>>24);
        askbuf[8] = (u8)(saddr>>16);
        askbuf[9] = (u8)(saddr>>8);
        askbuf[10] = (u8)(saddr);
        askbuf[11] = ASK_IAPDATA;
        askbuf[16] = docrc8(0, askbuf+1, 15);
        askbuf[17] = 0x7e;
        
        // MIDWARE_DATA_MESSAGE
        funcwlan4g(askbuf, 18, gUploadDeviceId, dstid, MIDWARE_DATA_MESSAGE, 1);
    }
    else if (frmsrc == 0 && funcwireless != NULL)
    {
        // FRAME_NEED_NO_ACK
        funcwireless(ASK_IAPDATA, FRAME_NEED_NO_ACK, askbuf+12, 4, saddr, daddr, dstid, 0);
    }
}


void RcvVerData(u8 *buf, u16 len, u8 frmsrc)
{
    INPUT_EVENT event;
    
    RcvIapDt.type = buf[0];
    RcvIapDt.mver = buf[1];
    RcvIapDt.vver = buf[2];
    
    RcvIapDt.wraddr = ((u32)buf[3] << 16) | ((u32)buf[4] << 8) | buf[5];               
    RcvIapDt.len = buf[6];
    
    if (len == (7+RcvIapDt.len))
    {
        if (RcvIapDt.len > 0)
            OS_MemCpy(RcvIapDt.data, &buf[7], RcvIapDt.len);
        
        event.Type = AirIap_Type_Msg;
        event.Subtype = AirIap_Data_Msg;
        OS_MsgBoxSend(gIapMsg_Queue, &event, OS_NO_DELAY, FALSE);
    }
}


void RcvAskIapInfo(u32 dstid, u32 saddr, u32 daddr, u8 frmsrc)
{
    u8 askbuf[18];
    askbuf[12] = DefInfo.record.mver;
    askbuf[13] = DefInfo.record.vver;
    askbuf[14] = 0;
    askbuf[15] = 0;
    
    if (frmsrc && funcwlan4g != NULL)
    {
        askbuf[0] = 0x7e;
        askbuf[1] = 15;
        askbuf[2] = FRAME_NEED_NO_ACK;
        askbuf[3] = (u8)(daddr>>24);
        askbuf[4] = (u8)(daddr>>16);
        askbuf[5] = (u8)(daddr>>8);
        askbuf[6] = (u8)(daddr);
        askbuf[7] = (u8)(saddr>>24);
        askbuf[8] = (u8)(saddr>>16);
        askbuf[9] = (u8)(saddr>>8);
        askbuf[10] = (u8)(saddr);
        askbuf[11] = ASK_IAPINFO;
        askbuf[16] = docrc8(0, askbuf+1, 15);
        askbuf[17] = 0x7e;
        
        // MIDWARE_DATA_MESSAGE
        funcwlan4g(askbuf, 18, gUploadDeviceId, dstid, MIDWARE_DATA_MESSAGE, 1);
    }
    else if (frmsrc == 0 && funcwireless != NULL)
    {
        // FRAME_NEED_NO_ACK
        funcwireless(ASK_IAPINFO, FRAME_NEED_NO_ACK, askbuf+12, 4, saddr, daddr, dstid, 0);
    }
}


void RcvAskGuid(u32 dstid, u32 saddr, u32 daddr, u8 frmsrc)
{
    u8 askbuf[26];
    askbuf[12] = chip_guid[0] & 0xFF;
    askbuf[13] = (chip_guid[0]>>8) & 0xFF;
    askbuf[14] = (chip_guid[0]>>16) & 0xFF;
    askbuf[15] = (chip_guid[0]>>24) & 0xFF;
    askbuf[16] = chip_guid[1] & 0xFF;
    askbuf[17] = (chip_guid[1]>>8) & 0xFF;
    askbuf[18] = (chip_guid[1]>>16) & 0xFF;
    askbuf[19] = (chip_guid[1]>>24) & 0xFF;
    askbuf[20] = chip_guid[2] & 0xFF;
    askbuf[21] = (chip_guid[2]>>8) & 0xFF;
    askbuf[22] = (chip_guid[2]>>16) & 0xFF;
    askbuf[23] = (chip_guid[2]>>24) & 0xFF;
    
    if (frmsrc && funcwlan4g != NULL)
    {
        askbuf[0] = 0x7e;
        askbuf[1] = 23;
        askbuf[2] = FRAME_NEED_NO_ACK;
        askbuf[3] = (u8)(daddr>>24);
        askbuf[4] = (u8)(daddr>>16);
        askbuf[5] = (u8)(daddr>>8);
        askbuf[6] = (u8)(daddr);
        askbuf[7] = (u8)(saddr>>24);
        askbuf[8] = (u8)(saddr>>16);
        askbuf[9] = (u8)(saddr>>8);
        askbuf[10] = (u8)(saddr);
        askbuf[11] = ASK_CHIPGUID;
        askbuf[24] = docrc8(0, askbuf+1, 23);
        askbuf[25] = 0x7e;
        
        // MIDWARE_DATA_MESSAGE
        funcwlan4g(askbuf, 26, gUploadDeviceId, dstid, MIDWARE_DATA_MESSAGE, 1);
    }
    else if (frmsrc == 0 && funcwireless != NULL)
    {
        // FRAME_NEED_NO_ACK
        funcwireless(ASK_CHIPGUID, FRAME_NEED_NO_ACK, askbuf+12, 12, saddr, daddr, dstid, 0);
    }
}


void RespondConfigPara(u8 cmd, u16 seq, u8 result, u32 saddr, u32 daddr, u32 dstid, u8 frmsrc)
{
    u8 buff[18];
    buff[14] = result;
    
    if (frmsrc && funcwlan4g != NULL)
    {
        buff[0] = 0x7e;
        buff[1] = 14;
        buff[2] = FRAME_JUST_ACK;
        buff[3] = (u8)(daddr>>24);
        buff[4] = (u8)(daddr>>16);
        buff[5] = (u8)(daddr>>8);
        buff[6] = (u8)(daddr);
        buff[7] = (u8)(saddr>>24);
        buff[8] = (u8)(saddr>>16);
        buff[9] = (u8)(saddr>>8);
        buff[10] = (u8)(saddr);
        buff[11] = cmd;
        buff[12] = (u8)(seq>>8);
        buff[13] = (u8)(seq);
        buff[15] = docrc8(0, buff+1, 14);
        buff[16] = 0x7e;
        
        // MIDWARE_DATA_MESSAGE
        funcwlan4g(buff, 17, gUploadDeviceId, dstid, MIDWARE_DATA_MESSAGE, 1);
    }
    else if (frmsrc == 0 && funcwireless != NULL)
    {
        // FRAME_JUST_ACK
        funcwireless(cmd, FRAME_JUST_ACK, buff+14, 1, saddr, daddr, dstid, seq);
    }
}


void RcvConfigPara(u16 seq, u8 *buf, u16 len, u32 saddr, u32 daddr, u32 dstid, u8 frmsrc)
{
    if (len > 0)
    {
        u8 cfgbuf[32];
        
        // do config actions here, max 31 bytes
        if (len > 31)
            len = 31;
        
        // here omit check for paras
        OS_MemCopy(cfgbuf, buf, len);
        if (len < 31)
        {
            OS_MemSet(cfgbuf+len, 0xff, 31-len);
        }
        
        if (WriteLocalConfig(cfgbuf) == FLASH_OP_SUCCESS)
        {
            if (ReadLocalConfig(cfgbuf) == FLASH_OP_SUCCESS)
            {
                OSTimeDly(400);
                Sys_Soft_Reset();
            }
        }
    }
}


void RespConfigPara(u16 seq, u8 *buf, u16 len, u32 saddr, u32 daddr, u32 dstid, u8 frmsrc)
{
    u8 buff[52];
    
    if (ReadLocalConfig(buff+15) == FLASH_OP_SUCCESS)
    {
        buff[14] = 0;
        len = 33;
    }
    else
    {
        buff[14] = 1;
        len = 1;
    }
    
    if (frmsrc && funcwlan4g != NULL)
    {
        buff[0] = 0x7e;
        buff[1] = 13+len;
        buff[2] = FRAME_JUST_ACK;
        buff[3] = (u8)(daddr>>24);
        buff[4] = (u8)(daddr>>16);
        buff[5] = (u8)(daddr>>8);
        buff[6] = (u8)(daddr);
        buff[7] = (u8)(saddr>>24);
        buff[8] = (u8)(saddr>>16);
        buff[9] = (u8)(saddr>>8);
        buff[10] = (u8)(saddr);
        buff[11] = READ_LOCAL_DEVICE_PARA;
        buff[12] = (u8)(seq>>8);
        buff[13] = (u8)(seq);
        buff[14+len] = docrc8(0, buff+1, 13+len);
        buff[15+len] = 0x7e;
        
        // MIDWARE_DATA_MESSAGE
        funcwlan4g(buff, 16+len, gUploadDeviceId, dstid, MIDWARE_DATA_MESSAGE, 1);
    }
    else if (frmsrc == 0 && funcwireless != NULL)
    {
        // FRAME_JUST_ACK
        funcwireless(READ_LOCAL_DEVICE_PARA, FRAME_JUST_ACK, buff+14, len, saddr, daddr, dstid, seq);
    }
}


void RcvConfigID(u16 seq, u8 *buf, u16 len, u32 saddr, u32 daddr, u32 dstid, u8 frmsrc)
{
    if (len > 0)
    {
        u8 cfgbuf[32];
        
        if (ReadLocalConfig(cfgbuf) == FLASH_OP_SUCCESS)
        {
            OS_MemCpy(cfgbuf, buf, 4);
            
            if (WriteLocalConfig(cfgbuf) == FLASH_OP_SUCCESS)
            {
                if (ReadLocalConfig(cfgbuf) == FLASH_OP_SUCCESS)
                {
                    OSTimeDly(400);
                    Sys_Soft_Reset();
                }
            }
        }
    }
}


void ResetDevice(u16 seq, u8 *buf, u16 len, u32 saddr, u32 daddr, u32 dstid, u8 frmsrc)
{
    OSTimeDly(400);
    Sys_Soft_Reset();
}


void RcvAdjustTime(u8 *msg, u16 len, u32 saddr, u32 daddr, u32 dstid, u8 frmsrc)
{
    u8 buff[22];
    RTC_TIME t;
    
    if (len == 0)
    {
        RTC_GetTime(&t);
        buff[12] = (t.year>>8) & 0xff;
        buff[13] = t.year & 0xff;
        buff[14] = t.mon;
        buff[15] = t.day;
        buff[16] = t.hour;
        buff[17] = t.min;
        buff[18] = t.sec;
    }
    else
    {
        t.year = (msg[0]<<8)|msg[1];
        t.mon = msg[2];
        t.day = msg[3];
        t.hour = msg[4];
        t.min = msg[5];
        t.sec = msg[6];
        t.week = msg[7];
        
        RTC_Adjust(&t, RtcOp_SetTime);
        
        buff[12] = 0;
    }
    
    if (len == 0 || len > 8)
    {
        if (frmsrc && funcwlan4g != NULL)
        {
            buff[0] = 0x7e;
            buff[1] = (len == 0)?18:12;
            buff[2] = FRAME_NEED_NO_ACK;
            buff[3] = (u8)(daddr>>24);
            buff[4] = (u8)(daddr>>16);
            buff[5] = (u8)(daddr>>8);
            buff[6] = (u8)(daddr);
            buff[7] = (u8)(saddr>>24);
            buff[8] = (u8)(saddr>>16);
            buff[9] = (u8)(saddr>>8);
            buff[10] = (u8)(saddr);
            buff[11] = ADJUST_NETWORK_TIME;
            if (len == 0)
            {
                buff[19] = docrc8(0, buff+1, 18);
                buff[20] = 0x7e;
            }
            else
            {
                buff[13] = docrc8(0, buff+1, 12);
                buff[14] = 0x7e;
            }
            
            // MIDWARE_DATA_MESSAGE
            funcwlan4g(buff, (len == 0)?21:15, gUploadDeviceId, dstid, MIDWARE_DATA_MESSAGE, 1);
        }
        else if (frmsrc == 0 && funcwireless != NULL)
        {
            // FRAME_NEED_NO_ACK
            funcwireless(ADJUST_NETWORK_TIME, FRAME_NEED_NO_ACK, buff+12, (len == 0)?7:1, saddr, daddr, dstid, 0);
        }
    }
}


void RcvIapData(u8 cmd, u16 seq, u8 *pbuf, u16 len, u32 saddr, u32 daddr, u32 srcid, u8 frmsrc)
{
    switch (cmd)
    {
    case ASK_IAPVERSON:
        RcvVersion(pbuf, len, srcid, saddr, frmsrc);
        break;
    case ASK_IAPDATA:
        RcvVerData(pbuf, len, frmsrc);
        break;
    case ASK_IAPINFO:
        RcvAskIapInfo(srcid, daddr, saddr, frmsrc);
        break;
    case ASK_CHIPGUID:
        RcvAskGuid(srcid, daddr, saddr, frmsrc);
        break;
    case CONFIG_LOCAL_DEVICE_PARA:
        RcvConfigPara(seq, pbuf, len, daddr, saddr, srcid, frmsrc);
        break;
    case READ_LOCAL_DEVICE_PARA:
        RespConfigPara(seq, pbuf, len, daddr, saddr, srcid, frmsrc);
        break;
    case CONFIG_LOCAL_DEVICE_ID:
        RcvConfigID(seq, pbuf, len, daddr, saddr, srcid, frmsrc);
        break;
    case RESET_LOCAL_DEVICE:
        ResetDevice(seq, pbuf, len, daddr, saddr, srcid, frmsrc);
        break;
    case ADJUST_NETWORK_TIME:
        RcvAdjustTime(pbuf, len, daddr, saddr, srcid, frmsrc);
        break;
    default:
        break;
    }
}


void AirIapReq_Task(void *Task_Parameters)
{
    u8 delayask = (docrc8(0, (u8*)&gLocalDeviceId, 4) & 0x1f)+20;
    
    OSTimeDly(600+delayask*200);
    call_time = iap_loopasktime;
    
    while (1)
    {
        call_time ++;
        if (call_time > iap_loopasktime)
        {
            call_time = 0;
            if (IapInfo.record.iap_rdy == 0 && IapAsklen == 0)
            {
                AskVersion(gCtrlPlatformId, gLocalDeviceId);
            }
        }
        OSTimeDly(200);
    }
}


void AirIapDat_Task(void *Task_Parameters)
{
    INPUT_EVENT event;
    
    u8 readdata[64];
    u32 k;
    u16 rdval;
    u8 err, wcrc8, rcrc8, len2;
    
    while (1)
    {
        err = OS_MsgBoxReceive(gIapMsg_Queue, &event, 100);
        if (err == OS_TIMEOUT)
        {
            if (IapAsklen)
            {
                k = OSTimeGet();
                if ((k-last_rcvtick) > 8*200)
                {
                    if (last_optimes < 32)
                    {
                        last_optimes ++;
                        last_rcvtick = k;
                        
                        AskVerData(IapInfo.updatesvrid, IapInfo.updatesvraddr, gLocalDeviceId, IapAskaddr, 
                            IapAsklen, IapInfo.fromsrc);
                    }
                    else
                    {
                        last_rcvtick = 0;
                        IapAsklen = 0;
                    }
                }
            }
        }
        
        if (err != OS_NO_ERR)
            continue;
        
        if (event.Subtype == AirIap_AskVer_Msg)
        {
            if (IapAsklen == 0)
            {
                k = IapInfo.iaptime;
                
                if (iap_en_rtc)
                {
                    if (IapInfo.nettime != 0)
                    {
                        RTC_WaitForLastTask();
                        RTC_SetCounter(IapInfo.nettime);
                        RTC_WaitForLastTask();
                    }
                }
                
                if (IapInfo.record.verlen == 0)
                    continue;
                
                if (IapInfo.record.type != DefInfo.record.type)
                    continue;
                
                if (k != 0xffffffff)
                {
                    if ((IapInfo.record.mver == DefInfo.record.mver) && 
                        (IapInfo.record.vver == DefInfo.record.vver))
                        continue;
                }
                
                last_rcvtick = OSTimeGet();
                last_optimes = 0;
                
                if (k == 0xffffffff)
                    k = 0;
                
                if (k > 60)
                    k = 60;
                
                if (k > 0)
                {
                    OSTimeDly(k*200);
                }
                
                IapAskaddr = 0;
                IapAsklen = 64;
                AskVerData(IapInfo.updatesvrid, IapInfo.updatesvraddr, gLocalDeviceId, IapAskaddr, IapAsklen, 
                    IapInfo.fromsrc);
            }
        }
        else if (event.Subtype == AirIap_Data_Msg)
        {
            if ((RcvIapDt.type != IapInfo.record.type) || (RcvIapDt.mver != IapInfo.record.mver) || 
                (RcvIapDt.vver!= IapInfo.record.vver))
                continue;
            
            if (IapAsklen == 0 || RcvIapDt.wraddr != IapAskaddr || RcvIapDt.len == 0)
                continue;
            
            len2 = RcvIapDt.len;
            last_rcvtick = OSTimeGet();
            last_optimes = 0;
            
            OSTimeDly(2);
            
            wcrc8 = docrc8(0, RcvIapDt.data, len2);
            err = WriteIapdata(RcvIapDt.data, IapAskaddr, len2);
            if (err != 0)
            {
                IapInfo.record.errcnt ++;
                if (IapInfo.record.errcnt < 5)
                {
                    OSTimeDly(iap_reqdattime);
                    AskVerData(IapInfo.updatesvrid, IapInfo.updatesvraddr, gLocalDeviceId, IapAskaddr, IapAsklen, 
                        IapInfo.fromsrc);
                }
                continue;
            }
            
            OSTimeDly(1);
            
            err = ReadIapdata(readdata, IapAskaddr, len2);
            if (err == FLASH_OP_SUCCESS)
            {
                rcrc8 = docrc8(0, readdata, len2);
            }
            else
            {
                rcrc8 = ~wcrc8;
            }
            
            if (rcrc8 != wcrc8)
            {
                IapInfo.record.errcnt ++;
                if (IapInfo.record.errcnt < 5)
                {
                    OSTimeDly(iap_reqdattime);
                    AskVerData(IapInfo.updatesvrid, IapInfo.updatesvraddr, gLocalDeviceId, IapAskaddr, IapAsklen, 
                        IapInfo.fromsrc);
                }
                continue;
            }
            
            IapInfo.record.errcnt = 0;
            IapAskaddr += len2;
            
            if (IapAskaddr < IapInfo.record.verlen)
            {
                OSTimeDly(iap_reqdattime);
                AskVerData(IapInfo.updatesvrid, IapInfo.updatesvraddr, gLocalDeviceId, IapAskaddr, IapAsklen, 
                    IapInfo.fromsrc);
            }
            else
            {
                rcrc8 = 0;
                for (k=0;k<IapInfo.record.verlen/2;k++)
                {
                    rdval = *(__IO uint16_t*)(BIN_IAPFILE2+k*2);
                    rcrc8 = docrc8(rcrc8, (u8 *)(&rdval), 2);
                }
                if (IapInfo.record.vercrc8 != rcrc8)
                {
                    call_time = iap_loopasktime-100;
                    IapAsklen = 0;
                    continue;
                }
                else
                {
                    IapInfo.record.errcnt = 0;
                    IapInfo.record.iap_rdy = 1;
                    WriteIapInfo(&(IapInfo.record));
                    
                    OSTimeDly(1200);
                    Sys_Soft_Reset();
                }
            }
        }
    }
}


void CreateIapTask(u8 reqstack, u8 reqprio, u8 datastack, u8 dataprio)
{
    OS_TaskCreate(AirIapReq_Task, (u8*)"IAPReq", OS_MINIMAL_STACK_SIZE*reqstack, NULL, reqprio, &IapReq_Task_Handle);
    OS_TaskCreate(AirIapDat_Task, (u8*)"IAPData", OS_MINIMAL_STACK_SIZE*datastack, NULL, dataprio, &IapDat_Task_Handle);
}


void ParaFlash_Init(u32 rom_flashsize)
{
    mac_address = FLASH_START+rom_flashsize-FLASH_PAGE_SIZE;
    paras_address = FLASH_START+rom_flashsize-FLASH_PAGE_SIZE*2;
    iapinfo_address = FLASH_START+rom_flashsize-FLASH_PAGE_SIZE*3;
}


void AirIap_Init(u8 enablertc, void *pfunc4g, void *pfuncwl, u16 loopasktime, u16 interreqms, 
    u8 reqtaskprio, u8 dattaskprio, u8 mver, u8 vver, u8 reqstack, u8 datstack, u32 *guid)
{
    gIapMsg_Queue = OS_MsgBoxCreate((u8*)"GiapREvt", sizeof(INPUT_EVENT), 12);
    if (gIapMsg_Queue == NULL)
        AIRIAP_Debug("gIapMsg_Queue NULL\n\r");
    
    chip_guid[0] = guid[0];
    chip_guid[1] = guid[1];
    chip_guid[2] = guid[2];
    
    OS_MemSet(&DefInfo, 0, sizeof(IAP_INFO));
    DefInfo.record.mver = mver;
    DefInfo.record.vver = vver;
    DefInfo.record.type = (gLocalDeviceId>>24)&0xff;
    
    OS_MemSet(&IapInfo, 0, sizeof(IAP_INFO));
    OS_MemSet(&RcvIapDt, 0, sizeof(IAP_DATA));
    
    if (loopasktime < 120)
        loopasktime = 120;
    else if (loopasktime > 60000)
        loopasktime = 60000;
    
    iap_loopasktime = loopasktime;
    
    if (interreqms < 300)
        interreqms = 300;
    else if (interreqms > 30000)
        interreqms = 30000;
    
    iap_reqdattime = interreqms/5;
    iap_en_rtc = enablertc;
    
    funcwlan4g = (Wlan4GSendIap)pfunc4g;
    funcwireless = (WirelessSendIap)pfuncwl;
    
    CreateIapTask(reqstack, reqtaskprio, datstack, dattaskprio);
}


void Sys_Soft_Reset(void)
{
    SCB->AIRCR = 0X05FA0000|(u32)0x04;
}

