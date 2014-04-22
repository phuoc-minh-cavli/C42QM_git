/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            PBM _ W M S. C

GENERAL DESCRIPTION
  This file contains the PBM functions to interact with WMS module.

  Copyright (c) 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE
$Header: //components/rel/uim.mpss/3.3.1/pbm/src/pbm_wms.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/23/16   nr      ECC optimizations for IoE devices
05/16/16   sp      Logging improvements
02/01/16   nr      Initial Revision
===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "pbm_common_types.h"
#include "pbm.h"
#include "string.h"
#include "pbmgsdi.h"
#include "pbmutils.h"
#include "pbmuim.h"
#include "stringl/stringl.h"
#include "wms.h"
#include "uim_msg.h"
#include "uim_pbm.h"

/*===========================================================================
                         DEFINITIONS
===========================================================================*/
uint32 pbm_wms_init_finished = 0;

/*===========================================================================
FUNCTION pbm_wms_ready_event_cb

DESCRIPTION
  This function is registered with WMS. A PBM command is added to the
  gsdi_resp queue when we receive this event.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_wms_ready_event_cb(
  wms_cfg_efs_read_evt_s_type *wms_cfg_efs_read_evt
)
{
  pbm_wms_resp_s_type *wms_resp_ptr = NULL;

  wms_resp_ptr = (pbm_wms_resp_s_type *)pbm_cmd_alloc(sizeof(pbm_wms_resp_s_type));
  if (!wms_resp_ptr)
  {
    UIM_MSG_ERR_0("Unable to allocate PBM command");
    (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
    return;
  }

  wms_resp_ptr->cmd_type              = PBM_WMS_STATUS_CB;
  wms_resp_ptr->info_ptr.session_type = wms_cfg_efs_read_evt->session_type;
  wms_resp_ptr->info_ptr.t_id         = wms_cfg_efs_read_evt->t_id;
  UIM_MSG_HIGH_2("PBM pbm_wms_ready_event_cb WMS TxId 0x%x session_type 0x%x",
	           wms_cfg_efs_read_evt->t_id,wms_cfg_efs_read_evt->session_type);

  pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(wms_resp_ptr));
} /* pbm_wms_ready_event_cb */


/*===========================================================================
FUNCTION pbm_alloc_txid

DESCRIPTION
  This function is will return the first availble free TxId to include in the
  pbm ready event mesage to WMS .

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
uint8 pbm_alloc_txid(
  pbm_session_enum_type pbm_session
)
{
  pbm_session_info[pbm_session].last_txid += 1;
  if(pbm_session_info[pbm_session].last_txid == 0)
  {
    pbm_session_info[pbm_session].last_txid += 1;
  }
  
  return pbm_session_info[pbm_session].last_txid;
} /* pbm_alloc_txid */


/*===========================================================================
FUNCTION pbm_free_txid

DESCRIPTION
  This function is will free the TxId once PBM receieved the CARD error event
  from WMS.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_free_txid(
  void
)
{
  uint8 session_loop_itrator;

  /* Intilizing the global last updated TxID to zero */
  for(session_loop_itrator = 0;session_loop_itrator < PBM_SESSION_MAX;session_loop_itrator++)
  {
    pbm_session_info[session_loop_itrator].last_txid = 0;
  }
} /* pbm_free_txid */


/*===========================================================================
FUNCTION pbm_session_to_mmgsdi_session_mapping

DESCRIPTION
  This function is will map the PBM sessions to MMGSDi session to send to WMS.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
mmgsdi_session_type_enum_type pbm_session_to_mmgsdi_session_mapping(
  pbm_session_enum_type pbm_session
)
{
  mmgsdi_session_type_enum_type mmgsdi_session_type = MMGSDI_MAX_SESSION_TYPE_ENUM;

  switch (pbm_session)
  {
    case PBM_SESSION_GPB_1:
      mmgsdi_session_type = MMGSDI_CARD_SESSION_SLOT_1;
      break;
    case PBM_SESSION_GPB_2:
      mmgsdi_session_type = MMGSDI_CARD_SESSION_SLOT_2;
      break;
    case PBM_SESSION_LPB_GW:
      mmgsdi_session_type = MMGSDI_GW_PROV_PRI_SESSION;
      break;
    case PBM_SESSION_LPB_1X:
      mmgsdi_session_type = MMGSDI_1X_PROV_PRI_SESSION;
      break;
#ifdef FEATURE_DUAL_SIM
    case PBM_SESSION_LPB_GW_SECONDARY:
      mmgsdi_session_type = MMGSDI_GW_PROV_SEC_SESSION;
      break;
    case PBM_SESSION_LPB_1X_SECONDARY:
      mmgsdi_session_type = MMGSDI_1X_PROV_SEC_SESSION;
      break;
#endif  /* FEATURE_DUAL_SIM */
#ifdef FEATURE_TRIPLE_SIM
    case PBM_SESSION_GPB_3:
      mmgsdi_session_type = MMGSDI_CARD_SESSION_SLOT_3;
      break;
    case PBM_SESSION_LPB_GW_TERTIARY:
      mmgsdi_session_type = MMGSDI_GW_PROV_TER_SESSION;
      break;
    case PBM_SESSION_LPB_1X_TERTIARY:
      mmgsdi_session_type = MMGSDI_1X_PROV_TER_SESSION;
      break;
