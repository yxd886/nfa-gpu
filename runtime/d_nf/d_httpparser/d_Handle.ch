#ifndef D_HANDLEA_H_
#define D_HANDLEA_H_

#include "d_Public.ch"
#include "d_BehaviorInfo.ch"
#include "d_FormatPacket.ch"
#include "d_SessionHash.ch"
#include "d_HttpParse.ch"

class d_CHandle{
public:
	d_CHandle();
	~d_CHandle();
	void Init();
	void Process(d_CFormatPacket packet, d_CSharedBehaviorInfo* pInfo, d_http_parser_fsPtr& sesp);
	void Create(d_IFormatPacket *pPacket,d_CSharedBehaviorInfo* pInfo,d_http_parser_fsPtr& ptr);

private:
	void TimeOutCheck();


	d_CHttpParse         _httpParse;


};






#endif
