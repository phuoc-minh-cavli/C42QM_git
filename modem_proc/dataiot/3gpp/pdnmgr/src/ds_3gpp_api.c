/*===========================================================================
                      DS_3GPP_API.C
DESCRIPTION
This file defines 3GPP APIs that needs to be exposed to external clients 
outside DS.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_3gpp_api.c#4 $
  $DateTime: 2023/02/07 02:18:02 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#include "dstaski.h"
#include "ds3gpp_api.h"
#include "dsm.h"
#include "ds_pdn_manager.h"
#include "ds_pdn_cp_data_hdlr.h"
#include "ds_pdn_nv_manager.h"
#include "ds_pdn_hdlr.h"
#include "ds_3gpp_srv_req_throt_sm.h"

/*===========================================================================
FUNCTION DS_3GPP_NOTIFY_IND

DESCRIPTION
   This is a generic function, used to notify DS module.

PARAMETERS
 indication type and 
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_notify_ind(ds_3gpp_notify_ind_e_type indication, void *notify_ind_data_ptr)
{
#if 0
#if (defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS))
  ds_cmd_type 		        *cmd_ptr = NULL; 
  sys_modem_as_id_e_type        *data_ptr = NULL;
  ds_3gpp_rau_event_s_type*      ind_ptr=
  	                  ((ds_3gpp_rau_event_s_type*)notify_ind_data_ptr);
/*--------------------------------------------------------------------------------*/
  if(ind_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("ds_3gpp_notify_ind: recieved NULL ptr");
    return;
  }

   DS_3GPP_MSG3_HIGH("ds_3gpp_notify_ind: ind type:%d evt:%d rau type:%d", 
   	                    indication, ind_ptr->event, ind_ptr->rau_type);

  if(indication == DS_3GPP_RAU_IND)
  {
    switch(ind_ptr->event)
    {
     case DS_3GPP_RAU_COMPLETE_EVT:

	if(ind_ptr->rau_type != DS_3GPP_RA_CAHNGE_TYPE)
        {
          return;
	}
	if( (cmd_ptr = ds_allocate_cmd_buf(sizeof(sys_modem_as_id_e_type))) == NULL )
        {
          DS_3GPP_MSG0_ERROR("DS3GPP cant get cmd buf from DS task");
          return;
        }

        cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_RAU_COMPLTE_CMD;
        data_ptr = (sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr;
        *data_ptr = (sys_modem_as_id_e_type)ind_ptr->subs_id;
        ds_put_cmd(cmd_ptr);
       break;
     default:
       break;			
  }
  }
#else
  DS_3GPP_MSG0_ERROR("ds_3gpp_notify_ind: not supported"); */
#endif /* (defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)) */
#endif
}/*ds3gpp_notify_ind*/

/*===========================================================================
FUNCTION DS3GPP_IS_PLMN_BLOCKED_FORCE_LTE_PREF

DESCRIPTION
   This is called by lower layers during powere off to see if any PLMN is blocked.
   lower layer will set their next pref sys based on the PLMN blocked status

PARAMETERS
  subs_id 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If any PLMN is blocked
  FALSE - otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds3gpp_is_plmn_blocked_force_lte_pref
(
  sys_modem_as_id_e_type	  subs_id
)
{
  list_type  *plmn_list = NULL;
  boolean ret_val = FALSE;
/*-------------------------------------------------------------------------*/

  plmn_list = ds_pdn_hdlr_get_blocked_plmn_list();
  if(plmn_list != NULL &&
     list_size(plmn_list) > 0)    
 {
   ret_val = TRUE;
 }

  DS_MSG1(MSG_LEGACY_HIGH, "ds3gpp_is_plmn_blocked_force_lte_pref: %d", ret_val);
  return ret_val;
}/*ds3gpp_is_plmn_blocked_force_lte_pref*/

