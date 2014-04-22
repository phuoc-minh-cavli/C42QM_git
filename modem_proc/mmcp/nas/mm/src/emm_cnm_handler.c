
/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mm/src/emm_cnm_handler.c#1 $

when       who     what
--------   ---     ---------------------------------------------------------- 
12/26/11   rajesh   Allowing CSFB call to go through if establishment request is for CSVT 
10/01/11   abhi    Processing REL_REQ only if MO_CSFB call or SMS is active
08/22/11   MNK     CR: 301219 Clear CSFB context and call if aborted by the upper layer  
07/01/10   zren    Updated to include correct length for MMCNM_DATA_IND 

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>

#ifdef FEATURE_LTE

#include "msgr_nas.h"
#include "emm_cnm_handler.h"
#include "emm_service_request_lib.h"
#include "emm_utility.h"
#include "emm_rrc_if.h"
#include "stringl.h"
#include "emm_mem.h"
/*===========================================================================

FUNCTION EMM_SEND_MMCNM_EST_CNF

DESCRIPTION
  This function sends MMCNM_EST_CNF.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_send_mmcnm_est_cnf
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  mmcnm_est_cnf_T  mmcnm_est_cnf;

  memset((void *)&mmcnm_est_cnf, 0x0, sizeof(mmcnm_est_cnf_T));

  mmcnm_est_cnf.header.message_set = MS_CC_MM_PS;
  mmcnm_est_cnf.header.message_id  = MMCNM_EST_CNF;

  /*lint -save -e778 */
  PUT_IMH_LEN(sizeof(mmcnm_est_cnf_T) - sizeof(IMH_T), &mmcnm_est_cnf.header);
  /*lint -restore */

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending MMCNM_EST_CNF");
  
#ifdef DISABLE_9x05_OPTIMIZATIONS
  mm_send_message((IMH_T*)&mmcnm_est_cnf, GS_QUEUE_MN_CM);
#else
	  CNM_route_CNM_messages((IMH_T *)&mmcnm_est_cnf);
#endif

} /* emm_send_mmcnm_est_cnf */

/*===========================================================================

FUNCTION EMM_SEND_MMCNM_EST_REJ

DESCRIPTION
  This function sends MMCNM_EST_REJ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_send_mmcnm_est_rej
( 
  reject_type_enum_T   cause_type,
  byte                 cause_value,
  emm_ctrl_data_type  *emm_ctrl_data_ptr 
)
{
  mmcnm_est_rej_T  mmcnm_est_rej;

  memset((void *)&mmcnm_est_rej, 0x0, sizeof(mmcnm_est_rej_T));

  mmcnm_est_rej.header.message_set = MS_CC_MM_PS;
  mmcnm_est_rej.header.message_id  = MMCNM_EST_REJ;

  mmcnm_est_rej.cause_type = cause_type;
  mmcnm_est_rej.cause_value = cause_value;

#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
  mmcnm_est_rej.domain = MMCC_SMS_PS;
#endif

  /*lint -save -e778 */
  PUT_IMH_LEN(sizeof(mmcnm_est_rej_T) - sizeof(IMH_T), &mmcnm_est_rej.header);
  /*lint -restore */

  MSG_HIGH_DS_2(MM_SUB, "=EMM= Sending MMCNM_EST_REJ w/ type %u, value %d",
                cause_type,
                cause_value);

#ifdef DISABLE_9x05_OPTIMIZATIONS
  mm_send_message((IMH_T*)&mmcnm_est_rej, GS_QUEUE_MN_CM);
#else
		CNM_route_CNM_messages((IMH_T *)&mmcnm_est_rej);
#endif

  emm_ctrl_data_ptr->emm_sms_active = FALSE;

} /* emm_send_mmcnm_est_rej */

