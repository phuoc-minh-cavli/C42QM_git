/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                P B M E C C _ U T I L S.C

GENERAL DESCRIPTION
  This file contains the PBM wrapper functions for GSDI

  Copyright (c) 2016, 2018 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE
$Header: //components/rel/uim.mpss/3.3.1/pbm/src/pbmecc_utils.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/18    tq     Deprecated MMGSDI_TERMINAL_PROFILE_DL_EVT and MMGSDI_CARD_REMOVED_EVT 
07/05/16   sp      Fix compilation error
06/23/16   nr      ECC optimizations for IoE devices
05/19/16   nr      Logging improvements
05/11/16   sp      Fix low level compiler warnings
05/09/16   sp      Fix compiler warnings
02/03/16   nr      Initial Revision
===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "pbm_common_types.h"
#include "pbm.h"
#include "string.h"
#include "pbmgsdi.h"
#include "pbmutils.h"
#include "pbmtask_i.h"
#include "pbm_conversion.h"
#include "pbmuim.h"
#include "atomic_ops.h"
#include "stringl/stringl.h"
#include "pbmlib.h"
#include "cmutil.h"
#include "uim_msg.h"
#include "uim_pbm.h"
#include "pbmefs.h"
#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "pbm_nv_refresh.h"
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
/*===========================================================================
                         DEFINITIONS
===========================================================================*/
/* Global array to store the card status 
 * pbm_card_status set --- In card inserted event 
 * pbm_card_status Reset -- In card error event
 */
boolean       pbm_card_status[PBM_MAX_SLOTS];

atomic_word_t pbm_pin1_verified; /* to know if PIN1 is verified for particular session */
atomic_word_t pbm_pin2_verified; /* to know if PIN2 is verified for particular session */


/*===========================================================================
FUNCTION pbm_session_deactivate_handling

DESCRIPTION
This stub function handles session deactivation event from MMGSDI.

RETURN VALUE
  void.

DEPENDENCIES

===========================================================================*/
void pbm_session_deactivate_handling(
  pbm_session_enum_type deact_session_type,
  uint16                slot_id
)
{
  pbm_session_enum_type  deact_session_complement = PBM_SESSION_DEFAULT;
  pbm_session_enum_type  card_session             = PBM_SESSION_DEFAULT;

  pbm_stop_fileinfo_internal(deact_session_type);
  
  /* get session_index for GPB for same (slot) first */
  card_session = pbm_pb_type_and_slot_to_session_type(
                                          pbm_session_info[deact_session_type].slot_id,
                                          PBM_GPB);

  deact_session_complement = pbm_find_provioning_sesssion_in_queue(deact_session_type);

  /* Initlize ECC NV per card session only if no provisiong is activated in a slot */
  /* Skip ECC ready indication if an card error received before session de-activation */
  if(deact_session_complement == PBM_SESSION_MAX &&
     pbm_card_status[slot_id - 1])
  {
    pbm_build_slot_specific_ecc_cache(card_session, EMERGENCY_GW_1X, 
                                      PBM_NV_EMERGENCY_NUMBERS_PER_SUB);
  }
  
  /* Clearing slot info as well */
  pbm_session_info[deact_session_type].slot_id  = 0 ;
}/* pbm_session_deactivate_handling */


