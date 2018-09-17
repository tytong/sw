/*
 * {C} Copyright 2018 Pensando Systems Inc.
 * All rights reserved.
 *
 */
#include <netdevice.h>
#include <net.h>
#include <kernel.h>

#include "sonic_dev.h"
#include "sonic_lif.h"
#include "sonic_api_int.h"

#include "osal.h"
#include "pnso_api.h"

#include "pnso_utils.h"
#include "pnso_chain_params.h"
#include "pnso_seq_ops.h"
#include "pnso_cpdc.h"

/**
 * TODO-seq:
 *	- although chaining can be done generically for compression
 *	related chains, focus for now is comp+hash bring-up.
 *	- revisit layer violations
 *	- storage_seq_p4pd. vs utils.h
 *
 */
#ifdef NDEBUG
#define PPRINT_SEQUENCER_DESC(d)
#define PPRINT_SEQUENCER_INFO(sqi)
#define PPRINT_CPDC_CHAIN_PARAMS(cp)
#define PPRINT_XTS_CHAIN_PARAMS(cp)
#else
#define PPRINT_SEQUENCER_DESC(d)	pprint_seq_desc(d)
#define PPRINT_SEQUENCER_INFO(sqi)					\
	do {								\
		OSAL_LOG_INFO("%.*s", 30, "=========================================");\
		pprint_seq_info(sqi);					\
	} while (0)
#define PPRINT_CPDC_CHAIN_PARAMS(cp)					\
	do {								\
		OSAL_LOG_INFO("%.*s", 30, "=========================================");\
		pprint_cpdc_chain_params(cp);				\
	} while (0)
#define PPRINT_XTS_CHAIN_PARAMS(cp)					\
	do {								\
		OSAL_LOG_INFO("%.*s", 30, "=========================================");\
		pprint_xts_chain_params(cp);				\
	} while (0)
#endif

#define kDbAddrCapri	(0x8800000)
#define kDbAddrUpdate	(0xB)
#define kDbUpdateShift	(17)
#define kDbLifShift	(6)
#define kDbTypeShift	(3)

extern uint64_t pad_buffer;

static void __attribute__((unused))
pprint_seq_desc(const struct sequencer_desc *desc)
{
	if (!desc)
		return;

	OSAL_LOG_INFO("%30s: 0x%llx", "seq_desc", (uint64_t) desc);
	OSAL_LOG_INFO("%30s: 0x%llx", "seq_desc_pa",
			osal_virt_to_phy((void *) desc));

	OSAL_LOG_INFO("%30s: 0x%llx", "sd_desc_addr", desc->sd_desc_addr);
	OSAL_LOG_INFO("%30s: 0x%llx", "sd_pndx_addr", desc->sd_pndx_addr);
	OSAL_LOG_INFO("%30s: 0x%llx", "sd_pndx_shadow_addr",
			desc->sd_pndx_shadow_addr);
	OSAL_LOG_INFO("%30s: 0x%llx", "sd_ring_addr", desc->sd_ring_addr);

	OSAL_LOG_INFO("%30s: %d", "sd_desc_size", desc->sd_desc_size);
	OSAL_LOG_INFO("%30s: %d", "sd_pndx_size", desc->sd_pndx_size);
	OSAL_LOG_INFO("%30s: %d", "sd_ring_size", desc->sd_ring_size);
	OSAL_LOG_INFO("%30s: %d", "sd_batch_mode", desc->sd_batch_mode);
	OSAL_LOG_INFO("%30s: %d", "sd_batch_size", desc->sd_batch_size);
}

static void __attribute__((unused))
pprint_seq_info(const struct sequencer_info *seq_info)
{
	if (!seq_info)
		return;

	OSAL_LOG_INFO("%30s: %d", "sqi_ring_id", seq_info->sqi_ring_id);
	OSAL_LOG_INFO("%30s: %d", "sqi_qtype", seq_info->sqi_qtype);
	OSAL_LOG_INFO("%30s: %d", "sqi_index", seq_info->sqi_index);
	OSAL_LOG_INFO("%30s: %d", "sqi_batch_mode", seq_info->sqi_batch_mode);
	OSAL_LOG_INFO("%30s: %d", "sqi_batch_size", seq_info->sqi_batch_size);
	OSAL_LOG_INFO("%30s: 0x%llx", "sqi_sqi_desc", (u64) seq_info->sqi_desc);
	OSAL_LOG_INFO("%30s: 0x%llx", "sqi_status_desc",
			(u64) seq_info->sqi_status_desc);
}

