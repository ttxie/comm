/*
	* Copyright (C) 2013
	* use_sha1.c
	* Editor: ZhangErjiang
	* Data: 3013-01-15
	* Description: A test function of how to use sha1 to encryption algorithm
	* sha1: Secure Hash Algorithm
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>

#include <utl.h>
#include <sha1.h>

int main(int argc, char **argv)
{
	/*
	SHA1_CTX ctxt;
	uint8 *data="abcd";
	uint8 result[64];
	int len;
	len = strlen(data);
	sha1_init(&ctxt);
	sha1_loop(&ctxt, data, len);
	sha1_result(&ctxt, result);
	hexdump(result, 20);
	printf("the result is %s: \n", result);
	*/
	
	//u_char *data = "abcdefgh";
	u_char data[64];
	u_char result[64];
	int flag;

	printf("������Ҫ���ܵ��ַ���data��\n");
	scanf("%s", data);
	
	sha1(data, result);	//����sha1���ܺ��������ܺ��ֵ����result��

	printf("����sha1���ܺ��ҽ���ʮ������ת����Ľ��Ϊ��%s \n", result);

	return 0;
}