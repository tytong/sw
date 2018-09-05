
#include <iostream>
#include <cmath>

#include "ethlif.hpp"
#include "print.hpp"

using namespace std;


const char qstate_64[64] = { 0 };
const char qstate_1024[1024] = { 0 };


sdk::lib::indexer *EthLif::filter_allocator = sdk::lib::indexer::factory(EthLif::max_filters_per_lif, false, true);

EthLifMap EthLif::ethlif_db;


EthLif *
EthLif::Factory(lif_info_t *info)
{
    api_trace("EthLif Create");

    if (ethlif_db.find(info->hw_lif_id) != ethlif_db.end()) {
        HAL_TRACE_WARN("Duplicate Create of EthLif with id: {}",
                       info->hw_lif_id);
        return NULL;
    }

    EthLif *eth_lif = new EthLif(info);

    // Store in DB for disruptive upgrade
    ethlif_db[info->hw_lif_id] = eth_lif;

    // Create Enic for every Lif in Classic Mode
    if (hal->GetMode() == FWD_MODE_CLASSIC) {
        eth_lif->SetEnic(Enic::Factory(eth_lif));
    } else {
        // If its promiscuos. send (Lif, *, *) filter to HAL
        if (info->receive_promiscuous) {
            eth_lif->CreateMacVlanFilter(0, 0);
        }
    }

    return eth_lif;
}

void
EthLif::Destroy(EthLif *eth_lif)
{
    api_trace("EthLif Delete");

    // Remove from DB
    ethlif_db.erase(eth_lif->GetHwLifId());

    if (eth_lif) {

        eth_lif->remove_mac_filters();
        eth_lif->remove_vlan_filters();
        eth_lif->remove_mac_vlan_filters();


        if (hal->GetMode() == FWD_MODE_CLASSIC) {
            Enic::Destroy(eth_lif->GetEnic());
        } else {
            if (eth_lif->GetIsPromiscuous()) {
                eth_lif->DeleteMacVlanFilter(0, 0);
            }
        }

        eth_lif->~EthLif();
    }
}

EthLif::EthLif(lif_info_t *info)
{
    memcpy(&info_, info, sizeof(lif_info_t));

    lif_ = Lif::Factory(this);
}

EthLif::~EthLif()
{
    mac_table_.clear();
    vlan_table_.clear();
    mac_vlan_table_.clear();
    mac_vlan_filter_table.clear();

    // Delete Lif
    Lif::Destroy(lif_);
}

void
EthLif::remove_mac_filters()
{
    // Remove mac filters
    for (auto it = mac_table_.begin(); it != mac_table_.end(); it++) {
        DelMac(*it);
    }

}

void
EthLif::remove_vlan_filters()
{
    // Remove vlan filters
    for (auto it = vlan_table_.begin(); it != vlan_table_.end(); it++) {
        DelVlan(*it);
    }
}

void
EthLif::remove_mac_vlan_filters()
{
    // Remove (mac,vlan) filters
    for (auto it = mac_vlan_table_.begin(); it != mac_vlan_table_.end(); it++) {
        DelMacVlan(std::get<0>(*it), std::get<1>(*it));
    }
}


void
EthLif::AddMac(mac_t mac)
{
    mac_vlan_t mac_vlan;

    api_trace("Adding Mac Filter");
    HAL_TRACE_DEBUG("Adding Mac filter: {}", macaddr2str(mac));

    if (mac_table_.find(mac) == mac_table_.end()) {
        /*
         * Classic:
         *      - Walk through Vlans and create (Mac,Vlan) filters
         * Smart:
         *      - Create Mac filter
         */
        if (hal->GetMode() == FWD_MODE_CLASSIC) {
            // Register new mac across all existing vlans
            for (auto vlan_it = vlan_table_.cbegin(); vlan_it != vlan_table_.cend(); vlan_it++) {
                // Check if (MacVlan) filter is already present
                mac_vlan = make_tuple(mac, *vlan_it);
                if (mac_vlan_table_.find(mac_vlan) == mac_vlan_table_.end()) {
                    // No (MacVlan) filter. Creating (Mac, Vlan)
                    CreateMacVlanFilter(mac, *vlan_it);
                } else {
                    HAL_TRACE_DEBUG("(Mac,Vlan) filter present. No-op");
                }
            }
        } else {
            CreateMacFilter(mac);
        }

        // Store mac filter
        mac_table_.insert(mac);
    } else {
        HAL_TRACE_WARN("Mac already registered: {}", mac);
    }
}

