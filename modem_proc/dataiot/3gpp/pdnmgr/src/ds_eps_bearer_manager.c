/*===========================================================================
                      DS_EPS_BEARER_MANAGER.C
DESCRIPTION
This file defines bearer manager for LTE APIs.

Copyright (c) 2018 - 2019 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_eps_bearer_manager.c#15 $
  $DateTime: 2023/06/23 03:58:24 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_pdn_manager.h"
#include "ds_bearer_manager.h"
#include "ds_eps_bearer_manager.h"
#include "ds_bearer_flow_manager.h"
#include "ds_eps_pdn_manager.h"
#include "ds_pdn_nv_manager.h"
#include "dsmsgr.h"
#include "ds_pdn_cp_data_hdlr.h"
#include "ds_pdn_apn_rate_ctrl.h"
#include "ds_pdn_profile_validator.h"
#include "ds_pdn_flow_manager.h"
#include "ds_fwki.h"
#include "ds_pdn_psm_hdlr.h"
#include "ds_3gpp_pdn_limit_hdlr.h"

#include "lte_pdcp_ext_api.h"
#include "nb1_pdcp_ext_api.h"
#include "dssysmgr.h"
#include "cm_gw.h"
#include "ULogFront.h"

#ifdef FEATURE_MODE_TLB
#include "ds_eps_tlb.h"
#endif  /* FEATURE_MODE_TLB */

#ifdef TEST_FRAMEWORK
#error code not present
#endif
/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_INIT

DESCRIPTION
  This function is used to initialize the EPS bearer manager. 

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_bearer_mgr_init( void )
{

  /*------------------------------------------------------------------------
    Register the PDCP MSGRTR for UL PDCP CNF
  ------------------------------------------------------------------------*/
  dsmsgrrcv_reg_hdlr(
    LTE_PDCPUL_RAB_REGISTER_CNF,
            (dsmsgrcv_msg_hdlr_f)ds_eps_bearer_mgr_pdcpul_rab_reg_cnf_msg_hdlr
  );
  
  /*------------------------------------------------------------------------
    Register the PDCP MSGRTR for DL PDCP CNF
  ------------------------------------------------------------------------*/
  dsmsgrrcv_reg_hdlr(
    LTE_PDCPDL_RAB_REGISTER_CNF,
            (dsmsgrcv_msg_hdlr_f)ds_eps_bearer_mgr_pdcpdl_rab_reg_cnf_msg_hdlr
  );
  
  /*------------------------------------------------------------------------
    Register the PDCP MSGRTR for UL PDCP  DEREG CNF
  ------------------------------------------------------------------------*/
  dsmsgrrcv_reg_hdlr(
    LTE_PDCPUL_RAB_DEREGISTER_CNF,
            (dsmsgrcv_msg_hdlr_f)ds_eps_bearer_mgr_pdcpul_rab_dereg_cnf_msg_hdlr
  );
  
  /*------------------------------------------------------------------------
    Register the PDCP MSGRTR for DL PDCP  DEREG CNF
  ------------------------------------------------------------------------*/
  dsmsgrrcv_reg_hdlr(
    LTE_PDCPDL_RAB_DEREGISTER_CNF,
            (dsmsgrcv_msg_hdlr_f)ds_eps_bearer_mgr_pdcpdl_rab_dereg_cnf_msg_hdlr
  );

  // TLB init

  return;
}

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_IS_ANY_BEARER_NOT_DOWN

DESCRIPTION
  This function checks if any of the bearer is not in DOWN state
  
PARAMETERS  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Returns TRUE if any bearer is not in down state
  FALSE - Otherwise
   
SIDE EFFECTS 
  None.  
===========================================================================*/
boolean ds_eps_bearer_mgr_is_any_bearer_not_down
(
  void
)
{
  uint8                    bearer_index = 0;
  ds_bearer_mgr_s         *bearer_mgr_p= NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( bearer_index = 0;bearer_index < DS_3GPP_MAX_BEARER_CONTEXT;
       bearer_index++)
  {
    bearer_mgr_p = ds_bearer_mgr_get_instance_ptr(bearer_index);

    if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      continue;
    }

    DS_MSG2(MSG_LEGACY_HIGH, "Bearer index:%d is in state:%d",
    	                        bearer_index, bearer_mgr_p->state);

    if( (bearer_mgr_p->state != DS_BEARER_MGR_STATE_DOWN) &&
        (bearer_mgr_p->state != DS_BEARER_MGR_STATE_PEND_REDIAL) )
    {
      return TRUE;
    }
  }

  return FALSE;
}/*ds_eps_bearer_mgr_is_any_bearer_not_down*/

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_REQ_DEACT_FOR_PENDING_BEARERS

DESCRIPTION
  This function initiates call end for bearers that are in COMING_UP state. 
  
PARAMETERS  
  None.
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Returns TRUE if call end was initiated with CM for any pending bearers.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
boolean ds_eps_bearer_mgr_req_deact_for_pending_bearers
(
  void
)
{
  ds_bearer_mgr_s         *bearer_mgr_p= NULL;
  uint8                    bearer_cntx_inst= 0;
  boolean                  any_pending_bearers = FALSE;
  ds_pdn_mgr_s            *pdn_mgr_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( bearer_cntx_inst=0;\
       bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;\
       bearer_cntx_inst++ )
  {
    bearer_mgr_p = ds_bearer_mgr_get_instance_ptr(bearer_cntx_inst);

    if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      continue;
    }

    if (bearer_mgr_p->state == DS_BEARER_MGR_STATE_COMING_UP)
    {   
      DS_MSG1(MSG_LEGACY_HIGH,
      	       "Bearer_inst : %d,in COMING_UP state, ending call",
              bearer_cntx_inst);        

      pdn_mgr_p = (ds_pdn_mgr_s*)(bearer_mgr_p->pdn_mgr_ptr);
      ds_bearer_mgr_tear_down_bearer(pdn_mgr_p, bearer_mgr_p);

      /*---------------------------------------------------------------------
        If bearer was locally deactivated in DS then state will be DOWN and
        not GOING_DOWN. We do not redial calls for this local deactivation
        in DS scenario
      ---------------------------------------------------------------------*/ 
      if(bearer_mgr_p->state == DS_BEARER_MGR_STATE_GOING_DOWN )
      {
        any_pending_bearers = TRUE;

        /* we only support silent redial on default pdn*/        
        if ((bearer_mgr_p->is_default == TRUE) && 
            (pdn_mgr_p->is_default == TRUE) &&
            (ds_eps_is_silent_redial_allowed(pdn_mgr_p)))
        {
          bearer_mgr_p->state = DS_BEARER_MGR_STATE_PEND_REDIAL;
          DS_MSG0(MSG_LEGACY_HIGH,"Moving bearer to pending redial state");
        }
      }
    }
  /*---------------------------------------------------------------------
     If bearer is in going down state cache attach SM state so that we can start 
     attach procedure after ending this call successfully
   ---------------------------------------------------------------------*/
    if (bearer_mgr_p->state == DS_BEARER_MGR_STATE_GOING_DOWN) 
    {
      DS_MSG1(MSG_LEGACY_HIGH, 
      	       "Bearer %d is in going down state, return pending bearers TRUE",
              bearer_cntx_inst);
      any_pending_bearers = TRUE;
    }
  }
  
  return any_pending_bearers;
} /* ds_bearer_cntxt_req_deact_for_pending_bearers */


/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_BRING_UP_PDN

DESCRIPTION
  This function initiates call bring up for LTE.
  
PARAMETERS  
  bearer ptr
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
boolean ds_eps_bearer_mgr_bring_up_pdn
(
  ds_bearer_mgr_s   *bearer_mgr_p
)
{
  ds_pdn_mgr_s               *pdn_mgr_p = NULL;
  ds_pdn_mgr_call_info_type   call_info = {0};
  uint8                       index = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
  /* Get the PDN context */
  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;

  // PDN limit handling
  if(ds_3gpp_pdn_limit_is_enabled() && 
     ds_eps_pdn_attach_sm_get_state() == DS_EPS_ATTACHED)
  {
    if((index = ds_3gpp_pdn_limit_tbl_find_entry
                   ((byte *)pdn_mgr_p->pdp_profile->context.apn))
                                     != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
    {
    /*---------------------------------------------------------------------------
        Start the PDN Connection Timer if it has not already started.
      ---------------------------------------------------------------------------*/
      if(ds_3gpp_pdn_limit_get_current_pdn_conn_tmr_val(index) == 0)
      {
        ds_3gpp_pdn_limit_start_pdn_conn_tmr(index);
      }
    }
  }
     
  call_info.attach_wo_pdn = FALSE;
  call_info.call_instance = bearer_mgr_p->index;
  call_info.low_priority_signaling = 
      	           ds_pdn_mgr_get_low_priority_signaling_bit(pdn_mgr_p);

  return ds_eps_initiate_pdn_connection(&call_info); 
}

/*===========================================================================
FUNCTION DS_EPS_BEARER_GET_ORIG_PARAMS

DESCRIPTION
  This function retrives the origination parameters for EPS bearer.
  
PARAMETERS  
  bearer ptr
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_eps_bearer_get_orig_params
(
  ds_bearer_mgr_s             *bearer_mgr_p,
  emm_pdn_connectivity_req_s_type *lte_orig_params
)
{
  bearer_mgr_p->sdf_id = lte_orig_params->esm_hdr.sdf_id;
  
  ds_pdn_cp_data_populate_pdn_conn_params(bearer_mgr_p,
                                          lte_orig_params);
  return;
}

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_TX_UM_DATA_CB

DESCRIPTION
  This function forwards pkts received to the lower layers for
  pdp-ip call.
  
PARAMETERS  
  Bearer ptr 
  DSM item ptr
  meta_info_ptr  - PS meta info ptr
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_eps_bearer_mgr_tx_um_data_cb
(
  ds_bearer_mgr_s    *bearer_mgr_p,
  dsm_item_type     **item_ptr,
  ps_meta_info_type  *meta_info_ptr
)
{
  ds_bearer_mgr_wm_s                  *tx_watermark_ptr;
  ds_bearer_mgr_tput_opt_struct_type   tput_opt_params;
  ds_apn_rate_ctrl_context_s          *apn_rate_ctrl_info_p = NULL;
  int                                  apn_index = -1; 
  boolean                              is_mox_data_pkt = FALSE;
  rex_crit_sect_type                  *ul_data_crit_sect;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ul_data_crit_sect = ds_bearer_mgr_get_ul_data_crit_sect();

  rex_enter_crit_sect(ul_data_crit_sect);
  
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    (void)dsm_free_packet(item_ptr);
    rex_leave_crit_sect(ul_data_crit_sect);  
    return;
  }

  /*-------------------------------------------------------------------------
    Get PDCP state information for call.
  -------------------------------------------------------------------------*/
  tx_watermark_ptr = &(bearer_mgr_p->tx_wm);

  tput_opt_params = ds_pdn_nv_get_back_pressure_params();  

  /*-------------------------------------------------------------------------
    Check for uplink Acks and increment the ack counter
  -------------------------------------------------------------------------*/
  if ( (DSM_HIGHEST == ((*item_ptr)->priority)) &&
       (TRUE == tput_opt_params.back_pressure_removal) )
  {
    bearer_mgr_p->tcp_dl_ack_counter++;

    /*-------------------------------------------------------------------------
      Start the BP timer on getting the first ACK, but do not disable BP. Once
      the timer expires, based on the current ack cnt, BP would be disabled.
    -------------------------------------------------------------------------*/
    if (bearer_mgr_p->tcp_dl_ack_counter == 1)
    {
      ds_bearer_mgr_start_bp_timer(bearer_mgr_p);
    }
  }

  (*item_ptr)->link.next_ptr = NULL;
#ifdef FEATURE_NB1
  if (SYS_RAT_LTE_NB1_RADIO_ACCESS == ds_pdn_mgr_get_active_rat() 
#ifdef FEATURE_NBIOT_NTN
      || SYS_RAT_LTE_NB1_NTN_RADIO_ACCESS == ds_pdn_mgr_get_active_rat()
#endif /* FEATURE_NBIOT_NTN */
)
  {
    nb1_pdcpul_tag_each_pkt(*item_ptr);
  }
  else
  {
    lte_pdcpul_tag_each_pkt(*item_ptr);
  }
#else
  lte_pdcpul_tag_each_pkt(*item_ptr);
#endif /* FEATURE_NB1 */

  is_mox_data_pkt = (meta_info_ptr != NULL) &&
     (PS_TX_META_GET_FILTER_RESULT( meta_info_ptr, FLTR_CLIENT_MO_EXCP));

#ifdef TEST_FRAMEWORK
  #error code not present
#endif
     	
  if(is_mox_data_pkt)
  {
    dsm_item_set_priority(*item_ptr, DSM_ENQ_HIGHEST_PRIORITY);

    (*item_ptr)->app_field = DS_PDN_MO_EXCEPTIONAL_DATA;

    if (bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT) 
    {
#ifdef FEATURE_GPS_WWAN_PRIORITY_SEARCH
      if(dssysmgr_get_gps_search_status())
      {
        cm_ph_cmd_terminate_get_networks(NULL,NULL,CM_CLIENT_TYPE_DS);
      }
#endif /* FEATURE_GPS_WWAN_PRIORITY_SEARCH */
      ds_pdn_cp_data_increment_num_mo_ex_data_cnt(bearer_mgr_p->index);
    }
  }
  
  if( bearer_mgr_p->is_in_tlb && 
  	  (*item_ptr)->app_field == DS_TLB_MODE_B_I )
  {
    (void)dsm_free_packet(item_ptr);
    rex_leave_crit_sect(ul_data_crit_sect);  
    return;
  }
  
  apn_index = bearer_mgr_p->apn_index;
  if (apn_index == -1)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Packet enqueued to Tx WM !");
    dsm_enqueue(&tx_watermark_ptr->wm_item, item_ptr);  
  }
  else
  {
    apn_rate_ctrl_info_p = 
         ds_pdn_apn_rate_ctrl_get_instance_from_apn_index(apn_index);

    if(!ds_pdn_apn_rate_ctrl_drop_data_packet(meta_info_ptr,
                                               bearer_mgr_p,
                                             apn_rate_ctrl_info_p,
                                             is_mox_data_pkt))
    {      
      DS_MSG0(MSG_LEGACY_HIGH,"Packet enqueued to Tx WM !");
      dsm_enqueue(&tx_watermark_ptr->wm_item, item_ptr);  

      /*-----------------------------------------------------------------------
         Increment the number of packets sent on this bearer's APN for 
         APN Rate Control Info except when 
         1.  APN Rate Control does not apply to MO Exception Data, and 
         2.  Data enqueued is MO Exception Data 
      -----------------------------------------------------------------------*/

      
      if (apn_rate_ctrl_info_p != NULL)
      {
        
        if((TRUE == apn_rate_ctrl_info_p->rate_ctrl_info.threshold_exceeded) &&
             (TRUE == is_mox_data_pkt) &&
               (FALSE == apn_rate_ctrl_info_p->config.applies_to_mo_ex))
        {
          if(TRUE == apn_rate_ctrl_info_p->additional_rate_control_config.enabled)//Increment REL 14 APN Rate Control Packet Counter
          {           
            if (!ds_pdn_rate_ctrl_is_interval_timer_running(
                                           &(apn_rate_ctrl_info_p->additional_rate_ctrl_info)))
            {
              ds_pdn_rate_ctrl_start_interval_timer(
                                             &(apn_rate_ctrl_info_p->additional_rate_ctrl_info));
            }
ds_pdn_rate_ctrl_increment_counter(&(apn_rate_ctrl_info_p->additional_rate_ctrl_info));
          }
        }
        else //Increment REL 13 APN Rate Control Packet Counter
      {
        if (!ds_pdn_rate_ctrl_is_interval_timer_running(
                                           &(apn_rate_ctrl_info_p->rate_ctrl_info)))
        {
          ds_pdn_rate_ctrl_start_interval_timer(
                                           &(apn_rate_ctrl_info_p->rate_ctrl_info));
        }
		ds_pdn_rate_ctrl_increment_counter(&(apn_rate_ctrl_info_p->rate_ctrl_info));
      }

        if ((TRUE == apn_rate_ctrl_info_p->additional_rate_ctrl_info.threshold_exceeded) ||
          (TRUE == apn_rate_ctrl_info_p->rate_ctrl_info.threshold_exceeded))
      {
          ds_pdn_apn_rate_ctrl_evaluate_flow_for_all_bearers(apn_rate_ctrl_info_p);
      } /* APN Rate Control exceeded */

      }

    } /* Do not drop data packet */
    else
    {
      (void)dsm_free_packet(item_ptr);
    } /* Drop data packet */
  }

  rex_leave_crit_sect(ul_data_crit_sect);  
  return;
} /* ds_eps_bearer_mgr_tx_um_data_cb() */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_DEFAULT_BEARER_ACTIVATE_IND

DESCRIPTION
  This function sets up the bearer when bearer activation indication
  is processed.
  
PARAMETERS  
  info_p: LTE info pointer from NAS
    
DEPENDENCIES 
  None.

RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
boolean ds_eps_bearer_mgr_default_bearer_activate_ind
(
  emm_act_default_bearer_context_request_ind_s_type  *info_ptr, 
  ds_bearer_mgr_s                                     *bearer_mgr_p
)
{
  ds_pdn_mgr_s                       *pdn_mgr_p = NULL;
#ifdef FEATURE_DATA_REL10
  char                               *apn = NULL;
  byte                                decoded_apn[DS_PROFILE_APN_STRING_LEN] = {0};
#endif /*FEATURE_DATA_REL10*/
#ifdef FEATURE_MODE_TLB
  ds_eps_tlb_per_subs_info_type      *hndl = NULL;
#endif /* FEATURE_MODE_TLB */
  boolean                             tlb_hndl_is_valid = FALSE;
  uint32                              rat_mask = DS_3GPP_THROT_RAT_MASK_GLOBAL;
  boolean                             cp_psm_activation_result = FALSE;
  ds_local_err_val_e_type             local_err = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) ||
      !bearer_mgr_p->eps_bearer )
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  bearer_mgr_p->bearer_id = info_ptr->eps_bearer_id;
  bearer_mgr_p->eps_bearer->lbi = info_ptr->esm_hdr.lbi;

  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {   
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

#ifdef FEATURE_MODE_TLB
  hndl = ds_eps_tlb_get_info_hndl();

  if (ds_eps_tlb_validate_info_hndl(hndl))
  {
    tlb_hndl_is_valid = TRUE;
  }

  /*------------------------------------------------------------------------- 
    Put the bearer into TLB state if TLB is active
  -------------------------------------------------------------------------*/
  if ( 
  	     tlb_hndl_is_valid &&
       ( (hndl->tlb_state.ds_eps_bearer_tlb_state == DS_EPS_BEARER_TLB_ACTIVE) ||
         (hndl->tlb_state.ds_eps_bearer_tlb_state == DS_EPS_BEARER_TLB_A_CLOSED) ||
         (hndl->tlb_state.ds_eps_bearer_tlb_state == DS_EPS_BEARER_TLB_G_CLOSED) ||
         (hndl->tlb_state.ds_eps_bearer_tlb_state == DS_EPS_BEARER_TLB_H_CLOSED) ) 
     )
  {
    bearer_mgr_p->is_in_tlb = TRUE;
  }

  /*-------------------------------------------------------------------------
    Set the Tx function 
  -------------------------------------------------------------------------*/
  if ( tlb_hndl_is_valid &&
       ( ds_eps_tlb_is_tlb_mode_active(hndl, LTE_TLB_MODE_A) ||
         ds_eps_tlb_is_tlb_mode_active(hndl, LTE_TLB_MODE_G) ||
         ds_eps_tlb_is_tlb_mode_active(hndl, LTE_TLB_MODE_H) )
     )
  {
    bearer_mgr_p->tx_ptr = dsEpsTlb_pdp_ip_tx_um_data_cb;
  } 
  else
#endif /*FEATURE_MODE_TLB */
  {
    bearer_mgr_p->tx_ptr = ds_eps_bearer_mgr_tx_um_data_cb;
  }
  
#ifdef FEATURE_MODE_TLB
  // Set  protocol to NULL if required later...
  
  /*-------------------------------------------------------------------------
    Set the handler for the downlink signal for the call.
  -------------------------------------------------------------------------*/
  if ( tlb_hndl_is_valid &&
      ((hndl->tlb_state.ds_eps_bearer_tlb_state == DS_EPS_BEARER_TLB_ACTIVE) ||
       (hndl->tlb_state.ds_eps_bearer_tlb_state == DS_EPS_BEARER_TLB_A_CLOSED)) &&
      (hndl->tlb_state.mode == LTE_TLB_MODE_A))
  {
    (void)ps_set_sig_handler(
                    bearer_mgr_p->rx_sig,
                    dsEpsTlb_pdp_ip_rx_um_drop_data_cb, 
                    (void *)bearer_mgr_p);
  /*-------------------------------------------------------------------------  
    Enable signal
  -------------------------------------------------------------------------*/
    ps_enable_sig(bearer_mgr_p->rx_sig);
  } 

#endif /* FEATURE_MODE_TLB*/

  /*---------------------------------------------------------------------
    Allocate timer handle for dynamic back pressure disable  -- revisit once timer module is final
  ---------------------------------------------------------------------*/
  bearer_mgr_p->tmr_hndl_dyn_back_pressure
    = ps_timer_alloc(ds_bearer_mgr_tmr_dyn_back_pressure_exp_hdlr,
                     (void *)bearer_mgr_p);

  if (PS_TIMER_INVALID_HANDLE == 
                bearer_mgr_p->tmr_hndl_dyn_back_pressure)
  {
    DS_ERROR_LOG_1("Invalid PS timer handle for bearer index: =d",
                   bearer_mgr_p->index);
  }

  bearer_mgr_p->tcp_dl_ack_counter = bearer_mgr_p->last_tcp_dl_ack_count = 0;

  /*-------------------------------------------------------------------------
    Move the state of the bearer to DS_BEARER_CONTEXT_STATE_PEND_RAB_REG
    This indicates the fact that we have responded to the Activate bearer
    indication from CM with DS3G_SUCCESS.
    Should there be a tear down requested from the APP, we will send a PDN
    disconnect and not a PDN Abort.
  -------------------------------------------------------------------------*/
  bearer_mgr_p->state = DS_BEARER_MGR_STATE_PEND_RAB_REG;

#ifdef FEATURE_DATA_REL10
  /*---------------------------------------------------------------------
    EPS Bearer context activate ind got successfull processed by UE.
    Clear APN throttling state machine if it was globally blocked.
  ----------------------------------------------------------------------*/  
  if(ds_pdn_nv_manager_get_nv_lte_rel_10())
  {
    if (info_ptr->apn_name.valid)
    {
      apn = (char*)info_ptr->apn_name.address;
      if((apn!=NULL)&&
         (info_ptr->apn_name.apn_addr_len <= DS_PROFILE_APN_STRING_LEN) &&
         (ds_pdn_decode_apn_name(info_ptr->apn_name.address,
            (uint32)info_ptr->apn_name.apn_addr_len,
            decoded_apn) == TRUE))
      {
        apn = (char*)decoded_apn;
      }
      else
      {
        DS_ERROR_LOG_0("unable to decode the APN");
        apn = pdn_mgr_p->pdp_profile->context.apn;
      }
    }
    else
    {
      apn = pdn_mgr_p->pdp_profile->context.apn;
    }
    
    DS_MSG_SPRINTF_1(MSG_LEGACY_HIGH,
                     "Trying to unblock potentially blocked APN: %s",
                     (char*)apn);
    /*---------------------------------------------------------------------
      Unblock the APN on Global RAT and LTE RAT if throttling per RAT is
      enabled
      ----------------------------------------------------------------------*/
    if (ds_pdn_nv_manager_get_enable_pdn_throt_per_rat()||
        ds_pdn_nv_manager_is_rel_12_or_above_supported())
    {
       rat_mask |= DS_3GPP_THROT_RAT_MASK_LTE;
    }

    ds_3gpp_pdn_throt_handle_nw_inited_unblock_apn((byte *)apn, rat_mask);
  }
#endif /*FEATURE_DATA_REL10*/
 
  /*---------------------------------------------------------------------
      Copy the CPCIOT Info from the Network (i.e. cp_only, splmn_rate_ctrl)
   ---------------------------------------------------------------------*/
  ds_pdn_cp_data_save_cpciot_info_per_bearer(info_ptr, 
                                              bearer_mgr_p,
                                              pdn_mgr_p->is_restore_mode);

  /*--------------------------------------------------------------------- 
    The case where we are exiting PSM mode and activating bearers and/or
    CP CIOT data is enabled
  ---------------------------------------------------------------------*/
  if(TRUE == pdn_mgr_p->is_restore_mode)
  {
    cp_psm_activation_result = ds_pdn_psm_handle_bearer_activation(bearer_mgr_p);
  }


  if ((FALSE == cp_psm_activation_result) && 
      (TRUE == ds_pdn_cp_data_is_enabled()))
  {
    cp_psm_activation_result = 
    	         ds_pdn_cp_data_handle_bearer_activation(bearer_mgr_p);
  }
  
  if ((FALSE == cp_psm_activation_result) && 
      ((TRUE == pdn_mgr_p->is_restore_mode) ||
       (TRUE == ds_pdn_cp_data_is_enabled())) ) 
  {
    return FALSE;  
  }

  return TRUE;
  
func_exit:
  if(local_err!= DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Default bearer activation failed with error %d",
    	                        local_err);
  }

  return FALSE;
} /* ds_eps_bearer_cntxt_bearer_activate_ind */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_DEDICATED_BEARER_ACTIVATE_IND

DESCRIPTION
  This function sets up the bearer when dedicated bearer activation indication
  is processed.
  
PARAMETERS  
  info_p: LTE info pointer from NAS
    
DEPENDENCIES 
  None.

RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
boolean ds_eps_bearer_mgr_dedicated_bearer_activate_ind
(
  emm_act_dedicated_bearer_context_request_ind_s_type  *info_ptr, 
  ds_bearer_mgr_s                                      *bearer_mgr_p
)
{
  ds_pdn_mgr_s                       *pdn_mgr_p = NULL;
#ifdef FEATURE_MODE_TLB
  ds_eps_tlb_per_subs_info_type      *hndl = NULL;
#endif /* FEATURE_MODE_TLB */
  boolean                             tlb_hndl_is_valid = FALSE;
  char                               *apn = NULL;
  uint32                              rat_mask = DS_3GPP_THROT_RAT_MASK_GLOBAL;
  ds_local_err_val_e_type             local_err = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) ||
      !bearer_mgr_p->eps_bearer )
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  bearer_mgr_p->bearer_id = info_ptr->eps_bearer_id;
  bearer_mgr_p->eps_bearer->lbi = info_ptr->esm_hdr.lbi;

  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {   
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

#ifdef FEATURE_MODE_TLB

  hndl = ds_eps_tlb_get_info_hndl();

  if (ds_eps_tlb_validate_info_hndl(hndl))
  {
    tlb_hndl_is_valid = TRUE;
  }

  /*------------------------------------------------------------------------- 
    Put the bearer into TLB state if TLB is active
  -------------------------------------------------------------------------*/
  if ( 
  	  tlb_hndl_is_valid &&
       ( (hndl->tlb_state.ds_eps_bearer_tlb_state == DS_EPS_BEARER_TLB_ACTIVE) ||
         (hndl->tlb_state.ds_eps_bearer_tlb_state == DS_EPS_BEARER_TLB_A_CLOSED) ||
         (hndl->tlb_state.ds_eps_bearer_tlb_state == DS_EPS_BEARER_TLB_G_CLOSED) ||
         (hndl->tlb_state.ds_eps_bearer_tlb_state == DS_EPS_BEARER_TLB_H_CLOSED) ) 
     )
  {
    bearer_mgr_p->is_in_tlb = TRUE;
  }

  /*-------------------------------------------------------------------------
    Set the Tx function 
  -------------------------------------------------------------------------*/
  if ( tlb_hndl_is_valid &&
       ( ds_eps_tlb_is_tlb_mode_active(hndl, LTE_TLB_MODE_A) ||
         ds_eps_tlb_is_tlb_mode_active(hndl, LTE_TLB_MODE_G) ||
         ds_eps_tlb_is_tlb_mode_active(hndl, LTE_TLB_MODE_H) )
     )
  {

    bearer_mgr_p->tx_ptr = dsEpsTlb_pdp_ip_tx_um_data_cb; 
  } 
  else
#endif /*FEATURE_MODE_TLB*/
  {
    bearer_mgr_p->tx_ptr = ds_eps_bearer_mgr_tx_um_data_cb;
  }
  
#ifdef FEATURE_MODE_TLB
  // Set  protocol to NULL if required later...
  
  /*-------------------------------------------------------------------------
    Set the handler for the downlink signal for the call.
  -------------------------------------------------------------------------*/
  if ( tlb_hndl_is_valid &&
      ((hndl->tlb_state.ds_eps_bearer_tlb_state == DS_EPS_BEARER_TLB_ACTIVE) ||
       (hndl->tlb_state.ds_eps_bearer_tlb_state == DS_EPS_BEARER_TLB_A_CLOSED)) &&
      (hndl->tlb_state.mode == LTE_TLB_MODE_A))
  {
    (void)ps_set_sig_handler(
                    bearer_mgr_p->rx_sig,
                    dsEpsTlb_pdp_ip_rx_um_drop_data_cb, 
                    (void *)bearer_mgr_p);
  /*-------------------------------------------------------------------------  
    Enable signal
   -------------------------------------------------------------------------*/
    ps_enable_sig(bearer_mgr_p->rx_sig);
  } 


#endif /* FEATURE_MODE_TLB */
  /*---------------------------------------------------------------------
    Allocate timer handle for dynamic back pressure disable  -- revisit once timer module is final
  ---------------------------------------------------------------------*/
  bearer_mgr_p->tmr_hndl_dyn_back_pressure
    = ps_timer_alloc(ds_bearer_mgr_tmr_dyn_back_pressure_exp_hdlr,
                     (void *)bearer_mgr_p);

  if (PS_TIMER_INVALID_HANDLE == 
                bearer_mgr_p->tmr_hndl_dyn_back_pressure)
  {
    DS_ERROR_LOG_1("Invalid PS timer handle for bearer index: =d",
                   bearer_mgr_p->index);
  }

  bearer_mgr_p->tcp_dl_ack_counter = bearer_mgr_p->last_tcp_dl_ack_count = 0;

  ds_bearer_flow_manager_bind_flow(bearer_mgr_p);
  
  /*-------------------------------------------------------------------------
    Move the state of the bearer to DS_BEARER_CONTEXT_STATE_PEND_RAB_REG
    This indicates the fact that we have responded to the Activate bearer
    indication from CM with DS3G_SUCCESS.
    Should there be a tear down requested from the APP, we will send a PDN
    disconnect and not a PDN Abort.
  -------------------------------------------------------------------------*/
  bearer_mgr_p->state = DS_BEARER_MGR_STATE_PEND_RAB_REG;
 
  /*---------------------------------------------------------------------
    EPS Bearer context activate ind got successfull processed by UE.
    Clear APN throttling state machine if it was globally blocked.
  ----------------------------------------------------------------------*/  
  if(ds_pdn_nv_manager_get_nv_lte_rel_10())
  {
    apn = pdn_mgr_p->pdp_profile->context.apn;
    
    DS_MSG_SPRINTF_1(MSG_LEGACY_HIGH,
                     "Trying to unblock potentially blocked APN: %s",
                     (char*)apn);
    /*---------------------------------------------------------------------
      Unblock the APN on Global RAT and LTE RAT if throttling per RAT is
      enabled
      ----------------------------------------------------------------------*/
    if (ds_pdn_nv_manager_get_enable_pdn_throt_per_rat()||
        ds_pdn_nv_manager_is_rel_12_or_above_supported())
    {
       rat_mask |= DS_3GPP_THROT_RAT_MASK_LTE;
    }

    ds_3gpp_pdn_throt_handle_nw_inited_unblock_apn((byte*)apn, rat_mask);
  }

  return TRUE;
  
func_exit:
  if(local_err!= DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Dedicated bearer activation failed with error %d",
    	                        local_err);
  }

  return FALSE;
} /* ds_eps_bearer_cntxt_bearer_activate_ind */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_GET_FLOW_DIRECTION

DESCRIPTION
  This function returns the direction of the flow in a particular bearer.

PARAMETERS
  bearer_mgr_p - pointer to the bearer context
  
DEPENDENCIES
  None.

RETURN VALUE
ds_eps_bearer_mgr_flow_dir_e - Flow direction for this bearer
  
SIDE EFFECTS
  None.
===========================================================================*/
ds_eps_bearer_mgr_flow_dir_e ds_eps_bearer_mgr_get_flow_direction
(
  ds_bearer_mgr_s*  bearer_mgr_p
)
{
  ds_bearer_flow_manager_s     *bflow_manager_p = NULL;
  sdf_qos_T                     sdf_qos = {0};

  // default flow is always bidirectional
  if (bearer_mgr_p->is_default)
  {
    return DS_EPS_FLOW_DIR_BIDIRECTIONAL;
  }
  else
  {
    bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_mgr_p);
    if( !bflow_manager_p || 
    	   !bflow_manager_p->eps_flow )
    {
      DS_ERROR_LOG_0("eps_bearer flow is NULL");
      return DS_EPS_FLOW_DIR_INVALID;
    }
    
    sdf_qos = bflow_manager_p->eps_flow->sdf_qos;
    if((sdf_qos.qci >=(byte)(DS_LTE_QCI_5) && sdf_qos.qci <=(byte)(DS_LTE_QCI_9)) ||
       (sdf_qos.qci ==(byte)(DS_LTE_QCI_69) || sdf_qos.qci ==(byte)(DS_LTE_QCI_70))
      )
    {
      // if the QCI is non-GBR, it is a bidirectional flow
      return DS_EPS_FLOW_DIR_BIDIRECTIONAL;
    }
    else if((sdf_qos.qci >=(byte)(DS_LTE_QCI_1) &&
              sdf_qos.qci <=(byte)(DS_LTE_QCI_4)) ||
             (sdf_qos.qci ==(byte)(DS_LTE_QCI_65) ||
              sdf_qos.qci ==(byte)(DS_LTE_QCI_66))
           )
    {
      // QCI is GBR, decide based on UL and DL rate
      if ((sdf_qos.gbr_dl != DS_EPS_0KB_BITRATE_VALUE) &&
          (sdf_qos.gbr_ul != DS_EPS_0KB_BITRATE_VALUE))
      {
        return DS_EPS_FLOW_DIR_BIDIRECTIONAL;
      }
      else if ((sdf_qos.gbr_dl != DS_EPS_0KB_BITRATE_VALUE) &&
               (sdf_qos.gbr_ul == DS_EPS_0KB_BITRATE_VALUE))
      {
        return DS_EPS_FLOW_DIR_DOWNLINK;
      }
      else if ((sdf_qos.gbr_dl == DS_EPS_0KB_BITRATE_VALUE) &&
               (sdf_qos.gbr_ul != DS_EPS_0KB_BITRATE_VALUE))
      {
        return DS_EPS_FLOW_DIR_UPLINK;
      }
      else
      {
        return DS_EPS_FLOW_DIR_INVALID;
      }
    }
    else
    {
      return DS_EPS_FLOW_DIR_INVALID;
    }
  }/* non-default bearer*/
}/* ds_eps_bearer_mgr_get_flow_direction */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_SETUP_UL_WM

