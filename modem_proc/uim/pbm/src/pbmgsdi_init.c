/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

		  P B M G S D I _ I N I T.C


GENERAL DESCRIPTION
  This file contains the PBM wrapper functions for GSDI

  Copyright (c) 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE
$Header: //components/rel/uim.mpss/3.3.1/pbm/src/pbmgsdi_init.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/13/16    sn     Fix compilation error
09/02/16    dt     Fixed compilation error
06/23/16    nr     ECC optimizations for IoE devices
02/01/16    nr     Initial Revision
===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "pbm_common_types.h"
#include "pbm.h"
#include "string.h"
#include "pbmgsdi.h"
#include "pbmutils.h"
#include "pbmtask_i.h"
#include "pbmuim.h"
#include "stringl/stringl.h"
#include "pbmlib.h"
#include "uim_msg.h"
#include "uim_pbm.h"
#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "pbm_nv_refresh.h"
#endif /*FEATURE_MODEM_CONFIG_REFRESH*/

/*===========================================================================
                         DEFINITIONS
===========================================================================*/

mmgsdi_client_id_type pbm_mmgsdi_client_id = 0;

/* Save session information in PBM for each slot or subscription (1X/GW) */
SESSION_INFO pbm_session_info[PBM_SESSION_MAX];


/*===========================================================================*
 *              PHONE BOOK MANAGER TO GSDI INTERFACE FUNCTIONS.              *
 *              These function are only used internally by PBM               *
 *              and provide the interface to GSDI.                           *
 *===========================================================================*/

