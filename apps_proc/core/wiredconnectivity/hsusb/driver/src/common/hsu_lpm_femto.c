/*==============================================================================

  High Speed USB Low Power Management.
  
  GENERAL DESCRIPTION
    This module defines APIs for HS USB low power management.

  EXTERNALIZED FUNCTIONS
    hsu_lpm_init
    hsu_lpm_enter
    hsu_lpm_exit
    hsu_lpm_get_ctx
    
  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.

  Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_lpm_femto.c#1 $
  $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  11/11/11  jch      Initial version

==============================================================================*/

/*==============================================================================
                            INCLUDE FILES FOR MODULE
==============================================================================*/
#include <comdef.h>
#include <customer.h>

//#include <rex.h>  /* For rex_sleep */
#include "hsu_qurt_util.h"

#include "hsu_clkmgr.h"
#include "hsu_common_int.h"
#include "HAL_hsusb.h"
#include "hsu_os_abstract_bus.h"
#include "hsu_os_abstract_bus_ext_int.h"

#ifdef FEATURE_HS_USB_POLLING
#include "hsu_polling.h"
#endif /* FEATURE_HS_USB_POLLING */
#include "hsu_lpm.h"
#include "hsu_conf_sel_i.h"
#include "hsu_conf_sel_nv.h"
#include "hsu_vregmgr.h"
#include "hsu_phy_utils_int.h"
#include "hsu_core_config.h"
#include "hsu_core_config_int.h"

/* GPIO APIs */
#include "DDITlmm.h"
#include "DDIGPIOInt.h"
#include "DalDevice.h"
#include "DALDeviceId.h"

#include "hsu_log.h"
#include "npa.h"
#include "hsu_platform.h"
#include "hsu_hwio_dependency.h"
/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
  Macro and Constant Definition
==============================================================================*/

/* LPM on SOF SW workaround constants */
#define PHCD_WRITE_DELAY           (20)
#define PHCD_READ_DELAY            (20)
#define PHCD_WRITE_MAX_TRY         (5)
#define PHCD_READ_MAX_TRY          (5)

/* Vendor Specific Link Register Definition */
#define HSU_LINK_USB_OTG_HS_PHY_CTR__ADDR                               (0x240)
#define HSU_LINK_USB_OTG_HS_PHY_CTR__USB2_PHY_IDHV_CLAMP_EN_BMSK        (0x1000)
#define HSU_LINK_USB_OTG_HS_PHY_CTR__USB2_PHY_OTGSESSVLDHV_CLAMP_EN_BMSK (0x0800)
#define HSU_LINK_USB_OTG_HS_PHY_CTR__PHY_MPM_HV_CLAMP_EN_BMSK           (0x0400)
#define HSU_LINK_USB_OTG_HS_PHY_CTR__USB2_PHY_OTGSESSVLDHV_INTEN_BMSK   (0x0200)
#define HSU_LINK_USB_OTG_HS_PHY_CTR__USB2_PHY_IDHV_INTEN_BMSK           (0x0100)
#define HSU_LINK_USB_OTG_HS_PHY_CTR__USB2_PHY_ULPI_POR_BMSK             (0x0080)
#define HSU_LINK_USB_OTG_HS_PHY_CTR__USB2_PHY_FSEL_BMSK                 (0x0070)
#define HSU_LINK_USB_OTG_HS_PHY_CTR__HOST_PORTCTRL_FORCE_SUSEN_BMSK     (0x0008)
#define HSU_LINK_USB_OTG_HS_PHY_CTR__USB2_PHY_SIDDQ_BMSK                (0x0004)
#define HSU_LINK_USB_OTG_HS_PHY_CTR__USB2_PHY_RETEN_BMSK                (0x0002)
#define HSU_LINK_USB_OTG_HS_PHY_CTR__USB2_PHY_POR_BMSK                  (0x0001)

/* USB PHY Register */
#define HSU_PHY_INTERRUPT_STATUS__ADDR                                  (0x13)    
#define HSU_PHY_INTERRUPT_STATUS__SESSVALID_BMSK                        (0x04)    

#define HSU_PHY_OTG_CTRL__ADDR                                          (0x0A)
#define HSU_PHY_OTG_CTRL__SET_ADDR                                      (0x0B)
#define HSU_PHY_OTG_CTRL__CLR_ADDR                                      (0x0C)
#define HSU_PHY_OTG_CTRL__IDPULLUP_BMSK                                 (0x01)

/* Vendor Specific PHY Register Definition */
#define HSU_PHY_POWER_AND_CLOCK_MANAGEMENT__ADDR                        (0x88)
#define HSU_PHY_POWER_AND_CLOCK_MANAGEMENT__SET_ADDR                    (0x89)
#define HSU_PHY_POWER_AND_CLOCK_MANAGEMENT__CLR_ADDR                    (0x8A)
#define HSU_PHY_POWER_AND_CLOCK_MANAGEMENT__COMMONONN_BMSK              (0x02)
#define HSU_PHY_POWER_AND_CLOCK_MANAGEMENT__OTGDISABLE_BMSK             (0x01)