/*===========================================================================
FUNCTION DSUMTSPS_IS_PS_DATA_AVAILABLE

DESCRIPTION
   This function returns TRUE if data is available in any of the uplink
   buffers, otherwise, it returns FALSE.
   The function is accessible at the SU api level.

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If Data is available
  FALSE - If Data is not available

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds3gpp_is_ps_data_available(void)
{
  ds_bearer_mgr_s   *bearer_context_p;
  uint32              bearer_cntx_inst;
/*-----------------------------------------------------------------------*/  
 DS_MSG0(MSG_LEGACY_HIGH,"Checking for Uplink Data on DS UM watermark");
/*--------------------------------------------------------------------------
   For each bearer in the bearer context table, check the UL WM.
 --------------------------------------------------------------------------*/
  for( bearer_cntx_inst=0;\
       bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;\
       bearer_cntx_inst++ )
  {
    bearer_context_p = ds_bearer_mgr_get_instance_ptr(bearer_cntx_inst);
    if (!ds_bearer_mgr_validate_bearer_ptr (bearer_context_p))
    {
      continue;
    }
    if((bearer_context_p->state == DS_BEARER_MGR_STATE_PEND_REDIAL) ||
        (bearer_context_p->state ==DS_BEARER_MGR_STATE_COMING_UP))
    {
      DS_MSG1(MSG_LEGACY_HIGH,"Bearer_inst:%d Pending Redial, ps_data_available",
                        bearer_cntx_inst);
      return TRUE;
    }

    DS_MSG3(MSG_LEGACY_HIGH,"Bearer_inst:%d, state: %d,"
                  "wm count %d", bearer_cntx_inst, 
                  bearer_context_p->state,
                  bearer_context_p->tx_wm.wm_item.current_cnt);
  
     /*If all the bearers are in Up state ; do not check the WM always
        set Active_Flag to False.This is because; Active Flag in TAU will
          be used to reestabilish the bearers in dormant state.*/
      if((bearer_context_p->eps_bearer == NULL ||
          bearer_context_p->eps_bearer->is_registered_pdcp == FALSE) &&
          ( (bearer_context_p->state == 
               DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB) ||
              (bearer_context_p->state ==  DS_BEARER_MGR_STATE_UP_DORMANT) ||
              (bearer_context_p->state == DS_BEARER_MGR_STATE_UP)))
      {
        if( bearer_context_p->tx_wm.wm_item.current_cnt > 0 )
        {
          DS_MSG0(MSG_LEGACY_HIGH,"Uplink Data on DS UM watermark");
          return TRUE;
        }
      } /*  UP_DORMANT_REESTAB || UP_DORMANT || UP*/
    }/* for all bearer contexts */
    return FALSE;
}

/*===========================================================================
FUNCTION DS3GPP_IS_UE_INIT_QOS_SUPPORTED

DESCRIPTION
   This function returns TRUE if umts UE initiated QOS support
   is available, otherwise, it returns FALSE.

PARAMETERS
  None.
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If UE initiated UMTS QOS support is available
  FALSE - If UE initiated UMTS QOS support is not available

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_is_ue_init_qos_supported(void)
{
  return FALSE;
}/* ds3gpp_is_ue_init_qos_supported */

/*===========================================================================
FUNCTION DS_3GPP_COMPARE_PDP_ADDRESS

DESCRIPTION
  This function is used by NAS to compare the pdp_address when network
  sends MODIFY_REQUEST with pdp_address included. DS will compare the
  pdp_address passed by NAS against the pdp_address which is configured on
  V4/V6 interfaces.

PARAMETERS
  identifier   - Identifier used to uniquely identify a PDP/PDN Context.
                 (For W/G/TD it would be nsapi, For LTE it would be eps_bid)
  sys_mode     - Current sys_mode.
  subs_id      - Subscription on which the PDP/PDN is active.
  *pdp_address - Structure containing the pdp_address and pdp_type_num 
                 given by network, which would be used for comparison.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE -  if PDP address sent by network is same as the one configured.
  FALSE - if PDP address sent by network is different than the one
          configured.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_compare_pdp_address
(
  uint8 identifier,
  sys_sys_mode_e_type sys_mode,
  sys_modem_as_id_e_type subs_id,
  pdp_address_T *pdp_address
)
{
/*
  boolean ret_val = FALSE;
  if (NULL == pdp_address)
  {
    DS_3GPP_MSG3_ERROR("Invalid pdp_address ptr passed", 0,0,0);
	return FALSE;
  }

  ret_val = ds_3gpp_pdn_cntxt_compare_pdp_address(identifier, 
  	                                           sys_mode, 
  	                                           subs_id, 
  	                                           pdp_address);
  DS_3GPP_MSG3_HIGH("PDP addr compare returning: %d", ret_val, 0, 0);*/
  return TRUE;
}

