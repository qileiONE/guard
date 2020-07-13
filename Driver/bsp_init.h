
#ifndef		__bsp_init
#define		__bsp_init

#include "stm32f10x.h"

#define		SysTick_time	1	//ms	<  1000


#define		LED_ERR_ON()		GPIOE->BRR  = GPIO_Pin_1
#define		LED_ERR_OFF()		GPIOE->BSRR = GPIO_Pin_1
#define		LED_RUN_ON()		GPIOA->BRR  = GPIO_Pin_8
#define		LED_RUN_OFF()		GPIOA->BSRR = GPIO_Pin_8


#define 	LED_LOW    				GPIO_ResetBits(GPIOB,GPIO_Pin_8)
#define 	LED_HI   				GPIO_SetBits(GPIOB,GPIO_Pin_8)

#define 	MOTOR_OFF  				GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define 	MOTOR_ON   				GPIO_SetBits(GPIOA,GPIO_Pin_1)

#define		COM485_1TX()			GPIOC->BRR  = GPIO_Pin_1	//0  RX
#define		COM485_1RX()			GPIOC->BSRR = GPIO_Pin_1	//1  TX

#define		COM485_0TX()			GPIOC->BRR  = GPIO_Pin_0	//0  RX
#define		COM485_0RX()			GPIOC->BSRR = GPIO_Pin_0	//1  TX

#define		KeyRead()				(GPIOB->IDR & GPIO_Pin_8)//(GPIOA->IDR & GPIO_Pin_4)

void RCC_Configuration(void);
void prvSetupHardware(void);
void InitBspGPIO(void);
void ConfigWakeTrigger(uint8_t sta);
void uart_exti_init(void);
void IWDG_Init(void);
void USART_COM1_Init(void);
void USART_COM2_Init(void);
void USART_COM3_Init(void);
void UART_COM4_Init(void);

void EnterStopMode(void);
void InitMPhaseBsp(void);

#endif

