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
	do { RCC->AHB3ENR |= RCC_AHB3ENR_IOMNGREN; } while (!(RCC->AHB3ENR & RCC_AHB3ENR_IOMNGREN));
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

	OCTOSPIM->PCR[sck_pin.id.sub & 0b1U] &= ~(OCTOSPIM_PCR_CLKSRC);
	OCTOSPIM->PCR[sck_pin.id.sub & 0b1U] |= (
		(ospi_num << OCTOSPIM_PCR_CLKSRC_Pos)	|
		OCTOSPIM_PCR_CLKEN
	);
	if (nss) {
		OCTOSPIM->PCR[nss_pin.id.sub & 0b1U] &= ~(OCTOSPIM_PCR_NCSSRC);
		OCTOSPIM->PCR[nss_pin.id.sub & 0b1U] |= (
			(ospi_num << OCTOSPIM_PCR_NCSSRC_Pos)	|
			OCTOSPIM_PCR_NCSEN
		);
	}
	if (io0_pin.id.sub & 0x4U) {
		OCTOSPIM->PCR[io0_pin.id.sub & 0b1U] &= ~(OCTOSPIM_PCR_IOHSRC);
		OCTOSPIM->PCR[io0_pin.id.sub & 0b1U] |= (
			((ospi_num << 1U) << OCTOSPIM_PCR_IOHSRC_Pos)	|
			OCTOSPIM_PCR_IOHEN
		);
	} else {
		OCTOSPIM->PCR[io0_pin.id.sub & 0b1U] &= ~(OCTOSPIM_PCR_IOLSRC);
		OCTOSPIM->PCR[io0_pin.id.sub & 0b1U] |= (
			((ospi_num << 1U) << OCTOSPIM_PCR_IOLSRC_Pos)	|
			OCTOSPIM_PCR_IOLEN
		);
	}
	if(io4) {
	if (io4_pin.id.sub & 0x4U) {
		OCTOSPIM->PCR[io4_pin.id.sub & 0b1U] &= ~(OCTOSPIM_PCR_IOHSRC);
		OCTOSPIM->PCR[io4_pin.id.sub & 0b1U] |= (
			(((ospi_num << 1U) | 0b1U) << OCTOSPIM_PCR_IOHSRC_Pos)	|
			OCTOSPIM_PCR_IOHEN
		);
	} else {
		OCTOSPIM->PCR[io4_pin.id.sub & 0b1U] &= ~(OCTOSPIM_PCR_IOLSRC);
		OCTOSPIM->PCR[io4_pin.id.sub & 0b1U] |= (
			(((ospi_num << 1U) | 0b1U) << OCTOSPIM_PCR_IOLSRC_Pos)	|
			OCTOSPIM_PCR_IOLEN
		);
	}
	}
}


void fconfig_OSPI(  // TODO: mode??
	OCTOSPI_TypeDef* ospi,
	uint8_t prescaler, OSPI_MEMORY_t mem, uint8_t addr_size, uint8_t nss_high_time, uint8_t nss_boundary,
	uint8_t DLYB_bypass, OSPI_CLOCK_MODE_t clk_mode, OSPI_WRAP_SIZE_t wrap, uint8_t max_transfer,
	uint32_t refresh, uint8_t fifo_threshold, OSPI_SHIFT_t shift, OSPI_HOLD_t delay, uint8_t dummy_cycles
) {
	uint32_t mask = ospi == OCTOSPI1 ? RCC_AHB3ENR_OSPI1EN : RCC_AHB3ENR_OSPI2EN;
	do { RCC->AHB3ENR |= mask; } while (!(RCC->AHB3ENR & mask));

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


// TODO: mmapped:
/**
	// instruction phase
In memory-mapped mode, the instruction used for the write operation is specified
in OCTOSPI_WIR, and the instruction format is specified in OCTOSPI_WCCR.
The instruction used for the read operation and the instruction format are specified in OCTOSPI_IR and OCTOSPI_CCR
	// address phase
In memory-mapped mode, the address is given directly via the AXI (from any master in the system)
In memory-mapped mode, the address format for the write operation is specified in
OCTOSPI_WCCR. The address format for the read operation is specified in OCTOSPI_CCR
	// alternate byte phase
In memory-mapped mode, the alternate bytes used for the write operation are specified
in OCTOSPI_WABR, and the alternate byte format is specified in OCTOSPI_WCCR.
The alternate bytes used for read operation and the alternate byte format are specified in OCTOSPI_ABR and OCTOSPI_CCR
	// dummy-cycle phase
In memory-mapped mode, the dummy cycles for the write operations are specified in
OCTOSPI_WTCR. The dummy cycles for the read operation are specified in OCTOSPI_TCR
	// data phase
In memory-mapped mode, the data format for the write operation is specified in
OCTOSPI_WCCR. The data format for the read operation is specified in OCTOSPI_CCR
*/
/*
 •Instruction phase WIP
 •Address phase TODO
 •Alternate-byte phase TODO
 •Dummy-cycle phase TODO
 •Data phase TODO
 */
// TODO: dynamic modes?
void fconfig_OSPI_MMAP(
	OCTOSPI_TypeDef* ospi,
	uint32_t read_instruction,	uint32_t write_instruction,	OSPI_SIZE_t instruction_size,
	OSPI_SIZE_t address_size,	uint32_t alt_bytes,			OSPI_SIZE_t alt_bytes_size,
	OSPI_MODE_t imode,	uint8_t idtr,	OSPI_MODE_t admode,	uint8_t addtr,
	OSPI_MODE_t abmode,	uint8_t abdtr,	OSPI_MODE_t dmode,	uint8_t ddtr
) {
	uint32_t ccfg = (
		// set modes
		(imode << OCTOSPI_CCR_IMODE_Pos)	|	// instruction mode
		(admode << OCTOSPI_CCR_ADMODE_Pos)	|	// address mode
		(abmode << OCTOSPI_CCR_ABMODE_Pos)	|	// alternate byte mode
		(dmode << OCTOSPI_CCR_IMODE_Pos)	|	// data mode
		// set sizes
		(instruction_size << OCTOSPI_CCR_ISIZE_Pos)	|
		(address_size << OCTOSPI_CCR_ADSIZE_Pos)	|
		(alt_bytes_size << OCTOSPI_CCR_ABSIZE_Pos)
	);
	if (idtr)	{ ccfg |= OCTOSPI_CCR_IDTR; }
	if (addtr)	{ ccfg |= OCTOSPI_CCR_ADDTR; }
	if (abdtr)	{ ccfg |= OCTOSPI_CCR_ABDTR; }
	if (ddtr)	{ ccfg |= OCTOSPI_CCR_DDTR; }
	ospi->CCR =  ccfg;
	ospi->WCCR = ccfg;

	ospi->IR = read_instruction;	// set read instruction
	ospi->WIR = write_instruction;	// set write instruction
	ospi->ABR = alt_bytes;			// set read alt-bytes
	ospi->WABR = alt_bytes;			// set write alt-bytes

	ospi->WTCR = ospi->TCR;			// set write TCR
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
		ospi,
		prescaler, mem, addr_size, OSPI_DEFAULT_NSS_HIGH_TIME,
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
		ospi,
		prescaler, mem, addr_size, OSPI_DEFAULT_NSS_HIGH_TIME,
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
		ospi,
		prescaler, mem, addr_size, OSPI_DEFAULT_NSS_HIGH_TIME,
		OSPI_DEFAULT_NSS_BOUNDARY, OSPI_DEFAULT_DLYB_BYPASS,
		OSPI_CLOCK_MODE_0, OSPI_WRAP_SIZE_NONE, 0U /* max_transfer */,
		0U /* refresh */, 0U /* fifo_threshold */, OSPI_SHIFT_NONE,
		OSPI_HOLD_NONE, 0U /* dummy_cycles */
	);
}



