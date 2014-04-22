/*==============================================================================

High Speed USB Module Remote APIs on Modem processor 

GENERAL DESCRIPTION
This module defines High Speed USB APIs that are implemented on the modem 
processor is the server. 

EXTERNALIZED FUNCTIONS
hsu_api_init_phy
hsu_api_vbus_powerup
hsu_api_vbus_shutdown
hsu_api_update_product_id
hsu_api_update_serial_number
hsu_api_update_is_serial_number_null   
hsu_api_update_vendor_id
hsu_api_init_chg
hsu_api_chg_is_connected
hsu_api_disable_pmic_otg_comparators
hsu_api_init_uicc
hsu_api_start_uicc
hsu_api_disconnect_pulldowns_uicc
hsu_init_snps_usb

INITALIZATION AND SEQUENCING REQUIREMENTS
None.

Copyright (c) 2007-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/9205/hsu_mdm_apis.c#1 $
$DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when      who     what, where, why
--------  ---     ------------------------------------------------------------
04/08/10  dk  Added hsu_al_bulk support for flashless boot
11/16/09  jch Added to support USB charger detection with FEATURE_HS_USB_CHG_MGR
07/23/09  jc  Fixed Halcyon MSM7X30 compilation dependency on 8k PMIC
07/08/09  dk  Added EHOSTDL support for MSM7x27
02/08/07  ke  Initial version
05/02/07  me  Add functions to update the product id and the serial number

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_apis.h"
#include "comdef.h"
#include "hsu_common_int.h"
#include "hsu_phy_utils_int.h"
#include "hsu_conf_sel_stack_utils.h"
#include "hsu_core_config_int.h"
//#include "rex.h"

#include "hsu_modem_proc_services.h"
#include "hsu_conf_sel_types_int.h"

#include "HAL_hsusb.h"
#include "hw.h"
#include "hsu_vregmgr.h"

#include "DDIClock.h"
#include "hsu_log.h"
#include "hsu_platform.h"
#include "hsu_lpm_snps.h"

#include "hsu_hwio_dependency.h"

// For VMIDT restoration // MOCK_UTMI is disable since host mode is not used
//#include "tz_syscalls.h"
//#include "syscalls.h"

#include "hsu_os_abstract_bus.h"
#include "hsu_al_bam_utils.h"

/*==============================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/

/* Define the PM_MPP to be used in order to power up / down the VBUS
 * HS_USB_MPP_FOR_VBUS is build time configurable.
 */
  #ifdef HS_USB_MPP_FOR_VBUS
    #define HSU_MPP_FOR_VBUS      (HS_USB_MPP_FOR_VBUS)
  #elif defined(T_FFA)
    #define HSU_MPP_FOR_VBUS      (PM_MPP_20)
  #else
    #define HSU_MPP_FOR_VBUS      (PM_MPP_16)
  #endif /* HS_USB_MPP_FOR_VBUS */

/* Voltage setting for the USB VREG = 3.3 Volts. */
#define PM_VREG_USB_LEVEL         (3300)

/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
Variables
==============================================================================*/

#ifndef FEATURE_HS_USB_NO_CI_CORE
static boolean vbus_is_on = FALSE;
#endif // FEATURE_HS_USB_NO_CI_CORE

#ifdef FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND
static boolean usb_reset_rework_irq_handle_enabled = FALSE;
static boolean tested_usb_reset_rework_is_installed = FALSE;
static boolean usb_reset_rework_is_installed = FALSE;
#endif /* FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND */

// Used to check QUSB2 PHY PLL STATUS
#define HSU_HS_PHY_PLL_MAX_CNT           (200)
#define HSU_HS_PHY_PLL_BMSK              (0x1)

#define HSU_HWIO_ADDR_EMPTY      (0xFFFFFFFF)
#define HSU_HWIO_ADDR_END        (0x0)


// HS PHY initializations
#define HSU_DCI_HS_PHY_CFG_ARRAY_ENTRY_CNT      (32)

