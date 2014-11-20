/*
	* Copyright (c) 2013
	* bitset.h
	* Editor: ZhangErjiang
	* Date: 2013-01-18
	* Description:
	
	* Modify: by ZhangErjiang 2013-01-21 add some function
*/

#ifndef _COMM_BITSET_H_
#define _COMM_BITSET_H_

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>

typedef unsigned char u_char;

typedef struct bitset_s{
	u_char *bits;
	size_t size;
} bitset_t;

bitset_t *bitset_create(size_t size);					//初始化位图
void bitset_free(bitset_t *set);						//释放位空间

int bitset_reset(bitset_t *set);						//将所有位置为0
int bitset_setall(bitset_t *set);               	//将所有位置为1
int bitset_unset(bitset_t *set, size_t pos);	//将pos位置0
int bitset_set(bitset_t *set, size_t pos);		//将pos位置1
int bitset_isset(bitset_t *set, size_t pos);			//判断pos是1还是0

int bitset_size(bitset_t *set);                  		//返回位数, 对应_len的值。
int bitset_count(bitset_t *set);                 	//返回设置为1的个数，对应。
void bitset_print(bitset_t *set);                		//输出位图中的每个位的内容。
int bitset_reverse(bitset_t *set);					//翻转所有位
int bitset_bit_reverse(bitset_t *set, size_t pos);	//翻转pos位

#endif	//_BITSET_H_