void
EthLif::DelMac(mac_t mac)
{
    mac_vlan_t mac_vlan_key, mac_key, vlan_key;

    api_trace("Deleting Mac Filter");
    HAL_TRACE_DEBUG("Deleting Mac filter: {}", macaddr2str(mac));

    mac_key = make_tuple(mac, 0);
    if (mac_table_.find(mac) != mac_table_.end()) {
        if (hal->GetMode() == FWD_MODE_CLASSIC) {
            for (auto vlan_it = vlan_table_.cbegin(); vlan_it != vlan_table_.cend(); vlan_it++) {
                vlan_key = make_tuple(0, *vlan_it);
                mac_vlan_key = make_tuple(mac, *vlan_it);
                if (vlan_table_.find(*vlan_it) != vlan_table_.end() &&
                    mac_vlan_table_.find(mac_vlan_key) == mac_vlan_table_.end()) {
                    HAL_TRACE_DEBUG("Mac Delete: Mac, Vlan are present but (Mac,Vlan) is not. Remove (Mac,Vlan) entity");
                    // Mac, Vlan are present and (Mac,Vlan) is not
                    DeleteMacVlanFilter(mac, *vlan_it);
                } else {
                    // Case:
                    //  Case 1: Vlan filter not present but (Mac,Vlan) is either present or not.
                    //  Case 2: Vlan filter is present along with (Mac,Vlan)
                    HAL_TRACE_DEBUG("Mac Delete: No-op");
                }
            }
        } else {
            DeleteMacFilter(mac);
        }

        // Erase mac filter
        mac_table_.erase(mac);
    } else {
        HAL_TRACE_ERR("Mac not registered: {}", mac);
    }
}

void
EthLif::AddVlan(vlan_t vlan)
{
    mac_vlan_t mac_vlan;

    api_trace("Adding Vlan Filter");
    HAL_TRACE_DEBUG("Adding Vlan filter: {}", vlan);

    if (vlan_table_.find(vlan) == vlan_table_.end()) {
        /*
         * Classic:
         *      - Walk through Vlans and create (Mac,Vlan) filters
         * Smart:
         *      - Create Vlan filter
         */
        if (hal->GetMode() == FWD_MODE_CLASSIC) {
            // Register new mac across all existing vlans
            for (auto it = mac_table_.cbegin(); it != mac_table_.cend(); it++) {
                // Check if (MacVlan) filter is already present
                mac_vlan = make_tuple(*it, vlan);
                if (mac_vlan_table_.find(mac_vlan) == mac_vlan_table_.end()) {
                    // No (MacVlan) filter. Creating (Mac, Vlan)
                    CreateMacVlanFilter(*it, vlan);
                } else {
                    HAL_TRACE_DEBUG("(Mac,Vlan) filter present. No-op");
                }
            }
        } else {
            CreateVlanFilter(vlan);
        }

        // Store vlan filter
        vlan_table_.insert(vlan);
    } else {
        HAL_TRACE_WARN("Vlan already registered: {}", vlan);
    }
}

void
EthLif::DelVlan(vlan_t vlan)
{
    mac_vlan_t mac_vlan_key;

    api_trace("Deleting Vlan Filter");
    HAL_TRACE_DEBUG("Deleting Vlan filter: {}", vlan);

    if (vlan_table_.find(vlan) != vlan_table_.end()) {
        if (hal->GetMode() == FWD_MODE_CLASSIC) {
            for (auto it = mac_table_.cbegin(); it != mac_table_.cend(); it++) {
                mac_vlan_key = make_tuple(*it, vlan);
                if (mac_table_.find(*it) != mac_table_.end() &&
                    mac_vlan_table_.find(mac_vlan_key) == mac_vlan_table_.end()) {
                    HAL_TRACE_DEBUG("Vlan Delete: Mac, Vlan are present but (Mac,Vlan) is not. Remove (Mac,Vlan) entity");
                    // Mac, Vlan are present and (Mac,Vlan) is not
                    DeleteMacVlanFilter(*it, vlan);
                } else {
                    // Case:
                    //  Case 1: Mac filter not present but (Mac,Vlan) is either present or not.
                    //  Case 2: Mac filter is present along with (Mac,Vlan)
                    HAL_TRACE_DEBUG("Vlan Delete: No-op");
                }
            }
        } else {
            DeleteVlanFilter(vlan);
        }

        // Erase mac filter
        vlan_table_.erase(vlan);
    } else {
        HAL_TRACE_ERR("Vlan not registered: {}", vlan);
    }
}

