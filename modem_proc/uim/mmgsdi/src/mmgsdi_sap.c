/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   U I M   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the UICC protocol processing support for MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2004 - 2006, 2008 - 2014, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_sap.c#1 $$ $DateTime: 2020/07/22 10:53:27 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/24/18   rps     Refragment alloc extra param code
12/05/16   bcho    Remove compilation error
06/22/16   tkl     Stub out additional APIs
06/07/16   ar      Removing string prints from APDU and AUTH
05/24/16   ar      Review of macros used by MMGSDI
05/19/16   tkl     F3 log prints cleanup
03/08/16   sp      Remove UIM command in client request table
09/27/14   ar      Add support for multiple slots in BT-SAP
08/14/14   tl      Remove support for silent SAP PIN verficiation in modem
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
01/11/14   df      Fix off-target compilation error
01/06/14   df      Use v2.0 diag macros
10/16/13   tl      Remove MMGSDI_FEATURE_SAP
10/04/13   vdc     F3 message reduction
05/16/13   vdc     Replace memcpy with safer version memscpy
05/14/13   vdc     Memory optimizations for slot, app, pin, upin tables
04/15/13   tl      Remove SAP init, dereg and pin verify functionality
10/11/12   spo     Removed unnecessary F3 messages
10/01/12   abg     Removed Problem determining protocol type F3 message
08/29/12   at      Support for new API mmgsdi_sap_connect_ext
12/21/11   shr     Legacy GSDI removal updates
10/05/11   nb      Update to differentiate SAP Connect rejection due to active call
07/18/11   nb      Do not generate SAP event when connection failed
05/17/11   kk      Updates for Thread safety
05/04/11   nb      Pass slot information with SAP Event
11/30/10   yt      Including private UIMDRV header
10/14/10   yt      Updated handling of client req table entries in case of error
09/09/10   nb      Send SAP notification using client ID
06/24/10   yt      Klocwork fixes
04/07/10   nb      SAP Update
12/18/09   nb      Moved Dual Slot Changes
04/22/09   sun     Fixed includes
11/05/08   ssr     Resolved compiler error for RUIM enablement
09/22/08   nb      Resolved compiler warning
06/23/08   tml     Added featurization for BT in WM
09/10/07   tml     Used generic get ATR and card reader status
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
07/26/06   sun     Lint Fixes and Clean up
07/10/06   tml     compilation fix
07/07/06   sun     Lint Fixes
06/27/06   sun     Send UIM_RESET_SWITCH_UIM_PASSIVE_F during SAP_RESET
                   Fixed Alignment
02/22/06   tml     lint fix
12/20/05   sun     Populated extra_param for Sap_Disconnect and return proper
                   errors on invalid state
12/14/05   tml     MMGSDI Cleanup
12/05/05   sun     Added support for MMGSDI_SAP_CARD_READER_STATUS_REQ and
                   MMGSDI_SAP_RESET_REQ
11/09/05   sun     Fixed Lint Errors
11/03/05   tml     Fixed header
11/03/05   sun     On Disconnect, do not do a refresh immediately
10/28/05   sun     Fixed Sap Disconnect
10/20/05   sun     Added support for BT Deregisteration
08/29/05   tml     Fixed compilation
08/26/05   tml     Fixed compilation issue
08/26/05   sun     Added support for BT SAP
08/25/05   pv      Moved functions and data types relating to refresh to the
                   mmgsdi_gen module
07/28/05   sst     Fixed compile errors when SAP feature is not defined
07/28/05   sst     Fixed compile errors when SAP feature is not defined
06/06/05   sst     Initial version

===========================================================================*/


#include "uim_variation.h"
#include "comdef.h"
#include "intconv.h"
#include "mmgsdi.h"
#include "mmgsdi_sap.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi_uim_uicc.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "uim.h"
#include "uim_p.h"
#include "mmgsdi_sap_rsp.h"
#include "mmgsdi_gen.h"
#include "mmgsdi_evt.h"
#include "mmgsdi_cnf.h"


