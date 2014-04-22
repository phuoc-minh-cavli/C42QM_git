/**
 * @file smdl_profile_stub.c
 *
 * Stubbed SMD Lite profiling code for pack/strip builds
 */

/*--------------------------------------------------------------------------
     Copyright (c) 2010 Qualcomm Technologies Incorporated. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
----------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smdl_profile_stub.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/30/10   tl      Created stub
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "smdl_profile.h"

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/**
 * Initializes the SMD Lite profiling system.
 *
 * The profiling system will remain dormant until triggered by the debugger.
 *
 * @param[in]    reserved  Must be 0
 *
 * @return       @li 0 if the port is closed
 *               @li A negative error code if an error occurred.
 * @sideeffects  Creates a thread for SMD Lite profiling, which remains
 *               dormant until triggered by the debugger.
 */
int smdl_profile_init
(
  uint32 reserved
)
{
  /* Stubbed out */

  return 0;
}
