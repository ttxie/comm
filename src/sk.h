/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 *
 * #LINUX -> WIN32:
 * 
 * 1. has no ssize_t
 * 2. must const
 * 3. win socket head file dup err between <winsock.h> and <winsock2.h> 
 *       a. include first <winsock2.h> then <windows.h>
 *       b. define WIN32_LEAN_AND_MEAN
 *       c. modify <windows.h> change winsock.h to winsock2.h
 * 4. has no inet_ntop, change to inet_ntoa
 * 5. WSA errno need WSAGetLastError to get errno, and the errno set diffrent linux errno
 * 6. have no close, ioctl,  but have closesocket, ioctlsocket
 * 7. WSA need init and release. WSAStartup(), WSACleanup()
 * 8. macro _WIN32, __linux__ reference to: http://sourceforge.net/p/predef/wiki/OperatingSystems/
 * 9. mmap, thread and gettimeofday,
 *
 */
 
#ifndef _COMM_SK_H_
#define _COMM_SK_H_

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN 

#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define SHUT_RD         0x00
#define SHUT_WR         0x01
#define SHUT_RDWR	0x02

typedef int socklen_t;

#else

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#endif

#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

#include "utl.h"

/*
TODO:
tcp_max_syn_backlog其实也容易测，找个syn的dos工具应该就行。
backlog其实也容易测，只要listen不accpet,这个backlog就会增加了。
*/

#define BACKLOG 65536



/*mini ipv4 addr structure*/
struct sk_inaddr_s {
        uint32_t        ip;     /* ip address,  ip = inet_addr() */
        uint16_t        port;   /* port number, port = htons() */
}; 


/*C++支持*/
#ifdef __cplusplus
extern "C" {
#endif

/*
just _WIN32 need init to call sk_wsa_init/sk_wsa_release
return 0 on success and -1 on fail
*/
int sk_wsa_init();
int sk_wsa_release();

/*
 * 建立网络连接
 * 连接成功则返回socket文件描述符
 * 错误返回-1，同时设置错误代码errno
*/
int sk_connect(const char* ip, short port);


/*
 * 域名解析
 */
char* sk_name(char *dst, int len, const char *src);

/*
 * 以非阻塞方式建立网络连接
 * 连接成功则返回socket文件描述符
 * 错误返回-1，同时设置错误代码errno
*/
int sk_nonblock_connect(const char* ip, short port, int *fd);


/*
 *
 */
int sk_unix_connect();


/*
 * 指定本地IP建立网络连接
 * 连接成功则返回socket文件描述符
 * 错误返回-1，同时设置错误代码errno
 */
int sk_bind_connect(const char* ip, short port, const char* local_ip);


/*
 * 服务器绑定端口，开启监听
 * 监听成功则返回listen文件描述符
 * 错误返回-1，同时设置错误代码errno
 */
int sk_listen(struct sockaddr *addr, int len);


/*
 *
 */
int sk_unix_listen();


/*
 * 获取连接
 * 成功返回连接描述符，失败返回-1,同时设置错误代码errno

int sk_accept(int sfd, char *ip, int *port);
 */
 
 
/*
 * 关闭网络连接
 * 成功返回0
 * 错误返回-1，同时设置错误代码errno
 */
int sk_close(int fd);



/*强制断开连接，同时返回RST给到对方*/
int sk_rst(int fd);



/*
 *查询接收缓冲区中待处理数据量
 *返回缓冲区字节数
 */
int sk_peek_recvbuf(int fd);


/*
 * 允许不同IP绑定同一个端口
 * 允许连接在监听不在的时候，应可以重新绑定
 */
int sk_reuseaddr(int fd);


/*
        测试发现，关闭nagle效果更差,未出现200ms的延时
        不建议关闭
*/
int sk_nonagle(int fd);

/*
 * 设置为非阻塞
 */
int sk_nonblock(int fd);


/*
 * 设置为阻塞
 */
int sk_block(int fd);


/*
 *  get peeraddr, localaddr by socket fd
 */
int sk_peeraddr(int fd, struct sk_inaddr_s *peer);
int sk_localaddr(int fd, struct sk_inaddr_s *local);




/*
uint32 to ip address
@ip unit32 ipaddress

return ip string like 192.168.1.1
*/
char* sk_ntoa(uint32_t ip);




/*C++支持*/
#ifdef __cplusplus
}
#endif



#endif

