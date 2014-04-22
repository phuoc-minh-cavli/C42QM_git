/*==============================================================================

  SNPS controller Low Power Management
  
  GENERAL DESCRIPTION
    This module defines APIs for SNPS Link and Phy low power management.

  EXTERNALIZED FUNCTIONS
    hsu_lpm_snps_init
    hsu_lpm_snps_enter
    hsu_lpm_snps_exit
    hsu_lpm_snps_get_ctx
    
  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.

  Copyright (c) 2014, 2020 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/9205/hsu_lpm_snps.c#6 $
  $DateTime: 2023/07/05 02:30:34 $ $Author: pwbldsvc $
    
  when      who           what, where, why
  --------  ---           ------------------------------------------------------------
  2/20/20   CS            Programmed QUSB2 interrupts to recieve interrupts from QGIC, in case of APSS collapse
  2/7/14    dpatnaik      Initial version

==============================================================================*/

//==============================================================================
//                            INCLUDE FILES FOR MODULE
//==============================================================================
#include <comdef.h>
#include <customer.h>

#include "hsu_clkmgr.h"
#include "hsu_common_int.h"
#include "hsu_os_abstract_bus.h"
#include "hsu_os_abstract_bus_ext_int.h"

#include "hsu_lpm_snps.h"
#include "hsu_conf_sel_i.h"
#include "hsu_conf_sel_nv.h"
#include "hsu_vregmgr.h"
#include "hsu_phy_utils_int.h"
#include "hsu_core_config.h"
#include "hsu_core_config_int.h"

#include "hsu_log.h"
#include "npa.h"
#include "hsu_platform.h"
#include "hsu_hwio_dependency.h"
#include "jusb_core.h"
#include "hsu_bus_mode_notifier.h"
#include "hsu_mdm_apis.h"

#include "hsu_qurt_util.h"

#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DDIInterruptController.h"
#include "DALStdErr.h"

//==============================================================================
// Typedefs
//==============================================================================

//==============================================================================
//  Macro and Constant Definition
//==============================================================================
#define LINK_STATE_SUSPEND_U3 (0x3)
#define LINK_STATE_SS_DISABLED (0x4)

#define TRAMP_USB3_POWER_EVENT_IRQ 74
#define TRAMP_USB_QUSB2_PHY_IRQ    168

//==============================================================================
// Externalized Variables
//==============================================================================

uint32 clear_qusb2_reg = 0;
//==============================================================================
// Local Variables
//==============================================================================

// HSU LPM context
hsu_lpm_snps_ctx_type hsu_lpm_snps_ctx = {
  /* core_index             */       0,
  /* is_lpm_feature_active  */       0,
  /* is_pwr_event_phy_in_L2 */       0,
  /* is_pwr_event_phy_in_P3 */       0,
  /* is_lpm_voted_by_sw     */       0,


  /* is_hs_phy_in_suspend   */       0,
  /* is_ss_phy_in_autonomous_mode */ 0,

  /* power_event            */       PWR_EVNT_SW, 
  /* crit_sec               */       NULL,
  /* lpm_info               */       NULL,
  /* ldo_power_collapse_enabled */   FALSE,
  /* xo_handle */                    NULL,
  /* xo_voted */                     FALSE,
};

//==============================================================================
// Local Function Declarations
//==============================================================================
static void hsu_lpm_snps_hs_phy_enter_suspend(hsu_lpm_snps_ctx_type *ctx);
static void hsu_lpm_snps_hs_phy_enter_disconnect(hsu_lpm_snps_ctx_type *ctx);
static void hsu_lpm_snps_hs_phy_exit_suspend(hsu_lpm_snps_ctx_type *ctx);

//==============================================================================
//
// Public Function Definitions
//
//==============================================================================

//*=============================================================================
// FUNCTION hsu_lpm_get_snps_ctx
//==============================================================================
hsu_lpm_snps_ctx_type *hsu_lpm_get_snps_ctx(void)
{
  return &hsu_lpm_snps_ctx;
}

//==============================================================================
// FUNCTION hsu_lpm_snps_init
//==============================================================================
void hsu_lpm_snps_init(void)
{
  // Bring SNPS USB 3.0 controller out of low power mode

  // WARNING:
  // This is only done once during BOOT UP.
  // Originally, we intended to put this operation on top of 
  // hsu_init_snps_usb(), but DBM breaks after this operation,
  // so it is kept here.
  
  // Disable the power domain in case it is left On in SBL
  hsu_clkmgr_enable_power_domain(FALSE, HSU_USB3_SNPS_CORE);
  
  // Enable power domain
  hsu_clkmgr_enable_power_domain(TRUE, HSU_USB3_SNPS_CORE);

  hsu_lpm_snps_ctx.xo_handle = 
    npa_create_sync_client(
      "/xo/cxo", 
      "hsusb_xo",
      NPA_CLIENT_REQUIRED);

  HSU_ASSERT(hsu_lpm_snps_ctx.xo_handle != NULL);

  /* Vote against xo shutdown */
  npa_issue_required_request(hsu_lpm_snps_ctx.xo_handle, 2);  
  hsu_lpm_snps_ctx.xo_voted = TRUE;
}

