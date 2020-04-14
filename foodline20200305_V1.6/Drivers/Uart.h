#ifndef MZ_UART_HEADER_H
#define MZ_UART_HEADER_H

#include "stm32f10x.h"
#include "Debug.h"

#define WIRELESS_RESET_PIN				(GPIO_Pin_1)
#define WIRELESS_RESET_PORT				(GPIOB)


void COM1_Init(u32 baudrate);
void COM2_Init(void);
void COM3_Init(void);
void COM4_Init(void);
void COM5_Init(void);

void WirelessReset(void);

#endif
