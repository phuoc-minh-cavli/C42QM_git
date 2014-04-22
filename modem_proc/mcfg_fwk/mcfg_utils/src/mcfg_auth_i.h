#ifndef MCFG_AUTH_I_H
#define MCFG_AUTH_I_H
/*==============================================================================

                M O D E M   C O N F I G   A U T H
 
GENERAL DESCRIPTION
  Internal header for MCFG AUTH library

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/mcfg_utils/src/mcfg_auth_i.h#1 $ 
$DateTime: 2020/02/19 23:07:19 $ 
$Author: pwbldsvc $ 
$Change: 22644646 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
05/08/12 cy   Initial creation

==============================================================================*/

#include "comdef.h"

#include "msg.h"

#include "mcfg_common.h"
#include "mcfg_utils.h"

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

  FUNCTION mcfg_auth_check_config

  DESCRIPTION
    This function authenticates the config file

  DEPENDENCIES
    None

  PARAMETERS
      config [in] config structure including config size and buffer pointer to be authenticated
    
  RETURN VALUE
    mcfg_error_e_type

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_auth_check_config
(
  mcfg_config_s_type *config
);

/*===========================================================================

  FUNCTION mcfg_auth_init

  DESCRIPTION
    Initialize mcfg auth

  DEPENDENCIES
    None

  PARAMETERS
    None
    
  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_auth_init
(
  void
);

#endif /* MCFG_AUTH_I_H */
