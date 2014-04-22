#include "ecm.h"
#include "bam.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DDIHWIO.h"
#include "DDIClock.h"
#include "diag.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "msg.h"
#include "DALFramework.h"
#include "DALSys.h"
#include "busywait.h"
#define ECM_CONSUMER_PIPE 12
#define SPMIF_BAM_IRQ  126
#define BAMDMA_PA
#define NUM_DESCS  4
#define DESC_SZ  (8*8/2)
#define DATA_SZ  (0x200*2)
#define SLAB_SZ  (DESC_SZ + DATA_SZ)
#define SPMIF_BAM_PA 0xF9984000


#define SPMIF_TEST_START_TRANSFER  (256 + 20)
#ifdef BAMDMA
enum bamdma_clock_ids
{
	BAM_DMA_CLK=0,
    PMEM_ACLK=1,
    DFAB_HCLK=2,
    BAMDMA_MAX_CLK_ID
};


static DalDeviceHandle* hDalClock = NULL;
static ClockIdType  clock_ids[BAMDMA_MAX_CLK_ID];
static bam_handle bam_h;
static bam_handle pipe_h;
static bam_config_type bam_cfg;
static bam_pipe_config_type pipe_cfg;
static bam_callback_type pipe_cb;
static void* slab_memh;
static DALSYSMemInfo slab_meminfo;
static struct testdrv_mem_part* mem_part;
static struct testdrv_mem_part* mem_part_phy;
#endif
static DALSYSWorkLoopHandle wl;
static DALSYSEventHandle trigger_event;   
struct testdrv_mem_part
{
	uint32 desc_fifo[NUM_DESCS*2];
	uint32 data[DATA_SZ/4];
};

#pragma pack(1)
struct spmif_test_req_pkt
{
   diagpkt_subsys_header_type hdr;     /**< -- Do not access directly                  */
   uint8 req;                 /**< -- What type of request is being made      */
   uint8 reserved1;
   
};