/*===========================================================================
FUNCTION PBM_CARD_STATUS_CB

DESCRIPTION
  Callback stub function to handle MMGSDI events.

RETURN VALUE
  void.

DEPENDENCIES
  None
===========================================================================*/
void pbm_card_status_cb(
  mmgsdi_event_data_type   mmgsdi_event
)
{
  pbm_slot_type                   slot         = 0;
  int                             index        = 0;
  pbm_session_enum_type           session_type = PBM_SESSION_DEFAULT;

  session_type = pbm_mmgsdi_session_to_pbm_session(mmgsdi_event.session_id);

  switch (mmgsdi_event.evt)
  {
    case MMGSDI_CARD_INSERTED_EVT:
    {
      pbm_session_enum_type card_sess = PBM_SESSION_DEFAULT ;

      slot = PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_event.data.card_inserted.slot);
      if(slot <= 0 || slot > PBM_MAX_SLOTS)
      {
        UIM_MSG_ERR_1("PBM MMGSDI_CARD_INSERTED_EVT invalid slot %d ",slot);
        break;
      }

      pbm_card_status[slot - 1] = TRUE;
      UIM_MSG_HIGH_2("PBM received MMGSDI_CARD_INSERTED_EVT for slot %d pbm_card_status %d",
                      slot, pbm_card_status[slot - 1]);

      card_sess = pbm_pb_type_and_slot_to_session_type(slot,PBM_GPB);

      rex_enter_crit_sect(&pbm_crit_sect);

      pbm_session_info[card_sess].slot_id = (uint16)slot ; 
      for (index = 0; index < (int) MAX_UIM_APPS; index++ )
      {
        /* clear caches in all sessions present on this slot */
        if ((slot == (pbm_slot_type)pbm_session_info[index].slot_id ) &&
              !(pbm_session_lpm_status & PBM_SESSIONID_TO_SESSION_MASK(index)))
        {
          pbm_ecc_clear_all_id((pbm_session_enum_type)index,
                                 PBM_FIELD_SIM_ECC);
          pbm_ecc_clear_all_id((pbm_session_enum_type)index,
                                  PBM_FIELD_NETWORK_ECC);
        }
      }

      rex_leave_crit_sect(&pbm_crit_sect);

      for (index = 0; index < (int) MAX_UIM_APPS; index++ )
      {
        if((slot == (pbm_slot_type)pbm_session_info[index].slot_id ) &&
            !(pbm_session_lpm_status & PBM_SESSIONID_TO_SESSION_MASK(index)))
        {
          /* Initlize ECC NV per SUB */
          pbm_build_slot_specific_ecc_cache(index, EMERGENCY_GW_1X, 
                                            PBM_NV_EMERGENCY_NUMBERS_PER_SUB);

          /* need to set hardcoded eccs as if card is present, and LPM mode is not ON */
          pbm_ecc_set_hardcoded_eccs(TRUE);
          break;
        }
      }

      for (index = 0; index < (int) MAX_UIM_APPS; index++ )
      {
        if((slot == (pbm_slot_type)pbm_session_info[index].slot_id ) &&
            !(pbm_session_lpm_status & PBM_SESSIONID_TO_SESSION_MASK(index)))
        {
          PBM_SET_PB_CACHE_READY(&ecc_other_cache); /* cache ready for hardcoded #s */
          break;
        }
      }
    }
    break;

    case MMGSDI_ECC_EVT:
    {
      pbm_session_enum_type          pbm_card_session = PBM_SESSION_GPB_1;

      UIM_MSG_HIGH_3("PBM received MMGSDI_ECC_EVT slot %d for session %d, num ecc %d.",
                      mmgsdi_event.data.ecc.slot,
                      session_type,
                      mmgsdi_event.data.ecc.ecc_count);

      rex_enter_crit_sect(&pbm_crit_sect);

      /* Get session_index for GPB for same (slot) first */
      pbm_card_session = pbm_pb_type_and_slot_to_session_type(
                                   pbm_session_info[session_type].slot_id,
                                   PBM_GPB);

      /* clear NV ecc per sub no's if any */
      pbm_ecc_clear_all_id(pbm_card_session, PBM_FIELD_SIM_ECC);

	  rex_leave_crit_sect(&pbm_crit_sect);

      /* Make sure we free the memory which was allocated in deep copy in the below function.*/
      pbm_update_sim_ecc_cache(session_type,&mmgsdi_event.data.ecc);
    }
    break;

    case MMGSDI_PIN1_EVT:
    {
      boolean                found_another = FALSE;
      uint8                  l_i;
      pbm_session_enum_type  card_session = PBM_SESSION_GPB_1;
      atomic_word_t          pin1_prev_status; 

      pin1_prev_status.value  = pbm_pin1_verified.value ; 

      UIM_MSG_HIGH_3("PBM received MMGSDI_PIN1_EVT with status  %d for session %d, for pin %d.",
                      mmgsdi_event.data.pin.pin_info.status,
                      session_type,
                       mmgsdi_event.data.pin.pin_info.pin_id);
      if( pbm_session_type_to_pb_category(session_type) != PBM_GPB  )
      {
        UIM_MSG_ERR_2("PIN app_type %d for slot %d",
                       mmgsdi_event.data.pin.aid_type[0].app_type,
                       mmgsdi_event.data.pin.slot);
        pbm_session_info[session_type].app_type = mmgsdi_event.data.pin.aid_type[0].app_type;
	    pbm_session_info[session_type].slot_id = mmgsdi_event.data.pin.slot;
      }

      PBM_VALIDATE_SESSION_RET(session_type, VOID);

      /* since PIN events would come only for PROV sessions
       * we need to take the appropriate action on
       * the card session that belonged to this prov session
       */
      switch( mmgsdi_event.data.pin.slot )
      {
        case MMGSDI_SLOT_1 :
          card_session = PBM_SESSION_GPB_1;
          break;

        case MMGSDI_SLOT_2 :
          card_session = PBM_SESSION_GPB_2;
          break;

#ifdef FEATURE_TRIPLE_SIM
        case MMGSDI_SLOT_3 :
          card_session = PBM_SESSION_GPB_3;
          break;
#endif /* FEATURE_TRIPLE_SIM */
        default :
          break;
      }

      for(l_i = (uint8)PBM_SESSION_GPB_1;l_i < (uint8)PBM_SESSION_MAX; l_i++)
      {
        /* check if there exists 2nd provisioning session on the same slot
         * if not we should clear the appropriate card session as well
         */
        if(pbm_session_type_to_pb_category((pbm_session_enum_type)l_i) == PBM_LPB && l_i != (uint8)session_type)
        {
          if(pbm_session_info[l_i].isActive == TRUE &&
             pbm_session_info[l_i].slot_id == (uint16) PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_event.data.pin.slot))
            found_another = TRUE;
        }
      }

      switch ( mmgsdi_event.data.pin.pin_info.status )
      {
        case  MMGSDI_PIN_PERM_BLOCKED :
        {
          PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, session_type);
          if(!found_another)
          {
            PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, card_session);
          }
        }
        break;

        case MMGSDI_PIN_ENABLED_NOT_VERIFIED :
        {
          PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, session_type);
          if(!found_another)
          {
            PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, card_session);
          }
        }
        break;

        /* there can be 2 possibilities for event MMGSDI_PIN_ENABLED_VERIFIED
         * 1 - Earlier PIN is enabled and not verified , now it is verified
         * 2 - SIM doesnt need PIN verification and now PIN is
         * enabled to prompt for verification in the next powerup
         * for case 2 we dont need to take any action since not applicable for PBM
         */
        case MMGSDI_PIN_ENABLED_VERIFIED :
        {
          PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, session_type);

		  /* in case of enabled verified no need to check other  app's status for pin1_verified on same slot */
          PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, card_session);
        }
        break;

        case MMGSDI_PIN_BLOCKED :
        {
          PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, session_type);
          if(!found_another)
          {
            PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, card_session);
          }
        }
        break;

        case MMGSDI_PIN_UNBLOCKED :
        {
          PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, session_type);

		  /* in case of pin unblocked no need to check other  app's status for pin1_verified on same slot */
          PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, card_session);
        }
        break;

        case MMGSDI_PIN_DISABLED :
        {
          PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, session_type);
          PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, card_session);
        }
        break;

        case MMGSDI_PIN_CHANGED :
        {
          /* changed is equivalent to verified case */
          PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, session_type);
          PBM_SESSION_ATOMIC_SET(pbm_pin1_verified, card_session);
        }
        break;

        default :
        break;
      }

      if( pbm_populate_ecc_other_if_pin1_status_change(pin1_prev_status) ) 
      {
        /* if hardcoded list is updated send ready */
        PBM_SET_PB_CACHE_READY(&ecc_other_cache);
      }
    }
    break;

    case MMGSDI_CARD_ERROR_EVT:
    {
      uint8                           ecc_sim_airplane         = 0;
      boolean                         card_present_on_any_slot = FALSE;
      uim_common_efs_status_enum_type efs_status               = UIM_COMMON_EFS_ERROR;
      atomic_word_t                   pin1_prev_status;

	  pin1_prev_status.value   = pbm_pin1_verified.value ; 

      efs_status = uim_common_efs_read(UIM_COMMON_EFS_PBM_SIM_ECC_AIRPLANE,
                                       UIM_COMMON_EFS_ITEM_FILE_TYPE,
                                       UIM_COMMON_EFS_DEVICE,
                                       &ecc_sim_airplane,
                                       (uint32)sizeof(ecc_sim_airplane));
            
      if(efs_status != UIM_COMMON_EFS_SUCCESS)
      {
        UIM_MSG_MED_0("Failed reading pbm_sim_ecc_airplane - pick default");
      }

      slot = PBM_MMGSDI_SLOT_TO_PBM_SLOT(mmgsdi_event.data.card_error.slot);
      PBM_VALIDATE_SLOT_RET(slot, VOID);

      pbm_card_status[slot - 1] = FALSE;
      UIM_MSG_HIGH_3("PBM received MMGSDI_CARD_ERROR_EVT for slot 0x%x, pbm_card_status 0x%x ecc_sim_airplane nv 0x%x",
                     slot, pbm_card_status[slot - 1], ecc_sim_airplane);

      for (index = 0; index < (int) MAX_UIM_APPS; index++ )
      {
        /* clear caches in all sessions present on this slot */
        if ( slot == (pbm_slot_type)pbm_session_info[index].slot_id )
        {
          pbm_stop_fileinfo_internal((pbm_session_enum_type)index);

          rex_enter_crit_sect(&pbm_crit_sect);

		  pbm_session_info[index].isActive = FALSE;
          if ( ecc_sim_airplane && (mmgsdi_event.evt == MMGSDI_CARD_ERROR_EVT) && 
               (mmgsdi_event.data.card_error.info == MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY ))
          {
            PBM_SESSION_SET(pbm_session_lpm_status, index);
          }
          else
          {
            PBM_SESSION_RESET(pbm_session_lpm_status, index);
            pbm_ecc_clear_all_id((pbm_session_enum_type)index, PBM_FIELD_SIM_ECC);
            pbm_ecc_clear_all_id((pbm_session_enum_type)index, PBM_FIELD_NETWORK_ECC);
          }

          pbm_session_info[index].slot_id  = 0 ;
          rex_leave_crit_sect(&pbm_crit_sect);
        }
      }

      if(pbm_card_status[PBM_SLOT_1 - 1] == TRUE || 
         pbm_card_status[PBM_SLOT_2 - 1] == TRUE || pbm_card_status[PBM_SLOT_3 - 1] == TRUE)
      {
        card_present_on_any_slot = TRUE;
        UIM_MSG_HIGH_1("PBM Card is present in slot1 or slot2 or slot3 %d",
                        card_present_on_any_slot);
      }

      if ( !card_present_on_any_slot && 
		   (!ecc_sim_airplane || (mmgsdi_event.evt != MMGSDI_CARD_ERROR_EVT) || 
           (mmgsdi_event.data.card_error.info != MMGSDI_CARD_ERR_PWR_DN_CMD_NOTIFY) ))
      {
        pbm_ecc_set_hardcoded_eccs(FALSE);
        PBM_SET_PB_CACHE_READY(&ecc_other_cache);
      }
      else if ( card_present_on_any_slot )
      {
        if ( pbm_populate_ecc_other_if_pin1_status_change(pin1_prev_status) ) 
        {
          PBM_SET_PB_CACHE_READY(&ecc_other_cache);
        }
      }
    }
    break;

    case MMGSDI_SESSION_CHANGED_EVT:
    {
      mmgsdi_app_info_type     app_info          = mmgsdi_event.data.session_changed.app_info;
      PB_CACHE                *ecc_cache         = NULL;
      atomic_word_t            pin1_prev_status;

      pin1_prev_status.value = pbm_pin1_verified.value ; 

      UIM_MSG_HIGH_3("PBM received MMGSDI_SESSION_CHANGED_EVT event on session %d app_type %d activated Status %d",
                      session_type, app_info.app_data.app_type,mmgsdi_event.data.session_changed.activated);
      if( pbm_session_type_to_pb_category(session_type) != PBM_LPB )
      {
        UIM_MSG_ERR_1("Sess changed on incorrect session %d",session_type);
        break;
      }

      if(mmgsdi_event.data.session_changed.activated == TRUE)
      {
        if( pbm_session_type_to_pb_category(session_type) != PBM_GPB )
        {
          pbm_session_info[session_type].app_type = app_info.app_data.app_type;
        }

        pbm_session_info[session_type].slot_id = (uint16) mmgsdi_event.data.session_changed.app_info.slot;
      }
      else
      {
        rex_enter_crit_sect(&pbm_crit_sect);

        /* clearing the card ECC and OTA numbers */
        pbm_ecc_clear_all_id(session_type, PBM_FIELD_SIM_ECC);
        pbm_ecc_clear_all_id(session_type, PBM_FIELD_NETWORK_ECC);

        rex_leave_crit_sect(&pbm_crit_sect);

        ecc_cache = pbm_file_id_to_cache(session_type, PBM_FILE_ECC);
        UIM_MSG_HIGH_1("PBM received MMGSDI_SESSION_CHANGED_EVT event on slot %d",
                        (uint16) mmgsdi_event.data.session_changed.app_info.slot);

        /* PBM session deactivate handling */
        pbm_session_deactivate_handling(session_type,pbm_session_info[session_type].slot_id);

        /* pin1 cleared in the above de-activate handling and hence cache building is to be after that */
       (void)pbm_populate_ecc_other_if_pin1_status_change(pin1_prev_status);

        /* Skip ECC ready indication if an card error received before session de-activation */
        if(pbm_card_status[PBM_SLOT_1 - 1] == TRUE || 
           pbm_card_status[PBM_SLOT_2 - 1] == TRUE || pbm_card_status[PBM_SLOT_3 - 1] == TRUE)
        {
          /* sending cache ready to clients - not checking for pin1_status_change has updated
             hardcoded list because this ready has to go irrespective of its update.
             Because network and card ecc are cleared out */
          PBM_SET_PB_CACHE_READY(ecc_cache);
        }
      }
      break;
    }

    default:
    break;
  }
}/* pbm_card_status_cb */


