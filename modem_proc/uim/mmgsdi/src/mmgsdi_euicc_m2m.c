/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   M 2 M E  U I C C   R E L A T E D  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains routines used for M2M eUICCs.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS



                        COPYRIGHT INFORMATION

Copyright (c) 2017 QUALCOMM Technologies, Inc (QTI) and
its licensors. All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_euicc_m2m.c#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/19   vgd     GET EID for M2M cards where LPA is not supported
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "uim_msg.h"
#include "err.h"
#include "mmgsdiutil.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_session.h"
#include "mmgsdi.h"
#include "mmgsdilib_p.h"
#include "mmgsdilib_common.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_euicc_m2m.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/* GET_EID PARAMETERS */
#define MMGSDI_GET_EID_APDU_DATA_P2        0x5A
#define MMGSDI_GET_DATA_INS                0xCA
#define MMGSDI_GET_EID_APDU_RESP_MIN_LEN   0x04
#define MMGSDI_EID_LEN                     0x10


/* ==========================================================================
  FUNCTION MMGSDI_EUICC_M2M_SESSION_CLOSE_CB

  DESCRIPTION
    This function defined as dummy mmgsdi_callback_type, will handle the
    close session request for GET_EID request 

  DEPENDENCIES
    None

  LIMITATIONS
    None

  RETURN VALUE
    void

  SIDE EFFECTS
    None
=============================================================================*/
static void mmgsdi_euicc_m2m_session_close_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr)
{
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_slot_data_type   *slot_data_ptr = NULL;

  (void)cnf;

  if (cnf_ptr == NULL)
  {
    return;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(cnf_ptr->response_header.slot_id);

  if (slot_data_ptr == NULL)
  {
    return;
  }

  slot_data_ptr->is_eid_in_process = FALSE;
} /* mmgsdi_euicc_m2m_session_close_cb */


/*===========================================================================
   FUNCTION:      MMGSDI_EUICC_M2M_CHANNEL_TO_CLA_BYTE

   DESCRIPTION:
     MMGSDI converts the channel id to the CLA byte

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     uint8

   SIDE EFFECTS:
     None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_euicc_m2m_channel_to_cla_byte (
  uint8   channel_id, 
  uint8  *class_byte_ptr
)
{
  MMGSDIUTIL_RETURN_IF_NULL(class_byte_ptr);
  
  if(channel_id < MMGSDI_LEGACY_LOGICAL_CHANNELS)
  {
    /* For basic logical channels,
         * b7 is 0.
         * for non inter-industry (proprietary), b8 is 1. */
    *class_byte_ptr = (0x80 + channel_id);
    return MMGSDI_SUCCESS;
  }
  else if(channel_id < UIM_MAX_CHANNELS)
  {
    /* For extended logical channels,
         * b7 is 1.
         * for non inter-industry (proprietary), b8 is 1. */
    *class_byte_ptr = (0xC0 + channel_id - MMGSDI_LEGACY_LOGICAL_CHANNELS);
    return MMGSDI_SUCCESS;
  }

  return MMGSDI_ERROR;
} /* mmgsdi_euicc_m2m_channel_to_cla_byte */


/*===========================================================================
   FUNCTION:      MMGSDI_EUICC_M2M_CREATE_EID_APDU

   DESCRIPTION:
     Utility called to create APDU for GET_EID 

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_euicc_m2m_create_eid_apdu(
  mmgsdi_len_type          *data_len_ptr,
  uint8                   **data_pptr,
  mmgsdi_channel_id_type    channel_id)
{
  uint8                   cla_byte      = 0;

  MMGSDIUTIL_RETURN_IF_NULL_2(data_pptr, data_len_ptr);
  
  *data_len_ptr  =  UIM_CASE2_7816_APDU_SIZE;

  if(mmgsdi_euicc_m2m_channel_to_cla_byte(channel_id, &cla_byte) != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }
  
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(*data_pptr, UIM_CASE2_7816_APDU_SIZE);
  
  if(*data_pptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  (*data_pptr)[UIM_7816_APDU_CLASS_OFFSET]  = cla_byte;                       /* CLA */
  (*data_pptr)[UIM_7816_APDU_INSTRN_OFFSET] = MMGSDI_GET_DATA_INS;            /* INS */
  (*data_pptr)[UIM_7816_APDU_P1_OFFSET]     = 0x00;                           /* P1 */
  (*data_pptr)[UIM_7816_APDU_P2_OFFSET]     = MMGSDI_GET_EID_APDU_DATA_P2;    /* P2 */

  (*data_pptr)[UIM_CASE3_4_7816_LC_OFFSET] = 0x00;

  return MMGSDI_SUCCESS;
} /* mmgsdi_euicc_m2m_create_eid_apdu */


