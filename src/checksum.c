/*
 * Copyright (c) 2012, ttxie <ioizit at gmail dot com>
 * All rights reserved.
 *
 */

#include "checksum.h"

uint16_t checksum(uint16_t *dp, uint32_t length)
{
        uint32_t sum = 0;
        uint32_t len = length >> 1;

        /*sum event the short bytes*/
        while(len-- > 0)
                sum += *dp++;
        if(length & 1) /*the length is odd number, the last bytes*/
                sum += (*dp & 0xFF00);
               // sum += (*(u_char*)dp);

        /*high 2bytes + low 2byte*/
        sum = (sum >> 16) + (sum & 0xFFFF);
        sum += (sum >> 16);

        return (uint16_t)(~sum);
}

