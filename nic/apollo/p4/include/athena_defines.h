#ifndef _ATHENA_DEFINES_H_
#define _ATHENA_DEFINES_H_

#include "nic/p4/common/defines.h"

/*****************************************************************************/
/* Key types                                                                 */
/*****************************************************************************/
#define KEY_TYPE_NONE                   0
#define KEY_TYPE_IPV4                   1
#define KEY_TYPE_IPV6                   2
#define KEY_TYPE_MAC                    3

/*****************************************************************************/
/* Packet direction                                                          */
/*****************************************************************************/
#define TX_FROM_HOST                    0
#define RX_FROM_SWITCH                  1

/*****************************************************************************/
/* User L3 rewrite types                                                     */
/*****************************************************************************/
#define L3REWRITE_NONE                          0
#define L3REWRITE_SNAT                          1
#define L3REWRITE_DNAT                          2


/*****************************************************************************/
/* Encap types                                                               */
/*****************************************************************************/
#define REWRITE_ENCAP_NONE                      0
#define REWRITE_ENCAP_L2                        1
#define REWRITE_ENCAP_MPLSOUDP                  2
#define REWRITE_ENCAP_MPLSOGRE                  3


/*****************************************************************************/
/* P4E packet types                                                          */
/*****************************************************************************/
#define P4E_PACKET_NORMAL                       0
#define P4E_PACKET_OVERLAY_IPV4                 1
#define P4E_PACKET_OVERLAY_IPV6                 2

/*****************************************************************************/
/* NACL redirect types                                                       */
/*****************************************************************************/
#define NACL_REDIR_RXDMA                        0
#define NACL_REDIR_UPLINK                       1

/*****************************************************************************/
/* drop reasons - these are bit positions to be used in ASM                  */
/*****************************************************************************/
#define P4I_DROP_SRC_MAC_ZERO           0
#define P4I_DROP_SRC_MAC_MISMATCH       1
#define P4I_DROP_VNIC_INFO_TX_MISS      2
#define P4I_DROP_VNIC_INFO_RX_MISS      3
#define P4I_DROP_SRC_DST_CHECK_FAIL     4
#define P4I_DROP_FLOW_HIT               5
#define P4I_DROP_TEP_RX_DST_IP_MISMATCH 6
#define P4I_DROP_RVPATH_SRC_IP_MISMATCH 7
#define P4I_DROP_RVPATH_VPC_MISMATCH    8
#define P4I_DROP_NACL                   9
#define P4I_DROP_REASON_MIN             P4I_DROP_SRC_MAC_ZERO
#define P4I_DROP_REASON_MAX             P4I_DROP_NACL

#define P4E_DROP_INVALID_NEXTHOP        0
#define P4E_DROP_REASON_MIN             P4E_DROP_INVALID_NEXTHOP
#define P4E_DROP_REASON_MAX             P4E_DROP_INVALID_NEXTHOP

#endif /* _ATHENA_DEFINES_H_ */
