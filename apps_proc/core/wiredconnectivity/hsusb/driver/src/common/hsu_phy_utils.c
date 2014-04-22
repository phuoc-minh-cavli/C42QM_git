/*==============================================================================

H S U _ P H Y _ U T I L S . C


GENERAL DESCRIPTION
High Speed USB general utility functions for PHY (external/internal), 
to be used whether or not the HS-USB stack is running on the processor invoking
these functions.

EXTERNALIZED FUNCTIONS
hsu_phy_utils_ahb_clock_is_on
hsu_phy_utils_enable_lpm_exit
hsu_phy_utils_disable_lpm_exit
hsu_phy_utils_reg_read
hsu_phy_utils_reg_read_bmsk
hsu_phy_utils_reg_write
hsu_phy_utils_reg_write_bmsk
hsu_phy_utils_config_phy_cdr_auto_reset
hsu_phy_utils_config_phy_auto_resume
hsu_phy_utils_init_hsic
hsu_phy_utils_start_hsic
hsu_phy_utils_spoof_disconnect_core
hsu_phy_utils_spoof_connect_core
hsu_phy_utils_enable_se1_gating

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

Copyright (c) 2008, 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/


/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_phy_utils.c#1 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
03/27/08  esh     Initial version

==============================================================================*/
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "customer.h"
#include "hsu_common_int.h"
#include "HAL_hsusb.h"
#include "hsu_core_config_int.h"
#include "hsu_phy_utils_int.h"
#include "hsu_conf_sel_nv.h"
#include "hsu_config_selector.h"

#include "jusb_core_dcd.h"

#if defined (FEATURE_HS_USB_OS_DETECTION)
#include "hsu_os_detection.h"
#endif /* FEATURE_HS_USB_OS_DETECTION */

#include "hsu_os_abstract_bus.h"

#ifdef FEATURE_HS_USB_HEALTH_MONITOR
#include "hsu_health_diag_init.h"
#endif /* FEATURE_HS_USB_HEALTH_MONITOR */

#include "DDIClock.h"
#include "hsu_log.h"

/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
#define HSU_ULPI_READ_MAX_ATTEMPTS 3
/*==============================================================================
Typedefs
==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/
/*==============================================================================
Externalized Variables
==============================================================================*/
/*==============================================================================
Local Function Definitions 
==============================================================================*/
/*==============================================================================
File Static Variables
==============================================================================*/
/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
#ifdef FEATURE_HS_USB_LINK_POWER_MANAGEMENT
boolean hsu_conf_sel_enable_link_pwr_mgmt(void);
#endif /* FEATURE_HS_USB_LINK_POWER_MANAGEMENT */

/*==============================================================================
FUNCTION HSU_PHY_UTILS_REG_READ

DESCRIPTION
The function reads a PHY register with address as paramter.

DEPENDENCIES
Only supports HSU_CORE_IDX_PRIMARY.

RETURN VALUE
TRUE for successful operation, FALSE otherwise.

SIDE EFFECTS
None.
==============================================================================*/
//TODO deprecated: replace by using hsu_os_bus_write_to_phy and related functions
boolean hsu_phy_utils_reg_read(uint8 reg_addr, uint8 *val_ptr)
{
  HAL_HSUSB_ErrorType hal_result;

  hal_result = HAL_hsusb_ReadUlpiPhy(
    HSU_CORE_IDX_PRIMARY, 0, reg_addr, val_ptr);

  if (hal_result != HAL_HSUSB_Success)
  {
    HSU_ERR_FATAL("hsu_phy_utils: Reading from PHY failed. Err %u", 
      hal_result, 0, 0);
    return FALSE;
  }

  return TRUE;
}


