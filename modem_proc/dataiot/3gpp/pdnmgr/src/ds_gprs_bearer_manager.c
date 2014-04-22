/*===========================================================================
                      DS_GPRS_BEARER_MANAGER.C
DESCRIPTION
This file defines bearer manager for GSM APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_gprs_bearer_manager.c#11 $
  $DateTime: 2023/06/02 03:11:51 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_gprs_bearer_manager.h"
#include "ds_pdn_pco.h"
#include "comptask_api.h"
#include "ds_pdn_flow_manager.h"
#include "ds_profile.h"
#include "ds_pdn_manager.h"
#include "ds_pdn_nv_manager.h"
#include "ds_pdn_hdlr.h"
#include "ds_bearer_flow_manager.h"
#include "ds_flow.h"
#include "ds_fwki.h"

#include "gsndcp.h"
#include "rr_ds_if.h"
#include "ds_pdn_psm_hdlr.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_3gpp_network_override_hdlr.h"
#include "ds_pdn_psm_hdlr.h"
#include "ds_3gpp_pdn_throt_rules.h"
#include "ds_3gpp_rpm.h"
#include "ds_pdn_clat_hdlr.h"
#ifdef FEATURE_DATA_PS_464XLAT
#include "ps_clat_sm.h"
#endif /* #ifdef FEATURE_DATA_PS_464XLAT */
#include "ds_3gpp_cc_sp_throt_sm.h"

#define DS_3GPP_PDP_TYPE_IPV4_NUM    0x21
#define DS_3GPP_PDP_TYPE_IPV6_NUM    0x57
#define DS_3GPP_PDP_TYPE_IPV4V6_NUM  0x8D

#define DS_3GPP_GPRS_IPV6_IID_OFFSET 8
#define DS_3GPP_GPRS_DUAL_IP_IPV6_IID_OFFSET 12


/*===========================================================================
FUNCTION DS_GPRS_PDN_IP_FILL_PPP_PARAMS

DESCRIPTION
  This function fills in the PPP authentication and IPCP packet information
  for a PDP-IP call
    
PARAMETERS  
    *pdn_data      - Ptr to  Call origination parameters 
      profile_p      -  Ptr to the PDP profile parameters


DEPENDENCIES
  None.
  
RETURN VALUE
  none  

SIDE EFFECTS
  orig_params_ptr is updated with the PPP packet information from the profile
  
===========================================================================*/
void ds_gprs_pdn_ip_fill_ppp_params
(
  mm_pdp_activate_req_T           *gprs_orig_params,
  ds_profile_type_s               *profile_p,
  ds_pdn_network_params_info_type *info_p,
  ds_pdn_protocol_cfg_options_s  *ds_proto_config_option_p
)
{
  boolean result = FALSE;
  net_cfg_params_mask_e_type valid_fields = 
      (net_cfg_params_mask_e_type)0x00000000; /* IPCP options to be requested*/  
  uint8   rel_version = ds_pdn_nv_manager_get_3gpp_rel_version();
  uint32  pdp_addr =0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
   Fill in Protocol specific options : for IP, this would contain the PPP 
   pkts (lcp, ipcp ) The purpose of the protocol configuration options
   information element is to transfer external network protocol options 
   associated with a PDP context activation . The IE is defined in 10.5.6.3.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
   start off assuming that the field is not valid ( nothing filled in ) 
  -------------------------------------------------------------------------*/
  ds_proto_config_option_p->valid  = FALSE; 
  
  /*-------------------------------------------------------------------------
    Config protocol field is 0x80 : ref: TS24.008 sec 10.5.6.3
    MSB should be 1 (ext field) protocol field b0-b2 should be 0x0 
    Initilize this field
  -------------------------------------------------------------------------*/
  ds_proto_config_option_p->config_protocol   = PROT_CFG_IE_OCT3_VAL; 
  ds_proto_config_option_p->num_protocol_ids  = 0;

  /*-------------------------------------------------------------------------
     Check if a valid static IP is requested, if so update origination parameter 
    -------------------------------------------------------------------------*/
  if ( (rel_version < (uint8)DS_3GPP_REL8) &&
       (profile_p->context.pdp_type != DS_PROFILE_PDP_IPV6) &&
       ((info_p != NULL) &&
         (info_p->valid_fields & NET_CFG_PRM_IP_ADDR_MASK) &&
         ((pdp_addr = info_p->ipcp_info.ip_address) != 0)) ) 
  {
    DS_MSG1(MSG_LEGACY_HIGH, 
    	       "Setting PDP Static IP address to :%lx from info/profile",
            pdp_addr);
    gprs_orig_params->pdp_addr.pdp_addr_len  = 4;
    gprs_orig_params->pdp_addr.address[0] = (byte)( (pdp_addr & 0xff000000) >> 24 );
    gprs_orig_params->pdp_addr.address[1] = (byte)( (pdp_addr & 0x00ff0000) >> 16 );
    gprs_orig_params->pdp_addr.address[2] = (byte)( (pdp_addr & 0x0000ff00) >> 8 );
    gprs_orig_params->pdp_addr.address[3] = (byte)( (pdp_addr & 0x000000ff) );    
  }  
  
  if( info_p != NULL &&
  	   ((info_p->valid_fields & NET_CFG_PRM_AUTH_PREF_MASK)||
       (info_p->valid_fields & NET_CFG_PRM_AUTH_USERNAME_MASK)||
       (info_p->valid_fields & NET_CFG_PRM_AUTH_PASSWORD_MASK)) &&
       (info_p->auth_info.params.user_id_len != 0))
  {
	DS_MSG1(MSG_LEGACY_HIGH, "User id length %d", info_p->auth_info.params.user_id_len);
   /*-------------------------------------------------------------------------
       Use the auth info from info_p
     -------------------------------------------------------------------------*/  
    result = ds_pdn_mgr_fill_auth_params_from_info(
                  ds_proto_config_option_p, profile_p, info_p);
  }
  else
  {
   /*-------------------------------------------------------------------------
      Use the auth info from profile
    -------------------------------------------------------------------------*/
    result = ds_pdn_mgr_fill_auth_params_from_profile(
                  ds_proto_config_option_p,profile_p);   
  }

  DS_MSG1(MSG_LEGACY_HIGH,"Auth result = %d", result);
 
  if((profile_p->context.pdp_type == DS_PROFILE_PDP_IPV4) ||
     (profile_p->context.pdp_type == DS_PROFILE_PDP_IPV4V6) )
  {
    if((info_p != NULL) &&
    	  ((info_p->valid_fields & NET_CFG_PRM_PRI_DNS_MASK) ||
        (info_p->valid_fields & NET_CFG_PRM_SEC_DNS_MASK)) )
    {
      valid_fields = (net_cfg_params_mask_e_type)(info_p->valid_fields);

#ifdef FEATURE_GSM_DTM
      /* In case of reduced PCO we need to ignore secondary address requests. */
      if (rr_dtm_get_max_LAPDm_octets() != 0)
      {
        valid_fields &= ~(NET_CFG_PRM_SEC_DNS_MASK);
        DS_MSG0(MSG_LEGACY_HIGH,"Secondary DNS container omitted");
      }
#endif  
      
   /*---------------------------------------------------------------------------
       We convert the DNS address from Little Endian to Big Endian before
       sending the request to OTA, since QMI sends information in Little Endian
       format, however modifications to the profile are sent in Big Endian format
     ---------------------------------------------------------------------------*/
      result = ds_pdn_mgr_fill_ipcp_orig_params 
              (
               ds_proto_config_option_p,
               TRUE,
               ds_pdn_mgr_change_byte_order(info_p->ipcp_info.primary_dns),
               ds_pdn_mgr_change_byte_order(info_p->ipcp_info.secondary_dns),
               valid_fields
             );
    }
    else
    {
    /*-------------------------------------------------------------------------
        Request Primary and Secondary DNS addresses from the network.
        Since we are populating values from profile, only request DNS address.
      -------------------------------------------------------------------------*/
      valid_fields = (net_cfg_params_mask_e_type)
                 (NET_CFG_PRM_PRI_DNS_MASK | NET_CFG_PRM_SEC_DNS_MASK);

#ifdef FEATURE_GSM_DTM
      /* In case of reduced PCO we need to ignore secondary address requests. */
      if (rr_dtm_get_max_LAPDm_octets() != 0)
      {
        valid_fields &= ~(NET_CFG_PRM_SEC_DNS_MASK);
        DS_MSG0(MSG_LEGACY_HIGH,"Secondary DNS container omitted");
      }
#endif  

      result = ds_pdn_mgr_fill_ipcp_orig_params
               (
                 ds_proto_config_option_p,
                 FALSE,
                 profile_p->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4,
                 profile_p->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4,
                 valid_fields
               );
    }

    
    /*-------------------------------------------------------------------------
      Check if we have to fill this protocol info pkt in the pdp 
      context request based on NV item 3gpp_rel_support,
      if mode is not LTE. For LTE always fill this container.
    -------------------------------------------------------------------------*/
    if ( rel_version >= (uint8)DS_3GPP_REL8 )
    {
      result = ds_pdn_mgr_fill_ipv4_dns_container
               (
                 ds_proto_config_option_p
               );
    }
  }
  
  if((profile_p->context.pdp_type == DS_PROFILE_PDP_IPV6) ||
     (profile_p->context.pdp_type == DS_PROFILE_PDP_IPV4V6))
  {
    result = ds_pdn_mgr_fill_ipv6_dns_params
             (
               ds_proto_config_option_p
             );
  }

  return;
}



/*===========================================================================
FUNCTION DS_GPRS_PDN_GET_ORIG_PARAMS

DESCRIPTION
This function is used to fill up the orig params for GSM data call.

PARAMETERS
  pdn_cntx_p       - Pointer to the PDN context
  mm_orig_params_p - Pointer to the orig params structure

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_gprs_pdn_get_orig_params
(
  ds_pdn_mgr_s                    *pdn_mgr_p,
  mm_pdp_activate_req_T           *gprs_orig_params
)
{
  ds_profile_type_s               *profile_p = NULL;
  ds_pdn_protocol_cfg_options_s   ds_proto_config_option = {0};
  uint16                           loop = 0;  
  uint8                            apn_encoded_length = 0;
  ds_profile_pdp_type_e            pdp_type;
  uint8                            override_ip_bmask = 
                                    DS_3GPP_PDP_UNDEFINED_BMASK;
  sys_plmn_id_s_type               current_plmn = {0};
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  apn_encoded_length = ds_pdn_mgr_store_encoded_apn(
                               (byte*)gprs_orig_params->apn_name.address, 
                               (byte*)pdn_mgr_p->pdp_profile->context.apn);
  if(apn_encoded_length > 1)
  {
    gprs_orig_params->apn_name.valid = TRUE;

    /*-----------------------------------------------------------------------
      Fill the total length of the encoded APN
    -----------------------------------------------------------------------*/
    gprs_orig_params->apn_name.apn_addr_len = apn_encoded_length;
  }

  gprs_orig_params->low_priority_signaling = 
           	ds_pdn_mgr_get_low_priority_signaling_bit(pdn_mgr_p);
  
  /*-------------------------------------------------------------------------
     Fill in compression parameters.
    -------------------------------------------------------------------------*/
  gprs_orig_params->h_comp = ds_pdn_mgr_conv_header_comp_from_pdp_t_to_sys_t(
                                    &pdn_mgr_p->pdp_profile->context.h_comp);
  gprs_orig_params->d_comp = ds_pdn_mgr_conv_data_comp_from_pdp_t_to_sys_t(
                                    &pdn_mgr_p->pdp_profile->context.d_comp);

  /*-------------------------------------------------------------------------
    Fill in PDP address, The IE is defined in 10.5.6.4.
    For PDP type IP with dynamic IP address,
    no address is required in this information element
    If a static IP address is requested in the AT+CGDCONT command, use it
    -------------------------------------------------------------------------*/
  gprs_orig_params->pdp_addr.valid = TRUE;
  gprs_orig_params->pdp_addr.pdp_type_org = 1;
  gprs_orig_params->pdp_addr.pdp_addr_len  = 0;    /* default-dynamic IP */

  profile_p = pdn_mgr_p->pdp_profile;
  pdp_type = profile_p->context.pdp_type;
  if(pdp_type == DS_PROFILE_PDP_IPV4)
  {
    gprs_orig_params->pdp_addr.pdp_type_num  = DS_3GPP_PDP_TYPE_IPV4_NUM; /* PDP type - IP-V4   */      
  }
  else if(pdp_type == DS_PROFILE_PDP_IPV6)
  {
    gprs_orig_params->pdp_addr.pdp_type_num  = DS_3GPP_PDP_TYPE_IPV6_NUM; /* PDP type - IP-V6   */
  }
  else if(pdp_type == DS_PROFILE_PDP_IPV4V6)
  {
   /* -----------------------------------------------------------------
       For Rel 11+, Check if an apn is network overridden, If it is
       overridden overwrite the IP type that needs to be sent out
       Also update the internal cache and apn table to reflect
       information correctly
       ------------------------------------------------------------------*/
    ds_pdn_hdlr_get_current_plmn(&current_plmn);
    override_ip_bmask = ds_3gpp_network_override_hdlr_get_overridden_ip
                  ((byte *)profile_p->context.apn,DS_3GPP_THROT_RAT_UMTS, 
                    &current_plmn);

    DS_MSG1(MSG_LEGACY_HIGH, "Network_override_IP is %d", override_ip_bmask);

    if (DS_3GPP_NW_OVERRIDE_IPV4_ALLOWED(override_ip_bmask))
    {
      gprs_orig_params->pdp_addr.pdp_type_num  = DS_3GPP_PDP_TYPE_IPV4_NUM;
      profile_p->context.pdp_type = DS_PROFILE_PDP_IPV4;
      (void)ds_apn_table_update_apn_table(profile_p->context.apn,
                                          DS_IPV4_ONLY, pdn_mgr_p); 
    }
    else if (DS_3GPP_NW_OVERRIDE_IPV6_ALLOWED(override_ip_bmask))
    {
      gprs_orig_params->pdp_addr.pdp_type_num  = DS_3GPP_PDP_TYPE_IPV6_NUM;
      profile_p->context.pdp_type = DS_PROFILE_PDP_IPV6;
      (void)ds_apn_table_update_apn_table(profile_p->context.apn,
                                          DS_IPV6_ONLY, pdn_mgr_p); 
    }
    /* Check if NON-IP needs to be added*/
    else
    {
      gprs_orig_params->pdp_addr.pdp_type_num  = DS_3GPP_PDP_TYPE_IPV4V6_NUM;
    }
  }
  else
  {
    DS_ERROR_LOG_0("Invalid IP type");
    return FALSE;
  }

  ds_gprs_pdn_ip_fill_ppp_params(gprs_orig_params,
  	                              profile_p,
  	                              &pdn_mgr_p->info,
  	                              &ds_proto_config_option);

#ifdef FEATURE_DATA_IMS
  /*-------------------------------------------------------------------------
    Get the request_pcscf_address_flag from profile registry to see if pcscf
    address should be requested for this profile.
  -------------------------------------------------------------------------*/
  /* Create a pcscf address request */
  if( ds_pdn_mgr_gen_pcscf_address_request_packet(&ds_proto_config_option,
                                                  profile_p) == FALSE )
  {
    /*-------------------------------------------------------------------
      If an error occurs in encoding pkt, do not include the pkt but
      proceed with call If we return error here, call setup would fail and
       the user would'nt know why?  and all subsequent call attemtps for the
       PDP ctxt would also fail (using the same params)
    ------------------------------------------------------------------*/
    DS_ERROR_LOG_0("PCSCF Address request packet generation failed");
    goto memfree;
  }

  /*-------------------------------------------------------------------------
    Get the im_cn_flag from call table to see if im cn flag should be
    requested for this call.
  -------------------------------------------------------------------------*/
  if(profile_p->im_cn_flag == TRUE)
  {
    if( ds_bearer_mgr_is_IMCN_call_up() == TRUE)
    {
      DS_ERROR_LOG_0("Call with IM CN Flag already up");
      goto memfree;
    }
    else
    {
      if(ds_pdn_mgr_gen_imcn_subsystem_request_packet(
          &ds_proto_config_option) == FALSE)
      {
        DS_ERROR_LOG_0("IM CN flag generate failed.");
      }
    }
  }
#endif //FEATURE_DATA_IMS  

  ds_pdn_mgr_fill_bcm_indication(&ds_proto_config_option);

  /*-------------------------------------------------------------------------
    Fill IPV4 Link MTU Request if profile pdp_type is IPV4 or IPV4V6
  --------------------------------------------------------------------------*/
  ds_pdn_mgr_fill_ipv4v6_link_mtu_container(&ds_proto_config_option);


  // OP PCO from app required ?? then add later 

  /*-------------------------------------------------------------------------
    Fill local address support in TFT container
  --------------------------------------------------------------------------*/    
  if( ds_pdn_nv_manager_is_rel_11_or_above_supported())
  {        
    ds_pdn_mgr_fill_ipv4v6_local_address_support_container(&ds_proto_config_option);            
  }

  gprs_orig_params->config_options.valid = 
                  ds_pdn_pco_epco_hdlr_convert_ds_pco_to_nas
                      (&ds_proto_config_option,&(gprs_orig_params->config_options));

  /*-------------------------------------------------------------------------
    Fill the mandatory QoS parameters.
  --------------------------------------------------------------------------*/    
  memset(&(gprs_orig_params->qos), 0, sizeof(qos_T));
  gprs_orig_params->qos.valid = TRUE;

  memset(&(gprs_orig_params->minimum_qos), 0, sizeof(qos_T));
  gprs_orig_params->minimum_qos.valid = TRUE;

  return TRUE;

memfree:
  for ( loop = 0; loop < ds_proto_config_option.num_protocol_ids; loop ++)
  {
    DS_SYSTEM_HEAP_MEM_FREE( ds_proto_config_option.protocol_info_p[loop]);
    ds_proto_config_option.protocol_info_p[loop] = NULL;   
  }
  
  return FALSE;  
}


/*===========================================================================
FUNCTION DS_GPRS_BEARER_MGR_BRING_UP_PDP

DESCRIPTION
  This function initiates call bring up for GSM.
  
PARAMETERS  
  bearer ptr
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
boolean ds_gprs_bearer_mgr_bring_up_pdp
(
  ds_bearer_mgr_s   *bearer_mgr_p
)
{
  mm_pdp_activate_req_T      *gprs_orig_params = NULL; /* Pointer to orig params*/
  mm_restore_pdp_ctxt_req_s_type  *restore_bearer_req_ptr = NULL;
  ds_pdn_mgr_s               *pdn_mgr_p = NULL;
  byte                        default_dial_str[NV_PKT_DIAL_STRING_SIZE] = {0};  
//  dsat_dial_val_e_type        dial_str_digits_type = DSAT_DIAL_MAX;
  byte                        pkt_orig_str[5];
  boolean                     result = FALSE;
  uint8                       psm_index = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
  /* Get the PDN context */
  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    DS_ERROR_FATAL("NULL PDN");
    return FALSE;
  }

  if (ds_pdn_nv_manager_read_efs_nv( 
  	                 "/nv/item_files/modem/data/3gpp/ps/3gpp_data_pkt_orig_str",
                    (uint8 *)&(default_dial_str),
                     sizeof(default_dial_str)) != NV_DONE_S)
  {
    DS_MSG0(MSG_LEGACY_HIGH, "default_dial_str set in EFS");
  }
  else
  {
    (void)strlcpy( (char *)default_dial_str,
                     "*98#",
                     NV_PKT_DIAL_STRING_SIZE );
  }
  
  default_dial_str[NV_PKT_DIAL_STRING_SIZE - 1] = 0; 

  if(pdn_mgr_p->info.valid_fields & (uint32)NET_CFG_PRM_DIAL_STR_MASK)
  {
    memscpy( pkt_orig_str, sizeof(pkt_orig_str),
             pdn_mgr_p->info.dial_string, NV_PKT_DIAL_STRING_SIZE - 1 );
  }
  else
  {
    memscpy( pkt_orig_str, sizeof(pkt_orig_str),
             default_dial_str, NV_PKT_DIAL_STRING_SIZE - 1 );    
  }

  pkt_orig_str[NV_PKT_DIAL_STRING_SIZE - 1] = 0;

  /*-------------------------------------------------------------------
    Process the dial string for the call for validity else prevent call
    origination to go through if dial string is not valid.
  -------------------------------------------------------------------*/
//  dial_str_digits_type = dsat_proc_dial_str(pkt_orig_str, pkt_orig_str);

  
  if ( 0/*!(dial_str_digits_type == DSAT_DIAL_DIGIT ||
       dial_str_digits_type == DSAT_DIAL_ASCII) */)  
  {
    DS_ERROR_LOG_0("Invalid dial string. Bring up failed.");
    return FALSE;
  }

  DS_SYSTEM_HEAP_MEM_ALLOC(gprs_orig_params, sizeof(mm_pdp_activate_req_T),
  	                        mm_pdp_activate_req_T*);
  if(gprs_orig_params == NULL)
  {
    return FALSE;
  }
  
  DS_SYSTEM_HEAP_MEM_ALLOC(restore_bearer_req_ptr,
  	                       sizeof(mm_restore_pdp_ctxt_req_s_type),
  	                       mm_restore_pdp_ctxt_req_s_type*);
  if(restore_bearer_req_ptr == NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(gprs_orig_params);
    return FALSE;
  }
  
  /* Memset the orig params structure to 0 */
  memset(gprs_orig_params, 0, sizeof(mm_pdp_activate_req_T));
  memset(restore_bearer_req_ptr, 0, sizeof(mm_restore_pdp_ctxt_req_s_type));
        
  if(bearer_mgr_p->is_default)
  {               
    result = ds_gprs_pdn_get_orig_params(pdn_mgr_p, gprs_orig_params);
  }

  if(result)
  {
    /* Fill the trans/conn ID so that it is used during cnf/reject message from NAS */
    gprs_orig_params->connection_id = pdn_mgr_p->index;
    bearer_mgr_p->sdf_id = pdn_mgr_p->index;

#if 0  
	   if( bearer_mgr_p->is_default == TRUE)
    {
      gprs_orig_params->primary_pdp = TRUE;
    }
    else
    {
      orig_params_p->orig_params_u.gw_ps_orig_params_ptr->primary_pdp = FALSE;   
      orig_params_p->orig_params_u.gw_ps_orig_params_ptr->assoc_pri_profile_num=
          (uint16) (((ds_pdn_context_s*)
  	  (bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr))->ds_pdn_context_dyn_p->pdp_profile_num);    
      orig_params_p->orig_params_u.gw_ps_orig_params_ptr->pdp_group_id =   
          (byte)((ds_pdn_context_s*) (bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr))->
        ds_pdn_context_dyn_p->pdp_group_id;                                    
    }
#endif
    
    // FILL QoS params if confirmed later  -- dsUmtsBearerCntxt_fill_origination_qos_params
    
  }
  
  if(result == TRUE)
  {
    /* PSM handling */
    psm_index = ds_pdn_psm_info_find_current_index();
  
    if( psm_index != DS_3GPP_MAX_BEARER_CONTEXT)
    {
#ifdef FEATURE_GSM_GPRS

       if(ds_pdn_psm_fill_pdp_restore_ctxt_req( restore_bearer_req_ptr,
                                                             pdn_mgr_p->nw_mode))
       {
         restore_bearer_req_ptr->message_header.message_id = MM_RESTORE_PDP_CTXT_REQ;
         restore_bearer_req_ptr->message_header.message_set = MS_MM_DS;
         restore_bearer_req_ptr->message_header.message_len_lsb =
           (sizeof( mm_restore_pdp_ctxt_req_s_type ) - sizeof(IMH_T)) % 256;
         restore_bearer_req_ptr->message_header.message_len_msb =
           (sizeof( mm_restore_pdp_ctxt_req_s_type ) - sizeof(IMH_T)) / 256;
		 DS_MSG0(MSG_LEGACY_HIGH,"Call re-origination");   
		 ds_pdn_mgr_ds_to_nas_send_msg((void*)restore_bearer_req_ptr);	 
         }
#endif /* FEATURE_GSM_GPRS */
       }
    else
    {
      gprs_orig_params->message_header.message_id = MM_PDP_ACTIVATE_REQ;
      gprs_orig_params->message_header.message_set = MS_MM_DS;      
      gprs_orig_params->message_header.message_len_lsb =
        (sizeof( mm_pdp_activate_req_T ) - sizeof(IMH_T)) % 256;
      gprs_orig_params->message_header.message_len_msb =
        (sizeof( mm_pdp_activate_req_T ) - sizeof(IMH_T)) / 256;
      
      DS_MSG0(MSG_LEGACY_HIGH,"Call origination");   
      ds_pdn_mgr_ds_to_nas_send_msg((void*)gprs_orig_params);   
    }
  }

  DS_SYSTEM_HEAP_MEM_FREE(gprs_orig_params);
  DS_SYSTEM_HEAP_MEM_FREE(restore_bearer_req_ptr);
  
  return result;
}

