/**
  @file mhi_link.c
  @brief
  This file contains the implementation of the MHI Link Abstraction component

*/
/*
===============================================================================

Edit History

$Header:

Change revision history maintained in version system
===============================================================================
Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "mhi_link.h"
#include "mhi_pm.h"
#include "mhi_mmio.h"
#include "mhi_core.h"
#include "mhi_hwacc.h"
#include "pcie.h"

#ifdef MHI_DMA 
#include "ipa_dma.h"
#endif

/* DMA minimum request size. Any request below this will not use the DMA  */
#define DMA_REQ_MIN_SIZE 128

/* Translates host address to local address */
#ifdef MHI_TRANSLATE_HOST_ADDR
#define HOST_TO_LOCAL(index,addr) ((addr)-(address_map[index].host)+(address_map[index].local.va))
#define HOST_TO_LOCAL_PA(index,addr) ((addr)-(address_map[index].host)+(address_map[index].local.pa))
#else
#define HOST_TO_LOCAL(index,addr)    (addr)
#define HOST_TO_LOCAL_PA(index,addr) (addr)
#endif

#define VALIDATE_TRANS(index, addr,len) ((addr)>=address_map[index].host && \
                                         (((addr)-address_map[index].host+(len)) <= address_map[index].size) && \
                                         (0 != len))
#ifdef MHI_DMA 
/** DMA device context */
typedef struct _mhi_dma_dev
{
   ipa_dma_ch_param_t sync_channel;  //sync channel handle
   ipa_dma_ch_param_t async_channel; //async channel handle
   dma_req_cb async_rw_req_cb;          /** < -- Generic callback for asynchronous read/write requests */
}mhi_dma_dev_t;

/** DMA device */
static mhi_dma_dev_t dma_dev;

/* DMA FIFO size for each channel */
#define DMA_DESC_FIFO_SIZE 128

/* DMA callback type */
typedef void (*mhi_dma_rw_cb_t)(void *user_data, int32 status);

/* Local Prototypes */

void mhi_link_rw_cb_wrapper(void *user_data, int32 status);
#endif

/* Static address map used to store host to local address mappings */
static addr_map_t address_map[2];

#ifndef MHI_BOOT

/* PCIE to MHI Notification map */
static uint32 pcie_to_mhi_notification_map[PCIE_TO_MHI_PM_MAX] = {0};

#define SCRATCH_POOL_SIZE                128           /** Scratch Pool Size in bytes*/

/* DMA prototypes */
mhi_status_t mhi_dma_write_va(void *buf, uint64 host_addr, uint32 size, void *user_data, uint32 options);
mhi_status_t mhi_dma_read_va(void *buf, uint64 host_addr, uint32 size, void *user_data, uint32 options);
mhi_status_t mhi_dma_write_pa(void *buf, uint64 host_addr, uint32 size, void *user_data, uint32 options);
mhi_status_t mhi_dma_read_pa(void *buf, uint64 host_addr, uint32 size, void *user_data, uint32 options);

/**
 * @brief      mhi_link_map_pcie_notification_to_mhi
 *
 *             Map the notifications enumerated in PCIE driver to equivalent MHI notifications
 *
 *
 * @param[in]  pcie_notif - pcie notification type
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             None
 *
 * @sideeffect None
 */
uint32 mhi_link_map_pcie_notification_to_mhi(pcie_event_notification_type pcie_notif)
{
   switch (pcie_notif)
   {
      case PCIE_PM_PERST_DEASSERTED:
         pcie_to_mhi_notification_map[pcie_notif] = MHI_SM_TRIG_HOST_WAKEUP_RECEIVED_SHIFT;
         break;
      case PCIE_PM_DEVICE_STATE_IN_D3_COLD:
         pcie_to_mhi_notification_map[pcie_notif] = MHI_SM_TRIG_D_STATE_CHANGE_SHIFT;
         break;
      case PCIE_PM_DEVICE_STATE_IN_D3_HOT:
         pcie_to_mhi_notification_map[pcie_notif] = MHI_SM_TRIG_D_STATE_CHANGE_SHIFT;
         break;
      case PCIE_PM_DEVICE_STATE_IN_D0:
         pcie_to_mhi_notification_map[pcie_notif] = MHI_SM_TRIG_D_STATE_CHANGE_SHIFT;
         break;
      case PCIE_LINK_UP:
         pcie_to_mhi_notification_map[pcie_notif] = MHI_SM_TRIG_LINK_UP_SHIFT;
         break;
      case PCIE_LINK_DOWN:
         pcie_to_mhi_notification_map[pcie_notif] = MHI_SM_TRIG_LINK_DOWN_SHIFT;
         break;
      case PCIE_PM_L1_INACT_TIMEOUT:
         pcie_to_mhi_notification_map[pcie_notif] = MHI_SM_TRIG_LINK_INACT_TIMER_SHIFT;
         break;
      case PCIE_CLKREQ_ASSERTED:
         pcie_to_mhi_notification_map[pcie_notif] = MHI_SM_TRIG_LINK_ACTIVITY_SHIFT;
         break;
      case PCIE_MHI_INT:
      case PCIE_MMIO_WRITE:
         mhi_osal_debug(MSG_HIGH,"mhi_link_map_pcie_notification_to_mhi: Received notification %x",pcie_notif);
         break;
      default:
         mhi_osal_debug(MSG_HIGH,"mhi_link_map_pcie_notification_to_mhi: Unknown notification received %x",pcie_notif);
         return ERROR;
   }

   return SUCCESS;
}

/**
 * @brief      mhi_link_notifications
 *
 *             MHI Link Notifications. Called by pcie driver
 *
 *
 * @param[in]  pcie_notif  - Notification
 * @param[in]  *data - User supplied data
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             MHI_SUCCESS if operation succeeded
 *
 * @sideeffect None
 */