/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_connect

  DESCRIPTION:
    Connects the UIM to the SAP.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_connect (
  const mmgsdi_sap_connect_req_type* req_ptr
)
{
  uim_cmd_type*                       uim_cmd_ptr     = NULL;
  uim_slot_type                       uim_slot        = UIM_SLOT_NONE;
  mmgsdi_return_enum_type             mmgsdi_status   = MMGSDI_ERROR;
  uint8                               index           = 0;
  mmgsdi_event_data_type              event_data;
  boolean                             reject_req      = FALSE;

  /* Parameter checks. */

  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  memset(&event_data, 0x00, sizeof(mmgsdi_event_data_type));

  event_data.data.sap_connect.sap_mode = MMGSDIBT_SERVER;
  event_data.evt                       = MMGSDI_SAP_CONNECT_EVT;
  event_data.data.sap_connect.slot     = req_ptr->request_header.slot_id;

  UIM_MSG_HIGH_3("Connection condition: 0x%x, voice_call_active: 0x%x, data_call_active = 0x%x",
                 req_ptr->condition,
                 mmgsdi_generic_data_ptr->voice_call_active,
                 mmgsdi_generic_data_ptr->data_call_active);

  /* Check respective call status & decide to proceed or not */
  switch (req_ptr->condition)
  {
    case MMGSDI_SAP_CONN_COND_BLOCK_DATA:
      reject_req = mmgsdi_generic_data_ptr->data_call_active ? TRUE : FALSE;
      break;

      case MMGSDI_SAP_CONN_COND_BLOCK_VOICE:
      reject_req = mmgsdi_generic_data_ptr->voice_call_active ? TRUE : FALSE;
      break;

    case MMGSDI_SAP_CONN_COND_BLOCK_NONE:
      /* Nothing to do */
      break;

    case MMGSDI_SAP_CONN_COND_BLOCK_VOICE_OR_DATA:
    default:
      reject_req = (mmgsdi_generic_data_ptr->voice_call_active ||
                    mmgsdi_generic_data_ptr->data_call_active) ? TRUE : FALSE;
      break;
  }

  if (reject_req)
  {
    UIM_MSG_ERR_0("Call is in progress, cannot connect");
    return MMGSDI_SIM_BUSY;
  }

  /* Convert to UIM slot. */
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id, &uim_slot);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*----------------------------------------------------------------------------
    1) Get a UIM buffer for the request.
    2) Check for if the buffer is NULL or not.
   -----------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));

  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Populate UIM information.*/
  uim_cmd_ptr->hdr.command  = UIM_RESET_SWITCH_UIM_PASSIVE_F;
  uim_cmd_ptr->hdr.protocol = UIM_NO_SUCH_MODE;
  uim_cmd_ptr->hdr.slot     = uim_slot;

  /* Allocate and populate extra parameter.*/
  mmgsdi_status = mmgsdi_util_fill_and_alloc_request_table(MMGSDI_SAP_CONNECT_REQ,
                                                           (void*)req_ptr,
                                                           NULL,
                                                           &index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data.*/
  uim_cmd_ptr->hdr.user_data = index;

  UIM_MSG_HIGH_0("Sending down SAP connect command to UIM");
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  else
  {
    event_data.data.sap_connect.connect_state = MMGSDI_SAP_CONNECTING;
    mmgsdi_evt_notify_client(&event_data);
  }

  return mmgsdi_status;
} /* mmgsdi_sap_sap_connect */


/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_disconnect

  DESCRIPTION:
    Disconnects the UIM from the SAP.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_disconnect (
  const mmgsdi_sap_disconnect_req_type* req_ptr
)
{
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_ERROR;
  int32                              index           = 0;
  uint8                              slot_index      = 0;

  /* Parameter checks.*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr->request_header.response_cb);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* If SAP is not in connected mode for requested SLOT than no need
     to handle disconnect request */
  if(req_ptr->sap_mode == MMGSDIBT_CLIENT || mmgsdi_sap_connected[slot_index] == FALSE)
  {
    /*Nothing to do. Return from here. */
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_fill_and_alloc_request_table(MMGSDI_SAP_DISCONNECT_REQ,
                                                           (void*)req_ptr,
                                                           NULL,
                                                           &index);
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index,
                    MMGSDI_SAP_DISCONNECT_REQ,
                    mmgsdi_status);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;

} /* mmgsdi_sap_sap_disconnect *//*lint !e715 */