static void __attribute__((unused))
pprint_cpdc_chain_params(const struct cpdc_chain_params *chain_params)
{
	const struct sequencer_spec *spec;
	const struct barco_spec *barco_spec;
	const struct cpdc_chain_params_command *cmd;

	if (!chain_params)
		return;

	OSAL_LOG_INFO("%30s: 0x%llx", "cpdc_chain_params",
			(uint64_t) chain_params);

	spec = &chain_params->ccp_seq_spec;
	OSAL_LOG_INFO("%30s: 0x%llx", "sequencer_spec", (uint64_t) spec);
	OSAL_LOG_INFO("%30s: 0x%llx", "sqs_seq_q",
			spec->sqs_seq_q);
	OSAL_LOG_INFO("%30s: 0x%llx", "sqs_seq_status_q",
			spec->sqs_seq_status_q);
	OSAL_LOG_INFO("%30s: 0x%llx", "sqs_seq_next_q",
			spec->sqs_seq_next_q);
	OSAL_LOG_INFO("%30s: 0x%llx", "sqs_seq_next_status_q",
			spec->sqs_seq_next_status_q);
	OSAL_LOG_INFO("%30s: 0x%llx", "sqs_ret_doorbell_addr",
			spec->sqs_ret_doorbell_addr);
	OSAL_LOG_INFO("%30s: 0x%llx", "sqs_ret_doorbell_data",
			spec->sqs_ret_doorbell_data);
	OSAL_LOG_INFO("%30s: %d", "sqs_ret_seq_status_index",
			spec->sqs_ret_seq_status_index);

	barco_spec = &chain_params->ccp_barco_spec;
	OSAL_LOG_INFO("%30s: 0x%llx", "barco_spec", (uint64_t) barco_spec);
	OSAL_LOG_INFO("%30s: 0x%llx", "bs_ring_addr",
			barco_spec->bs_ring_addr);
	OSAL_LOG_INFO("%30s: 0x%llx", "bs_pndx_addr",
			barco_spec->bs_pndx_addr);
	OSAL_LOG_INFO("%30s: 0x%llx", "bs_pndx_shadow_addr",
			barco_spec->bs_pndx_shadow_addr);
	OSAL_LOG_INFO("%30s: 0x%llx", "bs_desc_addr",
			barco_spec->bs_desc_addr);
	OSAL_LOG_INFO("%30s: %d", "bs_desc_size",
			barco_spec->bs_desc_size);
	OSAL_LOG_INFO("%30s: %d", "bs_pndx_size",
			barco_spec->bs_pndx_size);
	OSAL_LOG_INFO("%30s: %d", "bs_ring_size",
			barco_spec->bs_ring_size);
	OSAL_LOG_INFO("%30s: %d", "bs_num_descs",
			barco_spec->bs_num_descs);

	OSAL_LOG_INFO("%30s: 0x%llx", "ccp_status_addr_0",
			chain_params->ccp_status_addr_0);
	OSAL_LOG_INFO("%30s: 0x%llx", "ccp_status_addr_1",
			chain_params->ccp_status_addr_1);
	OSAL_LOG_INFO("%30s: 0x%llx", "ccp_comp_buf_addr",
			chain_params->ccp_comp_buf_addr);
	OSAL_LOG_INFO("%30s: 0x%llx", "ccp_alt_buf_addr",
			chain_params->ccp_alt_buf_addr);

	OSAL_LOG_INFO("%30s: 0x%llx", "ccp_aol_src_vec_addr",
			chain_params->ccp_aol_src_vec_addr);
	OSAL_LOG_INFO("%30s: 0x%llx", "ccp_aol_dst_vec_addr",
			chain_params->ccp_aol_dst_vec_addr);
	OSAL_LOG_INFO("%30s: 0x%llx", "ccp_sgl_vec_addr",
			chain_params->ccp_sgl_vec_addr);
	OSAL_LOG_INFO("%30s: 0x%llx", "ccp_pad_buf_addr",
			chain_params->ccp_pad_buf_addr);

	OSAL_LOG_INFO("%30s: 0x%llx", "ccp_intr_addr",
			chain_params->ccp_intr_addr);
	OSAL_LOG_INFO("%30s: %d", "ccp_intr_data",
			chain_params->ccp_intr_data);
	OSAL_LOG_INFO("%30s: %d", "ccp_status_len",
			chain_params->ccp_status_len);
	OSAL_LOG_INFO("%30s: %d", "ccp_data_len",
			chain_params->ccp_data_len);

	OSAL_LOG_INFO("%30s: %d", "ccp_status_offset_0",
			chain_params->ccp_status_offset_0);
	OSAL_LOG_INFO("%30s: %d", "ccp_pad_boundary_shift",
			chain_params->ccp_pad_boundary_shift);

	cmd = &chain_params->ccp_cmd;
	OSAL_LOG_INFO("%30s: %d", "ccpc_data_len_from_desc",
			cmd->ccpc_data_len_from_desc);
	OSAL_LOG_INFO("%30s: %d", "ccpc_status_dma_en",
			cmd->ccpc_status_dma_en);
	OSAL_LOG_INFO("%30s: %d", "ccpc_next_doorbell_en",
			cmd->ccpc_next_doorbell_en);
	OSAL_LOG_INFO("%30s: %d", "ccpc_intr_en",
			cmd->ccpc_intr_en);
	OSAL_LOG_INFO("%30s: %d", "ccpc_next_db_action_barco_push",
			cmd->ccpc_next_db_action_barco_push);
	OSAL_LOG_INFO("%30s: %d", "ccpc_stop_chain_on_error",
			cmd->ccpc_stop_chain_on_error);
	OSAL_LOG_INFO("%30s: %d", "ccpc_chain_alt_desc_on_error",
			cmd->ccpc_chain_alt_desc_on_error);
	OSAL_LOG_INFO("%30s: %d", "ccpc_aol_pad_en",
			cmd->ccpc_aol_pad_en);
	OSAL_LOG_INFO("%30s: %d", "ccpc_sgl_pad_en",
			cmd->ccpc_sgl_pad_en);
	OSAL_LOG_INFO("%30s: %d", "ccpc_sgl_sparse_format_en",
			cmd->ccpc_sgl_sparse_format_en);
	OSAL_LOG_INFO("%30s: %d", "ccpc_sgl_pdma_en",
			cmd->ccpc_sgl_pdma_en);
	OSAL_LOG_INFO("%30s: %d", "ccpc_sgl_pdma_pad_only",
			cmd->ccpc_sgl_pdma_pad_only);
	OSAL_LOG_INFO("%30s: %d", "ccpc_sgl_pdma_alt_src_on_error",
			cmd->ccpc_sgl_pdma_alt_src_on_error);
	OSAL_LOG_INFO("%30s: %d", "ccpc_desc_vec_push_en",
			cmd->ccpc_desc_vec_push_en);
}