//==============================================================================
// FUNCTION hsu_lpm_snps_qusb2_ref_clk_enable
//==============================================================================
void hsu_lpm_snps_qusb2_ref_clk_enable(boolean enable)
{
#if 0 // 9x65
  uint32 reg;

  if (enable)
  {
    HSU_ULOG(NO_MSG, BUS_LOG, "enable");
    // Tune the value in PHY CSR to ensure it can work with Single Ended Reference Clock
    if (HSU_SINGLE_ENDED_REF_CLK == hsu_hw_phy_reference_clk())
    {
      HWIO_QUSB2PHY_QUSB2PHY_PLL_TEST_OUT(0x00); 
      hsu_os_delay_us(150);
      HWIO_QUSB2PHY_QUSB2PHY_PLL_TEST_OUT(HWIO_QUSB2PHY_QUSB2PHY_PLL_TEST_CLK_REF_SEL_BMSK); // 0x80
    }
    else
    {
      hsu_os_delay_us(150);
      HWIO_QUSB2PHY_QUSB2PHY_PLL_TEST_OUT(0x00);

      // USB2 PHY ref clock must be enabled after taking phy out of power down

      reg = HWIO_GCC_QUSB_REF_CLK_EN_IN;
      reg = HSU_VAR_OUTM(reg, 0x1, HWIO_GCC_QUSB_REF_CLK_EN_CLK_ENABLE_BMSK, HWIO_GCC_QUSB_REF_CLK_EN_CLK_ENABLE_SHFT);
      reg = HSU_VAR_OUTM(reg, 0x1, HWIO_GCC_QUSB_REF_CLK_EN_HW_CONTROL_BMSK, HWIO_GCC_QUSB_REF_CLK_EN_HW_CONTROL_SHFT);
      HWIO_GCC_QUSB_REF_CLK_EN_OUT(reg);
    }
    hsu_api_qusb2_phy_pll_lock();
  }
  else
  {
    HSU_ULOG(NO_MSG, BUS_LOG, "disable");

    // Tune the value in PHY CSR to ensure it can work with Single Ended Reference Clock
    if (HSU_SINGLE_ENDED_REF_CLK == hsu_hw_phy_reference_clk())
    {
      if (hsu_clkmgr_is_hs_only_mode())
      {
        // For HS PHY only mode, REF CLK cannot be disabled/enabled.
        // When QUSB2 REF CLK is disabled, it will prvent OUT_L2 and QUSB2 PHY IRQ
        HSU_ULOG_1(NO_MSG, BUS_LOG, "skipped_se_refclk_disable_hs_phy_only, enabled %u", enable);

        // Perform masked-write to retain SE clock setting
        HWIO_QUSB2PHY_QUSB2PHY_PLL_TEST_OUTM(HWIO_QUSB2PHY_QUSB2PHY_PLL_TEST_PLL_REG_RPDN_CTRL_BMSK,
         0x1 << HWIO_QUSB2PHY_QUSB2PHY_PLL_TEST_PLL_REG_RPDN_CTRL_SHFT); // 0x88
      }
      else
      {
        HWIO_QUSB2PHY_QUSB2PHY_PLL_TEST_OUT(HWIO_QUSB2PHY_QUSB2PHY_PLL_TEST_PLL_REG_RPDN_CTRL_BMSK); // 0x08
      }
    }
    else
    {
      if (hsu_clkmgr_is_hs_only_mode())
      {
        // For HS PHY only mode, REF CLK cannot be disabled/enabled.
        // When QUSB2 REF CLK is disabled, it will prvent OUT_L2 and QUSB2 PHY IRQ
        HSU_ULOG_1(NO_MSG, BUS_LOG, "skipped_diff_refclk_disable_hs_phy_only, enabled %u", enable);
      }
      else
      {
        // USB2 PHY ref clock must be disabled after putting phy into power down
        reg = HWIO_GCC_QUSB_REF_CLK_EN_IN;
        reg = HSU_VAR_OUTM(reg, 0x0, HWIO_GCC_QUSB_REF_CLK_EN_CLK_ENABLE_BMSK, HWIO_GCC_QUSB_REF_CLK_EN_CLK_ENABLE_SHFT);
        reg = HSU_VAR_OUTM(reg, 0x0, HWIO_GCC_QUSB_REF_CLK_EN_HW_CONTROL_BMSK, HWIO_GCC_QUSB_REF_CLK_EN_HW_CONTROL_SHFT);
        HWIO_GCC_QUSB_REF_CLK_EN_OUT(reg);
      }
      HWIO_QUSB2PHY_QUSB2PHY_PLL_TEST_OUT(HWIO_QUSB2PHY_QUSB2PHY_PLL_TEST_PLL_REG_RPDN_CTRL_BMSK); // 0x08
    }
  }
#endif
}



