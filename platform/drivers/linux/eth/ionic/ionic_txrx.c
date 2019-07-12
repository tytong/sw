// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2017 - 2019 Pensando Systems, Inc */

#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/if_vlan.h>
#include <net/ip6_checksum.h>
#include <linux/if_macvlan.h>

#include "ionic.h"
#include "ionic_lif.h"
#include "ionic_txrx.h"

static void ionic_tx_clean(struct queue *q, struct desc_info *desc_info,
			   struct cq_info *cq_info, void *cb_arg);
static void ionic_rx_clean(struct queue *q, struct desc_info *desc_info,
			   struct cq_info *cq_info, void *cb_arg);

static inline void ionic_txq_post(struct queue *q, bool ring_dbell,
				  desc_cb cb_func, void *cb_arg)
{
	DEBUG_STATS_TXQ_POST(q_to_qcq(q), q->head->desc, ring_dbell);

	ionic_q_post(q, ring_dbell, cb_func, cb_arg);
}

static inline void ionic_rxq_post(struct queue *q, bool ring_dbell,
				  desc_cb cb_func, void *cb_arg)
{
	ionic_q_post(q, ring_dbell, cb_func, cb_arg);

	DEBUG_STATS_RX_BUFF_CNT(q_to_qcq(q));
}

static void ionic_rx_recycle(struct queue *q, struct desc_info *desc_info,
			     struct sk_buff *skb)
{
	struct rxq_desc *old = desc_info->desc;
	struct rxq_desc *new = q->head->desc;

	new->addr = old->addr;
	new->len = old->len;

	ionic_rxq_post(q, true, ionic_rx_clean, skb);
}

static bool ionic_rx_copybreak(struct queue *q, struct desc_info *desc_info,
			       struct cq_info *cq_info, struct sk_buff **skb)
{
	struct net_device *netdev = q->lif->netdev;
	struct device *dev = q->lif->ionic->dev;
	struct rxq_desc *desc = desc_info->desc;
	struct rxq_comp *comp = cq_info->cq_desc;
	struct sk_buff *new_skb;
	u16 clen, dlen;

	clen = le16_to_cpu(comp->len);
	dlen = le16_to_cpu(desc->len);
	if (clen > q->lif->rx_copybreak) {
		dma_unmap_single(dev, (dma_addr_t)le64_to_cpu(desc->addr),
				 dlen, DMA_FROM_DEVICE);
		return false;
	}

	new_skb = netdev_alloc_skb_ip_align(netdev, clen);
	if (!new_skb) {
		dma_unmap_single(dev, (dma_addr_t)le64_to_cpu(desc->addr),
				 dlen, DMA_FROM_DEVICE);
		return false;
	}

	dma_sync_single_for_cpu(dev, (dma_addr_t)le64_to_cpu(desc->addr),
				clen, DMA_FROM_DEVICE);

	memcpy(new_skb->data, (*skb)->data, clen);

	ionic_rx_recycle(q, desc_info, *skb);
	*skb = new_skb;

	return true;
}

static void ionic_rx_clean(struct queue *q, struct desc_info *desc_info,
			   struct cq_info *cq_info, void *cb_arg)
{
	struct rxq_comp *comp = cq_info->cq_desc;
	struct sk_buff *skb = cb_arg;
	struct qcq *qcq = q_to_qcq(q);
	struct net_device *netdev;
	struct rx_stats *stats;
#ifdef CSUM_DEBUG
	__sum16 csum;
#endif

	stats = q_to_rx_stats(q);
	netdev = q->lif->netdev;

	if (comp->status) {
		// TODO record errors
		ionic_rx_recycle(q, desc_info, skb);
		return;
	}

	if (unlikely(test_bit(LIF_QUEUE_RESET, q->lif->state))) {
		/* no packet processing while resetting */
		ionic_rx_recycle(q, desc_info, skb);
		return;
	}

#ifdef CSUM_DEBUG
	if (le16_to_cpu(comp->len) > netdev->mtu + VLAN_ETH_HLEN) {
		netdev_warn(netdev, "RX PKT TOO LARGE!  comp->len %d\n",
			    le16_to_cpu(comp->len));
		ionic_rx_recycle(q, desc_info, skb);
		return;
	}
#endif