void
EthLif::AddMacVlan(mac_t mac, vlan_t vlan)
{
    mac_vlan_t key(mac, vlan);

    api_trace("Adding (Mac,Vlan) Filter");
    HAL_TRACE_DEBUG("Adding (Mac,Vlan) mac: {}, filter: {}", macaddr2str(mac), vlan);

    if (mac_vlan_table_.find(key) == mac_vlan_table_.end()) {
        if (hal->GetMode() == FWD_MODE_CLASSIC) {
            // Check if mac filter and vlan filter is present
            if (mac_table_.find(mac) == mac_table_.end() ||
                vlan_table_.find(vlan) == vlan_table_.end()) {
                CreateMacVlanFilter(mac, vlan);
            } else {
                HAL_TRACE_DEBUG("Mac filter and Vlan filter preset. "
                                "No-op for (Mac,Vlan) filter");
            }
        } else {
            CreateMacVlanFilter(mac, vlan);
        }

        // Store mac-vlan filter
        mac_vlan_table_.insert(key);
    } else {
        HAL_TRACE_WARN("Mac-Vlan already registered: {}", mac);
    }
}

void
EthLif::DelMacVlan(mac_t mac, vlan_t vlan)
{
    mac_vlan_t mac_vlan_key;

    api_trace("Deleting (Mac,Vlan) Filter");
    HAL_TRACE_DEBUG("Deleting (Mac,Vlan) mac: {}, filter: {}", macaddr2str(mac), vlan);

    mac_vlan_key = make_tuple(mac, vlan);
    if (mac_vlan_table_.find(mac_vlan_key) != mac_vlan_table_.end()) {
        if (hal->GetMode() == FWD_MODE_CLASSIC) {
            if (mac_table_.find(mac) == mac_table_.end() ||
                 vlan_table_.find(vlan) == vlan_table_.end()) {
                // One of Mac or Vlan is not present.
                // (Mac,Vlan) entity was created only by (Mac,Vlan) filter
                DeleteMacVlanFilter(mac, vlan);
            } else {
                // Mac filter and Vlan filter both exist
                HAL_TRACE_DEBUG("Mac filter and Vlan filter present. "
                                "No-op for (Mac,Vlan) filter");
            }
        } else {
            DeleteMacVlanFilter(mac, vlan);
        }
        // Erase mac-vlan filter
        mac_vlan_table_.erase(mac_vlan_key);
    } else {
        HAL_TRACE_ERR("(Mac,Vlan) already not registered: mac: {}, vlan: {}",
                      mac, vlan);
    }
}

void
EthLif::UpdateReceivePromiscuous(bool receive_promiscuous)
{
    api_trace("Promiscuous Flag change");
    if (receive_promiscuous == info_.receive_promiscuous) {
        HAL_TRACE_WARN("Prom flag: {}. No change in promiscuous flag. Nop",
                       receive_promiscuous);
        goto end;
    }

    HAL_TRACE_DEBUG("Lif: {}. Prom. flag change {} -> {}",
                    lif_->GetId(), info_.receive_promiscuous,
                    receive_promiscuous);

    info_.receive_promiscuous = receive_promiscuous;

    if (hal->GetMode() == FWD_MODE_CLASSIC) {
    } else {
        if (receive_promiscuous) {
            CreateMacVlanFilter(0, 0);
        } else {
            DeleteMacVlanFilter(0, 0);
        }
    }

    // Update Lif to Hal
    lif_->TriggerHalUpdate();
end:
    return;
}

void
EthLif::UpdateReceiveBroadcast(bool receive_broadcast)
{
    api_trace("Broadcast change");
    if (receive_broadcast == info_.receive_broadcast) {
        HAL_TRACE_WARN("Prom flag: {}. No change in broadcast flag. Nop",
                       receive_broadcast);
        goto end;
    }

    HAL_TRACE_DEBUG("Lif: {}. Prom. flag change {} -> {}",
                    lif_->GetId(), info_.receive_broadcast,
                    receive_broadcast);

    info_.receive_broadcast = receive_broadcast;

    // Update Lif to Hal
    lif_->TriggerHalUpdate();
end:
    return;
}

void
EthLif::UpdateReceiveAllMulticast(bool receive_all_multicast)
{
    api_trace("AllMulticast change");
    if (receive_all_multicast == info_.receive_all_multicast) {
        HAL_TRACE_WARN("Prom flag: {}. No change in all_multicast flag. Nop",
                       receive_all_multicast);
        goto end;
    }

    HAL_TRACE_DEBUG("Lif: {}. Prom. flag change {} -> {}",
                    lif_->GetId(), info_.receive_all_multicast,
                    receive_all_multicast);

    info_.receive_all_multicast = receive_all_multicast;

    // Update Lif to Hal
    lif_->TriggerHalUpdate();
end:
    return;
}

