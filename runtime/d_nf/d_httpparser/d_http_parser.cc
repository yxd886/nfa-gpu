#include "d_http_parser.h"

#include "../d_base/d_network_function_register.h"

bool d_registered_http_parser =
    d_static_nf_register::get_register().register_nf<d_http_parser, d_http_parser_fs>("http_parser", 4);
