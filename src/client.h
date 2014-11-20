/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */
 
#ifndef _COMM_CLIENT_H_
#define _COMM_CLIENT_H_

#include "Network/utl.h"
#include "Network/sk.h"
#include "Network/buf.h"
#include "Network/ringbuf.h"

#define RECVSIZE 512

struct client_s {
    int fd;
    char ip[20];
    int port;
    pthread_t tid;
    
    struct buf_s *rbuf;
    struct buf_s *wbuf;

    /*get a msg*/
    void (*read_filter)(struct client_s *cli);

    struct ringbuf_s *rb;
    struct ringbuf_s *rb_ext;

};


#ifdef __cplusplus
extern "C" {
#endif

struct client_s* client_init();

int client_conn(struct client_s* client, char *ip, int port);

void client_close(struct client_s* client);


#ifdef __cplusplus
}
#endif

#endif

