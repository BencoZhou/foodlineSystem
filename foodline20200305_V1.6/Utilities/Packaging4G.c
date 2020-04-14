/*****************************************************************
 * File: Packaging4G.c
 * Date: 2019/06/20 14:46
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "Packaging4G.h"
#include "Wlan4GComm.h"
#include "WirelessComm.h"

extern u32 gUploadDeviceId;

void Wlan4G_SendDataPackaging(u8 cmd, u8 type, u8 *msg, u8 len, u32 srcaddr, u32 dstaddr, u16 ackseq)
{
    u8 buf[87], i = 0, crc = 0;
    buf[i++] = 0x7e;
    buf[i++] = len + 11;
    buf[i++] = type;
    buf[i++] = (dstaddr >> 24) & 0xFF;
    buf[i++] = (dstaddr >> 16) & 0xFF;
    buf[i++] = (dstaddr >> 8) & 0xFF;
    buf[i++] = dstaddr & 0xFF;
    buf[i++] = (srcaddr >> 24) & 0xFF;
    buf[i++] = (srcaddr >> 16) & 0xFF;
    buf[i++] = (srcaddr >> 8) & 0xFF;
    buf[i++] = srcaddr & 0xFF;
    buf[i++] = cmd;
    switch(type)
    {
        case 0:
        case 2:
            buf[i++] = (ackseq >> 8) & 0xFF;
            buf[i++] = ackseq & 0xFF;
            break;
    }
    if(len > 0 && len < 71)
    {
        memcpy(buf+i, msg, len);
        i += len;
    }
    crc = docrc8(crc, buf+1, i-1);
    buf[i++] = crc;
    buf[i++] = 0x7e;
    Wlan4G_SendData(buf, i, srcaddr, dstaddr, 0, 1);
}

void App_SendData(u8 cmd, u8 type, u8 *msg, u8 len, u32 srcaddr, u32 dstaddr, u16 ackseq)
{
    if(LocalConfigParaGet()->isUse4G == 1)
    {
        Wlan4G_SendDataPackaging(cmd, type, msg, len, gUploadDeviceId, dstaddr, ackseq);
    }
    else if(LocalConfigParaGet()->isUse4G == 0)
    {
        WirelessApp_SendData(cmd, type, msg, len, srcaddr, dstaddr, dstaddr, ackseq);
    }
}


