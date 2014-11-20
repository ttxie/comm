/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#ifndef _COMM_CHECKSUM_H_
#define _COMM_CHECKSUM_H_


#include <stdint.h>
#include <sys/types.h>



#ifdef __cplusplus
extern "C"{
#endif

uint16_t checksum(uint16_t *dp, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif
