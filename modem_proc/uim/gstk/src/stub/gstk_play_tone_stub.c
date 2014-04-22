/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   P L A Y   T O N E  S T U B


GENERAL DESCRIPTION
  This source file contains dummy implmentation of functions handled in gstk_play_tone.c

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

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/stub/gstk_play_tone_stub.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/16   gs      Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk_proactive_cmd.h"

/*===========================================================================
FUNCTION gstk_play_tone_req

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
gstk_status_enum_type gstk_play_tone_req (
  gstk_proactive_cmd_type* command_ptr)
{
  (void) command_ptr;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_play_tone_req */

/*===========================================================================
FUNCTION gstk_play_tone_cnf

DESCRIPTION
  Dummy Functionality

PARAMETERS
  play_tone_cnf_ptr: [Input] Pointer to message that is required to
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
gstk_status_enum_type gstk_play_tone_cnf(
  gstk_play_tone_cnf_type* play_tone_cnf_ptr)
{
  (void) play_tone_cnf_ptr;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_play_tone_cnf*/
