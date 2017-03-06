#include "d_network_function_base.cuh"
#include "d_network_function_derived.cuh"
#include "d_nf_item.h"
#include "../d_firewall/d_firewall.cuh"
#include "../d_flowmonitor/d_flow_monitor.cuh"
#include "../d_httpparser/d_http_parser.cuh"
#include "../d_pktcounter/d_pkt_counter.cuh"
#include "Pkt.h"

__device__ void Init_nfs(struct d_flow_actor_nfs* nfs){


	nfs->nf[1]=new d_network_function_derived<d_pkt_counter, d_pkt_counter_fs>(1);
	nfs->nf[2]=new d_network_function_derived<d_flow_monitor, d_flow_monitor_fs>(2);
	nfs->nf[3]=new d_network_function_derived<d_firewall, d_firewall_fs>(3);
	nfs->nf[4]=new d_network_function_derived<d_http_parser, d_http_parser_fs>(4);

}

extern "C"{
	void gpu_nf_process(Pkt* pkts){

	}
}

