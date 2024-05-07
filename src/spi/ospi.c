//
// Created by marijn on 4/19/24.
//

#include "spi/ospi.h"


/*!<
 * static types
 * */
typedef struct __PACKED {
	uint32_t	CLKEN	: 1U;
	uint32_t	CLKSRC	: 1U;
	uint32_t	RES_0	: 2U;
	uint32_t	DQSEN	: 1U;
	uint32_t	DQSSRC	: 1U;
	uint32_t	RES_1	: 2U;
	uint32_t	NCSEN	: 1U;
	uint32_t	NCSSRC	: 1U;
	uint32_t	RES_2	: 6U;
	uint32_t	IOLEN	: 1U;
	uint32_t	IOLSRC	: 2U;
	uint32_t	RES_3	: 5U;
	uint32_t	IOHEN	: 1U;
	uint32_t	IOHSRC	: 2U;
	uint32_t	RES_4	: 5U;
} PCR_t;


/*!<
 * variables
 * */
uint32_t OSPI12_kernel_frequency =	0;


/*!<
 * init
 * */
void config_OSPI_kernel_clock(OSPI_CLK_SRC_t ospi_src) {
	RCC->CDCCIPR &= ~RCC_CDCCIPR_OCTOSPISEL;
	RCC->CDCCIPR |= ospi_src << RCC_CDCCIPR_OCTOSPISEL_Pos;
	switch (ospi_src) {
	case OSPI_CLK_SRC_AHB:			OSPI12_kernel_frequency = AHB_clock_frequency;		return;	// AHB3
	case OSPI_CLK_SRC_PLL1_Q:		OSPI12_kernel_frequency = PLL1_Q_clock_frequency;	return;
	case OSPI_CLK_SRC_PLL2_R:		OSPI12_kernel_frequency = PLL2_R_clock_frequency;	return;
	case OSPI_CLK_SRC_PER:			OSPI12_kernel_frequency = PER_clock_frequency;		return;
	}
}