/*===========================================================================

FUNCTION EMM_SEND_MMCNM_REL_IND

DESCRIPTION
  This function sends MMCNM_REL_IND to CNM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_send_mmcnm_rel_ind
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr,
  lower_layer_cause_T  rel_cause  
)
{
  mmcnm_rel_ind_T  mmcnm_rel_ind;

  memset((void *)&mmcnm_rel_ind, 0x0, sizeof(mmcnm_rel_ind_T));

  mmcnm_rel_ind.header.message_set = MS_CC_MM_PS;
  mmcnm_rel_ind.header.message_id  = MMCNM_REL_IND;
  mmcnm_rel_ind.rel_cause = rel_cause;

  /*lint -save -e778 */
  PUT_IMH_LEN(sizeof(mmcnm_rel_ind_T) - sizeof(IMH_T), &mmcnm_rel_ind.header);
  /*lint -restore */

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending MMCNM_REL_IND");
  
#ifdef DISABLE_9x05_OPTIMIZATIONS
  mm_send_message((IMH_T*)&mmcnm_rel_ind, GS_QUEUE_MN_CM);
#else
	CNM_route_CNM_messages((IMH_T *)&mmcnm_rel_ind);
#endif

  emm_ctrl_data_ptr->emm_sms_active = FALSE;

} /* emm_send_mmcnm_rel_ind */

/*===========================================================================

FUNCTION EMM_PROCESS_MMCNM_REL_REQ

DESCRIPTION
  This function processes MMCNM_REL_REQ and will send MMCNM_REL_CNF if SMS is
  active on EMM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_process_mmcnm_rel_req
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  mmcnm_rel_cnf_T  mmcnm_rel_cnf;

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Rcved MMCNM_REL_REQ");

  /* Always send a cnf directly. 
     No need to tear down the connection. */
  memset((void *)&mmcnm_rel_cnf, 0x0, sizeof(mmcnm_rel_cnf_T));
  mmcnm_rel_cnf.header.message_set = MS_CC_MM;
  mmcnm_rel_cnf.header.message_id  = MMCNM_REL_CNF;

  /*lint -save -e778 */
  PUT_IMH_LEN(sizeof(mmcnm_rel_cnf_T) - sizeof(IMH_T), &mmcnm_rel_cnf.header);
  /*lint -restore */

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending MMCNM_REL_CNF");
  
#ifdef DISABLE_9x05_OPTIMIZATIONS
  mm_send_message((IMH_T*)&mmcnm_rel_cnf, GS_QUEUE_MN_CM);
#else
	  CNM_route_CNM_messages((IMH_T *)&mmcnm_rel_cnf);
#endif

  /* Reset flag */
  emm_ctrl_data_ptr->emm_sms_active = FALSE;

} /* emm_process_mmcnm_rel_req */