//PHY tuning parameters to be used for CML Differential Reference Clock for the PHY
static const uint32 hsu_dci_hs_phy_cfg_address[HSU_DCI_HS_PHY_CFG_ARRAY_ENTRY_CNT] = 
{
  //-------------------------------------------------------------------------------------------
  HSU_HWIO_ADDR_EMPTY,                                                         //  00 : (0x00),
  HSU_HWIO_ADDR_EMPTY,                                                         //  01 : (0x00),
  HSU_HWIO_ADDR_EMPTY,                                                         //  02 : (0x00),
  HSU_HWIO_ADDR_EMPTY,                                                         //  03 : (0x00),
  //-------------------------------------------------------------------------------------------
  HWIO_USB2_PHY_USB_PHY_CFG0_ADDR,                                             //  04 : (0x05),
  HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_ADDR,                                       //  05 : (0x12),
  HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_ADDR,                                      //  06 : (0x0D),
  HSU_HWIO_ADDR_EMPTY,                                                         //  07 : (0x00),
  //-------------------------------------------------------------------------------------------
  HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_ADDR,                              //  08 : (0x88),
  HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_ADDR,                              //  09 : (0xDB),
  HSU_HWIO_ADDR_EMPTY,                                                         //  10 : (0x00),
  HSU_HWIO_ADDR_EMPTY,                                                         //  11 : (0x00),
  //-------------------------------------------------------------------------------------------
  HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_ADDR,                                     //  12 : (0x23),
  HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_ADDR,                                     //  13 : (0x60),
  HSU_HWIO_ADDR_EMPTY,                                                        //  14 : (0x00),
  HSU_HWIO_ADDR_EMPTY,                                                         //  15 : (0x00),
  //-------------------------------------------------------------------------------------------
  HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_ADDR,                            //  16 : (0x63),
  HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_ADDR,                            //  17 : (0x03),
  HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_ADDR,                            //  18 : (0x1D),
  HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_ADDR,                            //  19 : (0x03),
  //-------------------------------------------------------------------------------------------
  HSU_HWIO_ADDR_EMPTY,                                                         //  20 : (0x00),
  HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_ADDR,                              //  21 : (0x61),
  HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_ADDR,                                       //  22 : (0x12),
  HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_ADDR,                                     //  23 : (0x00),
  //-------------------------------------------------------------------------------------------
  HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_ADDR,                              //  24 : (0x89),
  HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_ADDR,                                     //  25 : (0x00),
  HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_ADDR,                                     //  26 : (0xE0),
  HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_ADDR,                                       //  27 : (0x02),
  //-------------------------------------------------------------------------------------------
  HSU_HWIO_ADDR_EMPTY,                                                         //  28 : (0x00),
  HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_ADDR,                                       //  29 : (0x10),
  HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_ADDR,                                     //  30 : (0x60),
  HSU_HWIO_ADDR_END,                                                           //  31 : (0x00),
};

static const uint8 hsu_dci_hs_phy_cfg_value[HSU_DCI_HS_PHY_CFG_ARRAY_ENTRY_CNT] = 
{
  //-------------------------------------------------------------------------------------------
  /* HSU_HWIO_ADDR_EMPTY,                                                          00 : */ (0x00),
  /* HSU_HWIO_ADDR_EMPTY,                                                          01 : */ (0x00),
  /* HSU_HWIO_ADDR_EMPTY,                                                          02 : */ (0x00),
  /* HSU_HWIO_ADDR_EMPTY,                                                          03 : */ (0x00),
  //-------------------------------------------------------------------------------------------
  /* HWIO_USB2_PHY_USB_PHY_CFG0_ADDR,                                              04 : */ (0x05),
  /* HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_ADDR,                                        05 : */ (0x12),
  /* HWIO_USB2_PHY_USB_PHY_REFCLK_CTRL_ADDR,                                       06 : */ (0x0D),
  /* HSU_HWIO_ADDR_EMPTY,                                                          07 : */ (0x00),
  //-------------------------------------------------------------------------------------------
  /* HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_ADDR,                               08 : */ (0x88),
  /* HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON1_ADDR,                               09 : */ (0xDB),
  /* HSU_HWIO_ADDR_EMPTY,                                                          10 : */ (0x00),
  /* HSU_HWIO_ADDR_EMPTY,                                                          11 : */ (0x00),
  //-------------------------------------------------------------------------------------------
  /* HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL1_ADDR,                                      12 : */ (0x23),
  /* HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_ADDR,                                      13 : */ (0x60),
  /* HSU_HWIO_ADDR_EMPTY,                                                          14 : */ (0x00),
  /* HSU_HWIO_ADDR_EMPTY,                                                          15 : */ (0x00),
  //-------------------------------------------------------------------------------------------
  /* HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X0_ADDR,                             16 : */ (0x63),
  /* HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X1_ADDR,                             17 : */ (0x03),
  /* HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X2_ADDR,                             18 : */ (0x1D),
  /* HWIO_USB2_PHY_USB_PHY_PARAMETER_OVERRIDE_X3_ADDR,                             19 : */ (0x03),
  //-------------------------------------------------------------------------------------------
  /* HSU_HWIO_ADDR_EMPTY,                                                          20 : */ (0x00),
  /* HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON2_ADDR,                               21 : */ (0x61),
  /* HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_ADDR,                                        22 : */ (0x12),
  /* HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST1_ADDR,                                      23 : */ (0x00),
  //-------------------------------------------------------------------------------------------
  /* HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_ADDR,                               24 : */ (0x89),
  /* HWIO_USB2_PHY_USB_PHY_HS_PHY_TEST0_ADDR,                                      25 : */ (0x00),
  /* HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_ADDR,                                      26 : */ (0xE0),
  /* HWIO_USB2_PHY_USB_PHY_UTMI_CTRL0_ADDR,                                        27 : */ (0x02),
  //-------------------------------------------------------------------------------------------
  /* HSU_HWIO_ADDR_EMPTY,                                                          28 : */ (0x00),
  /* HWIO_USB2_PHY_USB_PHY_UTMI_CTRL5_ADDR,                                        29 : */ (0x10),
  /* HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL2_ADDR,                                      30 : */ (0x60),
  /* HSU_HWIO_ADDR_END,                                                            31 : */ (0x00),
};

