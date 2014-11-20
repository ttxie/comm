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
    uint64_t a1;
    uint64_t a2;
    uint64_t a3;
    uint64_t a4;
    uint64_t a5;
    uint64_t a6;
    uint64_t a7;
    uint64_t a8;
    uint64_t a9;
    uint64_t a10;
    uint64_t a11;    
    uint64_t a12;
    uint64_t a13;
    uint64_t a14;
};



/*
有N行，每行2列
需要加入到网路数据包的字段说明：
第一列：结构字段偏移量，第二列：结构字段长度 字符串指针用0x00作区分, char[]用0xFFFF区分

encode的字段顺序已数组为准。

*/
const size_t struct_t[][2] = {
        {offsetof(struct body_s, a1), PACK_UINT64},
        {offsetof(struct body_s, a2), PACK_UINT64},
        {offsetof(struct body_s, a3), PACK_UINT64},
        {offsetof(struct body_s, a4), PACK_UINT64},
        {offsetof(struct body_s, a5), PACK_UINT64},
        {offsetof(struct body_s, a6), PACK_UINT64},
        {offsetof(struct body_s, a7), PACK_UINT64},
        {offsetof(struct body_s, a8), PACK_UINT64},
        {offsetof(struct body_s, a9), PACK_UINT64},
        {offsetof(struct body_s, a10), PACK_UINT64},
        {offsetof(struct body_s, a11), PACK_UINT64},
        {offsetof(struct body_s, a12), PACK_UINT64},
        {offsetof(struct body_s, a13), PACK_UINT64},
        {offsetof(struct body_s, a14), PACK_UINT64},
};



void body_handler(struct head_s *head, struct body_s *body)
{
        printf("len:%u,  cmdid: %x, seq:%x, uid:%x, aid:%x\n", head->len, head->cmdid, head->seq, head->uid, head->aid);
        printf("a1:%llu\n", body->a1);
        printf("a2:%llu\n", body->a2);
        printf("a3:%llu\n", body->a3);
        printf("a4:%llu\n", body->a4);
        printf("a5:%llu\n", body->a5);
        printf("a6:%llu\n", body->a6);
        printf("a7:%llu\n", body->a7);
        printf("a8:%llu\n", body->a8);
        printf("a9:%llu\n", body->a9);
        printf("a10:%llu\n", body->a10);
        printf("a11:%llu\n", body->a11);
        printf("a12:%llu\n", body->a12);
        printf("a13:%llu\n", body->a13);
        printf("a14:%llu\n", body->a14);
}


#define PACKET_SIZE 1


int main()
{
        //struct body_s *bod, *out;
        struct buf_s *buf;
        int n = 0,i=0;
        ssize_t len = 0;
        uint16_t cmdid = 0;
        int32_t num=-16;
        struct darray_s *da;
        struct head_s *head;
        uint64_t start, end;
        struct body_s body_t;
        
        struct body_s bod = {0};
        struct body_s out = {0};

        printf("boy size:%d\n", sizeof(struct body_s));
        bod.a1 = 0xFFFFFFFFFFFFFFFF;
        bod.a2 = 0xFFFFFFFFFFFFFFFF;
        bod.a3 = 0xFFFFFFFFFFFFFFFF;
        bod.a4 = 0xFFFFFFFFFFFFFFFF;
        bod.a5 = 0xFFFFFFFFFFFFFFFF;
        bod.a6 = 0xFFFFFFFFFFFFFFFF;
        bod.a7 = 0xFFFFFFFFFFFFFFFF;
        bod.a8 = 0xFFFFFFFFFFFFFFFF;
        bod.a9 = 0xFFFFFFFFFFFFFFFF;
        bod.a10 = 0xFFFFFFFFFFFFFFFF;
        bod.a11 = 0xFFFFFFFFFFFFFFFF;
        bod.a12 = 0xFFFFFFFFFFFFFFFF;
        bod.a13 = 0xFFFFFFFFFFFFFFFF;
        bod.a14 = 0xFFFFFFFFFFFFFFFF;
        
        printf("num:%lld, zigzag:%lld\n", (int64_t)num, zigzag((int64_t)num));

        //hexdump(bod, 128*2);
        //for(;i<PACKET_SIZE;i++)
        //{
        printf("a1:%llu\n",  bod.a1);
        printf("a2:%llu\n",  bod.a2);
        printf("a3:%llu\n",  bod.a3);
        printf("a4:%llu\n",  bod.a4);
        printf("a5:%llu\n",  bod.a5);
        printf("a6:%llu\n",  bod.a6);
        printf("a7:%llu\n",  bod.a7);
        printf("a8:%llu\n",  bod.a8);
        printf("a9:%llu\n",  bod.a9);
        printf("a10:%llu\n", bod.a10);
        printf("a11:%llu\n", bod.a11);
        printf("a12:%llu\n", bod.a12);
        printf("a13:%llu\n", bod.a13);
        printf("a14:%llu\n", bod.a14);
        
        printf("**************************\n");
      //};

        head = (struct head_s*)calloc(1, sizeof(struct head_s)); 
        if (head == NULL) return 0;
        
        head->cmdid = 0xF001;
        head->seq = 0x10001;
        head->uid = 0x1000;
        head->aid = 0x0001;

        buf = buf_calloc(1024);
        start = tm64();
        len = enpacket(head, (u_char*)&bod, 1, sizeof(struct body_s), struct_t, ARR_SIZE(struct_t), buf);
        end = tm64();
        printf("packet buf:%p, len:%d, time:%d\n", buf->buf_pos, (int)buf->buf_len, end-start); 
        hexdump((u_char *)buf->buf_pos, buf->buf_len);
        
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
//                struct body_s body_t;
                
                start = tm64();
                if (depacket(buf, struct_t, ARR_SIZE(struct_t), (u_char*)&out) == -1)
                end = tm64();
                body_handler(head, &out);

                break;
        default:
                break;
        }
        
        //free(bod);
        //free(out);

        free(head);
        buf_free(buf);
        
        


}

