//
// Created by marijn on 4/19/24.
//

#ifndef STM32H7B0_CMSIS_OSPI_H
#define STM32H7B0_CMSIS_OSPI_H
#include "main.h"
#include "sys.h"
#include "gpio.h"


/*!<
 * defines
 * */
#define OSPI_DEFAULT_NSS_HIGH_TIME 0
#define OSPI_DEFAULT_NSS_BOUNDARY 3  /* HAL: 0, recomended: 3 */
#define OSPI_DEFAULT_DLYB_BYPASS 1

/*!<
 * types
 * */
typedef enum {
	OSPI_CLK_SRC_AHB =			0b00,	//R
	OSPI_CLK_SRC_PLL1_Q =		0b01,
	OSPI_CLK_SRC_PLL2_R =		0b10,
	OSPI_CLK_SRC_PER =			0b11
} OSPI_CLK_SRC_t;

// enum sub {
//		uint8_t port	: 1;
//		uint8_t io		: 1;
//		uint8_t io_h	: 1;
// };
typedef enum {
	OSPIM_PIN_DISABLE =		0x00000000,
	// OSPIM_PORT1
	OSPIM_PORT1_IO3_A1 =	0x01090800,		OSPIM_PORT1_IO3_D13 =	0x0D390800,
	OSPIM_PORT1_NSCK_B12 =	0x0C130000,		OSPIM_PORT1_SCK_F10 =	0x0A590000,
	OSPIM_PORT1_IO5_D5 =	0x053A1800,		OSPIM_PORT1_IO0_B1 =	0x011B0800,
	OSPIM_PORT1_NSS_E11 =	0x0B4B0000,		OSPIM_PORT1_DQS_C5 =	0x052A0000,
	OSPIM_PORT1_DQS_A1 =	0x010B0000,		OSPIM_PORT1_IO2_E2 =	0x02490800,
	OSPIM_PORT1_IO4_C1 =	0x012A1800,		OSPIM_PORT1_NSCK_F11 =	0x0B590000,
	OSPIM_PORT1_IO6_D6 =	0x063A1800,		OSPIM_PORT1_SCK_B2 =	0x02190000,
	OSPIM_PORT1_IO3_F6 =	0x065A0800,		OSPIM_PORT1_IO0_C9 =	0x09290800,
	OSPIM_PORT1_SCK_A3 =	0x03030000,		OSPIM_PORT1_IO4_E7 =	0x074A1800,
	OSPIM_PORT1_IO2_C2 =	0x02290800,		OSPIM_PORT1_NSS_G6 =	0x066A0000,
	OSPIM_PORT1_IO7_D7 =	0x073A1800,		OSPIM_PORT1_DQS_B2 =	0x021A0000,
	OSPIM_PORT1_IO2_F7 =	0x075A0800,		OSPIM_PORT1_IO1_C10 =	0x0A290800,
	OSPIM_PORT1_IO3_A6 =	0x06060800,		OSPIM_PORT1_IO5_E8 =	0x084A1800,
	OSPIM_PORT1_IO5_C2 =	0x022B1800,		OSPIM_PORT1_IO6_G9 =	0x09691800,
	OSPIM_PORT1_IO0_D11 =	0x0B390800,		OSPIM_PORT1_NSS_B6 =	0x061A0000,
	OSPIM_PORT1_IO0_F8 =	0x085A0800,		OSPIM_PORT1_NSS_C11 =	0x0B290000,
	OSPIM_PORT1_IO2_A7 =	0x070A0800,		OSPIM_PORT1_IO6_E9 =	0x094A1800,
	OSPIM_PORT1_IO0_C3 =	0x03290800,		OSPIM_PORT1_IO7_G14 =	0x0E691800,
	OSPIM_PORT1_IO1_D12 =	0x0C390800,		OSPIM_PORT1_NSS_B10 =	0x0A190000,
	OSPIM_PORT1_IO1_F9 =	0x095A0800,		OSPIM_PORT1_IO4_D4 =	0x043A1800,
	OSPIM_PORT1_IO1_B0 =	0x001B0800,		OSPIM_PORT1_IO7_E10 =	0x0A4A1800,
	OSPIM_PORT1_IO6_C3 =	0x032B1800,		OSPIM_PORT1_IO4_H2 =	0x02791800,
	OSPIM_PORT1_IO5_H3 =	0x03791800,
	// OSPIM_PORT2
	OSPIM_PORT2_IO0_F0 =	0x00590C00,		OSPIM_PORT2_IO2_F2 =	0x02590C00,
	OSPIM_PORT2_SCK_F4 =	0x04590400,		OSPIM_PORT2_DQS_F12 =	0x0C590400,
	OSPIM_PORT2_IO5_G1 =	0x01691C00,		OSPIM_PORT2_IO6_G10 =	0x0A631C00,
	OSPIM_PORT2_NSS_G12 =	0x0C630400,		OSPIM_PORT2_IO0_I9 =	0x09830C00,
	OSPIM_PORT2_IO1_F1 =	0x01590C00,		OSPIM_PORT2_IO3_F3 =	0x03590C00,
	OSPIM_PORT2_NSCK_F5 =	0x05590400,		OSPIM_PORT2_IO4_G0 =	0x00691C00,
	OSPIM_PORT2_DQS_G7 =	0x07690400,		OSPIM_PORT2_IO7_G11 =	0x0B691C00,
	OSPIM_PORT2_DQS_G15 =	0x0F690400,		OSPIM_PORT2_IO1_I10 =	0x0A830C00,
	OSPIM_PORT2_IO2_I11 =	0x0B830C00,
} OSPIM_GPIO_t;

