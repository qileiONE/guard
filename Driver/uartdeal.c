
#define __UARTDEAL_C

//通讯接收处理  包含NRF24L01
#include "includes.h"
#include "Crc16.h"


void Usart1DmaSend(uint8_t *pdata,uint16_t len)
{
//	uint32_t	time = 0;
	if((pdata== NULL)||(len==0))
		return;
//	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET)
//	{
//		time++;
//		if(time > 10000)
//			break;
//	}
	//  DMA1->IFCR |= DMA1_Channel4_IT_Mask;
	DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA1_Channel4->CMAR = (u32)pdata;
	DMA1_Channel4->CNDTR = len;
	USART_ClearFlag(USART1	,USART_FLAG_TC);
	DMA_Cmd(DMA1_Channel4, ENABLE); 
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); 
}
//COIN IN
#include "stm32f10x_dma.h"
void Usart2DmaSend(uint8_t *pdata,uint16_t len)
{
//	uint32_t	time = 0;
	if((pdata== NULL)||(len==0))
		return;
//	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET)
//	{
//		time++;
//		if(time > 10000)
//			break;
//	}
	DMA_Cmd(DMA1_Channel7, DISABLE);
	DMA1->IFCR |= (DMA_ISR_GIF7 | DMA_ISR_TCIF7 | DMA_ISR_HTIF7 | DMA_ISR_TEIF7);
	DMA1_Channel7->CMAR = (u32)pdata;
	DMA1_Channel7->CNDTR = len;
	USART_ClearFlag(USART2	,USART_FLAG_TC);
	//DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Channel7, ENABLE); 
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE); 
}
//
void Usart3DmaSend(uint8_t *pdata,uint16_t len)
{
//	uint32_t	time = 0;
	if((pdata== NULL)||(len==0))
		return;
//	while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET)
//	{
//		time++;
//		if(time > 10000)
//			break;
//	}
	DMA_Cmd(DMA1_Channel2, DISABLE);
	DMA1->IFCR |= (DMA_ISR_GIF2 | DMA_ISR_TCIF2 | DMA_ISR_HTIF2 | DMA_ISR_TEIF2);
	DMA1_Channel2->CMAR = (u32)pdata;
	DMA1_Channel2->CNDTR = len;
	USART_ClearFlag(USART3	,USART_FLAG_TC);
	//DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Channel2, ENABLE); 
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE); 
}
//COIN OUT
void Uart4DmaSend(uint8_t *pdata,uint16_t len)
{
	uint32_t	time = 0;
	if((pdata== NULL)||(len==0))
		return;
	while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET)
	{
		time++;
		if(time > 10000)
			break;
	}
	//  DMA1->IFCR |= DMA1_Channel4_IT_Mask;
	DMA_Cmd(DMA2_Channel5, DISABLE);
	DMA2_Channel5->CMAR = (u32)pdata;
	DMA2_Channel5->CNDTR = len;
	USART_ClearFlag(UART4,USART_FLAG_TC);
	DMA_Cmd(DMA2_Channel5, ENABLE); 
	USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE); 
}
UINT8 UartSendNoDMA(uint8_t *pdata,UINT16 len)
{
	volatile UINT32 timeout = 0;

	while(len--)
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
		{
			timeout++;
			if(timeout >100000)
			{
				return 1;
			}
		}
		timeout = 0;
		/* write a character to the USART */
		USART_SendData(USART1, *pdata);
		pdata++;
	}
	return 0;
}

UINT8 Uart2SendNoDMA(uint8_t *pdata,UINT16 len)
{
	volatile UINT32 timeout = 0;

	while(len--)
	{
		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
		{
			timeout++;
			if(timeout >100000)
			{
				return 1;
			}
		}
		timeout = 0;
		/* write a character to the USART */
		USART_SendData(USART2, *pdata);
		pdata++;
	}
	return 0;
}

#ifdef _DEBUG_msg
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}
  
  /* write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  return ch;
}
#endif


