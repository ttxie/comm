/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#include "endianness.h"


/**
 * Normalize a 16-bit number in the native order into the network byte order.
 */
uint16_t hton16(uint16_t num) {
#if _BIG_ENDIAN 
        return num;
#else
        return ((num & 0x00ffU) << 8) | ((num & 0xff00U) >> 8);
#endif
}


/**
 * Normalize a 32-bit number in the native order into the network byte order.
 */
uint32_t hton32(uint32_t num) {
#if _BIG_ENDIAN 
        return num;
#else
        return ((num & 0x000000ffUL) << 24) | ((num & 0x0000ff00UL) << 8) | \
                ((num & 0x00ff0000UL) >> 8) | ((num & 0xff000000UL) >> 24);
#endif
}


/**
 * Normalize a 64-bit number in the native order into the network byte order.
 */
uint64_t hton64(uint64_t num) {
#if _BIG_ENDIAN 
        return num;
#else
        return  ((num & 0x00000000000000ffULL) << 56) | \
                ((num & 0x000000000000ff00ULL) << 40) | \
                ((num & 0x0000000000ff0000ULL) << 24) | \
                ((num & 0x00000000ff000000ULL) << 8 ) | \
                ((num & 0x000000ff00000000ULL) >> 8 ) | \
                ((num & 0x0000ff0000000000ULL) >> 24) | \
                ((num & 0x00ff000000000000ULL) >> 40) | \
                ((num & 0xff00000000000000ULL) >> 56);
#endif
}


/**
 * Denormalize a 16-bit number in the network byte order into the native order.
 */
uint16_t ntoh16(uint16_t num) {
#if _BIG_ENDIAN 
        return num;
#else
        return ((num & 0x00ffU) << 8) | ((num & 0xff00U) >> 8);
#endif
}


/**
 * Denormalize a 32-bit number in the network byte order into the native order.
 */
uint32_t ntoh32(uint32_t num) {
#if _BIG_ENDIAN 
        return num;
#else
        return ((num & 0x000000ffUL) << 24) | ((num & 0x0000ff00UL) << 8) | \
                ((num & 0x00ff0000UL) >> 8) | ((num & 0xff000000UL) >> 24);
#endif
}


/**
 * Denormalize a 64-bit number in the network byte order into the native order.
 * if (_BIG_ENDIAN) return num;
 */
uint64_t ntoh64(uint64_t num) {
#if _BIG_ENDIAN 
        return num;
#else
        return  ((num & 0x00000000000000ffULL) << 56) | \
                ((num & 0x000000000000ff00ULL) << 40) | \
                ((num & 0x0000000000ff0000ULL) << 24) | \
                ((num & 0x00000000ff000000ULL) << 8 ) | \
                ((num & 0x000000ff00000000ULL) >> 8 ) | \
                ((num & 0x0000ff0000000000ULL) >> 24) | \
                ((num & 0x00ff000000000000ULL) >> 40) | \
                ((num & 0xff00000000000000ULL) >> 56);
#endif
}