/*===========================================================================
FUNCTION ds3gpp_per_subs_get_config_multi_pdn_same_apn

DESCRIPTION
  This function gets the multi pdn same apn nv value for each subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  
  Boolean Flag indicating whether Multi Pdn same apn nv is enabled.

SIDE EFFECTS
===========================================================================*/

boolean ds3gpp_per_subs_get_config_multi_pdn_same_apn
(
  sys_modem_as_id_e_type subs_id
)
{
  return ds_pdn_nv_manager_get_multi_pdn_same_apn_nv();
}

/*===========================================================================
FUNCTION ds3gpp_per_subs_get_config_umts_nw_init_qos_support

DESCRIPTION
  This function gets the umts_nw_init_qos_support NV item: 70331
  modem/data/3gpp/umts_nw_initiated_qos_support

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  Boolean Flag indicating whether UMTS NW Init QoS is enabled or not

SIDE EFFECTS
  None

===========================================================================*/
boolean ds3gpp_per_subs_get_config_umts_nw_init_qos_support
(
  sys_modem_as_id_e_type subs_id
)
{
  if (ds_pdn_nv_manager_get_umts_nw_init_qos_support_nv() == DS_PDN_NV_ENABLED)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

void ds3gpp_is_user_data_or_signalling_pending
(
  boolean *user_data_pending,
  boolean *signalling_pending,
    boolean *dl_data_expected 
)
{
  ds_bearer_mgr_s                  *bearer_mgr_p;
  uint8                             bearer_cntx_inst;
  sys_radio_access_tech_e_type      radio_tech_type = emm_get_active_rat();
  boolean                           mo_ex_data_avail = FALSE;
  /*-----------------------------------------------------------------------*/  
  
  if (user_data_pending == NULL || signalling_pending == NULL)
  {
    return;
  }
  
  *user_data_pending = FALSE;
  *signalling_pending = FALSE;
  *dl_data_expected   = FALSE;
  
  DS_MSG0(MSG_LEGACY_HIGH, "Checking for Uplink Data on DS UM watermark");

  /*--------------------------------------------------------------------------
    For each bearer in the bearer context table, check the UL WM.
  --------------------------------------------------------------------------*/  
  for( bearer_cntx_inst=0;\
       bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;\
       bearer_cntx_inst++ )
  {
    bearer_mgr_p = ds_bearer_mgr_get_instance_ptr(bearer_cntx_inst);
  
    if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      continue;
    }
    if(!mo_ex_data_avail)
    {
      mo_ex_data_avail =
	        ds_pdn_cp_data_pending_mo_ex_data_avail(bearer_cntx_inst);
    }
    if((bearer_mgr_p->state == DS_BEARER_MGR_STATE_PEND_REDIAL) ||
       (bearer_mgr_p->state == DS_BEARER_MGR_STATE_COMING_UP))
    {
      DS_MSG1(MSG_LEGACY_HIGH, 
      	       "Bearer_inst:%d Pending Redial, ps_data_available",
      	       bearer_cntx_inst);
  
      if (radio_tech_type != SYS_RAT_LTE_NB1_RADIO_ACCESS 
#ifdef FEATURE_NBIOT_NTN
          && radio_tech_type != SYS_RAT_LTE_NB1_NTN_RADIO_ACCESS
#endif /* FEATURE_NBIOT_NTN */
          )
      {
        *user_data_pending = TRUE;
      }

      *signalling_pending = TRUE;
    }

   /*If all the bearers are in Up state ; do not check the WM always
      set Active_Flag to False.This is because; Active Flag in TAU will
        be used to reestabilish the bearers in dormant state.*/
    if((bearer_mgr_p->eps_bearer != NULL &&
    	   !bearer_mgr_p->eps_bearer->is_registered_pdcp) && 
    	  (bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB ||
        bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP_DORMANT ||
        bearer_mgr_p->state == DS_BEARER_MGR_STATE_UP))
    {  
      DS_MSG2(MSG_LEGACY_HIGH,
      	       "Bearer_inst:%d UM watermark cnt:%d",
              bearer_cntx_inst,
              bearer_mgr_p->tx_wm.wm_item.current_cnt);
      
      if( bearer_mgr_p->tx_wm.wm_item.current_cnt > 0 )
      { 
        if (radio_tech_type != SYS_RAT_LTE_NB1_RADIO_ACCESS 
#ifdef FEATURE_NBIOT_NTN
            && radio_tech_type != SYS_RAT_LTE_NB1_NTN_RADIO_ACCESS
#endif /* FEATURE_NBIOT_NTN */
            )
        {
          *user_data_pending = TRUE;
        }
        else if  (ds_pdn_cp_data_drb_setup_is_reqd(bearer_mgr_p->index,
                               bearer_mgr_p->tx_wm.wm_item.current_cnt))
        { 
          *user_data_pending = TRUE;
          *signalling_pending = TRUE;
        }
        else
        {
          *signalling_pending = TRUE;
        }
      }
      /* if there is only one cp data packet, tx_wm will be empty but 
       so we need to check if there are pending packets to be sent
       over srb.*/
      else if(ds_pdn_cp_data_ack_is_pending())
      {
        *signalling_pending = TRUE;
      }
    } /*  UP_DORMANT_REESTAB || UP_DORMANT || UP*/
  
    if (TRUE == ds_pdn_cp_data_is_dl_data_expected(bearer_mgr_p->index))
    {
      *dl_data_expected = TRUE;
    }
  
    if (radio_tech_type == SYS_RAT_LTE_NB1_RADIO_ACCESS 
#ifdef FEATURE_NBIOT_NTN
        || radio_tech_type == SYS_RAT_LTE_NB1_NTN_RADIO_ACCESS
#endif /* FEATURE_NBIOT_NTN */
        )
    {
      if ((*user_data_pending == TRUE) && (*signalling_pending == TRUE) 
           && (*dl_data_expected == TRUE))
      {
        break;
      }
    }
    else
    {
      if (*user_data_pending == TRUE)
      {
        break;
      }
    }
  }/* for all bearer contexts */
  /* if signalling is pending reset it when t3448 timer running while 
      UE is not in AC11-15 and there is no cp data pending */  
  if(ds_pdn_mgr_is_T3448_running() && !ds_pdn_hdlr_is_ue_in_ac11to15()
	            && !mo_ex_data_avail)
  {
    *signalling_pending = FALSE;
  }
    
  return;
}

