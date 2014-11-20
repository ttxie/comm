#include <pthread.h>
#include "client.h"

#if defined(_WIN32)
#include "websock-w32.h"
#endif

static int _client_recv(struct client_s *client)
{
    int len = 0;
    char buf[RECVSIZE];

    while (1) {
		memset(buf, 0, RECVSIZE);
        len = recv(client->fd, (char *)buf, RECVSIZE, 0);
        if (len == -1) {
#if defined(_WIN32)
            errno = WSAGetLastError();
#endif
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == 10035 ||errno == EINTR) {
                return 0;
            } else {
                return -1;
            }
        } else if (len == 0) {
            return -1;
        }

        buf_append(client->rbuf, (unsigned char*)buf, len);
    }

}

static void* _client_main(void *cli)
{
    int len = 0;
    struct client_s *client = (struct client_s*)cli;

    while (1) {
        if (_client_recv(client) == -1) {
            perror("socket error thread exit.\n");
            break;
        }
        
        /*  callback  */
        if (client->read_filter!=NULL && client->rbuf->buf_len>0)
            client->read_filter(client);

        if (client->wbuf->buf_len > 0) {
            len = send(client->fd, (const char *)client->wbuf->buf_pos, client->wbuf->buf_len, 0);
            if (len == -1) {
                perror("socket error thread exit.\n");
                break;
            }
            buf_del(client->wbuf, client->wbuf->buf_len);
        }

        usleep(50);

    } // end while

	if (client->fd) {
		sk_close(client->fd);
		client->fd = 0;
	}
}

static int _client_conn(char *ip, int port)
{
    int sock_fd=0;

    sk_wsa_init(); 
    sock_fd = sk_connect(ip, port);
    if (sock_fd == -1) {
        return -1;
    }

    if (sk_nonblock(sock_fd) == -1)
        perror("conn nonblock"); 

    return sock_fd;
}


struct client_s* client_init()
{
	struct client_s *client = NULL;

	do {

		client = (struct client_s*)calloc(1, sizeof(struct client_s));
		if (client == NULL) break;

		client->rbuf = buf_calloc(32*1024*1024);
		if (client->rbuf == NULL) break;
		client->wbuf = buf_calloc(1024*1024);
		if (client->wbuf == NULL) break;

		client->rb = rb_calloc(1024);
		if (client->rb == NULL) break;

		client->rb_ext = rb_calloc(32);
		if (client->rb_ext == NULL) break;

		return client;
	} while(0);

	sk_wsa_release();
	if (client->rb_ext) rb_free(client->rb_ext);
	if (client->rb) rb_free(client->rb);
	if (client->rbuf) buf_free(client->rbuf);
	if (client->wbuf) buf_free(client->wbuf);
	if (client) free(client);
	client = NULL;

	return NULL;
}


int client_conn(struct client_s* client, char *ip, int port)
{
    pthread_attr_t attr;
    pthread_t tid;

	do {
		client->fd = _client_conn(ip, port);
		if (client->fd == -1) break;

		strcpy(client->ip, ip);
		client->port = port;

        /* pthread create */
        if (pthread_attr_init(&attr) !=0) {
            perror("client::client_open pthread_attr_init err");
        }
        if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) !=0) {
            perror("client::client_open pthread_attr_setdetachstate err");
        }   
        if (pthread_create(&tid, NULL, _client_main, client) != 0) break;

        client->tid = tid;

        return client->fd;
    } while(0);

    sk_wsa_release();
    if (client->fd) { 
		sk_close(client->fd);
		client->fd = 0;
	}

    return -1;
}

void client_close(struct client_s* client){
    sk_wsa_release();
    if (client->fd) { 
		sk_close(client->fd);
		client->fd = 0;
	}
    if (client->rb_ext) rb_free(client->rb_ext);
    if (client->rb) rb_free(client->rb);
    if (client->rbuf) buf_free(client->rbuf);
    if (client->wbuf) buf_free(client->wbuf);
    if (client) free(client);
    client = NULL;
}