//==============================================================================
// FUNCTION hsu_lpm_snps_enter
//==============================================================================
void hsu_lpm_snps_enter(hsu_lpm_snps_ctx_type *ctx)
{
  hsu_lpm_info_type  *lpm_info;
  uint32              link_state;
  uint32 			irq_status;
  device_speed_t      curr_speed;
  DALResult eRes = 0;
  DalDeviceHandle *hsu_h_int_cntlr = NULL;

  HSU_ASSERT(ctx);
  
  eRes = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &hsu_h_int_cntlr);
  HSU_ASSERT(hsu_h_int_cntlr != NULL && eRes == DAL_SUCCESS);


  rex_enter_crit_sect(ctx->crit_sec);

  lpm_info = ctx->lpm_info;

  if ( (FALSE == ctx->is_lpm_feature_active)
    || (FALSE == lpm_info->lpm_is_active)
    || (TRUE == lpm_info->is_in_low_power_mode) )
  {   
    HSU_ULOG_3(NO_MSG, BUS_LOG, "filtered, lpm_feature_active %u, lpm_active %u, in_low_power_mode %u", 
      ctx->is_lpm_feature_active, 
      lpm_info->lpm_is_active, 
      lpm_info->is_in_low_power_mode);
    
    rex_leave_crit_sect(ctx->crit_sec);   
    return;
  }
  eRes = DalInterruptController_InterruptDisable(hsu_h_int_cntlr, TRAMP_USB_QUSB2_PHY_IRQ);
  eRes = DalInterruptController_InterruptDisable(hsu_h_int_cntlr, TRAMP_USB3_POWER_EVENT_IRQ);

  if (hsu_clkmgr_is_hs_only_mode())
  {
    // P3 related power event IRQs are disabled for high speed mode only
    ctx->is_pwr_event_phy_in_P3 = TRUE;
    ctx->is_ss_phy_in_autonomous_mode = TRUE;
  }

  // If the core is not enabled, then put into maximum low power mode
  // same as same disconnect case
  if (FALSE == hsu_conf_sel_hs_main_core_enabled())
  {
    lpm_info->cable_is_connected_during_lpm = FALSE;
  }
  else
  { 
    // SW checks where cable is attached
    lpm_info->cable_is_connected_during_lpm = 
      hsu_port_is_attached(ctx->core_index);
  }
  
  lpm_info->lpm_entry_is_pending = FALSE;

  // Check if disconnected or suspended
  if (FALSE == lpm_info->cable_is_connected_during_lpm)
  {
    switch (ctx->power_event)
    {
      case PWR_EVNT_SW :
      {
        // Enable HS SUSPEND L2 transition
        hsu_lpm_snps_set_hs_phy_suspend_L2_transition(TRUE);       

        ctx->is_lpm_voted_by_sw = TRUE;
      }
      break;  
      #if 0  
      case PWR_EVNT_PHY_L2 :
      {
        ctx->is_pwr_event_phy_in_L2 = TRUE;
        //hsu_lpm_snps_qusb2_ref_clk_enable(FALSE);
      }
      break;
	  #endif      
      
      default:
      {
        HSU_ERR_FATAL("Invalid event passed to Enter LPM for DISCONNET %x", ctx->power_event, 0, 0);
      }
    }
	//delay to clear out power event irq
	hsu_os_delay_us(200);
	irq_status = HWIO_PWR_EVNT_IRQ_STAT_IN;
	//clearing out pwr evt irq to prevent interrupt storm
    HWIO_PWR_EVNT_IRQ_STAT_OUT(irq_status);
	//Setting L2 to TRUE by default so that USB into LPM when USB is disconnected 
    ctx->is_pwr_event_phy_in_L2 = TRUE;
    HSU_ULOG_ENUM_3(NO_MSG, BUS_LOG, "hsu_lpm_snps_enter, disconnected, lpm_vote_by_sw %u, pwr_event_L2 %u, pwr_evt_P3 %u", 
      ctx->power_event, HSU_PWR_EVNT_OFFSET2, 
      ctx->is_lpm_voted_by_sw,
      ctx->is_pwr_event_phy_in_L2,
      ctx->is_pwr_event_phy_in_P3);

    // Turn OFF clocks if both PHY's are in low power mode
    // and s/w voted for low power mode.
    if ( (ctx->is_pwr_event_phy_in_L2)
      && (ctx->is_lpm_voted_by_sw) )
    {
      //hsu_event_log(EVT_LPM_ENTER_DISCONNECTED, USB3_CORE_INDEX);
      
      hsu_lpm_snps_hs_phy_enter_disconnect(ctx);
	  
	  out_byte (HWIO_USB2_PHY_USB_PHY_PWRDOWN_CTRL_ADDR, 0x00);

      hsu_clkmgr_release_clocks(HSU_USB3_SNPS_CORE);
      hsu_clkmgr_release_npa_clocks(HSU_USB3_SNPS_CORE);
      
      hsu_vregmgr_turn_off(HSU_VREG_ID_HSUSB);

      lpm_info->is_in_low_power_mode = TRUE;

      // Disable interrupt to rely on VBUS notification
      hsu_os_bus_interrupt_uninit(HSU_USB3_SNPS_CORE);

      // Put controller into power collapse
      hsu_clkmgr_enable_power_domain(FALSE, HSU_USB3_SNPS_CORE);
	  
	  if (ctx->xo_voted == TRUE)
      {
        /* Vote for XO shutdown */
        npa_issue_required_request(ctx->xo_handle, 0);
        ctx->xo_voted = FALSE;
      }
    }
  }
  else
  {
    // Entering low power due to SUPSEND (U3/L2)
    curr_speed = hsu_lpm_snps_get_curr_speed_from_hw();
    
    HSU_ULOG_ENUM(NO_MSG, BUS_LOG, "curr_speed", curr_speed, HSU_SPEED_OFFSET2);       

    if (hsu_conf_sel_disable_a2_pc_for_suspend())
    {
      HSU_ULOG(NO_MSG, BUS_LOG, "suspended, filtered");
    }
    else
    { 
      link_state = HWIO_DSTS_INM(HWIO_DSTS_USBLNKST_BMSK) >> HWIO_DSTS_USBLNKST_SHFT;

      switch (ctx->power_event)
      {
        case PWR_EVNT_SW :
        {
          // Sometimes SW votes for low power mode when the bus is not even 
          // in SUSPEND.  Filter these requests.
          if (hsu_get_current_bus_mode(hsu_os_bus_hal_core_id(ctx->core_index)) 
            != HSU_BUS_MODE_SUSPENDED)
          {
            HSU_ULOG(NO_MSG, BUS_LOG, "pwr_event_sw, reject_due_to_not_in_suspend");
            hsu_event_log(EVT_SW_VOTED_FOR_LPM_CONNECTED__WITHOUT_SUSPEND, ctx->core_index);
          }
          else if (FALSE == ctx->is_lpm_voted_by_sw)
          {
#if 0 // TODO: -- Akash --
            if (hsu_clkmgr_is_hs_only_mode())
            {
              // For HS PHY only mode, disable IN_L2 transition (effectively
              // disabling low power mode) due to following:
              // When HS PHY REF CLK is OFF for handling L2, the PHY ISR / OUT_L2
              // event is not generated
              HSU_ULOG(NO_MSG, BUS_LOG, "hs_phy_only_suspend, enable_L2_trans_skipped");              
            }
            else
#endif
            {
              // Enable HS SUSPEND L2 transition
              hsu_lpm_snps_set_hs_phy_suspend_L2_transition(TRUE);
              ctx->is_lpm_voted_by_sw = TRUE;
              
              //hsu_event_log(EVT_LPM_ENTER_SUSPEND, USB3_CORE_INDEX);
            }            
          }
        }
        break;
    
        case PWR_EVNT_PHY_L2 :
        {
          if ( (curr_speed != SPEED_SUPER) 
            && (link_state != LINK_STATE_SUSPEND_U3) )
          {
            HSU_ULOG_1(NO_MSG, BUS_LOG, "suspend, not_u3, link state %u", link_state);
          }
          else
          {
            // Power event can be used for checking suspend state for SNPS controller
            if (FALSE == ctx->is_pwr_event_phy_in_L2)
            {
              // Wait for 125us then check link state        
              hsu_os_delay_us(125);
              HSU_ULOG_3(NO_MSG, BUS_LOG, "LTDBLINK 0x%08x DSTS_USBLNKST 0x%08x PHY_STATUS 0x%08x", 
                (HWIO_GDBGLTSSM_INM(HWIO_GDBGLTSSM_LTDBLINKSTATE_BMSK) >> HWIO_GDBGLTSSM_LTDBLINKSTATE_SHFT),
                (HWIO_DSTS_INM(HWIO_DSTS_USBLNKST_BMSK) >> HWIO_DSTS_USBLNKST_SHFT),
                (HWIO_USB30_STS_REG_INM(HWIO_USB30_STS_REG_USB30_PHY_STATUS_BMSK) >> HWIO_USB30_STS_REG_USB30_PHY_STATUS_SHFT));

              
              ctx->is_pwr_event_phy_in_L2 = TRUE; 
              ctx->is_lpm_voted_by_sw = TRUE; 
              hsu_lpm_snps_hs_phy_enter_suspend(ctx);
              hsu_lpm_snps_qusb2_ref_clk_enable(FALSE);
            }
          }
        }
        break;
    
        default:
        {
          HSU_ERR_FATAL("Invalid event passed to Enter LPM %x", ctx->power_event, 0, 0);
        }
      }
       
       HSU_ULOG_ENUM_3(NO_MSG, BUS_LOG, "hsu_lpm_snps_enter, suspend, lpm_vote_by_sw %u, pwr_event_L2 %u, pwr_evt_P3 %u", 
         ctx->power_event, HSU_PWR_EVNT_OFFSET2, 
         ctx->is_lpm_voted_by_sw,
         ctx->is_pwr_event_phy_in_L2,
         ctx->is_pwr_event_phy_in_P3);

      // Turn OFF clocks if both PHY's are in low power mode
      // and s/w voted for low power mode.
      if ( (ctx->is_pwr_event_phy_in_L2)
        && (ctx->is_lpm_voted_by_sw) 
        && (link_state == LINK_STATE_SUSPEND_U3) )
      {
		//if QUSB2 registers are cleared, then program them before turning off the clocks for RESUME event in case of APSS collapse.
		if(clear_qusb2_reg == 0)
		{
			HWIO_USB2_PHY_USB_PHY_IRQ_CMD_OUT(0);
			hsu_os_delay_us(20);
		
			HWIO_USB2_PHY_USB_PHY_IRQ_CMD_OUT(0x01010101);
			hsu_os_delay_us(20);
		
			clear_qusb2_reg = 1;
		
			HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_OUT(0x02020202);
			hsu_os_delay_us(20);
		}
		lpm_info->is_in_low_power_mode = TRUE;
        hsu_clkmgr_release_clocks(HSU_USB3_SNPS_CORE);
        hsu_clkmgr_release_npa_clocks(HSU_USB3_SNPS_CORE);
		if (ctx->xo_voted == TRUE)
        {
          /* Vote for XO shutdown */
          npa_issue_required_request(ctx->xo_handle, 0);
          ctx->xo_voted = FALSE;
		  HSU_ULOG(ERROR_MSG, DBG_E_LOG, "-------  Voting for XO shutdown -------");
        }
      }
    }
    eRes = DalInterruptController_InterruptEnable(hsu_h_int_cntlr, TRAMP_USB3_POWER_EVENT_IRQ);
    eRes = DalInterruptController_InterruptEnable(hsu_h_int_cntlr, TRAMP_USB_QUSB2_PHY_IRQ);
  }
  
  rex_leave_crit_sect(ctx->crit_sec);   
}

