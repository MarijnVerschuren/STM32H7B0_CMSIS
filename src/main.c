#include "gpio.h"
#include "exti.h"
#include "tim.h"
#include "pwm.h"
#include "encoder.h"
#include "usart.h"
#include "i2c.h"
#include "spi/spi.h"
#include "spi/ospi.h"
#include "crc.h"
#include "rng.h"
#include "watchdog.h"
#include "mco.h"
#include "usb/usb.h"
#include "usb/hid.h"
#include "hash.h"
#include "cryp.h"

#include "spi/W25Q64.h"


/*!<
 * variables
 * */
I2C_setting_t I2C_setting = {
	.I2C_clock_frequency = 4000000,
	.scl_l_pre = 0x13U, .scl_h_pre = 0x0FU,
	.sda_delay = 0x02U, .scl_delay = 0x04U
};  // 100 KHz
uint8_t HID_buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
volatile uint8_t GO = 0;

uint8_t cryp_IV[16] = {  // TODO: random and read
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};


/*!<
 * interrupts
 * */
extern void TIM8_UP_TIM13_IRQHandler(void) {
	TIM8->SR &= ~TIM_SR_UIF;  // clear interrupt flag
	//GPIO_toggle(GPIOC, 1);
}

extern void EXTI15_10_IRQHandler(void) {	// button K2
	EXTI->PR1 |= EXTI_PR1_PR13;  // clear interrupt flag
	//stop_TIM(TIM8);
	GO = 1;
}


/*!<
 * functions
 * */
