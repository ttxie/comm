/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#include "varint.h"


/*
zigzag:
     0  ->   0
    -1  ->   1
     1  ->   2
    -2  ->   3
     2  ->   4

*/
uint64_t zigzag(int64_t n)
{
//        return (n << 1) ^ (n >> 63);
        if (n < 0)
                return ((uint64_t)(-n)) * 2 - 1;
        else
                return n * 2;
}

int64_t unzigzag(uint64_t n)
{
//        return (n >> 1) ^ -((int64_t)(n & 1));
        if (n&1)
                return -(n>>1) - 1;
        else
                return n>>1;
}


/**
 * unsigned number convert to varint.
 * varint code by little_endianness 
 */
size_t utov(uint64_t num, void* buf)
{
        unsigned char* wp = (unsigned char*)buf;

        if (num < (1ULL << 7)) {
                *(wp++) = num;
        } else if (num < (1ULL << 14)) {
                *(wp++) = (num >> 7) | 0x80;
                *(wp++) = num & 0x7f;
        } else if (num < (1ULL << 21)) {
                *(wp++) = (num >> 14) | 0x80;
                *(wp++) = ((num >> 7) & 0x7f) | 0x80;
                *(wp++) = num & 0x7f;
        } else if (num < (1ULL << 28)) {
                *(wp++) = (num >> 21) | 0x80;
                *(wp++) = ((num >> 14) & 0x7f) | 0x80;
                *(wp++) = ((num >> 7) & 0x7f) | 0x80;
                *(wp++) = num & 0x7f;
        } else if (num < (1ULL << 35)) {
                *(wp++) = (num >> 28) | 0x80;
                *(wp++) = ((num >> 21) & 0x7f) | 0x80;
                *(wp++) = ((num >> 14) & 0x7f) | 0x80;
                *(wp++) = ((num >> 7) & 0x7f) | 0x80;
                *(wp++) = num & 0x7f;
        } else if (num < (1ULL << 42)) {
                *(wp++) = (num >> 35) | 0x80;
                *(wp++) = ((num >> 28) & 0x7f) | 0x80;
                *(wp++) = ((num >> 21) & 0x7f) | 0x80;
                *(wp++) = ((num >> 14) & 0x7f) | 0x80;
                *(wp++) = ((num >> 7) & 0x7f) | 0x80;
                *(wp++) = num & 0x7f;
        } else if (num < (1ULL << 49)) {
                *(wp++) = (num >> 42) | 0x80;
                *(wp++) = ((num >> 35) & 0x7f) | 0x80;
                *(wp++) = ((num >> 28) & 0x7f) | 0x80;
                *(wp++) = ((num >> 21) & 0x7f) | 0x80;
                *(wp++) = ((num >> 14) & 0x7f) | 0x80;
                *(wp++) = ((num >> 7) & 0x7f) | 0x80;
                *(wp++) = num & 0x7f;
        } else if (num < (1ULL << 56)) {
                *(wp++) = (num >> 49) | 0x80;
                *(wp++) = ((num >> 42) & 0x7f) | 0x80;
                *(wp++) = ((num >> 35) & 0x7f) | 0x80;
                *(wp++) = ((num >> 28) & 0x7f) | 0x80;
                *(wp++) = ((num >> 21) & 0x7f) | 0x80;
                *(wp++) = ((num >> 14) & 0x7f) | 0x80;
                *(wp++) = ((num >> 7) & 0x7f) | 0x80;
                *(wp++) = num & 0x7f;
        } else if (num < (1ULL << 63)) {
                *(wp++) = (num >> 56) | 0x80;
                *(wp++) = ((num >> 49) & 0x7f) | 0x80;
                *(wp++) = ((num >> 42) & 0x7f) | 0x80;
                *(wp++) = ((num >> 35) & 0x7f) | 0x80;
                *(wp++) = ((num >> 28) & 0x7f) | 0x80;
                *(wp++) = ((num >> 21) & 0x7f) | 0x80;
                *(wp++) = ((num >> 14) & 0x7f) | 0x80;
                *(wp++) = ((num >> 7) & 0x7f) | 0x80;
                *(wp++) = num & 0x7f;
        } else {
                *(wp++) = (num >> 63) | 0x80;
                *(wp++) = ((num >> 56) & 0x7f) | 0x80;
                *(wp++) = ((num >> 49) & 0x7f) | 0x80;
                *(wp++) = ((num >> 42) & 0x7f) | 0x80;
                *(wp++) = ((num >> 35) & 0x7f) | 0x80;
                *(wp++) = ((num >> 28) & 0x7f) | 0x80;
                *(wp++) = ((num >> 21) & 0x7f) | 0x80;
                *(wp++) = ((num >> 14) & 0x7f) | 0x80;
                *(wp++) = ((num >> 7) & 0x7f) | 0x80;
                *(wp++) = num & 0x7f;
        }

        return wp - (unsigned char*)buf;
}




/**
 * Read a number in variable length format from a buffer.
 * varint code by little_endianness 
 * return the length of variable number.
 */
size_t vtou(const void* buf, size_t size, uint64_t* n) 
{
        const unsigned char* src = (const unsigned char*)buf; /*poiter to begin*/
        const unsigned char* tail = src + size; /*pointer to end*/
        uint64_t num = 0;
        int b=0;  /*number of bytes*/

        do {
                if (src >= tail) {
                        *n = 0;
                        return 0;
                        //b=0;
                }

                num = (num << 7) + (*src & 0x7f);
                b = b+7;
        } while (*src++ >= 0x80); /*first >= then ++*/
/*
        if (b > 32) *n = ntoh64(num);
        else if (b > 16) *n = ntoh32(num);
        else if (b > 8) *n = ntoh16(num);
        else *n = num;
*/
        *n = num;
        return src - (const unsigned char*)buf;
}



/**
 * Check the size of variable length format of a number.
 */
size_t sizeofuv(uint64_t num) 
{
        if (num < (1ULL << 7)) return 1;
        if (num < (1ULL << 14)) return 2;
        if (num < (1ULL << 21)) return 3;
        if (num < (1ULL << 28)) return 4;
        if (num < (1ULL << 35)) return 5;
        if (num < (1ULL << 42)) return 6;
        if (num < (1ULL << 49)) return 7;
        if (num < (1ULL << 56)) return 8;
        if (num < (1ULL << 63)) return 9;
        return 10;
}


size_t sizeofiv(int64_t num) 
{
        return sizeofuv(zigzag(num));
}

