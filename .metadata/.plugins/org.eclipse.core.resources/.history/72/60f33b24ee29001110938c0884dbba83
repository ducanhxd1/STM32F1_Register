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
    I2C_MODE_STANDARD = 0,
    I2C_MODE_FAST     = 1
} I2C_Mode;

typedef enum {
    I2C_DUTY_1_2  = 0, // Fast mode 1:2
    I2C_DUTY_16_9 = 1  // Fast mode 16/9
} I2C_Duty;

void I2Cx_init(I2C_TypeDef *I2Cx, Pins_I2C pins, uint32_t speed, I2C_Mode mode, I2C_Duty duty);

#endif /* _I2C_H_ */
