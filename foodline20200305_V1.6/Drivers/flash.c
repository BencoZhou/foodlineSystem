#include "flash.h"
#include "Os_Wrap.h"



// 0: all are FF, invalid    1: valid value
u8 FlashCheckFF(u32 addr, u16 len)
{
    u16 rdval;
    u8 notallf = 0;
    
    addr &= 0xfffffffe;
    len &= 0xfffe;
    while (len)
    {
        rdval = *(__IO uint16_t*)addr;
        
        addr += 2;
        len -= 2;
        
        if (rdval != 0xffff)
            notallf = 1;
    }
    return notallf;
}


u8 FlashRead(u32 addr, u8 *data, u16 len)
{
    u16 rdval;
    u8 notallf = 0;
    
    // last byte must be crc
    if ((len < 2) || (len & 0x1))
        return FLASH_OP_ERR_LEN;
    
    while (len)
    {
        rdval = *(__IO uint16_t*)addr;
        *(u16*)data = rdval;
        
        addr += 2;
        data += 2;
        len -= 2;
        
        if (rdval != 0xffff)
            notallf = 1;
    }
    if (notallf)
        return FLASH_OP_SUCCESS;
    
    return FLASH_OP_ERR_ALLF;
}


u8 FlashReadWithCheck(u32 addr, u8 *data, u16 len)
{
    u8 *data2 = data;
    u16 rdval, len2 = len;
    u8 val, notallf = 0;
    
    // last byte must be crc
    if ((len < 2) || (len & 0x1))
        return FLASH_OP_ERR_LEN;
    
    while (len)
    {
        rdval = *(__IO uint16_t*)addr;
        *(u16*)data = rdval;
        
        addr += 2;
        data += 2;
        len -= 2;
        
        if (rdval != 0xffff)
            notallf = 1;
    }
    if (notallf)
    {
        val = docrc8(0, data2, len2-1);
        if (val == *((u8*)(data2+len2-1)))
            return FLASH_OP_SUCCESS;
        
        return FLASH_OP_ERR_CHK;
    }
    
    return FLASH_OP_ERR_ALLF;
}


u8 FlashWrite(u32 addr, u8 *data, u16 len)
{
    // last byte must be crc
    if ((len < 2) || (len & 0x1))
        return FLASH_OP_ERR_LEN;
    
    FLASH_Unlock();
    while (len)
    {
        if (FLASH_ProgramHalfWord(addr, *(uint16_t*)data) != FLASH_COMPLETE)
        {
            FLASH_Lock();
            return FLASH_OP_ERR_DRV;
        }
        
        addr += 2;
        data += 2;
        len -= 2;
    }
    FLASH_Lock();
    return FLASH_OP_SUCCESS;
}


u8 FlashWriteWithCheck(u32 addr, u8 *data, u16 len)
{
    u8 val;
    
    // last byte must be crc
    if ((len < 2) || (len & 0x1))
        return FLASH_OP_ERR_LEN;
    
    val = docrc8(0, data, len-1);
    *((u8*)(data+len-1)) = val;
    
    FLASH_Unlock();
    while (len)
    {
        if (FLASH_ProgramHalfWord(addr, *(uint16_t*)data) != FLASH_COMPLETE)
        {
            FLASH_Lock();
            return FLASH_OP_ERR_DRV;
        }
        
        addr += 2;
        data += 2;
        len -= 2;
    }
    FLASH_Lock();
    return FLASH_OP_SUCCESS;
}


u8 FlashErase(u32 addr)
{
    u8 rst;
    
    if ((addr & FLASH_PAGE_MASK) != 0)
        return FLASH_OP_ERR_ADDR;
    
    FLASH_Unlock();
    rst = (FLASH_ErasePage(addr) == FLASH_COMPLETE)?FLASH_OP_SUCCESS:FLASH_OP_ERR_DRV;
    FLASH_Lock();
    return rst;
}