/*===========================================================================

FUNCTION EMM_PROCESS_MMCNM_EST_REQ

DESCRIPTION
  This function processes MMCNM_EST_REQ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_process_mmcnm_est_req
( 
  mm_cmd_type         *cc_msg_ptr, 
  emm_ctrl_data_type  *emm_ctrl_data_ptr 
)
{
  emm_csfb_rsp                     csfb_rsp;
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Processing MMCNM_EST_REQ");

  memset((void *)&csfb_rsp,0,sizeof(emm_csfb_rsp));

  if(((EMM_GET_STATE() == EMM_REGISTERED) && 
      (EMM_GET_SUBSTATE() == (emm_substate_type)EMM_REGISTERED_NORMAL_SERVICE))
     &&
     (emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                            SYS_SRV_STATUS_SRV)
     &&
     (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED)
     &&
     (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                         SYS_SIM_STATE_AVAILABLE)
     &&
     (emm_reg_container == NULL)
#ifdef FEATURE_LTE_REL11 
     &&
     ((emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL11) ||
      (mm_timer_status[TIMER_T3346] == TIMER_STOPPED)||
      (emm_check_high_priority(emm_ctrl_data_ptr))
#if !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

      ||
      (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE)
#endif
#endif
      )
#endif
       &&((emm_ctrl_data_ptr->aborting_conn == FALSE)&&
         (emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending == FALSE))
    )
  {
    emm_ctrl_data_ptr->emm_sms_active = TRUE;

	if(EMM_GET_CONNECTION_STATE() != EMM_CONNECTED_STATE)
    {
#ifdef FEATURE_CIOT
      if( emm_cp_opt_active( emm_ctrl_data_ptr ) == TRUE )
      {
        emm_send_mmcnm_est_cnf(emm_ctrl_data_ptr);
      }
      else
#endif
      if(emm_check_esr_for_ps(emm_ctrl_data_ptr) == TRUE)
      {
        /* Start extended service request procedure for PS */
        emm_start_nas_extended_service_request_procedure(LTE_RRC_EST_CAUSE_MO_DATA,LTE_PACKET_SERVICES_VIA_S1,
                                                         csfb_rsp
#if defined(FEATURE_LTE_TO_1X) || (defined(FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE))
                                                         ,NAS_EMM_CSFB_NONE
#endif
                                                         );
      }
      else
      {
        /* Start service request procedure with the cause of MO_DATA */
        emm_start_nas_service_request_procedure(emm_ctrl_data_ptr,
                                                LTE_RRC_EST_CAUSE_MO_DATA);
      }
    }
    else
    {
      /* Connection exists. Send est cnf directly. */
      emm_send_mmcnm_est_cnf(emm_ctrl_data_ptr);
    }
  }
  else if(EMM_GET_STATE() == EMM_TRACKING_AREA_UPDATING_INITIATED)
  {
      /* SMS could go through after TAU/SR procedure done. */
    emm_send_mmcnm_est_rej(EMM_REJECT_CAUSE, 
                           EMM_REJ_TEMP_STATE, 
                           emm_ctrl_data_ptr);
  }
  else
  {
    /* Invalid state. Reject est req. */
    emm_send_mmcnm_est_rej(MM_REJECT_CAUSE, 
                           MM_REJ_WRONG_STATE, 
                           emm_ctrl_data_ptr);
  }

} /* emm_process_mmcnm_est_req */

