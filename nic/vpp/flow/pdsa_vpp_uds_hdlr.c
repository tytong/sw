//
// {C} Copyright 2019 Pensando Systems Inc. All rights reserved
//

#include "node.h"

// Return main thread's pointer to FTL v4 flow table
void * pds_flow_get_table4 (void)
{
    pds_flow_main_t *fm = &pds_flow_main;
    
    return fm->table4[0];
}

// Return main thread's pointer to FTL v6 flow table
void * pds_flow_get_table6 (void)
{
    pds_flow_main_t *fm = &pds_flow_main;

    return fm->table6[0];
}