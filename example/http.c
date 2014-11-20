#include "sk.h"
#include "buf.h"
#include "utl.h"

int main()
{
        int conn, len;
        const char *head = "HEAD /img/baidu_sylogo1.gif HTTP/1.1\r\nhost:www.baidu.com\r\nconnection:close\r\n\r\n";
        const char *get = "GET /img/baidu_sylogo1.gif HTTP/1.1\nhost:www.baidu.com\nconnection:keepalive\n\n";
        char buf_recv[256], get_buf[1024];
        struct buf_s *buf=buf_mmap(16*1024, "buf.data");

        conn = sk_connect("www.baidu.com", 80);
        if (conn == -1) {
                perror("connect err..");
                return 1;
        }
        
//        sk_nonblock(conn);
        send(conn, get, strlen(get), 0);
        if (buf == NULL) {
                perror("buf_mmap err");
                return 1;
        }
        printf("file:%s, len:%u\n", buf->buf_file, (unsigned)buf->buf_len);

        while (1) {
                len = recv(conn, buf_recv, sizeof(buf_recv), 0);
                if (len == -1) {
                        perror("recv err...\n");
                        break;
                }
                if (len == 0) {
                        printf("connection close.\n");
                        break; 
                }
                printf("recv len:%d\n", len);
                if (buf_append(buf, (u_char*)buf_recv, len) == -1) {
                        perror("buf_append err.");
                }
        }

				
        //hexdump(buf->buf_pos, buf->buf_len);
        printf("len:%d\n", (int)buf->buf_len);
        buf_pull(buf, (u_char*)get_buf, 1024);
        printf("len:%d\n", (int)buf->buf_len);
        //hexdump((u_char*)get_buf, 1024);
        //hexdump(buf->buf_start, buf->buf_len+1024);

//        printf("buf:%s\n", buf->buf_pos);
        errno = EPIPE;        
        printf("close:%d\n", close(conn)); 
        printf("errno:%d\n", errno);
        
        buf_free(buf);

        return 0;
}

