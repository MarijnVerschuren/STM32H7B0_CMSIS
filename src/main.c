#include "gpio.h"
#include "exti.h"
#include "tim.h"
#include "pwm.h"
#include "encoder.h"
#include "usart.h"
#include "i2c.h"
#include "crc.h"
#include "rng.h"
#include "watchdog.h"
#include "mco.h"
#include "usb/usb.h"
#include "usb/hid.h"
#include "hash.h"
#include "cryp.h"


// I2C_setting_t I2C_setting;  // TODO

uint8_t HID_buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
volatile uint8_t GO = 0;


extern void TIM8_UP_TIM13_IRQHandler(void) {
	TIM8->SR &= ~TIM_SR_UIF;  // clear interrupt flag
	GPIO_toggle(GPIOC, 1);
}

extern void EXTI15_10_IRQHandler(void) {	// button K2
	EXTI->PR1 |= EXTI_PR1_PR13;  // clear interrupt flag
	//stop_TIM(TIM8);
	GO = 1;
}


int main(void) {
	/* clock config */
	set_PLL_config(
		0, 1, 1, 1, 0, 0,								// enable PLL1 (P, Q)
		PLL_IN_4MHz_8MHz, PLL_VCO_WIDE,					// 5MHz in, 192MHz < VCO < 960MHz
		5/*M*/, 2/*P*/, 4/*Q*/, 2/*R*/, 112/*N*/, 0		// M = 5, P = 2, Q = 2, R = 2, N = 112, N_frac = 0
	);  // 25MHz / 5 * 112 / (2, 2, 2)	=>	(280Mhz, 280Mhz, *280Mhz)
	set_PLL_config(
		1, 0, 0, 0, 0, 0,								// disable PLL2
		PLL_IN_4MHz_8MHz, PLL_VCO_WIDE,					// 5MHz in, 192MHz < VCO < 960MHz
		5/*M*/, 2/*P*/, 4/*Q*/, 2/*R*/, 112/*N*/, 0		// M = 5, P = 2, Q = 2, R = 2, N = 112, N_frac = 0
	);  // 25MHz / 5 * 112 / (2, 2, 2)	=>	(280Mhz, 280Mhz, *280Mhz)
	set_PLL_config(
		2, 0, 0, 0, 0, 0,								// disable PLL3
		PLL_IN_4MHz_8MHz, PLL_VCO_WIDE,					// 5MHz in, 192MHz < VCO < 960MHz
		5/*M*/, 2/*P*/, 4/*Q*/, 2/*R*/, 112/*N*/, 0		// M = 5, P = 2, Q = 2, R = 2, N = 112, N_frac = 0
	);  // 25MHz / 5 * 112 / (2, 2, 2)	=>	(280Mhz, 280Mhz, *280Mhz)
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
	config_TIM(TIM8, TIM_APB2_kernel_frequency / 10000, 10000);  // 10 Hz
	start_TIM_update_irq(TIM8);
	start_TIM(TIM8);

	/* GPIO config */
	config_GPIO(GPIOC, 1, GPIO_output, GPIO_no_pull, GPIO_push_pull);	// user led D2
	config_GPIO(GPIOC, 13, GPIO_input, GPIO_pull_up, GPIO_open_drain);	// user button K2
	GPIO_write(GPIOC, 1, 1);

	/* EXTI config */
	config_EXTI(13, GPIOC, 0, 1);	start_EXTI(13);

	/* MCO config */
	config_MCO(MCO2_C9, MCO2_SRC_PLL1_P, 1);  // 280 MHz

	/* PWM config */
	config_PWM(TIM1_CH1_A8, TIM_APB2_kernel_frequency / 1000000, 20000);  // 50Hz


	/* CRC config */
	config_CRC();

	/* HASH config */
	config_HASH();  // TODO: endian-ness swap??
	uint8_t hash_data[] = {0x00, 0x00, 0x00, 'a'};
	uint8_t hash_key[] = {0x00, 0x00, 0x00, 'b'};
	volatile uint32_t digest[8];
	process_HMAC(hash_data, 1, hash_key, 1, HASH_ALGO_SHA2_256);
	for (uint8_t i = 0; i < 8; i++) { digest[i] = ((__IO uint32_t*)HASH_digest)[i]; }

	/* CRYP config */
	config_CRYP(CRYP_ALGO_AES_CBC, CRYP_KEY_128);

	/* RNG config */
	config_RNG_kernel_clock(RNG_CLK_SRC_PLL1_Q);  // 280 MHz

	// TODO: [5]

	/* UART config */
	config_USART_kernel_clocks(USART_CLK_SRC_APBx, USART_CLK_SRC_APBx, USART_CLK_SRC_APBx);
	config_UART(USART1_TX_A9, USART1_RX_A10, 115200, 1);

	/* I2C config */
	config_I2C_kernel_clocks(I2C_CLK_SRC_APBx, I2C_CLK_SRC_APBx);
	I2C_setting_t I2C_setting = {  // 100 KHz
			APB1_clock_frequency / 4000000,
			0x13UL, 0x0FU, 2, 4
	};
	config_I2C(I2C3_SCL_A8, I2C3_SDA_C9, I2C_setting, 0x50);
	//A8 SCL, C9 SDA

	/* USB config */  // TODO: do low power later (when debugging is fixed)
	config_USB_kernel_clock(USB_CLK_SRC_HSI48);
	config_USB(USB1_OTG_HS, &HID_class, &FS_Desc, 0, 0);  // TODO low power doesnt work!!
	start_USB(USB1_OTG_HS);


	//Watchdog config (32kHz / (4 << prescaler))
	//config_watchdog(0, 0xFFFUL);	// 1s
	//start_watchdog();


	uint8_t delay = 20;

	uint8_t cryp_data[128] = "When I was, a young boy, my father, took me into the city to see a marching band. He said: Son,if you read this. Vouw bak -Cdirk";
	uint8_t cryp_key[16] = "ENC_KEY!!";	// TODO: UUID into key
	uint8_t cryp_IV[16] = {
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};

	uint8_t rom_enc[128];
	uint8_t rom_data[128];
	for (uint8_t i = 0; i < 128; i++) { rom_data[i] = 0; }
	// main loop
	for(;;) {
		if (!GO) { continue; }

		/*	Keyboard */
		/*
		HID_buffer[2] = 0x4;
		send_HID_report(&USB_handle, HID_buffer, 8);
		delay_ms(delay);
		HID_buffer[2] = 0;
		send_HID_report(&USB_handle, HID_buffer, 8);
		*/

		/*  ROM */
		/*
		CRYP_encrypt_setup(cryp_IV, cryp_key);
		for (uint8_t i = 0; i < 8; i++) {
			CRYP_process_block(&cryp_data[i * 16], 16, &rom_enc[i * 16]);
		}
		CRYP->CR &= ~CRYP_CR_CRYPEN;
		I2C_master_write_reg(I2C3, 0x50, 0x1234, I2C_REG_16, rom_enc, 128, 100);
		*/

		/*
		I2C_master_read_reg(I2C3, 0x50, 0x1234, I2C_REG_16, rom_enc, 128, 100);
		CRYP_decrypt_setup(cryp_IV, cryp_key);
		for (uint8_t i = 0; i < 8; i++) {
			CRYP_process_block(&rom_enc[i * 16], 16, &rom_data[i * 16]);
		}
		CRYP->CR &= ~CRYP_CR_CRYPEN;
		*/

		GO = 0;
	}


	/*!< ROM code */
}

/*
 *
LED (blue) = PC1
BOOT = SW1 = BOOT0
RESET = SW2 = NRST
SW3 = PC13
TX device = USART1 PA9
RX device = USART1 PA10
USB D- = PA11
USB D+ = PA12
 */
// p.g. 346 RCC diagram
// TODO: RTC!!!
// TODO: PWR_CR1_SVOS (stop mode VOS)
// TODO: VBAT!!!

// TODO: look at WKUPCR in PWR in USB wakup it