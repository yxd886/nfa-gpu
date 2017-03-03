
#include "d_Buffer.ch"

void d_CBuffer_Reset(struct d_CBuffer& Cbuf){
	if(!Cbuf.buf){
		Cbuf.buf = (char*) malloc(BUFFER_SIZE);
		memset(Cbuf.buf,0x00,Cbuf._free);
	}

	if(Cbuf.len > BUFFER_SIZE * 2 && Cbuf.buf){
		//如果目前buf的大小是默认值的2倍，则对其裁剪内存，保持buf的大小为默认值，减小内存耗费
		char* newbuf = (char*) realloc(Cbuf.buf,BUFFER_SIZE);
		if(newbuf != Cbuf.buf)
		Cbuf.buf = newbuf;
	}

	Cbuf.len = 0;
	Cbuf._free = BUFFER_SIZE;
}

bool d_Append(struct d_CBuffer& Cbuf,char* p, size_t size){
	if(!p || !size)
			return true;
	if(size < Cbuf._free){
		memcpy(Cbuf.buf + Cbuf.len, p , size);
		Cbuf.len += size;
		Cbuf._free -= size;
	}else{
		return false;
	}

	return true;
}

char* d_GetBuf(struct d_CBuffer Cbuf,uint32_t& size){
	size = Cbuf.len;
	return Cbuf.buf;
}

uint32_t d_GetBufLen(struct d_CBuffer Cbuf){

	return Cbuf.len;

}


void d_Buf_init(struct d_CBuffer& Cbuf){
	Cbuf.len=0;
	Cbuf._free=0;
	Cbuf.buf=0;
	d_CBuffer_Reset(Cbuf);
}