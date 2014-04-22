/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   S E N D   U S S D   S T U B


GENERAL DESCRIPTION
  This source file contains dummy implementation of functions handled in gstk_send_ussd.c


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights  Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/stub/gstk_send_ussd_stub.c#1 $

when         who     what, where, why
--------   ---     ----------------------------------------------------------
09/15/16   gs      Changes in TP based on RAT and App Type
05/04/16   gs      Memory reduction for IoE
04/14/16   gs      Initial Version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk_proactive_cmd.h"

/*===========================================================================
FUNCTION gstk_send_ussd_req

DESCRIPTION
  Dummy functionality

PARAMETERS
  command_ptr: [Input] Pointer to message parsed and sent to clients

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_ussd_req  (
  gstk_proactive_cmd_type* command_ptr)
{
  (void) command_ptr;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_send_ussd_req */

/*===========================================================================
FUNCTION gstk_send_ussd_cnf

DESCRIPTION
  Dummy functionality

PARAMETERS
  send_ussd_cnf_ptr: [Input] Pointer to message that is required to
                            be processed and sent to UIM

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_ussd_cnf (
  gstk_send_ussd_cnf_type* send_ussd_cnf_ptr )
{
  (void) send_ussd_cnf_ptr;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_send_ussd_cnf */

/*===========================================================================
  FUNCTION gstk_populate_send_ussd_tr_cnf
  
  DESCRIPTION
	Populates the response paramters for TR
  
  PARAMETERS
	task_cmd_ptr	   :[Input/Output] Pointer to the task_cmd_type to which the
                                           terminal response is going to be populated
  
	additional_res_ptr :[Input] Specifies the optional additional result for
                                    the proactive command, the length field in
                                    the gstk_additonal_info_type specifies number
                                    of additional_info in the structure
  
	other_info_ptr	   :[Input] Pointer to the gstk_terminal_rsp_extra_param_type
  
  DEPENDENCIES
	None
  
  RETURN VALUE
	None
  
  COMMENTS
	None
  
  SIDE EFFECTS
	None
  
  SEE ALSO
	None
  ===========================================================================*/
gstk_status_enum_type gstk_populate_send_ussd_tr_cnf(
  gstk_task_cmd_type			   *task_cmd_ptr,
  const gstk_additional_info_ptr_type 	   *additional_res_ptr,
  const gstk_terminal_rsp_extra_param_type *other_info_ptr
)
{
  (void) additional_res_ptr;
  (void) other_info_ptr;
  
  task_cmd_ptr->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.has_cc_type_modification = FALSE;
  task_cmd_ptr->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.data.length = 0;
  task_cmd_ptr->cmd.send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2_ptr.length = 0;
  /* free the alpha */
  if (gstk_shared_data.cc_req_action.alpha.text != NULL) {
    gstk_free(gstk_shared_data.cc_req_action.alpha.text);
    gstk_shared_data.cc_req_action.alpha.text = NULL;
  }
  memset(&gstk_shared_data.cc_req_action, 0x00, sizeof(gstk_cc_req_action_type));
  return GSTK_SUCCESS;
} /* gstk_populate_send_ussd_tr_cnf */

/*===========================================================================
FUNCTION gstk_check_ussd_capability

DESCRIPTION
  This function checks the USSD capabilites

PARAMETERS
  None

RETURN VALUE
  FALSE

SEE ALSO
  None
===========================================================================*/
boolean gstk_check_ussd_capability(void)
{
  return FALSE;
} /* gstk_check_ussd_capability */

