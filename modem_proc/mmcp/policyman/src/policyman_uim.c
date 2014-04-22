/**
  @file policyman_uim.c

  @brief
*/

/*
    Copyright (c) 2013-2015, 2018 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/policyman_uim.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/
#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES

#include "policyman_cfgitem.h"
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_plmn.h"
#include "policyman_rules.h"
#include "policyman_set.h"
#include "policyman_subs.h"
#include "policyman_task.h"
#include "policyman_util.h"
#include "policyman_uim.h"
#include "policyman_xml.h"
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"
#include "simlock_modem_lib.h"
#include "policyman_svc_mode.h"
#include <stringl/stringl.h>

#if defined(VOID) && defined(XX_WIN32)
#undef VOID
#endif
#define VOID

#define SIM_TYPE_TAG  "sim_type"
#define PM_MMGSDI_NUM_REGISTERED_FILES 2
#define IMSI_M_DATA_LEN (MMGSDI_IMSI_LEN + 1)


typedef struct
{
  mmgsdi_session_type_enum_type session_type;  /* the key */ 
  sys_modem_as_id_e_type        asubs_id;      /* we assign this */
  mmgsdi_session_id_type        session_id;    /* comes from mmgsdi */
  mmgsdi_app_enum_type          app_type;      /* comes from mmgsdi */
  boolean                       in_use;        /* we manage. true if session open */ 
  boolean                       active;        /* we manage. true if session active */ 
} pm_mmgsdi_session_id_table_entry_s_type;

static boolean is_mmgsdi_subscription_ready_received = FALSE;

static pm_mmgsdi_session_id_table_entry_s_type pm_mmgsdi_session_id_table[] = {
  {MMGSDI_GW_PROV_PRI_SESSION, SYS_MODEM_AS_ID_1, 0, MMGSDI_APP_NONE, FALSE, FALSE},
  {MMGSDI_1X_PROV_PRI_SESSION, SYS_MODEM_AS_ID_1, 0, MMGSDI_APP_NONE, FALSE, FALSE},
  {MMGSDI_GW_PROV_SEC_SESSION, SYS_MODEM_AS_ID_2, 0, MMGSDI_APP_NONE, FALSE, FALSE},
  {MMGSDI_1X_PROV_SEC_SESSION, SYS_MODEM_AS_ID_2, 0, MMGSDI_APP_NONE, FALSE, FALSE},
};

/*=============================================================================
  Function Prototype
=============================================================================*/
static void pm_mmgsdi_card_status_cb(
  const mmgsdi_event_data_type   *mmgsdi_event );

static void pm_mmgsdi_reg_status_cb( 
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    data_type,
  const mmgsdi_cnf_type  *data_ptr
);

static void pm_open_sessions(
  void
);

static policyman_status_t pm_mmgsdi_read
(
    mmgsdi_session_type_enum_type     session_type,
      /* session whose files to read */

    mmgsdi_file_enum_type             file_name,
        /* Name of the file to read */

    mmgsdi_rec_num_type               record_number,
        /* Record number to begin with, in the file */

    mmgsdi_len_type                   request_length,
        /* Total bytes to be read */

    mmgsdi_callback_type              response_cb_ptr,
      /* Callback function to be called */

    mmgsdi_client_data_type client_ref
);

boolean sys_get_num_of_mnc_digits(
  mmgsdi_data_type * imsi_ptr,
  mmgsdi_data_type * ad_ptr,
  uint8            * num_mnc_digits_ptr
);

typedef void (*policyman_mmgsdi_callback_type) (
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

/*=============================================================================
  Data structures
=============================================================================*/
typedef struct
{
  mmgsdi_file_enum_type                          file_name;
  pm_mmgsdi_session_id_table_entry_s_type const *session_info;
  size_t                                         num_mnc_digits;
  uint8                                          imsi[MMGSDI_IMSI_LEN];
  uint8                                          imsi_m[IMSI_M_DATA_LEN];
} policy_mmgsdi_ctx_t;

/*=============================================================================
  APIs for UIM information
=============================================================================*/


/*-------- policyman_uim_init --------*/
boolean
policyman_uim_init(policyman_uim_info_t  **pUimInfo )
{
  *pUimInfo = (policyman_uim_info_t *) policyman_mem_alloc(sizeof(policyman_uim_info_t));
  return (*pUimInfo != NULL);
}

/*-------- policyman_uim_deinit --------*/
void
policyman_uim_deinit(
  policyman_uim_info_t  *pUimInfo
  )
{
  POLICYMAN_MEM_FREE_IF(pUimInfo);
}

/*-------- policyman_register_for_uim_events --------*/
void policyman_register_for_uim_events(
  void
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  is_mmgsdi_subscription_ready_received = FALSE;

  mmgsdi_status = mmgsdi_client_id_and_evt_reg( pm_mmgsdi_card_status_cb,
                                                pm_mmgsdi_reg_status_cb,
                                                0 );
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    POLICYMAN_MSG_ERROR("mmgsdi_client_id_and_evt_reg failed %d", mmgsdi_status, 0, 0);
  }
}

/*-------- policyman_uim_get_uim_info --------*/
static policyman_uim_info_t* policyman_uim_get_uim_info(
  sys_modem_as_id_e_type subsId
  )
{
  if(subsId < 0 || subsId >= POLICYMAN_NUM_SUBS)
  {
    POLICYMAN_ERR_FATAL("subsId out of range", 0, 0, 0);
  }
 
  return policyman_state_get_state()->pSubsState[subsId]->pUimInfo;
}

/*=============================================================================
  Event callback definitions
=============================================================================*/

typedef struct
{
  POLICYMAN_CMD_HDR;

  mmgsdi_event_data_type   mmgsdi_event;
}policyman_uim_evt_cb_s;

typedef struct
{
   POLICYMAN_CMD_HDR;

   mmgsdi_return_enum_type status;
   mmgsdi_cnf_enum_type    data_type;
   mmgsdi_cnf_type         data;
} pm_mmgsdi_resp_s_type;

typedef struct
{
  POLICYMAN_CMD_HDR;

  policy_mmgsdi_ctx_t   ctx;
}policyman_ctx_cmd_t;


static mmgsdi_client_id_type pm_mmgsdi_client_id = 0;

/*===========================================================================
  FUNCTION PM_PROCESS_MMGSDI_REG_STATUS()

  DESCRIPTION
    MMGSDI process function
 ===========================================================================*/
void pm_process_mmgsdi_reg_status(
  policyman_cmd_t *pCmd
  )
{
  pm_mmgsdi_resp_s_type *pMmgsdi_resp = (pm_mmgsdi_resp_s_type*)pCmd;

  if (pMmgsdi_resp->status == MMGSDI_SUCCESS)
  {
    if (pMmgsdi_resp->data_type== MMGSDI_CLIENT_ID_AND_EVT_REG_CNF)
    {
      pm_mmgsdi_client_id = pMmgsdi_resp->data.client_id_and_evt_reg_cnf.response_header.client_id;

      //open sessions irrespective of knowing the SIM inserted or not
      pm_open_sessions();
    }
  }
  else
  {
    switch (pMmgsdi_resp->data_type)
    {
      case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF:
        POLICYMAN_MSG_ERROR("MMGSDI failed registering PM client ID & EVT registration.  Status %d, Confirmation %d",
        pMmgsdi_resp->status, pMmgsdi_resp->data_type, 0);
        break;

      default:
        POLICYMAN_MSG_ERROR("MMGSDI returned Failure async. Status %d, Confirmation %d",
        pMmgsdi_resp->status, pMmgsdi_resp->data_type, 0);
        break;
    }
  }
}