	stats->pkts++;
	stats->bytes += le16_to_cpu(comp->len);

	ionic_rx_copybreak(q, desc_info, cq_info, &skb);

	//prefetch(skb->data - NET_IP_ALIGN);

	skb_put(skb, le16_to_cpu(comp->len));
	skb->protocol = eth_type_trans(skb, netdev);
#ifdef CSUM_DEBUG
	csum = ip_compute_csum(skb->data, skb->len);
#endif

	if (is_master_lif(q->lif))
		skb_record_rx_queue(skb, q->index);
	else
		macvlan_count_rx(netdev_priv(netdev), skb->len + ETH_HLEN, true,
				 false);

	if (netdev->features & NETIF_F_RXHASH) {
		switch (comp->pkt_type_color & IONIC_RXQ_COMP_PKT_TYPE_MASK) {
		case PKT_TYPE_IPV4:
		case PKT_TYPE_IPV6:
			skb_set_hash(skb, le32_to_cpu(comp->rss_hash),
				     PKT_HASH_TYPE_L3);
			break;
		case PKT_TYPE_IPV4_TCP:
		case PKT_TYPE_IPV6_TCP:
		case PKT_TYPE_IPV4_UDP:
		case PKT_TYPE_IPV6_UDP:
			skb_set_hash(skb, le32_to_cpu(comp->rss_hash),
				     PKT_HASH_TYPE_L4);
			break;
		}
	}

	if (netdev->features & NETIF_F_RXCSUM) {
		if (comp->csum_flags & IONIC_RXQ_COMP_CSUM_F_CALC) {
			skb->ip_summed = CHECKSUM_COMPLETE;
			skb->csum = (__wsum)le16_to_cpu(comp->csum);
			stats->csum_complete++;
#ifdef CSUM_DEBUG
			if (skb->csum != (u16)~csum)
				netdev_warn(netdev, "Rx CSUM incorrect. Want 0x%04x got 0x%04x, protocol 0x%04x\n",
					    (u16)~csum, skb->csum,
					    htons(skb->protocol));
#endif
		}
	} else {
		stats->csum_none++;
	}

	if ((comp->csum_flags & IONIC_RXQ_COMP_CSUM_F_TCP_BAD) ||
	    (comp->csum_flags & IONIC_RXQ_COMP_CSUM_F_UDP_BAD) ||
	    (comp->csum_flags & IONIC_RXQ_COMP_CSUM_F_IP_BAD))
		stats->csum_error++;

	if (netdev->features & NETIF_F_HW_VLAN_CTAG_RX) {
		if (comp->csum_flags & IONIC_RXQ_COMP_CSUM_F_VLAN)
			__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q),
					       le16_to_cpu(comp->vlan_tci));
	}

	napi_gro_receive(&qcq->napi, skb);
}

static bool ionic_rx_service(struct cq *cq, struct cq_info *cq_info)
{
	struct rxq_comp *comp = cq_info->cq_desc;
	struct queue *q = cq->bound_q;
	struct desc_info *desc_info;

	if (!color_match(comp->pkt_type_color, cq->done_color))
		return false;

	/* check for empty queue */
	if (q->tail->index == q->head->index)
		return false;

	desc_info = q->tail;
	if (desc_info->index != le16_to_cpu(comp->comp_index))
		return false;

	q->tail = desc_info->next;

	/* clean the related q entry, only one per qc completion */
	ionic_rx_clean(q, desc_info, cq_info, desc_info->cb_arg);

	desc_info->cb = NULL;
	desc_info->cb_arg = NULL;

	return true;
}

static u32 ionic_rx_walk_cq(struct cq *rxcq, u32 limit)
{
	u32 work_done = 0;

	while (ionic_rx_service(rxcq, rxcq->tail)) {
		if (rxcq->tail->last)
			rxcq->done_color = !rxcq->done_color;
		rxcq->tail = rxcq->tail->next;
		DEBUG_STATS_CQE_CNT(rxcq);

		if (++work_done >= limit)
			break;
	}

	return work_done;
}

