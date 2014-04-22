/**
 * @file pcie_tgt.c
 * @brief
 * Implements the interfaces that provide platform specific support to PCIe
 * Device Driver
 */
/*
===============================================================================

Edit History

$Header:

Change revision history maintained in version system
===============================================================================
                   Copyright (c) 2013-15 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "pcie_tgt.h"
#include "pcie_gpio.h"
#include "pcie_interrupt.h"
#include "DDIChipInfo.h"

/** Target specific pcie device info */
static pcie_devinfo_type *pcie_dev_info;

#ifdef PCIE_XML_PROPS
/** DALSYS property handle for target */
DALSYS_PROPERTY_HANDLE_DECLARE(hProppcie);

/** DALSYS property handle for GPIOs */
DALSYS_PROPERTY_HANDLE_DECLARE(hProppcie_gpio);
#endif

/** Pointer to Target configuration */
pcie_tgtcfg_type *pcie_tgt_config_ptr = NULL;

/* ============================================================================
**  Function : pcie_tgt_init_chip_info
** ============================================================================
*/
/**
 * Initializes the chip info.
 *
 * @param[in]  void
 *
 * @return     void
 */
static void pcie_tgt_init_chip_info(void)
{
   DALResult dal_result;
   DalDeviceHandle *ChipInfo = NULL;
   static DalChipInfoVersionType nVer_info;

   pcie_osal_debug(MSG_HIGH, "Initializing Chip Info");

   dal_result = DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &ChipInfo);
   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DAL_DeviceAttach failed");
   }

   if(NULL == ChipInfo)
   {
      pcie_osal_debug(MSG_FATAL, "ChipInfo is NULL");
      return;
   }

   dal_result = DalDevice_Open(ChipInfo, DAL_OPEN_SHARED);
   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DalDevice_Open failed");
   }

   dal_result = DalChipInfo_GetChipVersion(ChipInfo, &nVer_info);
   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DalChipInfo_GetChipVersion failed");
   }

   dal_result = DalDevice_Close(ChipInfo);
   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DalDevice_Close failed");
   }

   pcie_dev_info->dev_version = (uint32)nVer_info;
}

/* ============================================================================
**  Function : pcie_tgt_init
** ============================================================================
*/
/**
 * Performs target specific initialization like setting up hwio bases, interrupt
 * info, osal layer etc.
 *
 * @param[in,out]  pcie_dev         Pointer to the pcie_dev
 * @param[in,out]  pcie_tgt_cfg     Pointer to PCIe Target configuration
 *
 * @return    pcie_return_status
 *    Status of the operation.
 */
