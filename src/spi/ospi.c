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