void mhi_link_notifications(pcie_event_notification_type pcie_notif, void *data)
{
   uint32 ret=SUCCESS;
    
   /* Multiple interrupts of varying priorities may invoke this callback */
   mhi_osal_syncenter(&mhiCtx.mhi_link_info.mhi_link_notifSync);

   switch (pcie_notif)
   {
      case PCIE_PM_PERST_DEASSERTED:
         mhi_osal_debug(MSG_HIGH,"mhi_link_notifications: Received PCIE_PM_PERST_DEASSERTED \
         in mhiState %d",mhiCtx.mhi_sm_info.mhiState);
         break;
      /* In the current implementation PCIe driver reports D3cold whenever PERST is asserted */
      case PCIE_PM_DEVICE_STATE_IN_D3_COLD:
         mhi_osal_debug(MSG_HIGH,"mhi_link_notifications: Received PCIE_PM_DEVICE_STATE_IN_D3_COLD \
         in mhiState %d",mhiCtx.mhi_sm_info.mhiState);
         mhi_pm_set_d_state(MHI_LINK_D3cold);
         break;
      case PCIE_PM_DEVICE_STATE_IN_D3_HOT:
         mhi_osal_debug(MSG_HIGH,"mhi_link_notifications: Received PCIE_PM_DEVICE_STATE_IN_D3_HOT \
         in mhiState %d",mhiCtx.mhi_sm_info.mhiState);
         mhi_pm_set_d_state(MHI_LINK_D3hot);
         break;
      case PCIE_PM_DEVICE_STATE_IN_D0:
         mhi_osal_debug(MSG_HIGH,"mhi_link_notifications: Received PCIE_PM_DEVICE_STATE_IN_D0 \
         in mhiState %d",mhiCtx.mhi_sm_info.mhiState);
         mhi_pm_set_d_state(MHI_LINK_D0);
         break;
      case PCIE_LINK_UP:
         mhi_osal_debug(MSG_HIGH,"mhi_link_notifications: Received PCIE_LINK_UP \
         in mhiState %d",mhiCtx.mhi_sm_info.mhiState);
         mhi_pm_set_d_state(MHI_LINK_D0);
         break;
      case PCIE_LINK_DOWN:
         mhi_osal_debug(MSG_HIGH,"mhi_link_notifications: Received PCIE_LINK_DOWN \
         in mhiState %d",mhiCtx.mhi_sm_info.mhiState);
         mhi_core_sys_error();
         break;
      case PCIE_PM_L1_INACT_TIMEOUT:
         mhi_osal_debug(MSG_HIGH,"mhi_link_notifications: Received PCIE_PM_L1_INACT_TIMEOUT \
         in mhiState %d",mhiCtx.mhi_sm_info.mhiState);
         /* Set timer running flag to FALSE, since the timer expired */
         mhiCtx.mhi_link_info.mhi_link_inact_timer_running=FALSE;
         break;
      case PCIE_CLKREQ_ASSERTED:
         mhi_osal_debug(MSG_HIGH,"mhi_link_notifications: Received PCIE_CLKREQ_ASSERTED \
         in mhiState %d",mhiCtx.mhi_sm_info.mhiState);
         break;
      case PCIE_MHI_INT:
      case PCIE_MMIO_WRITE:
         mhi_core_isr(&mhiCtx);
         break;
      default:
         mhi_osal_debug(MSG_FATAL,"mhi_link_notifications: Unknown notification received %x \
         in mhiState %d",pcie_notif,mhiCtx.mhi_sm_info.mhiState);
   }

   /* If notification was not mapped, map it, so when we set the pending trigger we can use the MHI
    * LINK notification map */
   if(!pcie_to_mhi_notification_map[pcie_notif])
   {
      ret=mhi_link_map_pcie_notification_to_mhi(pcie_notif);
   }

   mhi_osal_syncleave(&mhiCtx.mhi_link_info.mhi_link_notifSync);

   /* Set the state machine trigger as pending and signal the MHI CORE thread, if mapping exists */
   if (SUCCESS == ret)
   mhi_sm_set_trig_pending(pcie_to_mhi_notification_map[pcie_notif],TRUE);

}
#endif

#ifdef MHI_DMA 
/* Initialize the DMA */
uint32 mhi_dma_init(mhi_dma_rw_cb_t rw_cb)
{
   mhi_osal_debug(MSG_HIGH, "Initialize DMA begin");

   if (0 != ipa_per_pm_request(IPA_PER_TYPE_MHI,NULL))
      goto exit;

   /* Init sync channel */
   dma_dev.sync_channel.type=IPA_PER_TYPE_MHI;
   dma_dev.sync_channel.is_async=FALSE;
   dma_dev.sync_channel.size=DMA_DESC_FIFO_SIZE;
   dma_dev.sync_channel.cb=NULL;
   if (0 != ipa_dma_open(&dma_dev.sync_channel))
      goto exit;

   /* Init async channel */
   dma_dev.async_channel.type=IPA_PER_TYPE_MHI;
   dma_dev.async_channel.is_async=TRUE;
   dma_dev.async_channel.size=DMA_DESC_FIFO_SIZE;
   dma_dev.async_channel.cb=rw_cb;
   if (0 != ipa_dma_open(&dma_dev.async_channel))
      goto exit;

   if (0 != ipa_per_pm_release(IPA_PER_TYPE_MHI,IPA_PER_PM_RES_CLK_GATING_BM,NULL))
      goto exit; 
   
   mhi_osal_debug(MSG_HIGH, "Initialize DMA done successfully");
   return SUCCESS;

exit:
   mhi_osal_debug(MSG_FATAL, "Initialize DMA done with errors");
   return ERROR;
}
#endif

