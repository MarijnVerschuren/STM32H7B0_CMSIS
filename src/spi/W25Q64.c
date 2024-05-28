//
// Created by marijn on 5/28/24.
//
#include "spi/W25Q64.h"


W25Q64_status_t stat;

static uint8_t W25Q64_id() {
	uint8_t id;
	OSPI_TX_t tx = {
		.instruction =		0xABU,
		.instruction_size =	OSPI_SIZE_8B,
		.imode =			OSPI_MODE_SINGLE,
		.address =			0x000000,
		.address_size =		OSPI_SIZE_24B,
		.admode =			OSPI_MODE_SINGLE
	};
	OSPI_transmit(OCTOSPI1, &tx, 100);
	OSPI_test_receive(OCTOSPI1, &id);
	return id;
}

static void W25Q64_stat() {
	OSPI_TX_t tx = {
		.instruction =		0x00U,
		.instruction_size =	OSPI_SIZE_8B,
		.imode =			OSPI_MODE_SINGLE
	};
	tx.instruction = 0x05U;
	OSPI_transmit(OCTOSPI1, &tx, 100);
	OSPI_test_receive(OCTOSPI1, &((uint8_t*)&stat)[0]);
	tx.instruction = 0x35U;
	OSPI_transmit(OCTOSPI1, &tx, 100);
	OSPI_test_receive(OCTOSPI1, &((uint8_t*)&stat)[1]);
	tx.instruction = 0x15U;
	OSPI_transmit(OCTOSPI1, &tx, 100);
	OSPI_test_receive(OCTOSPI1, &((uint8_t*)&stat)[2]);
}

static void W25Q64_SR_write_enable() {
	OSPI_TX_t tx = {
		.instruction =		0x50U,
		.instruction_size =	OSPI_SIZE_8B,
		.imode =			OSPI_MODE_SINGLE
	};
	OSPI_transmit(OCTOSPI1, &tx, 100);
	delay_ms(1); // stat.WEL = 1;
}
static void W25Q64_write_enable() {
	OSPI_TX_t tx = {
		.instruction =		0x06U,
		.instruction_size =	OSPI_SIZE_8B,
		.imode =			OSPI_MODE_SINGLE
	};
	OSPI_transmit(OCTOSPI1, &tx, 100);
	delay_ms(1); // stat.WEL = 1;
}
static void W25Q64_write_disable() {
	OSPI_TX_t tx = {
		.instruction =		0x04U,
		.instruction_size =	OSPI_SIZE_8B,
		.imode =			OSPI_MODE_SINGLE
	};
	OSPI_transmit(OCTOSPI1, &tx, 100);
	delay_ms(1); // stat.WEL = 1;
}

static void W25Q64_quad_enable() {
	OSPI_TX_t tx = {
		.instruction =		0x35U,
		.instruction_size =	OSPI_SIZE_8B,
		.imode =			OSPI_MODE_SINGLE
	};
	uint8_t buffer = 0;
	OSPI_transmit(OCTOSPI1, &tx, 100);
	OSPI_test_receive(OCTOSPI1, &buffer);

	W25Q64_SR_write_enable();
	buffer |= 0b10U;

	tx.instruction = 0x31U;
	tx.buffer = &buffer;
	tx.size = 1U;
	tx.dmode = OSPI_MODE_SINGLE;
	OSPI_transmit(OCTOSPI1, &tx, 100);
	W25Q64_write_disable();
}


uint8_t W25Q64_init() {
	uint8_t id = W25Q64_id();

	W25Q64_stat();

	if (!stat.QE) {
		W25Q64_quad_enable();
	}

	delay_ms(100);
	W25Q64_stat();


	return 0;
}