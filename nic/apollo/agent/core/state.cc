//-----------------------------------------------------------------------------
// {C} Copyright 2019 Pensando Systems Inc. All rights reserved
//-----------------------------------------------------------------------------

#include "nic/apollo/agent/core/core.hpp"
#include "nic/apollo/agent/core/state.hpp"

namespace core {

#define ADD_TO_DB(obj, key, value) {                                            \
    if (key == NULL) {                                                          \
        return SDK_RET_ERR;                                                     \
    }                                                                           \
    obj##_map()->insert(make_pair((uint32_t)key->id, value));                   \
    return SDK_RET_OK;                                                          \
}

#define FIND_IN_DB(obj, key) {                                                  \
    if (key == NULL) {                                                          \
        return NULL;                                                            \
    }                                                                           \
    obj##_db_t::const_iterator iterator = obj##_map()->find((uint32_t)key->id); \
    if (iterator == obj##_map()->end()) {                                       \
        return NULL;                                                            \
    }                                                                           \
    return iterator->second;                                                    \
}

#define DEL_FROM_DB(obj, key) {                                                 \
    obj##_map()->erase((uint32_t)key->id);                                      \
    return true;                                                                \
}

class agent_state *g_state;

//------------------------------------------------------------------------------
// init() function to instantiate all the config db init state
//------------------------------------------------------------------------------
bool
cfg_db::init(void) {
    void *mem;

    mem = CALLOC(MEM_ALLOC_ID_INFRA, sizeof(tep_db_t));
    if (mem == NULL) {
        return false;
    }
    tep_map_ = new(mem) tep_db_t();

    mem = CALLOC(MEM_ALLOC_ID_INFRA, sizeof(pcn_db_t));
    if (mem == NULL) {
        return false;
    }
    pcn_map_ = new(mem) pcn_db_t();

    mem = CALLOC(MEM_ALLOC_ID_INFRA, sizeof(subnet_db_t));
    if (mem == NULL) {
        return false;
    }
    subnet_map_ = new(mem) subnet_db_t();

    mem = CALLOC(MEM_ALLOC_ID_INFRA, sizeof(vnic_db_t));
    if (mem == NULL) {
        return false;
    }
    vnic_map_ = new(mem) vnic_db_t();

    mem = CALLOC(MEM_ALLOC_ID_INFRA, sizeof(route_table_db_t));
    if (mem == NULL) {
        return false;
    }
    route_table_map_ = new(mem) route_table_db_t();

    slabs_[SLAB_ID_PCN] =
        slab::factory("pcn", SLAB_ID_PCN, sizeof(pds_vcn_spec_t),
                      16, true, true, true);
    slabs_[SLAB_ID_SUBNET] =
        slab::factory("subnet", SLAB_ID_SUBNET, sizeof(pds_subnet_spec_t),
                      16, true, true, true);
    slabs_[SLAB_ID_TEP] =
        slab::factory("tep", SLAB_ID_TEP, sizeof(pds_tep_spec_t),
                      16, true, true, true);
    slabs_[SLAB_ID_VNIC] =
        slab::factory("vnic", SLAB_ID_SUBNET, sizeof(pds_vnic_spec_t),
                      16, true, true, true);
    slabs_[SLAB_ID_ROUTE] =
        slab::factory("route_table", SLAB_ID_SUBNET,
                      sizeof(pds_route_table_spec_t),
                      16, true, true, true);
    return true;
}

//------------------------------------------------------------------------------
// (private) constructor method
//------------------------------------------------------------------------------
cfg_db::cfg_db() {
    tep_map_ = NULL;
    pcn_map_ = NULL;
    subnet_map_ = NULL;
    vnic_map_ = NULL;
    route_table_map_ = NULL;
    memset(&device_, 0, sizeof(pds_device_spec_t));
    memset(slabs_, 0, sizeof(slabs_));
}

