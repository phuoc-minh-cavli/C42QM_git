/*===========================================================================
                      DS3GMMGSDIIF.C
DESCRIPTION
This file defines ds3gmmgsdi interface APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE
when        who    what, where, why
--------    ---    ----------------------------------------------------------
30/08/18    pk      First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "ds3gmmgsdiif.h"
#include "modem_mem.h"
#include "data_msg.h"
#include "mmgsdilib_common.h"
#include "ds_system_heap.h"
#include "ds_pdn_manager.h"
#include "ds_pdn_nv_manager.h"
#include "ds_pdn_roaming_hdlr.h"
#include "ds_pdn_hdlr.h"
#include "ds_pdn_cp_data_hdlr.h"
#include "ds_3gpp_srv_req_throt_sm.h"
#include "ps_proto_init.h"
#include "ds_3gpp_dam.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_3gpp_network_override_hdlr.h"
#include "ds_3gpp_pdn_throt_spec.h"
#include "ds_3gpp_pdn_limit_hdlr.h"
#include "ds_3gpp_rpm.h"
#include "ds_3gpp_cc_sp_throt_sm.h"

ds3g_mmgsdi_info_type *ds3g_mmgsdi_info = NULL;
ds3g_mmgsdi_non_prov_isim_session_info_type *ds3g_mmgsdi_isim_info = NULL;
 ds3g_mmgsdi_imsi_info_type *ds3g_mmgsdi_imsi_info = NULL;

/* IMSI_M info table */
ds3g_mmgsdi_imsi_m_info_type *ds3g_mmgsdi_imsi_m_info_i = NULL;

mmgsdi_session_open_info_type *mmgsdi_session_info_ptr;
mmgsdi_client_id_type     *client_id_ptr = NULL;

/* Storing UE EFacc values for each GW session */
static uint32 ds3g_mmgsdi_efacc_class_value = 0;

#define DS3G_MMGSDI_ISIM_AID_CMP_LENGTH 7

/* All ISIM apps start with this identifier: 3GPP spec 31-103 */
const uint8 ds3g_mmgsdi_isim_aid_value[DS3G_MMGSDI_ISIM_AID_CMP_LENGTH]
                                 = {0xA0,0x00,0x00,0x00,0x87,0x10,0x04};

#define DS3G_MMGSDI_MAX_NUM_REFRESH_FILES  4
#define DS3G_MMGSDI_USIM_SUBSCRIPTION_READY_MASK     0x04


#define DS3G_MMGSDI_IMSI_OFFSET      0
#define DS3G_MMGSDI_MNC_OFFSET       3
#define DS3G_MMGSDI_MNC_2_DIGITS     2
#define DS3G_MMGSDI_MNC_3_DIGITS     3
#define DS3G_MMGSDI_IMSI_DIGIT_1_OFFSET         0x01
#define DS3G_MMGSDI_IMSI_DIGIT_2_OFFSET         0x02
#define DS3G_MMGSDI_IMSI_DIGIT_3_OFFSET         0x02
#define DS3G_MMGSDI_IMSI_DIGIT_4_OFFSET         0x03
#define DS3G_MMGSDI_IMSI_DIGIT_5_OFFSET         0x03
#define DS3G_MMGSDI_IMSI_DIGIT_6_OFFSET         0x04
#define DS3G_MMGSDI_IMSI_DIGIT_7_OFFSET         0x04
#define DS3G_MMGSDI_IMSI_DIGIT_8_OFFSET         0x05
#define DS3G_MMGSDI_IMSI_HIGH_DIGIT_MASK        0xF0
#define DS3G_MMGSDI_IMSI_LOW_DIGIT_MASK         0x0F

#define DS3G_MMGSDI_SUBSCRIPTION_READY_MASK     0x01
#define DS3G_MMGSDI_SUBS_ID_READY_MASK          0x02
#define DS3G_MMGSDI_USIM_SUBS_ID_READY_MASK          0x08
#define DS3G_MMGSDI_CSIM_SUBSCRIPTION_READY_MASK     0x10
#define DS3G_MMGSDI_CSIM_SUBS_ID_READY_MASK          0x20

/*---------------------------------------------------------------------------
  Variable used to store refres vote ok NV
  ---------------------------------------------------------------------------*/
static boolean ds3g_refresh_vote_ok = FALSE;

/*---------------------------------------------------------------------------
                       Local Declarations
  ---------------------------------------------------------------------------*/
