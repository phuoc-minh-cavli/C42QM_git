/*
===============================================================================

                             Edit History

 $Header: //components/rel/core.tx/6.0/hwengines/ecm/ecm.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/1/15   rl      Created

===============================================================================
                   Copyright (c) 2015 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "ecm.h"
#include "spmi_fetcher.h"
#include "DALSys.h"
#include "DalDevice.h"
#include "DALFramework.h"
#include "DDISpmi.h"
#include "ecm_frame.h"
#include "msg.h"
#include "pm_version.h"
#include "ecm_diag.h"
#include "busywait.h"
#include <stdarg.h>
#include "ecm_log.h"
#include "spmif_cfgdata.h"
#include "bam.h"
#include "DALGlbCtxt.h"
#include "pm_rtc.h"
#include "spmif_hw.h"
#include "pmapp_ecm.h"
#define MAX_ECM_READERS   5

struct dma_buffer
{
    uint32 va;
    uint32 pa;
    uint32 size;
};

struct buffer
{
    uint32 b[BUFFER_SZ_WRDS];
};
/* DMA buffers layout */
struct dma_buffer_layout
{
    uint32 descriptor_fifo[DESC_SZ_WRDS];
    struct buffer buffers[NR_BUFFERS];
};

enum ecm_state
{
    ECM_NOT_READY = 0,    /* ECM not initialized */
    ECM_INITIALIZED = 1,  /* ECM ready to be used */
	ECM_IDLE=2,           /* ECM is idle state, still active*/
	ECM_BUSY=3,           /* ECM BUSY, active */
    ECM_STOPPED=4         /* ECM stopped. PFIFO disabled, ECM global enable = 0*/
};

struct ecm_reader
{
	enum ecm_logger type;
	ecm_cb_type cb;
	int valid;
};

struct ecm_dev_type
{
	enum ecm_state state;	
    uint32 frame_size;
    uint32 frames_per_eot;
    uint8 ecm_sample_rate;
    uint32 enabled_ldo_cnt;
    uint32 enabled_buck_cnt;
    uint32 enabled_misc_cnt;
	DalDeviceHandle* spmi_handle;
	DALSYSMemInfo data_slab_meminfo;
	void* data_slab_memh;
    struct dma_buffer mem_slab;
    struct dma_buffer_layout* va_layout;
    struct dma_buffer_layout* pa_layout;
    uint32 eot_length;
};


static struct ecm_dev_type ecm_dev;
static struct ecm_reader ecm_readers[MAX_ECM_READERS];
static int ecm_readers_index=0;
static struct dma_buffer dma_buffers[NR_BUFFERS];

/* helper functions */
static uint32 ecm_vthp_mv(void);
static uint32 ecm_vref_mv(void);
static uint32 ecm_clk_freq_hz(void);
static int ecm_reset_pfifo(void);
static uint32 ecm_vthp_mv(void);
static void ecm_update_enabled_status(void);
/** A buffer returned to its clients. It's important to keep the VA as the first
 *
 *  field in the structure
 */
static void spmif_ecm_cb(struct dma_buffer* buf, uint32 size)
{
    int i;
	for (i=0; i < MAX_ECM_READERS; i++) {
		if (ecm_readers[i].valid && ecm_readers[i].cb){
			ecm_readers[i].cb((void*)buf->va, size);
			if (spmif_is_idle()) {
				//notify that there is no data to the client
				//through NULL data
				ecm_readers[i].cb(NULL, 0);
			}
		}
	}
	spmif_queue_buffer(buf->pa, buf->size, buf);
}
    
static void ecm_dev_cb(bam_result_type bam_result)
{
	if (bam_result.event == BAM_EVENT_EOT) {
        struct dma_buffer* buf = (struct dma_buffer*)bam_result.data.xfer.xfer_cb_data;
        spmif_ecm_cb(buf, bam_result.data.xfer.iovec.buf_size);
	}
}

static void ecm_queue_all_buffers(void) 
{
    int i = 0;
    for (i = 0 ; i < NR_BUFFERS; i++) {
        spmif_queue_buffer(dma_buffers[i].pa, dma_buffers[i].size, &dma_buffers[i]);
    }
}

static int ecm_reset_pfifo(void)
{
    uint8 data = 3;
        data = 3;
        if (DAL_SUCCESS != DalSpmi_WriteLong(ecm_dev.spmi_handle,
                    0,  /* slave id*/
                    SPMI_BUS_ACCESS_PRIORITY_LOW,  
                    0x3D6B,
                    &data, /* value */
                    1)) {
        return 1;
    }
    return 0;
}

static uint32 ecm_vthp_mv(void)
{
    return VTHP_MV;
}

