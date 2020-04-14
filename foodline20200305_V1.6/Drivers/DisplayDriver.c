/*****************************************************************
 * File: DisplayDriver.c
 * Date: 2018/03/22 14:48
 * Copyright@2015-2025: INNOTEK, All Right Reserved
 *
 * Note:
 *
******************************************************************/
#include "DisplayDriver.h"

static RcDisplayISR gRcDisplayISR = NULL;
static TxDisplayISR gTxDisplayISR = NULL;

void RHDisplayUsartInit(RcDisplayISR pRcDisplayISR, TxDisplayISR pTxDisplayISR)
{
	gRcDisplayISR = pRcDisplayISR;
	gTxDisplayISR = pTxDisplayISR;
}

void RHDisplaySendData(u8 nData)
{
	USART_SendData(USART2, nData);
}  
/*
void USART2_IRQHandler(void)  
{
	OSIntEnter();    

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //Ω” ’
	{		
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);	    

		if (gRcDisplayISR != NULL)
		{
			gRcDisplayISR(USART_ReceiveData(USART2));
		}		
	}
	OSIntExit();  			
}
*/