void ds3g_mmgsdi_exception_data_allowed_update_cache
(
  void
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_SUBSCRIPTION_READY_EVENT_PROCESS

DESCRIPTION
  This function processes event when receiving MMGSDI_SUBSCRIPTION_READY_EVENT
  and it should be called by ds3g_mmgsdi_process_cmd()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_mmgsdi_subscription_ready_event_process
(
 ds_command_type  *cmd_ptr
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_SUBS_READY_TO_PROCESS_USIM

DESCRIPTION
  Function to determine if both subscription ready event and subs id info
  has been received for USIM session to process.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean ds3g_mmgsdi_subs_ready_to_process_usim
(
 uint8                          mask,
 mmgsdi_session_type_enum_type  session_type
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_OPEN_NON_PROV_ISIM_SESSION

DESCRIPTION
  Function to open non-provisioned session for ISIM after subscription is
  ready

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_open_non_prov_isim_session
(
 mmgsdi_slot_id_enum_type  slot_id
);

/*==============================================================================
FUNCTION  DS3G_MMGSDI_READ_IMSI

DESCRIPTION
  Get IMSI info afer receiving Subscription Ready Event

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

==============================================================================*/

boolean ds3g_mmgsdi_read_imsi
(
 mmgsdi_session_type_enum_type  session_type
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_IMSI_DATA()

DESCRIPTION
  Passed cached IMSI data to client.

DEPENDENCIES
  None

RETURN VALUE
  FALSE: Failed to read IMSI data
  TRUE: Read IMSI data successfully

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_get_imsi_data
(
  nv_ehrpd_imsi_type*    imsi_data_ptr
);

/*==============================================================================
FUNCTION  DS3G_MMGSDI_GET_APP_TYPE

DESCRIPTION
  Get session app type from ds3g_mmgsdi_info based on session_type

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Successfully getting app type from ds3g_mmgsdi_info
  FALSE: Failed to get app type from ds3g_mmgsdi_info

SIDE EFFECTS
None
==============================================================================*/

boolean ds3g_mmgsdi_get_app_type
(
 mmgsdi_session_type_enum_type   session_type,
 mmgsdi_app_enum_type           *app_type_ptr
);

/*==============================================================================
FUNCTION  DS3G_MMGSDI_READ_EFACC

DESCRIPTION
  Read EF ACC info during Subscription Ready/Refresh Events for GW sessions

DEPENDENCIES
  None.

PARAMETERS
  mmgsdi_session_type_enum_type session_type

RETURN VALUE
  TRUE-if the read is successful.
  FALSE-otherwise.

SIDE EFFECTS
  None

==============================================================================*/

boolean ds3g_mmgsdi_read_efacc
(
 mmgsdi_session_id_type  session_id
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_RESET_EFACC_VALUE

DESCRIPTION
  This function is called when  DS3G_MMGSDI_CARD_ERROR_EVENT is posted.
   It reset the efacc values to zero for the corresponding GW session.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_mmgsdi_reset_efacc_value
(
  void
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_EFACC_REGISTER_FOR_REFRESH

DESCRIPTION
  This function registers for refresh on the EFacc files on SIM/USIM MMGSDI
  interface.

PARAMETERS
  mmgsdi_session_type_enum_type session_type

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_mmgsdi_efacc_register_for_refresh
(
  mmgsdi_session_type_enum_type session_type
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_NON_PROV_ISIM_SESSION_EVT_REG_CB

DESCRIPTION
  This function handles the MMGSDI client event registration callback for
  the non provisioned isim app.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void ds3g_mmgsdi_non_prov_isim_session_evt_reg_cb
(
 const mmgsdi_event_data_type  *event
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_ISIM_REFRESH_COMPLETE_PROCESSING

DESCRIPTION
  This function processes event when receiving MMGSDI_REFRESH_EVT with
  MMGSDI_REFRESH_STAGE_END_SUCCESS from MMGSDI interface.
  This function should be called by ds3g_mmgsdi_process_cmd().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void ds3g_mmgsdi_isim_refresh_complete_processing
(
 ds_command_type  *cmd_ptr
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_REFRESH_VOTING_EVENT_PROCESS

DESCRIPTION
  This function processes event when receiving MMGSDI_REFRESH_EVT with
  MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT from MMGSDI interface.
  This function should be called by ds3g_mmgsdi_process_cmd().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_mmgsdi_refresh_voting_event_process
(
 ds_command_type  *cmd_ptr
);

/*============================================================================
FUNCTION  DS3G_MMGSDI_CARD_REFRESH_EVENT_PROCESS 

DESCRIPTION
  This function is the handler function for the MMGSDI CARD ERROR event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void ds3g_mmgsdi_card_refresh_event_process
(
 ds_command_type  *cmd_ptr
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_REFRESH_FCN_EVENT_PROCESS

DESCRIPTION
  This function processes event when receiving file change notification from
  MMGSDI interface. This function should be called by ds3g_mmgsdi_process_cmd()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_refresh_fcn_event_process
(
 ds_command_type  *cmd_ptr
);

/*============================================================================
FUNCTION  DS3G_MMGSDI_CARD_ERROR_EVENT_PROCESS 

DESCRIPTION
  This function is the handler function for the MMGSDI CARD ERROR event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void ds3g_mmgsdi_card_error_event_process
(
 ds_command_type  *cmd_ptr
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_IMSI_RESP_CB()

DESCRIPTION
  Gets IMSI data from the mmgsdi response, and copies into variables local
  to the file.  This is a callback function which is called from the
  mmgsdi context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void ds3g_mmgsdi_imsi_resp_cb
(
 mmgsdi_return_enum_type  status,
 mmgsdi_cnf_enum_type     cnf,
 const mmgsdi_cnf_type   *cnf_ptr
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_IMSI_RESP_CB()

DESCRIPTION
  Gets IMSI data from the mmgsdi response, and copies into variables local
  to the file.  This is a callback function which is called from the
  mmgsdi context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_mnc_resp_cb
(
 mmgsdi_return_enum_type status,
 mmgsdi_cnf_enum_type    cnf,
 const mmgsdi_cnf_type  *cnf_ptr
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_EFACC_READ_RESP_HDLR_CB

DESCRIPTION
  Process EFACC data from the mmgsdi response, and posts a cmd to dstask for
  read confirmation.This is a callback function called from the mmgsdi task
  context.

PARAMETERS
  mmgsdi_return_enum_type   status
  mmgsdi_cnf_enum_type      cnf
  const mmgsdi_cnf_type     *cnf_ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void ds3g_mmgsdi_efacc_read_resp_hdlr_cb
(
 mmgsdi_return_enum_type  status,
 mmgsdi_cnf_enum_type     cnf,
 const mmgsdi_cnf_type   *cnf_ptr
);
/*===========================================================================
FUNCTION DS3G_MMGSDI_ICCID_RESP_CB()

DESCRIPTION
  Gets ICCID data from the mmgsdi response, and copies into variables local
  to the file.  This is a callback function which is called from the
  mmgsdi context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void ds3g_mmgsdi_iccid_resp_cb
(
 mmgsdi_return_enum_type  status,
 mmgsdi_cnf_enum_type     cnf,
 const mmgsdi_cnf_type   *cnf_ptr
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_IS_ISIM_APP

DESCRIPTION
  Function to determine if the passed app is an ISIM app

DEPENDENCIES
  None

RETURN VALUE
  TRUE - ISIM app
  FALSE - Not ISIM app

SIDE EFFECTS
  None
===========================================================================*/

boolean ds3g_mmgsdi_is_isim_app
(
 mmgsdi_app_info_type app_info
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_NON_PROV_ISIM_SESSION_REG_CB

DESCRIPTION
  This function handles the MMGSDI SESSION OPEN command callback for the
  non provisioned isim session. It will assign the global Session ID value
  which will be used in mmgsdi function calls

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_non_prov_isim_session_reg_cb
(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

/*===========================================================================

FUNCTION DS3G_MMGSDI_RESET_IMSI_INFO

DESCRIPTION
  This function resets the IMSI info.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_reset_imsi_info
(
  mmgsdi_app_enum_type         app_type,
  ds3g_mmgsdi_imsi_info_type  *imsi_info_ptr
);

/*===========================================================================
FUNCTION DS3G_MMGSDI_CLIENT_RSP_CB

DESCRIPTION
  This function handles the MMGSDI registration response callback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_client_rsp_cb
(
  mmgsdi_return_enum_type   status,
  mmgsdi_cnf_enum_type      cnf,
  const mmgsdi_cnf_type    *cnf_ptr
);


/*===========================================================================
FUNCTION      DS3G_MMGSDI_INIT

DESCRIPTION
  This Function registers DS manager as a client of MMGSDI

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_init( void )
{
  mmgsdi_return_enum_type        status = MMGSDI_SUCCESS;
  uint32                         session_mask = 0;
  mmgsdi_session_type_enum_type  ds3g_mmgsdi_session_type =
                                   MMGSDI_GW_PROV_PRI_SESSION;
  mmgsdi_session_type_enum_type  ds3g_mmgsdi_non_prov_isim_session_type =
                                   MMGSDI_NON_PROV_SESSION_SLOT_1;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_HIGH,"ds3g_mmgsdi_init");
  /* Initialize the MMGSDI info table */
  
  DS_SYSTEM_HEAP_MEM_ALLOC( ds3g_mmgsdi_imsi_info,
	  	            sizeof(ds3g_mmgsdi_imsi_info_type),ds3g_mmgsdi_imsi_info_type*);
  if (ds3g_mmgsdi_imsi_info == NULL)
  {
    return;
  }
  
  DS_SYSTEM_HEAP_MEM_ALLOC( ds3g_mmgsdi_info,
	  	            sizeof(ds3g_mmgsdi_info_type),ds3g_mmgsdi_info_type*);
					
  if (ds3g_mmgsdi_info == NULL)
  {
    return;
  }
  
  DS_SYSTEM_HEAP_MEM_ALLOC( ds3g_mmgsdi_isim_info,
	  	            sizeof(ds3g_mmgsdi_non_prov_isim_session_info_type),
					ds3g_mmgsdi_non_prov_isim_session_info_type*);
					
  if (ds3g_mmgsdi_isim_info == NULL)
  {
    return;
  }

  DS_SYSTEM_HEAP_MEM_ALLOC( mmgsdi_session_info_ptr,
	  	            sizeof(mmgsdi_session_open_info_type),
					mmgsdi_session_open_info_type*);
					
  if (mmgsdi_session_info_ptr == NULL)
  {
    return;
  }
  memset((void*)ds3g_mmgsdi_imsi_info, 0 , sizeof(ds3g_mmgsdi_imsi_info_type));
  memset((void*)mmgsdi_session_info_ptr, 0 , sizeof(mmgsdi_session_open_info_type));
  memset((void*)ds3g_mmgsdi_info, 0 , sizeof(ds3g_mmgsdi_info_type));
  memset((void*)ds3g_mmgsdi_isim_info, 0 , sizeof(ds3g_mmgsdi_non_prov_isim_session_info_type));
  ds3g_mmgsdi_info->client_id = DS3G_MMGSDI_INVALID_CLIENT_ID;

  ds3g_mmgsdi_info->session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
  ds3g_mmgsdi_info->session_type = ds3g_mmgsdi_session_type;
  ds3g_mmgsdi_info->app_type = MMGSDI_APP_NONE;
  ds3g_mmgsdi_info->iccid_len = 0xFF;
  ds3g_mmgsdi_subscription_ready_status.session_type =
    ds3g_mmgsdi_session_type;
  ds3g_mmgsdi_subscription_ready_status.subscription_ready_status = 0;
  ds3g_mmgsdi_isim_info->session_type = ds3g_mmgsdi_non_prov_isim_session_type;
  ds3g_mmgsdi_isim_info->session_id = DS3G_MMGSDI_INVALID_SESSION_ID;


  /* Initialize the IMSI_M info table */
  DS_SYSTEM_HEAP_MEM_ALLOC( ds3g_mmgsdi_imsi_m_info_i,
	  	            sizeof(ds3g_mmgsdi_imsi_m_info_type),
					ds3g_mmgsdi_imsi_m_info_type*);
					
  if(ds3g_mmgsdi_imsi_m_info_i == NULL)
  {
    return;
  }
  memset(ds3g_mmgsdi_imsi_m_info_i,0,sizeof(ds3g_mmgsdi_imsi_m_info_type));

  /* Register Client Id with MMGSDI */

  DS_SYSTEM_HEAP_MEM_ALLOC( client_id_ptr,
	  	            sizeof(mmgsdi_client_id_type), mmgsdi_client_id_type*);
					
  if(client_id_ptr == NULL)
  {
    return;
  }
  
  *client_id_ptr = DS3G_MMGSDI_INVALID_CLIENT_ID;
  *client_id_ptr = mmgsdi_client_id_and_evt_reg_sync
                   ( ds3g_mmgsdi_client_evt_reg_cb, NULL, 0);

  DS_MSG1(MSG_LEGACY_HIGH,"mmgsdi client_id_ptr = %d", *client_id_ptr);
  if(*client_id_ptr != DS3G_MMGSDI_INVALID_CLIENT_ID)
  {
    /* We get client Id */
    ds3g_mmgsdi_info->client_id = *client_id_ptr;
    DS_MSG1(MSG_LEGACY_HIGH,
           "ds3g_mmgsdi_client_id: Client Id = [%d]",
           *client_id_ptr);
    session_mask = MMGSDI_GW_PROV_PRI_SESSION_MASK;

    /* open 6 sessions */  //need to check
    status = mmgsdi_session_open_ext_sync(*client_id_ptr,
             session_mask, ds3g_mmgsdi_client_evt_reg_cb,
             mmgsdi_session_info_ptr);
    
    if ( status != MMGSDI_SUCCESS )
    {
      DS_MSG1(MSG_LEGACY_HIGH,
             "ds3g_mmgsdi_client_id_reg_cb(): Session Open Registration "
             "Failed status = [%d]",
            status);
    }
	ds3g_mmgsdi_info->session_id = mmgsdi_session_info_ptr->session_id;
    #ifdef FEATURE_DATA_ATCOP
	      /* Post client & session info to ATCoP */
    dsat_mmgsdi_client_init(mmgsdi_session_info_ptr, *client_id_ptr);
    #endif
  }
  else
  {
    DS_MSG1(MSG_LEGACY_ERROR,
          "mmgsdi_client_id_and_evt_reg(): Failed [%d]",status);
  } 
  return;
} /* ds3g_mmgsdi_init */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_VALIDATE_CLIENT_ID

DESCRIPTION
  Validate provided client id with client id getting from MMGSDI interface.

DEPENDENCIES
  None.

RETURN VALUE
 TRUE: Valid client id
 FALSE: Invalid client id

SIDE EFFECTS
 None
==============================================================================*/
 boolean ds3g_mmgsdi_validate_client_id
(
 mmgsdi_client_id_type  client_id
 )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (( client_id == ds3g_mmgsdi_info->client_id ) &&
      ( client_id != DS3G_MMGSDI_INVALID_CLIENT_ID ))
  {
    return TRUE;
  }
  else
  {
    DS_MSG1(MSG_LEGACY_ERROR, "Invalid client Id:%d", client_id);
    return FALSE;
  }
}

/*==============================================================================
FUNCTION DS3G_MMGSDI_CLIENT_EVT_REG_CB

DESCRIPTION
 This function handles the MMGSDI client event registration callback.

DEPENDENCIES
 None

RETURN VALUE
 None

SIDE EFFECTS
 None
===========================================================================*/
 void ds3g_mmgsdi_client_evt_reg_cb
(
 const mmgsdi_event_data_type  *event
 )
{
  ds_command_type                   *cmd_ptr = NULL;
  ds3g_mmgsdi_event_info_type   *mmgsdi_event_info_ptr = NULL;
  ds_at_mmgsdi_event_type       *cmd_rd_ptr = NULL;
  //ds_local_err_val_e_type       ret_val = DS_LOCAL_ERR_VAL_NONE;
  ds_local_err_val_e_type        ret_val = DS_LOCAL_ERR_VAL_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_HIGH,"ds3g_mmgsdi_client_evt_reg_cb");
  if ( event == NULL )
  {
    /* if not event info, we return right away */
    ret_val = DS_LOCAL_ERR_VAL_0;
    goto EXIT;;
  }

  /*-------------------------------------------------------------------------
    check client id
    -------------------------------------------------------------------------*/
  DS_MSG1(MSG_LEGACY_HIGH," client id in event reg cb %d", event->client_id);
  if ( ds3g_mmgsdi_validate_client_id( event->client_id ) != TRUE )
  {
    ret_val = DS_LOCAL_ERR_VAL_1;
    goto EXIT;;
  }
  DS_MSG3( MSG_LEGACY_HIGH,
      "ds3g_mmgsdi_client_evt_reg_cb(): event:%d client_id:%d session_id:%d",
      event->evt, event->client_id, event->session_id);

#ifdef FEATURE_DATA_ATCOP
  /* Post the event to ATCoP */
  dsat_mmgsdi_event_cb(event);
#endif /* FEATURE_DATA_ATCOP */
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_mmgsdi_event_info_type));
  if ( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    ret_val = DS_LOCAL_ERR_VAL_2;
    goto EXIT;
  }

  mmgsdi_event_info_ptr
    = (ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  mmgsdi_event_info_ptr->session_id = DS3G_MMGSDI_INVALID_SESSION_ID;

  switch( event->evt )
  {
    case MMGSDI_SUBSCRIPTION_READY_EVT:
      /* send command to ds task to process MMGSDI_SUBSCRIPTION_READY_EVT */
      mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_SUBSCRIPTION_READY_EVENT;
      mmgsdi_event_info_ptr->session_id = event->session_id;
      // record application type
      mmgsdi_event_info_ptr->app_type
        = event->data.subscription_ready.app_info.app_data.app_type;
      // record slot info
      mmgsdi_event_info_ptr->slot
        = event->data.subscription_ready.app_info.slot;
      break;

    case MMGSDI_REFRESH_EVT:
      DS_MSG1(MSG_LEGACY_HIGH,
          "MMGSDI_REFRESH_EVT: refresh stage [%d] ",
          event->data.refresh.stage);

      switch( event->data.refresh.stage )
      {
        case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT:
        {
          /* send refresh voting command to ds task */
          mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_REFRESH_VOTING_EVENT;
          mmgsdi_event_info_ptr->session_id = event->session_id;
          mmgsdi_event_info_ptr->source = event->data.refresh.source;

          memscpy( &(mmgsdi_event_info_ptr->refresh_enforcement_policy),
                    sizeof(mmgsdi_refresh_enforcement_policy_type),
                    &(event->data.refresh.enforcement_policy),
                    sizeof(mmgsdi_refresh_enforcement_policy_type));
        }
        break;

        case MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN:
        {
          /* send OK refresh command to ds task */
          mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_OK_REFRESH_EVENT;
          mmgsdi_event_info_ptr->session_id = event->session_id;
        }
        break;

        case MMGSDI_REFRESH_STAGE_START:
        {
          if (MMGSDI_REFRESH_NAA_FCN == event->data.refresh.mode)
          {
            ret_val = DS_LOCAL_ERR_VAL_3;
            goto EXIT;
          }

          mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_REFRESH_START_EVENT;
          mmgsdi_event_info_ptr->slot = event->data.refresh.slot;
          mmgsdi_event_info_ptr->app_type
            = event->data.refresh.app_info.app_data.app_type;

          DS_MSG0(MSG_LEGACY_HIGH, "process MMGSDI_REFRESH_STAGE_START");
        }
        break;

        case MMGSDI_REFRESH_STAGE_END_SUCCESS:
        {
          cmd_rd_ptr = &(mmgsdi_event_info_ptr->cmd_info.mmgsdi_event_info);

          if((NULL == event->data.refresh.refresh_files.file_list_ptr) ||
              (0 == event->data.refresh.refresh_files.num_files) ||
              (NULL == cmd_rd_ptr))
          {
             /* Free DS cmd buffer, this being a error case */
            ret_val = DS_LOCAL_ERR_VAL_4;
            goto EXIT;
          }

          DS_MSG1(MSG_LEGACY_HIGH,
              "Received file change notification [%d]",
              event->data.refresh.refresh_files.num_files);
          /* Fill in command type */
          cmd_rd_ptr->mmgsdi_event = event->evt;
          cmd_rd_ptr->cmd_status =(int16) MMGSDI_SUCCESS;
  
          /* Copy data to command buffer */
          cmd_rd_ptr->u.fcn_ind.num_files =
            event->data.refresh.refresh_files.num_files;
          memscpy(cmd_rd_ptr->u.fcn_ind.file_list,
              sizeof(cmd_rd_ptr->u.fcn_ind.file_list),
              event->data.refresh.refresh_files.file_list_ptr,
              (MIN(event->data.refresh.refresh_files.num_files,
                   DS3G_MMGSDI_MAX_NUM_REFRESH_FILES) *
               sizeof(mmgsdi_file_enum_type)));

          /* send File change event command to DS task */
          mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_REFRESH_COMPLETE_EVENT;
          mmgsdi_event_info_ptr->session_id = event->session_id;
        }
        break;

        default:
        {
          /* Free DS cmd buffer, this being a error case */
          ret_val = DS_LOCAL_ERR_VAL_5;
          goto EXIT;
        }
        break;
     }
	    break;
	
    case MMGSDI_SESSION_CHANGED_EVT: // --> need to check as becasue we have only one session only MPPM listening to this
      mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_SESSION_CHANGED_EVENT;
      mmgsdi_event_info_ptr->slot = event->data.session_changed.app_info.slot;
      mmgsdi_event_info_ptr->session_id = event->session_id;
      memscpy( &(mmgsdi_event_info_ptr->session_changed_info),
          sizeof(mmgsdi_session_changed_evt_info_type),
          &(event->data.session_changed),
          sizeof(mmgsdi_session_changed_evt_info_type) );
      break;

    case MMGSDI_CARD_ERROR_EVT:
      mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_CARD_ERROR_EVENT;
      mmgsdi_event_info_ptr->slot = event->data.card_error.slot;

      DS_MSG2(MSG_LEGACY_HIGH,
          "MMGSDI_CARD_ERROR_EVT: event_type [%d], slot [%d]",
          mmgsdi_event_info_ptr->event_type,
          mmgsdi_event_info_ptr->slot);
      break;

    default:
      // all other events are not of interest to DS manager
      /* Free DS cmd buffer, this being a error case */
      ret_val = DS_LOCAL_ERR_VAL_6;
      goto EXIT;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_MMGSDI_SIM_INFO;
  ds_put_cmd( cmd_ptr );

EXIT:
  if (ret_val != -1)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"evt_reg_cb failed return with ret_val %d:",ret_val);
    ds_release_cmd_buf(&cmd_ptr);
    return;
  }
  return;
} /* ds3g_mmgsdi_client_evt_reg_cb */

/*===========================================================================
FUNCTION DS3G_MMGSDI_PROCESS_CMD

DESCRIPTION
  A command processing function is called based on the type of command received.
  This function should be called by the top-level task dispatcher whenever
  any MMGSDI command is received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_process_cmd
(
 ds_command_type  *cmd_ptr
)
{
  ds3g_mmgsdi_event_info_type  *mmgsdi_event_info_ptr = NULL;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_ASSERT( cmd_ptr != NULL );
  DS_ASSERT( cmd_ptr->cmd_payload_ptr != NULL );
 
  DS_MSG0(MSG_LEGACY_HIGH,"ds3g_mmgsdi_process_cmd");
  mmgsdi_event_info_ptr = (ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;
  
  DS_MSG3(MSG_LEGACY_HIGH,
      "Processing cmd = [%d] event = [%d] session id = [%d]",
      cmd_ptr->hdr.cmd_id,
      mmgsdi_event_info_ptr->event_type,
      mmgsdi_event_info_ptr->session_id );

  /*-------------------------------------------------------------------------
    Invoke the appropriate function to process the command, based on ds3g
    mmgsdi events.
    -------------------------------------------------------------------------*/
  switch( mmgsdi_event_info_ptr->event_type )
  {

    case DS3G_MMGSDI_SUBSCRIPTION_READY_EVENT:
      ds3g_mmgsdi_subscription_ready_event_process( cmd_ptr );
#ifdef FEATURE_DATA_CC_SP_THROTTLING      
      // ds3g_mmgsdi_reset_cc_sp_throttle_if_required( cmd_ptr );   3gpp functions
      if(ds_3gpp_throt_sm_is_cc_sp_throttling_enabled ())
      {
        ds_3gpp_cc_sp_reset_throttling();
      }
#endif /* FEATURE_DATA_CC_SP_THROTTLING */
      break;

    case DS3G_MMGSDI_REFRESH_VOTING_EVENT:
      ds3g_mmgsdi_refresh_voting_event_process( cmd_ptr );
      break;

    case DS3G_MMGSDI_OK_REFRESH_EVENT:
      //      ds3g_mmgsdi_ok_refresh_event_process( cmd_ptr ); replacing with the functionality
      if((mmgsdi_status = mmgsdi_session_ok_to_refresh
	                     ( mmgsdi_event_info_ptr->session_id, TRUE,
		               ds3g_mmgsdi_client_rsp_cb,
                               ( mmgsdi_client_data_type )0 )) != MMGSDI_SUCCESS)
      {
        return;
      }
      break;

    case DS3G_MMGSDI_CARD_ERROR_EVENT:
      ds3g_mmgsdi_card_error_event_process( cmd_ptr );
#ifdef FEATURE_DATA_CC_SP_THROTTLING
      /*      ds3g_mmgsdi_reset_cc_sp_throttle_if_required( cmd_ptr );   3gpp functions
      if(cmd_ptr->cmd_payload_ptr != NULL && 
          ds_3gpp_throt_sm_is_cc_sp_throttling_enabled((sys_modem_as_id_e_type)
            (*((sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr))) == TRUE )
      {
        ds_cc_sp_throt_plmn_unblock_timer_exp_hdlr(
            (sys_modem_as_id_e_type)
            (*((sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr)));
      }*/
#endif /* FEATURE_DATA_CC_SP_THROTTLING */
      break;

    case DS3G_MMGSDI_REFRESH_START_EVENT:
      ds3g_mmgsdi_card_refresh_event_process( cmd_ptr ); 
      break;

    case DS3G_MMGSDI_REFRESH_COMPLETE_EVENT:
      ds3g_mmgsdi_refresh_fcn_event_process( cmd_ptr ); 
      break;

    case DS3G_MMGSDI_SESSION_CHANGED_EVENT:
	  
      DS_MSG1(MSG_LEGACY_HIGH,
            "ds3g_mmgsdi_session_changed_event_process activated = %d",
            (int)mmgsdi_event_info_ptr->session_changed_info.activated);
	     if(mmgsdi_event_info_ptr->session_changed_info.activated == FALSE)
      {
        mmgsdi_status = mmgsdi_session_close(
                               ds3g_mmgsdi_isim_info->session_id,
                               ds3g_mmgsdi_client_rsp_cb,
                               (mmgsdi_client_data_type)0);

        if (mmgsdi_status != MMGSDI_SUCCESS)
        {
          DS_MSG2(MSG_LEGACY_HIGH,
                "non-prov ISIM session close failed "
                "mmgsdi_status [%d] session id [%d]",
                 mmgsdi_status,
                 ds3g_mmgsdi_isim_info->session_id);
        }

        ds3g_mmgsdi_isim_info->is_operational = FALSE;
        memset(&ds3g_mmgsdi_isim_info->app_info,
                   0, sizeof(mmgsdi_aid_type));
        ds3g_mmgsdi_isim_info->channel_id = 0;
        ds3g_mmgsdi_isim_info->session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
        ds3g_mmgsdi_isim_info->slot_id = MMGSDI_MAX_SLOT_ID_ENUM;		
	     }
      break;

    case DS3G_MMGSDI_ISIM_REFRESH_COMPLETE_EVENT:
      ds3g_mmgsdi_isim_refresh_complete_processing( cmd_ptr );
      break;

    case DS3G_MMGSDI_MNC_INFO_EVENT:
      ds3g_mmgsdi_mnc_info_event_process();
      break;
    
    default:
      DS_MSG1(MSG_LEGACY_HIGH, "Unrecognized event type [%d] rcvd",
          mmgsdi_event_info_ptr->event_type);
      break;
  }
  return;
} /* ds3g_mmgsdi_process_cmd */

/*===========================================================================
FUNCTION DS3G_MMGSDI_SUBSCRIPTION_READY_EVENT_PROCESS

DESCRIPTION
  This function processes event when receiving MMGSDI_SUBSCRIPTION_READY_EVENT
  and it should be called by ds3g_mmgsdi_process_cmd()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_subscription_ready_event_process
(
 ds_command_type  *cmd_ptr
)
{
  mmgsdi_session_id_type         session_id;
  mmgsdi_session_type_enum_type  session_type = MMGSDI_MAX_SESSION_TYPE_ENUM;
  ds3g_mmgsdi_event_info_type   *mmgsdi_event_info_ptr = NULL;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_file_list_type refresh_files = {0};
  mmgsdi_file_enum_type         file_list;
  mmgsdi_app_enum_type           app_type;
  mmgsdi_access_type            iccid_access;
#ifdef FEATURE_DATA_RPM
  mmgsdi_return_enum_type        ret_val = MMGSDI_MAX_RETURN_ENUM;
  boolean                        iccid_changed = FALSE;

#endif /* FEATURE_DATA_RPM */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_HIGH,"ds3g_mmgsdi_subscription_ready_event_process");

  ASSERT(NULL != cmd_ptr);
  ASSERT(NULL != cmd_ptr->cmd_payload_ptr);

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;
  session_id = mmgsdi_event_info_ptr->session_id;

  DS_MSG2(MSG_LEGACY_HIGH,
      "Subscription Ready Event: session_id:%d app_type:%d ",
      session_id, mmgsdi_event_info_ptr->app_type);

  /* Get session id index from the table */
  if ( session_id == ds3g_mmgsdi_info->session_id)
  {
    /* update operational flag in the table */
    ds3g_mmgsdi_info->is_operational = TRUE;
    ds3g_mmgsdi_info->app_type
      = mmgsdi_event_info_ptr->app_type;
    session_type = ds3g_mmgsdi_info->session_type;

#if 0
    -->  /* posting DS3GEVENTMGR_MMGSDI_SUBSCRIPTION_READY_EV listened by MPPM */

      if( ds3g_mmgsdi_set_and_notify_subscription_ready_status(
            DS3G_MMGSDI_SUBSCRIPTION_READY_MASK,
            session_type) == FALSE )
      {
        DS_MSG0(MSG_LEGACY_HIGH,
            "cannot set DS3G_MMGSDI_SUBSCRIPTION_READY_MASK");
      }
#endif

    if( session_type == MMGSDI_GW_PROV_PRI_SESSION)
    {
      /* Invoke ISIM data refresh voting */
      /* Register for refresh voting */
      if((mmgsdi_status = mmgsdi_session_register_for_refresh_sync
	            (session_id, refresh_files, TRUE, FALSE))!= MMGSDI_SUCCESS)
      {
        DS_ERROR_LOG_1(
            "Failed to register for file change notification [=d]",
            mmgsdi_status);
      }
      if(ds3g_mmgsdi_subs_ready_to_process_usim(
            DS3G_MMGSDI_USIM_SUBSCRIPTION_READY_MASK,
            session_type) == FALSE)
      {
        DS_MSG0(MSG_LEGACY_HIGH,
            "subs not ready to process usim session");
      }

      /* Read register for EFacc file refresh */
      ds3g_mmgsdi_efacc_register_for_refresh(session_type);
      if ( ds3g_mmgsdi_get_app_type( session_type, &app_type ) == TRUE )
      {
        if ( app_type == MMGSDI_APP_USIM )
        {
          DS_MSG0(MSG_LEGACY_HIGH, "Registers UMTS file change from USIM");
          file_list = MMGSDI_USIM_ACC; /* EF for Access  Control Class */
          refresh_files.num_files = 1;
        }
        else if(app_type == MMGSDI_APP_SIM)
        {
          DS_MSG0(MSG_LEGACY_HIGH, "Registers UMTS file change from USIM");
          file_list = MMGSDI_GSM_ACC; /* EF for Access Control Class */
          refresh_files.num_files = 1;
        }
        refresh_files.file_list_ptr = &file_list ;
        if ((mmgsdi_status = 
                mmgsdi_session_register_for_refresh_sync( session_id,
                 refresh_files, TRUE, FALSE)) != MMGSDI_SUCCESS )    
        {
          DS_ERROR_LOG_1(
              "Failed to register for file change notification [=d]",
              mmgsdi_status);
        }
      }

      /* Read register for EFacc file refresh */
      ds3g_mmgsdi_efacc_register_for_refresh(session_type);
      ds3g_mmgsdi_read_efacc(ds3g_mmgsdi_info->session_id);

   	  ds_pdn_roaming_update_plmn_info_on_subs_ready();

#ifdef FEATURE_DATA_RPM
      ret_val = mmgsdi_rpm_has_iccid_changed(mmgsdi_event_info_ptr->slot,
          &iccid_changed);
      if (MMGSDI_SUCCESS == ret_val)
      {
        if (TRUE == iccid_changed)
        {
          ds_3gpp_rpm_process_iccid_chg();
        }
      }
      else
      {
        DS_MSG1(MSG_LEGACY_HIGH,
            "mmgsdi_rpm_has_iccid_changed returned: %d",ret_val);
      }
#endif /* FEATURE_DATA_RPM */

      if (ds3g_mmgsdi_info->is_operational == TRUE)
      {
        if ( ds3g_mmgsdi_info->client_id != DS3G_MMGSDI_INVALID_CLIENT_ID )
        {
          /* Get ICCID  */
          iccid_access.access_method = MMGSDI_EF_ENUM_ACCESS;
          iccid_access.file.file_enum = MMGSDI_ICCID;
          if((mmgsdi_session_read_transparent(ds3g_mmgsdi_info->session_id,
                  iccid_access,
                  DS3G_MMGSDI_ICCID_OFFSET,
                  DS3G_MMGSDI_ICCID_LEN,
                  ds3g_mmgsdi_iccid_resp_cb,
                  ( mmgsdi_client_data_type ) 0)) != MMGSDI_SUCCESS)
          {
            DS_MSG0( MSG_LEGACY_HIGH,"read iccid failed");
          }
        }
      }
      else
      {
        DS_MSG1( MSG_LEGACY_HIGH, "read iccid failed is_operation %d",
            ds3g_mmgsdi_info->is_operational == TRUE);
      }
    }
  }
  return;
} /* ds3g_mmgsdi_subscription_ready_event_process */

/*===========================================================================
FUNCTION DS3G_MMGSDI_SUBS_READY_TO_PROCESS_USIM

DESCRIPTION
  Function to determine if both subscription ready event and subs id info
  has been received for USIM session to process.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean ds3g_mmgsdi_subs_ready_to_process_usim
(
 uint8                          mask,
 mmgsdi_session_type_enum_type  session_type
)
{
  uint8                          ready_mask = 0;
  mmgsdi_session_type_enum_type  non_prov_session_type
    = MMGSDI_MAX_SESSION_TYPE_ENUM;
  /*- - - - - - - - - - - - - - -- - - - - -- - - - - - - - - - - - - - - */

  DS_MSG2(MSG_LEGACY_HIGH,
      "ds3g_mmgsdi_subs_ready_to_process_usim, mask %d session type %d",
      mask, session_type);

  if(session_type == MMGSDI_GW_PROV_PRI_SESSION )
  {

    ds3g_mmgsdi_subscription_ready_status.subscription_ready_status |= mask;

    ready_mask = DS3G_MMGSDI_USIM_SUBSCRIPTION_READY_MASK;

    if ((ds3g_mmgsdi_subscription_ready_status.subscription_ready_status & 
          ready_mask) == ready_mask)
    {
      ds3g_mmgsdi_subscription_ready_status.subscription_ready_status &=
        (~ready_mask);

      non_prov_session_type = MMGSDI_NON_PROV_SESSION_SLOT_1;
      if (ds3g_mmgsdi_isim_info->is_operational == FALSE)
      {
        ds3g_mmgsdi_open_non_prov_isim_session(MMGSDI_SLOT_1);
      }
      else
      {
        DS_MSG1(MSG_LEGACY_HIGH,
            "non-prov ISIM session already open on slot %d",
            MMGSDI_SLOT_1);
      }
    }
    if (ds3g_mmgsdi_info->is_operational == TRUE)
    {
      if(ds3g_mmgsdi_read_imsi(session_type) == FALSE)
      {
        DS_MSG1(MSG_LEGACY_ERROR,
            "Failed to read imsi info for session type [%d]",
            session_type);
      }
    }

    ds3g_mmgsdi_exception_data_allowed_update_cache();
    return TRUE;
  }
  return FALSE;
} /* ds3g_mmgsdi_subs_ready_to_process_usim */

/*===========================================================================
FUNCTION DS3G_MMGSDI_OPEN_NON_PROV_ISIM_SESSION

DESCRIPTION
  Function to open non-provisioned session for ISIM after subscription is
  ready

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_open_non_prov_isim_session
(
 mmgsdi_slot_id_enum_type  slot_id
)
{
  mmgsdi_available_apps_type    *mmgsdi_available_apps_ptr = NULL;
  mmgsdi_session_type_enum_type  non_provisioned_session
    = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_app_info_type          *isim_app_info = NULL;
  mmgsdi_non_prov_app_info_type  non_prov_app_reg_info = {{0}};
  mmgsdi_return_enum_type        status = MMGSDI_ERROR;
  uint8  app_index = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG1(MSG_LEGACY_HIGH,
      "ds3g_mmgsdi_open_non_prov_isim_session for slot %d",
      slot_id);

  DS_SYSTEM_HEAP_MEM_ALLOC( mmgsdi_available_apps_ptr,
	  	            sizeof(mmgsdi_available_apps_type),
					mmgsdi_available_apps_type*);

  if (mmgsdi_available_apps_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,
        "Could not allocate memory for mmgsdi_available_apps_ptr");
    return;
  }

  memset(mmgsdi_available_apps_ptr,0,sizeof(mmgsdi_available_apps_type));

  if(mmgsdi_get_all_available_apps_sync(ds3g_mmgsdi_info->client_id,
        mmgsdi_available_apps_ptr)
      != MMGSDI_SUCCESS)
  {
    DS_MSG0(MSG_LEGACY_HIGH,
        "Could not get avaialble apps from MMGSDI");
    DS_SYSTEM_HEAP_MEM_FREE(mmgsdi_available_apps_ptr);
    return;
  }

  if (mmgsdi_available_apps_ptr->card1_num_app != 0)
  {
    for (app_index = 0;
         app_index < mmgsdi_available_apps_ptr->card1_num_app;
         app_index++)
    {
      if(mmgsdi_available_apps_ptr->card1_app_info[app_index].slot
	                                         != MMGSDI_SLOT_1)
      {
        break;
      }

      if(ds3g_mmgsdi_is_isim_app(mmgsdi_available_apps_ptr->
            card1_app_info[app_index]) == TRUE)
      {
        isim_app_info = &mmgsdi_available_apps_ptr->card1_app_info[app_index];
      }
    }
  }
  if(isim_app_info == NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH, "No ISIM App found");
    DS_SYSTEM_HEAP_MEM_FREE(mmgsdi_available_apps_ptr);
    return;
  }

  non_prov_app_reg_info.exclusive_channel = FALSE;
  non_prov_app_reg_info.app_id_data.data_len
    = isim_app_info->app_data.aid.data_len;
  non_prov_app_reg_info.app_id_data.data_ptr
    = isim_app_info->app_data.aid.data_ptr;

  non_provisioned_session = MMGSDI_NON_PROV_SESSION_SLOT_1;

  /* register event call back. Register for refresh event to receive it in this callback */
  status = mmgsdi_session_open(ds3g_mmgsdi_info->client_id,
                               non_provisioned_session,
                               &non_prov_app_reg_info,
                               ds3g_mmgsdi_non_prov_isim_session_evt_reg_cb,
                               FALSE,
                               ds3g_mmgsdi_non_prov_isim_session_reg_cb,
                               (mmgsdi_client_data_type)0);

  if ( status != MMGSDI_SUCCESS )
  {
    DS_MSG1(MSG_LEGACY_HIGH,
        "ds3g_mmgsdi_open_non_prov_isim_session(): "
        "Session Open Registration Failed [%d]",
        status);
  }

  DS_SYSTEM_HEAP_MEM_FREE(mmgsdi_available_apps_ptr);

  return;
} /* ds3g_mmgsdi_open_non_provisioned_isim_session */



/*===========================================================================
FUNCTION DS3G_MMGSDI_NON_PROV_ISIM_SESSION_REG_CB

DESCRIPTION
  This function handles the MMGSDI SESSION OPEN command callback for the
  non provisioned isim session. It will assign the global Session ID value
  which will be used in mmgsdi function calls

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_non_prov_isim_session_reg_cb
(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  mmgsdi_refresh_file_list_type  refresh_files = {0};
  mmgsdi_session_type_enum_type  non_prov_session = MMGSDI_MAX_SESSION_TYPE_ENUM;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == cnf_ptr)
  {
    DS_ERROR_FATAL("cnf_ptr is NULL");
    return;
  }

  if (MMGSDI_SUCCESS == status)
  {
    if (MMGSDI_SESSION_OPEN_CNF == cnf)
    {
      non_prov_session = MMGSDI_NON_PROV_SESSION_SLOT_1;

      ds3g_mmgsdi_isim_info->is_operational = TRUE;
      ds3g_mmgsdi_isim_info->app_info =
                                         cnf_ptr->session_open_cnf.app_info;
      ds3g_mmgsdi_isim_info->channel_id =
                                         cnf_ptr->session_open_cnf.channel_id;
      ds3g_mmgsdi_isim_info->session_id =
                                         cnf_ptr->session_open_cnf.session_id;
      ds3g_mmgsdi_isim_info->slot_id =
                                         cnf_ptr->session_open_cnf.slot_id;

      DS_MSG3(MSG_LEGACY_HIGH,
                      "Opened ISIM session for session_type %d, "
                      "session_id %d, slot_id %d",
                      ds3g_mmgsdi_isim_info->session_type,
                      ds3g_mmgsdi_isim_info->session_id,
                      ds3g_mmgsdi_isim_info->slot_id);

      if((mmgsdi_status = mmgsdi_session_register_for_refresh_sync
	            (ds3g_mmgsdi_isim_info->session_id,
                  refresh_files, FALSE, FALSE))!= MMGSDI_SUCCESS )
       {
          DS_ERROR_LOG_1(
              "Failed to register for file change notification [=d]",
              mmgsdi_status);
       }

    }
    else
    {
      DS_MSG1(MSG_LEGACY_ERROR,
                      "Received Invalid CNF [%d] from mmgsdi", cnf);
    }
  }
  else
  {
    DS_MSG1(MSG_LEGACY_ERROR,
                    "Session ID Registration Failed for ISIM [%d]", status);
  }
  return;
} /* ds3g_mmgsdi_non_prov_isim_session_reg_cb */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_READ_IMSI

DESCRIPTION
  Get IMSI info afer receiving Subscription Ready Event

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

==============================================================================*/
boolean ds3g_mmgsdi_read_imsi
(
 mmgsdi_session_type_enum_type  session_type
)
{
  mmgsdi_access_type       imsi_access;
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_app_enum_type     app_type = MMGSDI_APP_NONE;
  mmgsdi_len_type          imsi_len = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_HIGH,"ds3g_mmgsdi_read_imsi");

  /* check client id */
  if ( ds3g_mmgsdi_info->client_id == DS3G_MMGSDI_INVALID_CLIENT_ID )
  {
    DS_MSG1(MSG_LEGACY_HIGH, "Invalid Client ID [%d]",
        ds3g_mmgsdi_info->client_id);
    return FALSE;
  }

  /* Get IMSI from USIM */

  ds3g_mmgsdi_get_app_type(session_type, &app_type);

  imsi_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  switch(app_type)
  {
    case MMGSDI_APP_USIM:
      imsi_access.file.file_enum = MMGSDI_USIM_IMSI;
      imsi_len = DS3G_MMGSDI_IMSI_LEN;
      break;

    case MMGSDI_APP_CSIM:
      imsi_access.file.file_enum = MMGSDI_CSIM_IMSI_M;
      imsi_len = DS3G_MMGSDI_IMSI_M_LEN;
      break;

    case MMGSDI_APP_SIM:
      imsi_access.file.file_enum = MMGSDI_GSM_IMSI;
      imsi_len = DS3G_MMGSDI_IMSI_LEN;
      break;

    case MMGSDI_APP_RUIM:
      imsi_access.file.file_enum = MMGSDI_CDMA_IMSI_M;
      imsi_len = DS3G_MMGSDI_IMSI_M_LEN;
      break;

    default:
      return FALSE;
  }

  /* Read the data */
  
  mmgsdi_status = mmgsdi_session_read_transparent(ds3g_mmgsdi_info->session_id,
									imsi_access,
									DS3G_MMGSDI_IMSI_OFFSET,
									imsi_len,
									ds3g_mmgsdi_imsi_resp_cb,
									( mmgsdi_client_data_type ) 0);

  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DS_MSG2(MSG_LEGACY_HIGH,
        "Failed to read IMSI using session type [%d], status [%d]",
        session_type, mmgsdi_status);
    return FALSE;
  }

  return TRUE;
}/* ds3g_mmgsdi_read_imsi */

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_IMSI_DATA()