static uint32 ecm_vref_mv(void)
{
    uint8 data =0xde;
    uint32 bytes_read = 0;

    if (DAL_SUCCESS != DalSpmi_ReadLong(ecm_dev.spmi_handle,
				1,  /* slave id*/
				SPMI_BUS_ACCESS_PRIORITY_LOW,  
				0x1052,
				&data, /* value */
				1, &bytes_read)) {
        return 1;
	}
    if (data < 0x3) {
        return ecm_vref_map[data];
    } else {
        return 1;
    }
}

static uint32 ecm_clk_freq_hz(void)
{
     uint8 data =0xde;
    uint32 bytes_read = 0;

    if (DAL_SUCCESS != DalSpmi_ReadLong(ecm_dev.spmi_handle,
				0,  /* slave id*/
				SPMI_BUS_ACCESS_PRIORITY_LOW,  
				0x3b50,
				&data, /* value */
				1, &bytes_read)) {
        return 1;
	}
    if ( data <=15) {
        return ecm_clk_freq_map[data];
    } else {
        return 1;
    }
}

#ifdef LOCAL_ENABLE
    /* ECM local enables. This should be set in 
     * the PMIC SBL init sequence 
     */
int spmi_write(uint32 address, uint32 slave_id, uint8 value)
{
    if (DAL_SUCCESS != DalSpmi_WriteLong(ecm_dev.spmi_handle,
                slave_id,  /* slave id*/
                SPMI_BUS_ACCESS_PRIORITY_LOW,  
                address,
                &value, /* value */
                1)) {
        return 1;
			}
    return 0;
		}

static int ecm_local_enables(void)
{
#define CHECK(x)   if (x) goto exit

    CHECK(spmi_write(0x7442, 0, 6));
    CHECK(spmi_write(0x7441, 0, 6));
    CHECK(spmi_write(0x1582, 1, 1));
    CHECK(spmi_write(0x1882, 1, 1));
    CHECK(spmi_write(0x1b82, 1, 1));
    CHECK(spmi_write(0x1e82, 1, 1));
    CHECK(spmi_write(0x2182, 1, 1));
    CHECK(spmi_write(0x10d0, 1, 0xA5));
    CHECK(spmi_write(0x10f2, 1, 0x2));
    CHECK(spmi_write(0x3bd0, 0, 0xa5));
    CHECK(spmi_write(0x3b50, 0, 0xb));
    CHECK(spmi_write(0x3a54,0, 0));
    CHECK(spmi_write(0x3a55,0, 4));
    CHECK(spmi_write(0x3a56,0, 0));
    CHECK(spmi_write(0x3a50,0, 0xf4));

    CHECK(spmi_write(0x1580,1, 0x80));
    CHECK(spmi_write(0x1880,1, 0x80));
    CHECK(spmi_write(0x1b80,1, 0x80));
    CHECK(spmi_write(0x1e80,1, 0x80));
    CHECK(spmi_write(0x2180,1, 0x80));
    CHECK(spmi_write(0x0980,0, 0x80));
    CHECK(spmi_write(0x4080,1, 0x80));
    CHECK(spmi_write(0x4180,1, 0x80));
    CHECK(spmi_write(0x4280,1, 0x80));
    CHECK(spmi_write(0x4380,1, 0x80));
    CHECK(spmi_write(0x4480,1, 0x80));
    CHECK(spmi_write(0x4580,1, 0x80));
    CHECK(spmi_write(0x4780,1, 0x80));
    CHECK(spmi_write(0x4880,1, 0x80));
    CHECK(spmi_write(0x4980,1, 0x80));

    CHECK(spmi_write(0x60d0,0, 0xa5));
    CHECK(spmi_write(0x60e3,0, 0x1));
    return  0;
exit:
    return 1;
}
#else

static int ecm_local_enables(void)
{
    return 0;
}
#endif

#define ALIGN_8(x)   ((x +7) & (~7))
static int allocate_buffer_space(void)
{
    int i;
    /* allocate a buffer that's aligned to 8 bytes */
    uint32 buf_size = TOTAL_BUF_SIZE + 7;
    if (DAL_SUCCESS != DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED|DALSYS_MEM_PROPS_PHYS_CONT,
                DALSYS_MEM_ADDR_NOT_SPECIFIED,
                DALSYS_MEM_ADDR_NOT_SPECIFIED,
                buf_size,
                &ecm_dev.data_slab_memh,
                NULL)) {
        ecm_log(ECM_LOG_ERROR, "Error allocating memory for descripor fifo\n");
        return 1;
    } 
    DALSYS_MemInfo(ecm_dev.data_slab_memh, &ecm_dev.data_slab_meminfo); 
    /* get the aligned buffer */
    ecm_dev.mem_slab.pa = ALIGN_8(ecm_dev.data_slab_meminfo.PhysicalAddr);
    ecm_dev.mem_slab.va = ALIGN_8(ecm_dev.data_slab_meminfo.VirtualAddr);
    ecm_dev.va_layout = (struct dma_buffer_layout*)ecm_dev.mem_slab.va;
    ecm_dev.pa_layout = (struct dma_buffer_layout*)ecm_dev.mem_slab.pa;

    for (i=0 ; i < NR_BUFFERS; i++) {
        dma_buffers[i].va = (uint32)&ecm_dev.va_layout->buffers[i];
        dma_buffers[i].pa = (uint32)&ecm_dev.pa_layout->buffers[i];
        dma_buffers[i].size = BUFFER_SZ_WRDS*4;
        memset((void*)dma_buffers[i].va, 0, dma_buffers[i].size);
    }
    return 0;
}

