#include "d_flow_monitor.ch"

#include "../d_base/d_network_function_register.h"

bool d_registered_flow_monitor =
    d_static_nf_register::get_register().register_nf<d_flow_monitor, d_flow_monitor_fs>("flow_monitor", 2);
