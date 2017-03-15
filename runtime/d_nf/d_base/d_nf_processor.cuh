#ifndef D_NF_PROCESSOR_H
#define D_NF_PROCESSOR_H

extern "C"{

void gpu_nf_process(Pkt* pkts,Fs* fs,uint64_t service_chain,int packet_num);


}
#endif
