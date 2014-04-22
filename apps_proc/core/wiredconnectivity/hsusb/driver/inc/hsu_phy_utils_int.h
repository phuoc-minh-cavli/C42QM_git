#ifndef HSU_PHY_UTILS_INT_H
#define HSU_PHY_UTILS_INT_H

/*==============================================================================

H S U _ P H Y _ U T I L S _ I N T . H


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
hsu_phy_utils_save_known_cal_value
hsu_phy_utils_restore_known_cal_value
hsu_phy_utils_spoof_connect_core
hsu_phy_utils_spoof_disconnect_core
hsu_phy_utils_enable_se1_gating
hsu_phy_utils_disable_BAM
hsu_phy_utils_enable_BAM

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

Copyright (c) 2008, 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/


/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_phy_utils_int.h#1 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
03/27/08  esh     Initial version

==============================================================================*/


/*==============================================================================

INCLUDE FILES

==============================================================================*/
#include <comdef.h>
#include "hsu_phy_utils.h"
/*==============================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/


/*==============================================================================
Typedefs
==============================================================================*/
typedef union
{
  uint32 value;
  struct {
  /* HSUSB PHY configuration parameters */
  uint8   pre_emphasis_enable;
  /* Pre Emphasis Enable */
  uint8   pre_emphasis_adjustment;
  /* Pre Emphasis Pulse adjustment */
  uint8   pre_emphasis_amp_enable;
  /* Pre Emphasis Amplitude Enable */
  uint8   pre_emphasis_amp_adjustment; 
  /* Pre Emphasis Amplitude Adjustment */
  } reg;
} hsu_phy_utils_config_par_type;
typedef union
{
  uint32 value;
  struct {
  /* HSUSB PHY configuration parameters2 */
  uint8   qusb2phy_port_test2;    //LSB
  uint8   qusb2phy_pll_user_ctl2;
  uint8   qusb2phy_pll_user_ctl1;
  uint8   qusb2phy_pll;           //MSB
  } reg;
} hsu_phy_utils_config_pll;

/*==============================================================================

  Constants and Macros
  
==============================================================================*/
#define HSU_PHY_VAL_SET                   (0xFF)
#define HSU_PHY_VAL_CLR                   (0x00)

/* Function Control : 45nm / 65nm / 180nm
--------------------------------------------------------------------------------
|r-0b0    |r-0b1    |rw-0b0 |rw-0b01 |rw-0b0     |rw-0b01
|reserved |suspendM |reset  |opmode  |termselect |xcvrselect
------------------------------------------------------------------------------*/
#define HSU_PHY_FNCTL_ADDR                (0x04)
#define HSU_PHY_FNCTL_DFLT_VAL            (0x49)

/* 0-low power operation, 1-normal power. (default) */
#define HSU_PHY_FNCTL_SUSPEND_M_SHFT      (6)
#define HSU_PHY_FNCTL_SUSPEND_M_BMSK      (0x1 << HSU_PHY_FNCTL_SUSPEND_M_SHFT)

/* 0-inactive state. (default), 1-executes a reset operation. */
#define HSU_PHY_FNCTL_RESET_SHFT          (5)
#define HSU_PHY_FNCTL_RESET_BMSK          (0x1 << HSU_PHY_FNCTL_RESET_SHFT)

/* Mode of operation:
 * 00-normal drive.
 * 01-non-drive.
 * 10-disable bit-stuff and NRZI encoding.
 * 11-no SYNC and EOP generation. */
#define HSU_PHY_FNCTL_OPMODE_SHFT         (3)
#define HSU_PHY_FNCTL_OPMODE_BMSK         (0x3 << HSU_PHY_FNCTL_OPMODE_SHFT)
#define HSU_PHY_FNCTL_OPMODE_VAL_NORMAL   (0x0 << HSU_PHY_FNCTL_OPMODE_SHFT)
#define HSU_PHY_FNCTL_OPMODE_VAL_NONDRIV  (0x1 << HSU_PHY_FNCTL_OPMODE_SHFT)
#define HSU_PHY_FNCTL_OPMODE_VAL_NRZI     (0x2 << HSU_PHY_FNCTL_OPMODE_SHFT)
#define HSU_PHY_FNCTL_OPMODE_VAL_NOSYNC   (0x3 << HSU_PHY_FNCTL_OPMODE_SHFT)

/* 0-enable HS termination. (default), 1-enable FS termination. */
#define HSU_PHY_FNCTL_TERMSEL_SHFT        (2)
#define HSU_PHY_FNCTL_TERMSEL_BMSK        (0x1 << HSU_PHY_FNCTL_TERMSEL_SHFT)

