#include "dstr.h"
#include "utl.h"

int main()
{

        dstr_t ds;
	
	printf("dstr_s sizeof:%d\n", sizeof(struct dstr_s));

	ds = dstrallocf("%d,%lld\taaaaa", 1234, 12345669871123L);
        printf("dstr:%d\t%s\n", dstrsize(ds), ds);

        ds = dstrcpy(ds, "   \t1234567890123123123123123098898890000998811777444444443333322222222");
        printf("dstr:%d\t%s\n", dstrsize(ds), ds);

        ds = dstrcat(ds, "\nabcdefghi\n");
        trim(ds);
        printf("dstr:%d\t%s\n", dstrsize(ds), ds);
                

        dstrfree(ds);
        ds = NULL;
}

