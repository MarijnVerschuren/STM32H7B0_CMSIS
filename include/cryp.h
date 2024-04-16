//
// Created by marijn on 4/14/24.
//

#ifndef STM32H7B0_CMSIS_CRYP_H
#define STM32H7B0_CMSIS_CRYP_H
#include "main.h"


/*!<
 * enum types
 * */
typedef enum {
	CRYP_ALGO_TDES_ECB =	CRYP_CR_ALGOMODE_TDES_ECB,	// TODO!!
	CRYP_ALGO_TDES_CBC =	CRYP_CR_ALGOMODE_TDES_CBC,	// TODO!!
	CRYP_ALGO_DES_ECB =		CRYP_CR_ALGOMODE_DES_ECB,	// TODO!!
	CRYP_ALGO_DES_CBC =		CRYP_CR_ALGOMODE_DES_CBC,	// TODO!!
	CRYP_ALGO_AES_ECB =		CRYP_CR_ALGOMODE_AES_ECB,
	CRYP_ALGO_AES_CBC =		CRYP_CR_ALGOMODE_AES_CBC,
	CRYP_ALGO_AES_CTR =		CRYP_CR_ALGOMODE_AES_CTR,
	CRYP_ALGO_AES_GCM =		CRYP_CR_ALGOMODE_AES_GCM,	// TODO!!
	CRYP_ALGO_AES_CCM =		CRYP_CR_ALGOMODE_AES_CCM	// TODO!!
}	CRYP_ALGO_t;

typedef enum {
	CRYP_KEY_128 =			0b00UL,
	CRYP_KEY_192 =			0b01UL,
	CRYP_KEY_256 =			0b10UL,
}	CRYP_KEY_t;


/*!<
 * init
 * */
void config_CRYP(CRYP_ALGO_t algorithm, CRYP_KEY_t key);
void CRYP_encrypt_setup(void* IV, void* key);
void CRYP_decrypt_setup(void* IV, void* key);


/*!<
 * usage
 * */
void CRYP_process_block(void* buffer, uint32_t size, void* out);


#endif // STM32H7B0_CMSIS_CRYP_H
