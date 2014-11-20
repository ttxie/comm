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
        u_char         *buf_start;      /*�ڴ���ʼλ*/
        u_char         *buf_end;        /*�ڴ����λ*/
        u_char         *buf_pos;        /*������ʼλ*/
        size_t         buf_size;       /*�������ܴ�С*/
        size_t         buf_len;        /*��ǰռ�ÿռ��С*/
        pthread_mutex_t mutex;
};


/*C++֧��*/
#ifdef __cplusplus
extern "C" {
#endif

/*
�����������ṹ���������ڴ�
*/
struct buf_s *buf_calloc(size_t size);

/*
�����������ṹ���������ڴ�
*/
struct buf_s *buf_mmap(size_t size, const char *file);

/*
 * ������ݽ��뻺���� 

 * ʵ�����绷������������������
 *       ��ͨ��buf_append��������ݣ�����ֱ�ӽ�����recv��buf->buf_pos��
 *       �����Ϳ��Լ������ݵ�<<COPY����>>������Ҫ�Լ�������buf->buf_len.
 *
 */
int buf_append(struct buf_s *buf, u_char *data, size_t len);
int buf_append_unlock(struct buf_s *buf, u_char *data, size_t len);


/*
��ջ����� 
*/
void buf_reset(struct buf_s *buf);

/*
�����ݴӻ�����������, ͬʱ�����Ѷ�����
*/
int buf_pull(struct buf_s *buf, u_char *data, size_t len);


/*
�����ݴӻ�����������, �������Ѷ�����
int buf_peek(struct buf_s *buf, u_char *data, size_t len);
*/

/*
�����ݴӻ�����ɾ��
*/
int buf_del(struct buf_s *buf, size_t len);
int buf_del_unlock(struct buf_s *buf, size_t len);

/*
�ͷ��ڴ�
*/
void buf_free(struct buf_s *buf);


/*C++֧��*/
#ifdef __cplusplus
}
#endif

#endif
