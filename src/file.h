/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#ifndef _COMM_FILE_H_
#define _COMM_FILE_H_


#include <stdint.h>
#include <sys/types.h>


/*C++支持*/
#ifdef __cplusplus
extern "C"{
#endif


uint32_t linecount(char* filename);
uint32_t linecount_fd(int fd);

size_t filesize(const char* filename);
size_t filesize_fd(int fd);

char * filedata(const char* filename, int *len);

int fdprintf(int fd, const char *fmt, ...);

/*C++支持*/
#ifdef __cplusplus
}
#endif
#endif
