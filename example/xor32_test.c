#include "utl.h"
#include "file.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>



int main()
{
        uint32_t factor=0x3FFEFFFF;
        char *a, *b;

        char *fdata;
        int len;

        fdata = filedata("hero.csv", &len);
        if (fdata==NULL) perror("filedata:");
        printf("fdata:%p, len:%d\n", fdata, len);

        a = xor32(fdata, len, factor);
        //printf("dd2:%s\n", a); 

        b = xor32(a, len, factor);
        //printf("dd3:%s\n", b);

        //a = xor32(b, 12, factor);
        //printf("dd4:%s\n", a);

        free(a);free(b);
        return 0;
}


