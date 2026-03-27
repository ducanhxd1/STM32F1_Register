/*
 * bitbanding.h
 *
 *  Created on: Mar 5, 2026
 *      Author: DucAnh
 */

#ifndef _BITBANDING_H_
#define _BITBANDING_H_

/*
 *  	Giải thích công thức bitband
	1. Phân tích công thức Bit-banding trong Datasheet
	Theo tài liệu ARM Cortex-M3/M4, công thức chuẩn là:
		bit_word_addr = bit_band_base + (byte_offset x 32) + (bit_number × 4)
	Trong đó:
		SRAM: Gốc là 0x20000000, Alias bắt đầu từ 0x22000000.
		Peripheral: Gốc là 0x40000000, Alias bắt đầu từ 0x42000000.
		Khoảng cách giữa Gốc và Alias luôn là 0x02000000.

	2. Giải mã các con số trong Macro BITBAND
		Dòng code đầu tiên:
		#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x02000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))

	Hãy bóc tách từng cụm:
		(addr & 0xF0000000):
		- Mục đích: Xác định xem bạn đang ở vùng 0x20000000 (RAM) hay 0x40000000 (Ngoại vi).
		- Nó chỉ giữ lại con số đầu tiên (2 hoặc 4). Đây chính là Region_Base.

		+ 0x02000000:
		- Đây là bước chuyển từ vùng Gốc sang vùng Alias.
		- Nếu là RAM: 0x20000000 + 0x02000000 = 0x22000000 (đúng chuẩn Alias Base).
		- Nếu là Ngoại vi: 0x40000000 + 0x02000000 = 0x42000000.

		((addr & 0xFFFFF) << 5):
		- (addr & 0xFFFFF): Phép AND này giúp lấy ra phần "đuôi" của địa chỉ (offset).
							Vùng bit-band chỉ rộng 1MB, nên 0xFFFFF (20 bit)
							là đủ để đại diện cho mọi địa chỉ trong vùng đó. Đây là Byte_Offset.
		- << 5: Trong lập trình C, dịch trái 5 bit tương đương với nhân với 2^5(tức là 32).
		- Liên hệ: Chính là cụm "(byte_offset x 32)" trong công thức

		(bitnum << 2):
		- << 2: Dịch trái 2 bit tương đương với nhân với 2^2 (tức là 4).
		- Liên hệ: Chính là cụm "(bit_number × 4)" trong công thức.

	3. Ý nghĩa của các Macro còn lại
	MEM_ADDR(addr):
Nó ép kiểu con số addr thành một con trỏ kiểu volatile unsigned long* (số nguyên không dấu 32-bit).
Dấu * ngoài cùng dùng để lấy giá trị tại địa chỉ đó.
Tại sao có volatile? Để báo trình biên dịch không được tối ưu hóa dòng này, vì giá trị thanh ghi phần cứng có thể thay đổi bất cứ lúc nào.
	BIT_ADDR(addr, bitnum):
Đây là "trạm tổng hợp". Nó lấy địa chỉ đã tính toán từ BITBAND rồi đưa vào MEM_ADDR để bạn có thể đọc/ghi trực tiếp.

	4. Tại sao lại dùng dịch bit (<<) thay vì phép nhân (*)?
Trong lập trình nhúng:
Tốc độ: CPU xử lý phép dịch bit (<<) nhanh hơn rất nhiều so với phép nhân (*).
Tối ưu: Trình biên dịch sẽ chuyển mã này thành các lệnh xử lý cực kỳ gọn nhẹ, giúp code chạy nhanh nhất có thể (gần như tức thời).

*/

// IO
#define BITBAND(addr, bitnum)  ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr)	*((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) 	MEM_ADDR(BITBAND(addr, bitnum))

// Dia chi
#define GPIOA_ODR_Addr 	(GPIOA_BASE + 12)  // 0x4001080C
#define GPIOB_ODR_Addr 	(GPIOB_BASE + 12)  // 0x40010C0C
#define GPIOC_ODR_Addr 	(GPIOC_BASE + 12)  // 0x4001100C
#define GPIOD_ODR_Addr 	(GPIOD_BASE + 12)  // 0x4001140C
#define GPIOE_ODR_Addr 	(GPIOE_BASE + 12)  // 0x4001180C
#define GPIOF_ODR_Addr 	(GPIOF_BASE + 12)  // 0x40011A0C
#define GPIOG_ODR_Addr 	(GPIOG_BASE + 12)  // 0x40011E0C

#define GPIOA_IDR_Addr 	(GPIOA_BASE + 8)  // 0x40010808
#define GPIOB_IDR_Addr 	(GPIOB_BASE + 8)  // 0x40010C08
#define GPIOC_IDR_Addr 	(GPIOC_BASE + 8)  // 0x40011008
#define GPIOD_IDR_Addr 	(GPIOD_BASE + 8)  // 0x40011408
#define GPIOE_IDR_Addr 	(GPIOE_BASE + 8)  // 0x40011808
#define GPIOF_IDR_Addr 	(GPIOF_BASE + 8)  // 0x40011A08
#define GPIOG_IDR_Addr 	(GPIOG_BASE + 8)  // 0x40011E08

#define PortAOutput(n) BIT_ADDR(GPIOA_ODR_Addr, n) // ngo ra
#define PortAInput(n)  BIT_ADDR(GPIOA_IDR_Addr, n) // ngo vao

#define PortBOutput(n) BIT_ADDR(GPIOB_ODR_Addr, n)
#define PortBInput(n)  BIT_ADDR(GPIOB_IDR_Addr, n)

#define PortCOutput(n) BIT_ADDR(GPIOC_ODR_Addr, n)
#define PortCInput(n)  BIT_ADDR(GPIOC_IDR_Addr, n)

#define PortDOutput(n) BIT_ADDR(GPIOD_ODR_Addr, n)
#define PortDInput(n)  BIT_ADDR(GPIOD_IDR_Addr, n)

#define PortEOutput(n) BIT_ADDR(GPIOE_ODR_Addr, n)
#define PortEInput(n)  BIT_ADDR(GPIOE_IDR_Addr, n)

#define PortFOutput(n) BIT_ADDR(GPIOF_ODR_Addr, n)
#define PortFInput(n)  BIT_ADDR(GPIOF_IDR_Addr, n)

#define PortGOutput(n) BIT_ADDR(GPIOG_ODR_Addr, n)
#define PortGInput(n)  BIT_ADDR(GPIOG_IDR_Addr, n)


#endif /* _BITBANDING_H_ */
