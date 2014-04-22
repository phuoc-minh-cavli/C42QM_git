/*===========================================================================


            G S T K   E N V E L O P E   C B S  S T U B

GENERAL DESCRIPTION
  This source file contains stubs functions to handle Cell Broadcast Data Download


FUNCTIONS
  gstk_cell_broadcast_dl_ind
    This function builds the cell broadcast download command and sends it
    to UIM

  gstk_cell_broadcast_dl_rsp
    This function builds the cell broadcast download rsp and sends it to client

  gstk_non_idle_st_cell_change_brcst_err_rsp
    This function builds the cell brcst rsp error message in non idle state
    and sends it to client.  GSTK will set the status to SUCCESS and no MOD
    in the data so that the mobile can proceed with the call as the Toolkit
    is no longer valid 


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


$Header: //components/rel/uim.mpss/3.3.1/gstk/src/stub/gstk_envelope_cbs_stub.c#1 $

when         who     what, where, why
--------   ---     --------------------------------------------------------------------
09/15/16   gs      Changes in TP based on RAT and App Type
01/28/16   gs      Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk_envelope_cmd.h"
#include "gstk_envelope_rsp.h"

/*===========================================================================
FUNCTION gstk_cell_broadcast_dl_ind

DESCRIPTION

  This function builds the cell broadcast download command and sends it to UIM

PARAMETERS
  cb_dl_cmd: [Input] Pointer to the envelope command to be processed

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
gstk_status_enum_type gstk_cell_broadcast_dl_ind(
          gstk_envelope_cell_broadcast_download_ind_type * cb_dl_cmd)
{
  (void) cb_dl_cmd;
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_cell_broadcast_dl_ind */

/*===========================================================================
FUNCTION gstk_cell_broadcast_dl_rsp

DESCRIPTION

  This function builds the cell broadcast rsp and sends it to client

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
gstk_status_enum_type gstk_cell_broadcast_dl_rsp(
  const gstk_cmd_type * cmd)
{
   (void) cmd; 
   return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_cell_broadcast_dl_rsp */

/*===========================================================================
FUNCTION gstk_non_idle_st_cell_change_brcst_err_rsp

DESCRIPTION

  This function builds the cell brcst rsp error message in non idle state
  and sends it to client.  GSTK will set the status to SUCCESS and no MOD
  in the data so that the mobile can proceed with the call as the Toolkit
  is no longer valid

PARAMETERS
  cell_brcst_cmd_ptr: [Input] Pointer to the envelope rsp to be processed

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
gstk_status_enum_type gstk_non_idle_st_cell_change_brcst_err_rsp(
  const gstk_envelope_cell_broadcast_download_ind_type * cell_brcst_cmd_ptr)
{
   (void) cell_brcst_cmd_ptr; 
   return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_non_idle_st_cell_change_brcst_err_rsp */

/*===========================================================================
FUNCTION gstk_check_cbs_capability

DESCRIPTION
  This function checks the cbs capabilites.

RETURN VALUE
  FALSE

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_check_cbs_capability(void)
{
  return FALSE;
} /* gstk_check_cbs_capability */

