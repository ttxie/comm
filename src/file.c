/*
 * file.c
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>

#include "file.h"
#include <stdio.h>
#include <fcntl.h>

#define BUFSIZE 512*1024


uint32_t linecount_fd(int fd)
{
    int len, count=0, n;
    u_char *buf;

    if (fd < 0) return 0;
    buf = (u_char *)calloc(1, BUFSIZE);
    if (buf == NULL) return 0;

    while (len=read(fd, buf, BUFSIZE)) {
        for (n=0; n<len; n++) {
            if (buf[n] == 0x0A) count++;
        }
    }
    lseek(fd, SEEK_SET, 0); 

    free(buf);

    return count;
}

/*
 *  count the number of line of the file.
 */
uint32_t linecount(char* filename)
{
    int fd, count=0;

    if (filename == NULL) return 0;

    fd = open(filename, O_RDONLY);
    if (fd == -1) return 0;

    count = linecount_fd(fd);
    close(fd);

    return count;
}



size_t filesize_fd(int fd)
{
    struct stat st;

    if (fstat(fd, &st) == -1) return -1;
    else return st.st_size;
}



size_t filesize(const char* filename)
{
    int fd;
    size_t sz;

    if (filename == NULL) return -1;

    fd = open(filename, O_RDONLY);
    if (fd == -1) return -1;

    sz = filesize_fd(fd);
    close(fd);

    return sz;
}


char * filedata(const char* filename, int *len)
{
    int fd, sz;
    char* data;

    if (filename == NULL || len == NULL) return NULL;

    fd = open(filename, O_RDONLY);
    if (fd == -1) return NULL;

    sz = filesize_fd(fd);
    data = (char*)calloc(1, sz);
    if (data == NULL) {
        close(fd);
        return NULL;
    }

    *len = read(fd, data, sz);
    close(fd);

    if ((*len) == -1) {
        free(data);                
        return NULL;
    } else return data;
}

/*has no prefix of record*/
int fdprintf(int fd, const char *fmt, ...)
{
    va_list args;
    int len;
    char buf[1024];

    if (fd < 0) return -1;

    memset(buf, 0, sizeof buf);
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    len = write(fd, buf, strlen(buf));
    if (len == -1) {
        perror("file_printf err");
        printf("file_printf err fd:%d\n", fd);
        return -1;
    }
    return len;
}