void ecm_get_enabled_regulator_cnt(uint8* buck_cnt, uint8* ldo_cnt, uint8* misc_cnt)
{
    *buck_cnt = ecm_dev.enabled_buck_cnt;
    *ldo_cnt = ecm_dev.enabled_ldo_cnt;
    *misc_cnt = ecm_dev.enabled_misc_cnt;
}

static int ecm_get_programmed_sample_rate(uint8* sample_rate) 
{
    uint8 data;
    uint32 address = 0x3a54;
    uint32 bytes_read;
    if (DAL_SUCCESS != DalSpmi_ReadLong(ecm_dev.spmi_handle,
				0,  /* slave id*/
				SPMI_BUS_ACCESS_PRIORITY_LOW,  
                0x3A56,
				&data, /* value */
				1, &bytes_read)) {
        return 1;
	}
    /* the default sample rate is set to sample_a */
    if (data == 1) {
        address = 0x3a55;
    }
    if (DAL_SUCCESS != DalSpmi_ReadLong(ecm_dev.spmi_handle,
				0,  /* slave id*/
				SPMI_BUS_ACCESS_PRIORITY_LOW,  
                address,
				&data, /* value */
				1, &bytes_read)) {
        return 1;
	}
    *sample_rate = data;
    return 0;
}

void ecm_program_poll_n_read_gap(uint8 prg_sample_rate)
{
    uint32 poll_gap_count,read_gap_count;

    /*poll_gap is set to 4*sample period which means half of PFIFO is filled. 
     4 ecm frames need to trnasfer 61 times when 8 bytes are transferred per read.
      read_gap is set to meet the goal with margin*/
    poll_gap_count = 10;
    read_gap_count = 5;

    if (prg_sample_rate <= 9)
    {
        poll_gap_count *= ((uint32)0X1 << prg_sample_rate);
        read_gap_count *= ((uint32)0X1 << prg_sample_rate);
    }

    spmif_hw_set_pollgap_time(poll_gap_count);
    spmif_hw_set_readgap_time(read_gap_count);

}

static void ecm_update_enabled_status(void)
{
   pmapp_ecm_enable_status_type ecm_enabled_cnt;
   pm_err_flag_type   pmic_api_ret;

   pmic_api_ret =  pmapp_ecm_enable_status(&ecm_enabled_cnt);

   if (pmic_api_ret == PM_ERR_FLAG__SUCCESS)
   {
      ecm_dev.enabled_buck_cnt = ecm_enabled_cnt.smps_count;
      ecm_dev.enabled_ldo_cnt = ecm_enabled_cnt.ldo_count;
      ecm_dev.enabled_misc_cnt = ecm_enabled_cnt.ext_ch_count;
   }
   else{
       ecm_log(ECM_LOG_ERROR,"Error getting PMIC ECM enable status");

   }

}

static  struct otp_region {
        DALGLB_HEADER             dal_glb_header;
        uint32 format;
        uint8  otp[60];
    } ecm_otp_data;


