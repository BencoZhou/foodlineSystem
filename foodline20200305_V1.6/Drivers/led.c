#include "led.h"
 
 
void LED_Init()     
{
    GPIO_InitTypeDef GPIO_InitStructure; 
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);
    GPIO_InitStructure.GPIO_Pin=GLED_PIN|RLED_PIN; 
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;     
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;     
    GPIO_Init(GPIOF,&GPIO_InitStructure);       
//  RLED = 0;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
    GPIO_InitStructure.GPIO_Pin=LED1_PIN|LED2_PIN; 
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;     
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;     
    GPIO_Init(GPIOD,&GPIO_InitStructure);     
    LED1 = 1;
    LED2 = 1;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
    GPIO_InitStructure.GPIO_Pin=LED3_PIN; 
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;     
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;     
    GPIO_Init(GPIOC,&GPIO_InitStructure); 
    LED3 = 1;    
}



