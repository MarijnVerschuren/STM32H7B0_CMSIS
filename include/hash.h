//
// Created by marijn on 4/14/24.
//

#ifndef STM32H7B0_CMSIS_HASH_H
#define STM32H7B0_CMSIS_HASH_H
#include "main.h"


/*!<
 * enum types
 * */
typedef enum {
	HASH_ALGO_SHA1 =		0x000UL,
	HASH_ALGO_MD5 =			0x001UL,
	HASH_ALGO_SHA2_224 =	0x800UL,
	HASH_ALGO_SHA2_256 =	0x801UL
}	HASH_ALGO_t;


/*!<
 * variables
 * */
extern volatile uint8_t* HASH_digest;


/*!<
 * init
 * */
void config_HASH(void);


/*!<
 * functions
 * */
void process_HASH(void* buffer, uint32_t size, HASH_ALGO_t algo);
void process_HMAC(void* buffer, uint32_t size, void* key, uint32_t key_size, HASH_ALGO_t algo);


#endif // STM32H7B0_CMSIS_HASH_H