/**
 * @brief      mhi_link_init
 *
 *             Initialize the MHI Link layer
 *
 *
 * @param[in]  *link_info - Pointer to link information context
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             MHI_SUCCESS if operation succeeded
 *
 * @sideeffect None
 */
mhi_status_t mhi_link_init(mhi_link_info_t *link_info)
{
   mhi_osal_debug(MSG_MEDIUM,"mhi_link_init: Enter");

   if(FALSE == pcie_is_pcie_boot_enabled())
   {
      return MHI_ERROR;
   }

   /* This mutex is exclusively for link notifications. Should not be used for
    * anything else since it will be used in isr context.
    */
   mhi_osal_syncinit(&mhiCtx.mhi_link_info.mhi_link_notifSync);

#ifdef MHI_BOOT
   pcie_init();
#endif

   /* Link hardware must be enabled at init */
   mhiCtx.mhi_link_info.mhi_link_hw_enabled = TRUE;

   /* Init Inactivity timer to FALSE */
   mhiCtx.mhi_link_info.mhi_link_inact_timer_running = FALSE;

#ifndef MHI_SW_LINK_INACTIVITY
   mhiCtx.mhi_link_info.hw_link_inact_timer_supported = TRUE;
#else
   mhiCtx.mhi_link_info.hw_link_inact_timer_supported = pcie_l1ss_supported();
#endif /* MHI_SW_LINK_INACTIVITY */

#ifdef MHI_DMA 
   /* Init the mhi DMA */
   if (SUCCESS != mhi_dma_init(mhi_link_rw_cb_wrapper))
      return MHI_ERROR;
#endif   

   /* Init mhi scratch pool */
   mhi_osal_mallocmemregion(&mhiCtx.mhi_link_info.scratch_pool, SCRATCH_POOL_SIZE, MHI_MEMORY_MAPPING);
   if(NULL == mhiCtx.mhi_link_info.scratch_pool.handle)
   {
      mhi_osal_debug(MSG_FATAL, "Couldn't malloc memregion");
      return MHI_ERROR;
   }
   mhi_osal_syncinit(&mhiCtx.mhi_link_info.scratch_sync);

   return MHI_SUCCESS;
}

/**
 * @brief      mhi_link_triggerinterrupt
 *
 *             Trigger an interrupt over the link
 *
 *
 * @param[in]  vec - Vector to trigger
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             MHI_SUCCESS if operation succeeded
 *
 * @sideeffect None
 */
mhi_status_t mhi_link_triggerinterrupt(uint32 vec)
{
   pcie_msiconfig_type curr_msi_config = { 0 };
   uint32 mask, value;

   mhi_osal_debug(PROFILING_LOG|MSG_LOW,"mhi_link_triggerinterrupt: send_msi=start");

   /* Get current MSI config */
   pcie_get_msi_config(&curr_msi_config);

   if(0x1 == curr_msi_config.enabled)
   {
      /* Send the msi */
      mask = (1 << curr_msi_config.num_multi_msgs) - 1;
      value = (curr_msi_config.data & ~mask) | (vec & mask);
#ifdef MHI_TRANSLATE_HOST_ADDR
      mhi_dma_write_va((char*)&value, ((uint64)curr_msi_config.local_addr.pa_hi<<32)|(uint64)curr_msi_config.local_addr.pa_low
, 4, NULL, 0);
#else
      mhi_dma_write_va((char*)&value, ((uint64)curr_msi_config.addr_hi<<32)|(uint64)curr_msi_config.addr_low
, 4, NULL, 0);
#endif
   }
   else
   {
       mhi_osal_debug(MSG_ERR,"mhi_link_triggerinterrupt: send_msi disable");
   }

   mhi_osal_debug(PROFILING_LOG|MSG_LOW,"mhi_link_triggerinterrupt: send_msi=end");
   return MHI_SUCCESS;
}

#ifdef MHI_DMA
/**
 * @brief      mhi_link_rw_cb_wrapper
 *
 *             Link CB wrapper that receives callbacks from underlying transport
 *             driver
 *
 *
 * @param[in]  result
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             None
 *
 * @sideeffect None
 */
void mhi_link_rw_cb_wrapper(void *user_data, int32 status)
{
   mhi_link_ret_t ret={0};

   /* Translate the return structure from pcie to mhi and issue the CB */
   ret.userdata=user_data;
   ret.status=(0==status)?MHI_SUCCESS:MHI_ERROR;

   mhiCtx.mhi_link_info.cbFunc(ret);
}
#endif

/**
 * @brief      mhi_link_rw
 *
 *             Perform a read/write operation over the link
 *
 *
 * @param[in]  index - Address index
 * @param[in]  destAddr  - Destination address
 * @param[in]  *srcAddr  - Source address
 * @param[in]  size      - Size of write in bytes
 * @param[in]  *userdata - User data to associate with write
 * @param[in]  *op       - Operation (READ/WRITE)
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             MHI_SUCCESS if operation succeeded
 *
 * @sideeffect None
 */
