
#ifndef D_CAF_NF_HTTP_PARSER_HPP
#define D_CAF_NF_HTTP_PARSER_HPP

#include "d_Public.ch"
#include "d_Receiver.ch"
#include "d_SessionHash.ch"
#include <netinet/ip6.h>
#include "d_SessionHash.ch"
//#include "../../bessport/packet.h"

#include <glog/logging.h>

class d_http_parser{
public:
  d_http_parser(){
    rcv = d_Receiver();
  }

  void nf_logic_impl(char* pkt,d_http_parser_fs* fs){

		process(pkt,fs);

  }


private:

  void process(char* raw_packet,d_http_parser_fs* fs){


	  rcv.Work(raw_packet,fs);

  }


  d_Receiver  rcv;
};


#endif