void ionic_rx_flush(struct cq *cq)
{
	struct ionic_dev *idev = &cq->lif->ionic->idev;
	u32 work_done;

	work_done = ionic_rx_walk_cq(cq, cq->num_descs);

	if (work_done)
		ionic_intr_credits(idev->intr_ctrl, cq->bound_intr->index,
				   work_done, IONIC_INTR_CRED_RESET_COALESCE);
}

void ionic_tx_flush(struct cq *cq)
{
	struct ionic_dev *idev = &cq->lif->ionic->idev;
	struct txq_comp *comp = cq->tail->cq_desc;
	struct queue *q = cq->bound_q;
	struct desc_info *desc_info;
	unsigned int work_done = 0;

	/* walk the completed cq entries */
	while (work_done < cq->num_descs &&
	       color_match(comp->color, cq->done_color)) {

		/* clean the related q entries, there could be
		 * several q entries completed for each cq completion
		 */
		do {
			desc_info = q->tail;
			q->tail = desc_info->next;
			ionic_tx_clean(q, desc_info, cq->tail,
				       desc_info->cb_arg);
			desc_info->cb = NULL;
			desc_info->cb_arg = NULL;
		} while (desc_info->index != le16_to_cpu(comp->comp_index));

		if (cq->tail->last)
			cq->done_color = !cq->done_color;

		cq->tail = cq->tail->next;
		comp = cq->tail->cq_desc;
		DEBUG_STATS_CQE_CNT(cq);

		work_done++;
	}

	if (work_done)
		ionic_intr_credits(idev->intr_ctrl, cq->bound_intr->index,
				   work_done, 0);
}

static struct sk_buff *ionic_rx_skb_alloc(struct queue *q, unsigned int len,
					  dma_addr_t *dma_addr)
{
	struct lif *lif = q->lif;
	struct net_device *netdev = lif->netdev;
	struct device *dev = lif->ionic->dev;
	struct rx_stats *stats;
	struct sk_buff *skb;

	stats = q_to_rx_stats(q);
	skb = netdev_alloc_skb_ip_align(netdev, len);
	if (!skb) {
		net_warn_ratelimited("%s: SKB alloc failed on %s!\n",
				     netdev->name, q->name);
		stats->alloc_err++;
		return NULL;
	}

	*dma_addr = dma_map_single(dev, skb->data, len, DMA_FROM_DEVICE);
	if (dma_mapping_error(dev, *dma_addr)) {
		dev_kfree_skb(skb);
		net_warn_ratelimited("%s: DMA single map failed on %s!\n",
				     netdev->name, q->name);
		stats->dma_map_err++;
		return NULL;
	}

	return skb;
}

static void ionic_rx_skb_free(struct queue *q, struct sk_buff *skb,
			      unsigned int len, dma_addr_t dma_addr)
{
	struct device *dev = q->lif->ionic->dev;

	dma_unmap_single(dev, dma_addr, len, DMA_FROM_DEVICE);
	dev_kfree_skb(skb);
}

#define RX_RING_DOORBELL_STRIDE		((1 << 2) - 1)

void ionic_rx_fill(struct queue *q)
{
	struct net_device *netdev = q->lif->netdev;
	struct rxq_desc *desc;
	struct sk_buff *skb;
	dma_addr_t dma_addr;
	bool ring_doorbell;
	unsigned int len;
	unsigned int i;

	len = netdev->mtu + ETH_HLEN;

	for (i = ionic_q_space_avail(q); i; i--) {
		skb = ionic_rx_skb_alloc(q, len, &dma_addr);
		if (!skb)
			return;

		desc = q->head->desc;
		desc->addr = cpu_to_le64(dma_addr);
		desc->len = cpu_to_le16(len);
		desc->opcode = RXQ_DESC_OPCODE_SIMPLE;

		ring_doorbell = ((q->head->index + 1) &
				RX_RING_DOORBELL_STRIDE) == 0;

		ionic_rxq_post(q, ring_doorbell, ionic_rx_clean, skb);
	}
}

static void ionic_rx_fill_cb(void *arg)
{
	ionic_rx_fill(arg);
}