/*==============================================================================
FUNCTION HSU_PHY_UTILS_REG_READ_BMSK

DESCRIPTION
The function reads a PHY register with address and bit mask as paramters.

DEPENDENCIES
Only supports HSU_CORE_IDX_PRIMARY.

RETURN VALUE
TRUE for successful operation, FALSE otherwise.

SIDE EFFECTS
None.
==============================================================================*/
//TODO deprecated: replace by using hsu_os_bus_write_to_phy and related functions
boolean hsu_phy_utils_reg_read_bmsk(uint8 reg_addr, uint8 bmsk, uint8 *val_ptr)
{  
  HAL_HSUSB_ErrorType hal_result;
  uint8 reg_val = 0;

  hal_result = HAL_hsusb_ReadUlpiPhy(
    HSU_CORE_IDX_PRIMARY, 0, reg_addr, &reg_val);

  if (hal_result != HAL_HSUSB_Success)
  {
    HSU_ERR_FATAL("hsu_phy_utils: Reading from PHY failed. Err %u", 
      hal_result, 0, 0);
    return FALSE;
  }

  *val_ptr = (reg_val & bmsk);

  return TRUE;
}


/*==============================================================================
FUNCTION HSU_PHY_UTILS_REG_WRITE

DESCRIPTION
The function wrtie to a PHY register with address as paramter.

DEPENDENCIES
Only supports HSU_CORE_IDX_PRIMARY.

RETURN VALUE
TRUE for successful operation, FALSE otherwise.

SIDE EFFECTS
None.
==============================================================================*/
//TODO deprecated: replace by using hsu_os_bus_write_to_phy and related functions
boolean hsu_phy_utils_reg_write(uint8 reg_addr, uint8 val)
{  
  HAL_HSUSB_ErrorType hal_result;

  hal_result = HAL_hsusb_WriteUlpiPhy(
    HSU_CORE_IDX_PRIMARY, 0, reg_addr, val); 
  
  if (hal_result != HAL_HSUSB_Success)
  {
    HSU_ERR_FATAL("hsu_phy_utils: Writing to PHY failed. Err %u", 
      hal_result, 0, 0);
    return FALSE;
  }

  return TRUE;
}


/*==============================================================================
FUNCTION HSU_PHY_UTILS_REG_WRITE_BMSK

DESCRIPTION
The function wrtie to a PHY register with address and bit mask as paramters.

DEPENDENCIES
Only supports HSU_CORE_IDX_PRIMARY.

RETURN VALUE
TRUE for successful operation, FALSE otherwise.

SIDE EFFECTS
None.
==============================================================================*/
//TODO deprecated: replace by using hsu_os_bus_write_to_phy and related functions
boolean hsu_phy_utils_reg_write_bmsk(uint8 reg_addr, uint8 bmsk, uint8 val)
{
  uint8 reg_val = 0;

  if (hsu_phy_utils_reg_read(reg_addr, &reg_val) == FALSE)
  {
    return FALSE;
  }

  if (hsu_phy_utils_reg_write(reg_addr, (reg_val & ~(bmsk)) | (val & bmsk)))
  {
    return FALSE;
  }

  return FALSE;
}

