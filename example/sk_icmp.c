#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <stdlib.h>

#include "checksum.h"
#include "utl.h"
#include "endianness.h"
#include "sk.h"

/*
 TODO: IP_HDRINCL apply example
*/

/*
 * IP header
 */
struct iphdr_s {
#if _LITTLE_ENDIAN
        uint8_t    ihl:4,               /*length of ip header*/
                   version:4;
#elif _BIG_ENDIAN
        uint8_t    version:4,
                   ihl:4;               /*length of ip header*/
#endif
        uint8_t    tos;
        uint16_t   tot_len;             /*total length*/
        uint16_t   id;
        uint16_t   frag_off;
        uint8_t    ttl;                /*time to live*/
        uint8_t    protocol;            /*1:ICMP 6:TCP 7:UDP*/
        uint16_t   check;               /*checksum*/
        uint32_t   saddr;
        uint32_t   daddr;
};

/*
 * ICMP Body
 */
struct icmp_s{
        uint8_t icmp_type;      /*8:echo req 0:echo reply*/
        uint8_t icmp_code;      /*0:ok other wise error */
        uint16_t icmp_check;
        uint16_t icmp_id;        /*identification*/ 
        uint16_t icmp_seq;      /*for seq of ping*/
        uint32_t icmp_origts;
        uint32_t icmp_recvts;
        uint32_t icmp_sendts;
};

/*
 *  timestamp from 00:00 
 *  unit: 0.0001second  = microsecond/100
 */
static uint32_t dayts()
{
        struct timeval now;
        uint32_t sec;
        
        gettimeofday(&now, NULL);
        sec = now.tv_sec % (24*3600);
        
        return sec*10000 + now.tv_usec/100;
}



static void icmp_pack(struct icmp_s *icmp, uint16_t icmp_id, uint16_t icmp_seq)
{

        icmp->icmp_type = 13;    /*8:echo request, 13:ts request*/
        icmp->icmp_code = 0;
        icmp->icmp_id = htons(icmp_id);
        icmp->icmp_check = 0;
        icmp->icmp_seq = htons(icmp_seq);
        icmp->icmp_origts = htonl(dayts());
        icmp->icmp_check = checksum((uint16_t *)icmp, sizeof(*icmp));

}

int main()
{
        int sk, l, difftime=0, on;
        uint16_t seq=0;
        uint32_t recvts, rtt=0;
        struct icmp_s icmp, *reply;
        struct iphdr_s *iphdr;
        struct sockaddr_in addr, addr0, from;
        socklen_t len;
        u_char buf[512];
        
        sk = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
        if (sk == -1) {
                perror("socket create err");
                exit(0);
        }
//        sk_nonblock(sk);

        /* IP_HDRINCL: indicates the application provides the IP header. Applies only to SOCK_RAW sockets.
        on = 0;
        setsockopt(sk, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));
        */

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr("220.112.33.73");
        addr0.sin_family = AF_INET;
        addr0.sin_addr.s_addr = inet_addr("211.154.151.73");

        while(1) {
                icmp_pack(&icmp, getpid(), seq++);
                sendto(sk, &icmp, sizeof(icmp), 0, (struct sockaddr *)&addr0, sizeof(addr0));

                //printf("packsize:%u\n", sizeof(icmp));
                //sendto(sk, &icmp, sizeof(icmp), 0, (struct sockaddr *)&addr, sizeof(addr));

                len = sizeof(from);
                l = recvfrom(sk, buf, 512, 0, (struct sockaddr*)&from, &len);
                recvts = dayts();

                printf("ip:%s\n", inet_ntoa(from.sin_addr));
                hexdump((u_char*)buf, l);

                iphdr = buf;
                printf("ipheader:\tihl:%u, pack length:%u, saddr:%x\n", iphdr->ihl, iphdr->tot_len, iphdr->saddr);
                reply = buf+iphdr->ihl*4;
                printf("icmp:\ticmp_id:%u, icmp_seq:%u\n", ntohs(reply->icmp_id), ntohs(reply->icmp_seq));

                //if (ntohs(reply->icmp_id != getpid())) continue;
                
                printf("recvts:%u, sendts:%u\n", recvts, ntohl(reply->icmp_origts));
                printf("server recvts:%u\n", ntohl(reply->icmp_recvts));

/***rtt, difftime***/
                rtt = recvts-ntohl(reply->icmp_origts);
                difftime = ntohl(reply->icmp_recvts) - (recvts-rtt/2)/10;

                printf("rtt:%u, difftime:%d\n\n", rtt, difftime);

                sleep(1);
        }
        

}

