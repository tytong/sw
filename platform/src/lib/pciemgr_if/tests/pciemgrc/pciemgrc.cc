/*
 * Copyright (c) 2018-2019, Pensando Systems Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <cinttypes>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/time.h>

#include "nic/sdk/platform/misc/include/misc.h"
#include "nic/sdk/platform/evutils/include/evutils.h"
#include "nic/sdk/platform/pciehdevices/include/pciehdevices.h"
#include "nic/sdk/platform/pciemgrutils/include/pciemgrutils.h"
#include "nic/sdk/platform/pciemgr/include/pciemgr.h"
#include "platform/src/lib/pciemgr_if/include/pciemgr_if.hpp"

static int verbose_flag;
static pciemgr *pciemgr;

static void
usage(void)
{
    fprintf(stderr,
"Usage: pciemgr [-Fnv][-e <enabled_ports>[-b <first_bus_num>][-P gen<G>x<W>][-s subdeviceid]\n"
"    -b <first_bus_num> set first bus used to <first_bus_num>\n"
"    -e <enabled_ports> max of enabled pcie ports\n"
"    -F                 no fake bios scan\n"
"    -h                 initializing hw\n"
"    -H                 no initializing hw\n"
"    -P gen<G>x<W>      spec devices as pcie gen <G>, lane width <W>\n"
"    -s subdeviceid     default subsystem device id\n"
"    -v                 verbose\n");
}

static void verbose(const char *fmt, ...)
    __attribute__((format (printf, 1, 2)));
static void
verbose(const char *fmt, ...)
{
    va_list arg;

    if (verbose_flag) {
        va_start(arg, fmt);
        vprintf(fmt, arg);
        va_end(arg);
    }
}

class myevhandler : public pciemgr::evhandler {
    virtual void memrd(const int port,
                       const uint32_t lif,
                       const pciehdev_memrw_notify_t *n) {
        printf("memrd: port %d lif %u\n"
               "    bar %d baraddr 0x%" PRIx64 " baroffset 0x%" PRIx64 " "
               "size %u localpa 0x%" PRIx64 " data 0x%" PRIx64 "\n",
               port, lif,
               n->cfgidx, n->baraddr, n->baroffset,
               n->size, n->localpa, n->data);
    }
    virtual void memwr(const int port,
                       const uint32_t lif,
                       const pciehdev_memrw_notify_t *n) {
        printf("memwr: port %d lif %u\n"
               "    bar %d baraddr 0x%" PRIx64 " baroffset 0x%" PRIx64 " "
               "size %u localpa 0x%" PRIx64 " data 0x%" PRIx64 "\n",
               port, lif,
               n->cfgidx, n->baraddr, n->baroffset,
               n->size, n->localpa, n->data);
    }
};

int
main(int argc, char *argv[])
{
    const char *myname = "pciemgr";
    myevhandler myevh;
    pciemgr = new class pciemgr(myname, myevh, EV_DEFAULT);
    int opt, totalvfs = 0;

    while ((opt = getopt(argc, argv, "v:")) != -1) {
        switch (opt) {
        case 'v':
            totalvfs = strtoul(optarg, NULL, 0);
            break;
        default:
            exit(1);
        }
    }

    pciemgr->initialize();

    pciehdevice_resources_t pres;
    pciehdev_res_t *pfres = &pres.pfres;
    pciehdev_res_t *vfres = &pres.vfres;
    memset(&pres, 0, sizeof(pres));
    pres.type = PCIEHDEVICE_ETH;
    strncpy0(pres.pfres.name, "eth", sizeof(pres.pfres.name));
    pfres->lifb = 5;
    pfres->lifc = 1;
    pfres->intrb = 0;
    pfres->intrc = 4;
    pfres->intrdmask = 1;
    pfres->romsz = 4096;
    pfres->rompa = 0x13f000000;
    pfres->eth.devregspa = 0x13f000000;
    pfres->eth.devregssz = 0x1000;

    /* sriov pf with totalvfs */
    if (totalvfs) {
        pfres->totalvfs = totalvfs;
        vfres->is_vf = 1;
        if (pfres->lifc) {
            vfres->lifb = pfres->lifb + pfres->lifc;
            vfres->lifc = pfres->lifc;
        }
        if (pfres->intrc) {
            vfres->intrb = pfres->intrb + pfres->intrc;
            vfres->intrc = pfres->intrc;
            vfres->intrdmask = pfres->intrdmask;
        }
        vfres->eth.devregspa = pfres->eth.devregspa + 0x1000;
        vfres->eth.devregssz = pfres->eth.devregssz;
        vfres->eth.devregs_stride = vfres->eth.devregssz;
    }

    if (pciemgr->add_devres(&pres) < 0) {
        fprintf(stderr, "add_devres failed\n");
        exit(1);
    }
    pciemgr->finalize();

    printf("evutil_run()\n");
    evutil_run(EV_DEFAULT);

    delete pciemgr;
    exit(0);

    if (0) verbose("reference");
    if (0) usage();
}
