#include "ingress.h"
#include "INGRESS_p.h"
#include "../../p4/nw/include/defines.h"
#include "../../include/capri_common.h"
#include "nw.h"

struct drop_stats_k k;
struct drop_stats_d d;
struct phv_         p;

%%

drop_stats:
  K_DBG_WR(0xa0)
  DBG_WR(0xa8, k.control_metadata_drop_reason)
  seq         c1, d.drop_stats_d.mirror_en, 1
  phvwr.c1    p.capri_intrinsic_tm_span_session, d.drop_stats_d.mirror_session_id
  nop.e
  nop