/*===========================================================================
FUNCTION ds_gprs_mgr_hndl_unkown_cause

DESCRIPTION
   This function is used to handle cases for CAUSE CODE set to UNKNOWN

PARAMETERS
  Pdp_type_ctx_req: IP type requested to N/W
  pdp_type_num : IP type returned by N/W
  Pdn_mgr_p  : PDN manager pointer
  ip_support   : Updated IP type

DEPENDENCIES
  None.

RETURN VALUE 
  TRUE   SUCCESS.
  FALSE  FAILURE. 

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_gprs_mgr_hndl_unknown_cause
( 
  ds_profile_pdp_type_e         req_pdp_type,
  byte                          pdp_type_num,
  ds_pdn_mgr_s                 *pdn_cntx_p,
  ds_apn_ip_support_type_e     *ip_support
)
{

  boolean                  result = TRUE;
  boolean                  is_redial_allowed = FALSE;  
/*-------------------------------------------------------------------------*/

  is_redial_allowed = ds_pdn_nv_manager_get_redial_no_cause_code_from_nv();

  DS_MSG3(MSG_LEGACY_HIGH, 
  	       "Received invalid cause code for req PDP %d, accepted PDP type %d "
  	       "is redial allowed %d",
          req_pdp_type, pdp_type_num, is_redial_allowed); 

  switch(req_pdp_type)
  {
    case DS_PROFILE_PDP_IPV4:
      if( pdp_type_num == DS_3GPP_PDP_TYPE_IPV6_NUM )
      {
        result = FALSE;
      }
      break;

    case DS_PROFILE_PDP_IPV6:
      if( pdp_type_num == DS_3GPP_PDP_TYPE_IPV4_NUM )
      {
        result = FALSE;
      }
      break;

    case DS_PROFILE_PDP_IPV4V6:
    {
      switch (pdp_type_num)
      {
        case DS_3GPP_PDP_TYPE_IPV4_NUM:
        {
          if (is_redial_allowed)
          {
            *ip_support = DS_IPV4_SINGLE_BEARER;
          }
          else
          {
            *ip_support =  DS_IPV4_ONLY;

          }

          ds_pdn_mgr_dissociate_instance(pdn_cntx_p, *ip_support);
          break;
        }

        case DS_3GPP_PDP_TYPE_IPV6_NUM:
        {
          if (is_redial_allowed)
          {
            *ip_support = DS_IPV6_SINGLE_BEARER;
          }
          else
          {
            *ip_support =  DS_IPV6_ONLY;
          }

          ds_pdn_mgr_dissociate_instance(pdn_cntx_p, *ip_support);
          break;
        }

        case DS_3GPP_PDP_TYPE_IPV4V6_NUM:
          break;

        default:
          result = FALSE;
          break;
      }
      break;
    }
    break;

    default:
      result = FALSE;
      break;  
  }
  
  return result;
}/*ds_gprs_mgr_hndl_unknown_cause */

boolean ds_gprs_mgr_verify_pdp_type
(
  mm_pdp_activate_cnf_T  *pdp_act_cnf, 
  ds_pdn_mgr_s           *pdn_mgr_p
)
{  
  boolean                   rel12_plus_support = FALSE;
  byte                      pdp_type_org;
  byte                      pdp_type_num;
  ds_profile_pdp_type_e     req_pdp_type;
  ds_profile_pdp_type_e     accept_pdp_type;
  ds_apn_ip_support_type_e  ip_support;
  boolean                   result = TRUE;
  reattempt_ind_T           reattempt_indicator = {0};
  
  rel12_plus_support = ds_pdn_nv_manager_is_rel_12_or_above_supported();

  /*-------------------------------------------------------------------------
     Determine the PDP type received from context accept message. 
     TS 24.008: pdp_type_orig is only 4 bits 
   -------------------------------------------------------------------------*/
  pdp_type_org = pdp_act_cnf->pdp_addr.pdp_type_org & 0x0F;
  pdp_type_num = pdp_act_cnf->pdp_addr.pdp_type_num;

  req_pdp_type = pdn_mgr_p->pdp_profile->context.pdp_type;

  pdn_mgr_p->gsm_params->pdp_type_org = pdp_act_cnf->pdp_addr.pdp_type_org;
  pdn_mgr_p->gsm_params->pdp_type_num = pdp_act_cnf->pdp_addr.pdp_type_num;
  pdn_mgr_p->gsm_params->pdp_addr_len = pdp_act_cnf->pdp_addr.pdp_addr_len;
    
  /*----------------------------------------------------------------------
    We assume the IP support type of this APN to be what we requested,
    this info will be adjusted based on ESM cause code if it is returned
  ----------------------------------------------------------------------*/
  switch(req_pdp_type)
  {
    case DS_PROFILE_PDP_IPV4:
      ip_support = DS_IPV4_SINGLE_BEARER;
      break;
    case DS_PROFILE_PDP_IPV6:
      ip_support = DS_IPV6_SINGLE_BEARER;
      break;
    case DS_PROFILE_PDP_IPV4V6:
      ip_support = DS_IPV4V6;
      break;
    default:
      DS_ERROR_LOG_0("Unknown PDP type.");
      return FALSE;
  }

  DS_MSG4(MSG_LEGACY_HIGH, 
  	       "Received cause code %d for req PDP %d, "
  	       "accepted PDP type num %d , PDP type org %d ",
          pdp_act_cnf->cause.ext_cause, req_pdp_type, 
          pdp_type_num, pdp_type_org); 

  if(pdp_act_cnf->cause.ext_cause != SM_UNKNOWN_CAUSE)
  {
    switch(req_pdp_type)
    {
      case DS_PROFILE_PDP_IPV4:
        if(pdp_act_cnf->cause.ext_cause == SM_PDP_TYPE_IPV6_ONLY_ALLOWED)
      	 {
          ip_support = DS_IPV6_ONLY;
      	   result = FALSE;

          if(rel12_plus_support)
          {
            ds_3gpp_network_override_hdlr_set_overridden_ip(
               (byte *)pdn_mgr_p->pdp_profile->context.apn,
               DS_3GPP_PDN_THROT_PDP_IPV6,
               DS_3GPP_PDN_THROT_BEARER_ACT_ACCEPT,
               DS_3GPP_THROT_RAT_MASK_UMTS,
               reattempt_indicator);
          }
      	 }
        break;
        
      case DS_PROFILE_PDP_IPV6:
        if(pdp_act_cnf->cause.ext_cause == SM_PDP_TYPE_IPV4_ONLY_ALLOWED)
       	{
          ip_support = DS_IPV4_ONLY;
          result = FALSE;

            ds_3gpp_network_override_hdlr_set_overridden_ip(
               (byte *)pdn_mgr_p->pdp_profile->context.apn,
               DS_3GPP_PDN_THROT_PDP_IPV4,
               DS_3GPP_PDN_THROT_BEARER_ACT_ACCEPT,
               DS_3GPP_THROT_RAT_MASK_UMTS,
             reattempt_indicator);
        }
      	 break;
      	 
      case DS_PROFILE_PDP_IPV4V6:
        if(pdp_act_cnf->cause.ext_cause == SM_PDP_TYPE_IPV4_ONLY_ALLOWED)
        {
          ip_support = DS_IPV4_ONLY;

            ds_3gpp_network_override_hdlr_set_overridden_ip(
               (byte *)pdn_mgr_p->pdp_profile->context.apn,
               DS_3GPP_PDN_THROT_PDP_IPV4,
               DS_3GPP_PDN_THROT_BEARER_ACT_ACCEPT,
               DS_3GPP_THROT_RAT_MASK_UMTS,
             reattempt_indicator);
        }
        else if(pdp_act_cnf->cause.ext_cause == SM_PDP_TYPE_IPV6_ONLY_ALLOWED)
        {
          if(ds_pdnmgr_is_prof_clat_enabled(pdn_mgr_p->pdp_profile_num))
          {
            pdn_mgr_p->cache_v4_inst_down = TRUE;
            ip_support = DS_IPV6_ONLY;
            break;
          }
          else
          {
          ip_support = DS_IPV6_ONLY;

            ds_3gpp_network_override_hdlr_set_overridden_ip(
               (byte *)pdn_mgr_p->pdp_profile->context.apn,
               DS_3GPP_PDN_THROT_PDP_IPV6,
               DS_3GPP_PDN_THROT_BEARER_ACT_ACCEPT,
               DS_3GPP_THROT_RAT_MASK_UMTS,
             reattempt_indicator);
        }
        }
        else if(pdp_act_cnf->cause.ext_cause == SM_SINGLE_ADR_BEARERS_ONLY_ALLOWED)
        {
          if(pdp_act_cnf->pdp_addr.pdp_type_num == DS_3GPP_PDP_TYPE_IPV4_NUM)
          {
            ip_support = DS_IPV4_SINGLE_BEARER;
          }
          else if (pdp_act_cnf->pdp_addr.pdp_type_num == DS_3GPP_PDP_TYPE_IPV6_NUM)
          {
            ip_support = DS_IPV6_SINGLE_BEARER;
          }
          else
          {
            DS_ERROR_LOG_1("Incorrect PDN address type: =d",
                           pdp_act_cnf->pdp_addr.pdp_type_num);
            result = FALSE;
          }         
        }
        else
        {
          break;
        }

        ds_pdn_mgr_dissociate_instance(pdn_mgr_p, ip_support);
        break;
        
      default:
       	result = FALSE;
        break;
    }
  } // if( ext_cause != SM_UNKNOWN_CAUSE )
  else
  {
    result = ds_gprs_mgr_hndl_unknown_cause(
                                      req_pdp_type,
                                      pdp_type_num,
                                      pdn_mgr_p,
                                      &ip_support);
  }  

  if(result == FALSE)
  {
    return FALSE;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "IP support = %d", ip_support);
  switch(ip_support)
  {
    case DS_IPV4_ONLY:
    case DS_IPV4_SINGLE_BEARER:
      if( pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4] == NULL ||
      	   (pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4]->in_use == FALSE &&
            pdn_mgr_p->is_restore_mode == FALSE) )
      {
          /*------------------------------------------------------------------
            Reject bearer activation since IP V6 pipe cannot be obtained
            If a V6 pipe were needed, the request would have been made while
            dissociating. It is safe to reject this bearer activation
         ------------------------------------------------------------------*/       
        return FALSE;
      }
      break;
      
    case DS_IPV6_ONLY:
    case DS_IPV6_SINGLE_BEARER:
     if( pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6] == NULL ||
          (pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6]->in_use == FALSE &&
           pdn_mgr_p->is_restore_mode == FALSE) )
      {
          /*------------------------------------------------------------------
            Reject bearer activation since IP V4 pipe cannot be obtained
            If a V4 pipe were needed, the request would have been made while
            dissociating. It is safe to reject this bearer activation
          ------------------------------------------------------------------*/  
        return FALSE;
      } 
      break;

    default:
      break;
  }

  /*-------------------------------------------------------------------------
     TS 24.008- 
     If PDP org type is of type ETSI(0x0), pdp_type_num should be
     0x01 for PDP type PPP. All other values are reserved. 
     if PDP org type is of type IETF(0x1), pdp_type_num should be 0x21 (IPV4)
     or 0x57 (IPV6). All other values should be treated as IPv4.
  -------------------------------------------------------------------------*/
  if(pdp_type_org == 0)
  {
    return FALSE;
  } /* pdp_type_org == PPP */
  else if(pdp_type_org == 0x01)
  {
    if( ( pdp_type_num !=  DS_3GPP_PDP_TYPE_IPV6_NUM) &&
        ( pdp_type_num !=  DS_3GPP_PDP_TYPE_IPV4V6_NUM) )
    {
      if(ip_support != DS_IPV4_ONLY &&
         ip_support != DS_IPV4_SINGLE_BEARER)
      {
        return FALSE;
      }
      
      accept_pdp_type = DS_PROFILE_PDP_IPV4;
    }
    else if( pdp_type_num ==  DS_3GPP_PDP_TYPE_IPV6_NUM)   
    {
      if(ip_support != DS_IPV6_ONLY &&
         ip_support != DS_IPV6_SINGLE_BEARER)
      {
        return FALSE;
      }
      
      accept_pdp_type = DS_PROFILE_PDP_IPV6;
    }
    else if( pdp_type_num ==  DS_3GPP_PDP_TYPE_IPV4V6_NUM)   
    {
      if(ip_support != DS_IPV4V6)
      {
        return FALSE;
      }
      
      accept_pdp_type = DS_PROFILE_PDP_IPV4V6;
    }
  } /* pdp_type_org == IP */  
  else
  { 
    return FALSE;   
  }

  /*-------------------------------------------------------------------------
    Update APN table with APN name and IP support
  -------------------------------------------------------------------------*/
  if (ds_apn_table_update_apn_table( pdn_mgr_p->pdp_profile->context.apn,
                                     ip_support,
                                     pdn_mgr_p ) != TRUE)
  {
    return FALSE;
  }
  
  return TRUE;   
}

