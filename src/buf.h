/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#ifndef _COMM_BUF_H_
#define _COMM_BUF_H_

#if defined(_WIN32)

#include <winsock2.h>
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <io.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "utl.h"

/*

https://github.com/postgres/postgres/blob/8f00f73dc240a034668023f6d1ab18695bb1be32/contrib/pgcrypto/mbuf.c

this is a ring buf.
*/

struct buf_s {
        u_char         *buf_file;       /*mmap file*/
        u_char         *buf_start;      /*内存起始位*/
        u_char         *buf_end;        /*内存结束位*/
        u_char         *buf_pos;        /*数据起始位*/
        size_t         buf_size;       /*缓冲区总大小*/
        size_t         buf_len;        /*当前占用空间大小*/
        pthread_mutex_t mutex;
};


/*C++支持*/
#ifdef __cplusplus
extern "C" {
#endif

/*
建立缓冲区结构，并分配内存
*/
struct buf_s *buf_calloc(size_t size);

/*
建立缓冲区结构，并分配内存
*/
struct buf_s *buf_mmap(size_t size, const char *file);

/*
 * 添加数据进入缓冲区 

 * 实际网络环境还可以这样操作：
 *       不通过buf_append来添加数据，而是直接将数据recv到buf->buf_pos，
 *       这样就可以减少数据的<<COPY次数>>，但需要自己来修正buf->buf_len.
 *
 */
int buf_append(struct buf_s *buf, u_char *data, size_t len);
int buf_append_unlock(struct buf_s *buf, u_char *data, size_t len);


/*
清空缓冲区 
*/
void buf_reset(struct buf_s *buf);

/*
将数据从缓冲区读出来, 同时清零已读内容
*/
int buf_pull(struct buf_s *buf, u_char *data, size_t len);


/*
将数据从缓冲区读出来, 不清零已读内容
int buf_peek(struct buf_s *buf, u_char *data, size_t len);
*/

/*
将数据从缓冲区删除
*/
int buf_del(struct buf_s *buf, size_t len);
int buf_del_unlock(struct buf_s *buf, size_t len);

/*
释放内存
*/
void buf_free(struct buf_s *buf);


/*C++支持*/
#ifdef __cplusplus
}
#endif

#endif