mhi_status_t mhi_link_rw(uint32 index, uint64 link_addr, mhi_addr_t *local_addr, uint32 size, void* userdata, mhi_op_t op)
{
   mhi_status_t ret=MHI_SUCCESS;
   uint32 addr;
   mhi_status_t (*dma_func)(void *, uint64, uint32, void *, uint32);
   uint32 priority=PCIE_RW_OPTION_DEFAULT;

#ifdef MHI_ENABLE_HIGH_PRIORITY_MSG
   /* Set link read/write priority to high if no userdata is supplied or if link request priority is high */
   if (NULL == userdata || HIGH == ((mhi_link_async_req_t *)userdata)->priority)
      priority=PCIE_RW_PRIORITY_HIGH;
#endif /* MHI_ENABLE_HIGH_PRIORITY_MSG */

   if(0 != local_addr->pa)
   {
      dma_func=(MHI_READ == op?mhi_dma_read_pa:mhi_dma_write_pa);
      addr=local_addr->pa;
   }
   else
   {
      dma_func=(MHI_READ == op?mhi_dma_read_va:mhi_dma_write_va);
      addr=local_addr->va;
   }

   if(!VALIDATE_TRANS(index,link_addr,size))
   {
      mhi_osal_debug(MSG_ERR,"mhi_link_rw: Invalid rw attempted link_addr:0x%x%x size:%d",QW2DW(link_addr),size);
      return MHI_ERROR;
   }

#ifdef MHI_DMA
   /* Check size, if less than threshold use synchronous pcie write (memcpy).
    * Otherwise use DMA based (async) */
   if(size < DMA_REQ_MIN_SIZE || NULL == userdata)
#endif
   {
      ret = dma_func((void*)addr,HOST_TO_LOCAL_PA(index,link_addr),size,NULL,priority);
   }
#ifdef MHI_DMA
   else
   {
      mhiCtx.mhi_link_info.is_DMA_flush_needed=TRUE;
      ret = dma_func((void*)addr,HOST_TO_LOCAL_PA(index,link_addr),size,userdata,priority);

      /* If no more free link requests are available, flush the DMA requests */
      if (mhi_q_is_queue_empty(&mhiCtx.freeLinkReqQ))
      {
         mhi_link_dma_flush();
         mhiCtx.mhi_link_info.is_DMA_flush_needed=FALSE;
      }
   }
#endif

   return ret;
}

/**
 * @brief      mhi_link_write
 *
 *             Perform a write operation over the link
 *
 *
 * @param[in]  index - Address index
 * @param[in]  destAddr  - Destination address
 * @param[in]  *srcAddr  - Source address
 * @param[in]  size      - Size of write in bytes
 * @param[in]  *userdata - User data to associate with write
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             MHI_SUCCESS if operation succeeded
 *
 * @sideeffect None
 */
mhi_status_t mhi_link_write(uint32 index,uint64 destAddr,mhi_addr_t *srcAddr, uint32 size, void* userdata )
{
#ifdef MHI_CHECK_HW_ENABLED
   if (!mhiCtx.mhi_link_info.mhi_link_hw_enabled)
   {
      mhi_osal_debug(MSG_FATAL,"mhi_link_write: Attempted access when hardware is off");
   }
#endif

   return mhi_link_rw(index, destAddr, srcAddr, size, userdata, MHI_WRITE);
}

/**
 * @brief      mhi_link_read
 *
 *             Perform a read operation over the link
 *
 *
 * @param[in]  index      - Address index
 * @param[in]  srcAddr    - Source address
 * @param[in]  *destAddr  - Destination address
 * @param[in]  size       - Size of operation in bytes
 * @param[in]  *userdata  - User supplied data to associate with operation
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             MHI_SUCCESS if operation succeeded
 *
 * @sideeffect None
 */
mhi_status_t mhi_link_read(uint32 index, uint64 srcAddr, mhi_addr_t *destAddr, uint32 size, void *userdata)
{
#ifdef MHI_CHECK_HW_ENABLED
   if (!mhiCtx.mhi_link_info.mhi_link_hw_enabled)
   {
      mhi_osal_debug(MSG_FATAL,"mhi_link_read: Attempted access when hardware is off");
   }
#endif

   return mhi_link_rw(index, srcAddr, destAddr, size, userdata, MHI_READ);
}

/**
 * @brief      mhi_link_reset
 *
 *             Reset the link layer
 *
 *
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             MHI_SUCCESS if operation succeeded
 *
 * @sideeffect None
 */
mhi_status_t mhi_link_reset(void)
{
   /* Remove the atu mappings */
   pcie_remove_index_obatu(1);
   pcie_remove_index_obatu(2);

   mhi_osal_memset(address_map, 0, sizeof(address_map));

   mhiCtx.mhi_link_info.mhi_link_hw_enabled = TRUE;
   mhi_link_disable_inactivity_intr();

   return MHI_SUCCESS;
}

/**
 * @brief      mhi_link_maphostaddrspace
 *
 *             Map multiple host base to limit addresses
 *
 *
 * @param[in]  *base   - Host base address array
 * @param[in]  *limit  - Limit address array
 * @param[out] **local - Local Address information obtained from underlying transport
 * @param[in]  nMap    - Number of mappings
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             MHI_SUCCESS if operation succeeded
 *
 * @sideeffect None
 */
mhi_status_t mhi_link_maphostaddrspace(uint64 *base, uint64 *limit, mhi_addr_t **local, uint32 nMap)
{
   pcie_iatu_entry_type iatu_map[5];
   pcie_address_type local_addr[5];

   uint32 i;

   /* Pcie driver does not support more than 2 */
   if(nMap > 2)
      return MHI_ERROR;

   /* For each map populate the iatu_map to describe the addresses */
   for(i =0; i< nMap; i++)
   {
      local[i]->va=0;
      local[i]->pa=0;

      if(limit[i] < base[i] || base[i]&0xFFFULL)
         return MHI_ERROR;

      iatu_map[i].host_addr.pa_low=*(uint32*)&base[i];
      iatu_map[i].host_addr.pa_hi=*(((uint32*)&base[i])+1);
      iatu_map[i].size=limit[i]-base[i]+1;
      iatu_map[i].local_addr=&local_addr[i];
   }

#ifdef MHI_SUPPORT_IPABAM
   /* Request addresses to be mapped */
   if(SUCCESS != pcie_set_host2dev_mmap(iatu_map,nMap))
      return MHI_ERROR;
#endif

   for (i=0; i< nMap; i++)
   {
#ifdef MHI_SUPPORT_IPABAM
      if(0 == local_addr[i].va_low)
         return MHI_ERROR;

      local[i]->va = local_addr[i].va_low;
      local[i]->pa = local_addr[i].pa_low;

      address_map[i].local=*local[i];
#endif
      address_map[i].host=base[i];
      address_map[i].size=iatu_map[i].size;
   }

   return MHI_SUCCESS;
}

