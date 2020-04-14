#ifndef MZSYS_IWDG_HEADER_H
#define MZSYS_IWDG_HEADER_H

#include "stm32f10x.h"
#include "Debug.h"

void IWDG_Init(u8 prer, u16 rlr);
void IWDG_Feed(void);

#endif