/*==============================================================================
FUNCTION HSU_PHY_UTILS_SET_CONFIG_PARAM

DESCRIPTION 
 Configure PHY signal adjust parameters

DEPENDENCIES
 use_nv parameter can be TRUE only when the function is called in TMC task and 
 DIAG task.

RETURN VALUE
 None

SIDE EFFECTS
 None
==============================================================================*/
void hsu_phy_utils_set_config_param(uint8 core_id, boolean use_nv)
{
  static hsu_phy_utils_config_par_type conf_par;

  if (core_id == HSU_CORE_CONFIG_USB_IDX)
  {
    if(use_nv)
    {
      conf_par = hsu_conf_sel_get_phy_config_param();
    }

    // Check if valid value is returned
    if(conf_par.reg.pre_emphasis_adjustment || 
       conf_par.reg.pre_emphasis_amp_adjustment||
       conf_par.reg.pre_emphasis_amp_enable ||
       conf_par.reg.pre_emphasis_enable)
    {
      if(!conf_par.reg.pre_emphasis_enable)
      {
        /*Use the default value of pre_emphasis*/
        conf_par.reg.pre_emphasis_adjustment = HSU_PREEMP_ADJ_DEFAULT_VALUE;
      }
      if(!conf_par.reg.pre_emphasis_amp_enable)
      {
        conf_par.reg.pre_emphasis_amp_adjustment = HSU_PREEMP_AMP_DEFAULT_VALUE;
      }
      /*Set the Parameter_Override_A*/
      hsu_phy_utils_set_param_override_a(core_id,
          HSU_DISCONN_THRESHOLD_DEFAULT_VALUE,
          HSU_VBUS_VALID_THRESHOLD_DEFAULT_VALUE);

      /*Set the Parameter_Override_B*/
      hsu_phy_utils_set_param_override_b(core_id,
          HSU_SQUELCH_THRESHOLD_DEFAULT_VALUE,
          HSU_DC_VOLTAGE_DEFAULT_VALUE);

      /*Set the Parameter_Override_C*/
      hsu_phy_utils_set_param_override_c(core_id,
          conf_par.reg.pre_emphasis_adjustment,
          conf_par.reg.pre_emphasis_amp_adjustment,
          HSU_RISE_FALL_DEFAULT_VALUE,
          HSU_CROSSOVER_DEFAULT_VALUE);

      /*Set the Parameter_Override_D*/
      hsu_phy_utils_set_param_override_d(core_id,
          HSU_SOURCE_IMPEDANCE_DEFAULT_VALUE,
          HSU_FS_LS_SOURCE_IMPEDANCE_DEFAULT_VALUE);
    }
    else
    {
#if !defined(FEATURE_HS_USB_PRE_SILICON_BU)
      /*Set the Default Parameters */
      hsu_phy_utils_set_default_override(core_id);
#endif /* FEATURE_HS_USB_PRE_SILICON_BU */
    }
  }
}

/*==============================================================================
FUNCTION HSU_PHY_UTILS_SPOOF_DISCONNECT_CORE

DESCRIPTION
This function causes the device to disconnect from host via specific core, 
while USB cable can still be attached.

DEPENDENCIES
None.

RETURN VALUE
TRUE - Operation successful. FALSE - Otherwise.

SIDE EFFECTS
None.
==============================================================================*/
boolean hsu_phy_utils_spoof_disconnect_core(uint32 hal_core_index)
{

  uint32 hsu_core_index = hsu_hal_os_bus_core_id(hal_core_index);
  
#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  /* HS OTG core specific actions */
  if (HSU_CORE_IDX_PRIMARY == HSU_CORE_CONFIG_USB_IDX)
  {
  #if defined (FEATURE_HS_USB_OS_DETECTION)
    /* Cancel any pending OS detection timer */
    hsu_os_detection_clr_timer();
  #endif /* FEATURE_HS_USB_OS_DETECTION */
  }
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE */

  hsu_os_bus_lpm_disable(hsu_core_index);

  /* Stop the core */
  HAL_hsusb_ClearBits32((uint8)hal_core_index, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_RS_BMSK); 

#ifdef FEATURE_HS_USB_INTEGRATED_PHY
  {
    uint8 ulpi_func_ctl = 0;
    uint8 op_mode_mask = HSU_PHY_FNCTL_OPMODE_BMSK;

    if ( HAL_hsusb_ReadUlpiPhy((uint8)hal_core_index, 0, 
      HSU_PHY_FNCTL_ADDR, &ulpi_func_ctl)
      != HAL_HSUSB_Success )
    {
      hsu_os_bus_lpm_enable(hsu_core_index);
      return FALSE;
    }

    /* clear current OpMode bits */
    ulpi_func_ctl &= ~op_mode_mask;  

    /* set OpMode to Non-driving */
    ulpi_func_ctl |= HSU_PHY_FNCTL_OPMODE_VAL_NONDRIV; 

    /* SW workaround for intg. phy in which clearing the RS bit does not pull down D+   
    Write function control register: */
    if ( HAL_hsusb_WriteUlpiPhy((uint8)hal_core_index, 0, 
      HSU_PHY_FNCTL_ADDR, ulpi_func_ctl) 
      == HAL_HSUSB_Success)
    {
      /* Inform the core about the disconnect event. After clearing RS bit the 
         device stops getting interrupts. For Non-OTG stack, we need to inform 
         the core of the disconnection event explicitly. */

      /* Un-register the HS-USB interrupt handler to prevent interrupt generated
       * even when R/S bit is disable in Link */
      hsu_os_bus_interrupt_uninit(hsu_core_index);

      if(FALSE == hsu_conf_sel_is_usb_core_snps()) 
      {
        /* When calling stack level API, USB mutex should be locked */
        jsafe_enter();
//TODO: -- Akash --
#if 0 //Iw own't come here because it is SNPS core.
        if (handle_spoof_disconnect(hsu_core_index))
        {
          HSU_MSG_ERROR("hsu_phy_utils_spoof_disconnect_core - handle_spoof_disconnect failed");
        }
#endif
        jsafe_leave();
      }
      /* Register the HS-USB interrupt back. */
      hsu_os_bus_interrupt_init(hsu_core_index);

      hsu_os_bus_lpm_enable(hsu_core_index);
  return TRUE;
    }
  }

  hsu_os_bus_lpm_enable(hsu_core_index);

  HSU_MSG_ERROR("hsu_phy_utils_spoof_disconnect_core: couldn't write func ctl");

  return FALSE;

#else /* !FEATURE_HS_USB_INTEGRATED_PHY */

  return TRUE;

#endif /* FEATURE_HS_USB_INTEGRATED_PHY */
}  /* hsu_phy_utils_spoof_disconnect_core */