/**
 * @brief       mhi_link_wakeup_req
 *
 *              Abstract link API that actually does the wake up procedure.
 *
 *
 * @dependencies
 *              mhi_link_init() must be called first.
 *
 * @return
 *              MHI_STATUS if successful.
 *              MHI_ERROR  if error occurred.
 *
 * @sideeffect  None
 */
mhi_status_t mhi_link_wakeup_req(void)
{
#ifndef MHI_BOOT
   /* Decision to assert Wake GPIO or send PME is left to PCIe EP driver */
   pcie_wakeup_host();
#endif
   return MHI_SUCCESS;
}

/**
 * @brief       mhi_link_enable
 *
 *              Abstract link API that re-enables link hardware after a power collapse.
 *
 *
 * @dependencies
 *              Should be called only after PERST de-assertion notification is received.
 *
 * @return
 *              MHI_SUCCESS/MHI_ERROR
 *
 * @sideeffect  None
 */
mhi_status_t mhi_link_enable(void)
{
#ifndef MHI_BOOT

   mhi_osal_debug(MSG_HIGH,"mhi_link_enable: Enter ");

   /* Turn on PCIe hardware blocks and clocks */
   pcie_enable_endpoint();

   /* MMIO is now accessible */
   mhiCtx.mhi_link_info.mhi_link_hw_enabled = TRUE;

   /* Enable ipa db routing  */
   mhi_link_setup_hwacc_db_routing(mhiCtx.mhi_core_tgt_config->mhi_hw_acc_mailbox_cdb_target,mhiCtx.mhi_core_tgt_config->mhi_hw_acc_mailbox_edb_target);

   /* Clocks are up, restore MMIO */
   mhi_mmio_restore(&mhiCtx.mhiConfig);

   /* Enable PCIe link training and enumeration */
   pcie_enable_ltssm();

   mhi_osal_debug(MSG_HIGH, "mhi_link_enable: Restore Control and Data ATUs");

   if (MHI_SUCCESS != mhi_core_map_ctrl_and_base_regions(mhiCtx.mhiaddr,&mhiCtx.mhiLocalAddr))
   {
      mhi_osal_debug(MSG_FATAL, "mhi_link_enable: FATAL - Could not allocate local address space.");
      return MHI_ERROR;
   }
#endif

   return MHI_SUCCESS;
}

/**
 * @brief       mhi_link_disable
 *
 *              Abstract link API that disables link hardware.
 *
 *
 * @dependencies
 *              None
 *
 * @return
 *              MHI_STATUS if successful.
 *              MHI_ERROR  if error occurred.
 *
 * @sideeffect
 *              MMIO and link address space will be in accessible
 */
mhi_status_t mhi_link_disable(void)
{
   mhi_osal_debug(MSG_HIGH,"mhi_link_disable: Enter ");

   if (mhiCtx.mhi_link_info.mhi_link_hw_enabled)
   {
      /* Backup MMIO to prepare for clocks turning off */
      mhi_mmio_backup(&mhiCtx.mhiConfig);

      /* Link hw will be disabled, prevent accesses to MMIO */
      mhiCtx.mhi_link_info.mhi_link_hw_enabled = FALSE;
   }

   /* Disable link hw */
   pcie_disable_endpoint();

   return MHI_SUCCESS;
}

/**
 * @brief       mhi_link_is_active
 *
 *              Determine whether the link can actively push traffic or is in some
 *              power saving state.
 *
 *
 * @dependencies
 *              mhi_link_init() should be called first.
 *
 * @return
 *              TRUE if link is active, False is not.
 *
 * @sideeffect  None
 */
boolean mhi_link_is_active(void)
{
#ifndef MHI_BOOT /* Enable if supported by boot */
   return (pcie_get_link_state() == PCIE_LINK_STATE_L0);
#else
   return TRUE;
#endif
}

/**
 * @brief       mhi_link_is_hw_enabled
 *
 *              Report whether link hw is enabled or not, i.e. that link has not
 *              turned off hardware blocks to save power. Otherwise will not be
 *              able to access MMIO.
 *
 *
 * @dependencies
 *              mhi_link_init must be called.
 *
 * @return
 *              TRUE if link hardware is enabled, False if not.
 *
 * @sideeffect  None
 */
boolean mhi_link_is_hw_enabled(void)
{
   return mhiCtx.mhi_link_info.mhi_link_hw_enabled;
}

#ifndef MHI_BOOT
/**
 * @brief       mhi_link_enable_inactivity_intr
 *
 *              Enable the L1ss inactivity timer interrupt.
 *
 *
 * @param[in]   inact_time - Amount of inactivity time should cause interrupt
 *                           The unit of time is implementation specific
 *
 * @dependencies
 *              mhi_link_init must be called.
 *
 * @return
 *              None
 *
 * @sideeffect  None
 */
void mhi_link_enable_inactivity_intr(uint32 inact_time)
{
   mhi_osal_debug(MSG_HIGH,"mhi_link_enable_inactivity_intr: Arm the link inactivity timer interrupt");

   /* Enable link inactivity timer only if it isn't already running or if MIN_INACTIVITY
    * is being set */
   if (FALSE == mhiCtx.mhi_link_info.mhi_link_inact_timer_running ||
       MIN_INACTIVITY == inact_time)
   {
      /* Record sys time in ms when link inactivity is armed for MAX_INACTIVITY */
      if (MAX_INACTIVITY == inact_time)
      {
         mhiCtx.mhi_link_info.time_max_link_inact_armed_ms=mhi_osal_get_systime_ms();
      }

      if (mhiCtx.mhi_link_info.hw_link_inact_timer_supported) 
         mhiCtx.mhi_link_info.min_inact_expiry_time_us = pcie_control_l1ss_inact_interrupt(TRUE,inact_time);
      else
         mhiCtx.mhi_link_info.min_inact_expiry_time_us = 0;

      mhiCtx.mhi_link_info.mhi_link_inact_timer_running=TRUE;

      mhi_osal_debug(MSG_HIGH,"mhi_link_enable_inactivity_intr: inact_time=0x%x timer enabled",inact_time);
   }
   else
      mhi_osal_debug(MSG_MEDIUM,"mhi_link_enable_inactivity_intr: timer already running");
}

