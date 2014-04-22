/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   I M S I   S W T I C h   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the processing of imsi switch logic 

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2023 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/dev/uim.mpss/3.3.1/tqidwai.uim.mpss.3.3.1.3454742_3_3_1/mmgsdi/src/mmgsdi_imsi_swithc.c#1 $$ $DateTime: 2023/04/04 00:54:00 $

when       who      what, where, why
--------   ---      ---------------------------------------------------------- 
05/23/23   tq      Add refresh reset after switching profile
05/23/23   tq      Initial version
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "custuim.h"
#include "intconv.h"
#include "uim.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_gen.h"
#include "fs_lib.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_uicc.h"
#include "uim_common_efs.h"
#include "mmgsdi_imsi_switch.h"
#include "mmgsdi.h"
#include "mmgsdi_ss_event.h"
#include "mmgsdi_uicc_rsp.h"
#include "mmgsdi_refresh.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */


/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
static  uint8                              switch_imsi_cnt                          = MMGSDI_IMSI_SWITCH_MAX_PING_PONG_CNT;
boolean                                    mmgsdi_imsi_switch_timer_running         = FALSE;
mmgsdi_imsi_switch_nw_enum_type            mmgsdi_imsi_switch_crnt_nw_type          = MMGSDI_IMSI_SWITCH_NW_UNKNOWN;
mmgsdi_imsi_switch_profile_id_enum_type    mmgsdi_imsi_switch_crnt_active_profile   = MMGSDI_IMSI_SWITCH_PROFILE_UNKNOWN;
uint8                                      mmgsdi_switch_imsi_enable                = 0;
/* Tick is in ms*/
uint32                                     mmgsdi_imsi_switch_timer_tick            = 0; 
/* Tick is in ms*/ 
uint32                                     mmgsdi_imsi_switch_ping_pong_tick        = 0;
sys_srv_status_e_type                      mmgsdi_imsi_switch_srv_status            = SYS_SRV_STATUS_NONE;
sys_plmn_id_s_type                         dummy_plmn_id                            = {{0xFF, 0xFF, 0xFF}};

/*===========================================================================
FUNCTION MMGSDI_SWITCH_IMSI_SELECT

DESCRIPTION
  Select EF before send APDU cmd for switch IMSI

DEPENDENCIES
  None

RETURN VALUE:
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          
  MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED
  MMGSDI_ERROR           

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_switch_imsi_select(void)
{
  mmgsdi_return_enum_type mmgsdi_status          = MMGSDI_SUCCESS;
  mmgsdi_get_file_attr_cnf_type *select_cnf_ptr  = NULL;
  mmgsdi_get_file_attr_req_type *select_ptr      = NULL;


  UIM_MSG_HIGH_0("mmgsdi_switch_imsi_select");
  
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(select_ptr,
                                     sizeof(mmgsdi_get_file_attr_req_type));
  if(select_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  select_ptr->request_header.client_id          = mmgsdi_generic_data_ptr->client_id;
  select_ptr->request_header.session_id         = mmgsdi_generic_data_ptr->pri_gw_session_id;
  select_ptr->request_header.request_type       = MMGSDI_GET_FILE_ATTR_REQ;
  select_ptr->request_header.orig_request_type  = MMGSDI_GET_FILE_ATTR_REQ;
  select_ptr->access.access_method              = MMGSDI_BY_PATH_ACCESS;
  select_ptr->access.file.path_type.path_len    = 0x03;
  select_ptr->access.file.path_type.path_buf[0] = MMGSDI_MF_DIR;
  select_ptr->access.file.path_type.path_buf[1] = 0x7FFF;
  select_ptr->access.file.path_type.path_buf[2] = 0x6FC8;
  select_ptr->request_header.slot_id            = MMGSDI_SLOT_1;
  select_ptr->skip_uicc_arr                     = TRUE;

  mmgsdi_status = mmgsdi_uim_uicc_select(select_ptr,
                                         TRUE,
                                         &select_cnf_ptr,
                                         TRUE);
  if(mmgsdi_status != MMGSDI_SUCCESS ||
       select_cnf_ptr == NULL ||
       select_cnf_ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_status = MMGSDI_ERROR;
  }
        
  /* Free request and confirm pointers */
  mmgsdi_util_free_response_ptr((mmgsdi_cnf_type*)select_cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(select_cnf_ptr);
  MMGSDIUTIL_TMC_MEM_FREE(select_ptr);

  return mmgsdi_status;
}/* mmgsdi_switch_imsi_select */


