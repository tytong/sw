/*
 * Copyright 2017-2018 Pensando Systems, Inc.  All rights reserved.
 *
 * This program is free software; you may redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */


#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/interrupt.h>
#include <linux/if_ether.h>

#include "ionic.h"
#include "ionic_bus.h"
#include "ionic_api.h"
#include "ionic_lif.h"
#include "ionic_txrx.h"

#include "opt_rss.h"

#ifdef	RSS
#include <net/rss_config.h>
#include <netinet/in_rss.h>
#endif

static int ionic_q_enable_disable(struct lif* lif, struct intr *intr, unsigned int qid, unsigned int qtype, bool enable)
{
	struct ionic_admin_ctx ctx = {
		.work = COMPLETION_INITIALIZER_ONSTACK(ctx.work),
		.cmd.q_enable = {
			.opcode = enable ? CMD_OPCODE_Q_ENABLE : CMD_OPCODE_Q_DISABLE,
			.qid = qid,
			.qtype = qtype,
		},
	};
	int err;

	IONIC_NETDEV_INFO(lif->netdev, "%s qid %d qtype:%d\n", enable ? "Enable" : "Disable",
		ctx.cmd.q_enable.qid, ctx.cmd.q_enable.qtype);

	if (enable) {
		err = ionic_adminq_post_wait(lif, &ctx);
		if (err) {
			IONIC_NETDEV_ERROR(lif->netdev, "AdminQ enbale failed for qid %d qtype:%d\n",
				ctx.cmd.q_enable.qid, ctx.cmd.q_enable.qtype);
			return err;
		}
		ionic_intr_mask(intr, false);

		return (0);
	}

	ionic_intr_mask(intr, true);
#ifndef FREEBSD
	synchronize_irq(qcq->intr.vector);
#endif
	return ionic_adminq_post_wait(lif, &ctx);
}


void ionic_open(void *arg)
{
	struct lif *lif = arg;
	struct rxque *rxq;
	struct txque *txq;
	unsigned int i;
	int err;

	for (i = 0; i < lif->ntxqs; i++) {
		txq = lif->txqs[i];
		mtx_lock(&txq->mtx);
		err = ionic_q_enable_disable(lif, &txq->intr, txq->qid, txq->qtype, true /* enable */);
		WARN_ON(err);
		mtx_unlock(&txq->mtx);
		//napi_enable(&lif->txqs[i]->napi);
	}

	for (i = 0; i < lif->nrxqs; i++) {
		rxq = lif->rxqs[i];
		mtx_lock(&rxq->mtx);
		ionic_rx_fill(rxq);
		err = ionic_q_enable_disable(lif, &rxq->intr, rxq->qid, rxq->qtype, true /* enable */);
		WARN_ON(err);
		mtx_unlock(&rxq->mtx);
	}

	ionic_up_link(lif->netdev);
}


static int ionic_stop(struct net_device *netdev)
{
	struct lif *lif = netdev_priv(netdev);
	struct rxque* rxq;
	struct txque* txq;
	unsigned int i;
	int err;


	for (i = 0; i < lif->ntxqs; i++) {
		// TODO post NOP Tx desc and wait for its completion
		// TODO before disabling Tx queue
		txq = lif->txqs[i];
		mtx_lock(&txq->mtx);
		err = ionic_q_enable_disable(lif, &txq->intr, txq->qid, txq->qtype, false /* disable */);
		if (err)
			return err;
 
		mtx_unlock(&txq->mtx);
	}

	for (i = 0; i < lif->nrxqs; i++) {
		rxq = lif->rxqs[i];
		mtx_lock(&rxq->mtx);
		err = ionic_q_enable_disable(lif, &rxq->intr, rxq->qid, rxq->qtype, false /* disable */);
		if (err) {
			/* XXX: should we continue? */
		}
		ionic_rx_flush(rxq);
		mtx_unlock(&rxq->mtx);
	}

	return 0;
}

static irqreturn_t ionic_adminq_isr(int irq, void *data)
{
	struct adminq* adminq = data;

	ionic_intr_mask(&adminq->intr, true);

	napi_schedule_irqoff(&adminq->napi);

	return IRQ_HANDLED;
}

static void ionic_adminq_napi(struct napi_struct *napi)
{
	struct admin_comp *comp;
	struct admin_cmd *cmd;
	int comp_index, cmd_index, processed, cmd_stop_index;
	int budget = NAPI_POLL_WEIGHT;
	struct adminq* adminq = container_of(napi, struct adminq, napi);

	mtx_lock(&adminq->mtx);

	for ( processed = 0 ; processed < budget ; ) {
		comp_index = adminq->comp_index;
		comp = &adminq->comp_ring[comp_index];
		/* Sync every time descriptors. */
		bus_dmamap_sync(adminq->cmd_dma.dma_tag, adminq->cmd_dma.dma_map,
			BUS_DMASYNC_POSTREAD | BUS_DMASYNC_POSTWRITE);

		cmd_stop_index = comp->comp_index;
		cmd_index = adminq->cmd_tail_index;
		cmd = &adminq->cmd_ring[cmd_index];

		if (comp->color != adminq->done_color)
			break;

		IONIC_NETDEV_QINFO(adminq, "comp :%d cmd start: %d cmd stop: %d comp->color %d done_color %d\n",
			comp_index, cmd_index, cmd_stop_index, comp->color, adminq->done_color);
		IONIC_NETDEV_QINFO(adminq, "buf[%d] opcode:%d\n", cmd_index, cmd->opcode);

		for ( ; cmd_index == cmd_stop_index; cmd_index++, processed++ ) {
			/* XXX: loop to do???? */
			cmd = &adminq->cmd_ring[cmd_index];
		}

		adminq->comp_index = (adminq->comp_index + 1) % adminq->num_descs;
		adminq->cmd_tail_index = (adminq->cmd_tail_index + 1) % adminq->num_descs;
		/* Roll over condition, flip color. */
		if (adminq->comp_index == 0) {
			adminq->done_color = !adminq->done_color;
		}
	}

	IONIC_NETDEV_QINFO(adminq, "ionic_adminq_napi processed %d\n", processed);

	if (processed == budget)
		napi_schedule(&adminq->napi);

	ionic_intr_return_credits(&adminq->intr, processed, 0, true);

	// Enable interrupt.
	ionic_intr_mask(&adminq->intr, false);
	mtx_unlock(&adminq->mtx);
}

static int _ionic_lif_addr(struct lif *lif, const u8 *addr, bool add)
{
	struct ionic_admin_ctx ctx = {
		.work = COMPLETION_INITIALIZER_ONSTACK(ctx.work),
		.cmd.rx_filter = {
			.opcode = add ? CMD_OPCODE_RX_FILTER_ADD :
					CMD_OPCODE_RX_FILTER_DEL,
			.match = RX_FILTER_MATCH_MAC,
		},
	};

	memcpy(ctx.cmd.rx_filter.addr, addr, ETH_ALEN);

	IONIC_NETDEV_INFO(lif->netdev, "rx_filter %s %pM\n",
		   add ? "add" : "del", addr);

	return ionic_adminq_post_wait(lif, &ctx);
}

struct lif_addr_work {
	struct work_struct work;
	struct lif *lif;
	u8 addr[ETH_ALEN];
	bool add;
};

static void ionic_lif_addr_work(struct work_struct *work)
{
	struct lif_addr_work *w  = container_of(work, struct lif_addr_work,
						work);

	_ionic_lif_addr(w->lif, w->addr, w->add);
	kfree(w);
}

static int ionic_lif_addr(struct lif *lif, const u8 *addr, bool add)
{
	struct lif_addr_work *work;

	IONIC_DEV_TRACE(lif->ionic->dev, "%02x:%02x:%02x:%02x:%02x:%02x %s\n",
		  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5],
		  add ? "add" : "del");

	if (true) {
		work = kmalloc(sizeof(*work), GFP_ATOMIC);
		if (!work) {
			IONIC_NETDEV_ERROR(lif->netdev, "failed to allocate memory for address.\n");
			return -ENOMEM;
		}
		INIT_WORK(&work->work, ionic_lif_addr_work);
		work->lif = lif;
		memcpy(work->addr, addr, ETH_ALEN);
		work->add = add;
		IONIC_NETDEV_INFO(lif->netdev, "deferred: rx_filter %s %pM\n",
			   add ? "add" : "del", addr);
		queue_work(lif->adminq_wq, &work->work);
	} else {
		return _ionic_lif_addr(lif, addr, add);
	}

	return 0;
}

