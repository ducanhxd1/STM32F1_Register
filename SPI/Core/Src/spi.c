/*
 * spi.c
 *
 *  Created on: Mar 6, 2026
 *      Author: DucAnh
 */

#include "spi.h"


void SPIx_Init(SPI_TypeDef *SPIx, uint16_t Mode, uint32_t Speed, uint8_t Size)
{
	RCC->APB2ENR |= 1 << 0; // AFIO
	if (SPIx == SPI1) {
		RCC->APB2ENR |= 1 << 12; // spi1
		GPIOx_Init(GPIOA, 5, OUTPUT_AF_PP, NOPULL, MODE_OUTPUT_50MHZ); // CLK
		GPIOx_Init(GPIOA, 6, INPUT_FLOATING, NOPULL, MODE_INPUT); // MISO
		GPIOx_Init(GPIOA, 7, OUTPUT_AF_PP, NOPULL, MODE_OUTPUT_50MHZ); // MOSI
	}

	if (SPIx == SPI2) {
		RCC->APB1ENR |= 1 << 14; // spi2
		GPIOx_Init(GPIOB, 13, OUTPUT_AF_PP, NOPULL, MODE_OUTPUT_50MHZ); // CLK
		GPIOx_Init(GPIOB, 14, INPUT_FLOATING, NOPULL, MODE_INPUT); // MISO
		GPIOx_Init(GPIOB, 15, OUTPUT_AF_PP, NOPULL, MODE_OUTPUT_50MHZ); // MOSI
	}

	SPIx->CR1 |= Mode;
	SPIx->CR1 |= Size;

	// SPI_DIRECTION_2LINES

	// SPI_DATASIZE_8BIT

	// SPI_POLARITY_LOW

//	SPI_PHASE_1EDGE

	// SPI_NSS_SOFT

	// SPI_BAUDRATEPRESCALER_8


}


