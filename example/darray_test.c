
/*darray*/
#include <stdio.h>

#include "file.h"
#include "darray.h"
#include "utl.h"

int main(int argc, char **argv)
{
        int n=0;
	int *a;
        struct darray_s *da = darray_calloc(3, 4);
        int *p = darray_pushback(da);
        *p = 0x1234;
        p = darray_pushback(da);
        *p = 0x5678;

        p = darray_pushback(da);
        *p = 0x5678;

        hexdump(da, 32);
        printf("\n");
        p = darray_pushback(da);
        *p = 0x5678;

        p = darray_pushback(da);
        *p = 0x5678;

        hexdump(da, 32);
        hexdump(da->da_values, 56);

        a  = da->da_values;
        for(n=0; n<darray_count(da); n++)
                printf("%d:%x,\n", n, a[n]);


        
        printf("file line count:%d\n", linecount("bl_new.txt")); 
        
        darray_free(da);

        return 0;
}

