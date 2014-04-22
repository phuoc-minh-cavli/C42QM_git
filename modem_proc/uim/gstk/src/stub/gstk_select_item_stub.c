/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   S E L E C T    I T E M  S T U B


GENERAL DESCRIPTION
  This source file contains dummy implmentation of functions handled in gstk_select_item.c

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/stub/gstk_select_item_stub.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/16   gs      Memory reduction for IoE
03/22/16   gs      Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk_proactive_cmd.h"

/*===========================================================================
FUNCTION GSTK_SELECT_ITEM_REQ

DESCRIPTION
  Dummy Functionality

PARAMETERS
  command: [Input] Pointer to message parsed and sent to clients

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
gstk_status_enum_type gstk_select_item_req (
  gstk_proactive_cmd_type *command_ptr
)
{
  (void) command_ptr;
  return GSTK_UNSUPPORTED_COMMAND;
}/*  gstk_select_item_req */

/*===========================================================================
FUNCTION gstk_select_item_cnf

DESCRIPTION
  Dummy Functionality
  
PARAMETERS
  select_item_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_select_item_cnf (
  gstk_select_item_cnf_type* select_item_cnf_ptr
)
{
  (void) select_item_cnf_ptr;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_select_item_cnf */

/*===========================================================================
  FUNCTION gstk_populate_select_item_tr_cnf
  
  DESCRIPTION
     Dummy Functionality
  
  PARAMETERS
     task_cmd_ptr	   :[Input/Output] Pointer to the task_cmd_type to which the
                                           terminal response is going to be populated
     additional_res_ptr    :[Input] Specifies the optional additional result for
                                    the proactive command, the length field in
                                    the gstk_additonal_info_type specifies number
                                    of additional_info in the structure  
     other_info_ptr	   :[Input] Pointer to the gstk_terminal_rsp_extra_param_type
     terminal_response_enum:[Input] Command Value
     general_result        :[Input] Genral result
     cat_version           :[Input] CAT Version
  
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
gstk_status_enum_type gstk_populate_select_item_tr_cnf(
  gstk_task_cmd_type			   *task_cmd_ptr,
  const gstk_additional_info_ptr_type 	   *additional_res_ptr,
  const gstk_terminal_rsp_extra_param_type *other_info_ptr,
  gstk_cmd_enum_type                        terminal_response_enum,
  gstk_general_result_enum_type             general_result,
  uint8                                     cat_version
)
{
  (void) additional_res_ptr;
  (void) other_info_ptr;
  (void) terminal_response_enum;
  (void) general_result;
  (void) cat_version;

  task_cmd_ptr->cmd.select_item_term_rsp_cnf.data_present = FALSE;  
  return GSTK_SUCCESS;
}

