#ifndef MZ_IRQ_PRIORITY_HEADER_H
#define MZ_IRQ_PRIORITY_HEADER_H


#include "stm32f10x.h"


// ISR Priority 0(highest) - 15(lowest)
#define TIME2_IRQn_Priority             3

#define TIME4_IRQn_Priority             4

#define TIME5_IRQn_Priority             5

#define USART1_IRQn_Priority            6   // 4G

#define USART3_IRQn_Priority            7   // Wireless

#define USART2_IRQn_Priority            8

#define UART4_IRQn_Priority             9

#define UART5_IRQn_Priority             10

#define RTC_Priority                    11

#endif

