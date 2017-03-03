#include "d_network_function_base.ch"
#include "d_network_function_derived.ch"
#include "d_nf_item.h"
#include "../d_firewall/d_firewall.ch"
#include "../d_flowmonitor/d_flow_monitor.ch"
#include "../d_httpparser/d_http_parser.ch"
#include "../d_pktcounter/d_pkt_counter.ch"
#include "Pkt.h"

__device__ void Init_nfs(struct d_flow_actor_nfs* nfs){


	//nfs->nf[1]=new d_network_function_derived<d_pkt_counter, d_pkt_counter_fs>(1);
	nfs->nf[2]=new d_network_function_derived<d_flow_monitor, d_flow_monitor_fs>(2);
	nfs->nf[3]=new d_network_function_derived<d_firewall, d_firewall_fs>(3);
	nfs->nf[4]=new d_network_function_derived<d_http_parser, d_http_parser_fs>(4);

}

extern "C"{
	void gpu_nf_process(Pkt* pkts){

	}
}