/*===========================================================================
   FUNCTION:      MMGSDI_EUICC_M2M_GET_EID_CB

   DESCRIPTION:
     This is a callback function for open channel, send APDU and close channel called
     specifically for GET_EID request.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     void

   SIDE EFFECTS:
     None
===========================================================================*/
static void mmgsdi_euicc_m2m_get_eid_cb(
  mmgsdi_return_enum_type   status,
  mmgsdi_cnf_enum_type      cnf,
  const mmgsdi_cnf_type    *cnf_ptr)
{
  uint32                              index              = 0;
  mmgsdi_request_header_type          req_header         = {0};
  mmgsdi_client_req_extra_info_type  *extra_param_ptr    = NULL;
  mmgsdi_return_enum_type             mmgsdi_status      = MMGSDI_SUCCESS;
  mmgsdi_sw_status_type               status_word        = {0};
  boolean                             close_channel      = FALSE;
  boolean                             apdu_data_received = FALSE;
  mmgsdi_slot_data_type              *slot_data_ptr      = NULL;

  if (cnf_ptr == NULL)
  {
    return;
  }

  UIM_MSG_HIGH_3("mmgsdi_euicc_m2m_get_eid_cb: cnf is 0x%x, the slot_id is 0x%x and the status is 0x%x",
                 cnf, cnf_ptr->response_header.slot_id, status);

  /* Obtain the index of cnf_ptr */
  index = cnf_ptr->response_header.client_data;
  if (index >= MMGSDI_MAX_CLIENT_REQ_INFO             ||
      mmgsdi_client_req_table_info_ptr[index] == NULL ||
      mmgsdi_client_req_table_info_ptr[index]->orig_request_type != MMGSDI_GET_M2M_EID_REQ)
  {
    return;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(cnf_ptr->response_header.slot_id);

  if (slot_data_ptr == NULL)
  {
    return;
  }

  if(mmgsdi_client_req_table_info_ptr[index]->uim_rsp_status == MMGSDI_UIM_RSP_NOT_RECEIVED)  
     /* If rsp_status is already MMGSDI_UIM_RSP_NOT_RECEIVED,
        it means an error cnf is already sent for this */
  {
    UIM_MSG_ERR_2("Free client_req_table index 0x%x without processing rsp for cmd 0x%x",
                  index, mmgsdi_client_req_table_info_ptr[index]->request_type);
    slot_data_ptr->is_eid_in_process = FALSE;
    mmgsdi_util_free_client_request_table_index(index);
    return;
  }

  /* Retrieve the GET_EID request */
  mmgsdi_status = mmgsdi_util_get_client_request_table_info(index,
                                                            &req_header,                                                                 &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_0("mmgsdi_euicc_m2m_get_eid_cb: Could not extract request data");
    slot_data_ptr->is_eid_in_process = FALSE;
    mmgsdi_util_free_client_request_table_index(index);
    return;
  }

  do
  {
    if (cnf == MMGSDI_SESSION_OPEN_WITH_SELECT_RSP_CNF)
    {
      mmgsdi_send_apdu_data_type     apdu_data;

      memset(&apdu_data, 0x00, sizeof(mmgsdi_data_type));

      if(status != MMGSDI_SUCCESS)
      {
        slot_data_ptr->is_eid_in_process = FALSE;
        mmgsdi_status = status;
        break;
      }

      mmgsdi_client_req_table_info_ptr[index]->session_id = cnf_ptr->session_open_with_sel_rsp_cnf.session_id;

      /* Create APDU for GET_EID request */
      mmgsdi_status = mmgsdi_euicc_m2m_create_eid_apdu(&apdu_data.data_len,
                                                       &apdu_data.data_ptr,
                                                       cnf_ptr->session_open_with_sel_rsp_cnf.channel_id);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        close_channel = TRUE;
        break;
      }

      mmgsdi_status = mmgsdi_send_apdu_ext(cnf_ptr->session_open_with_sel_rsp_cnf.response_header.client_id,
                                           cnf_ptr->session_open_with_sel_rsp_cnf.slot_id,
                                           apdu_data,
                                           MMGSDI_SEND_APDU_NO_IMPLICIT_STATUS_WORDS,
                                           mmgsdi_euicc_m2m_get_eid_cb,
                                           (mmgsdi_client_data_type)index);

      MMGSDIUTIL_TMC_MEM_FREE(apdu_data.data_ptr);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        close_channel = TRUE;
        break;
      }
      return;
    }  
    else if (cnf == MMGSDI_SEND_APDU_EXT_CNF)
    {
      /* Channel needs to be closed in error or success both cases */
      close_channel = TRUE;

      /* As per Global Platform specification, when the CLA byte has b8=1,
         the response from the card is in TLV format. So, the first byte must
         be the tag used for the request and the second byte contains the
         length of the EID. */
      if (status != MMGSDI_SUCCESS ||
          cnf_ptr->send_apdu_ext_cnf.apdu_data.data_len < MMGSDI_GET_EID_APDU_RESP_MIN_LEN ||
          cnf_ptr->send_apdu_ext_cnf.apdu_data.data_ptr[0] != MMGSDI_GET_EID_APDU_DATA_P2  ||
          cnf_ptr->send_apdu_ext_cnf.apdu_data.data_ptr[1] != MMGSDI_EID_LEN               ||
          cnf_ptr->send_apdu_ext_cnf.apdu_data.data_len - MMGSDI_GET_EID_APDU_RESP_MIN_LEN != cnf_ptr->send_apdu_ext_cnf.apdu_data.data_ptr[1])
      {
        mmgsdi_status = MMGSDI_ERROR;
        break;
      }
      else
      {
        apdu_data_received = TRUE;
      }
    }
    else
    {
      mmgsdi_status = MMGSDI_ERROR;
    }
  } while (0);

  /* Queue the confirmation to QMI UIM In error case */
  (void)mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                   &req_header,
                                   NULL,
                                   apdu_data_received ? &cnf_ptr->send_apdu_ext_cnf.apdu_data : NULL,
                                   FALSE,
                                   status_word);

  if (close_channel)
  {
    (void)mmgsdi_session_close(mmgsdi_client_req_table_info_ptr[index]->session_id ,
                               mmgsdi_euicc_m2m_session_close_cb,
                               0);
  }
  mmgsdi_util_free_client_request_table_index(index);
} /* mmgsdi_euicc_m2m_get_eid_cb */


