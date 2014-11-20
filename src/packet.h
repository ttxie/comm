/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *  
 *  encoding and decoding for socket stream 
 *  socket stream  (1--n)   packet  (1--1)  [head][body]
 *
 *  TODO:语言无关性
 */

#ifndef _COMM_PACKET_H_
#define _COMM_PACKET_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stddef.h>

#include "varint.h"
#include "endianness.h"
#include "sk.h"
#include "crc16.h"
#include "bitset.h"
#include "utl.h"
#include "buf.h"
#include "darray.h"

#define ARR_SIZE(x) (sizeof(x)/sizeof((x)[0]))  /*compute size of array */
#define HEAD_LEN 20             /* bytes of head struct in socket stream, filte padding of struct*/

/*
 * NEXT: 
 * 1. 字符串使用：LZW压缩算法
 * 2. 加入结构体指针：LEN[bitmap][结构体][bitmap][结构体]
 * */

/*
basic packet data type:
        PACK_SINT/UINT: unsigned number store in varint char[], signed number require zigzag encoded.
        PACK_STRING: char[] with terminating '\0' character
        PACK_CHARP: char* pointer to string with terminating '\0' character
*/
enum packet_type {
        PACK_SINT8  = 0,
        PACK_UINT8  = 1,
        PACK_SINT16 = 2,
        PACK_UINT16 = 3,
        PACK_SINT32 = 4,
        PACK_UINT32 = 5,
        PACK_SINT64 = 6,
        PACK_UINT64 = 7,
        PACK_STRING = 8,
        PACK_CHARP  = 9
};

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
 * | packet | null bitmap       12 : > 2 |
 * | body1  +----------------------------+
 * |        | filed value   [len][value] |
 * +=====================================+
 * | packet | null bitmap       12 : > 2 |
 * | bodyn  +----------------------------+
 * |        | filed value   [len][value] |
 * +=====================================+
 *
 *   len: varint
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
 *  encode packet body for socket stream
 */
int enbody(u_char *ptr, const size_t arr[][2], int rows, struct buf_s *body, size_t bm_pos);


/*
 *  encode packet for socket stream
 */
int enpacket(struct head_s *head, u_char *ptr, int item_len, int item_size,
                const size_t arr[][2], int rows, struct buf_s *buf);


/*
 *  解析网络包头
 */
int dehead(struct buf_s *buf, struct head_s *head);



/*
输入网络数据包，解析到结构体中

buf:输入数据
arr:结构字段说明
rows:矩阵行数
*ptr:输出结构指针

返回值：body的长度

*/
int depacket(struct buf_s *buf, const size_t arr[][2], int rows, u_char *ptr);

int depacket_da(struct buf_s *buf, const size_t arr[][2], int rows, struct darray_s *da);


#ifdef __cplusplus
}
#endif


#endif
