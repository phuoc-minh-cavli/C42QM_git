/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   S E N D   D T M F  S T U B


GENERAL DESCRIPTION
  This source file contains dummy implementation of functions handled in gstk_send_dtmf.c

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/stub/gstk_send_dtmf_stub.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/15/16   gs      Changes in TP based on RAT and App Type
02/08/16   gs      Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk_proactive_cmd.h"

/*===========================================================================
FUNCTION gstk_send_dtmf_req

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
gstk_status_enum_type gstk_send_dtmf_req  (gstk_proactive_cmd_type* command_ptr)
{
  (void) command_ptr;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_send_dtmf_req */

/*===========================================================================
FUNCTION gstk_send_dtmf_cnf

DESCRIPTION
  Dummy Functionality

PARAMETERS
  send_dtmf_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_send_dtmf_cnf (
  gstk_send_dtmf_cnf_type* send_dtmf_cnf_ptr )
{
  (void) send_dtmf_cnf_ptr;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_send_dtmf_cnf */

/*===========================================================================
FUNCTION gstk_check_dtmf_capability

DESCRIPTION
  This function checks the DTMF capabilites

PARAMETERS
  None

RETURN VALUE
  FALSE

SEE ALSO
  None
===========================================================================*/
boolean gstk_check_dtmf_capability(void)
{
  return FALSE;
} /* gstk_check_dtmf_capability */