/*===========================================================================
FUNCTION ds3gpp_is_ue_ciot_only

DESCRIPTION
  This function returns the info about device in ciot
  if any of the bearer is not in CIOT only then return false

PARAMETERS 
  None.  

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds3gpp_is_ue_ciot_only()
{
  return ds_pdn_cp_data_is_ue_cp_only();
} /* ds3gpp_is_ue_ciot_only */

void ds_eps_bearer_flush_bytes
(
  uint8   eps_id,                        /*eps bearer id which needs to be flused*/
  uint32  num_pkt_bytes               /*Total number of bytes to be remaining at end of procedure*/
)
{
  dsm_item_type               *item_ptr = NULL;
  ds_bearer_mgr_s             *bearer_mgr_p = NULL;
  dsm_watermark_type          *wm_ptr = NULL;
  uint32                       initial_count = 0;     
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  bearer_mgr_p = ds_bearer_mgr_get_bearer_from_eps_id(eps_id);  
  if(ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    wm_ptr = &(bearer_mgr_p->rx_wm.wm_item);
    initial_count = wm_ptr->current_cnt;
    if(num_pkt_bytes == 0)
    {
      dsm_empty_queue(wm_ptr);  
    }
    else
    {
      while((item_ptr = dsm_dequeue(wm_ptr)) != NULL)
      {
        (void)dsm_free_packet(&item_ptr);
        if(wm_ptr->current_cnt <= num_pkt_bytes)
        {
          break;
        }
      }
    } 
    
    DS_MSG3(MSG_LEGACY_HIGH, "Inital count %d Current count %d packets "
           "in the eps id %d",initial_count,wm_ptr->current_cnt,eps_id);
  }
}

