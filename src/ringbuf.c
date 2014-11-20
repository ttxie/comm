/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#include "ringbuf.h"

/*
 * alloc memory
 *
 * [ ] [ ] [2] [3] [4] [5] [6] [7] [8] [ ] [ ] [ ] [ ]
 *         head                        tail
 */
struct ringbuf_s *rb_calloc(int size)
{
    struct ringbuf_s *rb=NULL;

    do {
        rb = (struct ringbuf_s*)calloc(1, sizeof(struct ringbuf_s));
        if (rb == NULL) break;

        rb->rb_buf = (char **)calloc(size, sizeof(char*));
        if (rb->rb_buf == NULL) break;

        if (pthread_mutex_init(&rb->rb_mutex, NULL) != 0) break;

        rb->rb_size = size;
        rb->rb_head = rb->rb_tail = -1;
        return rb;
    }while (0);

    if (rb->rb_buf == NULL) free(rb->rb_buf);
    if (rb == NULL) free(rb);
    return NULL;
}

/*
 * push data into ring buffer
 */
int rb_push(struct ringbuf_s *rb, u_char *data)
{
    char *old = NULL;
    if (rb == NULL || data == NULL) return -1;

    if (rb->rb_head == -1) {
        pthread_mutex_lock(&rb->rb_mutex);
        rb->rb_head = 0;
        rb->rb_tail = 1;
        rb->rb_buf[0] = data;
        pthread_mutex_unlock(&rb->rb_mutex);

        return 0;
    }

    pthread_mutex_lock(&rb->rb_mutex);
    /*ring buffer is full, printf WARNING*/
    if (rb->rb_buf[rb->rb_tail%rb->rb_size] != NULL) {
        old = rb->rb_buf[rb->rb_tail%rb->rb_size];
        rb->rb_head++;
    }
    rb->rb_buf[rb->rb_tail%rb->rb_size] = data;
    rb->rb_tail++;
    pthread_mutex_unlock(&rb->rb_mutex);

/* must be c malloc not c++ new.*/
    if (old) {
        free(old);
        printf("[WARNING]the ringbuf node have no response.\n");
    }

    return 0;
}

/*
 * delete from ring buffer
 * if seq not exist return -1, return 0 success
 */
char *rb_pop(struct ringbuf_s *rb)
{
    char *ret=NULL;
    uint32_t pos = 0;

    do {
        pthread_mutex_lock(&rb->rb_mutex);
        pos = rb->rb_head%rb->rb_size;

        if (rb == NULL) break;
        if (rb->rb_head == -1) break;
        if (rb->rb_buf[pos] == NULL) break;

        /*pop out the node*/
        ret = rb->rb_buf[pos];
        rb->rb_buf[pos] = NULL;
        rb->rb_head++;
        pthread_mutex_unlock(&rb->rb_mutex);

        return ret;
    } while(0);

    pthread_mutex_unlock(&rb->rb_mutex);
    return NULL;
}

/*
 * free ring buffer
 */
void rb_free(struct ringbuf_s *rb)
{
    if (rb->rb_buf != NULL) free(rb->rb_buf);
    if (rb != NULL) free(rb);
}

