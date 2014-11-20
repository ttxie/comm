#include "minheap.h"

int main()
{
        int32_t i; 
        u_char* ptr=NULL; 
        const char *a="a", *b="b", *c="c", *d="d", *e="e", *f="f", *g="g";
        minheap_t* heap = minheap_create(256);
        if (heap == NULL) {
                printf("malloc minheap memory error!\n");
        }
        
        minheap_insert(heap, 34, (u_char *)a);
        printf("index:%d\n", (int)minheap_insert(heap, 25, (u_char*)b));
        minheap_insert(heap, 89, (u_char *)c);
        minheap_insert(heap, 4,  (u_char *)d);
        minheap_insert(heap, 12, (u_char *)e);
        minheap_insert(heap, 25, (u_char *)b);
        minheap_insert(heap, 10, (u_char *)g);
     
         
        for(i=0; i<minheap_count(heap); i++) {
                printf("index:%d, weight:%d, ptr:%s\n", i,
                        heap->mh_nodes[i].mn_weight, (char *)heap->mh_nodes[i].mn_ptr);
        } 
       
        ptr = minheap_pop(heap);
        printf("minheap pop ptr:%p\n", ptr);
        printf("minheap pop ptr:%s\n", ptr);

        for(i=0; i<minheap_count(heap); i++) {
                printf("index:%d, weight:%d, ptr:%s\n", i,
                        heap->mh_nodes[i].mn_weight, (char *)heap->mh_nodes[i].mn_ptr);
        } 
        
        minheap_free(heap);
        
}