typedef enum {
	OSPIM_MUX_DISABLE =		0b0U,
	OSPIM_MUX_ENABLE =		0b1U
} OSPIM_MUX_t;

typedef enum {
	OSPI_MODE_DISABLE =		0b000U,
	OSPI_MODE_SINGLE =		0b001U,
	OSPI_MODE_DUAL =		0b010U,
	OSPI_MODE_QUAD =		0b011U,
	OSPI_MODE_OCTO =		0b100U,
} OSPI_MODE_t;  // TODO!!!

typedef enum {
	OPSI_MEMORY_MICRON =		0b000U,
	OPSI_MEMORY_MACRONIX =		0b001U,
	OPSI_MEMORY_STANDARD =		0b010U,
	OPSI_MEMORY_MACRONIX_RAM =	0b011U,
	OPSI_MEMORY_HYPERBUS =		0b100U,
	OPSI_MEMORY_HYPERBUS_REG =	0b101U
} OSPI_MEMORY_t;

typedef enum {
	OSPI_CLOCK_MODE_0 =			0b0U,
	OSPI_CLOCK_MODE_3 =			0b1U,
} OSPI_CLOCK_MODE_t;

typedef enum {
	OSPI_SIZE_8B =				0b00U,
	OSPI_SIZE_16B =				0b01U,
	OSPI_SIZE_24B =				0b10U,
	OSPI_SIZE_32B =				0b11U
} OSPI_SIZE_t;

typedef enum {
	OSPI_WRAP_SIZE_NONE =		0b000U,
	OSPI_WRAP_SIZE_16B =		0b010U,
	OSPI_WRAP_SIZE_32B =		0b011U,
	OSPI_WRAP_SIZE_64B =		0b100U,
	OSPI_WRAP_SIZE_128B =		0b101U
} OSPI_WRAP_SIZE_t;

typedef enum {
	OSPI_SHIFT_NONE =			0b0U,
	OSPI_SHIFT_HALF_CYCLE =		0b1U
} OSPI_SHIFT_t;

typedef enum {
	OSPI_HOLD_NONE =			0b0U,
	OSPI_HOLD_QUARTER_CYCLE =	0b1U
} OSPI_HOLD_t;

typedef struct __PACKED {  // TODO: redo names
	const uint8_t* buffer;					// 0x00
	uint32_t instruction;					// 0x04
	uint32_t address;						// 0x08
	uint32_t alt_bytes;						// 0x0C
	uint32_t size;							// 0x10
	uint8_t idtr					: 1;	// 0x14:0
	uint8_t addtr					: 1;	// 0x14:1
	uint8_t abdtr					: 1;	// 0x14:2
	uint8_t ddtr					: 1;	// 0x14:3
	OSPI_SIZE_t instruction_size	: 2;	// 0x14:4
	OSPI_SIZE_t address_size		: 2;	// 0x14:6
	OSPI_MODE_t imode				: 3;	// 0x15:0
	OSPI_MODE_t admode				: 3;	// 0x15:3
	OSPI_SIZE_t alt_bytes_size		: 2;	// 0x15:6
	OSPI_MODE_t abmode				: 3;	// 0x16:0
	OSPI_MODE_t dmode				: 3;	// 0x16:3
} OSPI_TX_t;	// 0x17 -> 23B