#define HSU_PHY_CHARGING_DETECTION_CONTROL__ADDR                        (0x84)
#define HSU_PHY_CHARGING_DETECTION_CONTROL__SET_ADDR                    (0x85)
#define HSU_PHY_CHARGING_DETECTION_CONTROL__CLR_ADDR                    (0x86)
#define HSU_PHY_CHARGING_DETECTION_CONTROL__ACAENB_BMSK                 (0x40)
#define HSU_PHY_CHARGING_DETECTION_CONTROL__DCDENB_BMSK                 (0x20)
#define HSU_PHY_CHARGING_DETECTION_CONTROL__CHRGSEL_BMSK                (0x10)
#define HSU_PHY_CHARGING_DETECTION_CONTROL__VDATSRCAUTO_BMSK            (0x0C)
#define HSU_PHY_CHARGING_DETECTION_CONTROL__VDATSRCENB_BMSK             (0x02)
#define HSU_PHY_CHARGING_DETECTION_CONTROL__VDATDETENB_BMSK             (0x01)

/*==============================================================================
Externalized Variables
==============================================================================*/
/*==============================================================================
Local Variables
==============================================================================*/

/* HSU LPM context */
hsu_lpm_ctx_type hsu_lpm_ctx = {
  0,/* core_idx */
  0,/* is_called_by_lpm_timer */
  0,/* is_analog_audio_mode */
  0,/* is_lpm_feature_active */
  0,/* is_ext_ldo_installed */
  0,/* is_pmic_comparator_used */
  0,/* is_otg_enabled */
  0,/* is_pmic_chg_pin_used */
  NULL,/* crit_sec */
  NULL,/* lpm_info */
  FALSE,/* ldo_power_collapse_enabled */
  NULL,/* xo_handle */
  FALSE,/* xo_voted */
};

/*==============================================================================
Local Function Declarations
==============================================================================*/
static void hsu_lpm_enter_suspend(hsu_lpm_ctx_type *ctx);
static void hsu_lpm_enter_disconnect(hsu_lpm_ctx_type *ctx);

static boolean hsu_lpm_set_phcd_bit(hsu_lpm_ctx_type *ctx);
static boolean hsu_lpm_clear_phcd_bit(hsu_lpm_ctx_type *ctx);

static boolean hsu_lpm_enter_is_valid(hsu_lpm_ctx_type *ctx);
static boolean hsu_lpm_exit_is_valid(hsu_lpm_ctx_type *ctx);

/*==============================================================================
Public Function Definitions
==============================================================================*/

/*===========================================================================
FUNCTION hsu_lpm_init
===========================================================================*/
void hsu_lpm_init(void)
{
  hsu_lpm_ctx_type *ctx = hsu_lpm_get_ctx();

  /* Set HS-USB to turn off LDO on detach*/
  ctx->ldo_power_collapse_enabled = 
       hsu_conf_sel_get_ldo_power_collapse()
    || (FALSE == hsu_conf_sel_hs_main_core_enabled());

#if !defined(FEATURE_HS_USB_PRE_SILICON_BU)
  /* 
  ** Separate XO vote for HS-USB is required since turning ON HS-USB clocks 
  ** will not prevent XO shutdown.
  */
  ctx->xo_handle = 
    npa_create_sync_client(
      "/xo/cxo", 
      "hsusb_xo",
      NPA_CLIENT_REQUIRED);

  HSU_ASSERT(ctx->xo_handle != NULL);

  /* Vote against xo shutdown */
  npa_issue_required_request(ctx->xo_handle, 2);
#endif /* FEATURE_HS_USB_PRE_SILICON_BU */    
  ctx->xo_voted = TRUE;    
}


