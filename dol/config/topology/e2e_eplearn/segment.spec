# Segment Configuration Spec
meta:
    id: SEGMENT_EPLEARN

type        : tenant
native      : False
broadcast   : flood
multicast   : flood
l4lb        : False
eplearn :
    arp_entry_timeout : 20
    dhcp              : True
endpoints   :
    sgenable: True
    useg    : 0
    pvlan   : 4
    direct  : 0
    remote  : 2