/*!<
 * usage
 * */
// indirect mode
uint8_t OSPI_test_receive(OCTOSPI_TypeDef* ospi, uint8_t* buffer) {
	// TODO: check influence of: com.NbData = 1;
	// TODO: then fix this function
	uint8_t cnt = ospi->DLR + 1;
	ospi->CR &= ~OCTOSPI_CR_FMODE;
	ospi->CR |= (0b1 << OCTOSPI_CR_FMODE_Pos);
	ospi->AR = ospi->AR; // ?????

	do {
		while (!(ospi->SR & (OCTOSPI_SR_FTF | OCTOSPI_SR_TCF)));
		*buffer = *((volatile uint8_t*)&ospi->DR);
		buffer++; cnt--;
	} while (cnt);

	while (!(ospi->SR & OCTOSPI_SR_TCF));
	ospi->FCR = OCTOSPI_FCR_CTCF;

	return 0;
}


uint32_t OSPI_transmit(OCTOSPI_TypeDef* ospi, OSPI_TX_t* tx, uint32_t timeout) {
	uint64_t start = tick;
	while (ospi->SR & OCTOSPI_SR_BUSY) { if ( tick - start > timeout) { return 0xFFFFFFFFUL; } }

	ospi->CCR = (
		// set modes
		(tx->imode << OCTOSPI_CCR_IMODE_Pos)	|	// instruction mode
		(tx->admode << OCTOSPI_CCR_ADMODE_Pos)	|	// address mode
		(tx->abmode << OCTOSPI_CCR_ABMODE_Pos)	|	// alternate byte mode
		(tx->dmode << OCTOSPI_CCR_IMODE_Pos)	|	// data mode
		// set sizes
		(tx->instruction_size << OCTOSPI_CCR_ISIZE_Pos)	|
		(tx->address_size << OCTOSPI_CCR_ADSIZE_Pos)	|
		(tx->alt_bytes_size << OCTOSPI_CCR_ABSIZE_Pos)
	);

	if (tx->idtr) { ospi->CCR |= OCTOSPI_CCR_IDTR; }
	if (tx->addtr) { ospi->CCR |= OCTOSPI_CCR_ADDTR; }
	if (tx->abdtr) { ospi->CCR |= OCTOSPI_CCR_ABDTR; }
	if (tx->ddtr) { ospi->CCR |= OCTOSPI_CCR_DDTR; }

	ospi->IR = tx->instruction;		// set instruction
	ospi->AR = tx->address;			// set address
	ospi->ABR = tx->alt_bytes;		// set alt-bytes

	// dummy-cycle / data phase
	ospi->DLR = tx->size;			// set transfer size
	for (uint32_t i = 0; i < tx->size; i++) {
		while (ospi->SR & OCTOSPI_SR_FTF) { if ( tick - start > timeout) { return tx->size - i; } }
		*((__IO uint8_t*)&ospi->DR) = tx->buffer[i];
	}
	while (ospi->SR & OCTOSPI_SR_TCF) { if ( tick - start > timeout) { return 0xFFFFFFFFUL; } }
	ospi->FCR |= OCTOSPI_FCR_CTCF;
	return 0UL;
}