void ionic_rx_empty(struct queue *q)
{
	struct desc_info *cur = q->tail;
	struct rxq_desc *desc;

	while (cur != q->head) {
		desc = cur->desc;

		ionic_rx_skb_free(q, cur->cb_arg, le16_to_cpu(desc->len),
				  le64_to_cpu(desc->addr));
		cur->cb_arg = NULL;

		cur = cur->next;
	}
}

int ionic_rx_napi(struct napi_struct *napi, int budget)
{
	struct qcq *qcq = napi_to_qcq(napi);
	struct cq *rxcq = napi_to_cq(napi);
	unsigned int qi = rxcq->bound_q->index;
	struct lif *lif = rxcq->bound_q->lif;
	struct ionic_dev *idev = &lif->ionic->idev;
	struct cq *txcq = &lif->txqcqs[qi].qcq->cq;
	u32 work_done = 0;
	u32 flags = 0;

	ionic_tx_flush(txcq);

	work_done = ionic_rx_walk_cq(rxcq, budget);

	if (work_done)
		ionic_rx_fill_cb(rxcq->bound_q);

	if (work_done < budget && napi_complete_done(napi, work_done)) {
		flags |= IONIC_INTR_CRED_UNMASK;
		DEBUG_STATS_INTR_REARM(rxcq->bound_intr);
	}

	if (work_done || flags) {
		flags |= IONIC_INTR_CRED_RESET_COALESCE;
		ionic_intr_credits(idev->intr_ctrl, rxcq->bound_intr->index,
				   work_done, flags);
	}

	DEBUG_STATS_NAPI_POLL(qcq, work_done);

	return work_done;
}

static dma_addr_t ionic_tx_map_single(struct queue *q, void *data, size_t len)
{
	struct tx_stats *stats = q_to_tx_stats(q);
	struct device *dev = q->lif->ionic->dev;
	dma_addr_t dma_addr;

	dma_addr = dma_map_single(dev, data, len, DMA_TO_DEVICE);
	if (dma_mapping_error(dev, dma_addr)) {
		net_warn_ratelimited("%s: DMA single map failed on %s!\n",
				     q->lif->netdev->name, q->name);
		stats->dma_map_err++;
		return 0;
	}
	return dma_addr;
}

static dma_addr_t ionic_tx_map_frag(struct queue *q, const skb_frag_t *frag,
				    size_t offset, size_t len)
{
	struct tx_stats *stats = q_to_tx_stats(q);
	struct device *dev = q->lif->ionic->dev;
	dma_addr_t dma_addr;

	dma_addr = skb_frag_dma_map(dev, frag, offset, len, DMA_TO_DEVICE);
	if (dma_mapping_error(dev, dma_addr)) {
		net_warn_ratelimited("%s: DMA frag map failed on %s!\n",
				     q->lif->netdev->name, q->name);
		stats->dma_map_err++;
		return 0;
	}
	return dma_addr;
}

static void ionic_tx_clean(struct queue *q, struct desc_info *desc_info,
			   struct cq_info *cq_info, void *cb_arg)
{
	struct txq_sg_desc *sg_desc = desc_info->sg_desc;
	struct txq_sg_elem *elem = sg_desc->elems;
	struct tx_stats *stats = q_to_tx_stats(q);
	struct txq_desc *desc = desc_info->desc;
	struct device *dev = q->lif->ionic->dev;
	struct sk_buff *skb = cb_arg;
	u8 opcode, flags, nsge;
	u16 queue_index;
	unsigned int i;
	u64 addr;

	decode_txq_desc_cmd(le64_to_cpu(desc->cmd),
			    &opcode, &flags, &nsge, &addr);

	dma_unmap_page(dev, (dma_addr_t)addr,
		       le16_to_cpu(desc->len), DMA_TO_DEVICE);
	for (i = 0; i < nsge; i++, elem++)
		dma_unmap_page(dev, (dma_addr_t)le64_to_cpu(elem->addr),
			       le16_to_cpu(elem->len), DMA_TO_DEVICE);

	if (skb) {
		queue_index = skb_get_queue_mapping(skb);
		if (unlikely(__netif_subqueue_stopped(q->lif->netdev,
						      queue_index))) {
			netif_wake_subqueue(q->lif->netdev, queue_index);
			q->wake++;
		}
		dev_kfree_skb_any(skb);
		stats->clean++;
	}
}

