/*
 * Copyright (c) 2017, Pensando Systems Inc.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>

#include "pci_ids.h"
#include "pciehost.h"
#include "pciehdevices.h"

static void
initialize_bars(pciehbars_t *pbars, const pciehdevice_resources_t *pres)
{
    pciehbarreg_t preg;
    pciehbar_t pbar;

    /* bar mem */
    memset(&pbar, 0, sizeof(pbar));
    memset(&preg, 0, sizeof(preg));
    preg.flags = PCIEHBARREGF_RW;
    preg.paddr = 0;
    preg.size = 0x1000;
    preg.align = 0;
    pciehbar_add_reg(&pbar, &preg);

    pbar.type = PCIEHBARTYPE_MEM;
    pciehbars_add_bar(pbars, &pbar);

    /* bar mem64 */
    memset(&pbar, 0, sizeof(pbar));
    memset(&preg, 0, sizeof(preg));
    preg.flags = (PCIEHBARREGF_RW | PCIEHBARREGF_MSIX_TBL);
    preg.paddr = 0;
    preg.size = 0x1000;
    preg.align = 0;
    pciehbar_add_reg(&pbar, &preg);

    memset(&preg, 0, sizeof(preg));
    preg.flags = (PCIEHBARREGF_RW | PCIEHBARREGF_MSIX_PBA);
    preg.paddr = 0;
    preg.size = 0x1000;
    preg.align = 0;
    pciehbar_add_reg(&pbar, &preg);

    pbar.type = PCIEHBARTYPE_MEM64;
    pciehbars_add_bar(pbars, &pbar);

    /* bar io */
    memset(&pbar, 0, sizeof(pbar));
    memset(&preg, 0, sizeof(preg));
    preg.flags = PCIEHBARREGF_RW;
    preg.paddr = 0;
    preg.size = 0x10;
    preg.align = 0;
    pciehbar_add_reg(&pbar, &preg);

    pbar.type = PCIEHBARTYPE_IO;
    pciehbars_add_bar(pbars, &pbar);
}

static void
initialize_cfg(pciehcfg_t *pcfg, pciehbars_t *pbars,
               const pciehdevice_resources_t *pres)
{
    pciehcfg_setconf_deviceid(pcfg, PCI_DEVICE_ID_PENSANDO_NVME);
    pciehcfg_setconf_classcode(pcfg, 0x010802);
    pciehcfg_setconf_classcode(pcfg, 0x088000); // XXX
    pciehcfg_setconf_nintrs(pcfg, pres->intrc);
    pciehcfg_setconf_msix_tblbir(pcfg, pciehbars_get_msix_tblbir(pbars));
    pciehcfg_setconf_msix_tbloff(pcfg, pciehbars_get_msix_tbloff(pbars));
    pciehcfg_setconf_msix_pbabir(pcfg, pciehbars_get_msix_pbabir(pbars));
    pciehcfg_setconf_msix_pbaoff(pcfg, pciehbars_get_msix_pbaoff(pbars));
    pciehcfg_setconf_fnn(pcfg, pres->fnn);

    pciehcfg_sethdr_type0(pcfg, pbars);
    pciehcfg_add_standard_caps(pcfg);
}

static void
nvme_initialize_bars(pciehdev_t *pdev, const pciehdevice_resources_t *pres)
{
    pciehbars_t *pbars;

    pbars = pciehbars_new();
    initialize_bars(pbars, pres);
    pciehdev_set_bars(pdev, pbars);
}

static void
nvme_initialize_cfg(pciehdev_t *pdev, const pciehdevice_resources_t *pres)
{
    pciehcfg_t *pcfg = pciehcfg_new();
    pciehbars_t *pbars = pciehdev_get_bars(pdev);

    initialize_cfg(pcfg, pbars, pres);
    pciehdev_set_cfg(pdev, pcfg);
}

pciehdev_t *
pciehdev_nvme_new(const char *name, const pciehdevice_resources_t *pres)
{
    pciehdev_t *pdev = pciehdev_new(name, pres);
    nvme_initialize_bars(pdev, pres);
    nvme_initialize_cfg(pdev, pres);
    return pdev;
}
