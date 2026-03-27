/*
 * spi.h
 *
 *  Created on: Mar 6, 2026
 *      Author: DucAnh
 */

#ifndef _SPI_H_
#define _SPI_H_

#include "main.h"
#include "gpio.h"
#include <stdint.h>

void SPIx_Init(SPI_TypeDef *SPIx, uint16_t Mode, uint32_t Speed, uint8_t Size);


#endif /* _SPI_H_ */
