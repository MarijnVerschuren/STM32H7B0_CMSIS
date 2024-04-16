//
// Created by marijn on 7/19/23.
//

#include "rng.h"


/*!<
 * defines
 * */  // these are recommended configurations from the datasheet
#define RNG_CONFIG_1 0x18UL
#define RNG_CONFIG_2 0x00UL
#define RNG_CONFIG_3 0x00UL
#define RNG_CONFIG_HTCR 0x000072ACUL


/*!<
 * variables
 * */
uint32_t RNG_kernel_frequency = 0;


/*!<
 * init
 * */
void config_RNG_kernel_clock(RNG_CLK_SRC_t src) {
	RCC->CDCCIP2R &= ~RCC_CDCCIP2R_RNGSEL;
	RCC->CDCCIP2R |= src << RCC_CDCCIP2R_RNGSEL_Pos;
	switch (src) {
		case RNG_CLK_SRC_HSI48:		RNG_kernel_frequency = HSI48_clock_frequency; return;
		case RNG_CLK_SRC_PLL1_Q:	RNG_kernel_frequency = PLL1_Q_clock_frequency; return;
		case RNG_CLK_SRC_LSE:		RNG_kernel_frequency = LSE_clock_frequency; return;
		case RNG_CLK_SRC_LSI:		RNG_kernel_frequency = LSI_clock_frequency; return;
	}
}


/*!<
 * usage
 * */
void start_RNG(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;
	RNG->CR = (
		RNG_CR_CONDRST								|
		(RNG_CONFIG_1 << RNG_CR_RNG_CONFIG1_Pos)	|
		(RNG_CONFIG_2 << RNG_CR_RNG_CONFIG2_Pos)	|
		(RNG_CONFIG_3 << RNG_CR_RNG_CONFIG3_Pos)
	);
	RNG->CR = ((RNG->CR & ~RNG_CR_CONDRST) | RNG_CR_RNGEN);
}

void stop_RNG(void) {
	RNG->CR &= ~RNG_CR_CONDRST;
}

uint32_t RNG_generate(void) {
	while (RNG->SR & (RNG_SR_SECS | RNG_SR_CECS));
	while (!(RNG->SR & RNG_SR_DRDY));
	return RNG->DR;
}