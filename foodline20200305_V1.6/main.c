#include "System.h"
#include <stddef.h>
#include "Debug.h"
#include "Os_Wrap.h"
#include "Os_TaskPrio.h"
#include "Uart.h"



void __asm UndefinedInstruction(void)
{
    DCI 0xf123;
    DCI 0x4567;
    BX LR;
}

void __asm BadAlignedLDM(void)
{
    MOVS r0, #1
    LDM r0,{r1-r2}
    BX LR;
}


void RCC_Config(void)
{
    ErrorStatus HSEStartUpStatus;
    
    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);
    HSEStartUpStatus = RCC_WaitForHSEStartUp();
    if (HSEStartUpStatus == SUCCESS)
    {
        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        RCC_PCLK2Config(RCC_HCLK_Div1);
        RCC_PCLK1Config(RCC_HCLK_Div2);
        FLASH_SetLatency(FLASH_Latency_2);
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
        
        RCC_PLLCmd(ENABLE);
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
        
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        
        while (RCC_GetSYSCLKSource() != 0x08);
    }
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}


extern void Dispatcher_Task(void *Task_Parameters);
void *Dispatcher_Task_Handle = NULL;


#define INT_VECTOR_TABLE_BASE   0x2800


int main(void)
{
    RCC_Config();
    
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, INT_VECTOR_TABLE_BASE);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
#if MZ_PRINT_DEBUG
    COM5_Init();
#endif
    
    OS_WrapInit(HEAP_SIZE_BYTE);
    
#if MZ_INSTRUCTION_DIAG
    UndefinedInstruction();
    BadAlignedLDM();
#endif
    
    OS_TaskCreate(Dispatcher_Task, "Dispatch", OS_MINIMAL_STACK_SIZE*8, NULL, DISPATCHER_TASK_PRIORITY, &Dispatcher_Task_Handle);
    OS_StartRun();
    
    return (0);
}

