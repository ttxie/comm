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



//#define ngx_signal_helper(n)     SIG##n
//#define JOIN(x,y) x->y
#define JOIN(x,y) x##y
#define STR(x) #x

struct ping_s{
};

struct body_s {
        int8_t a0;
        uint16_t a1;
        uint8_t a2;
        uint32_t a3;
        uint32_t a4;
        uint32_t a5;
        uint32_t a6;
        uint32_t a7;
        uint32_t a8;
        uint32_t a9;
        uint32_t a10;
        uint32_t a11;
        uint32_t a12;
        uint32_t a13;
        uint16_t a14;
        uint8_t a15;
        uint64_t a16;
        u_char  a17[10];
        u_char  a18[8];
};



/*
有N行，每行2列
需要加入到网路数据包的字段说明：
第一列：结构字段偏移量，第二列：结构字段长度 字符串指针用0x00作区分, char[]用0xFFFF区分

encode的字段顺序已数组为准。

*/
const size_t struct_t[][2] = {
        {offsetof(struct body_s, a0), PACK_SINT8},
        {offsetof(struct body_s, a1), PACK_UINT16},
        {offsetof(struct body_s, a2), PACK_UINT8},
        {offsetof(struct body_s, a3), PACK_UINT32},
        {offsetof(struct body_s, a4), PACK_UINT32},
        {offsetof(struct body_s, a5), PACK_UINT32},
        {offsetof(struct body_s, a6), PACK_UINT32},
        {offsetof(struct body_s, a7), PACK_UINT32},
        {offsetof(struct body_s, a8), PACK_UINT32},
        {offsetof(struct body_s, a9), PACK_UINT32},
        {offsetof(struct body_s, a10), PACK_UINT32},
        {offsetof(struct body_s, a11), PACK_UINT32},
        {offsetof(struct body_s, a12), PACK_UINT32},
        {offsetof(struct body_s, a13), PACK_UINT32},
        {offsetof(struct body_s, a14), PACK_UINT16},
        {offsetof(struct body_s, a15), PACK_UINT8},
        {offsetof(struct body_s, a16), PACK_UINT64},
        {offsetof(struct body_s, a17), PACK_STRING},
        {offsetof(struct body_s, a18), PACK_STRING}
};



void body_handler(struct head_s *head, struct body_s *body)
{
        printf("len:%u,  cmdid: %x, seq:%x, uid:%x, aid:%x\n", head->len, head->cmdid, head->seq, head->uid, head->aid);
        printf("a0:%d\n", body->a0);
        printf("a1:%d\n", body->a1);
        printf("a2:%d\n", body->a2);
        printf("a3:%d\n", body->a3);
        printf("a4:%d\n", body->a4);
        printf("a5:%d\n", body->a5);
        printf("a6:%d\n", body->a6);
        printf("a7:%d\n", body->a7);
        printf("a8:%d\n", body->a8);
        printf("a9:%d\n", body->a9);
        printf("a10:%d\n", body->a10);
        printf("a11:%d\n", body->a11);
        printf("a12:%d\n", body->a12);
        printf("a13:%d\n", body->a13);
        printf("a14:%d\n", body->a14);
        printf("a15:%d\n", body->a15);
        printf("a16:%llx\n", body->a16);
        printf("a17:%s, %u\n", body->a17, (unsigned)strlen((const char*)body->a17));
/*
        if (body->a17 != NULL)
                printf("a17:%s \n", (body->a17));
*/
        printf("a18:%s, %u\n", body->a18, (unsigned)strlen((const char*)body->a18));
        
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
        uint64_t start, end;
        
        bod = (struct body_s*)calloc(PACKET_SIZE, sizeof(struct body_s));
        out = (struct body_s*)calloc(1, sizeof(struct body_s));

        printf("boy size:%d\n", sizeof(struct body_s));
        bod[0].a0 = -16;
        bod[0].a1 = 1024;
        bod[0].a2 = 6;
        bod[0].a16 = 0x0100007F;
        
        //bod->a17 = (u_char*)malloc(10);
        strcpy((char*)bod[0].a17, "TT");
        strcpy((char*)bod[0].a18, "abcdef");
        
        bod[1].a0 = -20;
        bod[1].a1 = 1023;
        bod[1].a2 = 8;
        bod[1].a16 = 0x01000064;
        
        //bod->a17 = (u_char*)malloc(10);
        strcpy((char*)bod[1].a17, "MMMM");
        strcpy((char*)bod[1].a18, "abcderefdffs");
        
        printf("num:%lld, zigzag:%lld\n", (int64_t)num, zigzag((int64_t)num));

        //hexdump(bod, 128*2);
        for(;i<PACKET_SIZE;i++)
        {
        printf("a0:%d\n",  bod[i].a0);
        printf("a1:%d\n",  bod[i].a1);
        printf("a2:%d\n",  bod[i].a2);
        printf("a3:%d\n",  bod[i].a3);
        printf("a4:%d\n",  bod[i].a4);
        printf("a5:%d\n",  bod[i].a5);
        printf("a6:%d\n",  bod[i].a6);
        printf("a7:%d\n",  bod[i].a7);
        printf("a8:%d\n",  bod[i].a8);
        printf("a9:%d\n",  bod[i].a9);
        printf("a10:%d\n", bod[i].a10);
        printf("a11:%d\n", bod[i].a11);
        printf("a12:%d\n", bod[i].a12);
        printf("a13:%d\n", bod[i].a13);
        printf("a14:%d\n", bod[i].a14);
        printf("a15:%d\n", bod[i].a15);
        printf("a16:%llx\n", bod[i].a16);

        
        printf("a17:%s \n", (bod[i].a17));
        printf("a18:%s, %u\n", bod[i].a18, (unsigned)strlen((const char*)bod[i].a18));
        printf("**************************\n");
      };

        head = (struct head_s*)calloc(1, sizeof(struct head_s)); 
        if (head == NULL) return 0;
        
        head->cmdid = 0xF001;
        head->seq = 0x10001;
        head->uid = 0x1000;
        head->aid = 0x0001;

        buf = buf_calloc(1024);
        start = tm64();
        len = enpacket(head, (u_char*)bod, PACKET_SIZE, sizeof(struct body_s), struct_t, ARR_SIZE(struct_t), buf);
        end = tm64();
        printf("packet buf:%p, len:%d, time:%d\n", buf->buf_pos, (int)buf->buf_len, end-start); 
        //hexdump((u_char *)buf->buf_pos, buf->buf_len);        
        
        memset(head, 0, sizeof(struct head_s));
        len = dehead(buf, head);
        if (len==-1) {
                printf("dehead errno:%d, errstr:%s\n", errno, strerror(errno));
                exit(1);
        }
        printf("cmdid:%x\n", head->cmdid); 
        switch (head->cmdid){
        case 0xF001:
                //out->a17 = (u_char*)calloc(1,10);
                da = darray_calloc(1, sizeof(*bod));
                
                start = tm64();
                if (depacket_da(buf, struct_t, ARR_SIZE(struct_t), da) == -1)
                        perror("ERR DEBODY\n");
                end = tm64();
                printf("da used %d, time:%d\n",da->da_used, end-start);
                body_handler(head, (struct body_s*)da->da_values);
                body_handler(head, (struct body_s*)(da->da_values+da->da_size));

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

