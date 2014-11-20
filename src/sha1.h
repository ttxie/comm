/*
 * FIPS pub 180-1: Secure Hash Algorithm (SHA-1)
 * based on: http://www.itl.nist.gov/fipspubs/fip180-1.htm
 * implemented by Jun-ichiro itojun Itoh <itojun@itojun.org>
 * Modify by ZhangErjiang 
 * Date: 2013-01-14
 */

#ifndef _COMM_SHA1_H_
#define _COMM_SHA1_H_

#include <stdio.h>
//#include <sys/param.h>
#include <string.h>

typedef unsigned char uint8;		/* == 8 bits */
typedef unsigned short uint16;		/* == 16 bits */
typedef unsigned int uint32;		/* == 32 bits */
typedef unsigned long int uint64;	/* == 64 bits */

struct sha1_ctxt
{
	union
	{
		uint8		b8[20];
		uint32		b32[5];
	}			h;
	union
	{
		uint8		b8[8];
		uint64		b64[1];
	}			c;
	union
	{
		uint8		b8[64];
		uint32		b32[16];
	}			m;
	uint8		count;
};

void sha1_init(struct sha1_ctxt *);
void sha1_pad(struct sha1_ctxt *);
void sha1_loop(struct sha1_ctxt *, const uint8 *, size_t);
void sha1_result(struct sha1_ctxt *, uint8 *);

void sha1(uint8 *srcdata, uint8 *result);

/* compatibilty with other SHA1 source codes */
typedef struct sha1_ctxt SHA1_CTX;

//#define SHA1_RESULTLEN	(160/8)

#endif   /* _SHA1_H_ */