//==============================================================================
// FUNCTION hsu_lpm_snps_exit
//==============================================================================
void hsu_lpm_snps_exit(hsu_lpm_snps_ctx_type *ctx)
{
  hsu_lpm_info_type *lpm_info = ctx->lpm_info;
  uint32 irq_status = 0;
  rex_enter_crit_sect(ctx->crit_sec);
    
  if ( (FALSE == ctx->is_lpm_feature_active)
    || (FALSE == lpm_info->lpm_is_active) )
  {
    HSU_ULOG_2(NO_MSG, BUS_LOG, "filtered, lpm_feature_active %u, lpm_is_active %u",
      ctx->is_lpm_feature_active, lpm_info->lpm_is_active);
    
    rex_leave_crit_sect(ctx->crit_sec);
    return;
  }

  if ((FALSE == hsu_vregmgr_is_usb_cable_attached())
      && (PWR_EVNT_SW != ctx->power_event)
      && (TRUE == lpm_info->is_in_low_power_mode))
  {
    HSU_ULOG(NO_MSG, BUS_LOG, "discard_pwr_evt");
    rex_leave_crit_sect(ctx->crit_sec);
    return;
  }

  hsu_vregmgr_turn_on(HSU_VREG_ID_HSUSB);

  hsu_clkmgr_config_npa_clocks(HSU_USB3_SNPS_CORE);
  hsu_clkmgr_config_clocks(HSU_USB3_SNPS_CORE);
  
  //if QUSB2 registers are not cleared, then clear them just after turning on the clocks to prevent interrupt flood.
	if(clear_qusb2_reg == 1)
	{
	HWIO_USB2_PHY_USB_PHY_INTERRUPT_MASK0_OUT(0);
	hsu_os_delay_us(20);
	
	irq_status = HWIO_USB2_PHY_USB_PHY_INTERRUPT_STATUS0_IN;
	
	HWIO_USB2_PHY_USB_PHY_INTERRUPT_CLEAR0_OUT(irq_status);
	hsu_os_delay_us(20);
	
	HWIO_USB2_PHY_USB_PHY_IRQ_CMD_OUT(0);
	hsu_os_delay_us(20);
	HWIO_USB2_PHY_USB_PHY_IRQ_CMD_OUT(0x01010101);
	hsu_os_delay_us(20);
	
	clear_qusb2_reg = 0;
	}
  
  // Is this exiting Low Power Mode from suspend or Disconnect
  if (lpm_info->cable_is_connected_during_lpm)
  {
    // Exit from suspend
    if (hsu_conf_sel_disable_a2_pc_for_suspend())
    {
      HSU_ULOG(NO_MSG, BUS_LOG, "suspend, filtered");
    }
    else
    {
      switch (ctx->power_event)
      {
        case PWR_EVNT_SW :
        {
          ctx->is_lpm_voted_by_sw = FALSE;
          if (hsu_lpm_snps_get_curr_speed_from_hw() != SPEED_SUPER)
          {
            // It is required to bring PHY out of retention before
            // disabling/enabling L2 transition
            // Do not clear L2 immediately, and let transition event clear it.
            hsu_lpm_snps_hs_phy_exit_suspend(ctx);

            // Disable HS SUSPEND L2 transition
            hsu_lpm_snps_set_hs_phy_suspend_L2_transition(FALSE);

          }
          else
          {
            // Enable HS SUSPEND L2 transition
            hsu_lpm_snps_set_hs_phy_suspend_L2_transition(TRUE);
          }
        }
        break;
    
        case PWR_EVNT_PHY_L2:
        {
          hsu_lpm_snps_qusb2_ref_clk_enable(TRUE);
          if (TRUE == ctx->is_pwr_event_phy_in_L2)
          {
            ctx->is_pwr_event_phy_in_L2 = FALSE;
            hsu_lpm_snps_hs_phy_exit_suspend(ctx);
            hsu_lpm_snps_set_hs_phy_suspend_L2_transition(FALSE);
            hsu_os_bus_interrupt_init(ctx->core_index);
          }
        }
        break;

        case QUSB2_PHY_IRQ:
        {
          if (TRUE == ctx->is_pwr_event_phy_in_L2)
          {
            ctx->is_pwr_event_phy_in_L2 = FALSE;
            //hsu_lpm_snps_qusb2_ref_clk_enable(TRUE);

            hsu_lpm_snps_hs_phy_exit_suspend(ctx);
            hsu_lpm_snps_set_hs_phy_suspend_L2_transition(FALSE);
            hsu_os_bus_interrupt_init(ctx->core_index);
          }
        }
        break;
    
        default:
        {
          HSU_ERR_FATAL("Invalid event passed to Exit LPM %x", ctx->power_event, 0, 0);
        }
      }

      HSU_ULOG_ENUM_3(NO_MSG, BUS_LOG, "hsu_lpm_snps_exit, suspend, lpm_vote_by_sw %u, pwr_event_L2 %u, pwr_evt_P3 %u", 
        ctx->power_event, HSU_PWR_EVNT_OFFSET2, 
        ctx->is_lpm_voted_by_sw,
        ctx->is_pwr_event_phy_in_L2,
        ctx->is_pwr_event_phy_in_P3);
    }

    if (ctx->xo_voted == FALSE)
    {
      /* Vote against XO shutdown */
      npa_issue_required_request(ctx->xo_handle, 2);
      ctx->xo_voted = TRUE;
    }

  }
  else
  {
    if (FALSE == lpm_info->is_in_low_power_mode)
    {
      // Exit from disconnect should be filter when we are not in low power 
      // mode for disconnect case
      HSU_ULOG_1(NO_MSG, BUS_LOG, "disconnected, filtered, lpm_exit_due_remote_wakeup %u",
        lpm_info->was_lpm_exit_due_to_remote_wakeup);
    }
    else
    {
      hsu_platform_info_type const * platform_info = hsu_platform_get_info();

      // Exit from disconnect 
      HSU_ULOG_1(NO_MSG, BUS_LOG, "disconnected, lpm_exit_due_remote_wakeup %u",
        lpm_info->was_lpm_exit_due_to_remote_wakeup);

      // Controller exit power collapse
      hsu_clkmgr_enable_power_domain(TRUE, HSU_USB3_SNPS_CORE);

      switch (platform_info->hsu_platform_id)
      {
        case HSU_PLATFORM_VIRTIO:
        case HSU_PLATFORM_RUMI:
        {
          // Do nothing for pre-sil
        }
        break;
        default:
        {
#if 0 // 9x65
          /* Bring Out the PHY from power down mode */
          HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_OUTM(HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_POWER_DOWN_BMSK,
              (0x0 << HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_POWER_DOWN_SHFT));
#endif
        }
        break;
      }

      if (TRUE == ctx->is_pwr_event_phy_in_L2)
      {
        hsu_lpm_snps_hs_phy_exit_suspend(ctx);
      }

      if (FALSE == hsu_clkmgr_is_hs_only_mode())
      {
        ctx->is_pwr_event_phy_in_P3 = FALSE;
      }
      ctx->is_pwr_event_phy_in_L2 = FALSE;
      ctx->is_lpm_voted_by_sw = FALSE;

      HSU_ULOG_ENUM_3(NO_MSG, BUS_LOG, "hsu_lpm_snps_exit, disconnected, lpm_vote_by_sw %u, pwr_event_L2 %u, pwr_evt_P3 %u", 
        ctx->power_event, HSU_PWR_EVNT_OFFSET2, 
        ctx->is_lpm_voted_by_sw,
        ctx->is_pwr_event_phy_in_L2,
        ctx->is_pwr_event_phy_in_P3);

      //hsu_event_log(EVT_LPM_EXIT, USB3_CORE_INDEX);
    }

    // Register the HS-USB interrupt back.
    hsu_os_bus_interrupt_init(ctx->core_index);

	if (ctx->xo_voted == FALSE)
    {
      /* Vote against XO shutdown */
      npa_issue_required_request(ctx->xo_handle, 2);
      ctx->xo_voted = TRUE;
    }
  }

  lpm_info->is_in_low_power_mode = FALSE;
  lpm_info->was_lpm_exit_due_to_remote_wakeup = FALSE;

  rex_leave_crit_sect(ctx->crit_sec);
}