/* Selects the required transceiver speed.
 * 00-enable HS transceiver.
 * 01-enable FS transceiver. (default)
 * 10-enable LS transceiver.
 * 11-enable FS transceiver for LS packets. */
#define HSU_PHY_FNCTL_XCVR_SHFT           (0)
#define HSU_PHY_FNCTL_XCVR_BMSK           (0x3 << HSU_PHY_FNCTL_XCVR_SHFT)
#define HSU_PHY_FNCTL_XCVR_VAL_ENA_HS     (0x0 << HSU_PHY_FNCTL_XCVR_SHFT)
#define HSU_PHY_FNCTL_XCVR_VAL_ENA_FS     (0x1 << HSU_PHY_FNCTL_XCVR_SHFT)
#define HSU_PHY_FNCTL_XCVR_VAL_ENA_LS     (0x2 << HSU_PHY_FNCTL_XCVR_SHFT)
#define HSU_PHY_FNCTL_XCVR_VAL_ENA_FS_LS  (0x3 << HSU_PHY_FNCTL_XCVR_SHFT)

/* Charger Detection : 45nm
--------------------------------------------------------------------------------
|r-0b00   |r-0b0 |r-0b0      |rw-0b1      |rw-0b1   |rw-0b1     |rw-0b1
|reserved |dcd   |chrgr_vout |extchgrctrl |chgrmode |chgrdetonM |chgrdetenM
------------------------------------------------------------------------------*/
#define HSU_PHY_CHGRDET_ADDR              (0x34)
#define HSU_PHY_CHGRDET_DFLT_VAL          (0x0F)

/* 0-disable (default), 1-enable */
#define HSU_PHY_CHGRDET_DCD_SHFT          (5)
#define HSU_PHY_CHGRDET_DCD_BMSK          (0x1 << HSU_PHY_CHGRDET_DCD_SHFT)

/* 0-host (default), 1-charger RO */
#define HSU_PHY_CHGRDET_VOUT_SHFT         (4)
#define HSU_PHY_CHGRDET_VOUT_BMSK         (0x1 << HSU_PHY_CHGRDET_VOUT_SHFT)

/* 0-ulpi, 1-ext (default) */
#define HSU_PHY_CHGRDET_EXTCTRL_SHFT      (3)
#define HSU_PHY_CHGRDET_EXTCTRL_BMSK      (0x1 << HSU_PHY_CHGRDET_EXTCTRL_SHFT)

/* 0-host, 1-device (default) */
#define HSU_PHY_CHGRDET_MODE_DEV_SHFT     (2)
#define HSU_PHY_CHGRDET_MODE_DEV_BMSK     (0x1 << HSU_PHY_CHGRDET_MODE_DEV_SHFT)

/* 0-on, 1-off (default) */
#define HSU_PHY_CHGRDET_DETON_M_SHFT      (1)
#define HSU_PHY_CHGRDET_DETON_M_BMSK      (0x1 << HSU_PHY_CHGRDET_DETON_M_SHFT)

/* 0-enable, 1-disable (default) */
#define HSU_PHY_CHGRDET_DETEN_M_SHFT      (0)
#define HSU_PHY_CHGRDET_DETEN_M_BMSK      (0x1 << HSU_PHY_CHGRDET_DETEN_M_SHFT)

/* PHY configuration register debug */
#define HSU_PHY_DEBUG_ADDR                0x15
#define HSU_PHY_DEBUG_CALIB_STATUS_MSK    0x80
#define HSU_PHY_DEBUG_CALIB_VALUE_MSK     0x7C

/* PHY configureation external calibration */
#define HSU_PHY_EXTERNAL_CALIB_ADDR            0x35
#define HSU_PHY_EXTERNAL_CALIB_EFUSESSEL_MSK   0x80
#define HSU_PHY_EXTERNAL_CALIB_BYPASS_MSK      0x01
#define HSU_PHY_EXTERNAL_CALIB_VALUE_SHFT      (1)

/* PHY configuration register 1/4 */
#define HSU_PHY_CFGREG1_ADDR 0x30

/* PHY configuration register 2/4 */
#define HSU_PHY_CFGREG2_ADDR 0x31
#define HSU_PHY_CFGREG2_HSDRVAMPLITUDE_MASK 0x0c
#define HSU_PHY_CFGREG2_HSDRVAMPLITUDE_POS 2

