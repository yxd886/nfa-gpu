#ifndef D_NETWORK_FUNCTION_DERIVED_H
#define D_NETWORK_FUNCTION_DERIVED_H

#include "d_network_function_base.h"

template<class TNF, class TNFState, class... TNFArgs>
class d_network_function_derived : public d_network_function_base{
public:
  d_network_function_derived(uint8_t nf_id, TNFArgs&&... tnf_args) :
    d_network_function_base(sizeof(TNFState), nf_id),
    nf_instance_(std::forward<TNFArgs>(tnf_args)...){
    static_assert(std::is_pod<TNFState>::value, "NF flow state is not a POD Type");
  }

  inline void nf_logic(bess::Packet* pkt, char* state_ptr) override{
    nf_instance_.nf_logic_impl(pkt, reinterpret_cast<TNFState*>(state_ptr));
  }

  ~d_network_function_derived() = default;

private:
  TNF nf_instance_;
};

#endif