/*===========================================================================
FUNCTION MMGSDI_SWITCH_IMSI_GET_OTHER_TN_PROFILE

DESCRIPTION
  This function is called to get other TN profile

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_imsi_switch_profile_id_enum_type
  MMGSDI_PROFILE_1
  MMGSDI_PROFILE_2

SIDE EFFECTS
  NONE
===========================================================================*/
static mmgsdi_imsi_switch_profile_id_enum_type mmgsdi_switch_imsi_get_other_tn_profile(mmgsdi_imsi_switch_profile_id_enum_type profile_id)
{

  UIM_MSG_HIGH_1("mmgsdi_switch_imsi_get_other_tn_profile profile_id 0x%x", profile_id);
 
  if(profile_id == MMGSDI_IMSI_SWITCH_PROFILE_1)
  {
    return MMGSDI_IMSI_SWITCH_PROFILE_2;
  }

  return MMGSDI_IMSI_SWITCH_PROFILE_1;
}/* mmgsdi_switch_imsi_get_other_tn_profile */


/*===========================================================================
FUNCTION MMGSDI_SWITCH_IMSI

DESCRIPTION
  This function is called in mmgsdi_process_command.
  It will switch the profile to the given profile id.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_switch_imsi(mmgsdi_imsi_switch_profile_id_enum_type profile_id)
{
  mmgsdi_return_enum_type    mmgsdi_status  = MMGSDI_SUCCESS;
  uim_cmd_type*              uim_cmd_ptr    = NULL;
  mmgsdi_task_enum_type      mmgsdi_task    = MMGSDI_TASK_MAX;
  uint8                      profile_1_apdu[] = {0x0, 0xDC,0x1, 0x4, 0xD, 0x00, 0x1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  uint8                      profile_2_apdu[] = {0x0, 0xDC,0x1, 0x4, 0xD, 0x00, 0x2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  /* NTN Profile */
  uint8                      profile_3_apdu[] = {0x0, 0xDC,0x1, 0x4, 0xD, 0x00, 0x3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  UIM_MSG_HIGH_1(" mmgsdi_switch_imsi UIM profile_id 0x%x", profile_id);

  if(profile_id < MMGSDI_IMSI_SWITCH_PROFILE_1 || profile_id > MMGSDI_IMSI_SWITCH_PROFILE_3 )
  {
    return MMGSDI_INCORRECT_PARAMS;
  }
  
  mmgsdi_status = mmgsdi_switch_imsi_select();
  /* Send select before sending APDU for IMSI switch*/
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* 1) Get a UIM buffer for the request.
     2) Check for if the buffer is NULL or not. */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Populate UIM information */
  uim_cmd_ptr->hdr.command  = UIM_STREAM_ISO7816_APDU_F;
  uim_cmd_ptr->hdr.slot     = UIM_SLOT_1;
  uim_cmd_ptr->hdr.protocol = UIM_UICC;
  if(profile_id == MMGSDI_IMSI_SWITCH_PROFILE_1)
  {
  uim_cmd_ptr->stream_iso7816_apdu.num_cmd_bytes = sizeof(profile_1_apdu);
  mmgsdi_memscpy(uim_cmd_ptr->stream_iso7816_apdu.cmd_data,
                 sizeof(uim_cmd_ptr->stream_iso7816_apdu.cmd_data),
                 &profile_1_apdu,
                 sizeof(profile_1_apdu));
  }
  else if(profile_id == MMGSDI_IMSI_SWITCH_PROFILE_2)
  {
    uim_cmd_ptr->stream_iso7816_apdu.num_cmd_bytes = sizeof(profile_2_apdu);
    mmgsdi_memscpy(uim_cmd_ptr->stream_iso7816_apdu.cmd_data,
                   sizeof(uim_cmd_ptr->stream_iso7816_apdu.cmd_data),
                   &profile_2_apdu,
                   sizeof(profile_2_apdu));
  }
  else 
  {
    uim_cmd_ptr->stream_iso7816_apdu.num_cmd_bytes = sizeof(profile_3_apdu);
    mmgsdi_memscpy(uim_cmd_ptr->stream_iso7816_apdu.cmd_data,
                   sizeof(uim_cmd_ptr->stream_iso7816_apdu.cmd_data),
                   &profile_3_apdu,
                   sizeof(profile_3_apdu));
  }
  
  /* Assign uim_cmd_ptr's user data */
  UIM_MSG_HIGH_1(" mmgsdi_switch_imsi Sending down SIM profile APDU command to UIM profile_id 0x%x", profile_id);
  
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
     UIM_MSG_ERR_0("send cmd to UIM fail");
  }
  else
  {
    mmgsdi_task = mmgsdi_util_get_current_task();
    if(mmgsdi_task != MMGSDI_TASK_MAX &&
       mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL)
    {
      mmgsdi_status = mmgsdi_uicc_rsp_parse_uim_response(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);
      if(mmgsdi_status == MMGSDI_SUCCESS)
      {
        if(profile_id == MMGSDI_IMSI_SWITCH_PROFILE_3)
        {
          mmgsdi_imsi_switch_crnt_nw_type = MMGSDI_IMSI_SWITCH_NW_NTN;
        }
        else
        {
          mmgsdi_imsi_switch_timer_start();
          mmgsdi_imsi_switch_crnt_nw_type = MMGSDI_IMSI_SWITCH_NW_TN;
        }
        mmgsdi_imsi_switch_crnt_active_profile = profile_id;

        /* Reset the card to reflect the new selected profile */
        mmgsdi_util_preinit_mmgsdi(MMGSDI_SESSION_ID_ZERO,
                                   MMGSDI_APP_NONE,
                                   MMGSDI_REFRESH_RESET,
                                   MMGSDI_SLOT_1,
                                   FALSE);

        mmgsdi_status = mmgsdi_gen_build_card_reset(
                                   mmgsdi_generic_data_ptr->client_id,
                                   MMGSDI_SLOT_1,
                                   MMGSDI_CARD_RESET_REQ,
                                   UIM_UICC,
                                   MMGSDI_RESET_CAUSE_GENERAL,
                                   mmgsdi_util_internal_cb,
                                   0);
        mmgsdi_refresh_update_card_state(MMGSDI_SLOT_1);
      }
    }
    else
    {
      UIM_MSG_ERR_0("Sync process for request has null mmgsdi_switch_imsi");
      mmgsdi_status = MMGSDI_ERROR;
    }
  }

  UIM_MSG_HIGH_4(" mmgsdi_switch_imsi mmgsdi_imsi_switch_crnt_active_profile 0x%x, mmgsdi_imsi_switch_crnt_nw_type 0x%x  profile_id 0x%x, mmgsdi_status 0x%x",
                  mmgsdi_imsi_switch_crnt_active_profile, mmgsdi_imsi_switch_crnt_nw_type,  profile_id, mmgsdi_status);

  return mmgsdi_status;
}/* mmgsdi_switch_imsi */