/*===========================================================================
  FUNCTION pm_mmgsdi_ssn_tbl_get_entry_by_type()

  DESCRIPTION
 ===========================================================================*/
static pm_mmgsdi_session_id_table_entry_s_type* pm_mmgsdi_ssn_tbl_get_entry_by_type(
  mmgsdi_session_type_enum_type session_type
  )
{
  int index;
  pm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;


  for(index = 0; index < ARR_SIZE(pm_mmgsdi_session_id_table); index++)
  {
    if( pm_mmgsdi_session_id_table[index].session_type == session_type &&
        pm_mmgsdi_session_id_table[index].in_use == TRUE
      )
    {
      ssn_entry = &pm_mmgsdi_session_id_table[index];
      break;
    }
  }
  return ssn_entry;
}

/*===========================================================================
  FUNCTION pm_mmgsdi_ssn_tbl_get_entry_by_id()

  DESCRIPTION
 ===========================================================================*/
static pm_mmgsdi_session_id_table_entry_s_type* pm_mmgsdi_ssn_tbl_get_entry_by_id(
  mmgsdi_session_id_type        session_id
  )
{
  int index;
  pm_mmgsdi_session_id_table_entry_s_type* ssn_entry = NULL;


  for(index = 0; index < ARR_SIZE(pm_mmgsdi_session_id_table); index++)
  {
    if( (pm_mmgsdi_session_id_table[index].session_id == session_id) &&
        (pm_mmgsdi_session_id_table[index].in_use == TRUE)
      )
    {
      ssn_entry = &pm_mmgsdi_session_id_table[index];
      break;
    }
  }
  if(ssn_entry == NULL)
  {
    POLICYMAN_MSG_ERROR("invalid session-id 0x%x", session_id, 0, 0);
  }
  return ssn_entry;
}

/*===========================================================================
  FUNCTION print_session_info()
 ===========================================================================*/
static void print_session_info(
  pm_mmgsdi_session_id_table_entry_s_type *ssn_entry
  )
{
  POLICYMAN_MSG_MED_4( "session identified Type:%d App: %d active:%d inuse: %d",
                       ssn_entry->session_type,
                       ssn_entry->app_type,
                       ssn_entry->active,
                       ssn_entry->in_use );
}

/*===========================================================================
  Data Structure for READ CNF from MMGSDI
 ===========================================================================*/

#define PM_MMGSDI_RPM_AD_SIZE           4 /* EF-AD */

typedef struct
{
   POLICYMAN_CMD_HDR;
   mmgsdi_cnf_type  pm_mmgsdi_cnf;
} pm_mmgsdi_read_cnf_s_type;

/*===========================================================================
  FUNCTION pm_dtor_mmgsdi_read_cnf()

  DESCRIPTION
 ===========================================================================*/
void pm_dtor_mmgsdi_read_cnf
(
  policyman_cmd_t *pCmd
)
{
   /* Free the memory for the array of items. */
   POLICYMAN_MSG_MED("pm_dtor_mmgsdi_read_cnf called for mmgsdi cmd", 0, 0, 0);
   POLICYMAN_MEM_FREE_IF(((pm_mmgsdi_read_cnf_s_type*)pCmd)->pm_mmgsdi_cnf.read_cnf.read_data.data_ptr);
}

/*-------- sim_session_is_gw --------*/
STATIC boolean sim_session_is_gw(
  mmgsdi_session_type_enum_type session
)
{
  return    session == MMGSDI_GW_PROV_PRI_SESSION
         || session == MMGSDI_GW_PROV_SEC_SESSION;
}

/*-------- sim_session_is_1x --------*/
STATIC boolean sim_session_is_1x(
  mmgsdi_session_type_enum_type session
)
{
  return session == MMGSDI_1X_PROV_PRI_SESSION;
}

/*-------- policyman_get_imsi_file_id --------*/
STATIC mmgsdi_file_enum_type policyman_get_imsi_file_id(
  mmgsdi_app_enum_type app_type
)
{
  mmgsdi_file_enum_type fileId = MMGSDI_GSM_IMSI;

  switch (app_type)
  {
    case MMGSDI_APP_USIM:
      fileId = MMGSDI_USIM_IMSI;
      break;

    case MMGSDI_APP_CSIM:
      fileId = MMGSDI_CSIM_IMSI_M;
      break;

    case MMGSDI_APP_RUIM:
      fileId = MMGSDI_CDMA_IMSI_M;
      break;

    default:
      break;
  }

  return fileId;
}

/*-------- policyman_get_ad_file_id --------*/
STATIC mmgsdi_file_enum_type policyman_get_ad_file_id(
  mmgsdi_app_enum_type app_type
)
{
  mmgsdi_file_enum_type fileId = MMGSDI_GSM_AD;

  switch (app_type)
  {
    case MMGSDI_APP_USIM:
      fileId = MMGSDI_USIM_AD;
      break;

    default:
      break;
  }

  return fileId;
}


/*-------- policyman_uim_simlock_verified --------*/
STATIC boolean policyman_uim_simlock_verified(
  simlock_imsi_type   imsi
)
{
  simlock_result_enum_type simlock_verify_status;

  simlock_verify_status = simlock_verify_imsi(imsi);
  POLICYMAN_MSG_HIGH_1("simlock_verified status: %d", simlock_verify_status);

  return (   simlock_verify_status == SIMLOCK_SUCCESS
          || simlock_verify_status == SIMLOCK_UNSUPPORTED );

}

STATIC void policyman_uim_handle_imsi_update(
  uint8                  *pImsi,
  uint8                   imsiLen,
  sys_modem_as_id_e_type  asubs_id
)
{
  simlock_imsi_type            imsi;
  boolean                      simlock_verified;
  policyman_svc_mode_item_t   *pSvcItem;
  policyman_set_t             *pItemSet = policyman_itemset_new();

  /* check if this IMSI is successfully SIM Lock verified by simlock engine */
  imsi.imsi_len = memscpy(imsi.imsi, sizeof(imsi.imsi) , pImsi, imsiLen);

  simlock_verified = policyman_uim_simlock_verified(imsi);
  policyman_state_update_simlock_verified(asubs_id, simlock_verified);

  simlock_verified = policyman_state_get_simlock_verified(asubs_id);
    
  if (simlock_verified)
  {
    pSvcItem = policyman_svc_mode_item_new(POLICYMAN_SVC_MODE_FULL, asubs_id);
  }
  else /* If SIM Lock not verified, restrict the subscription to limited mode */
  {
    pSvcItem = policyman_svc_mode_item_new(POLICYMAN_SVC_MODE_LIMITED, asubs_id);  
  }

  policyman_set_add(pItemSet, &pSvcItem);
  ref_cnt_obj_release(pSvcItem);

  policyman_policy_run_policy_check(policyman_state_get_state(), asubs_id, pItemSet);
  policyman_cfgitem_update_items(pItemSet);
  ref_cnt_obj_release(pItemSet);
  
}