/*===========================================================================
FUNCTION hsu_lpm_enter
===========================================================================*/
void hsu_lpm_enter(hsu_lpm_ctx_type *ctx)
{
  boolean is_suspend_bit_set = FALSE; 

  /* check SessValid bit */
  hsu_lpm_info_type *lpm_info = ctx->lpm_info;

  HSU_ULOG_ENUM(NO_MSG, BUS_LOG, "hsu_lpm_enter", HSU_CORE_OFFSET2, ctx->core_idx);

  rex_enter_crit_sect(ctx->crit_sec);

  lpm_info->was_lpm_exit_due_to_remote_wakeup = FALSE;

  if(!hsu_lpm_enter_is_valid(ctx))
  {
    rex_leave_crit_sect(ctx->crit_sec);
    return;
  }

  /* Un-register the HS-USB interrupt handler. */
  hsu_os_bus_interrupt_uninit(ctx->core_idx);

  lpm_info->lpm_entry_is_pending = FALSE;
  
  if ( (FALSE == hsu_conf_sel_hs_main_core_enabled())
    || hsu_conf_sel_is_usb_core_snps() )
  {
    // If USB is disabled or SNPS USB controller is used, then CI USB needs
    // to be in low power mode as if USB is not connected
    lpm_info->cable_is_connected_during_lpm = FALSE;
  }
  else
  { 
    /* SW driver checks that there is no active session (cable is disconnected 
    ** PHY_SESS_VLD bit of USB_OTG_HS_USBSTS register, Run/Stop bit of 
    ** USB_OTG_HS_USBCMD register is 0) and decides to enter Low Power Mode. 
    */

    lpm_info->cable_is_connected_during_lpm = 
        hsu_vregmgr_is_usb_cable_attached();

    /* SW driver receives Suspend interrupt (SLI bit in USB_OTG_HS_USBSTS 
    ** register) and decides to enter Low Power Mode. As result, PORTSC SUS bit
    ** should be set.
    */
    is_suspend_bit_set = 
      HAL_hsusb_GetBits32(
        hsu_os_bus_hal_core_id(ctx->core_idx),
        HAL_HSUSB_PORTSC_ADDR(0), 
        HAL_HSUSB_PORTSC_SUSP_BMSK);
  }
  
  /* Check if disconnected or suspended */
  if (lpm_info->cable_is_connected_during_lpm == FALSE)
  {
    hsu_lpm_enter_disconnect(ctx);
  }
  else if (is_suspend_bit_set)
  {
    hsu_lpm_enter_suspend(ctx);
  }
  else
  {
    HSU_MSG_ERROR_2("hsu_lpm_enter: not disconnected (%d) or suspended (%d)", 
      lpm_info->cable_is_connected_during_lpm, is_suspend_bit_set);
  }
  
  rex_leave_crit_sect(ctx->crit_sec);

  if ( (lpm_info->cable_is_connected_during_lpm)
    || (FALSE == hsu_lpm_is_ldo_power_collapse_enabled()) )
  {
    /* Register the HS-USB interrupt back. */
    hsu_os_bus_interrupt_init(ctx->core_idx);
  }

}


