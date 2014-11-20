/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#ifndef _COMM_CONF_H_
#define _COMM_CONF_H_

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

struct conf_item_s {
        char            *item_section;  /* [section] */
        char            *item_key; /* key = value*/
        char            *item_value;
};


struct conf_s {
        char                    *conf_file;      /*config filename*/
        size_t                  conf_count;       /*item count*/
        struct conf_item_s      *conf_items;      /*pointer to item array*/
};

#ifdef __cplusplus
extern "C" {
#endif


/*
load the conf item into memory

@file config file name
return pointer to structure on success, NULL on failure
*/
struct conf_s* conf_init(const char *file);



/*
get the item from memory

@conf structure
@section
@key of item
return value on found, return NULL if not found.
*/
char* conf_get(struct conf_s *conf, const char *section, const char *key);


/*
destory the config item in  memory

*/
void conf_free(struct conf_s *conf);


#ifdef __cplusplus
}
#endif

#endif

