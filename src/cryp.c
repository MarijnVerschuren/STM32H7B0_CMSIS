//
// Created by marijn on 4/14/24.
//

#include "cryp.h"


void config_CRYP(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_CRYPEN;


}