/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 * 
 *
 * dynamic string
 *
 */

#ifndef __DSTR_H
#define __DSTR_H

#define DSTR_MAX_PREALLOC (1024*1024)
#define DSTR_HEAD 8
typedef char * dstr_t;

#include <stdio.h>      /* printf, scanf, puts */
#include <stdlib.h>     /* realloc, free, exit, NULL */
#include <sys/types.h>
#include <stdarg.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

struct dstr_s {
        uint32_t size;  /*the capacity of buf*/
        uint32_t len;
        char buf[1];
};

/*
 * create or free dynamic string
 */
dstr_t dstrnalloc(const void *init, size_t initlen);
dstr_t dstralloc(const char *init);
dstr_t dstrallocf(const char *fmt, ...);
void dstrfree(dstr_t s);


dstr_t dstrempty(dstr_t s);
void dstrclear(dstr_t s);


/*append string maybe generate new point and return*/
dstr_t dstrcat(dstr_t s, const char *t);
dstr_t dstrcatf(dstr_t s, const char *fmt, ...);
dstr_t dstrncat(dstr_t s, const void *t, size_t len);

dstr_t dstrcpy(dstr_t s, const char *t);
dstr_t dstrcpyf(dstr_t s, const char *fmt, ...);
dstr_t dstrncpy(dstr_t s, const void *t, size_t len);


/*
#ifdef __GNUC__
sds sdscatprintf(sds s, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
#else
sds sdscatprintf(sds s, const char *fmt, ...);
#endif
*/

/*
 * return the used memory size of the buf (no include sizeof struct dstr_s)
 */
static size_t dstrlen(const dstr_t s) {
    struct dstr_s *ds = (struct dstr_s*)(s-DSTR_HEAD);
    return ds->len;
}

/*
 * return the memory size of the buf (no include sizeof struct dstr_s)
 */
static size_t dstrsize(const dstr_t s) {
    struct dstr_s *ds = (struct dstr_s*)(s-DSTR_HEAD);
    return ds->size;
}

/*
 * return the free memory size of the struct dstr_s 
 */
static size_t dstravail(const dstr_t s) {
    struct dstr_s *ds = (struct dstr_s*)(s-DSTR_HEAD);
    return ds->size - ds->len;
}


#ifdef __cplusplus
}
#endif


#endif