static void __attribute__((unused))
pprint_xts_chain_params(const struct xts_chain_params *chain_params)
{
	const struct sequencer_spec *spec;
	const struct barco_spec *barco_spec;
	const struct xts_chain_params_command *cmd;

	if (!chain_params)
		return;

	OSAL_LOG_INFO("%30s: 0x%llx", "xts_chain_params",
			(uint64_t) chain_params);

	spec = &chain_params->xcp_seq_spec;
	OSAL_LOG_INFO("%30s: 0x%llx", "sequencer_spec", (uint64_t) spec);
	OSAL_LOG_INFO("%30s: 0x%llx", "sqs_seq_q",
			spec->sqs_seq_q);
	OSAL_LOG_INFO("%30s: 0x%llx", "sqs_seq_status_q",
			spec->sqs_seq_status_q);
	OSAL_LOG_INFO("%30s: 0x%llx", "sqs_seq_next_q",
			spec->sqs_seq_next_q);
	OSAL_LOG_INFO("%30s: 0x%llx", "sqs_seq_next_status_q",
			spec->sqs_seq_next_status_q);
	OSAL_LOG_INFO("%30s: 0x%llx", "sqs_ret_doorbell_addr",
			spec->sqs_ret_doorbell_addr);
	OSAL_LOG_INFO("%30s: 0x%llx", "sqs_ret_doorbell_data",
			spec->sqs_ret_doorbell_data);
	OSAL_LOG_INFO("%30s: %d", "sqs_ret_seq_status_index",
			spec->sqs_ret_seq_status_index);

	barco_spec = &chain_params->xcp_barco_spec;
	OSAL_LOG_INFO("%30s: 0x%llx", "barco_spec", (uint64_t) barco_spec);
	OSAL_LOG_INFO("%30s: 0x%llx", "bs_ring_addr",
			barco_spec->bs_ring_addr);
	OSAL_LOG_INFO("%30s: 0x%llx", "bs_pndx_addr",
			barco_spec->bs_pndx_addr);
	OSAL_LOG_INFO("%30s: 0x%llx", "bs_pndx_shadow_addr",
			barco_spec->bs_pndx_shadow_addr);
	OSAL_LOG_INFO("%30s: 0x%llx", "bs_desc_addr",
			barco_spec->bs_desc_addr);
	OSAL_LOG_INFO("%30s: %d", "bs_desc_size",
			barco_spec->bs_desc_size);
	OSAL_LOG_INFO("%30s: %d", "bs_pndx_size",
			barco_spec->bs_pndx_size);
	OSAL_LOG_INFO("%30s: %d", "bs_ring_size",
			barco_spec->bs_ring_size);
	OSAL_LOG_INFO("%30s: %d", "bs_num_descs",
			barco_spec->bs_num_descs);

	OSAL_LOG_INFO("%30s: 0x%llx", "xcp_status_addr_0",
			chain_params->xcp_status_addr_0);
	OSAL_LOG_INFO("%30s: 0x%llx", "xcp_status_addr_1",
			chain_params->xcp_status_addr_1);
	OSAL_LOG_INFO("%30s: 0x%llx", "xcp_decr_buf_addr",
			chain_params->xcp_decr_buf_addr);
	OSAL_LOG_INFO("%30s: 0x%llx", "xcp_comp_sgl_src_addr",
			chain_params->xcp_comp_sgl_src_addr);
	OSAL_LOG_INFO("%30s: 0x%llx", "xcp_sgl_pdma_dst_addr",
			chain_params->xcp_sgl_pdma_dst_addr);

	OSAL_LOG_INFO("%30s: 0x%llx", "xcp_intr_addr",
			chain_params->xcp_intr_addr);
	OSAL_LOG_INFO("%30s: %d", "xcp_intr_data",
			chain_params->xcp_intr_data);

	OSAL_LOG_INFO("%30s: %d", "xcp_status_offset_0",
			chain_params->xcp_status_offset_0);
	OSAL_LOG_INFO("%30s: %d", "xcp_blk_boundary_shift",
			chain_params->xcp_blk_boundary_shift);

	cmd = &chain_params->xcp_cmd;
	OSAL_LOG_INFO("%30s: %d", "xcpc_status_dma_en",
			cmd->xcpc_status_dma_en);
	OSAL_LOG_INFO("%30s: %d", "xcpc_next_doorbell_en",
			cmd->xcpc_next_doorbell_en);
	OSAL_LOG_INFO("%30s: %d", "xcpc_intr_en",
			cmd->xcpc_intr_en);
	OSAL_LOG_INFO("%30s: %d", "xcpc_next_db_action_barco_push",
			cmd->xcpc_next_db_action_barco_push);
	OSAL_LOG_INFO("%30s: %d", "xcpc_stop_chain_on_error",
			cmd->xcpc_stop_chain_on_error);
	OSAL_LOG_INFO("%30s: %d", "xcpc_comp_len_update_en",
			cmd->xcpc_comp_len_update_en);
	OSAL_LOG_INFO("%30s: %d", "xcpc_comp_sgl_src_en",
			cmd->xcpc_comp_sgl_src_en);
	OSAL_LOG_INFO("%30s: %d", "xcpc_comp_sgl_src_vec_en",
			cmd->xcpc_comp_sgl_src_vec_en);
	OSAL_LOG_INFO("%30s: %d", "xcpc_sgl_sparse_format_en",
			cmd->xcpc_sgl_sparse_format_en);
	OSAL_LOG_INFO("%30s: %d", "xcpc_sgl_pdma_en",
			cmd->xcpc_sgl_pdma_en);
	OSAL_LOG_INFO("%30s: %d", "xcpc_sgl_pdma_len_from_desc",
			cmd->xcpc_sgl_pdma_len_from_desc);
	OSAL_LOG_INFO("%30s: %d", "xcpc_desc_vec_push_en",
			cmd->xcpc_desc_vec_push_en);
}

