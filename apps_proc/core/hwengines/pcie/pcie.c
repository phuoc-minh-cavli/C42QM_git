/**
 * @file pcie.c
 * @brief
 * Implements the interfaces to the PCIe Core HW
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
#include "pcie_hwio.h"
#include "pcie.h"
#include "pcie_core.h"
#include "pcie_osal.h"
#include "pcie_phy.h"
#include "DDIPlatformInfo.h"
#include "pcie_dma.h"
#include "pcie_res.h"
#include "pcie_gpio.h"
#include "pcie_interrupt.h"
#include "pcie_tgt.h"

#define PCIE_OATU_INDEX_MSI              0             /** Outbound index 0: used only for the region mapped for MSIs */
#define PCIE_OATU_INDEX_MHICTRL          1             /** OUTBOUND INDEX 1: used for address mapping for MHI control */
#define PCIE_OATU_INDEX_MHIDATA          2             /** OUTBOUND INDEX 2: used for address mapping for MHI data */

#define PCIE_AXI_MEMOFFSET_FOR_MSI       0X000000      /** Offset for MSI address */
#define PCIE_AXI_MEMOFFSET_FOR_MHICTRL   0X001000      /** Offset for MHI control  */
#define PCIE_AXI_MEMSPACE_LIMIT          0X3FFFD000    /** Upper limit for PCIe memory space */

#define SCRATCH_POOL_SIZE                128           /** Scratch Pool Size in bytes*/

/** IATU Debug Structure */
typedef struct
{
   pcie_address_type host_ctrl;     /** < -- Host Control Address */
   pcie_address_type local_ctrl;    /** < -- Pointer to local Control Address */
   uint64 ctrl_size;                /** < -- Control Size */
   pcie_address_type host_data;     /** < -- Host Data Address */
   pcie_address_type local_data;    /** < -- Pointer to Local Data Address */
   uint64 data_size;                /** < -- Data size */
   byte atu1_used;                  /** < -- ATU1 used */
   byte atu2_used;                  /** < -- ATU2 used */
}pcie_iatu_debug_type;

/** PCIe Context structure Object */
typedef struct {
   pcie_tgtcfg_type           *pcie_tgt_cfg;                      /** < -- Handle to the target configuration data from plat */
   pcie_devinfo_type          **pcie_dev;                         /** < -- Address of PCIe dev pointer */
   pcie_linkinfo_type         pcie_link_info;                     /** < -- Link status indicator */
   pcie_iatu_debug_type       pcie_atu_debug_info;                /** < -- Debug info for pcie atu */
   uint32                     mhictrl_upper_offset;               /** < -- PCIe AXI memory space used for MHI control region */
   uint32                     mhidata_lower_offset;               /** < -- PCIe AXI memory space used for MHI data region */
   pcie_boot_chain_type       pcie_boot_chain;                    /** < -- PCIe boot chain type */
   pcie_osal_meminfo          scratch_pool;                       /** < -- Scratch pool  */
   pcie_osal_sync_type        scratch_sync;                       /** < -- Scratch pool sync */
   pcie_msiconfig_type        pcie_msi_config;                    /** < -- MSI Configuration */
   pcie_coredump_type         *pcie_coredump_ptr;                 /** < -- Pointer to PCIe Coredump */
} pcie_ctx_type;

/** PCIe Context */
static pcie_ctx_type *pcie_ctx;

/** Scratch buffer */
__align(32) uint32 scratch_read_buf;

/* Initializes the PCIe Context Structure */
static pcie_return_status pcie_ctx_init(void);

/* Configures the iATU in the PCIe for outbound (EP <-> Host Memory) traffic */
static pcie_return_status  pcie_add_index_obatu(uint8 index, pcie_address_type host_addr, pcie_address_type *local_addr, uint32 size);

/* ============================================================================
**  Function : pcie_ctx_init
** ============================================================================
*/
/**
 * Initializes the PCIe context.
 *
 * @param[in]         void
 *
 * @return            pcie_return_status
 */
static pcie_return_status pcie_ctx_init(void)
{
   pcie_ctx = (pcie_ctx_type * )pcie_osal_malloc(sizeof(pcie_ctx_type));
   if(NULL ==  pcie_ctx)
   {
      pcie_osal_debug(MSG_FATAL, "Couldn't malloc memregion for pcie_ctx");
      return(PCIE_FAILED);
   }

   pcie_osal_memset(pcie_ctx, 0, sizeof(pcie_ctx_type));

   /* Handle to the target configuration data from plat */
   pcie_ctx->pcie_tgt_cfg = NULL;

   /* Save pcie_dev structure inside the context as well */
   pcie_ctx->pcie_dev = &pcie_dev;

   /* Current PCIe AXI memory space used for MHI control region. reserved at least 4KB initially */
   pcie_ctx->mhictrl_upper_offset = PCIE_AXI_MEMOFFSET_FOR_MHICTRL + 0XFFF;

   /* Current PCIe AXI memory space used for MHI data region. reserved at least 4KB initially */
   pcie_ctx->mhidata_lower_offset = PCIE_AXI_MEMSPACE_LIMIT - 0X1000;

   pcie_osal_mallocmemregion(&pcie_ctx->scratch_pool, SCRATCH_POOL_SIZE, PCIE_MEMORY_MAPPING);
   if(NULL == pcie_ctx->scratch_pool.handle)
   {
      pcie_osal_debug(MSG_FATAL, "Couldn't malloc memregion");
      return(PCIE_FAILED);
   }
   pcie_osal_syncinit(&pcie_ctx->scratch_sync);

   return(PCIE_SUCCESS);
}

/* ============================================================================
**  Function : pcie_reg_dump
** ============================================================================
*/
/**
 * Calls the core dump.
 *
 * @param[in,out]     dump_addr   Address where the registers are dumped .
 * @param[in]         max_size    Maximum allowed size for the dump .
 *
 * @return            void
 */