static int ecm_update_calibration_constants(void)
{
    struct otp_region *ecm_otp_shared = &ecm_otp_data ;
    
    if (DAL_SUCCESS != DALGLBCTXT_FindCtxt("PM_ECM_INFO",(void**)&ecm_otp_shared)) {
        ecm_log(ECM_LOG_ERROR,"Error getting dal global context");
        return 1;
    }

  
    /* start updating opt constants */
    if (ecm_otp_shared->otp[56] == 0xFF) {
        ecm_log(ECM_LOG_INFO,"OTP region not programmed, using defaults");
        return 0;
    } 

    if (ecm_otp_shared->otp[56] == 0x1) {
        ecm_log(ECM_LOG_INFO,"Updating buck coefficients");
        regulator_map[0].consts[0] = ecm_otp_shared->otp[0];
        regulator_map[1].consts[0] = ecm_otp_shared->otp[4];
        regulator_map[2].consts[0] = ecm_otp_shared->otp[8];
        regulator_map[3].consts[0] = ecm_otp_shared->otp[12];
        regulator_map[4].consts[0] = ecm_otp_shared->otp[16];
        return 0;
    }

    if (ecm_otp_shared->otp[56] == 0x2) {
        ecm_log(ECM_LOG_INFO,"Updating LDO coefficients");
        regulator_map[5].consts[0] = ((uint16)ecm_otp_shared->otp[21]<<8)| (uint16)ecm_otp_shared->otp[20];
        regulator_map[5].consts[1] = ((uint16)ecm_otp_shared->otp[23]<<8)| (uint16)ecm_otp_shared->otp[22];

        regulator_map[6].consts[0] = ((uint16)ecm_otp_shared->otp[25]<<8)| (uint16)ecm_otp_shared->otp[24];
        regulator_map[6].consts[1] = ((uint16)ecm_otp_shared->otp[27]<<8)| (uint16)ecm_otp_shared->otp[26];

        regulator_map[7].consts[0] = ((uint16)ecm_otp_shared->otp[29]<<8)| (uint16)ecm_otp_shared->otp[28];
        regulator_map[7].consts[1] = ((uint16)ecm_otp_shared->otp[31]<<8)| (uint16)ecm_otp_shared->otp[30];

        regulator_map[8].consts[0] = ((uint16)ecm_otp_shared->otp[33]<<8)| (uint16)ecm_otp_shared->otp[32];
        regulator_map[8].consts[1] = ((uint16)ecm_otp_shared->otp[35]<<8)| (uint16)ecm_otp_shared->otp[34];

        regulator_map[9].consts[0] = ((uint16)ecm_otp_shared->otp[37]<<8)| (uint16)ecm_otp_shared->otp[36];
        regulator_map[9].consts[1] = ((uint16)ecm_otp_shared->otp[39]<<8)| (uint16)ecm_otp_shared->otp[38];

        regulator_map[10].consts[0] = ((uint16)ecm_otp_shared->otp[41]<<8)| (uint16)ecm_otp_shared->otp[40];
        regulator_map[10].consts[1] =  ((uint16)ecm_otp_shared->otp[43]<<8)| (uint16)ecm_otp_shared->otp[42];

        regulator_map[11].consts[0] = ((uint16)ecm_otp_shared->otp[45]<<8)| (uint16)ecm_otp_shared->otp[44];
        regulator_map[11].consts[1] =  ((uint16)ecm_otp_shared->otp[47]<<8)| (uint16)ecm_otp_shared->otp[46];

        regulator_map[12].consts[0] = ((uint16)ecm_otp_shared->otp[49]<<8)| (uint16)ecm_otp_shared->otp[48];
        regulator_map[12].consts[1] =  ((uint16)ecm_otp_shared->otp[51]<<8)| (uint16)ecm_otp_shared->otp[50];

        regulator_map[13].consts[0] = ((uint16)ecm_otp_shared->otp[53]<<8)| (uint16)ecm_otp_shared->otp[52];
        regulator_map[13].consts[1] =  ((uint16)ecm_otp_shared->otp[55]<<8)| (uint16)ecm_otp_shared->otp[54];
        return 0;
    }
  
    if ((ecm_otp_shared->otp[56] >= 0x3) && (ecm_otp_shared->otp[56] < 0XFF) ) {
        ecm_log(ECM_LOG_INFO,"Updating buck coefficients");
        regulator_map[0].consts[0] = ecm_otp_shared->otp[0];
        regulator_map[1].consts[0] = ecm_otp_shared->otp[4];
        regulator_map[2].consts[0] = ecm_otp_shared->otp[8];
        regulator_map[3].consts[0] = ecm_otp_shared->otp[12];
        regulator_map[4].consts[0] = ecm_otp_shared->otp[16];

        ecm_log(ECM_LOG_INFO,"Updating LDO coefficients");
        regulator_map[5].consts[0] = ((uint16)ecm_otp_shared->otp[21]<<8)| (uint16)ecm_otp_shared->otp[20];
        regulator_map[5].consts[1] = ((uint16)ecm_otp_shared->otp[23]<<8)| (uint16)ecm_otp_shared->otp[22];

        regulator_map[6].consts[0] = ((uint16)ecm_otp_shared->otp[25]<<8)| (uint16)ecm_otp_shared->otp[24];
        regulator_map[6].consts[1] = ((uint16)ecm_otp_shared->otp[27]<<8)| (uint16)ecm_otp_shared->otp[26];

        regulator_map[7].consts[0] = ((uint16)ecm_otp_shared->otp[29]<<8)| (uint16)ecm_otp_shared->otp[28];
        regulator_map[7].consts[1] = ((uint16)ecm_otp_shared->otp[31]<<8)| (uint16)ecm_otp_shared->otp[30];

        regulator_map[8].consts[0] = ((uint16)ecm_otp_shared->otp[33]<<8)| (uint16)ecm_otp_shared->otp[32];
        regulator_map[8].consts[1] = ((uint16)ecm_otp_shared->otp[35]<<8)| (uint16)ecm_otp_shared->otp[34];

        regulator_map[9].consts[0] = ((uint16)ecm_otp_shared->otp[37]<<8)| (uint16)ecm_otp_shared->otp[36];
        regulator_map[9].consts[1] = ((uint16)ecm_otp_shared->otp[39]<<8)| (uint16)ecm_otp_shared->otp[38];

        regulator_map[10].consts[0] = ((uint16)ecm_otp_shared->otp[41]<<8)| (uint16)ecm_otp_shared->otp[40];
        regulator_map[10].consts[1] =  ((uint16)ecm_otp_shared->otp[43]<<8)| (uint16)ecm_otp_shared->otp[42];

        regulator_map[11].consts[0] = ((uint16)ecm_otp_shared->otp[45]<<8)| (uint16)ecm_otp_shared->otp[44];
        regulator_map[11].consts[1] =  ((uint16)ecm_otp_shared->otp[47]<<8)| (uint16)ecm_otp_shared->otp[46];

        regulator_map[12].consts[0] = ((uint16)ecm_otp_shared->otp[49]<<8)| (uint16)ecm_otp_shared->otp[48];
        regulator_map[12].consts[1] =  ((uint16)ecm_otp_shared->otp[51]<<8)| (uint16)ecm_otp_shared->otp[50];

        regulator_map[13].consts[0] = ((uint16)ecm_otp_shared->otp[53]<<8)| (uint16)ecm_otp_shared->otp[52];
        regulator_map[13].consts[1] =  ((uint16)ecm_otp_shared->otp[55]<<8)| (uint16)ecm_otp_shared->otp[54];

        return 0;
    }
    return 0;
}


