#ifndef MZSYS_DELAY_HEADER_H
#define MZSYS_DELAY_HEADER_H

#include "stm32f10x.h"
#include "ucos_ii.h"
#include "Debug.h"

void delay_init(void);
void delay_us(u32 nus);

#endif
