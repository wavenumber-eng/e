#include "e_util.h"

uint16_t e__crc16_ccit_step(uint16_t crc_in, uint8_t data)
{
    uint16_t crc_tmp;

    crc_tmp = (uint16_t)((uint8_t)(crc_in >> 8) | (crc_in << 8));
    crc_tmp ^= data;
    crc_tmp ^= (uint8_t)(crc_tmp & 0xff) >> 4;
    crc_tmp ^= crc_tmp << 12;
    crc_tmp ^= (crc_tmp & 0xff) << 5;

    return crc_tmp;
}