/*===========================================================================
FUNCTION MMGSDI_SWITCH_IMSI_GET_CURRENT_PROFILE

DESCRIPTION
  This function is called to get the active IMSI profile 

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_switch_imsi_get_current_profile
(mmgsdi_imsi_switch_profile_id_enum_type *profile_id_ptr,
  mmgsdi_imsi_switch_nw_enum_type        *nw_type_ptr
)
{
  mmgsdi_return_enum_type    mmgsdi_status      = MMGSDI_SUCCESS;
  uim_cmd_type*              uim_cmd_ptr        = NULL;
  mmgsdi_task_enum_type      mmgsdi_task        = MMGSDI_TASK_MAX;
  uint8                      get_profile_apdu[] = {0x0, 0xB2,0x1, 0x4, 13};

  if (profile_id_ptr == NULL || nw_type_ptr == NULL)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_switch_imsi_select();
  /* Send select before sending APDU for IMSi switch*/
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* 1) Get a UIM buffer for the request.
     2) Check for if the buffer is NULL or not. */
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(uim_cmd_ptr, sizeof(uim_cmd_type));
  if (uim_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* Populate UIM information */
  uim_cmd_ptr->hdr.command  = UIM_STREAM_ISO7816_APDU_F;
  uim_cmd_ptr->hdr.slot     = UIM_SLOT_1;
  uim_cmd_ptr->hdr.protocol = UIM_UICC;
  
  uim_cmd_ptr->stream_iso7816_apdu.num_cmd_bytes = sizeof(get_profile_apdu);
  mmgsdi_memscpy(uim_cmd_ptr->stream_iso7816_apdu.cmd_data,
                 sizeof(uim_cmd_ptr->stream_iso7816_apdu.cmd_data),
                 &get_profile_apdu,
                 sizeof(get_profile_apdu));

  UIM_MSG_HIGH_0("Sending down SIM profile APDU command to UIM");
  
  mmgsdi_status = mmgsdi_send_cmd_to_uim_server_synch(uim_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
     UIM_MSG_ERR_0("send cmd to UIM fail");
  }
  else
  {
    mmgsdi_task = mmgsdi_util_get_current_task();
    if(mmgsdi_task != MMGSDI_TASK_MAX &&
       mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task] != NULL)
    {
      mmgsdi_status = mmgsdi_uicc_rsp_parse_uim_response(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);
      if(mmgsdi_status ==  MMGSDI_SUCCESS && mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_len > 0)
      {
        *profile_id_ptr = (mmgsdi_imsi_switch_profile_id_enum_type) mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_ptr[2];
        *nw_type_ptr = MMGSDI_IMSI_SWITCH_NW_TN;

        if(*profile_id_ptr ==  MMGSDI_IMSI_SWITCH_PROFILE_3)
        {
          *nw_type_ptr = MMGSDI_IMSI_SWITCH_NW_NTN;
         }
      }
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(
        mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]->rsp_data.data_ptr);
      MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(mmgsdi_internal_synch_uim_rpt_rsp[mmgsdi_task]);
    }
    else
    {
      UIM_MSG_ERR_0("Sync process for request has null mmgsdi_switch_imsi_get_current_profile");
      mmgsdi_status = MMGSDI_ERROR;
    }
    UIM_MSG_HIGH_3("mmgsdi_switch_imsi_get_current_profile mmgsdi_status 0x%x, profile_id_ptr 0x%x, nw_type_ptr 0x%x",
                    mmgsdi_status,  *profile_id_ptr, *nw_type_ptr);
  }

  return mmgsdi_status;
}/* mmgsdi_switch_imsi_get_current_profile */