/*===========================================================================
FUNCTION PBM_MMGSDI_CARD_STATUS_CB

DESCRIPTION
  Callback function to return SIM card status.

RETURN VALUE
  void.

DEPENDENCIES

===========================================================================*/
static void pbm_mmgsdi_card_status_cb(
  const mmgsdi_event_data_type   *mmgsdi_event
)
{
  pbm_mmgsdi_card_status_s_type *card_status;

  PBM_CHECK_PTR_RET(mmgsdi_event, VOID);

  card_status = (pbm_mmgsdi_card_status_s_type *)
                 pbm_cmd_alloc(sizeof(pbm_mmgsdi_card_status_s_type));
  PBM_CHECK_PTR_RET(card_status, VOID);

  card_status->cmd_type = PBM_MMGSDI_CARD_STATUS_CB;

  card_status->mmgsdi_event = *mmgsdi_event;

  if (mmgsdi_event->evt == MMGSDI_REFRESH_EVT)
  {
    //Init this in case number of files is zero
    card_status->mmgsdi_event.data.refresh.refresh_files.file_list_ptr = NULL;

    if ((mmgsdi_event->data.refresh.refresh_files.file_path_len > 0) &&
        (mmgsdi_event->data.refresh.refresh_files.file_path_ptr != NULL))
    {
      uint32 list_size = mmgsdi_event->data.refresh.refresh_files.file_path_len;

      //Need to copy FCN refresh path list seperately.
      //Need to copy only paths, since we always get only paths
      PBM_MEM_ALLOC(card_status->mmgsdi_event.data.refresh.refresh_files.file_path_ptr,
                    list_size);
      PBM_CHECK_PTR_RET(card_status->mmgsdi_event.data.refresh.refresh_files.file_path_ptr, VOID);
      memscpy(card_status->mmgsdi_event.data.refresh.refresh_files.file_path_ptr, list_size,
              mmgsdi_event->data.refresh.refresh_files.file_path_ptr,list_size);
    }
  }
  if (mmgsdi_event->evt == MMGSDI_FDN_EVT)
  {
    if(mmgsdi_event->data.fdn.rec_num!= 0)
    {
      card_status->mmgsdi_event.data.fdn.rec_data.data_ptr = NULL;

       if (mmgsdi_event->data.fdn.rec_data.data_len> 0)
       {
         uint32 rec_len = mmgsdi_event->data.fdn.rec_data.data_len;
         PBM_MEM_ALLOC(card_status->mmgsdi_event.data.fdn.rec_data.data_ptr,rec_len);
         PBM_CHECK_PTR_RET(card_status->mmgsdi_event.data.fdn.rec_data.data_ptr, VOID);
         memscpy(card_status->mmgsdi_event.data.fdn.rec_data.data_ptr, rec_len,
                 mmgsdi_event->data.fdn.rec_data.data_ptr,rec_len);
       }
     }
   }
   if (mmgsdi_event->evt == MMGSDI_ECC_EVT)
   {
     if(mmgsdi_event->data.ecc.ecc_count != 0)
     {
       uint16 i,j;
       mmgsdi_ecc_evt_info_type *temp_src_ecc_evt_info = (mmgsdi_ecc_evt_info_type *)&mmgsdi_event->data.ecc;
       card_status->mmgsdi_event.data.ecc.ecc_data = NULL;

       PBM_MEM_ALLOC(card_status->mmgsdi_event.data.ecc.ecc_data,
                       sizeof(mmgsdi_ecc_data_type) *
					             mmgsdi_event->data.ecc.ecc_count);
       PBM_CHECK_PTR_RET(card_status->mmgsdi_event.data.ecc.ecc_data,VOID);
       for(i = 0; i<temp_src_ecc_evt_info->ecc_count; i++)
       {
         PBM_MEM_ALLOC(card_status->mmgsdi_event.data.ecc.ecc_data[i].data.data_ptr,
                         temp_src_ecc_evt_info->ecc_data[i].data.data_len);
         if(card_status->mmgsdi_event.data.ecc.ecc_data[i].data.data_ptr == NULL)
         {
           for(j = 0; j <= i; j++)
           {
             PBM_MEM_FREEIF(card_status->mmgsdi_event.data.ecc.ecc_data[j].data.data_ptr);
           }
           PBM_MEM_FREEIF(card_status->mmgsdi_event.data.ecc.ecc_data);
           return;
         }
         card_status->mmgsdi_event.data.ecc.ecc_data[i].data.data_len =
                             temp_src_ecc_evt_info->ecc_data[i].data.data_len;
         memscpy(card_status->mmgsdi_event.data.ecc.ecc_data[i].data.data_ptr,
		 	     temp_src_ecc_evt_info->ecc_data[i].data.data_len,
                 temp_src_ecc_evt_info->ecc_data[i].data.data_ptr,
                 temp_src_ecc_evt_info->ecc_data[i].data.data_len);
       }
     }
   }
   pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(card_status));
}/* pbm_mmgsdi_card_status_cb */


/*===========================================================================
FUNCTION PBM_MMGSDI_REG_STATUS_CB

DESCRIPTION
  Callback function from MMGSDI to log errors

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static void pbm_mmgsdi_reg_status_cb (
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    data_type,
  const mmgsdi_cnf_type  *data_ptr
)
{
  pbm_mmgsdi_resp_s_type *mmgsdi_resp_ptr;

  PBM_CHECK_PTR_RET(data_ptr, VOID);

  mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *)
                     pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));
  PBM_CHECK_PTR_RET(mmgsdi_resp_ptr, VOID);

  mmgsdi_resp_ptr->cmd_type = PBM_MMGSDI_REG_STATUS_CB;

  mmgsdi_resp_ptr->data_type = data_type;
  mmgsdi_resp_ptr->status = status;

  memscpy(&mmgsdi_resp_ptr->data, sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));

  //unblock PBM task
  (void) rex_set_sigs(PBM_TCB, PBM_MMGSDI_WAIT_SIG);

  pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));
}/* pbm_mmgsdi_reg_status_cb */


