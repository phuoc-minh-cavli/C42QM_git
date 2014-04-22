/*===========================================================================

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2018 - 2019 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.3.1/uimdiag/src/uimdiag_mmgsdi_refresh_only.c#1 $$ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who   what, where, why
--------   ---   -----------------------------------------------------------
06/13/19   bcho  Rel 12 refresh enforcement policy support
07/25/18   bcho  Initial Revision

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#include "uimdiag_util.h"
#include "mmgsdilib_common.h"
#include "mmgsdilib.h"
#include "mmgsdilib_v.h"
#include "comdef.h"
#include "uimdiag.h"
#include "uimdiag_mmgsdi.h"
#include "uimdiag_mmgsdi_util.h"

#include "intconv.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "event.h" /* event_report function */
#include "rex.h"


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_REFRESH_CNF

DESCRIPTION
  Function to populate the response pointer with the confirmation data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_refresh_cnf(
  uimdiag_generic_rsp_type      *rsp_ptr,
  const mmgsdi_cnf_type         *cnf_data_ptr
)
{
  UIMDIAG_UTIL_RETURN_IF_NULL(cnf_data_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(rsp_ptr);

  (void)memset(
    (void*)&rsp_ptr->rsp_payload.response.session_refresh,
      0x00, sizeof(uimdiag_mmgsdi_sessionid_type));

  rsp_ptr->rsp_payload.response.session_refresh.session_id_low =
    UIMDIAG_LODWORD((cnf_data_ptr->refresh_cnf.response_header.session_id));

  rsp_ptr->rsp_payload.response.session_refresh.session_id_high =
    UIMDIAG_HIDWORD(
      (cnf_data_ptr->refresh_cnf.response_header.session_id));
  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_refresh_cnf */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_GENERIC_RSP_CB

DESCRIPTION
  Generic comand callback function, handles all mmgsdi command cnfs and
  prepares diag response packet

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void uimdiag_mmgsdi_generic_rsp_cb(
  mmgsdi_return_enum_type       mmgsdi_status,
  mmgsdi_cnf_enum_type          cnf_type,
  const mmgsdi_cnf_type       * cnf_data_ptr
)
{
  uimdiag_generic_rsp_type   *rsp_ptr                 = NULL;
  uint32                      len                     = 0;
  int32                       index                   = 0;
  boolean                     index_valid             = FALSE;
  uimdiag_return_enum_type    uimdiag_return          = UIMDIAG_ERROR;
  uint32                      temp_dynamic_len        = 0;
  mmgsdi_available_apps_type *available_apps_info_ptr = NULL;

  UIMDIAG_MSG_HIGH_2("uimdiag generic_rsp_cb: CNF Type is 0x%x & mmgsdi status is 0x%x",
                     cnf_type, mmgsdi_status);

  if (( cnf_data_ptr == NULL )||
      ( uimdiag_req_table_ptr == NULL ))
  {
    UIMDIAG_MSG_ERROR_0("Invalid cnf data or uimdiag_req_table_ptr is NULL");
    return;
  }

  len         = sizeof( uimdiag_generic_rsp_type );
  index       = (int)cnf_data_ptr->response_header.client_data;
  index_valid = UIMDIAG_REQ_TABLE_IS_INDEX_VALID(index);

  if ( FALSE == index_valid )
  {
    UIMDIAG_MSG_ERROR_0("Invalid client data, index is invalid");
    return;
  }

  UIMDIAG_MSG_HIGH_1("Length to be allocated is %d", len);

  /* Allocate memory for the delayed response pointer */
  rsp_ptr =(uimdiag_generic_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
            (diagpkt_subsys_id_type)DIAG_SUBSYS_UIM,
            (diagpkt_subsys_cmd_code_type)uimdiag_req_table_ptr[index].subcmd,
            (diagpkt_subsys_delayed_rsp_id_type)
            uimdiag_req_table_ptr[index].rsp_id,len );

  if(rsp_ptr == NULL)
  {
    /*do memory allocation for uimdiag_generic_rsp_type only*/
    len = sizeof( uimdiag_generic_rsp_type );

    rsp_ptr =(uimdiag_generic_rsp_type  * )diagpkt_subsys_alloc_v2_delay(
          (diagpkt_subsys_id_type)DIAG_SUBSYS_GSDI,
          (diagpkt_subsys_cmd_code_type)uimdiag_req_table_ptr[index].subcmd,
          (diagpkt_subsys_delayed_rsp_id_type)
          uimdiag_req_table_ptr[index].rsp_id,len );

    if(rsp_ptr == NULL)
    {
      uimdiag_free_index(index);
      return;
    }
    mmgsdi_status = MMGSDI_ERROR;

    diagpkt_subsys_set_status(rsp_ptr, (uint32) UIMDIAG_SUCCESS);
    rsp_ptr->rsp_payload.mmgsdi_status = mmgsdi_status;
    diagpkt_delay_commit(rsp_ptr);
    uimdiag_free_index(index);
    return;
  }

  memset(&rsp_ptr->rsp_payload,0x00, sizeof(uimdiag_cmd_rsp_pyld_type));

  diagpkt_subsys_set_status(rsp_ptr, (uint32) UIMDIAG_SUCCESS);
  rsp_ptr->rsp_payload.mmgsdi_status = mmgsdi_status;

  if (MMGSDI_SUCCESS != mmgsdi_status )
  {
    diagpkt_delay_commit(rsp_ptr);
    uimdiag_free_index(index);
    return;
  }

  switch(cnf_type)
  {
    case MMGSDI_REFRESH_CNF:
      uimdiag_return =
        uimdiag_mmgsdi_refresh_cnf(rsp_ptr,cnf_data_ptr);
      break;

    default:
      UIMDIAG_MSG_ERROR_1("Unexpected confiramtion type: %x ", cnf_type);
      uimdiag_return = UIMDIAG_ERROR;
      break;
  }

  if (uimdiag_return != UIMDIAG_SUCCESS )
  {
    UIMDIAG_MSG_ERROR_1("CNF: 0x%x handling failed", cnf_type);
    rsp_ptr->rsp_payload.mmgsdi_status = MMGSDI_ERROR;
  }

  uimdiag_free_index(index);
  diagpkt_delay_commit(rsp_ptr);
}/* uimdiag_mmgsdi_generic_rsp_cb */


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

FUNCTION UIMDIAG_MMGSDI_SESSION_PROV_APP_INIT_COMPLETE_CMD

DESCRIPTION
  Notifies the MMGSDI that the client has finished its initialization procedure
  for the application.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * uimdiag_mmgsdi_session_prov_app_init_complete_cmd (
  PACKED void * cmd_ptr,
  word          pkt_len
)
{
  (void)cmd_ptr;
  (void)pkt_len;
  return NULL;
} /* uimdiag_mmgsdi_session_prov_app_init_complete_cmd */


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

FUNCTION UIMDIAG_MMGSDI_FREE_CMD_DATA

DESCRIPTION
  Function to free up allocated memory to command data

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void uimdiag_mmgsdi_free_cmd_data(
  uimdiag_generic_req_type              * req_ptr,
  uimdiag_subsystem_cmd_enum_type         subsys_cmd
)
{
  if (req_ptr == NULL )
  {
    UIMDIAG_MSG_ERROR_0("Invalid req ptr");
    return;
  }

  switch(subsys_cmd)
  {
    case UIMDIAG_MMGSDI_NAA_REFRESH_CMD:
      UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_trigger_refresh.sim_app_id);
      UIMDIAGUTIL_TMC_MEM_FREE(req_ptr->request.session_trigger_refresh.path);
      break;

    default:
      break;
  }
} /* uimdiag_mmgsdi_free_cmd_data */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_PARSE_COMMANDS

DESCRIPTION
  Function to preparse and populate the response pointer with the conformation
  data

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_parse_commands(
  PACKED void                           * cmd_ptr,
  uint16                                  pkt_len,
  uimdiag_generic_rsp_type             ** rsp_pptr,
  diagpkt_subsys_delayed_rsp_id_type    * rsp_id_ptr,
  int32                                 * index_ptr,
  uimdiag_generic_req_type              * request_ptr,
  uimdiag_subsystem_cmd_enum_type         subsys_cmd
)
{
  uimdiag_return_enum_type              uimdiag_status         = UIMDIAG_ERROR;

  UIMDIAG_UTIL_RETURN_IF_NULL_3(rsp_pptr, rsp_id_ptr, request_ptr);

  /*  allocate immediate response pointer  */
  *rsp_pptr = (uimdiag_generic_rsp_type *)
                diagpkt_subsys_alloc_v2(
                  (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
                  (diagpkt_subsys_cmd_code_type) subsys_cmd,
                  sizeof(uimdiag_generic_rsp_type));

  if (*rsp_pptr == NULL)
  {
    return UIMDIAG_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  memset(&(*rsp_pptr)->rsp_payload,0x00, sizeof(uimdiag_cmd_rsp_pyld_type));

  /* Initialize to MMGSDI_ERROR for the immediate response */
  (*rsp_pptr)->rsp_payload.mmgsdi_status = MMGSDI_ERROR;

  *rsp_id_ptr = diagpkt_subsys_get_delayed_rsp_id(*rsp_pptr );

  /* extract data from req packet
  and populate fields in the request structure */
  uimdiag_status = uimdiag_parse_cmd_data(request_ptr, cmd_ptr, pkt_len);

  if (uimdiag_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    /* Do an immediate rsp - No delayed response expected*/
    diagpkt_subsys_reset_delayed_rsp_id(*rsp_pptr);
    diagpkt_subsys_set_status(*rsp_pptr, (uint32) UIMDIAG_SUCCESS);
    diagpkt_commit(*rsp_pptr);
    uimdiag_mmgsdi_free_cmd_data(request_ptr, subsys_cmd);
    return uimdiag_status;
  }
  /* find a free space in req_table to hold information for callback */
  *index_ptr = uimdiag_get_index();
  if (*index_ptr == UIMDIAG_GET_FN_ERR )
  {
    /*  req_table is full */
    uimdiag_status = UIMDIAG_MAX_ASYNC_REQ_EXCEED_ERR;
    UIMDIAG_MSG_ERROR_1("uimdiag_mmgsdi_parse_commands: Parsing error 0x%x",
                        uimdiag_status);
    /* Do an immediate rsp - No delayed response expected*/
    diagpkt_subsys_reset_delayed_rsp_id(*rsp_pptr);
    diagpkt_subsys_set_status(*rsp_pptr, (uint32) UIMDIAG_SUCCESS);
    diagpkt_commit(*rsp_pptr);
    uimdiag_mmgsdi_free_cmd_data(request_ptr, subsys_cmd);
    return uimdiag_status;
  }
  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_parse_commands */


/*===========================================================================

FUNCTION UIMDIAG_MMGSDI_POST_PARSE_COMMANDS

DESCRIPTION
  Function post parsing the command to commit delayed response

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
static uimdiag_return_enum_type uimdiag_mmgsdi_post_parse_commands(
  uimdiag_generic_rsp_type             ** rsp_pptr,
  diagpkt_subsys_delayed_rsp_id_type      rsp_id,
  uimdiag_return_enum_type                uimdiag_status,
  int32                                   index,
  mmgsdi_return_enum_type                 mmgsdi_status,
  uimdiag_generic_req_type              * request_ptr,
  uimdiag_subsystem_cmd_enum_type         subsys_cmd
)
{
  uimdiag_generic_rsp_type               * delayed_rsp_ptr = NULL;
  if (rsp_pptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("rsp_pptr == NULL");
    return UIMDIAG_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  if (request_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("request_ptr == NULL");
    return UIMDIAG_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  diagpkt_subsys_set_status(* rsp_pptr, (uint32) UIMDIAG_SUCCESS);
  diagpkt_commit(* rsp_pptr);

  if ( MMGSDI_SUCCESS != mmgsdi_status )
  {
    /* error in handling API , we need a delayed response too */
    UIMDIAG_MSG_ERROR_2("FAILED TO QUEUE MMGSDI SESSION COMMAND, status: 0x%x for cmd: 0x%x",
                        mmgsdi_status, subsys_cmd);
    uimdiag_free_index(index);
    delayed_rsp_ptr = (uimdiag_generic_rsp_type *)
                        diagpkt_subsys_alloc_v2_delay(
                          (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
                          (diagpkt_subsys_cmd_code_type) subsys_cmd,
                           rsp_id,
                           sizeof(uimdiag_generic_rsp_type));

    if (delayed_rsp_ptr == NULL)
    {
      uimdiag_mmgsdi_free_cmd_data(request_ptr, subsys_cmd);
      return UIMDIAG_ERROR;
    }
    memset(&delayed_rsp_ptr->rsp_payload,0x00, sizeof(uimdiag_cmd_rsp_pyld_type));
    delayed_rsp_ptr->rsp_payload.mmgsdi_status = mmgsdi_status;
    diagpkt_subsys_set_status(delayed_rsp_ptr,(uint32)UIMDIAG_SUCCESS);
    rex_sleep(10);
    diagpkt_delay_commit(delayed_rsp_ptr);
  }
  /*Set the immediate response to SUCCESS*/
  (*rsp_pptr)->rsp_payload.mmgsdi_status = MMGSDI_SUCCESS;
  return UIMDIAG_SUCCESS;
} /* uimdiag_mmgsdi_post_parse_commands */


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
  uimdiag_generic_req_type                request;
  uimdiag_generic_rsp_type               *rsp_ptr             = NULL;
  diagpkt_subsys_delayed_rsp_id_type      rsp_id              = 0;
  int32                                   index               = 0;
  uimdiag_return_enum_type                uimdiag_status      = UIMDIAG_ERROR;
  uimdiag_return_enum_type                delayed_rsp_status  = UIMDIAG_ERROR;
  mmgsdi_return_enum_type                 mmgsdi_status       = MMGSDI_ERROR;
  uimdiag_mmgsdi_naa_refresh_cmd_type    *naa_refresh_ptr     = NULL;
  mmgsdi_option_type                      option;
  mmgsdi_slot_id_enum_type                card_slot;
  mmgsdi_aid_type                         aid;
  mmgsdi_refresh_mode_enum_type           refresh_mode;
  uint8                                   num_files;
  mmgsdi_data_type                        refresh_files;
  mmgsdi_refresh_enforcement_policy_type  refresh_enforcement_policy;

  UIMDIAG_MSG_HIGH_0("uimdiag_mmgsdi_naa_refresh_cmd");

  (void)memset((void *)&option, 0x00, sizeof(mmgsdi_option_type));
  (void)memset((void *)&request, 0x00, sizeof(uimdiag_generic_req_type));
  (void)memset((void *)&refresh_files, 0x00, sizeof(mmgsdi_data_type));
  (void)memset((void *)&aid, 0x00, sizeof(mmgsdi_aid_type));
  (void)memset((void *)&refresh_enforcement_policy, 0x00, sizeof(refresh_enforcement_policy));

  if ((cmd_ptr == NULL)||
      (uimdiag_req_table_ptr == NULL))
  {
    UIMDIAG_MSG_ERROR_0("cmd_ptr == NULL or uimdiag_req_table_ptr == NULL");
    return NULL;
  }

  uimdiag_status = uimdiag_mmgsdi_parse_commands(
                     cmd_ptr,
                     pkt_len,
                     &rsp_ptr,
                     &rsp_id,
                     &index,
                     &request,
                     UIMDIAG_MMGSDI_NAA_REFRESH_CMD);

   if(uimdiag_status != UIMDIAG_SUCCESS)
   {
     return rsp_ptr;
   }

  uimdiag_req_table_ptr[index].rsp_id = rsp_id;
  uimdiag_req_table_ptr[index].subcmd =
                                    UIMDIAG_MMGSDI_NAA_REFRESH_CMD;

  naa_refresh_ptr = &request.request.session_trigger_refresh;

  card_slot              = (mmgsdi_slot_id_enum_type)
                            request.request.session_trigger_refresh.slot_id;

  aid.app_type           = MMGSDI_APP_NONE;

  aid.aid.data_len       = (mmgsdi_len_type)
                            request.request.session_trigger_refresh.sim_app_id_len;

  if(aid.aid.data_len > 0)
  {
    memscpy(aid.aid.data_ptr,
           sizeof(aid.aid.data_ptr),
           request.request.session_trigger_refresh.sim_app_id,
           aid.aid.data_len*sizeof(uint8));
  }
  else
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG: uimdiag_mmgsdi_naa_refresh aid length <= 0  ");
  }


  aid.label.data_len     = 0;
  memset(aid.label.data_ptr,0, MMGSDI_MAX_AID_LEN);


  refresh_mode           = (mmgsdi_refresh_mode_enum_type)
                            request.request.session_trigger_refresh.refresh_mode;

  num_files              = 0; /* num_files set to zero, as it is no longer used.*/

  refresh_files.data_len = (mmgsdi_len_type)
                            request.request.session_trigger_refresh.path_len;
  refresh_files.data_ptr = request.request.session_trigger_refresh.path;

  mmgsdi_status = mmgsdi_naa_refresh(
                    MMGSDI_CLIENT_ID_ZERO,
                    card_slot,
                    aid,
                    refresh_mode,
                    refresh_files,
                    num_files,
                    NULL,
                    refresh_enforcement_policy,
                    option,
                    (mmgsdi_callback_type)uimdiag_mmgsdi_generic_rsp_cb,
                    (mmgsdi_client_data_type)index);

  delayed_rsp_status = uimdiag_mmgsdi_post_parse_commands(
    &rsp_ptr,
    rsp_id,
    uimdiag_status,
    index,
    mmgsdi_status,
    &request,
    UIMDIAG_MMGSDI_NAA_REFRESH_CMD
    );

  if(delayed_rsp_status != UIMDIAG_SUCCESS)
  {
    UIMDIAG_MSG_ERROR_0("UIMDIAG uimdiag_mmgsdi_naa_refresh_cmd:Delayed Response error");
    return NULL;
  }

  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_trigger_refresh.sim_app_id);
  UIMDIAGUTIL_TMC_MEM_FREE(request.request.session_trigger_refresh.path);

  return rsp_ptr;
}/* uimdiag_mmgsdi_naa_refresh_cmd */


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
  uimdiag_check_diag_support_rsp_type          *rsp_ptr            = NULL;

  UIMDIAG_MSG_HIGH_0("uimdiag_check_diag_support_cmd");

  /*  allocate immediate response pointer  */
  rsp_ptr = (uimdiag_check_diag_support_rsp_type *)
                diagpkt_subsys_alloc_v2(
                  (diagpkt_subsys_id_type) DIAG_SUBSYS_GSDI,
                  (diagpkt_subsys_cmd_code_type) UIMDIAG_MMGSDI_CHECK_DIAG_SUPPORT_CMD,
                  sizeof(uimdiag_check_diag_support_rsp_type));
  if( rsp_ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("rsp_ptr == NULL");
    return NULL;
  }
  rsp_ptr->status = 2;
  diagpkt_commit(rsp_ptr);

  return rsp_ptr;
}/* uimdiag_check_diag_support_cmd */