#ifdef FEATURE_DATA_IMS
/*===========================================================================
FUNCTION DS_GPRS_MGR_DECODE_PROTOCOL_PARAMS

DESCRIPTION
  This function decodes the IMS protocol parameters received for a PDP-IP call.

PARAMETERS
   mm_pdp_activate_cnf_T - PDP ACT CNF pointer
   bearer_mgr_s - Bearer ptr

DEPENDENCIES
  None.

RETURN VALUE
  TRUE : Successful setup
  FALSE : Setup error

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_gprs_mgr_decode_protocol_params
(
  mm_pdp_activate_cnf_T   *pdp_act_cnf,
  ds_bearer_mgr_s         *bearer_mgr_p  
)
{
  word    num_pids    = 0;                       /* Number of Protocol IDs */
  word    protocol_id = 0;                                  /* Protocol ID */
  boolean valid;                          /* Protocol options valid or not */
  byte    index       = 0;                       /* Protocol options index */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  valid = pdp_act_cnf->config_options.valid;
  if (valid == FALSE)
  {
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Get the number of protocol IDs received. Max is NUM_PROTOCOL_ID ( for PCO)
    MAX_NUM_EPCO_PROTOCOL_ID ( for EPCO )
  -------------------------------------------------------------------------*/
  num_pids = pdp_act_cnf->config_options.num_protocol_ids;

  /*-------------------------------------------------------------------------
    Process the protocol IDs received.
  -------------------------------------------------------------------------*/
  while (num_pids > 0)
  {
    /*----------------------------------------------------------------------
      Get the Protocol ID.
    ----------------------------------------------------------------------*/
    protocol_id = pdp_act_cnf->config_options.
                              procol_info[index].protocol_id[0] << 8;
    protocol_id |= pdp_act_cnf->config_options.
                              procol_info[index].protocol_id[1];

    switch (protocol_id)
    {
      case IM_CN_SUBSYSTEM_PROTOCOL:
        DS_MSG2(MSG_LEGACY_HIGH, "For Bearer  %d Decode: Protocol ID %d",
                bearer_mgr_p->index,protocol_id);
        /*-------------------------------------------------------------------
           Decode IM CN Subsystem packet
        -------------------------------------------------------------------*/
        if(pdp_act_cnf->config_options.procol_info[index].protocol_len != 0)
        {
          bearer_mgr_p->im_cn_subsystem_flag = FALSE;
        }
        else
        {
          DS_MSG0(MSG_LEGACY_HIGH, "IMCN flag set");
          bearer_mgr_p->im_cn_subsystem_flag = TRUE;
        }
        break;

      default:
        // No-op
        break;
    }
    index++;
    num_pids--;
  }

  return TRUE;
} /* ds_gprs_mgr_decode_protocol_params() */
#endif // FEATURE_DATA_IMS

/*===========================================================================
FUNCTION DS_GPRS_MGR_SNDCP_REG_REQ_CNF

DESCRIPTION
  This function posts a command to set up the transmit function once 
  registration with SNDCP is complete.

PARAMETERS
  Context - PDP context instance
  cnf_status - Indicates whether registration was successful or not.
  Setup the transmit function only if registration was successful. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_gprs_mgr_sndcp_reg_req_cnf
(
  void          *context,
  boolean        cnf_status
)
{
  ds_command_type                *cmd_ptr;          
  ds_bearer_mgr_s                *bearer_mgr_p = (ds_bearer_mgr_s*)context;
  ds_gprs_gsndcp_reg_cnf_ev_type *reg_cnf_ev_ptr = NULL;

  DS_MSG0(MSG_LEGACY_HIGH, "DS3GPP GSNDCP REG CNF MSG");

  /*------------------------------------------------------------------------
    Post command to DS task to process in the proper task context
   ------------------------------------------------------------------------*/

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_gprs_gsndcp_reg_cnf_ev_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DS_ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_GSNDCP_REG_CNF;
  reg_cnf_ev_ptr = (ds_gprs_gsndcp_reg_cnf_ev_type*)cmd_ptr->cmd_payload_ptr;

  reg_cnf_ev_ptr->bearer_mgr_p = bearer_mgr_p;
  reg_cnf_ev_ptr->cnf_status = cnf_status;

  ds_put_cmd(cmd_ptr);
} /* ds_gprs_mgr_sndcp_reg_req_cnf */

/*===========================================================================
FUNCTION DS_GPRS_MGR_TX_UM_DATA_CB

DESCRIPTION
  This function forwards pkts received to the lower layers for 
  pdp-ip call.
  This function is registered with FWK for a PDP-IP call 
  
PARAMETERS  
  *bearer_ptr      - Bearer info.
  **item_ptr       - Payload
  *meta_info_ptr   - Not used.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_gprs_mgr_tx_um_data_cb
(
  ds_bearer_mgr_s    *bearer_mgr_p,
  dsm_item_type     **item_ptr,
  ps_meta_info_type  *meta_info_ptr
)
{
  ds_bearer_mgr_wm_s  *tx_watermark_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    return;
  }
  /*-------------------------------------------------------------------------
    Get PDCP state information for call.
  -------------------------------------------------------------------------*/
  tx_watermark_ptr = &(bearer_mgr_p->tx_wm);

  /*-------------------------------------------------------------------------
    Enqueue this item to PDCP
  -------------------------------------------------------------------------*/
  DS_MSG1(MSG_LEGACY_HIGH, "ip tx um data cb. Bearer %d ", bearer_mgr_p->index);

  dsm_enqueue(&tx_watermark_ptr->wm_item, item_ptr);

//  PS_META_INFO_FREE(&meta_info_ptr);
} /* ds_gprs_mgr_tx_um_data_cb() */

/*===========================================================================
FUNCTION DS_GPRS_MGR_SETUP_PS_TO_SNDCP_WM

DESCRIPTION
  This function initializes the PS->SNDCP watermark for PDP calls.
  
PARAMETERS
  *bearer_mgr_p - This is the bearer to work with.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
void ds_gprs_mgr_setup_ps_to_sndcp_wm
(
  ds_bearer_mgr_s     *bearer_mgr_p
)
{
  ds_bearer_mgr_wm_s      *watermark_p;
  ds_bearer_mgr_um_wm_threshold_struct_type  um_wm_threshold;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  um_wm_threshold = ds_pdn_nv_get_um_wm_threshold();
  watermark_p = &(bearer_mgr_p->tx_wm);  
  
  /*-------------------------------------------------------------------------
    Set up PS->SNDCP watermark 
  -------------------------------------------------------------------------*/
  dsm_set_low_wm(&(watermark_p->wm_item), 
                 um_wm_threshold.gprs_ul_lo);
  dsm_set_hi_wm(&(watermark_p->wm_item),
                um_wm_threshold.gprs_ul_hi);
  /*-------------------------------------------------------------------------
    Set the threshold for PS->SNDCP Q. 
    SNDCP to PS can hold a max of 3 IPpkt 
  -------------------------------------------------------------------------*/
  dsm_set_dne(&(watermark_p->wm_item),
              um_wm_threshold.gprs_ul_dne);
  dsm_set_dne_q_cnt(&(watermark_p->wm_item),DSGPSD_UL_WM_DEFAULT_DNE_QCNT);
  
   /*-----------------------------------------------------------------------
     Pass the bearer ptr in watermarks registered so that we
     can identify the call when the callback is called
   -----------------------------------------------------------------------*/
  watermark_p->wm_item.lowater_func_ptr = ds_bearer_mgr_wm_flow_enable_cb;
  watermark_p->wm_item.lowater_func_data = (void*)bearer_mgr_p;
  
  watermark_p->wm_item.hiwater_func_ptr = ds_bearer_mgr_wm_flow_disable_cb;
  watermark_p->wm_item.hiwater_func_data = (void*)bearer_mgr_p;

  watermark_p->wm_item.gone_empty_func_ptr   = NULL;
  watermark_p->wm_item.non_empty_func_ptr    = NULL;
  watermark_p->wm_item.each_enqueue_func_ptr = NULL;

  return;
} /* ds_gprs_mgr_setup_ps_to_sndcp_wm() */

/*===========================================================================
FUNCTION DS_GPRS_MGR_SETUP_SNDCP_TO_PS_WM

DESCRIPTION
  This function initializes the PS->SNDCP watermark for PDP calls.
  
PARAMETERS
  *bearer_mgr_p - This is the bearer to work with.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
void ds_gprs_mgr_setup_sndcp_to_ps_wm
(
  ds_bearer_mgr_s    *bearer_mgr_p
)
{
  ds_bearer_mgr_wm_s      *watermark_p;
  ds_bearer_mgr_um_wm_threshold_struct_type  um_wm_threshold;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  um_wm_threshold = ds_pdn_nv_get_um_wm_threshold();
  watermark_p = &(bearer_mgr_p->rx_wm);  

  dsm_set_low_wm(&(watermark_p->wm_item), 
                 um_wm_threshold.gprs_dl_lo);
  dsm_set_hi_wm(&(watermark_p->wm_item),
                um_wm_threshold.gprs_dl_hi);
  dsm_set_dne(&(watermark_p->wm_item), 
              um_wm_threshold.gprs_dl_dne);
  dsm_set_dne_q_cnt(&(watermark_p->wm_item),DSGPSD_DL_WM_DEFAULT_DNE_QCNT);
  
  /*-----------------------------------------------------------------------
    SNDCP will implement the WM based CBs for flow control.
  -----------------------------------------------------------------------*/
  
  //watermark_p->wm_item.gone_empty_func_ptr  = NULL;
  
  //watermark_p->wm_item.lowater_func_ptr     = NULL;
  
  //watermark_p->wm_item.hiwater_func_ptr     = NULL;
  
  //watermark_p->wm_item.non_empty_func_ptr   = NULL;

}/* ds_gprs_mgr_setup_sndcp_to_ps_wm */

/*===========================================================================
FUNCTION DS_GPRS_MGR_RX_DATA_CB()

DESCRIPTION
  This function is called when there is data in the  RX queue to signal PS
  that there is data for processing.

PARAMETERS
  logical channel ID
  num_sdus: Number of SDUs queued into WM for each TTI
  callback_data : Callback data registered 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_gprs_mgr_rx_data_cb
(
  byte id,
  byte num_sdus,
  void *callback_data
)
{
  // No op
  return;
} /* ds_gprs_mgr_rx_data_cb() */