void pcie_reg_dump(void *dump_addr, uint32 max_size)
{
   if(max_size >= sizeof(pcie_coredump_type))
   {
      /* Turn on boot clock */
      pcie_res_boot_clk_enable();
      pcie_core_dump((pcie_coredump_type * )dump_addr, PCIE_FULL_DUMP);
      pcie_ctx->pcie_coredump_ptr = (pcie_coredump_type *)dump_addr;
   }
}

/* ============================================================================
**  Function : pcie_init
** ============================================================================
*/
/**
 * Initializes the PCIe Link,PCIe MAC and the PHY layer and starts link enumeration.
 *
 * @param[in]         void
 *
 * @return            pcie_status_type
 *    Status of the initialization.
 */
pcie_status_type pcie_init(void)
{
   pcie_return_status status = PCIE_SUCCESS;

   if(pcie_i_is_pcie_fuse_disabled())
   {
      return(PCIE_FUSE_DISABLED);
   }

   if(!pcie_is_pcie_boot_enabled())
   {
      return(PCIE_STATUS_SUCCESS);
   }

   /* Doing osal_init before pcie_tgt_init so that we can error fatal with logs if chip_info versions are not captured */
   pcie_osal_init();

   /* Initalize PCIe context */
   status = pcie_ctx_init();
   if(PCIE_SUCCESS != status)
   {
      return(PCIE_STATUS_ERROR);
   }

   /* Perform target sw-specific initialization */
   status = pcie_tgt_init(&pcie_dev, &pcie_ctx->pcie_tgt_cfg);
   if(PCIE_SUCCESS != status)
   {
      return(PCIE_STATUS_ERROR);
   }

   /* Initialize PCIe Internals */
   pcie_i_init();

   /* Check for PCIe Link up */
   if(pcie_i_read_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_PM_STTS, XMLH_LINK_UP)) == 0x1)
   {
      pcie_ctx->pcie_boot_chain = PCIE_FULL_BOOT;

      /* Post linkup sequence */
      status = pcie_i_post_linkup_sequence(PCIE_FULL_BOOT);
      if(PCIE_SUCCESS != status)
      {
         pcie_osal_debug(MSG_FATAL, "pcie postlinkup failed");
         return(PCIE_STATUS_ERROR);
      }
      pcie_osal_debug(MSG_HIGH, "Exit: XMLH_LINK_UP");
      return(PCIE_STATUS_SUCCESS);
   }

   /* Linkup sequence */
   status = pcie_i_execute_linkup_sequence();
   if(PCIE_SUCCESS != status)
   {
      pcie_osal_debug(MSG_FATAL, "pcie linkup failed");
      return(PCIE_STATUS_PHYINITFAILED);
   }

   /* Post linkup sequence */
   pcie_i_post_linkup_sequence(PCIE_NO_FULL_BOOT);
   if(PCIE_SUCCESS != status)
   {
      pcie_osal_debug(MSG_FATAL, "pcie postlinkup failed");
      return(PCIE_STATUS_ERROR);
   }
   pcie_osal_debug(MSG_HIGH, "Exit");

   return(PCIE_STATUS_SUCCESS);
}

/* ============================================================================
**  Function : pcie_deinit
** ============================================================================
*/
/**
 * Release resource allocated to PCIe module.
 *
 * @param[in]         void
 *
 * @return            void
 */
void pcie_deinit(void)
{
#ifndef  PCIE_BOOT
   pcie_interrupt_uninstall_isrs();
   pcie_gpio_deregister_isr(PCIE_GPIO_PERST);

#ifdef PCIE_CLKREQ_INTR_SUPPORTED
   pcie_gpio_deregister_isr(PCIE_GPIO_CLKREQ);
#endif /* PCIE_CLKREQ_INTR_SUPPORTED */

#endif /* PCIE_BOOT */
   pcie_osal_deinit();
}

/* ============================================================================
**  Function : pcie_add_index_obatu
** ============================================================================
*/
/**
 * Configures the PCIe iATU for outbound (EP <-> Host Memory) traffic.
 *
 * @param[in]      index         Index of IATU.
 * @param[in]      host_addr     Address of the host memory to be access enabled
 * @param[in,out]  local_addr    Address relevant in the endpoint memory space
 * @param[in]      size          size of the region
 *
 * @return         pcie_return_status
 *    Status of the operation
 */
static pcie_return_status  pcie_add_index_obatu(uint8 index, pcie_address_type host_addr, pcie_address_type *local_addr, uint32 size)
{
   pcie_atuconfig_type atu_config;

   if((1 != index) && (2 != index))
   {
      return(PCIE_FAILED);
   }

   atu_config.index = index;
   atu_config.dir = PCIE_ATUDIR_OUTBOUND;
   atu_config.mode = PCIE_ATUMODE_ADDRESSMATCHED;
   atu_config.base_addr_hi = 0x100;

   if(1 == index)
   {
      if((size >= PCIE_AXI_MEMSPACE_LIMIT) || (((PCIE_AXI_MEMOFFSET_FOR_MHICTRL + size - 1) | 0XFFF) > pcie_ctx->mhidata_lower_offset))
      {
         /* MHI data region overlap with MHI control region */
         pcie_osal_debug(MSG_FATAL, "MHI data region overlap with MHI control region");
         return(PCIE_FAILED);
      }

      pcie_ctx->mhictrl_upper_offset = PCIE_AXI_MEMOFFSET_FOR_MHICTRL + size - 1;
      pcie_ctx->mhictrl_upper_offset = pcie_ctx->mhictrl_upper_offset | 0XFFF;
      atu_config.base_addr_low = pcie_dev->memmap_base_phys + PCIE_AXI_MEMOFFSET_FOR_MHICTRL;
      local_addr->pa_hi = 0;
      local_addr->pa_low = pcie_dev->memmap_base_phys + PCIE_AXI_MEMOFFSET_FOR_MHICTRL;
      local_addr->va_hi = 0;
      local_addr->va_low = pcie_dev->memmap_base + PCIE_AXI_MEMOFFSET_FOR_MHICTRL;

   }
   else if(2 == index)
   {
      if((size >= PCIE_AXI_MEMSPACE_LIMIT) || (((PCIE_AXI_MEMSPACE_LIMIT - size) & 0XFFFFF000) < pcie_ctx->mhictrl_upper_offset))
      {
         /* MHI data region overlap with MHI control region */
         pcie_osal_debug(MSG_FATAL, "MHI data region overlap with MHI control region");
         return(PCIE_FAILED);
      }
      pcie_ctx->mhidata_lower_offset = (PCIE_AXI_MEMSPACE_LIMIT - size) & 0XFFFFF000;
      atu_config.base_addr_low = pcie_dev->memmap_base_phys + pcie_ctx->mhidata_lower_offset;
      local_addr->pa_hi = 0;
      local_addr->pa_low = pcie_dev->memmap_base_phys + pcie_ctx->mhidata_lower_offset;
      local_addr->va_hi = 0;
      local_addr->va_low = pcie_dev->memmap_base + pcie_ctx->mhidata_lower_offset;
   }

   atu_config.target_addr_hi = host_addr.pa_hi;
   atu_config.target_addr_low = host_addr.pa_low;
   atu_config.size = size;
   pcie_core_setatuconfig(TRUE, &atu_config);

   return(PCIE_SUCCESS);
}