/*===========================================================================
 FUNCTION policyman_uim_set_imsi_plmn()

 DESCRIPTION
   Set IMSI PLMN into UIM state
===========================================================================*/
STATIC boolean policyman_uim_set_imsi_plmn(
  size_t              subsId,
  sys_plmn_id_s_type *pPlmn
)
{
  boolean               changed  = FALSE;
  policyman_uim_info_t *pUimInfo = policyman_uim_get_uim_info(subsId);

  if (!policyman_plmns_are_equal(&pUimInfo->plmn, pPlmn))
  {
    POLICYMAN_MSG_HIGH_1("updating IMSI PLMN for Sub %d", subsId);
    pUimInfo->plmn = *pPlmn;
    changed = TRUE;
  }

  return changed;
}

/*-------- policyman_extract_plmn_from_imsi --------*/
STATIC void policyman_extract_plmn_from_imsi(
  policy_mmgsdi_ctx_t      *pCtx,
  policyman_uim_info_t     *pUimInfo
)
{
   pUimInfo->plmn.identity[0] = (byte)(( ( pCtx->imsi[1] & 0xF0 ) / 0x10 ) +
                                         ( ( pCtx->imsi[2] & 0x0F ) * 0x10 ));

   /*
   ** Fill MCC Digit 3 and MNC Digit 3
    */
   pUimInfo->plmn.identity[1] = (byte)(( ( pCtx->imsi[2] & 0xF0 ) / 0x10 ) +
                                       ( ( pCtx->imsi[4] & 0x0F ) * 0x10 ));

   /*
   ** If 2 digits MNC is indicated in EF-AD then set MNC digit 3 to 'F'.
   */
   if(pCtx->num_mnc_digits == 2)
   {
    POLICYMAN_MSG_HIGH_0("2 digit MNC");  
    pUimInfo->plmn.identity[1] |= 0xF0;
   }
 
   /*
   ** Fill MNC Digit 1 and MNC Digit 2
   */
   pUimInfo->plmn.identity[2] = pCtx->imsi[3];
   POLICYMAN_MSG_HIGH_0("IMSI PLMN:");
   policyman_plmn_print(&pUimInfo->plmn);
}

 /*-------- policyman_process_imsi --------*/
 STATIC void policyman_process_imsi(
   policyman_cmd_t *pCmd
)
    {
   policyman_uim_info_t  subs_info;
   policy_mmgsdi_ctx_t  *pCtx       = &((policyman_ctx_cmd_t *)pCmd)->ctx;
   sys_modem_as_id_e_type       asubs_id  = pCtx->session_info->asubs_id;

   /* Extract imsi plmn from imsi bytes */
   policyman_extract_plmn_from_imsi(pCtx, &subs_info);
 
   /* Set SIM refresh only after first time IMSI is set
    */
   if (policyman_state_check_precondition_met(POLICYMAN_PRECOND_IMSI, asubs_id))
   {
     policyman_uim_set_sim_refresh(asubs_id);
   }
 
   policyman_state_set_precondition_met(POLICYMAN_PRECOND_IMSI, asubs_id);
   (void)policyman_uim_set_imsi_plmn(asubs_id, &subs_info.plmn);
   policyman_uim_handle_imsi_update(pCtx->imsi, sizeof(pCtx->imsi), asubs_id);

 }

/*-------- policyman_process_ad_cb --------*/
STATIC void policyman_process_ad_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  policy_mmgsdi_ctx_t     *pCtx = NULL;
  mmgsdi_data_type const  *pAd  = NULL;
  boolean                  readStatus;

 if (cnf_ptr == NULL || status != MMGSDI_SUCCESS || cnf != MMGSDI_READ_CNF)
 {
   POLICYMAN_MSG_ERROR_3("EF-AD read failure: cnf_ptr = 0x%x, status = %d, cnf = %d ", cnf_ptr, status, cnf);
   goto Done;
 }
 
 pCtx = (policy_mmgsdi_ctx_t *)cnf_ptr->response_header.client_data;
 pAd  = (mmgsdi_data_type *)&cnf_ptr->read_cnf.read_data;

  // determine MNC digits
  {
    uint8             num_mnc_digits;
    mmgsdi_data_type  imsi;

    imsi.data_ptr = (uint8 *)policyman_mem_alloc(MMGSDI_IMSI_LEN);
    imsi.data_len = MMGSDI_IMSI_LEN;

    memscpy(imsi.data_ptr, MMGSDI_IMSI_LEN, pCtx->imsi, MMGSDI_IMSI_LEN);
    readStatus = sys_get_num_of_mnc_digits(&imsi, (mmgsdi_data_type *)pAd, &num_mnc_digits);

    if (readStatus)
    {
      pCtx->num_mnc_digits = (size_t)num_mnc_digits;
    } 

    policyman_mem_free(imsi.data_ptr);
  }

  /* post a command to process the received IMSI */
  {
    policyman_ctx_cmd_t  *pCmd = NULL; 

    pCmd = (policyman_ctx_cmd_t *) policyman_cmd_new( sizeof(policyman_ctx_cmd_t),
                                                      policyman_process_imsi,
                                                      NULL,
                                                      NULL );


    pCmd->ctx = *pCtx;
  
    policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
    ref_cnt_obj_release(pCmd);
  }

Done:

  // memory was allocated in pm_read_imsi_hplmn()
  if (pCtx != NULL)
  {
    policyman_mem_free(pCtx);
  }
}

/*-------- policyman_process_imsi_cb --------*/
STATIC void policyman_process_imsi_cb(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  policy_mmgsdi_ctx_t    *pCtx;
  mmgsdi_data_type const *pRawImsidata;
  policyman_status_t      readStatus;

  if (cnf_ptr == NULL || status != MMGSDI_SUCCESS || cnf != MMGSDI_READ_CNF)
  {
    POLICYMAN_MSG_MED_3("cnf_ptr - %x, status - %d, cnf - %d ", cnf_ptr, status, cnf);
    return;
  }

  pCtx         = (policy_mmgsdi_ctx_t *)cnf_ptr->response_header.client_data;
  pRawImsidata = (mmgsdi_data_type *)&cnf_ptr->read_cnf.read_data;

  // process MMGSDI data to get IMSI into context
  POLICYMAN_MSG_HIGH_1("received Raw IMSI bytes with data length %d", pRawImsidata->data_len);

  memscpy( pCtx->imsi, 
           sizeof(pCtx->imsi), 
           pRawImsidata->data_ptr, 
           pRawImsidata->data_len );


  // request for EF-AD file read
  pCtx->file_name = policyman_get_ad_file_id(pCtx->session_info->app_type); 
  readStatus      = pm_mmgsdi_read( pCtx->session_info->session_type,
                                    pCtx->file_name,
                                    0,
                                    PM_MMGSDI_RPM_AD_SIZE,
                                    policyman_process_ad_cb,
                                    (mmgsdi_client_data_type)pCtx );

  if (readStatus != POLICYMAN_STATUS_SUCCESS)
  {
    policyman_mem_free(pCtx);
    POLICYMAN_MSG_ERROR_1("triggered EFS-AD file read returned status %d", readStatus);
  }
}