static struct queue *
get_seq_q(const struct service_info *svc_info, bool status_q)
{
	struct queue *q = NULL;
	struct per_core_resource *pc_res;

	/* TODO-seq: remove using hard-coded 0th status queue */
	pc_res = svc_info->si_pc_res;

	switch (svc_info->si_type) {
	case PNSO_SVC_TYPE_ENCRYPT:
		q = status_q ? &pc_res->crypto_seq_status_qs[0] :
			&pc_res->crypto_enc_seq_q;
		break;
	case PNSO_SVC_TYPE_DECRYPT:
		q = status_q ? &pc_res->crypto_seq_status_qs[0] :
			&pc_res->crypto_dec_seq_q;
		break;
	case PNSO_SVC_TYPE_COMPRESS:
	case PNSO_SVC_TYPE_HASH:
	case PNSO_SVC_TYPE_CHKSUM:
		q = status_q ? &pc_res->cpdc_seq_status_qs[0] :
			&pc_res->cp_seq_q;
		break;
	case PNSO_SVC_TYPE_DECOMPRESS:
		q = status_q ? &pc_res->cpdc_seq_status_qs[0] :
			&pc_res->dc_seq_q;
		break;
	case PNSO_SVC_TYPE_DECOMPACT:
	default:
		OSAL_ASSERT(0);
		break;
	}

	return q;
}

