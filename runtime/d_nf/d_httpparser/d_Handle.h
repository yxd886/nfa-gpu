#ifndef D_HANDLEA_H_
#define D_HANDLEA_H_

#include "d_Public.h"
#include "d_BehaviorInfo.h"
#include "d_FormatPacket.h"
#include "d_SessionHash.h"
#include "d_HttpParse.h"

class d_CHandle{
public:
	d_CHandle();
	~d_CHandle();
	void Init();
	void Process(d_CFormatPacket packet, CSharedBehaviorInfo* pInfo, d_http_parser_fsPtr& sesp);
	void Create(d_IFormatPacket *pPacket,CSharedBehaviorInfo* pInfo,d_http_parser_fsPtr& ptr);

private:
	void TimeOutCheck();


	d_CHttpParse         _httpParse;


};






#endif
