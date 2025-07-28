
#include <stdint.h>

#ifndef _E_UTIL_H_
#define _E_UTIL_H_



#define CRC_CCIT_SEED       0xFFFF

#ifdef __cplusplus
extern "C"
{
#endif

uint16_t crc16_ccit_step(uint16_t crc, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif
