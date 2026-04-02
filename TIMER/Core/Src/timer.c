#include "timer.h"

/*
 The time-base unit includes:
	- Counter register (TIMx_CNT)
	- Prescaler register (TIMx_PSC) : Timer clock = f_ck / (PSC + 1)
	- Auto-Reload register (TIMx_ARR)
*/
void TIMx_Init(TIM_TypeDef *TIMx, uint16_t ARR, uint16_t PSC)
{
	if (TIMx == TIM1) {
		RCC->APB2ENR |= (1U << 11);
	}
	if(TIMx == TIM2) {
		RCC->APB1ENR |= (1U << 0);
	}
	if(TIMx == TIM3) {
		RCC->APB1ENR |= (1U << 1);
	}
	if(TIMx == TIM4) {
		RCC->APB1ENR |= (1U << 2);
	}

	TIMx->CR1 = 0x0000; // Reset value

	// Down counter mode bit 4 CR1 default = 0, up counter
	// fCK_PSC / (PSC[15:0] + 1)
	// 72Mhz / 7199 + 1 = 10 khz timer clock speed
	// Or: 72Mhz / 71 + 1 = 1 Mhz timer clock speed
	TIMx->PSC = PSC - 1; // 7200 - 1

	// Set auto reload value = 100000
	// which should generate roughly 1 second interrupts
	TIMx->ARR = ARR; // 0xFFFF = 65535

	TIMx->CR1 |= (1 << 0); // Enable timer
}

void timerDelay_ms(TIM_TypeDef *TIMx, volatile uint32_t s) {
	for(s; s>0; s--) {
		// reset timer CNT = 0
		TIMx->EGR |= 0x0001; // bit UG
		// Wait until timer reaches to 1000
		// It is 1000 because timer is running at 1 MHz and 1000 will
		//   generate 1 milli-second
		while(TIMx->CNT < 1000); // break loop
	}
}

