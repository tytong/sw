/*
 * Copyright (c) 2017, Pensando Systems Inc.
 */

#ifndef __PCIEHDEV_IMPL_H__
#define __PCIEHDEV_IMPL_H__

struct pciehcfg_s;
typedef struct pciehcfg_s pciehcfg_t;
struct pciehbars_s;
typedef struct pciehbars_s pciehbars_t;

typedef struct pciehdev_s {
    char name[32];
    u_int8_t port;
    u_int16_t bdf;
    u_int32_t pf:1;                     /* sriov pf */
    u_int32_t vf:1;                     /* sriov vf */
    u_int32_t fn0:1;                    /* multifunction dev, function 0 */
    u_int32_t fnn;                      /* multifunction dev, function N */
    u_int16_t totalvfs;
    u_int16_t vfidx;                    /* vf position */
    u_int32_t lifb;
    u_int32_t lifc;
    u_int32_t intrb;
    u_int32_t intrc;
    u_int32_t intrdmask:1;
    pciehcfg_t *pcfg;
    pciehbars_t *pbars;
    void *phwdev;
    /* tree links */
    pciehdev_t *parent;                 /* parent bridge/vf's pf */
    pciehdev_t *child;                  /* child bridge/dev/vf */
    pciehdev_t *peer;                   /* peer dev/fn/vf */
} pciehdev_t;

#endif /* __PCIEHDEV_IMPL_H__ */