/*===========================================================================

FUNCTION EMM_PROCESS_CNM_COMMAND

DESCRIPTION
  This function processes commands from CNM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  emm_process_cnm_command
( 
  mm_cmd_type         *cnm_msg_ptr, 
  emm_ctrl_data_type  *emm_ctrl_data_ptr 
)
{
  emm_state_type emm_state = EMM_INVALID_STATE; 
  emm_substate_type emm_substate = EMM_UNKNOWN_STATE;
  boolean reject_call_during_plmn_search = FALSE;
  emm_state = EMM_GET_STATE();
  emm_substate = EMM_GET_SUBSTATE();
  

  if((cnm_msg_ptr->cmd.hdr.message_set != MS_CC_MM) &&
     (cnm_msg_ptr->cmd.hdr.message_set != MS_CC_MM_PS))
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= Can not handle message set %u",
                   cnm_msg_ptr->cmd.hdr.message_set);
    return;
  }

  MSG_HIGH_DS_3(MM_SUB, "=EMM= Rcved MS_CC_MM msg %d in srv domain %d sim state %d",
                cnm_msg_ptr->cmd.hdr.message_id,
                emm_ctrl_data_ptr->plmn_service_state->reg_service_domain,
                emm_ctrl_data_ptr->plmn_service_state->sim_state );

#ifdef FEATURE_CIOT
  MSG_HIGH_DS_1(MM_SUB, "=EMM= CP SMS status %d ",
                        emm_ctrl_data_ptr->plmn_service_state->cp_sms_service_status );
#ifdef FEATURE_LTE_REL14
  if ((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL14) &&  
      (emm_chk_nb1_rat() == TRUE) && 
      (cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type == SHORT_MESSAGE_SERVICE_ESTABLISHMENT) &&
      (mm_timer_status[TIMER_T3448] == TIMER_ACTIVE))
  {
     emm_send_mmcnm_est_rej(MM_REJECT_CAUSE, 
                              MM_REJ_WRONG_STATE,
                              emm_ctrl_data_ptr);
    return;
  }
  else
#endif
  if(cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type
                                        == SHORT_MESSAGE_SERVICE_ESTABLISHMENT &&
     (emm_ctrl_data_ptr->plmn_service_state->reg_service_domain != 
                                                          SYS_SRV_DOMAIN_CS_PS)
    )
  {
    if(emm_ctrl_data_ptr->plmn_service_state->cp_sms_service_status == SYS_CP_SMS_STATUS_NOT_AVAILABLE ||
       emm_ctrl_data_ptr->plmn_service_state->cp_sms_service_status == SYS_CP_SMS_STATUS_SERVICE_NOT_AVAILABLE_IN_PLMN)
    {
      emm_send_mmcnm_est_rej(EMM_REJECT_CAUSE, 
						   EMM_REJ_HARD_FAIL,
						   emm_ctrl_data_ptr);
    return;
    }
    else if(emm_ctrl_data_ptr->plmn_service_state->cp_sms_service_status == SYS_CP_SMS_STATUS_SERVICE_FAIL_DUE_TO_NW_FAILURE ||
	    emm_ctrl_data_ptr->plmn_service_state->cp_sms_service_status == SYS_CP_SMS_STATUS_SERVICE_FAIL_DUE_TO_CONGESTION ||
	    emm_ctrl_data_ptr->plmn_service_state->cp_sms_service_status == SYS_CP_SMS_STATUS_SERVICE_TEMP_FAILURE)
    {
     emm_send_mmcnm_est_rej(MM_REJECT_CAUSE, 
						MM_REJ_WRONG_STATE,
						emm_ctrl_data_ptr);
    return;
    } 
  } 
#endif
  if(emm_state == EMM_REGISTERED && emm_substate == EMM_REGISTERED_PLMN_SEARCH)
  {
    reject_call_during_plmn_search = TRUE;
  }
  switch(cnm_msg_ptr->cmd.hdr.message_id)
  {
    case MMCNM_EST_REQ:
      if(((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                          SYS_SRV_DOMAIN_CS_PS)
#ifdef FEATURE_CIOT
       || (emm_ctrl_data_ptr->plmn_service_state->cp_sms_service_status == SYS_CP_SMS_STATUS_AVAILABLE)
#endif
         )
         &&
         (cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type
                                       == SHORT_MESSAGE_SERVICE_ESTABLISHMENT))
      {
        emm_process_mmcnm_est_req(cnm_msg_ptr, emm_ctrl_data_ptr);
      }
#if defined (FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE)
#ifdef FEATURE_NAS_CSFB
      else if((((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                          SYS_SRV_DOMAIN_CS_PS)&&
      	         (reject_call_during_plmn_search == FALSE) &&  
                 (emm_state != EMM_REGISTERED_INITIATED) &&
                 (emm_state != EMM_DEREGISTERED_INITIATED))||
                (emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED)||
                ((mm_timer_status[TIMER_T3411] == TIMER_ACTIVE) &&
                (emm_ctrl_data_ptr->TIN != NAS_MM_TIN_GUTI))||
                (emm_ctrl_data_ptr->cell_barred_tau_pending==TRUE))&&
              (cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type
                                    == MO_CALL_ESTABLISHMENT  ||
               cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type 
                                    == MO_CALL_ESTABLISHMENT_HALF_SPEECH ||
               cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type 
                                   == MO_CALL_ESTABLISHMENT_FULL_SPEECH ||
               cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type 
                                  == EMERGENCY_CALL_ESTABLISHMENT ||
               cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type 
                                  == SUPPLEMENTARY_SERVICE_ACTIVATION ||
               cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type 
                                  == MO_CALL_ESTABLISHMENT_DATA_9600 ||
               cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type 
                                  == MO_CALL_ESTABLISHMENT_DATA_4800))
      {
        mm_csfb_start_mo_call_orig(cnm_msg_ptr);
      }
#endif
#endif
      else
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Rejecting MMCNM_EST_REQ w/ service type - %u",
                      cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type);
        if(cnm_msg_ptr->cmd.mmcnm_est_req.CNM_service_type 
                                  == EMERGENCY_CALL_ESTABLISHMENT)
        {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
          mm_send_mmcnm_est_rej( AS_REJECT_CAUSE, AS_REJ_LRRC_LOWER_LAYER_FAILURE , MMCC_SMS_CS);
#else
          mm_send_mmcnm_est_rej( AS_REJECT_CAUSE, AS_REJ_LRRC_LOWER_LAYER_FAILURE );
#endif
        }
        else if(emm_state == EMM_REGISTERED_INITIATED ||
                emm_state == EMM_DEREGISTERED_INITIATED)
        {
          emm_send_mmcnm_est_rej(MM_REJECT_CAUSE, 
                                 MM_REJ_WRONG_STATE,
                                 emm_ctrl_data_ptr);
        }
        else
        {
          emm_send_mmcnm_est_rej(MM_REJECT_CAUSE, 
                                 MM_REJ_NO_SERVICE,
                                 emm_ctrl_data_ptr);
        }
      }

      break;
    case MMCNM_DATA_REQ:
      if(((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                          SYS_SRV_DOMAIN_CS_PS)
#ifdef FEATURE_CIOT
       || (emm_ctrl_data_ptr->plmn_service_state->cp_sms_service_status == SYS_CP_SMS_STATUS_AVAILABLE)
#endif
         )
         &&
         ((cnm_msg_ptr->cmd.mmcnm_data_req.data[0] & 0x0F) == (byte)SMS_MESSGES) &&
         (emm_ctrl_data_ptr->rrc_active == TRUE))
      {
        emm_process_ul_sms(cnm_msg_ptr, emm_ctrl_data_ptr);
      }
      else
      {
        MSG_ERROR_DS_2(MM_SUB, "=EMM= Discarding MMCNM_DATA_REQ w/ PD %d / rrc_active %d",
                       cnm_msg_ptr->cmd.mmcnm_data_req.data[0] & 0x0F,
                       emm_ctrl_data_ptr->rrc_active);
      }
      break;
    case MMCNM_REL_REQ:
      emm_process_mmcnm_rel_req(emm_ctrl_data_ptr);
#if defined (FEATURE_NAS_GW) && !defined(FEATURE_NAS_DISABLE_VOICE)
      /* Remember the ESR procedure in case UE gets redirected
      ** to GSM later. This is to remeber if PS data needs to be resumed later */
      if((emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI) &&
         (mm_csfb_get_state() != MM_CSFB_STATE_NULL))
      {
        emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_ESR_STATE;
      }
      if(mm_csfb_get_state() == MM_CSFB_STATE_MO_CALL_ORIG)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= MO CSFB Call ended");
        mm_csfb_clear_csfb_call();
        /*set this flag to know if blind redirection indication needs to be sent to rr*/
        emm_ctrl_data_ptr->rr_blind_redir_pending_call_abort = TRUE;
        /* Autonomously clean up the pended ESR messages and reset the ESR type and STOP the TIMER*/
        /* It is a conscious decision to let the ESR run even after the call is borted by the upper lyaer/user. If UE is redirected it will go to GW.
         If ESR fails, it will not be retransmitted or failure is indicated to the upper layers */
        emm_abort_and_clean_csfb_call(emm_ctrl_data_ptr);
      }
