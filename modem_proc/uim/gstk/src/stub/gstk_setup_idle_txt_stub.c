/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   S E T U P  I D L E   T E X T  S T U B 


GENERAL DESCRIPTION
  This source file contains dummy implmentation of functions handled in gstk_setup_idle_txt.c

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

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/stub/gstk_setup_idle_txt_stub.c#1 $

when         who     what, where, why
--------   ---     ----------------------------------------------------------
04/28/16   gs       Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk_proactive_cmd.h"

/*===========================================================================
FUNCTION gstk_setup_idle_mode_text_req

DESCRIPTION
  Dummy Functionality

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
gstk_status_enum_type gstk_setup_idle_mode_text_req  (
  gstk_proactive_cmd_type* command_ptr)
{
  (void) command_ptr;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_setup_idle_mode_text_req*/

/*===========================================================================
FUNCTION gstk_setup_idle_mode_text_cnf

DESCRIPTION
  Dummy Functionality

PARAMETERS
  idle_text_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_setup_idle_mode_text_cnf (
  gstk_setup_idle_text_cnf_type* idle_text_cnf_ptr )
{
  (void) idle_text_cnf_ptr;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_setup_idle_mode_text_cnf */

/*===========================================================================
FUNCTION gstk_release_setup_idle_text_memory

DESCRIPTION
  This function frees all the allocated memory for the command pointer input.

PARAMETERS
  message_ptr: [Input] Pointer to message to which memory location is required
                     to be freed

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  The member fields in the gstk_cmd_from_card_type pointer that client passed
  in will be set to null upon the return of this function

SEE ALSO
  None
===========================================================================*/
void gstk_release_setup_idle_text_memory(gstk_cmd_from_card_type * message_ptr)
{
  (void)message_ptr;
}