void fconfig_OSPIM(
	OSPIM_GPIO_t sck, OSPIM_GPIO_t io0, OSPIM_GPIO_t io1, OSPIM_GPIO_t io2, OSPIM_GPIO_t io3,
	OSPIM_GPIO_t io4, OSPIM_GPIO_t io5, OSPIM_GPIO_t io6, OSPIM_GPIO_t io7, OSPIM_GPIO_t nss,
	uint8_t ospi_num, OSPIM_MUX_t mux, uint8_t mux_delay
) {
	dev_pin_t		sck_pin = *((dev_pin_t*)&sck),
					io0_pin = *((dev_pin_t*)&io0),
					io1_pin = *((dev_pin_t*)&io1),
					io2_pin = *((dev_pin_t*)&io2),
					io3_pin = *((dev_pin_t*)&io3),
					io4_pin = *((dev_pin_t*)&io4),
					io5_pin = *((dev_pin_t*)&io5),
					io6_pin = *((dev_pin_t*)&io6),
					io7_pin = *((dev_pin_t*)&io7),
					nss_pin = *((dev_pin_t*)&nss);
	GPIO_TypeDef	*sck_port = int_to_GPIO(sck_pin.port),
					*io0_port = int_to_GPIO(io0_pin.port),
					*io1_port = int_to_GPIO(io1_pin.port),
					*io2_port = int_to_GPIO(io2_pin.port),
					*io3_port = int_to_GPIO(io3_pin.port),
					*io4_port = int_to_GPIO(io4_pin.port),
					*io5_port = int_to_GPIO(io5_pin.port),
					*io6_port = int_to_GPIO(io6_pin.port),
					*io7_port = int_to_GPIO(io7_pin.port),
					*nss_port = int_to_GPIO(nss_pin.port);
	PCR_t			*port = (PCR_t*)OCTOSPIM->PCR;
	enable_dev(OCTOSPIM);
	fconfig_GPIO(sck_port, sck_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, sck_pin.alt);
	fconfig_GPIO(io0_port, io0_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, io0_pin.alt);
	if (nss) { fconfig_GPIO(nss_port, nss_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, nss_pin.alt); }  // TODO: always hardware nss??
	if (io1) { fconfig_GPIO(io1_port, io1_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, io1_pin.alt); }
	if (io2) { fconfig_GPIO(io2_port, io2_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, io2_pin.alt); }
	if (io3) { fconfig_GPIO(io3_port, io3_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, io3_pin.alt); }
	if (io4) { fconfig_GPIO(io4_port, io4_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, io4_pin.alt); }
	if (io5) { fconfig_GPIO(io5_port, io5_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, io5_pin.alt); }
	if (io6) { fconfig_GPIO(io6_port, io6_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, io6_pin.alt); }
	if (io7) { fconfig_GPIO(io7_port, io7_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, io7_pin.alt); }

	// disable all pins TODO: needed??
	//if (port_1->CLKEN && port_1->CLKSRC == ospi_num)			{ port_1->CLKEN = 0b0U; }
	//if (port_2->CLKEN && port_2->CLKSRC == ospi_num)			{ port_2->CLKEN = 0b0U; }
	//if (port_1->DQSEN && port_1->DQSSRC == ospi_num)			{ port_1->DQSEN = 0b0U; }
	//if (port_2->DQSEN && port_2->DQSSRC == ospi_num)			{ port_2->DQSEN = 0b0U; }
	//if (port_1->NCSEN && port_1->NCSSRC == ospi_num)			{ port_1->NCSEN = 0b0U; }
	//if (port_2->NCSEN && port_2->NCSSRC == ospi_num)			{ port_2->NCSEN = 0b0U; }
	//if (port_1->IOLEN && (port_1->IOLSRC >> 1U) == ospi_num)	{ port_1->IOLEN = 0b0U; }
	//if (port_2->IOLEN && (port_2->IOLSRC >> 1U) == ospi_num)	{ port_2->IOLEN = 0b0U; }
	//if (port_1->IOHEN && (port_1->IOHSRC >> 1U) == ospi_num)	{ port_1->IOHEN = 0b0U; }
	//if (port_2->IOHEN && (port_2->IOHSRC >> 1U) == ospi_num)	{ port_2->IOHEN = 0b0U; }
	OCTOSPIM->CR = (
		(mux_delay << OCTOSPIM_CR_REQ2ACK_TIME_Pos)	|
		(mux << OCTOSPIM_CR_MUXEN_Pos)
	); if (mux) { ospi_num = 0U; }

	port[sck_pin.id.sub & 0b1U].CLKSRC =		ospi_num;
	port[sck_pin.id.sub & 0b1U].NCSEN =			0b1U;
	if (nss) {
		port[nss_pin.id.sub & 0b1U].NCSSRC =	ospi_num;
		port[nss_pin.id.sub & 0b1U].NCSEN =		0b1U;
	}
	if (io0_pin.id.sub & 0x4U) {
		port[io0_pin.id.sub & 0b1U].IOHSRC =	(ospi_num << 1U);
		port[io0_pin.id.sub & 0b1U].IOHEN =		0b1U;
	} else {
		port[io0_pin.id.sub & 0b1U].IOLSRC =	(ospi_num << 1U);
		port[io0_pin.id.sub & 0b1U].IOLEN =		0b1U;
	}
	if(io4) {
	if (io4_pin.id.sub & 0x4U) {
		port[io4_pin.id.sub & 0b1U].IOHSRC =	(ospi_num << 1U) | 0b1U;
		port[io0_pin.id.sub & 0b1U].IOHEN =		0b1U;
	} else {
		port[io4_pin.id.sub & 0b1U].IOLSRC =	(ospi_num << 1U) | 0b1U;
		port[io0_pin.id.sub & 0b1U].IOLEN =		0b1U;
	}
	}
}

void fconfig_OSPI(  // TODO: mode??
	OCTOSPI_TypeDef* ospi,
	OSPI_MODE_t mode, uint8_t prescaler, OSPI_MEMORY_t mem, uint8_t addr_size,
	uint8_t nss_high_time, uint8_t nss_boundary, uint8_t DLYB_bypass, OSPI_CLOCK_MODE_t clk_mode,
	OSPI_WRAP_SIZE_t wrap, uint8_t max_transfer, uint32_t refresh, uint8_t fifo_threshold,
	OSPI_SHIFT_t shift, OSPI_HOLD_t delay, uint8_t dummy_cycles
) {
	enable_dev(ospi);

	// TODO: |= ??
	ospi->DCR1 = (
		(mem << OCTOSPI_DCR1_MTYP_Pos)						|
		((addr_size & 0x1FU) << OCTOSPI_DCR1_DEVSIZE_Pos)	|
		((nss_high_time & 0x7U) << OCTOSPI_DCR1_CSHT_Pos)	|
		(DLYB_bypass << OCTOSPI_DCR1_DLYBYP_Pos)			|
		(clk_mode << OCTOSPI_DCR1_CKMODE_Pos)
	);
	ospi->DCR2 = (wrap << OCTOSPI_DCR2_WRAPSIZE_Pos);	// TODO: |= ??
	ospi->DCR3 = (
		((nss_boundary & 0xFU) << OCTOSPI_DCR3_CSBOUND_Pos)	|
		(max_transfer << OCTOSPI_DCR3_MAXTRAN_Pos)
	);
	ospi->DCR4 = refresh;
	ospi->CR = (fifo_threshold << OCTOSPI_CR_FTHRES_Pos);	// TODO: |= ??
	while (ospi->SR & OCTOSPI_SR_BUSY);
	ospi->DCR2 |= (prescaler << OCTOSPI_DCR2_PRESCALER_Pos);
	ospi->TCR = (
		(shift << OCTOSPI_TCR_SSHIFT_Pos)					|
		(delay << OCTOSPI_TCR_DHQC_Pos)						|
		(dummy_cycles << OCTOSPI_TCR_DCYC_Pos)
	);

	ospi->CR |= OCTOSPI_CR_EN;
}

