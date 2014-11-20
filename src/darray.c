/*
 * darray.c
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>

#include "darray.h"


struct darray_s* darray_calloc(uint32_t number, uint32_t size)
{
        struct darray_s *da;

        da = (struct darray_s *)calloc(1, sizeof(*da));
        if (da == NULL) return NULL;

        da->da_values = (u_char*)calloc(1, number*size);
        if (da->da_values == NULL) {
                free(da);
                return NULL;
        }

        da->da_nelts = number;
        da->da_size = size; 
        da->da_used = 0;
 
        return da; 
}


void darray_free(struct darray_s *da)
{
        free(da->da_values);
        free(da);        
}

/*      */
void * darray_pushback(struct darray_s *da)
{
        u_char *old;
        size_t newsize;

        /* has no element */
        if (da->da_used == da->da_nelts) {
                /*
                old = da->da_values;
                da->da_values = (u_char *)calloc(1, 2*da->da_nelts*da->da_size);
                if (da->da_values == NULL) return NULL;

                memmove(da->da_values, old, da->da_size*da->da_nelts);
                free(old);
                */
                newsize = fitsize(2*da->da_nelts*da->da_size);
                da->da_values = (u_char *)realloc(da->da_values, newsize);
                da->da_nelts = newsize/da->da_size;
        }
        
        da->da_used++;

        return da->da_values + (da->da_used-1)*da->da_size; 

}



uint32_t darray_count(struct darray_s *da) 
{
        return da->da_used;
}

