
#ifndef __ENIC_HPP__
#define __ENIC_HPP__

#include <vector>

#include "nic/sdk/include/sdk/indexer.hpp"

#include "types.hpp"
#include "hal.hpp"
#include "vrf.hpp"
#include "l2seg.hpp"


// lif
typedef uint64_t enic_classic_key_t;

typedef struct l2seg_info_s {
    L2Segment *l2seg;
    uint32_t filter_ref_cnt;
} l2seg_info_t;


class Enic : public HalObject
{
public:
    static Enic *Factory(EthLif *ethlif);
    static void Destroy(Enic *enic);

    // Classic ENIC APIs only
    void AddVlan(vlan_t vlan);
    void DelVlan(vlan_t vlan);

    ~Enic();

    uint64_t GetId();
    uint64_t GetHandle();
    L2Segment *GetL2seg(vlan_t vlan);

private:
    Enic(EthLif *ethlif);

    uint32_t id;
    uint64_t handle;

    mac_t _mac;
    vlan_t _vlan;

    intf::InterfaceSpec spec;

    EthLif *ethlif;
    std::map<vlan_t, l2seg_info_t *> l2seg_refs;

    static sdk::lib::indexer *allocator;
    static constexpr uint32_t max_enics = 4096;

    void TriggerHalUpdate();
};

#endif /* __ENIC_HPP__ */
