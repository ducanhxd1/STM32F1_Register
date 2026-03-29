/*
 * lcd_i2c.h / lcd_i2c.c
 * LCD HD44780 driver qua PCF8574 I2C backpack
 * Dùng API: I2Cx_Write(I2Cx, addrSlave, addrReg, data, size)
 *
 * PCF8574 pin mapping:
 *   Bit 7 6 5 4  3  2  1  0
 *       D7D6D5D4 BL EN RW RS
 *
 * Địa chỉ mặc định: 0x27 (A0=A1=A2=1)
 * Nếu module của bạn là 0x3F thì đổi LCD_I2C_ADDR bên dưới
 */

#ifndef _LCD_I2C_H_
#define _LCD_I2C_H_

#include "stm32f1xx.h"
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

/* ── Địa chỉ I2C của PCF8574 ───────────────────────────────
 * 0x27: A0=A1=A2=1 (phổ biến)
 * 0x3F: A0=A1=A2=1 trên PCF8574A
 * Nếu không chắc: chạy I2C scanner để tìm */
#define LCD_I2C_ADDR    0x27U

/* ── Kích thước LCD ────────────────────────────────────────
 * Đổi thành 20, 4 nếu dùng LCD 20x4 */
#define LCD_COLS        16U
#define LCD_ROWS        2U

/* ── PCF8574 bit masks ─────────────────────────────────────
 * Tương ứng với wiring chuẩn của module I2C backpack */
#define LCD_RS          (1U << 0)   /* Register Select: 0=cmd, 1=data */
#define LCD_RW          (1U << 1)   /* Read/Write: luôn 0 (write)     */
#define LCD_EN          (1U << 2)   /* Enable pulse                    */
#define LCD_BL          (1U << 3)   /* Backlight                       */
/* D4–D7 = bit 4–7 của byte PCF8574 */

/* ── HD44780 Commands (datasheet §8) ───────────────────────*/
#define LCD_CMD_CLEAR           0x01U   /* Clear display, cursor home   */
#define LCD_CMD_HOME            0x02U   /* Cursor home (giữ data)       */
#define LCD_CMD_ENTRY_MODE      0x06U   /* Tăng cursor, không shift     */
#define LCD_CMD_DISPLAY_ON      0x0CU   /* Display on, cursor off       */
#define LCD_CMD_DISPLAY_ON_CUR  0x0EU   /* Display on, cursor on        */
#define LCD_CMD_DISPLAY_ON_BLINK 0x0FU  /* Display on, cursor blink     */
#define LCD_CMD_DISPLAY_OFF     0x08U
#define LCD_CMD_4BIT_2LINE      0x28U   /* 4-bit, 2 dòng, 5×8 font     */
#define LCD_CMD_SHIFT_LEFT      0x18U   /* Shift display trái           */
#define LCD_CMD_SHIFT_RIGHT     0x1CU   /* Shift display phải           */

/* DDRAM address của từng dòng (HD44780 datasheet §10) */
#define LCD_ROW0_ADDR   0x00U
#define LCD_ROW1_ADDR   0x40U
#define LCD_ROW2_ADDR   0x14U   /* chỉ LCD 20x4 */
#define LCD_ROW3_ADDR   0x54U   /* chỉ LCD 20x4 */

/* ── Public API ─────────────────────────────────────────── */

/**
 * LCD_Init — khởi tạo LCD ở 4-bit mode
 * @param I2Cx  : I2C1 hoặc I2C2
 * Gọi sau khi đã I2Cx_init() xong
 */
void LCD_Init(I2C_TypeDef *I2Cx);

/**
 * LCD_Clear — xóa toàn bộ màn hình, cursor về (0,0)
 * Tốn ~2ms (HD44780 cần thời gian xử lý)
 */
void LCD_Clear(I2C_TypeDef *I2Cx);

/**
 * LCD_SetCursor — đặt vị trí cursor
 * @param col : 0 → LCD_COLS-1
 * @param row : 0 → LCD_ROWS-1
 */
void LCD_SetCursor(I2C_TypeDef *I2Cx, uint8_t col, uint8_t row);

/**
 * LCD_WriteChar — in 1 ký tự tại vị trí cursor hiện tại
 */
void LCD_WriteChar(I2C_TypeDef *I2Cx, char c);

/**
 * LCD_WriteString — in chuỗi tại vị trí cursor hiện tại
 * Tự dừng khi gặp '\0' hoặc hết cột
 */
void LCD_WriteString(I2C_TypeDef *I2Cx, const char *str);

/**
 * LCD_Printf — in có format, giống printf
 * Ví dụ: LCD_Printf(I2C1, "Temp: %.1f C", 36.5f);
 */
void LCD_Printf(I2C_TypeDef *I2Cx, const char *fmt, ...);

/**
 * LCD_Backlight — bật/tắt đèn nền
 * @param on : 1=bật, 0=tắt
 */
void LCD_Backlight(I2C_TypeDef *I2Cx, uint8_t on);

/**
 * LCD_DisplayOn / LCD_DisplayOff — bật/tắt display (data vẫn giữ)
 */
void LCD_DisplayOn(I2C_TypeDef *I2Cx);
void LCD_DisplayOff(I2C_TypeDef *I2Cx);

/**
 * LCD_CreateChar — tạo ký tự đặc biệt (custom character)
 * @param slot : 0–7 (HD44780 có 8 slot CGRAM)
 * @param pattern : mảng 8 byte, mỗi byte = 1 hàng pixel (5 bit thấp)
 *
 * Ví dụ ký tự mũi tên lên:
 *   uint8_t arrow[] = {0x04,0x0E,0x1F,0x04,0x04,0x04,0x04,0x00};
 *   LCD_CreateChar(I2C1, 0, arrow);
 *   LCD_WriteChar(I2C1, '\x00');  // dùng slot 0
 */
void LCD_CreateChar(I2C_TypeDef *I2Cx, uint8_t slot, const uint8_t *pattern);

#endif /* LCD_I2C_H */
