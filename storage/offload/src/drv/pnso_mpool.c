/*
 * {C} Copyright 2018 Pensando Systems Inc.
 * All rights reserved.
 *
 */
#include "osal.h"
#include "pnso_mpool.h"

#define MPOOL_MAGIC_VALID	0xffff012345670000
#define MPOOL_MAGIC_INVALID	0xabf0cdf0eff0abf0

#define PNSO_MAX(a, b) ((a) > (b) ? (a) : (b))

const char __attribute__ ((unused)) *mem_pool_types[] = {
	[MPOOL_TYPE_NONE] = "None (invalid)",
	[MPOOL_TYPE_CPDC_DESC] = "CPDC DESC",
	[MPOOL_TYPE_CPDC_DESC_VECTOR] = "CPDC DESC VECTOR",
	[MPOOL_TYPE_CPDC_SGL] = "CPDC SGL",
	[MPOOL_TYPE_CPDC_SGL_VECTOR] = "CPDC SGL VECTOR",
	[MPOOL_TYPE_CPDC_STATUS_DESC] = "CPDC STATUS DESC",
	[MPOOL_TYPE_CPDC_STATUS_DESC_VECTOR] = "CPDC STATUS DESC VECTOR",
	[MPOOL_TYPE_CRYPTO_DESC] = "CRYPTO DESC",
	[MPOOL_TYPE_CRYPTO_STATUS_DESC] = "CRYPTO STATUS DESC",
	[MPOOL_TYPE_CRYPTO_AOL] = "CRYPTO AOL",
	[MPOOL_TYPE_CRYPTO_AOL_VECTOR] = "CRYPTO AOL VECTOR",
	[MPOOL_TYPE_SERVICE_CHAIN] = "SERVICE CHAIN",
	[MPOOL_TYPE_SERVICE_CHAIN_ENTRY] = "SERVICE CHAIN ENTRY",
	[MPOOL_TYPE_MAX] = "Max (invalid)"
};

const char *
mem_pool_get_type_str(enum mem_pool_type mpool_type)
{
	if (mpool_type < MPOOL_TYPE_MAX)
		return mem_pool_types[mpool_type];
	return "unknown";
}

#ifndef __KERNEL__
static inline bool
is_power_of_2(unsigned long n)
{
	return (n != 0 && ((n & (n - 1)) == 0));
}
#endif

static bool __attribute__ ((unused))
is_pool_valid(struct mem_pool *mpool)
{
	return (mpool->mp_magic & MPOOL_MAGIC_VALID) ? true : false;
}

static bool
is_pool_type_valid(enum mem_pool_type mpool_type)
{
	switch (mpool_type) {
	case MPOOL_TYPE_CPDC_DESC:
	case MPOOL_TYPE_CPDC_DESC_VECTOR:
	case MPOOL_TYPE_CPDC_SGL:
	case MPOOL_TYPE_CPDC_SGL_VECTOR:
	case MPOOL_TYPE_CPDC_STATUS_DESC:
	case MPOOL_TYPE_CPDC_STATUS_DESC_VECTOR:
	case MPOOL_TYPE_CRYPTO_DESC:
	case MPOOL_TYPE_CRYPTO_STATUS_DESC:
	case MPOOL_TYPE_CRYPTO_AOL:
	case MPOOL_TYPE_CRYPTO_AOL_VECTOR:
	case MPOOL_TYPE_SERVICE_CHAIN:
	case MPOOL_TYPE_SERVICE_CHAIN_ENTRY:
		return true;
	default:
		return false;
	}

	return false;
}

uint32_t
mpool_get_pad_size(uint32_t object_size, uint32_t align_size)
{
	uint32_t pad_size = 0;

	if (PNSO_MAX(object_size, align_size) == align_size) {
		pad_size = align_size - object_size;
		goto out;
	}

	if (object_size % align_size == 0)
		goto out;

	pad_size = (object_size + align_size -
			(object_size % align_size)) - object_size;

out:
	return pad_size;
}