//==============================================================================
// FUNCTION hsu_lpm_snps_set_hs_phy_suspend_L2_transition
//==============================================================================
void hsu_lpm_snps_set_hs_phy_suspend_L2_transition(boolean set_enable)
{
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();

  if (HSU_PLATFORM_RUMI != platform_info->hsu_platform_id)
  {
    if (set_enable)
    {
      // Enable HS SUSPEND L2 transition only if it was disabled
      if (0 == HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_INMI(0, HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_SUSPENDUSB20_BMSK))
      {
        HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_OUTMI(0, HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_SUSPENDUSB20_BMSK, 1 << HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_SUSPENDUSB20_SHFT);
        HSU_ULOG(NO_MSG, BUS_LOG, "enable_L2_transition, suspendusb20 1");
      }
      if (0 == HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_INMI(0, HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_ENBLSLPM_BMSK))
      {
        HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_OUTMI(0, HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_ENBLSLPM_BMSK, 1 << HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_ENBLSLPM_SHFT);
        HSU_ULOG(NO_MSG, BUS_LOG, "enable_L2_transition, enblslpm 1");
        
      }
    }
    else
    {
      if (0 != HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_INMI(0, HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_ENBLSLPM_BMSK))
      {
        HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_OUTMI(0, HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_ENBLSLPM_BMSK, 0 << HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_ENBLSLPM_SHFT);
        HSU_ULOG(NO_MSG, BUS_LOG, "disable_L2_transition, enblslpm 0");

      }
      // Disable HS SUSPEND L2 transition only if it was enabled
      if (0 != HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_INMI(0, HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_SUSPENDUSB20_BMSK))
      {
        HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_OUTMI(0, HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_SUSPENDUSB20_BMSK, 0 << HWIO_GUSB2PHYCFG_REGS_p_GUSB2PHYCFG_SUSPENDUSB20_SHFT);
        HSU_ULOG(NO_MSG, BUS_LOG, "disable_L2_transition, suspendusb20 0");      
      }
    }
  }
}


