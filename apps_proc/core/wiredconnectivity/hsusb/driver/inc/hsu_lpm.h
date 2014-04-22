#ifndef HSU_LPM_H
#define HSU_LPM_H
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
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_lpm.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  06/05/09  tk  Initial version

==============================================================================*/

/*==============================================================================
                            INCLUDE FILES FOR MODULE
==============================================================================*/
/* AMSS include files. */
#include "comdef.h"
//#include "rex.h"
#include "hsu_qurt_util.h"
#include "hsu_os_abstract_bus_ext_int.h"
#include "pm.h"
#include "hsu_clkmgr.h"
#include "npa.h"

/*==============================================================================
                    DEFINITIONS AND DECLARATIONS FOR MODULE
==============================================================================*/


/*==============================================================================
  Typedefs
==============================================================================*/
typedef struct {
  uint32 core_idx;
  boolean is_called_by_lpm_timer;
  boolean is_analog_audio_mode;
  boolean is_lpm_feature_active;
  boolean is_ext_ldo_installed;
  boolean is_pmic_comparator_used;
  boolean is_otg_enabled;
  boolean is_pmic_chg_pin_used;
  rex_crit_sect_type *crit_sec;
  hsu_lpm_info_type *lpm_info;
  boolean ldo_power_collapse_enabled;
  npa_client_handle xo_handle;
  boolean xo_voted;
} hsu_lpm_ctx_type;

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
                    FUNCTION DECLARATIONS
==============================================================================*/
/*===========================================================================
FUNCTION HSU_LPM_GET_CTX

DESCRIPTION
  get LPM context 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
hsu_lpm_ctx_type * hsu_lpm_get_ctx(void);

/*===========================================================================
FUNCTION HSU_LPM_INIT

DESCRIPTION
  It initializes context variable in HSU LPM module. 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_lpm_init(void);

/*===========================================================================
FUNCTION HSU_LPM_ENTER

DESCRIPTION
  The function is called from hsu_os_bus_abstract_bus.c when suspend and cable 
  is disconnected to make the PHY and HS USB link ready for low power mode 
  operation. 

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_lpm_enter(hsu_lpm_ctx_type *ctx);

/*===========================================================================
FUNCTION HSU_LPM_EXIT

DESCRIPTION
  The function is called from hsu_os_bus_abstract_bus.c when resume and cable 
  connect event occur. PHY and HS USB link will be ready to wake up from 
  suspend mode. 

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_lpm_exit(hsu_lpm_ctx_type *ctx);

/*===========================================================================
FUNCTION hsu_lpm_is_ldo_power_collapse_enabled

DESCRIPTION
 Returns boolean whether HS-USB LDO power collapse is enabled during 
 low power mode.

DEPENDENCIES
 None.

RETURN VALUE
 TRUE if LDO power collapse is enabled during low power Mode.
 FALSE otherwise.
 
SIDE EFFECTS
 None.
===========================================================================*/
boolean hsu_lpm_is_ldo_power_collapse_enabled(void);

/*===========================================================================
FUNCTION HSU_LPM_TURN_OFF_ALL_CLOCKS

DESCRIPTION
  Turn off all USB clocks, only if USB is in low power mode.
  One of the intentions for creating this is to be able to turn off
  USB BAM clocks when the bus is in suspend or disconnected

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_lpm_turn_off_all_clocks(void);

#endif
