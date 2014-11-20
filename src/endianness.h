/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#ifndef _COMM_ENDIANESS_H_
#define _COMM_ENDAINESS_H_

#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


/*C++支持*/
#ifdef __cplusplus
extern "C" {
#endif


/*
 *  macro: detection of the system endianess
 * ENDIAN = 'L', LITTLE_ENDIAN = 1 on little_endian system
 * ENDIAN = 'B', BIG_ENDIAN = 1 on big_endian system
 *
 * 0x0A0B0C0D = 
 *      [0A0B0C0D] on big_endian system   ibm, sun
 *      [0D0C0B0A] on little_endian system  x86, android
 */
static union { char c[4]; unsigned int n; }  endian = { { 'L', '\0', '\0', 'B' } };
 
#define _ENDIAN ((char)endian.n)

//#define _LITTLE_ENDIAN (((char)endian.n) == 'L')  /*if (_LITTLE_ENDIAN)  can't #if _LITTLE_ENDIAN*/
//#define _BIG_ENDIAN (((char)endian.n) == 'B')  /*if (_BIG_ENDIAN)*/

#define _LITTLE_ENDIAN 1
#define _BIG_ENDIAN 0


/*
 * 
 *
 * host byte ordering[little-endian] convert to network byte ordering[big-endian]
 */
uint16_t hton16(uint16_t num);
uint32_t hton32(uint32_t num);
uint64_t hton64(uint64_t num);



/*
 * 
 *
 * network byte ordering[big-endian] convert to host byte ordering[little-endian]
 */
uint16_t ntoh16(uint16_t num);
uint32_t ntoh32(uint32_t num);
uint64_t ntoh64(uint64_t num);




/*C++支持*/
#ifdef __cplusplus
}
#endif

#endif