DESCRIPTION
  Passed cached IMSI data to client.

DEPENDENCIES
  None

RETURN VALUE
  FALSE: Failed to read IMSI data
  TRUE: Read IMSI data successfully

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_get_imsi_data
(
  nv_ehrpd_imsi_type*    imsi_data_ptr
)
{

  if(imsi_data_ptr == NULL)
  {
    return FALSE;
  }
  else
  {
    if(ds3g_mmgsdi_imsi_info->ready_status == TRUE)
    {
      memscpy( (void*)imsi_data_ptr,
               sizeof(nv_ehrpd_imsi_type),
               (void*)&ds3g_mmgsdi_imsi_info->imsi_info,
               sizeof(nv_ehrpd_imsi_type) );
    }
    else
    {
      DS_ERROR_LOG_0("IMSI data is not ready for querying");
      return FALSE;
    }
  }
  
  return TRUE;
}

/*==============================================================================
FUNCTION  DS3G_MMGSDI_GET_APP_TYPE

DESCRIPTION
  Get session app type from ds3g_mmgsdi_info based on session_type

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Successfully getting app type from ds3g_mmgsdi_info
  FALSE: Failed to get app type from ds3g_mmgsdi_info

SIDE EFFECTS
None
==============================================================================*/
boolean ds3g_mmgsdi_get_app_type
(
 mmgsdi_session_type_enum_type   session_type,
 mmgsdi_app_enum_type           *app_type_ptr
)
{
  boolean  ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (( ds3g_mmgsdi_info->session_type == session_type ) &&
      ( ds3g_mmgsdi_info->is_operational ))
  {
    *app_type_ptr = ds3g_mmgsdi_info->app_type;
    ret_val = TRUE;
  }

  if(!ret_val)
  {
    DS_MSG1(MSG_LEGACY_ERROR, "session_type:%d not found",
        session_type);
  }

  return ret_val;
}

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_APP_TYPE_FROM_SESSION_ID