static void
fill_cpdc_seq_status_desc(struct cpdc_chain_params *chain_params,
		uint8_t *seq_status_desc)
{
	struct next_db_spec *next_db_spec;
	struct barco_spec *barco_spec;
	struct cpdc_chain_params_command *cmd;

	barco_spec = &chain_params->ccp_barco_spec;
	next_db_spec = &chain_params->ccp_next_db_spec;
	cmd = &chain_params->ccp_cmd;

	memset(seq_status_desc, 0, SONIC_SEQ_STATUS_Q_DESC_SIZE);
	// desc bytes 0-63
	if (cmd->ccpc_next_db_action_barco_push) {
		write_bit_fields(seq_status_desc, 0, 64,
				barco_spec->bs_ring_addr);
		write_bit_fields(seq_status_desc, 64, 64,
				barco_spec->bs_desc_addr);
		write_bit_fields(seq_status_desc, 128, 34,
				barco_spec->bs_pndx_addr);
		write_bit_fields(seq_status_desc, 162, 34,
				barco_spec->bs_pndx_shadow_addr);
		write_bit_fields(seq_status_desc, 196, 4,
				barco_spec->bs_desc_size);
		write_bit_fields(seq_status_desc, 200, 3,
				barco_spec->bs_pndx_size);
		write_bit_fields(seq_status_desc, 203, 5,
				barco_spec->bs_ring_size);
		write_bit_fields(seq_status_desc, 208, 10,
				barco_spec->bs_num_descs);
	} else {
		write_bit_fields(seq_status_desc, 0, 64,
				next_db_spec->nds_addr);
		write_bit_fields(seq_status_desc, 64, 64,
				next_db_spec->nds_data);
	}

	write_bit_fields(seq_status_desc, 218, 64,
			chain_params->ccp_status_addr_0);
	write_bit_fields(seq_status_desc, 282, 64,
			chain_params->ccp_status_addr_1);
	write_bit_fields(seq_status_desc, 346, 64,
			chain_params->ccp_intr_addr);
	write_bit_fields(seq_status_desc, 410, 32,
			chain_params->ccp_intr_data);
	write_bit_fields(seq_status_desc, 442, 16,
			chain_params->ccp_status_len);
	write_bit_fields(seq_status_desc, 458, 7,
			chain_params->ccp_status_offset_0);
	write_bit_fields(seq_status_desc, 465, 1,
			cmd->ccpc_status_dma_en);
	write_bit_fields(seq_status_desc, 466, 1,
			cmd->ccpc_next_doorbell_en);
	write_bit_fields(seq_status_desc, 467, 1,
			cmd->ccpc_intr_en);
	write_bit_fields(seq_status_desc, 468, 1,
			cmd->ccpc_next_db_action_barco_push);

	// desc bytes 64-127
	write_bit_fields(seq_status_desc, 512 + 0, 64, 0);
	write_bit_fields(seq_status_desc, 512 + 64, 64,
			chain_params->ccp_comp_buf_addr);
	write_bit_fields(seq_status_desc, 512 + 128, 64,
			chain_params->ccp_aol_src_vec_addr);
	write_bit_fields(seq_status_desc, 512 + 192, 64,
			chain_params->ccp_aol_dst_vec_addr);
	write_bit_fields(seq_status_desc, 512 + 256, 64,
			chain_params->ccp_sgl_vec_addr);
	write_bit_fields(seq_status_desc, 512 + 320, 64,
			chain_params->ccp_pad_buf_addr);
	write_bit_fields(seq_status_desc, 512 + 384, 64,
			chain_params->ccp_alt_buf_addr);
	write_bit_fields(seq_status_desc, 512 + 448, 16,
			chain_params->ccp_data_len);

	write_bit_fields(seq_status_desc, 512 + 464, 5,
			chain_params->ccp_pad_boundary_shift);

	write_bit_fields(seq_status_desc, 512 + 469, 1,
			cmd->ccpc_stop_chain_on_error);
	write_bit_fields(seq_status_desc, 512 + 470, 1,
			cmd->ccpc_data_len_from_desc);
	write_bit_fields(seq_status_desc, 512 + 471, 1,
			cmd->ccpc_aol_pad_en);
	write_bit_fields(seq_status_desc, 512 + 472, 1,
			cmd->ccpc_sgl_pad_en);
	write_bit_fields(seq_status_desc, 512 + 473, 1,
			cmd->ccpc_sgl_sparse_format_en);
	write_bit_fields(seq_status_desc, 512 + 474, 1,
			cmd->ccpc_sgl_pdma_en);
	write_bit_fields(seq_status_desc, 512 + 475, 1,
			cmd->ccpc_sgl_pdma_pad_only);
	write_bit_fields(seq_status_desc, 512 + 476, 1,
			cmd->ccpc_sgl_pdma_alt_src_on_error);
	write_bit_fields(seq_status_desc, 512 + 477, 1,
			cmd->ccpc_desc_vec_push_en);
	write_bit_fields(seq_status_desc, 512 + 478, 1,
			cmd->ccpc_chain_alt_desc_on_error);
}

