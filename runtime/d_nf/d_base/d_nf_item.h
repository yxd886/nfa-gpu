#ifndef D_NF_ITEM_H
#define D_NF_ITEM_H

#include "d_network_function_base.h"

static constexpr int max_chain_length = 8;;

struct d_flow_actor_nfs{
  d_network_function_base* nf[max_chain_length];
};

struct d_flow_actor_fs{
  char* nf_flow_state_ptr[max_chain_length];
};

struct d_flow_actor_fs_size{
  size_t nf_flow_state_size[max_chain_length];
};

static_assert(sizeof(d_flow_actor_nfs) == 64, "flow_actor_nfs can't fit into a cache line");

static_assert(sizeof(d_flow_actor_fs) == 64, "flow_actor_fs can't fit into a cache line");

static_assert(sizeof(d_flow_actor_fs_size) == 64, "flow_actor_fs_size can't fit into a cache line");

static_assert(std::is_pod<d_flow_actor_nfs>::value, "flow_actor_nfs is not pod");

static_assert(std::is_pod<d_flow_actor_fs>::value, "flow_actor_fs is not pod");

static_assert(std::is_pod<d_flow_actor_fs_size>::value, "flow_actor_fs_size is not pod");


#endif