/*===========================================================================
FUNCTION hsu_lpm_exit
===========================================================================*/
void hsu_lpm_exit(hsu_lpm_ctx_type *ctx)
{
  static boolean hsu_lpm_enter_por = FALSE;
  boolean is_suspend_bit_set;

  hsu_lpm_info_type *lpm_info = ctx->lpm_info;

  HSU_ULOG_ENUM_1(NO_MSG, BUS_LOG, "remote_wakeup %u", HSU_CORE_OFFSET2, ctx->core_idx, lpm_info->was_lpm_exit_due_to_remote_wakeup);

  rex_enter_crit_sect(ctx->crit_sec);

  if (!hsu_lpm_exit_is_valid(ctx))
  {
  
    is_suspend_bit_set = 
    HAL_hsusb_GetBits32(
      hsu_os_bus_hal_core_id(ctx->core_idx),
      HAL_HSUSB_PORTSC_ADDR(0), 
      HAL_HSUSB_PORTSC_SUSP_BMSK);

    /* Setting Force Port Resume to '1', which was once done on wake_the_host()
    but moved due to a remote wakeup SW workaround (CR# 236440) */
    if (lpm_info->was_lpm_exit_due_to_remote_wakeup && is_suspend_bit_set)
    {
      HSU_ULOG_2(NO_MSG, BUS_LOG, "set_force_port_resume0, remote_wakeup %u, suspend %u",
                              lpm_info->was_lpm_exit_due_to_remote_wakeup, is_suspend_bit_set);

      HAL_hsusb_SetBits32(
        hsu_os_bus_hal_core_id(ctx->core_idx),
        HAL_HSUSB_PORTSC_ADDR(0), 
        HAL_HSUSB_PORTSC_FPR_BMSK);
    }
    rex_leave_crit_sect(ctx->crit_sec);
    return;
  }
  
  if (ctx->xo_voted == FALSE)
  {
#if !defined(FEATURE_HS_USB_PRE_SILICON_BU)
    /* Vote against XO shutdown */
    npa_issue_required_request(ctx->xo_handle, 2);
#endif
    ctx->xo_voted = TRUE;
  }

  hsu_clkmgr_config_clocks(HSU_USB2_CI_CORE);

  /* Is this exiting Low Power Mode from suspend or Disconnect */
  if (lpm_info->cable_is_connected_during_lpm)
  {
    /***************************************/
    /* Exiting Low Power Mode from Suspend */
    /***************************************/

    /* SW driver writes 0 to ULPI_STP_CTRL and ASYNC_INTR_CTRL bits of 
    ** USB_OTG_HS_USBCMD register.
    */
    HAL_hsusb_ClearBits32(
      hsu_os_bus_hal_core_id(ctx->core_idx),
      HAL_HSUSB_USBCMD_ADDR,
      HAL_HSUSB_USBCMD_ULPI_STP_CTRL_BMSK
      | HAL_HSUSB_USBCMD_ASYNC_INTR_CTRL_BMSK);
	  
    // Disable Retention mode by writing 0 to SIDDQ bit of HS_PHY_CTRL_COMMON0
    HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_OUTM(
      HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_SIDDQ_BMSK,
      0x0 << HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_SIDDQ_SHFT);

    is_suspend_bit_set = 
    HAL_hsusb_GetBits32(
      hsu_os_bus_hal_core_id(ctx->core_idx),
      HAL_HSUSB_PORTSC_ADDR(0), 
      HAL_HSUSB_PORTSC_SUSP_BMSK);

    hsu_lpm_clear_phcd_bit(ctx);

    if (lpm_info->was_lpm_exit_due_to_remote_wakeup && is_suspend_bit_set)
    {
      HSU_ULOG_2(NO_MSG, BUS_LOG, "set_force_port_resume1, remote_wakeup %u, suspend %u",
                              lpm_info->was_lpm_exit_due_to_remote_wakeup, is_suspend_bit_set);      

      HAL_hsusb_SetBits32(
        hsu_os_bus_hal_core_id(ctx->core_idx),
        HAL_HSUSB_PORTSC_ADDR(0),
        HAL_HSUSB_PORTSC_FPR_BMSK);
    }

     /*HAL_hsusb_ClearBits32(
      hsu_os_bus_hal_core_id(ctx->core_idx),
      HSU_LINK_USB_OTG_HS_PHY_CTR__ADDR,
      HSU_LINK_USB_OTG_HS_PHY_CTR__USB2_PHY_OTGSESSVLDHV_INTEN_BMSK |
      HWIO_USB_OTG_HS_PHY_CTRL_USB2_PHY_DPSE_INTEN_BMSK             |
      HWIO_USB_OTG_HS_PHY_CTRL_USB2_PHY_DMSE_INTEN_BMSK             );*/

    
  }
  else
  {
    /******************************************/
    /* Exiting Low Power Mode from Disconnect */
    /*****************************************/
    hsu_vregmgr_turn_on(HSU_VREG_ID_HSUSB);

    /* New for 9x25 - 2 additional bits
    ** Disable the USB2_PHY_IDHV_INTEN,USB2_PHY_OTGSESSVLDHV_INTEN
    ** USB2_PHY_DPSE_INTEN, USB2_PHY_DMSE_INTEN of USB_OTG_HS_PHY_CTRL
    */
    HAL_hsusb_ClearBits32(
      hsu_os_bus_hal_core_id(ctx->core_idx),
      HSU_LINK_USB_OTG_HS_PHY_CTR__ADDR,
      HSU_LINK_USB_OTG_HS_PHY_CTR__USB2_PHY_OTGSESSVLDHV_INTEN_BMSK |
      HWIO_USB_OTG_HS_PHY_CTRL_USB2_PHY_DPSE_INTEN_BMSK             |
      HWIO_USB_OTG_HS_PHY_CTRL_USB2_PHY_DMSE_INTEN_BMSK             );

    /* New for 9x25 */
    /* SW driver writes 0 to USB2_PHY_CLAMP_MPM_DPSE_DMSE_EN_N
    ** bit of USB_OTG_HS_PHY_CTRL register. 
    */
    HAL_hsusb_ClearBits32(
      hsu_os_bus_hal_core_id(ctx->core_idx),
      HSU_LINK_USB_OTG_HS_PHY_CTR__ADDR,
      HWIO_USB_OTG_HS_PHY_CTRL_USB2_PHY_CLAMP_MPM_DPSE_DMSE_EN_N_BMSK);

    /* SW driver writes 0 to ULPI_STP_CTRL and ASYNC_INTR_CTRL bits of 
    ** USB_OTG_HS_USBCMD register.
    */
    HAL_hsusb_ClearBits32(
      hsu_os_bus_hal_core_id(ctx->core_idx),
      HAL_HSUSB_USBCMD_ADDR,
      HAL_HSUSB_USBCMD_ULPI_STP_CTRL_BMSK
      | HAL_HSUSB_USBCMD_ASYNC_INTR_CTRL_BMSK);

    // Disable Retention mode by writing 0 to SIDDQ bit of HS_PHY_CTRL_COMMON0
    HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_OUTM(
      HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_SIDDQ_BMSK,
      0x0 << HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_SIDDQ_SHFT);

    /* Read PHCD bit in PORTSC in order to see if it was cleared.
    */
    hsu_lpm_clear_phcd_bit(ctx);

    if (hsu_lpm_enter_por)
    {
      /* SW asserts USB_OTG_HS_PHY_CTRL[0] to POR the PHY for at least 10usec.
      */
      HAL_hsusb_SetBits32(
        hsu_os_bus_hal_core_id(ctx->core_idx),
        HSU_LINK_USB_OTG_HS_PHY_CTR__ADDR,
        HSU_LINK_USB_OTG_HS_PHY_CTR__USB2_PHY_POR_BMSK);
      
      hsu_os_delay_us(20);
    }

    if (hsu_lpm_is_ldo_power_collapse_enabled())
    {
      /* Register the HS-USB interrupt back. */
      hsu_os_bus_interrupt_init(ctx->core_idx);
    }
  }
  
  hsu_clkmgr_config_npa_clocks(HSU_USB2_CI_CORE);

  lpm_info->is_in_low_power_mode = FALSE;
  hsu_event_log(EVT_LPM_EXIT, HS_USB_CORE_INDEX);

  HSU_MSG_HIGH("hsu_lpm_exit: successful");

  rex_leave_crit_sect(ctx->crit_sec);
}


