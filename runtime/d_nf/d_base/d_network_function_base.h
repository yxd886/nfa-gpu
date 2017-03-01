#ifndef D_NETWORK_FUNCTION_H
#define D_NETWORK_FUNCTION_H
// CUDA runtime
#include <cuda_runtime.h>

// helper functions and utilities to work with CUDA
#include <helper_functions.h>
#include <helper_cuda.h>

class d_network_function_base{
public:

  explicit d_network_function_base(size_t nf_state_size, uint8_t nf_id)
    : nf_state_size_(nf_state_size), array_(0), ring_buf_(), nf_id_(nf_id){
  }
/*
  inline void init_ring(size_t nf_state_num){
    array_ = reinterpret_cast<char*>(mem_alloc(nf_state_size_*nf_state_num));
    ring_buf_.init(nf_state_num);
    for(size_t i=0; i<nf_state_num; i++){
      ring_buf_.push(array_+i*nf_state_size_);
    }
  }
  */

  virtual ~d_network_function_base(){}

  inline char* allocate(){
	cudaError_t err = cudaSuccess;
	char* d_p;
	err = cudaMalloc((void **)&d_p, nf_state_size_);
	if(err!=cudaSuccess){
		LOG(INFO)<<"allocate d_fs_state fail";
		exit(-1);
	}
	return d_p;
  }

  inline bool deallocate(char* state_ptr){

	cudaError_t err = cudaSuccess;
	err = cudaFree(state_ptr);
	if(err!=cudaSuccess){
		LOG(INFO)<<"deallocate d_fs_state fail";
		exit(-1);
	}else{
		return true;
	}
  }

  virtual void nf_logic(char* pkt, char* state_ptr) = 0;

  inline size_t get_nf_state_size(){
    return nf_state_size_;
  }

  inline uint8_t get_nf_id(){
    return nf_id_;
  }

private:
  size_t nf_state_size_;
  char* array_;
  //simple_ring_buffer<char> ring_buf_;
  uint8_t nf_id_;
};

#endif
