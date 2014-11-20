#include <math.h>
#include <assert.h>
#include "utl.h"
#include "sk.h"


int main()
{
        uint32_t ip;
        char ipc[25];
        char *line = (char*)calloc(1,64), *p=NULL;
//        char line[64];
        FILE *fl = fopen("tt.ini", "r");

assert(p);
        ip = inet_addr("210.211.212.213");
        printf("now:%u\n", (unsigned)time(NULL));
        printf("ip:%s,ts:%lld\n", sk_ntoa(ip), tm64());
        
	strcpy(ipc, "127.0.0.1:1024 ");
	p = strsub(ipc, 10, 4);
	hexdump(p, 16);
	printf("ip:%s, strlen(p):%d\n", p, (int)(strlen(p)));
	free(p);



        fgets(line, 96, fl);

        //strcliney(line, " \n\tabcd \n\n\r");
        //hexdump((u_char*)line, 16);

        printf("line:%s\n", trim(line));
        printf("line:%p\n", line);
        //strcliney(line, trim(line));
        //hexdump((u_char*)line, 16);

        strupper(line);
        printf("len:%u,trim(line):%s\n",(unsigned)strlen(line), line);
        strlower(line);
        printf("len:%u,trim(line):%s\n",(unsigned)strlen(line), line);
        
        free(line);
        return 0;

}
