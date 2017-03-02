// This module is where we poll input/output port and schedule
// execution_context actors.

#ifndef FORWARD_EC_SCHEDULER_H
#define FORWARD_EC_SCHEDULER_H
#define PKT_SIZE 500


#include "../bessport/module.h"
#include "../reliable/base/reliable_message_misc.h"
#include "../bessport/packet.h"
#include "../bessport/pktbatch.h"

class coordinator;

struct Pkt{
	bool empty;
	char pkt[PKT_SIZE];
};

void Pkt_insert(struct Pkt* Pkts,bess::Packet* bess_pkt,int i){

	while(Pkts[i].empty!=true){
		i+=bess::PacketBatch::kMaxBurst;
	}
	char* dst=Pkts[i].pkt;
	char* src=bess_pkt->head_data<char*>();
	memcpy(dst,src,bess_pkt->total_len());
	Pkts[i].empty=false;

}

class forward_ec_scheduler final : public Module {
public:

  static const gate_idx_t kNumOGates = 1;
  static const gate_idx_t kNumIGates = 1;

  forward_ec_scheduler() : Module(), coordinator_actor_(0){}

  virtual void ProcessBatch(bess::PacketBatch *batch);

  void customized_init(coordinator* coordinator_actor);

private:

  coordinator* coordinator_actor_;
  bess::PacketBatch dp_pkt_batch;
  bess::PacketBatch cp_pkt_batch;
};

#endif
