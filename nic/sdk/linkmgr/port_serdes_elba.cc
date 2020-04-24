// {C} Copyright 2017 Pensando Systems Inc. All rights reserved

#include "linkmgr.hpp"
#include "linkmgr_rw.hpp"
#include "port_serdes.hpp"
#include "linkmgr_types.hpp"
#include "linkmgr_internal.hpp"
#include "third-party/avago/build/include/aapl/aapl.h"
#include "platform/pal/include/pal.h"

namespace sdk {
namespace linkmgr {

// global aapl info
Aapl_t *aapl = NULL;

#define serdes_spico_int_check_hw(sbus_addr, int_code, int_data)  \
    avago_spico_int_check(aapl, __func__, __LINE__,         \
                          sbus_addr, int_code, int_data);   \

//---------------------------------------------------------------------------
// HAPS platform methods
//---------------------------------------------------------------------------

int
serdes_output_enable_haps (uint32_t sbus_addr, bool enable)
{
    // sbus_addr repurposed as instance id for HAPS
    uint32_t chip = sbus_addr;
    uint64_t addr = MXP_BASE_HAPS +
                    (chip * MXP_INST_STRIDE_HAPS) +
                    PHY_RESET_OFFSET_HAPS;
    uint32_t data = 0x0;

    if (enable == false) {
        data = 0x1;
    }

    // for HAPS, serdes enable/disable is un-reset/reset
    WRITE_REG_BASE(chip, addr, data);

    return SDK_RET_OK;
}

int
serdes_cfg_haps (uint32_t sbus_addr, serdes_info_t *serdes_info)
{
    // for HAPS, serdes cfg is to un-reset serdes
    serdes_output_enable_haps (sbus_addr, true);
    return SDK_RET_OK;
}

//---------------------------------------------------------------------------
// default methods
//---------------------------------------------------------------------------

int
serdes_cfg_default (uint32_t sbus_addr, serdes_info_t *serdes_info)
{
    return SDK_RET_OK;
}

bool
serdes_signal_detect_default (uint32_t sbus_addr)
{
    return true;
}

bool
serdes_rdy_default (uint32_t sbus_addr)
{
    return true;
}

int
serdes_output_enable_default (uint32_t sbus_addr, bool enable)
{
    return SDK_RET_OK;
}

int
serdes_tx_rx_enable_default (uint32_t sbus_addr, bool enable)
{
    return SDK_RET_OK;
}

int
serdes_sbus_reset_default (uint32_t sbus_addr, int hard)
{
    return SDK_RET_OK;
}

int
serdes_spico_reset_default (uint32_t sbus_addr)
{
    return SDK_RET_OK;
}

int
serdes_eye_check_default(uint32_t sbus_addr, uint32_t *values)
{
    for (int i = 0; i < (2*MAX_SERDES_EYE_HEIGHTS); ++i) {
        values[i] = 0x7f; // some valid value
    }
    return 0;
}

int
serdes_eye_get_default(uint32_t sbus_addr, int eye_type)
{
    return 0;
}

int
serdes_ical_start_default (uint32_t sbus_addr, port_speed_t serdes_speed)
{
    return 0;
}

int
serdes_pcal_start_default(uint32_t sbus_addr)
{
    return 0;
}

int
serdes_pcal_continuous_start_default(uint32_t sbus_addr)
{
    return 0;
}

int
serdes_dfe_status_default(uint32_t sbus_addr)
{
    return 1;
}

int
serdes_rx_lpbk_default(uint32_t sbus_addr, bool enable)
{
    return 0;
}

int
serdes_spico_status_default(uint32_t sbus_addr)
{
    return 0;
}

int
serdes_get_rev_default(uint32_t sbus_addr)
{
    return 0;
}

uint32_t
serdes_get_eng_id_default(uint32_t sbus_addr)
{
    return 0;
}

int
serdes_get_build_id_default(uint32_t sbus_addr)
{
    return 0;
}

bool
serdes_spico_crc_default(uint32_t sbus_addr)
{
    return true;
}

int
serdes_spico_upload_default (uint32_t sbus_addr, const char* file_name)
{
    return 0;
}

void
serdes_aacs_start_default(int port)
{
    return;
}

int serdes_set_tx_eq_default (uint32_t sbus_addr, uint32_t amp,
                              uint32_t pre,       uint32_t post)
{
    return 0;
}

int serdes_set_rx_term_default (uint32_t sbus_addr, uint8_t val)
{
    return 0;
}

bool
serdes_spico_int_default (uint32_t sbus_addr, int int_code, int int_data)
{
    return true;
}

uint32_t
serdes_get_errors_default (uint32_t sbus_addr, bool clear)
{
    return 0;
}

int
serdes_prbs_start_default (uint32_t sbus_addr, serdes_info_t *serdes_info)
{
    return 0;
}

int
serdes_an_init_default (uint32_t sbus_addr, serdes_info_t *serdes_info)
{
    return 0;
}

int
serdes_an_start_default (uint32_t sbus_addr, serdes_info_t *serdes_info,
                         uint32_t user_cap, bool fec_ability,
                         uint32_t fec_request)
{
    return 0;
}

bool
serdes_an_wait_hcd_default(uint32_t sbus_addr)
{
    return true;
}

int
serdes_an_hcd_read_default (uint32_t sbus_addr)
{
    return 0x0;
}

int
serdes_an_hcd_cfg_default (uint32_t sbus_addr, uint32_t *sbus_addr_arr)
{
    return 0;
}

int
serdes_invert_cfg_default (uint32_t sbus_addr, serdes_info_t *serdes_info)
{
    return 0;
}

int
serdes_an_fec_enable_read_default (uint32_t sbus_addr)
{
    return 0x0;
}

int
serdes_an_rsfec_enable_read_default (uint32_t sbus_addr)
{
    return 0x0;
}

int serdes_an_core_status_default (uint32_t sbus_addr)
{
    return 0x34;
}

//---------------------------------------------------------------------------
// HW methods
//---------------------------------------------------------------------------

uint32_t
sbus_access (uint32_t sbus_addr,
             unsigned char reg_addr,
             unsigned char command,
             uint *sbus_data)
{
    //uint32_t chip_id     = 0;
    uint32_t status      = 0;
    uint32_t ring_number = ((sbus_addr >> 8 ) & 0x0f);
    uint32_t sbus_id     = (sbus_addr         & 0xff);

    //Required commands are:
    //1: write
    //2: read
    //0: reset
    //sbus_data: Pointer to the SBus data to write. Results of SBus read operations will be placed here.

    pal_wr_lock(SBUSLOCK);

    switch (command) {
    case 0x0:   // sbus reset
        /*
        SDK_LINKMGR_TRACE_DEBUG("sbus_reset."
                " sbus_addr 0x%x, reg_addr: 0x%x, cmd: 0x%x, sbus_data: 0x%x,"
                " ring: 0x%x, sbus_id: 0x%x",
                sbus_addr, reg_addr, command, *sbus_data, ring_number, sbus_id);
        */

        if(ring_number == 0) {
            // TBD-ELBA_RELEASE cap_pp_sbus_reset(chip_id, sbus_id);
        } else {
            SDK_LINKMGR_TRACE_ERR("NO-OP for ring: %d", ring_number);
            // cap_ms_sbus_reset(chip_id, sbus_id);
        }
        status = 1;
        break;


    case 0x1:   // sbus write
        /*
        SDK_LINKMGR_TRACE_DEBUG("sbus_write."
                " sbus_addr 0x%x, reg_addr: 0x%x, cmd: 0x%x, sbus_data: 0x%x,"
                " ring: 0x%x, sbus_id: 0x%x",
                sbus_addr, reg_addr, command, *sbus_data, ring_number, sbus_id);
        */

        if(ring_number == 0) {
            // TBD-ELBA_RELEASE cap_pp_sbus_write(chip_id, sbus_id, reg_addr, *sbus_data);
        } else {
            SDK_LINKMGR_TRACE_ERR("NO-OP for ring: %d", ring_number);
            // cap_ms_sbus_write(chip_id, sbus_id, reg_addr, *sbus_data);
        }
        status = 1;
        break;

    case 0x2:   // sbus read
        if(ring_number == 0) {
            // TBD-ELBA_RELEASE *sbus_data = cap_pp_sbus_read(chip_id, sbus_id, reg_addr);
        } else {
            SDK_LINKMGR_TRACE_ERR("NO-OP for ring: %d", ring_number);
            // *sbus_data = cap_ms_sbus_read(chip_id, sbus_id, reg_addr);
        }

        /*
        SDK_LINKMGR_TRACE_DEBUG("sbus_read."
                " sbus_addr 0x%x, reg_addr: 0x%x, cmd: 0x%x, sbus_data: 0x%x,"
                " ring: 0x%x, sbus_id: 0x%x, sbus:data: 0x%x",
                sbus_addr, reg_addr, command, *sbus_data, ring_number, sbus_id,
                *sbus_data);
        */

        status = 1;
        break;

    default:
        SDK_LINKMGR_TRACE_DEBUG("Invalid cmd."
                " sbus_addr 0x%x, reg_addr: 0x%x, cmd: 0x%x, sbus_data: 0x%x,"
                " ring: 0x%x, sbus_id: 0x%x",
                sbus_addr, reg_addr, command, *sbus_data, ring_number, sbus_id);

        status = 0;
        break;
    }

    pal_wr_unlock(SBUSLOCK);

    return status;
}

uint32_t
aapl_sbus_access (Aapl_t *aapl,
                  uint32_t sbus_addr,
                  unsigned char reg_addr,
                  unsigned char command,
                  uint *sbus_data)
{
    return sbus_access(sbus_addr, reg_addr, command, sbus_data);
}

void
serdes_get_ip_info(int chip_reset)
{
    if (aapl != NULL) {
        /* Gather information about the device and place into AAPL struct */
        aapl_get_ip_info(aapl, chip_reset);
    }
}

int
serdes_sbm_set_sbus_clock_divider (int val)
{
    if (aapl != NULL) {
        SDK_LINKMGR_TRACE_DEBUG("SBUS master clock divider: %d", val);
        return avago_sbm_set_sbus_clock_divider(aapl, 0, val);
    }
    return 0;
}

static void
aapl_log_fn(Aapl_t *aapl, Aapl_log_type_t log_type,
            const char *buf, size_t new_item_len)
{
    switch (log_type) {
    case Aapl_log_type_t::AVAGO_ERR:
        SDK_LINKMGR_TRACE_ERR_SIZE(new_item_len, "%s", buf);
        break;
     case Aapl_log_type_t::AVAGO_WARNING:
        SDK_LINKMGR_TRACE_ERR_SIZE(new_item_len, "%s", buf);
        break;
     case Aapl_log_type_t::AVAGO_INFO:
        SDK_LINKMGR_TRACE_DEBUG_SIZE(new_item_len, "%s", buf);
        break;
     default:
        SDK_LINKMGR_TRACE_DEBUG_SIZE(new_item_len, "%s", buf);
        break;
    }
}

bool
serdes_spico_int_hw (uint32_t sbus_addr, int int_code, int int_data)
{
    return serdes_spico_int_check_hw(sbus_addr, int_code, int_data);
}

Aapl_t*
serdes_global_init_hw(uint32_t     jtag_id,
                      int          num_sbus_rings,
                      bool         aacs_server_en,
                      bool         aacs_connect,
                      int          port,
                      std::string& ip)
{
    Aapl_comm_method_t comm_method = AVAGO_SBUS;

    // TODO read from catalog
    int      num_chips      = 1;

    (void)aacs_server_en;

    Aapl_t *aapl = aapl_construct();

    // set the appl init params
    aapl->communication_method = comm_method;
    aapl->jtag_idcode[0]       = jtag_id;
    aapl->sbus_rings           = num_sbus_rings;
    aapl->chips                = num_chips;
    aapl->debug                = 0;
    aapl->verbose              = 0;
    aapl->serdes_int_timeout   = 3000;

    // disable logging to default stdout/stderr since we will log to file
    aapl->enable_stream_logging = 0;
    aapl->enable_stream_err_logging = 0;

    if (aacs_connect == true) {
        aapl->aacs  = aacs_connect;
    } else {
        // register access methods
        aapl_register_sbus_fn(aapl, aapl_sbus_access, NULL, NULL);

        // register AAPL logging
        aapl_register_logging_fn(aapl, &aapl_log_fn, NULL, NULL);

        // aapl_register_spico_int_fn(aapl, spico_int);
    }

    // Make a connection to the device
    aapl_connect(aapl, ip.c_str(), port);

    if(aapl->return_code < 0) {
        aapl_destruct(aapl);
        return NULL;
    }

    return aapl;
}

bool
serdes_signal_detect_hw (uint32_t sbus_addr)
{
    int64_t data[4] = {0};

    avago_serdes_get_rx_data(aapl, sbus_addr, data);

    SDK_LINKMGR_TRACE_DEBUG("sbus_addr: %u, Rx Data: 0x%lx 0x%lx 0x%lx 0x%lx",
                            sbus_addr, data[0], data[1], data[2], data[3]);
#if 0
    // TODO workaround
    FILE *d_fp = fopen("/aapl_enable_ei", "r");
    if (d_fp) {
        int  threshold = 3;
        bool idle      = false;

        fscanf(d_fp, "%d", &threshold);

        avago_serdes_initialize_signal_ok(aapl, sbus_addr, threshold);

        // check for electricle idle
        idle = avago_serdes_get_electrical_idle (aapl, sbus_addr);

        if (idle == false) {
            SDK_LINKMGR_TRACE_DEBUG("Signal detected, EI not detected on sbus: "
                            "0x%x, threshold: %d", sbus_addr, threshold);
        } else {
            SDK_LINKMGR_TRACE_DEBUG("Signal NOT detected on sbus: 0x%x, "
                            "threshold: %d", sbus_addr, threshold);
        }

        // Disable electrical idle
        avago_spico_int(aapl, sbus_addr, 0x20, 0x0);

        fclose(d_fp);
    }
#endif

    if (data[0] == 0xfffff || data[0] == 0x0 ||
        data[1] == 0xfffff || data[1] == 0x0 ||
        data[2] == 0xfffff || data[2] == 0x0 ||
        data[3] == 0xfffff || data[3] == 0x0) {
        return false;
    }

    return true;
}

bool
serdes_rdy_hw (uint32_t sbus_addr)
{
    int tx_rdy = 0;
    int rx_rdy = 0;

    avago_serdes_get_tx_rx_ready(aapl, sbus_addr, &tx_rdy, &rx_rdy);

    if (tx_rdy == 0 || rx_rdy == 0) {
        return false;
    }

    return true;
}

int
serdes_output_enable_hw (uint32_t sbus_addr, bool enable)
{
    if (avago_serdes_set_tx_output_enable(aapl, sbus_addr, enable) == -1) {
        return SDK_RET_ERR;
    }

    return SDK_RET_OK;
}

int
serdes_tx_rx_enable_hw (uint32_t sbus_addr, bool enable)
{
    // To be set only during init stage.
    // Need to wait for Tx/Rx ready once set
    return SDK_RET_OK;
}

int
serdes_sbus_reset_hw (uint32_t sbus_addr, int hard)
{
    avago_sbus_reset(aapl, sbus_addr, hard);
    return 0;
}

int
serdes_spico_reset_hw (uint32_t sbus_addr)
{
    int rc = avago_spico_reset (aapl, sbus_addr);

    if (rc < 0) {
        SDK_LINKMGR_TRACE_ERR("spico reset failed for sbus: %u", sbus_addr);
    }

    return rc;
}

int
serdes_spico_upload_hw (uint32_t sbus_addr, const char* filename)
{
    int rc = avago_spico_upload_file(aapl, sbus_addr, 0, filename);

    if (rc < 0) {
        SDK_LINKMGR_TRACE_ERR("spico upload failed for sbus: %u", sbus_addr);
    }

    return rc;
}

// static int
// serdes_pre_ical_start_hw (uint32_t sbus_addr)
// {
//     int int_code;
//     int int_data;
//     int int_ret;

//     int_code = 0x18;
//     int_data = 0x7;
//     int_ret = serdes_spico_int_check_hw(sbus_addr, int_code, int_data);
//     if (int_ret == false) {
//         SDK_LINKMGR_TRACE_ERR("Failed to send interrupt to serdes. "
//                               "sbus_addr %u, int_code 0x%x, int_data 0x%x",
//                               sbus_addr, int_code, int_data);
//         return -1;
//     }

//     int_code = 0x19;
//     int_data = 0x2710;
//     int_ret = serdes_spico_int_check_hw(sbus_addr, int_code, int_data);
//     if (int_ret == false) {
//         SDK_LINKMGR_TRACE_ERR("Failed to send interrupt to serdes. "
//                               "sbus_addr %u, int_code 0x%x, int_data 0x%x",
//                               sbus_addr, int_code, int_data);
//         return -1;
//     }

//     int_code = 0x19;
//     int_data = 0x0;
//     int_ret = serdes_spico_int_check_hw(sbus_addr, int_code, int_data);
//     if (int_ret == false) {
//         SDK_LINKMGR_TRACE_ERR("Failed to send interrupt to serdes. "
//                               "sbus_addr %u, int_code 0x%x, int_data 0x%x",
//                               sbus_addr, int_code, int_data);
//         return -1;
//     }
//     return 0;
// }

int
serdes_ical_start_hw (uint32_t sbus_addr, port_speed_t serdes_speed)
{
    Avago_serdes_dfe_tune_t dfe;

    avago_serdes_tune_init(aapl, &dfe);
    dfe.tune_mode = Avago_serdes_dfe_tune_mode_t::AVAGO_DFE_ICAL;

    // 10G ICAL would take ~1s.
    // issue additional interrupts to speed up ICAL for 10G
    if (serdes_speed == port_speed_t::PORT_SPEED_10G) {
        // serdes_pre_ical_start_hw(sbus_addr);
    }
    avago_serdes_tune(aapl, sbus_addr, &dfe);

    return 0;
}

int
serdes_pcal_start_hw(uint32_t sbus_addr)
{
    Avago_serdes_dfe_tune_t dfe;

    avago_serdes_tune_init(aapl, &dfe);
    dfe.tune_mode = Avago_serdes_dfe_tune_mode_t::AVAGO_DFE_PCAL;

    avago_serdes_tune(aapl, sbus_addr, &dfe);

    return 0;
}

int
serdes_pcal_continuous_start_hw(uint32_t sbus_addr)
{
    Avago_serdes_dfe_tune_t dfe;

    avago_serdes_tune_init(aapl, &dfe);
    dfe.tune_mode = Avago_serdes_dfe_tune_mode_t::AVAGO_DFE_START_ADAPTIVE;

    avago_serdes_tune(aapl, sbus_addr, &dfe);

    return 0;
}

int
serdes_dfe_status_hw(uint32_t sbus_addr)
{
    return avago_serdes_dfe_wait_timeout(aapl, sbus_addr, 0);
}

int serdes_eye_check_hw (uint32_t sbus_addr, uint32_t *values)
{
    Avago_serdes_dfe_state_t *dfe = avago_serdes_dfe_state_construct(aapl);

    avago_serdes_get_dfe_state_ext(
                        aapl, sbus_addr, dfe, AVAGO_DFE_MODE_DATALEV);
    for (int i = 0; i < (2*MAX_SERDES_EYE_HEIGHTS); i+=2) {
        values[i/2] = (dfe->dataLEV[i+1] - dfe->dataLEV[i]) & 0xfff;
    }

    avago_serdes_get_dfe_state_ext(
                        aapl, sbus_addr, dfe, AVAGO_DFE_MODE_TESTLEV);
    for (int i = 0; i < (2*MAX_SERDES_EYE_HEIGHTS); i+=2) {
        values[MAX_SERDES_EYE_HEIGHTS + i/2] =
                        (dfe->testLEV[i+1] - dfe->testLEV[i]) & 0xfff;
    }

    avago_serdes_dfe_state_destruct(aapl, dfe);
    return 0;
}

int
serdes_eye_get_hw(uint32_t sbus_addr, int eye_type)
{
    Avago_serdes_eye_config_t *cfg =
                                avago_serdes_eye_config_construct(aapl);

    Avago_serdes_eye_data_t *edata = avago_serdes_eye_data_construct(aapl);

    switch (eye_type) {
        case 0:
            cfg->ec_eye_type = AVAGO_EYE_SIZE;
            break;

        case 1:
            cfg->ec_eye_type = AVAGO_EYE_FULL;
            break;

        default:
            cfg->ec_eye_type = AVAGO_EYE_HEIGHT_DVOS;
            break;
    }

    cfg->ec_min_dwell_bits = 0x1e6;
    cfg->ec_max_dwell_bits = 0x1e8;
    cfg->ec_x_resolution   = 64;
    cfg->ec_y_points       = 512;

    // cfg->ec_cmp_mode       = AVAGO_SERDES_RX_CMP_MODE_XOR;
    // cfg->ec_y_step_size = 1;

    avago_serdes_eye_get(aapl, sbus_addr, cfg, edata);

    switch (eye_type) {
        case 0:
            SDK_LINKMGR_TRACE_DEBUG ("Eye width (in real PI steps): %d\n"
                             "Eye height (in DAC steps):    %d\n"
                             "Eye width (in mUI):           %d\n"
                             "Eye height (in mV):           %d",
                             edata->ed_width,
                             edata->ed_height,
                             edata->ed_width_mUI,
                             edata->ed_height_mV);
            break;

        case 1:
            avago_serdes_eye_plot_write(stdout, edata);
            break;

        default:
            avago_serdes_eye_vbtc_log_print(aapl, AVAGO_INFO,
                                    __func__, __LINE__, &(edata->ed_vbtc[0]));
            avago_serdes_eye_hbtc_log_print(aapl, AVAGO_INFO,
                                    __func__, __LINE__, &(edata->ed_hbtc[0]));
            break;
    }

    avago_serdes_eye_data_destruct(aapl, edata);
    avago_serdes_eye_config_destruct(aapl, cfg);

    return 0;
}

int
serdes_rx_lpbk_hw(uint32_t sbus_addr, bool enable)
{
    avago_serdes_set_rx_input_loopback(aapl, sbus_addr, enable);
    return 0;
}

int
serdes_spico_status_hw(uint32_t sbus_addr)
{
    Avago_spico_status_t state;
    memset(&state, 0, sizeof(Avago_spico_status_t));

    avago_spico_status(aapl, sbus_addr, false /*no cache*/, &state);

    SDK_LINKMGR_TRACE_DEBUG("rev: 0x%x, build: 0x%x, pc: 0x%x, en: %d, state: %d,"
                    " clk: %d",
                    state.revision, state.build, state.pc, state.enabled,
                    state.state, state.clk);
    return 0;
}

int
serdes_get_rev_hw(uint32_t sbus_addr)
{
    return aapl_get_firmware_rev(aapl, sbus_addr);
}

uint32_t
serdes_get_eng_id_hw(uint32_t sbus_addr)
{
    return avago_firmware_get_engineering_id(aapl, sbus_addr);
}

int
serdes_get_build_id_hw(uint32_t sbus_addr)
{
    return aapl_get_firmware_build(aapl, sbus_addr);
}

bool
serdes_spico_crc_hw(uint32_t sbus_addr)
{
    return avago_spico_crc(aapl, sbus_addr);
}

int
serdes_set_tx_eq_hw (uint32_t sbus_addr, uint32_t amp,
                     uint32_t pre,       uint32_t post)
{
    Avago_serdes_tx_eq_t tx_eq;
    memset (&tx_eq, 0, sizeof(Avago_serdes_tx_eq_t));

    avago_serdes_get_tx_eq(aapl, sbus_addr, &tx_eq);

    if (amp != 0) {
        tx_eq.atten = amp;
    }

    if (pre != 0) {
        tx_eq.pre   = pre;
    }

    if (post != 0) {
        tx_eq.post  = post;
    }

    return avago_serdes_set_tx_eq(aapl, sbus_addr, &tx_eq);
}

int
serdes_set_rx_term_hw (uint32_t sbus_addr, uint8_t val)
{
    Avago_serdes_rx_term_t rx_term;

    switch (val) {
    case 0:
        rx_term = AVAGO_SERDES_RX_TERM_AGND;
        break;

    case 1:
        rx_term = AVAGO_SERDES_RX_TERM_AVDD;
        break;

    default:    // 2
        rx_term = AVAGO_SERDES_RX_TERM_FLOAT;
        break;
    }

    return avago_serdes_set_rx_term (aapl, sbus_addr, rx_term );
}

uint32_t
serdes_get_errors_hw (uint32_t sbus_addr, bool clear)
{
    return avago_serdes_get_errors(aapl, sbus_addr, AVAGO_LSB, clear);
}

// Init serdes in 1G
int
serdes_an_init_hw (uint32_t sbus_addr, serdes_info_t *serdes_info)
{
    int      ret      = 0;
    bool     int_ret  = false;
    int      int_code = 0x0;
    int      int_data = 0x0;
    uint32_t divider  = serdes_info->sbus_divider;
    uint32_t width    = serdes_info->width;

    SDK_LINKMGR_TRACE_DEBUG("sbus_addr: %u, divider: %u, width: %u",
                            sbus_addr, divider, width);

    Avago_serdes_init_config_t *cfg = avago_serdes_init_config_construct(aapl);
    if (NULL == cfg) {
        SDK_LINKMGR_TRACE_ERR("Failed to construct avago config. "
                              "sbus_addr: %d", sbus_addr);
        return -1;
    }

    cfg->init_mode = Avago_serdes_init_mode_t::AVAGO_PRBS31_ELB;

    // divider and width
    cfg->tx_divider = divider;
    cfg->rx_divider = divider;
    cfg->tx_width   = width;
    cfg->rx_width   = width;

    // Disable signal_ok
    cfg->signal_ok_en = 0;

    // Enable tx_output for AN
    cfg->tx_output_en = 1;

    // Tx/Rx enable
    cfg->init_tx = 1;
    cfg->init_rx = 1;

    // encoding
    cfg->tx_encoding = AVAGO_SERDES_NRZ;
    cfg->rx_encoding = AVAGO_SERDES_NRZ;

    // resets
    cfg->sbus_reset  = 1;
    cfg->spico_reset = 1;

    // reset the earlier error code
    aapl->return_code = 0;

    ret = avago_serdes_init(aapl, sbus_addr, cfg);

    if (ret != 0) {
        SDK_LINKMGR_TRACE_DEBUG("PRBS errors during init. sbus_addr: %d, "
                                "ret: %d", sbus_addr, ret);
    }

    if(aapl->return_code) {
        SDK_LINKMGR_TRACE_ERR("Failed to initialize SerDes. sbus_addr: %d",
                              sbus_addr);
        ret = -1;
        goto cleanup;
    }

    // disable serdes output
    serdes_output_enable_hw(sbus_addr, false);

    // Reset TxPRBSGEN
    int_code = 0x2;
    int_data = 0x1ff;
    int_ret = serdes_spico_int_check_hw(sbus_addr, int_code, int_data);
    if (int_ret == false) {
        SDK_LINKMGR_TRACE_ERR("Failed to reset TxPRBS. sbus_addr: %d", sbus_addr);
        ret = -1;
    }

cleanup:
    avago_serdes_init_config_destruct(aapl, cfg);

    return ret;
}

// Reset AN to start from clean slate
// Start AN
// Assert Link Status
int
serdes_an_start_hw(uint32_t sbus_addr, serdes_info_t *serdes_info,
                   uint32_t user_cap, bool fec_ability, uint32_t fec_request)
{
    int  ret      = 0;
    bool int_ret  = false;
    int  int_code = 0x0;
    int  int_data = 0x0;
    FILE *d_fp = NULL;

    Avago_serdes_an_config_t *config = NULL;

    // Reset AN to start from clean slate
    int_code = 0x7;
    int_data = 0x0;
    int_ret = serdes_spico_int_check_hw(sbus_addr, int_code, int_data);
    if (int_ret == false) {
        SDK_LINKMGR_TRACE_ERR("Failed to reset AN. sbus_addr: %d", sbus_addr);
        ret = -1;
        goto cleanup;
    }

    config = avago_serdes_an_config_construct(aapl);
    if (config == NULL) {
        SDK_LINKMGR_TRACE_ERR("Failed to construct an config. "
                              "sbus_addr: %d", sbus_addr);
        ret = -1;
        goto cleanup;
    }

    config->disable_link_inhibit_timer = 0x1;

    // ignore nonce check since nonce pattern is not set
    config->ignore_nonce_match = 1;

    if (user_cap == 0) {
        config->user_cap    = 0x4   | // 10G  KR
                              0x8   | // 40G  KR4
                              0x10  | // 40G  CR4
                              0x80  | // 100G KR4
                              0x100 | // 100G CR4
                              0x200 | // 25G  KR/CR-S
                              0x400;  // 25G  KR/CR

        config->fec_ability = 1;
        config->fec_request = 0x2;   // 25G RS-FEC
    } else {
        config->user_cap    = user_cap;
        config->fec_ability = fec_ability;
        config->fec_request = fec_request;
    }

    d_fp = fopen("/user_cap", "r");
    if (d_fp) {
        fscanf(d_fp, "%u %d %u",
               &config->user_cap, &config->fec_ability, &config->fec_request);
        fclose(d_fp);
    }

    SDK_LINKMGR_TRACE_DEBUG("sbus_addr: %d, user_cap: %u, "
                            "fec_ability: %d, fec_request: %u",
                            sbus_addr, config->user_cap,
                            config->fec_ability, config->fec_request);

    config->an_clk = 0;     // 1.25Gbps

    config->np_continuous_load = 1;

    avago_serdes_an_start(aapl, sbus_addr, config);

cleanup:
    avago_serdes_an_config_destruct(aapl, config);

    return ret;
}

bool
serdes_an_wait_hcd_hw(uint32_t sbus_addr)
{
    return (avago_serdes_an_wait_hcd(aapl, sbus_addr, 0, 1) == 0);
}

int
serdes_an_hcd_read_hw (uint32_t sbus_addr)
{
    return avago_serdes_an_read_status(
            aapl, sbus_addr, AVAGO_SERDES_AN_READ_HCD);
}

int
serdes_an_fec_enable_read_hw (uint32_t sbus_addr)
{
    return avago_serdes_an_read_status(
            aapl, sbus_addr, AVAGO_SERDES_AN_READ_FEC_ENABLE);
}

int
serdes_an_rsfec_enable_read_hw (uint32_t sbus_addr)
{
    return avago_serdes_an_read_status(
            aapl, sbus_addr, AVAGO_SERDES_AN_READ_RSFEC_ENABLE);
}

int serdes_an_core_status_hw (uint32_t sbus_addr)
{
    return avago_serdes_mem_rd(aapl, sbus_addr, AVAGO_LSB_DIRECT, 0x27);
}

int
serdes_an_hcd_cfg_hw (uint32_t sbus_addr, uint32_t *sbus_addr_arr)
{
    int      tx_width  = 0;
    int      rx_width  = 0;
    uint32_t an_hcd    = 0;
    uint8_t  num_lanes = 0;
    int      int_code  = 0x0;
    int      int_data  = 0x0;
    bool     int_ret   = false;
    Avago_serdes_an_config_t  *config      = NULL;
    Avago_serdes_pmd_config_t *pmd_config  = NULL;
    Avago_addr_t              *addr_struct = NULL;
    Avago_addr_t              *head        = NULL;
    Avago_addr_t              *node        = NULL;

    an_hcd =
        avago_serdes_an_read_status(aapl, sbus_addr, AVAGO_SERDES_AN_READ_HCD);
    switch (an_hcd) {
        case 0x08: /* 100GBASE-KR4 */
        case 0x09: /* 100GBASE-CR4 */
            tx_width = 40;
            rx_width = 40;
            num_lanes = 4;
            break;

        case 0x0a: /* 25GBASE-KRCR-S */
        case 0x0b: /* 25GBASE-KRCR */
            tx_width = 40;
            rx_width = 40;
            num_lanes = 1;
            break;

        case 0x03: /* 40GBASE-KR4 */
        case 0x04: /* 40GBASE-CR4 */
            tx_width = 20;
            rx_width = 20;
            num_lanes = 4;
            break;

        case 0x02: /* 10GBASE-KR */
            tx_width = 20;
            rx_width = 20;
            num_lanes = 1;
            break;

            // unsupported modes
        case 0x00: /* 1000BASE-KX */
        case 0x01: /* 10GBASE-KX4 */
        case 0x0c: /* 2.5GBASE-KX */
        case 0x0d: /* 5GBASE-KR */
        case 0x05: /* 100GBASE-CR10 */
        case 0x06: /* 100GBASE-KP4 */
        case 0x0e: /* 50GBASE-KR/CR */
        case 0x10: /* 100GBASE-KR2/CR2 */
        case 0x11: /* 200GBASE-KR4/CR4 */
        default:
        case 0x07:
        case 0x0f:
        case 0x1f:
            SDK_LINKMGR_TRACE_ERR("unsupported an_hcd: %d, an_hcd_str: %s",
                                  an_hcd,
                                  aapl_an_hcd_to_str(an_hcd));
            return -1;
    }

    // assert link status
    int_code = 0x107;
    int_data = an_hcd | (1 << BIN_ENCODE_ASSERT_LINK_STATUS_SHIFT);
    SDK_LINKMGR_TRACE_DEBUG("an_hcd_cfg sbus_addr %d an_hcd 0x%x (%s) int_code 0x%x "
                            "int_data 0x%x num_lanes %d tx_width %d rx_width %d",
                             sbus_addr, an_hcd, aapl_an_hcd_to_str(an_hcd),
                             int_code, int_data, num_lanes, tx_width, rx_width);
    int_ret = serdes_spico_int_check_hw(sbus_addr, int_code, int_data);
    if (int_ret == false) {
        SDK_LINKMGR_TRACE_ERR("Failed to assert link status. sbus_addr: %d",
                              sbus_addr);
        return -1;
    }

    config     = avago_serdes_an_config_construct(aapl);
    pmd_config = avago_serdes_pmd_config_construct(aapl);

    // construct AAPL addr_list
    for (int i = 0; i < num_lanes; ++i) {
        node = avago_addr_new(aapl);
        avago_addr_init(node);
        node->sbus = sbus_addr_arr[i];

        if (head == NULL) {
            addr_struct = head = node;
        } else {
            head->next = node;
            head = head->next;
        }
        avago_spico_int(aapl, node->sbus, 0x26, 0x000f); // seed HF
        avago_spico_int(aapl, node->sbus, 0x26, 0x0103); // seed LF
    }
    avago_serdes_an_configure_to_hcd(
                    aapl, addr_struct, config, pmd_config, tx_width, rx_width);

    // cleanup
    avago_serdes_an_config_destruct(aapl, config);
    avago_serdes_pmd_config_destruct(aapl, pmd_config);
    avago_addr_delete(aapl, addr_struct);

    return an_hcd;
}

int
serdes_prbs_start_hw (uint32_t sbus_addr, serdes_info_t *serdes_info)
{
    int      ret     = 0;
    uint32_t divider = serdes_info->sbus_divider;
    uint32_t width   = serdes_info->width;
    uint32_t tx_slip = serdes_info->tx_slip_value;
    uint32_t rx_slip = serdes_info->rx_slip_value;
    uint8_t  rx_term = serdes_info->rx_term;
    uint32_t amp     = serdes_info->amp;
    uint32_t pre     = serdes_info->pre;
    uint32_t post    = serdes_info->post;

    SDK_LINKMGR_TRACE_DEBUG("sbus_addr: %u, divider: %u, width: %u, tx_slip: 0x%x,"
                    " rx_slip: 0x%x, rx_term: %d, amp: 0x%x, pre: 0x%x,"
                    " post: 0x%x",
                    sbus_addr, divider, width, tx_slip, rx_slip, rx_term,
                    amp, pre, post);

    Avago_serdes_init_config_t *cfg = avago_serdes_init_config_construct(aapl);
    if (NULL == cfg) {
        SDK_LINKMGR_TRACE_ERR("Failed to construct avago config");
        ret = -1;
        goto cleanup;
    }

    cfg->init_mode = Avago_serdes_init_mode_t::AVAGO_PRBS31_ILB;

    // divider and width
    cfg->tx_divider = divider;
    cfg->rx_divider = divider;
    cfg->tx_width   = width;
    cfg->rx_width   = width;

    // Disable signal_ok
    cfg->signal_ok_en = 0;

    // set tx_ouput_en for prbs
    cfg->tx_output_en = 1;

    // Tx/Rx enable
    cfg->init_tx = 1;
    cfg->init_rx = 1;

    // resets
    cfg->sbus_reset  = 1;
    cfg->spico_reset = 1;

    // reset the earlier error code
    aapl->return_code = 0;

    avago_serdes_init(aapl, sbus_addr, cfg);

    if(aapl->return_code) {
        SDK_LINKMGR_TRACE_ERR("Failed to initialize SerDes\n");
        ret = -1;
        goto cleanup;
    }

cleanup:
    // cleanup
    avago_serdes_init_config_destruct(aapl, cfg);
    return ret;
}

int
serdes_basic_cfg_hw (uint32_t sbus_addr, serdes_info_t *serdes_info)
{
    int      ret       = 0;
    uint32_t divider   = serdes_info->sbus_divider;
    uint32_t width     = serdes_info->width;
    uint8_t  tx_invert = serdes_info->tx_pol;
    uint8_t  rx_invert = serdes_info->rx_pol;

    SDK_LINKMGR_TRACE_DEBUG("sbus_addr: %u, divider: %u, width: %u, "
                            "tx_invert: %u, rx_invert:%u",
                            sbus_addr, divider, width,
                            tx_invert, rx_invert);

    Avago_serdes_init_config_t *cfg = avago_serdes_init_config_construct(aapl);
    if (NULL == cfg) {
        SDK_LINKMGR_TRACE_ERR("Failed to construct avago config");
        return -1;
    }

    // cfg->init_mode = Avago_serdes_init_mode_t::AVAGO_INIT_ONLY;
    cfg->init_mode = Avago_serdes_init_mode_t::AVAGO_CORE_DATA_ELB;

    // divider and width
    cfg->tx_divider = divider;
    cfg->rx_divider = divider;
    cfg->tx_width   = width;
    cfg->rx_width   = width;

    // Disable signal_ok and tx_output
    cfg->signal_ok_en = 0;
    cfg->tx_output_en = 0;

    // Tx/Rx enable
    cfg->init_tx = 1;
    cfg->init_rx = 1;

    // resets
    cfg->sbus_reset  = 1;
    cfg->spico_reset = 1;

    // reset the earlier error code
    aapl->return_code = 0;

    avago_serdes_init(aapl, sbus_addr, cfg);

    if(aapl->return_code) {
        SDK_LINKMGR_TRACE_ERR("Failed to initialize SerDes\n");
        ret = -1;
    }

    avago_serdes_init_config_destruct(aapl, cfg);

    return ret;
}

int
serdes_invert_cfg_hw (uint32_t sbus_addr, serdes_info_t *serdes_info)
{
    uint8_t  tx_invert = serdes_info->tx_pol;
    uint8_t  rx_invert = serdes_info->rx_pol;

    SDK_LINKMGR_TRACE_DEBUG("sbus_addr %u tx_invert 0x%u rx_invert 0x%u",
                            sbus_addr, tx_invert, rx_invert);

    // set the inversions
    avago_serdes_set_tx_invert(aapl, sbus_addr, tx_invert);
    avago_serdes_set_rx_invert(aapl, sbus_addr, rx_invert);
    return 0;
}

int
serdes_cfg_hw (uint32_t sbus_addr, serdes_info_t *serdes_info)
{
    int      ret     = 0;
    uint32_t divider = serdes_info->sbus_divider;
    uint32_t width   = serdes_info->width;
    uint32_t tx_slip = serdes_info->tx_slip_value;
    uint32_t rx_slip = serdes_info->rx_slip_value;
    uint8_t  rx_term = serdes_info->rx_term;
    uint32_t amp     = serdes_info->amp;
    uint32_t pre     = serdes_info->pre;
    uint32_t post    = serdes_info->post;

    SDK_LINKMGR_TRACE_DEBUG("sbus_addr: %u, divider: %u, width: %u, tx_slip: 0x%x,"
                    " rx_slip: 0x%x, rx_term: %d, amp: 0x%x, pre: 0x%x,"
                    " post: 0x%x",
                    sbus_addr, divider, width, tx_slip, rx_slip, rx_term,
                    amp, pre, post);

    // Tx/Rx slip values
    if (tx_slip != 0) {
        serdes_spico_int_check_hw(sbus_addr, 0xd, tx_slip);
    }

    if (rx_slip != 0) {
        serdes_spico_int_check_hw(sbus_addr, 0xe, rx_slip);
    }

    // Rx termination
    serdes_set_rx_term_hw(sbus_addr, rx_term);

    // DFE Tx params
    serdes_set_tx_eq_hw(sbus_addr, amp, pre, post);

    return ret;
}

void
serdes_aacs_start_hw(int port)
{
    // blocking call. waits for incoming connections
    avago_aacs_server(aapl, port);
}

sdk_ret_t
port_serdes_fn_init(platform_type_t platform_type,
                    uint32_t        jtag_id,
                    int             num_sbus_rings,
                    bool            aacs_server_en,
                    bool            aacs_connect,
                    int             port,
                    std::string     ip)
{
    serdes_fn_t        *serdes_fn = &serdes_fns;

    serdes_fn->serdes_basic_cfg     = &serdes_cfg_default;
    serdes_fn->serdes_cfg           = &serdes_cfg_default;
    serdes_fn->serdes_signal_detect = &serdes_signal_detect_default;
    serdes_fn->serdes_rdy           = &serdes_rdy_default;
    serdes_fn->serdes_output_enable = &serdes_output_enable_default;
    serdes_fn->serdes_tx_rx_enable  = &serdes_tx_rx_enable_default;
    serdes_fn->serdes_sbus_reset    = &serdes_sbus_reset_default;
    serdes_fn->serdes_spico_reset   = &serdes_spico_reset_default;
    serdes_fn->serdes_eye_get       = &serdes_eye_get_default;
    serdes_fn->serdes_ical_start    = &serdes_ical_start_default;
    serdes_fn->serdes_pcal_start    = &serdes_pcal_start_default;
    serdes_fn->serdes_an_pcal_start = &serdes_pcal_start_default;
    serdes_fn->serdes_pcal_continuous_start =
                            &serdes_pcal_continuous_start_default;
    serdes_fn->serdes_dfe_status    = &serdes_dfe_status_default;
    serdes_fn->serdes_rx_lpbk       = &serdes_rx_lpbk_default;
    serdes_fn->serdes_spico_upload  = &serdes_spico_upload_default;
    serdes_fn->serdes_spico_status  = &serdes_spico_status_default;
    serdes_fn->serdes_get_rev       = &serdes_get_rev_default;
    serdes_fn->serdes_get_eng_id    = &serdes_get_eng_id_default;
    serdes_fn->serdes_get_build_id  = &serdes_get_build_id_default;
    serdes_fn->serdes_spico_crc     = &serdes_spico_crc_default;
    serdes_fn->serdes_aacs_start    = &serdes_aacs_start_default;
    serdes_fn->serdes_set_tx_eq     = &serdes_set_tx_eq_default;
    serdes_fn->serdes_set_rx_term   = &serdes_set_rx_term_default;
    serdes_fn->serdes_spico_int     = &serdes_spico_int_default;
    serdes_fn->serdes_get_errors    = &serdes_get_errors_default;
    serdes_fn->serdes_prbs_start    = &serdes_prbs_start_default;
    serdes_fn->serdes_an_init       = &serdes_an_init_default;
    serdes_fn->serdes_an_start      = &serdes_an_start_default;
    serdes_fn->serdes_an_wait_hcd   = &serdes_an_wait_hcd_default;
    serdes_fn->serdes_an_hcd_read   = &serdes_an_hcd_read_default;
    serdes_fn->serdes_an_hcd_cfg    = &serdes_an_hcd_cfg_default;
    serdes_fn->serdes_invert_cfg    = &serdes_invert_cfg_default;
    serdes_fn->serdes_eye_check     = &serdes_eye_check_default;

    serdes_fn->serdes_an_core_status = &serdes_an_core_status_default;

    serdes_fn->serdes_an_fec_enable_read   =
                                      &serdes_an_fec_enable_read_default;
    serdes_fn->serdes_an_rsfec_enable_read =
                                      &serdes_an_rsfec_enable_read_default;

    switch (platform_type) {
    case platform_type_t::PLATFORM_TYPE_HW:
        serdes_fn->serdes_basic_cfg     = &serdes_basic_cfg_hw;
        serdes_fn->serdes_cfg           = &serdes_cfg_hw;
        serdes_fn->serdes_signal_detect = &serdes_signal_detect_hw;
        serdes_fn->serdes_rdy           = &serdes_rdy_hw;
        serdes_fn->serdes_output_enable = &serdes_output_enable_hw;
        serdes_fn->serdes_tx_rx_enable  = &serdes_tx_rx_enable_hw;
        serdes_fn->serdes_sbus_reset    = &serdes_sbus_reset_hw;
        serdes_fn->serdes_spico_reset   = &serdes_spico_reset_hw;
        serdes_fn->serdes_eye_get       = &serdes_eye_get_hw;
        serdes_fn->serdes_eye_check     = &serdes_eye_check_hw;
        serdes_fn->serdes_ical_start    = &serdes_ical_start_hw;

        // skip PCAL for HW since ICAL mode does both ICAL and PCAL
        serdes_fn->serdes_pcal_start    = &serdes_pcal_start_default;
        serdes_fn->serdes_an_pcal_start = &serdes_pcal_start_hw;

        serdes_fn->serdes_pcal_continuous_start =
                                &serdes_pcal_continuous_start_hw;
        serdes_fn->serdes_dfe_status    = &serdes_dfe_status_hw;
        serdes_fn->serdes_rx_lpbk       = &serdes_rx_lpbk_hw;
        serdes_fn->serdes_spico_upload  = &serdes_spico_upload_hw;
        serdes_fn->serdes_spico_status  = &serdes_spico_status_hw;
        serdes_fn->serdes_get_rev       = &serdes_get_rev_hw;
        serdes_fn->serdes_get_eng_id    = &serdes_get_eng_id_hw;
        serdes_fn->serdes_get_build_id  = &serdes_get_build_id_hw;
        serdes_fn->serdes_spico_crc     = &serdes_spico_crc_hw;
        serdes_fn->serdes_aacs_start    = &serdes_aacs_start_hw;
        serdes_fn->serdes_set_tx_eq     = &serdes_set_tx_eq_hw;
        serdes_fn->serdes_set_rx_term   = &serdes_set_rx_term_hw;
        serdes_fn->serdes_spico_int     = &serdes_spico_int_hw;
        serdes_fn->serdes_get_errors    = &serdes_get_errors_hw;
        serdes_fn->serdes_prbs_start    = &serdes_prbs_start_hw;
        serdes_fn->serdes_an_init       = &serdes_an_init_hw;
        serdes_fn->serdes_an_start      = &serdes_an_start_hw;
        serdes_fn->serdes_an_wait_hcd   = &serdes_an_wait_hcd_hw;
        serdes_fn->serdes_an_hcd_read   = &serdes_an_hcd_read_hw;
        serdes_fn->serdes_an_hcd_cfg    = &serdes_an_hcd_cfg_hw;
        serdes_fn->serdes_invert_cfg    = &serdes_invert_cfg_hw;
        serdes_fn->serdes_an_core_status = &serdes_an_core_status_hw;

        serdes_fn->serdes_an_fec_enable_read   =
                                          &serdes_an_fec_enable_read_hw;
        serdes_fn->serdes_an_rsfec_enable_read =
                                          &serdes_an_rsfec_enable_read_hw;

        // serdes global init
        aapl = serdes_global_init_hw(jtag_id, num_sbus_rings,
                                     aacs_server_en, aacs_connect, port, ip);
        break;

    default:
        break;
    }

    return SDK_RET_OK;
}

}    // namespace linkmgr
}    // namespace sdk