void ds_eps_bearer_reenqueue_pkts
(
  dsm_watermark_type* wm_ptr,     
  uint32 num_pkt_bytes,               
  dsm_item_type* head_dsm_ptr    
)
{
  dsm_item_type* iter_ptr = NULL;
  dsm_item_type* nx_ptr = NULL;
  uint32 new_dne_cnt =0;
  uint32 buffer = 0;

  if(head_dsm_ptr == NULL || wm_ptr == NULL)
  {
    return;
  }
  
  iter_ptr = (dsm_item_type *)head_dsm_ptr->link.next_ptr;
    
  /* Flow control is disabled /Back pressure is off/disabled*/
  if(wm_ptr->hi_watermark == wm_ptr->dont_exceed_cnt)
  {
    DS_MSG0(MSG_LEGACY_HIGH, "Back pressure is disabled ");
  }
  else
  {
    if((wm_ptr->current_cnt + num_pkt_bytes + buffer) > wm_ptr->dont_exceed_cnt)
    {
      new_dne_cnt = wm_ptr->dont_exceed_cnt + num_pkt_bytes + buffer;
      DS_MSG1(MSG_LEGACY_HIGH, "Wm pointer Dne Updated %d",new_dne_cnt); 
      dsm_set_dne(wm_ptr,new_dne_cnt);
    }
  }

  head_dsm_ptr->link.next_ptr = NULL;
  dsm_enqueue(wm_ptr,&head_dsm_ptr);

  while(iter_ptr != NULL)
  {
    nx_ptr = (dsm_item_type *) iter_ptr->link.next_ptr;
    iter_ptr->link.next_ptr = NULL;
    dsm_enqueue(wm_ptr,&iter_ptr);
    iter_ptr = nx_ptr;
  }
}

boolean ds3gpp_per_subs_is_service_req_allowed_on_plmn
(
  sys_plmn_id_s_type plmn_id,
  sys_modem_as_id_e_type as_id
)
{
  void*    sm_ptr  = NULL;
  boolean  ret_val = TRUE;
  /*-----------------------------------------------------------------------*/
    
  /*----------------------------------------------------------------------------
    Get a handle to the Service request pointer. Remember that here we are not
    looking to instantiate a core state machine.
  ----------------------------------------------------------------------------*/
  sm_ptr = ds_3gpp_srv_req_get_throt_sm(FALSE);
  
  if(sm_ptr != NULL)
  {
    if(ds_3gpp_srv_req_throt_get_state(sm_ptr, &ret_val) == TRUE)
    {
      ret_val = !ret_val;
      DS_MSG1(MSG_LEGACY_HIGH, "Throt SM consulted. Return %d to NAS",ret_val);
    }
    else
    {
      /*------------------------------------------------------------------------
        Set the ret_val to TRUE. This is just to be sure that the function that
        we called did not reset the contents of the memory pointed to by the
        pointer passed into the function.
      ------------------------------------------------------------------------*/
      ret_val = TRUE;
    }
  }

  return ret_val;
}

