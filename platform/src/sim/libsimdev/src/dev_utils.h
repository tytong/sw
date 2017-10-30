/*
 * Copyright (c) 2017, Pensando Systems Inc.
 */

#ifndef __DEV_UTILS_H__
#define __DEV_UTILS_H__

typedef struct qstate {     // 64 B
    uint8_t     pc_offset;
    uint8_t     rsvd0;
    uint8_t     cosA : 4;
    uint8_t     cosB : 4;
    uint8_t     cos_sel;
    uint8_t     eval_last;
    uint8_t     host : 4;
    uint8_t     total : 4;
    uint16_t    pid;

    uint16_t    p_index0;
    uint16_t    c_index0;
    uint16_t    p_index1;
    uint16_t    c_index1;
    uint16_t    p_index2;
    uint16_t    c_index2;
    uint16_t    p_index3;
    uint16_t    c_index3;
    uint16_t    p_index4;
    uint16_t    c_index4;
    uint16_t    p_index5;
    uint16_t    c_index5;
    uint16_t    p_index6;
    uint16_t    c_index6;
    uint16_t    p_index7;
    uint16_t    c_index7;

    uint8_t     app_data[24];   /* application specific area */
} __attribute__((packed)) qstate_t;

u_int64_t lif_qstate_map_addr(const int lif);
u_int64_t intr_msixcfg_addr(const int intr);
u_int64_t intr_fwcfg_addr(const int intr);
u_int64_t intr_drvcfg_addr(const int intr);
u_int64_t intr_pba_addr(const int lif);
u_int64_t intr_pba_cfg_addr(const int lif);
u_int64_t db_host_addr(const int lif);
void intr_pba_cfg(const int lif,
                  const int intr_start, const size_t intr_count);
void intr_drvcfg(const int intr);
void intr_pba_clear(const int intr);
void intr_msixcfg(const int intr,
                  const u_int64_t msgaddr,
                  const u_int32_t msgdata,
                  const int vctrl);
void intr_fwcfg_msi(const int intr, const int lif, const int port_id);

#endif /* __DEV_UTILS_H__ */