/*-------- policyman_uim_set_imsi_m_mcc --------*/
STATIC boolean policyman_uim_set_imsi_m_mcc(
  sys_modem_as_id_e_type  subsId,
  sys_mcc_type            mcc
)
{
  boolean               changed  = FALSE;
  policyman_uim_info_t *pUimInfo = policyman_uim_get_uim_info(subsId);

  if (pUimInfo->csim_mcc != mcc)
  {
    pUimInfo->csim_mcc = mcc;
    changed            = TRUE;
    POLICYMAN_MSG_HIGH_2("updating IMSI_M MCC %d for Sub %d", mcc, subsId);
  }

  return changed;
}
 
/*-------- policyman_uim_get_imsi_m_mcc --------*/
uint32 policyman_uim_get_imsi_m_mcc(
  size_t              subsId
)
{
  return policyman_uim_get_uim_info(subsId)->csim_mcc;
}
 
/*===========================================================================
  FUNCTION pm_mmgsdi_read()

  DESCRIPTION
 ===========================================================================*/
static policyman_status_t pm_mmgsdi_read
(
    mmgsdi_session_type_enum_type     session_type,
      /* session whose files to read */

    mmgsdi_file_enum_type             file_name,
        /* Name of the file to read */

    mmgsdi_rec_num_type               record_number,
        /* Record number to begin with, in the file */

    mmgsdi_len_type                   request_length,
        /* Total bytes to be read */

    mmgsdi_callback_type              response_cb_ptr,
      /* Callback function to be called */

    mmgsdi_client_data_type           client_ref
)
{
  mmgsdi_return_enum_type	      mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_session_id_type		  pm_mmgsdi_session_id;
  mmgsdi_access_type			  file_access;
  pm_mmgsdi_session_id_table_entry_s_type *ssn_entry;

  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = file_name;

  if (NULL == (ssn_entry = pm_mmgsdi_ssn_tbl_get_entry_by_type(session_type)))
  {
    POLICYMAN_MSG_ERROR("Unable to get session id; session type: %d", session_type, 0, 0);
    return POLICYMAN_STATUS_ERR;
  }
  else
  {
    pm_mmgsdi_session_id = ssn_entry->session_id;
  }

  mmgsdi_status = mmgsdi_session_read_transparent( pm_mmgsdi_session_id,
                                                   file_access,
                                                   0,
                                                   request_length,
                                                   response_cb_ptr,
                                                   client_ref );
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    POLICYMAN_MSG_ERROR_0("mmgsdi_read_transparent failed.");
    return POLICYMAN_STATUS_ERR;
  }

  return POLICYMAN_STATUS_SUCCESS;
}

/*===========================================================================
  FUNCTION pm_read_imsi_hplmn()

  DESCRIPTION
 ===========================================================================*/
static policyman_status_t pm_read_imsi_hplmn(
  mmgsdi_session_type_enum_type session_type
  )
{
  policyman_status_t                       status    = POLICYMAN_STATUS_ERR;
  pm_mmgsdi_session_id_table_entry_s_type *ssn_entry = NULL;
  policy_mmgsdi_ctx_t                     *pCtx;

  ssn_entry = pm_mmgsdi_ssn_tbl_get_entry_by_type(session_type);

  if (ssn_entry == NULL)
  {
    POLICYMAN_MSG_ERROR_0("NULL pointer for session entry");
    return status;
  }

  pCtx = (policy_mmgsdi_ctx_t *)policyman_mem_alloc(sizeof(policy_mmgsdi_ctx_t));

  /* Determine session info and file type to read */
  pCtx->session_info = ssn_entry;
  pCtx->file_name    = policyman_get_imsi_file_id(pCtx->session_info->app_type);

  /* Read EF-IMSI */
  status = pm_mmgsdi_read( session_type,
                           pCtx->file_name,
                           0,
                           MMGSDI_IMSI_LEN,
                           policyman_process_imsi_cb,
                           (mmgsdi_client_data_type)pCtx );

  if (status != POLICYMAN_STATUS_SUCCESS)
  {
    policyman_mem_free(pCtx);
    POLICYMAN_MSG_ERROR_1("triggered EFS-IMSI file read returned status %d", status);
  }

  return status;
} /* pm_read_imsi_hplmn() */

/*-------- policyman_uim_set_subs_active --------*/
static void policyman_uim_set_subs_active(
  sys_modem_as_id_e_type subsId,
  boolean                state
)
{  
  /* Update in state that SUBS is ready*/
  policyman_uim_info_t *pUimInfo = policyman_uim_get_uim_info(subsId);

  if (pUimInfo->subs_active != state)
  {
    pUimInfo->subs_active = state;
    POLICYMAN_MSG_HIGH_3("subs %d: active state updated from %d to %d", subsId, pUimInfo->subs_active, state);
  }  
}


/*===========================================================================
  FUNCTION pm_mmgsdi_refresh_cb()

  DESCRIPTION
 ===========================================================================*/
static void pm_mmgsdi_refresh_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
  )
{
   /* DO Nothing */
   if ((cnf != MMGSDI_REFRESH_CNF) || (status != MMGSDI_SUCCESS))
   {
      POLICYMAN_MSG_ERROR("Refresh Complete Ack failed %d %d", cnf, status, 0);
   }
} /* pm_mmgsdi_refresh_cb */


/*-------- policyman_process_gw_subs --------*/
STATIC void policyman_process_gw_subs(
  pm_mmgsdi_session_id_table_entry_s_type *ssn_entry,
  mmgsdi_event_data_type                  *pEvent
)
{
  mmgsdi_refresh_file_list_type  file_info;
  mmgsdi_file_enum_type          files[PM_MMGSDI_NUM_REGISTERED_FILES] = { MMGSDI_GSM_IMSI,MMGSDI_GSM_AD };
  policyman_status_t             status;
  mmgsdi_return_enum_type        mmgsdi_status;

  /* read IMSI PLMN for GW subs */
  status = pm_read_imsi_hplmn(ssn_entry->session_type);

  if (status != POLICYMAN_STATUS_SUCCESS)
  {
    POLICYMAN_MSG_ERROR_1("Internal error in reading HPLMN %d", status);
  }

  if (ssn_entry->app_type == MMGSDI_APP_USIM)
  {
    files[0] = MMGSDI_USIM_IMSI;
    files[1] = MMGSDI_USIM_AD;
  }

  file_info.file_list_ptr = files;
  file_info.num_files     = PM_MMGSDI_NUM_REGISTERED_FILES;

  mmgsdi_status = mmgsdi_session_register_for_refresh( pEvent->session_id,
                                                       file_info,
                                                       FALSE,
                                                       pm_mmgsdi_refresh_cb,
                                                       0 );

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    POLICYMAN_MSG_ERROR_1("got mmsgdi error in refresh %d", mmgsdi_status);
  }

  policyman_state_set_precondition_met(POLICYMAN_PRECOND_SIMTYPE, ssn_entry->asubs_id);
  policyman_uim_set_subs_active(ssn_entry->asubs_id, TRUE);
  policyman_state_set_sim_type(ssn_entry->asubs_id, ssn_entry->app_type);  
}


