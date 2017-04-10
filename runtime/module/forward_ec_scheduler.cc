//
#include "forward_ec_scheduler.h"


#include "../actor/base/local_send.h"
#include "../reliable/process_reliable_msg.h"
#include "../d_nf/d_base/d_nf_processor.cuh"
#include <time.h>
#include   <sys/time.h>

void send_batch(bess::PacketBatch *batch,sn_port* port_) {
  /* TODO: choose appropriate out queue */
  const uint8_t qid = 0;

  uint64_t sent_bytes = 0;
  int sent_pkts;

  sent_pkts = port_->SendPackets(qid, batch->pkts(), batch->cnt());

  const packet_dir_t dir = PACKET_DIR_OUT;

  for (int i = 0; i < sent_pkts; i++){
    sent_bytes += batch->pkts()[i]->total_len();
  }

    port_->queue_stats[dir][qid].packets += sent_pkts;
    port_->queue_stats[dir][qid].dropped += (batch->cnt() - sent_pkts);
    port_->queue_stats[dir][qid].bytes += sent_bytes;


/*  if (sent_pkts < batch->cnt()) {
    bess::Packet::Free(batch->pkts() + sent_pkts, batch->cnt() - sent_pkts);
  }*/
}

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


void Pkt_insert(coordinator* coordinator_,bess::Packet* bess_pkt,int i,int times){

	//while(coordinator_->pkts[i].full==1){
	//	i+=bess::PacketBatch::kMaxBurst;
	//}
	char* dst=coordinator_->pkts[i+times*bess::PacketBatch::kMaxBurst].pkt;
	char* src=bess_pkt->head_data<char*>();
	memcpy(dst,src,bess_pkt->total_len());

	Format(src,&(coordinator_->pkts[i+times*bess::PacketBatch::kMaxBurst].headinfo));

	coordinator_->pkts[i+times*bess::PacketBatch::kMaxBurst].full=1;

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

	//struct timeval whole_begin;
	//gettimeofday(&whole_begin,0);
	gpu_nf_process(pkts,fs,coordinator_actor->get_service_chain(),bess::PacketBatch::kMaxBurst*PROCESS_TIME);
	//struct timeval whole_end;
	//gettimeofday(&whole_end,0);
	for(int j=0;j<i;j++){
	  flow_actor** actor_ptr=coordinator_actor->actorid_htable_.Get(&(fs[j].actor_id_64));
	  if(unlikely(actor_ptr==nullptr)) continue;
	  flow_actor* actor=*actor_ptr;
	  Fs_copyback(&(fs[j]),actor);
	}

	//struct timeval whole_end1;
	//gettimeofday(&whole_end1,0);


	//long begin=whole_begin.tv_sec*1000000 + whole_begin.tv_usec;
	//long end=whole_end.tv_sec*1000000 + whole_end.tv_usec;
	//long begin1=whole_end.tv_sec*1000000 + whole_end.tv_usec;
	//long end1=whole_end1.tv_sec*1000000 + whole_end1.tv_usec;
	//printf("gpu time: %ld, fs_copy_backtime:%ld\n,",end-begin,end1-end);

	 // cudaFree(pkts);
	 // cudaFree(fs);

}

