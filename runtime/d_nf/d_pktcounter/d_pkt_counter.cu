#include "d_pkt_counter.ch"

#include "../d_base/d_network_function_register.h"


#include <stdio.h>

// For the CUDA runtime routines (prefixed with "cuda_")
#include <cuda_runtime.h>

#include <helper_cuda.h>

bool d_registered_pkt_counter =
    d_static_nf_register::get_register().register_nf<d_pkt_counter, d_pkt_counter_fs>("pkt_counter", 1);