/* ============================================================================
**  Function : pcie_remove_index_obatu
** ============================================================================
*/
/**
 * Configures the iATU in the PCIe for outbound (EP <-> Host Memory) traffic.
 *
 * @param[in]      index     Index of IATU.
 *
 * @return         void
 */
void pcie_remove_index_obatu(uint8 index)
{
   /* Setup the Index Register */
   pcie_i_write_reg(pcie_dev->dbi_base, IATU_VIEWPORT_REG, index);
   pcie_osal_memorybarrier();

   /* Disabled the index in IATU */
   pcie_i_write_reg(pcie_dev->dbi_base, PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0, 0X0);
   if(index == 1)
   {
      pcie_ctx->mhictrl_upper_offset = PCIE_AXI_MEMOFFSET_FOR_MHICTRL + 0XFFF;
   }
   else if(index == 2)
   {
      pcie_ctx->mhidata_lower_offset = PCIE_AXI_MEMSPACE_LIMIT - 0X1000;
   }
}

/* ============================================================================
**  Function : pcie_read_pa
** ============================================================================
*/
/**
 * Reads data from the host memory to a physical address via PCIe link.
 *
 * @param[in,out]     buf             Physical buffer to read into
 * @param[in]         pcie_axi_addr   PCIe AXI address in modem address space
 * @param[in]         size            Size of the data to be read in bytes
 * @param[in,out]     user_data       User data for this tranaction
 * @param[in]         options         User provided read options
 *
 * @return        uint32
 *    PCIE_STATUS_SUCCESS - Successfully .
 *    PCIE_STATUS_ERROR - An error code .
 */
uint32 pcie_read_pa(void *buf, uint32 pcie_axi_addr, uint32 size, void *user_data, uint32 options)
{
   uint32 retval,ret = PCIE_STATUS_SUCCESS;
   pcie_dma_req_type read_req = { 0 };

   read_req.device_phy_addr_low = (uint32)buf;
   read_req.host_phy_addr_low = pcie_axi_addr;
   read_req.size = size;
   read_req.user_data = user_data;
   read_req.rw_options = options;

#ifdef PCIE_AHB_AXI_CLK_CHECK
   if(pcie_res_get_interface_clk_state())
   {
      /* Error fatal */
      pcie_osal_debug(MSG_FATAL, "pcie_core_is_ahb_or_axi_clk_off is true");
      return(PCIE_STATUS_ERROR);
   }
#endif /* PCIE_AHB_AXI_CLK_CHECK */

#ifdef PCIE_BOOT
   /* Use CPU memcpy in boot for Sync trasactions */
   if(NULL == user_data)
   {
      if(0 == pcie_osal_memcpy(buf, size, (void*)pcie_axi_addr, size))
      {
         pcie_osal_debug(MSG_FATAL, "pcie_osal_memcpy failed!");
         ret = PCIE_STATUS_ERROR;
      }
   }
   else
   {
      retval = pcie_dma_read(&read_req);
      if(PCIE_DMA_ACCEPTED == retval)
      {
         ret =  PCIE_STATUS_SUCCESS;
      }
      else if(PCIE_DMA_CHANNEL_FULL == retval)
      {
         ret = PCIE_TRANSFER_REQUESTS_FULL;
      }
      else
      {
         ret = PCIE_STATUS_ERROR;
      }
   }
#else
   retval = pcie_dma_read(&read_req);
   if(PCIE_DMA_ACCEPTED == retval)
   {
      ret =  PCIE_STATUS_SUCCESS;
   }
   else if(PCIE_DMA_CHANNEL_FULL == retval)
   {
      ret = PCIE_TRANSFER_REQUESTS_FULL;
   }
   else
   {
      ret = PCIE_STATUS_ERROR;
   }
#endif /* PCIE_BOOT */
   return(ret);
}

/* ============================================================================
**  Function : pcie_write_pa
** ============================================================================
*/
/**
 * Writes data to the host memory using a physical buffer
 * Writes data to the host memory via PCIe link.
 *
 * @param[in,out]     buf             Buffer that contains data to be written  into host
 * @param[in]         pcie_axi_addr   PCIe AXI virtual address in modem address space
 * @param[in]         size            Size of the data to be written in bytes
 * @param[in,out]     user_data       User data for this tranaction
 * @param[in]         options         User provided write options
 *
 * @return        uint32
 *    PCIE_STATUS_SUCCESS - Successfully .
 *    PCIE_STATUS_ERROR - An error code .
 */
