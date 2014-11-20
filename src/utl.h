/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#ifndef  _COMM_UTL_H_
#define _COMM_UTL_H_

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN 
#include <WinSock2.h>
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/syscall.h>

#include "str.h"

#ifdef __cplusplus
extern "C" {
#endif



//#define gettid() syscall(__NR_gettid)
#define gettid() syscall(SYS_gettid)

#define UNUSED(x) ( (void)(x) )

#if !defined powerof2 
#define powerof2(x)     ((((x) - 1) & (x)) == 0) 
#endif

#if defined(_WIN32)
/*
cocos-2d debug msg printf in VS.

*/
void out(const char *fmt, ...);
#endif

/*

*/
void hexdump(unsigned char *src, int len);



/*date some fun*/
time_t tm_char(const time_t *timep, char *dt);
void tm_timechar(const time_t *timep, char *dt);

/*
get the 64bit time

*/
uint64_t tm64();


int detect_num(const char *dt);



char * redisff(char *str, int len);



u_char* xor32(u_char *src, size_t len, uint32_t factor);


#ifdef __cplusplus
}
#endif

#endif


