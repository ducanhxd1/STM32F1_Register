/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#define INPUT_ANALOG 				((uint8_t) 0x00)
#define INPUT_FLOATING 				((uint8_t) 0x01)
#define INPUT_PUPD					((uint8_t) 0x02)

#define	OUTPUT_PP					((uint8_t) 0x00)
#define	OUTPUT_OD					((uint8_t) 0x01)
#define	OUTPUT_AF_PP				((uint8_t) 0x02)
#define	OUTPUT_AF_OD				((uint8_t) 0x03)

#define MODE_INPUT					((uint8_t) 0x00)
#define MODE_OUTPUT_10MHZ			((uint8_t) 0x01)
#define MODE_OUTPUT_2MHZ			((uint8_t) 0x02)
#define MODE_OUTPUT_50MHZ			((uint8_t) 0x03)

#define NOPULL						((uint8_t) 0x00)
#define PU							((uint8_t) 0x01)
#define PD 							((uint8_t) 0x02)

/* GPIO Ports Clock Configuration */
#define _HAL_RCC_AFIO_CLK_ENABLE()		(RCC->APB2ENR |= (1 << 0))
#define _HAL_RCC_GPIOA_CLK_ENABLE()		(RCC->APB2ENR |= (1 << 2))
#define _HAL_RCC_GPIOB_CLK_ENABLE()		(RCC->APB2ENR |= (1 << 3))
#define _HAL_RCC_GPIOC_CLK_ENABLE()		(RCC->APB2ENR |= (1 << 4))
#define _HAL_RCC_GPIOD_CLK_ENABLE()		(RCC->APB2ENR |= (1 << 5))
#define _HAL_RCC_GPIOE_CLK_ENABLE()		(RCC->APB2ENR |= (1 << 6))

#define _HAL_RCC_GPIOA_CLK_DISABLE()		(RCC->APB2ENR &= ~(1 << 2))
#define _HAL_RCC_GPIOB_CLK_DISABLE()		(RCC->APB2ENR &= ~(1 << 3))
#define _HAL_RCC_GPIOC_CLK_DISABLE()		(RCC->APB2ENR &= ~(1 << 4))
#define _HAL_RCC_GPIOD_CLK_DISABLE()		(RCC->APB2ENR &= ~(1 << 5))
#define _HAL_RCC_GPIOE_CLK_DISABLE()		(RCC->APB2ENR &= ~(1 << 6))


/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

void GPIOx_Init(GPIO_TypeDef *GPIOx, uint8_t Pin, uint8_t Mode, uint8_t Pull, uint8_t Speed);
void GPIO_WriteBit(GPIO_TypeDef *GPIOx, uint8_t Pin, uint8_t bit);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