uint32 pcie_write_pa(void *buf, uint32 pcie_axi_addr, uint32 size, void *user_data, uint32 options)
{
   uint32 retval,ret = PCIE_STATUS_SUCCESS;
   pcie_dma_req_type write_req = { 0 };

   write_req.device_phy_addr_low = (uint32)buf;
   write_req.host_phy_addr_low = pcie_axi_addr;
   write_req.size = size;
   write_req.user_data = user_data;
   write_req.rw_options = options;

#ifdef PCIE_AHB_AXI_CLK_CHECK
   if(pcie_res_get_interface_clk_state())
   {
      /* Error fatal */
      pcie_osal_debug(MSG_FATAL, "pcie_core_is_ahb_or_axi_clk_off is true");
      return(PCIE_STATUS_ERROR);
   }
#endif /* PCIE_AHB_AXI_CLK_CHECK */

#ifdef PCIE_BOOT
      /* Use CPU memcpy in boot for Sync trasactions */
      if(NULL == user_data)
      {
         if(0 == pcie_osal_memcpy((void*)pcie_axi_addr, size, buf, size))
         {
            pcie_osal_debug(MSG_FATAL, "memcpy failed!");
            ret = PCIE_STATUS_ERROR;
         }
      }
      else
      {
         retval = pcie_dma_write(&write_req);
         if(PCIE_DMA_ACCEPTED == retval)
         {
            ret =  PCIE_STATUS_SUCCESS;
         }
         else if(PCIE_DMA_CHANNEL_FULL == retval)
         {
            ret = PCIE_TRANSFER_REQUESTS_FULL;
         }
         else
         {
            ret = PCIE_STATUS_ERROR;
         }
      }
#else
      retval = pcie_dma_write(&write_req);
      if(PCIE_DMA_ACCEPTED == retval)
      {
         ret =  PCIE_STATUS_SUCCESS;
      }
      else if(PCIE_DMA_CHANNEL_FULL == retval)
      {
         ret = PCIE_TRANSFER_REQUESTS_FULL;
      }
      else
      {
         ret = PCIE_STATUS_ERROR;
      }
#endif /* PCIE_BOOT */
   return(ret);
}

/* ============================================================================
**  Function : pcie_set_mhi_ipa_dbs_addr
** ============================================================================
*/
/**
 * Set registers for routing doorbells
 *
 * @param[in]     cdb_base            Channel doorbell base
 * @param[in]     cdb_end             Channel doorbell end
 * @param[in]     edb_base            Event doorbell base
 * @param[in]     edb_end             Event doorbell end
 * @param[in]     mhi_ipa_cdb_addr    MHI IPA channel Doorbell Target address
 * @param[in]     mhi_ipa_edb_addr    MHI IPA event Doorbell Target address
 *
 * @return        void
 */
void pcie_set_mhi_ipa_dbs_addr(uint8 cdb_base, uint8 cdb_end, uint8 edb_base, uint8 edb_end, uint32 mhi_ipa_cdb_addr, uint32 mhi_ipa_edb_addr)
{
   uint32 reg_val;
   reg_val = ((uint32)cdb_base) | ((uint32)cdb_end << 8) | ((uint32)edb_base << 16) | ((uint32)edb_end << 24);
   pcie_i_write_reg(pcie_dev->parf_base, PCIE20_PARF_MHI_IPA_DBS, reg_val);
   pcie_i_write_reg(pcie_dev->parf_base, PCIE20_PARF_MHI_IPA_CDB_TARGET_LOWER, mhi_ipa_cdb_addr);
   pcie_i_write_reg(pcie_dev->parf_base, PCIE20_PARF_MHI_IPA_EDB_TARGET_LOWER, mhi_ipa_edb_addr);
}

/* ============================================================================
**  Function : pcie_get_msi_config
** ============================================================================
*/
/**
 * Return MSI configuration information.
 *
 * @param[in,out]      pcie_msi_info     PCIE MSI information.
 *
 * @return         void
 */
void pcie_get_msi_config(pcie_msiconfig_type *pcie_msi_info)
{
   pcie_msiconfig_type curr_msi_config = { 0 };
   pcie_atuconfig_type atu_config = { 0 };

   if(0x0 == pcie_ctx->pcie_msi_config.enabled)
   {
      /* Check if the MSI info has changed */
      pcie_core_getmsiconfig(&curr_msi_config);

      if((curr_msi_config.enabled != pcie_ctx->pcie_msi_config.enabled) ||
         (curr_msi_config.addr_hi != pcie_ctx->pcie_msi_config.addr_hi) ||
         (curr_msi_config.addr_low != pcie_ctx->pcie_msi_config.addr_low) ||
         (curr_msi_config.data != pcie_ctx->pcie_msi_config.data) ||
         (curr_msi_config.num_multi_msgs != pcie_ctx->pcie_msi_config.num_multi_msgs)
         )
      {
         /* MSI info has changed. Setup the ATU to redirect to this new region */
         pcie_ctx->pcie_msi_config = curr_msi_config;
         atu_config.index = PCIE_OATU_INDEX_MSI;
         atu_config.dir = PCIE_ATUDIR_OUTBOUND;
         atu_config.mode = PCIE_ATUMODE_ADDRESSMATCHED;
         atu_config.base_addr_hi = 0x100;
         atu_config.base_addr_low = ((pcie_dev->memmap_base_phys + PCIE_AXI_MEMOFFSET_FOR_MSI) & ~0xFFF);
         atu_config.target_addr_hi = pcie_ctx->pcie_msi_config.addr_hi;
         atu_config.target_addr_low = pcie_ctx->pcie_msi_config.addr_low;
         atu_config.size = 0x1000;

         pcie_core_setatuconfig(TRUE, &atu_config);
         pcie_ctx->pcie_msi_config.local_addr.pa_hi = pcie_ctx->pcie_msi_config.local_addr.va_hi = 0x0;
         pcie_ctx->pcie_msi_config.local_addr.pa_low = (((pcie_dev->memmap_base_phys + PCIE_AXI_MEMOFFSET_FOR_MSI) & ~0xFFF) | (pcie_ctx->pcie_msi_config.addr_low & 0xFFF));
         pcie_ctx->pcie_msi_config.local_addr.va_low = (((pcie_dev->memmap_base + PCIE_AXI_MEMOFFSET_FOR_MSI) & ~0xFFF) | (pcie_ctx->pcie_msi_config.addr_low & 0xFFF));
      }
   }

   pcie_msi_info->addr_low = pcie_ctx->pcie_msi_config.addr_low;
   pcie_msi_info->addr_hi = pcie_ctx->pcie_msi_config.addr_hi;
   pcie_msi_info->data = pcie_ctx->pcie_msi_config.data;
   pcie_msi_info->enabled = pcie_ctx->pcie_msi_config.enabled;
   pcie_msi_info->num_multi_msgs = pcie_ctx->pcie_msi_config.num_multi_msgs;

   pcie_msi_info->local_addr.pa_hi = pcie_msi_info->local_addr.va_hi = 0x0;
   pcie_msi_info->local_addr.pa_low = (((pcie_dev->memmap_base_phys + PCIE_AXI_MEMOFFSET_FOR_MSI) & ~0xFFF) | (pcie_ctx->pcie_msi_config.addr_low & 0xFFF));
   pcie_msi_info->local_addr.va_low = (((pcie_dev->memmap_base + PCIE_AXI_MEMOFFSET_FOR_MSI) & ~0xFFF) | (pcie_ctx->pcie_msi_config.addr_low & 0xFFF));
}

