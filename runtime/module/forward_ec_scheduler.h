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
#include <thread>
#include <chrono>

class coordinator;

class forward_ec_scheduler final : public Module {
public:

  static const gate_idx_t kNumOGates = 1;
  static const gate_idx_t kNumIGates = 1;

  forward_ec_scheduler() : Module(), coordinator_actor_(0){}

  virtual void ProcessBatch(bess::PacketBatch *batch);

  void customized_init(coordinator* coordinator_actor,  sn_port* port_);

private:

  coordinator* coordinator_actor_;
  bess::PacketBatch dp_pkt_batch;
  bess::PacketBatch cp_pkt_batch;
  sn_port* port_;
};

#endif