static void ionic_tx_tcp_inner_pseudo_csum(struct sk_buff *skb)
{
	skb_cow_head(skb, 0); // TODO is this necessary before modifying hdrs?

	if (skb->protocol == cpu_to_be16(ETH_P_IP)) {
		inner_ip_hdr(skb)->check = 0;
		inner_tcp_hdr(skb)->check =
			~csum_tcpudp_magic(inner_ip_hdr(skb)->saddr,
					   inner_ip_hdr(skb)->daddr,
					   0, IPPROTO_TCP, 0);
	} else if (skb->protocol == cpu_to_be16(ETH_P_IPV6)) {
		inner_tcp_hdr(skb)->check =
			~csum_ipv6_magic(&inner_ipv6_hdr(skb)->saddr,
					 &inner_ipv6_hdr(skb)->daddr,
					 0, IPPROTO_TCP, 0);
	}
}

static void ionic_tx_tcp_pseudo_csum(struct sk_buff *skb)
{
	skb_cow_head(skb, 0); // TODO is this necessary before modifying hdrs?

	if (skb->protocol == cpu_to_be16(ETH_P_IP)) {
		ip_hdr(skb)->check = 0;
		tcp_hdr(skb)->check =
			~csum_tcpudp_magic(ip_hdr(skb)->saddr,
					   ip_hdr(skb)->daddr,
					   0, IPPROTO_TCP, 0);
	} else if (skb->protocol == cpu_to_be16(ETH_P_IPV6)) {
		tcp_hdr(skb)->check =
			~csum_ipv6_magic(&ipv6_hdr(skb)->saddr,
					 &ipv6_hdr(skb)->daddr,
					 0, IPPROTO_TCP, 0);
	}
}

static void ionic_tx_tso_post(struct queue *q, struct txq_desc *desc,
			      struct sk_buff *skb,
			      dma_addr_t addr, u8 nsge, u16 len,
			      unsigned int hdrlen, unsigned int mss,
			      bool outer_csum,
			      u16 vlan_tci, bool has_vlan,
			      bool start, bool done)
{
	u8 flags = 0;
	u64 cmd;

	flags |= has_vlan ? IONIC_TXQ_DESC_FLAG_VLAN : 0;
	flags |= outer_csum ? IONIC_TXQ_DESC_FLAG_ENCAP : 0;
	flags |= start ? IONIC_TXQ_DESC_FLAG_TSO_SOT : 0;
	flags |= done ? IONIC_TXQ_DESC_FLAG_TSO_EOT : 0;

	cmd = encode_txq_desc_cmd(IONIC_TXQ_DESC_OPCODE_TSO, flags, nsge, addr);
	desc->cmd = cpu_to_le64(cmd);
	desc->len = cpu_to_le16(len);
	desc->vlan_tci = cpu_to_le16(vlan_tci);
	desc->hdr_len = cpu_to_le16(hdrlen);
	desc->mss = cpu_to_le16(mss);

	if (done) {
		skb_tx_timestamp(skb);
#ifdef HAVE_NETDEV_XMIT_MORE
		ionic_txq_post(q, !netdev_xmit_more(), ionic_tx_clean, skb);
#elif defined HAVE_SKB_XMIT_MORE
		ionic_txq_post(q, !skb->xmit_more, ionic_tx_clean, skb);
#else
		ionic_txq_post(q, true, ionic_tx_clean, skb);
#endif
	} else {
		ionic_txq_post(q, false, ionic_tx_clean, NULL);
	}
}

static struct txq_desc *ionic_tx_tso_next(struct queue *q,
					  struct txq_sg_elem **elem)
{
	struct txq_sg_desc *sg_desc = q->head->sg_desc;
	struct txq_desc *desc = q->head->desc;

	*elem = sg_desc->elems;
	return desc;
}

