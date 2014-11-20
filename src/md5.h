
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 * Modify by ZhangErjiang
 * Date: 2013-01-10
 */



#ifndef _COMM_MD5_H_
#define _COMM_MD5_H_

#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include "utl.h"

typedef struct {
    uint64_t  bytes;
    uint32_t  a, b, c, d;
    u_char    buffer[64];
} md5_t;

void md5_init(md5_t *ctx);
void md5_update(md5_t *ctx, const void *data, size_t size);
void md5_final(u_char result[16], md5_t *ctx);

void md5(u_char *srcdata, char *result);


#endif /* _MD5_H_INCLUDED_ */
