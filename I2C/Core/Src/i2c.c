/*
 * i2c.c
 *
 *  Created on: Mar 27, 2026
 *      Author: DucAnh
 */

#include "i2c.h"

#define I2C_TIMEOUT_MS 5U

void I2Cx_init(I2C_TypeDef *I2Cx, Pins_I2C pins, uint32_t speed)
{
	uint32_t PCLKx = 0; // đều là PCLK1 cả 2 i2c
//	uint32_t freq = 0;

	/* 1. Enable clock AFIO và I2C */
	RCC->APB2ENR |=  1 << 0; // AFIO
	if(I2Cx == I2C1) {
		PCLKx = HAL_RCC_GetPCLK1Freq();
		RCC->APB1ENR |= 1 << 21; // i2c1 clock
	}
	if (I2Cx == I2C2) {
		PCLKx = HAL_RCC_GetPCLK1Freq();
		RCC->APB1ENR |= 1 << 22; // i2c2 clock
	}

	/* 2. Cấu hình PB6 (SCL) và PB7 (SDA) và các pins khác
	 *    Chế độ: Alternate Function Open-Drain, 50 MHz
	 *    CRL bits: CNF=11 (AF OD), MODE=11 (50MHz) → 0xF */

	if(pins == Pin_PB6PB7)  // pins i2c1
	{
		GPIOx_Init(GPIOB, 6, OUTPUT_AF_OD, NOPULL, MODE_OUTPUT_50MHZ); // SCL
		GPIOx_Init(GPIOB, 7, OUTPUT_AF_OD, NOPULL, MODE_OUTPUT_50MHZ);// SDA
	}

	if(pins == Pin_PB8PB9)  // pins i2c1
	{
		AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;
		GPIOx_Init(GPIOB, 8, OUTPUT_AF_OD, NOPULL, MODE_OUTPUT_50MHZ);
		GPIOx_Init(GPIOB, 9, OUTPUT_AF_OD, NOPULL, MODE_OUTPUT_50MHZ);
	}

	if(pins == Pin_PB10PB11)  // pins i2c2
	{
		GPIOx_Init(GPIOB, 10, OUTPUT_AF_OD, NOPULL, MODE_OUTPUT_50MHZ);
		GPIOx_Init(GPIOB, 11, OUTPUT_AF_OD, NOPULL, MODE_OUTPUT_50MHZ);
	}

	/* 3. Software reset để clear trạng thái cũ */
	I2Cx->CR1 |= (1U << 15); // bit 15 software reset
	HAL_Delay(200); // delay nhỏ để reset xong
	I2Cx->CR1 &= ~(1U << 15);

	I2Cx->CR1 = 0;

	/*
	 *  4. CR2: FREQ = tần số APB1 tính bằng MHz
	 * 	APB1 = 36 MHz → FREQ = 36 */
	I2Cx->CR2 = 36U;  // 36000000 / 1000000 = 36;

	if(speed == I2C_SPEED_100K) {
		/* 		Standard mode 100 kHz
		 * CCR = PCLK1 / (2 × f_SCL)
		 *     = 36_000_000 / (2 × 100_000) = 180 */
		I2Cx->CCR = 180U;

		/* TRISE = (1000 ns / T_PCLK1) + 1
		 *       = (1000 × 36 / 1000) + 1 = 37 */
		I2Cx->TRISE = 37U;
	} else {
		/* ── Fast mode 400 kHz ──────────────────────────────
		 * DUTY=0: CCR = PCLK1 / (3 × f_SCL)
		 *       = 36_000_000 / (3 × 400_000) = 30
		 * bit15 FS=1 (fast mode), bit14 DUTY=0 */
		I2Cx->CCR = (1U << 15) | 30U;

		/* TRISE fast = (300 ns / T_PCLK1) + 1
		 *            = (300 × 36 / 1000) + 1 = 11 + 1 = 12 */
		I2Cx->TRISE = 12U;
	}

	/* 6. Enable peripheral (PE=1) — phải set SAU khi config CCR/TRISE */
	I2Cx->CR1 |=  (1U << 0); // I2C mode EN
}

