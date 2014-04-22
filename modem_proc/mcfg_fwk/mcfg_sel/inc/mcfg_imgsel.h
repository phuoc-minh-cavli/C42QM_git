#ifndef MCFG_IMGSEL_I_H
#define MCFG_IMGSEL_I_H
/*==============================================================================

          M O D E M   C O N F I G   I M A G E   S E L E C T I O N

GENERAL DESCRIPTION
  Internal header file for image selection.

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/mcfg_sel/inc/mcfg_imgsel.h#1 $
$DateTime: 2020/02/19 23:07:19 $
$Author: pwbldsvc $
$Change: 22644646 $

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
01/07/12 cy   Created module

==============================================================================*/

#include "mcfg_common.h"

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/


/*==============================================================================
  Typedefs
==============================================================================*/


/*==============================================================================
  Variables
==============================================================================*/


/*==============================================================================

                    PUBLIC FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*===========================================================================

  FUNCTION mcfg_imgsel_switch_images_by_config

  DESCRIPTION
    Switch to new images for boot to load based on the new active config. 

  DEPENDENCIES
    None

  PARAMETERS
    config_id [in]

  RETURN VALUE
    TRUE if image switched is necessary and successful,
    FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_imgsel_switch_images_by_config
(
  mcfg_config_id_s_type *config_id
);
#endif /* MCFG_IMGSEL_I_H */
