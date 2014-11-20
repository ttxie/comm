#include <pthread.h>
#include <string.h>
#include "utl.h"
#include "ringbuf.h"

int main()
{
    struct ringbuf_s *rb = rb_calloc(5);
    int i=0;

    for (i=0; i<9; i++) {
        char *p = (char*)calloc(1,6);
        sprintf(p, "%d", i);
        rb_push(rb, p);
    }

    char *re;
    while (re = (char *)rb_pop(rb)) {
        printf("re:%s\n", re);
        free(re);
    }
    
    rb_free(rb);
}

