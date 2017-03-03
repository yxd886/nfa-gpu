#ifndef D_BUFFERA_H
#define D_BUFFERA_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "d_Public.ch"

const uint32_t  MAX_BUFFER_SIZE = 64 * 1024 * 1024;
const uint32_t  BUFFER_SIZE = 2048;

struct d_CBuffer{

	uint32_t len;
	uint32_t _free;
	char *   buf;

};


void d_CBuffer_Reset(struct d_CBuffer& Cbuf);

bool d_Append(struct d_CBuffer& Cbuf,char* p, size_t size);

char* d_GetBuf(struct d_CBuffer Cbuf,uint32_t& size);

uint32_t d_GetBufLen(struct d_CBuffer Cbuf);


void d_Buf_init(struct d_CBuffer& Cbuf);


#endif