/*==============================================================================
FUNCTION HSU_PHY_UTILS_SPOOF_CONNECT_CORE

DESCRIPTION
This function causes the device to connect to the host via specific core, 
after calling the hsu_phy_utils_spoof_disconnect_core() function.

DEPENDENCIES
None.

RETURN VALUE
TRUE - Operation successful. FALSE - Otherwise.

SIDE EFFECTS
None.
==============================================================================*/
boolean hsu_phy_utils_spoof_connect_core(uint32 core_index)
{
  /* Run the core */
  HAL_hsusb_SetBits32((uint8)core_index, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_RS_BMSK); 

  return TRUE;
}  /* hsu_phy_utils_spoof_connect_core */


/*==============================================================================

FUNCTION HSU_PHY_SET_PARAM_OVERRIDE_A       

DESCRIPTION
  Adjust USB disconnect threshold and VBUS Valid Threshold

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None. 

==============================================================================*/
void hsu_phy_utils_set_param_override_a(
  uint8 core_id,
  uint8 disconn_threshold,
  uint8 vbus_valid_threshold)
{
  uint8 value = 0x0; 
  if (HAL_HSUSB_Success != HAL_hsusb_ReadUlpiPhy(
                             core_id, 
                             0, 
                             HSU_PHY_PARAMETER_OVERRIDE_A_ADDR, 
                             &value))
  {
    HSU_ERR_FATAL("Reading HSU_PHY_PARAMETER_OVERRIDE_A_ADDR failed", 0, 0, 0);
  }
  value &= ~(DISCONNECT_THRESHOLD_MASK | VBUS_VALID_THRESHOLD_MASK);
  value |= ((disconn_threshold << DISCONNECT_THRESHOLD_POS) & DISCONNECT_THRESHOLD_MASK)
          |((vbus_valid_threshold << VBUS_VALID_THRESHOLD_POS) & VBUS_VALID_THRESHOLD_MASK);
  
  if (HAL_HSUSB_Success != HAL_hsusb_WriteUlpiPhy(core_id, 0, 
                                                  HSU_PHY_PARAMETER_OVERRIDE_A_ADDR, 
                                                  value))
  {
    HSU_ERR_FATAL("Writing HSU_PHY_PARAMETER_OVERRIDE_A_ADDR failed", 0, 0, 0);
  }
  
}