/**
 * @brief       mhi_link_disable_inactivity_intr
 *
 *              Disable the L1ss inactivity timer interrupt.
 *
 *
 * @dependencies
 *              mhi_link_init must be called.
 *
 * @return
 *              None
 *
 * @sideeffect  None
 */
void mhi_link_disable_inactivity_intr(void)
{
   mhi_osal_debug(MSG_HIGH,"mhi_link_disable_inactivity_intr: Disarm the link inactivity timer interrupt");
   pcie_control_l1ss_inact_interrupt(FALSE,DISABLE_INACTIVITY);
   mhiCtx.mhi_link_info.mhi_link_inact_timer_running=FALSE;
}

/**
 * @brief       mhi_link_update_latency
 *
 *              Update the tolerable latency in useconds to the link.
 *
 *
 * @dependencies
 *              mhi_link_init must be called.
 *
 * @return
 *              None
 *
 * @sideeffect  None
 */
void mhi_link_update_latency(uint32 latency_us)
{
   pcie_send_ltr(latency_us);
}

/**
 * @brief       mhi_link_release_hw_resources
 *
 *              Release link hardware resources. This may involve
 *              the link driver software turning off clocks and voting against
 *              hardware resources that are not required if the link will not
 *              be used. Note that when entering into D3cold, this task is handled
 *              implicitly by the link driver.
 *
 * @dependencies
 *              mhi_link_init must be called.
 *
 * @return
 *              MHI_SUCESS or MHI_ERROR
 *
 * @sideeffect  Link hardware is no longer accessible after this function is called.
 */
mhi_status_t mhi_link_release_hw_resources(void)
{
   mhi_status_t ret;
   mhi_osal_debug(MSG_HIGH,"mhi_link_release_hw_resources: Enter");

   /* Disable MHI interrupt to prevent unclocked access of MMIO due to race
    * between MHI interrupt and procedure to turn off clocks */
   mhi_link_ctrl_core_interrupt(DISABLED);

   /* If MHI core isr ran before disabling irq, it may have masked the interrupt.
    * Must check device wake here since we risk missing the interrupt due to masking.
    * If device wake db is written after irq is disabled mhi_osal_is_irq_set will detect it */
   if (HIGH == mhi_pm_get_device_wake_gpio_state())
   {
      mhi_osal_debug(MSG_HIGH,"mhi_link_release_hw_resources: Device wake is high. Cannot go to LPM");
      ret=MHI_ERROR;
      goto err;
   }

   if(PCIE_STATUS_SUCCESS == pcie_pm_enter_lpm())
   {
      mhiCtx.mhi_link_info.mhi_link_hw_enabled = FALSE;
      ret=MHI_SUCCESS;
   }
   else
   {
      ret=MHI_ERROR;
   }

err:
   if (MHI_ERROR == ret)
   {
      /* Enable MHI interrupt since lpm was not entered */
      mhi_link_ctrl_core_interrupt(ENABLED);

      ret=MHI_ERROR;
   }

   return ret;
}

/**
 * @brief       mhi_link_acquire_hw_resources
 *
 *              Acquire link hardware resources. This may involve
 *              the link driver software turning on clocks and voting for
 *              hardware resources that are required if the link will be used.
 *              Note that when exiting out of D3cold, this task is handled
 *              implicitly by the link driver.
 *
 * @dependencies
 *              mhi_link_init must be called.
 *
 * @return
 *              MHI_SUCCESS
 *
 * @sideeffect  Link hardware is now accessible.
 */
mhi_status_t mhi_link_acquire_hw_resources(void)
{
   mhi_osal_debug(MSG_HIGH,"mhi_link_acquire_hw_resources: Enter");
   pcie_pm_exit_lpm();
   mhiCtx.mhi_link_info.mhi_link_hw_enabled = TRUE;

   /* Enable MHI interrupt since it is disabled when entering lpm */
   mhi_link_ctrl_core_interrupt(ENABLED);

   return MHI_SUCCESS;
}
#endif /* #ifndef MHI_BOOT */

/**
 * @brief       mhi_link_setup_hwacc_db_routing
 *
 *              Setup the doorbell routing for the hardware accelerator.
 *
 * @param[in]   chdb_mailbox_addr - Target address for routing channel doorbells to
 *              the hardware accelerator.
 * @param[in]   erdb_mailbox_addr - Target address for routing event ring doorbells
 *              to the hardware accelerator.
 *
 * @dependencies
 *              mhi_link_init must be called.
 *
 * @return
 *              None
 *
 * @sideeffect  Writes to the channel and event ring doorbell registers dedicated to the
 *              hardware accelerator in MMIO generate interrupts to the hardware accelerator.
 */
void mhi_link_setup_hwacc_db_routing(uint32 chdb_mailbox_addr, uint32 erdb_mailbox_addr)
{
#ifdef MHIIPA
   pcie_set_mhi_ipa_dbs_addr(HWCHANNELBASE, HWCHANNELBASEEND, mhiCtx.mhiConfig.nEr-mhiCtx.mhiConfig.nHwEr, mhiCtx.mhiConfig.nEr-1,chdb_mailbox_addr,erdb_mailbox_addr);
#endif
}

