/*
 * uart.c
 *
 *  Created on: Mar 6, 2026
 *      Author: DucAnh
 */

#include "uart.h"
#include "gpio.h"

/*
 * 	Theo datasheet cua smt32f103
	PCLK1 to APB1 peripherals : USART2, USART3, UART4, UART5
	PCLK2 to APB2 peripherals : USART1
	// Ham cau hinh Uart khong dong bo
*/
void UARTx_Init(USART_TypeDef *USARTx, Pins_UART Pins, uint32_t baud)
{
	uint16_t mantisa = 0, fraction = 0;
	float USARTDIV = 0;
	uint32_t pclk = 0;

	if(USARTx == USART1) {
		pclk = HAL_RCC_GetPCLK2Freq();
		RCC->APB2ENR |= (1 << 14); // USART1 Clock Enable
	}

	if(USARTx == USART2) {
		pclk = HAL_RCC_GetPCLK1Freq();
		RCC->APB2ENR |= (1 << 14); // USART1 Clock Enable
	}

	if(USARTx == USART3) {
		pclk = HAL_RCC_GetPCLK1Freq();
		RCC->APB2ENR |= (1 << 14); // USART1 Clock Enable
	}

	switch (Pins) {
	case Pins_PA9PA10:
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
		GPIOx_Init(GPIOA, 9, OUTPUT_AF_PP, NOPULL, MODE_OUTPUT_50MHZ); // TX
		GPIOx_Init(GPIOA, 10, INPUT_FLOATING, NOPULL, 0);			   // RX
		break;

	case Pins_PA2PA3:
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
		GPIOx_Init(GPIOA, 2, OUTPUT_AF_PP, NOPULL, MODE_OUTPUT_50MHZ); // TX
		GPIOx_Init(GPIOA, 3, INPUT_FLOATING, NOPULL, 0);			   // RX
		break;

	case Pins_PB10PB11:
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
		GPIOx_Init(GPIOA, 10, OUTPUT_AF_PP, NOPULL, MODE_OUTPUT_50MHZ); // TX
		GPIOx_Init(GPIOA, 11, INPUT_FLOATING, NOPULL, 0);			   // RX
		break;

	default:
		break;
	}

	/* Disable USART */
	USARTx->CR1 &= ~USART_CR1_UE; // & ~(1 << 13)

	/* baudrate calculation */
	USARTDIV = pclk / (16*baud);

	mantisa = (uint16_t)USARTDIV;
	fraction = (USARTDIV - mantisa) * 16;

	USARTx->BRR = (mantisa << 4) | fraction;

	/*---- USART CR1 Configuration ------*/
	/* Configure the UART Word Length, Parity and mode:
		 Set the M bits
		 Set PCE and PS bits
		 Set TE and RE bits
	*/
	USARTx->CR1 &= ~(1 << 12); // M : 1 Start bit, 8 Data bits, n Stop bit
	USARTx->CR1 |= (1 << 2) | (1 << 3); // TE , RE enable

	/*---- USART CR2 Configuration ------*/
	/*
	 * Configure the UART Stop Bits: Set STOP[13:12] bits
	 */
	USARTx->CR2 &= ~(0x11 << 12);  // Stop bit = 1

	/* ENABLE USART */
	USARTx->CR1 |= USART_CR1_UE; // | (1 << 13)
}

void UARTx_SendChar(USART_TypeDef *USARTx, char data)
{
    while(!(USARTx->SR & USART_SR_TXE));
    USARTx->DR = data;
}

void UARTx_SendString(USART_TypeDef *USARTx, char *str)
{
    while(*str)
    {
    	UARTx_SendChar(USARTx, *str++);
    }
}

char UART_ReceiveChar(USART_TypeDef *USARTx)
{
    while(!(USARTx->SR & USART_SR_RXNE));
    return USARTx->DR;
}

void Ring_buffer_char_init(Ring_buffer_char_t* ring_buffer, void* buffer, uint16_t buffer_size)
{
	ring_buffer->tail_index = 0;
	ring_buffer->head_index = 0;
	ring_buffer->fill_size = 0;

	ring_buffer->buffer_size = buffer_size;
	ring_buffer->buffer = (uint8_t*)buffer;
}

void Ring_buffer_char_put(Ring_buffer_char_t* ring_buffer, uint8_t c)
{
	uint16_t next_tail_index;
	uint16_t next_head_index;

	ring_buffer->buffer[ring_buffer->tail_index] = c;

	next_tail_index = (++ring_buffer->tail_index) % ring_buffer->buffer_size;
	ring_buffer->tail_index = next_tail_index;

	// Overite khi full
	if (ring_buffer->fill_size == ring_buffer->buffer_size) {
		next_head_index = (++ring_buffer->head_index) % ring_buffer->buffer_size;
		ring_buffer->head_index = next_head_index;
	}
	else {
		ring_buffer->fill_size++;
	}
}

uint8_t Ring_buffer_char_get(Ring_buffer_char_t* ring_buffer)
{
	uint16_t ret = 0;
	uint16_t next_head_index;

	if (ring_buffer->fill_size) {
		ret = ring_buffer->buffer[ring_buffer->head_index];

		next_head_index = (++ring_buffer->head_index) % ring_buffer->buffer_size;
		ring_buffer->head_index = next_head_index;

		ring_buffer->fill_size--;
	}

	return ret;
}

bool Ring_buffer_char_is_empty(Ring_buffer_char_t* ring_buffer)
{
	return (ring_buffer->fill_size == 0) ? true : false;
}

bool Ring_buffer_char_is_full(Ring_buffer_char_t* ring_buffer)
{
	return (ring_buffer->fill_size == ring_buffer->buffer_size) ? true : false;
}