static void __attribute__((unused))
fill_xts_seq_status(struct xts_chain_params *chain_params,
		uint8_t *seq_status_desc)
{
	struct next_db_spec *next_db_spec;
	struct barco_spec *barco_spec;
	struct xts_chain_params_command *cmd;

	barco_spec = &chain_params->xcp_barco_spec;
	next_db_spec = &chain_params->xcp_next_db_spec;
	cmd = &chain_params->xcp_cmd;

	memset(seq_status_desc, 0, SONIC_SEQ_STATUS_Q_DESC_SIZE);
	// desc bytes 0-63
	if (cmd->xcpc_next_db_action_barco_push) {
		write_bit_fields(seq_status_desc, 0, 64,
				barco_spec->bs_ring_addr);
		write_bit_fields(seq_status_desc, 64, 64,
				barco_spec->bs_desc_addr);
		write_bit_fields(seq_status_desc, 128, 34,
				barco_spec->bs_pndx_addr);
		write_bit_fields(seq_status_desc, 162, 34,
				barco_spec->bs_pndx_shadow_addr);
		write_bit_fields(seq_status_desc, 196, 4,
				barco_spec->bs_desc_size);
		write_bit_fields(seq_status_desc, 200, 3,
				barco_spec->bs_pndx_size);
		write_bit_fields(seq_status_desc, 203, 5,
				barco_spec->bs_ring_size);
		write_bit_fields(seq_status_desc, 208, 10,
				barco_spec->bs_num_descs);
	} else {
		write_bit_fields(seq_status_desc, 0, 64,
				next_db_spec->nds_addr);
		write_bit_fields(seq_status_desc, 64, 64,
				next_db_spec->nds_data);
	}

	write_bit_fields(seq_status_desc, 218, 64,
			chain_params->xcp_status_addr_0);
	write_bit_fields(seq_status_desc, 282, 64,
			chain_params->xcp_status_addr_1);
	write_bit_fields(seq_status_desc, 346, 64,
			chain_params->xcp_intr_addr);
	write_bit_fields(seq_status_desc, 410, 32,
			chain_params->xcp_intr_data);
	write_bit_fields(seq_status_desc, 442, 16,
			chain_params->xcp_status_len);
	write_bit_fields(seq_status_desc, 458, 7,
			chain_params->xcp_status_offset_0);
	write_bit_fields(seq_status_desc, 465, 1,
			cmd->xcpc_status_dma_en);
	write_bit_fields(seq_status_desc, 466, 1,
			cmd->xcpc_next_doorbell_en);
	write_bit_fields(seq_status_desc, 467, 1,
			cmd->xcpc_intr_en);
	write_bit_fields(seq_status_desc, 468, 1,
			cmd->xcpc_next_db_action_barco_push);

	// desc bytes 64-127
	write_bit_fields(seq_status_desc, 512 + 0, 64,
			chain_params->xcp_comp_sgl_src_addr);
	write_bit_fields(seq_status_desc, 512 + 64, 64,
			chain_params->xcp_sgl_pdma_dst_addr);
	write_bit_fields(seq_status_desc, 512 + 128, 64,
			chain_params->xcp_decr_buf_addr);
	write_bit_fields(seq_status_desc, 512 + 192, 16,
			chain_params->xcp_data_len);
	write_bit_fields(seq_status_desc, 512 + 208, 5,
			chain_params->xcp_blk_boundary_shift);
	write_bit_fields(seq_status_desc, 512 + 213, 1,
			cmd->xcpc_stop_chain_on_error);
	write_bit_fields(seq_status_desc, 512 + 214, 1,
			cmd->xcpc_comp_len_update_en);
	write_bit_fields(seq_status_desc, 512 + 215, 1,
			cmd->xcpc_comp_sgl_src_en);
	write_bit_fields(seq_status_desc, 512 + 216, 1,
			cmd->xcpc_comp_sgl_src_vec_en);
	write_bit_fields(seq_status_desc, 512 + 217, 1,
			cmd->xcpc_sgl_sparse_format_en);
	write_bit_fields(seq_status_desc, 512 + 218, 1,
			cmd->xcpc_sgl_pdma_en);
	write_bit_fields(seq_status_desc, 512 + 219, 1,
			cmd->xcpc_sgl_pdma_len_from_desc);
	write_bit_fields(seq_status_desc, 512 + 220, 1,
			cmd->xcpc_desc_vec_push_en);
}

static void *
hw_setup_desc(struct service_info *svc_info, const void *src_desc,
		size_t desc_size)
{
	pnso_error_t err = EINVAL;
	struct accel_ring *ring;
	struct lif *lif;
	struct queue *q;
	struct sequencer_desc *seq_desc;
	uint32_t ring_id, index;
	uint16_t qtype;

	OSAL_LOG_DEBUG("enter ...");

	ring_id = svc_info->si_seq_info.sqi_ring_id;
	qtype = svc_info->si_seq_info.sqi_qtype;
	svc_info->si_seq_info.sqi_index = 0;

	ring = sonic_get_accel_ring(ring_id);
	if (!ring) {
		OSAL_ASSERT(ring);
		goto out;
	}

	lif = sonic_get_lif();
	if (!lif) {
		OSAL_ASSERT(lif);
		goto out;
	}

	err = sonic_get_seq_sq(lif, qtype, &q);
	if (err) {
		OSAL_ASSERT(err);
		goto out;
	}

	seq_desc = (struct sequencer_desc *) sonic_q_consume_entry(q, &index);
	if (!seq_desc) {
		err = EINVAL;
		OSAL_LOG_ERROR("failed to obtain sequencer desc! err: %d", err);
		OSAL_ASSERT(seq_desc);
		goto out;
	}
	svc_info->si_seq_info.sqi_index = index;

	memset(seq_desc, 0, sizeof(*seq_desc));
	seq_desc->sd_desc_addr =
		cpu_to_be64(osal_virt_to_phy((void *) src_desc));
	seq_desc->sd_pndx_addr = cpu_to_be64(ring->ring_pndx_pa);
	seq_desc->sd_pndx_shadow_addr = cpu_to_be64(ring->ring_shadow_pndx_pa);
	seq_desc->sd_ring_addr = cpu_to_be64(ring->ring_base_pa);
	seq_desc->sd_desc_size = (uint8_t) ilog2(ring->ring_desc_size);
	seq_desc->sd_pndx_size = (uint8_t) ilog2(ring->ring_pndx_size);
	seq_desc->sd_ring_size = (uint8_t) ilog2(ring->ring_size);
	if (svc_info->si_seq_info.sqi_batch_mode) {
		seq_desc->sd_batch_mode = true;
		seq_desc->sd_batch_size =
			cpu_to_be16(svc_info->si_seq_info.sqi_batch_size);
	}

	OSAL_LOG_INFO("ring_id: %u index: %u src_desc: 0x%llx  desc_size: %lu",
			ring_id, index, (u64) src_desc, desc_size);
	PPRINT_SEQUENCER_DESC(seq_desc);

	OSAL_LOG_DEBUG("exit!");
	return seq_desc;

out:
	OSAL_LOG_ERROR("exit! err: %d", err);
	return NULL;
}

