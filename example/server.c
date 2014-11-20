#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stddef.h>

#include "sk.h"
#include "buf.h"
#include "packet.h"
#include "crc16.h"
#include "utl.h"



//#define ngx_signal_helper(n)     SIG##n
//#define JOIN(x,y) x->y
#define JOIN(x,y) x##y
#define STR(x) #x

struct login_s {
        char acct[30];
        char passwd[50];
        char version[8];
        char imsi[18];
        char imei[18];
        char ua[30];
        uint32_t ts;
        uint8_t net; //0: 3G  1:wifi 
};

/*
有N行，每行2列
需要加入到网路数据包的字段说明：
第一列：结构字段偏移量，第二列：结构字段长度 字符串指针用0x00作区分, char[]用0xFFFF区分

encode的字段顺序已数组为准。

*/
const size_t login_arr[][2] = {
        {offsetof(struct login_s, acct), PACK_STRING},
        {offsetof(struct login_s, passwd), PACK_STRING},
        {offsetof(struct login_s, version), PACK_STRING},
        {offsetof(struct login_s, imsi), PACK_STRING},
        {offsetof(struct login_s, imei), PACK_STRING},
        {offsetof(struct login_s, ua), PACK_STRING},
        {offsetof(struct login_s, ts), PACK_UINT32},
        {offsetof(struct login_s, net), PACK_UINT8}
};





struct loginre_s {
        char err[5];
        char sid[40];
        uint32_t mid;
        char ip[16];
        uint16_t port;
};

/*
有N行，每行2列
需要加入到网路数据包的字段说明：
第一列：结构字段偏移量，第二列：结构字段长度 字符串指针用0x00作区分, char[]用0xFFFF区分

encode的字段顺序已数组为准。

*/
const size_t loginre_arr[][2] = {
        {offsetof(struct loginre_s, err), PACK_STRING},
        {offsetof(struct loginre_s, sid), PACK_STRING},
        {offsetof(struct loginre_s, mid), PACK_UINT32},
        {offsetof(struct loginre_s, ip), PACK_STRING},
        {offsetof(struct loginre_s, port), PACK_UINT16}
};


void login_handler(struct head_s *head, struct login_s *login, struct buf_s *wbuf)
{
        size_t len=0;
        struct loginre_s *loginre; 

        printf("len:%d,  cmdid: %d, uid:%x, aid:%x\n", head->len, head->cmdid, head->uid, head->aid);
        printf("acct:%s\n", login->acct);
        printf("passwd:%s\n", login->passwd);
        printf("imsi:%s\n", login->imsi);
        printf("imei:%s\n", login->imei);
        printf("ua:%s\n", login->ua);
        printf("version:%s\n", login->version);
        printf("ts:%d\n", login->ts);
        printf("net:%d\n", login->net);
        
        loginre = (struct loginre_s*)calloc(1, sizeof(struct loginre_s));
        strcpy(loginre->err, "E000");
        strcpy(loginre->sid, "SID9812319287312");
        loginre->mid = 0x827FAB;
        strcpy(loginre->ip, "192.168.0.1");
        loginre->port = 80;
        

        memset(head, 0, sizeof(struct head_s));
        head->cmdid = 0xF001;
        head->seq = 0x01;
        head->uid = 0x10000;
        head->aid = 0x0001;

        len = enpacket(head, (u_char*)loginre, 1, sizeof(struct loginre_s), loginre_arr, ARR_SIZE(loginre_arr), wbuf);
        free(loginre);
        
        printf("WBUF:\n");
        hexdump(wbuf->buf_pos, 60);
        
}


int main_handler(struct buf_s *rbuf, struct buf_s *wbuf)
{
        struct head_s *head;
        ssize_t len;
        u_char *msg;
	struct login_s *login;

        head = (struct head_s*)calloc(1, sizeof(struct head_s));
        //if (head == NULL) return -1;

        printf("buf_len:%d\n", rbuf->buf_len);
        hexdump(rbuf->buf_pos, rbuf->buf_len);

        len = dehead(rbuf, head);
        if (len == -1) {
                printf("package data err.\n");
                //错误处理返回
                return -1;
        }

        switch(head->cmdid) {
        case 0x0001: {
                login = (struct login_s*)calloc(1, sizeof(struct login_s));
                if (depacket(rbuf, login_arr, ARR_SIZE(login_arr), (u_char*)login) != -1)
                        login_handler(head, login, wbuf);
                free(login);
                break;
        } 
        default:

                break;
        }
        
        hexdump(rbuf->buf_pos, len);
        buf_del(rbuf, len); 
        hexdump(rbuf->buf_pos, len);

}


int main()
{
        struct buf_s *rbuf, *wbuf;
        int conn, sfd;
        struct sockaddr_in *addr_in;
        struct sockaddr_in addr_conn;
        socklen_t len;
        ssize_t result;
       

        addr_in = (struct sockaddr_in*)calloc(1, sizeof(struct sockaddr_in));
        if (addr_in == NULL) {
                printf("server_init: calloc sockaddr_in err. errno:%d, errstr:%s\n",
                                errno, strerror(errno));
                exit(1);
        }
        addr_in->sin_family = AF_INET;
        addr_in->sin_port = htons(1024);
        addr_in->sin_addr.s_addr = inet_addr("211.154.151.73");

        sfd = sk_listen((struct sockaddr*)addr_in, sizeof(struct sockaddr_in));
        if (sfd == -1)     {
                printf("server_init: sk_listen err. errno:%d, errstr:%s\n",
                                errno, strerror(errno));
                exit(1);
        }
        
        len = sizeof(struct sockaddr_in);
        conn = accept(sfd, (struct sockaddr*)&addr_conn, &len);
        if (conn == -1) {
               printf("accept err:%d, %s\n", errno, strerror(errno)); 
        }

        if (sk_nonblock(conn) == -1)
                perror("conn nonblock");

        rbuf = buf_calloc(1024);
        wbuf = buf_calloc(1024);
        if (rbuf ==NULL || wbuf ==NULL) return -1;

        while (1) {
                char buf[512];
                result = recv(conn, buf, 512, 0);
                printf("recv len:%d\n", (int)result);
                if (result == -1) {
                        if ((errno == EAGAIN || errno == EWOULDBLOCK) && rbuf->buf_len > 0)    /*EPOLLET模式*/
                                break;

                        if ((errno == EAGAIN || errno == EWOULDBLOCK) && rbuf->buf_len == 0)    /*EPOLLET模式*/
                                continue;

                        printf("read_handler, errno:%d, errstr:%s\n", errno, strerror(errno));
                        if (errno == EINTR)
                                continue;

                        /*断开连接, 清数据*/
                        sk_close(conn);
                        return 1;
                }else if (len ==0 ) {
                        printf("connection close\n");
                        /*断开连接, 清数据*/
                        sk_close(conn);
                        return 0;
                }
                buf_append(rbuf, (u_char*)buf, result);
                //log_printf(svr->log, "read_handler, fd:%d, size:%d, str:%s\n", fd, size, buf);
        }

        main_handler(rbuf, wbuf);       
        send(conn, wbuf->buf_pos, wbuf->buf_len, 0);
        sk_close(conn);

        printf("exit 0\n");
        return 0;
}