void ecm_dev_init(void)
{
	DALResult ret;
    int r;
    uint32 channel;
    /* initilize the initial ecm numbers */
    /* This should change if we decide to enable/disable ECMs on the fly*/
    ecm_dev.enabled_ldo_cnt = 8;
    ecm_dev.enabled_buck_cnt = 5;
    ecm_dev.enabled_misc_cnt = 1;
    ecm_update_enabled_status();
	ret = DAL_SpmiDeviceAttach("DALDEVICEID_SPMI_DEVICE", &ecm_dev.spmi_handle);
	if (ret != DAL_SUCCESS){
		ecm_log(ECM_LOG_ERROR, "Cannot attch to SPMI driver\n");
		return;
	}

	ret = DalDevice_Open(ecm_dev.spmi_handle, DAL_OPEN_SHARED| DAL_OPEN_READ | DAL_OPEN_WRITE);
	if (ret != DAL_SUCCESS) {
		ecm_log(ECM_LOG_ERROR, "Cannot open SPMI driver\n");
		return;
	}
    if (DAL_SUCCESS != DalSpmi_ConfigureFetcherChannel(ecm_dev.spmi_handle,PMIC_ECM_PERIPH_ID, 1, &channel)) {
        ecm_log(ECM_LOG_ERROR,"Error getting ECM channel");
    }

    /* read the sample rate that's set in PMIC. Use this as the default */
    if (ecm_get_programmed_sample_rate(&ecm_dev.ecm_sample_rate)) {
        ecm_log(ECM_LOG_ERROR, "Error getting the programmed sample rate from PMIC");
    }

    if (allocate_buffer_space()) {
        return;
    }
    if (spmif_dev_init((uint32)&ecm_dev.va_layout->descriptor_fifo,
            (uint32)&ecm_dev.pa_layout->descriptor_fifo,
            DESC_SZ_WRDS*4,
            ecm_dev_cb,
            channel)) {
        return;
    }
    /* local enables */
    r = ecm_local_enables();
    if (r) {
        ecm_log(ECM_LOG_ERROR,"Error enabling local enables");
    }
    /* update OTP constants */
	
    if (ecm_update_calibration_constants()) {
        ecm_log(ECM_LOG_ERROR,"Failed to update constants");
    }
    /* all good, transition to idle state */
    ecm_dev.state = ECM_INITIALIZED;
    spmif_clk_on();
    ecm_queue_all_buffers();
    spmif_clk_off();
}

void ecm_dev_deinit(void)
{
    spmif_dev_deinit();
	DalDevice_Close(ecm_dev.spmi_handle);
	DAL_DeviceDetach(ecm_dev.spmi_handle);
}

static int ecm_global_enable(int en)
{
	uint8 data = 0x80;
	/* enable ECM periperal, global enable = 1 */
	if (!en)
		data = 0;

	if (DAL_SUCCESS != DalSpmi_WriteLong(ecm_dev.spmi_handle,
				0,  /* slave id*/
				SPMI_BUS_ACCESS_PRIORITY_LOW,  
				0x3a46,
				&data, /* value */
				1)) {
        return 1;
	}
    return 0;
}

uint32 ecm_get_sample_rate(void)
{
    return ecm_dev.ecm_sample_rate;
}

uint32 ecm_get_frames_per_eot(void)
{
    return ecm_dev.frames_per_eot;
}