DESCRIPTION
  This function sets up UL watermark i.e. PS to PDCP watermark.
  
PARAMETERS  
  bearer_mgr_p
    
DEPENDENCIES 
  None.

RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None. 
===========================================================================*/
void ds_eps_bearer_mgr_setup_ul_wm
(
  ds_bearer_mgr_s       *bearer_mgr_p
)
{
  dsm_watermark_type      *tx_wm_ptr;
  ds_pdn_mgr_s            *pdn_mgr_p = NULL;
  ds_bearer_mgr_um_wm_threshold_struct_type  um_wm_threshold;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if (!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    return;
  }

  um_wm_threshold = ds_pdn_nv_get_um_wm_threshold();

  tx_wm_ptr = &(bearer_mgr_p->tx_wm.wm_item);

  dsm_set_low_wm(tx_wm_ptr,
                 um_wm_threshold.lte_ul_lo);
  dsm_set_hi_wm(tx_wm_ptr,
                um_wm_threshold.lte_ul_hi);
  dsm_set_dne(tx_wm_ptr,
              um_wm_threshold.lte_ul_dne);
  dsm_set_wm_cb(tx_wm_ptr,DSM_WM_LO_Q_CNT,
                FALSE,0,FALSE,0,
                TRUE,DSLTEPS_UL_WM_DEFAULT_LO_QCNT);
  dsm_set_wm_cb(tx_wm_ptr,DSM_WM_HI_Q_CNT,
                FALSE,0,FALSE,0,
                TRUE,DSLTEPS_UL_WM_DEFAULT_HI_QCNT);
  dsm_set_dne_q_cnt(tx_wm_ptr,DSLTEPS_UL_WM_DEFAULT_DNE_QCNT);

  /*-----------------------------------------------------------------------
    Pass the Bearer ptr in watermarks registered so that we
    can identify the call when the callback is called
  -----------------------------------------------------------------------*/
  tx_wm_ptr->lowater_func_ptr = ds_bearer_mgr_wm_flow_enable_cb;
  tx_wm_ptr->lowater_func_data = (void*)bearer_mgr_p;
  
  tx_wm_ptr->hiwater_func_ptr = ds_bearer_mgr_wm_flow_disable_cb;
  tx_wm_ptr->hiwater_func_data = (void*)bearer_mgr_p;

  tx_wm_ptr->lowater_q_cnt_func_ptr = ds_bearer_mgr_wm_flow_enable_cb;
  tx_wm_ptr->lowater_q_cnt_func_data = (void*)bearer_mgr_p;
  
  tx_wm_ptr->hiwater_q_cnt_func_ptr = ds_bearer_mgr_wm_flow_disable_cb;
  tx_wm_ptr->hiwater_q_cnt_func_data = (void*)bearer_mgr_p;

  return;
} /* ds_eps_bearer_mgr_setup_ul_wm() */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_SETUP_DL_WM

DESCRIPTION
  This function sets up the downlink watermark
  
PARAMETERS  
  bearer_mgr_p
      
DEPENDENCIES 
  None.

RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_eps_bearer_mgr_setup_dl_wm
(
  ds_bearer_mgr_s       *bearer_mgr_p
)
{
  ds_bearer_mgr_um_wm_threshold_struct_type  um_wm_threshold;
  /*-----------------------------------------------------------------------*/
  
  um_wm_threshold = ds_pdn_nv_get_um_wm_threshold();

  dsm_set_low_wm(&(bearer_mgr_p->rx_wm.wm_item),
                 um_wm_threshold.lte_dl_lo);
  dsm_set_hi_wm(&(bearer_mgr_p->rx_wm.wm_item),
                um_wm_threshold.lte_dl_hi);
  dsm_set_dne(&(bearer_mgr_p->rx_wm.wm_item),
              um_wm_threshold.lte_dl_dne);
  dsm_set_dne_q_cnt(&(bearer_mgr_p->rx_wm.wm_item),
                    DSLTEPS_DL_WM_DEFAULT_DNE_QCNT);
}/* ds_eps_bearer_mgr_setup_dl_wm */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_REGISTER_UL_WITH_PDCP

DESCRIPTION
  This function sends UL registration message to PDCP.
  
PARAMETERS  
  bearer_mgr_p
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_eps_bearer_mgr_register_ul_with_pdcp
(
  ds_bearer_mgr_s       *bearer_mgr_p
)
{
  lte_pdcpul_rab_register_req_msg_s   pdcp_ul_rab_reg_req_msg;
  boolean                             rtval = TRUE;
  /*-----------------------------------------------------------------------*/
  ULOG_RT_PRINTF_1(ds_fwk_get_ulog_handle(),"UL Reg Request for bearer id :%d",
                                 bearer_mgr_p->bearer_id);
  pdcp_ul_rab_reg_req_msg.eps_id = bearer_mgr_p->bearer_id;
  pdcp_ul_rab_reg_req_msg.ps_pdcp_wm_ptr = 
                                    (void *) &(bearer_mgr_p->tx_wm.wm_item);
  rtval = ds_msgr_snd_msg_send
          (
            LTE_PDCPUL_RAB_REGISTER_REQ,  
            (msgr_hdr_struct_type*)(&pdcp_ul_rab_reg_req_msg),
            sizeof(lte_pdcpul_rab_register_req_msg_s)
          );
  if(rtval == FALSE) 
  {
    DS_ERROR_LOG_0(" Unable to send message to DSMSGR");
  }

  return(rtval);                  
} /* ds_eps_bearer_mgr_register_ul_with_pdcp */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_REGISTER_DL_WITH_PDCP

DESCRIPTION
  This function sends DL registration message to PDCP.
  
PARAMETERS  
  bearer_mgr_p
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_eps_bearer_mgr_register_dl_with_pdcp
(
  ds_bearer_mgr_s       *bearer_mgr_p
)
{
  lte_pdcpdl_rab_register_req_msg_s   pdcp_dl_rab_reg_req_msg;
  boolean                             rtval = TRUE;
  /*-----------------------------------------------------------------------*/
  ULOG_RT_PRINTF_1(ds_fwk_get_ulog_handle(),"DL Reg Request for bearer id :%d",
                                 bearer_mgr_p->bearer_id);
  pdcp_dl_rab_reg_req_msg.eps_id = bearer_mgr_p->bearer_id;
  pdcp_dl_rab_reg_req_msg.pdcp_wm_ptr = 
                                  (void *)&(bearer_mgr_p->rx_wm.wm_item);

  rtval = ds_msgr_snd_msg_send
          (
            LTE_PDCPDL_RAB_REGISTER_REQ,  
            (msgr_hdr_struct_type*)(&pdcp_dl_rab_reg_req_msg),
            sizeof(lte_pdcpdl_rab_register_req_msg_s)
          );

  if(rtval == FALSE) 
  {
    DS_ERROR_LOG_0(" Unable to send message to DSMSGR");
  }

  return(rtval);                  
} /* ds_eps_bearer_mgr_register_dl_with_pdcp */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_REGISTER_UL_DATAPATH

DESCRIPTION
  This function is used to establish the complete uplink Data path.
  Setting up of watermarks, registering with UL PDCP.

PARAMETERS
 bearer_mgr_p

DEPENDENCIES
  None.

RETURN VALUE
None
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_bearer_mgr_register_ul_datapath
(
  ds_bearer_mgr_s *bearer_mgr_p
)
{
  boolean ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*----------------------------------------------------------------------
    Configure the PS-PDCP WM before registring with PDCP
  --------------------------------------------------------------------*/
  ds_eps_bearer_mgr_setup_ul_wm(bearer_mgr_p);

  ret_val = ds_eps_bearer_mgr_register_ul_with_pdcp(bearer_mgr_p);  
  if(ret_val == FALSE)
  {      
    DS_ERROR_LOG_0("UL Registering with lower layer failed");
    bearer_mgr_p->state = DS_BEARER_MGR_STATE_UP_DORMANT;
    
    /*---------------------------------------------------------------------  
      Register each enqueue function ptr.      
    ---------------------------------------------------------------------*/
    (bearer_mgr_p->tx_wm.wm_item).each_enqueue_func_ptr =
      (wm_cb_type)ds_bearer_mgr_reorig_dorm_call_cb;
    (bearer_mgr_p->tx_wm.wm_item).each_enqueue_func_data =\
      (void*) bearer_mgr_p;
  }

  return;
}/* ds_eps_bearer_mgr_register_ul_datapath */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_REGISTER_DL_DATAPATH

DESCRIPTION
  This function is used to establish the complete uplink Data path.
  Setting up of watermarks, registering with A2, registering with DL PDCP.

PARAMETERS
 bearer_mgr_p

DEPENDENCIES
  None.

RETURN VALUE
None
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_bearer_mgr_register_dl_datapath
(
  ds_bearer_mgr_s *bearer_mgr_p
)
{
  boolean ret_val = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*----------------------------------------------------------------------
    Configure the PS-PDCP WM before registring with PDCP
  --------------------------------------------------------------------*/
  ds_eps_bearer_mgr_setup_dl_wm(bearer_mgr_p);

  ret_val = ds_eps_bearer_mgr_register_dl_with_pdcp(bearer_mgr_p);  
  if(ret_val == FALSE)
  {      
    DS_ERROR_LOG_0("DL Registering with lower layer failed");
    bearer_mgr_p->state = DS_BEARER_MGR_STATE_UP_DORMANT;
    
    /*---------------------------------------------------------------------  
      Register each enqueue function ptr.      
    ---------------------------------------------------------------------*/
    (bearer_mgr_p->tx_wm.wm_item).each_enqueue_func_ptr =\
      (wm_cb_type)ds_bearer_mgr_reorig_dorm_call_cb;
    (bearer_mgr_p->tx_wm.wm_item).each_enqueue_func_data =\
      (void*) bearer_mgr_p;
  }

  return;
}/* ds_eps_bearer_mgr_register_dl_datapath */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_GET_LTE_NETW_ALLOC_QOS

DESCRIPTION
  Convert app QoS and stores the results in lte_netw_alloc_qos.

PARAMETERS
  bearer_mgr_p   - bearer pointer.
  lte_netw_alloc_qos - pointer to hold the converted values

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_bearer_mgr_get_lte_netw_alloc_qos
(
  ds_bearer_mgr_s              *bearer_mgr_p,
  ds_pdn_lte_qos_params_type   *lte_netw_alloc_qos
)
{
  ds_bearer_flow_manager_s    *bflow_manager_p;
  ip_flow_type                *app_rx_flow;
  ip_flow_type                *app_tx_flow;
  
  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_mgr_p);
  if(!bflow_manager_p)
  {
    return;
  }

  app_rx_flow = &(bflow_manager_p->rx_flow);
  app_tx_flow = &(bflow_manager_p->tx_flow);

  if(app_rx_flow == NULL || app_tx_flow == NULL)
  {
    DS_ERROR_LOG_0("Null flows in bflow_manager_p.");
    return;
  }

  if( (app_rx_flow->field_mask & (uint32)IPFLOW_MASK_LTE_QCI) || 
      (app_rx_flow->field_mask & (uint32)IPFLOW_MASK_DATA_RATE) ||
      (app_tx_flow->field_mask & (uint32)IPFLOW_MASK_DATA_RATE)
    )
  {
    lte_netw_alloc_qos->valid_flg = TRUE;
  }

  if (app_rx_flow->field_mask & (uint32)IPFLOW_MASK_LTE_QCI)
  {
    lte_netw_alloc_qos->qci = 
             (ds_pdn_lte_qci_e_type )app_rx_flow->lte_params.lte_qci;
  }
  if(app_rx_flow->field_mask & (uint32)IPFLOW_MASK_DATA_RATE)
  {
    if(app_rx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE)
    {
      lte_netw_alloc_qos->max_dl_bit_rate = 
         (app_rx_flow->data_rate.format.min_max.max_rate)/EPS_BEARER_KBPS_TO_BPS_MULTIPLIER;
      lte_netw_alloc_qos->g_dl_bit_rate = 
         (app_rx_flow->data_rate.format.min_max.guaranteed_rate)/EPS_BEARER_KBPS_TO_BPS_MULTIPLIER;
    }
    else if (app_rx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_EX_TYPE) 
    {
    lte_netw_alloc_qos->max_dl_bit_rate = 
       (uint32)((app_rx_flow->data_rate.format.min_max_ex.max_rate)/EPS_BEARER_KBPS_TO_BPS_MULTIPLIER);
    lte_netw_alloc_qos->g_dl_bit_rate = 
       (uint32)((app_rx_flow->data_rate.format.min_max_ex.guaranteed_rate)/EPS_BEARER_KBPS_TO_BPS_MULTIPLIER);
    }    
  }
  
  if(app_tx_flow->field_mask & (uint32)IPFLOW_MASK_DATA_RATE)
  {
    if(app_tx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_TYPE)
    {
    lte_netw_alloc_qos->max_ul_bit_rate = 
             (app_tx_flow->data_rate.format.min_max.max_rate)/EPS_BEARER_KBPS_TO_BPS_MULTIPLIER;
    lte_netw_alloc_qos->g_ul_bit_rate = 
             (app_tx_flow->data_rate.format.min_max.guaranteed_rate)/EPS_BEARER_KBPS_TO_BPS_MULTIPLIER;
    }
    else if(app_tx_flow->data_rate.format_type == DATA_RATE_FORMAT_MIN_MAX_EX_TYPE)
    {
      lte_netw_alloc_qos->max_ul_bit_rate = 
             (uint32)((app_tx_flow->data_rate.format.min_max_ex.max_rate)/EPS_BEARER_KBPS_TO_BPS_MULTIPLIER);
      lte_netw_alloc_qos->g_ul_bit_rate = 
             (uint32)((app_tx_flow->data_rate.format.min_max_ex.guaranteed_rate)/EPS_BEARER_KBPS_TO_BPS_MULTIPLIER);
    }    
  }

  return;
} /* ds_eps_bearer_mgr_get_lte_netw_alloc_qos */

/*===========================================================================
FUNCTION ds_eps_bearer_mgr_send_qos_indication

DESCRIPTION
  This function sends tehe QCI information of a particular bearer to interested
  clients.
  
PARAMETERS  
    bearer_mgr_p - Bearer Context.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_eps_bearer_mgr_send_qos_indication
(
   ds_bearer_mgr_s *bearer_mgr_p
)
{ 

  ds_pdn_lte_qos_params_type    lte_netw_alloc_qos = {0};  
  ds_msgr_bearer_qos_info_msg_s qos_info_msg = {{0}};

  ds_eps_bearer_mgr_get_lte_netw_alloc_qos(bearer_mgr_p,
                                           &lte_netw_alloc_qos); 

  if ((bearer_mgr_p->eps_bearer != NULL) &&
  	   (lte_netw_alloc_qos.valid_flg == TRUE))
  {       
    qos_info_msg.current_bearer_rat = DS_RAT_LTE;
    qos_info_msg.lte_qos_info.eps_bearer_id = 
                             bearer_mgr_p->bearer_id;

    DS_MSG1(MSG_LEGACY_HIGH,"QCI of bearer is %d",lte_netw_alloc_qos.qci);   
    qos_info_msg.lte_qos_info.qci_value = 
                        (ds_3gpp_lte_qci_enum_type)lte_netw_alloc_qos.qci;

    (void) ds_msgr_snd_msg_send_ext( 
       DS_3GPP_BEARER_QOS_INFO_IND,
       MSGR_DS_3GPP, 
       (msgr_hdr_struct_type*)(&qos_info_msg),
       sizeof(qos_info_msg)
    );    
  }
  return;
}/* ds_eps_bearer_mgr_send_qos_indication */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_SEND_TRM_PRIORITY_IND

DESCRIPTION  
  This function is used to send TRM Priority Indication to RRC on
  RAB Resstablishment / Release.
      
PARAMETERS 
  1. bearer_mgr_p 
  2. TRM Priority
 
DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_bearer_mgr_send_trm_priority_ind
(
  ds_bearer_mgr_s     *bearer_mgr_p,
  lte_trm_priority_e   trm_priority
)
{
  lte_dsds_trm_priority_req_s   trm_priority_req = {{0}};
  ds_pdn_lte_qos_params_type    lte_netw_alloc_qos = {0};
  ds_pdn_mgr_s                 *pdn_mgr_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds_pdn_nv_manager_get_enable_send_trm_priority() == TRUE &&
     ds_pdn_mgr_is_wwan_tech_suspended() == FALSE)
  {
    ds_eps_bearer_mgr_get_lte_netw_alloc_qos(bearer_mgr_p,
                                             &lte_netw_alloc_qos);

    pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
    if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
    {
      return;
    }

    if(((pdn_mgr_p->lte_params->rab_reestab_call_type[DS_PDN_INSTANCE_IPV4] == 
                                   DS_PDN_3GPP_LTE_REESTAB_CALLTYPE_VOLTE)||
        (pdn_mgr_p->lte_params->rab_reestab_call_type[DS_PDN_INSTANCE_IPV6] == 
                                   DS_PDN_3GPP_LTE_REESTAB_CALLTYPE_VOLTE))&&
       (lte_netw_alloc_qos.qci == DS_PDN_LTE_QCI_1))
    {
      // Post TRM priroty HIGH to RRC
      trm_priority_req.trm_priority = trm_priority;
      trm_priority_req.client_id = LTE_TRM_PRIORITY_REQ_FROM_DS;

      if( FALSE == ds_msgr_snd_msg_send_ext
             ( LTE_RRC_DSDS_TRM_PRIORITY_REQ,
               MSGR_DS_LTE, 
               (msgr_hdr_struct_type*)(&trm_priority_req),
               sizeof(trm_priority_req)
             ) )
      {
        DS_ERROR_LOG_0("Posting of TRM_PRIORITY_REQ failed");
      }
    }
  }
  else
  {
    DS_MSG0(MSG_LEGACY_HIGH,
    	       "NV: DS_3GPP_ENABLE_SEND_TRM_PRIORITY is FALSE. "
            "Not notifying TRM Priority to LTE RRC");
  }

  return;
}/* ds_eps_bearer_mgr_send_trm_priority_ind */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_RAB_RE_ESTAB_IND_HDLR

DESCRIPTION
  This function does LTE specific operations that have to be performed on
  RAB re-establishment.

  
PARAMETERS  
  bearer_mgr_p
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
===========================================================================*/
boolean ds_eps_bearer_mgr_rab_re_estab_ind_hdlr
(
  ds_bearer_mgr_s           *bearer_mgr_p,
  emm_drb_setup_ind_s_type  *info_ptr  
)
{
  ds_eps_bearer_mgr_flow_dir_e       bearer_flow_dir;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*----------------------------------------------------------------------
    Get the flow direction for this bearer
  --------------------------------------------------------------------*/
  bearer_flow_dir = ds_eps_bearer_mgr_get_flow_direction(bearer_mgr_p);
  if ((bearer_flow_dir == DS_EPS_FLOW_DIR_BIDIRECTIONAL) ||
       bearer_flow_dir == DS_EPS_FLOW_DIR_UPLINK)
  {
    ds_eps_bearer_mgr_register_ul_datapath(bearer_mgr_p);
  }
  else if (bearer_flow_dir == DS_EPS_FLOW_DIR_DOWNLINK)
  {
    ds_eps_bearer_mgr_register_dl_datapath(bearer_mgr_p);
    
    /* Disallow UL Flow as flow direction is only DL  */
    ds_pdn_flow_mgr_update_uplink_flow_capability(bearer_mgr_p, FALSE);
  }
  else
  {
    DS_ERROR_LOG_1("Invalid flow dir for bearer =d", bearer_mgr_p->index);
    return FALSE;        
  }

  if(bearer_mgr_p->eps_bearer)
  {
    bearer_mgr_p->eps_bearer->pdcp_reg_req_sent = TRUE;
  }

  //Send the QCI of this bearer to interested parties.
  ds_eps_bearer_mgr_send_qos_indication(bearer_mgr_p);
          
  ds_eps_bearer_mgr_send_trm_priority_ind(bearer_mgr_p,
                                          LTE_TRM_PRIORITY_HIGH );

  return TRUE;
} /* ds_eps_bearer_cntxt_call_rab_re_estab_ind_hdlr */

/*===========================================================================
FUNCTION  DS_EPS_BEARER_MGR_PDCPDL_RAB_REG_CNF_MSG_HDLR

DESCRIPTION
  This function handles the confirmation of the DL watermark registration 
  with PDCP
  
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_mgr_pdcpdl_rab_reg_cnf_msg_hdlr 
(
  msgr_umid_type              msgrtype,
  const msgr_hdr_struct_type *dsmsg
)
{
  ds_bearer_mgr_s                 *bearer_mgr_p = NULL;
  eps_bearer_id_T                  eps_bearer_id = 0;
  ds_bearer_mgr_state_e            bearer_state;
  /*------------------------------------------------------------------------*/

  if (dsmsg == NULL ||
  	   msgrtype != LTE_PDCPDL_RAB_REGISTER_CNF)
  {
    return FALSE;
  }

  eps_bearer_id = ((lte_pdcpdl_rab_register_cnf_msg_s*)dsmsg)->eps_id;

  bearer_mgr_p = ds_bearer_mgr_get_bearer_from_eps_id((uint8)eps_bearer_id);
  
  ULOG_RT_PRINTF_1(ds_fwk_get_ulog_handle(), "DL REG CNF for bearer id: %d",
                                              eps_bearer_id);

  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    /*----------------------------------------------------------------------
      It is possible that we are receiving this message after the call end
      due to disconnect was processed. In such a case, just ignore and ret
      DS3G_SUCCESS. Note that success is returned to the component that 
      handles incoming message. A success means only that the message was
      handled. It does not specify anything about the lower layers being
      set up right
    ----------------------------------------------------------------------*/
    return TRUE;
  }

  bearer_state = bearer_mgr_p->state;
  if(bearer_state != DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB &&
     bearer_state != DS_BEARER_MGR_STATE_PEND_RAB_REG)
  {
    /*----------------------------------------------------------------------
      Ignore the lower layer set up since it is not expected in the states
      other than the ones mentioned above
    ----------------------------------------------------------------------*/
    DS_ERROR_LOG_2("Recd wm set up conf - bearer state: =d Bearer inst: =d. Ignore",
                   bearer_mgr_p->state, bearer_mgr_p->index);;
    return TRUE;
  }

  if( ((lte_pdcpdl_rab_register_cnf_msg_s*)dsmsg)->status != E_SUCCESS)
  {  
    /* registration with pdcp DL failed, move bearer to dormant and notify 
       pdn context that bearer setup is complete*/
    DS_MSG2(MSG_LEGACY_HIGH, "unexpected MSG Status: %d, "
            "RAB_REG_UL_CNF FAILURE for eps bearer id :%d, "
            "setting bearer to UP dormant",
            ((lte_pdcpdl_rab_register_cnf_msg_s*)dsmsg)->status,
            eps_bearer_id);
			
	ULOG_RT_PRINTF_1(ds_fwk_get_ulog_handle(), "DL Reg status failed bearer id: %d",
                                              eps_bearer_id);

    /* if the bearer is coming out of dormancy, put it back to dormancy */
    bearer_mgr_p->state = DS_BEARER_MGR_STATE_UP_DORMANT;
    /*-------------------------------------------------------------------------  
        Register each enqueue function ptr.
      -------------------------------------------------------------------------*/
    (bearer_mgr_p->tx_wm.wm_item).each_enqueue_func_ptr =\
           (wm_cb_type)ds_bearer_mgr_reorig_dorm_call_cb;
    (bearer_mgr_p->tx_wm.wm_item).each_enqueue_func_data =\
           (void*) bearer_mgr_p;
    
    
    /* if the bearer is being setup, put it to dormant and notify pdn that bearer
       setup is complete */
    if (bearer_state == DS_BEARER_MGR_STATE_PEND_RAB_REG)
    {
      // if cp_data is enabled dont do anything else
      // enable bearer fow and invoke CB with DS_FLOW_PHYS_LINK_MASK
      
      /*----------------------------------------------------------------------
         Notify PDN that the bearer setup is complete
      ----------------------------------------------------------------------*/
      ds_pdn_mgr_bearer_up_ind(bearer_mgr_p->pdn_mgr_ptr, bearer_mgr_p);
      ds_pdn_mgr_execute_cached_deactivation(bearer_mgr_p->pdn_mgr_ptr, bearer_mgr_p);
    }
    
    return FALSE;
  }

  DS_MSG1(MSG_LEGACY_HIGH,"RAB_REG_DLCNF for EPS bearer id:%d",eps_bearer_id);

  ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, DS_BEARER_CONTROL_MASK);

  /*-----------------------------------------------------------------------
    If the bearer is coming out of dormancy, there is no need to call the 
    bearer_up_ind call back.
    If this is a new bearer (bearer state is COMING_UP), we need to call
    the bearer up ind call back
  -----------------------------------------------------------------------*/
  bearer_mgr_p->state = DS_BEARER_MGR_STATE_UP;
  ds_bearer_mgr_post_bearer_up_event(bearer_mgr_p);
  	
  if (bearer_state == DS_BEARER_MGR_STATE_PEND_RAB_REG)
  {    
    /*----------------------------------------------------------------------
         Notify PDN that the bearer setup is complete
       ----------------------------------------------------------------------*/
    ds_pdn_mgr_bearer_up_ind(bearer_mgr_p->pdn_mgr_ptr, bearer_mgr_p);
  }

  bearer_mgr_p->eps_bearer->is_registered_pdcp = TRUE; 
  bearer_mgr_p->eps_bearer->pdcp_reg_req_sent = FALSE; 
  ds_pdn_mgr_execute_cached_deactivation(bearer_mgr_p->pdn_mgr_ptr, bearer_mgr_p);
  return TRUE;   
} /* ds_eps_bearer_mgr_pdcpdl_rab_reg_cnf_msg_hdlr */

/*===========================================================================
FUNCTION  DS_EPS_BEARER_MGR_PDCPUL_RAB_REG_CNF_MSG_HDLR

DESCRIPTION
  This function handles the confirmation of the UL watermark registration 
  with PDCP
  
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_mgr_pdcpul_rab_reg_cnf_msg_hdlr 
(
  msgr_umid_type              msgrtype,
  const msgr_hdr_struct_type *dsmsg
)
{
  ds_bearer_mgr_s                 *bearer_mgr_p = NULL;
  eps_bearer_id_T                  eps_bearer_id = 0;
  ds_eps_bearer_mgr_flow_dir_e     bearer_flow_dir;
  ds_bearer_mgr_state_e            bearer_state;
  /*------------------------------------------------------------------------*/

  if (dsmsg == NULL ||
  	   msgrtype != LTE_PDCPUL_RAB_REGISTER_CNF)
  {
    return FALSE;
  }

  eps_bearer_id = ((lte_pdcpul_rab_register_cnf_msg_s*)dsmsg)->eps_id;

  bearer_mgr_p = ds_bearer_mgr_get_bearer_from_eps_id((uint8)eps_bearer_id);
  ULOG_RT_PRINTF_1(ds_fwk_get_ulog_handle(), "UL REG CNF for bearer id: %d",
                                              eps_bearer_id);
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    /*----------------------------------------------------------------------
      It is possible that we are receiving this message after the call end
      due to disconnect was processed. In such a case, just ignore and ret
      DS3G_SUCCESS. Note that success is returned to the component that 
      handles incoming message. A success means only that the message was
      handled. It does not specify anything about the lower layers being
      set up right
    ----------------------------------------------------------------------*/
    return TRUE;
  }

  bearer_state = bearer_mgr_p->state;
  if(bearer_state != DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB &&
     bearer_state != DS_BEARER_MGR_STATE_PEND_RAB_REG)
  {
    /*----------------------------------------------------------------------
      Ignore the lower layer set up since it is not expected in the states
      other than the ones mentioned above
    ----------------------------------------------------------------------*/
    DS_ERROR_LOG_2("Recd wm set up conf - bearer state: =d Bearer inst: =d. Ignore",
                   bearer_mgr_p->state, bearer_mgr_p->index);;
    return TRUE;
  }

  if( ((lte_pdcpul_rab_register_cnf_msg_s*)dsmsg)->status != E_SUCCESS)
  {  
    /* registration with pdcp UL failed, move bearer to dormant and notify 
       pdn context that bearer setup is complete*/
    DS_MSG2(MSG_LEGACY_HIGH, "unexpected MSG Status: %d, "
            "RAB_REG_UL_CNF FAILURE for eps bearer id :%d, "
            "setting bearer to UP dormant",
            ((lte_pdcpul_rab_register_cnf_msg_s*)dsmsg)->status,
            eps_bearer_id);
    ULOG_RT_PRINTF_1(ds_fwk_get_ulog_handle(), "UL Reg status failed bearer id: %d",
                                              eps_bearer_id);

    /* if the bearer is coming out of dormancy, put it back to dormancy */
    bearer_mgr_p->state = DS_BEARER_MGR_STATE_UP_DORMANT;
    /*-------------------------------------------------------------------------  
        Register each enqueue function ptr.
      -------------------------------------------------------------------------*/
    (bearer_mgr_p->tx_wm.wm_item).each_enqueue_func_ptr =\
           (wm_cb_type)ds_bearer_mgr_reorig_dorm_call_cb;
    (bearer_mgr_p->tx_wm.wm_item).each_enqueue_func_data =\
           (void*) bearer_mgr_p;
    
    
    /* if the bearer is being setup, put it to dormant and notify pdn that bearer
       setup is complete */
    if (bearer_state == DS_BEARER_MGR_STATE_PEND_RAB_REG)
    {
      // if cp_data is enabled dont do anything else
      
      ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, 
                                   DS_BEARER_CONTROL_MASK);
      
      /*----------------------------------------------------------------------
         Notify PDN that the bearer setup is complete
      ----------------------------------------------------------------------*/
      ds_pdn_mgr_bearer_up_ind(bearer_mgr_p->pdn_mgr_ptr, bearer_mgr_p);
      ds_pdn_mgr_execute_cached_deactivation(bearer_mgr_p->pdn_mgr_ptr, bearer_mgr_p);
    }
    
    return FALSE;
  }

  DS_MSG1(MSG_LEGACY_HIGH,"RAB_REG_ULCNF for EPS bearer id:%d",eps_bearer_id);

  /*----------------------------------------------------------------------
    Get the flow direction for this bearer
  --------------------------------------------------------------------*/
  bearer_flow_dir = ds_eps_bearer_mgr_get_flow_direction(bearer_mgr_p);
  if (bearer_flow_dir == DS_EPS_FLOW_DIR_BIDIRECTIONAL)
  {
    ds_eps_bearer_mgr_register_dl_datapath(bearer_mgr_p);
  }
  else if (bearer_flow_dir == DS_EPS_FLOW_DIR_UPLINK)
  {
    ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, DS_BEARER_CONTROL_MASK);

    /*-----------------------------------------------------------------------
      If the bearer is coming out of dormancy, there is no need to call the 
      bearer_up_ind call back.
      If this is a new bearer (bearer state is COMING_UP), we need to call
      the bearer up ind call back
    -----------------------------------------------------------------------*/
    bearer_mgr_p->state = DS_BEARER_MGR_STATE_UP;
    ds_bearer_mgr_post_bearer_up_event(bearer_mgr_p);
    
    if (bearer_state == DS_BEARER_MGR_STATE_PEND_RAB_REG)
    {    
      /*----------------------------------------------------------------------
           Notify PDN that the bearer setup is complete
         ----------------------------------------------------------------------*/
      ds_pdn_mgr_bearer_up_ind(bearer_mgr_p->pdn_mgr_ptr, bearer_mgr_p);
    }

    bearer_mgr_p->eps_bearer->is_registered_pdcp = TRUE;
    bearer_mgr_p->eps_bearer->pdcp_reg_req_sent = FALSE; 
    ds_pdn_mgr_execute_cached_deactivation(bearer_mgr_p->pdn_mgr_ptr, bearer_mgr_p);
  }
  else
  {
    // invalid case
    DS_ERROR_LOG_1("Invalid flow dir for bearer :=d", bearer_mgr_p->index);
    return FALSE;
  }
 
  return TRUE;   
} /* ds_eps_bearer_mgr_pdcpul_rab_reg_cnf_msg_hdlr */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_GO_NULL_CMD

DESCRIPTION
  This function is used clean up a bearer, posts cmd to DS task
  
PARAMETERS 
 bearer_mgr_p  - pointer to the bearer context that is to be used
                        to tear down the call  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
 None
   
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_eps_bearer_mgr_go_null_cmd
(
  ds_bearer_mgr_s      *bearer_mgr_p
)
{
  ds_command_type   *cmd_ptr = NULL;
  uint8             *data_ptr = NULL;
  /*------------------------------------------------------------------------*/

  cmd_ptr = ds_allocate_cmd_buf(sizeof(uint8));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }
  
  cmd_ptr->hdr.cmd_id = DS_CMD_PDN_EPS_BEARER_GO_NULL;
  data_ptr = (uint8*)cmd_ptr->cmd_payload_ptr;
  *data_ptr = bearer_mgr_p->index;
  
  ds_put_cmd(cmd_ptr);  
  return;
} /*ds_eps_bearer_cntxt_go_null_cmd() */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_DEREGISTER_DL_WITH_PDCP

DESCRIPTION
  This function deregisters downlink WM with PDCP
  