uint32 ds3gpp_um_wm_get_num_bytes
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_bearer_mgr_s   *bearer_mgr_p = NULL;
  uint8              bearer_cntx_inst;
  uint32             num_bytes = 0;
  /*---------------------------------------------------------------------*/
  
  for(bearer_cntx_inst = 0;
      bearer_cntx_inst < DS_3GPP_MAX_BEARER_CONTEXT;
      bearer_cntx_inst++) 
  {
    bearer_mgr_p = ds_bearer_mgr_get_instance_ptr(bearer_cntx_inst);
  
    if (ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) != FALSE &&
        bearer_mgr_p->drb_is_up == FALSE)
    {
      num_bytes += bearer_mgr_p->tx_wm.wm_item.current_cnt;
    }
  }
  
  return num_bytes;
}

/*=========================================================================
  FUNCTION DS_3GPP_CP_DATA_SEND_UL_PACKET

  DESCRIPTION
    This function processes the notification from L2 to send additional
    UL Packets.
  
  PARAMETERS
    max_uplink_packets - Max Uplink Packets that can be sent by DS.
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_3gpp_send_ul_packet
(
  uint8                   max_uplink_packets
)
{
  ds_command_type    *cmd_ptr   = NULL;
  uint8              *data_ptr  = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG1(MSG_LEGACY_HIGH, "ds_3gpp_send_ul_packet", max_uplink_packets);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(uint8));
  if(NULL == cmd_ptr || NULL == cmd_ptr->cmd_payload_ptr)
  {
    DS_ASSERT(0);
    return;
  }
 
  data_ptr = (uint8*)cmd_ptr->cmd_payload_ptr;
  *data_ptr = max_uplink_packets;

  cmd_ptr->hdr.cmd_id = DS_CMD_PDN_CP_DATA_L2_NOTIFY;
  ds_put_cmd(cmd_ptr);

  return;
}

/*===========================================================================
FUNCTION DS3GPP_CHECK_PENDING_DATA_FOR_CPSR

DESCRIPTION
   This is called by lower layers to query if there is any data pending before
   performing CPSR.If data is present then post a command to DS task so that
   so that CPSR can piggyback first packet.

PARAMETERS
  None 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If data is pending
  FALSE - otherwise

SIDE EFFECTS
  None.
===========================================================================*/


boolean ds3gpp_check_pending_data_for_cpsr
(
  void
)
{
  boolean ret_val = FALSE;
  uint8              bearer_cntx_inst;
  ds_command_type     *cmd_ptr = NULL;
  ds_bearer_mgr_s     *bearer_mgr_p = NULL;
  ds_bearer_mgr_s    **data_ptr = NULL;
/*-------------------------------------------------------------------------*/
  
  if(!ds_pdn_hdlr_get_ps_data_suspend_flag())
  {
    for(bearer_cntx_inst = 0;
      bearer_cntx_inst < DS_3GPP_MAX_BEARER_CONTEXT;
      bearer_cntx_inst++) 
    {
      bearer_mgr_p = ds_bearer_mgr_get_instance_ptr(bearer_cntx_inst);
  
      if (ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) != FALSE &&
          bearer_mgr_p->tx_wm.wm_item.current_cnt > 0)
      {
        cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_bearer_mgr_s*));

        if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
        {
          DS_ASSERT(0);
          return FALSE;
        }

        cmd_ptr->hdr.cmd_id = DS_CMD_PDN_DORM_REORIG;
        data_ptr = (ds_bearer_mgr_s**)cmd_ptr->cmd_payload_ptr;
        *data_ptr = bearer_mgr_p;
        ret_val = TRUE;
      }
    }
  }

  DS_MSG1(MSG_LEGACY_HIGH, "ds3gpp_check_pending_data_for_cpsr returning : %d",ret_val);
  return ret_val;
}/*ds3gpp_check_pending_data_for_cpsr*/