/**===========================================================================
FUNCTION PBM_MMGSDI_INIT

DESCRIPTION
  Initialize MMGSDI

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================**/
pbm_return_type pbm_mmgsdi_init(
  void
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  UIM_MSG_HIGH_0("Begin PBM MMGSDI init");

  /* Initialize pbm_session_info structure */
  memset(&pbm_session_info[0], 0, sizeof(pbm_session_info));

  (void) rex_clr_sigs(PBM_TCB, PBM_MMGSDI_WAIT_SIG);

  mmgsdi_status = mmgsdi_client_id_and_evt_reg_ext (pbm_mmgsdi_card_status_cb,
                                                    pbm_mmgsdi_reg_status_cb,
                                                    0,
                                                    PBM_CLIENT_NAME,
                                                    sizeof(PBM_CLIENT_NAME));
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("mmgsdi_client_id_and_evt_reg failed %d", mmgsdi_status);
    return PBM_ERROR;
  }

  //block waiting for callback
  (void) pbm_wait (PBM_MMGSDI_WAIT_SIG);
  (void) rex_clr_sigs(PBM_TCB, PBM_MMGSDI_WAIT_SIG);

  return PBM_SUCCESS;
}/* pbm_mmgsdi_init */


/*===========================================================================
FUNCTION PBM_OPEN_SESSIONS

DESCRIPTION
  Function to init SIM variables, and do processing that can be done
  without access restrictions (eg. ECC).

RETURN VALUE
  void.

DEPENDENCIES

===========================================================================*/
void pbm_open_sessions(
  void
)
{
  mmgsdi_return_enum_type result;
  uint32                  session_type_mask = (MMGSDI_GW_PROV_PRI_SESSION_MASK | \
                                               MMGSDI_1X_PROV_PRI_SESSION_MASK | \
                                               MMGSDI_CARD_SESSION_SLOT_1_MASK );
#ifdef FEATURE_DUAL_SIM
  session_type_mask |= (MMGSDI_GW_PROV_SEC_SESSION_MASK | \
                        MMGSDI_1X_PROV_SEC_SESSION_MASK | \
                        MMGSDI_CARD_SESSION_SLOT_2_MASK );
#endif /* FEATURE_DUAL_SIM */
#ifdef FEATURE_TRIPLE_SIM
  session_type_mask |= (MMGSDI_GW_PROV_TER_SESSION_MASK | \
                        MMGSDI_1X_PROV_TER_SESSION_MASK | \
                        MMGSDI_CARD_SESSION_SLOT_3_MASK);
#endif /* FEATURE_TRIPLE_SIM */

  result = mmgsdi_session_open_ext(pbm_mmgsdi_client_id,
                                   session_type_mask,
                                   pbm_mmgsdi_card_status_cb,
                                   FALSE,
                                   pbm_mmgsdi_open_session_cb,
                                   (mmgsdi_client_data_type)session_type_mask);

  if ( MMGSDI_SUCCESS != result )
  {
    /* log that error and return */
    UIM_MSG_ERR_1("PBM session open failed with error %d",result);
  }
}/* pbm_open_sessions */


/*===========================================================================
FUNCTION pbm_open_session_update_session_info

DESCRIPTION
  PBM internal function to update the session related information.
  protected under critical section.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static void pbm_open_session_update_session_info(
  pbm_session_enum_type        pbm_session,
  mmgsdi_session_id_type       session_id,
  mmgsdi_app_enum_type         app_type,
  pbm_slot_type                pbm_slot_id
)
{
  pbm_session_info[pbm_session].session_id = session_id;

  rex_enter_crit_sect(&pbm_crit_sect); /* for pbm_session_info slot id and app type */
  pbm_session_info[pbm_session].slot_id = (uint16) pbm_slot_id;
  pbm_session_info[pbm_session].app_type = app_type;
  rex_leave_crit_sect(&pbm_crit_sect);/* for pbm_session_info slot id and app type*/
}/* pbm_open_session_update_session_info */


