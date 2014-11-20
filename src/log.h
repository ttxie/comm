/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 * 目前默认支持大文件，同时以日期为基准分割文件
 */

#ifndef _COMM_LOG_H_
#define _COMM_LOG_H_


#ifndef __USE_FILE_OFFSET64
#define __USE_FILE_OFFSET64
#endif

#ifndef __USE_LARGEFILE64
#define __USE_LARGEFILE64
#endif

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
/*以上是打开2G以上的大文件需要的宏*/ 

#include <time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "utl.h"
#include "buf.h"

struct log_s {
        int log_fd;			/*文件描述符, 如果需要效率的话，请使用FILE,但是有BUFF数据不实时*/
        time_t log_ts;			/* 当前打开日志文件的时间戳 */
        char *log_path;	/*路径名 以/结尾 如：/home/work/ */
        char *log_filename;	/*文件名*/
        char *log_suffix;	/*后缀 如：.log*/
        struct buf_s *log_buf;
        int log_pid;
        int log_debug;
        char *log_record_prefix;	/* NOUSE 日志记录前缀，(暂时未实现)，暂时默认为 [时间][pid]  */
};


/*C++支持*/
#ifdef __cplusplus
extern "C"{
#endif

int log_open(struct log_s *log);
void log_close(struct log_s *log);
int log_printf(struct log_s *log, const char *fmt, ...);

/*C++支持*/
#ifdef __cplusplus
}
#endif
#endif