uint32 ecm_get_ecm_frame_size(void)
{
    uint32 s = sizeof(struct ecm_frame_hdr) +
               sizeof(struct ecm_buck_data)*ecm_dev.enabled_buck_cnt +
               sizeof(struct ecm_ldo_data)*ecm_dev.enabled_ldo_cnt +
               sizeof(struct ecm_qpoet_data)*ecm_dev.enabled_misc_cnt;
   
    return s;
}

void ecm_fill_calibration_data(ecm_regulator_info_type *pkt)
{
    pkt->vthp_mv = ecm_vthp_mv();
    pkt->vref_mv = ecm_vref_mv();
    pkt->ecm_clk_freq = ecm_clk_freq_hz(); 
    pkt->i_ref   =  I_REF_UA;
    pkt->ldo_rtrim = LDO_RTRIM;
    pkt->ldo_mmax = LDO_MMAX;
    pkt->qpoet_rtrim = QPOET_RTRIM;
    pkt->qpoet_mmax =  QPOET_MMAX;
    pkt->nr_regulators = NR_REGULATORS;
    memcpy(pkt->regulators, &regulator_map, sizeof(regulator_map));
}

uint32 ecm_fill_pmic_info(ecm_diag_pkt_pmic_dev_info_type* pkt)
{
    pm_device_info_type device_info; 
    pm_pbs_info_type pbs_info;

    if (PM_ERR_FLAG__SUCCESS == pm_get_pmic_info(0, &device_info)){
        pkt->pmic_metal_rev = device_info.nPmicMetalRevision;
        pkt->pmic_all_layer_rev = device_info.nPmicAllLayerRevision;
        pkt->pmic_model = (uint32)device_info.ePmicModel;
    }
    if (PM_ERR_FLAG__SUCCESS == pm_get_pbs_info(0, &pbs_info)) {
        memcpy(pkt->lot_id, pbs_info.nLotId, PM_PBS_INFO_NUM_LOT_IDS);
        pkt->rom_version = pbs_info.nPBSROMVersion;
        pkt->ram_version = pbs_info.nPBSRAMVersion;
        pkt->fab_id = pbs_info.nFabId;
        pkt->wafer_id = pbs_info.nWaferId;
        pkt->x_coord = pbs_info.nXcoord;
        pkt->y_coord = pbs_info.nYcoord;
        pkt->test_pgm_rev = pbs_info.nTestPgmRev;
    }
    return 0;
}

/**
 * @brief  open a ecm logger, and get a handle. The logger can be SW or 
 *         a special HW block ( SPMI fetcher). Use the returned 'fd' in
 *         subsequent calls
 * @param[in]  logger  which logger to use \see ecm_logger
 * @return     fd      handle
 */
ecm_ret ecm_open(enum ecm_logger logger, int* fd)
{
	if (logger == LOGGER_SW) {
        ecm_log(ECM_LOG_ERROR,"ECM non-fetcher mode not supported");
		return ECM_ERROR;   //not implemented yet
    }

    if (ecm_dev.state == ECM_NOT_READY) {
        ecm_log(ECM_LOG_ERROR, "ECM not initialied properly");
        return ECM_ERROR;
    }

	if (ecm_dev.state == ECM_BUSY) {
        ecm_log(ECM_LOG_ERROR,"ECM busy");
		return ECM_ERROR;
	}

	if (logger == LOGGER_SPMIF) {
		*fd = ecm_readers_index;
		ecm_readers[*fd].type = LOGGER_SPMIF;
		ecm_readers[*fd].valid = 1;
		ecm_readers_index++;
	}
    ecm_dev.state = ECM_STOPPED;
	return ECM_OK;
}

/* To make the APEX parsing easier, make sure we have an integer
 * multiple of frames within an EOT and it's a multiple of 8. The 
 * multiple of 8 restriction comes from the way SPMI FETCHER works.
 * This needs to be changed in the next revision of SPMI FETCHER.
 *
 */
static uint32 adjust_eot_size(uint32 frame_size, uint32 buffer_size)
{
    uint32 n;
    n = buffer_size/frame_size;
    while (n && n*frame_size%8) {
        n--;
    }
    return (n*frame_size);
}



/**
 * @brief  read all the available ecm data. 
 *
 * The user has to call this api multiple times, till there is no more 
 * data to read. This call will terminate after a finite number of
 * invocations ( will only fetch the available data, and will not 
 * generate data ). 
 *
 * @param[in]  fd      handle
 * @param[out] buffer  pointer to the buffer containing the data
 * @param[out  size    size of the valid data in the buffer
 * @return     size    size of the data. 0 means end-of-data
 *
 * @note this call is a blocking call.
 */