//==========================================================================
// FUNCTION hsu_lpm_snps_is_remote_wakeup_allowed
//==============================================================================
boolean hsu_lpm_snps_is_remote_wakeup_allowed(void)
{
  boolean is_allowed;
  
  if (SPEED_SUPER == hsu_lpm_snps_get_curr_speed_from_hw())
  {
    is_allowed = TRUE;
  }
  else if (hsu_clkmgr_is_hs_only_mode())
  {    
    HSU_ULOG(HIGH_MSG, BUS_LOG, "hs_phy_only_allow_remotewakeup");
    is_allowed = TRUE;
  }
  else
  {
    is_allowed = (FALSE == hsu_lpm_snps_ctx.is_pwr_event_phy_in_L2);
  }

  return is_allowed;
}


//==============================================================================
//
// Local Function Definitions
//
//==============================================================================


//==============================================================================
// FUNCTION hsu_lpm_snps_hs_phy_enter_disconnect
//==============================================================================
static void hsu_lpm_snps_hs_phy_enter_disconnect(hsu_lpm_snps_ctx_type *ctx)
{
#if 0 // 9x65
  uint32 reg;
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();

  HSU_ULOG_ENUM(NO_MSG, BUS_LOG, "hsu_lpm_snps_hs_phy_enter_disconnect", ctx->power_event, HSU_PWR_EVNT_OFFSET2);

  reg = HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_IN;

  // Disable DP/DM interrupts.
  reg = HSU_VAR_OUTM(reg, 0x0, HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_DMSE_INTR_EN_BMSK, HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_DMSE_INTR_EN_SHFT);
  reg = HSU_VAR_OUTM(reg, 0x0, HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_DPSE_INTR_EN_BMSK, HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_DPSE_INTR_EN_SHFT);
  HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_OUT(reg);

  // Clamp dpsehv and dmsehv outputs to power collapse 1.8V LDO
  HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_OUTM(
    HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_CLAMP_N_EN_BMSK,
    0x0 << HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_CLAMP_N_EN_SHFT);

  switch (platform_info->hsu_platform_id)
  {
    case HSU_PLATFORM_VIRTIO:
    case HSU_PLATFORM_RUMI:
    {
      // Do nothing for pre-sil
    }
    break;
    default:
    {
      /* Keep the PHY in power down mode */
      HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_OUTM(HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_POWER_DOWN_BMSK,
          (0x1 << HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_POWER_DOWN_SHFT));
    }
    break;
  }

  ctx->is_hs_phy_in_suspend = TRUE;
#endif
}

