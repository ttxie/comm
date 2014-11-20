/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */


#ifndef COMM_RANDOM_INCLUDED
#define COMM_RANDOM_INCLUDED

#include <stddef.h>

#ifdef __cplusplus
extern "C"{
#endif

/*  Seeds the pseudorandom number generator. */
void random_seed();

/*  Generate a pseudorandom byte sequence. */
void random_gen(void *buf, size_t len);

#ifdef __cplusplus
}
#endif


#endif
