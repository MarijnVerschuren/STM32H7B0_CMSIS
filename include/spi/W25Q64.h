//
// Created by marijn on 5/28/24.
//

#ifndef STM32H7B0_CMSIS_W25Q64_H
#define STM32H7B0_CMSIS_W25Q64_H
#include "main.h"
#include "sys.h"
#include "ospi.h"


/*!<
 * types
 * */
typedef struct {
	// [0]
	uint8_t BUSY	: 1;  // busy
	uint8_t WEL		: 1;  // write enable latch
	uint8_t BP0		: 1;  // block 0 write protection
	uint8_t BP1		: 1;  // block 1 write protection
	uint8_t BP2		: 1;  // block 2 write protection
	uint8_t SEC		: 1;  // sector (1) / block (0) protection [BPx protect 4KB sectors or 64KB blocks]
	uint8_t TB		: 1;  // top (0) / bottom (1) [BPx counts from the top or bottom]
	uint8_t _0		: 1;
	// [1]
	uint8_t SRL		: 1;  // status register lock
	uint8_t QE		: 1;  // quad enable
	uint8_t _1		: 1;
	uint8_t LB1		: 1;  // security register lock 1
	uint8_t LB2		: 1;  // security register lock 2
	uint8_t LB3		: 1;  // security register lock 3
	uint8_t CMP		: 1;  // complement protect
	uint8_t SUS		: 1;  // suspend status
	// [2]
	uint8_t ADS		: 1;  /* > 128MBit only */ // current addr mode (0-3 byte / 1-4 byte)
	uint8_t ADP		: 1;  /* > 128MBit only */ // power-up addr mode
	uint8_t WPS		: 1;
	uint8_t _2		: 2;
	uint8_t DRV0	: 1;
	uint8_t DRV1	: 1;
	uint8_t _3		: 1;
} W25Q64_status_t;



uint8_t W25Q64_init(OCTOSPI_TypeDef* qspi);

#endif // STM32H7B0_CMSIS_W25Q64_H