DESCRIPTION
  This function returns the app_type based on session_id.

PARAMETERS
  session_id : mmgsdi session id.

RETURN VALUE
  app_type : app_type corresponding to the given session_id.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_app_enum_type ds3g_mmgsdi_get_app_type_from_session_id
(
  mmgsdi_session_id_type  session_id
)
{
  if (ds3g_mmgsdi_info != NULL && 
  	   ds3g_mmgsdi_info->session_id == session_id)
  {
    return ds3g_mmgsdi_info->app_type;
  }

  return MMGSDI_APP_NONE;
} /* ds3g_mmgsdi_get_app_type_from_session_id */

/*==============================================================================
FUNCTION  DS3G_MMGSDI_READ_EFACC

DESCRIPTION
  Read EF ACC info during Subscription Ready/Refresh Events for GW sessions

DEPENDENCIES
  None.

PARAMETERS
  mmgsdi_session_type_enum_type session_type

RETURN VALUE
  TRUE-if the read is successful.
  FALSE-otherwise.

SIDE EFFECTS
  None

==============================================================================*/
boolean ds3g_mmgsdi_read_efacc
(
 mmgsdi_session_id_type  session_id
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_access_type      ef_access;
  mmgsdi_app_enum_type    app_type;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(session_id == DS3G_MMGSDI_INVALID_SESSION_ID)
  {
    DS_MSG1(MSG_LEGACY_HIGH, "Invalid session_type [%d]",
        session_id);
    return FALSE;
  }

  app_type   = ds3g_mmgsdi_info->app_type;

  DS_MSG2(MSG_LEGACY_HIGH,"App type[%d] Session id =[%d] from the table",
      app_type, session_id);

  if(MMGSDI_APP_USIM == app_type)
  {
    ef_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
    ef_access.file.file_enum = MMGSDI_USIM_ACC;
  }
  else if(MMGSDI_APP_SIM == app_type)
  {
    ef_access.access_method  = MMGSDI_EF_ENUM_ACCESS;
    ef_access.file.file_enum = MMGSDI_GSM_ACC;
  }
  else
  {
    /* Ef acc is not valid for non-GW session type .. ignoring */
    return FALSE;
  }
  mmgsdi_status = mmgsdi_session_read_transparent( session_id,
                  ef_access, 0, 0,
                  ds3g_mmgsdi_efacc_read_resp_hdlr_cb,
                  (mmgsdi_client_data_type ) 0);

  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DS_MSG2(MSG_LEGACY_HIGH,
        "Failed to read EFACC using session id [%d], status [%d]",
        session_id,
        mmgsdi_status);
    return FALSE;
  }
  return TRUE;
}/* ds3g_mmgsdi_read_efacc */