static void
hw_ring_db(const struct service_info *svc_info)
{
	struct queue *seq_q;
	uint16_t index;

	OSAL_LOG_DEBUG("enter ... ");

	seq_q = get_seq_q(svc_info, false);
	if (!seq_q) {
		OSAL_LOG_ERROR("failed to get sequencer q!");
		OSAL_ASSERT(seq_q);
		goto out;
	}

	index = svc_info->si_seq_info.sqi_index;
	sonic_q_ringdb(seq_q, index);

out:
	OSAL_LOG_DEBUG("exit!");
}

static pnso_error_t
hw_setup_cp_chain_params(struct chain_entry *centry,
		struct service_info *svc_info,
		struct cpdc_desc *cp_desc,
		struct cpdc_status_desc *status_desc)
{
	pnso_error_t err = EINVAL;
	struct service_chain *svc_chain;
	struct cpdc_chain_params *chain_params;
	struct sequencer_info *seq_info;
	struct sequencer_spec *seq_spec;
	uint32_t ring_id, index;
	uint16_t qtype;
	uint8_t *seq_status_desc;

	struct accel_ring *ring;
	struct lif *lif;
	struct queue *q, *status_q;
	uint16_t lif_id;

	OSAL_LOG_INFO("enter ...");

	svc_chain = centry->ce_chain_head;
	chain_params = &svc_chain->sc_chain_params;
	seq_spec = &chain_params->ccp_seq_spec;

	seq_info = &svc_info->si_seq_info;
	ring_id = seq_info->sqi_ring_id;
	qtype = seq_info->sqi_qtype;
	seq_info->sqi_index = 0;
	PPRINT_SEQUENCER_INFO(seq_info);

	ring = sonic_get_accel_ring(ring_id);
	if (!ring) {
		OSAL_ASSERT(ring);
		goto out;
	}

	lif = sonic_get_lif();
	if (!lif) {
		OSAL_ASSERT(lif);
		goto out;
	}
	lif_id = sonic_get_lif_id();

	err = sonic_get_seq_sq(lif, qtype, &q);
	if (err) {
		OSAL_ASSERT(err);
		goto out;
	}

	err = sonic_get_seq_statusq(lif, SONIC_QTYPE_CPDC_STATUS, &status_q);
	if (err) {
		OSAL_ASSERT(err);
		goto out;
	}

	seq_status_desc = (uint8_t *) sonic_q_consume_entry(status_q, &index);
	if (!seq_status_desc) {
		err = EINVAL;
		OSAL_LOG_ERROR("failed to obtain sequencer statusq desc! err: %d",
				err);
		OSAL_ASSERT(seq_status_desc);
		goto out;
	}
	seq_info->sqi_index = index;
	seq_info->sqi_status_desc = seq_status_desc;

	seq_spec->sqs_seq_q = (uint64_t) q;
	seq_spec->sqs_seq_status_q = (uint64_t) status_q;
	/* skip sqs_seq_next_q/sqs_seq_next_status_q not needed for comp+hash */

	cp_desc->cd_db_addr = kDbAddrCapri |
		((uint64_t) kDbAddrUpdate << kDbUpdateShift) |
		((uint64_t) lif_id << kDbLifShift) |
		((uint64_t) status_q->qtype << kDbTypeShift);

	cp_desc->cd_db_data = sonic_q_ringdb_data(status_q, index);
	cp_desc->u.cd_bits.cc_db_on = 1;

	chain_params->ccp_cmd.ccpc_next_doorbell_en = 1;
	chain_params->ccp_cmd.ccpc_next_db_action_barco_push = 1;
	chain_params->ccp_cmd.ccpc_stop_chain_on_error = 1;
	chain_params->ccp_cmd.ccpc_sgl_pdma_en = 1;