/*===========================================================================
FUNCTION DS_GPRS_BEARER_MGR_PDP_IP_REQ_REG_CNF_HDLR

DESCRIPTION
  This function posts a command to set up the transmit function once 
  registration with SNDCP is complete.

PARAMETERS
  Context - PDP context instance
  cnf_status - Indicates whether registration was successful or not.
  Setup the transmit function only if registration was successful. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_gprs_bearer_mgr_pdp_ip_req_reg_cnf_hdlr
(
  ds_gprs_gsndcp_reg_cnf_ev_type    *reg_cnf_ev_ptr
)
{
  ds_bearer_mgr_s          *bearer_mgr_p = NULL;
  boolean                   cnf_status;
  ds_pdn_mgr_s             *pdn_mgr_p = NULL;
  ds_bearer_mgr_wm_s       *tx_watermark_p = NULL;
  ds_bearer_mgr_wm_s       *rx_watermark_p = NULL;
  comp_cmd_type            *sndcp_cmd = NULL;
  uint32                   rat_mask = DS_3GPP_THROT_RAT_MASK_GLOBAL;
  boolean                          is_attach_apn = FALSE;
  #ifdef FEATURE_DATA_CC_SP_THROTTLING
  uint32                 throt_tmr_val;
  #endif /* FEATURE_DATA_CC_SP_THROTTLING */

  if(!reg_cnf_ev_ptr)
  {
    DS_ERROR_FATAL("NULL pointer");
    return;
  }

  bearer_mgr_p = reg_cnf_ev_ptr->bearer_mgr_p;
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    return;
  }
  
  cnf_status = reg_cnf_ev_ptr->cnf_status;
  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;

  
  /* Not doing anything as part of PSM restore as registration 
       always fails on bringup*/
  if(0/*(ds3gpp_get_ps_data_suspend_flag(subs_id) == TRUE)*/ ||
     ((bearer_mgr_p->state != DS_BEARER_MGR_STATE_UP) &&
      (bearer_mgr_p->state != DS_BEARER_MGR_STATE_UP_DORMANT) &&
      (bearer_mgr_p->state != DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB)))
  {
    DS_ERROR_LOG_0(" Reg req called in wrong state");
    return;
  }

  if(cnf_status)
  {
    bearer_mgr_p->tx_ptr = ds_gprs_mgr_tx_um_data_cb;

    // Set RX signal if required
    
    /*-------------------------------------------------------------------------  
      Set the PS->SNDCP watermark.
    -------------------------------------------------------------------------*/    
    tx_watermark_p = &(bearer_mgr_p->tx_wm);
    ds_gprs_mgr_setup_ps_to_sndcp_wm(bearer_mgr_p);
    
    /*-------------------------------------------------------------------------  
      Set the SNDCP->PS watermark.
    -------------------------------------------------------------------------*/
    rx_watermark_p = &(bearer_mgr_p->rx_wm);
    ds_gprs_mgr_setup_sndcp_to_ps_wm(bearer_mgr_p);

    /*-------------------------------------------------------------------------
      Cleanup the Tx watermark just in case any data got enqueued after
      the previous call ended.
    -------------------------------------------------------------------------*/
    dsm_empty_queue(&tx_watermark_p->wm_item);

     /*-----------------------------------------------------------------------
      Register WM with SNDCP 
     -----------------------------------------------------------------------*/
    if((sndcp_cmd = comp_get_cmd_buf()) == NULL)
    {
      /*-----------------------------------------------------------------------
        No free command buffers available
      -----------------------------------------------------------------------*/
      return;
    }
  
    memset(sndcp_cmd, 0, sizeof(comp_cmd_type));  
    sndcp_cmd->hdr.cmd_id = GSNDCP_REG_WM;
    sndcp_cmd->cmd.gsndcp_reg_wm.nsapi  = bearer_mgr_p->bearer_id;
    sndcp_cmd->cmd.gsndcp_reg_wm.to_sndcp_wm_ptr = &tx_watermark_p->wm_item;
    sndcp_cmd->cmd.gsndcp_reg_wm.from_sndcp_wm_ptr = &rx_watermark_p->wm_item;
    sndcp_cmd->cmd.gsndcp_reg_wm.from_sndcp_post_rx_func_ptr = 
    	                                             ds_gprs_mgr_rx_data_cb;
    sndcp_cmd->cmd.gsndcp_reg_wm.from_sndcp_rx_func_parm = (void*)bearer_mgr_p;
    comp_put_cmd(sndcp_cmd);       



    if(ds_pdn_nv_manager_is_rel_10_or_above_supported())
    {
      /*---------------------------------------------------------------------
       PDP context activation got successfully processed by UE.
       Clear APN throttling state machine if it was globally blocked.
      ----------------------------------------------------------------------*/
  
      if (ds_pdn_nv_manager_get_enable_pdn_throt_per_rat()||
          ds_pdn_nv_manager_is_rel_12_or_above_supported())
      {
        rat_mask |= ds_3gpp_pdn_throt_sm_sys_mode_to_rat_mask
                     (pdn_mgr_p->nw_mode);
      }
  
      ds_3gpp_pdn_throt_handle_nw_inited_unblock_apn
        ((byte *)pdn_mgr_p->pdp_profile->context.apn, 
         rat_mask);
  
      if (ds_pdn_apm_is_apn_in_attach_pdn_list((char*)pdn_mgr_p->pdp_profile->context.apn))
      {
        is_attach_apn = TRUE;
      }
  
  #ifdef FEATURE_DATA_LTE
      if(is_attach_apn)
      {
        ds_3gpp_pdn_cntxt_enable_lte_if_attach_profile_available();
      }
  #endif /* FEATURE_DATA_LTE */
    }//ds_3gpp_pdn_get_nv_lte_rel_10
  
  #ifdef FEATURE_DATA_CC_SP_THROTTLING
    if(ds_3gpp_throt_sm_is_cc_sp_throttling_enabled() &&
       ds_3gpp_pdn_cntxt_get_blocked_plmn_list_size() > 0)
    {
      throt_tmr_val = ds_3gpp_cc_sp_throt_get_plmn_unblock_timer_val();
  
      if (throt_tmr_val == 0)
      {
        throt_tmr_val = DS_3GPP_CC_SP_THROT_2_HOURS_TIMER;
      }
  
      ds_3gpp_cc_sp_throt_set_plmn_unblock_timer(throt_tmr_val);
  
      ds_3gpp_pdn_throt_clear_throttle_state((byte *)pdn_mgr_p->pdp_profile->context.apn,
                                             DS_PROFILE_PDP_IPV4V6,
                                             DS_3GPP_THROT_RAT_MASK_UMTS,
                                             DS_3GPP_THROT_PROC_MASK_GLOBAL);
    }
  #endif /* FEATURE_DATA_CC_SP_THROTTLING */   

    ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, DS_BEARER_CONTROL_MASK);

    ds_pdn_mgr_bearer_up_ind(pdn_mgr_p, bearer_mgr_p);

    return;
  }


  if( ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p) &&
  	   pdn_mgr_p->is_restore_mode )
  {
    return;
  }
  
  /*-----------------------------------------------------------------------
    End the call since we cannot setup the link to SNDCP
  -----------------------------------------------------------------------*/    
  ds_bearer_mgr_tear_down_bearer(pdn_mgr_p, bearer_mgr_p);

}

/*===========================================================================
FUNCTION DS_GPRS_BEARER_MGR_RESUME_SNDCP_REG_REQ_CNF_HDLR

DESCRIPTION
  This function posts a command to set up the transmit function once 
  registration with SNDCP is complete.

PARAMETERS
  Context - PDP context instance
  cnf_status - Indicates whether registration was successful or not.
  Setup the transmit function only if registration was successful. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_gprs_bearer_mgr_resume_sndcp_reg_req_cnf_hdlr
(
  ds_gprs_gsndcp_reg_cnf_ev_type    *reg_cnf_ev_ptr
)
{
  ds_bearer_mgr_s          *bearer_mgr_p = NULL;
  boolean                   cnf_status;
  ds_pdn_mgr_s             *pdn_mgr_p = NULL;
  ds_bearer_mgr_wm_s       *tx_watermark_p = NULL;
  ds_bearer_mgr_wm_s       *rx_watermark_p = NULL;
  comp_cmd_type            *sndcp_cmd = NULL;

  if(!reg_cnf_ev_ptr)
  {
    DS_ERROR_FATAL("NULL pointer");
    return;
  }

  bearer_mgr_p = reg_cnf_ev_ptr->bearer_mgr_p;
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    return;
  }
  
  cnf_status = reg_cnf_ev_ptr->cnf_status;
  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;

  
  /* Not doing anything as part of PSM restore as registration 
       always fails on bringup*/
  if((ds_pdn_hdlr_get_ps_data_suspend_flag() == TRUE) ||
     ((bearer_mgr_p->state != DS_BEARER_MGR_STATE_UP) &&
      (bearer_mgr_p->state != DS_BEARER_MGR_STATE_UP_DORMANT) &&
      (bearer_mgr_p->state != DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB)))
  {
    DS_ERROR_LOG_0(" Reg req called in wrong state");
    return;
  }

  if(cnf_status)
  {
    bearer_mgr_p->tx_ptr = ds_gprs_mgr_tx_um_data_cb;
   
    /*-------------------------------------------------------------------------  
      Set the PS->SNDCP watermark.
    -------------------------------------------------------------------------*/    
    tx_watermark_p = &(bearer_mgr_p->tx_wm);
    ds_gprs_mgr_setup_ps_to_sndcp_wm(bearer_mgr_p);
    
    /*-------------------------------------------------------------------------  
      Set the SNDCP->PS watermark.
    -------------------------------------------------------------------------*/
    rx_watermark_p = &(bearer_mgr_p->rx_wm);
    ds_gprs_mgr_setup_sndcp_to_ps_wm(bearer_mgr_p);

   /*-----------------------------------------------------------------------
      Register WM with SNDCP 
     -----------------------------------------------------------------------*/
    if((sndcp_cmd = comp_get_cmd_buf()) == NULL)
    {
      /*-----------------------------------------------------------------------
        No free command buffers available
      -----------------------------------------------------------------------*/
      return;
    }
  
    memset(sndcp_cmd, 0, sizeof(comp_cmd_type));  
    sndcp_cmd->hdr.cmd_id = GSNDCP_REG_WM;
    sndcp_cmd->cmd.gsndcp_reg_wm.nsapi  = bearer_mgr_p->bearer_id;
    sndcp_cmd->cmd.gsndcp_reg_wm.to_sndcp_wm_ptr = &tx_watermark_p->wm_item;
    sndcp_cmd->cmd.gsndcp_reg_wm.from_sndcp_wm_ptr = &rx_watermark_p->wm_item;
    sndcp_cmd->cmd.gsndcp_reg_wm.from_sndcp_post_rx_func_ptr = 
    	                                             ds_gprs_mgr_rx_data_cb;
    sndcp_cmd->cmd.gsndcp_reg_wm.from_sndcp_rx_func_parm = (void*)bearer_mgr_p;
    comp_put_cmd(sndcp_cmd);       

    ds_bearer_mgr_enable_tx_flow(bearer_mgr_p, DS_BEARER_CONTROL_MASK);

    bearer_mgr_p->state = DS_BEARER_MGR_STATE_UP;
    ds_bearer_mgr_post_bearer_up_event(bearer_mgr_p);
  }
  else
  {  
    /*-----------------------------------------------------------------------
      End the call since we cannot setup the link to SNDCP
    -----------------------------------------------------------------------*/    
    ds_bearer_mgr_tear_down_bearer(pdn_mgr_p, bearer_mgr_p);
  }

  return;
}

/*===========================================================================
FUNCTION DS_GPRS_MGR_RESUME_SNDCP_REG_REQ_CNF

DESCRIPTION
  This function posts a command to set up the transmit function once 
  registration with SNDCP is complete.

PARAMETERS
  Context - PDP context instance
  cnf_status - Indicates whether registration was successful or not.
  Setup the transmit function only if registration was successful. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_gprs_mgr_resume_sndcp_reg_req_cnf
(
  void          *context,
  boolean        cnf_status
)
{
  ds_command_type                *cmd_ptr;          
  ds_bearer_mgr_s                *bearer_mgr_p = (ds_bearer_mgr_s*)context;
  ds_gprs_gsndcp_reg_cnf_ev_type *reg_cnf_ev_ptr = NULL;

  DS_MSG0(MSG_LEGACY_HIGH, "DS3GPP GSNDCP RESUME REG CNF MSG");

  /*------------------------------------------------------------------------
    Post command to DS task to process in the proper task context
   ------------------------------------------------------------------------*/

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_gprs_gsndcp_reg_cnf_ev_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DS_ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_GSNDCP_RESUME_REG_CNF;
  reg_cnf_ev_ptr = (ds_gprs_gsndcp_reg_cnf_ev_type*)cmd_ptr->cmd_payload_ptr;

  reg_cnf_ev_ptr->bearer_mgr_p = bearer_mgr_p;
  reg_cnf_ev_ptr->cnf_status = cnf_status;

  ds_put_cmd(cmd_ptr);
} /* ds_gprs_mgr_resume_sndcp_reg_req_cnf */

/*===========================================================================
FUNCTION DS_GPRS_BEARER_MGR_RESUME_SNDCP_REG_REQ

DESCRIPTION
  This function initiates resume for data path with SNDCP..

PARAMETERS
  bearer_ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_gprs_bearer_mgr_resume_sndcp_reg_req
(
  ds_bearer_mgr_s    *bearer_mgr_p
)
{
#ifdef FEATURE_GSM_GPRS
  gsndcp_pdp_register_req
  (
    bearer_mgr_p->bearer_id,                    /* nsapi                  */
    NULL,                                       /* pdp_ul_suspend_fnc_ptr */
    NULL,                                       /* pdp_ul_resume_fnc_ptr  */
    NULL,                                       /* pdp_dl_fnc_ptr         */
    (void *)bearer_mgr_p,                       /* bearer_context            */
    TRUE,                                       /* cipher                 */
    ds_gprs_mgr_resume_sndcp_reg_req_cnf
