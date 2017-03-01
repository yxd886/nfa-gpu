#include "d_SessionHash.h"

void d_http_parser_fs_Reset(d_http_parser_fsPtr& ptr){
	d_CBuffer_Reset(ptr->ReqBuf);
	d_CBuffer_Reset(ptr->RspBuf);
	d_CResult_Reset(ptr->Result);

}
