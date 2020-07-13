
#ifndef __SETUPTIMER_H
#define __SETUPTIMER_H

#include "Ctypedef.h"
#include "stm32f10x.h"

void TIMx_Disable(TIM_TypeDef* TIMx);
void TIMx_Enable(TIM_TypeDef* TIMx);
void TIMx_Setus(TIM_TypeDef* TIMx,UINT16 value);
void Timx_SetupNVIC(uint8_t IRQn_channel);
void setUpPWM(UINT32 freq,float duty1,float duty2);
void PWMCtrl(UINT8 ctl);
void setPwmDuty1(float duty);
void setPwmDuty2(float duty);

#endif