#ifdef FEATURE_DUAL_SIM
    ,SYS_MODEM_AS_ID_1
#endif /* FEATURE_DUAL_SIM */
  );
#endif /* FEATURE_GSM_GPRS */
}

/*===========================================================================
FUNCTION DS_GPRS_MGR_BEARER_ACTIVATE_HDLR

DESCRIPTION
  This function handles the bearer activation from NAS.
  
PARAMETERS  
  mm_pdp_activate_cnf_T - PDP ACT CNF pointer
  bearer_mgr_s - Bearer ptr
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
boolean ds_gprs_mgr_bearer_activate_hdlr
(
  mm_pdp_activate_cnf_T   *pdp_act_cnf,
  ds_bearer_mgr_s         *bearer_mgr_p  
)
{
  qos_T                    *netw_qos_p;
  ds_gprs_qos_params_type  *gprs_qos_ptr;
  ds_local_err_val_e_type   local_err = DS_LOCAL_ERR_VAL_NONE;

  bearer_mgr_p->bearer_id = pdp_act_cnf->nsapi.nsapi;
  
  bearer_mgr_p->state = DS_BEARER_MGR_STATE_UP;
  ds_bearer_mgr_post_bearer_up_event(bearer_mgr_p);
  
#ifdef FEATURE_DATA_IMS
  /*-------------------------------------------------------------------------
     * Decode PCO for IMS - IM_CN subsystem flag
    -------------------------------------------------------------------------*/
  if(!ds_gprs_mgr_decode_protocol_params(pdp_act_cnf,bearer_mgr_p))
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
#endif /* FEATURE_DATA_IMS */

  netw_qos_p = &(pdp_act_cnf->qos);
  gprs_qos_ptr = &(bearer_mgr_p->gprs_bearer->gprs_qos_val);

  if( netw_qos_p == NULL ||
  	   netw_qos_p->valid == FALSE )
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    goto func_exit;    
  }
  
  /*----------------------------------------------------------------------
    RX/TX FLOW SPEC: Convert Precedence Class, same for UL and DL
   ----------------------------------------------------------------------*/
  gprs_qos_ptr->precedence = netw_qos_p->precedence_class;
  
  /*----------------------------------------------------------------------
    RX/TX FLOW SPEC: Convert Delay Class, same for UL and DL
   ----------------------------------------------------------------------*/
  gprs_qos_ptr->delay = netw_qos_p->delay_class;
  
  /*----------------------------------------------------------------------
    RX/TX FLOW SPEC: Convert Reliability Class, same for UL and DL
   ----------------------------------------------------------------------*/
  gprs_qos_ptr->reliability = netw_qos_p->reliability_class;
  
  /*----------------------------------------------------------------------
    RX/TX FLOW SPEC: Convert peak thruput, same for UL and DL
   ----------------------------------------------------------------------*/
  gprs_qos_ptr->peak = netw_qos_p->peak_throughput;
  
  /*----------------------------------------------------------------------
    RX/TX FLOW SPEC: Convert mean thruput, same for UL and DL
   ----------------------------------------------------------------------*/
  gprs_qos_ptr->mean = netw_qos_p->mean_throughput;
  
  /*----------------------------------------------------------------------
    Set the valid flag
   ----------------------------------------------------------------------*/
  gprs_qos_ptr->valid_flag = TRUE;

  // Come back later and convert NW QoS to app QoS
#if 0
  if (gw_ps_call_info->qos.valid)
  {
    /*-----------------------------------------------------------------------
     Allocate Heap Memory
    -----------------------------------------------------------------------*/
    app_qos_p = (qos_spec_type*)modem_mem_alloc(
                                       sizeof(qos_spec_type),
                                       MODEM_MEM_CLIENT_DATA);
    if(app_qos_p != NULL)        
    {
      DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      /*-------------------------------------------------------------------------
        Get the app specified QOS parameters saved in bearer_flow_manager      
      -------------------------------------------------------------------------*/    
      bflow_manager_p=ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
      if(bflow_manager_p == NULL)
      {
        DS_3GPP_MSG0_ERROR("No Bearer Flow Manager associated with the Bearer");      
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        modem_mem_free(app_qos_p, MODEM_MEM_CLIENT_DATA);
        return DS3G_FAILURE;
      }
    
      ds_umts_bearer_flow_manager_convert_nw_data_rate_to_app( bflow_manager_p,
                                                             (void *)mode_info_p,
                                                                    app_qos_p,
                                                               &icause_code) ;
    
  
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      modem_mem_free(app_qos_p, MODEM_MEM_CLIENT_DATA);
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Could not allocate memory to convert qos");      
    }
  }
#endif

// no APN passed by NAS
#if 0
  if (pdp_act_cnf->apn_name.valid)
  {
    ds_apn_table_update_resolved_apn_name(bearer_mgr_p->pdn_mgr_ptr,
                                          pdp_act_cnf->apn_name.address);
  }
  else
  {    
    ds_apn_table_update_resolved_apn_name(bearer_mgr_p->pdn_mgr_ptr,
                   bearer_mgr_p->pdn_mgr_ptr->pdp_profile->context.apn);
  }
#endif 

  if(!bearer_mgr_p->pdn_mgr_ptr->is_restore_mode)
  {
#ifdef FEATURE_GSM_GPRS
    gsndcp_pdp_register_req
    (
      bearer_mgr_p->bearer_id,                    /* nsapi                  */
      NULL,                                       /* pdp_ul_suspend_fnc_ptr */
      NULL,                                       /* pdp_ul_resume_fnc_ptr  */
      NULL,                                       /* pdp_dl_fnc_ptr         */
      (void *)bearer_mgr_p,                       /* bearer_context            */
      TRUE,                                       /* cipher                 */
      ds_gprs_mgr_sndcp_reg_req_cnf
  #ifdef FEATURE_DUAL_SIM
      ,SYS_MODEM_AS_ID_1
  #endif /* FEATURE_DUAL_SIM */
    );
#endif /* FEATURE_GSM_GPRS */
  }
  else
  {
    // Suspend handler
  }

  return TRUE;

func_exit:

	 if(local_err != DS_LOCAL_ERR_VAL_NONE)
	 {
	   DS_ERROR_LOG_1("Bearer activation failed with error =d", local_err);
     if (ds_pdn_psm_get_ulog_handle() != NULL)
     {
        ULOG_RT_PRINTF_1(ds_pdn_psm_get_ulog_handle(),
             "Bearer activation failed with error %d", local_err);
     }

    bearer_mgr_p->state = DS_BEARER_MGR_STATE_GOING_DOWN;
	 }

  return FALSE;
}

/*===========================================================================
FUNCTION DS_GPRS_MGR_PDP_ACT_CNF_HDLR

DESCRIPTION
  This function handles the PDP activate confirmation from NAS.
  
PARAMETERS  
  mm_pdp_activate_cnf_T - PDP ACT CNF pointer
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
boolean ds_gprs_mgr_pdp_activate_cnf_hdlr
(
  mm_pdp_activate_cnf_T   *pdp_act_cnf
)
{
  ds_local_err_val_e_type   local_err = DS_LOCAL_ERR_VAL_NONE;
  ds_bearer_mgr_s          *bearer_mgr_p = NULL;
  ds_pdn_mgr_s             *pdn_mgr_p = NULL;
  protocol_cfg_options_T   *protocol_config_options_p;
  ds_pdn_protocol_cfg_options_s   ds_proto_config_options = {0};
  ds_pdn_flow_manager_s    *flow_manager_p = NULL;  
  ds_profile_addr_type      ds_pdn_addr = {0};

  
  if(NULL == pdp_act_cnf)  
  {
    DS_ERROR_FATAL("NULL pointer");
    return FALSE;
  }

  // PS data suspend check
  if(0)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  /*-------------------------------------------------------------------------
     Get the bearer cntx for the connection ID.
  -------------------------------------------------------------------------*/
  bearer_mgr_p = ds_bearer_mgr_get_bearer_from_sdf_id
                                           (pdp_act_cnf->connection_id);  
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {    
    local_err = DS_LOCAL_ERR_VAL_1;
    goto func_exit;    
  }

  if(ds_pdn_hdlr_get_current_network_mode() != SYS_SYS_MODE_GSM)
  {
    local_err = DS_LOCAL_ERR_VAL_2;
    goto func_exit; 
  }

  if(bearer_mgr_p->state == DS_BEARER_MGR_STATE_PEND_REDIAL)
  {
    local_err = DS_LOCAL_ERR_VAL_3;
    goto func_exit;    
  }

  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    local_err = DS_LOCAL_ERR_VAL_4;
    goto func_exit;    
  }

  
  /*-------------------------------------------------------------------------
     Verify that PDP type received in context accept is same as one
     requested in context request message.
  -------------------------------------------------------------------------*/
  if(ds_gprs_mgr_verify_pdp_type(pdp_act_cnf, pdn_mgr_p) == FALSE)
  {
    local_err = DS_LOCAL_ERR_VAL_5;
    goto func_exit;
  }

    /*----------------------------------------------------------------------
   Retrieve the protocol config options from CM struct mode_info_p
  ----------------------------------------------------------------------*/
  protocol_config_options_p = &(pdp_act_cnf->config_options);
  ds_proto_config_options.valid = ds_pdn_pco_epco_hdlr_convert_nas_pco_to_ds
    (protocol_config_options_p,&ds_proto_config_options);
  
  /*-------------------------------------------------------------------------
    Default BCM to be UE only, if PCO contains BCM info from NW, it will be
    updated
  -------------------------------------------------------------------------*/
  flow_manager_p = ds_pdn_flow_mgr_get_entry(pdn_mgr_p->index);

  if(!flow_manager_p)
  {
    DS_ERROR_LOG_0("Invalid flow_manager ptr");
  }
  else if(ds_pdn_nv_manager_get_bcm_info_validation_nv() == FALSE)
  {
    /** Primary set to MS only if BCM flag is set
        Default BCM NV() is set to TRUE. All BCM related actions are as per spec.
        if BCM NV is set to FALSE, UE will not update the BCM to MS only
        from secondary /modify accept if BCM PCO is not present
        It will inherit primary PDP BCM value. 
    **/
    if ( bearer_mgr_p->state == DS_BEARER_MGR_STATE_COMING_UP )
    {
      flow_manager_p->bcm_info = DS_3GPP_BCM_UE_ONLY;
    }
  }
  else
  {
    flow_manager_p->bcm_info = DS_3GPP_BCM_UE_ONLY;
  }

  if( ( pdp_act_cnf->pdp_addr.pdp_type_num != DS_3GPP_PDP_TYPE_IPV6_NUM) &&
      ( pdp_act_cnf->pdp_addr.pdp_type_num != DS_3GPP_PDP_TYPE_IPV4V6_NUM) )
  {
    ds_pdn_addr.ip_vsn = DS_PROFILE_IP_V4;
    memscpy( (void *)(&ds_pdn_addr.pdp_addr.pdp_addr_ipv4),
             	sizeof(uint32),
              pdp_act_cnf->pdp_addr.address,
              sizeof(uint32)); 
    /*-------------------------------------------------------------------------
      Validate & Store IP address provided by the NW
    -------------------------------------------------------------------------*/    
    if(ds_pdn_addr.pdp_addr.pdp_addr_ipv4 == 0)
    {
      /*---------------------------------------------------------------------
        No DHCP/static IP support, so reject call_connected
      ----------------------------------------------------------------------*/
      local_err = DS_LOCAL_ERR_VAL_6;
      goto func_exit;
    }   
  }
  else if( pdp_act_cnf->pdp_addr.pdp_type_num == DS_3GPP_PDP_TYPE_IPV6_NUM)   
  {
    /*---------------------------------------------------------------------
     IID is the last 64 bits of the IP address. Hence it is copied into 
     the lower 64 bits of the PDP addr field.
     The IPV6 IID is present in octets 0-7 of the field "address" inside 
     the structure pdn_address_T
    ---------------------------------------------------------------------*/
    ds_pdn_addr.ip_vsn = DS_PROFILE_IP_V6;
    memscpy((void *)(&(ds_pdn_addr.pdp_addr.pdp_addr_ipv6.\
                         in6_u.u6_addr64[DS_3GPP_IPV6_IID_OFFSET])),
            sizeof(ds_pdn_addr.pdp_addr.pdp_addr_ipv6.\
                         in6_u.u6_addr64[DS_3GPP_IPV6_IID_OFFSET]),              
            &(pdp_act_cnf->pdp_addr.address[DS_3GPP_GPRS_IPV6_IID_OFFSET]),
            sizeof(uint64));  
    /*-------------------------------------------------------------------------
      Validate & Store IP address provided by the NW
    -------------------------------------------------------------------------*/    
    if( ds_pdn_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[DS_3GPP_IPV6_IID_OFFSET] == 0)
    {
      /*---------------------------------------------------------------------
        No DHCP/static IP support, so reject call_connected
      ----------------------------------------------------------------------*/
      local_err = DS_LOCAL_ERR_VAL_7;
      goto func_exit;
    } 

  }
  else if( pdp_act_cnf->pdp_addr.pdp_type_num == DS_3GPP_PDP_TYPE_IPV4V6_NUM)
  {
    ds_pdn_addr.ip_vsn = DS_PROFILE_IP_V4V6;
    memscpy((void *)(&(ds_pdn_addr.pdp_addr.pdp_addr_ipv6.\
                   in6_u.u6_addr64[DS_3GPP_IPV6_IID_OFFSET])),
            sizeof(ds_pdn_addr.pdp_addr.pdp_addr_ipv6.\
                   in6_u.u6_addr64[DS_3GPP_IPV6_IID_OFFSET]),
            &(pdp_act_cnf->pdp_addr.\
                   address[DS_3GPP_GPRS_DUAL_IP_IPV6_IID_OFFSET]),
            sizeof(uint64));    

    memscpy( (void *)(&ds_pdn_addr.pdp_addr.pdp_addr_ipv4),
 	            sizeof(uint32),
              pdp_act_cnf->pdp_addr.address,              
              sizeof(uint32)); 
    /*-------------------------------------------------------------------------
      Validate & Store IP address provided by the NW
    -------------------------------------------------------------------------*/    
    if( ds_pdn_addr.pdp_addr.pdp_addr_ipv4 == 0 ||
    	   ds_pdn_addr.pdp_addr.pdp_addr_ipv6.in6_u.\
    	            u6_addr64[DS_3GPP_IPV6_IID_OFFSET] == 0)
    {
      /*---------------------------------------------------------------------
        No DHCP/static IP support, so reject call_connected
      ----------------------------------------------------------------------*/
      local_err = DS_LOCAL_ERR_VAL_8;
      goto func_exit;
    } 
  }

  if(FALSE == ds_pdn_mgr_setup_pdp_proto_stack(pdn_mgr_p,
                                               &ds_pdn_addr,
                                               &ds_proto_config_options))
  {
    local_err = DS_LOCAL_ERR_VAL_9;
    goto func_exit;
  }

  /*---------------------------------------------------------------------
    Copy the group ID set by NAS.
    Will be used later for making the secondary calls
  ---------------------------------------------------------------------*/
  pdn_mgr_p->pdp_group_id = pdp_act_cnf->group_id;

  if(ds_gprs_mgr_bearer_activate_hdlr(pdp_act_cnf, bearer_mgr_p) == FALSE)
  {    
    local_err = DS_LOCAL_ERR_VAL_10;
    goto func_exit;
  }
  return TRUE;


