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


$Header: //components/rel/uim.mpss/3.3.1/gstk/src/stub/gstk_evt_dl_terminal_cap_stub.c#1 $

when         who     what, where, why
--------   ---     --------------------------------------------------------------------
02/05/16   gs       Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk_envelope_cmd.h"

/*===========================================================================
FUNCTION gstk_idle_scrn_evt_ind

DESCRIPTION
  Dummy Functionality

PARAMETERS
  idle_scrn_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_idle_scrn_evt_ind(
  const gstk_envelope_idle_scrn_evt_ind_type * idle_scrn_cmd)
{
  (void) idle_scrn_cmd;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_idle_scrn_evt_ind */

/*===========================================================================
FUNCTION gstk_user_act_evt_ind

DESCRIPTION
  Dummy Functionality

PARAMETERS
  user_act_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_user_act_evt_ind(
  const gstk_envelope_user_act_evt_ind_type * user_act_cmd)
{
  (void) user_act_cmd;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_user_act_evt_ind */


/*===========================================================================
FUNCTION gstk_browser_term_evt_ind

DESCRIPTION
  Dummy Functionality

PARAMETERS
  browser_term_evt_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_browser_term_evt_ind(
  const gstk_envelope_browser_term_evt_ind_type    * browser_term_evt_cmd)
{
  (void) browser_term_evt_cmd;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_browser_term_evt_ind*/

/*===========================================================================
FUNCTION gstk_lang_selection_evt_ind

DESCRIPTION
  Dummy Functionality

PARAMETERS
  lang_select_evt_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_lang_selection_evt_ind(
  const gstk_envelope_lang_sel_evt_ind_type    * lang_select_evt_cmd)
{
  (void) lang_select_evt_cmd;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_lang_selection_evt_ind */