#endif
      break;
    default:
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Discarding MS_CC_MM msg - %d",
                     cnm_msg_ptr->cmd.hdr.message_id);
      break;
  }

} /* emm_process_cnm_command */

/*===========================================================================

FUNCTION EMM_PROCESS_DL_NAS_TRANSPORT

DESCRIPTION
  This function passes SMS carried by downlink nas transport message to CNM 
  SMS handler

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_dl_nas_transport
(
  lte_nas_emm_dl_nas_transport  *dl_nas_transport_ptr
)
{
  word  msg_len = 0;
  mmcnm_data_ind_T  mmcnm_data_ind;

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending MMCNM_DATA_IND w/ SMS rcved in NAS Transport msg");

#ifdef FEATURE_LTE_REL11 
  if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL11)
  {
    mm_stop_timer(TIMER_T3246);
  }
#endif

  /* Init */
  memset((void *)&mmcnm_data_ind, 0x0, sizeof(mmcnm_data_ind_T));

  mmcnm_data_ind.msg_head.IMH.message_set = MS_CC_MM_PS;
  mmcnm_data_ind.msg_head.IMH.message_id = MMCNM_DATA_IND;

  /* SMS format 
     Byte 1                     Protocol Discriminator + Transaction Identifier
     Byte 2                     Message Type
     Byte 3 - 251(Optional)     User data  */


  /*lint -save -e778 */
  PUT_IMH_LEN(dl_nas_transport_ptr->nas_msg_container.msg_len, 
              &mmcnm_data_ind.msg_head.IMH);
  /*lint -restore */

  if((dl_nas_transport_ptr->nas_msg_container.msg_len < EMM_SMS_DATA_OFFSET)||
     (dl_nas_transport_ptr->nas_msg_container.msg_len > MAX_OCTETS_L3_MESSAGE))
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= invalid msg leng - %d, DL NAS transport ignored",
                                 dl_nas_transport_ptr->nas_msg_container.msg_len);
    return;
  }

  msg_len = dl_nas_transport_ptr->nas_msg_container.msg_len - 
                                                           EMM_SMS_DATA_OFFSET;

  mmcnm_data_ind.msg_head.ti_pd = 
      dl_nas_transport_ptr->nas_msg_container.msg_payload[EMM_SMS_TIPD_OFFSET];

  mmcnm_data_ind.msg_head.type = 
      dl_nas_transport_ptr->nas_msg_container.msg_payload[EMM_SMS_TYPE_OFFSET];

  if(msg_len > 0)
  {
    if(msg_len <= (MAX_OCTETS_L3_MESSAGE - EMM_SMS_DATA_OFFSET))
    {
      memscpy((void *)&mmcnm_data_ind.data[0], 
              sizeof(mmcnm_data_ind.data),
             (void *)&dl_nas_transport_ptr->nas_msg_container.\
                                              msg_payload[EMM_SMS_DATA_OFFSET],
             msg_len);
    }
    else
    {
      MSG_ERROR_DS_1(MM_SUB, "=EMM= DL SMS is too long to be sent - %d", msg_len);
    }
  }
