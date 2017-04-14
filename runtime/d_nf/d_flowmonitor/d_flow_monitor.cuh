#ifndef D_CAF_NF_FLOW_MONITOR_HPP
#define D_CAF_NF_FLOW_MONITOR_HPP
#include <rte_config.h>
#include <rte_mbuf.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
//#include <netinet/in.h>
#include <rte_ether.h>
#include <rte_ethdev.h>
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
#include "d_flow_monitor_fs.cuh"
#include "d_fm_headinfo.cuh"
#include "../d_firewall/d_fw_headinfo.cuh"
#include "../d_base/d_nf_item.h"


#include <glog/logging.h>

class d_flow_monitor{
public:
	__device__ d_flow_monitor(){

  }
	__device__ ~d_flow_monitor(){

  }



	__device__ void nf_logic_impl(Pkt* pkt, d_flow_monitor_fs* fs){

		process(pkt, fs);
		//    printf("total number: %d\nudp number: %d\ntcp number: %d\nicmp number: %d\n",ptr->no_total,ptr->no_tcp,ptr->no_udp,ptr->no_icmp);

	}

__device__ void Format(char* packet,struct d_headinfo* hd){
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

__device__ void Init_rules(struct d_rule*firewall_rules ){}
__device__ void process(Pkt* raw_packet,d_flow_monitor_fs* fs){

	struct d_headinfo headinfo;
	Format(raw_packet->pkt,&headinfo);
	if(fs->counter==0){

      //fs->CreatedTime=time(0);
      fs->SrcIp =1;
      fs->SrcIp = Ntohl(headinfo.m_pIphdr.saddr);
      fs->DstIp = Ntohl(headinfo.m_pIphdr.daddr);
      uint32_t tmp;

      fs->protocol   = headinfo.m_pIphdr.protocol;
      if(headinfo.is_tcp==0){
			  fs->SrcPort=0;
			  fs->DstPort=0;
      }else{
      	fs->SrcPort = Ntohs(headinfo.m_pTcphdr.th_sport);
      	fs->DstPort = Ntohs(headinfo.m_pTcphdr.th_dport);

       }

  	}

  	//fs->RefreshTime=time(0);
    if(fs->protocol==IPPROTO_TCP){
    	fs->no_tcp++;

    }else if(fs->protocol==IPPROTO_UDP){
    	fs->no_udp++;

    }else if(fs->protocol==IPPROTO_ICMP){
    	fs->no_icmp++;

    }
    fs->no_total++;
    fs->counter++;
  }


};

#endif
