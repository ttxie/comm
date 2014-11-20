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




int main()
{
        struct buf_s *rbuf, *wbuf;
        u_char buf[1024];
        int fd, result, len;
        struct login_s *login; 
        struct head_s *head;
       

        result = sk_nonblock_connect("218.206.70.146", 70, &fd);
        //fd = sk_connect("218.206.70.146", 80);
        //sk_nonblock(fd);
        printf("fd:%d\n", fd);

        if (result == -1) {
               printf("non connect err:%d, %s\n", errno, strerror(errno)); 
        }

        while (1) {
                len = recv(fd, buf, 512, 0);
                printf("recv, errno:%d, errstr:%s\n", errno, strerror(errno));

                if (len == -1) {
                        printf("recv, errno:%d, errstr:%s\n", errno, strerror(errno));
                        usleep(500*1000);

                        if (errno == EAGAIN)
                                continue;
                        else
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
        }

        printf("exit 0\n");
        return 0;
}