/*===========================================================================
FUNCTION pbm_mmgsdi_open_session_cb

DESCRIPTION
  Callback function from MMGSDI in response to OPEN SESSION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_mmgsdi_open_session_cb (
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     data_type,
  const mmgsdi_cnf_type   *data_ptr
)
{
  pbm_mmgsdi_resp_s_type           *mmgsdi_resp_ptr = NULL;
  uint32                            len = 0;
  boolean                           sent_success = FALSE;
  mmgsdi_session_open_info_type    *session_open_info = NULL; 

  if ( data_ptr && data_ptr->session_open_ext_cnf.session_info_ptr )
  {
    mmgsdi_resp_ptr = (pbm_mmgsdi_resp_s_type *) pbm_cmd_alloc(sizeof(pbm_mmgsdi_resp_s_type));

    if ( mmgsdi_resp_ptr )
    {
      len = sizeof(mmgsdi_session_open_info_type)* MIN(data_ptr->session_open_ext_cnf.num_sessions,PBM_SESSION_MAX );

      PBM_MEM_ALLOC(session_open_info, len);
      PBM_CHECK_PTR_RET(session_open_info, VOID);

      if ( session_open_info )
      {
        mmgsdi_resp_ptr->cmd_type  = PBM_MMGSDI_OPEN_SESSION_CB;
        mmgsdi_resp_ptr->status    = status;
        mmgsdi_resp_ptr->data_type = data_type;

        memscpy(&mmgsdi_resp_ptr->data,sizeof(mmgsdi_cnf_type), data_ptr, sizeof(mmgsdi_cnf_type));

        if ( data_ptr->session_open_ext_cnf.num_sessions > PBM_SESSION_MAX )
        {
          UIM_MSG_ERR_0("Error :Number of sessions are greater than PBM_SESSION_MAX ");
        }

        memscpy(session_open_info,len, data_ptr->session_open_ext_cnf.session_info_ptr, len);
        mmgsdi_resp_ptr->data.session_open_ext_cnf.session_info_ptr = session_open_info;

        pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(mmgsdi_resp_ptr));

        sent_success = TRUE;
      }
      else
      {
        UIM_MSG_ERR_0( "Unable to allocate session_info_ptr");
        PBM_MEM_FREEIF(mmgsdi_resp_ptr);
      }
    }
    else
    {
      UIM_MSG_ERR_0( "Unable to allocate mmgsdi_resp_ptr");
    }
  }
  else
  {
    UIM_MSG_ERR_0( "NULL ptr");
  }

  if ( !sent_success )
  {
    (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
  }
}/* pbm_mmgsdi_open_session_cb */


