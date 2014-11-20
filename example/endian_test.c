#include "utl.h"
#include "endianness.h"
#include <arpa/inet.h>

int main()
{
        unsigned long long tt;
        uint32_t bb=0x0A0B0C0D;
        struct in_addr cc;

        tt = 0xFEDCBA9876543210LLU;
        hexdump((u_char*)&tt, 16);
        tt = hton64(tt);
        hexdump((u_char*)&tt, 16);
        
        hexdump((u_char*)&bb, 16);
        bb = htonl(bb);
        hexdump((u_char*)&bb, 16);

        cc.s_addr = inet_addr("10.11.12.13");
        hexdump((u_char*)&cc, 16);
        cc.s_addr = ntohl(cc.s_addr);
        hexdump((u_char*)&cc, 16);

        printf("endian:%c\n", _ENDIAN); 
        if (_BIG_ENDIAN)
                printf("this is big_endian.\n");
        
        if (_LITTLE_ENDIAN)
                printf("this is little_endian.\n");


}

