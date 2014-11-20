/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 *
 */
#include "pack.h"



/*
 *  封装网络包
 */
int enpack(struct buf_s *buf, struct head_s *head, char *body, int body_len)
{
    int len = body_len+HEAD_LEN;
    char *pack = NULL;

    if (head == NULL || buf == NULL) return -1;
    pack = (char*)calloc(1, len);
    if (pack ==NULL) return -1;

    /*packet head*/
    head->flag = 0xFFFF;
    head->len = hton16(len);
    head->cmdid = hton16(head->cmdid);
    head->seq = hton32(head->seq);
    head->crc16 = crc16(body, body_len);
    head->uid = hton32(head->uid);
    head->aid = hton32(head->aid);

    /*into buf_s*/
    memcpy(pack, head, HEAD_LEN);
    memcpy(pack+HEAD_LEN, body, body_len);
    
    buf_append(buf, pack, len);
    free(pack);

    return len;
}


/*
 *  解析网络包头
 */
char* depack(struct buf_s *buf, struct head_s *head, int *len)
{
    char * body = NULL;

    if (buf == NULL) goto ERRRET;

    while (1) {
        /*just a part of packet, required more data */
        if (buf->buf_len < sizeof(struct head_s)) { /* length of buf LT length of header */
            errno = EINVAL;
            goto ERRRET;
        }

        memset(head, 0, HEAD_LEN);
        memcpy(head, buf->buf_pos, HEAD_LEN); 

        head->seq = ntoh32(head->seq);
        head->cmdid = ntoh16(head->cmdid);
        head->len = ntoh16(head->len);
        head->uid = ntoh32(head->uid);
        head->aid = ntoh32(head->aid);

        if (head->flag != 0xFFFF) {
            buf_del(buf, 1);
            continue;
        }

        if (head->len > buf->buf_len) {
            errno = EINVAL;
            goto ERRRET;
        }

        /*crc16 check*/ 
        if (head->crc16 !=  crc16(buf->buf_pos+HEAD_LEN, head->len-HEAD_LEN)) {
            buf_del(buf, 1);
            continue;
            /*
               errno = EBADMSG; 
               head = NULL;
               return -1;
               */
        } else break;
    }
    *len = head->len;
    if ((*len-HEAD_LEN) > 0) {
        body = (char *)malloc(*len-HEAD_LEN);
        memcpy(body, buf->buf_pos+HEAD_LEN, *len-HEAD_LEN);
    }
    buf_del(buf, head->len);

    return body;

ERRRET:
    *len = -1; 
    return NULL;
}



