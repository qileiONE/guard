
#define	__stm32f10x_it_c

#include "includes.h"
#include "Ble.h"


UINT8 usart2_485_stime;
UINT16 ledtime;
UINT8  ledflag;
extern volatile UartCtrStr	UartRBuf_Uart1;
extern volatile UartCtrStr	UartRBuf_Uart2;
void SysTick_Handler(void)
{
	//SysTickDelayTime_Counter();	
}

 /**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None		  
  */
void USART3_IRQHandler(void)
{
	UINT8 err;
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		err =  USART3->DR & 0xff;
	}
	
	  /* If overrun condition occurs, clear the ORE flag 
	                              and recover communication */
	if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)
	{
		err = USART3->DR & 0xff;
		if(err)
			;
	}
}
//PC
void USART2_IRQHandler(void)
{
	UINT8 err;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
    if((UartRBuf_Uart2.receFlag == 0)&&(UartRBuf_Uart2.recePos<(UART_MAX_LEN - 2)))
		{
			UartRBuf_Uart2.data[UartRBuf_Uart2.recePos++] = USART2->DR & 0xff;
			UartRBuf_Uart2.receTime = 10;
		}
		else
		{
			err = USART2->DR & 0xff;
		}
	}
	  /* If overrun condition occurs, clear the ORE flag 
	                              and recover communication */
	if (USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)
	{
		err = USART2->DR & 0xff;
		if(err)
			;
	}
}

void USART1_IRQHandler(void)
{
	UINT8 err;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
        if((UartRBuf_Uart1.receFlag == 0)&&(UartRBuf_Uart1.recePos<(UART_MAX_LEN - 2)))
		{
			UartRBuf_Uart1.data[UartRBuf_Uart1.recePos++] = USART1->DR & 0xff;
			UartRBuf_Uart1.receTime = 10;
		}
		else
		{
			err = USART1->DR & 0xff;
		}
	}
	  /* If overrun condition occurs, clear the ORE flag 
	                              and recover communication */
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
	{
		err = USART1->DR & 0xff;
		if(err)
			;
	}
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  
	{	
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );

			systick_ms++;
		if(UartRBuf_Uart1.receTime != 0)
		{
			UartRBuf_Uart1.receTime--;	
			if(UartRBuf_Uart1.receTime == 0)
			{
				if(UartRBuf_Uart1.recePos < 2)
				{
					UartRBuf_Uart1.recePos = 0;
					UartRBuf_Uart1.receFlag = 0;
				}
			else
				UartRBuf_Uart1.receFlag = 1;
			}
		}
		
		if(UartRBuf_Uart2.receTime != 0)
		{
			UartRBuf_Uart2.receTime--;	
			if(UartRBuf_Uart2.receTime == 0)
			{
				if(UartRBuf_Uart2.recePos < 2)
				{
					UartRBuf_Uart2.recePos = 0;
					UartRBuf_Uart2.receFlag = 0;
				}
			else
				UartRBuf_Uart2.receFlag = 1;
			}
		}
	}
}
extern  uint8_t ble_recv_flag;
void EXTI4_IRQHandler(void)
{
//	if((EXTI->PR & EXTI_Line4) != RESET)
	{
		//EXTI_ClearITPendingBit(EXTI_Line4);
        EXTI->PR = EXTI_Line4;
        ble_recv_flag = 1;
	}
}

void EXTI3_IRQHandler(void)
{
//	if((EXTI->PR & EXTI_Line4) != RESET)
	{
		//EXTI_ClearITPendingBit(EXTI_Line4);
        EXTI->PR = EXTI_Line3;
        nfc_recv_flag = 1;
	}
}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line8) != RESET)
	{
		/* Clear the  EXTI line 5 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line8);
	}
}
/**
  * @brief  This function handles DMA1 Channel 6 interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Channel7_IRQHandler(void)
{
	/* Test on DMA1 Channel6 Transfer Complete interrupt */
	if(DMA_GetITStatus(DMA1_IT_TC7))
	{
		DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, DISABLE);
		/* Clear DMA1 Channel6 Half Transfer, Transfer Complete and Global interrupt pending bits */
		DMA_ClearITPendingBit(DMA1_IT_GL7);
		
		usart2_485_stime = 5;
	}
}
void DMA1_Channel2_IRQHandler(void)
{
	/* Test on DMA1 Channel6 Transfer Complete interrupt */
	if(DMA_GetITStatus(DMA1_IT_TC2))
	{
		DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, DISABLE);
		/* Clear DMA1 Channel6 Half Transfer, Transfer Complete and Global interrupt pending bits */
		DMA_ClearITPendingBit(DMA1_IT_GL2);
		
//		usart3_485_stime = 5;
	}
}