static int ionic_tx_tso(struct queue *q, struct sk_buff *skb)
{
	struct tx_stats *stats = q_to_tx_stats(q);
	struct desc_info *abort = q->head;
	struct desc_info *rewind = abort;
	unsigned int frag_left = 0;
	struct txq_sg_elem *elem;
	unsigned int offset = 0;
	unsigned int len_left;
	struct txq_desc *desc;
	dma_addr_t desc_addr;
	unsigned int hdrlen;
	unsigned int nfrags;
	unsigned int seglen;
	u64 total_bytes = 0;
	u64 total_pkts = 0;
	unsigned int left;
	unsigned int len;
	unsigned int mss;
	skb_frag_t *frag;
	bool start, done;
	bool outer_csum;
	bool has_vlan;
	u16 desc_len;
	u8 desc_nsge;
	u16 vlan_tci;
	bool encap;

	mss = skb_shinfo(skb)->gso_size;
	nfrags = skb_shinfo(skb)->nr_frags;
	len_left = skb->len - skb_headlen(skb);
	outer_csum = (skb_shinfo(skb)->gso_type & SKB_GSO_GRE_CSUM) ||
		     (skb_shinfo(skb)->gso_type & SKB_GSO_UDP_TUNNEL_CSUM);
	has_vlan = !!skb_vlan_tag_present(skb);
	vlan_tci = skb_vlan_tag_get(skb);
	encap = skb->encapsulation;

	/* Preload inner-most TCP csum field with IP pseudo hdr
	 * calculated with IP length set to zero.  HW will later
	 * add in length to each TCP segment resulting from the TSO.
	 */

	if (encap)
		ionic_tx_tcp_inner_pseudo_csum(skb);
	else
		ionic_tx_tcp_pseudo_csum(skb);

	if (encap)
		hdrlen = skb_inner_transport_header(skb) - skb->data +
			 inner_tcp_hdrlen(skb);
	else
		hdrlen = skb_transport_offset(skb) + tcp_hdrlen(skb);

	seglen = hdrlen + mss;
	left = skb_headlen(skb);

	desc = ionic_tx_tso_next(q, &elem);
	start = true;

	/* Chop skb->data up into desc segments */

	while (left > 0) {
		len = min(seglen, left);
		frag_left = seglen - len;
		desc_addr = ionic_tx_map_single(q, skb->data + offset, len);
		if (!desc_addr)
			goto err_out_abort;
		desc_len = len;
		desc_nsge = 0;
		left -= len;
		offset += len;
		if (nfrags > 0 && frag_left > 0)
			continue;
		done = (nfrags == 0 && left == 0);
		ionic_tx_tso_post(q, desc, skb,
				  desc_addr, desc_nsge, desc_len,
				  hdrlen, mss,
				  outer_csum,
				  vlan_tci, has_vlan,
				  start, done);
		total_pkts++;
		total_bytes += start ? len : len + hdrlen;
		desc = ionic_tx_tso_next(q, &elem);
		start = false;
		seglen = mss;
	}

	/* Chop skb frags into desc segments */

	for (frag = skb_shinfo(skb)->frags; len_left; frag++) {
		offset = 0;
		left = skb_frag_size(frag);
		len_left -= left;
		nfrags--;
		stats->frags++;

		while (left > 0) {
			if (frag_left > 0) {
				len = min(frag_left, left);
				frag_left -= len;
				elem->addr =
				    cpu_to_le64(ionic_tx_map_frag(q, frag,
								  offset, len));
				if (!elem->addr)
					goto err_out_abort;
				elem->len = cpu_to_le16(len);
				elem++;
				desc_nsge++;
				left -= len;
				offset += len;
				if (nfrags > 0 && frag_left > 0)
					continue;
				done = (nfrags == 0 && left == 0);
				ionic_tx_tso_post(q, desc, skb, desc_addr,
						  desc_nsge, desc_len,
						  hdrlen, mss, outer_csum,
						  vlan_tci, has_vlan,
						  start, done);
				total_pkts++;
				total_bytes += start ? len : len + hdrlen;
				desc = ionic_tx_tso_next(q, &elem);
				start = false;
			} else {
				len = min(mss, left);
				frag_left = mss - len;
				desc_addr = ionic_tx_map_frag(q, frag,
							      offset, len);
				if (!desc_addr)
					goto err_out_abort;
				desc_len = len;
				desc_nsge = 0;
				left -= len;
				offset += len;
				if (nfrags > 0 && frag_left > 0)
					continue;
				done = (nfrags == 0 && left == 0);
				ionic_tx_tso_post(q, desc, skb, desc_addr,
						  desc_nsge, desc_len,
						  hdrlen, mss, outer_csum,
						  vlan_tci, has_vlan,
						  start, done);
				total_pkts++;
				total_bytes += start ? len : len + hdrlen;
				desc = ionic_tx_tso_next(q, &elem);
				start = false;
			}
		}
	}

	stats->pkts += total_pkts;
	stats->bytes += total_bytes;
	stats->tso++;

	return 0;

err_out_abort:
	while (rewind->desc != q->head->desc) {
		ionic_tx_clean(q, rewind, NULL, NULL);
		rewind = rewind->next;
	}
	q->head = abort;

	return -ENOMEM;
}