/*===========================================================================
FUNCTION DS3G_MMGSDI_NON_PROV_ISIM_SESSION_EVT_REG_CB

DESCRIPTION
  This function handles the MMGSDI client event registration callback for
  the non provisioned isim app.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_non_prov_isim_session_evt_reg_cb
(
 const mmgsdi_event_data_type  *event
)
{
  ds_command_type                  *cmd_ptr = NULL;
  ds3g_mmgsdi_event_info_type  *ds_mmgsdi_event_info = NULL;
  boolean                       event_valid = TRUE;
  ds_local_err_val_e_type        ret_val = DS_LOCAL_ERR_VAL_NONE;
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_ERROR;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( event == NULL )
  {
    /* if not event info, we return right away */
    ret_val = DS_LOCAL_ERR_VAL_0;
    goto EXIT;
  }

  /*-------------------------------------------------------------------------
    check client id
    -------------------------------------------------------------------------*/
  if ( ds3g_mmgsdi_validate_client_id( event->client_id ) != TRUE )
  {
    ret_val = DS_LOCAL_ERR_VAL_1;
    goto EXIT;

  }

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_mmgsdi_event_info_type));
  if ( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ret_val = DS_LOCAL_ERR_VAL_2;
    DS_ASSERT(0);
    goto EXIT;

  }

  ds_mmgsdi_event_info
    = (ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;
  ds_mmgsdi_event_info->session_id = DS3G_MMGSDI_INVALID_SESSION_ID;

  DS_MSG3(MSG_LEGACY_HIGH,
      "ds3g_mmgsdi_non_prov_isim_session_evt_reg_cb(): "
      "Session ID [%d] Event [%d] stage [%d]",
      event->session_id, event->evt, event->data.refresh.stage);

  switch( event->evt )
  {
    case MMGSDI_REFRESH_EVT:
      ds_mmgsdi_event_info->session_id = event->session_id;
      ds_mmgsdi_event_info->slot = event->data.refresh.slot;
      ds_mmgsdi_event_info->refresh_mode = event->data.refresh.mode;

      if (event->data.refresh.stage == MMGSDI_REFRESH_STAGE_START)
      {
        if( ds_mmgsdi_event_info->session_id == 
		                          ds3g_mmgsdi_isim_info->session_id)
        {
          mmgsdi_status = mmgsdi_session_refresh_complete(
              ds3g_mmgsdi_isim_info->session_id,
              TRUE, ds3g_mmgsdi_client_rsp_cb,
              (mmgsdi_client_data_type)0);
          /* Check to see if REQ was OK.  */
          if ( mmgsdi_status != MMGSDI_SUCCESS)
          {
            DS_MSG1(MSG_LEGACY_ERROR, "Failed to call MMGSDI OK to Refresh "
                "[%d]", mmgsdi_status);
          }
        }
      }
      else if(event->data.refresh.stage == MMGSDI_REFRESH_STAGE_END_SUCCESS)
      {
        ds_mmgsdi_event_info->event_type =
          DS3G_MMGSDI_ISIM_REFRESH_COMPLETE_EVENT;
      }
      else
      {
        DS_MSG1(MSG_LEGACY_HIGH,"Ignoring MMGSDI REFRESH stage %d",
            event->data.refresh.stage);
        ret_val = DS_LOCAL_ERR_VAL_3;
        goto EXIT;

      }
      break;
    default:
      event_valid = FALSE;
      /* all other events are not of interest to DS manager */
      break;
  }

  if (event_valid == TRUE)
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_MMGSDI_SIM_INFO;
    ds_put_cmd( cmd_ptr );
  }
  else
  {
    ret_val = 4;
    goto EXIT;

  }
EXIT:
  if (ret_val != -1)
  {
    DS_MSG1(MSG_LEGACY_HIGH, " isim_event_reg_cb ret_val %d", ret_val);
    /* Free DS cmd buffer, this being a error case */
    ds_release_cmd_buf(&cmd_ptr);
  }

  return;
} /* ds3g_mmgsdi_non_prov_isim_session_evt_reg_cb */

/*===========================================================================
FUNCTION DS3G_MMGSDI_ISIM_REFRESH_COMPLETE_PROCESSING

DESCRIPTION
  This function processes event when receiving MMGSDI_REFRESH_EVT with
  MMGSDI_REFRESH_STAGE_END_SUCCESS from MMGSDI interface.
  This function should be called by ds3g_mmgsdi_process_cmd().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_isim_refresh_complete_processing
(
 ds_command_type  *cmd_ptr
)
{
  ds3g_mmgsdi_event_info_type  *ds_mmgsdi_event_info = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_HIGH,
      "ds3g_mmgsdi_isim_refresh_complete_processing");

  if(cmd_ptr == NULL || cmd_ptr->cmd_payload_ptr == NULL)
  {
    ASSERT(0);
    return;
  }

  ds_mmgsdi_event_info = (ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  if (ds_mmgsdi_event_info->session_id == ds3g_mmgsdi_isim_info->session_id)
  {
    if (ds_mmgsdi_event_info->refresh_mode == MMGSDI_REFRESH_RESET ||
        ds_mmgsdi_event_info->refresh_mode == MMGSDI_REFRESH_3G_SESSION_RESET)
    {
      if(ds_3gpp_srv_req_throt_get_behavior() 
                        == DS_3GPP_SRV_REQ_THROT_BLOCK_PLMN_PREF_T3402)
      {
        DS_MSG0(MSG_LEGACY_HIGH,
                "Resetting pdn blocked in apn reject sm for mmgsdi refresh");
        ds_3gpp_dam_set_pdn_blocked(FALSE);
      }
    }
    else
    {
      DS_MSG0(MSG_LEGACY_HIGH,
          "MMGSDI refresh mode is not UICC reset of 3G reset. "
          "Not posting DS_3GPP_MMGSDI_UICC_3G_RESET_CMD");
    }
  }
  return;
} /* ds3g_mmgsdi_isim_refresh_complete_processing */

/*===========================================================================
FUNCTION DS3G_MMGSDI_READ_REFRESH_VOTE_OK_NV

DESCRIPTION    Utility to read from NV item - REFRESH VOTE OK

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3g_mmgsdi_read_refresh_vote_ok_nv 
(
   void
)
{
  char  item_path[] = "/nv/item_files/modem/uim/mmgsdi/refresh_vote_ok";

  if(ds_pdn_nv_manager_read_efs_nv(item_path,
                                   &ds3g_refresh_vote_ok,
                                   sizeof(boolean)) != NV_DONE_S)
  {
    ds3g_refresh_vote_ok = TRUE;
  }
  
  return;
}

/*===========================================================================
FUNCTION DS3G_MMGSDI_REFRESH_VOTING_EVENT_PROCESS

DESCRIPTION
  This function processes event when receiving MMGSDI_REFRESH_EVT with
  MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT from MMGSDI interface.
  This function should be called by ds3g_mmgsdi_process_cmd().

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_refresh_voting_event_process
(
 ds_command_type  *cmd_ptr
)
{
  mmgsdi_session_id_type         session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
  mmgsdi_session_type_enum_type  session_type = MMGSDI_GW_PROV_PRI_SESSION;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
  ds3g_mmgsdi_event_info_type   *mmgsdi_event_info_ptr = NULL;
  boolean                        vote_result = FALSE;
  mmgsdi_refresh_enforcement_policy_type 
                                 *refresh_enf_pol_ptr = NULL;
  boolean                         voting_ok_to_refresh_flag = FALSE;
  boolean                         voting_result = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(NULL != cmd_ptr);
  DS_ASSERT(NULL != cmd_ptr->cmd_payload_ptr);

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  session_id = mmgsdi_event_info_ptr->session_id;

  /* check session type */
  if (( ds3g_mmgsdi_info->session_id == session_id ) &&
      ( ds3g_mmgsdi_info->is_operational ))
  {
    session_type = ds3g_mmgsdi_info->session_type;
  }
  else
  {
    /* Voting OK to Refresh as session ID is not active and it doesnt effect DS */
    mmgsdi_status = mmgsdi_session_ok_to_refresh( session_id,
        TRUE, ds3g_mmgsdi_client_rsp_cb,
        ( mmgsdi_client_data_type ) 0 );
    /* Check to see if REQ was OK.  */
    if ( mmgsdi_status != MMGSDI_SUCCESS)
    {
      DS_MSG1(MSG_LEGACY_HIGH,
          "Failed to call MMGSDI OK to Refresh [%d]",
          mmgsdi_status);
    }
    return;
  }
  refresh_enf_pol_ptr = &(mmgsdi_event_info_ptr->refresh_enforcement_policy);

  if( refresh_enf_pol_ptr->is_enforcement_present == TRUE )
  {
    if( refresh_enf_pol_ptr->data.force_refresh_data_call == TRUE )
    {
      /* Voting OK to Refresh if enforcement policy is present and enforcement
         policy mentions force refresh over data call */
      voting_ok_to_refresh_flag = TRUE;
      voting_result = TRUE;
    }
  }
  else if( ds3g_refresh_vote_ok == TRUE )
  {
    /* Voting OK to Refresh if NV is set */
    voting_ok_to_refresh_flag = TRUE;
    voting_result = TRUE;
  }
  
  if(voting_ok_to_refresh_flag)
  {
    if((mmgsdi_status = mmgsdi_session_ok_to_refresh(
                          session_id, voting_result, ds3g_mmgsdi_client_rsp_cb,
                          (mmgsdi_client_data_type) 0 )) != MMGSDI_SUCCESS)
    {
       DS_MSG2(MSG_LEGACY_HIGH,
            "Refresh voting failed with return status [%d], "
            "session_type [%d]", mmgsdi_status, session_type);
    }
  }
  else
  {
    if( session_type == MMGSDI_GW_PROV_PRI_SESSION )
    {
   	  /* if the current system is not LTE, vote yes */
	     if ( ds_pdn_hdlr_get_current_network_mode() == SYS_SYS_MODE_LTE)
      {
		     	vote_result = ds_eps_bearer_mgr_isim_refresh_voting_hdlr();
      }
      else
      {
        vote_result = TRUE;
      }

      if((mmgsdi_status = mmgsdi_session_ok_to_refresh(
                           session_id, vote_result, ds3g_mmgsdi_client_rsp_cb,
                           ( mmgsdi_client_data_type ) 0 )) != MMGSDI_SUCCESS)
      {
        DS_MSG1(MSG_LEGACY_HIGH, "Failed to call MMGSDI OK to Refresh %d ", mmgsdi_status);
      }
    }
  }

   return;
} /* ds3g_mmgsdi_refresh_voting_event_process */

/*===========================================================================

FUNCTION DS3G_MMGSDI_RESET_IMSI_INFO

DESCRIPTION
  This function resets the IMSI info.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_mmgsdi_reset_imsi_info
(
  mmgsdi_app_enum_type         app_type,
  ds3g_mmgsdi_imsi_info_type  *imsi_info_ptr
)
{
  DS_MSG0(MSG_LEGACY_HIGH," REset IMSI data");
    /* Reset the IMSI info table */
  memset( imsi_info_ptr->imsi_response, 0, DS3G_MMGSDI_IMSI_LEN );
  imsi_info_ptr->mnc_response[0] = 0;
  memset( (void*)&imsi_info_ptr->imsi_info, 0, sizeof(nv_ehrpd_imsi_type));
  imsi_info_ptr->ready_status = FALSE;

} /* ds3g_mmgsdi_reset_imsi_info */


/*============================================================================
FUNCTION  DS3G_MMGSDI_CARD_REFRESH_EVENT_PROCESS 

DESCRIPTION
  This function is the handler function for the MMGSDI CARD ERROR event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void ds3g_mmgsdi_card_refresh_event_process
(
 ds_command_type  *cmd_ptr
)
{
  ds3g_mmgsdi_imsi_info_type     *imsi_info_ptr = NULL;
  ds3g_mmgsdi_event_info_type    *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_HIGH, "Card refresh event processing");
  if( (NULL == cmd_ptr) || (NULL ==  cmd_ptr->cmd_payload_ptr) )
  {
    ASSERT(0);
    return;
  }

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  if(mmgsdi_event_info_ptr->session_id ==  ds3g_mmgsdi_info->session_id)
  {
    /* update operational flag in the table */

    switch ( ds3g_mmgsdi_info->session_type )
    {
      case MMGSDI_GW_PROV_PRI_SESSION:
        /* Invalidate the IMSI  */

        imsi_info_ptr = ds3g_mmgsdi_imsi_info;

        if (imsi_info_ptr == NULL)
        {
          DS_MSG1(MSG_LEGACY_HIGH,
              "IMSI info ptr is NULL for session type[%d]",
              ds3g_mmgsdi_info->session_type);
          return;
        }
        ds3g_mmgsdi_reset_imsi_info(mmgsdi_event_info_ptr->app_type,
			                        imsi_info_ptr);
        break;

      default:
        DS_MSG1(MSG_LEGACY_ERROR,
            "Card refreshed/removed event for an invalid "
            "session type:%d",
            ds3g_mmgsdi_info->session_type);
        break;
    }
  }

  return;
}

