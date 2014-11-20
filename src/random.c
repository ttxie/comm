/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "utl.h"
#include "random.h"

static uint64_t seed = 0xfa9b23e307cc611fULL;

void random_seed()
{
    uint64_t pid;

    pid = (uint64_t) getpid ();

    /*  The initial state for pseudo-random number generator is computer from
        the exact timestamp and process ID. */
    seed ^= pid + tm64();
}



void random_gen(void *buf, size_t len)
{
    uint8_t *pos;

    pos = (uint8_t*) buf;

    while (1) {

        /*  Generate a pseudo-random integer. */
        seed = seed * 1103515245 + 12345;

        /*  Move the bytes to the output buffer. */
        memcpy (pos, &seed, len > 8 ? 8 : len);
        if (len <= 8)
            return;
        len -= 8;
        pos += 8;
    }
}