static int ionic_tx_calc_csum(struct queue *q, struct sk_buff *skb)
{
	struct tx_stats *stats = q_to_tx_stats(q);
	struct txq_desc *desc = q->head->desc;
	dma_addr_t addr;
	bool has_vlan;
	u8 flags = 0;
	bool encap;
	u64 cmd;

	has_vlan = !!skb_vlan_tag_present(skb);
	encap = skb->encapsulation;

	addr = ionic_tx_map_single(q, skb->data, skb_headlen(skb));
	if (!addr)
		return -ENOMEM;

	flags |= has_vlan ? IONIC_TXQ_DESC_FLAG_VLAN : 0;
	flags |= encap ? IONIC_TXQ_DESC_FLAG_ENCAP : 0;

	cmd = encode_txq_desc_cmd(IONIC_TXQ_DESC_OPCODE_CSUM_PARTIAL,
				  flags, skb_shinfo(skb)->nr_frags, addr);
	desc->cmd = cpu_to_le64(cmd);
	desc->len = cpu_to_le16(skb_headlen(skb));
	desc->vlan_tci = cpu_to_le16(skb_vlan_tag_get(skb));
	desc->csum_start = cpu_to_le16(skb_checksum_start_offset(skb));
	desc->csum_offset = cpu_to_le16(skb->csum_offset);

#ifdef HAVE_CSUM_NOT_INET
	if (skb->csum_not_inet)
		stats->crc32_csum++;
	else
#endif
		stats->csum++;

	return 0;
}

static int ionic_tx_calc_no_csum(struct queue *q, struct sk_buff *skb)
{
	struct tx_stats *stats = q_to_tx_stats(q);
	struct txq_desc *desc = q->head->desc;
	dma_addr_t addr;
	bool has_vlan;
	u8 flags = 0;
	bool encap;
	u64 cmd;

	has_vlan = !!skb_vlan_tag_present(skb);
	encap = skb->encapsulation;

	addr = ionic_tx_map_single(q, skb->data, skb_headlen(skb));
	if (!addr)
		return -ENOMEM;

	flags |= has_vlan ? IONIC_TXQ_DESC_FLAG_VLAN : 0;
	flags |= encap ? IONIC_TXQ_DESC_FLAG_ENCAP : 0;

	cmd = encode_txq_desc_cmd(IONIC_TXQ_DESC_OPCODE_CSUM_NONE,
				  flags, skb_shinfo(skb)->nr_frags, addr);
	desc->cmd = cpu_to_le64(cmd);
	desc->len = cpu_to_le16(skb_headlen(skb));
	desc->vlan_tci = cpu_to_le16(skb_vlan_tag_get(skb));

	stats->no_csum++;

	return 0;
}

static int ionic_tx_skb_frags(struct queue *q, struct sk_buff *skb)
{
	unsigned int len_left = skb->len - skb_headlen(skb);
	struct txq_sg_desc *sg_desc = q->head->sg_desc;
	struct txq_sg_elem *elem = sg_desc->elems;
	struct tx_stats *stats = q_to_tx_stats(q);
	dma_addr_t dma_addr;
	skb_frag_t *frag;
	u16 len;

	for (frag = skb_shinfo(skb)->frags; len_left; frag++, elem++) {
		len = skb_frag_size(frag);
		elem->len = cpu_to_le16(len);
		dma_addr = ionic_tx_map_frag(q, frag, 0, len);
		if (!dma_addr)
			return -ENOMEM;
		elem->addr = cpu_to_le64(dma_addr);
		len_left -= len;
		stats->frags++;
	}

	return 0;
}