#ifdef FEATURE_DUAL_SIM
  /* Append AS ID */
  {
    uint2 length;
    GET_IMH_LEN(length, &mmcnm_data_ind.msg_head.IMH);
    mmcnm_data_ind.data[msg_len] = (byte) mm_sub_id;
    PUT_IMH_LEN((length + sizeof(byte)), &mmcnm_data_ind.msg_head.IMH);
  }
#endif
  
  /* stopping 3440 timer*/
#ifdef FEATURE_LTE_REL14
  if((emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL14) &&
     ((emm_is_attach_without_pdn_supported() == TRUE) ||
      (emm_cp_opt_active(emm_ctrl_data_ptr) == TRUE)))
     {
        mm_stop_timer(TIMER_T3440);
     }
#endif

#ifdef DISABLE_9x05_OPTIMIZATIONS
  mm_send_message((IMH_T*)&mmcnm_data_ind, GS_QUEUE_MN_CM);
#else
  CNM_route_CNM_messages((IMH_T*)&mmcnm_data_ind);
#endif


} /* emm_process_dl_nas_transport */

/*===========================================================================

FUNCTION EMM_PROCESS_UL_SMS

DESCRIPTION
  This function passes uplink SMS messages to NW

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_ul_sms
(
  mm_cmd_type         *cnm_msg_ptr, 
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  emm_state_type                 emm_state;
  emm_substate_type              emm_substate;
  emm_connection_state_type      emm_conn_state;
  lte_nas_emm_ul_nas_transport  *ul_nas_transport_ptr = NULL;
  word msg_len;
#ifdef FEATURE_CIOT
  lte_nas_msg_container_type    *nas_msg_container_ptr = NULL;
#endif
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending UL NAS Transport msg carrying SMS");

  if (mm_get_ulog_handle() != NULL)
  {
    ULOG_RT_PRINTF_0(mm_get_ulog_handle(), "=EMM= Sending UL NAS Transport msg carrying SMS ");
  }
  /* Init */
  emm_state = EMM_GET_STATE();
  emm_substate = EMM_GET_SUBSTATE();
  emm_conn_state = EMM_GET_CONNECTION_STATE();

  GET_IMH_LEN(msg_len,&cnm_msg_ptr->cmd.hdr);

  if(msg_len > MAX_OCTETS_L3_MESSAGE)
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= SMS is too long to be sent in UL NAS Transport msg - %d(byte)",msg_len);
    return;
  }

  switch(emm_state)
  {  
    case EMM_REGISTERED:
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
      if(((emm_state == EMM_REGISTERED && emm_substate == (emm_substate_type)EMM_REGISTERED_NORMAL_SERVICE ) ||
          (emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED))&&         
         (emm_ctrl_data_ptr->emm_update_status == EMM_STATUS_UPDATED) &&
         (emm_ctrl_data_ptr->plmn_service_state->service_status == 
                                                    SYS_SRV_STATUS_SRV) &&
         (emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                      SYS_SIM_STATE_AVAILABLE)
#ifdef FEATURE_LTE_REL11
          &&
         ((emm_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL11) 
          ||
          ((mm_timer_status[TIMER_T3346] == TIMER_STOPPED) ||
          (emm_check_high_priority(emm_ctrl_data_ptr))
#if !defined(FEATURE_NAS_IMS_VOICE_DISABLED)
#ifdef FEATURE_NAS_E911_ENABLE

           ||
          (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE)
#endif
#endif
          ))
#endif
        && (mm_timer_status[TIMER_T3440] == TIMER_STOPPED))
      {
        if(emm_conn_state == EMM_CONNECTED_STATE)
        {
          memset((void *)emm_ctrl_data_ptr->out_msg_ptr,
                 0x0,
                 sizeof(lte_nas_outgoing_msg_type));

           ul_nas_transport_ptr = &emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.ul_nas_transport;

           /* Compose uplink nas transport message */
           ul_nas_transport_ptr->lte_nas_hdr.pd = EPS_MOBILITY_MANAGEMENT_MESSAGES;
           ul_nas_transport_ptr->lte_nas_hdr.msg_id = UPLINK_NAS_TRANSPORT;
           ul_nas_transport_ptr->security_hdr = PLAIN_NAS_MSG;
           ul_nas_transport_ptr->nas_msg_container.msg_len = msg_len;

           memscpy((void *)&ul_nas_transport_ptr->nas_msg_container.msg_payload[0],
                    sizeof(ul_nas_transport_ptr->nas_msg_container.msg_payload),
                    (void *)&cnm_msg_ptr->cmd.mmcnm_data_req.data[0],
                    ul_nas_transport_ptr->nas_msg_container.msg_len);

           (void)emm_send_message( emm_ctrl_data_ptr->out_msg_ptr, 
                                  emm_get_nas_transaction_id(), 
                                  MSGR_NAS_EMM,
                                  NULL,
                                  NULL,
                                  emm_ctrl_data_ptr);
        }
#ifdef FEATURE_CIOT
        else 
        {
           /* Allocate memory */
          nas_msg_container_ptr = emm_modem_mem_alloc(sizeof(lte_nas_msg_container_type),emm_ctrl_data_ptr);
          mm_check_for_null_ptr((void*)nas_msg_container_ptr);
          nas_msg_container_ptr->msg_len = msg_len; 
          memscpy((void *)&nas_msg_container_ptr->msg_payload[0], 
                  msg_len,
                  (void *)&cnm_msg_ptr->cmd.mmcnm_data_req.data[0],
                  msg_len);
          emm_start_nas_cp_service_request_procedure(LTE_RRC_EST_CAUSE_MO_DATA,NULL,nas_msg_container_ptr,FALSE);
          /* Free momery */
          emm_modem_mem_free(nas_msg_container_ptr, emm_ctrl_data_ptr);
        }
#endif
      }
      else
      {
        MSG_HIGH_DS_4(MM_SUB, "=EMM= Discarding UL SMS in REGISTERED state - %u, %u, %u, %u",
                 emm_state,
                 emm_conn_state, 
                 emm_substate, 
                 emm_ctrl_data_ptr->emm_update_status);
      }
      break ;
    default:
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Discarding UL SMS in EMM state - %u", emm_state);
      break;
  }
} /* emm_process_ul_sms */
#endif /*FEATURE_LTE*/
