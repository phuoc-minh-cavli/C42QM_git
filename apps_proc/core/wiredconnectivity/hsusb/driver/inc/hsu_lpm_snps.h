#ifndef HSU_LPM_SNPS_H
#define HSU_LPM_SNPS_H
/*==============================================================================

  High Speed USB Low Power Management
  
  GENERAL DESCRIPTION
    This module defines APIs for HS USB low power management.

  EXTERNALIZED FUNCTIONS
    hsu_lpm_init
    hsu_lpm_enter
    hsu_lpm_exit

  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.

  Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_lpm_snps.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  06/05/09  tk  Initial version

==============================================================================*/

//==============================================================================
//                        INCLUDE FILES FOR MODULE
//==============================================================================
// AMSS include file
#include "customer.h"
#include "hsu_common.h"
#include "jusb_common.h"
#include "hsu_qurt_util.h"
#include "npa.h"

//==============================================================================
//                DEFINITIONS AND DECLARATIONS FOR MODULE
//==============================================================================


//==============================================================================
// Typedefs
//==============================================================================
typedef struct {
  uint32              core_index;
  boolean             is_lpm_feature_active;
  boolean             is_pwr_event_phy_in_L2;
  boolean             is_pwr_event_phy_in_P3;
  boolean             is_lpm_voted_by_sw;

  boolean             is_hs_phy_in_suspend;
  boolean             is_ss_phy_in_autonomous_mode;

  pwr_evnt_t          power_event;
  rex_crit_sect_type *crit_sec;
  hsu_lpm_info_type  *lpm_info;
  boolean             ldo_power_collapse_enabled;
  
  npa_client_handle   xo_handle;
  boolean             xo_voted;
} hsu_lpm_snps_ctx_type;

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
                    FUNCTION DECLARATIONS
==============================================================================*/

/*=============================================================================
FUNCTION    hsu_lpm_get_snps_ctx
=============================================================================*/
/** 
Returns SNPS USB 3.0 Low Power Mode context

@param None

@dependencies
hsu_lpm_snps_init() must be called prior.

@sideeffects
None.

@return
SNPS USB 3.0 low power mode context.
*/
hsu_lpm_snps_ctx_type *hsu_lpm_get_snps_ctx(void);

/*=============================================================================
FUNCTION    hsu_lpm_snps_init
=============================================================================*/
/** 
Performs any initialization required for SNPS USB 3.0 controller low power mode

@param None

@dependencies
None.

@sideeffects
Initializes SNPS USB 3.0 controller low power mode

@return
None.
*/
void hsu_lpm_snps_init(void);

/*=============================================================================
FUNCTION    hsu_lpm_snps_enter
=============================================================================*/
/** 
Puts USB 3.0 contoller into low power mode.

@param USB 3.0 low power mode context

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_lpm_snps_enter(hsu_lpm_snps_ctx_type *ctx);

/*=============================================================================
FUNCTION    hsu_lpm_snps_enter
=============================================================================*/
/** 
Brings USB 3.0 contoller out of low power mode.

@param USB 3.0 low power mode context

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_lpm_snps_exit(hsu_lpm_snps_ctx_type *ctx);

/*=============================================================================
FUNCTION    hsu_lpm_snps_set_hs_phy_suspend_L2_transition
=============================================================================*/
/** 
Enables/Disales Link Controler to automatically program USB2 PHY to enter L2 
state upon detecting high speed bus SUSPEND or whenever USB2 PHY is not used 
(i.e. Super Speed Mode).

              WARNING: When operating in High Speed mode L2 transition must
                       be disabled befoer writing to Link Controller 
                       Command registers

@param set_enable: (IN) boolean to enable L2 transition

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_lpm_snps_set_ss_phy_suspend_P3_transition(boolean set_enable);

/*=============================================================================
FUNCTION    hsu_lpm_snps_set_hs_phy_suspend_L2_transition
=============================================================================*/
/** 
Enables/Disables Link Controller to automatically program SS PHY to enter 
P3 state upon detecting super speed bus SUSPEND or whenever SS PHY is not 
used (i.e. High Speed Mode).

              WARNING: SS PHY should not be programmed to enter P3 state 
                       during HS/FS operation as it will never exit P3
                       afterwards causing next USB cable disconnect and 
                       re-connect to fail.  This is due to PIPE clock 
                       generated from SS PHY to Link not supplied during
                       SNPS Link Controller RESET.

@param set_enable: (IN) boolean to enable P3 transition

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_lpm_snps_set_hs_phy_suspend_L2_transition(boolean set_enable);

/*=============================================================================
FUNCTION    hsu_lpm_snps_is_remote_wakeup_allowed
=============================================================================*/
/** 
Checks the OUT_L2 (for HS) and OUT_P3 (for SS) to allow driving REMOTE WAKEUP

@param
None.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
boolean hsu_lpm_snps_is_remote_wakeup_allowed(void);

/*=============================================================================
FUNCTION    hsu_lpm_snps_get_curr_speed_from_hw
=============================================================================*/
/** 
Query and return hardware speed

@param
None.

@dependencies
None.

@sideeffects
None.

@return
device_speed_t
*/
device_speed_t hsu_lpm_snps_get_curr_speed_from_hw(void);
#endif
