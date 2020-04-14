#include "System.h"

u8 GblPolicyBuf[5*1024];


u8* FindNextTarget(u8 dayage, u8 count, u8 ssize)
{
    u8 *pbuf = GblPolicyBuf+sizeof();
    u8 i = 0, tag = 0;
    if (dayage == 0xff)
        return pbuf;
    
    while (i < count)
    {
        if (((TargetPolicy*)pbuf)->dayage > dayage)
        {
            tag = 1;
            break;
        }
        pbuf += ssize;
        i ++;
    }
    if (tag)
        return pbuf;
    
    return NULL;
}


