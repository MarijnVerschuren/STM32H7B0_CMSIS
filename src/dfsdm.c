//
// Created by marijn on 7/19/23.
//

#include "dfsdm.h"


/*!<
 * variables
 * */
uint32_t DFSDM_kernel_frequency = 0;


/*!<
 * init
 * */
void config_DFSDM_kernel_clock(DFSDM_CLK_SRC_t src) {
	RCC->CDCCIP1R &= ~RCC_CDCCIP1R_DFSDM1SEL;
	RCC->CDCCIP1R |= src << RCC_CDCCIP1R_DFSDM1SEL_Pos;
	switch (src) {
		case DFSDM_CLK_SRC_APB2: DFSDM_kernel_frequency = APB2_clock_frequency; return;
		case DFSDM_CLK_SRC_SYS: DFSDM_kernel_frequency = SYS_clock_frequency; return;
	}
}