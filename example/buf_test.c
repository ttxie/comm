#include <math.h>
#include <assert.h>
#include "utl.h"
#include "buf.h"
#include "bitset.h"


int main()
{
    struct buf_s *buf;
    int n=0, m=0;
    uint64_t start, end;
    size_t s;
    u_char varint[10];
    struct bitset_s *fmap;


    start = tm64();
    for(n=0; n<10000; n++) {
        fmap = bitset_create(20);
        buf = buf_calloc(n%256+256);
        for(m=0; m<10; m++) {
            s = utov(n+m, varint);
            buf_append(buf, varint, s);
            bitset_set(fmap, m);
        }

        for(m=0; m<10; m++) {
            buf_del_unlock(buf, s);
        }
        buf_free(buf);
        bitset_free(fmap);
    }

    end = tm64();
    printf("buf time:%d\n", end-start);
    



    return 0;
}
