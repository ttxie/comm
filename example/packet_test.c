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


struct body_s {
        int32_t a0;
        int32_t a1;
        u_char  a2[32];
};




/*
有N行，每行2列
需要加入到网路数据包的字段说明：
第一列：结构字段偏移量，第二列：结构字段长度 字符串指针用0x00作区分, char[]用0xFFFF区分

encode的字段顺序已数组为准。

*/
const size_t struct_t[][2] = {
        {offsetof(struct body_s, a0), PACK_SINT32},
        {offsetof(struct body_s, a1), PACK_UINT32},
        {offsetof(struct body_s, a2), PACK_STRING}
};



void body_handler(struct head_s *head, struct body_s *body)
{
        printf("len:%u,  cmdid: %x, seq:%x, uid:%x, aid:%x\n", head->len, head->cmdid, head->seq, head->uid, head->aid);
        printf("a0:%d\n", body->a0);
        printf("a1:%d\n", body->a1);
        printf("a2:%s\n", body->a2);
}


#define PACKET_SIZE 1


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
        uint64_t start, end;
        
        bod = (struct body_s*)calloc(PACKET_SIZE, sizeof(struct body_s));
        out = (struct body_s*)calloc(1, sizeof(struct body_s));

        printf("boy size:%d\n", sizeof(struct body_s));
        bod[0].a0 = 123;
        bod[0].a1 = 456;
        strcpy((char*)bod[0].a2, "test string!");

/*
        bod[1].a0 = -20;
        bod[1].a1 = 1023;
        strcpy((char*)bod[1].a2, "MMMM");
*/

        for(;i<PACKET_SIZE;i++)
        {
        printf("a0:%d\n",  bod[i].a0);
        printf("a1:%d\n",  bod[i].a1);
        printf("a2:%s \n", (bod[i].a2));
      };

        head = (struct head_s*)calloc(1, sizeof(struct head_s)); 
        if (head == NULL) return 0;
        
        head->cmdid = 0xF001;
        head->seq = 0x10001;
        head->uid = 0x1000;
        head->aid = 0x0001;

        buf = buf_calloc(64*1000000);
        start = tm64();
        for (n=0; n<1000000; n++) {
            //for(len=0; len<PACKET_SIZE; len++) {
                len = enpacket(head, (u_char*)bod, PACKET_SIZE, sizeof(struct body_s), struct_t, ARR_SIZE(struct_t), buf);
            //}
        }
        end = tm64();
        printf("packet buf:%p, len:%d, time:%d\n", buf->buf_pos, (int)buf->buf_len, end-start); 
        hexdump((u_char *)buf->buf_pos, 128);        
        
        memset(head, 0, sizeof(struct head_s));
        len = dehead(buf, head);
        if (len==-1) {
                printf("errno:%d, errstr:%s\n", errno, strerror(errno));
                exit(1);
        }
        printf("cmdid:%x\n", head->cmdid); 
        switch (head->cmdid){
        case 0xF001:
                da = darray_calloc(1, sizeof(*bod));
                
                start = tm64();
                if (depacket_da(buf, struct_t, ARR_SIZE(struct_t), da) == -1)
                        perror("ERR DEBODY\n");
                end = tm64();
                printf("da used %d, time:%d\n",da->da_used, end-start);
                body_handler(head, (struct body_s*)da->da_values);
//                body_handler(head, (struct body_s*)(da->da_values+da->da_size));

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

