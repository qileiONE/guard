
#include "includes.h"
#include "delay.h"
#include "rtc.h" 
#include "ble.h" 
#include "eeprom.h"
#include "adc.h"

extern volatile UartCtrStr	UartRBuf_Uart2;

void RCC_Configuration(void)
{
	volatile uint32_t i;
	
    RCC_DeInit();//复位RCC寄存器为缺省值
    RCC_HSICmd(ENABLE);//使能HSI  
	i = 0;
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)//wait for hsi
	{
		if(i++ > 500000)	{	break;}
	}
 
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    FLASH_SetLatency(FLASH_Latency_1);
   
    RCC_HCLKConfig(RCC_SYSCLK_Div1);   
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_PCLK2Config(RCC_HCLK_Div1);
    
    //
    RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_9);//
    RCC_PLLCmd(ENABLE);//
    i = 0;
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{
		if(i ++> 500000)	{	break;}
	}
 
    //
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);//
	i = 0;
    while(RCC_GetSYSCLKSource() != 0x08)
	{
		if(i++ > 500000)	{	break;}
	}
 
	SystemCoreClock         = 36000000;
}

void prvSetupHardware( void )
{

	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE and AFIO clocks */
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC
							 | RCC_APB2Periph_AFIO , ENABLE );

	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	/* Set the Vector Table base address at 0x08000000 */
    //PA13  PA14 为SWD模式，不可用    PA15  PB3  PB4为JLINK禁用，可用做GPIO
	NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x0 );

	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_3 );
	
}
void uart_exti_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	USART_Cmd(USART2, DISABLE);
	//GPIO_DeInit(GPIOB);
	USART_DeInit(USART2);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource3);
	/* Configure EXTI3 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI3 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure);
	
}
void InitBspGPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;


    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;             
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
  	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;	               //
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	LED_HI;
//// AIR720H  PEN
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;             
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
  	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;	               //
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

//	PEN_OFF;
//// AIR720H  RI STA
//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13;             
//  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
//  	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;	               //
//  	GPIO_Init(GPIOB, &GPIO_InitStructure);
//// AIR720H  DTR
//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;             
//  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
//  	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;	               //
//  	GPIO_Init(GPIOB, &GPIO_InitStructure);



// KEY  输入
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;
//  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
//  	GPIO_Init(GPIOC, &GPIO_InitStructure);


}
void ConfigWakeTrigger(uint8_t sta)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);
	/* Configure EXTI9 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	if(sta == 1)
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	else
		EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI9_5 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	if(sta == 1)
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	else
		NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);
    //BUTTON
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource8);
	/* Configure EXTI9 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	if(sta == 1)
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	else
		EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI9_5 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	if(sta == 1)
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	else
		NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}
uint32_t LsiFreq = 40000;
void IWDG_Init(void)
{
  /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
     dispersion) */
	RCC_ClearFlag();
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  /* IWDG counter clock: LSI/32 */
  	IWDG_SetPrescaler(IWDG_Prescaler_64);
  	IWDG_SetReload(0xfff);	//3.2s

  /* Reload IWDG counter */
  	IWDG_ReloadCounter();

	DBGMCU_Config(DBGMCU_IWDG_STOP,ENABLE);
  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  	IWDG_Enable();
}



/**
  * @}
  串口1配置		    条码机
  */ 

void Uart1DMAInit(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

  DMA_DeInit(DMA1_Channel4);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (UINT32)(&USART1->DR);
  DMA_InitStructure.DMA_MemoryBaseAddr = NULL;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 0;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel4, &DMA_InitStructure);
  DMA_Cmd(DMA1_Channel4, DISABLE); 
  DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, DISABLE); 
  USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE); 
}
void Usart1Int_Configuration(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;


  
  /* Enable the Ethernet global Interrupt 使能串1中断 */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);    
  
}	
void USART_COM1_Init(void)
{ 
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
  RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );
  //TX INIT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//GPIO_Mode_AF_PP;	 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //RX INIT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //USART INIT
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//USART_WordLength_9b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;//USART_Parity_Even;//USART_Parity_No;//USART_Parity_Odd;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;

  /* USART configuration */
  USART_Init(USART1, &USART_InitStructure);	     
  /* Enable USART */
  USART_Cmd(USART1, ENABLE);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

