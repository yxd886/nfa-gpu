#ifndef D_MODIFIED_FIREWALL_HPP
#define D_MODIFIED_FIREWALL_HPP
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/in.h>
//#include <rte_ethdev.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include <sys/time.h>
#include <deque>
#include <set>
#include <map>
#include <list>
#include <asm-generic/int-ll64.h>
#include <rte_config.h>
#include <rte_memory.h>
#include <rte_memzone.h>
#include <rte_launch.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_debug.h>
#include <rte_common.h>
#include <rte_log.h>
#include <rte_malloc.h>
#include <rte_memory.h>
#include <rte_memcpy.h>
#include <rte_memzone.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_launch.h>
#include <rte_atomic.h>
#include <rte_cycles.h>
#include <rte_prefetch.h>
#include <rte_lcore.h>
#include <rte_per_lcore.h>
#include <rte_branch_prediction.h>
#include <rte_interrupts.h>
#include <rte_pci.h>
#include <rte_random.h>
#include <rte_debug.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
#include <rte_ring.h>
#include <rte_mempool.h>
#include <rte_mbuf.h>

#include "d_fw_headinfo.ch"
//#include "../../bessport/packet.h"
#include "d_firewall_fs.ch"
#include <cassert>


class Rules{
public: 
	__device__ Rules(){counter=0;}
	__device__ ~Rules(){}
	__device__ int get_number(){
		return counter;
	}
	__device__ struct d_rule* get_element(int i){
		return d_rules[i];
	}
	__device__ void push_back(struct d_rule* rule_ptr){
		d_rules[counter]=rule_ptr;
		counter++;
	}
private:
	int counter;
	struct d_rule* d_rules[10];

};

class d_firewall{
public:
	__device__ d_firewall(){
   
    char saddr[200];
    memset(saddr,0,sizeof(saddr));
    char daddr[200];
    memset(daddr,0,sizeof(daddr));
    
    struct d_rule r;
    struct d_rule* rp=&r;
  //  std::cout<<"begin to read rules"<<std::endl;
   
      *(unsigned char *)&rp->saddr.addr=0;
      *(((unsigned char *)&rp->saddr.addr)+1)=0;
      *(((unsigned char *)&rp->saddr.addr)+2)=0;
      *(((unsigned char *)&rp->saddr.addr)+3)=0;
      rp->saddr.mask=32;
      rp->sport=65535;
      *((unsigned char *)&rp->daddr.addr)=0;
      *((unsigned char *)&rp->daddr.addr+1)=0;
      *((unsigned char *)&rp->daddr.addr+2)=0;
      *((unsigned char *)&rp->daddr.addr+3)=0;
      rp->daddr.mask=32;
      rp->dport=65535;
      rp->protocol=6;
      rp->action=0;
     rules.push_back(&r);
      
      
      *(unsigned char *)&rp->saddr.addr=0;
      *(((unsigned char *)&rp->saddr.addr)+1)=0;
      *(((unsigned char *)&rp->saddr.addr)+2)=0;
      *(((unsigned char *)&rp->saddr.addr)+3)=0;
      rp->saddr.mask=32;
      rp->sport=65535;
      *((unsigned char *)&rp->daddr.addr)=119;
      *((unsigned char *)&rp->daddr.addr+1)=75;
      *((unsigned char *)&rp->daddr.addr+2)=217;
      *((unsigned char *)&rp->daddr.addr+3)=109;
      rp->daddr.mask=32;
      rp->dport=65535;
      rp->protocol=6;
      rp->action=1;
     rules.push_back(&r);
   
 //  std::cout<<"begin to close the rule file !"<<std::endl;
  
 //  std::cout<<"close the rule file successfully !"<<std::endl;
  }

	__device__ void nf_logic_impl(char* pkt, d_firewall_fs* fs);

private:

	__device__ void process(char* packet,d_firewall_fs* fs);

	__device__ void Format(char* packet,struct d_headinfo* hd);

	__device__ Bool CompareID_with_mask(uint32_t addr1, uint32_t addr2, uint8_t mask);

	__device__ void filter_local_out(struct d_headinfo *hd,d_firewall_fs* sesptr);

	__device__ uint16_t GetPort(struct d_headinfo *hd, int flag);



 Rules rules;

};

#endif
