/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *  
 *  encoding and decoding for socket stream 
 *  socket stream  (1--n)   packet  (1--1)  [head][body]
 *
 */

#ifndef _COMM_PACK_H_
#define _COMM_PACK_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stddef.h>

#include "endianness.h"
#include "sk.h"
#include "crc16.h"
#include "utl.h"
#include "buf.h"

#define HEAD_LEN 20             /* bytes of head struct in socket stream, filte padding of struct*/

/*
 * +=====================================+
 * | packet | packet flag[FFFF]  0 : 2   |
 * | header +----------------------------+
 * |        | packet length      2 : 2   |
 * |        +----------------------------+
 * |        | command id         4 : 2   |
 * |        +----------------------------+
 * |        | crc16              6 : 2   |
 * |        +----------------------------+
 * |        | sequence id        8 : 4   |
 * |        +----------------------------+
 * |        | user id            12 : 4  |
 * |        +----------------------------+
 * |        | area id[reserve]   16 : 4  |
 * +=====================================+
 * |  body  |    msgpack body            |
 * +=====================================+
 *
 */

struct head_s {
        uint16_t flag;
        uint16_t len;
        uint16_t cmdid;
        uint16_t crc16;
        uint32_t seq;
        uint32_t uid;
        uint32_t aid;
};

#ifdef __cplusplus
extern "C" {
#endif



/*
 *  encode packet for socket stream
 *
 * seq: svr->stat_numcmds
 */
int enpack(struct buf_s *buf, struct head_s *head, char *body, int len);


/*
 *  ½âÎöÍøÂç°üÍ·
 */
char* depack(struct buf_s *buf, struct head_s *head, int *len);


#ifdef __cplusplus
}
#endif


#endif
