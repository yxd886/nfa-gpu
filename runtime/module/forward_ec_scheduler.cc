//
#include "forward_ec_scheduler.h"

#include "../actor/coordinator.h"
#include "../actor/base/local_send.h"
#include "../reliable/process_reliable_msg.h"
#include "../d_nf/d_base/d_nf_processor.cuh"


void Format(char* packet,struct d_headinfo* hd){
  struct ether_hdr* m_pEthhdr;
  struct iphdr* m_pIphdr;
  struct tcphdr* m_pTcphdr;
  struct udphdr* m_pUdphdr;



  m_pEthhdr = (struct ether_hdr*)packet;
  memcpy(&(hd->m_pEthhdr),m_pEthhdr,sizeof(struct ether_hdr));
  m_pIphdr = (struct iphdr*)(packet + sizeof(struct ether_hdr));
  memcpy(&(hd->m_pIphdr),m_pIphdr,sizeof(struct iphdr));
  if(m_pIphdr->protocol==IPPROTO_TCP){
         m_pTcphdr = (struct tcphdr*)(packet + sizeof(struct ether_hdr)+(hd->m_pIphdr.ihl)*4);
         memcpy(&(hd->m_pTcphdr),m_pTcphdr,sizeof(struct tcphdr));
         hd->is_udp=0;
  }else if(m_pIphdr->protocol==IPPROTO_UDP){
     hd->is_tcp = 0;
     m_pUdphdr=(struct udphdr*)(packet + sizeof(struct ether_hdr)+(hd->m_pIphdr.ihl)*4);
     memcpy(&(hd->m_pUdphdr),m_pUdphdr,sizeof(struct udphdr));

   }else{
      hd->is_tcp = 0;
      hd->is_udp = 0;
    }

  hd->protocol =  m_pIphdr->protocol;
  return;
}


void Pkt_insert(struct Pkt* Pkts,bess::Packet* bess_pkt,int i){

	while(Pkts[i].empty!=1){
		i+=bess::PacketBatch::kMaxBurst;
	}
	char* dst=Pkts[i].pkt;
	char* src=bess_pkt->head_data<char*>();
	memcpy(dst,src,bess_pkt->total_len());
	Format(src,&(Pkts[i].headinfo));
	Pkts[i].empty=0;

}

void Pkt_reset(struct Pkt* Pkts,int num){

	for(int i=0;i<num;i++){
		Pkts[i].empty=1;
	}
}
void Fs_copy(struct Fs* Fs,flow_actor* flow_actor){

	size_t i=0;
	for(; i<flow_actor->get_service_chain_len(); i++){
	    char* fs_state_ptr = flow_actor->get_fs()->nf_flow_state_ptr[i];
	    memcpy(Fs->fs[i],fs_state_ptr,flow_actor->get_fs_size()->nf_flow_state_size[i]);
	  }
	Fs->actor_id_64=flow_actor->get_id_64();

}


void Fs_copyback(struct Fs* Fs,flow_actor* flow_actor){

	size_t i=0;
	for(; i<flow_actor->get_service_chain_len(); i++){
	    char* fs_state_ptr = flow_actor->get_fs()->nf_flow_state_ptr[i];
	    memcpy(fs_state_ptr,Fs->fs[i],flow_actor->get_fs_size()->nf_flow_state_size[i]);
	  }

}

void GPU_thread(coordinator* coordinator_actor,Pkt* pkts,Fs* fs, int i){


	  gpu_nf_process(pkts,fs,coordinator_actor->get_service_chain(),bess::PacketBatch::kMaxBurst);

	  for(int j=0;j<i;j++){
		  flow_actor** actor_ptr=coordinator_actor->actorid_htable_.Get(&(fs[j].actor_id_64));
		  flow_actor* actor=*actor_ptr;
		  Fs_copyback(&(fs[j]),actor);
	  }

	  cudaFree(pkts);
	  cudaFree(fs);

}

