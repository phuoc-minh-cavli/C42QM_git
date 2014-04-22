#ifndef _HSU_CORE_CONFIG_INT_H_
#define _HSU_CORE_CONFIG_INT_H_

/*==============================================================================

     High Speed USB core configuration Module
        I N T E R N A L  H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    core definitions used by the various HS-USB files. 

  EXTERNALIZED FUNCTIONS
    None.

  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.
    
  Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_core_config_int.h#1 $
  $DateTime: 2020/01/30 22:49:35 $

    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  07/17/08  me      Initial version

==============================================================================*/
#include "hsu_core_config.h"
#include "hsu_common_int.h"

/*==============================================================================
Forward Declarations
==============================================================================*/

/*==============================================================================
Constants
==============================================================================*/

/*==============================================================================
Macros
==============================================================================*/
#define HSU_CORE_IDX_PRIMARY hsu_core_config_get_primary_core_idx()
#define HSU_CORE_IDX_SECONDARY hsu_core_config_get_secondary_core_idx()

/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_GET_PRIMARY_CORE_IDX

DESCRIPTION
This function returns the HAL core index of the core that is currently active.

DEPENDENCIES
None.

RETURN VALUE
The primary core index.

SIDE EFFECTS
None. 

==============================================================================*/
uint8 hsu_core_config_get_primary_core_idx(void);

/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_GET_SECONDARY_CORE_IDX

DESCRIPTION
This function returns the HAL core index of the secondary core.

DEPENDENCIES
None.

RETURN VALUE
The secondary core index.

SIDE EFFECTS
None. 

==============================================================================*/
uint8 hsu_core_config_get_secondary_core_idx(void);

#endif /* _HSU_CORE_CONFIG_INT_H_ */