/*===========================================================================
FUNCTION hsu_lpm_get_ctx
===========================================================================*/
hsu_lpm_ctx_type * hsu_lpm_get_ctx(void)
{
  return &hsu_lpm_ctx;
}

/*===========================================================================
FUNCTION hsu_lpm_is_ldo_power_collapse_enabled
===========================================================================*/
boolean hsu_lpm_is_ldo_power_collapse_enabled(void)
{
  return hsu_lpm_ctx.ldo_power_collapse_enabled;
}

/*===========================================================================
FUNCTION hsu_lpm_turn_off_all_clocks
===========================================================================*/
void hsu_lpm_turn_off_all_clocks(void)
{
  hsu_lpm_ctx_type *ctx = hsu_lpm_get_ctx();

  HSU_ASSERT(ctx != NULL);

  /* Check if the lpm_info was initialized from hsu_lpm_enter().
  ** If not, then we should not be turning off the clock.
  */
  if (ctx->lpm_info && ctx->crit_sec)
  {
    rex_enter_crit_sect(ctx->crit_sec);
    HSU_ULOG(NO_MSG, BUS_LOG, "hsu_lpm_turn_off_all_clk, CI_CORE");

    if (ctx->lpm_info->is_in_low_power_mode)
    {
      hsu_clkmgr_release_clocks(HSU_USB2_CI_CORE);   
      hsu_clkmgr_release_npa_clocks(HSU_USB2_CI_CORE);   

      /* If cable is not connected, turn off the LDOs as well */
      if (!ctx->lpm_info->cable_is_connected_during_lpm)
      {
        hsu_vregmgr_turn_off(HSU_VREG_ID_HSUSB);
      }
    }
    
    rex_leave_crit_sect(ctx->crit_sec);
  }
}

/*==============================================================================
Local Function Definitions 
==============================================================================*/

/*------------------------------------------------------------------------------
Enter Low Power APIs
------------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION hsu_lpm_enter_suspend

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_lpm_enter_suspend(hsu_lpm_ctx_type *ctx)
{
  hsu_lpm_info_type *lpm_info = ctx->lpm_info;
  boolean is_suspend_bit_set;

  HSU_ULOG_FUNC(BUS_LOG);

  /* Set PHCD bit
  */
  if (!hsu_lpm_set_phcd_bit(ctx))
  {
    return;
  }
  
  /* SW driver writes 1 to ULPI_STP_CTRL and ASYNC_INTR_CTRL bits of 
  ** USB_OTG_HS_USBCMD register.
  */
  HAL_hsusb_SetBits32(
    hsu_os_bus_hal_core_id(ctx->core_idx),
    HAL_HSUSB_USBCMD_ADDR,
    HAL_HSUSB_USBCMD_ULPI_STP_CTRL_BMSK | 
    HAL_HSUSB_USBCMD_ASYNC_INTR_CTRL_BMSK);

  is_suspend_bit_set = 
    HAL_hsusb_GetBits32(
      hsu_os_bus_hal_core_id(ctx->core_idx),
      HAL_HSUSB_PORTSC_ADDR(0), 
      HAL_HSUSB_PORTSC_SUSP_BMSK);

  /* SW driver checks SUSP bit of USB_OTG_HS_PORTSC register */
  if (is_suspend_bit_set)
  {  
    HSU_ULOG(HIGH_MSG, BUS_LOG, "PASS");   
    lpm_info->is_in_low_power_mode = TRUE;
    hsu_event_log(EVT_LPM_ENTER_SUSPEND, HS_USB_CORE_INDEX);
	
    /* SW driver writes 1 to USB2_PHY_CLAMP_MPM_DPSE_DMSE_EN_N
    ** bit of USB_OTG_HS_PHY_CTRL register. */
    HAL_hsusb_SetBits32(
      hsu_os_bus_hal_core_id(ctx->core_idx),
      HSU_LINK_USB_OTG_HS_PHY_CTR__ADDR,
      HWIO_USB_OTG_HS_PHY_CTRL_USB2_PHY_CLAMP_MPM_DPSE_DMSE_EN_N_BMSK);

    // Enable Retention mode by writing 1 to SIDDQ bit of HS_PHY_CTRL_COMMON0
    HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_OUTM(
      HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_SIDDQ_BMSK,
      0x1 << HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_SIDDQ_SHFT);

    hsu_clkmgr_release_clocks(HSU_USB2_CI_CORE);
    hsu_clkmgr_release_npa_clocks(HSU_USB2_CI_CORE);   

    if (ctx->xo_voted == TRUE)
    {
      /* Vote for XO shutdown */
      npa_issue_required_request(ctx->xo_handle, 0);
      ctx->xo_voted = FALSE;
    }

    HSU_ULOG(HIGH_MSG, BUS_LOG, "lpm on suspend");

  }
  else
  {
    HSU_ULOG(ERROR_MSG, BUS_LOG, "FAILED");
    
    /* Clears ULPI_STP_CTRL and ASYNC_INTR_CTRL bits of 
    ** USB_OTG_HS_USBCMD register 
    */ 
    HAL_hsusb_ClearBits32(
      hsu_os_bus_hal_core_id(ctx->core_idx),
      HAL_HSUSB_USBCMD_ADDR,
      HAL_HSUSB_USBCMD_ULPI_STP_CTRL_BMSK
      | HAL_HSUSB_USBCMD_ASYNC_INTR_CTRL_BMSK);

    /* Clears PHCD bit of USB_OTG_HS_PORTSC register. */
    hsu_lpm_clear_phcd_bit(ctx);
  }
}


