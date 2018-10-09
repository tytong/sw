/*
* Copyright (c) 2018, Pensando Systems Inc.
*/

#ifndef __DEV_HPP__
#define __DEV_HPP__

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "hal_client.hpp"

#ifdef __x86_64__

#include "sdk/pal.hpp"

#define READ_MEM        sdk::lib::pal_mem_read
#define WRITE_MEM       sdk::lib::pal_mem_write
#define MEM_SET(pa, val, sz, flags) { \
    uint8_t v = val; \
    for (size_t i = 0; i < sz; i += sizeof(v)) { \
        sdk::lib::pal_mem_write(pa + i, &v, sizeof(v)); \
    } \
}

#define READ_REG        sdk::lib::pal_reg_read
#define WRITE_REG       sdk::lib::pal_reg_write
static inline uint32_t READ_REG32(uint64_t addr)
{
    uint32_t val;
    sdk::lib::pal_reg_write(addr, &val);
    return val;
}
#define WRITE_REG32(addr, val) { \
    uint32_t v = val; \
    sdk::lib::pal_reg_write(addr, &v); \
}
static inline uint64_t READ_REG64(uint64_t addr)
{
    uint64_t val;
    sdk::lib::pal_reg_read(addr, (uint32_t *)&val, 2);
    return val;
}
#define WRITE_REG64(addr, val) { \
    uint64_t v = val; \
    sdk::lib::pal_reg_write(addr, (uint32_t *)&v, 2); \
}

#define WRITE_DB64      sdk::lib::pal_ring_doorbell

#else
#include "pal.h"
#define READ_MEM        pal_mem_rd
#define WRITE_MEM       pal_mem_wr
#define MEM_SET         pal_memset

#define READ_REG        pal_reg_rd32w
#define WRITE_REG       pal_reg_wr32w
#define READ_REG32      pal_reg_rd32
#define WRITE_REG32     pal_reg_wr32
#define READ_REG64      pal_reg_rd64
#define WRITE_REG64     pal_reg_wr64

#define WRITE_DB64      pal_reg_wr64
#endif

#ifdef __aarch64__
#include "pci_ids.h"
#include "misc.h"
#include "bdf.h"
#include "cfgspace.h"
#include "pciehost.h"
#include "pciehdevices.h"
#include "pciehw.h"
#include "pcieport.h"
#endif

/**
 * Memory Regions
 */
#define DEVCMD_BASE             roundup(0x0138096c00, 4096)
#define NICMGR_BASE             roundup(0x013a096c00, 4096)

/**
 * ADMINQ
 */
#define NICMGR_QTYPE_REQ        0
#define NICMGR_QTYPE_RESP       1

#pragma pack(push, 1)
struct nicmgr_req_desc {
    uint16_t lif;
    uint8_t qtype;
    uint32_t qid;
    uint16_t comp_index;
    uint64_t adminq_qstate_addr;
    uint8_t pad[47];
    uint8_t cmd[64];
};

struct nicmgr_resp_desc {
    uint16_t lif;
    uint8_t qtype;
    uint32_t qid;
    uint16_t comp_index;
    uint64_t adminq_qstate_addr;
    uint8_t pad[47];
    uint8_t comp[64];
};
#pragma pack(pop)

static_assert(sizeof(struct nicmgr_req_desc) == 128, "");
static_assert(sizeof(struct nicmgr_resp_desc) == 128, "");

/**
 * Device Types
 */
enum DeviceType
{
    INVALID,
    MNIC,
    DEBUG,
    ETH,
    ACCEL,
    NVME,
    VIRTIO,
};

/**
 * Device Spec structures
 */

/**
 * Eth Device Spec
 */
struct eth_devspec {
    // Delphi Object Key
    uint64_t dev_uuid;
    // FWD
    uint32_t uplink_id;
    uint64_t vrf_id;
    uint64_t lif_id;
    uint64_t enic_id;
    uint64_t native_l2seg_id;
    // RES
    uint32_t rxq_count;
    uint32_t txq_count;
    uint32_t eq_count;
    uint32_t adminq_count;
    uint32_t intr_base;
    uint32_t intr_count;
    uint64_t mac_addr;
    // DEV
    bool     host_dev;
    uint8_t  pcie_port;
    // RDMA
    bool     enable_rdma;
    uint32_t pte_count;
    uint32_t key_count;
    uint32_t ah_count;
    uint32_t rdma_sq_count;
    uint32_t rdma_rq_count;
    uint32_t rdma_cq_count;
    uint32_t rdma_eq_count;
    uint32_t rdma_adminq_count;
    uint32_t rdma_pid_count;
};

typedef struct dev_cmd_db {
    uint32_t    v;
} dev_cmd_db_t;

/**
 * Devcmd Status codes
 */
enum DevcmdStatus
{
    DEVCMD_SUCCESS,
    DEVCMD_BUSY,
    DEVCMD_ERROR,
    DEVCMD_UNKNOWN,
};

/**
 * Utils
 */
void invalidate_rxdma_cacheline(uint64_t addr);
void invalidate_txdma_cacheline(uint64_t addr);

/**
 * Base Class for devices
 */
class Device {
public:
    virtual void DevcmdPoll() = 0;
    virtual void DevcmdHandler() = 0;
    virtual enum DevcmdStatus CmdHandler(
        void *req, void *req_data,
        void *resp, void *resp_data) = 0;
};

/**
 * Device Manager
 */
class DeviceManager {
public:
    DeviceManager(enum ForwardingMode fwd_mode);
    ~DeviceManager();
    int LoadConfig(std::string path);
    Device *AddDevice(enum DeviceType type, void *dev_spec);

#ifdef __aarch64__
    static void PcieEventHandler(pciehdev_t *pdev,
        const pciehdev_eventdata_t *evd);
#endif
    void DevcmdPoll();
    void AdminQPoll();

private:
    boost::property_tree::ptree spec;
    std::map<uint64_t, Device*> devices; // lif -> device

    // Service Lif Info
    struct lif_info info;
    static struct queue_info qinfo[NUM_QUEUE_TYPES];

    // HAL Info
    HalClient *hal;
    uint32_t lif_id;
    uint64_t lif_handle;

    // AdminQ
    uint64_t req_ring_base;
    uint64_t resp_ring_base;
    uint16_t ring_size;
    uint16_t req_head;
    uint16_t req_tail;
    uint16_t resp_head;
    uint16_t resp_tail;
};

#endif /* __DEV_HPP__ */
