#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stddef.h>

#include "varint.h"
#include "sk.h"
#include "packet.h"
#include "crc16.h"
#include "utl.h"
#include "darray.h"

#define JOIN(x,y) x##y
#define STR(x) #x

struct body_s {
};


void body_handler(struct head_s *head, struct body_s *body)
{
        printf("len:%u,  cmdid: %x, seq:%x, uid:%x, aid:%x\n", head->len, head->cmdid, head->seq, head->uid, head->aid);
}


#define PACKET_SIZE 2


int main()
{
        struct body_s *bod, *out;
        struct buf_s *buf;
        int n = 0,i=0;
        ssize_t len = 0;
        uint16_t cmdid = 0;
        int32_t num=-16;
        struct darray_s *da;
        struct head_s *head;
        
        bod = (struct body_s*)calloc(PACKET_SIZE, sizeof(struct body_s));
        out = (struct body_s*)calloc(1, sizeof(struct body_s));

        printf("boy size:%d\n", sizeof(struct body_s));
        
        printf("num:%lld, zigzag:%lld\n", (int64_t)num, zigzag((int64_t)num));

        head = (struct head_s*)calloc(1, sizeof(struct head_s)); 
        if (head == NULL) return 0;
        
        head->cmdid = 0xF001;
        head->seq = 0x10001;
        head->uid = 0x1000;
        head->aid = 0x0001;

        buf = buf_calloc(1024);
        len = enpacket(head, (u_char*)NULL, 0, 0, NULL, 0, buf);
        printf("packet buf:%p, len:%d\n", buf->buf_pos, (int)buf->buf_len); 
        hexdump((u_char *)buf->buf_pos, buf->buf_len);        
        
        memset(head, 0, sizeof(struct head_s));
        len = dehead(buf, head);
        if (len==-1) {
                printf("errno:%d, errstr:%s\n", errno, strerror(errno));
                exit(1);
        }
        printf("cmdid:%x\n", head->cmdid); 
        switch (head->cmdid){
        case 0xF001:
                //out->a17 = (u_char*)calloc(1,10);
                da = darray_calloc(1, sizeof(*bod));
                
                if (depacket_da(buf, NULL, 0, da) == -1)
                        perror("ERR DEBODY\n");

                printf("da used %d\n",da->da_used);
                body_handler(head, (struct body_s*)da->da_values);

                darray_free(da);
                break;
        default:
                break;
        }
        
        free(bod);
        free(out);

        free(head);
        buf_free(buf);
        
        


}