/*===========================================================================
FUNCTION hsu_lpm_enter_disconnect

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_lpm_enter_disconnect(hsu_lpm_ctx_type *ctx)
{
  uint32 regval;
  uint32 res;
  
  hsu_lpm_info_type *lpm_info = ctx->lpm_info;

  HSU_ULOG(NO_MSG, BUS_LOG, "disconnect, CI_CORE");
  hsu_event_log(EVT_LPM_ENTER_DISCONNECTED, HS_USB_CORE_INDEX);

  /* SW writes ‘10’ to PTS bits of USB_OTG_HS_PORTSC register.
  */ 
  regval = HAL_hsusb_Read32(
    hsu_os_bus_hal_core_id(ctx->core_idx),
    HAL_HSUSB_PORTSC_ADDR(0));

  HAL_hsusb_Write32(
    hsu_os_bus_hal_core_id(ctx->core_idx),
    HAL_HSUSB_PORTSC_ADDR(0),
    (regval & ~HAL_HSUSB_PORTSC_PTS_BMSK) | HAL_HSUSB_PORTSC_PTS_ULPI_BMSK); 

  /* Set the OTGDISABLE bit of the POWER_AND_CLOCK_MANAGEMENT register to 0x1 
  ** through the ULPI VIEW PORT
  */
  res = hsu_os_bus_write_to_phy(
    HSU_PHY_POWER_AND_CLOCK_MANAGEMENT__SET_ADDR,
    HSU_PHY_POWER_AND_CLOCK_MANAGEMENT__OTGDISABLE_BMSK,
    hsu_os_bus_hal_core_id(ctx->core_idx));

  if (!res)
  {
    HSU_MSG_ERROR("failed to write otg disable to phy");
  }

  /* Clear ACAENB, DCDENB, VDATSRCENB0, VDATDETENB0 and CHRGSEL in the 
  ** CHARGING_DETECTION_CONTROL register through the ULPI VIEW PORT.
  */
  res = hsu_os_bus_write_to_phy(
    HSU_PHY_CHARGING_DETECTION_CONTROL__CLR_ADDR,
    HSU_PHY_CHARGING_DETECTION_CONTROL__ACAENB_BMSK
    | HSU_PHY_CHARGING_DETECTION_CONTROL__DCDENB_BMSK
    | HSU_PHY_CHARGING_DETECTION_CONTROL__VDATSRCENB_BMSK
    | HSU_PHY_CHARGING_DETECTION_CONTROL__VDATDETENB_BMSK
    | HSU_PHY_CHARGING_DETECTION_CONTROL__CHRGSEL_BMSK,
    hsu_os_bus_hal_core_id(ctx->core_idx));

  if (!res)
  {
    HSU_MSG_ERROR("failed to write charger det ctrl to phy");
  }

  /* SW sets the IDPULLUP by writing 1 USB_OTG_HS_OTGSC[5] and to 
  ** OTG_CTRL_REG[0] through the ULPI VIEW PORT. 
  */  
  HAL_hsusb_SetBits32(
    hsu_os_bus_hal_core_id(ctx->core_idx),
    HAL_HSUSB_OTGSC_ADDR,
    HAL_HSUSB_OTGSC_IDPU_BMSK);

  res = hsu_os_bus_write_to_phy(
    HSU_PHY_OTG_CTRL__SET_ADDR,
    HSU_PHY_OTG_CTRL__IDPULLUP_BMSK,
    hsu_os_bus_hal_core_id(ctx->core_idx));

  /* Set PHCD bit */
  hsu_lpm_set_phcd_bit(ctx);

  /* SW driver writes 1 to ULPI_STP_CTRL and ASYNC_INTR_CTRL bits of 
  ** USB_OTG_HS_USBCMD register.
  */
  HAL_hsusb_SetBits32(
    hsu_os_bus_hal_core_id(ctx->core_idx),
    HAL_HSUSB_USBCMD_ADDR,
    HAL_HSUSB_USBCMD_ULPI_STP_CTRL_BMSK | 
    HAL_HSUSB_USBCMD_ASYNC_INTR_CTRL_BMSK);

  /* New for 9x25 */
  /* SW driver writes 1 to USB2_PHY_CLAMP_MPM_DPSE_DMSE_EN_N
  ** bit of USB_OTG_HS_PHY_CTRL register. */
  HAL_hsusb_SetBits32(
    hsu_os_bus_hal_core_id(ctx->core_idx),
    HSU_LINK_USB_OTG_HS_PHY_CTR__ADDR,
    HWIO_USB_OTG_HS_PHY_CTRL_USB2_PHY_CLAMP_MPM_DPSE_DMSE_EN_N_BMSK);


  /* New for 9x25 - 2 new bits */
  /* SW driver writes 1 to USB2_PHY_IDHV_INTEN,USB2_PHY_OTGSESSVLDHV_INTEN, 
  ** USB2_PHY_DPSE_INTEN, USB2_PHY_DMSE_INTEN bits of USB_OTG_HS_PHY_CTRL register. 
  */
  HAL_hsusb_SetBits32(
    hsu_os_bus_hal_core_id(ctx->core_idx),
    HSU_LINK_USB_OTG_HS_PHY_CTR__ADDR,
    HSU_LINK_USB_OTG_HS_PHY_CTR__USB2_PHY_OTGSESSVLDHV_INTEN_BMSK |
    HWIO_USB_OTG_HS_PHY_CTRL_USB2_PHY_DPSE_INTEN_BMSK |
    HWIO_USB_OTG_HS_PHY_CTRL_USB2_PHY_DMSE_INTEN_BMSK );


  // Enable Retention mode by writing 1 to SIDDQ bit of HS_PHY_CTRL_COMMON0
  HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_OUTM(
    HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_SIDDQ_BMSK,
    0x1 << HWIO_USB2_PHY_USB_PHY_HS_PHY_CTRL_COMMON0_SIDDQ_SHFT);

  if (hsu_lpm_is_ldo_power_collapse_enabled())
  {
    /* If HS-USB core is not enabled or LDO power collapse NV is enabled,
    ** program the USB HW to disable interrupts.
    */
    
    /* SW set PHY_MPM_HV_CLAMP_EN, USB2_PHY_OTGSESSVLDHV_CLAMP_EN, 
    ** USB2_PHY_IDHV_CLAMP_EN and USB2_PHY_CLAMP_MPM_DPSE_DMSE_EN_N bits 
    ** in USB_OTG_HS_PHY_CTRL to 0
    */
      HAL_hsusb_ClearBits32(
        hsu_os_bus_hal_core_id(ctx->core_idx),
        HSU_LINK_USB_OTG_HS_PHY_CTR__ADDR,
        HSU_LINK_USB_OTG_HS_PHY_CTR__PHY_MPM_HV_CLAMP_EN_BMSK |
        HSU_LINK_USB_OTG_HS_PHY_CTR__USB2_PHY_OTGSESSVLDHV_CLAMP_EN_BMSK |
        HSU_LINK_USB_OTG_HS_PHY_CTR__USB2_PHY_IDHV_CLAMP_EN_BMSK |
        HWIO_USB_OTG_HS_PHY_CTRL_USB2_PHY_CLAMP_MPM_DPSE_DMSE_EN_N_BMSK);
  }

  /* 9x35_BU - TODO: find similar api for 9x35
  ** if (hsu_sps_sm_is_usb_clock_off_allowed())
  */
  {
    /* SW driver stops usb_system_clk and votes for stopping the bus clock, 
    ** TCXO shutdown and VDD_MIN 
    */
    hsu_clkmgr_release_clocks(HSU_USB2_CI_CORE);
    hsu_clkmgr_release_npa_clocks(HSU_USB2_CI_CORE);   

    /* Vote to turn off LDO or put into low power */
    hsu_vregmgr_turn_off(HSU_VREG_ID_HSUSB);
  }
  
  if (ctx->xo_voted == TRUE)
  {
#if !defined(FEATURE_HS_USB_PRE_SILICON_BU)
    /* Vote for XO shutdown */
    npa_issue_required_request(ctx->xo_handle, 0);
#endif /* FEATURE_HS_USB_PRE_SILICON_BU */
    ctx->xo_voted = FALSE;
  }
  
  lpm_info->is_in_low_power_mode = TRUE;
}