ecm_ret ecm_read_single(int fd, uint32** buffer, uint32* size)
{
    uint8  cal_ratio ,cal_ratio_default;
    uint32  bytes_read;
    int rc = 0;
    struct ecm_reader* reader;
	reader = &ecm_readers[fd];
    
    if (ecm_dev.state != ECM_STOPPED) {
        ecm_log(ECM_LOG_ERROR,"ECM not stopped. Issue ECM stop first");
        return ECM_ERROR;
    } 
    ecm_update_enabled_status();
    ecm_dev.frame_size = ecm_get_ecm_frame_size();

  
	if (reader->type == LOGGER_SPMIF) {
		ecm_dev.state = ECM_BUSY;
        spmif_clk_on();
        /* reset pfifo if there is an error */
        rc = ecm_reset_pfifo();
        if (rc < 0) {
            ecm_log(ECM_LOG_ERROR,"Error resetting PFIFO");
            return ECM_ERROR;
        }
        /*Read SBL programmed  buck calibration period ratio*/
        if (DAL_SUCCESS != DalSpmi_ReadLong(ecm_dev.spmi_handle,
                    0,  /* slave id*/
                    SPMI_BUS_ACCESS_PRIORITY_LOW,  
                    0x3A50,
                    &cal_ratio_default, /* value */
                    1, &bytes_read)) {
            return ECM_ERROR;
        }

        /*Change buck calibration period ratio to CNT_1*/
        cal_ratio = 0X80;
        if (DAL_SUCCESS != DalSpmi_WriteLong(ecm_dev.spmi_handle,
                    0,  /* slave id*/
                    SPMI_BUS_ACCESS_PRIORITY_LOW,  
                    0x3A50,
                    &cal_ratio, /* value */
                    1)) {
            return ECM_ERROR;
        }


        ecm_dev.eot_length = adjust_eot_size(ecm_dev.frame_size, BUFFER_SZ_WRDS*4);
        ecm_dev.frames_per_eot = ecm_dev.eot_length/ecm_get_ecm_frame_size(); 
        spmif_set_eot_length(ecm_dev.eot_length);


		ecm_global_enable(1);
        /* wait for 40 ms and disable ecm */
        busywait(40*1000);
        /* disable ECM after 40 ms, then read the ECM frames
         * from PFIFO*/
        ecm_global_enable(0);

        /*Change buck calibration period ratio back to SBL programmed value*/
        
        if (DAL_SUCCESS != DalSpmi_WriteLong(ecm_dev.spmi_handle,
                    0,  /* slave id*/
                    SPMI_BUS_ACCESS_PRIORITY_LOW,  
                    0x3A50,
                    &cal_ratio_default, /* value */
                    1)) {
            return ECM_ERROR;
        }

        
		rc = spmif_start(SPMIF_OP_ONESHOT);
        ecm_dev.state = ECM_STOPPED;
		if (rc < 0) {
            ecm_log(ECM_LOG_ERROR,"Error starting spmi fetcher in oneshot mode");
            return ECM_ERROR;
		}
	} else if (reader->type == LOGGER_SW) {
        return ECM_ERROR;
	}
    return ECM_OK;
}

/**
 * @brief   change the ECM sample rate. 
 *
 * @param[in]  sample_rate     new sample rate
 *
 */
ecm_ret ecm_set_sample_rate(uint8 sample_rate)
{
    uint8 data;
    uint8 rate = (uint8)sample_rate;


    if (ecm_dev.state == ECM_IDLE || ecm_dev.state == ECM_BUSY) {
        ecm_log(ECM_LOG_ERROR,"ECM busy. Cannot change sample rate");
        return ECM_ERROR;
    }
    
   if (sample_rate > ECM_SAMPLE_RATE_MAX) {
        ecm_log_1(ECM_LOG_ERROR,"Unsupported sample rate %d",sample_rate);
        return ECM_ERROR;
    }
    if (ecm_dev.state != ECM_BUSY)
    {
         spmif_clk_on();
    }
   

	if (DAL_SUCCESS != DalSpmi_WriteLong(ecm_dev.spmi_handle,
				0,  /* slave id*/
				SPMI_BUS_ACCESS_PRIORITY_LOW,  
                0x3A54,
				&rate, /* value */
				1)) {
        return ECM_ERROR;
	}
    /* select sample A*/
    data = 0;
    if (DAL_SUCCESS != DalSpmi_WriteLong(ecm_dev.spmi_handle,
				0,  /* slave id*/
				SPMI_BUS_ACCESS_PRIORITY_LOW,  
                0x3A56,
				&data, /* value */
				1)) {
        return ECM_ERROR;
	}
    
    ecm_dev.ecm_sample_rate = sample_rate;
    ecm_program_poll_n_read_gap(ecm_dev.ecm_sample_rate);
    if (ecm_dev.state != ECM_BUSY)
    {
         spmif_clk_off();
    }

    return ECM_OK;
}
/**
 * @brief  read the ecm data continously from pmic
 *
 * The user has to call this api multiple times, till the user decides
 * to stop fetching data. When the user decides to not fetch any more
 * data, he/she has to call ecm_stop, and then continue to do ecm_read
 * till there is no more data to read.
 *
 * @param[in]  fd      handle
 * @param[out] buffer  pointer to the buffer containing the data
 * @param[out  size    size of the valid data in the buffer
 * @return     size    size of the data. 0 means end-of-data
 *
 * @note this call is a blocking call.
 */

