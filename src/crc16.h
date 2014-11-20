/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#ifndef _COMM_CRC16_H_
#define _COMM_CRC16_H_

#if defined(_WIN32)
#include <WinSock2.h>
#include <io.h>
#endif

#include <stdint.h>
#include <sys/types.h>



/*C++支持*/
#ifdef __cplusplus
extern "C"{
#endif


uint16_t crc16(u_char* data, size_t len);
int crc16_check(u_char* data, size_t len);


/*C++支持*/
#ifdef __cplusplus
}
#endif
#endif
