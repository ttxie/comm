/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#include "buf.h"


/*
   建立缓冲区结构，并分配内存
 */
struct buf_s *buf_calloc(size_t size)
{
        struct buf_s *buf;

        buf = (struct buf_s*)calloc(1, sizeof(struct buf_s));
        if (buf == NULL) { /*内存分配出错*/
            return NULL;        
        }        

        buf->buf_start = (u_char *)calloc(1, size);
        if (buf->buf_start == NULL) {
            free(buf);
            return NULL; 
        }

        buf->buf_size = size;
        buf->buf_pos = buf->buf_start;
        buf->buf_end = buf->buf_start + size;
        if (pthread_mutex_init(&buf->mutex, NULL) != 0) {
            free(buf);
            return NULL;
        }
        /*buf_file, buf_len = 0, NULL*/

        return buf;
}


/*
   建立缓冲区结构，并分配内存

_WIN32: CreateFileMapping()
*/
#if !defined(_WIN32)
struct buf_s *buf_mmap(size_t size, const char *file) 
{
    struct buf_s *buf;
    void *map;
    int fd, i;

    /*初始化结构*/
    if ((buf = buf_calloc(size)) == NULL) {
        buf_free(buf);
        return NULL;
    }

    /*判断mmap文件是否存在*/
    if (access(file ,F_OK) == -1) { /*file not exist*/
        if ((fd = open(file, O_RDWR|O_CREAT)) == -1) {
            buf_free(buf);
            return NULL;
        }

        if (write(fd, buf->buf_start, size) == -1) {
            buf_free(buf);
            close(fd);
            return NULL;
        }

        map = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0); 
        if (map == MAP_FAILED) {
            buf_free(buf);
            free(buf->buf_start);
            close(fd);
            return NULL;
        }
        close(fd); 
        free(buf->buf_start);

        buf->buf_start = buf->buf_pos = (u_char *)map;
        buf->buf_end = buf->buf_start + size;
        buf->buf_file = (u_char*)file;               /*need test*/
        return buf;
    } else {  /*file exist*/
        if ((fd = open(file, O_RDWR)) == -1) {
            buf_free(buf);
            return NULL;
        }

        map = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0); 
        if (map == MAP_FAILED) {
            buf_free(buf);
            free(buf->buf_start);
            close(fd);
            return NULL;
        }
        free(buf->buf_start);
        close(fd); 

        buf->buf_start = (u_char*)map;
        buf->buf_end = buf->buf_start + size;
        buf->buf_file = (u_char*)file;

        for (i=0; i<(int)size; i++) {            /*找回pos位置*/
            if (buf->buf_start[i] != 0x00) {
                buf->buf_pos = buf->buf_start+i;
                break;
            }
        }

        for (i=(int)size-1; i>=0; i--) {            /*找回len*/
            if (buf->buf_start[i] != 0x00) {
                buf->buf_len = buf->buf_start + i - buf->buf_pos + 1;
                break;
            }
        }

        return buf;
    }

}
#endif

/*
   添加数据进入缓冲区 
   */
static int _buf_append(struct buf_s *buf, u_char *data, size_t len, int lock)
{
    if (buf == NULL || data == NULL || len <= 0) return -1;

    /*缓冲区溢出*/
    if ((buf->buf_size - buf->buf_len) < len) {
        errno = ENOMEM;
        return -1;
    }

    /*移动数据*/
    if (lock) pthread_mutex_lock(&buf->mutex);
    //printf("len:%d end:%x pos:%x\n", len, buf->buf_end, buf->buf_pos);
    if ((size_t)(buf->buf_end - buf->buf_pos) < len) {
        memmove(buf->buf_start, buf->buf_pos, buf->buf_len);
        memset(buf->buf_start+buf->buf_len, 0, buf->buf_size-buf->buf_len);
        buf->buf_pos = buf->buf_start;
    }

    /*添加数据*/
    memcpy(buf->buf_pos+buf->buf_len, data, len);
    buf->buf_len += len;

    if (lock) pthread_mutex_unlock(&buf->mutex);

    return buf->buf_len; 
}

int buf_append(struct buf_s *buf, u_char *data, size_t len)
{
    return _buf_append(buf, data, len, 1);
}
int buf_append_unlock(struct buf_s *buf, u_char *data, size_t len)
{
    return _buf_append(buf, data, len, 0);
}


void buf_reset(struct buf_s *buf)
{
    if (buf == NULL) return;

    pthread_mutex_lock(&buf->mutex);

    memset(buf->buf_start, 0, buf->buf_size);
    buf->buf_pos = buf->buf_start;
    buf->buf_len = 0;

    pthread_mutex_unlock(&buf->mutex);
}

/*
   将数据从缓冲区读出来
   */
int buf_pull(struct buf_s *buf, u_char *data, size_t len)
{
    int ret; 

    if (buf == NULL || data == NULL) return -1;

    if (buf->buf_len == 0) {
        errno = ENODATA;
        return -1;
    }

    pthread_mutex_lock(&buf->mutex);
    if (len >= buf->buf_len) {
        memcpy(data, buf->buf_pos, buf->buf_len);
        memset(buf->buf_pos, 0, buf->buf_len);
        buf->buf_pos = buf->buf_start;
        ret = buf->buf_len;
        buf->buf_len = 0;
        //buf->buf_pos = buf->buf_pos +len;
    }else {
        memcpy(data, buf->buf_pos, len);
        memset(buf->buf_pos, 0, len);
        buf->buf_pos = buf->buf_pos +len;
        buf->buf_len = buf->buf_len -len;
        ret = len;
    }
    pthread_mutex_unlock(&buf->mutex);

    return ret;
}

/*
   将数据从缓冲区删除
   */
static int _buf_del(struct buf_s *buf, size_t len, int lock)
{
    int ret; 

    if (buf == NULL) return -1;

    if (buf->buf_len == 0) {
        errno = ENODATA;
        return -1;
    }

    if (lock) pthread_mutex_lock(&buf->mutex);
    if (len >= buf->buf_len) {
        //memset(buf->buf_pos, 0, buf->buf_len);
        buf->buf_pos = buf->buf_start;
        ret = buf->buf_len;
        buf->buf_len = 0;
        //buf->buf_pos = buf->buf_pos +len;
    }else {
        //memset(buf->buf_pos, 0, len);
        buf->buf_pos = buf->buf_pos +len;
        buf->buf_len = buf->buf_len -len;
        ret = len;
    }
    if (lock) pthread_mutex_unlock(&buf->mutex);

    return ret;
}


int buf_del(struct buf_s *buf, size_t len)
{
    return _buf_del(buf, len, 1);
}

int buf_del_unlock(struct buf_s *buf, size_t len)
{
    return _buf_del(buf, len, 0);
}

/*
   释放内存
   */
void buf_free(struct buf_s *buf)
{
    if (buf == NULL) return ;

    if (buf->buf_file != NULL) {
        UNUSED(NULL);
#if !defined(_WIN32)
        munmap(buf->buf_start, buf->buf_size);
#endif
    } else
        free(buf->buf_start);

    pthread_mutex_destroy(&buf->mutex);
    free(buf);
}


