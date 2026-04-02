#ifndef _TIMER_H_
#define _TIMER_H_

#include "main.h"

// f timer = 72Mhz, ARR, PSC = 72, 720, 7200,..., 36,...
void TIMx_Init(TIM_TypeDef *TIMx, uint16_t ARR, uint16_t PSC);
void timerDelay_ms(TIM_TypeDef *TIMx, volatile uint32_t s);

#endif /* INC_TIMER_H_ */