/*-------- policyman_process_subs_ready_event --------*/
STATIC void policyman_process_subs_ready_event(
  pm_mmgsdi_session_id_table_entry_s_type *ssn_entry,
  mmgsdi_event_data_type                  *pEvent
)
{
  policyman_state_set_uim_num_subs(policyman_state_get_uim_num_subs() + 1);

  if (is_mmgsdi_subscription_ready_received == FALSE)
  {
    is_mmgsdi_subscription_ready_received = TRUE;

    /* Print out all configuration database when receiving the first MMGSDI_SUBSCRIPTION_READY_EVT
        */
    POLICYMAN_MSG_MED_0("Received the first MMGSDI_SUB_READY_EVT, print all configuration database");
    policyman_cfgitem_display_all();
  }

  ssn_entry->app_type =  pEvent->data.subscription_ready.app_info.app_data.app_type;
  ssn_entry->active   = TRUE;

  if (sim_session_is_gw(ssn_entry->session_type))
  {
    policyman_process_gw_subs(ssn_entry, pEvent);
  }
}

/*-------- policyman_process_refresh_event --------*/
STATIC void policyman_process_refresh_event(
  pm_mmgsdi_session_id_table_entry_s_type *ssn_entry,
  mmgsdi_event_data_type                  *pEvent
)
{
  mmgsdi_return_enum_type ret = MMGSDI_ERROR;

  POLICYMAN_MSG_HIGH_3( "Received MMGSDI_REFRESH_EVT for session %d, mode %d, stage %d",
                        ssn_entry->session_type,
                        pEvent->data.refresh.mode,
                        pEvent->data.refresh.stage );

  if (pEvent->data.refresh.mode == MMGSDI_REFRESH_NAA_FCN)
  {
    if (pEvent->data.refresh.stage == MMGSDI_REFRESH_STAGE_START)
    {
      policyman_status_t status;

      status = pm_read_imsi_hplmn(ssn_entry->session_type);

      if (status != POLICYMAN_STATUS_SUCCESS)
      {
        POLICYMAN_MSG_ERROR_1("Internal error in reading HPLMN %d", status);
      }
    }
    else if (pEvent->data.refresh.stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN)
    {
      //Notify MMGSDI it's okay to proceed with FCN refresh
      ret = mmgsdi_session_ok_to_refresh( pEvent->session_id,
                                          TRUE,
                                          pm_mmgsdi_refresh_cb,
                                          0 );
    }
  }
  else if (  (pEvent->data.refresh.mode  == MMGSDI_REFRESH_NAA_INIT_FCN) 
          && (pEvent->data.refresh.stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN)
          )
  {
    //Notify MMGSDI it's okay to proceed with FCN refresh
    ret = mmgsdi_session_ok_to_refresh( pEvent->session_id,
                                        TRUE,
                                        pm_mmgsdi_refresh_cb,
                                        0 );
  }
  if ( ret != MMGSDI_SUCCESS )
  {
    POLICYMAN_MSG_ERROR_1(" Error: mmgsdi_session_ok_to_refresh returned error %d", ret);
  }
}

/*===========================================================================
  FUNCTION pm_process_uim_evt()

  DESCRIPTION
 ===========================================================================*/
void pm_process_uim_evt(
  policyman_cmd_t *pCmd
)
{
  mmgsdi_event_data_type                  *pEvent = &((policyman_uim_evt_cb_s*)pCmd)->mmgsdi_event;
  pm_mmgsdi_session_id_table_entry_s_type *ssn_entry = NULL;

  POLICYMAN_MSG_HIGH("PM got MMGSDI evt %d", pEvent->evt, 0, 0);

  ssn_entry = pm_mmgsdi_ssn_tbl_get_entry_by_id(pEvent->session_id);

  if (ssn_entry == NULL)
  {
    POLICYMAN_MSG_ERROR_1("session entry is NULL, not processing event %d", pEvent->evt);
    return;
  }

  if (policyman_subs_id_is_configured(ssn_entry->asubs_id))
  {
    POLICYMAN_MSG_ERROR_1("session entry is NULL, not processing event %d", pEvent->evt);
    print_session_info(ssn_entry);

    switch (pEvent->evt)
    {
      case MMGSDI_PIN1_EVT:
        break;

      case MMGSDI_SUBSCRIPTION_READY_EVT:
        policyman_process_subs_ready_event(ssn_entry, pEvent);
        break;

      case MMGSDI_SESSION_CHANGED_EVT:
        if (!pEvent->data.session_changed.activated 
           && ssn_entry->active == TRUE 
           )
        {
          ssn_entry->active = FALSE;
          policyman_uim_set_subs_active(ssn_entry->asubs_id, FALSE);
          policyman_state_delete_uim_info(ssn_entry->asubs_id, ssn_entry->session_type);
        }
        break;

      case MMGSDI_REFRESH_EVT:
        policyman_process_refresh_event(ssn_entry, pEvent);
        break;

      default:
       break;
    }
  }
}

/*===========================================================================
  FUNCTION policyman_dtor_card_status()

  DESCRIPTION
 ===========================================================================*/
void policyman_dtor_card_status
(
  policyman_cmd_t *pCmd
)
{
   /* Free the memory for the array of items. */
   if (((policyman_uim_evt_cb_s*)pCmd)->mmgsdi_event.evt == MMGSDI_REFRESH_EVT)
     POLICYMAN_MEM_FREE_IF(((policyman_uim_evt_cb_s*)pCmd)->mmgsdi_event.data.refresh.refresh_files.file_path_ptr);
}

/*===========================================================================
  FUNCTION pm_mmgsdi_reg_status_cb()

  DESCRIPTION
 ===========================================================================*/
static void pm_mmgsdi_reg_status_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    data_type,
  const mmgsdi_cnf_type  *data_ptr
  )
{
   pm_mmgsdi_resp_s_type *pCmd = NULL;

   PM_CHECK_PTR_RET(data_ptr, VOID);

   pCmd = (pm_mmgsdi_resp_s_type *) policyman_cmd_new( sizeof(pm_mmgsdi_resp_s_type),
                                                       pm_process_mmgsdi_reg_status,
                                                       NULL,
                                                       NULL
                                                     );

   pCmd->data_type = data_type;
   pCmd->status = status;
   pCmd->data = *data_ptr;

   policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
   ref_cnt_obj_release(pCmd);
}

/*===========================================================================
  FUNCTION pm_mmgsdi_card_status_cb()

  DESCRIPTION
 ===========================================================================*/