#endif /* FEATURE_TRIPLE_SIM */
    default:
      break;
  }

  return mmgsdi_session_type;
} /* pbm_session_to_mmgsdi_session_mapping */


/*===========================================================================
FUNCTION pbm_send_ready_event_to_wms

DESCRIPTION
  This function is will fill the session info,TxID and the call back for PBM 
  ready message and call the WMS exposed API.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_send_ready_event_to_wms(pbm_session_enum_type pbm_session_type)
{
  boolean                      ret_val = TRUE;
  wms_cfg_efs_read_evt_s_type  wms_cfg_efs_read_evt;		
  wms_status_e_type            wms_status;

  if( pbm_session_type_to_pb_category(pbm_session_type) != PBM_LPB )
  {
    return ret_val;
  }

  /* Check if WMS init is set already or not,IT was already set in case of REFRESH FCN */
  if(pbm_wms_init_finished & PBM_SESSIONID_TO_SESSION_MASK(pbm_session_type))
  {
    return ret_val;
  }

  /* Don't query WMS ready in case of 1x_secondary and 1x_teritary and proceed
   * with initlization as WMS don't open sessions on 1x_secondary and 1x_teritary
   */
  if(pbm_session_type == PBM_SESSION_LPB_1X_SECONDARY || 
     pbm_session_type == PBM_SESSION_LPB_1X_TERTIARY)
  {
    /* Setting pbm_wms_init_finished as if like wms ready receieved from WMS 
     * in case of 1x_secondary and 1x_teritary */
    PBM_SESSION_SET(pbm_wms_init_finished,pbm_session_type);
    return ret_val;
  }

  wms_cfg_efs_read_evt.session_type = pbm_session_to_mmgsdi_session_mapping( pbm_session_type );
  wms_cfg_efs_read_evt.t_id         = pbm_alloc_txid(pbm_session_type);
  wms_cfg_efs_read_evt.cfg_event_cb = pbm_wms_ready_event_cb;
  wms_cfg_efs_read_evt.user_data    = NULL; /* User_data should not be used */

  UIM_MSG_HIGH_2("PBM pbm_send_ready_event_to_wms TxId 0x%x MMGSDI session 0x%x",
                  wms_cfg_efs_read_evt.t_id,wms_cfg_efs_read_evt.session_type);	
  if(wms_cfg_efs_read_evt.t_id == 0 || wms_cfg_efs_read_evt.session_type == MMGSDI_MAX_SESSION_TYPE_ENUM)
  {
    ret_val = FALSE;
  }
  else
  {
    wms_status = wms_cfg_send_sim_efs_read_event_to_pbm ( wms_cfg_efs_read_evt );
    if(wms_status != WMS_OK_S)
    {
      UIM_MSG_ERR_1("PBM wms_cfg_send_sim_efs_read_event_to_pbm fails wms_status 0x%x",wms_status);
      ret_val = FALSE;
    }
  }

  if(ret_val == TRUE)
  {
    UIM_MSG_HIGH_1("PBM pbm_send_ready_event_to_wms () Success 0x%x ",pbm_session_type);
  }

  return ret_val;
} /* pbm_send_ready_event_to_wms */


/*===========================================================================
FUNCTION pbm_wms_cfg_cb

DESCRIPTION
WMS callback.  Used to start reading from SIM.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_wms_cfg_cb (
  pbm_wms_resp_s_type* wms_resp_ptr
)
{
  pbm_session_enum_type pbm_session = PBM_SESSION_GPB_1;

  pbm_session = pbmif_mmgsdi_session_to_pbm_session(wms_resp_ptr->info_ptr.session_type );

  UIM_MSG_HIGH_4("PBM Receieved WMS session 0x%x TxId 0x%x PBM Last TxId 0x%x in_use_adn_pb 0x%x",
                 wms_resp_ptr->info_ptr.session_type, wms_resp_ptr->info_ptr.t_id,
                 pbm_session_info[pbm_session].last_txid,
                 pbm_session_info[pbm_session].in_use_adn_pb);

  if(pbm_session != PBM_SESSION_MAX &&
     pbm_session_info[pbm_session].last_txid == wms_resp_ptr->info_ptr.t_id)
  {
    PBM_SESSION_SET(pbm_wms_init_finished,pbm_session);
    if(pbm_session_subs_ready & PBM_SESSIONID_TO_SESSION_MASK(pbm_session))
    {
      if ( pbm_session_info[pbm_session].in_use_adn_pb != PBM_PB_NOT_KNOWN )
      {
        pbm_make_fileinfo_request(pbm_session,0);
      }
    }
  }
} /* pbm_wms_cfg_cb */
