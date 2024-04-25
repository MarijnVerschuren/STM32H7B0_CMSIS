//
// Created by marijn on 7/19/23.
//

#include "spi/spi.h"


/*!<
 * variables
 * */
uint32_t SPI123_kernel_frequency =	0;
uint32_t SPI45_kernel_frequency =	0;
uint32_t SPI6_kernel_frequency =	0;
uint32_t OSPI12_kernel_frequency =	0;


/*!<
 * init
 * */
void config_SPI_kernel_clocks(
	SPI123_CLK_SRC_t spi123_src, SPI456_CLK_SRC_t spi45_src,
	SPI456_CLK_SRC_t spi6_src
) {
	RCC->CDCCIP1R &= ~(
		RCC_CDCCIP1R_SPI123SEL |
		RCC_CDCCIP1R_SPI45SEL
	);
	RCC->CDCCIP1R |= (
		(spi123_src << RCC_CDCCIP1R_SPI123SEL_Pos) |
		(spi45_src<< RCC_CDCCIP1R_SPI45SEL_Pos)
	);
	RCC->SRDCCIPR &= RCC_SRDCCIPR_SPI6SEL;
	RCC->SRDCCIPR |= spi6_src << RCC_SRDCCIPR_SPI6SEL_Pos;
	switch (spi123_src) {
		case SPI123_CLK_SRC_PLL1_Q:		SPI123_kernel_frequency = PLL1_Q_clock_frequency; break;
		case SPI123_CLK_SRC_PLL2_P:		SPI123_kernel_frequency = PLL2_P_clock_frequency; break;
		case SPI123_CLK_SRC_PLL3_P:		SPI123_kernel_frequency = PLL3_P_clock_frequency; break;
		case SPI123_CLK_SRC_I2S_CKIN:	SPI123_kernel_frequency = 0; break;  // external
		case SPI123_CLK_SRC_PER:		SPI123_kernel_frequency = PER_clock_frequency; break;
	}
	switch (spi45_src) {
		case SPI456_CLK_SRC_APBx:		SPI45_kernel_frequency = APB2_clock_frequency; break;	// APB2
		case SPI456_CLK_SRC_PLL2_Q:		SPI45_kernel_frequency = PLL2_Q_clock_frequency; break;
		case SPI456_CLK_SRC_PLL3_Q:		SPI45_kernel_frequency = PLL3_Q_clock_frequency; break;
		case SPI456_CLK_SRC_HSI:		SPI45_kernel_frequency = HSI_clock_frequency; break;
		case SPI456_CLK_SRC_CSI:		SPI45_kernel_frequency = CSI_clock_frequency; break;
		case SPI456_CLK_SRC_HSE:		SPI45_kernel_frequency = HSE_clock_frequency; break;
	}
	switch (spi6_src) {
		case SPI456_CLK_SRC_APBx:		SPI6_kernel_frequency = APB4_clock_frequency; break;	// APB4
		case SPI456_CLK_SRC_PLL2_Q:		SPI6_kernel_frequency = PLL2_Q_clock_frequency; break;
		case SPI456_CLK_SRC_PLL3_Q:		SPI6_kernel_frequency = PLL3_Q_clock_frequency; break;
		case SPI456_CLK_SRC_HSI:		SPI6_kernel_frequency = HSI_clock_frequency; break;
		case SPI456_CLK_SRC_CSI:		SPI6_kernel_frequency = CSI_clock_frequency; break;
		case SPI456_CLK_SRC_HSE:		SPI6_kernel_frequency = HSE_clock_frequency; break;
	}
}



void fconfig_SPI(
	SPI_GPIO_t sck, SPI_GPIO_t mosi, SPI_GPIO_t miso, SPI_GPIO_t ss,
	SPI_MODE_t mode, SPI_DIV_t div, SPI_SS_POL_t ss_pol, SPI_CLK_POL_t clk_pol,
	SPI_CLK_PHASE_t clk_phase
) {
	dev_pin_t		sck_pin = *((dev_pin_t*)&sck),
					mosi_pin = *((dev_pin_t*)&mosi),
					miso_pin = *((dev_pin_t*)&miso),
					ss_pin = *((dev_pin_t*)&ss);
	SPI_TypeDef*	spi = id_to_dev(sck_pin.id);
	GPIO_TypeDef	*sck_port = int_to_GPIO(sck_pin.port),
					*mosi_port = int_to_GPIO(mosi_pin.port),
					*miso_port = int_to_GPIO(miso_pin.port),
					*ss_port = int_to_GPIO(ss_pin.port);

	enable_dev(spi);
	fconfig_GPIO(sck_port, sck_pin.num, GPIO_alt_func, GPIO_pull_up, GPIO_open_drain, GPIO_very_high_speed, sck_pin.alt);
	if (mosi != SPI_PIN_DISABLE)	{ fconfig_GPIO(mosi_port, mosi_pin.num, GPIO_alt_func, GPIO_pull_up, GPIO_open_drain, GPIO_very_high_speed, mosi_pin.alt); }
	if (miso != SPI_PIN_DISABLE)	{ fconfig_GPIO(miso_port, miso_pin.num, GPIO_alt_func, GPIO_pull_up, GPIO_open_drain, GPIO_very_high_speed, miso_pin.alt); }
	if (ss != SPI_PIN_DISABLE)		{ fconfig_GPIO(ss_port, ss_pin.num, GPIO_alt_func, GPIO_pull_up, GPIO_open_drain, GPIO_very_high_speed, ss_pin.alt); }

	// TODO: frames and packets?
	spi->CFG1 = (
		((div & 0x7U) << SPI_CFG1_MBR_Pos)			//|
	   // TODO: FTHLV??
	   // TODO: DSIZE??
	);

	spi->CFG2 = (
		(SPI_CFG2_SSOE * (ss != SPI_PIN_DISABLE))	|
		(ss_pol << SPI_CFG2_SSIOP_Pos)				|
		(clk_pol << SPI_CFG2_CPOL_Pos)				|
		(clk_phase << SPI_CFG2_CPHA_Pos)			|
		// TODO: LSBFRST (MSB or LSB)
		SPI_CFG2_MASTER								|
		// TODO: SP (motorola or TI)
		(mode << SPI_CFG2_COMM_Pos)
		// TODO: MIDI??
		// TODO: MSSI??
	);

	// TODO
	// 2. SPI_CR2
	// 3. SPI_CR1
}


void config_SPI(SPI_GPIO_t sck, SPI_GPIO_t mosi, SPI_GPIO_t miso, SPI_GPIO_t ss, SPI_DIV_t div) {
	SPI_MODE_t mode = SPI_MODE_FULLDUPLEX;	// full duplex (default)
	if (miso == SPI_PIN_DISABLE) { mode = SPI_MODE_TRANSMIT; }  // simplex transmit
	if (mosi == SPI_PIN_DISABLE) { mode = SPI_MODE_RECEIVE; }  // simplex receive
	fconfig_SPI(sck, mosi, miso, ss, mode, div, SPI_SS_POL_LOW, SPI_CLK_POL_LOW);
}