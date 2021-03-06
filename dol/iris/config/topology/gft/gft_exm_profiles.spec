# GFT Exact Match Profiles Specification
# Profile Defaults:
#   enable  : True
#   rdma    : False
meta:
    id: GFT_EXM_PROFILES

profiles:
    - profile:
        name    : GFT_EXMP_ETH_QTAG
        enable  : False
        table   : EXACT_MATCH_INGRESS
        groups  :
            - ref://store/objects/id=GFT_HGEXM_ETH_QTAG

    - profile:
        name    : GFT_EXMP_IPV4_QTAG
        enable  : False
        table   : EXACT_MATCH_INGRESS
        groups  :
            - ref://store/objects/id=GFT_HGEXM_IPV4_QTAG

    - profile:
        name    : GFT_EXMP_IPV4_UDP_QTAG
        table   : EXACT_MATCH_INGRESS
        groups  :
            - ref://store/objects/id=GFT_HGEXM_IPV4_UDP_QTAG

    - profile:
        name    : GFT_EXMP_IPV4_UDP_NATIVE
        table   : EXACT_MATCH_INGRESS
        groups  :
            - ref://store/objects/id=GFT_HGEXM_IPV4_UDP_NATIVE

    - profile:
        name    : GFT_EXMP_IPV4_TCP_QTAG
        table   : EXACT_MATCH_INGRESS
        groups  :
            - ref://store/objects/id=GFT_HGEXM_IPV4_TCP_QTAG

    - profile:
        name    : GFT_EXMP_IPV4_TCP_NATIVE
        table   : EXACT_MATCH_INGRESS
        groups  :
            - ref://store/objects/id=GFT_HGEXM_IPV4_TCP_NATIVE

    - profile:
        name    : GFT_EXMP_IPV4_ICMP_QTAG
        table   : EXACT_MATCH_INGRESS
        groups  :
            - ref://store/objects/id=GFT_HGEXM_IPV4_ICMP_QTAG

    - profile:
        name    : GFT_EXMP_IPV4_ICMP_NATIVE
        table   : EXACT_MATCH_INGRESS
        groups  :
            - ref://store/objects/id=GFT_HGEXM_IPV4_ICMP_NATIVE

    - profile:
        name    : GFT_EXMP_IPV4_UDP_QTAG_EGRESS
        table   : EXACT_MATCH_EGRESS
        groups  :
            - ref://store/objects/id=GFT_HGEXM_IPV4_UDP_QTAG

    - profile:
        name    : GFT_EXMP_IPV4_TCP_QTAG_EGRESS
        table   : EXACT_MATCH_EGRESS
        groups  :
            - ref://store/objects/id=GFT_HGEXM_IPV4_TCP_QTAG

    - profile:
        name    : GFT_EXMP_IPV4_ICMP_QTAG_EGRESS
        table   : EXACT_MATCH_EGRESS
        groups  :
            - ref://store/objects/id=GFT_HGEXM_IPV4_ICMP_QTAG
