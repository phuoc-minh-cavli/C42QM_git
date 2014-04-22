/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   S I M   D R I V E R   I N T E R F A C E

       C O M M O N   P C N   H A N D S E T   S P E C I F I C A T I O N S

                   ( C P H S )   S T U B  P R O C E D U R E S


GENERAL DESCRIPTION

  This source file contains the Common PCN Handset Specifications (CPHS)
  stub procedures.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_cphs_stubs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/27/18   tq      Moving function to stub file
06/22/16   tkl     Stub out additional APIs
05/11/16   vdc     Guard with FEATURE_MMGSDI_3GPP2 for 3GPP2 related functions
04/29/16   bcho    Initial version

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"

/* System and Interfaces */
#include "mmgsdi_cphs.h"



/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================
FUNCTION MMGSDI_CPHS_POST_PIN1_INIT

DESCRIPTION
  This function will run the CPHS Intialization procedures.  The purpose
  of this function is to cache the relevant CPHS Information for use
  by other Tasks for later usage.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cphs_post_pin1_init(
  mmgsdi_session_id_type               session_id,
  mmgsdi_slot_id_enum_type             slot,
  mmgsdi_app_enum_type                 app_type,
  mmgsdi_cphs_read_cphs_efs_info_type  read_cphs_efs
)
{
  (void)session_id;
  (void)slot;
  (void)app_type;
  (void)read_cphs_efs;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_cphs_post_pin1_init */


/*==========================================================================
FUNCTION MMGSDI_CPHS_GET_ONS

DESCRIPTION
  Function to get operator name string from cphs file caches

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_cphs_get_ons(
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_plmn_info_type         * plmn_info_ptr)
{
  (void)session_type;
  (void)plmn_info_ptr;
  return FALSE;
}/* mmgsdi_cphs_get_ons */


/*===========================================================================
FUNCTION: MMGSDI_CPHS_GET_CPHS_INFO_PTR

DESCRIPTION
  Based on the session type, this function will return the cphs info ptr

DEPENDENCIES
  none

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cphs_get_cphs_info_ptr(
  mmgsdi_session_type_enum_type         session_type,
  mmgsdi_cphs_int_info_type           **cphs_int_info_pptr
)
{
  (void)session_type;
  (void)cphs_int_info_pptr;

  return MMGSDI_ERROR;
} /* mmgsdi_cphs_get_cphs_info_ptr */


/*===========================================================================
FUNCTION        MMGSDI_SESSION_QUEUE_GET_CPHS_INFO_REQ

DESCRIPTION
  This function is used to extract the CPHS related information which is
  already populated in MMGSDI. This is only valid and applicable when an USIM
  or GSM Application is available . This function will perform a check for
  the USIM or GSM Application before sending the command.

PARAMETERS
  mmgsdi_session_id_type  session_id:      Session ID of the caller
  mmgsdi_callback_type    response_cb_ptr: Call back function when the
                                           command execution is done.
  mmgsdi_client_data_type client_ref:      User Data returned upon
                                           completion of this cmd.

DEPENDENCIES
  The client must also have a valid session open if the USIM Application
  is not the default application
  Achieved by calling mmgsdi_session_open().

LIMITATIONS
  None.

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
  None.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_queue_get_cphs_info_req (
  mmgsdi_session_id_type              session_id,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
)
{
  (void)session_id;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/*mmgsdi_session_queue_get_cphs_info_req*/


/*===========================================================================
  FUNCTION:      MMGSDI_GEN_GET_CPHS_INFO

  DESCRIPTION:
    This function will handle get cphs information request

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:        The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
  SIDE EFFECTS:
    NONE

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_get_cphs_info(
  const mmgsdi_session_get_cphs_info_req_type *req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_gen_get_cphs_info */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SESSION_GET_CPHS_INFO

   DESCRIPTION:
     This function builds the Cnf data for session get cphs info command

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
mmgsdi_return_enum_type mmgsdi_cnf_build_session_get_cphs_info(
  mmgsdi_session_id_type                  session_id,
  mmgsdi_session_get_cphs_info_cnf_type  *session_get_cphs_info_cnf_ptr,
  mmgsdi_return_enum_type                 mmgsdi_status)
{
  (void)session_id;
  (void)session_get_cphs_info_cnf_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/*mmgsdi_cnf_build_session_get_cphs_info*/


/* ============================================================================
FUNCTION:      MMGSDI_GEN_RSP_GET_CPHS_INFO

DESCRIPTION:
  This function will build cnf for get cphs info command

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type
    MMGSDI_SUCCESS:     The cnf was successfully queued to cnf Queue
    MMGSDI_ERROR:       Failed in queuing cnf to cnf Queue
    MMGSDI_NOT_SUPPORTED:    This command not supported
SIDE EFFECTS:
  NONE
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_gen_rsp_get_cphs_info (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_gen_rsp_get_cphs_info */ 