/* ============================================================================
**  Function : pcie_is_pcie_boot_enabled
** ============================================================================
*/
/**
 * Checks if PCIe boot is enabled.
 *
 * @param[in]      void
 *
 * @return         Boolean
 *    TRUE  - if PCIe boot is enabled
 *    FALSE - if PCIe boot is not enabled
 */
boolean  pcie_is_pcie_boot_enabled(void)
{
#ifndef PCIE_BOOT
   uint32 read_fuse;

   read_fuse = HWIO_BOOT_CONFIG_INM(HWIO_BOOT_CONFIG_FAST_BOOT_BMSK);
   read_fuse >>= HWIO_BOOT_CONFIG_FAST_BOOT_SHFT;
#ifndef PCIE_ENABLED
   if((0x1 == read_fuse) || DalPlatformInfo_IsFusion())
   {
      return(TRUE);
   }
#else
   return (TRUE);
#endif /* PCIE_ENABLED */
   return(FALSE);
#else
   return(TRUE);
#endif /* PCIE_BOOT */
}

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_register_link_notifcation_cb
** ============================================================================
*/
/**
 * Register link notification call back with PCIe driver.
 *
 * @param[in,out]      pm_cb         Call back function
 * @param[in,out]      param         pointer to parameters provided by client.
 *
 * @return         void
 */
void pcie_register_link_notifcation_cb(pcie_i_link_notification_cb_fn_type pm_cb, void *param)
{
   pcie_osal_debug(MSG_HIGH, "Registered Power management callback");
   pcie_i_register_link_notification_cb(pm_cb, &param);
}
#endif /* PCIE_BOOT */

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_wakeup_host
** ============================================================================
*/
/**
 * Assert GPIO siganl WAKE# to wakeup host.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_wakeup_host(void)
{
   pcie_i_assert_wake();
}
#endif /* PCIE_BOOT */

/* ============================================================================
**  Function : pcie_enable_ltssm
** ============================================================================
*/
/**
 * API to enable LTSSSM.
 *
 * @param[in]      void
 *
 * @return         void
 *
 * @SideEffect
 * wake gpio is deasserted
 */
void pcie_enable_ltssm(void)
{
   uint64 time_start = 0;

   pcie_core_enableltssm();

   time_start = pcie_osal_get_systime_ms();

   /* Wait till enumeration completes */
   while((PCIE_LINKSTATUS_ENUMERATED != pcie_i_get_link_status(&pcie_ctx->pcie_link_info)) &&
         (pcie_dev->pcie_enumeration_timeout > (pcie_osal_get_systime_ms() - time_start)))
   {
      pcie_osal_sleep(2);
   }
   if(PCIE_LINKSTATUS_ENUMERATED != pcie_i_get_link_status(&pcie_ctx->pcie_link_info))
   {
      pcie_osal_debug(MSG_FATAL, "Enumeration did not happen within timeout");
   }

   /* Deassert wake only after the enumeration is complete*/
   pcie_i_deassert_wake();

   pcie_osal_debug(MSG_HIGH, "Establish PCIe link");

   pcie_send_ltr(pcie_dev->pcie_default_ltr);
}

#ifndef  PCIE_BOOT
/* ============================================================================
**  Function : pcie_read_perst_pin_value
** ============================================================================
*/
/**
 * Reads and returns the current value on the perst pin input.
 *
 * @param[in]  void
 *
 * @return     pcie_pin_value_type
 *    PCIE_PIN_HIGH - if perst pin is high
 *    PCIE_PIN_LOW  - if perst pin is low
 */
pcie_pin_value_type pcie_read_perst_pin_value(void)
{
   if(PCIE_GPIO_HIGH == pcie_gpio_get_state(PCIE_GPIO_PERST))
   {
      pcie_osal_debug(MSG_HIGH, "PERST pin high");
      return(PCIE_PIN_HIGH);
   }
   else
   {
      pcie_osal_debug(MSG_HIGH, "PERST pin low");
      return(PCIE_PIN_LOW);
   }
}
#endif /* PCIE_BOOT */

/* ============================================================================
**  Function : pcie_enable_endpoint
** ============================================================================
*/
/**
 * Handle PCIe link reestablishment.
 *
 * @param[in]  void
 *
 * @return         void
 */