void
EthLif::UpdateVlanStripEn(bool vlan_strip_en)
{
    api_trace("Vlan Strip change");
    if (vlan_strip_en == info_.vlan_strip_en) {
        HAL_TRACE_WARN("Prom flag: {}. No change in broadcast flag. Nop",
                       vlan_strip_en);
        goto end;
    }

    HAL_TRACE_DEBUG("Lif: {}. Prom. flag change {} -> {}",
                    lif_->GetId(), info_.vlan_strip_en,
                    vlan_strip_en);

    info_.vlan_strip_en = vlan_strip_en;

    // Update Lif to Hal
    lif_->TriggerHalUpdate();
end:
    return;
}

void
EthLif::UpdateVlanInsertEn(bool vlan_insert_en)
{
    api_trace("Vlan Strip change");
    if (vlan_insert_en == info_.vlan_insert_en) {
        HAL_TRACE_WARN("Prom flag: {}. No change in broadcast flag. Nop",
                       vlan_insert_en);
        goto end;
    }

    HAL_TRACE_DEBUG("Lif: {}. Prom. flag change {} -> {}",
                    lif_->GetId(), info_.vlan_insert_en,
                    vlan_insert_en);

    info_.vlan_insert_en = vlan_insert_en;

    // Update Lif to Hal
    lif_->TriggerHalUpdate();
end:
    return;
}

void
EthLif::CreateMacVlanFilter(mac_t mac, vlan_t vlan)
{
    mac_vlan_filter_t key;
    filter_type_t type;

    if (!mac && !vlan) {
        type = kh::FILTER_LIF;
    } else {
        type = kh::FILTER_LIF_MAC_VLAN;
    }

    key = make_tuple(type, mac, vlan);
    mac_vlan_filter_table[key] = MacVlanFilter::Factory(this, mac, vlan, type);
}

void
EthLif::DeleteMacVlanFilter(mac_t mac, vlan_t vlan)
{
    std::map<mac_vlan_filter_t, MacVlanFilter*>::iterator it;
    mac_vlan_filter_t key;
    filter_type_t type;

    if (!mac && !vlan) {
        type = kh::FILTER_LIF;
    } else {
        type = kh::FILTER_LIF_MAC_VLAN;
    }

    key = make_tuple(type, mac, vlan);
    it = mac_vlan_filter_table.find(key);
    MacVlanFilter *filter = it->second;

    mac_vlan_filter_table.erase(it);
    MacVlanFilter::Destroy(filter);
}

// Should be called only in case of smart.
void
EthLif::CreateMacFilter(mac_t mac)
{
    mac_vlan_filter_t key(kh::FILTER_LIF_MAC, mac, 0);

    mac_vlan_filter_table[key] = MacVlanFilter::Factory(this, mac, 0,
                                                        kh::FILTER_LIF_MAC);
}

void
EthLif::DeleteMacFilter(mac_t mac)
{
    std::map<mac_vlan_filter_t, MacVlanFilter*>::iterator it;
    mac_vlan_filter_t key(kh::FILTER_LIF_MAC, mac, 0);

    it = mac_vlan_filter_table.find(key);
    MacVlanFilter *filter = it->second;

    mac_vlan_filter_table.erase(it);
    MacVlanFilter::Destroy(filter);
}

// Should be called only in case of smart.
void
EthLif::CreateVlanFilter(vlan_t vlan)
{
    mac_vlan_filter_t key(kh::FILTER_LIF_VLAN, 0, vlan);

    mac_vlan_filter_table[key] = MacVlanFilter::Factory(this, 0, vlan,
                                                        kh::FILTER_LIF_VLAN);
}

void
EthLif::DeleteVlanFilter(vlan_t vlan)
{
    std::map<mac_vlan_filter_t, MacVlanFilter*>::iterator it;
    mac_vlan_filter_t key(kh::FILTER_LIF_VLAN, 0, vlan);

    it = mac_vlan_filter_table.find(key);
    MacVlanFilter *filter = it->second;

    mac_vlan_filter_table.erase(it);
    MacVlanFilter::Destroy(filter);
}


Lif *
EthLif::GetLif()
{
    return lif_;
}

Uplink *
EthLif::GetUplink()
{
    return info_.pinned_uplink;;
}

Enic *
EthLif::GetEnic()
{
    return enic_;
}

void
EthLif::SetEnic(Enic *enic)
{
    enic_ = enic;
}

uint32_t
EthLif::GetHwLifId()
{
    return info_.hw_lif_id;
}

bool
EthLif::GetIsPromiscuous()
{
    return info_.receive_promiscuous;
}

lif_info_t *
EthLif::GetLifInfo()
{
    return &info_;
}

