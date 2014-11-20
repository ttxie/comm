/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#include "sk.h"


static char ipstr[18]; /*ip string for sk_ntoa(@ip)*/


/*
   just _WIN32 need init to call sk_wsa_init/sk_wsa_release
   return 0 on success and -1 on fail
 */
int sk_wsa_init()
{
#if defined(_WIN32)
        WSADATA wsa;

        if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) { /* MAKEWORD: The highest version of Windows Sockets  */
                errno = WSAGetLastError();
                return -1;
        }
#endif
        return 0;
}

int sk_wsa_release()
{
#if defined(_WIN32)
        if (WSACleanup() != 0) {
                errno = WSAGetLastError();
                return -1;
        }
#endif
        return 0;
}


/*
 * ��IP�˿�
 */
static int sk_bind(int fd, const char* ip, short port)
{
        struct sockaddr_in addr;

        memset(&addr, 0, sizeof(struct sockaddr_in));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip);

        return bind(fd, (struct sockaddr *)&addr, sizeof(addr));
}



/*
 * ��������
 * nonblock 0:Ĭ��������ʽ��������  1:��������ʽ��������
 */
static int sk_conn(const char* ip, short port, short nonblock, int* conn)
{
        char ipsz[40]; /*2001:0DB8:0000:0000:0000:0000:1428:0000*/
        struct sockaddr_in addr;

        memset(&addr, 0, sizeof(struct sockaddr_in));
        *conn = socket(AF_INET, SOCK_STREAM, 0);
        if (*conn == -1) {
#if defined(_WIN32)
                errno = WSAGetLastError();
#endif
                return -1;
        }

        if(nonblock) {
                sk_nonblock(*conn);
        }

        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (!detect_num(ip)) {      /*IP�ж�*/
                if (sk_name(ipsz, sizeof(ipsz), ip) == NULL) { /*����תIP*/
                        return -1; 
                }
                addr.sin_addr.s_addr = inet_addr(ipsz);
        } else
                addr.sin_addr.s_addr = inet_addr(ip);

        if (connect(*conn, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
#if defined(_WIN32)
                errno = WSAGetLastError();
#endif
                /* WSAEINPROGRESS = 10036, WSAEWOULDBLOCK = 10035  at _WIN32*/
                if (errno != EINPROGRESS || errno == 10036 || errno == 10035) { 
                        sk_close(*conn);
                }
                return -1;
        }

        return 0;
}

/*
 * ��������
 */
char* sk_name(char *dst, int len, const char *src)
{
        struct hostent *hname;

        hname = gethostbyname(src);
        if (hname == NULL) {
#if defined(_WIN32)
                errno = WSAGetLastError();
#endif
                return NULL;
        }

#if defined(_WIN32)
        strcpy(dst, inet_ntoa(*(struct in_addr*)hname->h_addr));
        return dst;
#else
        return (char *)inet_ntop(hname->h_addrtype, hname->h_addr, dst, len);
#endif
}


/*��������*/
int sk_connect(const char* ip, short port)
{
        int fd;

        if (sk_conn(ip, port, 0, &fd) == -1) {
                return -1;
        }

        return fd;
}

/*��������ʽ��������*/
int sk_nonblock_connect(const char* ip, short port, int *fd)
{
        return sk_conn(ip, port, 1, fd);
}


/* ָ������IP��connect */
int sk_bind_connect(const char* ip, short port, const char* local_ip)
{
        int fd;
        struct sockaddr_in addr;
        char ipsz[40]; /*2001:0DB8:0000:0000:0000:0000:1428:0000*/

        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1) {
#if defined(_WIN32)
                errno = WSAGetLastError();
#endif
                return -1;
        }

        if (sk_bind(fd, local_ip, 0) == -1) {
#if defined(_WIN32)
                errno = WSAGetLastError();
#endif
                sk_close(fd);
                //close(fd);
                return -1;
        }

        memset(&addr, 0, sizeof(struct sockaddr_in));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (!detect_num(ip)) {
                if (sk_name(ipsz, sizeof(ipsz), ip) == NULL) return -1; 
                addr.sin_addr.s_addr = inet_addr(ipsz);
        } else
                addr.sin_addr.s_addr = inet_addr(ip);
        if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
#if defined(_WIN32)
                errno = WSAGetLastError();
#endif
                sk_close(fd);
                //close(fd);
                return -1;
        }

        return fd;
}


/*
 * �������󶨶˿ڣ���������
 */

int sk_listen(struct sockaddr *addr, int len)
{
        int sfd;

        if (addr == NULL) return -1;

        sfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sfd == -1) {
                return -1;
        }

        sk_reuseaddr(sfd);

        //if (sk_bind(sfd, ip, port) == -1) {	
        if (bind(sfd, addr, len) == -1) {
                //sk_close(sfd);
                return -1;
        }

        if (listen(sfd, BACKLOG) == -1) { /* the magic 511 constant is from nginx */
                sk_close(sfd);
                return -1;
        }
        return sfd;
}

