#ifndef ESTK_BIP_H
#define ESTK_BIP_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      ESTK  SHORT  MESSAGE


GENERAL DESCRIPTION

  This header file contains ESTK functions that handle BIP commands.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2010 - 2016, 2018 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/estk/src/estk_bip.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/31/18   dt      9x05 interface changes for memory reduction
09/15/16   gs      Changes in TP based on RAT and App Type
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
04/14/16   gm      Removal of SCWS code for IOE devices
06/15/15   vr      Remove TR delay logic for CLOSE CHANNEL
11/11/14   shr     Fixed compilation warnings
09/24/14   dy      Use ephemeral ports for BIP session
06/28/14   sw      Display alpha related changes
05/29/14   shr     Cycle through source ports for consecutive BIP sessions
05/06/14   gs      Removed the ESTK featurization
03/03/14   shr     ESTK BIP clean-up and enhancements
02/27/14   dy      Removed ESTK_BIP_MAX_APN_SIZE, already defined in estk_priv.h
02/21/14   dy      Fixed GSTK EFS stub fct bug and added default_apn nv
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
12/05/13   hn      Support DNS query and Channel reuse
11/13/13   shr     Added support for BIP over WLAN
09/13/13   gm      Support for Recovery and hot-swap
07/29/13   gm      Sending response to client when data available envelope fail
05/23/13   hn      Reset auth params after 3GPP create profile
05/07/13   vr      Send data buffer clean-up, if timer expires for send data
08/31/12   sg      Read auth parameter to set it in profile for 3gpp
03/26/12   sg      Create-reuse profile for 3gpp2
02/18/12   av      Cleaned up remaining ALWAYS OFF features
10/06/11   sg      Remove AEESTD APIs
09/11/11   dd      Clear data buffer in send data or listen state
08/24/11   xz      Fix compiler warnings and LINT/KW errors
08/22/11   sg      Clean up after no_sim state
05/05/11   dd      Cache overflow SCWS data in ESTK
04/24/11   dd      SCWS support
02/28/11   sg      Add new state for close in progress
01/04/11   sg      Clean up open channel on gstk timer expiry
12/28/10   sg      Cleanup Open channel proactive command
12/08/10   sg      Port BIP fixes
10/05/10   xz      Use DS naming convention
10/01/10   dd      Add filtering APIs
06/10/10   sg      Save profile number and clean it up on close channel
03/15/10   xz      Add support of LTE and setting bearer parameters in profile
3/12/10    sg      Added slot id to channel type for DS
2/5/10     sg      Include header for using std_strlprintf
1/14/10    sg      Included header file to handle api for alpha TLV in raw format
1/12/10    sg      Added featurization FEATURE_ESTK_BIP_INTERFACE
11/03/09   sg      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "customer.h"
#include "uim_msg.h"
#ifdef FEATURE_ESTK
#include "estk.h"
#include "estk_priv.h"
#include "gstk_exp.h"
#include "estk_cm.h"

#define ESTK_BIP_MAX_BUFFER_SIZE          1500
#define ESTK_BIP_MAX_CH_DATA_SIZE         237
#define ESTK_OOS_TIMER_DURATION           10000
#define ESTK_BIP_KEEP_CH_OPEN_DURATION    30000

#define ESTK_BIP_CHECK_SOCK_FD(x)                                              \
    if ((x) == DSS_ERROR)                                                      \
    {                                                                          \
      UIM_MSG_ERR_0("Invalid sockfd!");                                        \
      return ESTK_ERROR;                                                       \
    }

#define ESTK_BIP_SET_SLOT_INDEX_MASK(slot_id, index, mask, status)           \
  if (!ESTK_IS_VALID_SLOT_ID(slot_id))                                       \
  {                                                                          \
    UIM_MSG_ERR_1("Invalid Slot ID: 0x%x",(slot_id));                        \
    (status) = FALSE;                                                        \
  }                                                                          \
  (mask) = ((((uint16)(index)) << 16) | ((uint16)(slot_id)));                \
  (status) = TRUE;

#define ESTK_BIP_GET_SLOT_FROM_MASK(mask, slot_id)                           \
  (slot_id) = (gstk_slot_id_enum_type)((mask) & 0xFFFF);

