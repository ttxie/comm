#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stddef.h>
#include <arpa/inet.h>

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
第一列：结构字段偏移量，第二列：结构字段长度 字符串指针用0x00作区分, char[]用PACK_STRING区分

encode的字段顺序已数组为准。

*/
const size_t login_schema[][2] = {
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
第一列：结构字段偏移量，第二列：结构字段长度 字符串指针用0x00作区分, char[]用PACK_STRING区分

encode的字段顺序已数组为准。

*/
const size_t loginre_schema[][2] = {
        {offsetof(struct loginre_s, err), PACK_STRING},
        {offsetof(struct loginre_s, sid), PACK_STRING},
        {offsetof(struct loginre_s, mid), PACK_UINT32},
        {offsetof(struct loginre_s, ip), PACK_STRING},
        {offsetof(struct loginre_s, port), PACK_UINT16}
};


void loginre_handler(struct head_s *head, struct loginre_s *loginre, struct buf_s *wbuf)
{
        struct in_addr addr;
        //addr.s_addr = loginre->ip;

        printf("len:%d,  cmdid: %d\n", head->len, head->cmdid);
        printf("err:%s\n", loginre->err);
        printf("sid:%s\n", loginre->sid);
        printf("mid:%x\n", loginre->mid);
        printf("ip:%s\n", loginre->ip);
        printf("port:%d\n", loginre->port);
        
}


int main_handler(struct buf_s *rbuf, struct buf_s *wbuf)
{
        struct head_s *head;
        ssize_t len;
        u_char *msg;

        head = (struct head_s*)calloc(1, sizeof(struct head_s));
        if (head == NULL) return -1;

        hexdump(rbuf->buf_pos, rbuf->buf_len);
        len = dehead(rbuf, head);
        if (len == -1 && errno == EINVAL) {
                printf("package data err.\n");
                //错误处理返回
                return -1;
        }

        switch(head->cmdid) {
        case 0xF001: {
                struct loginre_s *loginre;
                loginre = (struct loginre_s*)calloc(1, sizeof(struct loginre_s));
                if (depacket(rbuf, loginre_schema, ARR_SIZE(loginre_schema), (u_char*)loginre) != -1)
                        loginre_handler(head, loginre, wbuf);
                else 
                        printf("err.\n");
                free(loginre);
                break;
        }
        default:

                break;
        }

        //buf_pull(rbuf, len); 

}


int main()
{
        struct buf_s *rbuf, *wbuf;
        u_char buf[1024];
        int fd;
        int len;
        struct login_s *login; 
        struct head_s *head;
       

        fd = sk_connect("mx.jizhiinfo.net", 1024);
        if (fd == -1) {
               printf("accept err:%d, %s\n", errno, strerror(errno)); 
        }

        if (sk_nonblock(fd) == -1)
                perror("conn nonblock");

        rbuf = buf_calloc(1024);
        wbuf = buf_calloc(1024);
        if (rbuf ==NULL || wbuf ==NULL) return -1;

        login = (struct login_s*)calloc(1, sizeof(struct login_s));
        strcpy(login->acct, "ACCT");
        strcpy(login->passwd, "PASSWD");
        strcpy(login->version, "1.30");
        strcpy(login->imsi, "987654321");
        strcpy(login->imei, "123456789");
        strcpy(login->ua, "SUMSANG");
        login->ts = time(NULL);
        login->net = 0;
        
        head = calloc(1, sizeof(struct head_s));
        if (head == NULL) return 0;

        head->cmdid = 0x0002;
        head->seq = 0x01;
        head->uid = 0x10000;
        head->aid = 0x99;
        memset(buf, 0, 1024);
        len = enpacket(head, (u_char*)login, 1, sizeof(struct login_s),login_schema, ARR_SIZE(login_schema), wbuf);
        free(login);
        
        hexdump(wbuf->buf_pos, wbuf->buf_len);

        send(fd, wbuf->buf_pos, wbuf->buf_len, 0);

        while (1) {
                len = recv(fd, buf, 512, 0);
                printf("recvlen:%d\n", len);
                if (len == -1) {
                        if ((errno == EAGAIN || errno == EWOULDBLOCK) && rbuf->buf_len > 0)    /*EPOLLET模式*/
                                break;

                        if ((errno == EAGAIN || errno == EWOULDBLOCK) && rbuf->buf_len == 0)    /*EPOLLET模式*/
                                continue;

                        printf("read_handler, errno:%d, errstr:%s\n", errno, strerror(errno));
                        if (errno == EINTR)
                                continue;

                        /*断开连接, 清数据*/
                        sk_close(fd);
                        return 1;
                }else if (len ==0 ) {
                        printf("connection close\n");
                        /*断开连接, 清数据*/
                        sk_close(fd);
                        break;
                }
                buf_append(rbuf, buf, len);
                //log_printf(svr->log, "read_handler, fd:%d, size:%d, str:%s\n", fd, size, buf);
        }

        main_handler(rbuf, wbuf);       

        printf("exit 0\n");
        return 0;
}