/*==============================================================================

FUNCTION HSU_PHY_SET_PARAM_OVERRIDE_B       

DESCRIPTION
  Adjust USB Squelch threshold and HS DC Voltage

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None. 

==============================================================================*/
void hsu_phy_utils_set_param_override_b(
  uint8 core_id,
  uint8 squelch_threshold,
  uint8 hs_dc_volt)
{
 
  uint8 value = 0x0;
  if (HAL_HSUSB_Success != HAL_hsusb_ReadUlpiPhy(
                             core_id, 
                             0, 
                             HSU_PHY_PARAMETER_OVERRIDE_B_ADDR, 
                             &value))
  {
    HSU_ERR_FATAL("Reading HSU_PHY_PARAMETER_OVERRIDE_B_ADDR failed", 0, 0, 0);
  }
  value &= ~(SQUELCH_THRESHOLD_MASK | HS_DC_VOLTAGE_MASK);
  value |= ((squelch_threshold << SQUELCH_THRESHOLD_POS) & SQUELCH_THRESHOLD_MASK)
       |((hs_dc_volt << HS_DC_VOLTAGE_POS ) & HS_DC_VOLTAGE_MASK );
  
  if (HAL_HSUSB_Success != HAL_hsusb_WriteUlpiPhy(core_id, 0, 
                                                  HSU_PHY_PARAMETER_OVERRIDE_B_ADDR, 
                                                  value))
  {
    HSU_ERR_FATAL("Writing HSU_PHY_PARAMETER_OVERRIDE_B_ADDR failed", 0, 0, 0);
  }
}
 
 /*==============================================================================

FUNCTION HSU_PHY_SET_PARAM_OVERRIDE_C       

DESCRIPTION
  Adjust USB Pre-emphasis,Rise Fall and Crossover values

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None. 

==============================================================================*/
void hsu_phy_utils_set_param_override_c(
  uint8 core_id,
  uint8 pre_emp,
  uint8 pre_emp_amp,
  uint8 rise_fall,
  uint8 crossover)
{
  uint8 value = 0x0; 
  if (HAL_HSUSB_Success != HAL_hsusb_ReadUlpiPhy(
                             core_id, 
                             0, 
                             HSU_PHY_PARAMETER_OVERRIDE_C_ADDR, 
                             &value))
  {
    HSU_ERR_FATAL("Reading HSU_PHY_PARAMETER_OVERRIDE_C_ADDR failed", 0, 0, 0);
  }
  value &= ~(PRE_EMPHASIS_MASK | PRE_EMPHASIS_AMPLITUDE_MASK 
            |RISE_FALL_MASK|CROSSOVER_MASK);
      
  value |= ((pre_emp << PRE_EMPHASIS_POS) & PRE_EMPHASIS_MASK)
           |((pre_emp_amp << PRE_EMPHASIS_AMPLITUDE_POS) & PRE_EMPHASIS_AMPLITUDE_MASK)
       |((rise_fall << RISE_FALL_POS) & RISE_FALL_MASK)
       |((crossover << CROSSOVER_POS) & CROSSOVER_MASK);
       
  if (HAL_HSUSB_Success != HAL_hsusb_WriteUlpiPhy(core_id, 0, 
                                                  HSU_PHY_PARAMETER_OVERRIDE_C_ADDR, 
                                                  value))
  {
    HSU_ERR_FATAL("Writing HSU_PHY_PARAMETER_OVERRIDE_C_ADDR failed", 0, 0, 0);
  } 
}
 
  /*==============================================================================

FUNCTION HSU_PHY_SET_PARAM_OVERRIDE_D       

DESCRIPTION
  Adjust USB Source Impedance for HS,FS,LS devices

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None. 

==============================================================================*/
 void hsu_phy_utils_set_param_override_d(
  uint8 core_id,
  uint8 src_z,
  uint8 ls_fs_src_z)
{
  uint8 value = 0x0;
  if (HAL_HSUSB_Success != HAL_hsusb_ReadUlpiPhy(
                             core_id, 
                             0, 
                             HSU_PHY_PARAMETER_OVERRIDE_D_ADDR, 
                             &value))
  {
    HSU_ERR_FATAL("Reading HSU_PHY_PARAMETER_OVERRIDE_D_ADDR failed", 0, 0, 0);
  }
  value &= ~(SOURCE_IMPEDANCE_MASK | FS_LS_SOURCE_IMPEDANCE_MASK);
  value |= ((src_z << SOURCE_IMPEDANCE_POS) & SOURCE_IMPEDANCE_MASK)
           | ((ls_fs_src_z << FS_LS_SOURCE_IMPEDANCE_POS) & FS_LS_SOURCE_IMPEDANCE_MASK);
		   
  if (HAL_HSUSB_Success != HAL_hsusb_WriteUlpiPhy(core_id, 0, 
                                                  HSU_PHY_PARAMETER_OVERRIDE_D_ADDR, 
                                                  value))
  {
    HSU_ERR_FATAL("Wrinting HSU_PHY_PARAMETER_OVERRIDE_D_ADDR failed", 0, 0, 0);
  } 
}

 /*==============================================================================

FUNCTION HSU_PHY_SET_DEFAULT_OVERRIDE       

DESCRIPTION
  Adjust USB PHY Calibration parameters with default values

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None. 

==============================================================================*/
void hsu_phy_utils_set_default_override(uint8 core_id)
{
		/*Set the Parameter_Override_A - Default*/
  hsu_phy_utils_set_param_override_a(core_id,
    HSU_DISCONN_THRESHOLD_DEFAULT_VALUE,
	HSU_VBUS_VALID_THRESHOLD_DEFAULT_VALUE);
		
		/*Set the Parameter_Override_B - Default*/
  hsu_phy_utils_set_param_override_b(core_id,
	HSU_SQUELCH_THRESHOLD_DEFAULT_VALUE,
	HSU_DC_VOLTAGE_DEFAULT_VALUE);
		
		/*Set the Parameter_Override_C - Default*/
  hsu_phy_utils_set_param_override_c(core_id,
	HSU_PREEMP_ADJ_DEFAULT_VALUE,
	HSU_PREEMP_AMP_DEFAULT_VALUE,
	HSU_RISE_FALL_DEFAULT_VALUE,
	HSU_CROSSOVER_DEFAULT_VALUE);
		
		/*Set the Parameter_Override_D - Default */
  hsu_phy_utils_set_param_override_d(core_id,
	HSU_SOURCE_IMPEDANCE_DEFAULT_VALUE,
	HSU_FS_LS_SOURCE_IMPEDANCE_DEFAULT_VALUE);
}

#ifndef CI_MERGE
/*==============================================================================
FUNCTION HSU_PHY_UTILS_DISABLE_BAM

DESCRIPTION
write 1 to USB_BAM_DISABLE bit of USB_OTG_HS_GEN_CONFIG register

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
==============================================================================*/
void hsu_phy_utils_disable_BAM(void)
{
  HSU_ULOG(NO_MSG, BAM_LOG, "bam_diable_bit_to_1");
  HAL_hsusb_SetLegacyMode(HSU_CORE_IDX_PRIMARY, TRUE);   
}  /* hsu_phy_utils_disable_BAM */

/*==============================================================================
FUNCTION HSU_PHY_UTILS_ENABLE_BAM

DESCRIPTION
write 0 to USB_BAM_DISABLE bit of USB_OTG_HS_GEN_CONFIG register

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
==============================================================================*/
void hsu_phy_utils_enable_BAM(void)
{
  HSU_ULOG(NO_MSG, BAM_LOG, "bam_diable_bit_to_0");
  HAL_hsusb_SetLegacyMode(HSU_CORE_IDX_PRIMARY, FALSE);   
}  /* hsu_phy_utils_enable_BAM */

#endif