#ifndef MHI_BOOT
/**
 * @brief       mhi_link_get_reset_line_state
 *
 *              Return the state of the reset line.
 *
 *
 * @dependencies
 *              None
 *
 * @return
 *              DEASSERTED\ASSERTED
 *
 * @sideeffect  None
 */
boolean mhi_link_get_reset_line_state(void)
{
   /* PERST is considered ASSERTED if the line state is LOW */
   return pcie_read_perst_pin_value()?DEASSERTED:ASSERTED;
}
#endif

/**
 * @brief      mhi_link_flush
 *
 *             Flush out any pending writes
 *
 *
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             None
 *
 * @sideeffect None
 */
void mhi_link_flush(void)
{
}

/**
 * @brief      mhi_link_dma_flush
 *
 *             Flush out any pending DMA requests
 *
 *
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             None
 *
 * @sideeffect None
 */
void mhi_link_dma_flush(void)
{
#if defined(MHI_DMA) && !defined(MHI_BOOT)
   mhi_osal_debug(MSG_MEDIUM,"mhi_link_dma_flush:");
#endif
}

/**
 * @brief      mhi_link_notify_dma_state
 *
 *             Notify the link driver whether MHI disabled the link DMA
 *
 *
 * @param[in]  state - ENABLED/DISABLED
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             None
 *
 * @sideeffect None
 */
void mhi_link_notify_dma_state(boolean state)
{
}

/**
 * @brief      mhi_link_dstate_ack
 *
 *             Acknowledge that device has entered the notified D state, and
 *             do any post processing if required
 *
 *
 * @param[in]  dstate - Device state
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             None
 *
 * @sideeffect None
 */
void mhi_link_dstate_ack(mhi_link_d_state_t dstate)
{
  switch(dstate)
  {
     case MHI_LINK_D0:
     case MHI_LINK_D1:
     case MHI_LINK_D2:
        break;
     case MHI_LINK_D3hot:
#ifndef MHI_BOOT
        pcie_device_enable_EnterL23();
#endif

        break;
     case MHI_LINK_D3cold:
        break;
     default:
        mhi_osal_debug(MSG_FATAL,"mhi_link_dstate_ack: Invalid D state: %d",dstate);
  }
}

/**
 * @brief      mhi_dma_write_va
 *
 *             Writes data to the host memory
 *
 * @param[in,out]     buf             Buffer that contains data to be written  into host
 * @param[in]         host_axi_addr   PCIe AXI virtual address in modem address space
 * @param[in]         size            Size of the data to be written in bytes
 * @param[in,out]     user data       User data for this tranaction
 * @param[in]         options         User provided write options
 *
 * @return            mhi_status_t
 *                    0 - Successfully .
 *                    - An error code .
 */
mhi_status_t mhi_dma_write_va(void *buf, uint64 host_addr, uint32 size, void *user_data, uint32 options)
{
  mhi_status_t ret = MHI_PENDING;
#ifdef MHI_DMA   
  boolean use_scratch = FALSE;
  void *buf_ptr = NULL;
  mhi_addr_t addr={0};
  
  addr.va=(uint32)buf;
  if(size < SCRATCH_POOL_SIZE && NULL == user_data)
     use_scratch = TRUE;
  
  mhi_osal_debug(MSG_HIGH,"mhi_dma_write_va: addr.va=0x%x size=%d",addr.va,size); 
  
  /* Use local un-cached scratch pool if size is less than limit and async
   * operation is not required. Scratch pool should not be used for async
   * operations, since that risks that same buffer being submitted twice in
   * succession */
  if(use_scratch)
  {
     /* Use local un-cached scratch pool if size is less than limit */
     mhi_osal_syncenter(&mhiCtx.mhi_link_info.scratch_sync);
     mhi_osal_memcpy((void*)mhiCtx.mhi_link_info.scratch_pool.va, buf, size);
     buf_ptr = (void*)mhiCtx.mhi_link_info.scratch_pool.pa;
  }
  else
  {
     /* Flush the cache for buf to prepare for DMA transfer */
     mhi_osal_cache_ctrl(MHI_CACHE_OP_FLUSH, &addr, size);
     buf_ptr = mhi_osal_va_to_pa(buf);
  }
  ret = mhi_dma_write_pa(buf_ptr, host_addr, size, user_data, options);
  
  if(use_scratch)
  {
     mhi_osal_syncleave(&mhiCtx.mhi_link_info.scratch_sync);
  }
#endif  
  return ret;

}

/**
 * @brief      mhi_dma_read_va
 *
 *             Reads data from the host memory
 *
 * @param[in,out]     buf             Buffer to read into
 * @param[in]         host_axi_addr   Host AXI address in modem address space
 * @param[in]         size            Size of the data to be read in bytes
 * @param[in,out]     user_data       User data for this tranaction
 * @param[in]         options         User provided read options
 *
 * @return            mhi_status_t
 *                    0 - Successfully .
 *                    - An error code .
 */
