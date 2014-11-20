/* 
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com> 
 * All rights reserved. 
 * 
 *  NEED: http_post();
 *
 */ 
#include "http.h"

#define MAX_BUF 128*1024

/*
 * not support chunked body
 * not support 302 redirect
 *
 * surpport timeout on nonblock 
 *
 */
static struct http_response_s*  _http_request(char *sendbuf, int fd, uint64_t timeout)
{
        int ret=0, len =0, n=0;
        size_t pos=0, rlen=0;
        char *buf=NULL, *tmp=NULL;
        struct http_response_s * hres=NULL;
        dstr_t recvbuf=NULL;
        uint64_t tm_start=0;

        len = send(fd, sendbuf, strlen(sendbuf), 0);
        dstrfree(sendbuf);
        if (len == -1) {
                perror("send err");
                return NULL;
        }

        buf = (u_char*)calloc(1, MAX_BUF);
        if (buf == NULL) return NULL;

        recvbuf = dstrnalloc(NULL, 512*1024);
        if (recvbuf == NULL) {
                perror("recvbuf err.");
                goto ERROR;
        }

	/*recv*/
        tm_start = tm64();
        while (1) {
                if ((tm64()-tm_start) > timeout) break; /*nonblock timeout*/

                len = 0;
                len = recv(fd, buf, MAX_BUF, 0);        /*on block timeout ???*/
                if (len == 0) {
                        break;
                } else if (len == -1) {
                        if (errno == EAGAIN) continue; /* nonblock try again */
                        goto ERROR;
                } else {
                        recvbuf = dstrncat(recvbuf, (const void *)buf, len);
                        tm_start = tm64();
                        //printf("recvbuf:%p, add len:%d, recvbuf:%d\n", recvbuf, len, dstrlen(recvbuf));
                }
        }

        hres = (struct http_response_s*)calloc(1, sizeof(struct http_response_s));
        if (hres == NULL) goto ERROR;

        rlen = dstrlen(recvbuf);
        if (rlen == 0) goto ERROR;

        pos = strpos(recvbuf, "\r\n\r\n") + 4;
        hres->data_len = rlen-pos;

        if (strpos(recvbuf, "Content-Length") > -1) {
                n = strpos(recvbuf, "Content-Length");
                tmp = strsub(recvbuf, n+15,
                        strpos((char*)recvbuf+n, "\r\n")-strpos((char*)recvbuf+n, "Content-Length")-15);
                hres->content_length = atoi(trim(tmp));
                free(tmp);
        }else hres->content_length = hres->data_len;

        hres->data = (u_char *)calloc(1, rlen-pos+1);
        if (hres->data == NULL) goto ERROR;
        memcpy(hres->data, (u_char *)recvbuf+pos, rlen-pos);

        tmp = strsub(recvbuf, strpos(recvbuf, "HTTP")+9, 3);        
        hres->status_code = atoi(trim(tmp));
        free(tmp);
        hres->status_text = strsub(recvbuf, 
                                strpos(recvbuf, "HTTP")+13, 
                                strpos(recvbuf, "\r\n")-strpos(recvbuf, "HTTP")-13);

        free(buf);
        dstrfree(recvbuf);
        return hres;

ERROR:
        if (buf != NULL) free(buf);
        if (recvbuf != NULL) dstrfree(recvbuf);
        if (hres != NULL) { 
                if (hres->data != NULL) free(hres->data);
                if (hres->status_text != NULL) free(hres->status_text);
                free(hres);
        }
        return NULL;
} 


/*
 * must get socket fd call sk_connect() first;
 * and call close(fd) after http_get/reget;
 * 
 * return pointer to http_response_s struct, return NULL on failure.
 */
struct http_response_s* http_get(struct http_request_s *hreq)
{
        dstr_t buf;
        struct http_response_s* ret;
        uint64_t timeout = hreq->timeout;