/*===========================================================================
   FUNCTION:      MMGSDI_PROCESS_GET_M2M_EID

   DESCRIPTION:
     This function processes the GET_EID by performing the following process.
     1) Open channel
     2) Select ECASD
     3) Send GET_DATA APDU to get EID
     4) Close channel

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_get_m2m_eid(
  mmgsdi_task_cmd_type           * task_cmd_ptr)
{
  mmgsdi_return_enum_type             mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_app_select_with_rsp_type     app_info;
  uint8                               index                = 0;
  mmgsdi_client_req_extra_info_type  *extra_param_ptr      = NULL;
  uint8                               mmgsdi_ecasd_aid[]   = {0xA0, 0x00, 0x00,0x05, 
                                                              0x59, 0x10, 0x10,0xFF,
                                                              0xFF, 0xFF, 0xFF,0x89, 
                                                              0x00, 0x00, 0x02,0x00};
  mmgsdi_slot_data_type              *slot_data_ptr        = NULL;
  mmgsdi_get_m2m_eid_req_type        *eid_req_ptr          = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(task_cmd_ptr);

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(task_cmd_ptr->cmd.cmd.get_m2m_eid_req.request_header.slot_id);

  if (slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Queuing the GET_EID request back to the same slot task_cmd_q,
     if previous GET_EID request on same slot is in progress.
     We are re-using the allocated task_cmd_ptr */
  if (slot_data_ptr->is_eid_in_process)
  {
    mmgsdi_status = mmgsdi_cmd_ext(task_cmd_ptr, mmgsdi_util_get_current_task());
    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      mmgsdi_status = MMGSDI_CMD_QUEUED_TO_SLOT_TASK;
    }
    return mmgsdi_status;
  }

  eid_req_ptr = &task_cmd_ptr->cmd.cmd.get_m2m_eid_req;

  memset(&app_info, 0x00, sizeof(app_info));

  app_info.exclusive_channel                 = TRUE;
  app_info.select_rsp_type                   = MMGSDI_SELECT_RSP_FCI;
  app_info.app_id_data.data_len              = sizeof(mmgsdi_ecasd_aid);
  app_info.app_id_data.data_ptr              = mmgsdi_ecasd_aid;

  mmgsdi_status = mmgsdi_util_alloc_and_populate_extra_param(MMGSDI_GET_M2M_EID_REQ,
                                                             (void *)eid_req_ptr,
                                                              &extra_param_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_get_client_request_table_free_index(&index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /*---------------------------------------------------------------------------
    1) Release extra param pointer (Since client table has not been populated
                                    yet)
    ---------------------------------------------------------------------------*/
    MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_populate_client_request_table_info(index,
                                                                 &eid_req_ptr->request_header,
                                                                 extra_param_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_session_open_with_select_rsp(mmgsdi_generic_data_ptr->client_id,
                                                      eid_req_ptr->request_header.slot_id,
                                                      app_info,
                                                      NULL,
                                                      mmgsdi_euicc_m2m_get_eid_cb, 
                                                      (mmgsdi_client_data_type)index);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_sw_status_type             status_word         = {0};

    mmgsdi_util_free_client_request_table_index(index);
    MMGSDIUTIL_TMC_MEM_FREE(extra_param_ptr);

    mmgsdi_status =  mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                                &eid_req_ptr->request_header,
                                                NULL,
                                                NULL,
                                                FALSE, /* sw not required */
                                                status_word);
  }
  else
  {
    /* Open session request is queued successfully. Make the global TRUE */
    slot_data_ptr->is_eid_in_process = TRUE;
  }
  return mmgsdi_status;  
} /* mmgsdi_process_get_m2m_eid */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_M2M_EID_DATA

   DESCRIPTION:
     This function build the get_eid confirmation specific data

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
mmgsdi_return_enum_type mmgsdi_cnf_build_get_m2m_eid_data(
  mmgsdi_get_m2m_eid_cnf_type             * eid_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_send_apdu_data_type        * data_read_ptr
)
{
  MMGSDIUTIL_RETURN_IF_NULL(eid_cnf_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS &&
      data_read_ptr != NULL &&
      data_read_ptr->data_len > MMGSDI_GET_EID_APDU_RESP_MIN_LEN &&
      data_read_ptr->data_len - MMGSDI_GET_EID_APDU_RESP_MIN_LEN <= sizeof(eid_cnf_ptr->eid_response.data_ptr))
  {
    eid_cnf_ptr->eid_response.data_len = data_read_ptr->data_len - MMGSDI_GET_EID_APDU_RESP_MIN_LEN;

    /* These 2 bytes are Tag (0x5A) and apdu data len (0x10) */
    mmgsdi_memscpy(eid_cnf_ptr->eid_response.data_ptr,
                   sizeof(eid_cnf_ptr->eid_response.data_ptr),
                   data_read_ptr->data_ptr + 2,
                   data_read_ptr->data_len - MMGSDI_GET_EID_APDU_RESP_MIN_LEN);
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_get_m2m_eid_data */