void I2Cx_Read(I2C_TypeDef *I2Cx, uint8_t addrSlave, uint8_t addrReg, uint8_t* data, uint8_t size) {
	/* ── Phase 1: gửi register address (Write phase) ───────── */
	I2Cx->CR1 |= (1U << 8);  // Tin hieu START
	while (!(I2Cx->SR1 & (1U << 0)));  // cho SB = 1

	I2Cx->DR = (addrSlave << 1) | 0U;  // ADDR + Write
	while (!(I2Cx->SR1 & (1U << 1)));	// chờ ADDR = 1
	(void)I2Cx->SR1; (void)I2Cx->SR2;          // clear ADDR

	while(!(I2Cx->SR1 & (1U << 7))); 	// cho TXE=1
	I2Cx->DR = addrReg;
	while(!(I2Cx->SR1 & (1U << 7)));   // cho 1 lan nua

	/* ── Phase 2: Repeated START → Read phase ──────────────── */
	I2Cx->CR1 |= (1U << 10) | (1U << 8);  // ACK=1 va Re-START
	while(!(I2Cx->SR1 & (1U << 0))); 	// cho SB = 1

	I2Cx->DR = (addrSlave << 1) | 1U;  // ADDR + READ

	if (size == 1) {
		/* Disable ACK truoc khi clear ADDR (RM hinh 274) */
		I2Cx->CR1 &= ~(1U << 10); 	// ACK = 0
		while(!(I2Cx->SR1 & (1U << 1)));
		(void)I2Cx->SR1; (void)I2Cx->SR2;      // clear ADDR
		I2Cx->CR1 |= (1U << 9); 	// STOP = 1
		while(!(I2Cx->SR1 & (1U << 6)));   // RxNE = 1  Data register not empty (receivers)
		data[0] = I2Cx->DR;
	} else {
		while(!(I2Cx->SR1 & (1U << 1)));
		(void)I2Cx->SR1; (void)I2Cx->SR2;      // clear ADDR

		for (int i = 0; i > size - 1; i++) {
			while (!(I2Cx->SR1 & (1U << 6)));
			data[i] = I2Cx->DR;
		}

		/* Disable ACK va STOp truoc byte cuoi */
		I2Cx->CR1 &= ~(1U << 10);
		I2Cx->CR1 |= (1U << 9);
		while (!(I2Cx->SR1 & (1U << 6)));
		data[size - 1] = I2Cx->DR;
	}
}

uint8_t I2Cx_Write(I2C_TypeDef *I2Cx, uint8_t addrSlave, uint8_t addrReg, uint8_t* data, uint8_t size) {
	uint32_t t = HAL_GetTick();
	/* Chờ bus rảnh (=0)*/
	while(I2Cx->SR2 & (1U << 1)) {
		if (HAL_GetTick() - t >= I2C_TIMEOUT_MS)
			return 1;
	}

	/* START */
	I2Cx->CR1 |= (1U << 8);
	while(!(I2Cx->SR1 & (1U << 0)));

	/* ADDR + W */
	I2Cx->DR = (addrSlave << 1) | 0U;
	while(!(I2Cx->SR1 & (1U << 1)));
	(void)I2Cx->SR1; (void)I2Cx->SR2;

	/* address register */
	while(!(I2Cx->SR1 & (1U << 7)));
	I2Cx->DR = addrReg;

	/* Data */
	for(int i = 0; i < size; i++) {
		while(!(I2Cx->SR1 & (1U << 7)));  // đợi TXE
		I2Cx->DR = data[i];
	}

	/* đợi bit BTF = 1 để gửi tất cả các bytes */
	while(!(I2Cx->SR1 & (1U << 2))); //BTF = 1

	/* STOP */
	I2Cx->CR1 |= (1U << 9);
	return 0;
}