	chain_params->ccp_status_addr_0 =
		osal_virt_to_phy((void *) status_desc);
	chain_params->ccp_pad_buf_addr =
		(uint64_t) osal_virt_to_phy((void *) pad_buffer);
	chain_params->ccp_pad_boundary_shift =
		(uint8_t) ilog2(PNSO_MEM_ALIGN_PAGE);

	chain_params->ccp_cmd.ccpc_sgl_pdma_pad_only = 1;

	OSAL_LOG_INFO("ring_id: %u index: %u src_desc: 0x%llx status_desc: 0x%llx",
			ring_id, index, (u64) cp_desc, (u64) status_desc);

	PPRINT_SEQUENCER_INFO(seq_info);

	err = PNSO_OK;
	OSAL_LOG_INFO("exit!");
	return err;

out:
	OSAL_LOG_ERROR("exit! err: %d", err);
	return err;
}

static pnso_error_t
hw_setup_hash_chain_params(struct chain_entry *centry,
		struct service_info *svc_info,
		struct cpdc_desc *hash_desc, struct cpdc_sgl *sgl,
		uint32_t num_hash_blks)
{
	pnso_error_t err = EINVAL;
	struct accel_ring *ring;
	uint32_t ring_id;

	struct service_chain *svc_chain;
	struct cpdc_chain_params *chain_params;
	struct sequencer_info *seq_info;
	struct barco_spec *barco_spec;

	OSAL_LOG_INFO("enter ...");

	svc_chain = centry->ce_chain_head;
	chain_params = &svc_chain->sc_chain_params;
	barco_spec = &chain_params->ccp_barco_spec;

	seq_info = &svc_info->si_seq_info;
	ring_id = seq_info->sqi_ring_id;
	PPRINT_SEQUENCER_INFO(seq_info);

	ring = sonic_get_accel_ring(ring_id);
	if (!ring) {
		OSAL_ASSERT(0);
		goto out;
	}

	barco_spec->bs_ring_addr = ring->ring_base_pa;
	barco_spec->bs_pndx_addr = ring->ring_pndx_pa;
	barco_spec->bs_pndx_shadow_addr = ring->ring_shadow_pndx_pa;
	barco_spec->bs_desc_addr = osal_virt_to_phy((void *) hash_desc);
	barco_spec->bs_desc_size = (uint8_t) ilog2(ring->ring_desc_size);
	barco_spec->bs_pndx_size = (uint8_t) ilog2(ring->ring_pndx_size);
	barco_spec->bs_ring_size = (uint8_t) ilog2(ring->ring_size);
	barco_spec->bs_num_descs = num_hash_blks;

	chain_params->ccp_sgl_vec_addr = osal_virt_to_phy((void *) sgl);
	chain_params->ccp_cmd.ccpc_sgl_pad_en = 1;
	chain_params->ccp_cmd.ccpc_sgl_sparse_format_en = 1;
	/*
	 * hash executes multiple requests, one per block; hence, indicate to
	 * P4+ to push a vector of descriptors
	 */
	chain_params->ccp_cmd.ccpc_desc_vec_push_en = 1;

	PPRINT_SEQUENCER_INFO(seq_info);

	err = PNSO_OK;
	OSAL_LOG_INFO("exit!");
	return err;
out:
	OSAL_LOG_ERROR("exit! err: %d", err);
	return err;
}

static void *
hw_setup_cpdc_chain_desc(struct chain_entry *centry,
		struct service_info *svc_info,
		const void *src_desc, size_t desc_size)
{
	pnso_error_t err = EINVAL;
	struct service_chain *svc_chain;
	struct cpdc_chain_params *chain_params;
	struct sequencer_info *seq_info;
	struct sequencer_desc *seq_desc;

	OSAL_LOG_INFO("enter ...");

	svc_chain = centry->ce_chain_head;
	chain_params = &svc_chain->sc_chain_params;
	seq_info = &svc_info->si_seq_info;
	PPRINT_SEQUENCER_INFO(seq_info);

	fill_cpdc_seq_status_desc(chain_params, seq_info->sqi_status_desc);
	PPRINT_CPDC_CHAIN_PARAMS(chain_params);

	seq_desc = hw_setup_desc(svc_info, src_desc, desc_size);
	if (!seq_desc) {
		OSAL_ASSERT(seq_desc);
		OSAL_LOG_ERROR("failed to setup seq desc! err: %d", err);
		goto out;
	}

	PPRINT_SEQUENCER_INFO(seq_info);

	OSAL_LOG_INFO("exit!");
	return seq_desc;

out:
	OSAL_LOG_ERROR("exit! err: %d", err);
	return NULL;
}

const struct sequencer_ops hw_seq_ops = {
	.setup_desc = hw_setup_desc,
	.ring_db = hw_ring_db,
	.setup_cp_chain_params = hw_setup_cp_chain_params,
	.setup_hash_chain_params = hw_setup_hash_chain_params,
	.setup_cpdc_chain_desc = hw_setup_cpdc_chain_desc,
};
