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


/*!<
 * static
 * */
static inline void SPI_end_transfer(SPI_TypeDef* spi) {
	spi->IFCR |= (
		SPI_IFCR_EOTC	|
		SPI_IFCR_TXTFC
	);
	spi->CR1 &= ~SPI_CR1_SPE;

	if (spi->SR & SPI_SR_MODF) {
		spi->IFCR | SPI_IFCR_MODFC;
	}
	// TODO: IT, DMA, errors
}

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


void fconfig_SPI_master(
	SPI_GPIO_t sck, SPI_GPIO_t mosi, SPI_GPIO_t miso, SPI_MODE_t mode,
	SPI_DIV_t div, SPI_CLK_POL_t clk_pol, SPI_CLK_PHASE_t clk_phase, uint8_t data_idle,
	uint8_t data_size, uint8_t fifo_threshold, SPI_ENDIANNESS_t endianness, SPI_PROTOCOL_t protocol
) {
	dev_pin_t		sck_pin = *((dev_pin_t*)&sck),
					mosi_pin = *((dev_pin_t*)&mosi),
					miso_pin = *((dev_pin_t*)&miso);
	SPI_TypeDef*	spi = id_to_dev(sck_pin.id);
	GPIO_TypeDef	*sck_port = int_to_GPIO(sck_pin.port),
					*mosi_port = int_to_GPIO(mosi_pin.port),
					*miso_port = int_to_GPIO(miso_pin.port);

	enable_dev(spi);
	fconfig_GPIO(sck_port, sck_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, sck_pin.alt);
	if (mosi) { fconfig_GPIO(mosi_port, mosi_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, mosi_pin.alt); }
	if (miso) { fconfig_GPIO(miso_port, miso_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, miso_pin.alt); }

	spi->CR1 = 0x00000000UL;
	// TODO: MASRX??? (clock suspend on RXFIFO full)

	spi->CFG1 = (
		((div & 0x7U) << SPI_CFG1_MBR_Pos)				|
		((fifo_threshold & 0xFU) << SPI_CFG1_FTHLV_Pos)	|
		((data_size & 0x1FU) << SPI_CFG1_DSIZE_Pos)
	);  // TODO: CRC??

	spi->CFG2 = (
		(clk_pol << SPI_CFG2_CPOL_Pos)					|
		(clk_phase << SPI_CFG2_CPHA_Pos)				|
		(endianness << SPI_CFG2_LSBFRST_Pos)			|
		(protocol << SPI_CFG2_SP_Pos)					|
		(mode << SPI_CFG2_COMM_Pos)						|
		(data_idle << SPI_CFG2_MIDI_Pos)				|
		SPI_CFG2_SSM
	);
	if (mode == SPI_MODE_HALFDUPLEX) {	// half duplex
		spi->CFG2 |= SPI_CFG2_IOSWP;
	}
	spi->CR1 |= SPI_CR1_SSI;
	spi->CFG2 |= SPI_CFG2_MASTER;
}

void config_SPI_master(SPI_GPIO_t sck, SPI_GPIO_t mosi, SPI_GPIO_t miso, SPI_DIV_t div) {
	SPI_MODE_t mode = SPI_MODE_FULLDUPLEX;	// full duplex (default)
	if (miso == SPI_PIN_DISABLE) { mode = SPI_MODE_TRANSMIT; }  // simplex transmit
	if (mosi == SPI_PIN_DISABLE) { mode = SPI_MODE_RECEIVE; }  // simplex receive
	fconfig_SPI_master(
		sck, mosi, miso, mode, div, SPI_CLK_POL_LOW, SPI_CLK_PHASE_EDGE1,
		0U /* 0-cycles data idle */, 7U /* 8-bits */, 0U /* 1-data */,
		SPI_ENDIANNESS_MSB, SPI_PROTOCOL_MOTOROLA
	);
}


/*!<
 * usage
 * */
uint32_t SPI_master_transmit(SPI_TypeDef* spi, GPIO_TypeDef* ss_port, uint8_t ss_pin, const uint8_t* buffer, uint32_t size, uint32_t timeout) {		// -> n unprocessed
	uint64_t start = tick;
	spi->CR2 &= ~SPI_CR2_TSIZE;
	spi->CR2 |= (size << SPI_CR2_TSIZE_Pos);

	GPIO_write(GPIOB, 12, 0);
	spi->CR1 |= SPI_CR1_SPE;
	spi->CR1 |= SPI_CR1_CSTART;
	while (size--) {
		while (!(spi->SR & SPI_SR_TXP)) { if ( tick - start > timeout) { SPI_end_transfer(spi); return size; } }
		*((__IO uint8_t*)&spi->TXDR) = *buffer++;
	}
	while (!(spi->SR & SPI_SR_EOT)) { if ( tick - start > timeout) { SPI_end_transfer(spi); return -1; } }
	GPIO_write(GPIOB, 12, 1);

	SPI_end_transfer(spi);
	return 0U;
}