int main(void) {
	/* clock config */
	set_PLL_config(
		0, 1, 1, 1, 0, 0,								// enable PLL1 (P, Q)
		PLL_IN_4MHz_8MHz, PLL_VCO_WIDE,					// 5MHz in, 192MHz < VCO < 960MHz
		5/*M*/, 2/*P*/, 4/*Q*/, 2/*R*/, 112/*N*/, 0		// M = 5, P = 2, Q = 2, R = 2, N = 112, N_frac = 0
	);  // 25MHz / 5 * 112 / (2, 2, 2)	=>	(280Mhz, 280Mhz, *280Mhz)
	set_PLL_config(
		1, 1, 1, 1, 1, 0,								// enable PLL2 (P, Q, R)
		PLL_IN_4MHz_8MHz, PLL_VCO_WIDE,					// 5MHz in, 192MHz < VCO < 960MHz
		5/*M*/, 5/*P*/, 5/*Q*/, 5/*R*/, 100/*N*/, 0		// M = 5, P = 5, Q = 5, R = 5, N = 100, N_frac = 0
	);  // 25MHz / 5 * 100 / (5, 5, 5)	=>	(100Mhz, 100Mhz, 100Mhz)
	set_PLL_config(
		2, 0, 0, 0, 0, 0,								// disable PLL3
		PLL_IN_4MHz_8MHz, PLL_VCO_WIDE,					// 5MHz in, 192MHz < VCO < 960MHz
		5/*M*/, 2/*P*/, 4/*Q*/, 2/*R*/, 112/*N*/, 0		// M = 5, P = 2, Q = 2, R = 2, N = 112, N_frac = 0
	);  // 25MHz / 5 * 112 / (2, 2, 2)	=>	(*280Mhz, *280Mhz, *280Mhz)
	set_RTC_config(0, RCC_SRC_DISABLED, 0);				// disable RTC
	set_clock_config(
		0, 1, 0, 0, 0, 1,								// disable HSI, enable HSE, enable HSI48
		0, 0, 1, 0, HSI_DIV_1, 25000000,				// enable HSE_CSS, HSE_freq = 25MHz
		PLL_SRC_HSE										// set HSE as PLL source clock
	);
	set_SYS_config(
		SYS_CLK_SRC_PLL1_P, SYS_CLK_DIV_1,				// SYS_CLK = PLL1_P
		CORE_VOS_1, FLASH_LATENCY3						// FLASH and PWR settings for AXI_freq = 280MHz, CORE_freq = 280MHz
	);
	set_domain_config(
		AHB_CLK_DIV_2, APB_CLK_DIV_2,					// AHB and AXI = 280MHz
		APB_CLK_DIV_2, APB_CLK_DIV_2, APB_CLK_DIV_2		// APB1-4 = 140MHz
	);
	set_systick_config(
		1, 1, SYSTICK_CLK_SRC_AXI_CLK_DIV_1				// SysTick (IRQ) enable at 280MHz
	);
	sys_clock_init();

	/* TIM config */
	config_TIM_kernel_clocks(
			TIM_MUL_2, LPTIM_CLK_SRC_APBx,
			LPTIM_CLK_SRC_APBx, LPTIM_CLK_SRC_APBx
	);
	config_TIM(TIM8, TIM_APB2_kernel_frequency / 10000, 10000);  // 1 Hz
	start_TIM_update_irq(TIM8);
	start_TIM(TIM8);

	/* GPIO config */
	config_GPIO(GPIOC, 1, GPIO_output, GPIO_no_pull, GPIO_push_pull);	// user led C1
	config_GPIO(GPIOC, 13, GPIO_input, GPIO_pull_up, GPIO_open_drain);	// user button C13
	GPIO_write(GPIOC, 1, 1);

	/* EXTI config */
	config_EXTI(13, GPIOC, 0, 1);
	start_EXTI(13);

	/* RNG, HASH, CRYP config */
	config_RNG_kernel_clock(RNG_CLK_SRC_HSI48);
	start_RNG(); config_HASH(); config_CRYP();

	/* UART config */
	config_USART_kernel_clocks(USART_CLK_SRC_APBx, USART_CLK_SRC_APBx, USART_CLK_SRC_APBx);
	config_UART(USART1_TX_A9, USART1_RX_A10, 115200, 1);

	/* I2C config */
	config_I2C_kernel_clocks(I2C_CLK_SRC_APBx, I2C_CLK_SRC_APBx);
	config_I2C(I2C3_SCL_A8, I2C3_SDA_C9, I2C_setting, 0x50);

	/* SPI config */
	config_SPI_kernel_clocks(SPI123_CLK_SRC_PLL2_P, SPI456_CLK_SRC_PLL2_Q, SPI456_CLK_SRC_PLL2_Q);
	config_SPI_master(SPI2_SCK_B13, SPI2_MOSI_B15, SPI_PIN_DISABLE, SPI_DIV_4);  // transmit only
	config_GPIO(GPIOB, 12, GPIO_output, GPIO_pull_up, GPIO_open_drain);  // NSS

	/* QSPI config */

	/* W25Q64!! TODO: program QSPI to work with this chip!!
	 * https://pdf1.alldatasheet.com/datasheet-pdf/view/1243795/WINBOND/W25Q64JVSSIQ.html
	 QuadSPI NCS/CLK/IO0/IO1/IO2/IO3 : PB6/PB2/PD11/PD12/PE2/PD13 (NOR Flash)
	*/
	config_OSPI_kernel_clock(OSPI_CLK_SRC_PLL2_R);
	config_QSPI(
		OCTOSPI1, OSPIM_PORT1_SCK_B2,
		OSPIM_PORT1_IO0_D11, OSPIM_PORT1_IO1_D12,
		OSPIM_PORT1_IO2_E2, OSPIM_PORT1_IO3_D13,
		OSPIM_PORT1_NSS_B6, 0U, OPSI_MEMORY_MICRON, 31
	);

	/* USB config */  // TODO: do low power later (when debugging is fixed)
	config_USB_kernel_clock(USB_CLK_SRC_HSI48);
	config_USB(USB1_OTG_HS, &HID_class, &FS_Desc, 0, 0);  // TODO low power doesnt work!!
	start_USB(USB1_OTG_HS);


	//Watchdog config (32kHz / (4 << prescaler))
	config_watchdog(0, 0xFFFUL);	// 1s
	//start_watchdog();


	uint8_t hash_data[] = {0x00, 0x00, 0x00, 'a'};	// TODO: endian-ness swap??
	uint8_t hash_key[] = {0x00, 0x00, 0x00, 'b'};	// TODO: endian-ness swap??
	volatile uint32_t digest[8];
	process_HMAC(hash_data, 1, hash_key, 1, HASH_ALGO_SHA2_256);
	for (uint8_t i = 0; i < 8; i++) { digest[i] = ((__IO uint32_t*)HASH_digest)[i]; }


	uint8_t delay = 20;

	uint32_t rn = RNG_generate();
	(void)rn;

	uint8_t data[128] = "passwords and stuff here!!";
	uint8_t key[32] = "PASSWORD for KEYBOARD!!";


	uint8_t buff[32];
	for (uint8_t i = 0; i < 32; i++) { buff[i] = i; }

	// main loop
	for(;;) {
		//reset_watchdog();
		//if (!GO) { continue; }

		/* SPI */ /*
		SPI_master_transmit(SPI2, GPIOB, 12, buff, 32U, 100);
		GPIO_toggle(GPIOC, 1);
		*/

		/* QSPI */
		// TODO https://github.com/Crazy-Geeks/STM32-W25Q-QSPI
		/*OSPI_transmit(
			OCTOSPI1,
			0x5A5A5A5A, OSPI_SIZE_32B, OSPI_MODE_QUAD, 0U,	// instruction
			0x6C6C6C6C, OSPI_SIZE_32B, OSPI_MODE_QUAD, 0U,	// address
			0x7D7D7D7D, OSPI_SIZE_32B, OSPI_MODE_QUAD, 0U,	// alt bytes
			buff, 32U, OSPI_MODE_QUAD, 0U,					// data
			100
		);*/


		//GPIO_toggle(GPIOC, 1);

		/* Keyboard */
		HID_buffer[2] = 0x4;
		send_HID_report(&USB_handle, HID_buffer, 8);
		delay_ms(delay);
		HID_buffer[2] = 0;
		send_HID_report(&USB_handle, HID_buffer, 8);


		/* ROM */ /*
		write_encrypted_page(I2C3, 0x50, 0x0, key, CRYP_KEY_256, data);
		read_encrypted_page(I2C3, 0x50, 0x0, key, CRYP_KEY_256, rom_data);
		*/

		GO = 0;
	}
}

// p.g. 346 RCC diagram
// TODO: RTC!!!
// TODO: PWR_CR1_SVOS (stop mode VOS)
// TODO: VBAT!!!
// TODO: make HASH, CRYP, RNG interrupt based for faster / parallel processing

// TODO: look at WKUPCR in PWR in USB wakup it