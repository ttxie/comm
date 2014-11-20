/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#ifndef _COMM_RINGBUF_H_
#define _COMM_RINGBUF_H_

#if defined(_WIN32)

#include <winsock2.h>
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <io.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

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
 * this is a simple ring buf 
 *
 * 【seq must be auto increment】
 * 
 * [ ] [ ] [2] [3] [4] [5] [6] [7] [8] [ ] [ ] [ ] [ ]
 *         head                        tail
 */

struct ringbuf_s {
    char           **rb_buf;        /*free(rb_buf); point to ringbuf*/
    int            rb_head;        /*head = the first data entry */
    int            rb_tail;        /*tail = SEQ of data = first empty entry*/
    int            rb_size;        /*size of ring buffer*/
    pthread_mutex_t rb_mutex;
};


/*C++*/
#ifdef __cplusplus
extern "C" {
#endif

    /*
     * alloc memory
     */
    struct ringbuf_s *rb_calloc(int size);


    /*
     * push data into ring buffer
     * 【data seq = tb->tail】
     */
    int rb_push(struct ringbuf_s *rb, u_char *data);


    /*
     * pop data from ring buffer, delete from ring buffer
     * if have no data return -1, return 0 success
     */
    char *rb_pop(struct ringbuf_s *rb);


    /*
     * free ring buffer
     */
    void rb_free(struct ringbuf_s *rb);


    /* C++ */
#ifdef __cplusplus
}
#endif

#endif