void forward_ec_scheduler::ProcessBatch(bess::PacketBatch *bat){

	struct timeval whole_begin;
	//struct timeval dp_end;
	//struct timeval dp_begin;
	//struct timeval cp_end;
	//struct timeval cp_begin;
	struct timeval insert_end;
	struct timeval insert_begin;
	gettimeofday(&whole_begin,0);
	RECVPacketBatches[counter].Copy(bat);
	counter++;
	if(counter!=PROCESS_TIME){
		return;
	}else{
		//Pkt_reset(coordinator_actor_->pkts,PROCESS_TIME*PROCESS_TIME*bess::PacketBatch::kMaxBurst*bess::PacketBatch::kMaxBurst);
		counter=0;


		//gettimeofday(&dp_begin,0);
		for(int loop=0;loop<PROCESS_TIME;loop++){
		  bess::PacketBatch *batch =&(RECVPacketBatches[loop]);
		  dp_pkt_batch.clear();
		  //cp_pkt_batch.clear();
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
			 // cp_pkt_batch.add(batch->pkts()[i]);

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
		  SENDPacketBatches[loop].Copy(&(coordinator_actor_->ec_scheduler_batch_));
		}

		//gettimeofday(&dp_end,0);

		long time1=0;
	  if(coordinator_actor_->service_chain_.empty()==false){
		  //counter++;

		  flow_actor* it_actor=nullptr;
		 // struct Pkt *pkts;
		 // struct Fs *fs;
		 // cudaMallocManaged(&pkts, bess::PacketBatch::kMaxBurst*bess::PacketBatch::kMaxBurst * sizeof(Pkt));
		 // cudaMallocManaged(&fs, bess::PacketBatch::kMaxBurst * sizeof(Fs));
		  //gettimeofday(&insert_begin,0);
		  int pos;


		  int size=coordinator_actor_->have_packet_flows_rrlist_.get_size();
		  while(pos<size){

			  it_actor=coordinator_actor_->have_packet_flows_rrlist_.pop_head();
			  if(it_actor==nullptr) break;
			 // coordinator_actor_->have_packet_flows_rrlist_.pop_head();
			  //LOG(INFO)<<"POP OK";
			  it_actor->set_in_have_packet_rrlist(0);
			  int times=0;
			  while(it_actor->get_queue_ptr()->empty()!=true){

				  gettimeofday(&insert_begin,0);
				  bess::Packet* it=it_actor->get_queue_ptr()->dequeue();

				  //Pkt_insert(coordinator_actor_,it,pos,times);
				  int i=pos;
				//	while(coordinator_actor_->pkts[i].full==1){
				//		i+=bess::PacketBatch::kMaxBurst;
				//	}

					//char* dst=coordinator_actor_->pkts[i+times*bess::PacketBatch::kMaxBurst].pkt;
					//char* src=it->head_data<char*>();
					//memcpy(dst,src,it->total_len());

					//Format(src,&(coordinator_actor_->pkts[i+times*bess::PacketBatch::kMaxBurst].headinfo));

					coordinator_actor_->pkts[i+times*bess::PacketBatch::kMaxBurst].full=1;

				  gettimeofday(&insert_end,0);
				long begin3=insert_begin.tv_sec*1000000 + insert_begin.tv_usec;
				long end3=insert_end.tv_sec*1000000 + insert_end.tv_usec;
				time1+=end3-begin3;


				  Fs_copy(&(coordinator_actor_->fs[pos]),it_actor);

			  }
			  pos++;

		  }

			 //std::thread gpu_thread(GPU_thread,coordinator_actor_,pkts,fs,i);
		  //PacketBatches[counter-1].Copy(&(coordinator_actor_->ec_scheduler_batch_));
		  //gettimeofday(&insert_end,0);
		  GPU_thread(coordinator_actor_,coordinator_actor_->pkts,coordinator_actor_->fs,pos);
			 //gpu_thread.join();
	  }
	  	//gettimeofday(&cp_begin,0);
		for(int loop=0;loop<PROCESS_TIME;loop++){
		  bess::PacketBatch *batch =&(RECVPacketBatches[loop]);
		  //dp_pkt_batch.clear();
		  cp_pkt_batch.clear();
		  //coordinator_actor_->ec_scheduler_batch_.clear();
		 // char keys[bess::PacketBatch::kMaxBurst][flow_key_size] __ymm_aligned;

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
		  }

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

			 send_batch(&(SENDPacketBatches[loop]),port_);
		}
		//gettimeofday(&cp_end,0);

		clean_batches(SENDPacketBatches);
		clean_batches(RECVPacketBatches);

		struct timeval whole_end;
		gettimeofday(&whole_end,0);

		long begin=whole_begin.tv_sec*1000000 + whole_begin.tv_usec;
		long end=whole_end.tv_sec*1000000 + whole_end.tv_usec;
	//	long begin1=dp_begin.tv_sec*1000000 + dp_begin.tv_usec;
	//	long end1=dp_end.tv_sec*1000000 + dp_end.tv_usec;
	//	long begin2=cp_begin.tv_sec*1000000 + cp_begin.tv_usec;
		//long end2=cp_end.tv_sec*1000000 + cp_end.tv_usec;
	//	long begin3=insert_begin.tv_sec*1000000 + insert_begin.tv_usec;
	//	long end3=insert_end.tv_sec*1000000 + insert_end.tv_usec;
		//printf("total time: %ld, dp_time: %ld, cp_time: %ld, insert_time:%ld \n,",end-begin,end1-begin1,end2-begin2,end3-begin3);
		printf("total time: %ld,insert_time:%ld \n,",end-begin,time1);
	}


}

void forward_ec_scheduler::customized_init(coordinator* coordinator_actor,sn_port* port){
  coordinator_actor_ = coordinator_actor;
  port_=port;
  counter=0;
}

ADD_MODULE(forward_ec_scheduler, "forward_ec_scheduler",
    "process packets received from input port to output port and schedule actors in forward direction")
