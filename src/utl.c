/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#include "utl.h"


#if defined(_WIN32)
void out(const char *fmt, ...)
{
        va_list args;
        char buf[1024];

        memset(buf, 0, sizeof buf);

        va_start(args, fmt);
        vsprintf(buf, fmt, args);	
        va_end(args);

        OutputDebugStringA(buf);	
}
#endif


/*
 * printf the memory as hex like tcpdump
 * @src pointer to memory address
 * @len memory length.
 */
void hexdump(unsigned char *src, int len)
{
#if defined(_WIN32)
#define printf out 
#endif
       int n = 0, i = 0;
        unsigned char line[16+1];

        if (src == NULL) return ;
        memset(line, 0, sizeof(line));
        printf("%p   ", src+n);
        while(n < len) {
                /* not character ascill will output [.] */
                if (src[n] > 0x7F || src[n] < 0x21)
                        line[n%16] = 0x2E;
                else
                        line[n%16] = src[n];

                printf("%02X", *(src+n));

                if ((n+1) % 2 == 0)
                        printf(" ");
                if ((n+1) % 16 == 0) {
                        printf("        %s", line);
                        memset(line, 0, sizeof(line));

                        if ((n+1) < len) 
                                printf("\n%p   ", (src+n+1) );
                }
                n++;
        }

        if (len % 16 != 0) {
                /* the last line need space*/
                for( ; i < (16-len%16); i++) {
                        if (i%2 ==0)
                                printf("   ");
                        else
                                printf("  ");
                }
                printf("        %s", line);
        }
        printf("\n");

#if defined(_WIN32)
#undef printf
#endif
}




/*
    tm to char that format 'YYYYMMDD'
 */
time_t tm_char(const time_t *timep, char *dt)
{
        struct tm *tv;

        if (timep == NULL || dt == NULL) return 0;

        tv = localtime(timep);
        sprintf(dt, "%04d%02d%02d", tv->tm_year+1900, tv->tm_mon+1, tv->tm_mday);

        tv->tm_sec = 0;
        tv->tm_min = 0;
        tv->tm_hour = 0;
        return mktime(tv);
}

/*
    tm to char that format 'YYYYMMDDHH24MISS'
 */
void tm_timechar(const time_t *timep, char *dt)
{
        struct tm *tv;

        if (timep == NULL || dt == NULL) return;

        tv = localtime(timep);
        sprintf(dt, "%02d%02d%02d%02d%02d%02d", 
                        tv->tm_year+1900-2000, tv->tm_mon+1, tv->tm_mday, 
                        tv->tm_hour, tv->tm_min, tv->tm_sec);

}


/*
get the 64bit time

unit: us(microsecond)
*/
uint64_t tm64()
{
#if !defined(_WIN32)

        struct timeval timev;
        uint64_t now;

        gettimeofday(&timev, NULL);
        now = (uint64_t)timev.tv_sec * (uint64_t)1000000 + timev.tv_usec;;
        
        return now;
#endif


#if defined(_WIN32)
        /* from nginx.org */
        uint64_t  intervals;
        FILETIME  ft;

        GetSystemTimeAsFileTime(&ft);

        /*
        * A file time is a 64-bit value that represents the number
        * of 100-nanosecond intervals that have elapsed since
        * January 1, 1601 12:00 A.M. UTC.
        *
        * Between January 1, 1970 (Epoch) and January 1, 1601 there were
        * 134744 days,
        * 11644473600 seconds or
        * 11644473600,000,000,0 100-nanosecond intervals.
        *
        * See also MSKB Q167296.
        */

        intervals = ((uint64_t) ft.dwHighDateTime << 32) | ft.dwLowDateTime;
        intervals -= 116444736000000000;

        return intervals/10;
#endif
}

/*
[0-9].

*/
int detect_num(const char *dt)
{
        size_t i;

        if (dt == NULL) return 0;
        for(i=0; i<strlen(dt); i++) {
                switch((u_char)dt[i]) {
                case 0x2e: case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: 
                case 0x35: case 0x36: case 0x37: case 0x38: case 0x39:
                        break;          /*break the switch not for loop*/
                default:
                       return 0;  /*return false*/
                }
        }

        return 1; /*return true*/
}





char * redisff(char *str, int len)
{
        unsigned short ff = 0xffff;
        char *p;

        if (str == NULL) return NULL;
        p = (char*)calloc(1, len+2);

        memcpy(p, &ff, 2);
        memcpy(p+2, str, len);
        
        return p;
}


/*
char *gbk2utf8()
{
    if (in == NULL)
        return -1;

    iconv_t cd;
    cd = iconv_open("GBK", "UTF-8");
    if ( cd == (iconv_t)(-1) ) {
        perror("iconv_open failed");
        return -1;
    }

    size_t in_left = strlen(in) + 1;
    char *out_ptr;
    size_t res;

    out_ptr = out;
    res = iconv(cd, &in, &in_left, &out_ptr, &size);
    if (res == (size_t)(-1)) {
        perror("iconv failed");
        return -1;
    }

    iconv_close(cd);
    return 0;

}
*/


/*  
    Finds next power of two for n. If n itself 
    is a power of two then returns n*/ 

size_t nextpowerof2(size_t n) 
{ 
        n--; 

        n |= n >> 1; 
        n |= n >> 2; 
        n |= n >> 4; 
        n |= n >> 8; 
        n |= n >> 16; 
        n++; 

        return n;
}     
/* 
   size_t nextpowerof2(size_t num) 
   { 
   size_t powerof2 = 8;  //start of 8 

   while(powerof2 < num) { 
   powerof2 <<= 1; 
   } 

   return powerof2; 
   } 
 */


size_t fitsize(size_t num)
{
        if(num < 4096) {
                return nextpowerof2(num);
        }else { //greater than size of one page 
                return (floor((num-1)/4096)+1)*4096;
        }
}




/*
 * 
 * factor = 0x3FFEFFFF   is prime number, is recommend
 *
 * return u_char * must be free(); return NULL if there is an error.
 */
u_char* xor32(u_char *src, size_t len, uint32_t factor)
{
        size_t n=0;
        uint32_t *dst = NULL, *org = NULL;

        if (src == NULL) return NULL;
        dst = (uint32_t*)calloc(1, len+1);
        if (dst == NULL) return NULL;
        org = (uint32_t*)src;

        while ((n*4) < len) {
                *(dst+n) = *(org+n) ^ factor;
                n++;
        }

        return (u_char *)dst;
}

