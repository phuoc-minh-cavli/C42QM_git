/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    G B A   M A I N   T A S K  S T U B S


GENERAL DESCRIPTION

  This file contains the stub functions for main GBA task.

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     -----------------------------------------------------------
11/15/16   shr     Add utility function to check GBA support
03/09/16   vdc    Initial Version

=============================================================================*/

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "rcinit.h"
#include "gba_lib.h"

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*=============================================================================
FUNCTION        GBA_IS_KEY_VALID

DESCRIPTION
  This is a stub function equivalent of the actual function by the same name
  that checks whether the B-TID is valid and its life time validity

PARAMETERS
  session_type:   Session type indicator
  aid_ptr:        Pointer to application id. This is required if session type
                  is non-prov
  btid_ptr:       Pointer to B-TID value
  life_time_ptr:  Life time pointer. This can be set to NULL if client is not
                  interested

RETURN VALUE
  Boolean:
    TRUE:         If the B-TID matches and life time validity has not expired
    FALSE:        If the B-TID does not match or the B-TID's life time has expired

===============================================================================*/
boolean gba_is_key_valid (
  gba_session_type        session_type,
  const gba_aid_type    * aid_ptr,
  const gba_btid_type   * btid_ptr,
  gba_lifetime_type     * life_time_ptr
)
{
  (void) session_type;
  (void) aid_ptr;
  (void) btid_ptr;
  (void) life_time_ptr;

  return FALSE;
} /* gba_is_key_valid */


/*=============================================================================
FUNCTION        GBA_BOOTSTRAP

DESCRIPTION
  This is a stub function equivalent of the actual function by the same name
  that begins the Generic Bootstrapping procedure. If the NAF id is NULL of
  no FQDN is provided then GBA does not calculate Ks_NAF and returns after
  bootstrapping

PARAMETERS
  naf_id_ptr:        Network Application Function ID
  session_type:      Session type indicator
  aid_ptr:           Application ID (NULL for provisioning sessions)
  force_bootstrap:   Indicates whether to force bootstrap procedure
  request_id_ptr:    Output request id of the queued command
                     (Can be NULL if request id is not required)
  apn_ptr:           Pointer to APN data
  gba_cb:            Client response function
  user_data_ptr:     Client user data

RETURN VALUE
  gba_result_enum_type:
    GBA_SUCCESS:               The command structure was properly
                               generated and queued onto the GBA
                               Command Queue.
    GBA_INCORRECT_PARAMS:      The parameters supplied to the API are not
                               within appropriate ranges.

===============================================================================*/
gba_result_enum_type gba_bootstrap (
  gba_naf_id_type                * naf_id_ptr,
  gba_session_type                 session_type,
  const gba_aid_type             * aid_ptr,
  boolean                          force_bootstrap,
  uint32                         * request_id_ptr,
  const char                     * apn_ptr,
  gba_callback_type                gba_cb,
  const void                     * user_data_ptr
)
{
  (void) naf_id_ptr;
  (void) session_type;
  (void) aid_ptr;
  (void) force_bootstrap;
  (void) request_id_ptr;
  (void) apn_ptr;
  (void) gba_cb;
  (void) user_data_ptr;

  return GBA_UNSUPPORTED;
} /* gba_bootstrap */


/*=============================================================================
FUNCTION        GBA_CANCEL

DESCRIPTION
  This is a stub function equivalent of the actual function by the same name
  which deletes a GBA bootstrap request that matches the request id passed in.
  If the matching request is currently being processed, then the bootstrap
  command will be cancelled and any stored data related to the command will be
  cleared. If the bootstrap request is still in the queue then it is simply
  removed. In both cases, the callback of the original request will be triggered
  allowing the client to clean-up any userdata that may have been allocated or
  any other extra processing that may have been required.

PARAMETERS
  request_id:                  Request ID of the command to be cancelled

RETURN VALUE
  gba_result_enum_type:
    GBA_SUCCESS:               The command was successfully cancelled
    GBA_INCORRECT_PARAMS:      The request id parameter was invalid

===============================================================================*/
gba_result_enum_type gba_cancel (
  uint32                           request_id
)
{
  (void) request_id;

  return GBA_UNSUPPORTED;
} /* gba_cancel */


/*===========================================================================
FUNCTION GBA_TASK_INIT

DESCRIPTION
  This is a stub function equivalent of the actual function by the same name
  which is called to initialize the GBA task queue and globals.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
  On targets with RCInit enabled, this function is called in RCInit context.
  Therefore, it should not include references to TCBs, NV
  operations, etc.
===========================================================================*/
void gba_task_init (
  void
)
{

} /* gba_task_init */


/*===========================================================================
FUNCTION GBA_MAIN

DESCRIPTION
  Entry point for the GBA task. This is a stub function equivalent of the
  actual function by the same name which performs task initialization, then
  sits in an infinite loop, waiting on an input queue, and responding to
  messages received. Here we only performs handshake with RCINIT framework.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void gba_main (
  dword dummy
)
{
  (void) dummy;

  rcinit_handshake_startup();
  (void)rcevt_signal_name("GBA:READY");
} /* gba_main */


/*=============================================================================
FUNCTION        GBA_IS_SUPPORTED

DESCRIPTION
  This function indicates whether GBA is supported or not

PARAMETERS
  None

RETURN VALUE
  Boolean:
    TRUE:         GBA is supported
    FALSE:        GBA is not supported

===============================================================================*/
boolean gba_is_supported(void)
{
  return FALSE;
} /* gba_is_supported */

