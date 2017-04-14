#include "d_network_function_base.cuh"
#include "d_network_function_derived.cuh"
#include "d_nf_item.h"
#include "../d_firewall/d_firewall.cuh"
#include "../d_flowmonitor/d_flow_monitor.cuh"
#include "../d_httpparser/d_http_parser.cuh"
#include "../d_pktcounter/d_pkt_counter.cuh"
#include "Pkt.h"
#include "d_nf_processor.cuh"


__device__ void Init_nfs(struct d_flow_actor_nfs* nfs,struct d_rule* firewall_rules){

    uint8_t i=3;

	nfs->nf[1]=new d_network_function_derived<d_pkt_counter, d_pkt_counter_fs>(1);
	nfs->nf[2]=new d_network_function_derived<d_flow_monitor, d_flow_monitor_fs>(2);
	nfs->nf[3]=new d_network_function_derived<d_firewall, d_firewall_fs>(3);
	nfs->nf[4]=new d_network_function_derived<d_http_parser, d_http_parser_fs>(4);

}


__device__ void Release_nfs(struct d_flow_actor_nfs* nfs){


	delete nfs->nf[1];
	delete nfs->nf[2];
	delete nfs->nf[3];
	delete nfs->nf[4];

}


__device__ uint8_t compute_network_function(uint64_t s, int pos){
  return static_cast<uint8_t>((s>>(8*pos))&0x00000000000000FF);
}

__device__ int compute_service_chain_length(uint64_t s){
  int length = 0;
  bool encounter_zero = false;

#pragma unroll
  for(int i=0; i<8; i++){
    uint8_t nf =
        static_cast<uint8_t>((s>>(8*i))&0x00000000000000FF);
    if(nf>0){
      length+=1;
      if(encounter_zero){
        return -1;
      }
    }
    else{
      encounter_zero = true;
    }
  }
  return length;
}


__global__ void
Runtask(Pkt* pkts, Fs* fs, uint64_t service_chain,int packet_num,int* flow_size,struct d_rule* firewall_rules)
{

	//__shared__ Pkt share_pkts[200*32*10];
	//__shared__ Fs share_fs[200*32];
	struct d_flow_actor_nfs  nfs;
	Init_nfs(&nfs,firewall_rules);
	int chain_len=compute_service_chain_length(service_chain);
	int i = blockDim.x * blockIdx.x + threadIdx.x;

    if (i < packet_num)
    {

    	//int begin=flow_pos[i];
    	int num=flow_size[i];
    //	for(int j=0;j<num;j++){
    //		memcpy(&share_pkts[i+j*32],&pkts[i+j*32],sizeof(Pkt));

    //	}
    //	memcpy(&share_fs[i],&fs[i],sizeof(Fs));
    //	__Syncthreads();


    	//int j=i;
    	for(int j=0;j<num;j++){
    		//pkts[j].full=0;
    		for(int k=0; k<chain_len; k++){
    			int nf_id=compute_network_function(service_chain,k);
    			Pkt* pkt=&(pkts[i+j*32]);
    			int l=nf_id;
    			nfs.nf[l]->nf_logic(pkt,fs[i].fs[l]);
    		}
    		//fs[i].ptr=NULL;

    		//j+=packet_num;


    	}
    }
    Release_nfs(&nfs);

}



void gpu_nf_process(Pkt* h_pkts,Fs* h_fs,uint64_t service_chain,int packet_num,int *h_flow_size,struct d_rule* h_firewall_rules){

    Pkt* pkts;
    Fs* fs;
    int* flow_size;
   // int* flow_pos;
    struct d_rule* firewall_rules;

    cudaHostGetDevicePointer((void **)&pkts, (void *)h_pkts, 0);
   // cudaSetDeviceFlags(cudaDeviceMapHost);
	cudaHostGetDevicePointer((void **)&fs, (void *)h_fs, 0);
	// cudaSetDeviceFlags(cudaDeviceMapHost);
	cudaHostGetDevicePointer((void **)&flow_size, (void *)h_flow_size, 0);
	// cudaSetDeviceFlags(cudaDeviceMapHost);
	cudaHostGetDevicePointer((void **)&firewall_rules, (void *)h_firewall_rules, 0);
	//cudaHostGetDevicePointer((void **)&flow_pos, (void *)h_flow_pos, 0);


	int threadsPerBlock = 256;
    int blocksPerGrid =(packet_num + threadsPerBlock - 1) / threadsPerBlock;
    //printf("CUDA kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock);
    cudaDeviceSynchronize();

    Runtask<<<blocksPerGrid, threadsPerBlock>>>(pkts, fs, service_chain, packet_num,flow_size,firewall_rules);
    //cudaDeviceSynchronize();


}