#pragma pack(1)
struct spmif_test_resp_pkt
{
   diagpkt_subsys_header_type hdr;  /**< -- do not access directly                     */
   char resp[4];
}; 
extern int ecm_diag_client_start_oneshot(void);
extern int ecm_diag_client_start_streaming(void);
PACK(void*) ecm_test_start_transfer(PACK(void*) req_pkt, uint16 pkt_len)
{
	struct spmif_test_req_pkt* req;
	struct spmif_test_resp_pkt * resp;
	int ret = 0;
	if ((NULL == req_pkt) ||  (pkt_len < sizeof(struct spmif_test_req_pkt))) {
		MSG_ERROR("[ECM TEST] invalid packet \n",0,0,0);
		return ((void*)diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
	}
	req = (struct spmif_test_req_pkt*)req_pkt;

	MSG_ERROR("[ECM TEST] recived pkt of length %d and req %d\n",pkt_len,req->req,0);

	if (DAL_SUCCESS != DALSYS_EventCtrl(trigger_event,DALSYS_EVENT_CTRL_TRIGGER))
	{
		MSG_ERROR("[ECM TEST] failed to trigger WL event \n",0,0,0);
	}
	if (req->req == 0x4) {
		//ret= spmif_start(SPMIF_OP_ONESHOT);
		ret = ecm_diag_client_start_oneshot();
	} else if (req->req == 0x5 ){
		ret = ecm_diag_client_start_streaming();
		//ret = spmif_start(SPMIF_OP_STREAMING);
	} else if (req->req == 0x6) {
		//ret = ecm_diag_client_stop();
		//ret = spmif_stop();
	}
	
	resp = (struct spmif_test_resp_pkt*)diagpkt_subsys_alloc(DIAG_SUBSYS_COREBSP , SPMIF_TEST_START_TRANSFER,sizeof(struct spmif_test_resp_pkt));
	if (ret == 0) {
		resp->resp[0] = 'O';
		resp->resp[1] = 'K';
		resp->resp[2] = '\0';
		MSG_ERROR("[ECM TEST] *** Request successfull req %d****\n",req->req,0,0);
	} else if (ret < 0) {
		MSG_ERROR("[ECM TEST] *** Request FAILED req %d ****\n",req->req,0,0);
		resp->resp[0] = 'B';
		resp->resp[1] = 'D';
		resp->resp[2] = '\0';
	}

	return resp;

}

static DALResult ecm_workloop(DALSYSEventHandle eh, void * ctxt) 
{
#ifdef BAMDMA
	if (BAM_SUCCESS != bam_pipe_transfer(pipe_h, (uint32)mem_part_phy->data, 
				DATA_SZ, BAM_IOVEC_FLAG_EOT,  
				(void*)mem_part_phy->data)) {
		MSG_ERROR("[ECM TEST] FAILED to do bam transfer \n",0,0,0);
	} else {
		MSG_ERROR("[ECM TEST] bam sent %d bytes\n",DATA_SZ,0,0);
	}
	busywait(1000);

	//self trigger
	if (DAL_SUCCESS != DALSYS_EventCtrl(trigger_event,DALSYS_EVENT_CTRL_TRIGGER))
	{
		MSG_ERROR("[ECM TEST] failed to trigger WL event \n",0,0,0);
	}
#endif
	
	return DAL_SUCCESS;
}

static const diagpkt_user_table_entry_type spmif_subsys_common_tbl[] =
{
  {SPMIF_TEST_START_TRANSFER, SPMIF_TEST_START_TRANSFER, ecm_test_start_transfer}
};

#if 0
static void ecm_pipe_cb(bam_result_type bam_result)
{
	if (bam_result.event == BAM_EVENT_EOT) {

		if (BAM_SUCCESS != bam_pipe_transfer(pipe_h, bam_result.data.xfer.iovec.buf_pa , 
					bam_result.data.xfer.iovec.buf_size, BAM_IOVEC_FLAG_EOT,  
					(void*)bam_result.data.xfer.iovec.buf_pa)) {
			return;
		}

	}
}
#endif
void spmif_init(void);

int spmif_test_init(void)
{
#ifdef BAM_DMA
	if ( DAL_SUCCESS == DAL_DeviceAttach(DALDEVICEID_CLOCK,&hDalClock))
	{
		if ( DAL_SUCCESS != DalClock_GetClockId(hDalClock,"gcc_bam_dma_ahb_clk",&clock_ids[BAM_DMA_CLK]))
			return 1;
	}

	DalClock_EnableClock(hDalClock,clock_ids[BAM_DMA_CLK]);
	bam_cfg.bam_pa = SPMIF_BAM_PA;
	bam_cfg.bam_irq = SPMIF_BAM_IRQ;
	bam_cfg.bam_irq_mask = BAM_IRQ_HRESP_ERR_EN|BAM_IRQ_ERR_EN;
	bam_cfg.sum_thresh = 1;


	bam_h = bam_init(&bam_cfg, NULL);

	if (!bam_h) {
		return 1;
	}

	/* allocate memory for descriptor fifo and data slab */
	if (DAL_SUCCESS != DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED|DALSYS_MEM_PROPS_PHYS_CONT,
				DALSYS_MEM_ADDR_NOT_SPECIFIED,
				DALSYS_MEM_ADDR_NOT_SPECIFIED,
				SLAB_SZ,
				&slab_memh,
				NULL)) {
		return 1;
	}
	DALSYS_MemInfo(slab_memh, &slab_meminfo);

	mem_part = (struct testdrv_mem_part*)slab_meminfo.VirtualAddr;
	mem_part_phy = (struct testdrv_mem_part*)slab_meminfo.PhysicalAddr;
	memset(mem_part->desc_fifo,'c',DATA_SZ);
	pipe_cfg.options = (BAM_O_DESC_DONE|BAM_O_EOT|BAM_O_ERROR);
	pipe_cfg.dir =  BAM_DIR_CONSUMER;
	pipe_cfg.mode = BAM_MODE_SYSTEM;
	pipe_cfg.desc_base_va = (uint32)mem_part->desc_fifo;
	pipe_cfg.desc_base_pa = (uint32)mem_part_phy->desc_fifo;
	pipe_cfg.desc_size = NUM_DESCS*8; 
	pipe_cfg.evt_thresh = 1;

	pipe_cb.data = NULL;
	pipe_cb.func = ecm_pipe_cb;

	pipe_h = bam_pipe_init(bam_h, ECM_CONSUMER_PIPE, &pipe_cfg, &pipe_cb );

	if (!pipe_h) {
		return 1;
	}
#endif
	DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_COREBSP, spmif_subsys_common_tbl);

	if (DAL_SUCCESS != DALSYS_EventCreate(DALSYS_SYNC_ATTR_RESOURCE,&trigger_event,NULL) ) 
	{
		MSG_ERROR("[ECM TEST] error creating event",0,0,0);
		return 1;
	}

	if (DAL_SUCCESS != DALSYS_RegisterWorkLoop(0, 8, &wl, NULL))
	{
		MSG_ERROR("[ECM TEST] error creating workloop",0,0,0);
		return 1;
	}
	if ( DAL_SUCCESS != DALSYS_AddEventToWorkLoop(wl,ecm_workloop,NULL, trigger_event, NULL ) )
	{
		MSG_ERROR("[ECM TEST] error attaching event to workloop",0,0,0);
		return 1;
	}

	return 0;
}
