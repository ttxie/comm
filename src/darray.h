/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#ifndef _COMM_DARRAY_H_
#define _COMM_DARRAY_H_


#include <stdint.h>
#include <sys/types.h>

struct darray_s {
        u_char   *da_values; 
        uint32_t da_nelts;                      /* number of elements in dynamic array */
        uint32_t da_size;                       /* size of element, bytes */
        uint32_t da_used;                       /* number of used elements in dynamic array */
};



/*C++支持*/
#ifdef __cplusplus
extern "C"{
#endif

struct darray_s* darray_calloc(uint32_t number, uint32_t size);
void darray_free(struct darray_s *da);
void *darray_pushback(struct darray_s *da);

uint32_t darray_count(struct darray_s *da);


/*C++支持*/
#ifdef __cplusplus
}
#endif
#endif