/*===========================================================================
FUNCTION MMGSDI_PROCESS_IMSI_SWITCH

DESCRIPTION
  This function is called in mmgsdi_process_command.
  It will switch the profile to the given profile id if needed

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_imsi_switch (
  mmgsdi_switch_imsi_profile_req_type *switch_imsi_profile_req_ptr
)
{
  mmgsdi_return_enum_type                 mmgsdi_status  = MMGSDI_SUCCESS;
  mmgsdi_sw_status_type                   status_word    = {90, 00};
  mmgsdi_imsi_switch_profile_id_enum_type profile_id     = MMGSDI_IMSI_SWITCH_PROFILE_UNKNOWN;

  MMGSDIUTIL_RETURN_IF_NULL(switch_imsi_profile_req_ptr);

  do
  {
    if(mmgsdi_imsi_switch_timer_running &&
       switch_imsi_profile_req_ptr->camped_srv == SYS_SRV_STATUS_SRV)
    {
      mmgsdi_imsi_switch_timer_clear();
    }
    else if(switch_imsi_profile_req_ptr->src == MMGSDI_IMSI_SWITCH_SRC_TIMER_EXPIRY && 
            mmgsdi_imsi_switch_srv_status == SYS_SRV_STATUS_SRV)
    {
      mmgsdi_imsi_switch_timer_clear();
      break;
    }
    else if(switch_imsi_profile_req_ptr->network_type == MMGSDI_IMSI_SWITCH_NW_NTN &&
       mmgsdi_imsi_switch_crnt_nw_type != MMGSDI_IMSI_SWITCH_NW_NTN && 
       mmgsdi_imsi_switch_crnt_active_profile != MMGSDI_IMSI_SWITCH_PROFILE_3)
    {
      mmgsdi_imsi_switch_timer_clear();
      mmgsdi_status = mmgsdi_switch_imsi(MMGSDI_IMSI_SWITCH_PROFILE_3);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_1("Not able to switch to profile 3: 0x%x",mmgsdi_status);
        break;
      }
    }
    else if(!mmgsdi_imsi_switch_timer_running &&
            switch_imsi_profile_req_ptr->camped_srv != SYS_SRV_STATUS_SRV &&
            (switch_imsi_profile_req_ptr->network_type != MMGSDI_IMSI_SWITCH_NW_NTN &&
             mmgsdi_imsi_switch_crnt_nw_type != MMGSDI_IMSI_SWITCH_NW_NTN && 
             mmgsdi_imsi_switch_crnt_active_profile != MMGSDI_IMSI_SWITCH_PROFILE_3))
    {
      profile_id = mmgsdi_switch_imsi_get_other_tn_profile(mmgsdi_imsi_switch_crnt_active_profile);
      mmgsdi_status = mmgsdi_switch_imsi(profile_id);
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        UIM_MSG_ERR_2("Not able to switch to profile 0x%x, mmgsdi_status 0x%x",profile_id, mmgsdi_status);
        break;
      }
    }
      mmgsdi_imsi_switch_srv_status = switch_imsi_profile_req_ptr->camped_srv;

  }while(0);

  mmgsdi_status = mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                             &switch_imsi_profile_req_ptr->request_header ,
                                             NULL,
                                             NULL,
                                             FALSE,
                                             status_word);

  UIM_MSG_HIGH_3("mmgsdi_process_imsi_switch crnt_network : 0x%x, crnt_srv: 0x%x, crnt_proflile: 0x%x", 
                   mmgsdi_imsi_switch_crnt_nw_type, mmgsdi_imsi_switch_srv_status, mmgsdi_imsi_switch_crnt_active_profile );

  return mmgsdi_status;
}/* mmgsdi_process_imsi_switch */