PARAMETERS  
  bearer_mgr_p  - bearer context pointer
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_eps_bearer_mgr_deregister_dl_with_pdcp
(
  ds_bearer_mgr_s       *bearer_mgr_p
)
{
  lte_pdcp_rab_deregister_req_msg_s  pdcp_dl_rab_dereg_req_msg = {{0}};
  /*-----------------------------------------------------------------------*/
  ULOG_RT_PRINTF_1(ds_fwk_get_ulog_handle(), "DL Dereg request sent for bearer id: %d",
                                 bearer_mgr_p->bearer_id);
  pdcp_dl_rab_dereg_req_msg.eps_id = bearer_mgr_p->bearer_id;  

  ds_msgr_snd_msg_send
          (
            LTE_PDCPDL_RAB_DEREGISTER_REQ, 
            (msgr_hdr_struct_type*)(&pdcp_dl_rab_dereg_req_msg),
            sizeof(lte_pdcp_rab_deregister_req_msg_s)
          );
  return;                  
} /* ds_eps_bearer_mgr_deregister_dl_with_pdcp */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_DEREGISTER_UL_WITH_PDCP

DESCRIPTION
  This function deregisters uplink WM with PDCP
  
PARAMETERS  
  bearer_mgr_p  - bearer context pointer
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_eps_bearer_mgr_deregister_ul_with_pdcp
(
  ds_bearer_mgr_s       *bearer_mgr_p
)
{
  lte_pdcp_rab_deregister_req_msg_s  pdcp_ul_rab_dereg_req_msg = {{0}};
  /*----------------------------------------------------------------------*/
  ULOG_RT_PRINTF_1(ds_fwk_get_ulog_handle(), "UL Dereg request sent for bearer id: %d",
                                 bearer_mgr_p->bearer_id);
  pdcp_ul_rab_dereg_req_msg.eps_id = bearer_mgr_p->bearer_id;     
  
 ds_msgr_snd_msg_send
          (
            LTE_PDCPUL_RAB_DEREGISTER_REQ, 
            (msgr_hdr_struct_type*)(&pdcp_ul_rab_dereg_req_msg),
            sizeof(lte_pdcp_rab_deregister_req_msg_s)
          );

  return;                  
} /* ds_eps_bearer_mgr_deregister_ul_with_pdcp */

/*===========================================================================
FUNCTION  DS_EPS_BEARER_MGR_PDCPDL_RAB_DEREG_CNF_MSG_HDLR

DESCRIPTION
  This function handles the confirmation of the DL watermark deregistration 
  with PDCP
  
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_eps_bearer_mgr_pdcpdl_rab_dereg_cnf_msg_hdlr 
(
  msgr_umid_type              msgrtype,
  const msgr_hdr_struct_type *dsmsg
)
{
  eps_bearer_id_T          eps_bearer_id = 0;
  ds_bearer_mgr_s         *bearer_mgr_p = NULL;
  /*------------------------------------------------------------------------*/

  if (dsmsg == NULL)
  {
    return FALSE;
  }

  eps_bearer_id = ((lte_pdcp_rab_deregister_cnf_msg_s*)dsmsg)->eps_id;

  if(msgrtype != LTE_PDCPDL_RAB_DEREGISTER_CNF)
  {
    return FALSE;
  }
  
  DS_MSG1(MSG_LEGACY_HIGH,"RAB_DEREG_DL_CNF for eps bearer id :%d",eps_bearer_id);
  ULOG_RT_PRINTF_1(ds_fwk_get_ulog_handle(),"RAB_DEREG_DL_CNF for eps bearer id :%d",
                                 eps_bearer_id);

  /* First check if this confirmation belongs to cached bearer, if so we just 
     need to free the bearer context as the call was already cleaned up earlier */
  bearer_mgr_p = ds_bearer_mgr_get_cached_bearer_from_eps_id(eps_bearer_id);
  if(ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    DS_MSG0(MSG_LEGACY_HIGH,"EPS bearer belongs to cached bearer, "
            "freeing the pending bearer");

    ds_bearer_mgr_reset_cached_bearer(bearer_mgr_p);
  }

  bearer_mgr_p = ds_bearer_mgr_get_bearer_from_eps_id(eps_bearer_id);
  
  if( bearer_mgr_p &&
      bearer_mgr_p->eps_bearer &&
      bearer_mgr_p->state == DS_BEARER_MGR_STATE_DOWN )
  {
    bearer_mgr_p->eps_bearer->is_registered_pdcp = FALSE;
    ds_eps_bearer_mgr_go_null_cmd(bearer_mgr_p);
  }    

  if( bearer_mgr_p &&
      bearer_mgr_p->eps_bearer &&
      ((bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT) || 
      (bearer_mgr_p->state ==DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB)) )
  {
	ULOG_RT_PRINTF_0(ds_fwk_get_ulog_handle(),"Resetting PDCP Dereg Req Sent");
    bearer_mgr_p->eps_bearer->pdcp_dereg_req_sent = FALSE;
  }

  return TRUE; 
} /* ds_eps_bearer_mgr_pdcpdl_rab_dereg_cnf_msg_hdlr */

/*===========================================================================
FUNCTION  DS_EPS_BEARER_MGR_PDCPUL_RAB_DEREG_CNF_MSG_HDLR

DESCRIPTION
  This function handles the confirmation of the UL watermark deregistration 
  with PDCP
  
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_bearer_mgr_pdcpul_rab_dereg_cnf_msg_hdlr 
(
  msgr_umid_type              msgrtype,
  const msgr_hdr_struct_type *dsmsg
)
{
  eps_bearer_id_T          eps_bearer_id = 0;
  ds_bearer_mgr_s              *bearer_mgr_p = NULL;
  ds_eps_bearer_mgr_flow_dir_e  bearer_flow_dir;
  /*------------------------------------------------------------------------*/

  if (dsmsg == NULL)
  {
    return FALSE;
  }

  eps_bearer_id = ((lte_pdcp_rab_deregister_cnf_msg_s*)dsmsg)->eps_id;

  if(msgrtype != LTE_PDCPUL_RAB_DEREGISTER_CNF)
  {
    return FALSE;
  }
  
  DS_MSG1(MSG_LEGACY_HIGH,"RAB_DEREG_UL_CNF for eps bearer id :%d",
  	                       eps_bearer_id);
  ULOG_RT_PRINTF_1(ds_fwk_get_ulog_handle(),"RAB_DEREG_UL_CNF for eps bearer id :%d",
                            eps_bearer_id);
						   

  // First check if this confirmation belongs to cached bearer, if so no action needed
  bearer_mgr_p = ds_bearer_mgr_get_cached_bearer_from_eps_id(eps_bearer_id);
  if(ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    DS_MSG0(MSG_LEGACY_HIGH,"EPS bearer belongs to cached bearer, dereg DL");
    ds_eps_bearer_mgr_deregister_dl_with_pdcp(bearer_mgr_p);        
    return TRUE; 
  }

  bearer_mgr_p = ds_bearer_mgr_get_bearer_from_eps_id(eps_bearer_id);
  
  if( bearer_mgr_p &&
      bearer_mgr_p->eps_bearer &&
      bearer_mgr_p->eps_bearer->pdcp_dereg_req_sent == FALSE &&
      bearer_mgr_p->state == DS_BEARER_MGR_STATE_DOWN )
  {
    /*-----------------------------------------------------------------
         Get the flow direction for this bearer
       -----------------------------------------------------------------*/
    bearer_flow_dir = ds_eps_bearer_mgr_get_flow_direction(bearer_mgr_p);
    if (((lte_pdcpul_rab_register_cnf_msg_s*)dsmsg)->status == E_FAILURE)
    {
      bearer_mgr_p->eps_bearer->is_registered_pdcp = FALSE;
      ds_eps_bearer_mgr_go_null_cmd(bearer_mgr_p);
    }
    else if ( bearer_flow_dir == DS_EPS_FLOW_DIR_BIDIRECTIONAL)
    {
      /*----------------------------------------------------------------
            If bearer is in down state, then DL deregistration will happen as part of 
            UL dereg cnf from PDCP.
         -----------------------------------------------------------------*/  
      ds_eps_bearer_mgr_deregister_dl_with_pdcp(bearer_mgr_p);
    }
    else if(bearer_flow_dir == DS_EPS_FLOW_DIR_UPLINK )
    {
      bearer_mgr_p->eps_bearer->is_registered_pdcp = FALSE;
      ds_eps_bearer_mgr_go_null_cmd(bearer_mgr_p);
    }    
  }  
  
  if( bearer_mgr_p &&
      bearer_mgr_p->eps_bearer &&
      bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT )
  {
    bearer_flow_dir = ds_eps_bearer_mgr_get_flow_direction(bearer_mgr_p);
    if(bearer_flow_dir == DS_EPS_FLOW_DIR_UPLINK )
    {
      ULOG_RT_PRINTF_0(ds_fwk_get_ulog_handle(),"Resetting PDCP Dereg Req Sent");
      bearer_mgr_p->eps_bearer->pdcp_dereg_req_sent = FALSE;
    }  
  }
  
  return TRUE; 
} /* ds_eps_bearer_mgr_pdcpul_rab_dereg_cnf_msg_hdlr */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_TEAR_DOWN_PDN_CONNECTION

DESCRIPTION
  This function is used to tear down a PDN connection
  This function will decide if it has to send out a PDN Abort request or a 
  PDN Disconnect request 

PARAMETERS
  bearer_context_p  - pointer to the bearer context that is to be used
                      to end the call

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_bearer_mgr_tear_down_pdn_connection
(
  ds_pdn_mgr_s              *pdn_mgr_p,
  ds_bearer_mgr_s           *bearer_mgr_p
)
{
  emm_pdn_connectivity_abort_req_s_type   abort_req = {{0}};
  emm_pdn_disconnect_req_s_type           disconnect_req = {{0}};

  
  DS_MSG1(MSG_LEGACY_HIGH, "Tear down Pdn Connection. Bearer State: %d",
                           bearer_mgr_p->state);
  /*-------------------------------------------------------------------------
    Based on the state of the bearer, invoke functions into DSMGR

    Logic:

    DS_BEARER_MGR_STATE_COMING_UP    - Send PDN Abort
    DS_BEARER_MGR_STATE_PEND_RAB_REG/UP/DORMANT - Received Activate Bearer Ind 
                                           already and responded to it with
                                           SUCCESS. Send PDN disconnect
    OTHER   - Do nothing (This means that the 
                                           Activate Bearer Indication was
                                           rejected
  -------------------------------------------------------------------------*/
  switch(bearer_mgr_p->state)
  {
    case DS_BEARER_MGR_STATE_COMING_UP:
      abort_req.message_header.message_id = EMM_PDN_CONNECTIVITY_ABORT_REQ;
      abort_req.message_header.message_set = MS_MM_DS;      
      abort_req.message_header.message_len_lsb =
        (sizeof( emm_pdn_connectivity_abort_req_s_type ) - sizeof(IMH_T)) % 256;
      abort_req.message_header.message_len_msb =
        (sizeof( emm_pdn_connectivity_abort_req_s_type ) - sizeof(IMH_T)) / 256;
      abort_req.esm_hdr.sdf_id = bearer_mgr_p->sdf_id;

      /* Set the connection ID as invalid */
      abort_req.esm_hdr.connection_id = ESM_INVALID_CON_ID;
      if(ds_pdn_mgr_is_wwan_tech_suspended())
      {
        abort_req.esm_local_cause.valid = TRUE;
        abort_req.esm_local_cause.local_cause = ESM_LOCAL_DEACT;
      }
      ds_pdn_mgr_ds_to_nas_send_msg((void*)&abort_req);
      break;

    case DS_BEARER_MGR_STATE_PEND_RAB_REG:
    case DS_BEARER_MGR_STATE_UP:
    case DS_BEARER_MGR_STATE_UP_DORMANT:
    case DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB:    
      disconnect_req.message_header.message_id = EMM_PDN_DISCONNECT_REQ;
      disconnect_req.message_header.message_set = MS_MM_DS;      
      disconnect_req.message_header.message_len_lsb =
           (sizeof( emm_pdn_disconnect_req_s_type ) - sizeof(IMH_T)) % 256;
      disconnect_req.message_header.message_len_msb =
           (sizeof( emm_pdn_disconnect_req_s_type ) - sizeof(IMH_T)) / 256;
      disconnect_req.eps_bearer_id = bearer_mgr_p->bearer_id;
      disconnect_req.esm_hdr.sdf_id = bearer_mgr_p->sdf_id;      

      /* Set the connection ID as invalid */
      disconnect_req.esm_hdr.connection_id = ESM_INVALID_CON_ID;
      if(ds_pdn_mgr_is_wwan_tech_suspended())
      {
        disconnect_req.esm_local_cause.valid = TRUE;
        disconnect_req.esm_local_cause.local_cause = ESM_LOCAL_DEACT;
      }
      ds_pdn_mgr_ds_to_nas_send_msg((void*)&disconnect_req);
      break;

    default:
      break;
  }
  
  return;
} /* ds_eps_bearer_mgr_tear_down_pdn_connection */

/*===========================================================================
FUNCTION ds_eps_bearer_mgr_deactivate_last_bearer_if_needed

DESCRIPTION
  This function checks 
  1. If all bearers are down
  2. If all PDN's are down

  If one of the above is not fulfilled a fresh attach is not tried
 
PARAMETERS 
 None
 
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_eps_bearer_mgr_deactivate_last_bearer_if_needed
(
  void
)
{
  uint32                    index= 0;  
  ds_bearer_mgr_s          *temp_bearer_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*--------------------------------------------------------------------------
    If attach request is cached, attach SM can be in CACHED state or 
    Attach SM can be in NULL state and cached_seq_num would be valid, 
    this happens when Attach request is received while the PDN 
    is being torn down.
  ---------------------------------------------------------------------------*/
  if( (ds_eps_pdn_attach_sm_get_state() == DS_EPS_CACHED_GET_PDN_CONN_IND) ||
      (ds_eps_attach_sm_is_pdn_conn_req_pending()))
  {
    DS_MSG0(MSG_LEGACY_HIGH, 
    	       "GET_PDN_CONN_IND for LTE Attach is cached, "
    	       " checking for GOING_DOWN bearers");

    for( index = 0;
         index < DS_3GPP_MAX_BEARER_CONTEXT;\
         index++ )
    {
      temp_bearer_p = ds_bearer_mgr_get_instance_ptr(index);
  
      if(!ds_bearer_mgr_validate_bearer_ptr(temp_bearer_p))
      {
        continue;
      }

      /*------------------------------------------------------------------------ 
        If any bearer is not in down or going down or pending redial return
        If there exists any bearer UP/COMING_UP state then no point processing
        this function
        ------------------------------------------------------------------------*/
      if((temp_bearer_p->state != DS_BEARER_MGR_STATE_DOWN) && 
         (temp_bearer_p->state != DS_BEARER_MGR_STATE_PEND_REDIAL))
      {
        return;
      }             
    }
    /*-------------------------------------------------------------------------
      No other bearer is in GOING_DOWN state, Also check if any PDN context
      is in going down state,
      If PDN context is in going down state it means iface associated with the
      PDN needs to be brought down, Hence dont proprocess GET_PDN_CONN_IND here
      It will be processed when the PDN is cleaned up.
    -------------------------------------------------------------------------*/
    if (!ds_pdn_mgr_is_any_pdn_going_down())
    {
      DS_MSG0(MSG_LEGACY_HIGH,
      	       "No Pending bearers, ready to process GET_PDN_CONN_IND");
      ds_eps_pdn_attach_sm_handle_last_bearer_deact();    
    }
  }

  return;
} /*ds_eps_bearer_mgr_deactivate_last_bearer_if_needed*/

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_DEREG_PDCP

DESCRIPTION
  This function is used to deregister with PDCP.
  
PARAMETERS 
 bearer_mgr_p  - pointer to the bearer context
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
 None
   
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_eps_bearer_mgr_dereg_pdcp
(
  ds_bearer_mgr_s     *bearer_mgr_p
)
{
  ds_eps_bearer_mgr_flow_dir_e bearer_flow_dir;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if( bearer_mgr_p->eps_bearer &&
      bearer_mgr_p->eps_bearer->is_registered_pdcp )
  {
    /*----------------------------------------------------------------------
      Get the flow direction for this bearer
    --------------------------------------------------------------------*/
    bearer_flow_dir = ds_eps_bearer_mgr_get_flow_direction(bearer_mgr_p);
    if ( bearer_flow_dir == DS_EPS_FLOW_DIR_BIDIRECTIONAL)
    {
      ds_eps_bearer_mgr_deregister_ul_with_pdcp(bearer_mgr_p);

      /*----------------------------------------------------------------
      If bearer is in down state, then DL deregistration will happen as 
      part of UL dereg cnf from PDCP.
      If bearer is not in down state, then it is a RAB release scenario, and 
      DL can be dergistered right away.
      -----------------------------------------------------------------*/  
      if(bearer_mgr_p->state != DS_BEARER_MGR_STATE_DOWN)
      {
        ds_eps_bearer_mgr_deregister_dl_with_pdcp(bearer_mgr_p);
      }
      else
      {
        /* is_registered_pdcp should not be reset in this case */
        return;
      }
    }
    else if(bearer_flow_dir == DS_EPS_FLOW_DIR_UPLINK )
    {
      ds_eps_bearer_mgr_deregister_ul_with_pdcp(bearer_mgr_p);
    }
    else if(bearer_flow_dir == DS_EPS_FLOW_DIR_DOWNLINK )
    {
      ds_eps_bearer_mgr_deregister_dl_with_pdcp(bearer_mgr_p);
    }
    ULOG_RT_PRINTF_0(ds_fwk_get_ulog_handle(),"Setting PDCP Dereg Req Sent");
    bearer_mgr_p->eps_bearer->pdcp_dereg_req_sent = TRUE;
    bearer_mgr_p->eps_bearer->is_registered_pdcp = FALSE;
  }

  return;
}/* ds_eps_bearer_mgr_dereg_pdcp */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_DEREG_WMK

