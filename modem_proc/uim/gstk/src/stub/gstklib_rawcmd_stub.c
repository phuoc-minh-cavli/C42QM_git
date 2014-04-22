/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   L I B R A R Y  R A W C M D S


GENERAL DESCRIPTION

  This source file contains the stub functions for raw TR and Envelope that are exposed to GSTK
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

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/stub/gstklib_rawcmd_stub.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

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
FUNCTION gstk_send_raw_terminal_response

DESCRIPTION
  Dummy Functionality

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the
                     value returned from the gstk_slot_client_init_ext()
  user_data: [Input] User data that client wants GSTK to keep track of
  cmd_detail_ref_id: [Input] Specifies the reference ID for command details of
                             the proactive command that the client is sending
                             a terminal response to.  The reference id value
                             is sent to the client earlier in the structure
                             of the proactive command request, inside the
                             gstk_exp_hdr_type
 term_rsp: [Input] The raw terminal response that the client wants to send

DEPENDENCIES
  None

RETURN VALUE
  GSTK_UNSUPPORTED_COMMAND

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_raw_terminal_response (
  gstk_client_id_type                     client_id,
  uint32                                  user_data,
  uint32                                  cmd_detail_ref_id,
  gstk_generic_data_type                  term_resp)
{
  (void) client_id;
  (void) user_data;
  (void) cmd_detail_ref_id;
  (void) term_resp;
  
  return GSTK_UNSUPPORTED_COMMAND;
} /*gstk_send_raw_terminal_response*/


/*===========================================================================
FUNCTION GSTK_SEND_RAW_ENVELOPE_COMMAND

DESCRIPTION
  Dummy Functionality

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the value
                     returned from the gstk_slot_client_init_ext()
  user_data: [Input] User data that client wants GSTK to keep track of
  env_cmd:   [Input] The envelope command data to be sent to SIM/USIM
  env_cmd_cb:[Input] Callback function

DEPENDENCIES
  None

RETURN VALUE
  GSTK_UNSUPPORTED_COMMAND

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called

SIDE EFFECTS
  Upon calling this function, client should expect to receive
  gstk_raw_envelope_rsp_type in its callback function.

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_raw_envelope_command (
  gstk_client_id_type          client_id,
  uint32                       user_data,
  gstk_generic_data_type       env_cmd,
  gstk_evt_cb_funct_type       gstk_raw_env_cb)
{
  (void) client_id;
  (void) user_data;
  (void) env_cmd;
  (void) gstk_raw_env_cb;
  
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_send_raw_envelope_command */