//------------------------------------------------------------------------------
// factory method
//------------------------------------------------------------------------------
cfg_db *
cfg_db::factory(void) {
    void *mem;
    cfg_db *db;

    mem = CALLOC(MEM_ALLOC_ID_INFRA, sizeof(cfg_db));
    if (mem) {
        db = new(mem) cfg_db();
        if (db->init() == false) {
            db->~cfg_db();
            FREE(MEM_ALLOC_ID_INFRA, mem);
            return NULL;
        }
        return db;
    }
    return NULL;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
cfg_db::~cfg_db() {
    uint32_t i;

    FREE(MEM_ALLOC_ID_INFRA, tep_map_);
    FREE(MEM_ALLOC_ID_INFRA, pcn_map_);
    FREE(MEM_ALLOC_ID_INFRA, subnet_map_);
    FREE(MEM_ALLOC_ID_INFRA, vnic_map_);
    FREE(MEM_ALLOC_ID_INFRA, route_table_map_);
    for (i = SLAB_ID_MIN; i < SLAB_ID_MAX; i++) {
        if (slabs_[i]) {
            slab::destroy(slabs_[i]);
        }
    }
}

//------------------------------------------------------------------------------
// destroy method
//------------------------------------------------------------------------------
void
cfg_db::destroy(cfg_db *cfg_db) {
    if (!cfg_db) {
        return;
    }
    cfg_db->~cfg_db();
    FREE(MEM_ALLOC_ID_INFRA, cfg_db);
}

//------------------------------------------------------------------------------
// constructor method
//------------------------------------------------------------------------------
agent_state::agent_state(void) {
    cfg_db_ = cfg_db::factory();
    SDK_ASSERT_GOTO(cfg_db_, error);
    return;

error:
    cleanup();
}

//------------------------------------------------------------------------------
// common cleanup method
//------------------------------------------------------------------------------
void
agent_state::cleanup(void) {
    if (cfg_db_) {
        cfg_db::destroy(cfg_db_);
    }
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
agent_state::~agent_state() {
    cleanup();
}

sdk_ret_t
agent_state::add_to_tep_db(uint32_t key,
                     pds_tep_spec_t *spec) {
    tep_map()->insert(make_pair(key, spec));
    return SDK_RET_OK;
}

pds_tep_spec_t *
agent_state::find_in_tep_db(uint32_t key) {
    tep_db_t::const_iterator iterator = tep_map()->find(key);
    if (iterator == tep_map()->end()) {
        return NULL;
    }
    return iterator->second;
}

bool
agent_state::del_from_tep_db(uint32_t key) {
    tep_map()->erase(key);
    return true;
}

sdk_ret_t
agent_state::add_to_pcn_db(pds_vcn_key_t *key, pds_vcn_spec_t *spec) {
    ADD_TO_DB(pcn, key, spec);
}

pds_vcn_spec_t *
agent_state::find_in_pcn_db(pds_vcn_key_t *key) {
    FIND_IN_DB(pcn, key);
}

bool
agent_state::del_from_pcn_db(pds_vcn_key_t *key) {
    DEL_FROM_DB(pcn, key);
}

sdk_ret_t
agent_state::add_to_subnet_db(pds_subnet_key_t *key,
                        pds_subnet_spec_t *spec) {
    ADD_TO_DB(subnet, key, spec);
}

pds_subnet_spec_t *
agent_state::find_in_subnet_db(pds_subnet_key_t *key) {
    FIND_IN_DB(subnet, key);
}

bool
agent_state::del_from_subnet_db(pds_subnet_key_t *key) {
    DEL_FROM_DB(subnet, key);
}

sdk_ret_t
agent_state::add_to_vnic_db(pds_vnic_key_t *key, pds_vnic_spec_t *spec) {
    ADD_TO_DB(vnic, key, spec);
}

pds_vnic_spec_t *
agent_state::find_in_vnic_db(pds_vnic_key_t *key) {
    FIND_IN_DB(vnic, key);
}

bool
agent_state::del_from_vnic_db(pds_vnic_key_t *key) {
    DEL_FROM_DB(vnic, key);
}

sdk_ret_t
agent_state::add_to_route_table_db(pds_route_table_key_t *key,
                             pds_route_table_spec_t *spec) {
    ADD_TO_DB(route_table, key, spec);
}

pds_route_table_spec_t *
agent_state::find_in_route_table_db(pds_route_table_key_t *key) {
    FIND_IN_DB(route_table, key);
}

bool
agent_state::del_from_route_table_db(pds_route_table_key_t *key) {
    DEL_FROM_DB(route_table, key);
}

class agent_state *
agent_state::state(void) {
    return g_state;
}

sdk_ret_t
agent_state::init(void) {
    void *mem;

    mem = CALLOC(MEM_ALLOC_ID_INFRA, sizeof(agent_state));
    if (mem) {
        g_state = new(mem) agent_state();
    }
    SDK_ASSERT_RETURN((g_state != NULL), SDK_RET_ERR);
    return SDK_RET_OK;
}

}    // namespace core