/*==============================================================================

FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

//OBSOLETE /*==============================================================================
//OBSOLETE 
//OBSOLETE FUNCTION          HSU_DELAY_MS
//OBSOLETE 
//OBSOLETE DESCRIPTION
//OBSOLETE Helper function. Delays execution for the given time in milliseconds.
//OBSOLETE 
//OBSOLETE DEPENDENCIES
//OBSOLETE None.
//OBSOLETE 
//OBSOLETE RETURN VALUE
//OBSOLETE None.
//OBSOLETE 
//OBSOLETE SIDE EFFECTS
//OBSOLETE None. 
//OBSOLETE 
//OBSOLETE ==============================================================================*/
//OBSOLETE static void hsu_delay_ms(uint32 milliseconds)
//OBSOLETE {
//OBSOLETE   rex_sleep(milliseconds);
//OBSOLETE }


// ===========================================================================
/**
 * @function    hsu_dci_phy_reg_array_process
 * 
 * @brief   This function reads from array which define list of hwio writes for
 *          USB PHY
 * 
 * @param   address_array   - array holding address of HW register
 *          value_array     - array holding values to be written to HW register
 *          start_index     - starting index for array processing
 *          array_entry_cnt - number of entries in the array
 * 
 * @return  None
 * 
 */
// ===========================================================================
static void hsu_dci_phy_reg_array_process
(
  const uint32 *address_array, 
  const uint8  *value_array, 
  uint32        start_index, 
  uint32        array_entry_cnt
)
{

  uint32 index = start_index;

  if ( (NULL == address_array)
      || (NULL == value_array)
      || (0 == array_entry_cnt) )
  {
    HSU_ULOG_1(NO_MSG, BUS_LOG, "fail, address_array 0x%08X", (uint32) address_array);
  }
  else
  {
    for (; index < array_entry_cnt; index++)
    {
      if (HSU_HWIO_ADDR_END == address_array[index])
      {
        break;
      }

      if (HSU_HWIO_ADDR_EMPTY == address_array[index])
      {
        continue;
      }

      out_byte(address_array[index], value_array[index]);
    }
  }
  HSU_ULOG_2(NO_MSG, BUS_LOG, "start %u, end %u", start_index, index);

}


