//
// Created by marijn on 5/28/24.
//
#include "spi/W25Q64.h"


W25Q64_status_t stat;

static inline uint8_t W25Q64_id(OCTOSPI_TypeDef* qspi) {
	uint8_t id;
	OSPI_TX_t tx = {
		.instruction =		0xABU,
		.instruction_size =	OSPI_SIZE_8B,
		.imode =			OSPI_MODE_SINGLE,
		.address =			0x000000,
		.address_size =		OSPI_SIZE_24B,
		.admode =			OSPI_MODE_SINGLE
	};
	OSPI_transmit(qspi, &tx, 100);
	OSPI_test_receive(qspi, &id);
	return id;
}

static inline void W25Q64_stat(OCTOSPI_TypeDef* qspi) {
	OSPI_TX_t tx = {
		.instruction =		0x00U,
		.instruction_size =	OSPI_SIZE_8B,
		.imode =			OSPI_MODE_SINGLE
	};
	tx.instruction = 0x05U;
	OSPI_transmit(qspi, &tx, 100);
	OSPI_test_receive(qspi, &((uint8_t*)&stat)[0]);
	tx.instruction = 0x35U;
	OSPI_transmit(qspi, &tx, 100);
	OSPI_test_receive(qspi, &((uint8_t*)&stat)[1]);
	tx.instruction = 0x15U;
	OSPI_transmit(qspi, &tx, 100);
	OSPI_test_receive(qspi, &((uint8_t*)&stat)[2]);
}

static inline void W25Q64_SR_write_enable(OCTOSPI_TypeDef* qspi) {
	OSPI_TX_t tx = {
		.instruction =		0x50U,
		.instruction_size =	OSPI_SIZE_8B,
		.imode =			OSPI_MODE_SINGLE
	};
	OSPI_transmit(qspi, &tx, 100);
	delay_ms(1); // stat.WEL = 1;
}
static inline void W25Q64_write_enable(OCTOSPI_TypeDef* qspi) {
	OSPI_TX_t tx = {
		.instruction =		0x06U,
		.instruction_size =	OSPI_SIZE_8B,
		.imode =			OSPI_MODE_SINGLE
	};
	OSPI_transmit(qspi, &tx, 100);
	delay_ms(1); // stat.WEL = 1;
}
static inline void W25Q64_write_disable(OCTOSPI_TypeDef* qspi) {
	OSPI_TX_t tx = {
		.instruction =		0x04U,
		.instruction_size =	OSPI_SIZE_8B,
		.imode =			OSPI_MODE_SINGLE
	};
	OSPI_transmit(qspi, &tx, 100);
	delay_ms(1); // stat.WEL = 1;
}

static inline void W25Q64_quad_enable(OCTOSPI_TypeDef* qspi) {
	OSPI_TX_t tx = {
		.instruction =		0x35U,
		.instruction_size =	OSPI_SIZE_8B,
		.imode =			OSPI_MODE_SINGLE
	};
	uint8_t buffer = 0;
	OSPI_transmit(qspi, &tx, 100);
	OSPI_test_receive(qspi, &buffer);

	W25Q64_SR_write_enable(qspi);
	buffer |= 0b10U;

	tx.instruction = 0x31U;
	tx.buffer = &buffer;
	tx.size = 1U;
	tx.dmode = OSPI_MODE_SINGLE;
	OSPI_transmit(qspi, &tx, 100);
	W25Q64_write_disable(qspi);
}


uint8_t W25Q64_init(OCTOSPI_TypeDef* qspi) {
	uint8_t id = W25Q64_id(qspi);

	W25Q64_stat(qspi);

	if (!stat.QE) {
		W25Q64_quad_enable(qspi);
	}

	delay_ms(100);
	W25Q64_stat(qspi);


	return 0;
}