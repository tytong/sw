// {C} Copyright 2017 Pensando Systems Inc. All rights reserved

#include "linkmgr.hpp"
#include "linkmgr_rw.hpp"
#include "port_serdes.hpp"
#include "linkmgr_types.hpp"
#include "linkmgr_internal.hpp"
#ifdef NRMAKE
#include "third-party/avago/include/aapl/aapl.h"
#else
#include "aapl.h"
#endif

namespace sdk {
namespace linkmgr {

// global aapl info
Aapl_t *aapl = NULL;

#define SPICO_INT_ENABLE 0x1
#define SPICO_INT_RESET  0x39

//---------------------------------------------------------------------------
// HAPS platform methods
//---------------------------------------------------------------------------

bool
serdes_signal_detect_haps (uint32_t sbus_addr)
{
    return true;
}

bool
serdes_rdy_haps (uint32_t sbus_addr)
{
    return true;
}

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

int
serdes_tx_rx_enable_haps (uint32_t sbus_addr, bool enable)
{
    return SDK_RET_OK;
}

int
serdes_reset_haps (uint32_t sbus_addr, bool reset)
{
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
serdes_reset_default (uint32_t sbus_addr, bool reset)
{
    return SDK_RET_OK;
}

int
serdes_eye_get_default(uint32_t sbus_addr, int eye_type)
{
    return 0;
}

int
serdes_ical_start_default(uint32_t sbus_addr)
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

//---------------------------------------------------------------------------
// HW methods
//---------------------------------------------------------------------------

uint32_t sbus_access (Aapl_t *aapl, uint32_t addr, unsigned char reg_addr,
                      unsigned char command, uint *sbus_data)
{
    return 0;
}

uint32_t spico_int (Aapl_t *aapl, uint32_t addr, int int_code, int int_data)
{
    return 0;
}

Aapl_t*
serdes_global_init_hw(void)
{
    Aapl_comm_method_t comm_method = AVAGO_SBUS;

    // TODO CLI
    int         verbose        = 1;
    int         debug          = 8;
    bool        aacs_server_en = false; // start aacs server
    bool        aacs_connect   = true;  // connect to aacs server
    int         port           = 90;    // aacs server port
    std::string ip             = "192.168.75.125"; //aacs server ip

    // TODO read from catalog
    uint32_t jtag_idcode    = 0x9b1657f;
    int      num_chips      = 1;
    int      num_sbus_rings = 1;

    (void)aacs_server_en;

    Aapl_t *aapl = aapl_construct();

    // set the appl init params
    aapl->communication_method = comm_method;
    aapl->jtag_idcode[0]       = jtag_idcode;
    aapl->sbus_rings           = num_sbus_rings;
    aapl->chips                = num_chips;
    aapl->debug                = debug;
    aapl->verbose              = verbose;

    if (aacs_connect == true) {
        aapl->aacs  = aacs_connect;
    } else {
        // register access methods
        aapl_register_sbus_fn(aapl, sbus_access, NULL, NULL);
        aapl_register_spico_int_fn(aapl, spico_int);
    }

    // Make a connection to the device
    aapl_connect(aapl, ip.c_str(), port);

    if(aapl->return_code < 0) {
        aapl_destruct(aapl);
        return NULL;
    }

    /* Gather information about the device and place into AAPL struct */
    aapl_get_ip_info(aapl, 1 /* chip reset */);

    if(aapl->return_code < 0) {
        aapl_destruct(aapl);
        return NULL;
    }

    return aapl;
}

bool
serdes_signal_detect_hw (uint32_t sbus_addr)
{
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
serdes_cfg_hw (uint32_t sbus_addr, serdes_info_t *serdes_info)
{
    uint32_t divider = serdes_info->sbus_divider;
    uint32_t width   = serdes_info->width;

    Avago_serdes_init_config_t *cfg = avago_serdes_init_config_construct(aapl);
    if (NULL == cfg) {
        SDK_TRACE_ERR("Failed to construct avago config");
        return SDK_RET_ERR;
    }

    cfg->init_mode = Avago_serdes_init_mode_t::AVAGO_INIT_ONLY;

    // divider and width
    cfg->tx_divider = divider;
    cfg->rx_divider = divider;
    cfg->tx_width   = width;
    cfg->rx_width   = width;

    cfg->signal_ok_en = 0;

    // Tx/Rx enable
    cfg->init_tx = 1;
    cfg->init_rx = 1;

    cfg->sbus_reset  = 1;
    cfg->spico_reset = 1;

    avago_serdes_init(aapl, sbus_addr, cfg);

    if(aapl->return_code) {
        SDK_TRACE_ERR("Failed to initialize SerDes\n");
    }

    avago_serdes_init_config_destruct(aapl, cfg);

    return SDK_RET_OK;
}

int
serdes_tx_rx_enable_hw (uint32_t sbus_addr, bool enable)
{
    // To be set only during init stage.
    // Need to wait for Tx/Rx ready once set
    return SDK_RET_OK;

#if 0
    int  mask     = 0;
    bool rc       = false;
    int  int_code = SPICO_INT_ENABLE;

    mask = serdes_get_int01_bits(aapl, sbus_addr, ~0x3) | (enable ? 0x3 : 0x0);

    rc = avago_spico_int_check(aapl, __func__, __LINE__, sbus_addr, int_code, mask);
    return rc ? SDK_RET_OK : SDK_RET_ERR;
#endif
}

int
serdes_reset_hw (uint32_t sbus_addr, bool reset)
{
    int  mask     = 0;
    bool rc       = false;
    int  int_code = SPICO_INT_RESET;

    if (reset == true) {
        mask = 1;
    }

    rc = avago_spico_int_check(aapl, __func__, __LINE__, sbus_addr, int_code, mask);
    return rc ? SDK_RET_OK : SDK_RET_ERR;
}

int
serdes_ical_start_hw(uint32_t sbus_addr)
{
    Avago_serdes_dfe_tune_t dfe;

    avago_serdes_tune_init(aapl, &dfe);
    dfe.tune_mode = Avago_serdes_dfe_tune_mode_t::AVAGO_DFE_ICAL;

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

    cfg->ec_cmp_mode    = AVAGO_SERDES_RX_CMP_MODE_XOR;
    // cfg->ec_y_step_size = 1;
    // cfg->ec_y_points    = 512;

    avago_serdes_eye_get(aapl, sbus_addr, cfg, edata);

    switch (eye_type) {
        case 0:
            SDK_TRACE_DEBUG ("Eye width (in real PI steps): %d\n"
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

sdk_ret_t
port_serdes_fn_init(linkmgr_cfg_t *cfg)
{
    serdes_fn_t        *serdes_fn = &serdes_fns;
    platform_type_t    platform_type = cfg->platform_type;

    serdes_fn->serdes_cfg = &serdes_cfg_default;
    serdes_fn->serdes_signal_detect = &serdes_signal_detect_default;
    serdes_fn->serdes_rdy = &serdes_rdy_default;
    serdes_fn->serdes_output_enable = &serdes_output_enable_default;
    serdes_fn->serdes_tx_rx_enable = &serdes_tx_rx_enable_default;
    serdes_fn->serdes_reset = &serdes_reset_default;
    serdes_fn->serdes_eye_get = &serdes_eye_get_default;
    serdes_fn->serdes_ical_start = &serdes_ical_start_default;
    serdes_fn->serdes_pcal_start = &serdes_pcal_start_default;
    serdes_fn->serdes_pcal_continuous_start = &serdes_pcal_continuous_start_default;
    serdes_fn->serdes_dfe_status = &serdes_dfe_status_default;
    serdes_fn->serdes_rx_lpbk = &serdes_rx_lpbk_default;

    switch (platform_type) {
    case platform_type_t::PLATFORM_TYPE_HW:
        serdes_fn->serdes_cfg = &serdes_cfg_hw;
        serdes_fn->serdes_signal_detect = &serdes_signal_detect_hw;
        serdes_fn->serdes_rdy = &serdes_rdy_hw;
        serdes_fn->serdes_output_enable = &serdes_output_enable_hw;
        serdes_fn->serdes_tx_rx_enable = &serdes_tx_rx_enable_hw;
        serdes_fn->serdes_reset = &serdes_reset_hw;
        serdes_fn->serdes_eye_get = &serdes_eye_get_hw;
        serdes_fn->serdes_ical_start = &serdes_ical_start_hw;
        serdes_fn->serdes_pcal_start = &serdes_pcal_start_hw;
        serdes_fn->serdes_pcal_continuous_start = &serdes_pcal_continuous_start_hw;
        serdes_fn->serdes_dfe_status = &serdes_dfe_status_hw;
        serdes_fn->serdes_rx_lpbk = &serdes_rx_lpbk_hw;

        // serdes global init
        aapl = serdes_global_init_hw();
        break;

    default:
        break;
    }

    return SDK_RET_OK;
}

}    // namespace linkmgr
}    // namespace sdk