static void pm_mmgsdi_card_status_cb(
  const mmgsdi_event_data_type   *mmgsdi_event
  )
{
   policyman_uim_evt_cb_s *pCmd = NULL;

   PM_CHECK_PTR_RET(mmgsdi_event, VOID);

   pCmd = (policyman_uim_evt_cb_s *) policyman_cmd_new( sizeof(policyman_uim_evt_cb_s),
                                          pm_process_uim_evt,
                                          policyman_dtor_card_status,
                                          NULL
                                          );

   pCmd->mmgsdi_event = *mmgsdi_event;

   if (mmgsdi_event->evt == MMGSDI_REFRESH_EVT)
   {
      //Init this in case number of files is zero
      pCmd->mmgsdi_event.data.refresh.refresh_files.file_list_ptr = NULL;

      if ((mmgsdi_event->data.refresh.refresh_files.file_path_len > 0) &&
          (mmgsdi_event->data.refresh.refresh_files.file_path_ptr != NULL))
      {
        uint32 list_size = mmgsdi_event->data.refresh.refresh_files.file_path_len;

        //Need to copy FCN refresh path list seperately.
        //Need to copy only paths, since we always get only paths
        pCmd->mmgsdi_event.data.refresh.refresh_files.file_path_ptr = (uint8*)policyman_mem_alloc(list_size);

        PM_CHECK_PTR_RET(pCmd->mmgsdi_event.data.refresh.refresh_files.file_path_ptr, VOID);

        memscpy( pCmd->mmgsdi_event.data.refresh.refresh_files.file_path_ptr,list_size,
                 mmgsdi_event->data.refresh.refresh_files.file_path_ptr,list_size
               );
      }
   }
   
   policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
   ref_cnt_obj_release(pCmd);
}


/*===========================================================================
  FUNCTION pm_open_session_cb()

  DESCRIPTION
 ===========================================================================*/
void pm_open_session_cb( 
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    data_type,
  const mmgsdi_cnf_type  *data_ptr
  )
{
  uint16 i = 0,j = 0;

  /* data_ptr is NULL for ICC card where open_session returns success */
  PM_CHECK_PTR_RET(data_ptr, VOID);
  PM_CHECK_PTR_RET(data_ptr->session_open_ext_cnf.session_info_ptr, VOID);

  if ( status == MMGSDI_SUCCESS )
  {
    if (data_type == MMGSDI_SESSION_OPEN_EXT_CNF)
    {
      for (i = 0; i < data_ptr->session_open_ext_cnf.num_sessions; i++)
      {
        for (j = 0; j < ARR_SIZE(pm_mmgsdi_session_id_table); j++)
        {
          if(pm_mmgsdi_session_id_table[j].session_type == data_ptr->session_open_ext_cnf.session_info_ptr[i].session_type)
          {
            pm_mmgsdi_session_id_table[j].session_id = data_ptr->session_open_ext_cnf.session_info_ptr[i].session_id;
            pm_mmgsdi_session_id_table[j].app_type = data_ptr->session_open_ext_cnf.session_info_ptr[i].app_info.app_type;
            pm_mmgsdi_session_id_table[j].in_use = TRUE;

            POLICYMAN_MSG_HIGH("num_sessions %d session_id %d session_type %d",
                               data_ptr->session_open_ext_cnf.num_sessions,
                               pm_mmgsdi_session_id_table[j].session_id,
                               pm_mmgsdi_session_id_table[j].session_type
                              );
          }
        }
      }
    }
  }
}

/*===========================================================================
  FUNCTION pm_open_sessions()

  DESCRIPTION
 ===========================================================================*/
static void pm_open_sessions(
  void
  )
{
  mmgsdi_return_enum_type result;
  uint32                  session_type_mask = ( MMGSDI_GW_PROV_PRI_SESSION_MASK
                                              | MMGSDI_1X_PROV_PRI_SESSION_MASK
                                              | MMGSDI_GW_PROV_SEC_SESSION_MASK );

  result = mmgsdi_session_open_ext(pm_mmgsdi_client_id,
                                    session_type_mask,
                                    pm_mmgsdi_card_status_cb,
                                    FALSE,
                                    pm_open_session_cb,
                                    (mmgsdi_client_data_type)session_type_mask );

  if ( MMGSDI_SUCCESS != result )
  {
    /* log that error and return */
    POLICYMAN_MSG_ERROR("PM session open failed with error %d",result,0,0);
  }
}

/*===========================================================================
 FUNCTION policyman_uim_get_num_subs()

 DESCRIPTION
===========================================================================*/
uint32 policyman_uim_get_num_subs(
  void
  )
{
  policyman_device_info_t *pInfo = policyman_state_get_state()->pDeviceInfo;

  return (pInfo != NULL) ? pInfo->uimNSubs : 0;
}

/*===========================================================================
 FUNCTION policyman_uim_set_num_subs()

 DESCRIPTION
===========================================================================*/
void policyman_uim_set_num_subs(
  policyman_state_t  *pInfo,
  uint32                nSubs
  )
{
  if (pInfo != NULL && nSubs <= MAX_SIMS)
  {
    pInfo->pDeviceInfo->uimNSubs = (size_t)nSubs;
  }
}


/*-------- policyman_uim_get_subs_plmn --------*/
static sys_plmn_id_s_type *policyman_uim_get_subs_plmn(
  sys_modem_as_id_e_type subsId
  )
{
  return &policyman_uim_get_uim_info(subsId)->plmn;
}


/*===========================================================================
 FUNCTION policyman_uim_get_imsi_mcc()

 DESCRIPTION
===========================================================================*/
uint32 policyman_uim_get_imsi_mcc(
  size_t             subsId
  )
{
  return policyman_plmn_get_mcc(policyman_uim_get_subs_plmn(subsId));
}


/*===========================================================================
 FUNCTION policyman_uim_get_imsi_plmn()

 DESCRIPTION
===========================================================================*/
boolean policyman_uim_get_imsi_plmn(
  policyman_state_t  *pInfo,
  size_t                subsId,
  sys_plmn_id_s_type    *pPlmn
  )
{
  if (pInfo == NULL || subsId >= MAX_SIMS)    
  { 
    return FALSE; 
  }

  *pPlmn = *policyman_uim_get_subs_plmn(subsId);

  return TRUE;
}

/*===========================================================================
 FUNCTION policyman_uim_set_sim_type()

 DESCRIPTION
   Set SIM app type into UIM state
===========================================================================*/
boolean policyman_uim_set_sim_type(
  policyman_state_t    *pInfo,
  size_t                subsId,
  mmgsdi_app_enum_type sim_app_type
  )
{
  boolean               changed  = FALSE;
  policyman_uim_info_t *pUimInfo = policyman_uim_get_uim_info(subsId);

  if (sim_app_type != pUimInfo->sim_app_type)
  {
    pUimInfo->sim_app_type = sim_app_type;
    POLICYMAN_MSG_HIGH("updated sim_app_type %d for subs %d", sim_app_type, subsId, 0);
    changed = TRUE;
  }

  return changed;
}

/*===========================================================================
 FUNCTION policyman_uim_get_sim_type()

 DESCRIPTION
   Get SIM app type from UIM state
===========================================================================*/
void policyman_uim_get_sim_type(
  policyman_state_t  *pInfo,
  size_t                subsId,
  mmgsdi_app_enum_type *sim_app_type
  )
{
  if(sim_app_type && pInfo)
  {
    *sim_app_type = policyman_uim_get_uim_info(subsId)->sim_app_type;
  }
  else
  {
    POLICYMAN_MSG_ERROR("sim_app_type is NULL", 0, 0, 0);
  }
}