/*===========================================================================
FUNCTION hsu_lpm_enter_is_valid

DESCRIPTION
  It verifies where the  present time is proper for following LPM operation. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static boolean hsu_lpm_enter_is_valid(hsu_lpm_ctx_type *ctx)
{
  hsu_lpm_info_type *lpm_info = ctx->lpm_info;

  if (!ctx->is_lpm_feature_active || 
      !lpm_info->lpm_is_active)
  {
      HSU_MSG_HIGH_2("hsu_lpm_is_valid_for_lpm():lpm is not active %d %d", 
        ctx->is_lpm_feature_active, lpm_info->lpm_is_active);  
      return FALSE;
  }

  if (!(!(lpm_info->is_in_low_power_mode) && 
       (!ctx->is_called_by_lpm_timer || lpm_info->lpm_entry_is_pending)))
  {
    return FALSE;
  }
  
  return TRUE;
}


/*===========================================================================
FUNCTION hsu_lpm_set_phcd_bit

DESCRIPTION


DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
static boolean hsu_lpm_set_phcd_bit(hsu_lpm_ctx_type *ctx)
{
  uint32 time_elapse_us;

  /* SW driver writes 1 to PHCD bit of SB_OTG_HS_PORTSC register.
  ** Keep the reference clock active for a minimum of 2.5 us 
  ** after setting the SUSPENDM to 1’b0 in order to ensure that all power 
  ** down signals are set.
  */
  HAL_hsusb_SetBits32(
    hsu_os_bus_hal_core_id(ctx->core_idx),
    HAL_HSUSB_PORTSC_ADDR(0),
    HAL_HSUSB_PORTSC_PHCD_BMSK); 

  HSU_ULOG_ENUM(NO_MSG, BUS_LOG, "phcd_bit, SET", HSU_CORE_OFFSET2, ctx->core_idx);

  for (time_elapse_us = 0; 
       time_elapse_us < (PHCD_WRITE_MAX_TRY * PHCD_WRITE_DELAY); 
       time_elapse_us+=PHCD_WRITE_DELAY)
  {
    if (HAL_hsusb_GetBits32(hsu_os_bus_hal_core_id(ctx->core_idx),
                           HAL_HSUSB_PORTSC_ADDR(0),
                           HAL_HSUSB_PORTSC_PHCD_BMSK))
    {
      break;
    }
    hsu_os_delay_us(PHCD_WRITE_DELAY);
  }
  if (time_elapse_us >= (PHCD_WRITE_MAX_TRY * PHCD_WRITE_DELAY))
  {
    HSU_ULOG_ENUM_1(ERROR_MSG, BUS_LOG, "phcd_bit, SET, FAIL, timeout %u us", HSU_CORE_OFFSET2, ctx->core_idx, time_elapse_us);
    hsu_event_log(EVT_PHCD_BIT_SET_FAIL, HS_USB_CORE_INDEX);

    return FALSE;
  }

  return TRUE;
}