void pcie_enable_endpoint(void)
{
#ifndef  PCIE_BOOT
   pcie_osal_debug(MSG_HIGH, "Begin");

   if(PCIE_GPIO_HIGH != pcie_gpio_get_state(PCIE_GPIO_PERST))
   {
      pcie_osal_debug(MSG_HIGH, "PERST pin LOW. Exit without enabling EP");
      return;
   }

   pcie_res_enable();
   
   /* Enable only relevant SW interrupt */
   pcie_i_enable_sw_int();

   pcie_res_restore_tz_sys_call();
   pcie_interrupt_install_isrs();
   pcie_core_enable_interrupt();

   /* Initialize the PHY */
   pcie_phy_init();

   pcie_core_config();

   pcie_osal_debug(MSG_HIGH, "Exit");
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_disable_endpoint
** ============================================================================
*/
/**
 * Handle PCIe link down.
 *
 * @param[in]    void
 *
 * @return       void
 */
void pcie_disable_endpoint(void)
{
#ifndef  PCIE_BOOT
   pcie_osal_debug(MSG_HIGH, "Begin");

   /* Unlock previously locked regions */
   pcie_i_unlock_regions();

   /* Disable MSI so it will get re-enabled and ATU will be configured appropriately */
   pcie_ctx->pcie_msi_config.enabled = FALSE;

   pcie_res_disable();

   pcie_interrupt_uninstall_isrs();

   pcie_osal_debug(MSG_HIGH, "Exit");
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_get_link_state
** ============================================================================
*/
/**
 * Return current PCIe link state.
 *
 * @param[in]      void
 *
 * @return         pcie_link_state_type
 *    PCIE_LINK_STATE_LINK_DOWN
 *    PCIE_LINK_STATE_L0
 *    PCIE_LINK_STATE_L0S
 *    PCIE_LINK_STATE_L1
 *    PCIE_LINK_STATE_L1SS
 *    PCIE_LINK_STATE_L2
 *    PCIE_LINK_STATE_OTHERS
 */
pcie_link_state_type pcie_get_link_state(void)
{
   uint8 reg_fld_val;

   reg_fld_val = (uint8)pcie_core_get_link_status(&pcie_ctx->pcie_link_info.link_state);
   if(0x0 == reg_fld_val)
   {
      return(PCIE_LINK_STATE_LINK_DOWN);
   }

   if(PCIE_LINKSTATE_L0 == pcie_ctx->pcie_link_info.link_state)
   {
      return(PCIE_LINK_STATE_L0);
   }
   else if(PCIE_LINKSTATE_L0S == pcie_ctx->pcie_link_info.link_state)
   {
      return(PCIE_LINK_STATE_L0S);
   }
   else if(PCIE_LINKSTATE_L1_IDLE == pcie_ctx->pcie_link_info.link_state)
   {
      return (PCIE_LINK_STATE_L1);
   }
   else if(PCIE_LINKSTATE_L2_IDLE == pcie_ctx->pcie_link_info.link_state)
   {
      return (PCIE_LINK_STATE_L2);
   }
   return(PCIE_LINK_STATE_OTHERS);
}

/* ============================================================================
**  Function : pcie_change_device_state
** ============================================================================
*/
/**
 * Change device state from EP driver side. Used only for internal testing purpose.
 *
 * @param[in]      d_state device state( o for D0 state, 3 for D3 state)
 *
 * @return         void
 */
void pcie_change_device_state(uint8 d_state)
{
   pcie_i_write_reg_field(pcie_dev->dbi_base, PCIE_FLD(TYPE0_CON_STATUS_REG_1, POWER_STATE), d_state);
   pcie_osal_debug(MSG_HIGH, "Change Device state to : %d", d_state, 0, 0);
}

/* ============================================================================
**  Function : pcie_send_ltr
** ============================================================================
*/
/**
 * Sends the LTR.
 *
 * @param[in]      ltr_us    LTR value in usecs (max value is ~ 30 seconds)
 *
 * @return         void
 */
void pcie_send_ltr(uint32 ltr_us)
{
   pcie_core_send_ltr(ltr_us);
}

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_control_l1ss_inact_interrupt
** ============================================================================
*/
/**
 * API to enable or disable L1SS inactivity timer interrupt.
 *
 * @param[in]       ctrl        True to enable interrut,False to disable interrupt.
 * @param[in]       maxtimer    Max time out value in us
 *
 * @return          uint32      Mininum time in us in which the timer can expire
 */
uint32 pcie_control_l1ss_inact_interrupt(boolean ctrl,uint32 maxtimer)
{
   uint32 freq=0;
   pcie_osal_debug(MSG_HIGH,"Programming inactivity value %d",maxtimer);
   freq = pcie_core_control_l1ss_inact_timeout(ctrl,maxtimer);
   pcie_i_sw_int_ctrl(PCIE_INT_L1SS_IDLE_TO,ctrl);

   return(maxtimer*(freq/pcie_ctx->pcie_tgt_cfg->aux_clk_freq_hz));
}

/* ============================================================================
**  Function : pcie_l1ss_supported
** ============================================================================
*/
/**
 * Returns whether L1ss is supported
 *
 *
 * @return
 *     0 - False
 *     1 - True
 *
 * @dependencies
 * None.
 */
boolean pcie_l1ss_supported(void)
{
   return (pcie_core_get_link_aspm() > PCIE_L0S_ENTRY_ENABLE)?TRUE:FALSE;
}
#endif /* PCIE_BOOT */

/* ============================================================================
**  Function : pcie_set_host2dev_mmap
** ============================================================================
*/
/**
 * Configures the iATU in the PCIe for outbound (EP <-> Host Memory) traffic.
 *
 * @param[in]      atu_array      Array of ATU configuration
 * @param[in]      entry_count     atu_array size
 *
 * @return         uint16
 *    Status of the operation
 */
int16 pcie_set_host2dev_mmap(pcie_iatu_entry_type atu_array[], uint8 entry_count)
{
   uint32 temp;
   pcie_address_type host_ctrl_addr;
   pcie_address_type *local_ctrl_addr;
   uint64 ctrl_size;
   pcie_address_type host_data_addr;
   pcie_address_type *local_data_addr;
   uint64 data_size;

   if((NULL == atu_array) || (entry_count > 2))
   {
      pcie_osal_debug(MSG_FATAL, "atu_array is NULL or entry_count = %d > 2", entry_count);
      return(-1);
   }

   host_ctrl_addr = atu_array[0].host_addr;
   local_ctrl_addr = atu_array[0].local_addr;
   ctrl_size = atu_array[0].size;

   host_data_addr = atu_array[1].host_addr;
   local_data_addr = atu_array[1].local_addr;
   data_size = atu_array[1].size;

   pcie_ctx->pcie_atu_debug_info.host_ctrl = host_ctrl_addr;
   pcie_ctx->pcie_atu_debug_info.ctrl_size = ctrl_size;
   pcie_ctx->pcie_atu_debug_info.host_data = host_data_addr;
   pcie_ctx->pcie_atu_debug_info.data_size = data_size;

   /* Disable ATU index 1 and 2 before configure ATU to make sure previous configuration is cleaned up */
   pcie_remove_index_obatu(1);
   pcie_remove_index_obatu(2);

   pcie_ctx->pcie_atu_debug_info.atu1_used = 1;
   pcie_ctx->pcie_atu_debug_info.atu2_used = 1;

   if(host_ctrl_addr.pa_hi == host_data_addr.pa_hi)
   {
      /* There is no overlapping between Control and data region */
      if(((host_ctrl_addr.pa_low < host_data_addr.pa_low) && (ctrl_size <= (host_data_addr.pa_low - host_ctrl_addr.pa_low))) ||
         ((host_ctrl_addr.pa_low > host_data_addr.pa_low) && (data_size <= (host_ctrl_addr.pa_low - host_data_addr.pa_low)))
         )
      {
         pcie_add_index_obatu(1, host_ctrl_addr, local_ctrl_addr, ctrl_size);
         pcie_add_index_obatu(2, host_data_addr, local_data_addr, data_size);
         goto end;
      }
      /* Control region is inclusive in Data region. only ATU index 2 is needed */
      else if((host_ctrl_addr.pa_low >= host_data_addr.pa_low) &&
         ((host_ctrl_addr.pa_low + ctrl_size) <= (host_data_addr.pa_low + data_size))
         )
      {
         pcie_add_index_obatu(2, host_data_addr, local_data_addr, data_size);
         local_ctrl_addr->pa_hi = 0;
         local_ctrl_addr->va_hi = 0;
         local_ctrl_addr->pa_low = local_data_addr->pa_low + host_ctrl_addr.pa_low - host_data_addr.pa_low;
         local_ctrl_addr->va_low = local_data_addr->va_low + host_ctrl_addr.pa_low - host_data_addr.pa_low;

         pcie_ctx->pcie_atu_debug_info.atu1_used = 0;
         pcie_ctx->pcie_atu_debug_info.atu2_used = 1;

         goto end;
      }
      /* Data region start address is within Control region . Consolidate the overlapped region */
      else if((host_data_addr.pa_low > host_ctrl_addr.pa_low) &&
         (host_data_addr.pa_low < (host_ctrl_addr.pa_low + ctrl_size))
         )
      {
         pcie_add_index_obatu(1, host_ctrl_addr, local_ctrl_addr, (host_data_addr.pa_low - host_ctrl_addr.pa_low));
         pcie_add_index_obatu(2, host_data_addr, local_data_addr, data_size);
         goto end;
      }
      /* Control region start address is within data region . Consolidate the overlapped region */
      else if((host_ctrl_addr.pa_low > host_data_addr.pa_low) &&
         (host_ctrl_addr.pa_low < (host_data_addr.pa_low + data_size))
         )
      {
         pcie_add_index_obatu(1, host_ctrl_addr, local_ctrl_addr, ctrl_size);
         pcie_add_index_obatu(2, host_data_addr, local_data_addr, (host_ctrl_addr.pa_low - host_data_addr.pa_low));
         goto end;
      }
      else
      {
         /* Conditions not handled Error fatal */
         pcie_osal_debug(MSG_FATAL, "Unhandled conditions  host_ctrl_addr.pa_low=0x%x  host_ctrl_addr.pa_hi=0x%x  host_data_addr.pa_low=0x%x  host_data_addr.pa_hi=0x%x", host_ctrl_addr.pa_low, host_ctrl_addr.pa_hi, host_data_addr.pa_low, host_data_addr.pa_hi);
         return(-1);
      }
   }
   else if((host_ctrl_addr.pa_hi == 0) && (host_data_addr.pa_hi != 0))
   {
      /* There is no overlapping between Control and data region */
      if(((0xFFFFFFFF - host_ctrl_addr.pa_low + 1) >= ctrl_size) ||
         (host_data_addr.pa_low >= (ctrl_size - (0XFFFFFFFF - host_ctrl_addr.pa_low + 1)))
         )
      {
         pcie_add_index_obatu(1, host_ctrl_addr, local_ctrl_addr, ctrl_size);
         pcie_add_index_obatu(2, host_data_addr, local_data_addr, data_size);
         goto end;
      }
      /* Data region start address is within Control region . Consolidate the overlapped region */
      else
      {
         temp = 0XFFFFFFFF - host_ctrl_addr.pa_low + 1 + host_data_addr.pa_low;
         pcie_add_index_obatu(1, host_ctrl_addr, local_ctrl_addr, temp);
         pcie_add_index_obatu(2, host_data_addr, local_data_addr, data_size);
         goto end;
      }
   }
   else if((host_ctrl_addr.pa_hi != 0) && (host_data_addr.pa_hi == 0))
   {
      /* There is no overlapping between Control and data region */
      if(((0xFFFFFFFF - host_data_addr.pa_low + 1) >= data_size) ||
         (host_ctrl_addr.pa_low >= (data_size - (0XFFFFFFFF - host_data_addr.pa_low + 1)))
         )
      {
         pcie_add_index_obatu(1, host_ctrl_addr, local_ctrl_addr, ctrl_size);
         pcie_add_index_obatu(2, host_data_addr, local_data_addr, data_size);
         goto end;
      }
      /* Data region start address is within Control region . Consolidate the overlapped region */
      else
      {
         temp = 0XFFFFFFFF - host_data_addr.pa_low + 1 + host_ctrl_addr.pa_low;
         pcie_add_index_obatu(1, host_ctrl_addr, local_ctrl_addr, ctrl_size);
         pcie_add_index_obatu(2, host_data_addr, local_data_addr, temp);
         goto end;
      }
   }
   else
   {
      /* Conditions not handled Error fatal */
      pcie_osal_debug(MSG_FATAL, "Unhandled conditions host_ctrl_addr.pa_low=0x%x  host_ctrl_addr.pa_hi=0x%x  host_data_addr.pa_low=0x%x  host_data_addr.pa_hi=0x%x", host_ctrl_addr.pa_low, host_ctrl_addr.pa_hi, host_data_addr.pa_low, host_data_addr.pa_hi);
      return(-1);
   }
end:

   /* Update atu debug info */
   pcie_ctx->pcie_atu_debug_info.local_ctrl = *local_ctrl_addr;
   pcie_ctx->pcie_atu_debug_info.local_data = *local_data_addr;

   /* Unlock the already locked regions */
   pcie_i_unlock_regions();

   /* Lock unused regions */
   if(!pcie_ctx->pcie_atu_debug_info.atu1_used)
   {
      pcie_i_lock_regions(0x1000,
                               pcie_ctx->pcie_atu_debug_info.local_data.va_low-pcie_dev->memmap_base-0x1000);
   }
   else
   {
      pcie_i_lock_regions(pcie_ctx->pcie_atu_debug_info.local_ctrl.va_low-pcie_dev->memmap_base+((pcie_ctx->pcie_atu_debug_info.ctrl_size+0xFFF)&~0xFFF),
                               pcie_ctx->pcie_atu_debug_info.local_data.va_low-(pcie_ctx->pcie_atu_debug_info.local_ctrl.va_low+((pcie_ctx->pcie_atu_debug_info.ctrl_size+0xFFF)&~0xFFF)));
   }

   return(0);
}

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_pm_enter_lpm
** ============================================================================
*/
/**
 * Request PCIe to enter low power mode
 *
 * @param[in]         void
 *
 * @return            pcie_status_type
 *    PCIE_STATUS_ERROR    if the clkreq gpio is ASSERTED(low) while enabling interrupt
 *    PCIE_STATUS_SUCCESS  if successful.
 */
pcie_status_type pcie_pm_enter_lpm(void)
{
   pcie_status_type ret = PCIE_STATUS_SUCCESS;

   if(PCIE_FAILED == pcie_i_check_for_l1ss())
   {
      ret = PCIE_STATUS_ERROR;
      return (ret);
   }

   /* Enter LPM */
   if(PCIE_FAILED == pcie_i_enter_lpm())
   {
      return (PCIE_STATUS_ERROR);
   }

   pcie_res_suspend();

   return(ret);
}
#endif /* PCIE_BOOT */

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_pm_exit_lpm
** ============================================================================
*/
/**
 * Request PCIe to exit low power mode
 *
 * @param[in]     void
 *
 * @return        void
 */
void pcie_pm_exit_lpm(void)
{
   pcie_res_resume();
   pcie_i_exit_lpm();
}
#endif /* PCIE_BOOT */

/* ============================================================================
**  Function : pcie_set_dma_state
** ============================================================================
*/
/**
 * Used by upper layer client to indicate if it suspended the DMA
 *
 * @param[in]      state      ENABLED/DISABLED
 *
 * @return         void
 */
void pcie_set_dma_state(boolean state)
{
   pcie_dma_set_state(state);
}

/* ============================================================================
**  Function : pcie_register_dma_cb
** ============================================================================
*/
/**
 * Register callbacks for DMA
 *
 * @param[in]      read_cb      Read callback
 * @param[in]      write_cb     Write callback
 *
 * @return         void
 */
void pcie_register_dma_cb(dma_req_cb read_cb, dma_req_cb write_cb)
{
   pcie_dma_register_cb(read_cb);
}

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_device_enable_EnterL23
** ============================================================================
*/
/**
 * Send the ready enter L23 DLLP to the RC. This should be done when device
 * is in D3hot state and is ready to enter D3cold.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_device_enable_EnterL23(void)
{
   /* Indicate to core that link  is ready to enter L23 */
   pcie_i_write_reg_field(pcie_dev->parf_base, PCIE_FLD(PCIE20_PARF_PM_CTRL, READY_ENTR_L23), 1);
   pcie_osal_debug(MSG_HIGH, "pcie_device_send_EnterL23 sent");
}
#endif /* PCIE_BOOT */

/* ============================================================================
 **  Function : pcie_flush_dma
 ** ============================================================================
 */
/**
 * Setup a transaction to flush out all descriptors
 *
 * @param[in]     void
 *
 * @return        void
 */
void pcie_flush_dma(void)
{
#ifndef PCIE_BOOT
   pcie_dma_flush();
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_poll_dma
** ============================================================================
*/
/**
 * API for PCIe clients to poll for dma request
 *
 * @param[in]     Void
 *
 * @return
 *     0 - Successful
 *    -1 - Error
 */
int pcie_poll_dma(void)
{
   int ret=0;
#ifdef PCIE_BOOT
   if(PCIE_FAILED == pcie_dma_poll())
      ret = -1;
#endif /* PCIE_BOOT */
   return(ret);
}

/* ============================================================================
**  Function : pcie_trigger_perst_int
** ============================================================================
*/
/**
 * Trigger PERST GPIO interrupt.. Used for internal testing purpose only.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_trigger_perst_int(void)
{
#ifndef PCIE_BOOT
   pcie_gpio_trigger_perst_int();
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_enable_mhi_interrupts
** ============================================================================
*/
/**
 * Enable MHI core interrupts.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_enable_mhi_interrupts(void)
{
#ifndef PCIE_BOOT
   pcie_i_sw_int_ctrl(PCIE_INT_MHI_A7,TRUE);
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_disable_mhi_interrupts
** ============================================================================
*/
/**
 * Disable MHI core interrupts.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_disable_mhi_interrupts(void)
{
#ifndef PCIE_BOOT
   pcie_i_sw_int_ctrl(PCIE_INT_MHI_A7,FALSE);
#endif /* PCIE_BOOT */
}