/*===========================================================================
FUNCTION PBM_MMGSDI_RESP_HANDLE

DESCRIPTION
  This stub function read the MMGSDI responses from the queue and handle it.
  Typically we do not have to keep the mmgsdi response data around, so it
  is freed immediately after processing.

DEPENDENCIES
  None

RETURN VALUE
  Void
  
SIDE EFFECTS
  None
===========================================================================*/
void pbm_mmgsdi_resp_handle(
  void
)
{
  pbm_cmd_s_type                       *cmd_ptr       = NULL;
  pbm_mmgsdi_resp_s_type               *mmgsdi_resp   = NULL;
  pbm_mmgsdi_card_status_s_type        *card_status   = NULL;

  while ((cmd_ptr = pbm_cmd_q_get(&pbm_gsdi_resp_q)) != NULL)
  {
    switch (cmd_ptr->cmd_type)
    {
      case PBM_CM_SS_CB:
      {
        pbm_cm_ss_evt_type *cm_ss_evt_ptr;

        PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_cm_ss_evt_type, break);
        cm_ss_evt_ptr = (pbm_cm_ss_evt_type*)cmd_ptr;
        pbm_handle_cm_ss_evt(cm_ss_evt_ptr);
        PBM_MEM_FREEIF(cm_ss_evt_ptr->ii_info_ptr);
      }
      break;

#ifdef FEATURE_MODEM_CONFIG_REFRESH
      case PBM_MCFG_REFRESH_CB:
      {
        pbm_handle_nv_refresh_req_type *pbm_nv_refresh_req_ptr = NULL;

        PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_handle_nv_refresh_req_type, break);
        pbm_nv_refresh_req_ptr = (pbm_handle_nv_refresh_req_type *)cmd_ptr;
        pbm_process_nv_refresh_req(pbm_nv_refresh_req_ptr);
      }
      break;
#endif /*FEATURE_MODEM_CONFIG_REFRESH*/            

      case PBM_MMGSDI_REG_STATUS_CB:
        PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
        mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);
        if (mmgsdi_resp->status == MMGSDI_SUCCESS)
        {
          if (mmgsdi_resp->data_type== MMGSDI_CLIENT_ID_AND_EVT_REG_CNF)
          {            
            pbm_mmgsdi_client_id =
                mmgsdi_resp->data.client_id_and_evt_reg_cnf.response_header.client_id;
            /* open sessions irrespective of knowing the SIM inserted or not */
            pbm_open_sessions();
          }
        } else
        {
          UIM_MSG_ERR_2("MMGSDI failed Status %d, Confirmation %d",
		  	 mmgsdi_resp->status, mmgsdi_resp->data_type);
        }
        break;

        case PBM_MMGSDI_CARD_STATUS_CB:
          /* MMGSDI Card status callback */
          PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_card_status_s_type, break);
          card_status = PBM_MMGSDI_CARDSTATUS_PTR(cmd_ptr);
          pbm_card_status_cb(card_status->mmgsdi_event);
          break;

        case PBM_MMGSDI_OPEN_SESSION_CB:
          PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_mmgsdi_resp_s_type, break);
          mmgsdi_resp = PBM_MMGSDI_RESP_PTR(cmd_ptr);
          pbm_open_session_cb(mmgsdi_resp->status,
                              mmgsdi_resp->data_type,
                              &mmgsdi_resp->data);
          break;

        default:
          UIM_MSG_ERR_1("Unknown command %d", cmd_ptr->cmd_type);
          break;
      }

      pbm_cmd_free(cmd_ptr);
   }
} /* pbm_mmgsdi_resp_handle */