/* PHY configuration register 3/4 */
#define HSU_PHY_CFGREG3_ADDR 0x32
#define HSU_PHY_CFGREG3_ENPRE_MASK 0x20
#define HSU_PHY_CFGREG3_ENPRE_POS 5
#define HSU_PHY_CFGREG3_PREEMDEPTH_MASK 0x10
#define HSU_PHY_CFGREG3_PREEMDEPTH_POS 4
#define HSU_PHY_CFGREG3_HSDRVSLOPE_MASK 0x0f
#define HSU_PHY_CFGREG3_HSDRVSLOPE_POS 0

/* PHY configuration register 4/4 */
#define HSU_PHY_CFGREG4_ADDR 0x33

/* PHY gpdigout register */
#define HSU_PHY_GPDIGOUT_ADDR 0x36

#define HSU_PHY_GPDIGOUT_SE1GATING_MASK 0x04
#define HSU_PHY_GPDIGOUT_SE1GATING_POS  2

/* 
0x96 MISC A
Type: Read/Write, 0x97 (Set), 0x98 (Clear)
Reset State: 0x00
*/
#define HSU_PHY_MISC_A_ADDR               (0x96)
#define HSU_PHY_MISC_A_SET_ADDR           (0x97)
#define HSU_PHY_MISC_A_CLEAR_ADDR         (0x98)
#define HSU_PHY_MISC_A_VBUSVLDEXTSEL_SHFT (1)
#define HSU_PHY_MISC_A_VBUSVLDEXTSEL_MSK  (0x1 << HSU_PHY_MISC_A_VBUSVLDEXTSEL_SHFT)
#define HSU_PHY_MISC_A_VBUSVLDEXT_SHFT    (0)
#define HSU_PHY_MISC_A_VBUSVLDEXT_MSK     (0x1 << HSU_PHY_MISC_A_VBUSVLDEXT_SHFT)

/*==============================================================================

  PHY Calibration Registers for MDM9x15
  
==============================================================================*/
#define HSU_PHY_PARAMETER_OVERRIDE_A_ADDR  0x80
#define HSU_PHY_PARAMETER_OVERRIDE_B_ADDR  0x81
#define HSU_PHY_PARAMETER_OVERRIDE_C_ADDR  0x82
#define HSU_PHY_PARAMETER_OVERRIDE_D_ADDR  0x83

#define DISCONNECT_THRESHOLD_POS	4
#define DISCONNECT_THRESHOLD_MASK	0x70
#define VBUS_VALID_THRESHOLD_POS	0
#define VBUS_VALID_THRESHOLD_MASK	0x07

#define SQUELCH_THRESHOLD_POS		4
#define SQUELCH_THRESHOLD_MASK		0x70
#define	HS_DC_VOLTAGE_POS			0
#define HS_DC_VOLTAGE_MASK			0x0F

#define PRE_EMPHASIS_POS			6
#define PRE_EMPHASIS_MASK			0x40
#define PRE_EMPHASIS_AMPLITUDE_POS	4
#define PRE_EMPHASIS_AMPLITUDE_MASK	0x30
#define RISE_FALL_POS				2
#define RISE_FALL_MASK				0x0C
#define CROSSOVER_POS				0
#define CROSSOVER_MASK				0x03

#define SOURCE_IMPEDANCE_POS		4
#define SOURCE_IMPEDANCE_MASK		0x30
#define FS_LS_SOURCE_IMPEDANCE_POS 	0
#define FS_LS_SOURCE_IMPEDANCE_MASK	0x0F

/*==============================================================================

PUBLIC FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
FUNCTION HSU_PHY_UTILS_REG_READ

DESCRIPTION
The function reads a PHY register with address as parameter.

DEPENDENCIES
Only supports HSU_CORE_IDX_PRIMARY.

RETURN VALUE
TRUE for successful operation, FALSE otherwise.

SIDE EFFECTS
None.
==============================================================================*/
boolean hsu_phy_utils_reg_read(uint8 reg_addr, uint8 *val_ptr);

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
boolean hsu_phy_utils_reg_read_bmsk(uint8 reg_addr, uint8 bmsk, uint8 *val_ptr);

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
boolean hsu_phy_utils_reg_write(uint8 reg_addr, uint8 val);

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
boolean hsu_phy_utils_reg_write_bmsk(uint8 reg_addr, uint8 bmsk, uint8 val);

