#ifndef SESSIONHASH_H_
#define SESSIONHASH_H_

#include "Public.h"
#include "Buffer.h"
#include "FormatPacket.h"
#include "BehaviorInfo.h"
#include <rte_config.h>
#include <rte_mbuf.h>
#include <rte_ether.h>
#include <rte_ethdev.h>

struct http_parser_fs{

	uint32_t ServerIp;
	uint32_t ClientIp;
	uint16_t ServerPort;
	uint16_t ClientPort;
	uint64_t ServerMac;
	uint64_t ClientMac;

	time_t   CreatedTime;
	time_t   RefreshTime;
	uint32_t SeqNo;
	uint32_t AckSeqNo;

	uint8_t Protocol;
	CBuffer  ReqBuf;
	CBuffer  RspBuf;
	CResult  Result;
	int counter;
};
typedef http_parser_fs*    http_parser_fsPtr;

void http_parser_fs_Reset(http_parser_fsPtr& ptr);







#endif
