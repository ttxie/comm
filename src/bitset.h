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

bitset_t *bitset_create(size_t size);					//��ʼ��λͼ
void bitset_free(bitset_t *set);						//�ͷ�λ�ռ�

int bitset_reset(bitset_t *set);						//������λ��Ϊ0
int bitset_setall(bitset_t *set);               	//������λ��Ϊ1
int bitset_unset(bitset_t *set, size_t pos);	//��posλ��0
int bitset_set(bitset_t *set, size_t pos);		//��posλ��1
int bitset_isset(bitset_t *set, size_t pos);			//�ж�pos��1����0

int bitset_size(bitset_t *set);                  		//����λ��, ��Ӧ_len��ֵ��
int bitset_count(bitset_t *set);                 	//��������Ϊ1�ĸ�������Ӧ��
void bitset_print(bitset_t *set);                		//���λͼ�е�ÿ��λ�����ݡ�
int bitset_reverse(bitset_t *set);					//��ת����λ
int bitset_bit_reverse(bitset_t *set, size_t pos);	//��תposλ

#endif	//_BITSET_H_