mhi_status_t mhi_dma_read_va(void *buf, uint64 host_addr, uint32 size, void *user_data, uint32 options)
{
  mhi_status_t ret = MHI_PENDING;
#ifdef MHI_DMA 
  boolean use_scratch = FALSE;
  void *buf_ptr = NULL;
  mhi_addr_t addr={0};
    
  addr.va=(uint32)buf;
  if(size < SCRATCH_POOL_SIZE && NULL == user_data)
     use_scratch = TRUE;
  
  mhi_osal_debug(MSG_HIGH,"mhi_dma_read_va: addr.va=0x%x size=%d",addr.va,size); 

  /* Use local un-cached scratch pool if size is less than limit and async
   * operation is not required. Scratch pool should not be used for async
   * operations, since that risks that same buffer being submitted twice in
   * succession */
  if(use_scratch)
  {
     mhi_osal_syncenter(&mhiCtx.mhi_link_info.scratch_sync);
     buf_ptr = (void*)mhiCtx.mhi_link_info.scratch_pool.pa;
  }
  else
  {
     /* Invalidate cache before kicking off the DMA */
     mhi_osal_cache_ctrl(MHI_CACHE_OP_INVALIDATE, &addr, size);
     buf_ptr = mhi_osal_va_to_pa(buf);
  }
  
  ret = mhi_dma_read_pa(buf_ptr, host_addr, size, user_data, options);
  
  if(use_scratch)
  {
     mhi_osal_memcpy(buf, (void*)mhiCtx.mhi_link_info.scratch_pool.va, size);
     mhi_osal_syncleave(&mhiCtx.mhi_link_info.scratch_sync);
  }
  else
  {
     if(NULL == user_data)
     {
        mhi_osal_debug(MSG_HIGH,"mhi_osal_cache_ctrl MHI_CACHE_OP_INVALIDATE: buf=0x%x size=%d",buf, size);
        mhi_osal_cache_ctrl(MHI_CACHE_OP_INVALIDATE, &addr, size);
     }
  }
#endif  
  return ret;

}

/**
 * @brief      mhi_dma_write_pa
 *
 *             Writes data to the host memory using a physical buffer
 *
 * @param[in,out]     buf             Buffer that contains data to be written  into host
 * @param[in]         host_axi_addr   PCIe AXI virtual address in modem address space
 * @param[in]         size            Size of the data to be written in bytes
 * @param[in,out]     user_data       User data for this tranaction
 * @param[in]         options         User provided write options
 *
 * @return            mhi_status_t
 *                    0 - Successfully .
 *                    - An error code .
 */
mhi_status_t mhi_dma_write_pa(void *buf, uint64 host_addr, uint32 size, void *user_data, uint32 options)
{
  mhi_status_t ret = MHI_PENDING;
#ifdef MHI_DMA
  int32 retval;
  ipa_dma_req_t write_req = { 0 };
  uint32 ch_h=(user_data)?dma_dev.async_channel.ch_handle:dma_dev.sync_channel.ch_handle; 

  mhi_osal_debug(MSG_HIGH,"mhi_dma_write_pa: buf=0x%x size=%d",buf,size); 
  
  write_req.device_addr = (uint64)buf;
  write_req.host_addr = host_addr;
  write_req.size = size;
  write_req.user_data = user_data;
  write_req.options = options;
    
  retval = ipa_dma_write(ch_h,&write_req);
  if(IPA_SUCCESS == retval)
  {
    ret =  (user_data)?MHI_PENDING:MHI_SUCCESS;
  }
  else if(IPA_EDESC_FULL  == retval) 
  {
    ret = MHI_OP_FULL;
  }
  else
  {
    ret = MHI_ERROR;
  }
#endif
  return ret;
}

/**
 * @brief      mhi_dma_read_pa
 *
 *             Reads data from the host memory to a physical address
 *
 * @param[in,out]     buf             Physical buffer to read into
 * @param[in]         host_axi_addr   Host AXI address in modem address space
 * @param[in]         size            Size of the data to be read in bytes
 * @param[in,out]     user_data       User data for this tranaction
 * @param[in]         options         User provided read options
 *
 * @return            mhi_status_t
 *                    0 - Successfully .
 *                    - An error code .
 */
mhi_status_t mhi_dma_read_pa(void *buf, uint64 host_addr, uint32 size, void *user_data, uint32 options)
{
   mhi_status_t ret = MHI_PENDING;
#ifdef MHI_DMA 
   int32 retval;
   ipa_dma_req_t read_req = { 0 };
   uint32 ch_h=(user_data)?dma_dev.async_channel.ch_handle:dma_dev.sync_channel.ch_handle;

   mhi_osal_debug(MSG_HIGH,"mhi_dma_read_pa: buf=0x%x size=%d",buf,size);

   read_req.device_addr = (uint64)buf;
   read_req.host_addr = host_addr;
   read_req.size = size;
   read_req.user_data = user_data;
   read_req.options = options;


   retval = ipa_dma_read(ch_h,&read_req);
  if(IPA_SUCCESS == retval)
  {
    ret =  (user_data)?MHI_PENDING:MHI_SUCCESS;
  }
  else if(IPA_EDESC_FULL == retval) 
  {
    ret = MHI_OP_FULL;
  }
  else
  {
    ret = MHI_ERROR;
  }
#endif
  return ret;
}

/**
 * @brief      mhi_link_ctrl_core_interrupt
 *
 *             Enable or disable the MHI Core interrupts
 *
 *
 * @param[in]  ctrl    Enable or disable the interrupt
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             None
 *
 * @sideeffect None
 */

void mhi_link_ctrl_core_interrupt(boolean ctrl)
{
   if(TRUE == ctrl)
   {
      if(MHI_ISR_INVALID_VEC != mhiCtx.mhi_core_tgt_config->mhi_isr_vec)
      {
         mhi_osal_enable_irq(mhiCtx.irqhandle);
      }
      else
      {
         pcie_enable_mhi_interrupts();
      }
   }
   else
   {
      if(MHI_ISR_INVALID_VEC != mhiCtx.mhi_core_tgt_config->mhi_isr_vec)
      {
         mhi_osal_disable_irq(mhiCtx.irqhandle);
      }
      else
      {
         pcie_disable_mhi_interrupts();
      }
   }
}

/**
 * @brief      mhi_link_register_notifications_cb
 *
 *             Register for MHI link call back
 *
 *
 * @param[in]  None
 *
 * @dependencies
 *             None
 *
 *
 * @return
 *             None
 *
 * @sideeffect None
 */
void mhi_link_register_notifications_cb(void)
{
#ifndef MHI_BOOT
  pcie_register_link_notifcation_cb(mhi_link_notifications,NULL);
#endif
}
