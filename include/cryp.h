//
// Created by marijn on 4/14/24.
//

#ifndef STM32H7B0_CMSIS_CRYP_H
#define STM32H7B0_CMSIS_CRYP_H
#include "main.h"


/*!<
 * defines
 * */
#define AES_BLOCK_SIZE 0x10UL


/*!<
 * enum types
 * */
typedef enum {
	CRYP_KEY_128 =			0b00UL,
	CRYP_KEY_192 =			0b01UL,
	CRYP_KEY_256 =			0b10UL,
}	CRYP_KEY_t;


/*!<
 * init
 * */
void config_CRYP(void);
void AES_CBC_encrypt_setup(const void* IV, const void* key, CRYP_KEY_t key_type);
void AES_CBC_decrypt_setup(const void* IV, const void* key, CRYP_KEY_t key_type);


/*!<
 * usage
 * */
void AES_CBC_process_block(const void* buffer, void* out);


#endif // STM32H7B0_CMSIS_CRYP_H