func_exit:

	 if(local_err != DS_LOCAL_ERR_VAL_NONE)
	 {
	   DS_ERROR_LOG_1("PDP activation failed with error =d", local_err);
    ds_bearer_mgr_tear_down_bearer(pdn_mgr_p, bearer_mgr_p);
	 }
  if (ds_pdn_psm_get_ulog_handle() != NULL)
  {
    ULOG_RT_PRINTF_1(ds_pdn_psm_get_ulog_handle(),
                     "PDP activation failed with error %d", local_err);
  }
  return FALSE;;
}

/*===========================================================================
FUNCTION DS_GPRS_MGR_TEAR_DOWN_PDN_CONNECTION

DESCRIPTION
  This function is used to tear down a PDN connection.  

PARAMETERS
  bearer_mgr_p  - pointer to the bearer context that is to be used
                         to end the call

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void ds_gprs_mgr_tear_down_pdn_connection
(
  ds_bearer_mgr_s        *bearer_mgr_p
)
{  
  mm_pdp_deactivate_req_T   msg_p = {{0}};

  DS_MSG1(MSG_LEGACY_HIGH, "Tear down Pdn Connection. Bearer State: %d",
                           bearer_mgr_p->state);
  
  /*-------------------------------------------------------------------------
    If PDN connection is active then tear down else do nothing.
  -------------------------------------------------------------------------*/
  switch(bearer_mgr_p->state)
  {
    case DS_BEARER_MGR_STATE_COMING_UP:  
    case DS_BEARER_MGR_STATE_PEND_RAB_REG:
    case DS_BEARER_MGR_STATE_UP:
    case DS_BEARER_MGR_STATE_UP_DORMANT:
    case DS_BEARER_MGR_STATE_UP_DORMANT_REESTAB:
    case DS_BEARER_MGR_STATE_PEND_REDIAL:
    {
     /*-------------------------------------------------------------------------
           Initiate Call End.
        -------------------------------------------------------------------------*/
      msg_p.message_header.message_id = MM_PDP_DEACTIVATE_REQ;
      msg_p.message_header.message_set = MS_MM_DS;      
      msg_p.message_header.message_len_lsb =
           (sizeof( mm_pdp_deactivate_req_T ) - sizeof(IMH_T)) % 256;
      msg_p.message_header.message_len_msb =
           (sizeof( mm_pdp_deactivate_req_T ) - sizeof(IMH_T)) / 256;

      msg_p.nsapi.valid = TRUE;
      msg_p.nsapi.nsapi = bearer_mgr_p->bearer_id;
      msg_p.connection_id = bearer_mgr_p->sdf_id;

      msg_p.cause = SM_REGULAR_DEACTIVATION;
      msg_p.tear_down_ind.valid = TRUE;
      if (bearer_mgr_p->is_default)
      {
        msg_p.tear_down_ind.ti_flag = TRUE;
      }
      else
      {
        msg_p.tear_down_ind.ti_flag = FALSE;
      }
      
      if(ds_pdn_mgr_is_wwan_tech_suspended())
      {
        msg_p.cause = SM_LOCAL_PDP_DEACTIVATION;
      } 
      ds_pdn_mgr_ds_to_nas_send_msg((void*)&msg_p);
      break;
    }
    break;

    default:
      break;
  }
  
  return;
} /* ds_gprs_mgr_tear_down_pdn_connection */

/*===========================================================================
FUNCTION DS_GPRS_MGR_MAP_SM_CAUSE_CODES_TO_DS

DESCRIPTION
  Converts NAS SM cause codes to DS net down reason type.  

PARAMETERS
  *ds_down_reason  - ptr to location holding converted cause-code for DS
  sm_cause           - ESM cause code to be converted

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/
void ds_gprs_mgr_map_sm_cause_codes_to_ds
(
  ds_net_down_reason_type  *ds_down_reason,
  sm_cm_status_T            sm_cause
)
{
	 switch(sm_cause)
	 {
	   case PDP_ACTIVATE_MAX_TIMEOUT:
	     *ds_down_reason = DS_NET_DOWN_REASON_PDP_ACTIVATE_MAX_RETRY_FAILED;
	     break;

	   case SM_ILLEGAL_MS:
	     *ds_down_reason = DS_NET_DOWN_REASON_ILLEGAL_MS;
	     break;

	   case SM_ILLEGAL_ME:
	     *ds_down_reason = DS_NET_DOWN_REASON_ILLEGAL_ME;
	     break;

	   case SM_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED:
	     *ds_down_reason = DS_NET_DOWN_REASON_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED;
	     break;

	   case SM_GPRS_SERVICES_NOT_ALLOWED:
	     *ds_down_reason = DS_NET_DOWN_REASON_GPRS_SERVICES_NOT_ALLOWED;
	     break;

	   case SM_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK:
	     *ds_down_reason = DS_NET_DOWN_REASON_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK;
	     break;

	   case SM_IMPLICITLY_DETACHED:
	     *ds_down_reason = DS_NET_DOWN_REASON_IMPLICITLY_DETACHED;
	     break;

	   case SM_PLMN_NOT_ALLOWED:
	     *ds_down_reason = DS_NET_DOWN_REASON_PLMN_NOT_ALLOWED;
	     break;

	   case SM_LA_NOT_ALLOWED:
	     *ds_down_reason = DS_NET_DOWN_REASON_LA_NOT_ALLOWED;
	     break;

	   case SM_GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN:
	     *ds_down_reason = DS_NET_DOWN_REASON_GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN;
	     break;

	   case NO_GPRS_CONTEXT:
	     *ds_down_reason = DS_NET_DOWN_REASON_NO_GPRS_CONTEXT;
	     break;

	   case PDP_DUPLICATE:
	     *ds_down_reason = DS_NET_DOWN_REASON_PDP_DUPLICATE;
	     break;

	   case UE_RAT_CHANGE:
	     *ds_down_reason = DS_NET_DOWN_REASON_UE_RAT_CHANGE;
	     break;

	   /* This will come only for PDP activation reject */
	   case SM_CONGESTION:
	     *ds_down_reason = DS_NET_DOWN_REASON_CONGESTION;
	     break;

	   /* Both PDP activation reject and deactivation */
	   case SM_NO_PDP_CONTEXT_ACTIVATED:
	     *ds_down_reason = DS_NET_DOWN_REASON_NO_PDP_CONTEXT_ACTIVATED;
	     break;

	   /* Both PDP activation reject and deactivation */
	   case SM_ACCESS_CLASS_DSAC_REJECTION:
	     *ds_down_reason = DS_NET_DOWN_REASON_ACCESS_CLASS_DSAC_REJECTION;
	     break;

	   /* PDP activation reject and deactivation */
	   case SM_MESSAGE_EXCEED_MAX_L2_LIMIT:
	     *ds_down_reason = DS_NET_DOWN_REASON_MESSAGE_EXCEED_MAX_L2_LIMIT;
	     break;
	   /*This will come only for NAS/Lowerlayer service request rejects*/
	   case SM_NAS_SRV_REQ_FAILURE:
	     *ds_down_reason = DS_NET_DOWN_REASON_SM_NAS_SRV_REQ_FAILURE;
	     break;
	     
	   /* No PDP exists with the given Connection Id while doing 
	      Modification/Deactivation OR Activation for already Active PDP */
	   case INVALID_CONNECTIN_ID:
	     *ds_down_reason = DS_NET_DOWN_REASON_INVALID_CONNECTION_ID;
	     break;

	   /* NSAPIs exceeds max while PDP activation. Invalid 
	      Modify/Deactivation Request by CM for ConId. While
	      activating Secondary when PDP already active with same
	      connection Id, Reject the ACTIVATE req */
	   case INVALID_NSAPI:
	     *ds_down_reason = DS_NET_DOWN_REASON_INVALID_NSAPI;
	     break;

	   /* Primary context for NSAPI Doesn�t exist, Reject the SEC
	      ACTIVATE req con_id */
	   case INVALID_PRI_NSAPI:
	     *ds_down_reason = DS_NET_DOWN_REASON_INVALID_PRI_NSAPI;
	     break;

	   /* Unable to Encode OTA message for MT PDP or Deactivate PDP */
	   case INVALID_FIELD:
	     *ds_down_reason = DS_NET_DOWN_REASON_INVALID_FIELD;
	     break;

	   /* When RAB is not established by lower layers during 
	      Activation/Modification/Deactivation */
	   case RAB_SETUP_FAILURE:
	     *ds_down_reason = DS_NET_DOWN_REASON_RAB_SETUP_FAILURE;
	     break;

	   /* On Expiry of PDP establish timer with MAX (5) retries */
	   case PDP_ESTABLISH_MAX_TIMEOUT:
	     *ds_down_reason = DS_NET_DOWN_REASON_PDP_ESTABLISH_MAX_TIMEOUT;
	     break;
	    
	   /* On Expiry of PDP Modify timer with MAX (4) retries */
	   case PDP_MODIFY_MAX_TIMEOUT:
	     *ds_down_reason = DS_NET_DOWN_REASON_PDP_MODIFY_MAX_TIMEOUT;
	     break;

	   /* on Expiry of PDP Deactivate timer with MAX (4) reties */
	   case PDP_INACTIVE_MAX_TIMEOUT:
	     *ds_down_reason = DS_NET_DOWN_REASON_PDP_INACTIVE_MAX_TIMEOUT;
	     break;

	   /* PDP activation failed because of RRC_ABORT or Forbidden PLMN */
	   case PDP_LOWERLAYER_ERROR:
	     *ds_down_reason = DS_NET_DOWN_REASON_PDP_LOWERLAYER_ERROR;
	     break;

	   /*  Local Deactivation 
	       SM_NATIONAL_ROAMING_NOT_ALLOWED: Roaming not allowed
	       SM_NO_SUITABLE_CELLS_IN_LA: No Suitable Cells In Location Area */
	   case PPD_UNKNOWN_REASON:
	     *ds_down_reason = DS_NET_DOWN_REASON_PPD_UNKNOWN_REASON;
	     break;
	    
	   /* MO PDP Modify collision when MT PDP is already in progress */
	   case PDP_MODIFY_COLLISION:
	     *ds_down_reason = DS_NET_DOWN_REASON_PDP_MODIFY_COLLISION;
	     break;
	    
	   /* PDP_MBMS_REQUEST received when PDP activation is already 
	      PDP_ACTIVE_PENDING on same Conn Id */
	   case PDP_MBMS_REQUEST_COLLISION:
	     *ds_down_reason = DS_NET_DOWN_REASON_PDP_MBMS_REQUEST_COLLISION;
	     break;

	   /* MBMS activation is already pending and PDP_MBMS_REQUEST is triggered */
	   case MBMS_DUPLICATE:
	     *ds_down_reason = DS_NET_DOWN_REASON_MBMS_DUPLICATE;
	     break;

	   /* Internal cause for call end due to PS DETACH  */
	   case SM_PS_DETACHED:
	     *ds_down_reason = DS_NET_DOWN_REASON_SM_PS_DETACHED;
	     break;

	   /* Internal cause to indicate that Radio resource is not available */
	   case SM_NO_RADIO_AVAILABLE:
	     *ds_down_reason = DS_NET_DOWN_REASON_SM_NO_RADIO_AVAILABLE;
	     break;

	   /* Internal cause to indicate abort because service is not available */
	   case SM_ABORT_SERVICE_NOT_AVAILABLE:
	     *ds_down_reason = DS_NET_DOWN_REASON_SM_ABORT_SERVICE_NOT_AVAILABLE;
	     break;

	   default:	     
      *ds_down_reason = DS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE;
      break;	     
	 }/* switch() internal cause */

  return;
}/* ds_gprs_mgr_map_sm_cause_codes_to_ds */