DESCRIPTION
  This function is used to deregister DS watermarks.
  
PARAMETERS 
 bearer_mgr_p  - pointer to the bearer context that is to be used
                        to tear down the call  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
 None
   
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_eps_bearer_mgr_dereg_wmk
(
  ds_bearer_mgr_s      *bearer_mgr_p
)
{
  if (!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    DS_MSG0(MSG_LEGACY_ERROR, "Bearer mgr is NULL/Invalid");
    return; 
  }

  // dereg with FWK
  ds_fwk_dereg_bearer_with_hw(bearer_mgr_p);
  if( bearer_mgr_p && bearer_mgr_p->eps_bearer)
  {
    DS_MSG3(MSG_LEGACY_HIGH,"Bearer registered with PDCP: %d" 
	                         "wwan tech suspend: %d "
							 "pdcp_dereg_req_sent: %d",
                             bearer_mgr_p->eps_bearer->is_registered_pdcp,
							 ds_pdn_mgr_is_wwan_tech_suspended(),
							 bearer_mgr_p->eps_bearer->pdcp_dereg_req_sent);
  }
  /*-------------------------------------------------------------------------
    DeRegister with PDCP if registered, else clean up bearer.
  -------------------------------------------------------------------------*/
  if( bearer_mgr_p && 
  	   bearer_mgr_p->eps_bearer &&
  	   bearer_mgr_p->eps_bearer->is_registered_pdcp &&
  	   ds_pdn_mgr_is_wwan_tech_suspended() == FALSE)
  {
    ds_eps_bearer_mgr_dereg_pdcp(bearer_mgr_p);
  }
  else if( bearer_mgr_p && 
       	   bearer_mgr_p->eps_bearer &&
       	   bearer_mgr_p->eps_bearer->pdcp_dereg_req_sent == TRUE)
  {
    // Dereg request has been sent, wait for confirmation before clean up.
    return;
  }
  else
  {
#ifndef FEATURE_NBIOT_NTN
    ds_eps_bearer_mgr_go_null_cmd(bearer_mgr_p);
#else
    // Process go NULL withput posting cmd - to support NTN context restoration
    ds_eps_bearer_mgr_go_null_hdlr(bearer_mgr_p->index);
#endif /* FEATURE_NBIOT_NTN */
  }

  return;
}/* ds_eps_bearer_mgr_dereg_wmk */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_GO_NULL_HDLR

DESCRIPTION
  This function is used clean up a bearer 
  
PARAMETERS 
 bearer_index  - Bearer index.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
 None
   
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_eps_bearer_mgr_go_null_hdlr
(
  uint8   bearer_index
)
{
  ds_bearer_mgr_s      *bearer_mgr_p = NULL;
  ds_pdn_mgr_s         *pdn_mgr_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bearer_mgr_p = ds_bearer_mgr_get_instance_ptr(bearer_index);
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    return;
  }

  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
   	return;
  }

  if( bearer_mgr_p->state != DS_BEARER_MGR_STATE_UP_DORMANT)
  {
    DS_MSG0(MSG_LEGACY_HIGH,
    	       "RAB not released, sending TRM_PRIORITY_LOW to Lower Layers");
    ds_eps_bearer_mgr_send_trm_priority_ind(bearer_mgr_p,
                                            LTE_TRM_PRIORITY_LOW );
  }
  
  /*---------------------------------------------------------------------------
    Tear Down flows and Also Add unbind flow to phys link
  ----------------------------------------------------------------------------*/
  ds_bearer_flow_manager_go_null(bearer_mgr_p);  

 /*--------------------------------------------------------------------------- 
   Release the memory for the flow context and clear the array entries for this
   bearer do this from a flow context context
  ----------------------------------------------------------------------------*/
  ds_bearer_flow_manager_delete_all_flows(bearer_mgr_p);

  if(bearer_mgr_p->is_default)
  {
    /*---------------------------------------------------------------------
     Unbind the phys_link from the default flow 
    ---------------------------------------------------------------------*/
    ds_flow_unbind_default_flows(pdn_mgr_p);
  }

  ds_pdn_mgr_bearer_down_ind(pdn_mgr_p, bearer_mgr_p,
  	                          pdn_mgr_p->net_down_reason);

  // to reset attach SM or to process cached attach request
  ds_eps_bearer_mgr_deactivate_last_bearer_if_needed();

  /*-------------------------------------------------------------------------- 
    If pdn context is in PENDING REDIAL state and if apn param chg teardown is
    set to TRUE, send out a PDN connectivity request out.
   
    We need not send out redial request in case of dedicated bearers since
    they have to be torn down
    --------------------------------------------------------------------------*/
  if (ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) &&
      ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) &&
      pdn_mgr_p->lte_params != NULL)
  {
    if((pdn_mgr_p->state == DS_PDN_MGR_STATE_PEND_REDIAL) && 
       (pdn_mgr_p->lte_params->apn_param_chg_teardown) &&
       (bearer_mgr_p->is_default == TRUE))
    {
      DS_MSG0(MSG_LEGACY_HIGH,
      	       "Sending out PDN connectivity req, Apn param chg");
      ds_eps_pdn_handle_apn_param_chg_redial(pdn_mgr_p,bearer_mgr_p);
    }

    if((pdn_mgr_p->state == DS_PDN_MGR_STATE_PEND_REDIAL) &&
        pdn_mgr_p->redial_reason == DS_PDN_MGR_REDIAL_OP_PCO_CHG)
    {
      DS_MSG0(MSG_LEGACY_HIGH,
      	       "Redial because of OP PCO chg");
      pdn_mgr_p->redial_reason = DS_PDN_MGR_NO_REDIAL;
      pdn_mgr_p->state = DS_PDN_MGR_STATE_COMING_UP;

      // Need to add OP PCO change handling here
    }   
  }  

  return;
} /*ds_eps_bearer_mgr_go_null_hdlr() */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_RE_REGISTER_WM_WITH_PDCP

DESCRIPTION
  This function is used to reestablish the Data path when Flow direction has got modified due to Bearer Modify indication
  from NW

PARAMETERS
  flow_dir: previous firection
  modified_flow_dir :new direction after modify
  call_id: Callid for which modify occured

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is all is successful, 
  FALSE otherwise. 
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_bearer_mgr_re_register_wm_with_pdcp
(
  ds_bearer_mgr_s                     *bearer_mgr_p,
  ds_eps_bearer_mgr_flow_dir_e         bearer_flow_dir,
  ds_eps_bearer_mgr_flow_dir_e         modified_bearer_flow_dir
)
{
  boolean                  ul_reg = FALSE;
  boolean                  ul_dereg = FALSE;
  boolean                  dl_reg = FALSE;
  boolean                  dl_dereg = FALSE;
  /*-----------------------------------------------------------------------*/ 
  
  switch(bearer_flow_dir)
  {
    case DS_EPS_FLOW_DIR_DOWNLINK:
      if(modified_bearer_flow_dir == DS_EPS_FLOW_DIR_UPLINK)
      {
        dl_dereg = TRUE;
        ul_reg = TRUE;
      }
      else if(modified_bearer_flow_dir == DS_EPS_FLOW_DIR_BIDIRECTIONAL)
      {
        ul_reg = TRUE;
      }
      break;
      
    case DS_EPS_FLOW_DIR_UPLINK:
      if(modified_bearer_flow_dir == DS_EPS_FLOW_DIR_DOWNLINK)
      {
        ul_dereg = TRUE;
        dl_reg = TRUE;
      }
      else if(modified_bearer_flow_dir == DS_EPS_FLOW_DIR_BIDIRECTIONAL)
      {
        dl_reg = TRUE;
      }
      break;
      
    case DS_EPS_FLOW_DIR_BIDIRECTIONAL:
      if(modified_bearer_flow_dir == DS_EPS_FLOW_DIR_DOWNLINK)
      {
        ul_dereg = TRUE;
      }
      else if(modified_bearer_flow_dir == DS_EPS_FLOW_DIR_UPLINK)
      {
        dl_dereg = TRUE;
      }
      break;
      
    default:
      break;
  }/*switch*/

  if(dl_dereg)
  {    
    ds_eps_bearer_mgr_deregister_dl_with_pdcp(bearer_mgr_p);
    dsm_empty_queue(&(bearer_mgr_p->rx_wm.wm_item));
  }
  else if(ul_dereg)
  {
    ds_eps_bearer_mgr_deregister_ul_with_pdcp(bearer_mgr_p);
    dsm_empty_queue(&(bearer_mgr_p->tx_wm.wm_item));
    
    ds_pdn_cp_data_flush_num_mo_ex_data_cnt(bearer_mgr_p->index);
    ds_pdn_flow_mgr_update_uplink_flow_capability(bearer_mgr_p, FALSE);
  }
  
  if(ul_reg)
  {
     /* Updating the Bearer state to RAB Registering */
    bearer_mgr_p->state = DS_BEARER_MGR_STATE_PEND_RAB_REG;

    DS_MSG1(MSG_LEGACY_HIGH,
            "Registering UL with PDCP, bearer %d", bearer_mgr_p->index);

    /*---------------------------------------------------------------------  
      Since previous direction was DL only, UL WM may have some stale data
      present, we need to flush it before establishing the path with PDCP
    ---------------------------------------------------------------------*/
    dsm_empty_queue(&(bearer_mgr_p->tx_wm.wm_item));
    ds_pdn_cp_data_flush_num_mo_ex_data_cnt(bearer_mgr_p->index);
  
    ds_eps_bearer_mgr_register_ul_datapath(bearer_mgr_p);
    
    /* Allow UL Flow as flow direction is UL  */   
    ds_pdn_flow_mgr_update_uplink_flow_capability(bearer_mgr_p, TRUE);
  }
  else if(dl_reg)
  {
     /* registering DL path */ 
     /* Updating the Bearer state to RAB Regestering */
     bearer_mgr_p->state = DS_BEARER_MGR_STATE_PEND_RAB_REG;
     ds_eps_bearer_mgr_register_dl_datapath(bearer_mgr_p);
  }
  
  return;
}/* ds_eps_bearer_mgr_re_register_wm_with_pdcp */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_GET_DATA_ACTIVITY

DESCRIPTION
  This function is used to get the data activity per bearer

PARAMETERS
  bearer_mgr_p     - pointer to the bearer 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if any activity
  FALSE otherwise. 
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_bearer_mgr_get_data_activity
(
  ds_bearer_mgr_s  *bearer_mgr_p,
  ds_pdn_mgr_s     *pdn_mgr_p
)
{
  boolean                     ret_val = FALSE;
  uint64                      tx_bytes_current = 0;
  uint64                      rx_bytes_current = 0;
  ds_pdn_mgr_inst_type_s     *v4_inst_p = NULL;
  ds_pdn_mgr_inst_type_s     *v6_inst_p = NULL;
  lte_pdcpdl_activity_stats_s pdcp_dl_stats;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  v4_inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4];
  v6_inst_p = pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6];
  

  /* Get V4 iface tx stats */
//    tx_bytes_current += tx_bytes_temp;

  /* Get V6 iface stats */
//    tx_bytes_current += tx_bytes_temp;

  /*------------------------------------------------------------------------
    For DL stats, query PDCP only if the bearer is in UP state
  ------------------------------------------------------------------------*/
  if(bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP)
  {
    if(lte_pdcpdl_get_activity_statistics(
    	                      bearer_mgr_p->bearer_id,
                                          &pdcp_dl_stats) == TRUE)
    {
      rx_bytes_current = (uint64)pdcp_dl_stats.num_bytes;

      /*--------------------------------------------------------------------
        If the current stats are the same as the one that has been cached in
        the bearer context, there has been no data transfer.
        Set the ret_val to FALSE indicating data inactivity
      ----------------------------------------------------------------------*/
      if( ((tx_bytes_current != 0) && (tx_bytes_current != bearer_mgr_p->tx_bytes)) ||
          ((rx_bytes_current != 0) && (rx_bytes_current != bearer_mgr_p->rx_bytes)) )
      {
        ret_val = TRUE;
      }
    } //PDCP api returned sucess
    else
    {
      ret_val = FALSE;
    }
  }//bearer context is UP
  else
  {
    /*----------------------------------------------------------------------
      Set the Rx bytes count to zero. This is needed as PDCP would have lost
      its counters. Hence we are synchronizing with PDCP so that when the
      bearer comes up again, DS and PDCP start from 0
    ----------------------------------------------------------------------*/
    rx_bytes_current = 0;

    /*--------------------------------------------------------------------
      Check data activity on uplink only
    ----------------------------------------------------------------------*/
    if((tx_bytes_current != 0) && 
       (tx_bytes_current != bearer_mgr_p->tx_bytes))
    {
      /*----------------------------------------------------------------------
        There has been data activity on UL. Although it cannot be established
        if the data activity was before or after the bearer went dormant, we
        take this as data activity
      ----------------------------------------------------------------------*/
      ret_val = TRUE;
    }
  }

  /*----------------------------------------------------------------------
    Update the stats in the bearer context with the current stats.
    This can be done irrespective of whether data transfer happened or not
  ----------------------------------------------------------------------*/
  bearer_mgr_p->tx_bytes = tx_bytes_current;
  bearer_mgr_p->rx_bytes = rx_bytes_current;

  DS_MSG1(MSG_LEGACY_HIGH, "Detected Data Activity %d", ret_val);
  return ret_val;
} /* ds_eps_bearer_cntxt_get_data_trfr_activity */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_MAP_EMM_CAUSE_CODES_TO_DS

DESCRIPTION
  Converts NAS EMM cause codes to DS net down reason type.  