/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_send_apdu

  DESCRIPTION:
    Sends an APDU from the SAP to the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_send_apdu (
  mmgsdi_sap_send_apdu_req_type* req_ptr,
  mmgsdi_protocol_enum_type      protocol
)
{
  uint8                              index           = 0;
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_slot_data_type             *slot_data_ptr   = NULL;
  uint8                              slot_index      = 0;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if (protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (!mmgsdi_sap_connected[slot_index])
  {
    UIM_MSG_ERR_0("BT is not connected");
    return MMGSDI_ERROR;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_ptr->request_header.slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if (mmgsdibt_pipe_apdu_ptr[slot_index] == NULL)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdibt_pipe_apdu_ptr[slot_index],
                                       sizeof(mmgsdi_cnf_type));
    if (mmgsdibt_pipe_apdu_ptr[slot_index] == NULL)
    {
      return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
    }
  }

  if (req_ptr->data.data_ptr[UIM_7816_APDU_INSTRN_OFFSET] == GET_RESPONSE)
  {
    /* The response APDU is buffered, no need to go to the SIM.*/
    mmgsdi_status = mmgsdi_util_fill_and_alloc_request_table(MMGSDI_SAP_SEND_APDU_REQ,
                                                             (void*)req_ptr,
                                                             NULL,
                                                             &index);
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      return mmgsdi_status;
    }

    mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(index,
                                                            MMGSDI_SAP_SEND_APDU_REQ,
                                                            MMGSDI_SUCCESS);

    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_util_free_client_request_table_index(index);
    }
  }
  else if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_uim_uicc_send_apdu((mmgsdi_send_apdu_req_type*)req_ptr);
  }
  else
  {
    mmgsdi_status = mmgsdi_uim_icc_send_apdu((mmgsdi_send_apdu_req_type*)req_ptr);
  }
  return mmgsdi_status;
} /* mmgsdi_sap_sap_send_apdu */


/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_power_on

  DESCRIPTION:
    Sends a SIM power on request for the SAP to the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_power_on (
  const mmgsdi_sap_power_on_req_type* req_ptr
)
{
  uim_cmd_type*                      uim_cmd_ptr     = NULL;
  uim_slot_type                      uim_slot        = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  uint8                              index           = 0;
  uint8                              slot_index      = 0;

  /* Parameter checks.*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr,
                              req_ptr->request_header.response_cb);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (!mmgsdi_sap_connected[slot_index])
  {
    UIM_MSG_ERR_0("SAP power ON req error: BT is Not Connected");
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot.*/
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id, &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*------------------------------------------------------------------------------------
    1) Get a UIM buffer for the request.
    2) Check for if the buffer is NULL or not.
   -------------------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));

  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Populate UIM information.*/
  uim_cmd_ptr->hdr.command  = UIM_POWER_UP_UIM_PASSIVE_F;
  uim_cmd_ptr->hdr.protocol = UIM_NO_SUCH_MODE;
  uim_cmd_ptr->hdr.slot     = uim_slot;

  /*-------------------------------------------------------------------------------------
    Get a new index from client req info table for response data.
    Set the uim data pointer to the client request table data buffer.
   -------------------------------------------------------------------------------------*/

  /* Allocate and populate extra parameter.*/
  mmgsdi_status = mmgsdi_util_fill_and_alloc_request_table(MMGSDI_SAP_POWER_ON_REQ,
                                                           (void*)req_ptr,
                                                           NULL,
                                                           &index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data.*/
  uim_cmd_ptr->hdr.user_data = index;
  UIM_MSG_HIGH_0("Sending down SAP power on command to UIM");

  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
} /* mmgsdi_sap_sap_power_on */


