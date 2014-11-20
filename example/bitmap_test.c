/*
	* Copyright (c) 2013
	* Editor: ZhangErjiang
	* Date: 2013-01-17
	* Description: A test function fo how to use bitmap
*/

#include <stdio.h>
#include <stdlib.h>

//#include <bitmap.h>
#include "bitset.h"
#include "utl.h"

int main(int argc, char **argv)
{
	//19=4;

	int a[20] = {5,8,7,6,3,1,10,78,56,34,23,12,43,54,65,76,87,98,89,99};
	//int a[20] = {5,8,7,6,3,1,10,78,56,34,23,12,43,54,65,76,87,98,89,99};

	//int a[2] = {34,23};
	int i, len, count;
	bitset_t *set;
	
	
	int rows=13;
	printf("size:%u\n", (rows-1)/16==rows/16?rows/16+1:rows/16);

	printf("========bitset========== \n");
	set = bitset_create(19);					//��ʼ��bitset
	//bitset_reset(set);						//��bitset����λ��0
	/*for (i=0; i<20; i++) {
                bitset_set(set, a[i]);
	}*/
        bitset_set(set, 0);
        bitset_set(set, 1);
        bitset_set(set, 2);
        bitset_set(set, 3);
        bitset_set(set, 18);
        hexdump(set->bits, 16);
        bitset_print(set);						//���bitset����λ��ֵ
        if(bitset_isset(set, 17))
                printf("bit18 is setted!\n");
	
	len = bitset_size(set);					//ͳ��bitset���ܳ���
	count = bitset_count(set);			//ͳ��bitset��λΪ1������
	printf("the size is:%d \n", len);
	printf("the bitset of 1's count is:%d \n", count);
        

	bitset_reverse(set);					//��ת����λ
        printf("bitset reverse:\n");
	bitset_print(set);	
	
	/*printf("\n=======bitset reverse===== \n");
	for (i=0; i<20; i++) {
		bitset_bit_reverse(set, a[i]);		//��תposλ
	}
	bitset_print(set);						
	
	for (i=0; i<20; i++) {
		bitset_unset(set, a[i]);		//��pos��0
	}*/
        printf("bitset unset:\n");
	bitset_print(set);						
	
	bitset_reverse(set);					//��ת����λ
        printf("bitset reverse:\n");
	bitset_print(set);	
	
	bitset_free(set);						//�ͷſռ�
	
	return 0;
}