PARAMETERS
  *ds_down_reason  - ptr to location holding converted cause-code for DS
  emm_cause          - ESM cause code to be converted

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_bearer_mgr_map_emm_cause_codes_to_ds
(
  ds_net_down_reason_type *ds_down_reason,
  emm_failure_type         emm_failure_cause
)
{
  /*------------------------------------------------------------------------- 
       If a DS Net Down reason is for EMM/ESM Local Cause codes, add to
       Net down reason category(type): 3
    -------------------------------------------------------------------------*/
  switch(emm_failure_cause.cause_type)
  {
    case LTE_NAS_IRAT_CONN_EST_FAILURE:
    {
      switch(emm_failure_cause.cause.est_status)
      {
        case LTE_RRC_CONN_EST_FAILURE:
           *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE;
           break;
         case LTE_RRC_CONN_EST_FAILURE_ABORTED:
           *ds_down_reason = 
              DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_ABORTED;
           break;
         case LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED:
           *ds_down_reason = DS_NET_DOWN_REASON_EMM_ACCESS_BARRED;
           break;  
         case LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB:
           *ds_down_reason = DS_NET_DOWN_REASON_EMM_ACCESS_BARRED_EAB;
           break;
         case LTE_RRC_CONN_EST_FAILURE_CELL_RESEL:
           *ds_down_reason = 
              DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_CELL_RESEL;
           break;    
         case LTE_RRC_CONN_EST_FAILURE_CONFIG_FAILURE:
           *ds_down_reason = 
              DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_CONFIG_FAILURE;
           break;    
         case LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED:
           *ds_down_reason = 
              DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_TIMER_EXPIRED;
           break;    
         case LTE_RRC_CONN_EST_FAILURE_LINK_FAILURE:
           *ds_down_reason = 
              DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_LINK_FAILURE;
           break;  
         case LTE_RRC_CONN_EST_FAILURE_NOT_CAMPED:
           *ds_down_reason = 
              DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_NOT_CAMPED;
           break;    
         case LTE_RRC_CONN_EST_FAILURE_SI_FAILURE:
           *ds_down_reason = 
              DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_SI_FAILURE;
           break;    
         case LTE_RRC_CONN_EST_FAILURE_CONN_REJECT:
           *ds_down_reason = 
              DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_CONN_REJECT;
           break;    
         case LTE_RRC_CONN_EST_FAILURE_REQ_ERROR:
           *ds_down_reason = 
              DS_NET_DOWN_REASON_RRC_CONN_EST_FAILURE_REQ_ERROR;
           break;    
         case LTE_RRC_CONN_EST_FAILURE_TAI_CHANGE:
           *ds_down_reason = 
              DS_NET_DOWN_REASON_RRC_CONN_EST_FAILURE_TAI_CHANGE;
           break;    
         case LTE_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE:
           *ds_down_reason = 
              DS_NET_DOWN_REASON_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE;
           break;    
         default:
           *ds_down_reason = DS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE;
           break;
      } /* switch(detailed_cause) */
      break;
    } /* case LTE_NAS_IRAT_CONN_EST_FAILURE*/
  
    case LTE_NAS_IRAT_CONN_REL:
    {
      switch(emm_failure_cause.cause.rel_reason)
      {
        case LTE_RRC_CONN_REL_NORMAL:
          *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_REL_NORMAL;
          break;
        case LTE_RRC_CONN_REL_RLF:
          *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_REL_RLF;
          break;
        case LTE_RRC_CONN_REL_CRE_FAILURE:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_LRRC_CONN_REL_CRE_FAILURE;
          break;
        case LTE_RRC_CONN_REL_OOS_DURING_CRE:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_LRRC_CONN_REL_OOS_DURING_CRE;
          break;
        case LTE_RRC_CONN_REL_ABORTED:
          *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_REL_ABORTED;
          break;
        case LTE_RRC_CONN_REL_SIB_READ_ERROR:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_LRRC_CONN_REL_SIB_READ_ERROR;
          break;
        case LTE_RRC_CONN_REL_ABORTED_IRAT_SUCCESS:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_RRC_CONN_REL_ABORTED_IRAT_SUCCESS;
          break;
        case LTE_RRC_CONN_REL_RLF_SEC_NOT_ACTIVE:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_RRC_CONN_REL_RLF_SEC_NOT_ACTIVE;
          break;
        case LTE_RRC_CONN_REL_IRAT_TO_LTE_ABORTED:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_RRC_CONN_REL_IRAT_TO_LTE_ABORTED;
          break;
        case LTE_RRC_CONN_REL_IRAT_FROM_LTE_TO_G_CCO_SUCCESS:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_RRC_CONN_REL_IRAT_FROM_LTE_TO_G_CCO_SUCCESS;
          break;
        case LTE_RRC_CONN_REL_IRAT_FROM_LTE_TO_G_CCO_ABORTED:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_RRC_CONN_REL_IRAT_FROM_LTE_TO_G_CCO_ABORTED;
          break;
        default:
           *ds_down_reason = DS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE;
           break;
      } /* switch(detailed_cause) */
      break;
    } /* case LTE_NAS_IRAT_CONN_REL*/
  
    case LTE_NAS_IRAT_UL_MSG_CNF_FAILURE:
    {
      switch(emm_failure_cause.cause.cnf_status)
      {          
        case LTE_RRC_UL_DATA_CNF_FAILURE_TXN:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_TXN;
          break;
        case LTE_RRC_UL_DATA_CNF_FAILURE_HO:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_HO;
          break;
        case LTE_RRC_UL_DATA_CNF_FAILURE_CONN_REL:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_CONN_REL;
          break;
        case LTE_RRC_UL_DATA_CNF_FAILURE_RLF:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_RLF;
          break;
        case LTE_RRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN;
          break;
        default:
           *ds_down_reason = DS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE;
           break;
      } /* switch(detailed_cause) */
 
      break;
    } /* case LTE_NAS_IRAT_UL_MSG_CNF_FAILURE*/
  
    case LTE_NAS_IRAT_SR_REJECT:
    {
      switch(emm_failure_cause.cause.emm_cause)
      {    
        case LTE_NAS_IMSI_UNKNOWN_IN_HSS:
          *ds_down_reason = DS_NET_DOWN_REASON_IMSI_UNKNOWN_IN_HSS;
          break;
        case LTE_NAS_ILLEGAL_UE:
          *ds_down_reason = DS_NET_DOWN_REASON_ILLEGAL_MS;
          break;
        case LTE_NAS_IMEI_NOT_ACCEPTED:
          *ds_down_reason = DS_NET_DOWN_REASON_IMEI_NOT_ACCEPTED;
          break;
        case LTE_NAS_ILLEGAL_ME:
          *ds_down_reason = DS_NET_DOWN_REASON_ILLEGAL_ME;
          break;
        case LTE_NAS_EPS_SERVICES_NOT_ALLOWED:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_EPS_SERVICES_NOT_ALLOWED;
          break;
        case LTE_NAS_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_EPS_SERVICES_AND_NON_EPS_SERVICES_NOT_ALLOWED;
          break;
        case LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK;
          break;
        case LTE_NAS_IMPLICITLY_DETACHED:
          *ds_down_reason = DS_NET_DOWN_REASON_IMPLICITLY_DETACHED;
          break;
        case LTE_NAS_PLMN_NOT_ALLOWED:
          *ds_down_reason = DS_NET_DOWN_REASON_PLMN_NOT_ALLOWED;
          break;
        case LTE_NAS_TRACKING_AREA_NOT_ALLOWED:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_TRACKING_AREA_NOT_ALLOWED;
          break;
        case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA;
          break;
        case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_IN_PLMN:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_EPS_SERVICES_NOT_ALLOWED_IN_PLMN;
          break;
        case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_NO_SUITABLE_CELLS_IN_TRACKING_AREA;
          break;
        case LTE_NAS_MSC_TEMPORARILY_NOT_REACHABLE:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_MSC_TEMPORARILY_NOT_REACHABLE;
          break;
        case LTE_NAS_NW_FAILURE:
          *ds_down_reason = DS_NET_DOWN_REASON_NETWORK_FAILURE;
          break;
        case LTE_NAS_CS_DOMAIN_NOT_AVAILABLE:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_CS_DOMAIN_NOT_AVAILABLE;
          break;
        case LTE_NAS_ESM_FAILURE:
          *ds_down_reason = DS_NET_DOWN_REASON_ESM_FAILURE;
          break;
        case LTE_NAS_MAC_FAILURE:
          *ds_down_reason = DS_NET_DOWN_REASON_MAC_FAILURE;
          break;
        case LTE_NAS_SYNCH_FAILURE:
          *ds_down_reason = DS_NET_DOWN_REASON_SYNCH_FAILURE;
          break;
        case LTE_NAS_CONGESTION:
          *ds_down_reason = DS_NET_DOWN_REASON_CONGESTION;
          break;
        case LTE_NAS_UE_SECURITY_CAPABILITIES_MISMATCH:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_UE_SECURITY_CAPABILITIES_MISMATCH;
          break;
        case LTE_NAS_SECURITY_MODE_REJ_UNSPECIFIED:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_SECURITY_MODE_REJ_UNSPECIFIED;
          break;
        case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_NOT_AUTHORIZED_FOR_THIS_CSG;
          break;
        case LTE_NON_EPS_AUTH_UNACCEPTABLE:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_NON_EPS_AUTH_UNACCEPTABLE;
          break;
        case LTE_NAS_CS_FALLBACK_CALL_EST_NOT_ALLOWED:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_CS_FALLBACK_CALL_EST_NOT_ALLOWED;
          break;
        case LTE_NAS_CS_DOMAIN_TEMP_NOT_AVAILABLE:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_CS_DOMAIN_NOT_AVAILABLE;
          break;
        case LTE_NAS_NO_EPS_BEARER_CONTEXT_ACTIVATED:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_NO_EPS_BEARER_CONTEXT_ACTIVATED;
          break;
        case LTE_NAS_SEMANTICALLY_INCORRECT_MSG:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_MESSAGE_INCORRECT_SEMANTIC;
          break;
        case LTE_NAS_INVALID_MANDATORY_INFO:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_INVALID_MANDATORY_INFO;
          break;
        case LTE_NAS_NON_EXISTENT_MSG_TYPE:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_MESSAGE_TYPE_UNSUPPORTED;
          break;
        case LTE_NAS_MSG_TYPE_INCOMPATIBLE_WITH_PROTOCOL_STATE:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_MSG_TYPE_NONCOMPATIBLE_STATE;
          break;
        case LTE_NAS_NON_EXISTANT_INFO:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_UNKNOWN_INFO_ELEMENT;
          break;
        case LTE_NAS_CONDITIONAL_IE_ERROR:
          *ds_down_reason = DS_NET_DOWN_REASON_CONDITIONAL_IE_ERROR;
          break;
        case LTE_NAS_MSG_INCOMPATIBLE_WITH_PROTOCOL_STATE:
          *ds_down_reason = 
             DS_NET_DOWN_REASON_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE;
          break;
        case LTE_NAS_PROTOCOL_ERROR_UNSPECIFIED:
          *ds_down_reason = DS_NET_DOWN_REASON_PROTOCOL_ERROR;
          break;
#ifdef FEATURE_NBIOT_NTN
        case LTE_NAS_PLMN_NOT_ALLOWED_IN_CURR_UE_LOC:
          *ds_down_reason = DS_NET_DOWN_REASON_PLMN_NOT_ALLOWED;
          break;
#endif /* FEATURE_NBIOT_NTN */
        default:
          *ds_down_reason = DS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE;
          break;
      } /* switch(detailed_cause) */

      break;
    } /* case LTE_NAS_IRAT_SR_REJECT */

    case LTE_NAS_IRAT_INVALID_STATE: 
      *ds_down_reason = DS_NET_DOWN_REASON_EMM_INVALID_STATE;
      break;

    case LTE_NAS_IRAT_ACCESS_BARRING:
      *ds_down_reason = DS_NET_DOWN_REASON_EMM_ACCESS_BARRED;
       break;

    case LTE_NAS_IRAT_T3417_EXPIRY:
      *ds_down_reason = DS_NET_DOWN_REASON_EMM_T3417_EXPIRED;
      break;

    case LTE_NAS_IRAT_LOWER_LAYER_FAILURE:
      *ds_down_reason = DS_NET_DOWN_REASON_NAS_LAYER_FAILURE;
      break;
#ifdef FEATURE_NBIOT_NTN
    case LTE_NAS_IRAT_ATTACH_REJECT:
      switch(emm_failure_cause.cause.emm_cause)
      { 
        case LTE_NAS_PLMN_NOT_ALLOWED_IN_CURR_UE_LOC:
          *ds_down_reason = DS_NET_DOWN_REASON_PLMN_NOT_ALLOWED;
          break;
        default:
          *ds_down_reason = DS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE;
          break;
      }
      break;
#endif /* FEATURE_NBIOT_NTN */
    default:
    {
      *ds_down_reason = DS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE;
      break;
    }
  } /* switch(mode_info_p->info.lte_call.emm_failure_cause.cause_type) */
}


/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_MAP_ESM_CAUSE_CODES_TO_DS

DESCRIPTION
  Converts NAS ESM cause codes to DS net down reason type.  