        hreq->fd = sk_connect(hreq->domain, hreq->port);
        if (hreq->fd == -1) {
                perror("connect err..");
                return NULL;
        } 
        //sk_nonblock(hreq->fd);

        if (hreq->start_byte==0 && hreq->end_byte==0) {
                buf = dstrallocf("GET %s HTTP/1.1\r\n"
                                "Host: %s:%d\r\n"
                                "Connection: close\r\n"
                                "\r\n",
                                hreq->url, hreq->domain, hreq->port);
        } else {
                buf = dstrallocf("GET %s HTTP/1.1\r\n"
                                "Host: %s:%d\r\n"
                                "Range: bytes=%d-%d\n"
                                "Connection: close\r\n"
                                "\r\n",
                                hreq->url, hreq->domain, hreq->port,
                                hreq->start_byte, hreq->end_byte);
        }
        
        if (hreq->timeout == 0) timeout = 300000; /*default 300ms on nonblock*/
        ret = _http_request(buf, hreq->fd, timeout);
        //if (ret == NULL) perror("request err");
        close(hreq->fd);

        return ret;
}




/*
 * must get socket fd call sk_connect() first;
 * and call close(fd) after http_get/reget;
 * 
 * return pointer to http_response_s struct, return NULL on failure.
 */
struct http_response_s* http_post(struct http_request_s *hreq)
{
        dstr_t buf;
        struct http_response_s* ret;
        uint64_t timeout = hreq->timeout;

        hreq->fd = sk_connect(hreq->domain, hreq->port);
        if (hreq->fd == -1) {
                perror("connect err..");
                return NULL;
        } 
        //sk_nonblock(hreq->fd);

        buf = dstrallocf("POST %s HTTP/1.1\r\n"
                        "Host: %s:%d\r\n"
                        "Connection: close\r\n"
                        "Content-Length: %d\r\n"
                        "\r\n"
                        "%s\r\n",
                        hreq->url, hreq->domain, hreq->port,
                        strlen(hreq->body), hreq->body);
        
        if (hreq->timeout == 0) timeout = 300000; /*default 300ms on nonblock*/
        ret = _http_request(buf, hreq->fd, timeout);
        //if (ret == NULL) perror("request err");
        close(hreq->fd);

        return ret;
}



/*
struct http_response_s* http_reget(struct http_request_s *hreq)
{
        dstr_t buf;
        struct http_response_s* ret;
        uint64_t timeout = hreq->timeout;

        hreq->fd = sk_connect(hreq->domain, hreq->port);
        if (hreq->fd == -1) {
                perror("connect err..");
                return NULL;
        } 
        sk_nonblock(hreq->fd);

        
        if (hreq->timeout == 0) timeout = 300000; 
        ret = _http_request(buf, hreq->fd, timeout);
        if (ret == NULL) perror("request err");
        close(hreq->fd);

        return ret;
}
*/

int http_headsize(struct http_request_s *hreq)
{
        dstr_t buf;
        struct http_response_s* hres;
        int ret = 0;
        uint64_t timeout = hreq->timeout;

        hreq->fd = sk_connect(hreq->domain, hreq->port);
        if (hreq->fd == -1) {
                return -1;
        } 
//        sk_nonblock(hreq->fd);

        buf = dstrallocf("HEAD %s HTTP/1.1\r\n"
                        "HOST:%s:%d\r\n"
                        "Connection: close\r\n"
                        "\r\n",
                        hreq->url, hreq->domain, hreq->port);

        if (hreq->timeout == 0) timeout = 300000; /*default 300ms on nonblock*/
        hres = _http_request(buf, hreq->fd, timeout);

        if (hres == NULL) ret = -1;
        else ret = hres->content_length;

        if (hres->status_text != NULL) free(hres->status_text);
        if (hres->data != NULL) free(hres->data);
        if (hres != NULL) free(hres);
        close(hreq->fd);
        
        return ret; 
}


