#include "crc16.h"
#include <stdio.h>

//ʹ���ֽ����㷨��ǰ����ֵ�PPP֡FCS�������֤���̣���������ĳ���Ƭ��ʵ�֣�
int main()
{
	u_char ppp[13] = {0xFF, 0x03, 0xC0, 0x21, 0x04, 0x03, 0x00, 0x07, 0x0D, 0x03, 0x06, 0x00, 0x00};
	uint16_t result;
	
	// ����CRC
	result = crc16(ppp, 11);
	
	printf("CRC:%X.\n", result);
	
	// ��дFCS���ȵͺ��
	ppp[11] = result & 0xff;
	ppp[12] = (result >> 8) & 0xff;
	
	// ��֤FCS
	if(crc16_check(ppp, 13)){
		printf("CRC OK.\n");
	}
	return 0;
}

