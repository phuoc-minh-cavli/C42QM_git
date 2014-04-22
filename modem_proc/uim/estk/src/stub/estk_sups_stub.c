/*===========================================================================
FILE:  estk_sups_stub.c


GENERAL DESCRIPTION : ENHANCED STK layer Send USSD and Send SS support Stub functionality

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

=============================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/estk/src/stub/estk_sups_stub.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/26/16   gs      Initial Version

=============================================================================*/

/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "estk_ssparser.h"
#include "uim_msg.h"

#if defined(FEATURE_ESTK)
/*===========================================================================
FUNCTION: estk_cm_sups_event_cb

DESCRIPTION:
  Dummy functionality

PARAMETERS:
  sups_event    : [Input] CM supplementary service event
  sups_info_ptr : [Input] CM supplementary service data info

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_cm_sups_event_cb(
  cm_sups_event_e_type          sups_event,
  const cm_sups_info_s_type    *sups_info_ptr
)
{
  (void) sups_event;
  (void) sups_info_ptr;
} /* estk_cm_sups_event_cb */

/*===========================================================================

FUNCTION: estk_sups_process_send_ussd_req

DESCRIPTION:
  Dummy functionality

PARAMETERS:
  gstk_req_ptr : [Input] request pointer for USSD in gstk format

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_sups_process_send_ussd_req (
   gstk_cmd_from_card_type *gstk_req_ptr
)
{
  (void) gstk_req_ptr;
  return ESTK_UNSUPPORTED;
} /* estk_sups_process_send_ussd_req*/

/*===========================================================================
FUNCTION: estk_process_send_ss_req

DESCRIPTION:
  Dummy functionality

PARAMETERS:
  gstk_req_ptr : [Input] request pointer for SS in gstk format

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_send_ss_req (
   gstk_cmd_from_card_type *gstk_req_ptr
)
{
  (void) gstk_req_ptr;
  return ESTK_UNSUPPORTED;
} /* estk_process_send_ss_req */

/*===========================================================================

FUNCTION: estk_sups_cleanup

DESCRIPTION:
  Dummy functionality

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_sups_cleanup( void )
{

} /* estk_sups_cleanup */

/*===========================================================================

FUNCTION: estk_process_cm_sups_event_cmd

DESCRIPTION :
  Dummy functionality

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS : If processing of call event is successful
    ESTK_ERROR   : If processing results in error

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_cm_sups_event_cmd(
  const estk_cmd_type *cmd_ptr
)
{
  (void) cmd_ptr;
  return ESTK_UNSUPPORTED;
}

/*===========================================================================

FUNCTION estk_cm_sups_reg_cmd_cb

DESCRIPTION:
  Dummy functionality

PARAMETERS:
  client_id : [Input] CM client id
  cmd       : [Input] CM supplementary service command
  cmd_err   : [Input] command result

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_cm_sups_reg_cmd_cb(
  cm_client_id_type            client_id,
  cm_sups_cmd_e_type           cmd,
  cm_sups_cmd_err_e_type       cmd_err
)
{
  (void)client_id;
  if (cmd_err != CM_SUPS_CMD_ERR_NOERR)
  {
    UIM_MSG_ERR_2("ESTK CM Stub Sups cmd 0x%x failed 0x%x", cmd, cmd_err);
  }
} /* estk_cm_sups_reg_cmd_cb */
#endif /* FEATURE_ESTK */