/*===========================================================================
 FUNCTION policyman_uim_set_cdma_sim_type()

 DESCRIPTION
   Set CDMA app type into UIM state
===========================================================================*/
boolean policyman_uim_set_cdma_sim_type(
  policyman_state_t    *pInfo,
  size_t                subsId,
  mmgsdi_app_enum_type sim_app_type
  )
{
  boolean               changed  = FALSE;
  policyman_uim_info_t *pUimInfo = policyman_uim_get_uim_info(subsId);

  if (sim_app_type != pUimInfo->cdma_app_type)
  {
    pUimInfo->cdma_app_type = sim_app_type;
    POLICYMAN_MSG_HIGH("updated cdma_sim_app type %d for subs %d", sim_app_type, subsId, 0);
    changed = TRUE;
  }

  return changed;
}

/*===========================================================================
 FUNCTION policyman_uim_get_cdma_sim_type()

 DESCRIPTION
   Get CDMA SIM app type from UIM state
===========================================================================*/
mmgsdi_app_enum_type policyman_uim_get_cdma_sim_type(
  policyman_state_t     *pInfo,
  size_t                 subsId
)
{
  return policyman_uim_get_uim_info(subsId)->cdma_app_type;
}

/*-------- policyman_uim_set_sim_refresh --------*/
boolean policyman_uim_set_sim_refresh(
  size_t  subsId
  )
{
  policyman_uim_info_t *pUimInfo = policyman_uim_get_uim_info(subsId);

  pUimInfo->sim_refreshed = TRUE;
  POLICYMAN_MSG_HIGH_1("SIM IMSI refreshed for subs %d", subsId);

  return TRUE;
}

/*-------- policyman_uim_get_sim_refresh --------*/
boolean policyman_uim_get_sim_refresh(
  size_t  subsId
  )
{
  policyman_uim_info_t *pUimInfo = policyman_uim_get_uim_info(subsId);
  return pUimInfo->sim_refreshed;
}

/*-------- policyman_uim_sim_is_3gpp2 --------*/
boolean policyman_uim_sim_is_3gpp2(
  policy_execute_ctx_t *pCtx
)
{
  mmgsdi_app_enum_type simType;

  // subsId here is DDS subs
  simType= policyman_uim_get_cdma_sim_type(pCtx->pState, pCtx->asubs_id);

  return (  simType == MMGSDI_APP_RUIM 
         || simType == MMGSDI_APP_CSIM 
         );
}

/*=============================================================================
  Conditions based on serving system.
=============================================================================*/

#define MAX_UIM_CONDITION_SET_SIZE   50


/*-----------------------------------------------------------------------------
  condition_imsi_mcc
-----------------------------------------------------------------------------*/


typedef struct
{
  POLICYMAN_CONDITION_BASE;

  policyman_set_t *pMccSet;
  char const      *pSetName;
  boolean          checkIMSI_M;
} policyman_imsi_mcc_condition_t;

/*-------- policyman_str_to_session --------*/
STATIC mmgsdi_session_type_enum_type policyman_str_to_session(
  char const  *pStr
  )
{
  typedef struct
  {
    const char                    *pStr;
    mmgsdi_session_type_enum_type  session;
  } session_map;

  static session_map   map[] =
  {
    {"gw_pri",  MMGSDI_GW_PROV_PRI_SESSION},
    {"gw_sec",  MMGSDI_GW_PROV_SEC_SESSION},
    {"1x",      MMGSDI_1X_PROV_PRI_SESSION},
  };

  mmgsdi_session_type_enum_type  session = MMGSDI_MAX_SESSION_TYPE_ENUM;

  size_t  i;
  char    token[32];

  policyman_get_token(&pStr, token, sizeof(token));

  for (i = 0 ; i < ARR_SIZE(map) ; ++i)
  {
    if (strcasecmp(token, map[i].pStr) == 0)
    {
      session = map[i].session;
      break;
    }
  }

#if defined(TEST_FRAMEWORK)
  #error code not present
#endif /* +TEST_FRAMEWORK. */

  return session;
}


/*-------- evaluate_imsi_mcc_in --------*/
static boolean evaluate_imsi_mcc_in(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
  )
{
  policyman_imsi_mcc_condition_t *pCond = (policyman_imsi_mcc_condition_t *) pCondition;
  policy_execute_ctx_t           *piCtx = (policy_execute_ctx_t *) pCtx;
  uint32                          uimMcc = 0;
  boolean                         result = FALSE;

  /*  If we don't have a set but *do* have a set name, try to find the named set.
   */

  if (pCond->pMccSet == NULL && pCond->pSetName != NULL)
  {    
    pCond->pMccSet = (policyman_set_t *) mre_named_object_find(
                            pCond->pSetName,
                            POLICYMAN_NAMED_MCC_SET,
                            pCond->pPolicy
                            );
    POLICYMAN_MEM_FREE_IF(pCond->pSetName);

    if (NULL != pCond->pMccSet)
    {
      ref_cnt_obj_add_ref(pCond->pMccSet);
    }

  }

  /*  Without a set, we can't work.
   */
  if (pCond->pMccSet == NULL)             
  { 
    POLICYMAN_MSG_ERROR_0("named PLMN set not found");
    goto Done;
  }

  if (pCond->checkIMSI_M)
  {
    uimMcc = policyman_uim_get_imsi_m_mcc(subsId);
  }
  else
  { 
    uimMcc = policyman_uim_get_imsi_mcc(subsId);
  }

  result =  policyman_set_contains(pCond->pMccSet, &uimMcc);

Done:
  POLICYMAN_MSG_HIGH_3("condition <imsi_mcc_in> for subs %d with mcc %d returns %d", subsId, uimMcc, result);
  return result;
}


/*-------- policyman_condition_imsi_mcc_evaluate --------*/
static boolean policyman_condition_imsi_mcc_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
  )
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;
  return policyman_evaluate_cond(pCond, pCtx, evaluate_imsi_mcc_in);
}


/*-------- policyman_condition_imsi_mcc_dtor --------*/
static void policyman_condition_imsi_mcc_dtor(
  void  *pObj
  )
{
  policyman_imsi_mcc_condition_t  *pCondition =
                                    (policyman_imsi_mcc_condition_t *) pObj;

  REF_CNT_OBJ_RELEASE_IF(pCondition->pMccSet);
  POLICYMAN_MEM_FREE_IF(pCondition->pSetName);
  policyman_condition_dtor(pCondition);
}


