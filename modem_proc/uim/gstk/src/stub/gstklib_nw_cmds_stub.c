/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   L I B R A R Y   N W   C M D S


GENERAL DESCRIPTION

  This source file contains the dummy functions for NW commands that are exposed to GSTK
  client as well as internal GSTK clients.


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/stub/gstklib_nw_cmds_stub.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/23/16   gs      Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk.h"
#include "uim_msg.h"

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_CELL_BROADCAST_DL_COMMAND

DESCRIPTION
  Dummy Functionality

PARAMETERS
  user_data: [Input] User data that client wants GSTK to keep track of
  cb_page: [Input] Pointer indicating the Cell Broadcast page for the download
                    command
  gstk_cb_dl_cb: [Input] Function pointer to which GSTK will send
                             the card response data corresponding to the
                             Cell Broadcast download cmd

DEPENDENCIES
  None

RETURN VALUE
  GSTK_UNSUPPORTED_COMMAND

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called.
  Client is responsible to pad the page to length 88 (31.111)

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_envelope_cell_broadcast_dl_command (
  uint32                     user_data,
  const gstk_cb_page_type   *cb_page_ptr,
  gstk_evt_cb_funct_type     gstk_cb_dl_cb
)
{
  (void) user_data;
  (void) cb_page_ptr;
  (void) gstk_cb_dl_cb;
  
  return GSTK_UNSUPPORTED_COMMAND;
}

/*===========================================================================
FUNCTION GSTK_SLOT_SEND_ENVELOPE_CELL_BROADCAST_DL_COMMAND

DESCRIPTION
  Dummy Functionlity

PARAMETERS
  slot: [Input] The SIM slot ID
  user_data: [Input] User data that client wants GSTK to keep track of
  cb_page: [Input] Pointer indicating the Cell Broadcast page for the download
                    command
  gstk_cb_dl_cb: [Input] Function pointer to which GSTK will send
                             the card response data corresponding to the
                             Cell Broadcast download cmd

DEPENDENCIES
  None

RETURN VALUE
  GSTK_UNSUPPORTED_COMMAND

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called.
  Client is responsible to pad the page to length 88 (31.111)

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_slot_send_envelope_cell_broadcast_dl_command (
  gstk_slot_id_enum_type      slot,
  uint32                      user_data,
  const gstk_cb_page_type   * cb_page_ptr,
  gstk_evt_cb_funct_type      gstk_cb_dl_cb
)
{
  (void) slot;
  (void) user_data;
  (void) cb_page_ptr;
  (void) gstk_cb_dl_cb;
  
  return GSTK_UNSUPPORTED_COMMAND;
} /*gstk_slot_send_envelope_cell_broadcast_dl_command*/