#define  FEATURE_HS_USB_QUSB2_PHY

//==============================================================================
// FUNCTION hsu_lpm_snps_hs_phy_enter_suspend
//==============================================================================
void hsu_lpm_snps_hs_phy_enter_suspend(hsu_lpm_snps_ctx_type *ctx)
{
#if 0 // 9x65
  if (ctx->is_hs_phy_in_suspend)
  {
    HSU_ULOG(NO_MSG, BUS_LOG, "filtered");
  }
  else
  {
    uint32 reg;
    ctx->is_hs_phy_in_suspend = TRUE;
    
    HSU_ULOG_ENUM(NO_MSG, BUS_LOG, "hsu_lpm_snps_hs_phy_enter_suspend", ctx->power_event, HSU_PWR_EVNT_OFFSET2);

    reg = HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_IN;
    // Enable DP Interrupt
    // Detection of fall edge (suspend -> reset) or (suspend -> resume)
    reg = HSU_VAR_OUTM(reg, 0x0, HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_DMSE_INTR_EN_BMSK, HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_DMSE_INTR_EN_SHFT);
    reg = HSU_VAR_OUTM(reg, 0x1, HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_DPSE_INTR_EN_BMSK, HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_DPSE_INTR_EN_SHFT);
    reg = HSU_VAR_OUTM(reg, 0x0, HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_DPSE_INTR_HIGH_SEL_BMSK, HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_DPSE_INTR_HIGH_SEL_SHFT);
    HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_OUT(reg);

    // HV interrupt is for non-XO shutdown case but enabled anyways as we do not know whether system will enter XO shutdown or not
    HWIO_ALT_INTERRUPT_EN_OUTM(
      HWIO_ALT_INTERRUPT_EN_LINESTATE_INTEN_BMSK,
      0x1 << HWIO_ALT_INTERRUPT_EN_LINESTATE_INTEN_SHFT);

    // HS1 interrupt to MPM requires non-clamp mode
    HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_OUTM(
      HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_CLAMP_N_EN_BMSK,
      0x1 << HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_CLAMP_N_EN_SHFT);
  }
#endif
}

