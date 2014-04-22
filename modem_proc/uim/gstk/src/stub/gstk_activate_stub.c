/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   A C T I V A T E


GENERAL DESCRIPTION
  This source file contains functions to handle Activate
  proactive command when not supported


FUNCTIONS
  gstk_activate_req
    This function parses the Activate Command from the card
    and sends it to the client.

  gstk_activate_cnf
    This function packs the Activate terminal response and sends
    the command to UIM server.


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

$Header: //commercial/MPSS.JO.1.2.r3/Main/modem_proc/uim/gstk/src/gstk_activate_stub.c#1

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/08/16   vr      Initial Version

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* GSTK Header Files   */
#include "gstk.h"
#include "gstk_proactive_cmd.h"
#include "gstk_terminal_rsp.h"
#include "uim_msg.h"

#define GSTK_DISABLE_CLASS_L_COMMAND_BYTE_THIRTY 0xEF

/*===========================================================================
FUNCTION gstk_activate_req

DESCRIPTION
  This function parses the Activate Command from the card
  and sends it to the client.

  In case of any parsing errors or when GSTK can't send the command to the
  client, it will call the gstk_send_terminal_response function so that
  gstk can send the terminal response back to the card.

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
gstk_status_enum_type gstk_activate_req (
  gstk_proactive_cmd_type* command_ptr
)
{
  (void) command_ptr;

  return GSTK_UNSUPPORTED_COMMAND;

} /*gstk_activate_req*/

/*===========================================================================
FUNCTION gstk_activate_cnf

DESCRIPTION
  This function packs the Activate terminal response and send
  the command to UIM server.

PARAMETERS
  activate_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_activate_cnf (
  gstk_activate_cnf_type* activate_cnf_ptr )
{
  (void) activate_cnf_ptr;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_activate_cnf */

/*===========================================================================
FUNCTION gstk_update_class_l_tp

DESCRIPTION
  This function will return false in stub code to disable TP and TRUE otherwise
  for class L TP bits

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_update_class_l_tp ( void )
{
  return FALSE;
} /* gstk_update_class_l_tp */
