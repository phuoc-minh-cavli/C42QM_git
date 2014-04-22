#ifndef MCFG_REMOTE_H
#define MCFG_REMOTE_H
/*==============================================================================

                M O D E M   C O N F I G   E F S
 
GENERAL DESCRIPTION
  Internal header for MCFG EFS Storage

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/mcfg_utils/inc/mcfg_remote.h#1 $ 
$DateTime: 2020/02/19 23:07:19 $ 
$Author: pwbldsvc $ 
$Change: 22644646 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
05/08/12 cy   Initial creation

==============================================================================*/

#include "mcfg_common.h"
#include "mcfg_utils.h"

/*==============================================================================

                PRIVATE DEFINITIONS AND DECLARATIONS FOR MODULE

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
 
                    PRIVATE FUNCTION DECLARATIONS FOR MODULE
 
==============================================================================*/

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/
#define MCFG_RFS_PATH_DISCOVER       "/readonly/firmware/image/modem_pr/mcfg/configs"
#define MCFG_DISCOVER_TIMER_TIMEOUT  (10000) // 10k ms (10 seconds)

/*==============================================================================
  Variables
==============================================================================*/


/*============================================================================== 
 
                    PUBLIC FUNCTION DECLARATIONS FOR MODULE
 
==============================================================================*/
#ifdef FEATURE_MCFG_REMOTE_STORAGE
boolean mcfg_remote_put_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_config_s_type     *config
);

boolean mcfg_remote_discover
(
  const char* filepath
);
#endif
#endif /* MCFG_REMOTE_H */
