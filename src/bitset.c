/*
 * Copyright (c) 2013
 * bitset.c
 * Editor: ZhangErjiang
 * Date: 2013-01-18
 * Description:

 * Modify: by ZhangErjiang 2013-01-21 add some function
 * modify by ttxie 20130208: filed bits use u_char instend of of size_t and more optimaze
 for null bitmap of packet.h
 */


#include "bitset.h"


/*
 * 存储单元u_char占用的位数 = 一个char字符占用的位数 * sizeof (u_char)
 */
#define BITSET_BITS (CHAR_BIT * sizeof(u_char)) /*CHAR_BIT = 8*/


/* pos位的掩码，pos位为1的size_t的值 */
#define BITSET_MASK(pos) \
    ( ((u_char)1) << (BITSET_BITS-1 - (pos) % (BITSET_BITS)) )

/*
 *   用多少个size_t来保存size个位 +(BITSET_BITS-1)保证至少有一个size_t来保存 
 *       if pos = 33 and size_t is 4 bytes, then the current size_t =  set->bits[2]
 */
#define BITSET_WORD(set, pos) \
    ( (set)->bits[(pos) / (BITSET_BITS)] )

/* 
 *       compute the number of signed to store size bytes
 *       result:  like 35bit need 2 of signed(4byte*8bit) return 2
 */
#define BITSET_USED(size) \
    ( ((size) + (BITSET_BITS) - 1) / (BITSET_BITS) )


/*
 * 功能：BitSet初始化
 * 参数：
 size: 初始化的位数(单位：size_t)
 * 返回值：bitset结构体
 */
bitset_t *bitset_create(size_t size)
{
    bitset_t *set;

    set = (bitset_t *)calloc(1, sizeof(*set));
    if (set == NULL) return NULL;

    set->bits = (u_char* )calloc(1, BITSET_USED(size) * sizeof(*set->bits));
    if (set->bits == NULL) {
        free(set);
        return NULL;
    }
    set->size = size;

    return set;
}

/*
 * 功能：把BitSet所有位置零
 * @set：bitset结构体
 * return: zero on success or -1 on fail
 */
int bitset_reset(bitset_t *set)
{
    if (set == NULL) return -1;
    memset(set->bits, 0, BITSET_USED(set->size) * sizeof(*set->bits));

    return 0;
}

/*
 * 功能：把BitSet所有位置1
 * @set：bitset结构体
 * return: zero on success or -1 on fail
 */
int bitset_setall(bitset_t *set) 
{
    if (set == NULL) return -1;

    /* padding would be fill 1  */
    memset(set->bits, 1, BITSET_USED(set->size) * sizeof(*set->bits));  

    return 0;
}


/*
 * 功能：释放BitSet
 * 参数：
 set：bitset结构体
 * 返回值：无
 */
void bitset_free(bitset_t *set)
{
    if (set->bits != NULL) free(set->bits);
    if (set != NULL) free(set);
}

/*
 * 功能：将pos位置0
 * 参数：
 set：bitset结构体
pos: sizet_t型整数
 * return: zero on success or -1 on fail
 */
int bitset_unset(bitset_t *set, size_t pos)
{
    if (set == NULL) return -1;
    if (pos > set->size) {
        errno = ESPIPE; /*illegal seek*/
        return -1;
    }

    BITSET_WORD(set, pos) &= ~BITSET_MASK(pos);

    return 0;
}

/*
 * 功能：将pos位置1
 * 参数：
 set：bitset结构体
pos: size_t型整数
 * return: zero on success or -1 on fail
 */
int bitset_set(bitset_t *set, size_t pos)
{
    if (set == NULL) return -1;
    if (pos > set->size) {
        errno = ESPIPE; /*illegal seek*/
        return -1;
    }

    //printf("pos:%u, %d,  mod:%d, mask:%x\n", pos, (pos) / (BITSET_BITS),  (pos) % (BITSET_BITS),  BITSET_MASK(pos));
    BITSET_WORD(set, pos) |= BITSET_MASK(pos);

    return 0;
}

/*
 * 功能：测试pos位是1还是0，是1返回1 否则返回0
 * 参数：
 set：bitset结构体
 pos：size_t型整数
 * 返回值：>0表示pos位为1；返回0表示pos位为0。
 */
int bitset_isset(bitset_t *set, size_t pos)
{
    if (set == NULL) return -1;
    if (pos > set->size) {
        errno = ESPIPE; /*illegal seek*/
        return -1;
    }

    //printf("isset: pos:%d, value:%llu\n", pos, BITSET_WORD(set, pos) & BITSET_MASK(pos) );

    /* must != 0, the return value int would overflow */
    return (BITSET_WORD(set, pos) & BITSET_MASK(pos)) != 0;  
}

/*
 * 功能：返回位图总位数
 * 参数：
 set：bitset结构体
 * 返回值：位图总位数
 */
int bitset_size(bitset_t *set)
{
    if (set == NULL) return -1;

    return set->size;
}

/*
 * 功能：返回设置位为1的总个数
 * 参数：
 set：bitset结构体
 * 返回值：位为1的总个数
 */
int bitset_count(bitset_t *set)
{
    int i;
    int count = 0;

    if (set == NULL) return -1;

    for (i=0; i<(int)set->size; i++) {
        if (bitset_isset(set, i) > 0) {
            count++;	
        } else continue;
    }

    return count;
}

/*
 * 功能：输出位图中的每个位的内容
 * 参数：
 set：bitset结构体
 * 返回值：无
 */
void bitset_print(bitset_t *set) 
{
    int i;

    if (set == NULL) return;

    for(i = 0; i <(int)set->size; ++i)
        printf("%c", (bitset_isset(set, i) ? '1' : '0'));

    printf("\n");	

}

/*
 * 功能：翻转所有位
 * 参数：
 set：bitset结构体
 * return: zero on success or -1 on fail

TODO:Toggling a bit
The XOR operator (^) can be used to toggle a bit.
number ^= 1 << x;
That will toggle bit x.


*/
int bitset_reverse(bitset_t *set)
{
    int i;

    if (set == NULL) return -1;

    for (i=0; i<(int)set->size; i++) {
        if (bitset_isset(set, i) > 0) {
            bitset_unset(set, i);
        } else {
            bitset_set(set, i);
        }
    }
    return 0;
}

/*
 * 功能：翻转pos位
 * 参数：
 set：bitset结构体
pos: 第pos位
 * return: zero on success or -1 on fail
 */
int bitset_bit_reverse(bitset_t *set, size_t pos)	
{
    if (set == NULL) return -1;
    if (pos > set->size) {
        errno = ESPIPE; /*illegal seek*/
        return -1;
    }

    if (bitset_isset(set, pos) > 0) {
        bitset_unset(set, pos);
    } else {
        bitset_set(set, pos);
    }

    return 0;

}

