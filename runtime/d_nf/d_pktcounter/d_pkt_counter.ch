#ifndef PKT_COUNTER_H
#define PKT_COUNTER_H

//#include "../../bessport/packet.h"
#include "d_pkt_counter_fs.ch"

class d_pkt_counter{
public:
  inline void nf_logic_impl(bess::Packet* pkt, d_pkt_counter_fs* fs){
    fs->counter += 1;
  }
};

#endif