/*================================================================
FUNCTION  MMGSDI_SWITCH_IMSI_PROFILE

DESCRIPTION:
  This function is called to to switch the imsi profile .

  INPUT PARAMETERS:
    client_id:                       Client ID of the caller.
    slot:                            Specific slot
    sys_srv_status_e_type:           srv, 
    sys_plmn_id_s_type:              plmn,
    mmgsdi_imsi_switch_nw_enum_type: switch_network,
    response_cb_ptr:                 Pointer to the response callback.
    src:                             Source of API trigger
    client_ref:                      User data returned upon completion of this command.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:

  MMGSDI_SUCCESS          -- Command structure was properly generated and sent
                             to the MMGSDI command queue.\n
  MMGSDI_INCORRECT_PARAMS -- Parameters are not within the appropriate ranges.
  MMGSDI_CMD_QUEUE_FULL   -- Command was not queued to the MMGSDI task because 
                             the maximum number of commands are already queued.

SIDE EFFECTS:
  None
===============================================================*/
mmgsdi_return_enum_type mmgsdi_switch_imsi_profile(
  mmgsdi_client_id_type            client_id,
  mmgsdi_slot_id_enum_type         slot,
  sys_srv_status_e_type            srv, 
  sys_plmn_id_s_type               plmn,
  mmgsdi_imsi_switch_nw_enum_type  switch_network,
  mmgsdi_callback_type             response_cb_ptr,
  mmgsdi_imsi_switch_src_enum_type src, 
  mmgsdi_client_data_type          client_ref
)
{
  mmgsdi_task_cmd_type                 * task_cmd_ptr  = NULL;
  mmgsdi_switch_imsi_profile_req_type  * msg_ptr       = NULL;
  mmgsdi_return_enum_type                mmgsdi_status = MMGSDI_ERROR;
  uint32                                 task_cmd_len  = 0;

  UIM_MSG_HIGH_2("mmgsdi_switch_imsi_profile switch_network : 0x%x, srv: 0x%x", switch_network, srv);

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Response CB function check
    2) Client ID
    3) Validate slot id
    4) Validate input profile id
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Invalid Client ID %0x%x",client_id);
    return MMGSDI_INCORRECT_PARAMS;
  }


  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(switch_network,
                                    MMGSDI_IMSI_SWITCH_NW_UNKNOWN,
                                    MMGSDI_IMSI_SWITCH_NW_NTN);

  if(slot != MMGSDI_SLOT_1)
  {
    UIM_MSG_ERR_1("SWITCH IMSI PROFILE API is not supported on slot:0x%x", slot);
    return MMGSDI_INCORRECT_PARAMS;
  }
  
  /* --------------------------------------------------------------------------
     Allocate memory for the MMGSDI Task Command
     ------------------------------------------------------------------------*/
  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* --------------------------------------------------------------------------
    No need to align pointers of the structures members
    ------------------------------------------------------------------------ */
  msg_ptr = &task_cmd_ptr->cmd.cmd.switch_imsi_profile_req;

  /* --------------------------------------------------------------------------
     Populating mmgsdi_switch_imsi_profile_req_type header.
     ----------------------------------------------------------------------- */
  task_cmd_ptr->cmd.cmd_enum                  = MMGSDI_IMSI_SWITCH_REQ;
  msg_ptr->request_header.client_id           = client_id;
  msg_ptr->request_header.request_type        = MMGSDI_IMSI_SWITCH_REQ;
  msg_ptr->request_header.orig_request_type   = MMGSDI_IMSI_SWITCH_REQ;
  msg_ptr->request_header.slot_id             = slot;
  msg_ptr->request_header.client_data         = client_ref;
  msg_ptr->request_header.response_cb         = response_cb_ptr;

  /* Populate imsi profile that needs to be activated */
  msg_ptr->camped_srv                         = srv;
  msg_ptr->network_type                       = switch_network;
  msg_ptr->plmn                               = plmn;
  msg_ptr->slot                               = slot;
  msg_ptr->src                                = src;
  /* --------------------------------------------------------------------------
     Put on mmgsdi command queue
     ----------------------------------------------------------------------- */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_IMSI_SWITCH_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
} /* mmgsdi_switch_imsi_profile */


