/*===========================================================================

                    Q M I _ I D L _ S H I M _ K E R N E L . C
  
  Shim layer that provides QMI IDL apis to DAM user space

  ---------------------------------------------------------------------------
  Copyright (c) 2020 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/mproc/qmi/encdec/qmi_idl_shim_kernel_stubs.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

04/03/17   ab      File created.

=============================================================================*/
/*=============================================================================
                           INCLUDE FILES
=============================================================================*/

#include "qapi_txm_base.h"
#include "qurt_txm_qapi_fwk.h"
#include "qmi_txm_shim.h"
#include "qmi_idl_lib.h"
#include "qmi_idl_shim.h"

/*===========================================================================
                      MACROS AND DEFINES
===========================================================================*/

/*===========================================================================
                      GLOBAL VARIABLES
===========================================================================*/

/*===========================================================================
                         FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  qmi_idl_shim_init
  DESCRIPTION: Handler function for handling qmi idl function requests
  from user space tx dam applications
===========================================================================*/
UINT qmi_idl_shim_kernel_handler
(
   UINT request_ID, UINT a, UINT b, UINT c, 
   UINT d, UINT e, UINT f, UINT g, UINT h, 
   UINT i, UINT j, UINT k, UINT l
)
{
  return -1;
}

/*===========================================================================
  FUNCTION  qmi_idl_shim_init
  DESCRIPTION: qmi idl shim init function
===========================================================================*/
void qmi_idl_shim_init(void)
{
  return;
} /* qmi_client_shim_init */

