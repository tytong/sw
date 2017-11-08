#include "egress.h"
#include "EGRESS_p.h"
#include "../../p4/nw/include/defines.h"

struct egress_policer_action_k k;
struct egress_policer_action_d d;
struct phv_                    p;

k = {
  policer_metadata_egress_policer_color = 1;
  capri_p4_intrinsic_packet_len_sbit0_ebit5 = 0;
  capri_p4_intrinsic_packet_len_sbit6_ebit13 = 0x80;
};

d = {
  egress_policer_action_d.permitted_packets = 0xf;
  egress_policer_action_d.permitted_bytes = 0x2345;
  egress_policer_action_d.denied_packets = 0xf;
  egress_policer_action_d.denied_bytes = 0x2345;
};

r5 = 0x12345600;
