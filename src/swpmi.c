//
// Created by marijn on 7/19/23.
//

#include "swpmi.h"


/*!<
 * variables
 * */
uint32_t SWPMI_kernel_frequency = 0;


/*!<
 * init
 * */
void config_SWPMI_kernel_clock(SWPMI_CLK_SRC_t src) {
	RCC->CDCCIP1R &= ~RCC_CDCCIP1R_SWPSEL;
	RCC->CDCCIP1R |= src << RCC_CDCCIP1R_SWPSEL_Pos;
	switch (src) {
		case SWPMI_CLK_SRC_APB1:	SWPMI_kernel_frequency = APB1_clock_frequency; return;
		case SWPMI_CLK_SRC_HSI:		SWPMI_kernel_frequency = HSI_clock_frequency; return;
	}
}