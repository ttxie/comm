/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#ifndef _COMM_MINHEAP_H_
#define _COMM_MINHEAP_H_

#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "utl.h"

/**/
struct minheap_node_s {
        int             mn_index;  /**/
        uint64_t        mn_weight;  /**/
        u_char          *mn_ptr;  /**/
};

typedef struct minheap_node_s minheap_node_t;



/**/
struct minheap_s {
        minheap_node_t*         mh_nodes;  /**/
        int                     mh_max_size; /**/
        int                     mh_curr_size; /**/
};
typedef struct minheap_s minheap_t;



/**/
#ifdef __cplusplus
extern "C" {
#endif

/*

 */
minheap_t* minheap_create(size_t size);

/*

 */
void minheap_free(minheap_t* heap);


/*

 */
int minheap_count(const minheap_t* heap);

/*

 */
u_char* minheap_pop(minheap_t* heap);


/*

 */
int minheap_insert(minheap_t* heap, uint64_t weight, u_char *ptr);


/*

*/
#ifdef __cplusplus
}
#endif


#endif