/*-------- policyman_condition_imsi_mcc_new --------*/
mre_status_t policyman_condition_imsi_mcc_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  )
{
  mre_status_t                    status = MRE_STATUS_ERR_INVALID_CONDITION;
  policyman_imsi_mcc_condition_t  *pCondition = NULL;
  policyman_set_t                 *pMccSet = NULL;
  char const                      *pSetName = NULL;
  char const                      *pStr;
  char                            mccStr[4];
  sys_mcc_type                    mcc;
  policyman_subs_t                pm_subs;
  boolean                         checkIMSI_M = FALSE;
  mmgsdi_session_type_enum_type   session;

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);
  if (MRE_FAILED(status))
  {
    goto Done;
  }

  /*  See if we need to check against 1x IMSI_M MCC.
   */
  pStr = policyman_xml_get_attribute(pElem, "session");
  if (pStr != NULL)
  {
    session = policyman_str_to_session(pStr);
    if (session == MMGSDI_1X_PROV_PRI_SESSION)
    {
      checkIMSI_M = TRUE;
    }
  }

  /*  See if there is a named MCC list to use for this condition.
   */
  pStr = policyman_xml_get_attribute(pElem, "list");
  if (pStr != NULL)
  {
    pSetName = policyman_str_dup(pStr);
  }
  else
  {
    /* No named set; try and get an inline list
     */
    pStr = policyman_xml_get_text(pElem);
    if (pStr != NULL)
    {
      /* Create a policyman set to hold the MCCs
       */
      pMccSet = policyman_set_new(sizeof(mcc), 5, MAX_UIM_CONDITION_SET_SIZE, NULL, NULL);

      /* Add the MCCs to the set
       */
      while (policyman_get_token(&pStr, mccStr, sizeof(mccStr)))
      {
        mcc = atoi(mccStr);
        if (mcc == 0)
        {
          REF_CNT_OBJ_RELEASE_IF(pMccSet);
          POLICYMAN_UT_MSG_1("<imsi_mcc_in>: invalid MCC: %s", mccStr);
          goto Done;
        }

        policyman_set_add(pMccSet, &mcc);
      }
    }
  }

  /* If there is neither an inline or named list, bail.
   */
  if (pMccSet == NULL && pSetName == NULL)
  {
    POLICYMAN_MSG_ERROR_0("<imsi_mcc_in>: no inline MCC or named list");
    goto Done;
  }

  /* Create the condition
   */
  pCondition = (policyman_imsi_mcc_condition_t *)
                  policyman_mem_alloc(sizeof(policyman_imsi_mcc_condition_t));

  ref_cnt_obj_init(pCondition, policyman_condition_imsi_mcc_dtor);

  pCondition->pMccSet     = pMccSet;
  pCondition->pSetName    = pSetName;
  pCondition->isTrue      = policyman_condition_imsi_mcc_evaluate;
  pCondition->pmSubsId    = pm_subs;
  pCondition->checkIMSI_M = checkIMSI_M;

  status = MRE_STATUS_SUCCESS;

Done:
  *ppCondition = (mre_condition_t *) pCondition;
  return status;
}



/*-----------------------------------------------------------------------------
  condition_imsi_plmn
-----------------------------------------------------------------------------*/


typedef struct
{
  POLICYMAN_CONDITION_BASE;

  char const      *pSetName;
  policyman_set_t *pPlmnSet;
} policyman_imsi_plmn_condition_t;


/*-------- evaluate_imsi_plmn_in --------*/
static boolean evaluate_imsi_plmn_in(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
  )
{
  policyman_imsi_plmn_condition_t *pCond = (policyman_imsi_plmn_condition_t *) pCondition;
  policy_execute_ctx_t            *piCtx = (policy_execute_ctx_t *) pCtx;
  sys_plmn_id_s_type               uimPlmn;
  boolean                          result = FALSE;

  if (pCond->pPlmnSet == NULL && pCond->pSetName != NULL)
  {
    pCond->pPlmnSet = (policyman_set_t *) mre_named_object_find(
                            pCond->pSetName,
                            POLICYMAN_NAMED_PLMN_SET,
                            pCond->pPolicy
                            );

    if (NULL != pCond->pPlmnSet)
    {
      ref_cnt_obj_add_ref(pCond->pPlmnSet);
    }
  }

  POLICYMAN_MEM_FREE_IF(pCond->pSetName);
  if (pCond->pPlmnSet == NULL)             
  { 
    POLICYMAN_MSG_ERROR_0("failed to get PLMN set");
    goto Done; 
  }

  policyman_state_get_uim_plmn(piCtx->pState, subsId, &uimPlmn);

  result = policyman_set_contains(pCond->pPlmnSet, &uimPlmn);
  POLICYMAN_MSG_MED_0("IMSI PLMN: ");
  policyman_plmn_print(&uimPlmn);

Done:

  POLICYMAN_MSG_HIGH_2("condition <imsi_plmn_in> for subs %d returns %d", subsId, result);
  return result;
}


/*-------- policyman_condition_imsi_plmn_evaluate --------*/
STATIC boolean policyman_condition_imsi_plmn_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
  )
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;
  return policyman_evaluate_cond(pCond, pCtx, evaluate_imsi_plmn_in);
}


/*-------- policyman_condition_imsi_plmn_dtor --------*/
static void policyman_condition_imsi_plmn_dtor(
  void  *pObj
  )
{
  policyman_imsi_plmn_condition_t *pCondition =
                                    (policyman_imsi_plmn_condition_t *) pObj;

  REF_CNT_OBJ_RELEASE_IF(pCondition->pPlmnSet);
  POLICYMAN_MEM_FREE_IF(pCondition->pSetName);
  policyman_condition_dtor(pCondition);
}


/*-------- policyman_condition_imsi_plmn_new --------*/
mre_status_t policyman_condition_imsi_plmn_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  )
{
  mre_status_t                    status = MRE_STATUS_ERR_INVALID_CONDITION;
  policyman_imsi_plmn_condition_t *pCondition = NULL;
  char const                      *pStr;
  policyman_subs_t                pm_subs;

  /*  Allocate the condition
   */
  pCondition = (policyman_imsi_plmn_condition_t *)
                  policyman_mem_alloc(sizeof(policyman_imsi_plmn_condition_t));
  ref_cnt_obj_init(pCondition, policyman_condition_imsi_plmn_dtor);
  pCondition->isTrue = policyman_condition_imsi_plmn_evaluate;

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);
  if (MRE_FAILED(status))
  {
    goto Done;
  }
  pCondition->pmSubsId = pm_subs;

  /*  See if there is a named PLMN list to use for this condition.
   */
  pStr = policyman_xml_get_attribute(pElem, "list");
  if (pStr != NULL)
  {
    pCondition->pSetName = policyman_str_dup(pStr);
    status = MRE_STATUS_SUCCESS;
    goto Done;
  }

  /*  No name - read an inline list.
   */
  pStr = policyman_xml_get_text(pElem);
  if (pStr != NULL)
  {
    pCondition->pPlmnSet = policyman_plmn_list_read(pStr);
    if (pCondition->pPlmnSet != NULL)
    {
      status = MRE_STATUS_SUCCESS;
    }
  }

Done:
  if (MRE_SUCCEEDED(status))
  {
    *ppCondition = (mre_condition_t *) pCondition;
  }
  else
  {
    POLICYMAN_UT_MSG_0("<imsi_plmn_in>: PLMN list required");
    REF_CNT_OBJ_RELEASE_IF(pCondition);
  }

  return status;
}
#endif