void config_DSPI(
	OCTOSPI_TypeDef* ospi,
	OSPIM_GPIO_t sck, OSPIM_GPIO_t io0, OSPIM_GPIO_t io1, OSPIM_GPIO_t nss, uint8_t prescaler,
	OSPI_MEMORY_t mem, uint8_t addr_size
) {
	fconfig_OSPIM(
		sck, io0, io1, OSPIM_PIN_DISABLE, OSPIM_PIN_DISABLE, OSPIM_PIN_DISABLE,
		OSPIM_PIN_DISABLE, OSPIM_PIN_DISABLE, OSPIM_PIN_DISABLE, nss,
		ospi == OCTOSPI2, OSPIM_MUX_DISABLE, 0U
	);
	fconfig_OSPI(
		ospi, OSPI_MODE_DUAL, prescaler, mem, addr_size, OSPI_DEFAULT_NSS_HIGH_TIME,
		OSPI_DEFAULT_NSS_BOUNDARY, OSPI_DEFAULT_DLYB_BYPASS,
		OSPI_CLOCK_MODE_0, OSPI_WRAP_SIZE_NONE, 0U /* max_transfer */,
		0U /* refresh */, 0U /* fifo_threshold */, OSPI_SHIFT_NONE,
		OSPI_HOLD_NONE, 0U /* dummy_cycles */
	);
}
void config_QSPI(
	OCTOSPI_TypeDef* ospi,
	OSPIM_GPIO_t sck, OSPIM_GPIO_t io0, OSPIM_GPIO_t io1, OSPIM_GPIO_t io2, OSPIM_GPIO_t io3,
	OSPIM_GPIO_t nss, uint8_t prescaler, OSPI_MEMORY_t mem, uint8_t addr_size
) {
	fconfig_OSPIM(
		sck, io0, io1, io2, io3, OSPIM_PIN_DISABLE,
		OSPIM_PIN_DISABLE, OSPIM_PIN_DISABLE, OSPIM_PIN_DISABLE, nss,
		ospi == OCTOSPI2, OSPIM_MUX_DISABLE, 0U
	);
	fconfig_OSPI(
		ospi, OSPI_MODE_QUAD, prescaler, mem, addr_size, OSPI_DEFAULT_NSS_HIGH_TIME,
		OSPI_DEFAULT_NSS_BOUNDARY, OSPI_DEFAULT_DLYB_BYPASS,
		OSPI_CLOCK_MODE_0, OSPI_WRAP_SIZE_NONE, 0U /* max_transfer */,
		0U /* refresh */, 0U /* fifo_threshold */, OSPI_SHIFT_NONE,
		OSPI_HOLD_NONE, 0U /* dummy_cycles */
	);
}
void config_OSPI(
	OCTOSPI_TypeDef* ospi,
	OSPIM_GPIO_t sck, OSPIM_GPIO_t io0, OSPIM_GPIO_t io1, OSPIM_GPIO_t io2, OSPIM_GPIO_t io3,
	OSPIM_GPIO_t io4, OSPIM_GPIO_t io5, OSPIM_GPIO_t io6, OSPIM_GPIO_t io7, OSPIM_GPIO_t nss,
	uint8_t prescaler, OSPI_MEMORY_t mem, uint8_t addr_size
) {
	fconfig_OSPIM(
		sck, io0, io1, io2, io3, io4, io5, io6, io7, nss,
		ospi == OCTOSPI2, OSPIM_MUX_DISABLE, 0U
	);
	fconfig_OSPI(
		ospi, OSPI_MODE_OCTO, prescaler, mem, addr_size, OSPI_DEFAULT_NSS_HIGH_TIME,
		OSPI_DEFAULT_NSS_BOUNDARY, OSPI_DEFAULT_DLYB_BYPASS,
		OSPI_CLOCK_MODE_0, OSPI_WRAP_SIZE_NONE, 0U /* max_transfer */,
		0U /* refresh */, 0U /* fifo_threshold */, OSPI_SHIFT_NONE,
		OSPI_HOLD_NONE, 0U /* dummy_cycles */
	);
}


/*!<
 * usage
 * */
uint32_t OSPI_transmit(OCTOSPI_TypeDef* ospi, const uint8_t* buffer, uint32_t size, uint32_t timeout) {


	return 0UL;
}