/*==============================================================================

FUNCTION          hsu_init_usb2_phy

DESCRIPTION
TODO

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_init_usb2_phy(void)
{

  // Moved the entire init sequence in the Array
  hsu_dci_phy_reg_array_process(hsu_dci_hs_phy_cfg_address, hsu_dci_hs_phy_cfg_value, 0, HSU_DCI_HS_PHY_CFG_ARRAY_ENTRY_CNT);

  hsu_os_delay_us(20);

}

/*==============================================================================

FUNCTION          hsu_init_snps_usb

DESCRIPTION
TODO

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_init_snps_usb(void)
{  
  uint32 irq_status;

  hsu_platform_info_type const *platform_info = hsu_platform_get_info();

  HSU_ULOG_FUNC(SNPS_LOG);
  HSU_ULOG_FUNC(BUS_LOG);

  switch (platform_info->hsu_platform_id)
  {
    case HSU_PLATFORM_VIRTIO:
    case HSU_PLATFORM_RUMI:
    {      
      HWIO_GCTL_OUTM(HWIO_GCTL_PRTCAPDIR_BMSK, (0x2 << HWIO_GCTL_PRTCAPDIR_SHFT));

      /*Reset the controller*/
      HWIO_DCTL_OUTM(HWIO_DCTL_CSFTRST_BMSK, (0x1 << HWIO_DCTL_CSFTRST_SHFT));
      while (0x0 != HWIO_DCTL_INM(HWIO_DCTL_CSFTRST_BMSK))
      {
        hsu_os_delay_us(1000);
      }
    }
    break;

    default:
    {
      uint32 index = 0;

      // Turn ON clock, so that initial state is ON, so release clock takes effect
      hsu_clkmgr_config_clocks(HSU_USB3_SNPS_CORE);

      // Explicitly put the controller into power collapse as it may not already be
      hsu_clkmgr_enable_power_domain(FALSE, HSU_USB3_SNPS_CORE);

      hsu_os_delay_us(100);

      // Disable USB 3.0 SW Power Collapse
      hsu_clkmgr_enable_power_domain(TRUE, HSU_USB3_SNPS_CORE);


// #if 0 // GDS reset in power domain take care of all the individual reset?
      // LPM Init turns clocks ON, so we release then before reset
      hsu_clkmgr_release_clocks(HSU_USB3_SNPS_CORE);

// No ref cllk register for Twizy
#if 0
      // USB2 PHY ref clock must be disabled before GCC reset
      HWIO_GCC_QUSB_REF_CLK_EN_OUTM(HWIO_GCC_QUSB_REF_CLK_EN_CLK_ENABLE_BMSK,
        0 << HWIO_GCC_QUSB_REF_CLK_EN_CLK_ENABLE_SHFT);
      HWIO_GCC_QUSB_REF_CLK_EN_OUTM(HWIO_GCC_QUSB_REF_CLK_EN_HW_CONTROL_BMSK,
        0 << HWIO_GCC_QUSB_REF_CLK_EN_HW_CONTROL_SHFT);
#endif
      // Reset the contoller through GCC
      hsu_clkmgr_assert_reset(TRUE, HSU_CLKMGR__RESET_GCC_USB2_M);

      // Add delay to settle
      hsu_os_delay_us(100);
      hsu_clkmgr_assert_reset(FALSE, HSU_CLKMGR__RESET_GCC_USB2_M);

      hsu_clkmgr_assert_reset(TRUE, HSU_CLKMGR__RESET_GCC_USB2_PHY_RESET_M);
      //hsu_clkmgr_assert_reset(TRUE, HSU_CLKMGR__RESET_GCC_USB3_PHY_BCR);
      //hsu_clkmgr_assert_reset(TRUE, HSU_CLKMGR__RESET_GCC_USB3PHY_PHY_BCR);
    
      // Add delay to settle
      hsu_os_delay_us(100);
    
      // TODO: 
      // USB30 VDD Power Domain is already ON at hsu_lpm_init().
      // Investigate if VDD Power Domain OFF is required between GCC RESET
      // hsu_clkmgr_enable_power_dowmain(TRUE, HSU_USB3_SNPS_CORE);

      //hsu_clkmgr_assert_reset(FALSE, HSU_CLKMGR__RESET_GCC_USB3PHY_PHY_BCR);
      //hsu_clkmgr_assert_reset(FALSE, HSU_CLKMGR__RESET_GCC_USB3_PHY_BCR);
      hsu_clkmgr_assert_reset(FALSE, HSU_CLKMGR__RESET_GCC_USB2_PHY_RESET_M);
// #endif

      // Add delay to settle
      hsu_os_delay_us(100);

      // Turn on clocks
      hsu_clkmgr_config_npa_clocks(HSU_USB3_SNPS_CORE);
      hsu_clkmgr_config_clocks(HSU_USB3_SNPS_CORE);

      // Don't need to restore USB VMID since it is TBU
      //HSU_ULOG(NO_MSG, BUS_LOG, "snps_tz_restore, start");
      //if (0 != scm_sec_restore_syscall(TZ_DEVICE_USB3_0))
      //{
      //  HSU_ERR_FATAL("TZ security restore failed", 0, 0, 0);
      //}
      //HSU_ULOG(NO_MSG, BUS_LOG, "snps_tz_restore, end");

      hsu_clkmgr_select_utmi_clk();
      
      // 7.
      hsu_init_usb2_phy();
    
      // Added check to skip SuperSpeed related configurations
      if (FALSE == hsu_clkmgr_is_hs_only_mode())
      {    
        // GUSB3PIPECTL
        //    a.  Set delayp1trans to 0x0;
        //    b.  Set disrxdetu3rxdet to 0x0;  relevant for host, this bit has to be 0 for controller to handle LFPS detection 
        HWIO_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_OUTMI(0, HWIO_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_DELAYP1TRANS_BMSK,
                                                  ((uint32) 0x0 << HWIO_GUSB3PIPECTL_REGS_p_GUSB3PIPECTL_DELAYP1TRANS_SHFT));
      }
      hsu_os_delay_us(100);
      
      HSU_ASSERT(index < 100);

      // Disable HS SUSPEND L2 transition
      hsu_lpm_snps_set_hs_phy_suspend_L2_transition(FALSE);

      // Clear all interrupts and masks for power events
      irq_status = HWIO_PWR_EVNT_IRQ_STAT_IN;
      HWIO_PWR_EVNT_IRQ_STAT_OUT(irq_status);
      HWIO_PWR_EVNT_IRQ_MASK_OUT(0);

      // Enable Power events for High speed and Super Speed mode
      HWIO_PWR_EVNT_IRQ_MASK_OUTM(HWIO_PWR_EVNT_IRQ_MASK_LPM_IN_L2_IRQ_MASK_BMSK, (1 << HWIO_PWR_EVNT_IRQ_MASK_LPM_IN_L2_IRQ_MASK_SHFT));
      HWIO_PWR_EVNT_IRQ_MASK_OUTM(HWIO_PWR_EVNT_IRQ_MASK_LPM_OUT_L2_IRQ_MASK_BMSK, (1 << HWIO_PWR_EVNT_IRQ_MASK_LPM_OUT_L2_IRQ_MASK_SHFT));

      // +++ The following are set inside dcd_enable callback's invocation of snps_dcd_config_core()
      //  HWIO_GCTL_OUTM(HWIO_GCTL_PRTCAPDIR_BMSK, (0x2 << HWIO_GCTL_PRTCAPDIR_SHFT));
      // ---

      // Pipe transfer limit related DCTL are moved into snps_dcd_ops_vbus_connect()
    
      // Remaining steps are done subsequently when we attach to the snps controller.
    }
    break;
  }
  return;
}