/*===========================================================================
  FUNCTION: MMGSDI_IMSI_SWITCH_CM_SS_EVT_CB

  DESCRIPTION:
    Handler for CM_SS_EVENT_SRV_CHANGED. It decides whether it is required to
    SWITCH the IMSI profile 

  PARAMETERS:
    ss_event         -> cm ss event
    mm_ss_info_ptr   -> more information regarding the ss event
===========================================================================*/
void mmgsdi_imsi_switch_cm_ss_evt_cb(
  cm_ss_event_e_type                ss_event,
  const cm_mm_ss_info_s_type      * mm_ss_info_ptr
)
{
  if(ss_event != CM_SS_EVENT_SRV_CHANGED || mm_ss_info_ptr == NULL)
  {
    return;
  }

  UIM_MSG_HIGH_4("CM SS Event, PLMN: 0x%x, Service Status: 0x%x Sys Mode: 0x%x Changed fields: 0x%x",
                 mm_ss_info_ptr->sys_id.id.plmn.identity[0] * 0x10000 +
                 mm_ss_info_ptr->sys_id.id.plmn.identity[1] * 0x100 +
                 mm_ss_info_ptr->sys_id.id.plmn.identity[2],
                 mm_ss_info_ptr->srv_status,
                 mm_ss_info_ptr->sys_mode,
                 mm_ss_info_ptr->changed_fields);

  if(MMGSDI_SS_INFO_CHANGED(mm_ss_info_ptr->changed_fields) &&
     MMGSDIIMSI_SWITCH_SRV_STATUS_CHANGED(mm_ss_info_ptr->srv_status))
  {
    if(((mmgsdi_imsi_switch_timer_running && 
       mm_ss_info_ptr->srv_status == SYS_SRV_STATUS_SRV ) ||
       (!mmgsdi_imsi_switch_timer_running && 
       mm_ss_info_ptr->srv_status != SYS_SRV_STATUS_SRV)) &&
       mmgsdi_generic_data_ptr &&
       mmgsdi_imsi_switch_crnt_nw_type != MMGSDI_IMSI_SWITCH_NW_NTN)
    {
      mmgsdi_switch_imsi_profile(mmgsdi_generic_data_ptr->client_id,
                                 MMGSDI_SLOT_1,
                                 mm_ss_info_ptr->srv_status,
                                 mm_ss_info_ptr->sys_id.id.plmn,
                                 MMGSDI_IMSI_SWITCH_NW_UNKNOWN,
                                 mmgsdi_util_internal_cb,
                                 MMGSDI_IMSI_SWITCH_SRC_CM_SS_EVT,
                                 0);
    }
  }
} /* mmgsdi_imsi_switch_cm_ss_evt_cb */