/*��ȡ����
  int sk_accept(int sfd, char *ip, int *port)
  {
  struct sockaddr_in addr;
  int conn, len;

ACCEPT:	
memset(&addr, 0, sizeof(struct sockaddr_in));
len = sizeof(struct sockaddr_in);
conn = accept(sfd, (struct sockaddr *)&addr, &len);

if (ip) strcpy(ip, inet_ntoa(addr.sin_addr));
if (port) *port = ntohs(addr.sin_port);	

if (conn == -1) {
if (errno == EINTR)
goto ACCEPT;
return -1;
}else {
return conn;
}
}
 */

/*
 * ǿ�ƶϿ����ӣ�ͬʱ����RST�����Է�
 */
int sk_rst(int fd)
{
        struct linger lg;
        int result;

        lg.l_onoff = 1;
        lg.l_linger = 0;

#if defined(_WIN32)
        result = setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char*)&lg, sizeof(struct linger));
#else
        result = setsockopt(fd, SOL_SOCKET, SO_LINGER, (const void*)&lg, sizeof(struct linger));
#endif
        if (result == -1) return -1;

#if defined(_WIN32)
        return closesocket(fd);
#else
        return close(fd);
#endif
}


/*
 * ����COW����Ӱ��ر�����
 */
int sk_close(int fd)
{
        return shutdown(fd, SHUT_RDWR);
}



/*
 *��ѯ���ջ������д�����������
 *���ػ������ֽ���
 */
int sk_peek_recvbuf(int fd)
{
        int count;

#if !defined(_WIN32)
        if (ioctl(fd, FIONREAD, &count) == -1) {
#else
        if (ioctlsocket(fd, FIONREAD, (u_long *)&count) == -1) {
                        errno = WSAGetLastError();
#endif
                        return -1;
                }

                return count;
        }




        /*
           ����ͬIP��ͬһ���˿�
           ���������ڼ������ڵ�ʱ��Ӧ�������°�
         */
        int sk_reuseaddr(int fd)
        {
                int on = 1;

#if defined(_WIN32)
                return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));
#else
                return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&on, sizeof(on));
#endif

        }


        /*
           ���Է��֣��ر�nagleЧ������,δ����200ms����ʱ
           ������ر�
         */
        int sk_nonagle(int fd)
        {
                int on = 1;

#if defined(_WIN32)
                return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&on, sizeof(on) );
#else
                return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const void*)&on, sizeof(on) );
#endif

        }

        /*
         * ����Ϊ������
         */
        int sk_nonblock(int fd)
        {
                /*
                 * ioctl(FIONBIO) sets a non-blocking mode with the single syscall
                 * while fcntl(F_SETFL, O_NONBLOCK) needs to learn the current state
                 * using fcntl(F_GETFL).
                 */
                int nb;

                nb = 1;

#if defined(_WIN32)
                if (ioctlsocket(fd, FIONBIO, (u_long *)&nb) == -1) {
                        errno = WSAGetLastError();
                        return -1;
                }
                return 0;
#else
                return ioctl(fd, FIONBIO, &nb);
#endif
        }


        /*
         * ����Ϊ����
         */
        int sk_block(int fd)
        {
                /*
                 * ioctl(FIONBIO) sets a non-blocking mode with the single syscall
                 * while fcntl(F_SETFL, O_NONBLOCK) needs to learn the current state
                 * using fcntl(F_GETFL).
                 */
                int  nb;

                nb = 0;

#if defined(_WIN32)
                if (ioctlsocket(fd, FIONBIO, (u_long *)&nb) == -1) {
                        errno = WSAGetLastError();
                        return -1;
                }
                return 0;
#else
                return ioctl(fd, FIONBIO, &nb);
#endif
        }



/*
 *  get peeraddr, localaddr by socket fd
 */
int sk_peeraddr(int fd, struct sk_inaddr_s *peer)
{
        struct sockaddr_in *in;
        socklen_t len;

        if (peer == NULL) return -1;

        in = (struct sockaddr_in*)calloc(1, sizeof(*in)); 
        if (in == NULL) return -1; 
        len = sizeof(*in); 

        if (getpeername(fd, (struct sockaddr*)in, &len) == 0) {
                peer->ip   = in->sin_addr.s_addr;
                peer->port = in->sin_port;
                free(in); 
                return 0; 
                 
        } else { 
                free(in); 
                return -1; 
        } 
}

int sk_localaddr(int fd, struct sk_inaddr_s *local) 
{ 
        struct sockaddr_in *in; 
        socklen_t len; 
 
        if (local == NULL) return -1;

        in = (struct sockaddr_in*)calloc(1, sizeof(*in)); 
        if (in == NULL) return -1; 
        len = sizeof(*in); 
 
        if (getsockname(fd, (struct sockaddr*)in, &len) == 0) { 
                local->ip   = in->sin_addr.s_addr; 
                local->port = in->sin_port; 
                free(in); 
                return 0; 
                 
        } else { 
                free(in); 
                return -1; 
        } 
 
} 


/*
uint32 to ip address
@ip unit32 ipaddress

return ip string like 192.168.1.1
*/
char* sk_ntoa(uint32_t ip)
{ 
        union {
                uint32_t        ip32;
                uint8_t         ip8[4];
        }ipu;

        ipu.ip32 = ip;
        sprintf(ipstr, "%u.%u.%u.%u", ipu.ip8[0], ipu.ip8[1], ipu.ip8[2], ipu.ip8[3]);
 
        return ipstr;
}


