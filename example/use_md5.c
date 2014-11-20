/*
	* use_md5.c
	* copright (c)
	* Editor: ZhangErjiang
	* date: 2013-01-10
	* Description: the example of how to use md5 encrypt data.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <md5.h>
#include <utl.h>

int main(int argc, char** argv)
{

	char result[64];
	u_char data[64];
	int flag;
	//u_char *md5;
	//int len = strlen(data);
	//memset(result, 0, 128);
	//md5_init(&md5Con);
	//md5_update(&md5Con, data, len);
	//md5_final(result, &md5Con);
	//hexdump(result, 128);
	
	printf("请输入要加密的字符串data：\n");
	scanf("%s", data);
	
	md5(data, result);	////调用md5加密函数，加密后的值存在result中

	printf("\n对data进行md5加密后的十六进制串: %s \n ",result);

	
	return 0;	
}
