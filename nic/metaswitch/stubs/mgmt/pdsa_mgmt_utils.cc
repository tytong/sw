// {C} Copyright 2019 Pensando Systems Inc. All rights reserved
// Purpose: Common helper APIs for metaswitch stub programming 

#include "nic/metaswitch/stubs/mgmt/pdsa_mgmt_utils.hpp"
#include <iostream>

#define SHARED_DATA_TYPE CSS_LOCAL

using namespace std;

NBB_VOID 
pdsa_convert_ip_addr_to_amb_ip_addr (ip_addr_t     pdsa_ip_addr, 
                                     NBB_LONG      *type, 
                                     NBB_ULONG     *len, 
                                     NBB_BYTE      *amb_ip_addr)
{
    switch (pdsa_ip_addr.af)
    {
        case IP_AF_IPV4:
            *type = AMB_INETWK_ADDR_TYPE_IPV4;
            *len = AMB_MAX_IPV4_ADDR_LEN;
            break;

        case IP_AF_IPV6:
            *type = AMB_INETWK_ADDR_TYPE_IPV6;
            *len = AMB_MAX_IPV6_ADDR_LEN;
            break;

        default:
            *type = *len = 0;
            return;
    }

    NBB_MEMCPY (amb_ip_addr, &pdsa_ip_addr.addr, *len);
    return;
}

NBB_VOID
pdsa_convert_long_to_pdsa_ipv4_addr (NBB_ULONG ip, ip_addr_t *pdsa_ip_addr)
{
    pdsa_ip_addr->af            = IP_AF_IPV4;
    pdsa_ip_addr->addr.v4_addr  = htonl(ip);
}

