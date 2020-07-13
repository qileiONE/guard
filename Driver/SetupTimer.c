
#include "includes.h"
#include "SetupTimer.h"


void TIMx_Setus(TIM_TypeDef* TIMx,UINT16 value)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	if(value <1)
		value = 1;
	TIM_DeInit(TIMx);
	// Time base configuration 
	TIM_TimeBaseStructure.TIM_Period = value;    
	TIM_TimeBaseStructure.TIM_Prescaler = 71;  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	
	TIM_ClearFlag(TIMx,TIM_FLAG_Update);//清中断,以免一启用中断后立即产生中断
	// TIM IT enable  使能中断源
	TIM_ITConfig(TIMx,TIM_IT_Update, DISABLE);
	// TIM2 总开关,开启enable counter 
	
	TIM_Cmd(TIMx, DISABLE);	
}
void TIMx_Disable(TIM_TypeDef* TIMx)
{
	TIM_Cmd(TIMx, DISABLE);
    TIM_ITConfig(TIMx,TIM_IT_Update, DISABLE);
}
void TIMx_Enable(TIM_TypeDef* TIMx)
{
    TIM_ClearFlag(TIMx,TIM_FLAG_Update);
    TIM_ITConfig(TIMx,TIM_IT_Update, ENABLE);
	TIM_Cmd(TIMx, ENABLE);    
}


void Timx_SetupNVIC(uint8_t IRQn_channel)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the TIM3 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = IRQn_channel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

//===========================================
//		freq : UINT Hz
//		duty : xx.yy %
//===========================================
void setUpPWM(UINT32 freq,float duty1,float duty2)
{ 	
	UINT32	tmp32;
	UINT16	prsc,peri;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* Compute the prescaler value */
	tmp32 = (SystemCoreClock / freq);
	prsc  = tmp32/0xffff;
	peri  = tmp32 / (prsc + 1) -1;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = peri;
	TIM_TimeBaseStructure.TIM_Prescaler = prsc;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = peri * duty1/100;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = peri * duty2/100;
	
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	/* PWM1 Mode configuration: Channel3 */
//	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//	TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
//	
//	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
//	
//	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	/* PWM1 Mode configuration: Channel4 */
//	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
//	
//	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
//	
//	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	/* TIM3 enable counter */
//	TIM_Cmd(TIM3, ENABLE);
}
void PWMCtrl(UINT8 ctl)
{
	if(ctl == ON)
	{
		TIM_Cmd(TIM3, ENABLE);
	}
	else
	{
		TIM_Cmd(TIM3, DISABLE);
	}
}
void setPwmDuty1(float duty)
{
	UINT16	tmp16;
	tmp16 = duty *	TIM_GetCounter(TIM3)/100;
	TIM_SetCompare1(TIM3,tmp16);
}
void setPwmDuty2(float duty)
{
	UINT16	tmp16;
	tmp16 = duty *	TIM_GetCounter(TIM3)/100;
	TIM_SetCompare2(TIM3,tmp16);
}

