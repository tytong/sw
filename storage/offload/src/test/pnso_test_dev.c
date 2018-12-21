#include <sys/types.h>
#include <sys/param.h>  /* defines used in kernel.h */
#include <sys/kernel.h> /* types used in module initialization */
#include <sys/conf.h>   /* cdevsw struct */
#include <sys/uio.h>    /* uio struct */
#include <sys/malloc.h>
#include "pnso_test.h"
#include "pnso_test_ctx.h"
#include "pnso_test_sysfs.h"
#include "pnso_test_dev.h"

static d_open_t pnso_test_open;
static d_close_t pnso_test_close;
static d_read_t pnso_test_read;
static d_write_t pnso_test_write;

#define DEVNAME "pencake"

/* Character device entry points */
static struct cdevsw pnso_test_cdevsw = {
	.d_version = D_VERSION,
	.d_open = pnso_test_open,
	.d_close = pnso_test_close,
	.d_read = pnso_test_read,
	.d_write = pnso_test_write,
	.d_name = DEVNAME,
};
#define BUFFERSIZE 40960

/*
 * XXX: Fix me for multi device support using device softc.
 */
static struct {
	char *input;
	int input_len;
	int input_max;

	char *output;
	int output_max;
	int output_len;

	struct mtx mtx;
} pnso_test_softc;

static struct cdev *pnso_test_dev;
static osal_atomic_int_t ctl_state;

int
pnso_test_cdev_init(void)
{
	mtx_init(&pnso_test_softc.mtx, "pnso char", NULL, MTX_SPIN);

	pnso_test_softc.input_max = BUFFERSIZE - 1;
	pnso_test_softc.output_max = BUFFERSIZE - 1;

        pnso_test_softc.input = TEST_ALLOC(pnso_test_softc.input_max + 1);
        pnso_test_softc.output = TEST_ALLOC(pnso_test_softc.output_max + 1);
	if ((pnso_test_softc.input == NULL) || (pnso_test_softc.output == NULL)) {
		PNSO_LOG_ERROR("Opened device pnso_test alloc failed\n");
		return (EINVAL);
	}

	return make_dev_p(MAKEDEV_CHECKNAME | MAKEDEV_WAITOK,
	    &pnso_test_dev,
	    &pnso_test_cdevsw,
	    0,
	    UID_ROOT,
	    GID_WHEEL,
	    0600,
	    DEVNAME);
}

void
pnso_test_cdev_deinit(void)
{
	
	destroy_dev(pnso_test_dev);
	
	TEST_FREE(pnso_test_softc.input);
	TEST_FREE(pnso_test_softc.output);

	pnso_test_softc.input = NULL;
	pnso_test_softc.output = NULL;
	mtx_destroy(&pnso_test_softc.mtx);
}

static int
pnso_test_open(struct cdev *dev __unused, int oflags __unused, int devtype __unused,
    struct thread *td __unused)
{

	PNSO_LOG_DEBUG("Opened device pnso_test successfully.\n");
	return (0);
}

static int
pnso_test_close(struct cdev *dev __unused, int fflag __unused, int devtype __unused,
    struct thread *td __unused)
{
	PNSO_LOG_DEBUG("Closing device pnso_test.\n");
	return (0);
}

static int
pnso_test_read(struct cdev *dev __unused, struct uio *uio, int ioflag __unused)
{
	size_t amt;
	int error;

	if ((pnso_test_softc.input == NULL) || (pnso_test_softc.output == NULL))
		return (ENXIO);

	mtx_lock_spin(&pnso_test_softc.mtx);
	/*
	 * How big is this read operation?  Either as big as the user wants,
	 * or as big as the remaining data.  Note that the 'input_len' does not
	 * include the trailing null character.
	 */
	amt = MIN(uio->uio_resid, uio->uio_offset >= pnso_test_softc.output_len + 1 ? 0 :
	    pnso_test_softc.output_len + 1 - uio->uio_offset);

	if ((error = uiomove(pnso_test_softc.output, amt, uio)) != 0)
		PNSO_LOG_ERROR("uiomove failed!\n");

	mtx_unlock_spin(&pnso_test_softc.mtx);
	return (error);
}

