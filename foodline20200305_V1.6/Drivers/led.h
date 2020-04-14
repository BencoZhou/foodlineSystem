#ifndef __LED_H
#define __LED_H


#include "stm32f10x.h"
#include "Debug.h"
#include "PeripheralIO.h"


#define LED_PIN     GPIO_Pin_6
#define LED         PCout(6)
#define LED_ON      PCout(6)=0
#define LED_OFF     PCout(6)=1


#define LED1_PIN    GPIO_Pin_14
#define LED2_PIN    GPIO_Pin_15
#define LED3_PIN    GPIO_Pin_9
#define GLED_PIN    GPIO_Pin_2      //
#define RLED_PIN    GPIO_Pin_3      //


#define GLED        PFout(2)
#define GLED_ON     PFout(2)=0
#define GLED_OFF    PFout(2)=1

#define RLED        PFout(3)
#define RLED_ON     PFout(3)=0
#define RLED_OFF    PFout(3)=1




#define LED1        PDout(14)
#define LED1_ON     PDout(14)=0
#define LED1_OFF    PDout(14)=1

#define LED2        PDout(15)
#define LED2_ON     PDout(15)=0
#define LED2_OFF    PDout(15)=1

#define LED3        PCout(9)
#define LED3_ON     PCout(9)=0
#define LED3_OFF    PCout(9)=1


void LED_Init(void);

                             
#endif
