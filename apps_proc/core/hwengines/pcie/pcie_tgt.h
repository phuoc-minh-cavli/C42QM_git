/**
 * @file pcie_tgt.h
 * @brief
 * Provides interfaces that provide platform specific support to PCIe
 * Device Driver
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

#ifndef __PCIE_TGT_H__
#define __PCIE_TGT_H__

#include "comdef.h"
#include "pcie_hwio.h"
#include "pcie_osal.h"
#include "pcie_gpio.h"
#include "pcie_i.h"
#include "pcie_interrupt.h"
#include "pcie_res.h"

#define PCIE_MAX_BARS                         (6)    /** Max Number of BARs per spec */

typedef struct _pcie_tgt_irqs_types
{
   uint32 pcie_irq_num;       /** < -- IRQ number */
   boolean irq_enabled;       /** < -- IRQ enabled/Disabled */
   pcie_int_trig_type trig;   /** < -- IRQ Trigger */
}pcie_tgt_irq_type;

/** Target specific Config structure */
typedef struct _pcie_tgtcfg_type
{
   uint32 device_id;                                   /** < -- Device ID */
   uint32 vendor_id;                                   /** < -- Vendor ID */
   pcie_barcfg_type bar_config[PCIE_MAX_BARS];         /** < -- Bar Config info */
   uint32 needs_init;                                  /** < -- Flag to indicate PCIe initialization */
   uint32 timeout_ms;                                  /** < -- Link enumeration Timeout in milliseconds (0 - no timeout) */

   uint32 pcie_wrapper_ahb_base_phy;                   /** < -- AHB base physical address */
   uint32 pcie_wrapper_ahb_base_size;                  /** < -- AHB size */
   uint32 pcie_wrapper_axi_base_phy;                   /** < -- AXI base physical address */
   uint32 pcie_wrapper_axi_base_size;                  /** < -- AXI size */

   uint32 elbi_reg_base_offs;                          /** < -- Offset of the ELBI regs */
   uint32 parf_reg_base_offs;                          /** < -- Offset of the PARF regs */
   uint32 mhi_reg_base_offs;                           /** < -- Offset of the MHI regs */
   uint32 dbi_reg_base_offs;                           /** < -- Offset of the DBI regs */
   uint32 phy_reg_base_offs;                           /** < -- Offset of the PHY regs */
   uint64 parf_slv_addr_space_size;                    /** < -- PARF Slave Address Space size */

   pcie_tgt_irq_type pcie_irqs[PCIE_MAX_IRQ];          /** < -- PCIe IRQs */

   pcie_res_clk_names_type pcie_clk_string;            /** < -- PCIe Clock Names string */

   uint32 aux_clk_freq_hz;                             /** < -- PCIe Aux clock frequency in Hz */

   char* pcie_power_domain_name;                       /** < -- PCIe Power domain name string */

   uint32 pcie_max_bars;                               /** < -- PCIe Max bars */
   uint32 pcie_link_timeout;                           /** < -- PCIe link timeout */

   uint32 pcie_enumeration_timeout;                    /** < -- PCIe enumeration timeout */
   uint32 pcie_default_ltr;                            /** < -- PCIe default LTR */

   pcie_res_npa_clk_param_type pcnoc;                  /** < -- PCNOC NPA parameters */
   pcie_res_npa_clk_param_type snoc;                   /** < -- SNOC NPA parameters */

   pcie_res_npa_rails_param_type cx;                   /** < -- CX NPA parameters */
   pcie_res_npa_rails_param_type mx;                   /** < -- MX NPA parameters */

   uint32 pcnoc_turbo_freq_mhz;                        /** < -- PCNOC Turbo Frequency in MHz */

   const pcie_gpio_cfg_type *pcie_gpio_config_ptr;     /** < -- Pointer to PCIe GPIO configuration */
}pcie_tgtcfg_type;

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
 * @return
 *    Status code.
 */
enum _pcie_return_status pcie_tgt_init(pcie_devinfo_type **pcie_dev, pcie_tgtcfg_type **pcie_tgt_cfg);

/* ============================================================================
**  Function : pcie_tgt_cfgcoreid
** ============================================================================
*/
/**
 * Update the target configuration for device id and vendor id at run time.
 * Update the target configuration for device id and vendor id at run time.
 * This API must be called prior to PCIe initialization to ensure that the IDs
 * are updated before the link gets enumerated.
 *
 * @param[in]  device_id    Device ID
 * @param[in]  vendor_id    Vendor ID
 *
 * @return
 *    Status code.
 *
 * @dependencies
 * pcie_init must not have been called prior to this API being called.
 * @sideeffects
 * The vendor and device IDs will be updated when PCIe core initializes.
 */
enum _pcie_return_status pcie_tgt_cfgcoreid(uint32 device_id, uint32 vendor_id);

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
 *    pcie_tgtcfg_type*
 */
pcie_tgtcfg_type* pcie_tgt_get_tgt_config(void);

#endif /* __PCIE_TGT_H__ */
