/*===========================================================================

      U I M D I A G   M M G S D I   S T U B S   D E F I N I T I O N S        

===========================================================================*/
/**
@file uimdiag_mmgsdi_stubs.c
@brief This library contains stubs functions of uimdiag mmgsdi functions
*/

/*=============================================================================
Copyright (c) 2017 - 2018 by QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header:

when       who   what, where, why
--------   ---   -----------------------------------------------------------
10/31/18   tq    Removal of MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_REQ
09/15/17   rps   Initial Revision


=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uimdiag.h"

/*=============================================================================

                              FUNCTIONS

=============================================================================*/

/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_REG_CMD

DESCRIPTION
  Command handler for client id and evt reg subsystem command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_client_id_and_evt_reg_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_client_id_and_evt_reg_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_OPEN_CMD

DESCRIPTION
  Command handler for session open subsystem command

DEPENDENCIES
  None.

RETURN VALUE
  void pointer

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_open_cmd (
  PACKED void *req_ptr,
  word        pkt_len
)
{
  (void)req_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_open_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_CLIENT_ID_AND_EVT_DEREG_CMD

DESCRIPTION
  Command handler for client id and evt dereg subsystem command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_client_id_and_evt_dereg_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_client_id_and_evt_dereg_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_CLOSE_CMD

DESCRIPTION
  Command handler for session close subsystem command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_close_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_close_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_ACT_OR_SW_PROV_CMD

DESCRIPTION
  Command handler for session activate or Switch Provisioning

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_act_or_sw_prov_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_act_or_sw_prov_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_INFO_CMD

DESCRIPTION
  Command handler for session get info subsystem command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_info_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_get_info_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_VERIFY_PIN_CMD

DESCRIPTION
  Command handler to perform a PIN verification procedure against a card for
  PIN1, PIN2 or UPIN.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_verify_pin_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_verify_pin_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_SESSION_GET_FILE_ATTR_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to get file attr

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_file_attr_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_get_file_attr_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_SESSION_READ_RECORD_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to read record

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_read_record_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_read_record_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_SESSION_READ_FROM_CACHE_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to read record

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_read_from_cache_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_read_from_cache_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_SESSION_READ_TRANSPARENT_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to read transparent file

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_read_transparent_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_read_transparent_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_SESSION_WRITE_RECORD_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to write record

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_write_record_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_write_record_cmd */



/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_SESSION_WRITE_TRANSPARENT_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to write to transparent file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_write_transparent_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_write_transparent_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_SESSION_DEACT_PROV_CMD

DESCRIPTION
  Command handler to build a request that allows the client to deactivate
  an application.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_deact_prov_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_deact_prov_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_APP_CAP_CMD

DESCRIPTION
  Command handler to build a request that allows the client to get application
  capabilities.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_app_cap_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_get_app_cap_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC_CMD

DESCRIPTION
This function allows a client to query MMGSDI for session information by
Session ID or Session Type. A client can get Session Type, Slot Number,
Channel ID and AID using this request.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_info_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_get_info_sync_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_APP_CAPABILITIES_SYNC_CMD

DESCRIPTION
  Command handler to extract all the provisioning application
  capabilities like fdn, bdn, acl, imsi status and phone book related.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_app_capabilities_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_get_app_capabilities_sync_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_GET_ALL_AVAIL_APPS_SYNC_EXT_CMD

DESCRIPTION
  Command handler to get all available apps.

DEPENDENCIES
  None.

RETURN VALUE
  void pointer.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_get_all_avail_apps_sync_ext_cmd (
  PACKED void  *cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_get_all_avail_apps_sync_ext_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_CARD_PDOWN_CMD

DESCRIPTION
  Command handler to extract data for CARD PDOWN

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_card_pdown_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_card_pdown_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_CARD_PUP_CMD

DESCRIPTION
  Command handler to extract data for CARD PUP

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_card_pup_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_card_pup_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_SIM_CMD

DESCRIPTION
  Command handler to extract data for ACTIVATE ONCHIP SIM

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_activate_onchip_sim_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_activate_onchip_sim_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_ACTIVATE_ONCHIP_USIM_CMD

DESCRIPTION
  Command handler to extract data for ACTIVATE ONCHIP USIM

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_activate_onchip_usim_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_activate_onchip_usim_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SEND_APDU_CMD

DESCRIPTION
  Command handler to send apdus

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_send_apdu_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_send_apdu_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_GET_ATR_CMD

DESCRIPTION
  Command handler to extract data for GET ATR

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_get_atr_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_get_atr_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_REG_FOR_REFRESH_PROCEEED_VOTING_CMD

DESCRIPTION
  Command handler to build a request to register for Refresh to MMGSDI aling
  with proceed voting

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_reg_for_refresh_proceed_voting_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_reg_for_refresh_proceed_voting_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_OK_TO_PROCEED_WITH_REFRESH_CMD

DESCRIPTION
  Command handler to build a request to simulate proceed with Refresh to MMGSDI

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_ok_to_proceed_with_refresh_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_ok_to_proceed_with_refresh_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_REG_FOR_REFRESH_CMD

DESCRIPTION
  Command handler to build a request to register for Refresh to MMGSDI

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_reg_for_refresh_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_reg_for_refresh_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_DEREG_FOR_REFRESH_CMD

DESCRIPTION
  Command handler to build a request to deregister files that were registered
  for Refresh to MMGSDI

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_dereg_for_refresh_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_dereg_for_refresh_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_OK_TO_REFRESH_CMD

DESCRIPTION
  Command handler to build a request to simulate OK to Refresh to MMGSDI

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_ok_to_refresh_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_ok_to_refresh_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_REFRESH_COMPLETE_CMD

DESCRIPTION
  Command handler to build a request to indicate to MMGSDI that the client
  has finished its refresh process

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_refresh_complete_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_refresh_complete_cmd */


/*===========================================================================

FUNCTION uimdiag_mmgsdi_get_all_available_apps_async_cmd

DESCRIPTION
  Command handler for get all available apps async

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_get_all_available_apps_async_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_get_all_available_apps_async_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_DISABLE_PIN_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to disable the PIN provided

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_disable_pin_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_disable_pin_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_ENABLE_PIN_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to enable the PIN provided

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_enable_pin_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_enable_pin_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_CHANGE_PIN_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to change the PIN provided

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_change_pin_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_change_pin_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_UNBLOCK_PIN_CMD

DESCRIPTION
  Command handler to build a request to MMGSDI to unblock the PIN provided

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_unblock_pin_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_unblock_pin_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_ALL_PIN_STATUS_CMD

DESCRIPTION
  Command handler to request the PIN status for all relevant PINs applicable
  to the session ID provided.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_all_pin_status_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_get_all_pin_status_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_DISABLE_SERVICE_CMD

DESCRIPTION
  Command handler for disabling a particular service

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_disable_service_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_disable_service_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_ENABLE_SERVICE_CMD

DESCRIPTION
  Command handler for enable service

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_enable_service_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_enable_service_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_IS_SERVICE_AVAIL_CMD

DESCRIPTION
  Command handler to determine if service is available or not.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_is_service_avail_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_is_service_avail_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_OPERATOR_NAME_TABLE_CMD

DESCRIPTION
  Command handler for getting operator Name(s)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_operator_name_table_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_get_operator_name_table_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_READ_PRL_CMD

DESCRIPTION
  Gets the IS-683-C or IS-683-A PRL from either the EF Extended
  PRL (EF-EPRL) or the EF-PRL when the EF-EPRL does not contain a valid PRL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_read_prl_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_read_prl_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_HIDE_CSIM_CMD

DESCRIPTION
  Command handler to extract data for HIDE CSIM CMD

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_hide_csim_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_hide_csim_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_MAP_SESSIONS_TO_SLOTS_CMD

DESCRIPTION
  Command handler to extract data for MAP SESSIONS TO SLOTS CMD

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_map_sessions_to_slots_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_map_sessions_to_slots_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_NAA_REFRESH_EXT_CMD

DESCRIPTION
  Command handler to build a request to trigger for Refresh to MMGSDI with refresh enforcement policy

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_naa_refresh_ext_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_naa_refresh_ext_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_CPHS_INFO_CMD

DESCRIPTION
  Command handler to build a request to get all CPHS related information

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_cphs_info_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_get_cphs_info_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_ISIM_AUTH_CMD

DESCRIPTION
  Command handler for ISIM Authenticate

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_isim_auth_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_isim_auth_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_SEND_CARD_STATUS_CMD

DESCRIPTION
  Command handler for getting content of current directory

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_send_card_status_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_send_card_status_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_SESSION_SEEK_CMD

DESCRIPTION
  Command handler to perform GSM Seek command on card

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_seek_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_seek_cmd */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_SESSION_SEARCH_CMD

DESCRIPTION
  Command handler to performs Search command on UICC card.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_search_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_search_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_COMPUTE_IP_AUTH_CMD

DESCRIPTION
  compute IP  authentication for 3GPD SIMPLE IP CHAP, Mobile IP, or HRPD CHAP
  Authentication.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_compute_ip_auth_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_compute_ip_auth_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_SSD_UPDATE_CMD

DESCRIPTION
  Used to initiate the calculation of a new SSD value
                    for authentication purposes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_ssd_update_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_ssd_update_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_CONFIRM_SSD_CMD

DESCRIPTION
  Performs a Confirm SSD command for authentication.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_confirm_ssd_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_confirm_ssd_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_BS_CHAL_CMD

DESCRIPTION
  Used to perform an OTASP base station challenge before
  updating its SSD to ensure that the update process is being initiated by a
  legitimate source

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_bs_chal_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_bs_chal_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_READ_CACHE_FILE_SIZE_CMD

DESCRIPTION
  Command handler to get the size of the file to be cached for a read operation.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_read_cache_file_size_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_read_cache_file_size_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_READ_CACHE_CMD

DESCRIPTION
  Command handler to read the cached file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_read_cache_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_read_cache_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_WRITE_CACHE_CMD

DESCRIPTION
  Command handler to write data to a cached file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_write_cache_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_write_cache_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_SYNC_CMD

DESCRIPTION
  Command handler to notify UIM that the subscription on the card is illegal and is to be blocked.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_illegal_subscription_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_illegal_subscription_sync_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_TOGGLE_UIM_PWR_CTRL_SYNC_CMD

DESCRIPTION
  Command handler to notify UIM when the modem is in or out of the traffic channel.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_toggle_uim_pwr_ctrl_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_toggle_uim_pwr_ctrl_sync_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SAP_GET_ATR_CMD

DESCRIPTION
  Command handler to extract data for SAP GET ATR

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_get_atr_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_sap_get_atr_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SAP_SEND_APDU_CMD

DESCRIPTION
  Command handler to extract data for SAP SEND APDU

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_send_apdu_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_sap_send_apdu_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SAP_POWER_ON_CMD

DESCRIPTION
  Command handler to extract data for SAP POWER ON

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_power_on_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_sap_power_on_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SAP_POWER_OFF_CMD

DESCRIPTION
  Command handler to extract data for SAP POWER OFF

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_power_off_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_sap_power_off_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SAP_POWER_CARD_READER_STATUS_CMD

DESCRIPTION
  Command handler to extract data for SAP CARD READER STATUS

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_card_reader_status_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_sap_card_reader_status_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SAP_RESET_CMD

DESCRIPTION
  Command handler to extract data for SAP RESET

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_reset_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_sap_reset_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SAP_CONNECT

DESCRIPTION
  Command handler to extract data for SAP CONNECT

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_connect_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_sap_connect_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SAP_DISCONNECT_CMD

DESCRIPTION
  Command handler to extract data for SAP DISCONNECT

DEPENDENCIES
  None.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_sap_disconnect_cmd(
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_sap_disconnect_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_NAA_REFRESH_CMD

DESCRIPTION
  Command handler to build a request to trigger for Refresh to MMGSDI

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_naa_refresh_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_naa_refresh_cmd */


/*===========================================================================

FUNCTION uimdiag_mmgsdi_get_all_available_apps_sync_cmd

DESCRIPTION
  Command handler to get all available apps.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_get_all_available_apps_sync_cmd (
  PACKED void *cmd_ptr,
  word        pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_get_all_available_apps_sync_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_IS_SERVICE_AVAILABLE_SYNC_CMD

DESCRIPTION
  Command handler for checking if a particular service is available. (Sync API)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_is_service_available_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_is_service_available_sync_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_OPEN_EXT_CMD

DESCRIPTION
  Command handler for multiple session open subsystem command

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_open_ext_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_open_ext_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_NVRUIM_DATA_SYNC_CMD

DESCRIPTION
This function allows a client to query MMGSDI to get nvruim state information and part of
nvruim's global cache. This is a synchronous call.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_get_nvruim_data_sync_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_get_nvruim_data_sync_cmd */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_SESSION_GET_INFO_SYNC_CMD

DESCRIPTION
  Command handler to determine if service is available or not.

DEPENDENCIES
  None.

RETURN VALUE
# if value is 1 , then uimdiag is supported, if value is 2, GSDIDIAG is supported.
# Preference and by default behaviour is UIMDIAG support.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_check_diag_support_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_check_diag_support_cmd */