/*===========================================================================

FUNCTION      HSU_API_UPDATE_PRODUCT_ID

DESCRIPTION
Updates the ARM9 product id variable.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_update_product_id(unsigned short product_id)
{
  hsu_modem_proc_services_set_product_id(product_id);
}

/*===========================================================================

FUNCTION      HSU_API_UPDATE_SERIAL_NUMBER

DESCRIPTION
Updates the ARM9 serial number variable.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_update_serial_number(const char* serial_number)
{
  hsu_modem_proc_services_set_serial_number(serial_number);
}

/*===========================================================================

FUNCTION      HSU_API_UPDATE_IS_SERIAL_NUMBER_NULL

DESCRIPTION
Updates the ARM9 whether the serial number is null.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_update_is_serial_number_null(boolean is_serial_number_null)
{
  hsu_modem_proc_services_set_is_serial_number_null(is_serial_number_null);
}

/*===========================================================================

FUNCTION      HSU_API_UPDATE_VENDOR_ID

DESCRIPTION
Updates the vendor id variable.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_update_vendor_id(unsigned short vendor_id)
{
#ifdef FEATURE_GOBI
  hsu_modem_proc_services_set_vendor_id(vendor_id);
#endif /* FEATURE_GOBI */
}

/*===========================================================================

FUNCTION      hsu_api_ss_functional

DESCRIPTION
detect if super speed phy is functional

DEPENDENCIES
None.

RETURN VALUE
boolean T

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_api_ss_phy_functional(void)
{
  if (hsu_conf_sel_nv_efs_get(HSU_EFS_SS_PHY_DISABLED))
  {
    //If SS PHY is disabled NV is checked at the end to set the automatic algorithm get chance to run first
    //If this is the case, then the PHY should be placed back in powerdown mode by writing 0x00 to PCIE_USB3_PHY_POWER_DOWN_CONTROL.

    //HWIO_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_POWER_DOWN_CONTROL_OUTM(HWIO_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_POWER_DOWN_CONTROL_SW_PWRDN_B_BMSK, (0x0 << HWIO_USB3_UNI_PHY_PCIE_USB3_UNI_PCS_POWER_DOWN_CONTROL_SW_PWRDN_B_SHFT));

    hsu_event_log(EVT_SS_PHY_NOT_FUNCTIONAL, USB3_CORE_INDEX);
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "nv_ss_phy_disabled");
    return FALSE;
  }
  return TRUE;
}

/*==============================================================================
 
 FUNCTION          HSU_API_INIT_PHY
 
 DESCRIPTION
 TODO
 
 DEPENDENCIES
 None.
 
 RETURN VALUE
 None.
 
 SIDE EFFECTS
 None.
 
 ==============================================================================*/
