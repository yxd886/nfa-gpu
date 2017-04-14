#ifndef D_NF_PROCESSOR_H
#define D_NF_PROCESSOR_H
#include <cuda_runtime.h>
#include "../d_firewall/d_firewall.cuh"
#include <helper_cuda.h>

#include <stdint.h>

#include "Pkt.h"

extern "C"{

void gpu_nf_process(Pkt* h_pkts,Fs* h_fs,uint64_t service_chain,int packet_num,int *h_flow_size,struct d_rule* h_firewall_rules);


}
#endif