pcie_return_status pcie_tgt_init(pcie_devinfo_type **pcie_dev, pcie_tgtcfg_type **pcie_tgt_cfg)
{
#ifdef PCIE_XML_PROPS
   DALSYSPropertyVar pcieprop;
   DALSYSPropertyVar pcieprop_gpio;
   DALResult status = DAL_SUCCESS;
#endif
   uint32 irq;
   pcie_osal_meminfo pcie_ahb_mem, pcie_axi_mem;

   if(pcie_dev == NULL || pcie_tgt_cfg == NULL)
   {
      return(PCIE_FAILED);
   }

   pcie_dev_info = (pcie_devinfo_type * )pcie_osal_malloc(sizeof(pcie_devinfo_type));
   if(NULL ==  pcie_dev_info)
   {
      pcie_osal_debug(MSG_FATAL, "Could not allocate memory for pcie_dev_info");
      return(PCIE_FAILED);
   }

   pcie_osal_memset(pcie_dev_info, 0, sizeof(pcie_dev_info));

#ifdef PCIE_XML_PROPS
   /* Read target configuration */
   pcie_osal_debug(MSG_HIGH, "Reading Target configuration from XML");
   status = DALSYS_GetDALPropertyHandleStr("/core/hwengines/pcie", hProppcie);
   if(status != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DALSYS_GetDALPropertyHandleStr failed for target");
      return(PCIE_FAILED);
   }

   if((status = DALSYS_GetPropertyValue(hProppcie, "pcie_tgt_config", 0, &pcieprop)) != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DALSYS_GetPropertyValue failed for target");
      return(PCIE_FAILED);
   }
   else
      pcie_tgt_config_ptr = (pcie_tgtcfg_type * )pcieprop.Val.pStruct;

   /* Read GPIO configuration */
   pcie_osal_debug(MSG_HIGH, "Reading GPIO configuration from XML");
   status = DALSYS_GetDALPropertyHandleStr("/core/hwengines/pcie", hProppcie_gpio);
   if(status != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DALSYS_GetDALPropertyHandleStr failed for gpio");
      return(PCIE_FAILED);
   }

   if((status = DALSYS_GetPropertyValue(hProppcie_gpio, "pcie_gpio_config", 0, &pcieprop_gpio)) != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DALSYS_GetPropertyValue failed for gpio");
      return(PCIE_FAILED);
   }
   else
      pcie_dev_info->pcie_gpio_config_ptr = (pcie_gpio_cfg_type * )pcieprop_gpio.Val.pStruct;
#else
   /* Read target configuration */
   pcie_tgt_config_ptr = pcie_tgt_get_tgt_config();
   /* Read GPIO configuration */
   pcie_dev_info->pcie_gpio_config_ptr = (pcie_gpio_cfg_type * )pcie_tgt_config_ptr->pcie_gpio_config_ptr;
#endif /* pcie_XML_PROPS */

   pcie_ahb_mem.pa = pcie_tgt_config_ptr->pcie_wrapper_ahb_base_phy;
   pcie_ahb_mem.size = pcie_tgt_config_ptr->pcie_wrapper_ahb_base_size;

   pcie_osal_mallocmemregion(&pcie_ahb_mem, pcie_ahb_mem.size, PCIE_DEVICE_MAPPING);
   if(NULL == pcie_ahb_mem.handle)
   {
      pcie_osal_debug(MSG_FATAL, "Could not allocate memory for pcie_ahb_mem");
      return(PCIE_FAILED);
   }

   pcie_axi_mem.pa = pcie_tgt_config_ptr->pcie_wrapper_axi_base_phy;
   pcie_axi_mem.size = pcie_tgt_config_ptr->pcie_wrapper_axi_base_size;

   pcie_osal_mallocmemregion(&pcie_axi_mem, pcie_axi_mem.size, PCIE_DEVICE_MAPPING);
   if(NULL == pcie_axi_mem.handle)
   {
      pcie_osal_debug(MSG_FATAL, "Could not allocate memory for pcie_axi_mem");
      return(PCIE_FAILED);
   }

   pcie_dev_info->pcie_link_down_counter = 0;
   pcie_dev_info->pcie_link_up_counter = 0;

   pcie_dev_info->pcnoc.res_name = pcie_tgt_config_ptr->pcnoc.res_name;
   pcie_dev_info->pcnoc.client_name = pcie_tgt_config_ptr->pcnoc.client_name;
   pcie_dev_info->pcnoc.freq_vote = pcie_tgt_config_ptr->pcnoc.freq_vote;

   pcie_dev_info->snoc.res_name = pcie_tgt_config_ptr->snoc.res_name;
   pcie_dev_info->snoc.client_name = pcie_tgt_config_ptr->snoc.client_name;
   pcie_dev_info->snoc.freq_vote = pcie_tgt_config_ptr->snoc.freq_vote;

   pcie_dev_info->cx.res_name = pcie_tgt_config_ptr->cx.res_name;
   pcie_dev_info->cx.client_name = pcie_tgt_config_ptr->cx.client_name;
   pcie_dev_info->cx.vote = pcie_tgt_config_ptr->cx.vote;
   
   pcie_dev_info->mx.res_name = pcie_tgt_config_ptr->mx.res_name;
   pcie_dev_info->mx.client_name = pcie_tgt_config_ptr->mx.client_name;
   pcie_dev_info->mx.vote = pcie_tgt_config_ptr->mx.vote;

   pcie_dev_info->pcnoc_turbo_freq_mhz = pcie_tgt_config_ptr->pcnoc_turbo_freq_mhz;

   pcie_dev_info->perst_in_expect.expected_perst = PCIE_GPIO_LOW;

   pcie_dev_info->max_bars = pcie_tgt_config_ptr->pcie_max_bars;
   pcie_dev_info->link_timeout = pcie_tgt_config_ptr->pcie_link_timeout;

   pcie_dev_info->pcie_enumeration_timeout = pcie_tgt_config_ptr->pcie_enumeration_timeout;
   pcie_dev_info->pcie_default_ltr = pcie_tgt_config_ptr->pcie_default_ltr;

   /* Populate the IRQ numbers */
   for(irq = 0;irq < PCIE_MAX_IRQ;irq++)
   {
      pcie_dev_info->pcie_irq[irq].pcie_irq_num = pcie_tgt_config_ptr->pcie_irqs[irq].pcie_irq_num;
      pcie_dev_info->pcie_irq[irq].irq_enabled = pcie_tgt_config_ptr->pcie_irqs[irq].irq_enabled;
      pcie_dev_info->pcie_irq[irq].int_trig = pcie_tgt_config_ptr->pcie_irqs[irq].trig;
   }

   /* Set the L1 entry/exit count to 0 */
   pcie_dev_info->pcie_req_l1_cnt = 0;
   /* Set the default state of clocks to FALSE. It will be set to TRUE in resources init */
   pcie_dev_info->pcie_clocks_on = FALSE;

   /* Set the guardtime violation count to 0 */
   pcie_dev_info->guard_time_violation_count = 0;

   pcie_dev_info->parf_base = pcie_ahb_mem.va;
   pcie_dev_info->parf_base_phys = pcie_ahb_mem.pa;

   pcie_dev_info->mhi_base = pcie_ahb_mem.va + pcie_tgt_config_ptr->mhi_reg_base_offs;
   pcie_dev_info->mhi_base_phys = pcie_ahb_mem.pa + pcie_tgt_config_ptr->mhi_reg_base_offs;

   pcie_dev_info->phy_base = pcie_ahb_mem.va + pcie_tgt_config_ptr->phy_reg_base_offs;
   pcie_dev_info->phy_base_phys = pcie_ahb_mem.pa + pcie_tgt_config_ptr->phy_reg_base_offs;

   pcie_dev_info->memmap_base = pcie_axi_mem.va;
   pcie_dev_info->memmap_base_phys = pcie_axi_mem.pa;

   pcie_dev_info->pcie_sw_int_mask = 0x0;

   pcie_dev_info->dbi_base = pcie_axi_mem.va + pcie_tgt_config_ptr->dbi_reg_base_offs;
   pcie_dev_info->dbi_base_phys = pcie_axi_mem.pa + pcie_tgt_config_ptr->dbi_reg_base_offs;
   pcie_dev_info->dbi_reg_base_offs =  pcie_tgt_config_ptr->dbi_reg_base_offs;
   pcie_dev_info->parf_slv_addr_space_size = pcie_tgt_config_ptr->parf_slv_addr_space_size;

   pcie_dev_info->elbi_base = pcie_axi_mem.va + pcie_tgt_config_ptr->elbi_reg_base_offs;
   pcie_dev_info->elbi_base_phys = pcie_axi_mem.pa + pcie_tgt_config_ptr->elbi_reg_base_offs;

   /* Initialize the Chip Info */
   pcie_tgt_init_chip_info();

   /* Provide configuration to the caller */
   *pcie_dev     = pcie_dev_info;
   *pcie_tgt_cfg = pcie_tgt_config_ptr;

#ifndef PCIE_BOOT
   /* Initialize PCIe Interrupts */
   pcie_interrupt_init();
   /* Initialize PCIe GPIOs */
   pcie_gpio_init();
#endif

   return(PCIE_SUCCESS);
}

/* ============================================================================
**  Function : pcie_tgt_cfgcoreid
** ============================================================================
*/
/**
 * Update the target configuration for device id and vendor id at run time. This API must be called prior to PCIe initialization to ensure that the IDs
 * are updated before the link gets enumerated.
 *
 * @param[in]  device_id    Device ID
 * @param[in]  vendor_id    Vendor ID
 *
 * @return     pcie_return_status
 *    Status code.
 *
 * @dependencies
 * pcie_init must not have been called prior to this API being called.
 * @sideeffects
 * The vendor and device IDs will be updated when PCIe core initializes.
 */
pcie_return_status pcie_tgt_cfgcoreid(uint32 device_id, uint32 vendor_id)
{
   pcie_tgt_config_ptr->device_id = device_id;
   pcie_tgt_config_ptr->vendor_id = vendor_id;

   return(PCIE_SUCCESS);
}

/* ============================================================================
**  Function : pcie_tgt_get_tgt_config
** ============================================================================
*/
/**
 * Returns the pointer to target config
 *
 * @param[in]  void
 *
 * @return
 * pcie_tgtcfg_type* .
 */
pcie_tgtcfg_type* pcie_tgt_get_tgt_config(void)
{
   return(pcie_tgt_config_ptr);
}

