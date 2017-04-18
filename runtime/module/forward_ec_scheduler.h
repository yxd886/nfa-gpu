// This module is where we poll input/output port and schedule
// execution_context actors.

#ifndef FORWARD_EC_SCHEDULER_H
#define FORWARD_EC_SCHEDULER_H

#include "../bessport/module.h"
#include "../reliable/base/reliable_message_misc.h"
#include "../bessport/packet.h"
#include "../bessport/pktbatch.h"
#include "../d_nf/d_base/Pkt.h"
#include "../port/sn_port.h"
#include "../actor/coordinator.h"
#include "../bessport/module_msg.pb.h"
#include <thread>
#include <chrono>

class coordinator;

class forward_ec_scheduler final : public Module {
public:

  static const gate_idx_t kNumOGates = 1;
  static const gate_idx_t kNumIGates = 1;

  forward_ec_scheduler() : Module(), coordinator_actor_(0),port_(),counter(0),pre_flow_num(0),idx(0),first_time(true){

	  gettimeofday(&time_now,0);
  }

  virtual void ProcessBatch(bess::PacketBatch *batch);

  pb_error_t Init(const bess::pb::PortIncArg &arg){
	  return pb_errno(0);
  }

  void customized_init(coordinator* coordinator_actor,  sn_port* port_);

private:
  void clean_batches(bess::PacketBatch* batches){
	  for(int i=0;i<MAX_PROCESS_TIME;i++){
		  batches[i].clear();
	  }
  }

  coordinator* coordinator_actor_;
  bess::PacketBatch dp_pkt_batch;
  bess::PacketBatch cp_pkt_batch;
  sn_port* port_;
  bess::PacketBatch RECVPacketBatches[MAX_PROCESS_TIME];
  bess::PacketBatch SENDPacketBatches[MAX_PROCESS_TIME];
  int counter;
  int pre_flow_num;
  int idx;
  bool first_time;
  int32_t flow_id[60000];
  struct timeval time_now;
};

#endif
