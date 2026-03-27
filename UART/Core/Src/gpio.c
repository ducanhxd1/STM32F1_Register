/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 2 */
void GPIOx_Init(GPIO_TypeDef *GPIOx, uint8_t Pin, uint8_t Mode, uint8_t Pull, uint8_t Speed)
{
	if(GPIOx == GPIOA) {
		_HAL_RCC_GPIOA_CLK_ENABLE();
	} else if (GPIOx == GPIOB) {
		_HAL_RCC_GPIOB_CLK_ENABLE();
	} else if (GPIOx == GPIOC) {
		_HAL_RCC_GPIOC_CLK_ENABLE();
	} else if (GPIOx == GPIOD) {
		_HAL_RCC_GPIOD_CLK_ENABLE();
	} else if (GPIOx == GPIOE) {
		_HAL_RCC_GPIOE_CLK_ENABLE();
	}

	if (Pin < 8) {
		GPIOx->CRL &= ~(0xF << (Pin * 4));  // GPIOA -> CRL &= ~(0xF << 20);
		GPIOx->CRL |= (((Mode << 2) + Speed) << (Pin * 4));
	} else {
		GPIOx->CRH &= ~(0xF << ((Pin-8) * 4));  // GPIOA -> CRL &= ~(0xF << 20);
		GPIOx->CRH |= (((Mode << 2) + Speed) << ((Pin-8) * 4));
	}

	if(Pull == PU) {
		GPIOx->ODR |= 1 << Pin; // PULL UP
	} else {
		GPIOx->ODR &= ~(1 << Pin); // PULL DOWN
	}
}

void GPIO_WriteBit(GPIO_TypeDef *GPIOx, uint8_t Pin, uint8_t pinState) {
	if(pinState != 0) {
		GPIOx->BSRR |= (1 << Pin);  // SET
	} else {
		GPIOx->BRR |= (1 << (Pin + 16)); // RESET
	}
}

uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint8_t Pin) {
	uint8_t bitstatus;

	if((GPIOx->IDR & (1 << Pin)) != 0) {
		bitstatus = 1;
	} else {
		bitstatus = 0;
	}

	return bitstatus;
}

void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint8_t Pin) {
	GPIOx->ODR ^= (1 << Pin);
}

/* USER CODE END 2 */
