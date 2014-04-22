/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   L I B R A R Y   D I S P L A Y  S T U B


GENERAL DESCRIPTION

  This source file contains the dummy functions for display related Envelopes that are exposed to GSTK
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

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/stub/gstklib_display_stub.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

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
FUNCTION GSTK_SEND_ENVELOPE_MENU_SEL_COMMAND

DESCRIPTION
  Dummy Functionality

PARAMETERS
  client_id: [Input] Client id of the calling client.  This should be the value
                     returned from the gstk_slot_client_init_ext()
  user_data: [Input] User data that client wants GSTK to keep track of
  item_chosen: [Input] User selected item.
                       Valid value [0x01, 0xFF],
                       0x00 is used to indicate a null item identifier
  help_requested: [Input] Optional data field in the envelope command:
                          TRUE -> help is requested
                          FALSE -> help is not requested and should not
                                   be sent down to the SIM/USIM

DEPENDENCIES
  None

RETURN VALUE
  GSTK_UNSUPPORTED_COMMAND

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called
  Any of the following values will be returned when gstk processes the command after
  dequeuing from the command queue.
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_INVALID_LENGTH,
    GSTK_UNSUPPORTED_COMMAND,
    GSTK_UNKNOWN_COMMAND,
    GSTK_INSUFFICIENT_INFO,
    GSTK_NOT_INIT

SIDE EFFECTS
  Upon calling this function, client should expect to receive
  gstk_general_envelope_rsp_enum_type in its callback function.

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_envelope_menu_sel_command (
        gstk_client_id_type            client_id,
        uint32                         user_data,
        uint8                          item_chosen,
        boolean                        help_requested )
{
  (void) client_id;
  (void) user_data;
  (void) item_chosen;
  (void) help_requested;

  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_send_envelope_menu_sel_command */

/*===========================================================================
FUNCTION GSTK_SEND_ENVELOPE_MENU_SEL_COMMAND_EXT

DESCRIPTION
  Dummy Functionality

PARAMETERS
  client_id     : [Input] Client id of the calling client. This should be the value
                          returned from the gstk_slot_client_init_ext()
  user_data     : [Input] User data that client wants GSTK to keep track of
  item_chosen   : [Input] User selected item.
                          Valid value [0x01, 0xFF],
                          0x00 is used to indicate a null item identifier
  help_requested: [Input] Optional data field in the envelope command:
                          TRUE -> help is requested
                          FALSE -> help is not requested and should not
                                   be sent down to the SIM/USIM
  gstk_raw_env_cb:[Input] Callback function

DEPENDENCIES
  None

RETURN VALUE
  GSTK_UNSUPPORTED_COMMAND

COMMENTS
  This function should only be called after gstk_slot_client_init_ext has been called
  Any of the following values will be returned when gstk processes the command after
  dequeuing from the command queue.
  gstk_status_enum_type:
    GSTK_SUCCESS,
    GSTK_ERROR,
    GSTK_CLIENT_NOT_REGISTERED,
    GSTK_MEMORY_ERROR,
    GSTK_BAD_PARAM,
    GSTK_NULL_INPUT_PARAM,
    GSTK_PARAM_EXCEED_BOUNDARY,
    GSTK_INVALID_LENGTH,
    GSTK_UNSUPPORTED_COMMAND,
    GSTK_UNKNOWN_COMMAND,
    GSTK_INSUFFICIENT_INFO,
    GSTK_NOT_INIT

SIDE EFFECTS
  Upon calling this function, client should expect to receive
  GSTK_MENU_IND_RSP in its callback function.

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_envelope_menu_sel_command_ext (
        gstk_client_id_type            client_id,
        uint32                         user_data,
        uint8                          item_chosen,
        boolean                        help_requested,
        gstk_evt_cb_funct_type         gstk_raw_env_cb)
{
  (void) client_id;
  (void) user_data;
  (void) item_chosen;
  (void) gstk_raw_env_cb;
  (void) help_requested;
  
  return GSTK_UNSUPPORTED_COMMAND;
} /* gstk_send_envelope_menu_sel_command_ext */


/*===========================================================================
FUNCTION   gstk_send_icon_display_rsp

DESCRIPTION
  Library function called by client to say if the icon was displayed.

PARAMETERS
  client_id  : client id
  cmd_ref_id : This is the gstk command reference id in the
               gstk header information sent to the client in the callback
  icon_disp :  yes / no response for whether the icon was displayed

DEPENDENCIES
  estk command buffer is available

RETURN VALUE
  gstk_status_enum_type
    GSTK_CLIENT_NOT_REGISTERED - if client id is not valid
    GSTK_MEMORY_ERROR          - if no free space in ESTK queue
    GSTK_BAD_PARAM             - Bad parameters passed in
    GSTK_SUCCESS               - Command was queued successfully to ESTK

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_send_icon_display_rsp(
  gstk_client_id_type  client_id,
  uint32               gstk_user_data,
  boolean              icon_disp
)
{
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#else /* FEATURE_UIM_TEST_FRAMEWORK */
  (void)client_id;
  (void)gstk_user_data;
  (void)icon_disp;
  return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
}/* gstk_send_icon_display_rsp */

