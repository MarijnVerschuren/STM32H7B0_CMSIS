//
// Created by marijn on 4/14/24.
//

#include "cryp.h"


/*!<
 * defines
 * */
#define inline __attribute__((always_inline))


/*!<
 * static
 * */
static inline void set_AES_key(const void* key, CRYP_KEY_t key_type) {
	CRYP->K3RR = ((uint32_t*)key)[0];
	CRYP->K3LR = ((uint32_t*)key)[1];
	CRYP->K2RR = ((uint32_t*)key)[2];
	CRYP->K2LR = ((uint32_t*)key)[3];
	if (key_type != CRYP_KEY_128) {
		CRYP->K1RR = ((uint32_t *)key)[4];
		CRYP->K1LR = ((uint32_t *)key)[5];
	if (key_type != CRYP_KEY_192) {
		CRYP->K0RR = ((uint32_t *)key)[6];
		CRYP->K0LR = ((uint32_t *)key)[7];
	}}
}

static inline void set_AES_IV(const void* IV) {
	CRYP->IV0LR = ((uint32_t*)IV)[0];
	CRYP->IV0RR = ((uint32_t*)IV)[1];
	CRYP->IV1LR = ((uint32_t*)IV)[2];
	CRYP->IV1RR = ((uint32_t*)IV)[3];
}


/*!<
 * init
 * */
void config_CRYP(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_CRYPEN;
}

void AES_CBC_encrypt_setup(const void* iv, const void* key, CRYP_KEY_t key_type) {
	CRYP->CR = (CRYP_CR_ALGOMODE_AES_CBC | (key_type << CRYP_CR_KEYSIZE_Pos));

	set_AES_key(key, key_type);
	set_AES_IV(iv);

	// flush in and out FIFOs
	CRYP->CR |= CRYP_CR_FFLUSH | CRYP_CR_CRYPEN;
}

void AES_CBC_decrypt_setup(const void* iv, const void* key, CRYP_KEY_t key_type) {
	CRYP->CR = (
		CRYP_CR_ALGOMODE_AES_KEY | CRYP_CR_ALGODIR |
		(key_type << CRYP_CR_KEYSIZE_Pos)
	);

	set_AES_key(key, key_type);
	CRYP->CR |= CRYP_CR_CRYPEN;
	while (CRYP->SR & CRYP_SR_BUSY);
	CRYP->CR = ((CRYP->CR & ~CRYP_CR_ALGOMODE) | CRYP_CR_ALGOMODE_AES_CBC);
	set_AES_IV(iv);

	// flush in and out FIFOs
	CRYP->CR |= CRYP_CR_FFLUSH | CRYP_CR_CRYPEN;
}


/*!<
 * init
 * */
void AES_CBC_process_block(const void* buffer, void* out) {
	while (!(CRYP->SR & CRYP_SR_IFEM));
	CRYP->DIN = ((uint32_t*)buffer)[0];
	CRYP->DIN = ((uint32_t*)buffer)[1];
	CRYP->DIN = ((uint32_t*)buffer)[2];
	CRYP->DIN = ((uint32_t*)buffer)[3];
	while (!(CRYP->SR & CRYP_SR_OFNE));
	((uint32_t*)out)[0] = CRYP->DOUT;
	((uint32_t*)out)[1] = CRYP->DOUT;
	((uint32_t*)out)[2] = CRYP->DOUT;
	((uint32_t*)out)[3] = CRYP->DOUT;
}