static int ionic_addr_add(struct net_device *netdev, const u8 *addr)
{
	struct lif *lif = netdev_priv(netdev);
	IONIC_DEV_TRACE(lif->ionic->dev, "%02x:%02x:%02x:%02x:%02x:%02x\n",
		  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

	return ionic_lif_addr(netdev_priv(netdev), addr, true);
}

static int ionic_addr_del(struct net_device *netdev, const u8 *addr)
{
	struct lif *lif = netdev_priv(netdev);
	IONIC_DEV_TRACE(lif->ionic->dev, "%02x:%02x:%02x:%02x:%02x:%02x\n",
		  addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

	return ionic_lif_addr(netdev_priv(netdev), addr, false);
}

static void _ionic_lif_rx_mode(struct lif *lif, unsigned int rx_mode)
{
	struct ionic_admin_ctx ctx = {
		.work = COMPLETION_INITIALIZER_ONSTACK(ctx.work),
		.cmd.rx_mode_set = {
			.opcode = CMD_OPCODE_RX_MODE_SET,
			.rx_mode = rx_mode,
		},
	};
	int err;

	IONIC_DEV_TRACE(lif->ionic->dev, "%#x\n", rx_mode);

	if (rx_mode & RX_MODE_F_UNICAST)
		IONIC_NETDEV_INFO(lif->netdev, "rx_mode RX_MODE_F_UNICAST\n");
	if (rx_mode & RX_MODE_F_MULTICAST)
		IONIC_NETDEV_INFO(lif->netdev, "rx_mode RX_MODE_F_MULTICAST\n");
	if (rx_mode & RX_MODE_F_BROADCAST)
		IONIC_NETDEV_INFO(lif->netdev, "rx_mode RX_MODE_F_BROADCAST\n");
	if (rx_mode & RX_MODE_F_PROMISC)
		IONIC_NETDEV_INFO(lif->netdev, "rx_mode RX_MODE_F_PROMISC\n");
	if (rx_mode & RX_MODE_F_ALLMULTI)
		IONIC_NETDEV_INFO(lif->netdev, "rx_mode RX_MODE_F_ALLMULTI\n");

	err = ionic_adminq_post_wait(lif, &ctx);
	if (err) {
		// XXX handle err
	}
}

struct rx_mode_work {
	struct work_struct work;
	struct lif *lif;
	unsigned int rx_mode;
};

static void ionic_lif_rx_mode_work(struct work_struct *work)
{
	struct rx_mode_work *w  = container_of(work, struct rx_mode_work, work);
	
	_ionic_lif_rx_mode(w->lif, w->rx_mode);
	kfree(w);
}

static void ionic_lif_rx_mode(struct lif *lif, unsigned int rx_mode)
{
	struct rx_mode_work *work;

	IONIC_DEV_TRACE(lif->ionic->dev, "%#x\n", rx_mode);

	if (true) {
		work = kmalloc(sizeof(*work), GFP_ATOMIC);
		if (!work) {
			IONIC_NETDEV_ERROR(lif->netdev, "%s OOM\n", __func__);
			return;
		}
		INIT_WORK(&work->work, ionic_lif_rx_mode_work);
		work->lif = lif;
		work->rx_mode = rx_mode;
		IONIC_NETDEV_INFO(lif->netdev, "deferred: rx_mode\n");
		queue_work(lif->adminq_wq, &work->work);
	} else {
		_ionic_lif_rx_mode(lif, rx_mode);
	}
}

static void ionic_set_rx_mode(struct net_device *netdev)
{
	struct lif *lif = netdev_priv(netdev);
	unsigned int rx_mode;

	IONIC_DEV_TRACE(lif->ionic->dev, "\n");

	rx_mode = RX_MODE_F_UNICAST;
	rx_mode |= (netdev->if_flags & IFF_MULTICAST) ? RX_MODE_F_MULTICAST : 0;
	rx_mode |= (netdev->if_flags & IFF_BROADCAST) ? RX_MODE_F_BROADCAST : 0;
	rx_mode |= (netdev->if_flags & IFF_PROMISC) ? RX_MODE_F_PROMISC : 0;
	rx_mode |= (netdev->if_flags & IFF_ALLMULTI) ? RX_MODE_F_ALLMULTI : 0;

	if (lif->rx_mode != rx_mode) {
		lif->rx_mode = rx_mode;
		ionic_lif_rx_mode(lif, rx_mode);
	}
}

static int ionic_set_mac_address(struct net_device *netdev, void *addr)
{
	// TODO implement
	IONIC_NETDEV_ERROR(netdev, "SET MAC ADDRESS not implemented\n");
	return 0;
}

int ionic_reinit_unlock(struct net_device *netdev)
{
	struct lif *lif = netdev_priv(netdev);

	IONIC_DEV_TRACE(lif->ionic->dev, "reinit\n");

	if (netif_running(netdev))
		ionic_stop(netdev);


#if 0 // XXX: Why to reallocate mbufs???
	for (i = 0; i < lif->nrxqs; i++)
		ionic_rx_refill(lif->rxqs[i]);
#endif
	ionic_open(lif);

	return (0);
}


static int ionic_change_mtu(struct net_device *netdev, int new_mtu)
{
	struct lif *lif = netdev_priv(netdev);
	struct ionic_admin_ctx ctx = {
		.work = COMPLETION_INITIALIZER_ONSTACK(ctx.work),
		.cmd.mtu_set = {
			.opcode = CMD_OPCODE_MTU_SET,
			.mtu = new_mtu,
		},
	};
	int i, err;

	IONIC_DEV_TRACE(lif->ionic->dev, "\n");

	if (netif_running(netdev))
		ionic_stop(netdev);

	err = ionic_adminq_post_wait(lif, &ctx);
	if (err)
		return err;

	if_setmtu(netdev, new_mtu);


	for (i = 0; i < lif->nrxqs; i++)
		ionic_rx_refill(lif->rxqs[i]);


	if (netif_running(netdev))
		ionic_open(lif);

	return 0;
}

static void ionic_tx_timeout(struct net_device *netdev)
{
	struct lif *lif = netdev_priv(netdev);
	IONIC_DEV_TRACE(lif->ionic->dev, "\n");

	// TODO implement
}

static int ionic_vlan_rx_filter(struct net_device *netdev, bool add,
				__be16 proto, u16 vid)
{
	struct lif *lif = netdev_priv(netdev);
	struct ionic_admin_ctx ctx = {
		.work = COMPLETION_INITIALIZER_ONSTACK(ctx.work),
		.cmd.rx_filter = {
			.opcode = add ? CMD_OPCODE_RX_FILTER_ADD :
					CMD_OPCODE_RX_FILTER_DEL,
			.match = RX_FILTER_MATCH_VLAN,
			.vlan = vid,
		},
	};

	IONIC_DEV_TRACE(lif->ionic->dev, "\n");

	IONIC_NETDEV_INFO(netdev, "rx_filter %s VLAN %d\n", add ? "add" : "del", vid);

	return ionic_adminq_post_wait(lif, &ctx);
}

static int ionic_vlan_rx_add_vid(struct net_device *netdev,
				 __be16 proto, u16 vid)
{
	struct lif *lif = netdev_priv(netdev);
	IONIC_DEV_TRACE(lif->ionic->dev, "\n");

	return ionic_vlan_rx_filter(netdev, true, proto, vid);
}

static int ionic_vlan_rx_kill_vid(struct net_device *netdev,
				  __be16 proto, u16 vid)
{
	struct lif *lif = netdev_priv(netdev);
	IONIC_DEV_TRACE(lif->ionic->dev, "\n");

	return ionic_vlan_rx_filter(netdev, false, proto, vid);
}

int ionic_intr_alloc(struct lif *lif, struct intr *intr)
{
	struct ionic *ionic = lif->ionic;
	struct ionic_dev *idev = &ionic->idev;
	unsigned long index;

	index = find_first_zero_bit(ionic->intrs, ionic->nintrs);
	if (index == ionic->nintrs)
		return -ENOSPC;
	set_bit(index, ionic->intrs);

	return ionic_intr_init(idev, intr, index);
}

void ionic_intr_free(struct lif *lif, struct intr *intr)
{
	if (intr->index != INTR_INDEX_NOT_ASSIGNED)
		clear_bit(intr->index, lif->ionic->intrs);
}

static int ionic_adminq_alloc(struct lif *lif, unsigned int qnum,
			unsigned int num_descs, unsigned int pid,
			struct adminq **padminq)
{
	struct adminq *adminq;
	int irq, error = ENOMEM; 
	uint32_t cmd_ring_size, comp_ring_size, total_size;

	*padminq = NULL;

	adminq = malloc(sizeof(*adminq), M_IONIC, M_NOWAIT | M_ZERO);
	if(adminq == NULL) {
		IONIC_NETDEV_ERROR(lif->netdev, "failed to allocate rxq%d\n", qnum);
		return (error);
	}

	snprintf(adminq->name, sizeof(adminq->name) - 1, "Adq%d", qnum);
	adminq->lif = lif;
	adminq->index = qnum;
	adminq->num_descs = num_descs;
	adminq->pid = pid;
	adminq->done_color = 1;

	mtx_init(&adminq->mtx, adminq->name, NULL, MTX_DEF);

	adminq->cmd_head_index = adminq->cmd_tail_index = 0;
	adminq->comp_index = 0;

	/* Allocate DMA for command and completion rings. They must be consecutive. */
	cmd_ring_size = sizeof(*adminq->cmd_ring) * num_descs;
	comp_ring_size = sizeof(*adminq->comp_ring) * num_descs;
	total_size = ALIGN(cmd_ring_size, PAGE_SIZE) + ALIGN(cmd_ring_size, PAGE_SIZE);

	if ((error = ionic_dma_alloc(adminq->lif->ionic, total_size, &adminq->cmd_dma, BUS_DMA_NOWAIT))) {
		IONIC_NETDEV_QERR(adminq, "failed to allocated DMA cmd ring, err: %d\n", error);
		goto failed_alloc;
	}

	adminq->cmd_ring_pa = adminq->cmd_dma.dma_paddr;
	adminq->cmd_ring = (struct admin_cmd *)adminq->cmd_dma.dma_vaddr;
	IONIC_NETDEV_QINFO(adminq, "cmd base pa: 0x%lx size: 0x%x comp size: 0x%x total size: 0x%x\n",
		adminq->cmd_ring_pa, cmd_ring_size, comp_ring_size, total_size);
	/*
	 * We assume that competion ring is next to command ring.
	 */
	adminq->comp_ring = (struct admin_comp *)(adminq->cmd_dma.dma_vaddr + ALIGN(cmd_ring_size, PAGE_SIZE));

	bzero((void *)adminq->cmd_ring, total_size);

	/* Setup interrupt */
	error = ionic_intr_alloc(lif, &adminq->intr);
	if (error) {
		IONIC_NETDEV_QERR(adminq, "no available interrupt, error: %d\n", error);
		goto failed_alloc;
	}

	irq = ionic_bus_get_irq(lif->ionic, adminq->intr.index);
	if (irq < 0) {
		IONIC_NETDEV_QERR(adminq, "no available IRQ, error: %d\n", error);
		goto free_intr;
	}

	adminq->intr.vector = irq;
	ionic_intr_mask_on_assertion(&adminq->intr);
 
	*padminq = adminq;
	return 0;

free_intr:
	ionic_intr_free(lif, &adminq->intr);

failed_alloc:
	if (adminq->cmd_ring) {
		/* completion ring is part of command ring allocation. */
		ionic_dma_free(adminq->lif->ionic, &adminq->cmd_dma);
		adminq->cmd_ring = NULL;
		adminq->comp_ring = NULL;
	}

	free(adminq, M_IONIC);

	return (error);
}

static int ionic_rxque_alloc(struct lif *lif, unsigned int qnum,
			unsigned int num_descs, unsigned int pid,
			struct rxque **prxq)
{
	struct rxque *rxq;
	struct ionic_rx_buf *rxbuf;
	int i, irq, error = ENOMEM; 
	uint32_t cmd_ring_size, comp_ring_size, total_size;

	*prxq = NULL;

	rxq = malloc(sizeof(*rxq), M_IONIC, M_NOWAIT | M_ZERO);
	if(rxq == NULL) {
		IONIC_NETDEV_ERROR(lif->netdev, "failed to allocate rxq%d\n", qnum);
		return (error);
	}

	snprintf(rxq->name, sizeof(rxq->name) - 1, "RxQ%d", qnum);
	rxq->lif = lif;
	rxq->index = qnum;
	rxq->num_descs = num_descs;
	rxq->pid = pid;
	rxq->done_color = 1;

	mtx_init(&rxq->mtx, rxq->name, NULL, MTX_DEF);

	/* rx buffer and command are in tendom */
	rxq->cmd_head_index = rxq->cmd_tail_index = 0;
	rxq->comp_index = 0;

	/* Setup command ring. */
	rxq->rxbuf = malloc(sizeof(*rxq->rxbuf) * num_descs, M_IONIC, M_NOWAIT | M_ZERO);
	if (rxq->rxbuf == NULL) {
		IONIC_NETDEV_QERR(rxq, "Couldn't allocate rx buffer descriptors\n");
		goto failed_alloc;
	}

	/* Allocate DMA for command and completion rings. They must be consecutive. */
	cmd_ring_size = sizeof(*rxq->cmd_ring) * num_descs;
	comp_ring_size = sizeof(*rxq->comp_ring) * num_descs;
	total_size = ALIGN(cmd_ring_size, PAGE_SIZE) + ALIGN(cmd_ring_size, PAGE_SIZE);

	if ((error = ionic_dma_alloc(rxq->lif->ionic, total_size, &rxq->cmd_dma, BUS_DMA_NOWAIT))) {
		IONIC_NETDEV_QERR(rxq, "failed to allocated DMA cmd ring, err: %d\n", error);
		goto failed_alloc;
	}

	rxq->cmd_ring_pa = rxq->cmd_dma.dma_paddr;
	rxq->cmd_ring = (struct rxq_desc *)rxq->cmd_dma.dma_vaddr;
	IONIC_NETDEV_QINFO(rxq, "cmd base pa: 0x%lx size: 0x%x comp size: 0x%x total size: 0x%x\n",
		rxq->cmd_ring_pa, cmd_ring_size, comp_ring_size, total_size);
	/*
	 * We assume that competion ring is next to command ring.
	 */
	rxq->comp_ring = (struct rxq_comp *)(rxq->cmd_dma.dma_vaddr + ALIGN(cmd_ring_size, PAGE_SIZE));

	bzero((void *)rxq->cmd_ring, total_size);

	/* Setup interrupt */
	error = ionic_intr_alloc(lif, &rxq->intr);
	if (error) {
		IONIC_NETDEV_QERR(rxq, "no available interrupt, error: %d\n", error);
		goto failed_alloc;
	}

	irq = ionic_bus_get_irq(lif->ionic, rxq->intr.index);
	if (irq < 0) {
		IONIC_NETDEV_QERR(rxq, "no available IRQ, error: %d\n", error);
		goto failed_alloc;
	}

	rxq->intr.vector = irq;
	ionic_intr_mask_on_assertion(&rxq->intr);
 
	/*
	 * Create just one tag for Rx bufferes.
	 */
	error = bus_dma_tag_create(
	         /*      parent */ bus_get_dma_tag(rxq->lif->ionic->dev->bsddev),
	         /*   alignment */ 1,
	         /*      bounds */ 0,
	         /*     lowaddr */ BUS_SPACE_MAXADDR,
	         /*    highaddr */ BUS_SPACE_MAXADDR,
	         /*      filter */ NULL,
	         /*   filterarg */ NULL,
	         /*     maxsize */ MJUM16BYTES,
	         /*   nsegments */ 1,
	         /*  maxsegsize */ MJUM16BYTES,
	         /*       flags */ 0,
	         /*    lockfunc */ NULL,
	         /* lockfuncarg */ NULL,
	                           &rxq->buf_tag);

	if (error) {
		IONIC_NETDEV_QERR(rxq, "failed to create DMA tag, err: %d\n", error);
		goto free_intr;
	}

	for ( rxbuf = rxq->rxbuf, i = 0 ; rxbuf != NULL && i < num_descs; i++, rxbuf++ ) {
		error = bus_dmamap_create(rxq->buf_tag, 0, &rxbuf->dma_map);
		if (error) {
			IONIC_NETDEV_QERR(rxq, "failed to create map for entry%d, err: %d\n", i, error);
			bus_dma_tag_destroy(rxq->buf_tag);
			goto free_intr;
		}
	}

	*prxq = rxq;

	return 0;

free_intr:
	ionic_intr_free(lif, &rxq->intr);

failed_alloc:
	if (rxq->cmd_ring) {
		/* completion ring is part of command ring allocation. */
		ionic_dma_free(rxq->lif->ionic, &rxq->cmd_dma);
		rxq->cmd_ring = NULL;
		rxq->comp_ring = NULL;
	}

	if (rxq->rxbuf) {
		free(rxq->rxbuf, M_IONIC);
		rxq->rxbuf = NULL;
	}

	free(rxq, M_IONIC);

	return (error);
}

static int ionic_txque_alloc(struct lif *lif, unsigned int qnum,
			   unsigned int num_descs, unsigned int pid,
			   struct txque **ptxq)
{
	struct txque *txq;
	struct ionic_tx_buf *txbuf;
	int i, irq, error = ENOMEM;
	uint32_t cmd_ring_size, comp_ring_size, sg_ring_size, total_size;

	*ptxq = NULL;

	txq = malloc(sizeof(*txq), M_IONIC, M_NOWAIT | M_ZERO);
	if(txq == NULL) {
		IONIC_NETDEV_ERROR(lif->netdev, "failed to allocate rxq%d\n", qnum);
		return (error);
	}

	snprintf(txq->name, sizeof(txq->name) - 1, "TxQ%d", qnum);
	txq->lif = lif;
	txq->index = qnum;
	txq->num_descs = num_descs;
	txq->pid = pid;
	txq->done_color = 1;

	mtx_init(&txq->mtx, txq->name, NULL, MTX_DEF);

	/* rx buffer and command are in tendom */
	txq->cmd_head_index = txq->cmd_tail_index = 0;
	txq->comp_index = 0;

	/* Setup command ring. */
	txq->txbuf = malloc(sizeof(*txq->txbuf) * num_descs, M_IONIC, M_NOWAIT | M_ZERO);
	if (txq->txbuf == NULL) {
		IONIC_NETDEV_QERR(txq, "Couldn't allocate tx buffer descriptors\n");
		goto failed_alloc;
	}

	/* Allocate DMA for command and completion rings. They must be consecutive. */
	cmd_ring_size = sizeof(*txq->cmd_ring) * num_descs;
	comp_ring_size = sizeof(*txq->comp_ring) * num_descs;
	sg_ring_size = sizeof(*txq->sg_ring) * num_descs;
	total_size = ALIGN(cmd_ring_size, PAGE_SIZE) + ALIGN(cmd_ring_size, PAGE_SIZE) + ALIGN(sg_ring_size, PAGE_SIZE);

	if ((error = ionic_dma_alloc(txq->lif->ionic, total_size, &txq->cmd_dma, BUS_DMA_NOWAIT))) {
		IONIC_NETDEV_QERR(txq, "failed to allocated DMA cmd ring, err: %d\n", error);
		goto failed_alloc;
	}

	txq->cmd_ring_pa = txq->cmd_dma.dma_paddr;
	txq->cmd_ring = (struct txq_desc *)txq->cmd_dma.dma_vaddr;
	IONIC_NETDEV_QINFO(txq, "cmd base pa: 0x%lx size: 0x%x comp size: 0x%x total size: 0x%x\n",
		txq->cmd_ring_pa, cmd_ring_size, comp_ring_size, total_size);
	/*
	 * We assume that competion ring is next to command ring.
	 */
	txq->comp_ring = (struct txq_comp *)(txq->cmd_dma.dma_vaddr + ALIGN(cmd_ring_size, PAGE_SIZE));
	txq->sg_ring = (struct txq_sg_desc *)(txq->cmd_dma.dma_vaddr + ALIGN(cmd_ring_size, PAGE_SIZE) + ALIGN(comp_ring_size, PAGE_SIZE));

	bzero((void *)txq->cmd_ring, total_size);

	/* Allocate buffere ring. */
	txq->br = buf_ring_alloc(4096, M_IONIC, M_WAITOK, &txq->mtx);
	if (txq->br == NULL) {
		IONIC_NETDEV_QERR(txq, "failed to allocated buffer ring\n");
		goto failed_alloc;
	}

	/* Setup interrupt */
	error = ionic_intr_alloc(lif, &txq->intr);
	if (error) {
		IONIC_NETDEV_QERR(txq, "no available interrupt, error: %d\n", error);
		goto failed_alloc;
	}

	irq = ionic_bus_get_irq(lif->ionic, txq->intr.index);
	if (irq < 0) {
		IONIC_NETDEV_QERR(txq, "no available IRQ, error: %d\n", error);
		goto failed_alloc;
	}

	txq->intr.vector = irq;
	ionic_intr_mask_on_assertion(&txq->intr);
 
	/*
	 * Create just one tag for Rx bufferrs. 
	 */
	error = bus_dma_tag_create(
	         /*      parent */ bus_get_dma_tag(txq->lif->ionic->dev->bsddev),
	         /*   alignment */ 1,
	         /*      bounds */ 0,
	         /*     lowaddr */ BUS_SPACE_MAXADDR,
	         /*    highaddr */ BUS_SPACE_MAXADDR,
	         /*      filter */ NULL,
	         /*   filterarg */ NULL,
	         /*     maxsize */ 64000,
	         /*   nsegments */ 16,
	         /*  maxsegsize */ 4096,
	         /*       flags */ 0,
	         /*    lockfunc */ NULL,
	         /* lockfuncarg */ NULL,
	                           &txq->buf_tag);

	if (error) {
		IONIC_NETDEV_QERR(txq, "failed to create DMA tag, err: %d\n", error);
		goto free_intr;
	}

	for ( txbuf = txq->txbuf, i = 0 ; txbuf != NULL && i < num_descs; i++, txbuf++ ) {
		error = bus_dmamap_create(txq->buf_tag, 0, &txbuf->dma_map);
		if (error) {
			IONIC_NETDEV_QERR(txq, "failed to create map for entry%d, err: %d\n", i, error);
			bus_dma_tag_destroy(txq->buf_tag);
			goto free_intr;
		}
	}

	IONIC_NETDEV_QINFO(txq, "create txq\n");
	*ptxq = txq;

	return 0;

free_intr:
	ionic_intr_free(lif, &txq->intr);

failed_alloc:
	if (txq->br) {
		buf_ring_free(txq->br, M_IONIC);
		txq->br = NULL;
	}

	if (txq->cmd_ring) {
		/* completion ring is part of command ring allocation. */
		ionic_dma_free(txq->lif->ionic, &txq->cmd_dma);
		txq->cmd_ring = NULL;
		txq->comp_ring = NULL;
		txq->sg_ring = NULL;
	}

	if (txq->txbuf) {
		free(txq->txbuf, M_IONIC);
		txq->txbuf = NULL;
	}

	free(txq, M_IONIC);

	return (error);
}

static void ionic_rxq_free(struct lif *lif, struct rxque *rxq)
{

	mtx_lock(&rxq->mtx);
	if (rxq->cmd_ring) {
		/* completion ring is part of command ring allocation. */
		ionic_dma_free(rxq->lif->ionic, &rxq->cmd_dma);
		rxq->cmd_ring = NULL;
		rxq->comp_ring = NULL;
	}

	ionic_intr_free(lif, &rxq->intr);
	mtx_unlock(&rxq->mtx);
	mtx_destroy(&rxq->mtx);
}

static void ionic_txq_free(struct lif *lif, struct txque *txq)
{

	mtx_lock(&txq->mtx);
	if (txq->cmd_ring) {
		/* completion ring is part of command ring allocation. */
		ionic_dma_free(txq->lif->ionic, &txq->cmd_dma);
		txq->cmd_ring = NULL;
		txq->comp_ring = NULL;
	}

	ionic_intr_free(lif, &txq->intr);
	mtx_unlock(&txq->mtx);
	mtx_destroy(&txq->mtx);
}

static void ionic_adminq_free(struct lif *lif, struct adminq *adminq)
{

	mtx_lock(&adminq->mtx);
	if (adminq->cmd_ring) {
		/* completion ring is part of command ring allocation. */
		ionic_dma_free(adminq->lif->ionic, &adminq->cmd_dma);
		adminq->cmd_ring = NULL;
		adminq->comp_ring = NULL;
	}

	ionic_intr_free(lif, &adminq->intr);
	mtx_unlock(&adminq->mtx);
	mtx_destroy(&adminq->mtx);
}


static unsigned int ionic_pid_get(struct lif *lif, unsigned int page)
{
	unsigned int ndbpgs_per_lif = lif->ionic->ident->dev.ndbpgs_per_lif;

	BUG_ON(ndbpgs_per_lif < page + 1);

	return lif->index * ndbpgs_per_lif + page;
}

static int ionic_qcqs_alloc(struct lif *lif)
{
	unsigned int pid;
	unsigned int i;
	int err = -ENOMEM;

	lif->txqs = kzalloc(sizeof(*lif->txqs) * lif->ntxqs, GFP_KERNEL);
	if (!lif->txqs)
		return -ENOMEM;

	lif->rxqs = kzalloc(sizeof(*lif->rxqs) * lif->nrxqs, GFP_KERNEL);
	if (!lif->rxqs)
		return -ENOMEM;

	pid = ionic_pid_get(lif, 0);

	/* XXX: we are tight on name description */
	err = ionic_adminq_alloc(lif, 0, adminq_descs, pid, &lif->adminqcq);
	if (err)
		return err;


	for (i = 0; i < lif->ntxqs; i++) {
		err = ionic_txque_alloc(lif, i, ntxq_descs, pid, &lif->txqs[i]);
		if (err)
			goto err_out_free_adminqcq;
	}

	for (i = 0; i < lif->nrxqs; i++) {
		err = ionic_rxque_alloc(lif, i, nrxq_descs, pid, &lif->rxqs[i]);
		if (err)
			goto err_out_free_txqs;
	}

	return 0;

err_out_free_txqs:
	for (i = 0; i < lif->ntxqs; i++)
		ionic_txq_free(lif, lif->txqs[i]);
err_out_free_adminqcq:
	ionic_adminq_free(lif, lif->adminqcq);

	return err;
}

static void ionic_qcqs_free(struct lif *lif)
{
	unsigned int i;

	for (i = 0; i < lif->nrxqs; i++) {
		ionic_rx_empty(lif->rxqs[i]);
		ionic_rxq_free(lif, lif->rxqs[i]);
	}
	for (i = 0; i < lif->ntxqs; i++)
		ionic_txq_free(lif, lif->txqs[i]);

	ionic_adminq_free(lif, lif->adminqcq);

}

static int ionic_lif_alloc(struct ionic *ionic, unsigned int index)
{
	struct device *dev = ionic->dev;
//	struct net_device *netdev;
	struct lif *lif;
	int err;

	lif = kzalloc(sizeof(*lif), GFP_KERNEL);
	if (!lif) {
		dev_err(dev, "Cannot allocate lif, aborting\n");
		return -ENOMEM;
	}

	snprintf(lif->name, sizeof(lif->name), "ionic%u", index);
	lif->ionic = ionic;
	lif->index = index;
	lif->neqs = ionic->neqs_per_lif;
	lif->ntxqs = ionic->ntxqs_per_lif;
	lif->nrxqs = ionic->nrxqs_per_lif;

	err = ionic_lif_netdev_alloc(lif, ntxq_descs);
	if (err) {
		dev_err(dev, "Cannot allocate netdev, aborting\n");
		return (err);
	}

	spin_lock_init(&lif->adminq_lock);
	lif->adminq_wq = create_workqueue(lif->name);

	err = ionic_qcqs_alloc(lif);
	if (err)
		goto err_out_free_netdev;

	list_add_tail(&lif->list, &ionic->lifs);

	return 0;

err_out_free_netdev:
	ionic_lif_netdev_free(lif);
	kfree(lif);

	return err;
}

int ionic_lifs_alloc(struct ionic *ionic)
{
	unsigned int i;
	int err;

	INIT_LIST_HEAD(&ionic->lifs);

	for (i = 0; i < ionic->ident->dev.nlifs; i++) {
		err = ionic_lif_alloc(ionic, i);
		if (err)
			return err;
	}

	return 0;
}

void ionic_lifs_free(struct ionic *ionic)
{
	struct list_head *cur, *tmp;
	struct lif *lif;

	list_for_each_safe(cur, tmp, &ionic->lifs) {
		lif = list_entry(cur, struct lif, list);
		list_del(&lif->list);
		flush_workqueue(lif->adminq_wq);
		destroy_workqueue(lif->adminq_wq);
		ionic_qcqs_free(lif);
		ionic_lif_netdev_free(lif);
		kfree(lif);
	}
}

#ifdef notyet
static int ionic_lif_stats_dump_start(struct lif *lif, unsigned int ver)
{
	struct net_device *netdev = lif->netdev;
	struct device *dev = lif->ionic->dev;
	struct ionic_admin_ctx ctx = {
		.work = COMPLETION_INITIALIZER_ONSTACK(ctx.work),
		.cmd.stats_dump = {
			.opcode = CMD_OPCODE_STATS_DUMP_START,
			.ver = ver,
		},
	};
	int err;

	lif->stats_dump = dma_alloc_coherent(dev, sizeof(*lif->stats_dump),
					     &lif->stats_dump_pa, GFP_KERNEL);

	if (!lif->stats_dump) {
		IONIC_NETDEV_ERROR(netdev, "%s OOM\n", __func__);
		return -ENOMEM;
	}

	ctx.cmd.stats_dump.addr = lif->stats_dump_pa;

	IONIC_NETDEV_INFO(netdev, "stats_dump START ver %d addr 0x%llx\n", ver,
		    lif->stats_dump_pa);

	return 0;
	err = ionic_adminq_post_wait(lif, &ctx);
	if (err)
		goto err_out_free;

	return 0;

err_out_free:
	dma_free_coherent(dev, sizeof(*lif->stats_dump), lif->stats_dump,
			  lif->stats_dump_pa);
	return err;
}

static void ionic_lif_stats_dump_stop(struct lif *lif)
{
	struct net_device *netdev = lif->netdev;
	struct device *dev = lif->ionic->dev;
	struct ionic_admin_ctx ctx = {
		.work = COMPLETION_INITIALIZER_ONSTACK(ctx.work),
		.cmd.stats_dump = {
			.opcode = CMD_OPCODE_STATS_DUMP_STOP,
		},
	};
	int err;

	IONIC_NETDEV_INFO(netdev, "stats_dump STOP\n");

	err = ionic_adminq_post_wait(lif, &ctx);
	if (err) {
		IONIC_NETDEV_ERROR(netdev, "stats_dump cmd failed %d\n", err);
		return;
	}

	dma_free_coherent(dev, sizeof(*lif->stats_dump), lif->stats_dump,
			  lif->stats_dump_pa);
}

#endif /* notyet */


int
ionic_rss_ind_tbl_set(struct lif *lif, const u32 *indir)
{
	struct ionic_admin_ctx ctx = {
		.work = COMPLETION_INITIALIZER_ONSTACK(ctx.work),
		.cmd.rss_indir_set = {
			.opcode = CMD_OPCODE_RSS_INDIR_SET,
			.addr = lif->rss_ind_tbl_pa,
		},
#ifdef HAPS
		.side_data = lif->rss_ind_tbl,
		.side_data_len = RSS_IND_TBL_SIZE,
#endif
	};
	unsigned int i;

	if (indir)
		for (i = 0; i < RSS_IND_TBL_SIZE; i++)
			lif->rss_ind_tbl[i] = indir[i];

	IONIC_NETDEV_INFO(lif->netdev, "rss_ind_tbl_set\n");

	return ionic_adminq_post_wait(lif, &ctx);
}

int ionic_rss_hash_key_set(struct lif *lif, const u8 *key, uint16_t rss_types)
{
	struct ionic_admin_ctx ctx = {
		.work = COMPLETION_INITIALIZER_ONSTACK(ctx.work),
		.cmd.rss_hash_set = {
			.opcode = CMD_OPCODE_RSS_HASH_SET,
			.types = rss_types,
		},
	};

	memcpy(lif->rss_hash_key, key, RSS_HASH_KEY_SIZE);

	memcpy(ctx.cmd.rss_hash_set.key, lif->rss_hash_key,
	       RSS_HASH_KEY_SIZE);

	IONIC_NETDEV_INFO(lif->netdev, "rss_hash_key_set\n");

	return ionic_adminq_post_wait(lif, &ctx);
}



static void ionic_lif_adminq_deinit(struct lif *lif, struct adminq *adminq)
{
	ionic_intr_mask(&adminq->intr, true);
	free_irq(adminq->intr.vector, &adminq->napi);
	netif_napi_del(&adminq->napi);
}


static void ionic_lif_txqs_deinit(struct lif *lif)
{
	unsigned int i;
	struct txque* txq;

	for (i = 0; i < lif->nrxqs; i++) {
		txq = lif->txqs[i];

		mtx_lock(&txq->mtx);

		ionic_intr_mask(&txq->intr, true);
		free_irq(txq->intr.vector, txq);

		if (txq->taskq)
			taskqueue_free(txq->taskq);

		mtx_unlock(&txq->mtx);
	}
}

static void ionic_lif_rxqs_deinit(struct lif *lif)
{
	unsigned int i;
	struct rxque* rxq;

	for (i = 0; i < lif->nrxqs; i++) {
		rxq = lif->rxqs[i];

		mtx_lock(&rxq->mtx);
		tcp_lro_free(&rxq->lro);

		ionic_intr_mask(&rxq->intr, true);
		free_irq(rxq->intr.vector, rxq);
		
		if (rxq->taskq)
			taskqueue_free(rxq->taskq);

		mtx_unlock(&rxq->mtx);
	}
}

static void ionic_lif_deinit(struct lif *lif)
{
	ether_ifdetach(lif->netdev);
	ionic_lif_adminq_deinit(lif, lif->adminqcq);
	ionic_lif_txqs_deinit(lif);
	ionic_lif_rxqs_deinit(lif);
}

void ionic_lifs_deinit(struct ionic *ionic)
{
	struct list_head *cur;
	struct lif *lif;

	list_for_each(cur, &ionic->lifs) {
		lif = list_entry(cur, struct lif, list);
		ionic_lif_deinit(lif);
	}
}

static int ionic_lif_adminq_init(struct lif *lif)
{
	struct adminq *adminq = lif->adminqcq;
	struct ionic_dev *idev = &lif->ionic->idev;
	struct napi_struct *napi = &adminq->napi;
	struct adminq_init_comp comp;
	int err = 0;

	union dev_cmd cmd = {
		.adminq_init.opcode = CMD_OPCODE_ADMINQ_INIT,
		.adminq_init.index = adminq->index,
		.adminq_init.pid = adminq->pid,
		.adminq_init.intr_index = 0,//intr_index,
		.adminq_init.lif_index = lif->index,
		.adminq_init.ring_size = ilog2(adminq->num_descs),
		.adminq_init.ring_base = adminq->cmd_ring_pa,
	};

	//printk(KERN_ERR "adminq_init.pid %d\n", cmd.adminq_init.pid);
	//printk(KERN_ERR "adminq_init.index %d\n", cmd.adminq_init.index);
	//printk(KERN_ERR "adminq_init.ring_base %llx\n",
	//       cmd.adminq_init.ring_base);
	//printk(KERN_ERR "adminq_init.ring_size %d\n",
	//       cmd.adminq_init.ring_size);
	ionic_dev_cmd_go(idev, &cmd);

	err = ionic_dev_cmd_wait_check(idev, IONIC_DEVCMD_TIMEOUT);
	if (err)
		return err;

	ionic_dev_cmd_comp(idev, &comp);

	IONIC_NETDEV_QINFO(adminq, "qid %d pid %d index %d ring_base 0x%lx ring_size %d\n",
		comp.qid, cmd.adminq_init.pid, cmd.adminq_init.index, cmd.adminq_init.ring_base,
		cmd.adminq_init.ring_size);

	adminq->qid = comp.qid;
	adminq->qtype = comp.qtype;
	adminq->db  = (void *)adminq->lif->ionic->idev.db_pages + (adminq->pid * PAGE_SIZE);
	adminq->db += adminq->qtype;

	snprintf(adminq->intr.name, sizeof(adminq->intr.name), "%s", adminq->name);

	netif_napi_add(lif->netdev, &adminq->napi, ionic_adminq_napi,
		       NAPI_POLL_WEIGHT);

	err = request_irq(adminq->intr.vector, ionic_adminq_isr, 0, adminq->intr.name, adminq);
	if (err) {
		netif_napi_del(napi);
		return err;
	}

	IONIC_NETDEV_QINFO(adminq, "qid: %d qtype: %d db: %pd\n",
		adminq->qid, adminq->qtype, adminq->db);

	return 0;
}


int ionic_tx_clean(struct txque* txq , int tx_limit)
{
	struct txq_comp *comp;
	struct ionic_tx_buf *txbuf;
	int comp_index, processed, cmd_stop_index;
	struct tx_stats * stats = &txq->stats;

	stats->clean++;
	
	/* Sync every time descriptors. */
	bus_dmamap_sync(txq->cmd_dma.dma_tag, txq->cmd_dma.dma_map,
		BUS_DMASYNC_POSTREAD | BUS_DMASYNC_POSTWRITE);
	
	for ( processed = 0 ; processed < tx_limit ; processed++) {
		comp_index = txq->comp_index;
	//	cmd_index = txq->cmd_tail_index;

		comp = &txq->comp_ring[comp_index];
		cmd_stop_index = comp->comp_index;

		if (comp->color != txq->done_color)
			break;

		IONIC_NETDEV_TX_TRACE(txq, "comp @ %d for desc @ %d comp->color %d done_color %d\n",
			comp_index, cmd_stop_index, comp->color, txq->done_color);

		txbuf = &txq->txbuf[cmd_stop_index];
		/* TSO last buffer only points to valid mbuf. */
		if (txbuf->m != NULL) {
			IONIC_NETDEV_TX_TRACE(txq, "free txbuf @:%d\n", cmd_stop_index);
			bus_dmamap_sync(txq->buf_tag, txbuf->dma_map, BUS_DMASYNC_POSTWRITE);
			bus_dmamap_unload(txq->buf_tag, txbuf->dma_map);
			m_freem(txbuf->m);
		}

		txq->comp_index = (txq->comp_index + 1) % txq->num_descs;
		/* XXX: should we comp stop index to jump for TSO. */
		txq->cmd_tail_index = (cmd_stop_index + 1) % txq->num_descs;
		/* Roll over condition, flip color. */
		if (txq->comp_index == 0) {
			txq->done_color = !txq->done_color;
		}
	}

//	IONIC_NETDEV_TX_TRACE(txq, "ionic_tx_clean processed %d\n", processed);

	if (comp->color == txq->done_color)
		taskqueue_enqueue(txq->taskq, &txq->task);

	return (processed);
}


static irqreturn_t ionic_tx_isr(int irq, void *data)
{
	struct txque* txq = data;
	//struct tx_stats* txstats = &txq->stats;
	int work_done = 0;

	mtx_lock(&txq->mtx);
	//txstats->isr_count++;

	ionic_intr_mask(&txq->intr, true);
 
	work_done = ionic_tx_clean(txq, 256/* XXX: tunable */);
	IONIC_NETDEV_TX_TRACE(txq, "ionic_tx_is processed %d descriptors\n", work_done);
	
	ionic_intr_return_credits(&txq->intr, work_done, 0, true);

	// Enable interrupt.
	ionic_intr_mask(&txq->intr, false);
	mtx_unlock(&txq->mtx);

	return IRQ_HANDLED;
}

static void
ionic_tx_task_handler(void *arg, int pendindg)
{

	struct txque* txq = arg;
	int err;

	KASSERT((txq == NULL), ("task handler called with txq == NULL"));

	if (drbr_empty(txq->lif->netdev, txq->br))
		return;

	IONIC_NETDEV_TX_TRACE(txq, "ionic_tx_task\n");
	mtx_lock(&txq->mtx);
	/*
	 * Process all Tx frames.
	 */
	err = ionic_start_xmit_locked(txq->lif->netdev, txq);
	mtx_unlock(&txq->mtx);
}

static int ionic_lif_txq_init(struct lif *lif, struct txque *txq)
{
	struct ionic_admin_ctx ctx = {
		.work = COMPLETION_INITIALIZER_ONSTACK(ctx.work),
		.cmd.txq_init = {
			.opcode = CMD_OPCODE_TXQ_INIT,
			.I = false,
			.E = false,
			.pid = txq->pid,
			.intr_index = txq->intr.index,
			.type = TXQ_TYPE_ETHERNET,
			.index = txq->index,
			.cos = 0,
			.ring_base = txq->cmd_ring_pa,
			.ring_size = ilog2(txq->num_descs),
		},
	};
	int err, bind_cpu;

	IONIC_NETDEV_QINFO(txq, "qid %d pid %d index %d ring_base 0x%lx ring_size %d\n",
		ctx.comp.txq_init.qid, ctx.cmd.txq_init.pid, ctx.cmd.txq_init.index, ctx.cmd.txq_init.ring_base, ctx.cmd.txq_init.ring_size);

	err = ionic_adminq_post_wait(lif, &ctx);
	if (err)
		return err;

	txq->qid = ctx.comp.txq_init.qid;
	txq->qtype = ctx.comp.txq_init.qtype;
	txq->db  = (void *)txq->lif->ionic->idev.db_pages + (txq->pid * PAGE_SIZE);
	txq->db += txq->qtype;

	snprintf(txq->intr.name, sizeof(txq->intr.name), "%s", txq->name);

	request_irq(txq->intr.vector, ionic_tx_isr, 0, txq->intr.name, txq);

	TASK_INIT(&txq->task, 0, ionic_tx_task_handler, txq);
    txq->taskq = taskqueue_create_fast(txq->name, M_NOWAIT,
	    taskqueue_thread_enqueue, &txq->taskq);

#ifdef RSS
	bind_cpu = rss_getcpu(txq->index % rss_getnumbuckets());
#else
	bind_cpu = txq->index;
#endif
	err = bind_irq_to_cpu(txq->intr.vector, bind_cpu);
	if (err) {
		IONIC_NETDEV_QWARN(txq, "failed to bind to cpu%d\n", bind_cpu);
	}
	IONIC_NETDEV_QINFO(txq, "bound to cpu%d\n", bind_cpu);

	IONIC_NETDEV_QINFO(txq, "qid: %d qtype: %d db: %p\n",
		txq->qid, txq->qtype, txq->db);

	return 0;
}

static int ionic_lif_txqs_init(struct lif *lif)
{
	unsigned int i;
	int err;

	for (i = 0; i < lif->ntxqs; i++) {
		err = ionic_lif_txq_init(lif, lif->txqs[i]);
		if (err)
			return err;
	}

	return 0;
}

/* XXX rx_limit/pending handling. */
static int ionic_rx_clean(struct rxque* rxq , int rx_limit)
{
	struct rxq_comp *comp;
	struct rxq_desc *cmd;
	struct ionic_rx_buf *rxbuf;
	int comp_index, cmd_index, processed, cmd_stop_index;

	for ( processed = 0 ; processed < rx_limit ; ) {
		comp_index = rxq->comp_index;
		comp = &rxq->comp_ring[comp_index];
		/* Sync every time descriptors. */
		bus_dmamap_sync(rxq->cmd_dma.dma_tag, rxq->cmd_dma.dma_map,
			BUS_DMASYNC_POSTREAD | BUS_DMASYNC_POSTWRITE);

		cmd_stop_index = comp->comp_index;
		cmd_index = rxq->cmd_tail_index;
		rxbuf = &rxq->rxbuf[cmd_index];
		cmd = &rxq->cmd_ring[cmd_index];

		if (comp->color != rxq->done_color)
			break;

		IONIC_NETDEV_RX_TRACE(rxq, "comp :%d cmd start: %d cmd stop: %d comp->color %d done_color %d\n",
			comp_index, cmd_index, cmd_stop_index, comp->color, rxq->done_color);
		IONIC_NETDEV_RX_TRACE(rxq, "buf[%d] opcode:%d addr:0%lx len:0x%x\n",
			cmd_index, cmd->opcode, cmd->addr, cmd->len);

		for ( ; cmd_index == cmd_stop_index; cmd_index++, processed++ ) {
			rxbuf = &rxq->rxbuf[cmd_index];
			cmd = &rxq->cmd_ring[cmd_index];
			ionic_rx_input(rxq, rxbuf, comp, cmd);
		}

		rxq->comp_index = (rxq->comp_index + 1) % rxq->num_descs;
		rxq->cmd_tail_index = (rxq->cmd_tail_index + 1) % rxq->num_descs;
		/* Roll over condition, flip color. */
		if (rxq->comp_index == 0) {
			rxq->done_color = !rxq->done_color;
		}
	}

	IONIC_NETDEV_RX_TRACE(rxq, "ionic_rx_clean processed %d\n", processed);

	if (comp->color == rxq->done_color)
		taskqueue_enqueue(rxq->taskq, &rxq->task);

	/* XXX: Refill mbufs if we have processed some n packets. */
	ionic_rx_fill(rxq);

	/* XXX: flush at the end of ISR or taskqueue handler? */
	tcp_lro_flush_all(&rxq->lro);

	return (processed);

}


static void
ionic_rx_task_handler(void *arg, int pendindg)
{

	struct rxque* rxq = arg;
	int processed;

	KASSERT((rxq == NULL), ("task handler called with qcq == NULL"));

	mtx_lock(&rxq->mtx);
	/* 
	 * Process all Rx frames.
	 */
	processed = ionic_rx_clean(rxq, -1);
	mtx_unlock(&rxq->mtx);
}

void ionic_rx_flush(struct rxque *rxq)
{
	unsigned int work_done;

	IONIC_NETDEV_RX_TRACE(rxq, "\n");

	work_done = ionic_rx_clean(rxq, -1);

	if (work_done > 0)
		ionic_intr_return_credits(&rxq->intr, work_done, 0, true);

	taskqueue_drain(rxq->taskq, &rxq->task);
}

static irqreturn_t ionic_rx_isr(int irq, void *data)
{
	struct rxque* rxq = data;
	struct rx_stats* rxstats = &rxq->stats;
	int work_done;

	mtx_lock(&rxq->mtx);
	rxstats->isr_count++;

	ionic_intr_mask(&rxq->intr, true);

	work_done = ionic_rx_clean(rxq, 256/* XXX: tunable */);

	ionic_intr_return_credits(&rxq->intr, work_done, 0, true);

	// Enable interrupt.
	ionic_intr_mask(&rxq->intr, false);
	mtx_unlock(&rxq->mtx);

	return IRQ_HANDLED;
}


static int ionic_lif_rxq_init(struct lif *lif, struct rxque *rxq)
{
	struct lro_ctrl *lro = &rxq->lro;
	struct ionic_admin_ctx ctx = {
		.work = COMPLETION_INITIALIZER_ONSTACK(ctx.work),
		.cmd.rxq_init = {
			.opcode = CMD_OPCODE_RXQ_INIT,
			.I = false,
			.E = false,
			.pid = rxq->pid,
			.intr_index = rxq->intr.index,
			.type = RXQ_TYPE_ETHERNET,
			.index = rxq->index,
			.ring_base = rxq->cmd_ring_pa,
			.ring_size = ilog2(rxq->num_descs),
		},
	};
	int err, bind_cpu;

	IONIC_NETDEV_QINFO(rxq, "pid %d index %d ring_base 0x%lx ring_size %d\n",
		 ctx.cmd.rxq_init.pid, ctx.cmd.rxq_init.index, ctx.cmd.rxq_init.ring_base, ctx.cmd.rxq_init.ring_size);

	err = ionic_adminq_post_wait(lif, &ctx);
	if (err)
		return err;

	rxq->qid = ctx.comp.rxq_init.qid;
	rxq->qtype = ctx.comp.rxq_init.qtype;

	/* XXX: move to be part of ring doorbell */
	rxq->db  = (void *)rxq->lif->ionic->idev.db_pages + (rxq->pid * PAGE_SIZE);
	rxq->db += rxq->qtype;
	IONIC_NETDEV_QINFO(rxq, "doorbell: %p\n", rxq->db);

	snprintf(rxq->intr.name, sizeof(rxq->intr.name), "%s", rxq->name);
	request_irq(rxq->intr.vector, ionic_rx_isr, 0, rxq->intr.name, rxq);

#ifdef RSS
	bind_cpu = rss_getcpu(rxq->index % rss_getnumbuckets());
#else
	bind_cpu = rxq->index;
#endif
	err = bind_irq_to_cpu(rxq->intr.vector, bind_cpu);
	if (err) {
		IONIC_NETDEV_QWARN(rxq, "failed to bindto cpu%d\n", bind_cpu);
	}
	IONIC_NETDEV_QINFO(rxq, "bound to cpu%d\n", bind_cpu);

	TASK_INIT(&rxq->task, 0, ionic_rx_task_handler, rxq);
    rxq->taskq = taskqueue_create_fast(rxq->intr.name, M_NOWAIT,
	    taskqueue_thread_enqueue, &rxq->taskq);
	/* RSS task queue binding. */
#ifdef RSS
#else
    err = taskqueue_start_threads(&rxq->taskq, 1, PI_NET,
        "%s (que %s)", device_get_nameunit(lif->ionic->dev), rxq->intr.name);
#endif
	if (err) {
		IONIC_NETDEV_QERR(rxq, "failed to create task queue, error: %d\n",
			err);
		taskqueue_free(rxq->taskq);
		return (err);
	}

	if (lif->netdev->if_capenable & IFCAP_LRO) {
		err = tcp_lro_init(lro);
		if (err) {
			IONIC_NETDEV_QERR(rxq, "LRO setup failed, error: %d\n", err);
		} else {
			lro->ifp = lif->netdev;
		}
	}

	IONIC_NETDEV_QINFO(rxq, "qid: %d qtype: %d db:%p\n", rxq->qid, rxq->qtype, rxq->db);

	return 0;
}

static int ionic_lif_rxqs_init(struct lif *lif)
{
	unsigned int i;
	int err;

	for (i = 0; i < lif->nrxqs; i++) {
		err = ionic_lif_rxq_init(lif, lif->rxqs[i]);
		if (err)
			return err;
	}

	return 0;
}

static int ionic_station_set(struct lif *lif)
{
	struct net_device *netdev = lif->netdev;
	struct ionic_admin_ctx ctx = {
		.work = COMPLETION_INITIALIZER_ONSTACK(ctx.work),
		.cmd.station_mac_addr_get = {
			.opcode = CMD_OPCODE_STATION_MAC_ADDR_GET,
		},
	};
	int err;

	err = ionic_adminq_post_wait(lif, &ctx);
	if (err)
		return err;

	if (!is_zero_ether_addr(lif->dev_addr)) {
		IONIC_NETDEV_INFO(netdev, "deleting station MAC addr %pM\n",
			   lif->dev_addr);
		ether_ifdetach(netdev);
		ionic_lif_addr(lif, lif->dev_addr, false);
	}
	memcpy(lif->dev_addr, ctx.comp.station_mac_addr_get.addr,
	       ETHER_ADDR_LEN);
	IONIC_NETDEV_INFO(netdev, "adding station MAC addr %pM\n",
		   lif->dev_addr);
	ionic_lif_addr(lif, lif->dev_addr, true);
	ether_ifattach(netdev, lif->dev_addr);

	return 0;
}

static int ionic_lif_init(struct lif *lif)
{
	struct ionic_dev *idev = &lif->ionic->idev;
	int err;

	ionic_dev_cmd_lif_init(idev, lif->index);
	err = ionic_dev_cmd_wait_check(idev, IONIC_DEVCMD_TIMEOUT);
	if (err)
		return err;

	err = ionic_lif_adminq_init(lif);
	if (err)
		return err;

	/* Enabling interrupts on adminq from here on... */
	ionic_intr_mask(&lif->adminqcq->intr, false);

	lif->buf_len = MCLBYTES;
#ifdef notyet
	if (lif->max_frame_size <= MCLBYTES)
		len = MCLBYTES;
	else
		len = MJUMPAGESIZE;
#endif

	err = ionic_lif_txqs_init(lif);
	if (err)
		goto err_out_mask_adminq;

	err = ionic_lif_rxqs_init(lif);
	if (err)
		goto err_out_txqs_deinit;

	err = ionic_station_set(lif);
	if (err)
		goto err_out_rxqs_deinit;

	err = ionic_lif_rss_setup(lif);
	if (err)
		goto err_out_rxqs_deinit;
#ifdef notyet
	err = ionic_lif_stats_dump_start(lif, STATS_DUMP_VERSION_1);
	if (err)
		goto err_out_rss_teardown;
#endif
	ionic_set_rx_mode(lif->netdev);

	ionic_setup_sysctls(lif);

	lif->api_private = NULL;

	return 0;

//err_out_rss_teardown:
	ionic_lif_rss_teardown(lif);
err_out_rxqs_deinit:
	ionic_lif_rxqs_deinit(lif);
err_out_txqs_deinit:
	ionic_lif_txqs_deinit(lif);
err_out_mask_adminq:
	ionic_intr_mask(&lif->adminqcq->intr, true);

	return err;
}

int ionic_lifs_init(struct ionic *ionic)
{
	struct list_head *cur;
	struct lif *lif;
	int err;

	list_for_each(cur, &ionic->lifs) {
		lif = list_entry(cur, struct lif, list);
		err = ionic_lif_init(lif);
		if (err)
			return err;
	}

	return 0;
}

/*
 * Configure the NIC for required capabilities.
 */
static int ionic_set_hw_feature(struct lif *lif, uint16_t set_feature)
{
	struct ionic_admin_ctx ctx = {
		.work = COMPLETION_INITIALIZER_ONSTACK(ctx.work),
		.cmd.features = {
			.opcode = CMD_OPCODE_FEATURES,
			.set = FEATURE_SET_ETH_HW_FEATURES,
			.wanted = set_feature,
		},
	};
	int err;

	err = ionic_adminq_post_wait(lif, &ctx);
	if (err)
		return err;

	if (ctx.cmd.features.wanted != ctx.comp.features.supported)
		IONIC_NETDEV_ERROR(lif->netdev, "Feature wanted 0x%X != supported 0x%X\n", ctx.cmd.features.wanted, ctx.comp.features.supported);

	lif->hw_features = ctx.cmd.features.wanted &
			   ctx.comp.features.supported;

	if (lif->hw_features & ETH_HW_VLAN_TX_TAG)
		IONIC_NETDEV_INFO(lif->netdev, "feature ETH_HW_VLAN_TX_TAG\n");
	if (lif->hw_features & ETH_HW_VLAN_RX_STRIP)
		IONIC_NETDEV_INFO(lif->netdev, "feature ETH_HW_VLAN_RX_STRIP\n");
	if (lif->hw_features & ETH_HW_VLAN_RX_FILTER)
		IONIC_NETDEV_INFO(lif->netdev, "feature ETH_HW_VLAN_RX_FILTER\n");
	if (lif->hw_features & ETH_HW_RX_HASH)
		IONIC_NETDEV_INFO(lif->netdev, "feature ETH_HW_RX_HASH\n");
	if (lif->hw_features & ETH_HW_TX_SG)
		IONIC_NETDEV_INFO(lif->netdev, "feature ETH_HW_TX_SG\n");
	if (lif->hw_features & ETH_HW_TX_CSUM)
		IONIC_NETDEV_INFO(lif->netdev, "feature ETH_HW_TX_CSUM\n");
	if (lif->hw_features & ETH_HW_RX_CSUM)
		IONIC_NETDEV_INFO(lif->netdev, "feature ETH_HW_RX_CSUM\n");
	if (lif->hw_features & ETH_HW_TSO)
		IONIC_NETDEV_INFO(lif->netdev, "feature ETH_HW_TSO\n");
	if (lif->hw_features & ETH_HW_TSO_IPV6)
		IONIC_NETDEV_INFO(lif->netdev, "feature ETH_HW_TSO_IPV6\n");
	if (lif->hw_features & ETH_HW_TSO_ECN)
		IONIC_NETDEV_INFO(lif->netdev, "feature ETH_HW_TSO_ECN\n");
	if (lif->hw_features & ETH_HW_TSO_GRE)
		IONIC_NETDEV_INFO(lif->netdev, "feature ETH_HW_TSO_GRE\n");
	if (lif->hw_features & ETH_HW_TSO_GRE_CSUM)
		IONIC_NETDEV_INFO(lif->netdev, "feature ETH_HW_TSO_GRE_CSUM\n");
	if (lif->hw_features & ETH_HW_TSO_IPXIP4)
		IONIC_NETDEV_INFO(lif->netdev, "feature ETH_HW_TSO_IPXIP4\n");
	if (lif->hw_features & ETH_HW_TSO_IPXIP6)
		IONIC_NETDEV_INFO(lif->netdev, "feature ETH_HW_TSO_IPXIP6\n");
	if (lif->hw_features & ETH_HW_TSO_UDP)
		IONIC_NETDEV_INFO(lif->netdev, "feature ETH_HW_TSO_UDP\n");
	if (lif->hw_features & ETH_HW_TSO_UDP_CSUM)
		IONIC_NETDEV_INFO(lif->netdev, "feature ETH_HW_TSO_UDP_CSUM\n");

	return 0;
}

int ionic_set_features(struct lif *lif, uint16_t set_features)
{
	//struct net_device *netdev = lif->netdev;
	int err;

	IONIC_NETDEV_INFO(lif->netdev, "Setting capabilities: 0x%x\n", set_features);
	err = ionic_set_hw_feature(lif, set_features);
	if (err)
		return err;

#ifdef FREEBSD
	ionic_set_os_features(lif->netdev, lif->hw_features);
#else
	netdev->features |= NETIF_F_HIGHDMA;

	if (lif->hw_features & ETH_HW_VLAN_TX_TAG)
		netdev->hw_features |= NETIF_F_HW_VLAN_CTAG_TX;
	if (lif->hw_features & ETH_HW_VLAN_RX_STRIP)
		netdev->hw_features |= NETIF_F_HW_VLAN_CTAG_RX;
	if (lif->hw_features & ETH_HW_VLAN_RX_FILTER)
		netdev->hw_features |= NETIF_F_HW_VLAN_CTAG_FILTER;
	if (lif->hw_features & ETH_HW_RX_HASH)
		netdev->hw_features |= NETIF_F_RXHASH;
	if (lif->hw_features & ETH_HW_TX_SG)
		netdev->hw_features |= NETIF_F_SG;

	if (lif->hw_features & ETH_HW_TX_CSUM)
		netdev->hw_enc_features |= NETIF_F_HW_CSUM;
	if (lif->hw_features & ETH_HW_RX_CSUM)
		netdev->hw_enc_features |= NETIF_F_RXCSUM;
	if (lif->hw_features & ETH_HW_TSO)
		netdev->hw_enc_features |= NETIF_F_TSO;
	if (lif->hw_features & ETH_HW_TSO_IPV6)
		netdev->hw_enc_features |= NETIF_F_TSO6;
	if (lif->hw_features & ETH_HW_TSO_ECN)
		netdev->hw_enc_features |= NETIF_F_TSO_ECN;
	if (lif->hw_features & ETH_HW_TSO_GRE)
		netdev->hw_enc_features |= NETIF_F_GSO_GRE;
	if (lif->hw_features & ETH_HW_TSO_GRE_CSUM)
		netdev->hw_enc_features |= NETIF_F_GSO_GRE_CSUM;
	if (lif->hw_features & ETH_HW_TSO_IPXIP4)
		netdev->hw_enc_features |= NETIF_F_GSO_IPXIP4;
	if (lif->hw_features & ETH_HW_TSO_IPXIP6)
		netdev->hw_enc_features |= NETIF_F_GSO_IPXIP6;
	if (lif->hw_features & ETH_HW_TSO_UDP)
		netdev->hw_enc_features |= NETIF_F_GSO_UDP_TUNNEL;
	if (lif->hw_features & ETH_HW_TSO_UDP_CSUM)
		netdev->hw_enc_features |= NETIF_F_GSO_UDP_TUNNEL_CSUM;

	netdev->hw_features |= netdev->hw_enc_features;
	netdev->features |= netdev->hw_features;
	netdev->vlan_features |= netdev->features;
#endif

	return 0;
}

static int ionic_lif_register(struct lif *lif)
{
	int err;

	err = ionic_set_features(lif,
				 ETH_HW_VLAN_TX_TAG
				| ETH_HW_VLAN_RX_STRIP
				| ETH_HW_VLAN_RX_FILTER
				| ETH_HW_RX_HASH
				| ETH_HW_TX_SG
				| ETH_HW_TX_CSUM
				| ETH_HW_RX_CSUM
				| ETH_HW_TSO
				| ETH_HW_TSO_IPV6);

	if (err)
		return err;

	lif->registered = true;

	return 0;
}

int ionic_lifs_register(struct ionic *ionic)
{
	struct list_head *cur;
	struct lif *lif;
	int err;

	list_for_each(cur, &ionic->lifs) {
		lif = list_entry(cur, struct lif, list);
		err = ionic_lif_register(lif);
		if (err)
			return err;
	}

	return 0;
}

void ionic_lifs_unregister(struct ionic *ionic)
{
	struct list_head *cur;
	struct lif *lif;

	list_for_each(cur, &ionic->lifs) {
		lif = list_entry(cur, struct lif, list);
		if (lif->registered) {
			lif->registered = false;
		}
	}
}

int ionic_lifs_size(struct ionic *ionic)
{
	union identity *ident = ionic->ident;
	unsigned int nlifs = ident->dev.nlifs;
	unsigned int neqs = ident->dev.neqs_per_lif;
	/* Tx and Rx Qs are in pair. */
	int nqs = min(ident->dev.ntxqs_per_lif, ident->dev.nrxqs_per_lif);
	unsigned int nintrs, dev_nintrs = ident->dev.nintrs;
	int err;

	if (ionic_max_queues && (nqs < ionic_max_queues))
		nqs = ionic_max_queues;

	dev_info(ionic->dev, "dev_nintrs %u\n", dev_nintrs);
try_again:
#ifdef RSS
	/*
	 * Max number of Qs can't be more than number of RSS buckets,
	 * since those Qs will not get any traffic.
	 */
	if (nqs > rss_getnumbuckets()) {
		nqs = rss_getnumbuckets();
		dev_info(ionic->dev, "reduced number of Qs to %u based on RSS buckets\n", nqs);
	} 
#endif

	nintrs = nlifs * (neqs + 2 * nqs + 1 /* adminq */);
	if (nintrs > dev_nintrs) {
		goto try_fewer;
	}

	err = ionic_bus_alloc_irq_vectors(ionic, nintrs);
	if (err < 0 && err != -ENOSPC) {
		return err;
	}

	if (err == -ENOSPC)
		goto try_fewer;

	if (err != nintrs) {
		ionic_bus_free_irq_vectors(ionic);
		goto try_fewer;
	}

	ionic->neqs_per_lif = neqs;
	ionic->ntxqs_per_lif = nqs;
	ionic->nrxqs_per_lif = nqs;
	ionic->nintrs = nintrs;

	dev_info(ionic->dev, "dev_nintrs %d Tx/Rx Qs: %d\n", dev_nintrs, nqs);
	ionic_max_queues = nqs;

	return 0;
try_fewer:
	if (neqs > 1) {
		neqs /= 2;
		goto try_again;
	}
	if (nqs > 1) {
		nqs /= 2;
		goto try_again;
	}

	return -ENOSPC;
}
