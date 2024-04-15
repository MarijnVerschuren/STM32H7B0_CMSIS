//
// Created by marijn on 4/14/24.
//

#include "hash.h"


/*!<
 * variables
 * */
volatile uint8_t* HASH_digest = HASH_DIGEST->HR;


/*!<
 * init
 * */
void config_HASH() {
	do { RCC->AHB2ENR |= RCC_AHB2ENR_HASHEN; } \
	while (!(RCC->AHB2ENR & RCC_AHB2ENR_HASHEN));
}


/*!<
 * functions
 * */
void process_HASH(void* buffer, uint32_t size, HASH_ALGO_t algo) {
	HASH->STR = (((size % 4) << 3) << HASH_STR_NBLW_Pos);
	HASH->CR = (								// 32-bit dat by default
		(algo << HASH_CR_ALGO_Pos)				// set algorithm
	);
	HASH->CR |= HASH_CR_INIT;

	uint32_t word_count = ((size + 3) >> 2);
	for (uint32_t i = 0; i < word_count; i++) {
		HASH->DIN = ((uint32_t*)buffer)[i];
	}

	HASH->STR |= HASH_STR_DCAL;
	while (HASH->SR & HASH_SR_DCIS);
}

void process_HMAC(void* buffer, uint32_t size, void* key, uint32_t key_size, HASH_ALGO_t algo) {
	uint32_t word_count;
	HASH->CR = (									// 32-bit dat by default
		((key_size > 0x40UL) * HASH_CR_LKEY)	|	// enable LKEY if key larger than 64 bytes
		(algo << HASH_CR_ALGO_Pos)				|	// set algorithm
		HASH_CR_MODE								// HMAC mode
	);
	HASH->CR |= HASH_CR_INIT;

	for(;;) {
		word_count = ((key_size + 3) >> 2);		// step 1, 3 (write key)
		HASH->STR = (((key_size % 4) << 3) << HASH_STR_NBLW_Pos);
		for (uint32_t i = 0; i < word_count; i++) {
			HASH->DIN = ((uint32_t*)key)[i];
		} HASH->STR |= HASH_STR_DCAL;

		if (!buffer) { while (HASH->SR & HASH_SR_DCIS); return; }
		while (HASH->SR & HASH_SR_BUSY);

		word_count = ((size + 3) >> 2);			// step 2	(write data)
		HASH->STR = (((size % 4) << 3) << HASH_STR_NBLW_Pos);
		for (uint32_t i = 0; i < word_count; i++) {
			HASH->DIN = ((uint32_t*)buffer)[i];
		} HASH->STR |= HASH_STR_DCAL;
		while (HASH->SR & HASH_SR_BUSY);
		buffer = NULL;
	}
}