//==============================================================================
// FUNCTION hsu_lpm_snps_hs_phy_exit_suspend
//==============================================================================
void hsu_lpm_snps_hs_phy_exit_suspend(hsu_lpm_snps_ctx_type *ctx)
{
#if 0 // 9x65
  if (FALSE == ctx->is_hs_phy_in_suspend)
  {
    HSU_ULOG(NO_MSG, BUS_LOG, "filtered");
    
  }
  else
  {
    uint32 reg;
    ctx->is_hs_phy_in_suspend = FALSE;
    HSU_ULOG_ENUM(NO_MSG, BUS_LOG, "hsu_lpm_snps_hs_phy_exit_suspend", ctx->power_event, HSU_PWR_EVNT_OFFSET2);

    // HPG Section 4.4.6.  
    // HS PHY Exiting Low Power Mode after cable is connected
    // Disable HV Interrupts
    HWIO_ALT_INTERRUPT_EN_OUTM(
      HWIO_ALT_INTERRUPT_EN_LINESTATE_INTEN_BMSK,
      0x0 << HWIO_ALT_INTERRUPT_EN_LINESTATE_INTEN_SHFT);

    // Used to clamp_n dpsehv and dmsehv outputs to zero during the time that vdd1p8 is collapsed.
    // Set to 1: after vdd1p8 comes back up.
    HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_OUTM(
      HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_CLAMP_N_EN_BMSK,
      0x1 << HWIO_QUSB2PHY_QUSB2PHY_PORT_POWERDOWN_CLAMP_N_EN_SHFT);

    reg = HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_IN;
    // Disable DP/DM interrupts.
    reg = HSU_VAR_OUTM(reg, 0x0, HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_DMSE_INTR_EN_BMSK, HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_DMSE_INTR_EN_SHFT);
    reg = HSU_VAR_OUTM(reg, 0x0, HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_DPSE_INTR_EN_BMSK, HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_DPSE_INTR_EN_SHFT);
    HWIO_QUSB2PHY_QUSB2PHY_PORT_INTR_CTRL_OUT(reg);
  }
#endif // 9x65
}

//==============================================================================
// FUNCTION hsu_lpm_snps_get_curr_speed_from_hw
//==============================================================================
device_speed_t hsu_lpm_snps_get_curr_speed_from_hw(void)
{
  uint32         hw_speed;
  device_speed_t sw_speed = SPEED_UNKNOWN;

  static const device_speed_t hw2soft_speed[] =
    {SPEED_HIGH, SPEED_FULL, SPEED_LOW, SPEED_FULL, SPEED_SUPER};

  /* Read connection speed, and store it */
  hw_speed = HWIO_DSTS_INM(HWIO_DSTS_CONNECTSPD_BMSK) >> HWIO_DSTS_CONNECTSPD_SHFT;

  // Check for index out of change.
  if ( hw_speed >= (sizeof(hw2soft_speed) / sizeof(device_speed_t)) )
  {
    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "DCD, connection_speed_is_out_of_bound %u", hw_speed);
  }
  else
  {
    sw_speed = hw2soft_speed[hw_speed];
  }
  
  return sw_speed;
}


/*===========================================================================
FUNCTION hsu_lpm_is_ldo_power_collapse_enabled
===========================================================================*/
boolean hsu_lpm_is_ldo_power_collapse_enabled(void)
{
  //return hsu_lpm_snps_ctx.ldo_power_collapse_enabled;
  return TRUE;
}

