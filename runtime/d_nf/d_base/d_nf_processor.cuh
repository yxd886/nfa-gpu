#ifndef D_NF_PROCESSOR_H
#define D_NF_PROCESSOR_H
#include <cuda_runtime.h>

#include <helper_cuda.h>

#include <stdint.h>

#include "Pkt.h"

extern "C"{

void gpu_nf_process(Pkt* h_pkts,Fs* h_fs,uint64_t service_chain,int packet_num,int *h_flow_size,int* h_flow_pos);


}
#endif