ecm_ret ecm_read(int fd, uint32** buffer, uint32* size)
{
	struct ecm_reader* reader;
    int rc;
	reader = &ecm_readers[fd];
    
    if (ecm_dev.state != ECM_STOPPED) {
        ecm_log(ECM_LOG_ERROR,"ECM not stopped. Issue ECM stop first");
        return ECM_ERROR;
    } 
    ecm_update_enabled_status();
    ecm_dev.frame_size = ecm_get_ecm_frame_size();

	if (reader->type == LOGGER_SPMIF) {
        spmif_clk_on();
        ecm_global_enable(0);
        /* reset pfifo if there is an error */
        rc = ecm_reset_pfifo();
        if (rc < 0) {
            ecm_log(ECM_LOG_ERROR,"Error resetting PFIFO");
            return ECM_ERROR;
        }
         ecm_program_poll_n_read_gap(ecm_dev.ecm_sample_rate);
        ecm_dev.eot_length = adjust_eot_size(ecm_dev.frame_size, BUFFER_SZ_WRDS*4);
        ecm_dev.frames_per_eot = ecm_dev.eot_length/ecm_get_ecm_frame_size(); 

        spmif_set_eot_length(ecm_dev.eot_length);
 
		rc = ecm_global_enable(1);
        if (rc) {
            ecm_global_enable(0);
            return ECM_ERROR;
        }
		rc = spmif_start(SPMIF_OP_STREAMING);
		if (rc){
			//TODO: set state to ERROR
			ecm_global_enable(0);
			return ECM_ERROR;
		}
		ecm_dev.state = ECM_BUSY;
	} else if (reader->type == LOGGER_SW) {
        return ECM_ERROR;
	}
	return ECM_OK;
}

/**
 * @brief  stop generating ecm data. 
 *
 * There can be data already generated in the buffers/fifo. The user
 * should call ecm_read to empty this buffer/fifo.
 *
 * @param[in]  fd      handle
 */
ecm_ret ecm_stop(int fd)
{
	ecm_ret rc = ECM_OK;
    int count = 0;

    if (ecm_dev.state == ECM_STOPPED) {
        ecm_log(ECM_LOG_ERROR, "ECM already stopped");
        return ECM_ERROR;
    }
	if (ecm_global_enable(0)){
        rc = ECM_ERROR;
        goto exit;
    }
	if (spmif_stop()) {
        rc = ECM_ERROR;
        goto exit;
	}
    /* wait till spmif goes to idle state */
    /* wait for 100ms before giving up*/
    while (!spmif_is_idle() && count < 100) {
        count++;
        busywait(1000);
	}

    if (count >= 100) {
        if (!spmif_is_idle()) {
            ecm_log(ECM_LOG_ERROR,"timeout while waiting for spmi fetcher idle state");
            rc = ECM_ERROR;
        }
    }
    
exit:
    ecm_dev.state = ECM_STOPPED;
    spmif_clk_off();
	return rc;
}

/**
 * @brief  close the logger
 *
 * The handle is not valid after close returns
 *
 * @param[in]  fd      handle
 */
void ecm_close(int fd)
{
    if (fd < MAX_ECM_READERS) {
        ecm_readers[fd].valid = 0;
        ecm_readers_index--;
    }
    return;
}

/**
 * @brief  register a read notifier callback
 *
 * Register a callback that gets invoked when data is ready
 *
 * @param[in]  fd      handle
 * @param[in]  cb      callback
 *
 */
ecm_ret ecm_register_read_notifier(int fd, ecm_cb_type cb)
{
	struct ecm_reader* reader;
	reader = &ecm_readers[fd];
	reader->cb = cb;
	return ECM_OK;

}

ecm_ret  ecm_get_pmic_rtc_count(uint32* count)
{
    uint8 time[3]; 
    uint32 bytes_read = 0;

    if (DAL_SUCCESS != DalSpmi_ReadLong(ecm_dev.spmi_handle,
                0,  /* slave id*/
                SPMI_BUS_ACCESS_PRIORITY_LOW,  
                0x6051,
                time, /* value */
                3, &bytes_read)) {
        return ECM_ERROR; 
    }
    if (bytes_read < 3) {
        ecm_log_1(ECM_LOG_ERROR,"RTC counter read error, bytes read %d",bytes_read);
        return ECM_ERROR;
    }
    *count =  time[0]|(time[1]<<8)|(time[2]<<16);
    return ECM_OK;
}