#define ESTK_BIP_GET_INDEX_FROM_MASK(mask, index)                            \
  (index) = ((mask) >> 16);

typedef enum
{
  ESTK_LOCAL_ADDRESS                  = 0x00,
  ESTK_DESTINATION_ADDRESS            = 0x01,
  ESTK_ADDRESS_NONE                   = 0xFF
}estk_address_type;

typedef enum
{
  ESTK_BIP_APN_INVALID                = 0x00,
  /* APN from OPEN CHANNEL used */
  ESTK_BIP_APN_OPEN_CH                = 0x01,
  /* APN from Default APN NV used */
  ESTK_BIP_APN_NV_73530               = 0x02,
  /* APN corresponding to default DS profile used */
  ESTK_BIP_APN_DEFAULT_PROFILE        = 0x03,
  /* Profile always_share to DS -DS chooses profile */
  ESTK_BIP_APN_DS_SHARING             = 0x04,
  ESTK_BIP_APN_MAX                    = 0xFF
}estk_bip_apn_select_method_type;

/*===========================================================================

FUNCTION: estk_bip_channel_table_init

DESCRIPTION :
  This function sets bip_channel_table fields with non-zero initial values

PARAMETERS:
  instance: the estk_instances_ptr instance to be initialized

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_channel_table_init(
  uint32 instance);

/*===========================================================================

FUNCTION: estk_bip_clean_up_ch_table

DESCRIPTION :
  Free memory allocated for the BIP channel

PARAMETERS:
  index : [Input] Index of the channel to be cleaned

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_clean_up_ch_table(
  int index);

/*===========================================================================

FUNCTION: estk_bip_cellular_oos_timer_cb

DESCRIPTION :
  The callback funciton supplied to Cellular OOS timer

PARAMETERS:
  slot_index: Index of the slot corresponding to the timer

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_cellular_oos_timer_cb(
  uint32 slot_index);

/*===========================================================================
FUNCTION: estk_process_open_ch_req

DESCRIPTION:
  GSTK calls this ESTK function to process the OPEN CHANNEL request.

PARAMETERS:
  gstk_req_ptr : [Input] gstk OPEN CHANNEL cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type
    ESTK_SUCCESS: - When SMS is successfully sent
    ESTK_ERROR:   - When there is failure in sending SMS to WMS module

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_open_ch_req(
  gstk_cmd_from_card_type *gstk_req_ptr);

/*===========================================================================
FUNCTION: estk_convert_ch_id_to_index

DESCRIPTION:
  Find out index in bip_channel_table based on channel id

PARAMETERS:
  ch_id :   [Input] Channel id whose index has to found
  index :   [Input] Result that contains the mapped index

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_convert_ch_id_to_index (
  int32                   ch_id,
  int32                  *index
);

/*===========================================================================
FUNCTION: estk_process_send_data_req

DESCRIPTION:
  Process the Send Data Toolkit Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] gstk SEND DATA cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_send_data_req (
  gstk_cmd_from_card_type *gstk_req_ptr);

/*===========================================================================
FUNCTION: estk_process_receive_data_req

DESCRIPTION:
  Process the Receive Data Toolkit Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] gstk RECEIVE DATA cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_receive_data_req (
  gstk_cmd_from_card_type *gstk_req_ptr);

/*===========================================================================
FUNCTION: estk_send_receive_data_terminal_response

DESCRIPTION:
  Send TR for RECEIVE DATA Toolkit Request from the card

PARAMETERS:
  cmd_ref_id : [Input] ref_id of the Receive Data request

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_send_receive_data_terminal_response(uint32 cmd_ref_id);

/*===========================================================================
FUNCTION: estk_process_get_ch_status_req

DESCRIPTION:
  Process the Get Channel Status Toolkit Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] gstk GET CHANNEL STATUS cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_get_ch_status_req (
  gstk_cmd_from_card_type *gstk_req_ptr);

/*===========================================================================
FUNCTION: estk_process_close_ch_req

DESCRIPTION:
  Process the Close Channel Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] gstk CLOSE CHANNEL cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_close_ch_req (
  gstk_cmd_from_card_type *gstk_req_ptr);

/*===========================================================================

FUNCTION: estk_process_bip_user_cnf_rsp

DESCRIPTION :
  This function will be called by ESTK to process the user confirmation response
  received from the UI client.If the user confirmed for an affirmative action,
  the call will be set up, otherwise an appropriate Terminal Response will be
  sent to the card saying "The user rejected the call setup".
  If the user fails to respond within a certain time, a Terminal response of
  "No response from the user will be sent to the card.

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_bip_user_cnf_rsp(
  const estk_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION: estk_bip_process_sock_evt

DESCRIPTION :
  This function processes a socket event from DS

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bip_process_sock_evt(
  const estk_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION: estk_perform_net_connect

DESCRIPTION :
  This function established the link

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_perform_net_connect(sint15 dss_nethandle);

/*===========================================================================

FUNCTION: estk_perform_net_disconnect

DESCRIPTION :
  This function tears down the link and takes care of releasing resources

PARAMETERS:
  dss_nethandle : [Input] nethandle to disconnect

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_perform_net_disconnect(
  sint15 dss_nethandle);

/*===========================================================================

FUNCTION: estk_process_net_cb_cmd

DESCRIPTION :
  This function processes the net callback result in ESTK context

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_net_cb_cmd(
  const estk_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION: estk_bip_bringup_call

DESCRIPTION :
  This function brings up call

PARAMETERS:
  index          : [Input] BIP channel index
  address_family : [Input] Address family type

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bip_bringup_call(
  uint32 index,
  uint16 address_family);

/*===========================================================================
FUNCTION: estk_process_setup_evt_list_req

DESCRIPTION:
  Process the Set Up Event List Toolkit Request from the card

PARAMETERS:
  gstk_req_ptr : [Input] gstk SET UP EVENT LIST cmd pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_setup_evt_list_req (
  gstk_cmd_from_card_type *gstk_req_ptr);

/*===========================================================================
FUNCTION: estk_process_cellular_oos_evt

DESCRIPTION:
  Processes the Cellular OOS event received from Data Services (via GSTK).
  If Location Status (from MM IDLE/CM SS event) is Normal Service,
  function starts a 10 sec timer to wait for Location status to change,
  else sends Link Dropped Envelope as required.

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_cellular_oos_evt(void);

/*===========================================================================
FUNCTION: estk_process_cellular_status_change_evt

DESCRIPTION :
  This function processes a Cellular Status Change notification from GSTK

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_cellular_status_change_evt(
  const estk_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION:   estk_open_ch_cleanup

DESCRIPTION:
  This ESTK function clears the open channel related private data.

PARAMETERS:
  None

DEPENDENCIES:
  open_ch_ptr : [Input] Open Channel pointer to clean-up

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_open_ch_cleanup(
  gstk_open_ch_req_type *open_ch_ptr
);

/*===========================================================================
FUNCTION:   estk_clean_up_bip_open_ch_timer_exp

DESCRIPTION:
  This ESTK function cleans up ESTK globals on open channel timer expiry

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_clean_up_bip_open_ch_timer_exp(void);

/*===========================================================================
FUNCTION:   estk_clean_up_bip_send_data_timer_exp

DESCRIPTION:
  This ESTK function cleans up send data buffer on send data timer expiry

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_clean_up_bip_send_data_timer_exp(void);

/*===========================================================================
FUNCTION:   estk_bip_full_clean_up

DESCRIPTION:
  This ESTK function cleans up entire BIP table

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_full_clean_up(void);

/*===========================================================================
FUNCTION:   estk_bip_full_clean_up_slot

DESCRIPTION :
  Free memory allocated for the BIP channel

PARAMETERS:
  slot_id : [Input] slot on which the BIP channel to be cleaned

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_full_clean_up_slot(gstk_slot_id_enum_type slot_id);

/*===========================================================================
FUNCTION: estk_bip_reset_curr_cmd_id_for_channel

DESCRIPTION:
  Clears the current command ID in the BIP channel table

PARAMETERS:
  index:      [Input] Index of BIP channel table for which the Current
                      command ID needs to be cleared. Clear only if index
                      is in valid range

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_bip_reset_curr_cmd_id_for_channel(
  uint32 index);

/*===========================================================================

FUNCTION estk_bip_send_terminal_response

DESCRIPTION:
  This function is served as a utility function to the estk client, so that
  the client can call this function in sending a terminal response to
  a BIP proactive command.

  The function resets the current command ID stored in the BIP channel table
  index (only if a valid index is passed in).

PARAMETERS:
  cmd_detail_ref_id:      [Input] Specifies the reference ID for command details of
                                  the proactive command that the client is sending
                                  a terminal response to.  The reference id value
                                  is sent to the client earlier in the structure
                                  of the proactive command request, inside the
                                  gstk_exp_hdr_type
  terminal_response_type: [Input] Specifies the _CNF for the response to
                                  the proactive cmd that the client wants to
                                  notify SIM/USIM
  general_result:         [Input] Specifies the result of the proactive command
                                  operation
  additional_result_ptr:  [Input] Pointer specifies the optional additional result
                                  for the proactive command, the length field in
                                  the gstk_additonal_info_type specifies number of
                                  additional_info in the structure
  other_info_ptr:         [Input] A union pointer of various extra parameters for the
                                  various proactive command response, please refer to
                                  gstk_terminal_rsp_param_type for details
  bip_channel_index:      [Input] Index of BIP channel table for which the Current
                                  command ID needs to be cleared. Clear only if index
                                  is in valid range

DEPENDENCIES:
  None

RETURN VALUE:
  estk_status_enum_type:
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  This function should only be called after estk_client_init has been called

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_bip_send_terminal_response (
  uint32                                     cmd_detail_ref_id,
  gstk_cmd_enum_type                         terminal_response_type,
  gstk_general_result_enum_type              general_result,
  gstk_additional_info_ptr_type              *additional_result_ptr,
  gstk_terminal_rsp_extra_param_type         *other_info_ptr,
  uint32                                     bip_channel_index
);

/*===========================================================================
FUNCTION: estk_process_keep_ch_open_expired_evt

DESCRIPTION:
  Process the keep ch open expired event

PARAMETERS:
  ch_idx : [Input] The channel index of which the link is to be torn down

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_keep_ch_open_expired_evt(
  uint32 ch_idx
); /* estk_process_keep_ch_open_expired_evt */

