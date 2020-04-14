/*****************************************************************
 * File: Page1flinePathConfig.h
 * Date: 2019/11/01 9:19
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _PAGE1FLINEPATHCONFIG_H_
#define _PAGE1FLINEPATHCONFIG_H_
#include "System.h"

#define PAGE1_FLINE_DEVICE_TYPE                     (0x0100)
#define PAGE1_FLINE_LETTER                          (0x0101)
#define PAGE1_FLINE_NUMBER                          (0x0102)

#define PAGE1_FLINE_PATH                            (0x011F)

#define PAGE1_FLINE_FULLID                          (0x0190)

#define PAGE1_FLINE_SEND_BUTTON                     (0x0120)    // 键值为回车键

typedef struct
{
    u8  deviceType;
    u8  letter;
    u16 number;
    u32 fullID;
}flinePara;

void Page1FlinePathConfigInit(void);
void Page1FlinePathConfigProcess(u8 reg, u16 addr, u8 *pbuf, u8 len);
void Page1FlinePathConfigRefresh(void);

#endif // _PAGE1FLINEPATHCONFIG_H_

