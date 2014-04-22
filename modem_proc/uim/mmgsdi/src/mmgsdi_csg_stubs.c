/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                   M M G S D I   C S G   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the CSG related stub functions

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_csg_stubs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/22/16   tkl     Add stub API for CNF
05/11/16   sp      Fix low level compiler warnings
03/09/16   vdc     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmgsdilib_common.h"
#include "mmgsdi_eons.h"
#include "mmgsdi.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*=============================================================================

                       FUNCTION IMPLEMENTATIONS

=============================================================================*/

/*==========================================================================
FUNCTION MMGSDI_EONS_RUN_ALGO_CSG

DESCRIPTION
  Stub function for mmgsdi-eons_run_algo_csg

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_run_algo_csg(
  mmgsdi_eons_call_stack_enum_type  stack_type,
  mmgsdi_session_id_type            session_id,
  mmgsdi_eons_trigger_enum_type     eons_trigger,
  mmgsdi_plmn_id_type             * plmn_csg_id_ptr,
  mmgsdi_list_plmn_name_type      * plmn_name_list_ptr,
  uint32                            index,
  boolean                           plmn_id_lac_rat_changed
)
{
  (void)stack_type;
  (void)session_id;
  (void)eons_trigger;
  (void)plmn_csg_id_ptr;
  (void)plmn_name_list_ptr;
  (void)index;
  (void)plmn_id_lac_rat_changed;

  return MMGSDI_ERROR;
} /* mmgsdi_eons_run_algo_csg */


/*==========================================================================
FUNCTION MMGSDI_CSG_REFRESH_CSGL_CACHE

DESCRIPTION
  Stub function for mmgsdi_csg_refresh_csgl_cache

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_csg_refresh_csgl_cache(
  mmgsdi_session_id_type session_id,
  mmgsdi_client_id_type  client_id,
  mmgsdi_file_enum_type  file
)
{
  (void)session_id;
  (void)client_id;
  (void)file;

  return MMGSDI_SUCCESS;
} /* mmgsdi_csg_refresh_csgl_cache */


/*==============================================================================
FUNCTION MMGSDI_CSG_PROCESS_UPDATE_ACSGL_RECORD_REQUEST

DESCRIPTION:
  Stub function for mmgsdi_csg_process_update_acsgl_record_reqest

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS:           The command processing was successful.
  MMGSDI_ERROR:             The command processing was not successful.
  MMGSDI_CSG_UPDATE_FAILED: The new csg_id cannot be added or the
                            csg_id to delete is not found.

SIDE EFFECTS:
  None
==============================================================================*/
mmgsdi_return_enum_type mmgsdi_csg_process_update_acsgl_record_request(
  mmgsdi_session_update_acsgl_record_req_type * req_ptr
)
{
  (void)req_ptr;

  return MMGSDI_SUCCESS;
} /* mmgsdi_csg_process_update_acsgl_record_request */


/* =============================================================================
FUNCTION:      MMGSDI_CSG_RESET_EONS_FLAGS_IN_APP_INFO

DESCRIPTION:
  Stub function for mmgsdi_csg_reset_eons_flags_in_app_info

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
=============================================================================*/
void mmgsdi_csg_reset_eons_flags_in_app_info(
  mmgsdi_session_id_type  session_id
)
{
  (void)session_id;
} /* mmgsdi_csg_reset_eons_flags_in_app_info */


/*===========================================================================
FUNCTION MMGSDI_EONS_TRIGGER_ACSGL_CSGT_HNBN_CACHE

DESCRIPTION
  Stub function for mmgsdi_eons_trigger_acsgl_csgt_hnbn_cache

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_trigger_acsgl_csgt_hnbn_cache(
  mmgsdi_session_id_type session_id
)
{
  (void)session_id;
} /* mmgsdi_eons_trigger_acsgl_csgt_hnbn_cache */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SESSION_UPDATE_ACSGL_DATA

   DESCRIPTION:
     This function builds the Cnf data for session update acsgl command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_session_update_acsgl_data(
  mmgsdi_session_update_acsgl_record_cnf_type       *session_update_acsgl_cnf_ptr,
  const mmgsdi_session_update_acsgl_extra_info_type *session_update_acsgl_data_ptr)
{
  (void)session_update_acsgl_cnf_ptr;
  (void)session_update_acsgl_data_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_cnf_build_session_update_acsgl_data */


/*=============================================================================
FUNCTION        MMGSDI_SESSION_QUEUE_UPDATE_ACSGL_RECORD_REQ

DESCRIPTION
  This function allows the client to update (add or delete) a CSG ID associated
  with a PLMN ID in the EF-ACSGL record.

PARAMETERS
  session_id:      Session ID of the caller
  update_type:     Type of update of CSG ID requested
  plmn_csg_id:     CSG ID to be updated and associated PLMN ID
  hbn_name:        Home Node(b) name corresponding to the CSG ID
  response_cb_ptr: Callback to this command
  client_ref:      User Data returned upon completion of this cmd.

DEPENDENCIES
  A valid Session ID is required, which can be achieved by calling
  mmgsdi_session_open().

LIMITATIONS
  This API will be limited to the session on which the client has a valid
  open session

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_session_queue_update_acsgl_record_req(
  mmgsdi_session_id_type               session_id,
  mmgsdi_update_acsgl_enum_type        update_type,
  mmgsdi_plmn_csg_id_type              plmn_csg_id,
  mmgsdi_data_type                     hnb_name,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
)
{
  (void)session_id;
  (void)update_type;
  (void)plmn_csg_id;
  (void)hnb_name;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_session_queue_update_acsgl_record_req */

