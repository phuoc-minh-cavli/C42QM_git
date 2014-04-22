/*==============================================================================

                            HS-USB Core configuration

GENERAL DESCRIPTION
    Contains the core configuration for HS-USB for MDM9x55.

EXTERNALIZED FUNCTIONS

INITALIZATION AND SEQUENCING REQUIREMENTS
    None

Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/core_config/9x65/hsu_core_config_target.c#1 $
  $DateTime: 2020/01/30 22:49:35 $


when     who       what, where, why
-------- --------  ----------------------------------------------------
04/01/11 jc        Initial version

==============================================================================*/

/*==============================================================================
INCLUDE FILES FOR MODULE
==============================================================================*/
#include "hsu_core_config_int.h"

/*==============================================================================
LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
==============================================================================*/
/*==============================================================================
Constants
==============================================================================*/
/*==============================================================================
Macros
==============================================================================*/
/*==============================================================================
Typedefs
==============================================================================*/
/*==============================================================================
Forward Declarations
==============================================================================*/
extern hsu_core_config_type cores_details[HSU_NUM_OF_CORES_TYPES]; 

/*==============================================================================
Variables
==============================================================================*/

/* A table that holds the cores that are relevant for this target
in all its possible configurations */
hsu_core_config_type* hsu_core_config_table[]=
{
  /* SNPS USB3.0 core (Virtio) */
#if (defined (FEATURE_HS_USB_SNPS_CORE))
  &cores_details[HSU_USB3_SNPS_CORE],
#else 
  NULL,   /* No USB3.0 core */
#endif /* FEATURE_HS_USB_SNPS_CORE */
};

/*==============================================================================
EXTERNAL FUNCTION DEFINITIONS FOR MODULE
==============================================================================*/


/*==============================================================================

FUNCTION          HSU_CORE_CONFIG_GET_CORES_TABLE_SIZE

DESCRIPTION
This .
The table will be function returns the number of cores in the cores table (including the NULL cores).

DEPENDENCIES
None.

RETURN VALUE
The number of cores in the cores table.

SIDE EFFECTS
None. 

==============================================================================*/
uint8 hsu_core_config_get_cores_table_size()
{
  return sizeof(hsu_core_config_table)/sizeof(hsu_core_config_type*);
}
