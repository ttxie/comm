#include "hash.h"

int main()
{
        struct hash_s *hash;
        int n = 123, m=123;
        int *p;

        hash = (struct hash_s*)hash_create(10);
        if (hash==NULL) perror("hash create error.");
        
        hash_insert(hash,&n,&n,1); 

        
        p = hash_search(hash, &m);
        printf("value:%d\n", *p);

        hash_free_f(hash, 0x00); 
}