/*!<
 * variables
 * */
extern uint32_t OSPI12_kernel_frequency;


/*!<
 * init
 * */
void config_OSPI_kernel_clock(OSPI_CLK_SRC_t ospi_src);

/*!<
 * - make sure that OCTOSPIs connected are disabled
 * - IO0123 pins may be swapped with IO4567 (only as complete ports)
 * */  // TODO: DQS!!
void fconfig_OSPIM(
	OSPIM_GPIO_t sck, OSPIM_GPIO_t io0, OSPIM_GPIO_t io1, OSPIM_GPIO_t io2, OSPIM_GPIO_t io3,
	OSPIM_GPIO_t io4, OSPIM_GPIO_t io5, OSPIM_GPIO_t io6, OSPIM_GPIO_t io7, OSPIM_GPIO_t nss,
	uint8_t ospi_num, OSPIM_MUX_t mux, uint8_t mux_delay
);
// TODO: init struct!!!!
// TODO: dual Xspi!!
// TODO: mode??
void fconfig_OSPI(
	OCTOSPI_TypeDef* ospi,
	uint8_t prescaler, OSPI_MEMORY_t mem, uint8_t addr_size, uint8_t nss_high_time, uint8_t nss_boundary,
	uint8_t DLYB_bypass, OSPI_CLOCK_MODE_t clk_mode, OSPI_WRAP_SIZE_t wrap, uint8_t max_transfer,
	uint32_t refresh, uint8_t fifo_threshold, OSPI_SHIFT_t shift, OSPI_HOLD_t delay, uint8_t dummy_cycles
);

void fconfig_OSPI_MMAP(
	OCTOSPI_TypeDef* ospi,
	uint32_t read_instruction,	uint32_t write_instruction,	OSPI_SIZE_t instruction_size,
	OSPI_SIZE_t address_size,	uint32_t alt_bytes,			OSPI_SIZE_t alt_bytes_size,
	OSPI_MODE_t imode,	uint8_t idtr,	OSPI_MODE_t admode,	uint8_t addtr,
	OSPI_MODE_t abmode,	uint8_t abdtr,	OSPI_MODE_t dmode,	uint8_t ddtr
);

// default configs NO MUX!
void config_DSPI(
	OCTOSPI_TypeDef* ospi,
	OSPIM_GPIO_t sck, OSPIM_GPIO_t io0, OSPIM_GPIO_t io1, OSPIM_GPIO_t nss, uint8_t prescaler,
	OSPI_MEMORY_t mem, uint8_t addr_size
);
void config_QSPI(
	OCTOSPI_TypeDef* ospi,
	OSPIM_GPIO_t sck, OSPIM_GPIO_t io0, OSPIM_GPIO_t io1, OSPIM_GPIO_t io2, OSPIM_GPIO_t io3,
	OSPIM_GPIO_t nss, uint8_t prescaler, OSPI_MEMORY_t mem, uint8_t addr_size
);
void config_OSPI(
	OCTOSPI_TypeDef* ospi,
	OSPIM_GPIO_t sck, OSPIM_GPIO_t io0, OSPIM_GPIO_t io1, OSPIM_GPIO_t io2, OSPIM_GPIO_t io3,
	OSPIM_GPIO_t io4, OSPIM_GPIO_t io5, OSPIM_GPIO_t io6, OSPIM_GPIO_t io7, OSPIM_GPIO_t nss,
	uint8_t prescaler, OSPI_MEMORY_t mem, uint8_t addr_size
);


/*!<
 * usage
 * */
uint8_t OSPI_test_receive(OCTOSPI_TypeDef* ospi, uint8_t* buffer);
uint32_t OSPI_transmit(OCTOSPI_TypeDef* ospi, OSPI_TX_t* tx, uint32_t timeout);



#endif // STM32H7B0_CMSIS_OSPI_H
