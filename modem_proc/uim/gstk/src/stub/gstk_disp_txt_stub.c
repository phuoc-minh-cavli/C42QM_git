/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   D I S P L A Y   T E X T   S T U B


GENERAL DESCRIPTION
  This source file contains dummy implmentation of functions handled in gstk_disp_txt.c

FUNCTIONS
  gstk_display_text_req
    This function parses the Display Text Proactive Command from the card
    and sends it to the client.

  gstk_display_text_cnf
    This function packs the Display Text terminal response and send
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

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/stub/gstk_disp_txt_stub.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/16   gs     Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk_proactive_cmd.h"

/*===========================================================================
FUNCTION gstk_display_text_req

DESCRIPTION
  Dummy Functionlity

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
gstk_status_enum_type gstk_display_text_req  (
  gstk_proactive_cmd_type* command_ptr)
{
  (void) command_ptr;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_display_text_req*/

/*===========================================================================
FUNCTION gstk_display_text_cnf

DESCRIPTION
  Dummy Functionlity

PARAMETERS
  display_text_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_display_text_cnf (
  gstk_display_text_cnf_type* display_text_cnf_ptr )
{
  (void) display_text_cnf_ptr;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_display_text_cnf */
