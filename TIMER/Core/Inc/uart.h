/*
 * uart.h
 *
 *  Created on: Mar 6, 2026
 *      Author: DucAnh
 */

#ifndef _UART_H_
#define _UART_H_

#include "main.h"
#include <stdbool.h>

typedef enum {
	Pins_PA9PA10,	 // USART1
	Pins_PA6PA7,	 // USART1
	Pins_PA2PA3,	 // USART2
	Pins_PD5PD6,	 // USART2
	Pins_PB10PB11,	 // USART3
	Pins_PC10PC11,	 // USART3 - UART4
	Pins_PD8PD9,	 // USART3
	Pins_PA0PA1,	 // USAR4
	Pins_PC12PD2,	 // UART5
	Pins_PC6PC7,	 // USART6
	Pins_PG14PG9,	 // USART6
	Pins_PB8PE7,	 // UART7
	Pins_PE1PE0		 // UART8
} Pins_UART;

// Ring buffer
typedef struct {
	uint16_t tail_index;
	uint16_t head_index;
	uint16_t fill_size;
	uint16_t buffer_size;
	uint8_t* buffer;
} Ring_buffer_char_t;

void UARTx_Init(USART_TypeDef *USARTx, Pins_UART Pins, uint32_t baud);
void UARTx_SendChar(USART_TypeDef *USARTx, char data);
void UARTx_SendString(USART_TypeDef *USARTx, char *str);

void Ring_buffer_char_init(Ring_buffer_char_t* ring_buffer, void* buffer, uint16_t buffer_size);
void Ring_buffer_char_put(Ring_buffer_char_t* ring_buffer, uint8_t c);
uint8_t Ring_buffer_char_get(Ring_buffer_char_t* ring_buffer);
bool Ring_buffer_char_is_empty(Ring_buffer_char_t* ring_buffer);
bool Ring_buffer_char_is_full(Ring_buffer_char_t* ring_buffer);


#endif /* _UART_H_ */
