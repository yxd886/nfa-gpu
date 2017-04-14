// This module is where we poll input/output port and schedule
// execution_context actors.

#ifndef PKT_H
#define PKT_H
#define PKT_SIZE 100
#define PKT_NUM 320
#define FS_STATE_MAX_SIZE 100
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
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#include "../../actor/flow_actor.h"

struct d_headinfo{
  struct ether_hdr m_pEthhdr;
  struct iphdr m_pIphdr;
  struct tcphdr m_pTcphdr;
  struct udphdr m_pUdphdr;
  uint8_t protocol;
  uint8_t is_tcp;
  uint8_t is_udp;
};


struct  Pkt{
	char full;
	char pad1;
	char pad2;
	char pad3;
	uint64_t flow_id;
	uint64_t pad5;
	struct d_headinfo headinfo;

	char pkt[PKT_SIZE];
};

struct  Fs{
	uint64_t actor_id_64;
	flow_actor* ptr;
	char fs[8][FS_STATE_MAX_SIZE];

};



#endif
