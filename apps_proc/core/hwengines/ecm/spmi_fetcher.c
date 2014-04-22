#include "bam.h"
#include "spmi_fetcher.h"
#include "spmif_hw.h"

#include "DALSys.h"
#include "DalDevice.h"
#include "DALFramework.h"
#include "msg.h"
#include "spmif_cfgdata.h"
#include "msmhwiobase.h"
#include "DDIClock.h"
#include "DALDeviceId.h"
#include "DalDevice.h"
#include "ecm_log.h"
#include "DDISpmi.h"

struct spmif_dev_t
{
	enum spmif_op_mode mode_current;
    uint32 eot_length;
	/* bam related */
	bam_handle bam_h;
	bam_handle pipe_h;
	bam_callback_type pipe_cb;
	bam_callback_type bam_cb;
	bam_config_type bam_cfg;
	bam_pipe_config_type pipe_cfg;
    ClockIdType spmif_clk_id;
    ClockIdType spmif_ahb_clk_id;
    DalDeviceHandle* dal_clock_h;
};

static struct spmif_dev_t spmif_dev;

static int spmif_bam_init(uint32 fifo_va, uint32 fifo_pa, uint32 size,bam_callback_func_type cb)
{
	memset(&spmif_dev.bam_cfg, 0, sizeof(bam_config_type));
	memset(&spmif_dev.pipe_cfg, 0, sizeof(bam_pipe_config_type));

	spmif_dev.bam_cfg.bam_pa = SPMIF_BAM_PA;
	spmif_dev.bam_cfg.bam_va = SPMI_FETCHER_BASE + 0x4000;
	spmif_dev.bam_cfg.bam_irq = SPMIF_BAM_IRQ;
	spmif_dev.bam_cfg.bam_irq_mask = BAM_IRQ_HRESP_ERR_EN|BAM_IRQ_ERR_EN;
	spmif_dev.bam_cfg.sum_thresh = 1;

	spmif_dev.bam_cb.func = NULL;
	spmif_dev.bam_cb.data = (void*)&spmif_dev;

	spmif_dev.bam_h = bam_init(&spmif_dev.bam_cfg, &spmif_dev.bam_cb);

	if (!spmif_dev.bam_h) {
		ecm_log(ECM_LOG_ERROR, "spmi bam init failed\n");
		return 1;
	}

	spmif_dev.pipe_cfg.options = (BAM_O_DESC_DONE|BAM_O_EOT|BAM_O_ERROR);
	spmif_dev.pipe_cfg.dir =  BAM_DIR_PRODUCER;
	spmif_dev.pipe_cfg.mode = BAM_MODE_SYSTEM;
	spmif_dev.pipe_cfg.desc_base_va = fifo_va;
	spmif_dev.pipe_cfg.desc_base_pa = fifo_pa;
	spmif_dev.pipe_cfg.desc_size = size;
	spmif_dev.pipe_cfg.evt_thresh = 1;

	spmif_dev.pipe_cb.data = NULL;
	spmif_dev.pipe_cb.func = cb;

	spmif_dev.pipe_h = bam_pipe_init(spmif_dev.bam_h, ECM_PRODUCER_PIPE, 
			&spmif_dev.pipe_cfg, &spmif_dev.pipe_cb );

	if (!spmif_dev.pipe_h) {
		ecm_log(ECM_LOG_ERROR, "Error initializing pipe \n");
		return 1;
	}

    return 0;
}