/*------------------------------------------------------------------------------
Exit Low Power APIs
------------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION hsu_lpm_exit_is_valid

DESCRIPTION 
  It verifies where the  present time is proper for following LPM operation. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean hsu_lpm_exit_is_valid(hsu_lpm_ctx_type *ctx)
{
  hsu_lpm_info_type *lpm_info = ctx->lpm_info;
  if (!ctx->is_lpm_feature_active || 
      !lpm_info->lpm_is_active)
  {
    return FALSE;
  }

  if(!lpm_info->is_in_low_power_mode)
  {
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION hsu_lpm_clear_phcd_bit

DESCRIPTION
 Function checks if PHCD has been cleared.

DEPENDENCIES
 None.

RETURN VALUE
 None.
 
SIDE EFFECTS
 None.
===========================================================================*/
static boolean hsu_lpm_clear_phcd_bit(hsu_lpm_ctx_type *ctx)
{
   uint32 time_elapse_us;
 
  /* Read PHCD bit in PORTSC in order to see if it was cleared. 
  ** Wait for PHCD_READ_MAX_TRY * PHCD_READ_DELAY 
  ** until PHCD bit is cleared 
  */
  for (time_elapse_us = 0; 
       time_elapse_us < (PHCD_READ_MAX_TRY * PHCD_READ_DELAY); 
       time_elapse_us+=PHCD_READ_DELAY)
  {    
    if (!HAL_hsusb_GetBits32(hsu_os_bus_hal_core_id(ctx->core_idx),
                           HAL_HSUSB_PORTSC_ADDR(0),
                           HAL_HSUSB_PORTSC_PHCD_BMSK))
    {
      break;
    }
    else
    {      
      /* If PHCD was not cleared by HW, then clear it manually.
      ** This may be the case when HS-USB is disconnected or supsended
      ** But we may need to bring it out of low power mode
      ** to configure the core.
      */
      HAL_hsusb_ClearBits32(
        hsu_os_bus_hal_core_id(ctx->core_idx),
        HAL_HSUSB_PORTSC_ADDR(0),
        HAL_HSUSB_PORTSC_PHCD_BMSK); 

      HSU_ULOG_ENUM(NO_MSG, BUS_LOG, "phcd_bit, CLEAR", HSU_CORE_OFFSET2, ctx->core_idx);
    }

    hsu_os_delay_us(PHCD_READ_DELAY);
  }
  if (time_elapse_us >= (PHCD_READ_MAX_TRY * PHCD_READ_DELAY))
  {
    hsu_event_log(EVT_PHCD_BIT_CLR_FAIL, HS_USB_CORE_INDEX);
    HSU_ERR_FATAL("hsu_lpm_clear_phcd_bit: PHCD didn't clear (%u)usec", 
      time_elapse_us, 0, 0);
    return FALSE;
  }

  return TRUE;
}

