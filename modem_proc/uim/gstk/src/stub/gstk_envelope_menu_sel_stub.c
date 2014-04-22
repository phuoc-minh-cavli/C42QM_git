/*===========================================================================


            G S T K   E N V E L O P E   M E N U   S E L   S T U B

GENERAL DESCRIPTION
  This source file contains dummy implmentation of functions handled in gstk_envelope_cbs.c


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


$Header: //components/rel/uim.mpss/3.3.1/gstk/src/stub/gstk_envelope_menu_sel_stub.c#1 $

when         who     what, where, why
--------   ---     --------------------------------------------------------------------
01/28/16   gs       Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk_envelope_cmd.h"
#include "gstk_envelope_rsp.h"

/*===========================================================================
FUNCTION gstk_menu_selection_ind

DESCRIPTION
  Dummy Functionlity

PARAMETERS
  menu_sel_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_menu_selection_ind(
  const gstk_envelope_menu_selection_ind_type * menu_sel_cmd)
{
  (void) menu_sel_cmd;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_menu_selection_ind */

/*===========================================================================
FUNCTION gstk_menu_selection_rsp

DESCRIPTION
  Dummy Functionlity

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
gstk_status_enum_type gstk_menu_selection_rsp(
  const gstk_cmd_type * cmd)
{
  (void) cmd;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_menu_selection_rsp */

/*===========================================================================
FUNCTION gstk_non_idle_st_menu_selection_err_rsp

DESCRIPTION
  Dummy Functionlity

PARAMETERS
  cmd: [Input] Pointer to the envelope rsp to be processed

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
gstk_status_enum_type gstk_non_idle_st_menu_selection_err_rsp(
  const gstk_envelope_menu_selection_ind_type * menu_sel_cmd)
{
  (void) menu_sel_cmd;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_non_idle_st_menu_selection_err_rsp */