int spmif_dev_init(uint32 fifo_va, uint32 fifo_pa, uint32 size, bam_callback_func_type cb, uint32 channel)
{
    /* enable clocks */
    if ( DAL_SUCCESS != DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &spmif_dev.dal_clock_h)) {
        ecm_log(ECM_LOG_ERROR,"Error attaching to clock device");
        return 1;
    }
    if (DAL_SUCCESS != DalClock_GetClockId(spmif_dev.dal_clock_h, "gcc_spmi_fetcher_clk",&spmif_dev.spmif_clk_id)){
        ecm_log(ECM_LOG_ERROR,"Error geting clock id gcc_spmi_fetcher_clk");
        return 1;
    }
    if (DAL_SUCCESS != DalClock_GetClockId(spmif_dev.dal_clock_h, "gcc_spmi_fetcher_ahb_clk",&spmif_dev.spmif_ahb_clk_id)){
        ecm_log(ECM_LOG_ERROR,"Error geting clock id gcc_spmi_fetcher_ahb_clk");
        return 1;
    }
    if (DAL_SUCCESS != DalClock_EnableClock(spmif_dev.dal_clock_h, spmif_dev.spmif_clk_id)){
        ecm_log(ECM_LOG_ERROR,"Error enabling clock gcc_spmi_fetcher_clk");
        return 1;
    }
    if (DAL_SUCCESS != DalClock_EnableClock(spmif_dev.dal_clock_h, spmif_dev.spmif_ahb_clk_id)){
        ecm_log(ECM_LOG_ERROR,"Error enabling clock gcc_spmi_fetcher_ahb_clk");
        return 1;
    }

 	if (spmif_bam_init(fifo_va, fifo_pa, size,cb)) {
        ecm_log(ECM_LOG_ERROR,"spmi fetcher bam init failed");
        return 1;
    }

	spmif_hw_init(channel);

    if (DAL_SUCCESS != DalClock_DisableClock(spmif_dev.dal_clock_h, spmif_dev.spmif_clk_id)){
        ecm_log(ECM_LOG_ERROR,"Error disabling clock gcc_spmi_fetcher_clk");
        return 1;
    }
    if (DAL_SUCCESS != DalClock_DisableClock(spmif_dev.dal_clock_h, spmif_dev.spmif_ahb_clk_id)){
        ecm_log(ECM_LOG_ERROR,"Error disabling clock gcc_spmi_fetcher_ahb_clk");
        return 1;
    }
    return 0;
}

void spmif_dev_deinit(void)
{
    if (spmif_dev.pipe_h) {
        bam_pipe_deinit(spmif_dev.pipe_h);
}

    if (spmif_dev.bam_h) {
        bam_deinit(spmif_dev.bam_h, 0);
    }
    }

int spmif_start(enum spmif_op_mode mode)
{
    spmif_dev.mode_current = mode;

    if (mode == SPMIF_OP_ONESHOT) {
        spmif_hw_start(SPMIF_HW_ONESHOT);
    } else if (mode == SPMIF_OP_STREAMING) {
        spmif_hw_start(SPMIF_HW_STREAMING);
    }
    return 0;
}

int spmif_stop(void)
{
	/* stop spmi fetcher */
	spmif_hw_stop();

	return 0;
    }

void spmif_set_eot_length(uint32 eot_length)
{
    spmif_hw_set_eot_length(eot_length);
}

int spmif_queue_buffer(uint32 pa, uint32 size, void* ctx)
{
    if (BAM_SUCCESS != bam_pipe_transfer(spmif_dev.pipe_h, pa, size,0, ctx)) {
        ecm_log(ECM_LOG_ERROR,"Error submitting buffer to bam");
        return 1;
	}
    ecm_log_1(ECM_LOG_INFO,"resubmitted buffer 0x%x", pa);
	return 0;
}

int spmif_is_idle(void)
{
	return spmif_hw_idle();
}

void spmif_clk_on(void)
{
    if (DAL_SUCCESS != DalClock_EnableClock(spmif_dev.dal_clock_h, spmif_dev.spmif_clk_id)){
        ecm_log(ECM_LOG_ERROR,"Error enabling clock gcc_spmi_fetcher_clk");
    }
    if (DAL_SUCCESS != DalClock_EnableClock(spmif_dev.dal_clock_h, spmif_dev.spmif_ahb_clk_id)){
        ecm_log(ECM_LOG_ERROR,"Error enabling clock gcc_spmi_fetcher_ahb_clk");
    }
}

void spmif_clk_off(void)
{
    if (DAL_SUCCESS != DalClock_DisableClock(spmif_dev.dal_clock_h, spmif_dev.spmif_clk_id)){
        ecm_log(ECM_LOG_ERROR,"Error disabling clock gcc_spmi_fetcher_clk");
    }
    if (DAL_SUCCESS != DalClock_DisableClock(spmif_dev.dal_clock_h, spmif_dev.spmif_ahb_clk_id)){
        ecm_log(ECM_LOG_ERROR,"Error disabling clock gcc_spmi_fetcher_ahb_clk");
    }
}

void spmif_set_pollgap_time(uint32 time_us)
{
   //convert time to a count
   uint32 count = time_us/SPMIF_POLLGAP_GRANULARITY;
   if (!count) 
       count = 1;
    spmif_hw_set_pollgap_time(count);
}

void spmif_set_readgap_time(uint32 time_us)
{
    //convert time to a count
    uint32 count = time_us/SPMIF_READGAP_GRANULARITY;
    if (!count) 
        count = 1;
    spmif_hw_set_readgap_time(count);

}