/*===========================================================================
FUNCTION HSU_PHY_UTILS_ENABLE_LPM_EXIT

DESCRIPTION
This function lets the HS-USB PHY start exiting from LPM when USB resume or 
reset events occur during TCXO shutdown, even before the HS-USB interrupt
is invoked.

This function comes to resolve issues with violating the USB specification
because sometimes it take a while until the phone wakes up from the shutdown 
state.

This function first checks whether the HS-USB AHB bus clock is enabled. If it
is not enabled, it means that the HS-USB port is not connected, and therefore
USB resume or reset events are not expected. Otherwise, the STP_CTRL bit in the
USBCMD register is cleared, letting any pending PHY LPM exit start executing.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_phy_utils_enable_lpm_exit(uint32 core_index);

/*===========================================================================
FUNCTION HSU_PHY_UTILS_DISABLE_LPM_EXIT

DESCRIPTION
This function disables undesirable PHY LPM exits during TCXO shutdown state.

This function comes to revert the actions taken by the 
hsu_phy_utils_enable_lpm_exit() when there was TCXO shutdown mode exit which 
was not triggered by a USB event.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_phy_utils_disable_lpm_exit(uint32 core_index);


/*==============================================================================
FUNCTION HSU_PHY_UTILS_CONFIG_PHY_CDR_AUTO_RESET

DESCRIPTION 
  Enables/Disables the integrated PHY CDR Auto Reset feature.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Successfully enabled/disabled CDR Auto Reset
  FALSE: CDR Auto Reset configuration failed

SIDE EFFECTS
  None
==============================================================================*/
boolean hsu_phy_utils_config_phy_cdr_auto_reset(boolean enable);


/*==============================================================================
FUNCTION HSU_PHY_UTILS_ENABLE_SE1_GATING

DESCRIPTION 
  Enables/Disables SE1 gating feature on PHY.  When enabled, the differential 
  receiver data will be ignored during SE1 state.  The HW default SE1 gating 
  is enabled.  SE1 gating bit is active low in the PHY register.  
  This SE1 gating should be enabled before charger detection to avoid 
  PHY lockup during SE1 condition.  After charger detection, 
  this SE1 gating should be disabled.  

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None
==============================================================================*/
void hsu_phy_utils_enable_se1_gating(boolean enable);

/*===========================================================================
FUNCTION HSU_PHY_UTILS_CONFIG_PHY_AUTO_RESUME

DESCRIPTION
  Enable\Disable the HS-USB PHY auto-resume feature.

DEPENDENCIES
  Applies only to targets with HS-USB 45nm revision C integrated PHY and above.

RETURN VALUE
  TRUE on success.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hsu_phy_utils_config_phy_auto_resume(boolean enable);

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
void hsu_phy_utils_set_config_param(uint8 core_id, boolean use_nv);


/*==============================================================================
FUNCTION HSU_PHY_UTILS_REPORT_CONFIG_SETTING  

DESCRIPTION 
 send F3 message having PHY configuration settings

DEPENDENCIES
 None

RETURN VALUE
 None

SIDE EFFECTS
 None
==============================================================================*/
void hsu_phy_utils_report_config_setting(uint8 core_id);

#if !defined(BUILD_HOSTDL)  || defined(BUILD_EHOSTDL)
/*==============================================================================

FUNCTION          HSU_PHY_UTILS_DELAY_MS

DESCRIPTION
Delays execution for the given time in milliseconds.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_phy_utils_delay_ms(uint32 milliseconds);
#endif /* !BUILD_HOSTDL || BUILD_EHOSTDL */

#ifndef T_REXNT
/*===========================================================================

FUNCTION      hsu_phy_utils_save_known_cal_value

DESCRIPTION   
Store last known calibration value obtained from the PHY

DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_phy_utils_save_known_cal_value(void);

/*===========================================================================

FUNCTION      hsu_phy_utils_restore_known_cal_value

DESCRIPTION   
Restores last known calibration value to the PHY

DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_phy_utils_restore_known_cal_value(void);


/*===========================================================================
FUNCTION    hsu_phy_utils_spoof_disconnect_core

DESCRIPTION   
This function causes the device to disconnect from host via specific core, 
while USB cable can still be attached.

DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_phy_utils_spoof_disconnect_core(uint32 core_index);

/*===========================================================================
FUNCTION hsu_phy_utils_spoof_connect_core

DESCRIPTION   
This function causes the device to connect to the host via specific core, 
after calling the hsu_phy_utils_spoof_disconnect_core() function.

DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_phy_utils_spoof_connect_core(uint32 core_index);

#endif /* !T_REXNT */

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
  uint8 vbus_valid_threshold);

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
  uint8 hs_dc_volt);
  
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
  uint8 crossover);
  
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
  uint8 ls_fs_src_z);
 
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
void hsu_phy_utils_set_default_override(uint8 core_id);

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
void hsu_phy_utils_disable_BAM(void);

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
void hsu_phy_utils_enable_BAM(void);

#endif /* HSU_PHY_UTILS_INT_H */