void forward_ec_scheduler::ProcessBatch(bess::PacketBatch *batch){
  dp_pkt_batch.clear();
  cp_pkt_batch.clear();
  coordinator_actor_->ec_scheduler_batch_.clear();
  char keys[bess::PacketBatch::kMaxBurst][flow_key_size] __ymm_aligned;

  for(int i=0; i<batch->cnt(); i++){
    char* data_start = batch->pkts()[i]->head_data<char*>();

    if(unlikely( ((*((uint16_t*)(data_start+14)) & 0x00f0) != 0x0040) ||
                 ( ((*((uint16_t*)(data_start+23)) & 0x00ff) != 0x0006) &&
                   ((*((uint16_t*)(data_start+23)) & 0x00ff) != 0x0011) &&
                   ((*((uint16_t*)(data_start+23)) & 0x00ff) != 0x00FF)) ) ){
      coordinator_actor_->gp_collector_.collect(batch->pkts()[i]);
      continue;
    }

    if(((*((uint16_t*)(data_start+23)) & 0x00ff) == 0x00FF)){
      cp_pkt_batch.add(batch->pkts()[i]);

    }
    else{
      dp_pkt_batch.add(batch->pkts()[i]);
    }
  }

  for(int i=0; i<dp_pkt_batch.cnt(); i++){
    char* data_start = dp_pkt_batch.pkts()[i]->head_data<char*>();

    memset(&keys[i][flow_key_size-8], 0, sizeof(uint64_t));
    for(int j=0; j<3; j++){
      char* key = keys[i]+coordinator_actor_->fields_[j].pos;
      *(uint64_t *)key = *(uint64_t *)(data_start + coordinator_actor_->fields_[j].offset) &
                         coordinator_actor_->fields_[j].mask;
    }

    flow_actor** actor_ptr = coordinator_actor_->htable_.Get(reinterpret_cast<flow_key_t*>(keys[i]));
    flow_actor* actor = 0;

    if(unlikely(actor_ptr==nullptr)){
      actor = coordinator_actor_->allocator_.allocate();

      if(unlikely(actor==nullptr)){
        LOG(WARNING)<<"No available flow actors to allocate";
        actor = coordinator_actor_->deadend_flow_actor_;
      }
      else{
        // using a round rubin to choose replica
        // LOG(INFO)<<"Receive a flow and select a replica";
        generic_list_item* replica_item = coordinator_actor_->replicas_rrlist_.rotate();

        coordinator_actor_->active_flows_rrlist_.add_to_tail(actor);

        send(actor, flow_actor_init_with_pkt_t::value,
             coordinator_actor_,
             reinterpret_cast<flow_key_t*>(keys[i]),
             coordinator_actor_->service_chain_,
             dp_pkt_batch.pkts()[i],
             replica_item);
      }

      coordinator_actor_->htable_.Set(reinterpret_cast<flow_key_t*>(keys[i]), &actor);

      uint64_t actor_id_64 = actor->get_id_64();
      coordinator_actor_->actorid_htable_.Set(&actor_id_64, &actor);

      actor_ptr = &actor;
    }

    send(*actor_ptr, pkt_msg_t::value, dp_pkt_batch.pkts()[i]);
  }

  //
  if(coordinator_actor_->service_chain_.empty()==false){
	  flow_actor* it_actor=nullptr;
	  struct Pkt *pkts;
	  struct Fs *fs;
	  cudaMallocManaged(&pkts, bess::PacketBatch::kMaxBurst*bess::PacketBatch::kMaxBurst * sizeof(Pkt));
	  cudaMallocManaged(&fs, bess::PacketBatch::kMaxBurst * sizeof(Fs));
	  int i=0;
	  Pkt_reset(pkts,32*32);
	  while(i<32/*coordinator_actor_->active_flows_rrlist_.get_size()*/){

		  it_actor=coordinator_actor_->active_flows_rrlist_.rotate();
		  if(it_actor->get_queue_ptr()->empty()){
			  continue;
		  }else{
			  while(it_actor->get_queue_ptr()->empty()!=true){

				  Pkt_insert(pkts,it_actor->get_queue_ptr()->dequeue(),i);
				  Fs_copy(&(fs[i]),it_actor);

			  }

			  i++;
		  }
	  }

	 // std::thread gpu_thread(GPU_thread,coordinator_actor_,pkts,fs,i);
	  GPU_thread(coordinator_actor_,pkts,fs,i);


	  for(int i=0; i<cp_pkt_batch.cnt(); i++){
	    char* data_start = cp_pkt_batch.pkts()[i]->head_data<char*>();
	    uint64_t mac_addr = ((*(reinterpret_cast<uint64_t *>(data_start+6))) & 0x0000FFffFFffFFfflu);

	    reliable_p2p** r_ptr = coordinator_actor_->mac_to_reliables_.Get(&mac_addr);
	    if(unlikely(r_ptr == nullptr)){
	      coordinator_actor_->gp_collector_.collect(cp_pkt_batch.pkts()[i]);
	      continue;
	    }

	    reliable_single_msg* msg_ptr = (*r_ptr)->recv(cp_pkt_batch.pkts()[i]);
	    if(unlikely(msg_ptr == nullptr)){
	      continue;
	    }

	    process_reliable_msg::match(msg_ptr, coordinator_actor_);
	    msg_ptr->clean(&(coordinator_actor_->gp_collector_));
	  }

	  RunNextModule(&(coordinator_actor_->ec_scheduler_batch_));
  }else{


	  for(int i=0; i<cp_pkt_batch.cnt(); i++){
	    char* data_start = cp_pkt_batch.pkts()[i]->head_data<char*>();
	    uint64_t mac_addr = ((*(reinterpret_cast<uint64_t *>(data_start+6))) & 0x0000FFffFFffFFfflu);

	    reliable_p2p** r_ptr = coordinator_actor_->mac_to_reliables_.Get(&mac_addr);
	    if(unlikely(r_ptr == nullptr)){
	      coordinator_actor_->gp_collector_.collect(cp_pkt_batch.pkts()[i]);
	      continue;
	    }

	    reliable_single_msg* msg_ptr = (*r_ptr)->recv(cp_pkt_batch.pkts()[i]);
	    if(unlikely(msg_ptr == nullptr)){
	      continue;
	    }

	    process_reliable_msg::match(msg_ptr, coordinator_actor_);
	    msg_ptr->clean(&(coordinator_actor_->gp_collector_));
	  }

	  RunNextModule(&(coordinator_actor_->ec_scheduler_batch_));
  }



}

void forward_ec_scheduler::customized_init(coordinator* coordinator_actor){
  coordinator_actor_ = coordinator_actor;
}

ADD_MODULE(forward_ec_scheduler, "forward_ec_scheduler",
    "process packets received from input port to output port and schedule actors in forward direction")