/*===========================================================================
FUNCTION PBM_STOP_FILEINFO_INTERNAL

DESCRIPTION
    This is stub function for pbm_stop_fileinfo_internal ().

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pbm_stop_fileinfo_internal(
  pbm_session_enum_type    session_type
)
{
  PBM_VALIDATE_SESSION_RET(session_type, VOID);

  PBM_SESSION_ATOMIC_RESET(pbm_pin1_verified, session_type);
  PBM_SESSION_ATOMIC_RESET(pbm_pin2_verified, session_type);
} /* pbm_stop_fileinfo_internal */


/*===========================================================================
FUNCTION PBM_FILE_ID_TO_CACHE

DESCRIPTION
    This is stub function for pbm_file_id_to_cache ().

SIDE EFFECTS
===========================================================================*/
void *pbm_file_id_to_cache(
  pbm_session_enum_type   session_type,
  pbm_file_type           file_id
)
{
  PBM_VALIDATE_SESSION_RET(session_type, NULL);

  switch (file_id)
  {
    case PBM_FILE_ECC:
      return &ecc_sim_cache[session_type];

    default:
      UIM_MSG_ERR_1("Unknown file id %d", file_id);
      return NULL;
  }
} /* pbm_file_id_to_cache */


/*===========================================================================
FUNCTION PBM_DEVICE_ID_TO_CACHE

DESCRIPTION
    This is stub function for pbm_device_id_to_cache ().

SIDE EFFECTS
===========================================================================*/
PB_CACHE *pbm_device_id_to_cache(
  pbm_session_enum_type session_type,
  pbm_device_type       device_id
)
{
  PBM_VALIDATE_SESSION_RET(session_type, NULL);

  switch (device_id)
  {
    case PBM_ECC:
      return &ecc_sim_cache[session_type];

    case PBM_ECC_OTHER:
      return &ecc_other_cache;

    default:
      return NULL;
  }
} /* pbm_device_id_to_cache */