/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_power_off

  DESCRIPTION:
    Sends a SIM power off request for the SAP to the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_power_off (
  const mmgsdi_sap_power_off_req_type* req_ptr
)
{
  uim_cmd_type*                      uim_cmd_ptr     = NULL;
  uim_slot_type                      uim_slot        = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  uint8                              index           = 0;
  uint8                              slot_index      = 0;

  /* Parameter checks.*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr,
                              req_ptr->request_header.response_cb);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (!mmgsdi_sap_connected[slot_index])
  {
    UIM_MSG_ERR_0("SAP power OFF req error: BT is Not Connected");
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot.*/
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id, &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*-------------------------------------------------------------------------------------------
    1) Get a UIM buffer for the request.
    2) Check for if the buffer is NULL or not.
   -------------------------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));

  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Populate UIM information.*/
  uim_cmd_ptr->hdr.command  = UIM_POWER_DOWN_F;
  uim_cmd_ptr->hdr.protocol  = UIM_NO_SUCH_MODE;
  uim_cmd_ptr->hdr.slot     = uim_slot;
  uim_cmd_ptr->pdown.pdown_option = UIM_OPTION_NO_NOTIFY_PDOWN;

  mmgsdi_status = mmgsdi_util_fill_and_alloc_request_table(MMGSDI_SAP_POWER_OFF_REQ,
                                                           (void*)req_ptr,
                                                           NULL,
                                                           &index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data.*/
  uim_cmd_ptr->hdr.user_data = index;

  UIM_MSG_HIGH_0("Sending down SAP power off command to UIM");
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
} /* mmgsdi_sap_sap_power_off */

/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_reset

  DESCRIPTION:
    Sends a SIM RESET request for the SAP to the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_reset (
  const mmgsdi_sap_reset_req_type* req_ptr
)
{
  uim_cmd_type*                      uim_cmd_ptr     = NULL;
  uim_slot_type                      uim_slot        = UIM_SLOT_NONE;
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  uint8                              index           = 0;
  uint8                              slot_index      = 0;

  /* Parameter checks.*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr,
                              req_ptr->request_header.response_cb);

  mmgsdi_status = mmgsdi_util_get_slot_index(req_ptr->request_header.slot_id,
                                             &slot_index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  if (!mmgsdi_sap_connected[slot_index])
  {
    UIM_MSG_ERR_0("SAP reset req error: BT is Not Connected");
    return MMGSDI_ERROR;
  }

  /* Convert to UIM slot.*/
  mmgsdi_status = mmgsdi_util_convert_uim_slot(req_ptr->request_header.slot_id, &uim_slot);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /*-------------------------------------------------------------------------------------------
    1) Get a UIM buffer for the request.
    2) Check for if the buffer is NULL or not.
   --------------------------------------------------------------------------------------------*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));

  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Populate UIM information.*/
  uim_cmd_ptr->hdr.command   = UIM_RESET_SWITCH_UIM_PASSIVE_F;
  uim_cmd_ptr->hdr.protocol  = UIM_NO_SUCH_MODE;
  uim_cmd_ptr->hdr.slot      = uim_slot;

  mmgsdi_status = mmgsdi_util_fill_and_alloc_request_table(MMGSDI_SAP_RESET_REQ,
                                                           (void*)req_ptr,
                                                           NULL,
                                                           &index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(uim_cmd_ptr);
    return mmgsdi_status;
  }

  /* Assign uim_cmd_ptr's user data. */
  uim_cmd_ptr->hdr.user_data = index;

  UIM_MSG_HIGH_0("Sending down SAP RESET command to UIM");
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }
  return mmgsdi_status;
} /* mmgsdi_sap_sap_reset */