PARAMETERS
  *ds_down_reason  - ptr to location holding converted cause-code for DS
  esm_cause           - ESM cause code to be converted
  local_esm_cause   - local ESM cause code to be converted

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/
void ds_eps_bearer_mgr_map_esm_cause_codes_to_ds
(
  ds_net_down_reason_type *ds_down_reason,
  esm_cause_T              esm_cause,
  lte_nas_local_cause_T    esm_local_cause 
)
{
  /*------------------------------------------------------------------------- 
       If a DS Net Down reason is for ESM Cause codes, add to
       Net down reason category(type): 6
       The value field should be the same as the ESM Cause code value.
       For example: DS_NET_DOWN_REASON_IP_V4_ONLY_ALLOWED - Type: 6, Value: 50
    -------------------------------------------------------------------------*/

  if(esm_cause.valid)
  {   
    switch(esm_cause.esm_cause)
    {
      case NAS_ESM_OPTR_DETERMINED_BARRING:
        *ds_down_reason = DS_NET_DOWN_REASON_OPERATOR_DETERMINED_BARRING;
        break;
      case NAS_ESM_INSUFFICIENT_RESOURCES:
        *ds_down_reason = DS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES;
        break;
      case NAS_ESM_UNKNOWN_MISSING_APN:
        *ds_down_reason = DS_NET_DOWN_REASON_UNKNOWN_APN;
        break;
      case NAS_ESM_UNKNOWN_PDN_TYPE:
        *ds_down_reason = DS_NET_DOWN_REASON_UNKNOWN_PDP;
        break;
      case NAS_ESM_USER_AUTHENTICATION_FAILED:
        *ds_down_reason = DS_NET_DOWN_REASON_AUTH_FAILED;
        break;
      case NAS_ESM_OPN_REJ_SERVICING_GW_OR_PDN_GW:
        *ds_down_reason = DS_NET_DOWN_REASON_GGSN_REJECT;
        break;
      case NAS_ESM_ACTIVATION_REJECTED:
        *ds_down_reason = DS_NET_DOWN_REASON_ACTIVATION_REJECT;
        break;
      case NAS_ESM_SERV_OPT_NOT_SUPPORTED:
        *ds_down_reason = DS_NET_DOWN_REASON_OPTION_NOT_SUPPORTED;
        break;
      case NAS_ESM_REQ_SERV_OPT_NOT_SPECIFIED:
        *ds_down_reason = DS_NET_DOWN_REASON_OPTION_UNSUBSCRIBED;
        break;
      case NAS_ESM_SERV_OPT_TEMP_OUT_OF_ORDER:
        *ds_down_reason = DS_NET_DOWN_REASON_OPTION_TEMP_OOO;
        break;
      case NAS_ESM_PTI_ALREADY_IN_USE:
        *ds_down_reason = DS_NET_DOWN_REASON_NSAPI_ALREADY_USED;
        break;
      case NAS_ESM_REGULAR_DEACTIVATION:
        *ds_down_reason = DS_NET_DOWN_REASON_REGULAR_DEACTIVATION;
        break; 
      case NAS_ESM_EPS_QOS_NOT_ACCEPTED:
        *ds_down_reason = DS_NET_DOWN_REASON_QOS_NOT_ACCEPTED;
        break; 
      case NAS_ESM_NETWORK_FAILURE:
        *ds_down_reason = DS_NET_DOWN_REASON_NETWORK_FAILURE;
        break;
      case NAS_ESM_REACTIVATION_REQUIRED:
        *ds_down_reason = DS_NET_DOWN_REASON_UMTS_REATTACH_REQ;
        break;
      case NAS_ESM_FEATURE_NOT_SUPPORTED:
        *ds_down_reason = DS_NET_DOWN_REASON_FEATURE_NOT_SUPPORTED;
        break; 
      case NAS_ESM_SEMANTIC_ERR_IN_TFT_OPERATION:
        *ds_down_reason = DS_NET_DOWN_REASON_TFT_SEMANTIC_ERROR;
        break;
      case NAS_ESM_SYNTACTICAL_ERR_IN_TFT_OPERATION:
        *ds_down_reason = DS_NET_DOWN_REASON_TFT_SYNTAX_ERROR;
        break;
      case NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT:
        *ds_down_reason = DS_NET_DOWN_REASON_UNKNOWN_PDP_CONTEXT;
        break;
      case NAS_ESM_SYMANTIC_ERR_IN_PKT_FILTER:
        *ds_down_reason = DS_NET_DOWN_REASON_FILTER_SEMANTIC_ERROR;
        break;
      case NAS_ESM_SYNTACTICAL_ERR_IN_PKT_FILTER:
        *ds_down_reason = DS_NET_DOWN_REASON_FILTER_SYNTAX_ERROR;
        break;
      case NAS_ESM_EPS_BEARER_CONT_WO_TFT_ALREADY_ACT:
        *ds_down_reason = DS_NET_DOWN_REASON_PDP_WITHOUT_ACTIVE_TFT;
        break;     
      case NAS_ESM_PDN_TYPE_IPV4_ONLY_ALLOWED:
        *ds_down_reason = DS_NET_DOWN_REASON_IP_V4_ONLY_ALLOWED;
        break;
      case NAS_ESM_PDN_TYPE_IPV6_ONLY_ALLOWED:
        *ds_down_reason = DS_NET_DOWN_REASON_IP_V6_ONLY_ALLOWED;
        break;
      case NAS_ESM_SINGLE_ADR_BEARERS_ONLY_ALLOWED:
        *ds_down_reason = DS_NET_DOWN_REASON_SINGLE_ADDR_BEARER_ONLY;
        break;
      case NAS_ESM_PDN_TYPE_IPv4v6_ONLY_ALLOWED:
        *ds_down_reason = DS_NET_DOWN_REASON_IP_V4V6_ONLY_ALLOWED;
        break;
      case NAS_ESM_PDN_TYPE_NON_IP_ONLY_ALLOWED:
        *ds_down_reason = DS_NET_DOWN_REASON_NON_IP_ONLY_ALLOWED;
        break;
      case NAS_ESM_MAX_PDP_CONTEXT_REACHED:
        *ds_down_reason = DS_NET_DOWN_REASON_MAX_ACTIVE_PDP_CONTEXT_REACHED;
        break;
      case NAS_ESM_APN_NOT_SUPPORTED_IN_PLMN_RAT_COMBINATION:
        *ds_down_reason = DS_NET_DOWN_REASON_UNSUPPORTED_APN_IN_CURRENT_PLMN;
        break;       
      case NAS_ESM_ESM_INFO_NOT_RECEIVED:
        *ds_down_reason = DS_NET_DOWN_REASON_ESM_INFO_NOT_RECEIVED;
        break;
      case NAS_ESM_PDN_CONN_DOES_NOT_EXIST:
        *ds_down_reason = DS_NET_DOWN_REASON_PDN_CONN_DOES_NOT_EXIST;
        break;
      case NAS_ESM_MULTIPLE_PDN_CONN_FOR_GIVEN_APN_NOT_ALLOWED:
        *ds_down_reason = DS_NET_DOWN_REASON_MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED;
        break;
      case NAS_ESM_COLLISION_WITH_NW_INIT_REQ:
        *ds_down_reason = DS_NET_DOWN_REASON_COLLISION_WITH_NW_INIT_REQ;
        break;
      case NAS_ESM_UNSUPPORTED_QCI_VALUE:
        *ds_down_reason = DS_NET_DOWN_REASON_UNSUPPORTED_QCI_VALUE;
        break;     
      case NAS_ESM_BEARER_HANDLING_NOT_SUPPORTED:
        *ds_down_reason = DS_NET_DOWN_REASON_BEARER_HANDLING_NOT_SUPPORTED;
        break;      
      case NAS_ESM_INVALID_PTI_VALUE:
        *ds_down_reason = DS_NET_DOWN_REASON_INVALID_TRANSACTION_ID;
        break;
      case NAS_ESM_SYMANTICALLY_INVALID_MSG:
        *ds_down_reason = DS_NET_DOWN_REASON_MESSAGE_INCORRECT_SEMANTIC;
        break;
      case NAS_ESM_INVALID_MANDATORY_INFORMATION:
        *ds_down_reason = DS_NET_DOWN_REASON_INVALID_MANDATORY_INFO;
        break;
      case NAS_ESM_MSG_TYPE_NON_EXISTANT:
        *ds_down_reason = DS_NET_DOWN_REASON_MESSAGE_TYPE_UNSUPPORTED;
        break;
      case NAS_ESM_MSG_TYPE_INCOMPATIBLE_PROT_STATE:
        *ds_down_reason = DS_NET_DOWN_REASON_MSG_TYPE_NONCOMPATIBLE_STATE;
        break;
      case NAS_ESM_INFO_ELEMENT_NON_EXISTANT:
        *ds_down_reason = DS_NET_DOWN_REASON_UNKNOWN_INFO_ELEMENT;
        break;
      case NAS_ESM_CONDITIONAL_IE_ERR:
        *ds_down_reason = DS_NET_DOWN_REASON_CONDITIONAL_IE_ERROR;
        break;
      case NAS_ESM_MSG_INCOMPATIBLE_WITH_PRO_STATE:
        *ds_down_reason = DS_NET_DOWN_REASON_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE;
        break;
      case NAS_ESM_PROTOCOL_ERROR:
        *ds_down_reason = DS_NET_DOWN_REASON_PROTOCOL_ERROR;
        break;
      case NAS_ESM_APN_RESTRICT_VAL_INCOMPAT_WITH_ACT_CONTEXT:
        *ds_down_reason = DS_NET_DOWN_REASON_APN_TYPE_CONFLICT;
        break;
      default:
        *ds_down_reason = DS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE;
        break;
    }
  } /* ESM Cause codes*/
  else if (esm_local_cause.valid)
  {
    switch(esm_local_cause.local_cause)
    {
      case LTE_NAS_ILLEGAL_UE_LOCAL:
        *ds_down_reason = DS_NET_DOWN_REASON_ILLEGAL_MS;
        break;

      case LTE_NAS_ILLEGAL_ME_LOCAL:
        *ds_down_reason = DS_NET_DOWN_REASON_ILLEGAL_ME;
        break;

      case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_LOCAL:
        *ds_down_reason = DS_NET_DOWN_REASON_DETACH_EPS_SERVICES_NOT_ALLOWED;
        break;

      case LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW_LOCAL:
        *ds_down_reason = DS_NET_DOWN_REASON_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK;
        break;

      case LTE_NAS_IMPLICITLY_DETACHED_LOCAL:
        *ds_down_reason = DS_NET_DOWN_REASON_IMPLICITLY_DETACHED;
        break;

      case LTE_NAS_PLMN_NOT_ALLOWED_LOCAL:
        *ds_down_reason = DS_NET_DOWN_REASON_PLMN_NOT_ALLOWED;
        break;

      case LTE_NAS_TRACKING_AREA_NOT_ALLOWED_LOCAL:
        *ds_down_reason = DS_NET_DOWN_REASON_TRACKING_AREA_NOT_ALLOWED;
        break;

      case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA_LOCAL:
        *ds_down_reason = DS_NET_DOWN_REASON_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA;
        break;

      case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA_LOCAL:
        *ds_down_reason = DS_NET_DOWN_REASON_NO_SUITABLE_CELLS_IN_TRACKING_AREA;
        break;

      case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG_LOCAL:
        *ds_down_reason = DS_NET_DOWN_REASON_NOT_AUTHORIZED_FOR_THIS_CSG;
        break;

      case LTE_NAS_ESM_UNKNOWN_EPS_BEARER_CONTEXT:
        *ds_down_reason = DS_NET_DOWN_REASON_ESM_UNKNOWN_EPS_BEARER_CONTEXT;
        break;

      case DRB_RELEASED_AT_RRC:
        *ds_down_reason = DS_NET_DOWN_REASON_DRB_RELEASED_AT_RRC;
        break;

      case NAS_SIG_CONN_RELEASED:
        *ds_down_reason = DS_NET_DOWN_REASON_NAS_SIG_CONN_RELEASED;
        break;

      case EMM_DETACHED:
        *ds_down_reason = DS_NET_DOWN_REASON_EMM_DETACHED;
        break;
     
      case EMM_DETACHED_PSM:
        *ds_down_reason = DS_NET_DOWN_REASON_EMM_DETACHED_PSM;
        break;   


      case EMM_ATTACH_FAILED:
        *ds_down_reason = DS_NET_DOWN_REASON_EMM_ATTACH_FAILED;
        break;

      case EMM_ATTACH_STARTED:
        *ds_down_reason = DS_NET_DOWN_REASON_EMM_ATTACH_STARTED;
        break;

      case LTE_NAS_SERVICE_REQ_FAILED:
        *ds_down_reason = DS_NET_DOWN_REASON_LTE_NAS_SERVICE_REQ_FAILED;
        break;

      case ESM_ACTIVE_DEDICATED_BEARER_REACTIVATED_BY_NW:
        *ds_down_reason = DS_NET_DOWN_REASON_ESM_ACTIVE_DEDICATED_BEARER_REACTIVATED_BY_NW;
        break;

      case ESM_LOWER_LAYER_FAILURE:
        *ds_down_reason = DS_NET_DOWN_REASON_ESM_LOWER_LAYER_FAILURE;
        break;

      case ESM_SYNC_UP_WITH_NW:
        *ds_down_reason = DS_NET_DOWN_REASON_ESM_SYNC_UP_WITH_NW;
        break;

      case ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER:
        *ds_down_reason = DS_NET_DOWN_REASON_ESM_NW_ACTIVATED_DED_BEARER_WITH_ID_OF_DEF_BEARER;
        break;

      case ESM_BAD_OTA_MESSAGE:
        *ds_down_reason = DS_NET_DOWN_REASON_ESM_BAD_OTA_MESSAGE;
        break;

      case ESM_DS_REJECTED_THE_CALL:
        *ds_down_reason = DS_NET_DOWN_REASON_ESM_DS_REJECTED_THE_CALL;
        break;

      case ESM_CONTEXT_TRANSFERED_DUE_TO_IRAT:
        *ds_down_reason = DS_NET_DOWN_REASON_ESM_CONTEXT_TRANSFERED_DUE_TO_IRAT;
        break;

      case DS_EXPLICIT_DEACT:
        *ds_down_reason = DS_NET_DOWN_REASON_DS_EXPLICIT_DEACT;
        break;

      case ESM_LOCAL_CUASE_NONE:
        *ds_down_reason = DS_NET_DOWN_REASON_ESM_LOCAL_CAUSE_NONE;
        break;

      case LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE:
        *ds_down_reason = DS_NET_DOWN_REASON_LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE;
        break;

      case ACL_FAILURE:
        *ds_down_reason = DS_NET_DOWN_REASON_ACL_FAILURE;
        break;

      case LTE_NAS_SERVICE_REQ_FAILED_DS_DISALLOW:
        *ds_down_reason = DS_NET_DOWN_REASON_LTE_NAS_SERVICE_REQ_FAILED_DS_DISALLOW;
        break;

      case EMM_T3417_EXPIRED:
        *ds_down_reason = DS_NET_DOWN_REASON_EMM_T3417_EXPIRED;
        break;

      case EMM_T3417_EXT_EXPIRED:
        *ds_down_reason = DS_NET_DOWN_REASON_EMM_T3417_EXT_EXPIRED;
        break;

      case LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_TXN:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_TXN;
        break;

      case LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_HO:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_HO;
        break;

      case LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_CONN_REL:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_CONN_REL;
        break;

      case LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_RLF:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_RLF;
        break;

      case LTE_NAS_LRRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN;
        break;

      case LTE_NAS_LRRC_CONN_EST_FAILURE:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE;
        break;

      case LTE_NAS_LRRC_CONN_EST_FAILURE_ABORTED:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_ABORTED;
        break;

      case LTE_NAS_LRRC_CONN_EST_FAILURE_ACCESS_BARRED:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_ACCESS_BARRED;
        break;

      case LTE_NAS_LRRC_CONN_EST_FAILURE_CELL_RESEL:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_CELL_RESEL;
        break;

      case LTE_NAS_LRRC_CONN_EST_FAILURE_CONFIG_FAILURE:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_CONFIG_FAILURE;
        break;

      case LTE_NAS_LRRC_CONN_EST_FAILURE_TIMER_EXPIRED:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_TIMER_EXPIRED;
        break;

      case LTE_NAS_LRRC_CONN_EST_FAILURE_LINK_FAILURE:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_LINK_FAILURE;
        break;

      case LTE_NAS_LRRC_CONN_EST_FAILURE_NOT_CAMPED:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_NOT_CAMPED;
        break;

      case LTE_NAS_LRRC_CONN_EST_FAILURE_SI_FAILURE:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_SI_FAILURE;
        break;

      case LTE_NAS_LRRC_CONN_EST_FAILURE_CONN_REJECT:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_EST_FAILURE_CONN_REJECT;
        break;

      case LTE_NAS_LRRC_CONN_REL_NORMAL:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_REL_NORMAL;
        break;

      case LTE_NAS_LRRC_CONN_REL_RLF:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_REL_RLF;
        break;

      case LTE_NAS_LRRC_CONN_REL_CRE_FAILURE:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_REL_CRE_FAILURE;
        break;

      case LTE_NAS_LRRC_CONN_REL_OOS_DURING_CRE:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_REL_OOS_DURING_CRE;
        break;

      case LTE_NAS_LRRC_CONN_REL_ABORTED:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_REL_ABORTED;
        break;

      case LTE_NAS_LRRC_CONN_REL_SIB_READ_ERROR:
        *ds_down_reason = DS_NET_DOWN_REASON_LRRC_CONN_REL_SIB_READ_ERROR;
        break;

      case LTE_NAS_DETACH_WITH_REATTACH_LTE_NW_DETACH:
        *ds_down_reason = DS_NET_DOWN_REASON_DETACH_WITH_REATTACH_LTE_NW_DETACH;
        break;

      case LTE_NAS_DETACH_WITH_OUT_REATTACH_LTE_NW_DETACH:
        *ds_down_reason = DS_NET_DOWN_REASON_DETACH_WITH_OUT_REATTACH_LTE_NW_DETACH;
        break;

      case LTE_NAS_LRRC_CONN_EST_FAILURE_RF_UNAVAILABLE:
        *ds_down_reason = DS_NET_DOWN_REASON_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE;
        break;

      case ESM_PROC_TIME_OUT:
        *ds_down_reason =  DS_NET_DOWN_REASON_ESM_PROC_TIME_OUT;
        break;
      
      default:
        DS_ERROR_LOG_0("EVENT_CAUSE_CODES: Unable to map DS Int to PS_INDR, Unknown code!");
        *ds_down_reason = DS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE;
        break;
    }
  } /* ESM local cause codes*/
}/* ds_eps_bearer_mgr_map_esm_cause_codes_to_ds */

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_ISIM_REFERSH_VOTING_HDLR

DESCRIPTION
  This function handles isim refresh voting event for LTE mode.

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
boolean - Whether ISIM fresh can proceed or not
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_eps_bearer_mgr_isim_refresh_voting_hdlr()
{
  ds_pdn_mgr_s *pdn_p = NULL;
  ds_bearer_mgr_s *bearer_p = NULL;
  int index = 0;
  int inst_index = 0;
  int bearer_index = 0;

  DS_MSG0(MSG_LEGACY_HIGH, "In ds_eps_bearer_mgr_isim_refresh_voting_hdlr");

  for(index = 0; index < DS_3GPP_MAX_PDN_CONTEXT; index++)
  {
    pdn_p = ds_pdn_mgr_get_pdn_ptr(index);   
   	if( ds_pdn_mgr_validate_pdn_ptr(pdn_p) && 
	       (pdn_p->state == DS_PDN_MGR_STATE_UP ||
       		pdn_p->state == DS_PDN_MGR_STATE_COMING_UP))
    {
      for(inst_index = DS_PDN_INSTANCE_IPV4;
          inst_index < DS_PDN_INSTANCE_MAX; inst_index++)
      {
        if(pdn_p->inst_ptr[inst_index] != NULL &&
           pdn_p->inst_ptr[inst_index]->in_use == TRUE) 
        {
          for(bearer_index = 0; 
		            bearer_index < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN; 
      		      bearer_index++ )
          {
      		    bearer_p = pdn_p->bearer_ptr_tbl[bearer_index];
      		    if(ds_bearer_mgr_validate_bearer_ptr(bearer_p) &&
      		       (bearer_p->state == DS_BEARER_MGR_STATE_UP ||
      			       bearer_p->state == DS_BEARER_MGR_STATE_COMING_UP))
      		    {
      		      DS_MSG1(MSG_LEGACY_HIGH,
                      "LTE ISIM refresh handler voted NO for pdn %d",
      				            pdn_p->index);
      		      return FALSE;
      		    }
	         }
     		 }
	     }
    }  
  }

  return TRUE;
}

/*===========================================================================
FUNCTION DS_EPS_BEARER_MGR_EXAMINE_IF_THROT_NEEDED

DESCRIPTION
  This function is used to examine the cause code of rejection and determine
  if we need to perform throttling.

PARAMETERS
  call_info_ptr  - Pointer to the call info to find out rejection cause

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if throttling is needed
  FALSE otherwise. 
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_eps_bearer_mgr_examine_if_throt_needed
(
  ds_bearer_deact_params_s *lte_call_info_ptr,
  ds_3gpp_throt_rab_info_s  *rab_throt_info_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
    1. We first check if EMM Failure Cause Type is other than None. If yes,    
       we check EMM Cause codes to determine whether Throttling is allowed.
   
    2. Next we check the ESM Local is valid flag.If the flag is set, we check
       the ESM Local cause codes to determine whether Throttling is allowed.
  */

  if(lte_call_info_ptr->emm_failure_cause.cause_type != LTE_NAS_IRAT_NONE 
      && rab_throt_info_ptr != NULL && rab_throt_info_ptr != NULL)
  {
     ds_eps_bearer_cntxt_examine_if_emm_cause_throt_needed(lte_call_info_ptr, rab_throt_info_ptr);
  }

  else if(lte_call_info_ptr->esm_local_cause.valid == TRUE && rab_throt_info_ptr!= NULL)
  {
    /*------------------------------------------------------------------------
      Now determine if any throttling has to be done. Examine emm cause codes
      that comes as esm_local_cause
    ------------------------------------------------------------------------*/
    switch(lte_call_info_ptr->esm_local_cause.local_cause)
    {
      case LTE_NAS_ILLEGAL_UE_LOCAL: // lint-fallthrough
      case LTE_NAS_ILLEGAL_ME_LOCAL:
      case LTE_NAS_EPS_SERVICES_NOT_ALLOWED_LOCAL:
      case LTE_NAS_UE_ID_CANNOT_BE_DERIVED_BY_NW_LOCAL:
      case LTE_NAS_IMPLICITLY_DETACHED_LOCAL:
      case LTE_NAS_PLMN_NOT_ALLOWED_LOCAL:
      case LTE_NAS_TRACKING_AREA_NOT_ALLOWED_LOCAL:
      case LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA_LOCAL:
      case LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA_LOCAL:
      case LTE_NAS_CS_DOMAIN_NOT_AVAILABLE_LOCAL:
      case LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG_LOCAL:
      case LTE_NAS_CS_DOMAIN_TEMP_NOT_AVAILABLE_LOCAL:
      case LTE_NAS_SERVICE_REQ_FAILED_NO_THROTTLE:
      case ESM_LOWER_LAYER_FAILURE:
      case LTE_NAS_SERVICE_REQ_FAILED_DS_DISALLOW:
      case LTE_NAS_LRRC_CONN_EST_FAILURE_RF_UNAVAILABLE:
        DS_MSG1(MSG_LEGACY_HIGH,
                "Rej cause: %d. No back off needed for listed cause",
                lte_call_info_ptr->esm_local_cause.local_cause);
        rab_throt_info_ptr->throttle_needed = FALSE;
        break;
      default:

        rab_throt_info_ptr->throttle_needed = TRUE;
        DS_MSG1(MSG_LEGACY_HIGH,
                "Rej cause: %d. Back off needed for unlisted cause",
                lte_call_info_ptr->esm_local_cause.local_cause);
        /*--------------------------------------------------------------------
          Set ret val to true so that we know we have to 
        --------------------------------------------------------------------*/
        break;
    } /* switch on EMM cause code returned */
  } /* If the EMM cause code is valid */

  return ;
} /* ds_eps_bearer_mgr_examine_if_throt_needed */