/*==============================================================================
FUNCTION DS3GPP_IS_BEARER_FOR_IMS_PDN

DESCRIPTION
  This function is used to check whether the passed bearer ID belongs
  to IMS PDN.

PARAMETERS   :
   EPS bearer ID.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if bearer belong to IMS PDN. 
  FALSE otherwise

SIDE EFFECTS
  None
==============================================================================*/
boolean ds3gpp_is_bearer_for_ims_pdn
(
  byte   bearer_id
)
{
  ds_bearer_mgr_s   *bearer_mgr_p = NULL;
/*-------------------------------------------------------------------*/

  bearer_mgr_p = ds_bearer_mgr_get_bearer_from_eps_id((uint8)bearer_id);

  if(ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) &&
     ds_pdn_mgr_validate_pdn_ptr(bearer_mgr_p->pdn_mgr_ptr) &&
     bearer_mgr_p->pdn_mgr_ptr->apn_type == DS_APN_TYPE_IMS)
  {
    return TRUE;
  }

  return FALSE;
}

/*==============================================================================
FUNCTION DS3GPP_IS_BEARER_FOR_NIP_PDN

DESCRIPTION
  This function is used to check whether the passed bearer ID belongs
  to non-IP PDN.

PARAMETERS   :
   EPS bearer ID.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if bearer belong to non-IP PDN. 
  FALSE otherwise

SIDE EFFECTS
  None
==============================================================================*/
boolean ds3gpp_is_bearer_for_nip_pdn
(
  byte   bearer_id
)
{
  ds_bearer_mgr_s   *bearer_mgr_p = NULL;
/*-------------------------------------------------------------------*/

  bearer_mgr_p = ds_bearer_mgr_get_bearer_from_eps_id((uint8)bearer_id);

  if(ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) &&
     ds_pdn_mgr_validate_pdn_ptr(bearer_mgr_p->pdn_mgr_ptr) &&
     bearer_mgr_p->pdn_mgr_ptr->inst_ptr[DS_PDN_INSTANCE_NON_IP] != NULL)
  {
    return TRUE;
  }

  return FALSE;
}

/*==============================================================================
FUNCTION DS3GPP_IS_PENDING_MO_EX_DATA_AVAIL

DESCRIPTION
  This function is used to check whether the passed bearer ID belongs
  to non-IP PDN.

PARAMETERS   :
  None

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if mo ex data is available. 
  FALSE otherwise

SIDE EFFECTS
  None
==============================================================================*/
boolean ds3gpp_is_pending_mo_ex_data_avail()
{
  ds_bearer_mgr_s                  *bearer_mgr_p;
  uint8                             bearer_cntx_inst;
  boolean                           mo_ex_data_avail = FALSE;
  /*-----------------------------------------------------------------------*/  

  DS_MSG0(MSG_LEGACY_HIGH, "Checking for MO_EX data availability");
  /*--------------------------------------------------------------------------
    For each bearer in the bearer context table, check the UL WM.
  --------------------------------------------------------------------------*/  
  for( bearer_cntx_inst=0;\
       bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;\
       bearer_cntx_inst++ )
  {
    bearer_mgr_p = ds_bearer_mgr_get_instance_ptr(bearer_cntx_inst);
  
    if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
    {
      continue;
    }
    mo_ex_data_avail =
       ds_pdn_cp_data_pending_mo_ex_data_avail(bearer_cntx_inst);
    if(mo_ex_data_avail)
    {
      return mo_ex_data_avail;
    }
  }
  return mo_ex_data_avail;
}

/*===========================================================================
FUNCTION DS_3GPP_IS_WWAN_TECH_SUSPENDED

DESCRIPTION
  This function return the status whether WWAN tech is suspended.

PARAMETERS
 None.

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE - IF WWAN tech is suspended.
  FALSE - otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_is_wwan_tech_suspended
(
  void
)
{
  return ds_pdn_mgr_is_wwan_tech_suspended();
} /* ds_pdn_mgr_is_wwan_tech_suspended */