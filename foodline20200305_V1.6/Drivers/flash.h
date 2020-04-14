#ifndef __MZSYS_FLASH_H__
#define __MZSYS_FLASH_H__

#include "stm32f10x.h"
#include "Debug.h"

#define FLASH_OP_SUCCESS    0
#define FLASH_OP_ERR_ADDR   1
#define FLASH_OP_ERR_DRV    2
#define FLASH_OP_ERR_CHK    3
#define FLASH_OP_ERR_ALLF   4
#define FLASH_OP_ERR_ERASE  5
#define FLASH_OP_ERR_WRITE  6
#define FLASH_OP_ERR_LEN    7
#define FLASH_OP_ERR_READ   8


#define FLASH_OP_TAG_CHAR   0xa5


#define FLASH_START         0x08000000
#define FLASH_SIZE          0x80000     // 512K
#define FLASH_PAGE_SIZE     0x800       // 2K
#define FLASH_PAGE_MASK     0x7ff


// len must BE EVEN bytes and len >= 2
u8 FlashRead(u32 addr, u8 *data, u16 len);
// len must BE EVEN bytes and len >= 2, last byte must be crc8 checksum byte
// checksum checked by function
u8 FlashReadWithCheck(u32 addr, u8 *data, u16 len);
u8 FlashWrite(u32 addr, u8 *data, u16 len);
// len must BE EVEN bytes and len >= 2, last byte must be crc8 checksum byte
// checksum computed by function
u8 FlashWriteWithCheck(u32 addr, u8 *data, u16 len);
u8 FlashErase(u32 addr);
u8 FlashCheckFF(u32 addr, u16 len);

#endif