/*===========================================================================

FUNCTION: estk_process_scws_ch_status_req

DESCRIPTION :
  This function processes Ch Status req from apps

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_ch_status_req(
  const estk_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION: estk_process_scws_data_avail_req

DESCRIPTION :
  This function processes data available req

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_data_avail_req(
  estk_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION: estk_process_scws_open_ch_rsp

DESCRIPTION :
  This function tears down the link and takes care of releasing resources

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_open_ch_rsp(
  const estk_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION: estk_process_scws_close_ch_rsp

DESCRIPTION :
  This function processes close channel response from the apps

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_close_ch_rsp(
  const estk_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION: estk_process_scws_send_data_rsp

DESCRIPTION :
  This function processes send data response from the apps

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_send_data_rsp(
  const estk_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION: estk_scws_populate_open_ch_table

DESCRIPTION :
  This function populates information into the Open channel Table.

PARAMETERS:
  index : [Input] Index of the channel to be populated

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_scws_populate_open_ch_table(
  int32 index);

/*===========================================================================
FUNCTION: estk_scws_send_data

DESCRIPTION:
   This function sends the SCWS Send Data req to the Agent

PARAMETERS:
  index     - Index into the bip ch table
  cmd_ref_id - Cmd ref id for terminal response
DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_scws_send_data(
  int32               index,
  uint32              cmd_ref_id
);  

/*===========================================================================

FUNCTION: estk_scws_send_data_avail_rsp

DESCRIPTION :
  This function sends the data available response

PARAMETERS:
  index : [Input] Index to the estk channel table

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_scws_send_data_avail_rsp (
  uint32 index, boolean result);

/*===========================================================================
FUNCTION: estk_check_scws_capability

DESCRIPTION :
  Checks the SCWS capability

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE
  FALSE

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_check_scws_capability(void);

#endif /* FEATURE_ESTK */
#endif /* ESTK_BIP_H */