/*===========================================================================
FUNCTION DS3G_MMGSDI_REFRESH_FCN_EVENT_PROCESS

DESCRIPTION
  This function processes event when receiving file change notification from
  MMGSDI interface. This function should be called by ds3g_mmgsdi_process_cmd()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_refresh_fcn_event_process
(
 ds_command_type  *cmd_ptr
)
{
  boolean                        pass_fail = TRUE;
  uint8                          index = 0;
  mmgsdi_session_id_type         session_id = DS3G_MMGSDI_INVALID_SESSION_ID;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_ERROR;
  dsat_fcn_ind_type              fcn_indication;
  ds3g_mmgsdi_event_info_type   *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT( NULL != cmd_ptr );
  DS_ASSERT( NULL != cmd_ptr->cmd_payload_ptr );

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  /* check session id */
  if ((ds3g_mmgsdi_get_session_id(&session_id) == FALSE) ||
  	   (mmgsdi_event_info_ptr->session_id != session_id))
  {
    DS_ERROR_LOG_0("Invalid session ID.");
    return;
  }

  fcn_indication =
    mmgsdi_event_info_ptr->cmd_info.mmgsdi_event_info.u.fcn_ind;

  if(( fcn_indication.num_files == 0 )||
     ( fcn_indication.num_files > DS3G_MMGSDI_MAX_NUM_REFRESH_FILES ))
  {
    pass_fail = FALSE;
  }
  else
  {
    while(( index < fcn_indication.num_files )&& ( pass_fail != FALSE ))
    {
      switch( fcn_indication.file_list[index] )
      {
#ifdef FEATURE_DATA_RPM
        /* RPM EF enums */
        case MMGSDI_USIM_7F66_PROP1_RPM_EF:
        case MMGSDI_USIM_7F66_PROP1_RPM_PARAM:
        case MMGSDI_USIM_7F66_PROP1_RPM_CNTR_LR:
        case MMGSDI_USIM_7F66_PROP1_RPM_CNTRS:
  
        case MMGSDI_SIM_7F66_PROP1_RPM_EF:
        case MMGSDI_SIM_7F66_PROP1_RPM_PARAM:
        case MMGSDI_SIM_7F66_PROP1_RPM_CNTR_LR:
        case MMGSDI_SIM_7F66_PROP1_RPM_CNTRS:
          ds_3gpp_rpm_handle_mmgsdi_refresh(fcn_indication.file_list[index],
                                            session_id);
          break;
#endif /* FEATURE_DATA_RPM */

        default:
          DS_ERROR_LOG_1("Unknown file [=d] in file list ; handling gracefully",
                         fcn_indication.file_list[index]);
          pass_fail = TRUE;
          break;
      }/* Switch end */
      
      index++;
    }/* While end */
  }

  mmgsdi_status = mmgsdi_session_refresh_complete( session_id,
                                                   pass_fail,
                                                   ds3g_mmgsdi_client_rsp_cb,
                                                   (mmgsdi_client_data_type)0 );
  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    DS_ERROR_LOG_1("Sending Refresh Complete to MMGSDI status [=d]",
                   mmgsdi_status);
  }

  DS_MSG2(MSG_LEGACY_HIGH, 
  	       "ds3g_mmgsdi_refresh_fcn_event_process() "
          "FCN number of files:%d pass_fail:%d",
          fcn_indication.num_files, (int)pass_fail);
  return;
}

/*============================================================================
FUNCTION  DS3G_MMGSDI_CARD_ERROR_EVENT_PROCESS 

DESCRIPTION
  This function is the handler function for the MMGSDI CARD ERROR event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void ds3g_mmgsdi_card_error_event_process
(
 ds_command_type  *cmd_ptr
)
{
  mmgsdi_session_type_enum_type   session_type;
  ds3g_mmgsdi_imsi_info_type     *imsi_info_ptr = NULL;
  ds3g_mmgsdi_event_info_type    *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT(NULL != cmd_ptr);
  DS_ASSERT(NULL != cmd_ptr->cmd_payload_ptr);

  mmgsdi_event_info_ptr =(ds3g_mmgsdi_event_info_type*)cmd_ptr->cmd_payload_ptr;

  DS_MSG0(MSG_LEGACY_HIGH, "Card error event processing");

  /* update operational flag in the table */
  session_type = ds3g_mmgsdi_info->session_type;
  switch ( session_type )
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      /* Invalidate the IMSI  */
      imsi_info_ptr = ds3g_mmgsdi_imsi_info;

      if (imsi_info_ptr == NULL)
      {
        DS_MSG1(MSG_LEGACY_HIGH,
            "IMSI info ptr is NULL for session type[%d]",
            session_type);
        return;
      }

      ds3g_mmgsdi_reset_imsi_info(mmgsdi_event_info_ptr->app_type,
                                   imsi_info_ptr); 
      break;

    default:
      DS_MSG1(MSG_LEGACY_ERROR,
          "Card refreshed/removed event for an invalid "
          "session type [%d]", session_type);
      break;
  }
  ds3g_mmgsdi_efacc_class_value = 0; // need to change for non-sub specific 
  
  /*-------------------------------------------------------------------- 
   As per REL 11, Clear the network override handler tbl here.
   Network override hdlr will have to be cleared when SIM ERROR EV occurs
   ---------------------------------------------------------------------*/
  ds_3gpp_network_override_hdlr_reset(FALSE);
  
  /*-------------------------------------------------------------------- 
   For Global throttling the only instance when Infinite throttling
   happens  is when T3396 is received as 0 and this is not cleared
   when USIM removal happens
  -------------------------------------------------------------------*/
  ds_3gpp_pdn_throt_clear_infinite_throttling();

  ds_pdn_throt_spec_clear_t3396_entry(FALSE, 
                        ds_pdn_nv_manager_is_rel_12_or_above_supported());

  /*--------------------------------------------------------------------
    For Rel 11 and above, reset the max simultaneous active pdp contexts 
    when CARD_ERROR_EV is received
  --------------------------------------------------------------------*/
  if(ds_pdn_nv_manager_is_rel_11_or_above_supported())
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Reset max active pdps in card err");
    ds_3gpp_pdn_reset_max_simul_active_pdp_context();
  }

  // ds3g_mmgsdi_reset_mmgsdi_info(index);
  ds3g_mmgsdi_info->app_type = MMGSDI_APP_NONE;

  ds3g_mmgsdi_info->is_operational = FALSE;
  ds3g_mmgsdi_info->iccid_len = 0xFF;
  memset( ds3g_mmgsdi_info->iccid, 0, DS3G_MMGSDI_ICCID_LEN );

  ds3g_mmgsdi_subscription_ready_status.subscription_ready_status = 0;

  return;
} /* ds3g_mmgsdi_card_error_event_process *

/*===========================================================================
FUNCTION DS3G_MMGSDI_IMSI_RESP_CB()

DESCRIPTION
  Gets IMSI data from the mmgsdi response, and copies into variables local
  to the file.  This is a callback function which is called from the
  mmgsdi context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_imsi_resp_cb
(
 mmgsdi_return_enum_type  status,
 mmgsdi_cnf_enum_type     cnf,
 const mmgsdi_cnf_type   *cnf_ptr
)
{
  mmgsdi_read_cnf_type         *read_cnf_ptr = NULL;
  ds3g_mmgsdi_imsi_info_type   *imsi_info_ptr = NULL;
  mmgsdi_session_id_type        session_id;
  mmgsdi_app_enum_type          app_type;
  mmgsdi_access_type             imsi_access;
  mmgsdi_return_enum_type       mmgsdi_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_HIGH,"ds3g_mmgsdi_imsi_resp_cb");

  if( NULL == cnf_ptr )
  {
    DS_MSG0(MSG_LEGACY_HIGH, "ds3g_mmgsdi_get_imsi_cb: cnf_ptr NULL");

    return;
  }
  read_cnf_ptr = (mmgsdi_read_cnf_type *) &(cnf_ptr->read_cnf);

  /*-------------------------------------------------------------------------
    check client id
    -------------------------------------------------------------------------*/
  if( ds3g_mmgsdi_validate_client_id(read_cnf_ptr->response_header.client_id )
      != TRUE )
  {
    DS_MSG1(MSG_LEGACY_HIGH, 
        "ds3g_mmgsdi_get_imsi_cb: Invalid client Id [%d] ",
        read_cnf_ptr->response_header.client_id);

    return;
  }

  if ( ( MMGSDI_READ_CNF == cnf ) &&
      ( MMGSDI_READ_CNF == read_cnf_ptr->response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) &&
        ( MMGSDI_SUCCESS == read_cnf_ptr->response_header.mmgsdi_status ) )
    {
      session_id = (cnf_ptr->read_cnf.response_header.session_id);
      app_type = ds3g_mmgsdi_info->app_type;

      if ( MMGSDI_APP_USIM == app_type || MMGSDI_APP_SIM == app_type)
      {
        imsi_info_ptr = ds3g_mmgsdi_imsi_info;
        if (imsi_info_ptr == NULL)
        {
          DS_MSG0(MSG_LEGACY_HIGH, "IMSI info ptr is NULL ");
          return;
        }

        /* Initialize the IMSI info table */
        memset( imsi_info_ptr->imsi_response, 0, DS3G_MMGSDI_IMSI_LEN );
        /*-----------------------------------------------------------------------
          The reading from IMSI efs was successful.  This copies the data from
          the mmgsdi response
          -----------------------------------------------------------------------*/
        if ( read_cnf_ptr->read_data.data_len <= DS3G_MMGSDI_IMSI_LEN )
        {
          memscpy( imsi_info_ptr->imsi_response,
              DS3G_MMGSDI_IMSI_LEN,
              read_cnf_ptr->read_data.data_ptr,
              read_cnf_ptr->read_data.data_len );
        }
        else
        {
          memscpy( imsi_info_ptr->imsi_response,
              DS3G_MMGSDI_IMSI_LEN,
              read_cnf_ptr->read_data.data_ptr,
              DS3G_MMGSDI_IMSI_LEN );
        }
        //        event_type = DS3G_MMGSDI_IMSI_INFO_EVENT; // no need of processing it implimented in this function itself
        /* Get AD from USIM */
        imsi_access.access_method = MMGSDI_EF_ENUM_ACCESS;
        imsi_access.file.file_enum = MMGSDI_USIM_AD;

        mmgsdi_status = mmgsdi_session_read_transparent( session_id,
            imsi_access,
            DS3G_MMGSDI_MNC_OFFSET,
            DS3G_MMGSDI_MNC_LEN,    // 1 Byte
            ds3g_mmgsdi_mnc_resp_cb,
            ( mmgsdi_client_data_type ) 0);
      }
      else
      {
        DS_MSG1(MSG_LEGACY_HIGH, "Invalid app type %d",app_type);
        return;
      }
    }
    else
    {
      DS_MSG2(MSG_LEGACY_ERROR,
          "Command failure status [%d] and mmgsdi_status [%d] ",
          status, read_cnf_ptr->response_header.mmgsdi_status);
    }
  }
  else
  {
    DS_MSG2(MSG_LEGACY_ERROR,
        "Improper command confirmation cnf[%d] response_type[%d]",
        cnf, read_cnf_ptr->response_header.response_type);
  }

  return;
} /* ds3g_mmgsdi_imsi_resp_cb */