static int ionic_tx(struct queue *q, struct sk_buff *skb)
{
	struct tx_stats *stats = q_to_tx_stats(q);
	int err;

	if (skb->ip_summed == CHECKSUM_PARTIAL)
		err = ionic_tx_calc_csum(q, skb);
	else
		err = ionic_tx_calc_no_csum(q, skb);
	if (err)
		return err;

	err = ionic_tx_skb_frags(q, skb);
	if (err)
		return err;

	skb_tx_timestamp(skb);
	stats->pkts++;
	stats->bytes += skb->len;

#ifdef HAVE_NETDEV_XMIT_MORE
	ionic_txq_post(q, !netdev_xmit_more(), ionic_tx_clean, skb);
#elif defined HAVE_SKB_XMIT_MORE
	ionic_txq_post(q, !skb->xmit_more, ionic_tx_clean, skb);
#else
	ionic_txq_post(q, true, ionic_tx_clean, skb);
#endif

	return 0;
}

static int ionic_tx_descs_needed(struct queue *q, struct sk_buff *skb)
{
	struct tx_stats *stats = q_to_tx_stats(q);
	int err;

	/* If TSO, need roundup(skb->len/mss) descs */
	if (skb_is_gso(skb))
		return (skb->len / skb_shinfo(skb)->gso_size) + 1;

	/* If non-TSO, just need 1 desc and nr_frags sg elems */
	if (skb_shinfo(skb)->nr_frags <= IONIC_TX_MAX_SG_ELEMS)
		return 1;

	/* Too many frags, so linearize */
	err = skb_linearize(skb);
	if (err)
		return err;

	stats->linearize++;

	/* Need 1 desc and zero sg elems */
	return 1;
}

#ifndef HAVE_NDO_SELECT_QUEUE_SB_DEV
u16 ionic_select_queue(struct net_device *netdev, struct sk_buff *skb,
			void *accel_priv, select_queue_fallback_t fallback)
{
	u16 index;

	if (netdev->features & NETIF_F_HW_L2FW_DOFFLOAD) {
		if (accel_priv) {
			struct lif *lif = (struct lif *)accel_priv;
			struct lif *master_lif = lif->ionic->master_lif;

			index = master_lif->nxqs + lif->index - 1;
		} else {
			struct lif *lif = (struct lif *)netdev_priv(netdev);

			index = lif->index;
		}
	} else {
		index = fallback(netdev, skb);
	}

	return index;
}
#endif

netdev_tx_t ionic_start_xmit(struct sk_buff *skb, struct net_device *netdev)
{
	u16 queue_index = skb_get_queue_mapping(skb);
	struct lif *lif = netdev_priv(netdev);
	struct queue *q;
	int ndescs;
	int err;

	if (unlikely(!test_bit(LIF_UP, lif->state))) {
		dev_kfree_skb(skb);
		return NETDEV_TX_OK;
	}

	if (likely(lif_to_txqcq(lif, queue_index)))
		q = lif_to_txq(lif, queue_index);
	else
		q = lif_to_txq(lif, 0);

	ndescs = ionic_tx_descs_needed(q, skb);
	if (ndescs < 0)
		goto err_out_drop;

	if (!ionic_q_has_space(q, ndescs)) {
		netif_stop_subqueue(netdev, queue_index);
		q->stop++;

		/* Might race with ionic_tx_clean, check again */
		smp_rmb();
		if (ionic_q_has_space(q, ndescs)) {
			netif_wake_subqueue(netdev, queue_index);
			q->wake++;
		} else {
			return NETDEV_TX_BUSY;
		}
	}

	if (skb_is_gso(skb))
		err = ionic_tx_tso(q, skb);
	else
		err = ionic_tx(q, skb);

	if (err)
		goto err_out_drop;

	return NETDEV_TX_OK;

err_out_drop:
	netif_stop_subqueue(netdev, queue_index);
	q->stop++;
	q->drop++;
	dev_kfree_skb(skb);
	return NETDEV_TX_OK;
}
