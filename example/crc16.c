#include "crc16.h"
#include <stdio.h>

//使用字节型算法，前面出现的PPP帧FCS计算和验证过程，可用下面的程序片断实现：
int main()
{
	u_char ppp[13] = {0xFF, 0x03, 0xC0, 0x21, 0x04, 0x03, 0x00, 0x07, 0x0D, 0x03, 0x06, 0x00, 0x00};
	uint16_t result;
	
	// 计算CRC
	result = crc16(ppp, 11);
	
	printf("CRC:%X.\n", result);
	
	// 填写FCS，先低后高
	ppp[11] = result & 0xff;
	ppp[12] = (result >> 8) & 0xff;
	
	// 验证FCS
	if(crc16_check(ppp, 13)){
		printf("CRC OK.\n");
	}
	return 0;
}

