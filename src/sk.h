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
tcp_max_syn_backlog��ʵҲ���ײ⣬�Ҹ�syn��dos����Ӧ�þ��С�
backlog��ʵҲ���ײ⣬ֻҪlisten��accpet,���backlog�ͻ������ˡ�
*/

#define BACKLOG 65536



/*mini ipv4 addr structure*/
struct sk_inaddr_s {
        uint32_t        ip;     /* ip address,  ip = inet_addr() */
        uint16_t        port;   /* port number, port = htons() */
}; 


/*C++֧��*/
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
 * ������������
 * ���ӳɹ��򷵻�socket�ļ�������
 * ���󷵻�-1��ͬʱ���ô������errno
*/
int sk_connect(const char* ip, short port);


/*
 * ��������
 */
char* sk_name(char *dst, int len, const char *src);

/*
 * �Է�������ʽ������������
 * ���ӳɹ��򷵻�socket�ļ�������
 * ���󷵻�-1��ͬʱ���ô������errno
*/
int sk_nonblock_connect(const char* ip, short port, int *fd);


/*
 *
 */
int sk_unix_connect();


/*
 * ָ������IP������������
 * ���ӳɹ��򷵻�socket�ļ�������
 * ���󷵻�-1��ͬʱ���ô������errno
 */
int sk_bind_connect(const char* ip, short port, const char* local_ip);


/*
 * �������󶨶˿ڣ���������
 * �����ɹ��򷵻�listen�ļ�������
 * ���󷵻�-1��ͬʱ���ô������errno
 */
int sk_listen(struct sockaddr *addr, int len);


/*
 *
 */
int sk_unix_listen();


/*
 * ��ȡ����
 * �ɹ�����������������ʧ�ܷ���-1,ͬʱ���ô������errno

int sk_accept(int sfd, char *ip, int *port);
 */
 
 
/*
 * �ر���������
 * �ɹ�����0
 * ���󷵻�-1��ͬʱ���ô������errno
 */
int sk_close(int fd);



/*ǿ�ƶϿ����ӣ�ͬʱ����RST�����Է�*/
int sk_rst(int fd);



/*
 *��ѯ���ջ������д�����������
 *���ػ������ֽ���
 */
int sk_peek_recvbuf(int fd);


/*
 * ����ͬIP��ͬһ���˿�
 * ���������ڼ������ڵ�ʱ��Ӧ�������°�
 */
int sk_reuseaddr(int fd);


/*
        ���Է��֣��ر�nagleЧ������,δ����200ms����ʱ
        ������ر�
*/
int sk_nonagle(int fd);

/*
 * ����Ϊ������
 */
int sk_nonblock(int fd);


/*
 * ����Ϊ����
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




/*C++֧��*/
#ifdef __cplusplus
}
#endif



#endif

