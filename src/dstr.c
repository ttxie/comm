/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#include "dstr.h"

/*
 * alloc memory of struct dstr_s and fill string
 * init: src string
 * initlen: len of buf
 * flag: extend size to power of 2
 */

static dstr_t _dstrnalloc(const void *init, size_t initlen, char flag)
{
        struct dstr_s *ds;
        size_t size=0;
        
        if (flag)       /*must len, no extend*/
                size = initlen + DSTR_HEAD;
        else
                size = fitsize(initlen + 1 + DSTR_HEAD);
                
        ds = (struct dstr_s*)calloc(1, size);
        if (ds == NULL) return NULL;

        ds->size = size - DSTR_HEAD;

        /*init the buf data*/
        if (initlen>0 && init!=NULL) {
                memcpy(ds->buf, init, initlen);
                ds->len = initlen;
        }

        return (dstr_t)ds->buf;
}

dstr_t dstrnalloc(const void *init, size_t initlen)
{
        return _dstrnalloc(init, initlen, 1);
}

dstr_t dstralloc(const char *init) 
{
        if (init == NULL) return NULL;
        return _dstrnalloc(init, strlen(init), 0);
}

dstr_t dstrallocf(const char *fmt, ...)
{
        va_list args;
        char *buf;
        char *ret;

        buf = (char *)calloc(1, 4096);
        if (buf == NULL) return NULL;

        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);
        
        ret = _dstrnalloc(buf, strlen(buf), 0);

        free(buf);
        return ret;
}

void dstrfree(dstr_t s) {
	struct dstr_s *ds;
        if (s == NULL) return;
        ds = (struct dstr_s*) (s-DSTR_HEAD);
        free(ds);
        
}

dstr_t dstrempty(dstr_t s) {
	struct dstr_s *ds;
        if (s == NULL) return NULL;
        ds = (struct dstr_s*) (s-DSTR_HEAD);
        ds->buf[0] = '\0';
        ds->size = 0;
        ds->len = 0;
        return realloc(ds, DSTR_HEAD);
}

void dstrclear(dstr_t s) {
	struct dstr_s *ds;
        if (s == NULL) return;
        ds = (struct dstr_s*) (s-DSTR_HEAD);
        ds->size = 0;
        ds->len = 0;
        ds->buf[0] = '\0';
}



/*

*/
static dstr_t _dstr_prepare(dstr_t s, size_t addlen) {
        struct dstr_s *ds, *newds;
        size_t avail = dstravail(s), newsize=0;

        if (avail >= addlen) return s;

        ds = (struct dstr_s*) (s-DSTR_HEAD);

        newsize = fitsize(ds->len + addlen + DSTR_HEAD);
        if (newsize < DSTR_MAX_PREALLOC)
                newsize *= 2;

        newds = (struct dstr_s*)realloc((void*)ds, newsize);
        if (newds == NULL) return NULL;
        newds->size = newsize-DSTR_HEAD;

        return newds->buf;
}




static dstr_t _dstrncat(dstr_t s, const void *add, size_t addlen) 
{
        struct dstr_s *ds;
        s = _dstr_prepare(s, addlen);
        ds = (struct dstr_s*) (s-DSTR_HEAD);
        if (s == NULL) return NULL;

        memset(s+ds->len, 0, addlen);
        memcpy(s+ds->len, add, addlen);
        ds->len += addlen;
        return s;
}

dstr_t dstrcat(dstr_t s, const char *t) 
{
        if (s == NULL) return NULL;
        return _dstrncat(s, t, strlen(t)+1);
}

dstr_t dstrncat(dstr_t s, const void *t, size_t len)
{
        if (s == NULL) return NULL;
        return _dstrncat(s, t, len);
}

dstr_t dstrcatf(dstr_t s, const char *fmt, ...)
{
        va_list args;
        char *buf;
        char *ret;

        buf = (char *)calloc(1, 4096);
        if (buf == NULL) return NULL;

        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);
        
        ret = _dstrncat(s, buf, strlen(buf)+1);

        free(buf);
        return ret;
}




static dstr_t _dstrncpy(dstr_t s, const void *t, size_t len) 
{
        struct dstr_s *ds = (struct dstr_s*) (s-DSTR_HEAD);
        if (ds->size < len) {
                s = _dstr_prepare(s, len - ds->len);
                if (s == NULL) return NULL;
                ds = (struct dstr_s*) (s-DSTR_HEAD);
        }
        
        memset(s, 0, len+1);
        memcpy(s, t, len);
        ds->len = len;
        return s;
}

dstr_t dstrcpy(dstr_t s, const char *t)
{
        if (s == NULL) return NULL;
        return _dstrncpy(s, t, strlen(t));
}

dstr_t dstrncpy(dstr_t s, const void *t, size_t len)
{
        if (s == NULL) return NULL;
        return _dstrncpy(s, t, len);
}

dstr_t dstrcpyf(dstr_t s, const char *fmt, ...)
{
        va_list args;
        char *buf;
        char *ret;

        buf = (char *)calloc(1, 4096);
        if (buf == NULL) return NULL;

        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);
        
        ret = _dstrncpy(s, buf, strlen(buf));

        free(buf);
        return ret;

}