pnso_error_t
mpool_create(enum mem_pool_type mpool_type,
		uint32_t num_objects, uint32_t object_size,
		uint32_t align_size, struct mem_pool **out_mpool)
{
	pnso_error_t err;
	struct mem_pool *mpool;
	size_t pool_size;
	uint32_t pad_size;
	void **objects;
	char *obj;
	int i;

	if (!is_pool_type_valid(mpool_type)) {
		err = EINVAL;
		OSAL_LOG_ERROR("invalid pool type specified. mpool_type: %d err: %d",
			       mpool_type, err);
		goto out;
	}

	if (object_size == 0) {
		err = EINVAL;
		OSAL_LOG_ERROR("invalid object size specified. object_size: %d err: %d",
			       object_size, err);
		goto out;
	}

	if (!is_power_of_2(align_size)) {
		err = EINVAL;
		OSAL_LOG_ERROR("invalid alignment size specified. align_size: %d err: %d",
			       align_size, err);
		goto out;
	}

	if (!out_mpool) {
		err = EINVAL;
		OSAL_LOG_ERROR("invalid pointer for pool specified. out_mpool: %p err: %d",
			       out_mpool, err);
		goto out;
	}

	/* compute pad and total pool size */
	pad_size = mpool_get_pad_size(object_size, align_size);
	pool_size = ((object_size + pad_size) * num_objects);

	/* allocate memory for pool, objects, and its stack */
	mpool = osal_alloc(sizeof(struct mem_pool));
	if (!mpool) {
		err = ENOMEM;
		OSAL_LOG_ERROR("failed to allocate memory for pool! mpool_type: %d num_objects: %d err: %d",
			       mpool_type, num_objects, err);
		goto out;
	}

	mpool->mp_objects = osal_aligned_alloc(align_size, pool_size);
	if (!mpool->mp_objects) {
		err = ENOMEM;
		OSAL_LOG_ERROR("failed to allocate memory for objects! mpool_type: %d num_objects: %d err: %d",
			       mpool_type, num_objects, err);
		goto out_free_pool;
	}

	objects = osal_alloc(sizeof(void *) * num_objects);
	if (!objects) {
		err = ENOMEM;
		OSAL_LOG_ERROR("failed to allocate memory for stack objects! mpool_type: %d num_objects: %d err: %d",
			       mpool_type, num_objects, err);
		goto out_free_objects;
	}

	/* populate the pool */
	mpool->mp_magic = MPOOL_MAGIC_VALID;

	mpool->mp_config.mpc_type = mpool_type;
	mpool->mp_config.mpc_num_objects = num_objects;
	mpool->mp_config.mpc_object_size = object_size;
	mpool->mp_config.mpc_align_size = align_size;
	mpool->mp_config.mpc_pad_size = pad_size;
	mpool->mp_config.mpc_pool_size = pool_size;

	mpool->mp_stack.mps_num_objects = num_objects;
	mpool->mp_stack.mps_top = 0;
	mpool->mp_stack.mps_objects = objects;

	/* populate the stack to point the newly created objects */
	obj = (char *) mpool->mp_objects;
	for (i = 0; i < mpool->mp_config.mpc_num_objects; i++) {
		objects[i] = obj;
		OSAL_LOG_DEBUG("%30s[%d]: 0x%llx 0x%llx %u %u %u",
			       "mpool->mp_dstack.mps_objects", i,
			       (uint64_t) &objects[i], (uint64_t) objects[i],
			       object_size, pad_size, align_size);
		obj += (object_size + pad_size);
	}
	mpool->mp_stack.mps_top = mpool->mp_config.mpc_num_objects;

	*out_mpool = mpool;
	OSAL_LOG_INFO("pool allocated. mpool_type: %d num_objects: %d object_size: %d align_size: %d pad_size: %d mpool: 0x%llx",
		      mpool_type, num_objects, object_size,
		      align_size, pad_size, (uint64_t) mpool);

	err = PNSO_OK;
	return err;

out_free_objects:
	osal_free(mpool->mp_objects);
out_free_pool:
	osal_free(mpool);
out:
	OSAL_LOG_ERROR("failed to allocate pool!  mpool_type: %d num_objects: %d object_size: %d align_size: %d",
			mpool_type, num_objects, object_size, align_size);
	return err;
}