/*=============================================================================
  FUNCTION: MMGSDI_CNF_GET_SAP_ATR_LEN

  DESCRIPTION:
    Retrieves the SAP ATR from the global

  PARAMETERS:
    slot: MMGSDI SAP slot

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    length of the ATR

  SIDE EFFECTS:
    None
=============================================================================*/
static uint8 mmgsdi_cnf_get_sap_atr_len(
    mmgsdi_slot_id_enum_type slot_id)
{
  uint8                  len           = 0;
  mmgsdi_slot_data_type *slot_data_ptr = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr != NULL)
  {
    len = slot_data_ptr->card_atr.atr_length;
  }

  return len;
}/* mmgsdi_cnf_get_sap_atr_len */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SAP_SEND_APDU_DATA

   DESCRIPTION:
     This function builds the SAP Send APDU for Get Response confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_sap_send_apdu_data(
  mmgsdi_sap_send_apdu_cnf_type           * sap_send_apdu_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_data_type                  * apdu_data_ptr)
{
  mmgsdi_return_enum_type  build_cnf_status    = MMGSDI_SUCCESS;
  int32                    req_data_len           = 0;
  int32                    sap_send_apdu_curr_len = 0;
  uint8                    slot_index             = 0;

  MMGSDIUTIL_RETURN_IF_NULL(sap_send_apdu_cnf_ptr);
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    if (extra_param_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }

    if (extra_param_ptr->sap_send_apdu_data.data.data_ptr [UIM_7816_APDU_INSTRN_OFFSET] ==
             GET_RESPONSE)
    {
      build_cnf_status = mmgsdi_util_get_slot_index(
                           extra_param_ptr->sap_send_apdu_data.slot_id,
                           &slot_index);

      if (build_cnf_status != MMGSDI_SUCCESS)
      {
        return build_cnf_status;
      }
      /* Length requested by the client */
      req_data_len = extra_param_ptr->sap_send_apdu_data.data.data_ptr[UIM_CASE2_7816_LE_OFFSET];

      if (mmgsdibt_pipe_apdu_ptr[slot_index] == NULL)
      {
        return MMGSDI_ERROR;
      }
      /* Length of the available apdu data length */
      sap_send_apdu_curr_len = mmgsdibt_pipe_apdu_ptr[slot_index]->sap_send_apdu_cnf.apdu_data.data_len;

      UIM_MSG_HIGH_2("Requested Length 0x%x, Avaiable Length 0x%x",
                     req_data_len, sap_send_apdu_curr_len);
      if(sap_send_apdu_curr_len <= 0)
      {
        return MMGSDI_ERROR;
      }
      else if ((req_data_len == 0)                          ||
               (req_data_len >= (sap_send_apdu_curr_len - 0x02)))
      {
        /* Second condition is when user asks for the amount of data greater
         * than or equal to SW2 of original command. But since we have status
         * words for GET RESPONSE in buffer as well we need to adjust the buffer
         * length by 2.
         * Buffer Content - Length = data_len + Status Words Length
         *                  Data   = [Data From card] SW1 SW2
         *                           <-- data_len -->
         */
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
          sap_send_apdu_cnf_ptr->apdu_data.data_ptr,
          sap_send_apdu_curr_len);

        if (sap_send_apdu_cnf_ptr->apdu_data.data_ptr == NULL)
        {
          UIM_MSG_ERR_0("SAP: mmgsdi_cnf_build_sap_send_apdu_data malloc error");
          return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        }
        sap_send_apdu_cnf_ptr->apdu_data.data_len = sap_send_apdu_curr_len;
        mmgsdi_memscpy(sap_send_apdu_cnf_ptr->apdu_data.data_ptr,
                       int32touint32(sap_send_apdu_curr_len),
                       mmgsdibt_pipe_apdu_ptr[slot_index]->sap_send_apdu_cnf.apdu_data.data_ptr,
                       int32touint32(sap_send_apdu_curr_len));

        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdibt_pipe_apdu_ptr[slot_index]->sap_send_apdu_cnf.apdu_data.data_ptr);
        mmgsdibt_pipe_apdu_ptr[slot_index]->sap_send_apdu_cnf.apdu_data.data_len = 0x00;

        /* If we are returning all available data,set sw1 what ever card has returned */
        sap_send_apdu_cnf_ptr->implicit_get_rsp_sw1 =
          mmgsdibt_pipe_apdu_ptr[slot_index]->sap_send_apdu_cnf.implicit_get_rsp_sw1;

        /* Set sw2 as 0 as no data is remaining in buffer to send further */
        sap_send_apdu_cnf_ptr->implicit_get_rsp_sw2 = 0x00;

        MMGSDIUTIL_TMC_MEM_FREE(mmgsdibt_pipe_apdu_ptr[slot_index]);
      }
      else /* where requested lenght < available length*/
      {
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
          sap_send_apdu_cnf_ptr->apdu_data.data_ptr,
          req_data_len);

        if (sap_send_apdu_cnf_ptr->apdu_data.data_ptr == NULL)
        {
          return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
        }
        mmgsdi_memscpy(sap_send_apdu_cnf_ptr->apdu_data.data_ptr,
                       int32touint32(req_data_len),
                       mmgsdibt_pipe_apdu_ptr[slot_index]->sap_send_apdu_cnf.apdu_data.data_ptr,
                       int32touint32(req_data_len));

        mmgsdi_memsmove(mmgsdibt_pipe_apdu_ptr[slot_index]->sap_send_apdu_cnf.apdu_data.data_ptr,
                        int32touint32(req_data_len),
                        &mmgsdibt_pipe_apdu_ptr[slot_index]->sap_send_apdu_cnf.apdu_data.data_ptr[req_data_len],
                        int32touint32(sap_send_apdu_curr_len - req_data_len));

        sap_send_apdu_cnf_ptr->apdu_data.data_len = req_data_len;
        mmgsdibt_pipe_apdu_ptr[slot_index]->sap_send_apdu_cnf.apdu_data.data_len =
          sap_send_apdu_curr_len - req_data_len;

        /* If the requested len < available data lenth,
           set sw1 as what ever card has returned */
        sap_send_apdu_cnf_ptr->implicit_get_rsp_sw1 =
          mmgsdibt_pipe_apdu_ptr[slot_index]->sap_send_apdu_cnf.implicit_get_rsp_sw1;

      /* coping sw2 with data length ramaining */
      sap_send_apdu_cnf_ptr->implicit_get_rsp_sw2 =
          (uint16)mmgsdibt_pipe_apdu_ptr[slot_index]->sap_send_apdu_cnf.apdu_data.data_len;
      }
    }
    else
    {
      if (apdu_data_ptr == NULL)
      {
        return MMGSDI_ERROR;
      }
      build_cnf_status = mmgsdi_cnf_build_send_apdu_data(sap_send_apdu_cnf_ptr,
                                                         mmgsdi_status,
                                                         extra_param_ptr,
                                                         apdu_data_ptr,
                                                         extra_param_ptr->sap_send_apdu_data.slot_id);
    }
  }

  return build_cnf_status;
} /* mmgsdi_cnf_build_sap_send_apdu_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_ATR_DATA

   DESCRIPTION:
     This function builds the get ATR confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_sap_get_atr_data(
  mmgsdi_sap_get_atr_cnf_type       * get_atr_cnf_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  mmgsdi_slot_id_enum_type            slot_id)
{
  uint32                   atr_len          = 0;
  mmgsdi_slot_data_type   *slot_data_ptr    = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(get_atr_cnf_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    atr_len = mmgsdi_cnf_get_sap_atr_len(slot_id);
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(get_atr_cnf_ptr->atr_data.data_ptr,
                                       atr_len);
    if (get_atr_cnf_ptr->atr_data.data_ptr == NULL)
    {
      return MMGSDI_ERROR;
    }

    slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

    if(slot_data_ptr == NULL)
    {
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(get_atr_cnf_ptr->atr_data.data_ptr);
      return MMGSDI_ERROR;
    }

    get_atr_cnf_ptr->atr_data.data_len =
      (int32)slot_data_ptr->card_atr.atr_length;

    if (slot_data_ptr->card_atr.atr_length > 0 &&
        slot_data_ptr->card_atr.atr_length <= MMGSDI_MAX_ATR_LEN)
    {
      mmgsdi_memscpy(get_atr_cnf_ptr->atr_data.data_ptr,
                     int32touint32(get_atr_cnf_ptr->atr_data.data_len),
                     slot_data_ptr->card_atr.atr_data,
                     slot_data_ptr->card_atr.atr_length);
    }
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_sap_get_atr_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SAP_DISCONNECT_DATA

   DESCRIPTION:
     This function builds the SAP Disconnection confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_sap_disconnect_data(
  mmgsdi_sap_disconnect_cnf_type    * disconnect_cnf_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  mmgsdi_disconnect_mode_enum_type    disconnect_mode)
{
  MMGSDIUTIL_RETURN_IF_NULL(disconnect_cnf_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    disconnect_cnf_ptr->disconnect_mode = disconnect_mode;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_sap_disconnect_data */
