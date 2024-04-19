//
// Created by marijn on 4/19/24.
//

#ifndef STM32H7B0_CMSIS_OSPI_H
#define STM32H7B0_CMSIS_OSPI_H
#include "main.h"
#include "sys.h"


/*!<
 * types
 * */
typedef enum {
	OSPI_CLK_SRC_AHB =			0b00,	//R
	OSPI_CLK_SRC_PLL1_Q =		0b01,
	OSPI_CLK_SRC_PLL2_R =		0b10,
	OSPI_CLK_SRC_PER =			0b11
} OSPI_CLK_SRC_t;


/*!<
 * variables
 * */
extern uint32_t OSPI12_kernel_frequency;


/*!<
 * init
 * */
void config_OSPI_kernel_clock(OSPI_CLK_SRC_t ospi_src);


#endif // STM32H7B0_CMSIS_OSPI_H