void
mpool_destroy(struct mem_pool **mpoolp)
{
	struct mem_pool *mpool;

	if (!mpoolp || !*mpoolp)
		return;

	mpool = *mpoolp;

	OSAL_LOG_INFO("pool deallocated. mpc_type: %d mpc_num_objects: %d mpool: 0x%llx",
		      mpool->mp_config.mpc_type,
		      mpool->mp_config.mpc_num_objects, (uint64_t) mpool);

	/* TODO-mpool: for graceful exit, ensure stack top is back to full */
	mpool->mp_magic = MPOOL_MAGIC_INVALID;

	osal_free(mpool->mp_stack.mps_objects);
	osal_free(mpool->mp_objects);
	osal_free(mpool);

	*mpoolp = NULL;
}

void *
mpool_get_object(struct mem_pool *mpool)
{
	struct mem_pool_stack *mem_stack;
	void *object = NULL;

	if (!mpool)
		return NULL;

	if (!is_pool_valid(mpool))
		return NULL;

	mem_stack = &mpool->mp_stack;
	OSAL_ASSERT(mem_stack);

	if (mem_stack->mps_top > 0)
		object = mem_stack->mps_objects[--(mem_stack->mps_top)];

	return object;
}

pnso_error_t
mpool_put_object(struct mem_pool *mpool, void *object)
{
	pnso_error_t err = ENOTEMPTY;
	struct mem_pool_stack *mem_stack;

	if (!mpool || !object)
		return EINVAL;

	if (!is_pool_valid(mpool))
		return EINVAL;

	mem_stack = &mpool->mp_stack;
	OSAL_ASSERT(mem_stack);

	if (mem_stack->mps_top < mem_stack->mps_num_objects) {
		mem_stack->mps_objects[mem_stack->mps_top] = object;
		mem_stack->mps_top++;
		err = PNSO_OK;
	}

	return err;
}

void __attribute__ ((unused))
mpool_pprint(const struct mem_pool *mpool)
{
	int i;
	void **objects;

	if (!mpool)
		return;

	OSAL_LOG_INFO("%-30s: 0x%llx", "mpool", (uint64_t) mpool);
	// OSAL_LOG_INFO("%-30s: %llx", "mpool->mp_magic", mpool->mp_magic);

	OSAL_LOG_INFO("%-30s: %u:%s", "mpool->mp_config.mpc_type",
			mpool->mp_config.mpc_type,
			mem_pool_get_type_str(mpool->mp_config.mpc_type));
	OSAL_LOG_INFO("%-30s: %u", "mpool->mp_config.mpc_num_objects",
			mpool->mp_config.mpc_num_objects);
	OSAL_LOG_INFO("%-30s: %u", "mpool->mp_config.mpc_object_size",
			mpool->mp_config.mpc_object_size);
	OSAL_LOG_INFO("%-30s: %u", "mpool->mp_config.mpc_align_size",
			mpool->mp_config.mpc_align_size);
	OSAL_LOG_INFO("%-30s: %u", "mpool->mp_config.mpc_pad_size",
			mpool->mp_config.mpc_pad_size);
	OSAL_LOG_INFO("%-30s: %u", "mpool->mp_config.mpc_pool_size",
			mpool->mp_config.mpc_pool_size);

	OSAL_LOG_INFO("%-30s: 0x%llx", "mpool->mp_objects",
			(uint64_t) mpool->mp_objects);

	OSAL_LOG_INFO("%-30s: %d", "mpool->mp_stack.mps_num_objects",
			mpool->mp_stack.mps_num_objects);
	OSAL_LOG_INFO("%-30s: %d", "mpool->mp_stack.mps_top",
			mpool->mp_stack.mps_top);
	OSAL_LOG_INFO("%-30s: 0x%llx", "mpool->mp_stack.mps_objects",
			(uint64_t) mpool->mp_stack.mps_objects);

	objects = mpool->mp_stack.mps_objects;
	for (i = 0; i < mpool->mp_config.mpc_num_objects; i++) {
		OSAL_LOG_DEBUG("%30s[%d]: 0x%llx 0x%llx",
				"mpool->mp_stack.mps_objects", i,
				(uint64_t) &objects[i], (uint64_t) objects[i]);
	}
}
