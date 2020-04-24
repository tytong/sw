// {C} Copyright 2017 Pensando Systems Inc. All rights reserved

#include "port.hpp"
#include "port_mac.hpp"
#include "port_serdes.hpp"
#include "linkmgr_internal.hpp"
#include "linkmgr.hpp"
#include "linkmgr_types.hpp"
#include "asic/pd/pd.hpp"
#include "platform/drivers/xcvr.hpp"
#include "platform/fru/fru.hpp"
#include "platform/pal/include/pal.h"
#include "lib/pal/pal.hpp"
#include "asic/rw/asicrw.hpp"

using namespace sdk::asic::pd;

namespace sdk {
namespace linkmgr {

mac_fn_t    mac_fns;
mac_fn_t    mac_mgmt_fns;
serdes_fn_t serdes_fns;

// Debounce timer expiration handler
sdk_ret_t
port::port_debounce_timer_cb(void)
{
    // Notify if link is still down
    if (port_link_status() == false) {
        // link bringup time = last_up_ts - last_down_ts
        // set last_down_ts:
        //  - on link down event
        //  - link is enabled from config
        set_last_down_ts();

        // Disable notifies link down if link was UP before
        port_disable();

        // Enable the port
        port_enable();
    }

    return SDK_RET_OK;
}

// fn returns max serdes ready retries value
// TODO: enhance to add
uint32_t
port::port_max_serdes_ready_retries (void)
{
    return MAX_PORT_SERDES_READY_RETRIES;
}

// fn returns AN retries value based on cable_type and XCVR type
// we want SFP/CU AN to have shorter retries cycle
uint32_t
port::port_max_an_retries (void)
{
    int phy_port = sdk::lib::catalog::logical_port_to_phy_port(this->port_num_);

    if (sdk::platform::xcvr_type(phy_port -1) == xcvr_type_t::XCVR_TYPE_SFP) {
        return MAX_PORT_SFP_AN_HCD_RETRIES;
    }
    return MAX_PORT_QSFP_AN_HCD_RETRIES;
}

bool
port::port_link_status(void)
{
    // check for MAC sync and MAC faults
    if (port_mac_sync_get() == false) {
        return false;
    }
    if (port_mac_faults_get() == true) {
        SDK_LINKMGR_TRACE_DEBUG("port %u, MAC %u, MAC faults detected",
                                port_num(), mac_id());
        return false;
    }
    return true;
}

sdk_ret_t
port::port_mac_init(void)
{
    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_fifo_ctrl(void)
{
    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_global_mode_cfg(void)
{
    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_ch_enable(bool enable)
{
    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_generic_cfg(void)
{
    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_rx_tx_enable(bool rx_enable, bool tx_enable)
{
    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_ch_mode_cfg(void)
{
    return SDK_RET_OK;
}

uint32_t
port::port_mac_port_num_calc(void)
{
    return (this->mac_id_ * MAX_PORT_LANES) + this->mac_ch_;
}

sdk_ret_t
port::port_mac_cfg_fec(port_fec_type_t fec_type)
{
    mac_info_t mac_info;
    memset(&mac_info, 0, sizeof(mac_info_t));

    mac_info.mac_id    = this->mac_id_;
    mac_info.mac_ch    = this->mac_ch_;
    mac_info.speed     = static_cast<uint32_t>(this->port_speed_);

    mac_info.mtu       = this->mtu_;
    mac_info.pause     = this->pause_;
    mac_info.tx_pause_enable = this->tx_pause_enable_;
    mac_info.rx_pause_enable = this->rx_pause_enable_;
    mac_info.num_lanes = this->num_lanes_;

    mac_info.fec       = static_cast<uint32_t>(fec_type);
    mac_info.loopback  =
        loopback_mode() == port_loopback_mode_t::PORT_LOOPBACK_MODE_MAC? 1 : 0;

    // Enable Tx padding. Disable Rx padding
    mac_info.tx_pad_enable = 1;
    mac_info.rx_pad_enable = 0;

    mac_info.force_global_init = false;

    mac_fns()->mac_cfg_fec(&mac_info);

    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_cfg(void)
{
    mac_info_t mac_info;
    memset(&mac_info, 0, sizeof(mac_info_t));

    mac_info.mac_id    = this->mac_id_;
    mac_info.mac_ch    = this->mac_ch_;
    mac_info.speed     = static_cast<uint32_t>(this->port_speed_);

    mac_info.mtu       = this->mtu_;
    mac_info.pause     = this->pause_;
    mac_info.tx_pause_enable = this->tx_pause_enable_;
    mac_info.rx_pause_enable = this->rx_pause_enable_;
    mac_info.num_lanes = this->num_lanes_;

    mac_info.fec       = static_cast<uint32_t>(fec_type());
    mac_info.loopback  =
        loopback_mode() == port_loopback_mode_t::PORT_LOOPBACK_MODE_MAC? 1 : 0;

    // Enable Tx padding. Disable Rx padding
    mac_info.tx_pad_enable = 1;
    mac_info.rx_pad_enable = 0;

    mac_info.force_global_init = false;

    mac_fns()->mac_cfg(&mac_info);

    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_enable(bool enable)
{
    uint32_t mac_port_num = port_mac_port_num_calc();

    mac_fns()->mac_enable(
               mac_port_num,
               static_cast<uint32_t>(this->port_speed_),
               this->num_lanes_,
               enable);

    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_soft_reset(bool reset)
{
    uint32_t mac_port_num = port_mac_port_num_calc();

    mac_fns()->mac_soft_reset(
                   mac_port_num,
                   static_cast<uint32_t>(this->port_speed_),
                   this->num_lanes_,
                   reset);

    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_stats_reset(bool reset)
{
    mac_fns()->mac_stats_reset(this->mac_id_, this->mac_ch_, reset);
    port_mac_stats_persist_clear(reset);

    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_intr_en(bool enable)
{
    uint32_t mac_port_num = port_mac_port_num_calc();

    mac_fns()->mac_intr_enable(
            mac_port_num,
            static_cast<uint32_t>(this->port_speed_),
            this->num_lanes_,
            enable);

    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_intr_clr(void)
{
    uint32_t mac_port_num = port_mac_port_num_calc();

    mac_fns()->mac_intr_clear(
            mac_port_num,
            static_cast<uint32_t>(this->port_speed_),
            this->num_lanes_);

    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_faults_clear(void)
{
    mac_fns()->mac_faults_clear(this->mac_id(), this->mac_ch());
    return SDK_RET_OK;
}

bool
port::port_mac_faults_get(void)
{
    return mac_fns()->mac_faults_get(this->mac_id(), this->mac_ch());
}

sdk_ret_t
port::port_mac_send_remote_faults(bool send) {
    mac_fns()->mac_send_remote_faults(
                this->mac_id(), this->mac_ch(), send);
    return SDK_RET_OK;
}

bool
port::port_mac_sync_get(void)
{
    uint32_t mac_port_num = port_mac_port_num_calc();

    return mac_fns()->mac_sync_get(mac_port_num);
}

sdk_ret_t
port::port_flush_set (bool enable)
{
    uint32_t mac_port_num = port_mac_port_num_calc();

    mac_fns()->mac_flush_set(mac_port_num, enable);

    return SDK_RET_OK;
}

uint32_t
port::port_sbus_addr(uint32_t lane)
{
    return this->sbus_addr_[lane];
}

port_speed_t
port::port_speed_to_serdes_speed(port_speed_t port_speed)
{
    switch(port_speed) {
    case port_speed_t::PORT_SPEED_100G:
    case port_speed_t::PORT_SPEED_50G:
    case port_speed_t::PORT_SPEED_25G:
        return port_speed_t::PORT_SPEED_25G;

    case port_speed_t::PORT_SPEED_40G:
    case port_speed_t::PORT_SPEED_10G:
        return port_speed_t::PORT_SPEED_10G;

    default:
        break;
    }

    return port_speed;
}

sdk_ret_t
port::port_serdes_cfg(void)
{
    uint32_t      lane;
    uint32_t      sbus_addr;
    serdes_info_t *serdes_info = NULL;

    port_speed_t serdes_speed =
                    port_speed_to_serdes_speed(this->port_speed_);

    for (lane = 0; lane < num_lanes_; ++lane) {
        sbus_addr = port_sbus_addr(lane);
        if (sbus_addr == 0) {
            continue;
        }
        serdes_info = serdes_info_get(
                                    sbus_addr,
                                    static_cast<uint32_t>(serdes_speed),
                                    cable_type());

        serdes_fns()->serdes_basic_cfg(sbus_addr, serdes_info);
        // configure the tx/rx inversions
        serdes_fns()->serdes_invert_cfg(sbus_addr, serdes_info);
        serdes_fns()->serdes_cfg(sbus_addr, serdes_info);
        serdes_fns()->serdes_rx_lpbk(sbus_addr,
                loopback_mode() == port_loopback_mode_t::PORT_LOOPBACK_MODE_PHY?
                true : false);
    }

    return SDK_RET_OK;
}

sdk_ret_t
port::port_serdes_tx_rx_enable(bool enable)
{
    uint32_t lane;
    uint32_t sbus_addr;

    for (lane = 0; lane < num_lanes_; ++lane) {
        sbus_addr = port_sbus_addr(lane);
        if (sbus_addr == 0) {
            continue;
        }
        serdes_fns()->serdes_tx_rx_enable(sbus_addr, enable);
    }

    return SDK_RET_OK;
}

sdk_ret_t
port::port_serdes_output_enable(bool enable)
{
    uint32_t lane;
    uint8_t  xcvr_en_mask = 0x0;
    sdk_ret_t sdk_ret = SDK_RET_OK;
    uint32_t sbus_addr;

    for (lane = 0; lane < num_lanes_; ++lane) {
        sbus_addr = port_sbus_addr(lane);
        if (sbus_addr == 0) {
            continue;
        }
        serdes_fns()->serdes_output_enable(sbus_addr, enable);
        xcvr_en_mask |= (1 << lane);
    }

    int phy_port = sdk::lib::catalog::logical_port_to_phy_port(port_num());

    if (phy_port != -1 &&
        cable_type() == sdk::types::cable_type_t::CABLE_TYPE_FIBER) {
        sdk_ret = sdk::platform::xcvr_enable(phy_port-1, enable, xcvr_en_mask);
        if (sdk_ret != SDK_RET_OK) {
            SDK_LINKMGR_TRACE_ERR ("Failed to %s phy_port: %d",
                                   enable == true? "enable" : "disable",
                                   phy_port);
        }
    }
    return sdk_ret;
}

sdk_ret_t
port::port_serdes_reset(bool reset)
{
    uint32_t lane;
    uint32_t sbus_addr;

    for (lane = 0; lane < num_lanes_; ++lane) {
        sbus_addr = port_sbus_addr(lane);
        if (sbus_addr == 0) {
            continue;
        }
        serdes_fns()->serdes_spico_reset(sbus_addr);
    }

    return SDK_RET_OK;
}

bool
port::port_serdes_signal_detect(void)
{
    uint32_t lane;
    bool signal_detect = false;
    uint32_t sbus_addr;

    for (lane = 0; lane < num_lanes_; ++lane) {
        sbus_addr = port_sbus_addr(lane);
        if (sbus_addr == 0) {
            // invalid config. fail signal detect
            signal_detect = false;
            break;
        }
        signal_detect = serdes_fns()->serdes_signal_detect(sbus_addr);
        if (signal_detect == false) {
            break;
        }
    }
    return signal_detect;
}

bool
port::port_serdes_rdy(void)
{
    uint32_t lane;
    bool serdes_rdy = false;
    uint32_t sbus_addr;

    for (lane = 0; lane < num_lanes_; ++lane) {
        sbus_addr = port_sbus_addr(lane);
        if (sbus_addr == 0) {
            // invalid config. fail serdes rdy
            serdes_rdy = false;
            break;
        }
        serdes_rdy =  serdes_fns()->serdes_rdy(sbus_addr);
        if (serdes_rdy == false) {
            break;
        }
    }

    return serdes_rdy;
}

sdk_ret_t
port::sbus_addr_set (uint32_t lane, uint32_t sbus_addr)
{
    this->sbus_addr_[lane] = sbus_addr;
    return SDK_RET_OK;
}

bool
port::port_dfe_tuning_enabled(void)
{
    switch (this->port_speed_) {
        case port_speed_t::PORT_SPEED_100G:
        case port_speed_t::PORT_SPEED_50G:
        case port_speed_t::PORT_SPEED_40G:
        case port_speed_t::PORT_SPEED_25G:
        case port_speed_t::PORT_SPEED_10G:
            return true;

        default:
            break;
    }

    return false;
}

int
port::port_serdes_ical_start(void)
{
    uint32_t lane;
    uint32_t sbus_addr;
    port_speed_t serdes_speed =
                        port_speed_to_serdes_speed(port_speed());

    for (lane = 0; lane < num_lanes_; ++lane) {
        sbus_addr = port_sbus_addr(lane);
        if (sbus_addr == 0) {
            continue;
        }
        serdes_fns()->serdes_ical_start(sbus_addr, serdes_speed);
    }

    return 0;
}

int
port::port_serdes_pcal_start(void)
{
    uint32_t lane;
    uint32_t sbus_addr;

    for (lane = 0; lane < num_lanes_; ++lane) {
        sbus_addr = port_sbus_addr(lane);
        if (sbus_addr == 0) {
            continue;
        }
        if (is_auto_neg() == AUTO_NEG) {
            serdes_fns()->serdes_an_pcal_start(sbus_addr);
        } else {
            serdes_fns()->serdes_pcal_start(sbus_addr);
        }
    }

    return 0;
}

int
port::port_serdes_pcal_continuous_start(void)
{
    uint32_t lane;
    uint32_t sbus_addr;

    for (lane = 0; lane < num_lanes_; ++lane) {
        sbus_addr = port_sbus_addr(lane);
        if (sbus_addr == 0) {
            continue;
        }
        serdes_fns()->serdes_pcal_continuous_start(sbus_addr);
    }

    return 0;
}

bool
port::port_serdes_dfe_complete(void)
{
    uint32_t lane;
    uint32_t sbus_addr;

    for (lane = 0; lane < num_lanes_; ++lane) {
        sbus_addr = port_sbus_addr(lane);
        if (sbus_addr == 0) {
            // invalid config. fail dfe complete
            return false;
        }
        if (serdes_fns()->serdes_dfe_status(sbus_addr) != 1) {
            return false;
        }
    }

    return true;
}

int
port::port_serdes_an_start (void)
{
    uint32_t      sbus_addr    = 0;
    serdes_info_t *serdes_info = NULL;
    port_speed_t  serdes_speed = port_speed_t::PORT_SPEED_25G;
    serdes_info_t serdes_info_an;

    memset(&serdes_info_an, 0, sizeof(serdes_info_t));

    // AN on lane 0
    sbus_addr = port_sbus_addr(0);

    serdes_info = serdes_info_get(
                                sbus_addr,
                                static_cast<uint32_t>(serdes_speed),
                                sdk::types::cable_type_t::CABLE_TYPE_CU);
    memcpy (&serdes_info_an, serdes_info, sizeof(serdes_info_t));

    // 1G AN settings
    serdes_info_an.width        = 20;
    serdes_info_an.sbus_divider = 8;

    // Init serdes in 1G
    serdes_fns()->serdes_an_init(port_sbus_addr(0), &serdes_info_an);

    // configure the tx/rx inversions
    serdes_fns()->serdes_invert_cfg(sbus_addr, serdes_info);

    // Configure Tx/Rx slip, Rx termination, Tx EQ for 25G serdes.
    // This is to avoid setting these values if negotiated speed
    // is 25G for each serdes
    // Do not configure lane 0 since AN is run on that lane
    for (uint32_t lane = 1; lane < num_lanes_; ++lane) {
        sbus_addr = port_sbus_addr(lane);
        if (sbus_addr == 0) {
            continue;
        }
        serdes_info = serdes_info_get(
                                sbus_addr,
                                static_cast<uint32_t>(serdes_speed),
                                sdk::types::cable_type_t::CABLE_TYPE_CU);
        serdes_fns()->serdes_cfg(sbus_addr, serdes_info);
    }
    // start AN
    return serdes_fns()->serdes_an_start(port_sbus_addr(0), &serdes_info_an,
                                         user_cap(), fec_ability(),
                                         fec_request());
}

bool
port::port_serdes_an_wait_hcd (void)
{
    return serdes_fns()->serdes_an_wait_hcd(port_sbus_addr(0));
}

bool
port::port_serdes_an_link_train_check (void)
{
    uint32_t lane           = 0;
    uint32_t sbus_addr      = 0;
    int      o_core_status  = 0x0;
    bool     training_fail  = false;
    int      training_done  = 0x0;
    int      all_lanes_mask = 0x0;
    int      o_core_status_arr[4] = {0};
    int      tries = 0;
    int      max_retries = 10000;   // timeout after 100usec * 10000 = 1 sec

    while (true) {
        for (lane = 0; lane < num_lanes_; ++lane) {

            all_lanes_mask = all_lanes_mask | (1 << lane);

            if ( (training_done & (1 << lane)) != 0) {
                continue;
            }

            sbus_addr = port_sbus_addr(lane);

            o_core_status = serdes_fns()->serdes_an_core_status(sbus_addr);

            if ( (o_core_status & 0x2) == 0x0) {    // training completed
                if (o_core_status != 0x34) {        // training not yet passed
                    if ( (o_core_status & 0x1) == 0x1) {
                        // training failed
                        training_fail = true;
                    } else {
                        // training has completed, but not yet failed/passed
                        continue;
                    }
                }

                o_core_status_arr[lane] = o_core_status;
                training_done = training_done | (1 << lane);
            }
        }

        if ((training_done == all_lanes_mask) ||
            (tries++ == max_retries)) {
            break;
        }

        usleep(100);
    }

    for (lane = 0; lane < num_lanes_; ++lane) {
        sbus_addr = port_sbus_addr(lane);

        SDK_LINKMGR_TRACE_DEBUG("sbus_addr: %u, o_core_status: 0x%x, "
                                "training_fail: %d",
                                sbus_addr,
                                o_core_status_arr[lane],
                                training_fail);
    }

    return training_fail == false? true:false;
}

int
port::port_serdes_an_hcd_cfg (void)
{
    bool          skip         = false;
    uint32_t      an_hcd       = 0;
    uint32_t      lane         = 0;
    uint32_t      sbus_addr    = 0;
    int           fec_enable   = 0;
    int           rsfec_enable = 0;
    serdes_info_t *serdes_info = NULL;
    uint32_t      num_lanes    = 0;

    an_hcd       = serdes_fns()->serdes_an_hcd_read(port_sbus_addr(0));
    fec_enable   = serdes_fns()->serdes_an_fec_enable_read(port_sbus_addr(0));
    rsfec_enable = serdes_fns()->serdes_an_rsfec_enable_read(port_sbus_addr(0));

    SDK_LINKMGR_TRACE_DEBUG("port: %d, an_hcd: %d, fec_enable: %d, "
                            "rsfec_enable: %d",
                            port_num(), an_hcd, fec_enable, rsfec_enable);

    // set the negotiated port params
    if (port_set_an_resolved_params(an_hcd, fec_enable, rsfec_enable) == -1) {
        SDK_PORT_SM_TRACE(this, "Resolved params set failed");
        return -1;
    }

    // skip the settings on lanes 1-3 for 25G serdes since its already set
    // before AN start
    switch(an_hcd) {
    case 0x08: /* 100GBASE-KR4 */
    case 0x09: /* 100GBASE-CR4 */
    case 0x0a: /* 25GBASE-KRCR-S */
    case 0x0b: /* 25GBASE-KRCR */
        skip = true;
    default:
        break;
    }

    if (skip == true) {
        num_lanes = 1;
    } else {
        num_lanes = num_lanes_;
    }

    port_speed_t serdes_speed =
                    port_speed_to_serdes_speed(this->port_speed_);

    for (lane = 0; lane < num_lanes; ++lane) {
        sbus_addr = port_sbus_addr(lane);

        serdes_info = serdes_info_get(
                                sbus_addr,
                                static_cast<uint32_t>(serdes_speed),
                                cable_type());

        // configure Tx/Rx slip, Rx termination, Tx EQ
        serdes_fns()->serdes_cfg(sbus_addr, serdes_info);
    }

    // configure divider, width, start link training
    return serdes_fns()->serdes_an_hcd_cfg(port_sbus_addr(0), this->sbus_addr_);
}

int
port::port_set_an_resolved_params_internal (port_speed_t speed, int num_lanes,
                                            port_fec_type_t fec_type)
{
    this->set_port_speed(speed);
    this->set_num_lanes(num_lanes);

    // in AN mode, oper fec_type = AN resolved fec type
    set_fec_type(fec_type);
    return 0;
}

int
port::port_set_an_resolved_params (int an_hcd, int fec_enable, int rsfec_enable)
{
    int ret = 0;
    port_fec_type_t fec_type = port_fec_type_t::PORT_FEC_TYPE_NONE;

    switch (an_hcd) {
    case 0x08: /* 100GBASE-KR4 */
    case 0x09: /* 100GBASE-CR4 */
        fec_type = port_fec_type_t::PORT_FEC_TYPE_RS;
        port_set_an_resolved_params_internal(
                port_speed_t::PORT_SPEED_100G, 4, fec_type);
        break;

    case 0x03: /* 40GBASE-KR4 */
    case 0x04: /* 40GBASE-CR4 */
        port_set_an_resolved_params_internal(
                port_speed_t::PORT_SPEED_40G, 4, fec_type);
        break;

    case 0x0a: /* 25GBASE-KRCR-S */
        // (1) For 25G PHYs if neither PHY requests FEC operation in bits F2 or
        //     F3 then FEC is not enabled.
        // (2) For 25GBASE-KR-S and 25GBASE-CR-S PHYs if either PHY requests
        //     RS-FEC or BASE-R FEC then BASE-R operation is enabled. This is
        //     because 25GBASE-KR-S and 25GBASE-CR-S PHYs do not support RS-FEC
        //     operation.
        if (fec_enable == 1 || rsfec_enable == 1) {
            fec_type = port_fec_type_t::PORT_FEC_TYPE_FC;
        }

        port_set_an_resolved_params_internal(
                port_speed_t::PORT_SPEED_25G, 1, fec_type);
        break;

    case 0x0b: /* 25GBASE-KRCR */
        // (1) For 25G PHYs if neither PHY requests FEC operation in bits F2 or
        //     F3 then FEC is not enabled.
        // (2) For 25GBASE-KR and 25GBASE-CR PHYs if either PHY requests RS-FEC
        //     then RS-FEC operation is enabled, otherwise if either PHY
        //     requests BASE-R FEC then BASE-R operation is enabled.
        if (fec_enable == 1) {
            fec_type = port_fec_type_t::PORT_FEC_TYPE_FC;
        } else if(rsfec_enable == 1) {
            fec_type = port_fec_type_t::PORT_FEC_TYPE_RS;
        }

        port_set_an_resolved_params_internal(
                port_speed_t::PORT_SPEED_25G, 1, fec_type);
        break;

    case 0x02:  /* 10GBASE-KR */
        port_set_an_resolved_params_internal(
                port_speed_t::PORT_SPEED_10G, 1, fec_type);
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
        SDK_LINKMGR_TRACE_ERR("port: %d, unsupported an_hcd: %d",
                              port_num(), an_hcd);
        ret = -1;
        break;
    }

    return ret;
}

sdk_ret_t
port::port_timer_start(sdk::event_thread::timer_t *timer, double timeout) {
    timer->ev_watcher.repeat = timeout/1000.0;    // convert ms to secs
    timer_again(timer);
    return SDK_RET_OK;
}

an_ret_t
port::port_link_sm_an_process(void)
{
    bool an_good = false;
    int  timeout = MIN_PORT_TIMER_INTERVAL;
    an_ret_t an_ret = AN_DONE;
    struct timespec before;
    struct timespec after;
    struct timespec ts_diff;
    uint64_t timediff;

    switch(this->link_an_sm_) {
    case port_link_sm_t::PORT_LINK_SM_AN_DISABLED:

        // transition to start AN
        set_port_link_an_sm(
                port_link_sm_t::PORT_LINK_SM_AN_START);

    case port_link_sm_t::PORT_LINK_SM_AN_START:

        SDK_PORT_SM_TRACE(this, "start AN");

        port_serdes_an_start();

        // transition to wait for AN HCD
        set_port_link_an_sm(
                port_link_sm_t::PORT_LINK_SM_AN_WAIT_HCD);

    case port_link_sm_t::PORT_LINK_SM_AN_WAIT_HCD:

        SDK_PORT_SM_TRACE(this, "wait AN HCD");

        clock_gettime(CLOCK_MONOTONIC, &before);
        // 100 msecs spin for AN HCD resolution
        do {
            clock_gettime(CLOCK_MONOTONIC, &after);
            an_good = port_serdes_an_wait_hcd();
            if (an_good == true) {
                break;
            }
            ts_diff = sdk::timestamp_diff(&after, &before);
            sdk::timestamp_to_nsecs(&ts_diff, &timediff);
            if (timediff > MAX_LINK_AN_WAIT_TIME) {
                break;
            }
        } while (true);

        // If AN HCD doesn't resolve:
        //   If retry count < port_max_an_retries():
        //     start bringup timer and return with AN_WAIT
        //  Else return with AN_RESET
        if(an_good == false) {
            set_num_an_hcd_retries(num_an_hcd_retries() + 1);
            if (num_an_hcd_retries() >= port_max_an_retries()) {
                an_ret = AN_RESET;
                break;
            }
            // 10ms timer for polling since Link Training needs to
            // be started asap (within tens of ms)
            timeout = 10;
            this->bringup_timer_val_ += timeout;
            port_timer_start(link_bringup_timer(), timeout);
            an_ret = AN_WAIT;
            break;
        }

        // transition to AN complete
        set_port_link_an_sm(
                port_link_sm_t::PORT_LINK_SM_AN_COMPLETE);

    case port_link_sm_t::PORT_LINK_SM_AN_COMPLETE:

        SDK_PORT_SM_TRACE(this, "AN HCD configure");

        // If AN HCD resolves, then peer is trying to do AN.
        // Skip toggle_neg_mode for the next iteration so that
        // only AN is retried if current bringup iteration fails.
        set_skip_toggle_neg_mode(true);

        port_serdes_an_hcd_cfg();

    default:
        break;
    }
    return an_ret;
}

int
port::port_serdes_eye_get(void) {
    uint32_t lane;

    for (lane = 0; lane < num_lanes_; ++lane) {
        if (serdes_fns()->serdes_eye_get(port_sbus_addr(lane), 0) != 0) {
            return -1;
        }
    }
    return 0;
}

bool
port::port_serdes_eye_check(void) {
    uint32_t lane;
    uint32_t min;
    uint32_t max;
    uint32_t values[2 * MAX_SERDES_EYE_HEIGHTS];

    for (lane = 0; lane < num_lanes_; ++lane) {
        min = 0xFFFF;
        max = 0x0;
        memset(values, 0, sizeof(uint32_t) * 2 * MAX_SERDES_EYE_HEIGHTS);

        if (serdes_fns()->serdes_eye_check(port_sbus_addr(lane), values) != 0) {
            return false;
        }
        SDK_LINKMGR_TRACE_DEBUG("port: %u, %3x %3x %3x %3x %3x %3x %3x %3x",
                                port_num(),
                                values[0], values[1], values[2], values[3],
                                values[4], values[5], values[6], values[7]);
        for (int i = 0; i < (2*MAX_SERDES_EYE_HEIGHTS); ++i) {
            if (values[i] == 0 || values[i] == 1 ||
                values[i] == 2 || values[i] >= 0xff) {
                return false;
            }
            if (values[i] < min) {
                min = values[i];
            }
            if (values[i] > max) {
                max = values[i];
            }
        }
        // ICAL can complete with invalid signal (remote peer trying AN).
        // DFE eye values shouldn't be very far apart for a good signal.
        // Do the below validation to make sure values are valid
        if ((max - min) > min) {
            return false;
        }
    }
    return true;
}

dfe_ret_t
port::port_link_sm_dfe_process(void)
{
    bool dfe_complete = false;
    dfe_ret_t ret     = DFE_DONE;
    int  timeout      = MIN_PORT_TIMER_INTERVAL;
    bool retry_sm     = false;

    while (true) {
        retry_sm = false;

        switch(this->link_dfe_sm_) {
            case port_link_sm_t::PORT_LINK_SM_DFE_DISABLED:

                // transition to start ical
                set_port_link_dfe_sm(
                        port_link_sm_t::PORT_LINK_SM_DFE_START_ICAL);

            case port_link_sm_t::PORT_LINK_SM_DFE_START_ICAL:

                SDK_PORT_SM_DEBUG(this, "start ICAL");

                port_serdes_ical_start();

                // transition to wait for ical complete
                set_port_link_dfe_sm(
                        port_link_sm_t::PORT_LINK_SM_DFE_WAIT_ICAL);

            case port_link_sm_t::PORT_LINK_SM_DFE_WAIT_ICAL:

                SDK_PORT_SM_DEBUG(this, "wait ICAL");

                dfe_complete = port_serdes_dfe_complete();

                // ICAL needs ~0.4 secs for 100G
                if(dfe_complete == false) {
                    timeout = port_mac_sync_ical_timeout();
                    set_num_dfe_ical_cmplt_retries(
                                           num_dfe_ical_cmplt_retries() + 1);
                    // 100msecs * 10 retries = 1sec
                    if (num_dfe_ical_cmplt_retries() <
                                     port_max_ical_cmplt_retries()) {
                        // start the timer and wait for ICAL to complete
                        this->bringup_timer_val_ += timeout;
                        port_timer_start(link_bringup_timer(), timeout);
                        ret = DFE_WAIT;
                    } else {
                        // return with DFE_RESET
                        ret = DFE_RESET;
                    }
                    break;
                }

                // If eye check fails
                //   if count < MAX_PORT_SERDES_DFE_ICAL_EYE_RETRIES
                //     start/retry ICAL
                //   else
                //     return with DFE_RESET
                if (port_serdes_eye_check() == false) {
                    set_port_link_dfe_sm(
                            port_link_sm_t::PORT_LINK_SM_DFE_START_ICAL);
                    set_num_dfe_ical_eye_retries(num_dfe_ical_eye_retries()+1);
                    if (num_dfe_ical_eye_retries() <
                            port_ical_eye_retries()) {
                        // reset the ical complete retry counter
                        set_num_dfe_ical_cmplt_retries(0);
                        // start/retry ICAL
                        retry_sm = true;
                    } else {
                        // return with DFE_RESET
                        ret = DFE_RESET;
                    }
                    break;
                }

                // transition to pcal one shot
                set_port_link_dfe_sm(
                        port_link_sm_t::PORT_LINK_SM_DFE_START_PCAL);

            case port_link_sm_t::PORT_LINK_SM_DFE_START_PCAL:

                SDK_PORT_SM_DEBUG(this, "start PCAL");

                port_serdes_pcal_start();

                // transition to wait for pcal complete
                set_port_link_dfe_sm(
                        port_link_sm_t::PORT_LINK_SM_DFE_WAIT_PCAL);

            case port_link_sm_t::PORT_LINK_SM_DFE_WAIT_PCAL:

                SDK_PORT_SM_DEBUG(this, "wait PCAL");

                dfe_complete = port_serdes_dfe_complete();

                if(dfe_complete == false) {
                    timeout = MIN_PORT_TIMER_INTERVAL;
                    this->bringup_timer_val_ += timeout;
                    port_timer_start(link_bringup_timer(), timeout);
                    ret = DFE_WAIT;
                    break;
                }

            default:
                break;
        }

        if (retry_sm == false) {
            break;
        }
    }

    return ret;
}

sdk_ret_t
port::port_link_sm_process(bool start_en_timer)
{
    int  timeout    = 500; // msecs
    bool sig_detect = false;
    bool serdes_rdy = false;
    bool mac_faults = true;
    bool mac_sync   = true;
    bool retry_sm   = false;
    an_ret_t an_ret = AN_DONE;
    dfe_ret_t dfe_ret = DFE_DONE;

    while (true) {
        retry_sm = false;

        switch (this->link_sm_) {
            case port_link_sm_t::PORT_LINK_SM_DISABLED:
                // Enable MAC TX drain and set Tx/Rx=0x0
                port_mac_tx_drain(true);

                // TODO Disable and drain PB
                port_pb_flush(true);
                port_pb_drain();
                port_pb_write_control(false);

                // reset timers
                timer_stop(link_bringup_timer());
                timer_stop(link_debounce_timer());

                // make the an/fixed mode toggle here - 25G-CU only
                toggle_negotiation_mode();

                // pull the stats and save in persist storage before bringing down
                port_mac_stats_persist_update();

                // reset MAC
                port_mac_state_reset();

                // reset SERDES
                port_serdes_state_reset();

                // reset counters
                port_link_sm_counters_reset();

                // reset state

                // remove from link poll timer if present
                port_link_poll_timer_delete(this);

                // set operational status as down
                this->set_oper_status(port_oper_status_t::PORT_OPER_STATUS_DOWN);

                SDK_PORT_SM_DEBUG(this, "Disabled");

                break;

            case port_link_sm_t::PORT_LINK_SM_ENABLED:

                // link bringup time = last_up_ts - last_down_ts
                // set last_down_ts:
                //  - on link down event
                //  - link is enabled from config
                if (start_en_timer == true) {
                    set_last_down_ts();
                }
                if ((start_en_timer == true) ||
                    (SDK_ATOMIC_LOAD_BOOL(&hal_cfg) == true)) {
                    // if this is invoked by cfg thread, start the timer so that
                    // the call can return
                    this->set_port_link_sm(port_link_sm_t::PORT_LINK_SM_ENABLED);
                    timeout = MIN_PORT_TIMER_INTERVAL;
                    this->bringup_timer_val_ += timeout;
                    port_timer_start(link_bringup_timer(), timeout);
                    break;
                }

                // reset counter for FEC retry with different FEC modes
                set_fec_retries(0);

                SDK_PORT_SM_DEBUG(this, "Enabled");

                // transition to AN cfg state
                this->set_port_link_sm(port_link_sm_t::PORT_LINK_SM_AN_CFG);

             case port_link_sm_t::PORT_LINK_SM_AN_CFG:

                if (is_auto_neg() == AUTO_NEG) {
                    an_ret = port_link_sm_an_process();

                    if (an_ret == AN_RESET) {
                        // AN HCD resolution failed. Restart AN
                        retry_sm = true;
                        set_num_an_hcd_retries(0);
                        // skip the serdes reset since it is done during AN_INIT
                        port_link_sm_retry_enabled(false);
                        break;
                    } else if (an_ret == AN_WAIT) {
                        // Timers started by AN SM. Just exit.
                        break;
                    } else if (an_ret == AN_SKIP) {
                        // only for debugging
                        break;
                    } else {
                        // AN SM configures serdes
                        // transition to wait serdes rdy state
                        this->set_port_link_sm(
                                port_link_sm_t::PORT_LINK_SM_WAIT_SERDES_RDY);
                        retry_sm = true;
                        break;
                    }
                }

                // transition to serdes cfg state
                this->set_port_link_sm(port_link_sm_t::PORT_LINK_SM_SERDES_CFG);

            case port_link_sm_t::PORT_LINK_SM_SERDES_CFG:

                SDK_PORT_SM_DEBUG(this, "SerDes CFG");

                // configure the serdes
                port_serdes_cfg();
                // enable serdes tx and rx
                port_serdes_tx_rx_enable(true);

                // in fixed mode, oper fec_type = derived_fec_type
                set_fec_type(derived_fec_type());

                // transition to wait for serdes rdy state
                this->set_port_link_sm(port_link_sm_t::PORT_LINK_SM_WAIT_SERDES_RDY);

            case port_link_sm_t::PORT_LINK_SM_WAIT_SERDES_RDY:

                SDK_PORT_SM_DEBUG(this, "Wait SerDes RDY");

                serdes_rdy = port_serdes_rdy();

                if(serdes_rdy == false) {
                    set_num_serdes_ready_retries(num_serdes_ready_retries()+1);
                    if (num_serdes_ready_retries() >=
                                         port_max_serdes_ready_retries()) {
                        retry_sm = true;
                        set_num_serdes_ready_retries(0);
                        // reset serdes and try SM again
                        port_link_sm_retry_enabled(true);
                        break;
                    }
                    this->bringup_timer_val_ += timeout;
                    port_timer_start(link_bringup_timer(), timeout);
                    break;
                }
                set_num_serdes_ready_retries(0);

                // transition to mac cfg state
                this->set_port_link_sm(port_link_sm_t::PORT_LINK_SM_MAC_CFG);

            case port_link_sm_t::PORT_LINK_SM_MAC_CFG:

                SDK_PORT_SM_DEBUG(this, "MAC CFG");

                // configure the mac
                port_mac_cfg();

                // send MAC remote faults
                port_mac_send_remote_faults(true);

                // bring MAC out of reset and enable
                port_mac_enable(true);
                port_mac_soft_reset(false);

                if (is_auto_neg() == AUTO_NEG) {
                    // Restart AN if link training failed.
                    if (port_serdes_an_link_train_check() == false ||
                            port_serdes_eye_check() == false ) {
                        set_num_link_train_retries(num_link_train_retries() + 1);

                        // if LT fails MAX times, retry enabled after timeout
                        // else retry enabled in the same loop
                        if (num_link_train_retries() == MAX_LINK_TRAIN_FAIL_COUNT) {
                            set_num_link_train_retries(0);
                            timeout = rand() % 2 == 0? 500 : MIN_PORT_TIMER_INTERVAL;
                            this->bringup_timer_val_ += timeout;
                            port_timer_start(link_bringup_timer(), timeout);
                        } else {
                            retry_sm = true;
                        }
                        set_num_an_hcd_retries(0);
                        // skip the serdes reset since it is done during AN_INIT
                        port_link_sm_retry_enabled(false);
                        break;
                    }

                    // Link training would have completed ICAL.
                    // Transition DFE SM to pcal one shot for AN
                    set_port_link_dfe_sm(
                            port_link_sm_t::PORT_LINK_SM_DFE_START_PCAL);

                    // transition link SM to AN DFE tuning
                    this->set_port_link_sm(
                                    port_link_sm_t::PORT_LINK_SM_AN_DFE_TUNING);
                    retry_sm = true;
                    break;
                }

                // transition to serdes signal detect state
                this->set_port_link_sm(port_link_sm_t::PORT_LINK_SM_SIGNAL_DETECT);
                retry_sm = true;
                break;

            case port_link_sm_t::PORT_LINK_SM_AN_DFE_TUNING:
                dfe_ret = port_link_sm_dfe_process();
                if (dfe_ret == DFE_WAIT) {
                    // DFE tuning is pending
                    // Timer would have been already started. So just break.
                    break;
                }

                // transition to clear MAC remote faults
                this->set_port_link_sm(
                    port_link_sm_t::PORT_LINK_SM_CLEAR_MAC_REMOTE_FAULTS);
                retry_sm = true;
                break;

            case port_link_sm_t::PORT_LINK_SM_SIGNAL_DETECT:

                // enable serdes
                port_serdes_output_enable(true);

                SDK_PORT_SM_DEBUG(this, "Wait for signal detect");

                sig_detect = port_serdes_signal_detect();

                if(sig_detect == false) {
                    set_num_sig_detect_retries(num_sig_detect_retries()+1);
                    if (num_sig_detect_retries() >=
                                       MAX_PORT_SERDES_SIG_DETECT_RETRIES) {
                        retry_sm = true;
                        set_num_sig_detect_retries(0);
                        port_link_sm_retry_enabled(true);
                    } else {
                        // 100msec * 10 = 1 sec
                        timeout = 100;
                        this->bringup_timer_val_ += timeout;
                        port_timer_start(link_bringup_timer(), timeout);
                    }
                    break;
                }

                // transition to DFE tuning stage
                this->set_port_link_sm(port_link_sm_t::PORT_LINK_SM_DFE_TUNING);

            case port_link_sm_t::PORT_LINK_SM_DFE_TUNING:

                if (port_dfe_tuning_enabled()) {
                    dfe_ret = port_link_sm_dfe_process();
                    if (dfe_ret == DFE_WAIT) {
                        // DFE tuning is pending
                        // Timer would have been already started. So just break.
                        break;
                    } else if (dfe_ret == DFE_RESET) {
                        retry_sm = true;
                        set_num_dfe_ical_cmplt_retries(0);
                        set_num_dfe_ical_eye_retries(0);
                        port_link_sm_retry_enabled(false);
                        break;
                    }
                }

                // transition to clear MAC remote faults
                this->set_port_link_sm(
                        port_link_sm_t::PORT_LINK_SM_CLEAR_MAC_REMOTE_FAULTS);

            case port_link_sm_t::PORT_LINK_SM_CLEAR_MAC_REMOTE_FAULTS:
                SDK_PORT_SM_DEBUG(this, "Clear MAC remote faults");

                // clear MAC remote faults
                port_mac_send_remote_faults(false);

                // transition to wait for mac sync
                this->set_port_link_sm(
                                port_link_sm_t::PORT_LINK_SM_WAIT_MAC_SYNC);

            case port_link_sm_t::PORT_LINK_SM_WAIT_MAC_SYNC:
                SDK_PORT_SM_DEBUG(this, "Wait MAC SYNC");

                mac_sync = port_mac_sync_get();

                if(mac_sync == false) {
                    set_num_mac_sync_retries(num_mac_sync_retries() + 1);
                    if (num_mac_sync_retries() >= port_max_mac_sync_retries()) {
                        retry_sm = true;
                        set_num_mac_sync_retries(0);
                        set_fec_retries(fec_retries() + 1);

                        // if MAC doesn't sync in fixed/forced mode, then
                        // try with different FEC modes
                        if ((is_auto_neg() == FIXED_NEG) &&
                                (toggle_fec_mode() == true) &&
                                (fec_retries() < MAX_PORT_FEC_RETRIES)) {
                            toggle_fec_type();
                        } else {
                            port_link_sm_retry_enabled(false);
                        }
                        break;
                    }
                    // reduce MAC sync timer to 100ms for 25G/10G
                    timeout = port_mac_sync_ical_timeout();
                    this->bringup_timer_val_ += timeout;
                    port_timer_start(link_bringup_timer(), timeout);
                    break;
                }

                // transition to mac faults to be cleared
                this->set_port_link_sm(port_link_sm_t::PORT_LINK_SM_WAIT_MAC_FAULTS_CLEAR);

            case port_link_sm_t::PORT_LINK_SM_WAIT_MAC_FAULTS_CLEAR:

                SDK_PORT_SM_DEBUG(this, "Wait MAC faults clear");

                mac_faults = port_mac_faults_get();
                if (mac_faults == true) {
                    set_mac_faults(this->mac_faults() + 1);
                } else {
                    set_mac_faults(0);
                }

                // If the PCS faults/errors after MAC sync is persistent, restart SM
                if (this->mac_faults() > 0) {
                    // if MAC errors are detected, reset the MAC no-faults counter
                    set_num_mac_nofaults(0);

                    if (this->mac_faults() < MAX_PORT_MAC_FAULTS_CHECK) {
                        SDK_PORT_SM_DEBUG(this, "MAC faults detected");
                        timeout = MIN_PORT_TIMER_INTERVAL;
                        this->bringup_timer_val_ += timeout;
                        port_timer_start(link_bringup_timer(), timeout);
                        break;
                    } else {
                        SDK_PORT_SM_DEBUG(this,
                                            "MAC faults persistent, retry SM");
                        retry_sm = true;
                        set_mac_faults(0);
                        port_link_sm_retry_enabled(false);
                        break;
                    }
                } else if (fec_type() == port_fec_type_t::PORT_FEC_TYPE_NONE) {
                    // For 10G/25G-no-fec scenario, comira MAC has an issue
                    // where MAC gets sync and PCS error counter could still
                    // be 0 though peer is shutdown or not connected.
                    // To workaround that, check the error counter
                    // multiple times
                    if (port_speed() == port_speed_t::PORT_SPEED_10G ||
                        port_speed() == port_speed_t::PORT_SPEED_25G) {
                        set_num_mac_nofaults(num_mac_nofaults() + 1);
                        if (num_mac_nofaults() < MAX_PORT_MAC_NOFAULTS_CHECK) {
                            SDK_PORT_SM_DEBUG(this, "MAC faults check retry");
                            timeout = MIN_PORT_TIMER_INTERVAL;
                            this->bringup_timer_val_ += timeout;
                            port_timer_start(link_bringup_timer(), timeout);
                            break;
                        }
                    }
                }

                // transition to link up state
                this->set_port_link_sm(port_link_sm_t::PORT_LINK_SM_UP);

            case port_link_sm_t::PORT_LINK_SM_UP:

                if (port_dfe_tuning_enabled()) {
                    SDK_PORT_SM_DEBUG(this, "PCAL continuous");
                    port_serdes_pcal_continuous_start();
                }

                set_last_up_ts();

                SDK_PORT_SM_TRACE(this, "Link UP");

                // start persist stats collection - relevant on first link-up and remains set
                port_mac_stats_persist_collect_enable();

                // TODO Enable PB
                port_pb_drain();
                port_pb_flush(false);
                port_pb_write_control(true);

                // Disable MAC TX drain and set Tx/Rx=0x1
                port_mac_tx_drain(false);

                // enable mac interrupts
                port_mac_intr_en(true);

                // set operational status as up
                this->set_oper_status(port_oper_status_t::PORT_OPER_STATUS_UP);
                set_bringup_duration();

                // enable pCal

                // add to link poll timer
                port_link_poll_timer_add(this);

                // update the AN oper mode
                set_auto_neg_enable(is_auto_neg() == AUTO_NEG? true : false);

                // notify others that link is up
                port_event_notify(port_event_t::PORT_EVENT_LINK_UP);
                break;

            default:
                break;
        }
        if (retry_sm == false) {
            break;
        }
    }

    return SDK_RET_OK;
}

neg_mode_t
port::is_auto_neg (void)
{
    // check if we need to switch if autoneg is disabled
    if (this->toggle_neg_mode_ == true) {
        return this->last_neg_mode_;
    }
    // default option return
    return (auto_neg_enable() ? AUTO_NEG : FIXED_NEG);
}

void
port::toggle_negotiation_mode (void)
{
    if (skip_toggle_neg_mode() == true) {
        set_skip_toggle_neg_mode(false);
        return;
    }
    if (this->toggle_neg_mode_ == true) {
        this->last_neg_mode_ =
              (this->last_neg_mode_ == AUTO_NEG)? FIXED_NEG : AUTO_NEG;
    }
}

void
port::toggle_fec_type (void) {
    switch (fec_type()) {
    case port_fec_type_t::PORT_FEC_TYPE_FC:
        set_fec_type(port_fec_type_t::PORT_FEC_TYPE_RS);
        break;

    case port_fec_type_t::PORT_FEC_TYPE_RS:
        set_fec_type(port_fec_type_t::PORT_FEC_TYPE_NONE);
        break;

    case port_fec_type_t::PORT_FEC_TYPE_NONE:
    default:
        set_fec_type(port_fec_type_t::PORT_FEC_TYPE_FC);
        break;
    }
    SDK_PORT_SM_DEBUG(this, "Trying fec_type %u", (uint32_t)fec_type());
    port_mac_cfg_fec(fec_type());
}

sdk_ret_t
port::port_enable(bool start_en_timer)
{
    // check if already enabled
    if (this->admin_state_ == port_admin_state_t::PORT_ADMIN_STATE_UP) {
        return SDK_RET_OK;
    }

    // enable the port
    set_port_link_sm(port_link_sm_t::PORT_LINK_SM_ENABLED);
    set_port_link_dfe_sm(port_link_sm_t::PORT_LINK_SM_DFE_DISABLED);
    set_port_link_an_sm(port_link_sm_t::PORT_LINK_SM_AN_DISABLED);

    port_link_sm_process(start_en_timer);

    this->admin_state_ = port_admin_state_t::PORT_ADMIN_STATE_UP;

    return SDK_RET_OK;
}

sdk_ret_t
port::port_link_sm_reset (void)
{
    set_port_link_sm(port_link_sm_t::PORT_LINK_SM_DISABLED);
    set_port_link_dfe_sm(port_link_sm_t::PORT_LINK_SM_DFE_DISABLED);
    set_port_link_an_sm(port_link_sm_t::PORT_LINK_SM_AN_DISABLED);
    return SDK_RET_OK;
}

sdk_ret_t
port::port_disable(void)
{
    // check if already disabled
    if (this->admin_state_ == port_admin_state_t::PORT_ADMIN_STATE_DOWN) {
        return SDK_RET_OK;
    }

    // store the current link status
    port_oper_status_t prev_oper_status = oper_status();

    port_link_sm_reset();

    port_link_sm_process();

    this->admin_state_ = port_admin_state_t::PORT_ADMIN_STATE_DOWN;

    // Notfiy the link down if link was up before disable
    if (prev_oper_status == port_oper_status_t::PORT_OPER_STATUS_UP) {
        port_event_notify(port_event_t::PORT_EVENT_LINK_DOWN);
    }

    return SDK_RET_OK;
}

sdk_ret_t
port::port_event_notify(port_event_t port_event)
{
    port_event_info_t port_event_info;

    switch(port_event) {
    case port_event_t::PORT_EVENT_LINK_UP:
        break;

    case port_event_t::PORT_EVENT_LINK_DOWN:
        // increment the link down counter
        set_num_link_down(num_link_down() + 1);

        port_link_poll_timer_delete(this);
        break;

    default:
        break;
    }

    port_event_info.logical_port = port_num();
    port_event_info.event = port_event;
    port_event_info.speed = port_speed();
    port_event_info.type = port_type();
    port_event_info.fec_type = fec_type();
    port_event_info.num_lanes = num_lanes();
    port_event_info.oper_status = oper_status();
    port_event_info.auto_neg_enable = auto_neg_enable();

    if (g_linkmgr_cfg.port_event_cb) {
        g_linkmgr_cfg.port_event_cb(&port_event_info);
    }

    return SDK_RET_OK;
}

sdk_ret_t
port::port_link_dn_handler(void)
{
    // start the debounce timer
    if (this->debounce_time_ != 0) {
        port_timer_start(link_debounce_timer(), this->debounce_time_);
        return SDK_RET_OK;
    }

    port_debounce_timer_cb();

    return SDK_RET_OK;
}

bool
port::bringup_timer_expired(void)
{
    return (this->bringup_timer_val_ > MAX_LINK_BRINGUP_TIMEOUT);
}

// for absoulte time diff, use CLOCK_MONOTONIC_RAW
sdk_ret_t
port::set_last_down_ts(void)
{
    time_t now = time(NULL);

    clock_gettime(CLOCK_MONOTONIC_RAW, &this->last_down_ts_);
    strftime(this->last_down_timestamp_, TIME_STR_SIZE, "%Y-%m-%d %H:%M:%S", localtime(&now));
    sm_logger_->reset();

    return SDK_RET_OK;
}

sdk_ret_t
port::set_last_up_ts(void)
{
    clock_gettime(CLOCK_MONOTONIC_RAW, &this->last_up_ts_);
    return SDK_RET_OK;
}

// If curent oper_status is UP, then, update bringup_duration_
sdk_ret_t
port::set_bringup_duration(void)
{
    if (this->oper_status_ == port_oper_status_t::PORT_OPER_STATUS_UP) {
        this->bringup_duration_ = sdk::timestamp_diff(&this->last_up_ts_, &this->last_down_ts_);
        SDK_LINKMGR_TRACE_DEBUG("port: %u Link down count %d last bringup duration: %lus.%luns\n", port_num(),
                                num_link_down(),
                                this->bringup_duration_.tv_sec,
                                this->bringup_duration_.tv_nsec);

    }
    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_stats_get (uint64_t *stats_data)
{
    mac_fns()->mac_stats_get(this->mac_id_, this->mac_ch_, stats_data);
    // correct mac errata stats
    this->port_mac_stats_errata_correct(stats_data);
    // add persist stats to this current stats_data and return
    this->port_mac_stats_persist_collate(stats_data);
    // Publish collated stats to HBM
    this->port_mac_stats_publish(stats_data);
    return SDK_RET_OK;
}

sdk_ret_t
port::port_deinit (void)
{
    mac_fns()->mac_deinit (this->mac_id_, this->mac_ch_);
    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_set_pause_src_addr(uint8_t *mac_addr) {
    mac_fns()->mac_pause_src_addr(mac_id(), mac_ch(), mac_addr);
    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_state_reset(void)
{
    // disable and clear mac interrupts
    port_mac_intr_en(false);
    port_mac_intr_clr();

    // disable and put mac in reset
    port_mac_soft_reset(true);
    port_mac_enable(false);

    // reset MAC global programming since serdes config
    // is changed to run AN/force
    // TODO: Need enhancement for breakout/switch of 100G/25G/10G in same MAC
    port_deinit();
    return SDK_RET_OK;
}

sdk_ret_t
port::port_serdes_state_reset(void)
{
    // disable serdes
    port_serdes_output_enable(false);

    // soft spico reset
    port_serdes_reset(true);
    return SDK_RET_OK;
}

void
port::port_link_sm_counters_reset(void)
{
    // reset bringup timer
    this->bringup_timer_val_ = 0;

    // reset MAC faults counter
    set_mac_faults(0);

    // reset MAC no-faults counter
    set_num_mac_nofaults(0);

    // reset number of MAC sync retries
    set_num_mac_sync_retries(0);

    // reset AN HCD retry counter
    set_num_an_hcd_retries(0);

    // reset serdes ready retry counter
    set_num_serdes_ready_retries(0);

    // reset serdes signal detect retry counter
    set_num_sig_detect_retries(0);

    // reset AN link training retry counter
    set_num_link_train_retries(0);

    // reset dfe retry count
    set_num_dfe_ical_cmplt_retries(0);
    set_num_dfe_ical_eye_retries(0);

    // reset number of link bringup retries
    set_num_retries(0);
}

sdk_ret_t
port::port_link_sm_retry_enabled(bool serdes_reset)
{
    // reset all SM states
    port_link_sm_reset();
    if (serdes_reset == true) {
        port_serdes_state_reset();
    }
    this->set_port_link_sm(port_link_sm_t::PORT_LINK_SM_ENABLED);

    // make the an/fixed mode toggle here - 25G-CU only
    toggle_negotiation_mode();

    return SDK_RET_OK;
}

// Function to add stats before last reset to current stats and return
sdk_ret_t
port::port_mac_stats_persist_collect_enable(void)
{
    if (this->persist_stats_collect_ == false) {
        SDK_PORT_SM_TRACE(this, "Enabling persistent stats collection");
    }
    this->persist_stats_collect_ = true;
    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_stats_persist_collect_disable(void)
{
    this->persist_stats_collect_ = false;
    SDK_PORT_SM_TRACE(this, "Disabling persistent stats collection");
    return SDK_RET_OK;
}

// Receiving PFC frames increments PORT_FRAMES_RX_PRIPAUSE, PORT_FRAMES_RX_PRI_x and PORT_FRAMES_RX_PAUSE
// Receiving LLC frames increments PORT_FRAMES_RX_PAUSE and PORT_FRAMES_RX_PRIPAUSE
// PORT_FRAMES_RX_PRIPAUSE counter doesn’t match aggregation of PORT_FRAMES_RX_PRI_x
sdk_ret_t
port::port_mac_stats_errata_correct(uint64_t *stats_data)
{
    uint64_t rx_pri_pause_all = 0;

    // errata correction only for MX ports
    if (port_type() == port_type_t::PORT_TYPE_MGMT) {
        return SDK_RET_OK;
    }
    // pripause frames summation is correct; PORT_FRAMES_RX_PRIPAUSE incorrect
    rx_pri_pause_all = stats_data[PORT_FRAMES_RX_PRI_0] +
                       stats_data[PORT_FRAMES_RX_PRI_1] +
                       stats_data[PORT_FRAMES_RX_PRI_2] +
                       stats_data[PORT_FRAMES_RX_PRI_3] +
                       stats_data[PORT_FRAMES_RX_PRI_4] +
                       stats_data[PORT_FRAMES_RX_PRI_5] +
                       stats_data[PORT_FRAMES_RX_PRI_6] +
                       stats_data[PORT_FRAMES_RX_PRI_7];

    stats_data[PORT_FRAMES_RX_PRIPAUSE] = rx_pri_pause_all;
    if (rx_pri_pause_all == 0) {
        // PORT_FRAMES_RX_PAUSE (llfc) only
        return SDK_RET_OK;
    }

    // counters can increment between each read (sw latency)
    if (stats_data[PORT_FRAMES_RX_PAUSE] >= rx_pri_pause_all) {
        stats_data[PORT_FRAMES_RX_PAUSE] = stats_data[PORT_FRAMES_RX_PAUSE] - rx_pri_pause_all;
    } else {
        // clearly, if we are here, no LLFC stats incremented
        stats_data[PORT_FRAMES_RX_PAUSE] = 0;
    }
    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_stats_persist_collate(uint64_t *stats_data)
{
    int iter;

    for (iter = 0; iter < MAX_MAC_STATS; iter++) {
        stats_data[iter] += this->persist_stats_data_[iter];
    }
    return SDK_RET_OK;
}

// Init mac stats reporting to HBM (json needs entry)
sdk_ret_t
port::port_mac_stats_init(void)
{
    uint32_t ifindex;

    ifindex = sdk::lib::catalog::logical_port_to_ifindex(port_num());
    this->port_stats_base_addr_ = sdk::linkmgr::port_stats_addr(ifindex);
    if (this->port_stats_base_addr_ == INVALID_MEM_ADDRESS) {
        return SDK_RET_ERR;
    }
    SDK_TRACE_DEBUG("port %u stats_init, port_stats_base_addr_ = 0x%lx",
                    this->port_num_, this->port_stats_base_addr_);
    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_stats_init(port *port_p)
{
    return port_p->port_mac_stats_init();
}

sdk_ret_t
port::port_mac_stats_publish(uint64_t *stats_data)
{
    uint32_t len;
    sdk_ret_t ret = SDK_RET_OK;

    if (this->port_stats_base_addr_ == INVALID_MEM_ADDRESS) {
        // We don't publish to HBM when we don't have the port_stats region available
        return SDK_RET_OK; // return ok. legacy stats collection
    }

    len = MAX_MAC_STATS * sizeof(uint64_t); // byte granularity

    ret = sdk::asic::asic_mem_write(this->port_stats_base_addr_, (uint8_t *) stats_data, len);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_DEBUG("port %u stats_publish port_stats_base_addr_ = 0x%lx asic_mem_write failed ret = 0x%x",
                         this->port_num_, this->port_stats_base_addr_, ret);
    }
    return ret;
}

sdk_ret_t
port::port_mac_stats_persist_update(void)
{
    int iter;
    uint64_t stats_data[MAX_MAC_STATS];

    if (this->persist_stats_collect_ == true) {
        mac_fns()->mac_stats_get(this->mac_id_, this->mac_ch_, stats_data);

        // correct mac errata stats
        this->port_mac_stats_errata_correct(stats_data);
        SDK_PORT_SM_TRACE(this, "Updating persistent stats");
        for (iter = 0; iter < MAX_MAC_STATS; iter++) {
            this->persist_stats_data_[iter] += stats_data[iter];
        }
    }
    return SDK_RET_OK;
}

sdk_ret_t
port::port_mac_stats_persist_clear(bool reset)
{
    if (reset) {
        SDK_PORT_SM_TRACE(this, "Clearing persistent stats");
        memset(this->persist_stats_data_, 0, sizeof(this->persist_stats_data_));
    }
    return SDK_RET_OK;
}

sdk_ret_t
port::port_pb_drain (void)
{
    uint32_t tm_port = logical_port_to_tm_port(port_num());

    return asicpd_tm_drain_uplink_port(tm_port);
}

sdk_ret_t
port::port_pb_write_control(bool enable)
{
    uint32_t tm_port = logical_port_to_tm_port(port_num());

    return asicpd_tm_write_control_uplink_port(tm_port, enable);
}

sdk_ret_t
port::port_pb_flush (bool enable)
{
    uint32_t tm_port = logical_port_to_tm_port(port_num());

    return asicpd_tm_flush_uplink_port(tm_port, enable);
}

sdk_ret_t
port::port_pb_enable (bool enable)
{
    uint32_t tm_port = logical_port_to_tm_port(port_num());

    return asicpd_tm_enable_disable_uplink_port(tm_port, enable);
}

sdk_ret_t
port::port_mac_tx_drain(bool drain) {
    mac_fns()->mac_tx_drain(mac_id(), mac_ch(), drain);
    return SDK_RET_OK;
}

sdk_ret_t
port::timers_init(void) {
    sdk::event_thread::timer_t *timer = link_bringup_timer();

    // init the bringup timer
    timer->ctx = this;
    timer_init(timer, port_bringup_timer_cb,
               PORT_TIMER_INIT_TIME, 0);

    // init the debounce timer
    timer = link_debounce_timer();
    timer->ctx = this;
    timer_init(timer, sdk::linkmgr::port_debounce_timer_cb,
               PORT_TIMER_INIT_TIME, 0);

    return SDK_RET_OK;
}

uint32_t
port::port_max_ical_cmplt_retries (void) {
    return MAX_PORT_SERDES_DFE_ICAL_CMPLT_RETRIES;
}

uint32_t
port::port_ical_eye_retries (void) {
    uint32_t retries;

    if (get_file_value(ICAL_EYE_RETRIES_FILE, &retries) == true) {
        if (retries > 0 && retries < MAX_ICAL_EYE_RETRIES) {
            SDK_PORT_SM_DEBUG(this, "%s retries %u", ICAL_EYE_RETRIES_FILE,
                              retries);
            return retries;
        }
    }
    switch (port_speed()) {
    case port_speed_t::PORT_SPEED_10G:
        return MAX_PORT_SERDES_DFE_ICAL_EYE_RETRIES_10G;
    default:
        return MAX_PORT_SERDES_DFE_ICAL_EYE_RETRIES;
    }
}

uint32_t
port::port_max_mac_sync_retries (void) {
    switch (port_speed()) {
    case port_speed_t::PORT_SPEED_10G:
        return MAX_PORT_MAC_SYNC_RETRIES_10G;
    default:
        return MAX_PORT_MAC_SYNC_RETRIES;
    }
}

uint32_t
port::port_mac_sync_ical_timeout (void) {
    uint32_t timeout;

    if (get_file_value(MAC_SYNC_ICAL_FILE, &timeout) == true) {
        if (timeout > 0 && timeout < MAX_MAC_SYNC_ICAL_TIMEOUT) {
            SDK_PORT_SM_DEBUG(this, "%s timeout %u", MAC_SYNC_ICAL_FILE,
                              timeout);
            return timeout;
        }
    }
    switch (port_speed()) {
    case port_speed_t::PORT_SPEED_100G:
    case port_speed_t::PORT_SPEED_40G:
    case port_speed_t::PORT_SPEED_10G:
        return 500; // msec
    default:
        return 100; // msec
    }
}

//----------------------------------------------------
// static methods
//----------------------------------------------------
sdk_ret_t
port::port_init(linkmgr_cfg_t *cfg)
{
    sdk_ret_t   rc           = SDK_RET_OK;

    SDK_ASSERT(cfg->cfg_path);

    rc = port_mac_fn_init(cfg);
    if (rc != SDK_RET_OK) {
        SDK_TRACE_ERR("port mac init failed");
    }

    rc = port_serdes_fn_init(cfg->platform_type,
                             jtag_id(),
                             num_sbus_rings(),
                             aacs_server_en(),
                             aacs_connect(),
                             aacs_server_port(),
                             aacs_server_ip());
    if (rc != SDK_RET_OK) {
        SDK_TRACE_ERR("port mac init failed");
    }

    return rc;
}

sdk_ret_t
port::phy_port_mac_addr(uint32_t phy_port, mac_addr_t mac_addr) {
    std::string   mac_addr_str;

    if (sdk::platform::readfrukey(BOARD_MACADDRESS_KEY, mac_addr_str) == -1) {
        return SDK_RET_ERR;
    }

    mac_str_to_addr((char *)mac_addr_str.c_str(), mac_addr);

    // base mac addr is for first xcvr port, increment the last byte for
    // subsequent ports
    mac_addr[5] += (uint8_t)(phy_port - 1);
    return SDK_RET_OK;
}

sdk_ret_t
port::port_enable(port *port_p)
{
    sdk_ret_t ret;

    // wait for linkmgr control thread to process port event
    while (!is_linkmgr_ctrl_thread_ready()) {
        pthread_yield();
    }

    linkmgr_entry_data_t data;
    data.ctxt  = port_p;
    data.timer = NULL;
    ret = linkmgr_notify(LINKMGR_OPERATION_PORT_ENABLE, &data,
                         q_notify_mode_t::Q_NOTIFY_MODE_BLOCKING);

    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR("Error notifying control-thread for port enable");
    }
    return ret;
}

sdk_ret_t
port::port_disable(port *port_p)
{
    sdk_ret_t ret;

    // wait for linkmgr control thread to process port event
    while (!is_linkmgr_ctrl_thread_ready()) {
        pthread_yield();
    }

    linkmgr_entry_data_t data;
    data.ctxt  = port_p;
    data.timer = NULL;

    ret = linkmgr_notify(LINKMGR_OPERATION_PORT_DISABLE, &data,
                         q_notify_mode_t::Q_NOTIFY_MODE_BLOCKING);

    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR("Error notifying control-thread for port disable");
    }
    return ret;
}

void
port::report_sm_(port_link_sm_t link_sm)
{
    int sm = static_cast<int>(link_sm);

    sm_logger_->append((char *)&sm);
}

}    // namespace linkmgr
}    // namespace sdk