static int
pnso_test_write(struct cdev *dev __unused, struct uio *uio, int ioflag __unused)
{
	size_t amt;
	int error;

	PNSO_LOG_DEBUG("Writing to device pnso_test.\n");
	if ((pnso_test_softc.input == NULL) || (pnso_test_softc.output == NULL))
		return (ENXIO);
	
	/*
	 * We either write from the beginning or are appending -- do
	 * not allow random access.
	 */
	if (uio->uio_offset != 0 && (uio->uio_offset != pnso_test_softc.input_len))
		return (EINVAL);

	mtx_lock_spin(&pnso_test_softc.mtx);
	/* This is a new message, reset length */
	if (uio->uio_offset == 0) {
		pnso_test_softc.input_len = 0;
		pnso_test_softc.output_len = 0;
	}

	/* Copy the string in from user memory to kernel memory */
	amt = MIN(uio->uio_resid, (pnso_test_softc.input_max - pnso_test_softc.input_len));

	error = uiomove(pnso_test_softc.input + uio->uio_offset, amt, uio);

	/* Now we need to null terminate and record the length */
	pnso_test_softc.input_len = uio->uio_offset;
	pnso_test_softc.input[pnso_test_softc.input_len] = 0;

	if (error != 0)
		PNSO_LOG_ERROR("Write failed: bad address!\n");

	mtx_unlock_spin(&pnso_test_softc.mtx);
	/* Start the test. */
	osal_atomic_set(&ctl_state, CTL_STATE_START);
	
	PNSO_LOG_DEBUG("Writing to device pnso_test done.\n");
	
	return (error);
}

char *pnso_test_sysfs_alloc_and_get_cfg(void)
{
	char *buf;
	int len;

	if (pnso_test_softc.input == NULL)
		return NULL;
	
	PNSO_LOG_DEBUG("Sending pnso_test buffer for test.\n");

	len = pnso_test_softc.input_len;
	buf = TEST_ALLOC(len + 1);

	if (buf == NULL)
		return (NULL);

	mtx_lock_spin(&pnso_test_softc.mtx);
	memcpy(buf, pnso_test_softc.input, len);
	mtx_unlock_spin(&pnso_test_softc.mtx);
	
	buf[len] = 0;
	return (buf);
}

int pnso_test_sysfs_read_ctl(void)
{
	return (osal_atomic_exchange(&ctl_state, CTL_STATE_READ));
}

static void write_dev_data(const char *src, uint32_t size)
{
	int len;
	
	if (pnso_test_softc.output == NULL)
		return;

	mtx_lock_spin(&pnso_test_softc.mtx);
	/* One less for null character. */
	len = min(size, pnso_test_softc.output_max  - pnso_test_softc.output_len);
	memcpy(pnso_test_softc.output + pnso_test_softc.output_len, src, len);
	pnso_test_softc.output_len += len;
	pnso_test_softc.output[pnso_test_softc.output_len] = 0;
	mtx_unlock_spin(&pnso_test_softc.mtx);
}

static void write_testcase_summary(uint32_t testcase_id, bool success)
{
	if (success) {
		pnso_test_inc_success_cnt();
	} else {
		pnso_test_inc_fail_cnt();
	}
}

void pnso_test_sysfs_write_status_data(const char *src, uint32_t len, void *opaque)
{
	struct testcase_context *test_ctx = (struct testcase_context *) opaque;

	write_dev_data(src, len);
	if (test_ctx) {
	write_testcase_summary(test_ctx->testcase->node.idx,
		test_ctx->stats.agg_stats.validation_failures == 0);
	}
}
