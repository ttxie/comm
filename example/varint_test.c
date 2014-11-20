
#include <stdint.h>

#include "varint.h"


int main()
{
        uint32_t n=255000, s=0;
        uint64_t m=0;
        /*-128 = 1000 0000(2) */
        int8_t num = -16, nn=0;
        char varint[10];

        printf("num:%u, zigzag:%llu\n", num, zigzag(num));

        hexdump((u_char*)&num, 8);
        num = zigzag(num);
        hexdump((u_char*)&num, 8);
        num = unzigzag(num);
        

        
        hexdump((u_char*)&n, 4);
        utov(n, varint);
        hexdump((u_char*)varint, 10);
        s = vtou(varint, 10, &m);
        hexdump((u_char*)&m, 4);

        printf("n:%llu, size:%u\n", m, s);

        


}