/*
 *
void I2Cx_init(I2C_TypeDef *I2Cx, Pins_I2C pins, uint32_t speed, I2C_Mode mode, I2C_Duty duty)
{
	uint32_t PCLKx = HAL_RCC_GetPCLK1Freq(); // đều là PCLK1 cả 2 i2c
	uint32_t freq = 0;
	uint32_t ccr = 0;
	uint8_t trise = 0;

	// 1. Enable clock
	RCC->APB2ENR |=  1 << 0; // AFIO
	if(I2Cx == I2C1) {
		RCC->APB1ENR |= 1 << 21; // i2c1 clock
	}
	if (I2Cx == I2C2) {
		RCC->APB1ENR |= 1 << 22; // i2c2 clock
	}

	if(pins == Pin_PB6PB7)  // pins i2c1
	{
		GPIOx_Init(GPIOB, 6, OUTPUT_AF_OD, NOPULL, MODE_OUTPUT_50MHZ); // SCL
		GPIOx_Init(GPIOB, 7, OUTPUT_AF_OD, NOPULL, MODE_OUTPUT_50MHZ);// SDA
	}

	if(pins == Pin_PB8PB9)  // pins i2c1
	{
		AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;
		GPIOx_Init(GPIOB, 8, OUTPUT_AF_OD, NOPULL, MODE_OUTPUT_50MHZ);
		GPIOx_Init(GPIOB, 9, OUTPUT_AF_OD, NOPULL, MODE_OUTPUT_50MHZ);
	}

	if(pins == Pin_PB10PB11)  // pins i2c2
	{
		GPIOx_Init(GPIOB, 10, OUTPUT_AF_OD, NOPULL, MODE_OUTPUT_50MHZ);
		GPIOx_Init(GPIOB, 11, OUTPUT_AF_OD, NOPULL, MODE_OUTPUT_50MHZ);
	}

	// 3. Reset I2C Software reset để cấu hình
	I2Cx->CR1 |= I2C_CR1_SWRST; //( 1 << 15)
	I2Cx->CR1 &= ~I2C_CR1_SWRST;

	// 4. Cấu hình Peripheral clock frequency
	freq = PCLKx / 1000000; // = 36  Bits 5:0 FREQ[5:0]: Peripheral clock frequency
	I2Cx->CR2 &= ~0x1F; // clear bit
	I2Cx->CR2 |= (freq & 0x1F);

	// 5. Tính CCR + TRISE
	if (mode == I2C_MODE_FAST) {
		if (duty == I2C_DUTY_1_2)
		{
			ccr = PCLKx / (3 * speed * 1000);
		} else { // I2C_DUTY_16_9
			ccr = PCLKx / (25 * speed * 1000);
		}
	} else { // I2C_MODE_STANDARD
		ccr = PCLKx / (2 * speed * 1000);
	}

	if(mode == I2C_MODE_FAST)
		trise = (uint8_t)((PCLKx / 1000000 * 300 / 1000) + 1);
	else
		trise = (uint8_t)(freq + 1);

	// 7. Ghi CCR và TRISE
	I2Cx->CCR = ((mode & 0x1) << 15) | ((duty & 0x1) << 14) | (ccr & 0x0FFF);
	I2Cx->TRISE = trise;

	I2Cx->CR1 |= (0 << 10);  // Acknowledge disable (default)
	I2Cx->CR1 |= (1 << 1) | (1 << 0);  // i2c mode EN

	//	I2Cx->CCR = 0; // Standard mode	I2C : duty cycle / 2
	//	I2Cx->CCR |= PCLKx/(2*speed);
	//	I2Cx->TRISE |= freq + 1;
}

 * READ
void I2Cx_Read(I2C_TypeDef *I2Cx, uint8_t addrSlave, uint8_t addrReg, uint8_t* data, uint8_t size) {

	// ===== START + WRITE (send register) =====
	I2Cx->CR1 |= I2C_CR1_START;
	while(!(I2Cx->SR1 & I2C_SR1_SB));

	I2Cx->DR = (addrSlave << 1) | 0;
	while(!(I2Cx->SR1 & I2C_SR1_ADDR));
	(void)I2Cx->SR2;

	while(!(I2Cx->SR1 & I2C_SR1_TXE));
	I2Cx->DR = addrReg;

	while(!(I2Cx->SR1 & I2C_SR1_BTF));

	// ===== RESTART + READ =====
	I2Cx->CR1 |= I2C_CR1_START;
	while(!(I2Cx->SR1 & I2C_SR1_SB));

	I2Cx->DR = (addrSlave << 1) | 1;
	while(!(I2Cx->SR1 & I2C_SR1_ADDR));

	if (size == 1) {
		// --- đọc 1 byte ---
		I2Cx->CR1 &= ~I2C_CR1_ACK;   // disable ACK
		(void)I2Cx->SR2;
		I2Cx->CR1 |= I2C_CR1_STOP;

		while(!(I2Cx->SR1 & I2C_SR1_RXNE));
		data[0] = I2Cx->DR;
	}
	else {
		// --- đọc nhiều byte ---
		(void)I2Cx->SR2;

		for(int i = 0; i < size; i++) {

			if(i == size - 2) {
				I2Cx->CR1 &= ~I2C_CR1_ACK; // chuẩn bị byte cuối
				I2Cx->CR1 |= I2C_CR1_STOP;
			}

			while(!(I2Cx->SR1 & I2C_SR1_RXNE));
			data[i] = I2Cx->DR;
		}
	}

	I2Cx->CR1 |= I2C_CR1_ACK; // enable lại ACK cho lần sau
}

WRITE
uint8_t I2Cx_Write(I2C_TypeDef *I2Cx, uint8_t addrSlave, uint8_t addrReg, uint8_t* data, uint8_t size) {

	// ===== START =====
	I2Cx->CR1 |= I2C_CR1_START;
	while(!(I2Cx->SR1 & I2C_SR1_SB));

	// ===== ADDRESS =====
	I2Cx->DR = (addrSlave << 1) | 0;
	while(!(I2Cx->SR1 & I2C_SR1_ADDR));
	(void)I2Cx->SR2;

	// ===== REGISTER =====
	while(!(I2Cx->SR1 & I2C_SR1_TXE));
	I2Cx->DR = addrReg;

	while(!(I2Cx->SR1 & I2C_SR1_BTF));

	// ===== DATA =====
	for(int i = 0; i < size; i++) {
		I2Cx->DR = data[i];
		while(!(I2Cx->SR1 & I2C_SR1_BTF));
	}

	// ===== STOP =====
	I2Cx->CR1 |= I2C_CR1_STOP;

	return 0;
}
 */