/*===========================================================================
FUNCTION DS_GPRS_MGR_PDP_DEACT_CNF_HDLR

DESCRIPTION
  This function handles the PDP deactivate confirmation from NAS.
  
PARAMETERS  
  mm_pdp_deactivate_cnf_T - PDP DEACT CNF pointer
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_gprs_mgr_pdp_deactivate_hdlr
(
  ds_bearer_deact_params_s   *info
)
{
  ds_local_err_val_e_type   local_err = DS_LOCAL_ERR_VAL_NONE;
  ds_bearer_mgr_s          *bearer_mgr_p = NULL;
  ds_pdn_mgr_s             *pdn_mgr_p = NULL;
  boolean                   rel12_plus = FALSE;
  byte                     *apn = NULL;
  boolean                   is_attach_apn = FALSE;
  ds_net_down_reason_type   down_reason = DS_NET_DOWN_REASON_NOT_SPECIFIED;
  reattempt_ind_T           reattempt_indicator = {0};
  uint32                    rat_mask = DS_3GPP_THROT_RAT_MASK_GLOBAL;
                                     /* RATs on which throttling is to be
                                                                        unblocked */

  if (ds_pdn_nv_manager_is_rel_12_or_above_supported())
  {
    rel12_plus = TRUE;
  }

  if (ds_pdn_nv_manager_get_enable_pdn_throt_per_rat() || rel12_plus)
  {
    rat_mask |= ds_3gpp_pdn_throt_sm_sys_mode_to_rat_mask(SYS_SYS_MODE_GSM);
  }

  bearer_mgr_p = ds_bearer_mgr_get_bearer_from_sdf_id(info->connection_id);
  if(!ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p))
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  pdn_mgr_p = bearer_mgr_p->pdn_mgr_ptr;
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  DS_MSG3(MSG_LEGACY_HIGH,
  	       "Bearer deactivation, PDN %d , bearer %d , state %d",
         pdn_mgr_p->index, bearer_mgr_p->index, bearer_mgr_p->state);

  if( bearer_mgr_p->state == DS_BEARER_MGR_STATE_DOWN ||
  	   bearer_mgr_p->state == DS_BEARER_MGR_STATE_PEND_REDIAL )
  {    
    local_err = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  if(ds_pdn_mgr_is_bearer_deactivated_by_network(pdn_mgr_p,
                                                 bearer_mgr_p))

  {
    (void)ds_3gpp_network_override_hdlr_set_overridden_ip
                      ((byte *)pdn_mgr_p->pdp_profile->context.apn,
                       DS_3GPP_PDN_THROT_PDP_UNDEFINED,
                       DS_3GPP_PDN_THROT_NTW_O_RIDE_CAUSE_MAX,
                       DS_3GPP_THROT_RAT_MASK_UMTS,
                       reattempt_indicator);
  }

  if(info->cause_type == SM_NETWORK_CAUSE)
  {
    down_reason = (ds_net_down_reason_type)TYPE_REASON_VAL(
                                             DS_LL_NET_DOWN_REASON_TYPE, 
                                             info->cause.ext_cause);
  }
  else if(info->cause_type == SM_INTERNAL_CAUSE)
  {
    ds_gprs_mgr_map_sm_cause_codes_to_ds(&down_reason,
    	                                    info->cause.int_cause);
  }
  DS_IOT_MSG4(MSG_LEGACY_HIGH,
                "Bearer deactivation, PDN %d , bearer %d , state %d down reason 0x%x",
            pdn_mgr_p->index, bearer_mgr_p->index, bearer_mgr_p->state, down_reason);

    ds_pdn_throt_rule_notify(pdn_mgr_p, info, SYS_SYS_MODE_GSM, down_reason);

    if(ds_pdn_nv_manager_is_rel_10_or_above_supported())
    {
      if(down_reason == DS_NET_DOWN_REASON_UMTS_REATTACH_REQ)
      {
        /*---------------------------------------------------------------------
         bearer context deactivation request with cause #39 �reactivation
         got successfull processed by UE. Clear APN throttling state machine 
         if it was globally blocked i.e. unthrottle the APN
        ----------------------------------------------------------------------*/
        apn = (byte *)pdn_mgr_p->pdp_profile->context.apn;
    
        if(*apn == '\0')
        {
          (void)ds_apn_table_get_resolved_apn_name(pdn_mgr_p, (char **)&apn);
        }

        ds_3gpp_pdn_throt_handle_nw_inited_unblock_apn(apn, rat_mask );

        if (ds_pdn_apm_is_apn_in_attach_pdn_list((char*)apn))
        {
          is_attach_apn = TRUE;
        }

        if(is_attach_apn)
        {
          ds_3gpp_pdn_cntxt_enable_lte_if_attach_profile_available();
        }
      }
    }//ds_3gpp_pdn_get_nv_lte_rel_10

  if(bearer_mgr_p->is_default)
  {
    bearer_mgr_p->state = DS_BEARER_MGR_STATE_PEND_REDIAL;
    ds_pdn_mgr_bearer_down_ind(pdn_mgr_p, bearer_mgr_p, down_reason);
  }
  else
  {
    bearer_mgr_p->state = DS_BEARER_MGR_STATE_DOWN;
    pdn_mgr_p->net_down_reason = down_reason;
    ds_gprs_mgr_go_null_hdlr(bearer_mgr_p, pdn_mgr_p);
  }
  

func_exit :
 	if(local_err != DS_LOCAL_ERR_VAL_NONE)
 	{
 	  DS_ERROR_LOG_1("PDP deactivation failed with error =d", local_err);
 	}

  return;
}

/*===========================================================================
FUNCTION DS_GPRS_MGR_DEREG_WM_WITH_SNDCP

DESCRIPTION
  This function posts WM dereg cmd to SNDCP.
  
PARAMETERS  
  bearer_mgr_p* - bearer pointer
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_gprs_mgr_dereg_wm_with_sndcp
(
  ds_bearer_mgr_s    *bearer_mgr_p
)
{
  comp_cmd_type    *sndcp_cmd = NULL;
  
  /*-------------------------------------------------------------------------
    Deregister UL and DL watermarks with SNDCP
  -------------------------------------------------------------------------*/
  if((sndcp_cmd = comp_get_cmd_buf()) == NULL)
  {
    /*-----------------------------------------------------------------------
      No free command buffers available
    -----------------------------------------------------------------------*/
    return;
  }

  memset(sndcp_cmd, 0, sizeof(comp_cmd_type));  
  sndcp_cmd->hdr.cmd_id = GSNDCP_DEREG_WM;
  sndcp_cmd->cmd.gsndcp_dereg_wm.nsapi  = bearer_mgr_p->bearer_id;
  comp_put_cmd(sndcp_cmd);
}

/*===========================================================================
FUNCTION DS_GPRS_MGR_GO_NULL_HDLR

DESCRIPTION
  This function handles the complete clean up of GPRS bearer
  
PARAMETERS  
  bearer_mgr_p* - bearer pointer
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean.
   
SIDE EFFECTS 
  None.  
===========================================================================*/
void ds_gprs_mgr_go_null_hdlr
(
  ds_bearer_mgr_s    *bearer_mgr_p,
  ds_pdn_mgr_s       *pdn_mgr_p 
)
{
  ds_gprs_mgr_dereg_wm_with_sndcp(bearer_mgr_p);
  
  // dereg with FWK
  ds_fwk_dereg_bearer_with_hw(bearer_mgr_p);

  ds_bearer_mgr_info_cleanup(bearer_mgr_p);
  
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

  /*--------------------------------------------------------------------------
    Move the bearer to down state.
  --------------------------------------------------------------------------*/
  bearer_mgr_p->state = DS_BEARER_MGR_STATE_DOWN;

  ds_pdn_mgr_bearer_down_ind(pdn_mgr_p, bearer_mgr_p,
  	                          pdn_mgr_p->net_down_reason);
  
}/* ds_gprs_mgr_go_null_hdlr */


/*===========================================================================
FUNCTION DS_GPRS_BEARER_CNTXT_ACTIVATE_ON_FULL_SRV

DESCRIPTION
  This function registers with GSNDCP on FULL srv while coming out of PSM
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_gprs_bearer_cntxt_activate_on_full_srv()
{
  int                    bearer_cntx_inst;
  ds_bearer_mgr_s        *bearer_mgr_p = NULL;
  ds_pdn_mgr_s           *pdn_cntx_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
  for(bearer_cntx_inst = 0;\
      bearer_cntx_inst < DS_3GPP_MAX_BEARER_CONTEXT;\
      bearer_cntx_inst++)
  {
    bearer_mgr_p = ds_bearer_mgr_get_instance_ptr(bearer_cntx_inst);
    
    if( ds_bearer_mgr_validate_bearer_ptr(bearer_mgr_p) )
    {
      pdn_cntx_p = bearer_mgr_p->pdn_mgr_ptr;
      if (ds_pdn_mgr_validate_pdn_ptr(pdn_cntx_p)
           && (pdn_cntx_p->is_restore_mode))
      {
#ifdef FEATURE_GSM_GPRS
        gsndcp_pdp_register_req
        (
          bearer_mgr_p->bearer_id,                    /* nsapi                  */
          NULL,                                       /* pdp_ul_suspend_fnc_ptr */
          NULL,                                       /* pdp_ul_resume_fnc_ptr  */
          NULL,                                       /* pdp_dl_fnc_ptr         */
          (void *)bearer_mgr_p,                       /* bearer_context            */
          TRUE,                                       /* cipher                 */
          ds_gprs_mgr_sndcp_reg_req_cnf
#ifdef FEATURE_DUAL_SIM
          ,SYS_MODEM_AS_ID_1
#endif /* FEATURE_DUAL_SIM */
         );
#endif /* FEATURE_GSM_GPRS */
      }
    }
  }
}/* ds_bearer_cntxt_activate_on_full_srv */