/*===========================================================================
FUNCTION DS3G_MMGSDI_MNC_RESP_CB()

DESCRIPTION
  Gets MNC data length from the mmgsdi response, and copies into variables
  local to the file.  This is a callback function which is called from the
  mmgsdi context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_mnc_resp_cb
(
 mmgsdi_return_enum_type status,
 mmgsdi_cnf_enum_type    cnf,
 const mmgsdi_cnf_type  *cnf_ptr
)
{
  mmgsdi_read_cnf_type         *read_cnf_ptr = NULL;
  ds_command_type                  *cmd_buf;
  ds3g_mmgsdi_imsi_info_type   *imsi_info_ptr = NULL;
  ds3g_mmgsdi_event_info_type  *mmgsdi_event_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == cnf_ptr )
  {
    DS_MSG0(MSG_LEGACY_HIGH, "ds3g_mmgsdi_mnc_resp_cb: cnf_ptr NULL");
    return;
  }

  read_cnf_ptr = (mmgsdi_read_cnf_type *) &(cnf_ptr->read_cnf);

  imsi_info_ptr = ds3g_mmgsdi_imsi_info;
  if (imsi_info_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH, "IMSI info ptr is NULL ");
    return;
  }

  /*-------------------------------------------------------------------------
    check client id
    -------------------------------------------------------------------------*/
  if( ds3g_mmgsdi_validate_client_id(
        read_cnf_ptr->response_header.client_id ) != TRUE )
  {
    DS_MSG1(MSG_LEGACY_HIGH,
        "ds3g_mmgsdi_mnc_resp_cb: Invalid client Id [%d] ",
        read_cnf_ptr->response_header.client_id);

    return;
  }
  if ( ( MMGSDI_READ_CNF == cnf ) &&
      ( MMGSDI_READ_CNF == read_cnf_ptr->response_header.response_type ))
  {
    if ( ( MMGSDI_SUCCESS == status ) &&
        ( MMGSDI_SUCCESS == read_cnf_ptr->response_header.mmgsdi_status )
       )
    {
      /* Initialize the MNC info table */
      memset( imsi_info_ptr->mnc_response, 0, DS3G_MMGSDI_MNC_LEN );
      /*-----------------------------------------------------------------------
        The reading from AD efs was successful.  This copies the data from
        the mmgsdi response
        -----------------------------------------------------------------------*/
      if ( read_cnf_ptr->read_data.data_len <=
          DS3G_MMGSDI_MNC_LEN )
      {
        memscpy( imsi_info_ptr->mnc_response, DS3G_MMGSDI_MNC_LEN,
            read_cnf_ptr->read_data.data_ptr,
            read_cnf_ptr->read_data.data_len );
      }
      else
      {
        memscpy( imsi_info_ptr->mnc_response, DS3G_MMGSDI_MNC_LEN,
            read_cnf_ptr->read_data.data_ptr, DS3G_MMGSDI_MNC_LEN);
      }

      cmd_buf = ds_allocate_cmd_buf(sizeof(ds3g_mmgsdi_event_info_type));
      if((cmd_buf == NULL) || (cmd_buf->cmd_payload_ptr == NULL))
      {
        DS_ASSERT(0);
        return;
      }

      mmgsdi_event_info_ptr
        = (ds3g_mmgsdi_event_info_type*)cmd_buf->cmd_payload_ptr;
      /* Set Ready to read for clients*/
      imsi_info_ptr->ready_status = TRUE;
      mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_MNC_INFO_EVENT;

      cmd_buf->hdr.cmd_id = DS_CMD_MMGSDI_SIM_INFO;
      ds_put_cmd(cmd_buf);
    }
    else
    {
      DS_MSG2(MSG_LEGACY_ERROR,
          "Command failure status [%d] and mmgsdi_status [%d]",
          status, read_cnf_ptr->response_header.mmgsdi_status);
    }

  }
  else
  {
    DS_MSG2(MSG_LEGACY_ERROR,
        "Improper command confirmation cnf [%d] response_type [%d]",
        cnf, read_cnf_ptr->response_header.response_type);
  }
  return;
} /* ds3g_mmgsdi_mnc_resp_cb */

/*===========================================================================
FUNCTION DS3G_MMGSDI_EFACC_READ_RESP_HDLR_CB

DESCRIPTION
  Process EFACC data from the mmgsdi response, and posts a cmd to dstask for read
  confirmation.This is a callback function called from the mmgsdi task
  context.

PARAMETERS
  mmgsdi_return_enum_type   status
  mmgsdi_cnf_enum_type      cnf
  const mmgsdi_cnf_type     *cnf_ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_mmgsdi_efacc_read_resp_hdlr_cb
(
 mmgsdi_return_enum_type  status,
 mmgsdi_cnf_enum_type     cnf,
 const mmgsdi_cnf_type   *cnf_ptr
)
{
  if( (NULL == cnf_ptr) || (status != MMGSDI_SUCCESS))
  {
    DS_ERROR_LOG_0("NULL ptr or CB status is failure.");
    return;
  }

  if ( ( MMGSDI_READ_CNF == cnf ) &&
      ( MMGSDI_READ_CNF == cnf_ptr->read_cnf.response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) &&
        ( MMGSDI_SUCCESS == cnf_ptr->read_cnf.response_header.mmgsdi_status ) )
    {
      memscpy(&ds3g_mmgsdi_efacc_class_value,
              sizeof(uint32), cnf_ptr->read_cnf.read_data.data_ptr,
              cnf_ptr->read_cnf.read_data.data_len);
    }
  }
  else
  {
    DS_ERROR_LOG_2("Improper command confirmation cnf [%d] "
                   "response_type [%d]",
                   cnf, cnf_ptr->read_cnf.response_header.response_type);
  }
}/* ds3g_mmgsdi_efacc_read_resp_hdlr_cb */

/*===========================================================================
FUNCTION DS3G_MMGSDI_RESET_EFACC_VALUE

DESCRIPTION
  This function is called when  DS3G_MMGSDI_CARD_ERROR_EVENT is posted.
   It reset the efacc values to zero for the corresponding GW session.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_mmgsdi_reset_efacc_value
(
  void
)
{
  DS_MSG0(MSG_LEGACY_HIGH, "Resetting EFACC value.");
  ds3g_mmgsdi_efacc_class_value = 0;
}/* ds3g_mmgsdi_reset_efacc_value */

/*===========================================================================
FUNCTION DS3G_MMGSDI_GET_EFACC_CLASS
  
DESCRIPTION
   Returns true if any of the bits from b4-b8 is set in the byte 1 
   i.e 11 -15 as shown below. 
  
   Byte 1:
   b8 b7 b6 b5 b4 b3 b2 b1
   15 14 13 12 11 10 09 08 Number of the ACC (except for bit b3)
   
   if any of the bits from 11-15 are enabled then EFACC flag is set to true.
   Spec reference: TS 3GPP: 31.102
   Byte 2:
   b8 b7 b6 b5 b4 b3 b2 b1
   07 06 05 04 03 02 01 00 Number of the ACC

   In addition this returns the EFACC CLASS value
  
PARAMETERS
    Pointer to retrieve EFACC class
  
DEPENDENCIES
    None.
  
RETURN VALUE
    TRUE  - if bits b4-b8 are set in the byte 1 of ACC
    FALSE - otherwise
  
SIDE EFFECTS
    None.
============================================================================*/
boolean ds3g_mmgsdi_get_efacc_class
(
  uint32                *efacc_class
)
{
  boolean  ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  if (efacc_class == NULL)
  {
    return ret_val;
  }

  *efacc_class = 0;
  /* -------------------------------------------------------------------
   Byte 1:
   b8 b7 b6 b5 b4 b3 b2 b1
   15 14 13 12 11 10 09 08 Number of the ACC (except for bit b3)

   Byte 2:
   b8 b7 b6 b5 b4 b3 b2 b1
   07 06 05 04 03 02 01 00 Number of the ACC
  -----------------------------------------------------------------------*/

  if((ds3g_mmgsdi_efacc_class_value & ACC_CLASS_11_15_MASK) != 0) 
  {
    *efacc_class = ds3g_mmgsdi_efacc_class_value;
    ret_val = TRUE;
  }

  DS_MSG2(MSG_LEGACY_HIGH, "EFACC Class is %d, ret_val = %d", 
  	       *efacc_class, ret_val);  
  return ret_val;
}/* ds3g_mmgsdi_get_efacc_class */

/*===========================================================================
FUNCTION DS3G_MMGSDI_EFACC_REGISTER_FOR_REFRESH

DESCRIPTION
  This function registers for refresh on the EFacc files on SIM/USIM MMGSDI
  interface.

PARAMETERS
  mmgsdi_session_type_enum_type session_type

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_mmgsdi_efacc_register_for_refresh
(
  mmgsdi_session_type_enum_type session_type
)
{
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_refresh_file_list_type refresh_files = {0};
  mmgsdi_file_enum_type         file_list;
  mmgsdi_app_enum_type          app_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(session_type == MMGSDI_MAX_SESSION_TYPE_ENUM)
  {
    return;
  }
  if ( ds3g_mmgsdi_get_app_type( session_type, &app_type ) != TRUE )
  {
    return;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "app_type:%d", app_type);
  
  if ( app_type == MMGSDI_APP_USIM )
  {
    file_list = MMGSDI_USIM_ACC; /* EF for Access  Control Class */
    refresh_files.num_files = 1;
  }
  else if(app_type == MMGSDI_APP_SIM)
  {
    file_list = MMGSDI_GSM_ACC; /* EF for Access Control Class */
    refresh_files.num_files = 1;
  }
  else
  {
    return;
  }
  
  refresh_files.file_list_ptr = &file_list ;

   if( (mmgsdi_status = mmgsdi_session_register_for_refresh_sync
            (ds3g_mmgsdi_info->session_id,
             refresh_files, TRUE, FALSE))!= MMGSDI_SUCCESS )
   {
     DS_ERROR_LOG_1(
             "Failed to register for refresh [=d]",
              mmgsdi_status);
   }

  return;
}/* ds3g_mmgsdi_efacc_register_for_refresh */

/*===========================================================================
FUNCTION DS3G_MMGSDI_ICCID_RESP_CB()

DESCRIPTION
  Gets ICCID data from the mmgsdi response, and copies into variables local
  to the file.  This is a callback function which is called from the
  mmgsdi context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_iccid_resp_cb
(
 mmgsdi_return_enum_type  status,
 mmgsdi_cnf_enum_type     cnf,
 const mmgsdi_cnf_type   *cnf_ptr
)
{
  mmgsdi_read_cnf_type         *read_cnf_ptr = NULL;
  mmgsdi_session_id_type        session_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_MSG0(MSG_LEGACY_HIGH,"ds3g_mmgsdi_iccid_resp_cb");
  if( NULL == cnf_ptr )
  {
    DS_MSG0(MSG_LEGACY_HIGH, "ds3g_mmgsdi_get_iccid_cb: cnf_ptr NULL");
    return;
  }

  read_cnf_ptr = (mmgsdi_read_cnf_type *) &(cnf_ptr->read_cnf);

  /*-------------------------------------------------------------------------
    check client id
    -------------------------------------------------------------------------*/
  if( ds3g_mmgsdi_validate_client_id(read_cnf_ptr->response_header.client_id )
      != TRUE )
  {
    DS_MSG1(MSG_LEGACY_HIGH, 
        "ds3g_mmgsdi_get_iccid_cb: Invalid client Id [%d] ",
        read_cnf_ptr->response_header.client_id);
    return;
  }

  if ( ( MMGSDI_READ_CNF == cnf ) &&
      ( MMGSDI_READ_CNF == read_cnf_ptr->response_header.response_type ) )
  {
    if ( ( MMGSDI_SUCCESS == status ) &&
        ( MMGSDI_SUCCESS == read_cnf_ptr->response_header.mmgsdi_status ) )
    {
      session_id = cnf_ptr->read_cnf.response_header.session_id;
      if( ds3g_mmgsdi_info->session_id == session_id )
      {   

        /*-----------------------------------------------------------------------
          The reading of ICCID was successful.  This copies the data from
          the mmgsdi response
          -----------------------------------------------------------------------*/
        if ( read_cnf_ptr->read_data.data_len <= DS3G_MMGSDI_ICCID_LEN )
        {
          memscpy( ds3g_mmgsdi_info->iccid,
              DS3G_MMGSDI_ICCID_LEN,
              read_cnf_ptr->read_data.data_ptr,
              read_cnf_ptr->read_data.data_len );
          ds3g_mmgsdi_info->iccid_len = 
            read_cnf_ptr->read_data.data_len;
        }
        else
        {
          memscpy( ds3g_mmgsdi_info->iccid,
              DS3G_MMGSDI_ICCID_LEN,
              read_cnf_ptr->read_data.data_ptr,
              DS3G_MMGSDI_ICCID_LEN );
          ds3g_mmgsdi_info->iccid_len = DS3G_MMGSDI_ICCID_LEN; 
        }
      }
      else
      {
        DS_MSG0(MSG_LEGACY_HIGH,"session_id not matched");
        return;
      }

#if 0
      mmgsdi_event_info_ptr = 
        (ds3g_mmgsdi_event_info_type*)cmd_buf->cmd_payload_ptr;
      mmgsdi_event_info_ptr->event_type = DS3G_MMGSDI_ICCID_INFO_EVENT;
      mmgsdi_event_info_ptr->session_id =
        cnf_ptr->read_cnf.response_header.session_id;

      cmd_buf->hdr.cmd_id = DS_CMD_MMGSDI_SIM_INFO;
      ds_put_cmd(cmd_buf);
#endif
    }
    else
    {
      DS_MSG2(MSG_LEGACY_ERROR,
          "Command failure status [%d] and mmgsdi_status [%d] ",
          status, read_cnf_ptr->response_header.mmgsdi_status);
    }
  }
  else
  {
    DS_MSG2(MSG_LEGACY_ERROR,
        "Improper command confirmation cnf[%d] response_type[%d]",
        cnf, read_cnf_ptr->response_header.response_type);
  }
  return;
} /* ds3g_mmgsdi_iccid_resp_cb */

/*===========================================================================
FUNCTION DS3G_MMGSDI_IS_ISIM_APP

DESCRIPTION
  Function to determine if the passed app is an ISIM app

DEPENDENCIES
  None

RETURN VALUE
  TRUE - ISIM app
  FALSE - Not ISIM app

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_is_isim_app
(
 mmgsdi_app_info_type  app_info
)
{
  if ((app_info.app_data.aid.data_len >= DS3G_MMGSDI_ISIM_AID_CMP_LENGTH)
      && (memcmp(app_info.app_data.aid.data_ptr,
          ds3g_mmgsdi_isim_aid_value,
          DS3G_MMGSDI_ISIM_AID_CMP_LENGTH) == 0) )
  {
    return TRUE;
  }

  return FALSE;
} /* ds3g_mmgsdi_is_isim_app */