/*===========================================================================
FUNCTION pbm_open_session_cb

DESCRIPTION
  PBM internal function in response to OPEN SESSION callback from MMGSDI

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_open_session_cb (
  mmgsdi_return_enum_type   status,
  mmgsdi_cnf_enum_type      data_type,
  mmgsdi_cnf_type          *data_ptr)
{
  pbm_slot_type                    slot_id;
  mmgsdi_session_id_type           session_id;
  mmgsdi_app_enum_type             app_type;
  mmgsdi_session_type_enum_type    mmgsdi_session_type;
  uint16                           i = 0;

  /* data_ptr is NULL for ICC card where open_session returns success */
  PBM_CHECK_PTR_RET(data_ptr, VOID);

  if ( status == MMGSDI_SUCCESS )
  {
    if (data_type == MMGSDI_SESSION_OPEN_EXT_CNF)
    {
      for (i = 0; i < data_ptr->session_open_ext_cnf.num_sessions; i++)
      {
        slot_id = PBM_MMGSDI_SLOT_TO_PBM_SLOT
                    (data_ptr->session_open_ext_cnf.session_info_ptr[i].slot_id);
        session_id = data_ptr->session_open_ext_cnf.session_info_ptr[i].session_id;;
        app_type = data_ptr->session_open_ext_cnf.session_info_ptr[i].app_info.app_type;
        mmgsdi_session_type = data_ptr->session_open_ext_cnf.session_info_ptr[i].session_type;
        switch (mmgsdi_session_type)
        {
          case MMGSDI_CARD_SESSION_SLOT_1:
            //app_type would be none for card session
            pbm_open_session_update_session_info(PBM_SESSION_GPB_1, session_id, MMGSDI_APP_NONE, slot_id);
            break;

          case MMGSDI_GW_PROV_PRI_SESSION:
             pbm_open_session_update_session_info(PBM_SESSION_LPB_GW, session_id, app_type, slot_id);
             break;

          case MMGSDI_1X_PROV_PRI_SESSION:
             pbm_open_session_update_session_info(PBM_SESSION_LPB_1X, session_id, app_type, slot_id);
             break;

#ifdef FEATURE_DUAL_SIM
          case MMGSDI_CARD_SESSION_SLOT_2:
             //app_type would be none for card session
             pbm_open_session_update_session_info(PBM_SESSION_GPB_2, session_id, MMGSDI_APP_NONE, slot_id);
             break;

           case MMGSDI_GW_PROV_SEC_SESSION:
             pbm_open_session_update_session_info(PBM_SESSION_LPB_GW_SECONDARY, session_id, app_type, slot_id);
             break;

           case MMGSDI_1X_PROV_SEC_SESSION:
             pbm_open_session_update_session_info(PBM_SESSION_LPB_1X_SECONDARY, session_id, app_type, slot_id);
             break;
#endif /* FEATURE_DUAL_SIM */
#ifdef FEATURE_TRIPLE_SIM
           case MMGSDI_CARD_SESSION_SLOT_3:
             //app_type would be none for card session
             pbm_open_session_update_session_info(PBM_SESSION_GPB_3, session_id, MMGSDI_APP_NONE, slot_id);
             break;

           case MMGSDI_GW_PROV_TER_SESSION:
             pbm_open_session_update_session_info(PBM_SESSION_LPB_GW_TERTIARY, session_id, app_type, slot_id);
             break;

           case MMGSDI_1X_PROV_TER_SESSION:
             pbm_open_session_update_session_info(PBM_SESSION_LPB_1X_TERTIARY, session_id, app_type, slot_id);
             break;
#endif /* FEATURE_TRIPLE_SIM */
           default:
             UIM_MSG_ERR_0("MMGSDI returned invalid session info");
             break;
         }
       }
       PBM_MEM_FREEIF(data_ptr->session_open_ext_cnf.session_info_ptr);
     }
  }
}/* pbm_open_session_cb */


/*===========================================================================
FUNCTION PBM_INIT

DESCRIPTION
  This Stub function is called during PBM task startup after the task related
  initialization is done.

  - Initialize Caches
  - Register functions with MMGSDI
  - Initialize Emergency Call Codes
  - MCFG Registration 

  Note that UIM based phonebook is read after SIM initialization is complete.

DEPENDENCIES
  pbm_task_init has been called before.

SIDE EFFECTS
  None
===========================================================================*/
pbm_return_type pbm_init(
  void
)
{
  pbm_return_type        ret_val      = PBM_ERROR;

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  /* MCFG Registration */
  pbm_nv_mcfg_refresh_register();
#endif /*FEATURE_MODEM_CONFIG_REFRESH*/

  /* initializing atomic variables */
  atomic_init(&pbm_pin1_verified, (atomic_plain_word_t) 0 );
  atomic_init(&pbm_pin2_verified, (atomic_plain_word_t) 0);

  /* Initialize caches to zero */
  ret_val = pbm_cache_init();
  PBM_CHECK_ERR_RET(ret_val, "Cache init failed");

  /* Initialize GSDI
   * UIM is initialized based on events we receive from GSDI */
  ret_val = pbm_mmgsdi_init();
  PBM_CHECK_ERR(ret_val, "GSDI init failed");

  /* Initialize Emergency Call Codes */
  ret_val = pbm_ecc_init();
  PBM_CHECK_ERR(ret_val, "ECC init failed");

  /* Initialize CM client */
  ret_val = pbm_cm_init();
  PBM_CHECK_ERR(ret_val, "PBM CM init failed");

  /* Get the pbm features' status via NV look up */
  pbm_nv_init_features_status_nv_lookup();

  return PBM_SUCCESS;
} /* pbm_init */

