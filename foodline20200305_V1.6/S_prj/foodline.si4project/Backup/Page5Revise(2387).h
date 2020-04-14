/*****************************************************************
 * File: Page5Revise.h
 * Date: 2019/11/27 14:49
 * Copyright@2018-2028: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#ifndef _PAGE5REVISE_H_
#define _PAGE5REVISE_H_


#include "System.h"

#define PAGE5_REVISE_OLD_TYPE                  (0x0500)	// 
#define PAGE5_REVISE_OLD_AREA                  (0x0501)
#define PAGE5_REVISE_OLD_MAIN_LINE             (0x0502)
#define PAGE5_REVISE_OLD_VICE_LINE             (0x0503)

#define PAGE5_REVISE_NEW_TYPE                  (0x0510)	// 
#define PAGE5_REVISE_NEW_AREA                  (0x0511)
#define PAGE5_REVISE_NEW_MAIN_LINE             (0x0512)
#define PAGE5_REVISE_NEW_VICE_LINE             (0x0513)

#define PAGE5_REVISE_LOCAL_TYPE                (0x0520)	// 
#define PAGE5_REVISE_LOCAL_AREA                (0x0521)
#define PAGE5_REVISE_LOCAL_MAIN_LINE           (0x0522)
#define PAGE5_REVISE_LOCAL_VICE_LINE           (0x0523)

void Page5ReviseInit(void);
void Page5ReviseProcess(u8 reg, u16 addr, u8 *pbuf, u8 len);
void Page5ReviseRefresh(void);

#endif // _PAGE5REVISE_H_