/*===========================================================================
FUNCTION DS3G_MMGSDI_CLIENT_RSP_CB

DESCRIPTION
  This function handles the MMGSDI registration response callback.

DEPENDENCIES
  None


RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds3g_mmgsdi_client_rsp_cb
(
  mmgsdi_return_enum_type   status,
  mmgsdi_cnf_enum_type      cnf,
  const mmgsdi_cnf_type    *cnf_ptr
)
{
  DS_ASSERT( cnf_ptr != NULL );

  DS_MSG4(MSG_LEGACY_HIGH,
                  "ds3g_mmgsdi_client_rsp_cb() for cnf = [%d] "
                  "with status = [%d] mmgsdi_status = [%d] client_data = [%d]",
                  cnf, status,
                  cnf_ptr->response_header.mmgsdi_status,
                  cnf_ptr->read_cnf.response_header.client_data );

  return;
}/* ds3g_mmgsdi_client_rsp_cb */

/*===========================================================================

FUNCTION DS3G_GET_MMGSDI_INFO_HNDL

DESCRIPTION
  This function returns ds3g_mmgsdi_info_handler

DEPENDENCIES
  None

RETURN VALUE
  ds3g_mmgsdi_info_handler

SIDE EFFECTS
  None

===========================================================================*/
ds3g_mmgsdi_info_type * ds3g_get_mmgsdi_info_hndl
(
)
{
  return ds3g_mmgsdi_info;
}

/*===========================================================================
FUNCTION DS3G_MMGSDIF_READ_NAS_CONFIG_FILE
 
DESCRIPTION
  This function reads the NAS Config File from USIM and fetches the 
  Tag Value coressponding to the Tag Id specified in the input
 
PARAMETERS 
  1. Tag Id
  2. Output ptr to write the Tag Value
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  TRUE if Tag Value can be read successfully. 
  FALSE otherwise.
 
SIDE EFFECTS 
  None   
===========================================================================*/
boolean ds3g_mmgsdi_read_nas_config_file
(
  ds3g_mmgsdi_tag_e_type    tag_id,
  void                      *tag_value_p
)
{
  boolean                      ret_val          = FALSE;
  mmgsdi_return_enum_type      mmgsdi_ret_val   = MMGSDI_MAX_RETURN_ENUM;
  mmgsdi_session_id_type       session_id = ds3g_mmgsdi_info->session_id;
  mmgsdi_data_type             read_data;
  mmgsdi_len_type              cache_len = 0;
  byte                        *efnas_config = NULL;
  ds_local_err_val_e_type      local_err = DS_LOCAL_ERR_VAL_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  read_data.data_len = 0;
  read_data.data_ptr = NULL;

  do
  {
    /*---------------------------------------------------------------------- 
      1. Get the cache file size
      2. Read the value from the cache
   ----------------------------------------------------------------------*/
    mmgsdi_ret_val = mmgsdi_session_read_cache_file_size(session_id,
                                                         MMGSDI_USIM_NASCONFIG,
                                                         &cache_len);                                                         
    if (mmgsdi_ret_val != MMGSDI_SUCCESS)
    {
      local_err = DS_LOCAL_ERR_VAL_0;
      break;
    }

    read_data.data_len = cache_len;
    DS_SYSTEM_HEAP_MEM_ALLOC(read_data.data_ptr, cache_len, uint8 *);
    if (read_data.data_ptr == NULL)
    {
      local_err = DS_LOCAL_ERR_VAL_1;
      break;
    }
    memset(read_data.data_ptr, 0, read_data.data_len);

    mmgsdi_ret_val = mmgsdi_session_read_cache(session_id,
                                               MMGSDI_USIM_NASCONFIG,
                                               read_data);
    if (mmgsdi_ret_val != MMGSDI_SUCCESS)
    {
      local_err = DS_LOCAL_ERR_VAL_2;
      break;
    }
    
    /*----------------------------------------------------------------- 
      Get the low priority bit from the utility function
      If this call returns TRUE, it means LOW PRIO SIGNALING
      is set in the FILE and the value will be populated in low_prio_sig
    ----------------------------------------------------------------*/
    if(ds3g_mmgsdi_read_nas_config_tag(read_data.data_ptr,
                                       read_data.data_len,
                                       tag_id,
                                       tag_value_p))
    {
      DS_MSG1(MSG_LEGACY_HIGH,
      	       "Tag Id: %x configured in the USIM NAS Config file", tag_id);
      ret_val = TRUE;
      break;
    }
  } while (0); 

  if(!ret_val)
  {
    DS_MSG0(MSG_LEGACY_HIGH, "Try to fetch EFNAS Config Info from EFS");

    efnas_config = ds_pdn_nv_manager_get_efnas_config();    
    if (efnas_config != NULL)
    {
      if(!ds3g_mmgsdi_read_nas_config_tag
           ((uint8 *)efnas_config, DS_PDN_EFNAS_CONFIG_SIZE, tag_id, tag_value_p))
      {
        local_err = DS_LOCAL_ERR_VAL_3;
      }
      else
      {
        ret_val = TRUE;
      }
    }
    else
    {
      local_err = DS_LOCAL_ERR_VAL_4;
    }
  }

  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_1("EFNAS read error : =d", local_err);
  }

  if (read_data.data_ptr != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(read_data.data_ptr);
  }

  return ret_val;
} /* ds3g_mmgsdi_read_nas_config_file */

/*===========================================================================
FUNCTION DS3G_MMGSDIF_GET_NAS_CONFIG_TAG_LENGTH
 
DESCRIPTION 
 The function takes NAS CONFIG read from UIM and for every TAG gives the
 tag length
 
 It also gives the offset --> Offset signifies how many bytes the length field
 occupy

PARAMETERS 
  Input File stream 
  Offset
 
DEPENDENCIES 
  The first byte must always be pointing to the stream length
  
RETURN VALUE 
  tag_length
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
uint8 ds3g_mmgsdif_get_nas_config_tag_length
(
   uint8   *input_file_stream,
   uint8   *offset,
   uint8    input_file_length
)
{
  uint8 tag_length = 0; 
  uint8 first_byte = 0;
  uint8 i = 0;
  uint8 number_of_octets = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if((input_file_stream == NULL) || (offset == NULL) ||
     (input_file_length == 0))
  {
    return tag_length;
  }

  /*---------------------------------------------------------------------- 
    Point the first byte to stream length
   
    If firstbyte has the MSB set, IT means the length field stretches
    over multiple bytes, Multiply by 256 and add the value for each bit.

    A value of 0xFF is not allowed as per ISO/IEC 8825-1
    ----------------------------------------------------------------------*/
  first_byte = *input_file_stream++;

  if(first_byte == 0xFF)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Invalid number of length octets");
    return tag_length;
  }
  else
  {
    if(first_byte & 0x80)
    { 
    /* Multi byte length */
      number_of_octets = first_byte & 0x7F;
      if ((number_of_octets + 1) >= input_file_length)
      {
        DS_MSG2(MSG_LEGACY_ERROR,"Too many length octets: Input_file_length:%d,"
                           "Number of octets:%d",
                           input_file_length, number_of_octets);
        return tag_length;
      }
      for(i = 0; i < number_of_octets; i++ )
      {
        tag_length *= (uint8)256;
        tag_length += *input_file_stream++;
      }
      *offset = (uint8)number_of_octets;
    } 
    else
    {
      /*Single byte length*/
      tag_length = first_byte;
      *offset = 1;
    }
  }

  /* Check if after reading the tag in the calling function, the file length 
  will not be violated */
  if (tag_length + (*offset) > input_file_length)
  {
   DS_MSG0(MSG_LEGACY_ERROR,"Tag length and offset is greater than file length");
    tag_length = 0;
  }
  
  DS_MSG1(MSG_LEGACY_HIGH,"Tag length returned is %d",tag_length);
  return tag_length;
}/*ds3g_mmgsdif_get_nas_config_tag_length */

/*===========================================================================
FUNCTION DS3G_MMGSDIF_READ_NAS_CONFIG_TAG
 
DESCRIPTION
  This function takes a byte stream and fetches Tag Value
  coressponding to the Tag Id from the byte stream
 
PARAMETERS 
  1. Byte stream
  2. Length of the byte stream
  3. Tag Id
  4. Output ptr to write the Tag Value
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  TRUE if Tag Value can be read successfully. 
  FALSE otherwise.
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
boolean ds3g_mmgsdi_read_nas_config_tag
(
  uint8                  *input_file_stream,
  uint8                   input_file_length,
  ds3g_mmgsdi_tag_e_type  tag_id,
  void                   *output_tag_value_p
)
{
  boolean                 ret_val          = FALSE;
  boolean                 tag_set          = FALSE;
  uint8                   offset_length    = 0;
  uint8                   tag_length       = 0;
  ds3g_mmgsdi_tag_e_type  current_tag_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ((input_file_stream == NULL) || (output_tag_value_p == NULL))
  {
     return ret_val;
  }

  /*----------------------------------------------------------------------- 
    Check for tag 80 in the input file stream
    80 would mean low priority signaling
    -----------------------------------------------------------------------*/

  while ((input_file_length > 0) && (tag_set == FALSE))
  {
    current_tag_id = *input_file_stream;

    /*----------------------------------------------------------------------- 
      Check to make sure that the tag 80 is in valid range
      -----------------------------------------------------------------------*/

    if (current_tag_id < DS3G_MMGSDI_TAG_LOW_PRIORITY_SIG || 
        current_tag_id > DS3G_MMGSDI_TAG_EXCEP_DATA_REPORT_ALLOWED)
    {
      DS_MSG1(MSG_LEGACY_ERROR,"Invalid tag found:0x%x", current_tag_id);
      return ret_val;
    }
    
    input_file_stream++;
    input_file_length--;

    tag_length = ds3g_mmgsdif_get_nas_config_tag_length(input_file_stream,
                                                        &offset_length,
                                                        input_file_length);

    if (tag_length == 0)
    {
      DS_MSG0(MSG_LEGACY_ERROR,"Invalid tag length");
      return ret_val;
    }
    
    input_file_length = input_file_length -(tag_length + offset_length);

    switch (current_tag_id)
    {
      case DS3G_MMGSDI_TAG_SM_RETRY_RAT_CHANGE:
      case DS3G_MMGSDI_TAG_SM_RETRY_TIMER:
      case DS3G_MMGSDI_TAG_LOW_PRIORITY_SIG:
      case DS3G_MMGSDI_TAG_EXCEP_DATA_REPORT_ALLOWED :
        if (current_tag_id == tag_id)
        {
          /*------------------------------------------------------------------- 
            Tag length for SM Retry at RAT Change, SM Retry Timer, and
            Exception data allowed should be 1 byte
            -------------------------------------------------------------------*/
          if (tag_length == 1)
          {
            input_file_stream += offset_length;
            ret_val = TRUE;
            *((uint8*)output_tag_value_p) = *input_file_stream;
          }
          else
          {
            DS_MSG0(MSG_LEGACY_ERROR,"Invalid tag length for a known tag");
          }
          tag_set = TRUE;
        }
        else
        {
          input_file_stream = input_file_stream + (tag_length + offset_length);
        }
        break;


      default:
       input_file_stream = input_file_stream + (tag_length + offset_length);
       break;
    }
  }

  return ret_val;
} /* ds3g_mmgsdi_read_nas_config_tag */

/*===========================================================================
FUNCTION ds3g_mmgsdi_exception_data_allowed_update_cache

DESCRIPTION
  This function updates the cache with the values read from EFNAS
  config file on USIM

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds3g_mmgsdi_exception_data_allowed_update_cache
(
  void
)
{
  boolean mo_exception_allowed = FALSE;
/*---------------------------------------------------------------------------*/

  if (ds3g_mmgsdi_read_nas_config_file
  	                       (DS3G_MMGSDI_TAG_EXCEP_DATA_REPORT_ALLOWED, 
                           (void*)&mo_exception_allowed) == FALSE)
  {
    ds_pdn_cp_data_set_mo_exception_support(FALSE);
  }
  else
  {
    ds_pdn_cp_data_set_mo_exception_support(mo_exception_allowed);
  }

  return;
}

/*===========================================================================
FUNCTION DS3G_MMGSDI_IS_SUBS_READY

DESCRIPTION
  Function to determine if subscription ready has been received

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If suscription ready was received
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds3g_mmgsdi_is_subs_ready
(
  void
)
{
  return ds3g_mmgsdi_info->is_operational;  
} /* ds3g_mmgsdi_is_subs_ready */

/*===========================================================================
FUNCTION  DS3G_MMGSDI_MNC_INFO_EVENT_PROCESS

DESCRIPTION
  This function is the handler function for DS3G_MMGSDI_MNC_INFO_EVENTS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_mmgsdi_mnc_info_event_process
(
  void
)
{

#ifdef FEATURE_DATA_RPM
  /* Invoke RPM SIM read    */
  ds_3gpp_rpm_read_sim_info();
#endif /* FEATURE_DATA_RPM */

  ds_pdn_throt_spec_imsi_info_avail_cmd_hdlr();
  
  if(ds_3gpp_pdn_limit_is_enabled())
  {
    ds_3gpp_pdn_limit_imsi_info_avail_cmd_hdlr();
  }
  
  return;
}

/*==============================================================================
FUNCTION  DS3G_MMGSDI_GET_SESSION_ID

DESCRIPTION
  Get session id getting from MMGSDI interface

DEPENDENCIES
  None.

RETURN VALUE
  session id

SIDE EFFECTS
  None
==============================================================================*/
boolean ds3g_mmgsdi_get_session_id
(
  mmgsdi_session_id_type         *session_id_ptr
)
{
  if (ds3g_mmgsdi_info != NULL &&
  	   ds3g_mmgsdi_info->is_operational)
  {
    *session_id_ptr = ds3g_mmgsdi_info->session_id;
    return TRUE;
  }

  return FALSE;
} /* ds3g_mmgsdi_get_session_id */


#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */