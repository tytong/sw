#include <math.h>
#include "tests.hpp"
#include "hal_if.hpp"
#include "utils.hpp"
#include "nicmgr_if.hpp"
#include "qstate_if.hpp"
#include "logger.hpp"
#include "nic/sdk/platform/utils/qstate_mgr.hpp"
#ifdef ELBA
#include "nic/sdk/platform/elba/elba_state.hpp"
#else
#include "nic/sdk/platform/capri/capri_state.hpp"
#endif
#include <assert.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#define ACCEL_DEV_CMD_ENUMERATE
#include "accel_dev.hpp"
#include "dev.hpp"

using namespace sdk::platform::utils;
using namespace utils;

/*
 * Public symbols for libnicmgr to silence linker warnings
 */
class pciemgr *pciemgr;

namespace nicmgr_if {

#define DEVMGR_ACCEL_DEV_NAME   "accel"

static DeviceManager *devmgr;
static AccelDev     *accel_dev;
static dp_mem_t     *accel_devcmdpa_buf;
static dp_mem_t     *accel_devcmdpa_done;
static dp_mem_t     *accel_devcmdpa_cmdcpl;
static uint64_t     nicmgr_accel_lif_id;

static dev_cmd_regs_t *nicmgr_if_push(dev_cmd_t& cmd);


static int
sdk_trace_cb (uint32_t mod_id, sdk_trace_level_e trace_level,
              const char *format, ...)
{
    char       logbuf[1024];
    va_list    args;

    va_start(args, format);
    vsnprintf(logbuf, sizeof(logbuf), format, args);
    printf("%s", logbuf);
    va_end(args);
    return 0;
}

static void
sdk_init (void)
{
    sdk::lib::logger::init(sdk_trace_cb);
}

int
nicmgr_if_init(void)
{
    devicemgr_cfg_t cfg;
    /*
     * Initializations needed by libnicmgr
     */
    utils::logger::init();
    sdk_init();

    cfg.platform_type = platform_type_t::PLATFORM_TYPE_SIM;
    cfg.cfg_path = std::string(getenv("HAL_CONFIG_PATH"));
    cfg.device_conf_file = FLAGS_nicmgr_config_file;
    cfg.fwd_mode = sdk::lib::FORWARDING_MODE_NONE;
    cfg.micro_seg_en = false;
    cfg.shm_mgr = NULL;
    cfg.EV_A = NULL;

    devmgr = new DeviceManager(&cfg);
    if (!devmgr) {
        fprintf(stderr, "Failed to init device manager\n");
        return -1;
    }
    devmgr->LoadProfile(FLAGS_nicmgr_config_file, false);
    devmgr->HalEventHandler(true);

    /*
     * Interface with the Accel device directly
     */
    accel_dev = (AccelDev *)devmgr->GetDevice(DEVMGR_ACCEL_DEV_NAME);
    if (!accel_dev) {
        fprintf(stderr, "Failed to locate Accel device\n");
        return -1;
    }
    qstate_if::set_nicmgr_pd_client(devmgr->GetPdClient());

    accel_devcmdpa_buf = new dp_mem_t((uint8_t *)accel_dev->DevcmdPageGet(),
                                      1, ACCEL_DEV_PAGE_SIZE,
                                      DP_MEM_TYPE_HBM, DP_MEM_ALLOC_NO_FILL);
    accel_devcmdpa_done =
           accel_devcmdpa_buf->fragment_find(0, offsetof(dev_cmd_regs_t, done) +
                                             sizeof(uint32_t));
    accel_devcmdpa_cmdcpl =
           accel_devcmdpa_buf->fragment_find(offsetof(dev_cmd_regs_t, cpl),
                                             sizeof(dev_cmd_cpl_t));

    printf("%s accel_devcmd_page 0x%lx\n",
           __FUNCTION__, accel_devcmdpa_buf->pa());
    return 0;
}


void
nicmgr_if_fini(void)
{
    if (utils::logger::logger()) {
        utils::logger::logger()->flush();
    }
}


int
nicmgr_if_identify(void)
{
    dev_cmd_regs_t      *dev_cmd;
    dev_identity_t      *identity;
    dev_identify_cpl_t  *cpl;
    dev_cmd_t           cmd = {0};

    cmd.dev_identify.opcode = CMD_OPCODE_IDENTIFY;
    cmd.dev_identify.type = ACCEL_DEV_TYPE_BASE;
    cmd.dev_identify.ver = IDENTITY_VERSION_1;
    dev_cmd = nicmgr_if_push(cmd);
    if (dev_cmd) {

        /*
         * Read the full result buffer including the data portion.
         */
        dev_cmd = (dev_cmd_regs_t *)accel_devcmdpa_buf->read_thru();
        cpl = &dev_cmd->cpl.dev_identify;
        if (cpl->ver != IDENTITY_VERSION_1) {
            printf("%s unsupported identity version %u\n",
                   __FUNCTION__, cpl->ver);
            return -1;
        }
        identity = (dev_identity_t *)dev_cmd->data;
        printf("%s nlifs %u nintrs %u ndbpgs_per_lif %u\n", __FUNCTION__,
               identity->base.nlifs, identity->base.nintrs,
               identity->base.ndbpgs_per_lif);
        if (!identity->base.nlifs || !identity->base.ndbpgs_per_lif) {
            printf("%s num_lifs or db_pages_per_lif ERROR\n", __FUNCTION__);
            return -1;
        }
        return 0;
    }

    return -1;
}


int
nicmgr_if_lif_identify(uint64_t *ret_seq_lif,
                       uint32_t *ret_seq_queues_per_lif,
                       accel_ring_t *ret_accel_ring_tbl,
                       uint32_t accel_ring_tbl_size)
{
    dev_cmd_regs_t      *dev_cmd;
    lif_identity_t      *identity;
    lif_identify_cpl_t  *cpl;
    dev_cmd_t           cmd = {0};

    *ret_seq_lif = 0;
    *ret_seq_queues_per_lif = 0;

    cmd.lif_identify.opcode = CMD_OPCODE_LIF_IDENTIFY;
    cmd.lif_identify.type = ACCEL_LIF_TYPE_BASE;
    cmd.lif_identify.ver = IDENTITY_VERSION_1;
    cmd.lif_identify.lif_index = 0;
    dev_cmd = nicmgr_if_push(cmd);
    if (dev_cmd) {

        /*
         * Read the full result buffer including the data portion.
         */
        dev_cmd = (dev_cmd_regs_t *)accel_devcmdpa_buf->read_thru();
        cpl = &dev_cmd->cpl.lif_identify;
        if (cpl->ver != IDENTITY_VERSION_1) {
            printf("%s unsupported identity version %u\n",
                   __FUNCTION__, cpl->ver);
            return -1;
        }
        identity = (lif_identity_t *)dev_cmd->data;
        nicmgr_accel_lif_id = identity->base.hw_index;
        *ret_seq_lif = nicmgr_accel_lif_id;
        *ret_seq_queues_per_lif = identity->base.queue_count[ACCEL_LOGICAL_QTYPE_SQ];

        printf("%s hw_index 0x%lx seq_queues_per_lif %u\n", __FUNCTION__,
               nicmgr_accel_lif_id, *ret_seq_queues_per_lif);

        if (ret_accel_ring_tbl) {
            if (accel_ring_tbl_size < sizeof(identity->base.accel_ring_tbl)) {
                printf("%s accel_ring_tbl_size %u less than required size %d\n",
                       __FUNCTION__, accel_ring_tbl_size,
                      (int) sizeof(identity->base.accel_ring_tbl));
                return -1;
            }
            memcpy(ret_accel_ring_tbl, identity->base.accel_ring_tbl,
                   accel_ring_tbl_size);
        }

        return 0;
    }

    return -1;
}


int
nicmgr_if_reset(void)
{
    dev_cmd_t   cmd = {0};

    cmd.dev_reset.opcode = CMD_OPCODE_RESET;
    return nicmgr_if_push(cmd) ? 0 : -1;
}


int
nicmgr_if_lif_init(uint64_t seq_lif)
{
    dev_cmd_t   cmd = {0};

    cmd.lif_init.opcode = CMD_OPCODE_LIF_INIT;
    cmd.lif_init.lif_index = 0;
    return nicmgr_if_push(cmd) ? 0 : -1;
}


int
nicmgr_if_lif_reset(uint64_t seq_lif)
{
    dev_cmd_t   cmd = {0};

    cmd.lif_reset.opcode = CMD_OPCODE_LIF_RESET;
    cmd.lif_init.lif_index = 0;
    return nicmgr_if_push(cmd) ? 0 : -1;
}


int
nicmgr_if_admin_queue_init(uint64_t seq_lif,
                           uint16_t log2_num_entries,
                           uint64_t base_addr)
{
    dev_cmd_t   cmd = {0};

    cmd.adminq_init.opcode = CMD_OPCODE_ADMINQ_INIT;
    cmd.adminq_init.lif_index = 0;
    cmd.adminq_init.index = 0;
    cmd.adminq_init.ring_size = log2_num_entries;
    cmd.adminq_init.ring_base = base_addr;
    return nicmgr_if_push(cmd) ? 0 : -1;
}


int
nicmgr_if_seq_queue_init(uint64_t lif,
                         storage_seq_qgroup_t qgroup,
                         uint32_t qid,
                         uint8_t total_wrings,
                         uint8_t host_wrings,
                         uint16_t log2_num_entries,
                         uint64_t base_addr,
                         uint16_t log2_entry_size)
{
    dev_cmd_t   cmd = {0};

    cmd.seq_q_init.opcode = CMD_OPCODE_SEQ_QUEUE_INIT;
    cmd.seq_q_init.lif_index = 0;
    cmd.seq_q_init.index = qid;
    cmd.seq_q_init.qgroup = qgroup;
    cmd.seq_q_init.enable = true;
    cmd.seq_q_init.total_wrings = total_wrings;
    cmd.seq_q_init.host_wrings = host_wrings;
    cmd.seq_q_init.entry_size = log2_entry_size;
    cmd.seq_q_init.wring_size = log2_num_entries;
    cmd.seq_q_init.wring_base = base_addr;
    return nicmgr_if_push(cmd) ? 0 : -1;
}


int
nicmgr_if_seq_queue_init_complete(void)
{
    dev_cmd_t   cmd = {0};

    cmd.seq_q_init_complete.opcode = CMD_OPCODE_SEQ_QUEUE_INIT_COMPLETE;
    cmd.seq_q_init_complete.lif_index = 0;
    return nicmgr_if_push(cmd) ? 0 : -1;
}


static dev_cmd_regs_t *
nicmgr_if_push(dev_cmd_t& cmd)
{
    volatile dev_cmd_regs_t  *dev_cmd;
    dev_cmd_cpl_t   *dev_cpl;
    bool            cmd_complete = false;

    accel_devcmdpa_buf->clear();
    dev_cmd = (dev_cmd_regs_t *)accel_devcmdpa_buf->read();

    while (!cmd_complete) {
        dev_cmd->doorbell = true;
        dev_cmd->done = false;
        memcpy((void *)&dev_cmd->cmd, &cmd, sizeof(dev_cmd->cmd));
        accel_devcmdpa_buf->fragment_find(0,
                            offsetof(dev_cmd_regs_t, cmd) +
                            sizeof(dev_cmd_t))->write_thru();

        /*
         * Sending command thru devcmdpa so ring its doorbell.
         *
         * Note: we don't have a way to trigger Accel timer so just go
         * directly to its DevcmdHandler.
         */
        accel_dev->DevcmdHandler();

        accel_devcmdpa_done->read_thru();
        if (!dev_cmd->done) {
            printf("%s command %u failed done %u\n", __FUNCTION__,
                   dev_cmd->cmd.nop.opcode, dev_cmd->done);
            return nullptr;
        }

        dev_cpl = (dev_cmd_cpl_t *)accel_devcmdpa_cmdcpl->read_thru();
        switch (dev_cpl->status) {

        case ACCEL_RC_EAGAIN:
            usleep(10000);
            break;

        case ACCEL_RC_SUCCESS:
            cmd_complete = true;
            break;

        default:
            printf("%s command %u failed status %u\n", __FUNCTION__,
                   dev_cmd->cmd.nop.opcode, dev_cpl->status);
            return nullptr;
        }
    }

    return (dev_cmd_regs_t *)dev_cmd;
}

}  // namespace nicmgr_if



