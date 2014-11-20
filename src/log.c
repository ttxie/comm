/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#include "log.h"

static int _log_init(struct log_s *log)
{
    char sdate[12]={0}, full_name[256]={0};
    time_t time_now, time_dt;

    if (log == NULL) return -1;

    memset(sdate, 0, sizeof sdate);
    memset(full_name, 0, sizeof full_name);

    time(&time_now);
    time_dt = tm_char(&time_now, sdate);
    log->log_ts = time_dt;

    /*拼装文件名*/
    strcat(full_name, log->log_path);
    strcat(full_name, log->log_filename);
    strcat(full_name, sdate);
    strcat(full_name, log->log_suffix);
    log->log_pid = gettid();

    log->log_buf = buf_calloc(8*1024*1024);
    if (log->log_buf == NULL) {
        perror("log: buf_calloc error");
        return -1;
    }


#if defined(__linux)
        log->log_fd = open(full_name, O_WRONLY|O_APPEND|O_CREAT|O_LARGEFILE, 0644);
#else
        log->log_fd = open(full_name, O_WRONLY|O_APPEND|O_CREAT, 0644);
#endif

    return 0;
}

static void* _log_main(void *plog)
{
    struct log_s *log = (struct log_s*)plog;
    int len = 0;

    if (_log_init(log) == -1) {
        perror("log_open err");
    }

    while (1) {
        if (log->log_buf->buf_len > 0) {
            len = log->log_buf->buf_len; 
            write(log->log_fd, log->log_buf->buf_pos, len);
            buf_del(log->log_buf, len);
        }
        usleep(10);
    }

}


/*打开日志*/
int log_open(struct log_s *log)
{
    pthread_attr_t attr;
    pthread_t tid;

    if (pthread_attr_init(&attr) !=0) {
        perror("log::log_open pthread_attr_init err");
    }

    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) !=0) {
        perror("log::log_open pthread_attr_setdetachstate err");
    }

    if (pthread_create(&tid, NULL, _log_main, log) != 0) {
        perror("log::log_open pthread_create error");
        return -1;
    }

    return tid;
}


/*关闭旧日志文件*/
void log_close(struct log_s *log)
{
    if (log == NULL) return ;

    close(log->log_fd);
}

/*写日志*/
int log_printf(struct log_s *log, const char *fmt, ...)
{
    static time_t time_a=0;
    time_t time_now;
    va_list args;
    int len;
    static char now[20]={0};
    static char line[1024]={0};
    uint64_t start, end;
    struct tm *tv;

    if (log == NULL) return -1;

    time(&time_now);
    if ((time_now - log->log_ts) > 3600*24) {
        log_close(log);
        if (_log_init(log) == -1) {
            perror("log_open err");
        }
    }
    if (time_now > time_a) {
        tv = localtime(&time_now);
        time_a = time_now;
        sprintf(now, "%02d%02d%02d%02d%02d%02d",
                tv->tm_year+1900-2000, tv->tm_mon+1, tv->tm_mday,
                tv->tm_hour, tv->tm_min, tv->tm_sec);
    }

    sprintf(line, "[%s][%d]", now, log->log_pid);

    va_start(args, fmt);
    len = vsprintf(line+strlen(line), fmt, args);
    va_end(args);
    if (len < 0 || len > 1024) {
        return -1;
    }

    if (log->log_debug) 
        fprintf(stderr, "%s", line);
    buf_append(log->log_buf, line, strlen(line));


    //return write(log->log_fd, line, strlen(line));
}


