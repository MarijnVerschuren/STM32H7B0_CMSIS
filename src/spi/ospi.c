//
// Created by marijn on 4/19/24.
//

#include "spi/ospi.h"


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
	case OSPI_CLK_SRC_AHB:			OSPI12_kernel_frequency = AHB_clock_frequency; return;	// AHB3
	case OSPI_CLK_SRC_PLL1_Q:		OSPI12_kernel_frequency = PLL1_Q_clock_frequency; return;
	case OSPI_CLK_SRC_PLL2_R:		OSPI12_kernel_frequency = PLL2_R_clock_frequency; return;
	case OSPI_CLK_SRC_PER:			OSPI12_kernel_frequency = PER_clock_frequency; return;
	}
}

void fconfig_OSPI(
	OSPI_GPIO_t sck, OSPI_GPIO_t io0, OSPI_GPIO_t io1, OSPI_GPIO_t io2, OSPI_GPIO_t io3,
	OSPI_GPIO_t io4, OSPI_GPIO_t io5, OSPI_GPIO_t io6, OSPI_GPIO_t io7, OSPI_MODE_t mode
) {
	dev_pin_t			sck_pin = *((dev_pin_t *)&sck),
			  			io0_pin = *((dev_pin_t *)&io0),
						io1_pin = *((dev_pin_t *)&io1),
			  			io2_pin = *((dev_pin_t *)&io2),
						io3_pin = *((dev_pin_t *)&io3),
			  			io4_pin = *((dev_pin_t *)&io4),
						io5_pin = *((dev_pin_t *)&io5),
			  			io6_pin = *((dev_pin_t *)&io6),
						io7_pin = *((dev_pin_t *)&io7);
	OCTOSPI_TypeDef*	ospi = id_to_dev(sck_pin.id);
	GPIO_TypeDef		*sck_port = int_to_GPIO(sck_pin.port),
						*io0_port = int_to_GPIO(io0_pin.port),
						*io1_port = int_to_GPIO(io1_pin.port),
						*io2_port = int_to_GPIO(io2_pin.port),
						*io3_port = int_to_GPIO(io3_pin.port),
						*io4_port = int_to_GPIO(io4_pin.port),
						*io5_port = int_to_GPIO(io5_pin.port),
						*io6_port = int_to_GPIO(io6_pin.port),
						*io7_port = int_to_GPIO(io7_pin.port);

	// TODO: AHB3 registers have a messed up offset FIX!!!
	OCTOSPI_TypeDef*	rospi = OCTOSPI1;
	OCTOSPIM_TypeDef*	rspim = OCTOSPIM;
	dev_id_t			ospim_id = dev_to_id(OCTOSPIM);
	dev_id_t			ospi_id = dev_to_id(OCTOSPI1);

	//enable_dev(ospi);	enable_dev(OCTOSPIM);
	fconfig_GPIO(sck_port, sck_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, sck_pin.alt);
	fconfig_GPIO(io0_port, io0_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, io0_pin.alt);
	if (io1) { fconfig_GPIO(io1_port, io1_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, io1_pin.alt); }
	if (io2) { fconfig_GPIO(io2_port, io2_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, io2_pin.alt); }
	if (io3) { fconfig_GPIO(io3_port, io3_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, io3_pin.alt); }
	if (io4) { fconfig_GPIO(io4_port, io4_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, io4_pin.alt); }
	if (io5) { fconfig_GPIO(io5_port, io5_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, io5_pin.alt); }
	if (io6) { fconfig_GPIO(io6_port, io6_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, io6_pin.alt); }
	if (io7) { fconfig_GPIO(io7_port, io7_pin.num, GPIO_alt_func, GPIO_no_pull, GPIO_push_pull, GPIO_very_high_speed, io7_pin.alt); }

	ospi->DCR1 = (  // TODO
		0
	);
}

void config_DSPI(OSPI_GPIO_t sck, OSPI_GPIO_t io0, OSPI_GPIO_t io1) {
	fconfig_OSPI(
		sck, io0, io1,
		OSPI_PIN_DISABLE, OSPI_PIN_DISABLE, OSPI_PIN_DISABLE,
		OSPI_PIN_DISABLE, OSPI_PIN_DISABLE, OSPI_PIN_DISABLE,
		OSPI_MODE_DUAL
	);
}
void config_QSPI(OSPI_GPIO_t sck, OSPI_GPIO_t io0, OSPI_GPIO_t io1, OSPI_GPIO_t io2, OSPI_GPIO_t io3) {
	fconfig_OSPI(
		sck, io0, io1, io2, io3,
		OSPI_PIN_DISABLE, OSPI_PIN_DISABLE,
		OSPI_PIN_DISABLE, OSPI_PIN_DISABLE,
		OSPI_MODE_QUAD
	);
}
void config_OSPI(
	OSPI_GPIO_t sck, OSPI_GPIO_t io0, OSPI_GPIO_t io1, OSPI_GPIO_t io2, OSPI_GPIO_t io3,
	OSPI_GPIO_t io4, OSPI_GPIO_t io5, OSPI_GPIO_t io6, OSPI_GPIO_t io7
) {
	fconfig_OSPI(sck, io0, io1, io2, io3, io4, io5, io6, io7, OSPI_MODE_OCTO);
}