//  Uart1DMAInit();
  Usart1Int_Configuration();

}

/**
  * @}
  串口2配置		PC 485
  */ 

void Uart2DMAInit(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

  DMA_DeInit(DMA1_Channel7);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (UINT32)(&USART2->DR);
  DMA_InitStructure.DMA_MemoryBaseAddr = NULL;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 0;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel7, &DMA_InitStructure);
  DMA_Cmd(DMA1_Channel7, DISABLE); 
  DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, DISABLE); 
  USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE); 
}
void Usart2Int_Configuration(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;

  
  /* Enable the Ethernet global Interrupt 使能串2中断 */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
  
//  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn; 
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
//  NVIC_Init(&NVIC_InitStructure);  
  
}	
void USART_COM2_Init(void)
{ 
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );
  //TX INIT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //RX INIT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //USART INIT
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//USART_WordLength_9b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;//
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* USART configuration */
  USART_Init(USART2, &USART_InitStructure);	     
  /* Enable USART */
  USART_Cmd(USART2, ENABLE);
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

  Uart2DMAInit();
  Usart2Int_Configuration();

}

/**
  * @}
  串口3配置			  纸币机
  */ 

void Uart3DMAInit(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

  DMA_DeInit(DMA1_Channel2);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (UINT32)(&USART3->DR);
  DMA_InitStructure.DMA_MemoryBaseAddr = NULL;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 0;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel2, &DMA_InitStructure);
  DMA_Cmd(DMA1_Channel2, DISABLE); 
  DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, DISABLE); 
  USART_DMACmd(USART3, USART_DMAReq_Tx, DISABLE); 
}
void Usart3Int_Configuration(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;

   
  /* Enable the Ethernet global Interrupt 使能串3中断 */
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
//  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn; 
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
//  NVIC_Init(&NVIC_InitStructure);    
  
}	
void USART_COM3_Init(void)
{ 
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
  //TX INIT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //RX INIT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //USART INIT
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;//USART_Parity_No;//USART_Parity_Odd;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* USART configuration */
  USART_Init(USART3, &USART_InitStructure);	     
  /* Enable USART */
  USART_Cmd(USART3, ENABLE);
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

  Uart3DMAInit();
  Usart3Int_Configuration();

}

void TimeTickInit(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIMx_Setus(TIM2,1000);
	Timx_SetupNVIC(TIM2_IRQn);
}
void MotoInit(void)
{
	setUpPWM(50,50,50);
	PWMCtrl(ON);
}
void EnterStopMode(void)
{
	//enter stop mode
	TIMx_Disable(TIM2);
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
    ConfigWakeTrigger(1);
    uart_exti_init();
    rtcsecond = RTC_GetCounter();
    
    RTC_WaitForLastTask();
    RTC_SetAlarm(rtcsecond + 2);
    //RTC_EXTI_INITIAL(ENABLE);  
    /* Clear RTC Alarm interrupt pending bit */
    RTC_ClearITPendingBit(RTC_IT_ALR|RTC_IT_SEC|PWR_FLAG_WU);
    EXTI_ClearITPendingBit(EXTI_Line17);
		EXTI_ClearITPendingBit(EXTI_Line13);
    RTC_WaitForLastTask();
		
		delay_ms(20);
		EXTI_ClearITPendingBit(EXTI_Line17);
		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	
	
	//exit stop mode
	RCC_Configuration();
	USART_COM2_Init();
	ADC1_Init();
	getNFCCardFlag();
//(nfccard_flag == 1)
	if((alarm_flag == 0)&&(nfc_recv_flag == 1))
	{
		UartRBuf_Uart2.receFlag = 1;
	}
    //ConfigWakeTrigger(0);
	if(alarm_flag == 1)
	{
		alarm_flag = 0;
	}
	if(nfc_recv_flag == 1)
	{
		nfc_recv_flag = 0;
	}
}

void InitMPhaseBsp(void)
{
	RCC_Configuration();
	prvSetupHardware();	
	InitBspGPIO();
	delay_init();

	TimeTickInit();
    USART_COM1_Init();
	USART_COM2_Init();
    RTC_Init();	
//	USART_COM2_Init();

//	USART_COM3_Init();
//	UART_COM4_Init();
	

}
