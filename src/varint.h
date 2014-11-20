/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#ifndef _COMM_VARINT_H_
#define _COMM_VARINT_H_


#include <time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utl.h"

/*
 *  
 *  1 byte:                    0 -                  127
 *  2 bytes:                 128 -                16383
 *  3 bytes:               16384 -              2097151
 *  4 bytes:             2097152 -            268435455
 *  5 bytes:           268435456 -          34359738367
 *  6 bytes:         34359738368 -        4398046511103
 *  7 bytes:       4398046511104 -      562949953421311
 *  8 bytes:     562949953421312 -    72057594037927935
 *  9 bytes:   72057594037927936 -  9223372036854775807
 * 10 bytes: 9223372036854775808 - 18446744073709551615
 *
 */

#ifdef __cplusplus
extern "C"{
#endif

/* 
 * zigzag encode signednumber 
 */

uint64_t zigzag(int64_t n);

int64_t unzigzag(uint64_t n);


/**
 * Write a unsigned number in variable length format into a buffer.
 */
size_t utov(uint64_t num, void* buf);

size_t itov(int64_t num, void* buf);

/**
 * Read a unsgned number in variable length format from a buffer.
 */
size_t vtou(const void* buf, size_t size, uint64_t* n);

size_t vtoi(const void* buf, size_t size, int64_t* n);

/**
 * Check the size of variable length format of a number.
 */
size_t sizeofuv(uint64_t num);

size_t sizeofiv(int64_t num);


#ifdef __cplusplus
}
#endif

#endif
