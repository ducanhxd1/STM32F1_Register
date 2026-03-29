/*
 * i2c.h
 *
 *  Created on: Mar 27, 2026
 *      Author: DucAnh
 */

#ifndef _I2C_H_
#define _I2C_H_

#include "main.h"
#include "gpio.h"

typedef enum {
	Pin_PB6PB7, // I2C1
	Pin_PB8PB9, // I2C1
	Pin_PB10PB11, // I2C2
} Pins_I2C;

typedef enum {
    I2C_SPEED_100K = 0,
    I2C_SPEED_400K = 1,
} I2C_Speed;



void I2Cx_init(I2C_TypeDef *I2Cx, Pins_I2C pins, uint32_t speed);
void I2Cx_Read(I2C_TypeDef *I2Cx, uint8_t addrSlave, uint8_t addrReg, uint8_t* data, uint8_t size);
uint8_t I2Cx_Write(I2C_TypeDef *I2Cx, uint8_t addrSlave, uint8_t addrReg, uint8_t* data, uint8_t size);
#endif /* _I2C_H_ */
