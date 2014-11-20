/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#include "minheap.h"

static int parent(int index) {
        return (index - 1) >> 1;
}

static int lchild(int index) {
        return (index << 1) + 1;
}

static int rchild(int index) {
        return (index << 1) + 2;
}

minheap_t* minheap_create(size_t size) 
{
        minheap_t* heap = (minheap_t *)calloc(1, sizeof(minheap_t));
        if (heap == NULL) {
                return NULL;
        }

        heap->mh_nodes = (minheap_node_t *)calloc(size, sizeof(minheap_node_t));
        if (heap->mh_nodes == NULL) {
                free(heap);
                return NULL;
        }

        heap->mh_max_size = size;
        heap->mh_curr_size = 0;

        return heap;
}

void minheap_free(minheap_t* heap) 
{
        if (heap == NULL) return;

        if (heap->mh_nodes != NULL) free(heap->mh_nodes);
        if (heap != NULL) free(heap);
}

int minheap_count(const minheap_t* heap) {
        if (heap == NULL) return -1;
        return heap->mh_curr_size;
}

/*

 */
static void swap(minheap_t* heap, int ia, int ib) 
{
        minheap_node_t temp;

        if (heap == NULL) return;
        temp = heap->mh_nodes[ia];
        heap->mh_nodes[ia] = heap->mh_nodes[ib];
        heap->mh_nodes[ib] = temp;
}



/*

 */
static int heapify_up(minheap_t* heap, int index) 
{
        int cur=0;

        if (heap == NULL) return -1;
        while (index > 0 && heap->mh_nodes[parent(index)].mn_weight > heap->mh_nodes[index].mn_weight) {
                swap(heap, parent(index), index);
                cur = index;
                index = parent(index);
        }

        return cur;
}

/*

 */
static int heapify_down(minheap_t* heap, int index) 
{
        if (heap == NULL) return -1;

        while (1) {
                /*index of children*/
                int left = lchild(index);
                int right = rchild(index);

                /*weight of children*/
                uint64_t lweight = left < heap->mh_curr_size ? heap->mh_nodes[left].mn_weight : UINT64_MAX;
                uint64_t rweight = right < heap->mh_curr_size ? heap->mh_nodes[right].mn_weight : UINT64_MAX;

                /*get min weight of children*/
                int sindex = (lweight < rweight) ? left : right;
                uint64_t sweight = (lweight < rweight) ? lweight : rweight;

                /* swap slot*/
                if (heap->mh_nodes[index].mn_weight > sweight) {
                        swap(heap, index, sindex);
                        index = sindex;
                } else {
                        break;
                }
        }
        
        return index;
}

/*

 */
u_char*  minheap_pop(minheap_t* heap) 
{
        u_char *ptr = NULL;

        if (heap == NULL) return NULL;
        if (heap->mh_curr_size==0) return NULL;

        ptr = heap->mh_nodes[0].mn_ptr;

        swap(heap, 0, heap->mh_curr_size - 1);
        (heap->mh_curr_size)--;
        heapify_down(heap, 0);

        return ptr;
}



/*

 */
int minheap_insert(minheap_t* heap, uint64_t weight, u_char* ptr) 
{
        int index;

        if (heap == NULL) return -1;
        if (heap->mh_curr_size == heap->mh_max_size) return -1;

        heap->mh_nodes[heap->mh_curr_size].mn_ptr = ptr;
        heap->mh_nodes[heap->mh_curr_size].mn_weight = weight;

        index = (heap->mh_curr_size)++; /*first = then ++*/

        return heapify_up(heap, index);

}

