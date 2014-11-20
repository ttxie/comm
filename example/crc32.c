#include "crc32.h"
#include "utl.h"
#include <stdio.h>

int main()
{
        printf("1234 crc32 gen:%x\n", crc32("1234", 4));
        printf("tttt crc32 gen:%x\n", crc32("tttt", 4));

}
