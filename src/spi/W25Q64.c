//
// Created by marijn on 5/28/24.
//
#include "spi/W25Q64.h"


uint8_t W25Q_ReadID(uint8_t* buf) {
	/*com.InstructionMode = QSPI_INSTRUCTION_1_LINE; // QSPI_INSTRUCTION_...
	com.Instruction = 0xABU;	 // Command

	com.AddressMode = QSPI_ADDRESS_1_LINE;
	com.AddressSize = QSPI_ADDRESS_24_BITS;
	com.Address = 0x0U;

	com.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	com.AlternateBytes = QSPI_ALTERNATE_BYTES_NONE;
	com.AlternateBytesSize = QSPI_ALTERNATE_BYTES_NONE;

	com.DummyCycles = 0;
	com.DataMode = QSPI_DATA_1_LINE;
	com.NbData = 1;

	com.DdrMode = QSPI_DDR_MODE_DISABLE;
	com.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	com.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;*/


	/*if (HAL_QSPI_Command(&hqspi, &com, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)
		!= HAL_OK) {
		return W25Q_SPI_ERR;
	}*/

	/*
	if (HAL_QSPI_Receive(&hqspi, buf, HAL_QSPI_TIMEOUT_DEFAULT_VALUE)
		!= HAL_OK) {
		return W25Q_SPI_ERR;
	}
	*/

	return 0;
}