void hsu_api_init_phy(void)
{
    DALResult dal_result;
    DalDeviceHandle *phClock;
    ClockIdType gcc_usb_hs_system_clk;
    
    HAL_hsusb_Init();
    
    dal_result = DAL_DeviceAttach(DALDEVICEID_CLOCK, &phClock);
    
    if ((dal_result != DAL_SUCCESS) || (NULL == phClock))
    {
      HSU_ERR_FATAL("DAL_DeviceAttach failed: %u", dal_result, 0, 0);
	  return;
    }
    
    // Select the ULPI access on ChipIdea Controller
    //HWIO_TCSR_PHSS_USB2_PHY_SEL_OUT(0x0);
    
    // Turn ON clock, so that initial state is ON, so release clock takes effect
    hsu_clkmgr_config_clocks(HSU_USB3_SNPS_CORE);
    
    // Turn OFF clocks before HSUSB Controller reset
    hsu_clkmgr_release_clocks(HSU_USB3_SNPS_CORE);
    
    DalClock_GetClockId(phClock, "gcc_usb_hs_system_clk", &gcc_usb_hs_system_clk);
    
    // Assert reset on clock to reset the HS-USB core
    //DalClock_ResetClock(phClock, gcc_usb_hs_system_clk, CLOCK_RESET_ASSERT);

    hsu_clkmgr_assert_reset(TRUE, HSU_CLKMGR__RESET_GCC_USB2_M);
    hsu_clkmgr_assert_reset(TRUE, HSU_CLKMGR__RESET_GCC_USB2_PHY_RESET_M);

    // Add delay to settle
    hsu_os_delay_us(10);
    
    hsu_clkmgr_assert_reset(FALSE, HSU_CLKMGR__RESET_GCC_USB2_PHY_RESET_M);
    hsu_os_delay_us(75);
    hsu_clkmgr_assert_reset(FALSE, HSU_CLKMGR__RESET_GCC_USB2_M);

    hsu_os_delay_us(10);
    
    // De-assert reset on clock to reset the HS-USB core
    //DalClock_ResetClock(phClock, gcc_usb_hs_system_clk, CLOCK_RESET_DEASSERT);
    
    hsu_clkmgr_config_clocks(HSU_USB3_SNPS_CORE);
    hsu_clkmgr_config_npa_clocks(HSU_USB3_SNPS_CORE);
    
    // Set core to ULPI mode - after reseting the USBH we may need to set the
    // mode again
    HAL_hsusb_Write32(0, HAL_HSUSB_PORTSC_ADDR(0), HAL_HSUSB_PORTSC_PTS_ULPI_BMSK);
    
    //vbus_is_on = TRUE; /* fake this, so that we indeed order VBUS power down */
    
    // To avoid warning of vbus_is_on not being used
    //HSU_USE_PARAM(vbus_is_on);
    
    HSU_ULOG(NO_MSG, BUS_LOG, "hsu_api_init_phy, end");
    
} /* hsu_api_init_phy */


/*===========================================================================
FUNCTION hsu_api_reg_dump
DESCRIPTION
  This function saves the following items in a predefined static memory:

    -USB device controller registers
    -BAM debug registers

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_api_reg_dump(void)
{

}


boolean hsu_api_disable_pmic_otg_comparators(void)
{
  /* SCMM, MDM7X30 and MDM9X00 targets do not have PMIC OTG XCVR, and therefore
  ** can't support PMIC comparators based LPM.
  */

  return FALSE;
}  /* hsu_api_disable_pmic_otg_comparators */


