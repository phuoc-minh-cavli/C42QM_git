/*===========================================================================


            G S T K   E V T  D L  T E R M I N A L  C A P  S T U B

GENERAL DESCRIPTION
  This source file contains dummy implmentation of functions handled in gstk_evt_dl_terminal_cap.c

INITIALIZATION AND SEQUENCING REQUIREMENTS

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //components/rel/uim.mpss/3.3.1/gstk/src/stub/gstk_evt_dl_voice_cap_stub.c#1 $

when         who     what, where, why
--------   ---     --------------------------------------------------------------------
09/15/16   gs      Changes in TP based on RAT and App Type
02/05/16   gs      Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk_envelope_cmd.h"
#include "gstk_envelope_rsp.h"

/*===========================================================================
FUNCTION gstk_mt_call_evt_ind

DESCRIPTION
  Dummy Functionality

PARAMETERS
  mt_evt_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_mt_call_evt_ind(
        gstk_envelope_mt_evt_ind_type    * mt_evt_cmd)
{
  (void) mt_evt_cmd;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_mt_call_evt_ind*/

/*===========================================================================
FUNCTION gstk_call_connected_evt_ind

DESCRIPTION
  Dummy Functionality

PARAMETERS
  call_connected_evt_cmd: [Input] Pointer to the envelope command to be
                                  processed

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
gstk_status_enum_type gstk_call_connected_evt_ind(
        gstk_envelope_call_connected_evt_ind_type    * call_connected_evt_cmd)
{
  (void) call_connected_evt_cmd;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_call_connected_evt_ind*/

/*===========================================================================
FUNCTION gstk_call_disconnected_evt_ind

DESCRIPTION
  Dummy Functionality
  
PARAMETERS
  call_connected_evt_cmd: [Input] Pointer to the envelope command to be
                                  processed

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
gstk_status_enum_type gstk_call_disconnected_evt_ind(
        gstk_envelope_call_disconnected_evt_ind_type    * call_disconnected_evt_cmd)
{
  (void) call_disconnected_evt_cmd;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_call_disconnected_evt_ind */

/*===========================================================================
FUNCTION gstk_check_evt_dl_voice_cap

DESCRIPTION
  This function checks the voice capabilites

PARAMETERS
  None

RETURN VALUE
  FALSE

SEE ALSO
  None
===========================================================================*/
boolean gstk_check_evt_dl_voice_cap(void)
{
  return FALSE;
} /* gstk_check_evt_dl_voice_cap */

