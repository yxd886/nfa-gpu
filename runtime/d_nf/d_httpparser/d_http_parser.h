
#ifndef D_CAF_NF_HTTP_PARSER_HPP
#define D_CAF_NF_HTTP_PARSER_HPP

#include "d_Public.h"
#include "d_Receiver.h"
#include "d_SessionHash.h"
#include <netinet/ip6.h>
#include "d_SessionHash.h"
#include "../../bessport/packet.h"

#include <glog/logging.h>

class d_http_parser{
public:
  d_http_parser(){
    rcv = d_Receiver();
  }

  void nf_logic_impl(bess::Packet* pkt,d_http_parser_fs* fs){
    // LOG(INFO)<<"http_parser processing logic is called";
		struct rte_mbuf* rte_pkt=reinterpret_cast<struct rte_mbuf *>(pkt);
		unsigned char *t =rte_pktmbuf_mtod(rte_pkt, unsigned char*);
		char* raw_packet = (char*)t;
		process(raw_packet,fs);

  }


private:

  void process(char* raw_packet,d_http_parser_fs* fs){


	  rcv.Work(raw_packet,fs);

  }


  d_Receiver  rcv;
};


#endif
