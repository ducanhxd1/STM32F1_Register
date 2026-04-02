/*
 * adc.c
 *
 *  Created on: Mar 5, 2026
 *      Author: DucAnh
 */

#include <stdint.h>
#include "adc.h"


void ADCx_Init(ADC_TypeDef *ADCx, uint8_t Channel)
{
	if(ADCx == ADC1) {
		RCC->APB2ENR |= (1 << 9); // ADC1
	} else if (ADCx == ADC2) {
		RCC->APB2ENR |= (1 << 10); // ADC2
	}

	RCC->CFGR |= (2 << 14); // ADC clock = PCLK2/6

	if(Channel == ADC_Channel_0)
		GPIOx_Init(GPIOA, 0, INPUT_ANALOG, NOPULL, 0);
	if(Channel == ADC_Channel_1)
		GPIOx_Init(GPIOA, 1, INPUT_ANALOG, NOPULL, 0);
	if(Channel == ADC_Channel_2)
		GPIOx_Init(GPIOA, 2, INPUT_ANALOG, NOPULL, 0);
	if(Channel == ADC_Channel_3)
		GPIOx_Init(GPIOA, 3, INPUT_ANALOG, NOPULL, 0);
	if(Channel == ADC_Channel_4)
		GPIOx_Init(GPIOA, 4, INPUT_ANALOG, NOPULL, 0);
	if(Channel == ADC_Channel_5)
		GPIOx_Init(GPIOA, 5, INPUT_ANALOG, NOPULL, 0);
	if(Channel == ADC_Channel_6)
		GPIOx_Init(GPIOA, 6, INPUT_ANALOG, NOPULL, 0);
	if(Channel == ADC_Channel_7)
		GPIOx_Init(GPIOA, 7, INPUT_ANALOG, NOPULL, 0);

	ADCx->CR1 |= 1 << 8;  // SCAN MOD
	ADCx->CR2 |= 1 << 1;  // CON MOD
	ADCx->CR2 |= 0 << 11; // Right Alignment
	ADCx->CR2 |= 1 << 0;

//	ADCx->CR2 &= ~(0x07 << 17);
//	ADCx->CR2 |= (0x07 << 17);  // SWSTART

	ADCx->CR2 |= 1 << 22; // start conversion

	if (Channel > 9) {
		ADCx->SMPR1 &= ~(0x07 << ((Channel-10) * 3));
		ADCx->SMPR1 |= (0x07 << ((Channel-10) * 3));
	} else {
		ADCx->SMPR2 &= ~(0x07 << (Channel * 3));
		ADCx->SMPR2 |= (0x07 << (Channel * 3));
	}

	ADCx->SQR1 &= ~(0xf << 20);
	ADCx->SQR1 |= (0xf << 20); // 1 conversion

	ADCx->SQR3 = Channel;

	ADCx->CR2 |= 1 << 3;  // Reset calibration
	while(ADCx->CR2 & (1 << 3));

	ADCx->CR2 |= 1 << 2;  // Reset calibration
	while(ADCx->CR2 & (1 << 2));

	ADCx->CR2 |= 1 << 0; // EN
}

uint16_t ADC_Read(ADC_TypeDef *ADCx, uint8_t Channel)
{
	ADCx->SQR3 = Channel;

	ADCx->CR2 |= (1 << 22);

	while(!(ADCx->SR & (1 << 1)));

	return ADCx->DR;
}

//Hàm đọc ADC trung bình (lọc nhiễu)
//Cực kỳ hữu ích khi đọc:
//biến trở
//cảm biến analog
//pin
uint16_t ADC_Read_Average(ADC_TypeDef *ADCx, uint8_t samples, uint8_t Channel)
{
	uint32_t sum = 0;

	for(uint8_t i=0;i<samples;i++)
	{
		sum += ADC_Read(ADCx, Channel);
	}

	return sum / samples;
}