#ifndef FEATURE_UIM_TEST_FRAMEWORK
/*===========================================================================
  FUNCTION MMGSDI_IMSI_SWITCH_TIMER_CLEAR

  DESCRIPTION
    This function clear the timer

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void mmgsdi_imsi_switch_timer_clear(void)
{

  UIM_MSG_HIGH_0("mmgsdi_imsi_switch_timer_clear");

  (void)rex_clr_timer(&mmgsdi_imsi_switch_timer);

  switch_imsi_cnt = MMGSDI_IMSI_SWITCH_MAX_PING_PONG_CNT;
  mmgsdi_imsi_switch_timer_running = FALSE;
}/*mmgsdi_imsi_switch_timer_clear*/


/*===========================================================================
  FUNCTION MMGSDI_IMSI_SWITCH_TIMER_EXPIRY

  DESCRIPTION
    This function got called when srv timer got expired 

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void mmgsdi_imsi_switch_timer_expiry (uint32 dummy)
{
  mmgsdi_return_enum_type       mmgsdi_status = MMGSDI_ERROR;

  mmgsdi_imsi_switch_timer_running = 0;
  
  (void)rex_clr_timer(&mmgsdi_imsi_switch_timer);
  
  mmgsdi_status = mmgsdi_switch_imsi_profile(mmgsdi_generic_data_ptr->client_id,
                                             MMGSDI_SLOT_1,
                                             mmgsdi_imsi_switch_srv_status,
                                             dummy_plmn_id,
                                             mmgsdi_imsi_switch_crnt_nw_type,
                                             mmgsdi_util_internal_cb,
                                             MMGSDI_IMSI_SWITCH_SRC_TIMER_EXPIRY,
                                             0);
  UIM_MSG_HIGH_1("mmgsdi_imsi_switch_timer_expiry mmgsdi_status 0x%x", mmgsdi_status);
}/* mmgsdi_imsi_switch_timer_expiry */


/*===========================================================================
  FUNCTION MMGSDI_IMSI_SWITCH_TIMER
  
  DESCRIPTION
    This function starts the timer based on the service

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    None

  SIDE EFFECTS
    Builds an Error Response to MMGSDI Client
===========================================================================*/
 void mmgsdi_imsi_switch_timer_start(void)
{
  rex_timer_cnt_type timer            = 0;

  /*Start timer and Wait for Processes to Respond*/
  rex_def_timer_ex(&mmgsdi_imsi_switch_timer,
                   mmgsdi_imsi_switch_timer_expiry,
                   0);

  if(switch_imsi_cnt > 0)
  {
    timer = mmgsdi_imsi_switch_timer_tick;
    switch_imsi_cnt--;
  }
  else
  {
    timer = mmgsdi_imsi_switch_ping_pong_tick;
    switch_imsi_cnt = MMGSDI_IMSI_SWITCH_MAX_PING_PONG_CNT;
    UIM_MSG_HIGH_0("mmgsdi_imsi_switch_timer_start starting ping pong");
  }

  (void)rex_set_timer(&mmgsdi_imsi_switch_timer, timer);
  mmgsdi_imsi_switch_timer_running = TRUE;
  UIM_MSG_HIGH_5("mmgsdi_imsi_switch_timer_start mmgsdi_imsi_switch_timer_tick 0x%x, mmgsdi_imsi_switch_ping_pong_tick 0x%x, mmgsdi_imsi_switch_timer_running 0x%x, timer 0x%x, switch_imsi_cnt 0x%x",
                  mmgsdi_imsi_switch_timer_tick, mmgsdi_imsi_switch_ping_pong_tick,  mmgsdi_imsi_switch_timer_running, timer, switch_imsi_cnt);
}/* mmgsdi_imsi_switch_timer_start */
#else
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

