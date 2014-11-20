/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 * ĿǰĬ��֧�ִ��ļ���ͬʱ������Ϊ��׼�ָ��ļ�
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
/*�����Ǵ�2G���ϵĴ��ļ���Ҫ�ĺ�*/ 

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
        int log_fd;			/*�ļ�������, �����ҪЧ�ʵĻ�����ʹ��FILE,������BUFF���ݲ�ʵʱ*/
        time_t log_ts;			/* ��ǰ����־�ļ���ʱ��� */
        char *log_path;	/*·���� ��/��β �磺/home/work/ */
        char *log_filename;	/*�ļ���*/
        char *log_suffix;	/*��׺ �磺.log*/
        struct buf_s *log_buf;
        int log_pid;
        int log_debug;
        char *log_record_prefix;	/* NOUSE ��־��¼ǰ׺��(��ʱδʵ��)����ʱĬ��Ϊ [ʱ��][pid]  */
};


/*C++֧��*/
#ifdef __cplusplus
extern "C"{
#endif

int log_open(struct log_s *log);
void log_close(struct log_s *log);
int log_printf(struct log_s *log, const char *fmt, ...);

/*C++֧��*/
#ifdef __cplusplus
}
#endif
#endif
