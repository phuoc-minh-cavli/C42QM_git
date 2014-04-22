/*===========================================================================
                       D S _ 3 G P P _ R M S M _ I P _ C O M M O N. C

DESCRIPTION
  The Data Services UMTS RM State Machine for pdp-ip calls source file. 
  This state machine manages the Um and Rm interfaces for pdp-ip type 
  pkt data calls initiated from laptop or conneted terminals .
  
  
EXTERNALIZED FUNCTIONS

Copyright (c) 2002-2019 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/rmsm/src/ds_3gpp_rmsm_ip_common.c#9 $ 
  $DateTime: 2022/05/13 03:58:57 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
02/18/19     ds    Code changes to remove PPP_UM_SN_DEV references from RMSM
                   for DUN call as there is no UM PPP device now.
/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ds_headers.h"

#include "dsati.h"
#include "ds3gsiolib.h"
#include "ds_flow.h"
#include "dstaski.h"
#include "ps_in.h"
#include "ps_ppp.h"
#include "ps_ppp_snoop.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_ipcp.h"
#include "ps_ppp_ipv6cp.h"
#include "ps_meta_info.h"
#include "ps_byte.h"
#include "ds_sys.h"
#include "ds_sys_event.h"

#ifdef FEATURE_GSM_DTM
#include "rr_ds_if.h"
#endif /* FEATURE_GSM_DTM */

#include "ds_rmsmi.h"
#include "ds_rmsm_ip.h"
#include "ds_3gpp_rmsm_ip_common.h"
#include "ds_pdn_nv_manager.h"
#include "ds_rmsmi.h"
#include "ps_addr_v6.h"
#include "ds_3gpp_rmsm_autoconfig_sm.h"
#include "ds3gsiolib_ex.h"
#include "ds_pdn_manager.h"
#include "ds_pdn_apn_table.h"
#include "ps_addr_v6.h"
#include "ds_fwki.h"


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL DATA STRUCTURE DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

boolean ds_3gpp_rmsm_ip_common_ipcp_creq_was_received 
(
  ds_3gpp_rmsm_ip_common_info_type* info_ptr
)
{
  boolean rval = FALSE;

  DS_RMSM_ENTER_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec)); 

  if (info_ptr->v4_info.ipcp_creq_recd == TRUE)
    rval = TRUE;

  DS_RMSM_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec)); 

  return rval;
}


boolean ds_3gpp_rmsm_ip_common_ipv6cp_creq_was_received 
(
  ds_3gpp_rmsm_ip_common_info_type* info_ptr
)
{
  boolean rval = FALSE;

  DS_RMSM_ENTER_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec)); 

  if (info_ptr->v6_info.ipv6cp_creq_recd == TRUE)
    rval = TRUE;

  DS_RMSM_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec)); 

  return rval;
}


static void ds_3gpp_rmsm_ip_common_set_ipcp_creq_recd 
(
  ds_3gpp_rmsm_ip_common_info_type* info_ptr
) 
{
  DS_RMSM_ENTER_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec)); 

  info_ptr->v4_info.ipcp_creq_recd = TRUE;

  DS_RMSM_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec)); 
}

static void ds_3gpp_rmsm_ip_common_reset_ipcp_creq_recd 
(
  ds_3gpp_rmsm_ip_common_info_type* info_ptr
) 
{
  DS_RMSM_ENTER_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec)); 

  info_ptr->v4_info.ipcp_creq_recd = FALSE;

  DS_RMSM_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec)); 
}

static void ds_3gpp_rmsm_ip_common_set_ipv6cp_creq_recd 
(
  ds_3gpp_rmsm_ip_common_info_type* info_ptr
) 
{
  DS_RMSM_ENTER_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec)); 

  info_ptr->v6_info.ipv6cp_creq_recd = TRUE;

  DS_RMSM_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec)); 
}

static void ds_3gpp_rmsm_ip_common_reset_ipv6cp_creq_recd 
(
  ds_3gpp_rmsm_ip_common_info_type* info_ptr
) 
{
  DS_RMSM_ENTER_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec)); 

  info_ptr->v6_info.ipv6cp_creq_recd = FALSE;

  DS_RMSM_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec)); 
}


/*---------------------------------------------------------------------------
  IPCP response Timer  value: If we need to generate IPCP responses,
  this timeout value specifies how much delay should be introduced to cut
  down on the IPCP packets exchanged between TE & Mobile.
  Time is in Milli seconds : A value of 0 will result in the response being
  generated immediately (timer fires off immly)
  one second delay
---------------------------------------------------------------------------*/
#define DS_3GPP_RMSM_IP_COMMON_IPCP_RES_DELAY_TIME  1000   

#define DS_3GPP_RMSM_IP_COMMON_IPV6CP_RES_DELAY_TIME  1000 

/*---------------------------------------------------------------------------
  DNS addresses advertised in IPCP NAKs generated by the SM if 
  send_ipcp_naks_flg flag is set in the state machine. 
---------------------------------------------------------------------------*/
static uint32 bogus_ip_pdns=0x0a0b0c0d;
static uint32 bogus_ip_sdns=0x0a0b0c0e;

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DECLARATIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_TRANSITION_STATE

 DESCRIPTION
   
   This function is used to maintain the state of RmSm IP Common.

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
void ds_3gpp_rmsm_ip_common_transition_state
(
  ds_rmsm_rm_dev_instance_type    instance,
  ds_3gpp_rmsm_ip_common_state_type    new_state
);

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPCP_RSP_TIMER_CB

 DESCRIPTION
   
   This function will be called when the IPCP_RSP_TIMER expires

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/

void ds_3gpp_rmsm_ip_common_ipcp_rsp_timer_cb
( 
  void*  callback_data
);

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPCP_RSP_TIMER_CB

 DESCRIPTION
   
   This function will be called when the IPV6CP_RSP_TIMER expires

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/

void ds_3gpp_rmsm_ip_common_ipv6cp_rsp_timer_cb
( 
  void*  callback_data
);

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPCP_RSP_TIMER_CMD_HDLR

 DESCRIPTION
   
   This function will generate a IPCP NAK with internal DNS addresses ONLY.
   This function is called in DS context when the rex timer 
   ipcp_res_del_timer fires off

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
void ds_3gpp_rmsm_ip_common_ipcp_rsp_timer_cmd_hdlr
(
  ds_rmsm_instance_type* inst_ptr
);

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPV6CP_RSP_TIMER_CMD_HDLR

 DESCRIPTION
   
   This function will generate a IPV6CP NAK with the Interface identifier
   (IID) to be used by the laptop.
 
   This function is called in DS context when the rex timer 
   ipv6cp_res_del_timer fires off

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
void ds_3gpp_rmsm_ip_common_ipv6cp_rsp_timer_cmd_hdlr
(
  ds_rmsm_instance_type* inst_ptr
);

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPCP_HTONCNF

 DESCRIPTION
   
   This function will add the IPCP header fields to the IPCP packet in 
   network order
   
 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
dsm_item_type  *ds_3gpp_rmsm_ip_common_ipcp_htoncnf 
(
  struct config_hdr *cnf,
  dsm_item_type     *bp
);

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_GET_AUTH_INFO_FROM_RM_PPP

 DESCRIPTION
   
   This function will retrieve the Authentication information from PPP
   device running on the Rm device 

 DEPENDENCIES
   If Auth information is sucessfully retrieved, it updates the Auth info
   in the RM control block to point to the valid auth info structure, else
   it is set to NULL

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
void ds_3gpp_rmsm_ip_common_get_auth_info_from_rm_ppp
(
  ds_rmsmi_info_type       *rmsmi_info_ptr
);

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_PARSE_IPCP_CFG_REQ

 DESCRIPTION
   
   This function will go through all of the options in a IPCP C-Req and store
   them Internally. 

 DEPENDENCIES
     This function expects to be called with a dsm_item whose data pointer is
  pointing at the code byte for the ppp packet.


   Assumes that it will ONLY be called forIPCP. If an error occurs in 
   pulling data from the packet, the packet is left as-is
   
 RETURN VALUE
   RET_ERROR (-1) if there is an error (i.e. parsing of the packet breaks).
   RET_OK    (0)  on sucessful completion of recording all options in packet.

 SIDE EFFECTS
   
===========================================================================*/
sint15 ds_3gpp_rmsm_ip_common_parse_ipcp_cfg_req
(
  ds_rmsmi_info_type       *rmsmi_info_ptr
);

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_PARSE_IPV6CP_CFG_REQ

 DESCRIPTION
   
   This function goes through the header of the IPv6CP packet. It will retrieve
   the IPv6CP Config Request ID from the header. This ID will be used during
   generation of a NAK / ACK.

 DEPENDENCIES
     This function expects to be called with a dsm_item whose data pointer is
  pointing at the code byte for the ppp packet.


   Assumes that it will ONLY be called forIPv6CP. If an error occurs in 
   pulling data from the packet, the packet is left as-is
   
 RETURN VALUE
   RET_ERROR (-1) if there is an error (i.e. parsing of the packet breaks).
   RET_OK    (0)  on sucessful completion of recording all options in packet.

 SIDE EFFECTS
   
===========================================================================*/
sint15 ds_3gpp_rmsm_ip_common_parse_ipv6cp_cfg_req
(
  ds_rmsmi_info_type       *rmsmi_info_ptr
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_IP_COMMON_RM_IPCP_CFG_REQ_CB

DESCRIPTION
  This callback function is called when a IPCP config request packet is 
  received on the Rm interface

DEPENDENCIES
  none.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type ds_3gpp_rmsm_ip_common_rm_ipcp_cfg_req_cb
(
  ppp_dev_enum_type      device,
  uint16                 protocol,
  dsm_item_type        **item_head_ptr,
  ps_rx_meta_info_type **meta_info_ref_ptr
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_IP_COMMON_RM_IPV6CP_CFG_REQ_CB

DESCRIPTION
  This callback function is called when a IPv6CP config request packet is 
  received on the Rm interface

DEPENDENCIES
  none.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type ds_3gpp_rmsm_ip_common_rm_ipv6cp_cfg_req_cb
(
  ppp_dev_enum_type      device,
  uint16                 protocol,
  dsm_item_type        **item_head_ptr,
  ps_rx_meta_info_type **meta_info_ref_ptr
);

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_IP_COMMON_CONFIGURE_PPP_CB

DESCRIPTION
  This callback function is called when RmSm IP(v4 or v6 instance) wants to
  configure PPP with IP Address / IID information.
 
  This function will inject the previously received IPCP / IPv6CP packet,
  which would generate a NAK with the newly configured IP Address / IID
  information.

DEPENDENCIES
  none.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_rmsm_ip_common_configure_ppp_cb 
(
  void *this_ptr,
  ppp_protocol_e_type protocol
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_RM_BRING_UP_PDN_EV
  
  DESCRIPTION
  This function processes RM_BRING_UP_PDN_EV event. 
  
  PARAMETERS
  instance ptr
  
  RETURN VALUE
  None
  
  DEPENDENCIES
  none
  
  SIDE EFFECTS
  None
 ===========================================================================*/
void ds_3gpp_rmsm_ip_common_process_rm_bring_up_pdn_ev
(
  ds_rmsm_instance_type* inst_ptr
);
/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_RM_WANTS_PKT_CALL_EV

  DESCRIPTION
    This function processes RM_WANTS_PKT_CALL_EV event. 
    
  PARAMETERS
    Instance pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/

void ds_3gpp_rmsm_ip_common_process_rm_wants_pkt_call_ev
(
  ds_rmsm_instance_type* inst_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_RM_PPP_CONFIGURED_EV

  DESCRIPTION
    This function processes RM_WANTS_PKT_CALL_EV event. 
    
  PARAMETERS
    Instance pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_ip_common_process_rm_ppp_configured_ev
(
  ds_rmsm_instance_type* inst_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_RM_PPP_DOWN_EV

  DESCRIPTION
    This function processes RM_PPP_DOWN_EV event. 
    
  PARAMETERS
    Instance pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_ip_common_process_rm_ppp_down_ev
(
  ds_rmsm_instance_type* inst_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_RM_PPP_UP_EV

  DESCRIPTION
    This function processes RM_PPP_UP_EV event. 
    
  PARAMETERS
    Instance pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_ip_common_process_rm_ppp_up_ev
(
  ds_rmsm_instance_type* inst_ptr
);

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_IS_CALL_TEARABLE

  DESCRIPTION
    This function checks if the call is tearable.
    
  PARAMETERS
    RMSM info pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/

boolean ds_3gpp_rmsm_ip_common_is_call_tearable
(
  ds_rmsmi_info_type* rmsmi_info_ptr
);


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/



/*===========================================================================
FUNCTION   DS_3GPP_RMSM_IP_COMMON_CONFIGURE_PPP_CB

DESCRIPTION
  This callback function is called when RmSm IP(v4 or v6 instance) wants to
  configure PPP with IP Address / IID information.
 
  This function will inject the previously received IPCP / IPv6CP packet,
  which would generate a NAK with the newly configured IP Address / IID
  information.

DEPENDENCIES
  none.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_rmsm_ip_common_configure_ppp_cb 
(
  void                *this_ptr,
  ppp_protocol_e_type protocol
)
{
  ds_rmsmi_sm_info_type            *sm_info_ptr = NULL;
  ds_rmsmi_info_type               *rmsmi_info_ptr = NULL;
  rex_crit_sect_type               *crit_sec_ptr = NULL;
  uint16                           proto = 0;
  dsm_item_type                    **ppp_pkt_ptr = NULL;
  uint32                           reg_tag = 0;
  uint64                           te_iid = 0;
  uint64                           rm_iid = 0;
  ds_rmsm_instance_type            inst;
  ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;
  ppp_dev_enum_type                ppp_dev;
  ip_addr_type                     dns_addr[2]={{0}};
  ps_ip_addr_type                  if_addr= {0};
  ip_addr_type                     gateway_ip_addr={0}; 
  sint15                           ds_errno;
  ds_local_err_val_e_type          err_val = DS_LOCAL_ERR_VAL_NONE;
  boolean                          ret_val = FALSE;
  /*-----------------------------------------------------------------*/

  DS_MSG1(MSG_LEGACY_HIGH,"Configure PPP callback invoked, this_ptr: %x", 
                    this_ptr);

  rmsmi_info_ptr = (ds_rmsmi_info_type*)this_ptr;
  if (rmsmi_info_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  if(info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  ppp_dev = ds_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);

  if (ds_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
  {
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  inst.rm_dev_instance = rmsmi_info_ptr->instance;

  if (protocol == PPP_PROTOCOL_IPCP)
  {
    inst.sm_instance = DS_RMSM_V4_SM_INSTANCE;
    
    sm_info_ptr = ds_rmsmi_get_sm_info_ptr(&inst);
    if (sm_info_ptr == NULL || sm_info_ptr->fw_inst == NULL) 
    {
      err_val = DS_LOCAL_ERR_VAL_3;
      goto func_exit;
    }

   ds_fwk_get_ip_addr(sm_info_ptr->fw_inst,IPV4_ADDR,&if_addr);
   sm_info_ptr->pdp_specific_info.pdp_ip.v4_sm_info.ip_addr.ps_s_addr = 
                                                 if_addr.addr.v4.ps_s_addr;
	
	 ds_pdn_ioctl(DS_FWK_CLIENT_RMSM,sm_info_ptr->instance_hdl,
                  DS_PDN_IOCTL_GET_GATEWAY_ADDR,
                  &gateway_ip_addr,
                  &ds_errno);
   sm_info_ptr->pdp_specific_info.pdp_ip.v4_sm_info.gateway.ps_s_addr = 
                                          gateway_ip_addr.addr.v4;
   
     ds_fwk_get_v4_dns_addrs(sm_info_ptr->fw_inst, &dns_addr[0], &dns_addr[1]);

	/*Get the primary and secondary DNS addresses alone*/
    
   sm_info_ptr->pdp_specific_info.pdp_ip.v4_sm_info.primary_dns.ps_s_addr = 
	  	                   dns_addr[0].addr.v4;
	 sm_info_ptr->pdp_specific_info.pdp_ip.v4_sm_info.secondary_dns.ps_s_addr = 
	                       dns_addr[1].addr.v4;
	
    /*---------------------------------------------------------------------
       Assign the IP address,the DNS addresses and the NBNS addresses 
       obtained by the Um from the network to the peer on the Rm interface. 
       If there is a gateway address obtained by Um from the network, 
       advertixe that as local IP 
    ---------------------------------------------------------------------*/

	  ppp_set_v4_server_opts(rmsmi_info_ptr->rm_ppp_config_ptr, 
      sm_info_ptr->pdp_specific_info.pdp_ip.v4_sm_info.gateway.ps_s_addr,
      sm_info_ptr->pdp_specific_info.pdp_ip.v4_sm_info.ip_addr.ps_s_addr,
      sm_info_ptr->pdp_specific_info.pdp_ip.v4_sm_info.primary_dns.ps_s_addr,
      sm_info_ptr->pdp_specific_info.pdp_ip.v4_sm_info.secondary_dns.ps_s_addr,
      0,0);
      
    /*---------------------------------------------------------------------
      Disable VJ, Set the new IPCP options in PPP
    ---------------------------------------------------------------------*/
    rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.will_mask[FSM_LOCAL] 
      &= ~IPCP_N_COMPRESS;
    rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.will_mask[FSM_REMOTE] 
      &=~IPCP_N_COMPRESS;
    rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.want_mask[FSM_LOCAL] 
      &= ~IPCP_N_COMPRESS;
    rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.want_mask[FSM_REMOTE] 
      &=~IPCP_N_COMPRESS;

    DS_MSG1(MSG_LEGACY_HIGH,"IP addr::%lx", ps_ntohl(sm_info_ptr->pdp_specific_info.pdp_ip.v4_sm_info.ip_addr.ps_s_addr)); 

    DS_MSG2(MSG_LEGACY_HIGH,"P DNS:%lx S DNS:%lx",
      ps_ntohl(sm_info_ptr->pdp_specific_info.pdp_ip.v4_sm_info.primary_dns.ps_s_addr),
      ps_ntohl(sm_info_ptr->pdp_specific_info.pdp_ip.v4_sm_info.secondary_dns.ps_s_addr));


    crit_sec_ptr = &(info_ptr->v4_info.ipi_sm_info_crit_sec);

    proto = PPP_IPCP_PROTOCOL;
    DS_RMSM_ENTER_CRIT_SECTION( crit_sec_ptr ); 
    ppp_pkt_ptr = &info_ptr->v4_info.ipcp_cfg_pkt_ptr;
    reg_tag = info_ptr->v4_info.ipcp_snoop_tag;
    DS_RMSM_LEAVE_CRIT_SECTION( crit_sec_ptr );  

  }

  else if (protocol == PPP_PROTOCOL_IPV6CP)
  {
    inst.sm_instance = DS_RMSM_V6_SM_INSTANCE; 
    sm_info_ptr = ds_rmsmi_get_sm_info_ptr(&inst);
    if (sm_info_ptr == NULL) 
    {
      err_val = DS_LOCAL_ERR_VAL_4;
      goto func_exit;
    }

    rm_iid = sm_info_ptr->pdp_specific_info.pdp_ip.v6_sm_info.gateway_iid;
    te_iid = sm_info_ptr->pdp_specific_info.pdp_ip.v6_sm_info.te_iid;

    ppp_set_v6_server_opts(rmsmi_info_ptr->rm_ppp_config_ptr, 
                           rm_iid, te_iid);

    DS_MSG2(MSG_LEGACY_HIGH,"Local IID::%lx, Global IID::%lx", 
                      ps_ntohll(rm_iid),ps_ntohll(te_iid)); 

    crit_sec_ptr = &(info_ptr->v6_info.ipi_sm_info_crit_sec);

    proto = PPP_IPV6CP_PROTOCOL;
    DS_RMSM_ENTER_CRIT_SECTION( crit_sec_ptr ); 
    ppp_pkt_ptr = &info_ptr->v6_info.ipv6cp_cfg_pkt_ptr;
    reg_tag = info_ptr->v6_info.ipv6cp_snoop_tag;
    DS_RMSM_LEAVE_CRIT_SECTION( crit_sec_ptr );  
  } 

  ppp_update_proto_config(ppp_dev, protocol,
                          rmsmi_info_ptr->rm_ppp_config_ptr);


  DS_MSG3(MSG_LEGACY_HIGH,"Protocol being configured to passive: %d"
                     "IPCP recd: %d, Ipv6Cp recd: %d", 
                     protocol, IPCP_CREQ_RECD(info_ptr), 
                     IPV6CP_CREQ_RECD(info_ptr)); 

  if (protocol == PPP_PROTOCOL_IPCP || protocol == PPP_PROTOCOL_IPV6CP)
  {
    ppp_set_cp_mode(ppp_dev, protocol, PPP_CP_MODE_PASSIVE); 
  }
  else
  {
    err_val = DS_LOCAL_ERR_VAL_5;
    goto func_exit;
  }

  /*---------------------------------------------------------------------
    Complete the configuration by setting PPP to INTERNAL mode so that packets
    get routed up the stack, inject the PPP packet and move the RMSM
    state to RM PPP CONFIGURED state in the following conditions :
    1. If PDP type is v4 only or v6 only.
    2. If PDP type is v4v6, then only if both the CREQ were received.
  ---------------------------------------------------------------------*/
  if (((IPCP_CREQ_RECD(info_ptr) && protocol == PPP_PROTOCOL_IPCP) ||
      (IPV6CP_CREQ_RECD(info_ptr) && protocol == PPP_PROTOCOL_IPV6CP)) &&
      (ppp_get_mode (ppp_dev) != PPP_INTERNAL_MODE))
  {
    snoop_unreg_event(reg_tag);
      
    if ( (DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) != DS_PROFILE_PDP_IPV4V6) ||
         (IPCP_CREQ_RECD(info_ptr) && IPV6CP_CREQ_RECD(info_ptr))
       )
    {
      if(ds_pdnmgr_is_prof_clat_enabled(rmsmi_info_ptr->profile_id) && 
         (DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) == DS_PROFILE_PDP_IPV6) &&
         !(IPCP_CREQ_RECD(info_ptr) && IPV6CP_CREQ_RECD(info_ptr)))

      {
        DS_MSG0(MSG_LEGACY_HIGH,"CLAT:Defer sending PPP inject - config pkt not received.");
        ret_val = TRUE;
        goto func_exit;
      }
      ppp_set_mode( ppp_dev, PPP_INTERNAL_MODE );
      
      if (DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) == DS_PROFILE_PDP_IPV4V6)
      {
        if(TRUE == ps_timer_is_running(rmsmi_info_ptr->ppp_up_delay_timer))
        {
          DS_MSG0(MSG_LEGACY_HIGH,"PPP UP delay timer cleared.");
          ps_timer_cancel(rmsmi_info_ptr->ppp_up_delay_timer);
        }
      }

    /*---------------------------------------------------------------------
      Play the buffered IPCP packet 
    ---------------------------------------------------------------------*/
      DS_RMSM_ENTER_CRIT_SECTION( crit_sec_ptr ); 

      if( !ppp_inject_packet(ppp_dev, proto, *ppp_pkt_ptr) )
      {
        *ppp_pkt_ptr= NULL;
        DS_RMSM_LEAVE_CRIT_SECTION( crit_sec_ptr );     
        DS_MSG0(MSG_LEGACY_HIGH,"ppp pkt injected ");
        ds_3gpp_rmsm_ip_common_post_event( &inst , DS_CMD_3GPP_RMSM_RM_PPP_CONFIGURED );
        ret_val = TRUE;
      }
      else
      {
        DS_RMSM_LEAVE_CRIT_SECTION( crit_sec_ptr );
        DS_MSG0(MSG_LEGACY_HIGH,"ppp pkt error ");
        ret_val = FALSE;
      }
    }
    else
    {
      ret_val = TRUE;
    }
  }
  else
  {
    DS_MSG1(MSG_LEGACY_HIGH,"Not injecting PPP pkt. PPP mode: %d", 
                       ppp_get_mode (ppp_dev));

    ret_val = TRUE;
  }

func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ip_common_configure_ppp_cb"
                              "err_val:%d",err_val);
  return ret_val;
} /*ds_3gpp_rmsm_ip_common_configure_ppp_cb*/

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_IP_COMMON_RM_IPCP_CFG_REQ_CB

  DESCRIPTION
  This callback function is called when a IPCP config request packet is 
  received on the Rm interface

  DEPENDENCIES
  none.

  RETURN VALUE
  None

  SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type ds_3gpp_rmsm_ip_common_rm_ipcp_cfg_req_cb
(
  ppp_dev_enum_type      device,
  uint16                 protocol,
  dsm_item_type        **item_head_ptr,
  ps_rx_meta_info_type **meta_info_ref_ptr
)
{

  ds_rmsm_sm_instance_type    sm_instance = DS_RMSM_V4_SM_INSTANCE; 
  ds_rmsmi_info_type          *rmsmi_info_ptr = NULL;
  ds_rmsm_instance_type       inst;
  ds_3gpp_rmsm_ip_common_info_type* info_ptr = NULL;
#ifdef FEATURE_DATA_ATCOP
  dsat_num_item_type          tmp_val;
#endif /* FEATURE_DATA_ATCOP */
  ds_local_err_val_e_type     err_val = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG2(MSG_LEGACY_HIGH,"Received  ds_3gpp_rmsm_ip_common_rm_ipcp_cfg_req_cb"
                     " device: %d protocol: %d", device, protocol ); 

  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);
  ASSERT((*item_head_ptr)->size > 0 && (*item_head_ptr)->used > 0);
  ASSERT(device < PPP_MAX_DEV);
  ASSERT(protocol == PPP_IPCP_PROTOCOL );

  inst.rm_dev_instance = ds_rmsmi_get_rm_dev_from_ppp_dev(device);
  inst.sm_instance = sm_instance;

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst.rm_dev_instance);
  if (rmsmi_info_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  if (info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  
  /*-------------------------------------------------------------------------
    Store the packet in the internal SM info. Discard the earlier packet
    we might have. 
  -------------------------------------------------------------------------*/
  DS_RMSM_ENTER_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );

  if (DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) != DS_PROFILE_PDP_IPV4 &&
      DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) != DS_PROFILE_PDP_IPV4V6 &&
	  ds_pdnmgr_is_prof_clat_enabled(rmsmi_info_ptr->profile_id) == FALSE)
  {                                
    DS_RMSM_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );
    DS_MSG1(MSG_LEGACY_HIGH,"PDP Type %d is not V4, dicarding the IPCP packet",
                       DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr));
    dsm_free_packet(item_head_ptr);
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }
  else if(DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) == DS_PROFILE_PDP_IPV4V6)
  {
    if(IPV6CP_CREQ_RECD(info_ptr) && 
       (TRUE == ps_timer_is_running(rmsmi_info_ptr->config_wait_timer)))
    {
      DS_MSG0(MSG_LEGACY_HIGH,"Cancel the config wait timer since IPCP packet recvd");
      ps_timer_cancel(rmsmi_info_ptr->config_wait_timer);
    }
  }
  /*-------------------------------------------------------------------------
    If we have already injected the packet to PPP and moved to 
    DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE state,
    ignore the config packet
  -------------------------------------------------------------------------*/
  if(info_ptr->state == DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE) 
  {                                
    DS_RMSM_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );
    DS_MSG1(MSG_LEGACY_HIGH,"IPCP packet received in state %d, freeing it",
                       DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE);
    dsm_free_packet(item_head_ptr);
    err_val = DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  }

  if( info_ptr->v4_info.ipcp_cfg_pkt_ptr != NULL )
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Freeing previous IPCP cfg req pkt");
    dsm_free_packet(&info_ptr->v4_info.ipcp_cfg_pkt_ptr);
  }

  info_ptr->v4_info.ipcp_cfg_pkt_ptr =  *item_head_ptr;
  
  /*---------------------------------------------------------------------
    Get PPP auth parameters from PPP on RM
    Parse IPCP config req packet on the Rm & bring up Um interface
  ---------------------------------------------------------------------*/
  if( ds_3gpp_rmsm_ip_common_parse_ipcp_cfg_req(rmsmi_info_ptr) == -1)
  {
    DS_RMSM_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );
    DS_MSG0(MSG_LEGACY_ERROR,"Error Parsing IPCP Config Req Packet, freeing it");
    dsm_free_packet(item_head_ptr);

    ds_3gpp_rmsm_ip_common_post_event(&inst, DS_CMD_3GPP_RMSM_RM_PPP_DOWN);
    err_val = DS_LOCAL_ERR_VAL_4;
    goto func_exit;
  } 

  /*-----------------------------------------------------------------
    See if we should respond to the IPCP packets with NAK's while the
    UM interface is coming UP.
   -----------------------------------------------------------------*/
#ifdef FEATURE_DATA_ATCOP
  tmp_val = (dsat_num_item_type)dsatutil_get_val(DSAT_VENDOR_QCPDPLT_IDX,0,NUM_TYPE);
  info_ptr->v4_info.send_ipcp_naks_flg = (uint8)tmp_val;
#endif /*  FEATURE_DATA_ATCOP */
      
  ds_3gpp_rmsm_ip_common_get_auth_info_from_rm_ppp(rmsmi_info_ptr);

  /*---------------------------------------------------------------------
    If we are set to generate IPCP naks, kick start a timer to delay
    the IPCP nak generation
  ---------------------------------------------------------------------*/
  if(info_ptr->v4_info.send_ipcp_naks_flg )
  {
    /*-----------------------------------------------------------------
      When the timer fires off& if  UM is not up, generate the NAK packet
    -----------------------------------------------------------------*/
	  ps_timer_start(info_ptr->v4_info.ipcp_res_del_timer,
		           (int64)DS_3GPP_RMSM_IP_COMMON_IPCP_RES_DELAY_TIME);

    DS_MSG0(MSG_LEGACY_HIGH," Initiating IPCP NAK delay timeout");
  }

  DS_RMSM_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );

  ds_3gpp_rmsm_ip_common_set_ipcp_creq_recd(info_ptr);

  DS_MSG1(MSG_LEGACY_HIGH,"Rx IPCP CReq: posting UMTS_RMSM_RM_BRING_UP_UM_IFACE_EV, "
                    "RmSM IP Common State: %d ",
                     info_ptr->state);
  ds_3gpp_rmsm_ip_common_post_event( &inst , DS_CMD_3GPP_RMSM_RM_BRING_UP_PDN );

func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ip_common_rm_ipvcp_cfg_req_cb err_val : %d",
                              err_val);
  return SNOOP_CB_IGNORE_PKT;
}

/*===========================================================================
FUNCTION   DS_3GPP_RMSM_IP_COMMON_RM_IPV6CP_CFG_REQ_CB

DESCRIPTION
  This callback function is called when a IPCP config request packet is 
  received on the Rm interface

DEPENDENCIES
  none.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type ds_3gpp_rmsm_ip_common_rm_ipv6cp_cfg_req_cb
(
  ppp_dev_enum_type      device,
  uint16                 protocol,
  dsm_item_type        **item_head_ptr,
  ps_rx_meta_info_type **meta_info_ref_ptr
)
{
  ds_rmsm_sm_instance_type sm_instance = DS_RMSM_V6_SM_INSTANCE;   
  ds_rmsmi_info_type *rmsmi_info_ptr = NULL;
  ds_rmsm_instance_type inst;
  ds_3gpp_rmsm_ip_common_info_type* info_ptr = NULL;
#ifdef FEATURE_DATA_ATCOP
  dsat_num_item_type tmp_val;
#endif /* FEATURE_DATA_ATCOP */
  ds_local_err_val_e_type     err_val = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG2(MSG_LEGACY_HIGH,"Received  ds_rmsmi_rm_ipv6cp_cfg_req_cb, device: %d"
                    "protocol: %d", device, protocol); 

  ASSERT(device < PPP_MAX_DEV);
  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);
  ASSERT((*item_head_ptr)->size > 0 && (*item_head_ptr)->used > 0);
  ASSERT(protocol == PPP_IPV6CP_PROTOCOL );

  inst.rm_dev_instance = ds_rmsmi_get_rm_dev_from_ppp_dev(device);
  inst.sm_instance = sm_instance;


  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst.rm_dev_instance);
  if (rmsmi_info_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  if(info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  
  /*-------------------------------------------------------------------------
    Store the packet in the internal SM info. Discard the earlier packet
    we might have. 
  -------------------------------------------------------------------------*/
  DS_RMSM_ENTER_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );

  if (DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) != DS_PROFILE_PDP_IPV6 &&
      DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) != DS_PROFILE_PDP_IPV4V6 )
  {                                
    DS_RMSM_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );
    DS_MSG1(MSG_LEGACY_ERROR,"PDP Type %d is not V6, dicarding the IPV6CP packet",
                       DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr));
    dsm_free_packet(item_head_ptr);
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }
  else if(DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) == DS_PROFILE_PDP_IPV4V6)
  {
    if(IPCP_CREQ_RECD(info_ptr) && 
       (TRUE == ps_timer_is_running(rmsmi_info_ptr->config_wait_timer)))
    {
	  DS_MSG0(MSG_LEGACY_HIGH,"Cancel the config wait timer since IPv6CP packet recvd");
      ps_timer_cancel(rmsmi_info_ptr->config_wait_timer);
    }
  }

  /*-------------------------------------------------------------------------
    If we have already injected the packet to PPP and moved to 
    DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE state,
    ignore the config packet
  -------------------------------------------------------------------------*/
  if(rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state == 
       DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE)
  {                                
    DS_RMSM_BRANCH_LEAVE_CRIT_SECTION(&(info_ptr->v6_info.ipi_sm_info_crit_sec) );
    DS_MSG1(MSG_LEGACY_HIGH,"IPV6CP packet received in state %d, freeing it",
                        DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE);
    dsm_free_packet(item_head_ptr);
    
    err_val = DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  }

  if( info_ptr->v6_info.ipv6cp_cfg_pkt_ptr != NULL )
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Freeing previous IPV6CP cfg req pkt");
    dsm_free_packet(&info_ptr->v6_info.ipv6cp_cfg_pkt_ptr);
  }

  info_ptr->v6_info.ipv6cp_cfg_pkt_ptr =  *item_head_ptr;
  
  DS_MSG0(MSG_LEGACY_HIGH,"Rx IPv6CP CReq: posting UMTS_RMSM_RM_BRING_UP_UM_IFACE_EV  ");

  /*---------------------------------------------------------------------
    Get PPP auth parameters from PPP on RM
    Parse IPCP config req packet on the Rm & bring up Um interface
  ---------------------------------------------------------------------*/
  if( ds_3gpp_rmsm_ip_common_parse_ipv6cp_cfg_req(rmsmi_info_ptr) == -1)
  {
    DS_RMSM_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );
    DS_MSG0(MSG_LEGACY_ERROR,"Error Parsing IPV6CP Config Req Packet, freeing it");
    dsm_free_packet(item_head_ptr);
    ds_3gpp_rmsm_ip_common_post_event(&inst, DS_CMD_3GPP_RMSM_RM_PPP_DOWN);

    err_val = DS_LOCAL_ERR_VAL_4;
    goto func_exit;
  } 

  /*-----------------------------------------------------------------
    See if we should respond to the IPV6CP packets with NAK's while the
    UM interface is coming UP.
   -----------------------------------------------------------------*/
#ifdef FEATURE_DATA_ATCOP
  tmp_val = (dsat_num_item_type)dsatutil_get_val(DSAT_VENDOR_QCPDPLT_IDX,0,NUM_TYPE); 
  info_ptr->v6_info.send_ipv6cp_naks_flg = (uint8)tmp_val;
#endif /* FEATURE_DATA_ATCOP */
  ds_3gpp_rmsm_ip_common_get_auth_info_from_rm_ppp(rmsmi_info_ptr);

    /*---------------------------------------------------------------------
    If we are set to generate IPv6CP naks, kick start a timer to delay
    the IPCP nak generation
  ---------------------------------------------------------------------*/
  if(info_ptr->v6_info.send_ipv6cp_naks_flg )
  {
    /*-----------------------------------------------------------------
      When the timer fires off& if  UM is not up, generate the NAK packet
    -----------------------------------------------------------------*/
	  ps_timer_start(info_ptr->v6_info.ipv6cp_res_del_timer,
		           (int64)DS_3GPP_RMSM_IP_COMMON_IPV6CP_RES_DELAY_TIME);
	
    DS_MSG0(MSG_LEGACY_HIGH," Initiating IPV6CP NAK delay timeout");
  }

  DS_RMSM_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );


  ds_3gpp_rmsm_ip_common_set_ipv6cp_creq_recd(info_ptr);


  DS_MSG1(MSG_LEGACY_HIGH,"Rx IPV6CP CReq: posting UMTS_RMSM_RM_BRING_UP_UM_PDN_EV, "
                      "RmSM IP Common State: %d ",
                       info_ptr->state);
	
  ds_3gpp_rmsm_ip_common_post_event( &inst , DS_CMD_3GPP_RMSM_RM_BRING_UP_PDN );

func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ip_common_rm_ipv6cp_cfg_req_cb err_val : %d",
                                err_val);
  return SNOOP_CB_IGNORE_PKT;

}

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_PARSE_IPCP_CFG_REQ

 DESCRIPTION
   
   This function will go through all of the options in a IPCP C-Req and store
   them Internally. 

 DEPENDENCIES
     This function expects to be called with a dsm_item whose data pointer is
  pointing at the code byte for the ppp packet.


   Assumes that it will ONLY be called forIPCP. If an error occurs in 
   pulling data from the packet, the packet is left as-is
   
 RETURN VALUE
   RET_ERROR (-1) if there is an error (i.e. parsing of the packet breaks).
   RET_OK    (0)  on sucessful completion of recording all options in packet.

 SIDE EFFECTS
   
===========================================================================*/
sint15 ds_3gpp_rmsm_ip_common_parse_ipcp_cfg_req
(
  ds_rmsmi_info_type       *rmsmi_info_ptr
)
{
  /*lint -e655*/
   dsm_item_type* item_ptr = NULL;       /* Working ptr to PPP packet */
   struct option_hdr opt_hdr;            /* option header */
   struct config_hdr cfg_hdr;            /* Config hdr: has Code, ID, len */
   sint31 pkt_len = 0;                   /* Length of the entire packet  */
   uint16 offset = 0;                    /* offset from begining of current item */
   uint16 copy_len = CONFIG_HDR_LEN;     /* Number of bytes copied into buffer */
   byte opt_val[PPP_OPT_MAX_LEN];        /* array to copy option value into */
   uint64 pkt_size = 0;                  /* Total item ptr size */
   uint64 read_data_len = 0;             /* Running data len count */

   ds_3gpp_rmsm_ip_common_info_type* info_ptr = NULL;

   /*-----------------------------------------------------------------------*/

   memset(opt_val, 0, PPP_OPT_MAX_LEN);
   memset(&opt_hdr, 0, sizeof(opt_hdr));
   memset(&cfg_hdr, 0, sizeof(cfg_hdr));

   DS_MSG1(MSG_LEGACY_HIGH,"Parsing IPCP Config Request Packet, rmsmi_info_ptr: %x",
                   rmsmi_info_ptr);

   if (rmsmi_info_ptr == NULL) 
   {
     DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
     /*lint -e527 */
     return -1;
     /*lint +e527 */
   }
   
   info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
   ASSERT(info_ptr != NULL);

   DS_RMSM_ENTER_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );

   item_ptr = info_ptr->v4_info.ipcp_cfg_pkt_ptr;   
      
   ASSERT(item_ptr != NULL);
   
   /*-------------------------------------------------------------------------
     Reset the options seen in the packet
   -------------------------------------------------------------------------*/
   memset(&info_ptr->ppp_info.ipcp_info,0x0, sizeof(ds_pdn_mgr_ipcp_info_type));
   pkt_size = dsm_length_packet(item_ptr);
  /*-------------------------------------------------------------------------
     Extract the code, id and length fields (4 bytes) into cfg_hdr. Convert
     the length from network to host.
   -------------------------------------------------------------------------*/

   if(dsm_seek_extract(&item_ptr, &offset, &cfg_hdr, &copy_len) == FALSE)
   {
     DS_RMSM_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );
     DS_MSG0(MSG_LEGACY_ERROR,"seek_extract() failed - ignoring pkt");
     return -1;
   }

   info_ptr->v4_info.ipcp_creq_id = cfg_hdr.identifier;
   pkt_len =  ps_ntohs(cfg_hdr.length);
   read_data_len += copy_len;

   DS_MSG1(MSG_LEGACY_HIGH,"ipcp id:%d",cfg_hdr.identifier);

   pkt_len -= CONFIG_HDR_LEN;
   offset  += CONFIG_HDR_LEN;

   /*-------------------------------------------------------------------------
     Go through the entire IPCP packet, one option at a time.
   -------------------------------------------------------------------------*/
   while ( pkt_len > 0)
   {
     /*-----------------------------------------------------------------------
       Get the option header, and update the packet length to reflect that the
       header has been grabbed.
     -----------------------------------------------------------------------*/
     memset(&opt_hdr, 0, sizeof(opt_hdr));
     copy_len = OPTION_HDR_LEN;
     if(dsm_seek_extract(&item_ptr, &offset, &opt_hdr, &copy_len) == FALSE)
     {
       DS_RMSM_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );
       DS_MSG0(MSG_LEGACY_ERROR, "seek_extract() failed - ignoring pkt");
       return -1;
     }
     read_data_len += copy_len;
     pkt_len -= opt_hdr.length;

     /*-----------------------------------------------------------------------
       Get the options value IFF it has one.
     -----------------------------------------------------------------------*/
     offset += OPTION_HDR_LEN;
     copy_len =  opt_hdr.length - OPTION_HDR_LEN;
     if ( (copy_len > 0) && (copy_len <= PPP_OPT_MAX_LEN) && (read_data_len <= pkt_size) )
     {
       if(dsm_seek_extract(&item_ptr, &offset, opt_val, &copy_len) == FALSE)
       {
         DS_RMSM_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );
         DS_MSG0(MSG_LEGACY_ERROR, "seek_extract() failed - ignoring pkt");
         return -1;
        }
       offset += copy_len;
       read_data_len += copy_len;
     }
     else
     {
       DS_RMSM_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );
       DS_MSG0(MSG_LEGACY_ERROR,"Invalid opt_hdr received");
       return -1;
     }
     /*-----------------------------------------------------------------------
       Save the options field
     -----------------------------------------------------------------------*/
    switch(opt_hdr.type)
    {
      /*-----------------------------------------------------------------------
        VJ compression
      -----------------------------------------------------------------------*/
      case IPCP_COMPRESS:
        DS_MSG0(MSG_LEGACY_HIGH,"Ignoring VJ");
        break;
  
      /*-----------------------------------------------------------------------
        IP address
      -----------------------------------------------------------------------*/
      case IPCP_1ADDRESS:
        info_ptr->ppp_info.ipcp_info.ip_address = 
          ps_ntohl((*((uint32*)opt_val)));
        info_ptr->ppp_info.valid_fields |= (uint32) NET_CFG_PRM_IP_ADDR_MASK;
        DS_MSG1(MSG_LEGACY_HIGH,"IP address in opt:%lx", 
                           info_ptr->ppp_info.ipcp_info.ip_address);
        break;
  
      /*-----------------------------------------------------------------------
        primary dns server address
      -----------------------------------------------------------------------*/
      case IPCP_PRIMARY_DNS:
        info_ptr->ppp_info.ipcp_info.primary_dns = 
          ps_ntohl((*((uint32*)opt_val)));
        info_ptr->ppp_info.valid_fields |= (uint32) NET_CFG_PRM_PRI_DNS_MASK;
        DS_MSG1(MSG_LEGACY_HIGH,"primary DNS address in opt:%lx", 
                           info_ptr->ppp_info.ipcp_info.primary_dns);
        break;
  
      /*-----------------------------------------------------------------------
        secondary dns server address
      -----------------------------------------------------------------------*/
      case IPCP_SECONDARY_DNS:
        info_ptr->ppp_info.ipcp_info.secondary_dns = 
          ps_ntohl((*((uint32*)opt_val)));
        info_ptr->ppp_info.valid_fields |= (uint32) NET_CFG_PRM_SEC_DNS_MASK;
        DS_MSG1(MSG_LEGACY_HIGH,"secondary DNS address in opt:%lx", 
                           info_ptr->ppp_info.ipcp_info.secondary_dns);
        break;

      default:
        DS_MSG1(MSG_LEGACY_HIGH,"Got unknown option %d - ignoring", opt_hdr.type);
  
    } /* switch(opt_hdr.type) */

   } /* while(more options) */
   
   DS_RMSM_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );

   return 0;
} /*ds_3gpp_rmsm_ip_common_parse_ipv6cp_cfg_req() */


/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_PARSE_IPV6CP_CFG_REQ

 DESCRIPTION
   
   This function goes through the header of the IPv6CP packet. It will retrieve
   the IPv6CP Config Request ID from the header. This ID will be used during
   generation of a NAK / ACK.

 DEPENDENCIES
     This function expects to be called with a dsm_item whose data pointer is
  pointing at the code byte for the ppp packet.


   Assumes that it will ONLY be called forIPv6CP. If an error occurs in 
   pulling data from the packet, the packet is left as-is
   
 RETURN VALUE
   RET_ERROR (-1) if there is an error (i.e. parsing of the packet breaks).
   RET_OK    (0)  on sucessful completion of recording all options in packet.

 SIDE EFFECTS
   
===========================================================================*/
sint15 ds_3gpp_rmsm_ip_common_parse_ipv6cp_cfg_req
(
  ds_rmsmi_info_type       *rmsmi_info_ptr
)
{
   dsm_item_type*    item_ptr = NULL;       /* Working ptr to PPP packet */
   struct option_hdr opt_hdr;            /* option header */
   struct config_hdr cfg_hdr;            /* Config hdr: has Code, ID, len */
   uint16            offset = 0; /* offset from begining of current item */
   uint16            copy_len = CONFIG_HDR_LEN; /* Number of bytes copied into buffer */
   byte              opt_val[PPP_OPT_MAX_LEN];  /* array to copy option value into */ 
   ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;
   ds_local_err_val_e_type err_val = DS_LOCAL_ERR_VAL_NONE;
   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   
   memset(opt_val, 0, PPP_OPT_MAX_LEN);
   memset(&opt_hdr, 0, sizeof(opt_hdr));
   memset(&cfg_hdr, 0, sizeof(cfg_hdr));
   
   DS_MSG1(MSG_LEGACY_HIGH,"Parsing IPV6CP Config Request Packet, rmsmi_info_ptr: %x", 
                    rmsmi_info_ptr);

   if (rmsmi_info_ptr == NULL) 
   {
     err_val = DS_LOCAL_ERR_VAL_0;
     goto func_exit;
   }
   
   info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
   if(info_ptr == NULL)
   {
     err_val = DS_LOCAL_ERR_VAL_1;
     goto func_exit;
   }

   DS_RMSM_ENTER_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );

   item_ptr = info_ptr->v6_info.ipv6cp_cfg_pkt_ptr;       
   if(item_ptr == NULL)
   {
     err_val = DS_LOCAL_ERR_VAL_2;
     goto func_exit;
   }
   
   if(dsm_seek_extract(&item_ptr, &offset, &cfg_hdr, &copy_len) == FALSE)
   {
     DS_RMSM_BRANCH_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );
     DS_MSG0(MSG_LEGACY_ERROR,"seek_extract() failed - ignoring pkt");
     err_val = DS_LOCAL_ERR_VAL_3;
     goto func_exit;
   }
   info_ptr->v6_info.ipv6cp_creq_id = cfg_hdr.identifier;

   DS_MSG1(MSG_LEGACY_HIGH,"ipv6cp id:%d",cfg_hdr.identifier);

   DS_RMSM_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );

   return 0;

func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ip_common_parse_ipv6cp_cfg_req err_val:%d",err_val);

  return -1;
} /*ds_3gpp_rmsm_ip_common_parse_ipv6cp_cfg_req() */

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_GET_AUTH_INFO_FROM_RM_PPP

 DESCRIPTION
   
   This function will retrieve the Authentitcation information from PPP
   device running on the Rm device 

 DEPENDENCIES
   If Auth information is sucessfully retrieved, it updates the Auth info
   in the RM state machine to point to the valid auth info structure, else
   it is set to NULL

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
void ds_3gpp_rmsm_ip_common_get_auth_info_from_rm_ppp
(
  ds_rmsmi_info_type* rmsmi_info_ptr
)
{
  int result = -1;
  ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;
  ppp_type *ppp_cb_array = NULL;
  ppp_dev_enum_type ppp_dev;
  ds_local_err_val_e_type     err_val = DS_LOCAL_ERR_VAL_NONE;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DS_MSG1(MSG_LEGACY_HIGH,"Getting Auth info from Rm PPP, rmsmi_info_ptr: %x", 
                   rmsmi_info_ptr);

  if (rmsmi_info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  if(info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  ppp_dev = ds_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);
  if (ds_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
  {
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  result = ppp_get_auth_info(ppp_dev,
                             &info_ptr->ppp_info.auth_info.params,
                             &info_ptr->ppp_info.auth_info.auth_id );

  if( result != 0 )
  {
    /*-----------------------------------------------------------------------
      Auth was never done on Rm. Update Mode
    -----------------------------------------------------------------------*/
    info_ptr->ppp_info.auth_info.auth_type = DS_PROFILE_AUTH_NONE;
  }
  else
  {
    info_ptr->ppp_info.valid_fields |= NET_CFG_PRM_AUTH_PREF_MASK |
                                       NET_CFG_PRM_AUTH_USERNAME_MASK | 
                                       NET_CFG_PRM_AUTH_PASSWORD_MASK;

   /* ppp_cb_array used by PPP_AUTH_WAS_CHAP macro */
   ppp_cb_array = fsm_get_ppp_cb_array();

   /*-----------------------------------------------------------------------
      Find out if it was PAP/CHAP
    -----------------------------------------------------------------------*/
    if( PPP_AUTH_WAS_CHAP(ppp_dev) )
    {
      info_ptr->ppp_info.auth_info.auth_type =DS_PROFILE_AUTH_CHAP;
      info_ptr->ppp_info.valid_fields |= NET_CFG_PRM_CHAP_CHAL_INFO_MASK  ;
    }
    else
    {
      info_ptr->ppp_info.auth_info.auth_type = DS_PROFILE_AUTH_PAP;
    }
  }
  
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ip_common_get_auth_info_from_rm_ppp "
                                "err_val %d", err_val);
  return;  
} /*ds_3gpp_rmsm_ip_common_get_auth_info_from_rm_ppp*/

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPCP_HTONCNF

 DESCRIPTION
   
   This function will add the IPCP header fields to the IPCP packet in 
   network order
   
 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/

dsm_item_type  *ds_3gpp_rmsm_ip_common_ipcp_htoncnf 
(
  struct config_hdr *cnf,
  dsm_item_type     *bp
)
{
  register uint8 *cp = NULL;           
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  DS_MSG2(MSG_LEGACY_HIGH,"Entered ds_3gpp_rmsm_ip_common_ipcp_htoncnf, config_hdr: %x"
                    "reply_bp: %x", cnf, bp);
 
  if (cnf == NULL) 
  {
    DATA_ERR_FATAL("cnf = NULL");
	  /*lint -e527 */
	  return NULL;
	  /*lint +e527 */
  }
 if (dsm_pushdown(&bp, NULL, CONFIG_HDR_LEN, 
           DSM_DS_SMALL_ITEM_POOL) == CONFIG_HDR_LEN)
  {
    cp = bp->data_ptr;
    *cp++ = cnf->code;
    *cp++ = cnf->identifier;
    (void)put16(cp, cnf->length);
  }
  else
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Out of Memory");
    dsm_free_packet(&bp);
  }
  
  return bp;
} /* ds_ip_common_ipcp_htoncnf() */

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPCP_RSP_TIMER_CB

 DESCRIPTION
   
   This function will be called when the IPCP_RSP_TIMER expires

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
void ds_3gpp_rmsm_ip_common_ipcp_rsp_timer_cb
( 
  void* callback_data
)
{
  ds_command_type                *cmd_ptr;  /* DS Task cmd buf        */
  ds_rmsm_instance_type  *data_ptr = NULL;
  ds_rmsmi_info_type     *rmsmi_info_ptr = NULL;

  /*-----------------------------------------------------------------------
    Get the pointer to the state machine. We need sm_ptr to 
  -----------------------------------------------------------------------*/
  rmsmi_info_ptr = (ds_rmsmi_info_type*) callback_data;

  if (rmsmi_info_ptr == NULL) 
  {
  	DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
	  /*lint -e527 */
  	return;
  	/*lint +e527 */
  }

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_rmsm_instance_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    //ASSERT(0);
    return;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_RMSM_IPCP_RSP_TIMER;
    data_ptr = (ds_rmsm_instance_type*)cmd_ptr->cmd_payload_ptr;
    data_ptr->rm_dev_instance = rmsmi_info_ptr->instance;
    data_ptr->sm_instance = DS_RMSM_V4_SM_INSTANCE;
    ds_put_cmd(cmd_ptr);
  }
} /*ds_3gpp_rmsm_ip_common_ipcp_rsp_timer_cb*/

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPCP_RSP_TIMER_CMD_HDLR

 DESCRIPTION
   
   This function will generate a IPCP NAK with internal DNS addresses ONLY.
   This function is called in DS context when the rex timer 
   ipcp_res_del_timer fires off

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_ipcp_rsp_timer_cmd_hdlr
( 
  ds_rmsm_instance_type* inst_ptr
)
{

  ds_rmsmi_info_type  *rmsmi_info_ptr = NULL;
  ds_rmsmi_sm_info_type  *sm_info_ptr = NULL;
  dsm_item_type           *reply_bp = NULL;              /* reply packet   */
  struct config_hdr       hdr;

  register uint8          *cp = NULL;
  uint16                  opt_len = 0;
  uint8                   *ipcp_buf = NULL;
  uint16                  protocol = 0 ;
  ppp_dev_enum_type       ppp_dev;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  memset(&hdr, 0, sizeof(hdr));

  if (inst_ptr == NULL) 
  {
    DATA_ERR_FATAL("inst_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  DS_MSG1(MSG_LEGACY_HIGH,"Entered IPCP Response Timer Callback handling,"
                   " Rm Dev instance: %d", 
                   inst_ptr->rm_dev_instance);

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL) 
  {
  	DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
	  /*lint -e527 */
  	return;
  	/*lint +e527 */
  }

  sm_info_ptr = ds_rmsmi_get_sm_info_ptr(inst_ptr);

  if (sm_info_ptr == NULL) 
  {
  	DATA_ERR_FATAL("sm_info_ptr = NULL");
	  /*lint -e527 */
  	return;
  	/*lint +e527 */
  }

  ppp_dev = ds_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);

  if (ds_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
  {
    return;
  }

  if( DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) != DS_PROFILE_PDP_IPV4 &&
      DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) != DS_PROFILE_PDP_IPV4V6)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"Wrong PDP Type: %d, Expected V4. Returning",
                       DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr));
    return;
  }

  /*-----------------------------------------------------------------------
    Generate the response packet ONLY if we get the timer in 
    DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_UP_STATE state
  -----------------------------------------------------------------------*/
  if( ds_rmsm_ip_get_state(sm_info_ptr) != DS_RMSM_IP_WAITING_FOR_UM_UP_STATE  )
  {
    DS_MSG1(MSG_LEGACY_HIGH,"IPCP rsp gen timer fired in wrong state:%d ignoring",
             rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
    return;
  }

  /*-----------------------------------------------------------------------
    Allocate DSM items with space reserved for IPCP config HDR. Reserve the
    space only if the Small DSM item has enough space to store config HDR. 
    Otherwise ds_ip_ipcp_htoncnf will allocate a new DSM item for HDR.
  -----------------------------------------------------------------------*/
  if( sizeof(struct config_hdr) <= DSM_POOL_ITEM_SIZE(DSM_DS_SMALL_ITEM_POOL))
  {
    reply_bp = dsm_offset_new_buffer(
                         DSM_DS_SMALL_ITEM_POOL,
                         sizeof(ppp_config_hdr_type));  
    if( reply_bp == NULL) 
    {
    /*-----------------------------------------------------------------------
        Assert since we ran out of small DSM items. We should not reach here 
        since after we hit low threshold on Small DSM items, RLC will release 
        all the DSM items in its queues.
    -----------------------------------------------------------------------*/
        DATA_ERR_FATAL("failed to DSM item for IPCP config HDR");
        return;
    }
  }

 opt_len = IPCP_1ADDRESS_SIZ;
 protocol = PPP_IPCP_PROTOCOL;
  DS_SYSTEM_HEAP_MEM_ALLOC(ipcp_buf,(opt_len*sizeof(uint8)), uint8*);
  if(ipcp_buf == NULL)
  {
    ASSERT(0);
    return;
  }

  /*-----------------------------------------------------------------------
    primary dns server address
  -----------------------------------------------------------------------*/
  cp    = ipcp_buf;
  *cp++ = IPCP_PRIMARY_DNS;
  *cp++ = IPCP_1ADDRESS_SIZ;
  cp    = put32(cp, bogus_ip_pdns);
  if( dsm_pushdown_tail( &reply_bp, ipcp_buf, 
                              opt_len, DSM_DS_SMALL_ITEM_POOL) != opt_len)
  {
    dsm_free_packet(&reply_bp);
    DS_SYSTEM_HEAP_MEM_FREE(ipcp_buf);
    DATA_ERR_FATAL("failed to push Pri DNS option to DSM item");
    return;
  }


  /*-----------------------------------------------------------------------
    secondary dns server address
  -----------------------------------------------------------------------*/
  cp    = ipcp_buf;
  *cp++ = IPCP_SECONDARY_DNS;
  *cp++ = IPCP_1ADDRESS_SIZ;
  cp    = put32(cp, bogus_ip_sdns);
  if( dsm_pushdown_tail( &reply_bp, ipcp_buf, 
                              opt_len, DSM_DS_SMALL_ITEM_POOL) != opt_len)
  {
    dsm_free_packet(&reply_bp);
    DS_SYSTEM_HEAP_MEM_FREE(ipcp_buf);
    DATA_ERR_FATAL("failed to push Sec DNS option to DSM item");
    return;
  }
  
  /*-----------------------------------------------------------------------
    Process NBNS options only if NBNS NV item is enabled.  
  -----------------------------------------------------------------------*/
 if(0)//ds_3gpp_nv_manager_get_nbns_enabled(rmsmi_info_ptr->subs_id))
  {
    /*---------------------------------------------------------------------
      primary nbns server address
  -----------------------------------------------------------------------*/
    cp    = ipcp_buf;
    *cp++ = IPCP_PRIMARY_NBNS;
    *cp++ = IPCP_1ADDRESS_SIZ;
    cp    = put32(cp, bogus_ip_pdns);
    if( dsm_pushdown_tail( &reply_bp, ipcp_buf, 
                              opt_len, DSM_DS_SMALL_ITEM_POOL) != opt_len)
    {
      dsm_free_packet(&reply_bp);
      DS_SYSTEM_HEAP_MEM_FREE(ipcp_buf);
      DATA_ERR_FATAL("failed to push Pri NBNS option to DSM item");
      return;
    }


    /*-----------------------------------------------------------------------
      secondary nbns server address
    -----------------------------------------------------------------------*/
    cp    = ipcp_buf;
   *cp++ = IPCP_SECONDARY_NBNS;
   *cp++ = IPCP_1ADDRESS_SIZ;
    cp    = put32(cp, bogus_ip_sdns);
    if( dsm_pushdown_tail( &reply_bp, ipcp_buf, 
                                opt_len, DSM_DS_SMALL_ITEM_POOL) != opt_len)
    {
      dsm_free_packet(&reply_bp);
      DS_SYSTEM_HEAP_MEM_FREE(ipcp_buf);
      DATA_ERR_FATAL("failed to push Sec NBNS option to DSM item");
      return;
    }
  }

  DS_MSG2(MSG_LEGACY_HIGH,"Bogus IP PDNs :%lx Bogus IP SDNs:%lx",
                    bogus_ip_pdns,bogus_ip_sdns);

 hdr.code = CONFIG_NAK;
  hdr.identifier =
    rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->v4_info.ipcp_creq_id; 
 hdr.length = (uint16)(dsm_length_packet(reply_bp) + CONFIG_HDR_LEN);

  /*-------------------------------------------------------------------------
    Prepend header to packet
  -------------------------------------------------------------------------*/
  reply_bp = ds_3gpp_rmsm_ip_common_ipcp_htoncnf(&hdr,reply_bp);
  if (reply_bp == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"htoncf failed : IPCP NAK not generated");
    dsm_free_packet(&reply_bp);
    DS_SYSTEM_HEAP_MEM_FREE(ipcp_buf);
    return ;
  }

  if(pppi_outgoing(ppp_dev, protocol, &reply_bp, NULL) == -1)
  {
   /*---------------------------------------------------------------------
     pppi_outgoing() frees the packet in case of an error
   ---------------------------------------------------------------------*/
   DS_MSG0(MSG_LEGACY_HIGH, "pppi_outgoing() Error");
  }
  DS_SYSTEM_HEAP_MEM_FREE(ipcp_buf);

  DS_MSG0(MSG_LEGACY_HIGH,"IPCP delayed NAK sent");

} /*ds_3gpp_rmsm_ip_common_ipcp_rsp_timer_cmd_hdlr*/

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPV6CP_RSP_TIMER_CB

 DESCRIPTION
   
   This function will be called when the IPV6CP_RSP_TIMER expires

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_ipv6cp_rsp_timer_cb
( 
  void*  callback_data
)
{
  ds_command_type                *cmd_ptr; /* DS Task cmd buf        */
  ds_rmsm_instance_type  *data_ptr = NULL;
  ds_rmsmi_info_type     *rmsmi_info_ptr = NULL;

  /*-----------------------------------------------------------------------
    Get the pointer to the state machine. We need sm_ptr to 
  -----------------------------------------------------------------------*/
  rmsmi_info_ptr = (ds_rmsmi_info_type*) callback_data;

  if (rmsmi_info_ptr == NULL) 
  {
  	DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
	  /*lint -e527 */
  	return;
  	/*lint +e527 */
  }

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_rmsm_instance_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_RMSM_IPV6CP_RSP_TIMER;
    data_ptr = (ds_rmsm_instance_type*)cmd_ptr->cmd_payload_ptr;
    data_ptr->rm_dev_instance = rmsmi_info_ptr->instance;
    data_ptr->sm_instance = DS_RMSM_V6_SM_INSTANCE;
    ds_put_cmd(cmd_ptr);
  }
} /* ds_rmsm_ip_ipv6cp_rsp_timer_cb */

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_IPV6CP_RSP_TIMER_CB

 DESCRIPTION
   
   This function will generate a IPCP NAK with internal DNS addresses ONLY.
   This function is called in DS context when the rex timer 
   ipcp_res_del_timer fires off

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_ipv6cp_rsp_timer_cmd_hdlr
( 
  ds_rmsm_instance_type* inst_ptr
)
{
  ds_rmsmi_info_type  *rmsmi_info_ptr = NULL;
  ds_rmsmi_sm_info_type* sm_ptr = NULL;
  dsm_item_type           *reply_bp = NULL;              /* reply packet   */
  struct config_hdr       hdr;

  register uint8          *cp = NULL;
  uint16                  opt_len = 0;
  uint8                   *ipv6cp_buf = NULL;
  uint64                  iid = 0;
  uint16                  protocol = 0;
  int16                   ps_errno = 0;
  ppp_dev_enum_type       ppp_dev;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset(&hdr, 0, sizeof(hdr));

  if (inst_ptr == NULL) 
  {
    DATA_ERR_FATAL("inst_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  DS_MSG1(MSG_LEGACY_HIGH,"Entered IPV6CP Response Timer Callback handling, "
                   "Rm Dev instance: %d", 
                   inst_ptr->rm_dev_instance);

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL) 
  {
    DATA_ERR_FATAL("rmsmi_info_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  ppp_dev = ds_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);

  if (ds_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
  {
    return;
  }

  sm_ptr = ds_rmsmi_get_sm_info_ptr(inst_ptr);

  if (sm_ptr == NULL) 
  {
    DATA_ERR_FATAL("sm_ptr = NULL");
    /*lint -e527 */
    return;
    /*lint +e527 */
  }

  if( DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) != DS_PROFILE_PDP_IPV6 &&
      DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) != DS_PROFILE_PDP_IPV4V6)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"Wrong PDP Type: %d, Expected V6. Returning",
                       DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr));
    return;
  }

  /*-----------------------------------------------------------------------
    Generate the response packet ONLY if we get the timer in 
    DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_UP_STATE state
  -----------------------------------------------------------------------*/
  if( ds_rmsm_ip_get_state(sm_ptr) != DS_RMSM_IP_WAITING_FOR_UM_UP_STATE  )
  {
    DS_MSG1(MSG_LEGACY_HIGH,"IPv6CP rsp gen timer fired in wrong state:%d ignoring",
             rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
    return;
  }

  /*-----------------------------------------------------------------------
    Allocate DSM items with space reserved for IPCP config HDR. Reserve the
    space only if the Small DSM item has enough space to store config HDR. 
    Otherwise ds_ip_ipcp_htoncnf will allocate a new DSM item for HDR.
  -----------------------------------------------------------------------*/
  if( sizeof(struct config_hdr) <= DSM_POOL_ITEM_SIZE(DSM_DS_SMALL_ITEM_POOL))
  {
    reply_bp = dsm_offset_new_buffer(
                         DSM_DS_SMALL_ITEM_POOL,
                         sizeof(ppp_config_hdr_type));  
    if( reply_bp == NULL) 
    {
    /*-----------------------------------------------------------------------
        Assert since we ran out of small DSM items. We should not reach here 
        since after we hit low threshold on Small DSM items, RLC will release 
        all the DSM items in its queues.
    -----------------------------------------------------------------------*/
        DATA_ERR_FATAL("failed to DSM item for IPCP config HDR");
        return;
    }
  }

 opt_len = IPV6CP_IFACE_ID_OPTION_SIZE;
 protocol = PPP_IPV6CP_PROTOCOL;
  DS_SYSTEM_HEAP_MEM_ALLOC(ipv6cp_buf, (opt_len*sizeof(uint8)), uint8*);
  if( ipv6cp_buf == NULL)
  {
    ASSERT(0);
    return;
  }

  if (-1 == ps_generate_ipv6_iid(sm_ptr->fw_inst, &iid, &ps_errno))
  {
    DS_SYSTEM_HEAP_MEM_FREE(ipv6cp_buf);
    DATA_ERR_FATAL("Failed to generate a random IPv6 IID");
    return;
  }

  /*-----------------------------------------------------------------------
    Interface ID
  -----------------------------------------------------------------------*/
  cp    = ipv6cp_buf;
  *cp++ = IPV6CP_IFACE_ID_OPTION;
  *cp++ = IPV6CP_IFACE_ID_OPTION_SIZE;
  cp    = put64(cp, iid);
  if( dsm_pushdown_tail( &reply_bp, ipv6cp_buf, 
                              opt_len, DSM_DS_SMALL_ITEM_POOL) != opt_len)
  {
    dsm_free_packet(&reply_bp);
    DS_SYSTEM_HEAP_MEM_FREE(ipv6cp_buf);
    DATA_ERR_FATAL("failed to push Pri DNS option to DSM item");
    return;
  }

  DS_MSG1(MSG_LEGACY_HIGH,"IID :%lx",iid);
  
  hdr.code = CONFIG_NAK;
  hdr.identifier = 
    rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->v4_info.ipcp_creq_id; 
  hdr.length = (uint16)(dsm_length_packet(reply_bp) + CONFIG_HDR_LEN);

  /*-------------------------------------------------------------------------
    Prepend header to packet
  -------------------------------------------------------------------------*/
  reply_bp = ds_3gpp_rmsm_ip_common_ipcp_htoncnf(&hdr,reply_bp);
  if (reply_bp == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"htoncf failed : IPCP NAK not generated");
    dsm_free_packet(&reply_bp);
    DS_SYSTEM_HEAP_MEM_FREE(ipv6cp_buf);
    return ;
  }

  if(pppi_outgoing(ppp_dev, protocol, &reply_bp, NULL) == -1)
  {
   /*---------------------------------------------------------------------
     pppi_outgoing() frees the packet in case of an error
   ---------------------------------------------------------------------*/
   DS_MSG0(MSG_LEGACY_HIGH, "pppi_outgoing() Error");
  }

  DS_SYSTEM_HEAP_MEM_FREE(ipv6cp_buf);
  DS_MSG0(MSG_LEGACY_HIGH,"IPv6CP delayed NAK sent");
} /* ds_3gpp_rmsm_ip_common_ipv6cp_rsp_timer_cmd_hdlr */


/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_CLEAR_NAK_TIMER

 DESCRIPTION
   
   This function is used to clear the NAK timer.

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
static void ds_3gpp_rmsm_ip_common_clear_nak_timer
(
  ds_rmsmi_info_type* rmsmi_info_ptr
)
{
  ds_3gpp_rmsm_ip_common_info_type* info_ptr = NULL;

  if (rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi info ptr is NULL");
    return;
  }
  
  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  ASSERT(info_ptr != NULL);


  if (rmsmi_info_ptr->instance == DS_RMSM_V4_SM_INSTANCE)
  {  
     DS_RMSM_ENTER_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );
   
    /*---------------------------------------------------------------------
      Stop the IPCP response generation timer, if we had started it
    ---------------------------------------------------------------------*/            
    if(info_ptr->v4_info.send_ipcp_naks_flg )
    {
      /*-----------------------------------------------------------------
         Stop the timer to delay the IPCP response packet generation.
      -----------------------------------------------------------------*/
     //(void)rex_clr_timer(&(info_ptr->v4_info.ipcp_res_del_timer));
	  ps_timer_cancel(info_ptr->v4_info.ipcp_res_del_timer);
    }

    DS_RMSM_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );
  }
  
  else if (rmsmi_info_ptr->instance == DS_RMSM_V6_SM_INSTANCE)
  {
    DS_RMSM_ENTER_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );
  
   /*---------------------------------------------------------------------
     Stop the IPV6CP response generation timer, if we had started it
    ---------------------------------------------------------------------*/            
    if(info_ptr->v6_info.send_ipv6cp_naks_flg )
    {
      /*-----------------------------------------------------------------
         Stop the timer to delay the IPv6CP response packet generation.
      -----------------------------------------------------------------*/
     //(void)rex_clr_timer(&(info_ptr->v6_info.ipv6cp_res_del_timer));
     ps_timer_cancel(info_ptr->v6_info.ipv6cp_res_del_timer);

    }

    DS_RMSM_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );
  }
  
  else
  {
    DS_MSG1(MSG_LEGACY_ERROR,"PDP Type is: %d, Expected V4 or V6", 
                        DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr));          
    ASSERT(0);
  }
} /* ds_3gpp_rmsm_ip_common_clear_nak_timer */


/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_TRANSITION_STATE

 DESCRIPTION
   
   This function is used to maintain the state of RmSm IP Common.

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
void ds_3gpp_rmsm_ip_common_transition_state
(
  ds_rmsm_rm_dev_instance_type    rm_dev_instance,
  ds_3gpp_rmsm_ip_common_state_type    new_state
)
{

 /* Report with next CONNECT */
 dsat_connect_rate_s_type              connect_rate; 
  ds_rmsmi_info_type*                  rmsmi_info_ptr = NULL;
  ds_rmsmi_sm_info_type*               v4_sm_info_ptr = NULL;
  ds_rmsmi_sm_info_type*               v6_sm_info_ptr = NULL;
  int                                  retval = -1;
  ds_rmsm_instance_type                inst;
  ds_rmsm_instance_type                temp_inst;
  ds_3gpp_rmsm_ip_common_info_type     *info_ptr = NULL;
  ppp_dev_enum_type                    ppp_dev;
  ds_profile_pdp_type_e                pdp_type;
  ds_rmsmi_sm_info_type*               sm_info_ptr = NULL;
  ds_rmsm_sm_instance_type             sm_instance = DS_RMSM_INVALID_SM_INSTANCE;
  ds_local_err_val_e_type              err_val = DS_LOCAL_ERR_VAL_NONE;
  
  /*---------------------------------------------------------------------------*/

  inst.rm_dev_instance = temp_inst.rm_dev_instance = rm_dev_instance;
  inst.sm_instance = DS_RMSM_INVALID_SM_INSTANCE; 

  memset( (void*)&connect_rate, 0, sizeof(dsat_connect_rate_s_type) );
  DS_MSG1(MSG_LEGACY_HIGH,"Entered ds_rmsmi_transition_state,"
                   " rm_dev_instance: %d", rm_dev_instance);

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(rm_dev_instance);
  if((rmsmi_info_ptr == NULL) || (rmsmi_info_ptr->sm_type != DS_RMSM_SM_TYPE_PDP_IP))
  {
    err_val = DS_LOCAL_ERR_VAL_0;
	  goto func_exit;
  }

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  if(info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_1;
	  goto func_exit;
  }

  ppp_dev = ds_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);

  if (ds_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
  {
    err_val = DS_LOCAL_ERR_VAL_2;
	  goto func_exit;
  }

  pdp_type = DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr);

  if ((pdp_type == DS_PROFILE_PDP_IPV4 || 
      pdp_type == DS_PROFILE_PDP_IPV4V6) ||
      (pdp_type == DS_PROFILE_PDP_IPV6 && 
      ds_pdnmgr_is_prof_clat_enabled(rmsmi_info_ptr->profile_id)))
  {
    temp_inst.sm_instance = DS_RMSM_V4_SM_INSTANCE;
    v4_sm_info_ptr = ds_rmsmi_get_sm_info_ptr(&temp_inst);
    DS_ASSERT(v4_sm_info_ptr != NULL);
  }

  if (pdp_type == DS_PROFILE_PDP_IPV6 ||
      pdp_type == DS_PROFILE_PDP_IPV4V6)
  {
    temp_inst.sm_instance = DS_RMSM_V6_SM_INSTANCE;
    v6_sm_info_ptr = ds_rmsmi_get_sm_info_ptr(&temp_inst);
    DS_ASSERT(v6_sm_info_ptr != NULL);
  }

  DS_MSG3(MSG_LEGACY_HIGH,"Changing the State of RmSm IP common from %d to %d", 
                     rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state,
                     new_state,
                     rmsmi_info_ptr->port_id);  

  /* Update the new common info state */					 
  rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state = new_state;

  DS_MSG1(MSG_LEGACY_HIGH,"Entering RMSM IP common %d state",new_state);

  switch (new_state)
  {
    case DS_3GPP_RMSM_IP_COMMON_NULL_STATE:
    {
      /* Pkt call is terminating, Perform all the necessary cleanup */
      ds3g_siolib_ex_set_inbound_flow( DS_FLOW_UMTS_RMSM_MASK, DS_FLOW_DISABLE,
                                       rmsmi_info_ptr->port_id );
      if (ds3g_siolib_ex_set_cd_state
           (DS3G_SIOLIB_REMOTE_CARRIER_OFF,rmsmi_info_ptr->port_id ) == 
            DS3G_SIOLIB_NO_PORT_OPEN)
      {
        DS_MSG0(MSG_LEGACY_ERROR,"Error setting CD State");  
      }

      if (ds3g_siolib_ex_change_mode
            ( SIO_DS_AUTODETECT_MODE, NULL, NULL, NULL, rmsmi_info_ptr->port_id)
           == DS3G_SIOLIB_NO_PORT_OPEN)
      {
        DS_MSG0(MSG_LEGACY_ERROR,"Error changing mode to autodetect");
      }
#ifdef FEATURE_DATA_ATCOP
      dsat_send_result_code_ex( DSAT_NO_CARRIER,rmsmi_info_ptr->port_id );
#endif /* FEATURE_DATA_ATCOP */
      rmsmi_info_ptr->singleip_dual_pf_call = FALSE;            

      /*---------------------------------------------------------------------
        Reset the PPP state and deregister the callbacks
      ---------------------------------------------------------------------*/

      for (sm_instance = DS_RMSM_V4_SM_INSTANCE;
             sm_instance < DS_RMSM_MAX_SM_INSTANCES; sm_instance++)
      {
        sm_info_ptr = rmsmi_info_ptr->sm_info_ptr[sm_instance];
        if(sm_info_ptr != NULL)
        {
          sm_info_ptr->recd_um_down = FALSE;
        }
      }

      PS_TIMER_FREE_HANDLE(rmsmi_info_ptr->ppp_up_delay_timer);
      PS_TIMER_FREE_HANDLE(rmsmi_info_ptr->config_wait_timer);

      if ((pdp_type == DS_PROFILE_PDP_IPV4 || pdp_type == DS_PROFILE_PDP_IPV4V6 ) ||
         (pdp_type == DS_PROFILE_PDP_IPV6 && ds_pdnmgr_is_prof_clat_enabled(rmsmi_info_ptr->profile_id)))
      {
        retval = ppp_event_deregister
                 (
                   rmsmi_info_ptr->ppp_handle,
                   PPP_PROTOCOL_IPCP,
                   (ppp_event_mask_e_type) (PPP_UP_EV_MASK | PPP_RESYNC_EV_MASK)
                 );

        if (retval == -1)
        {
          DS_MSG0(MSG_LEGACY_ERROR,"Unable to deregister IPCP events");
        }

        /*---------------------------------------------------------------------
        Free any IPCP packets we have stored & set sm specific info
        dsm_free_packet will set the pointer to NULL
        ---------------------------------------------------------------------*/
        DS_RMSM_ENTER_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );

        if(info_ptr->v4_info.ipcp_cfg_pkt_ptr != NULL )
        {
          dsm_free_packet(&info_ptr->v4_info.ipcp_cfg_pkt_ptr);
        }

        DS_RMSM_LEAVE_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );

        ds_3gpp_rmsm_ip_common_reset_ipcp_creq_recd(info_ptr);
        if(DS_RMSM_IP_NULL_STATE != 
                ds_rmsm_ip_get_state(rmsmi_info_ptr->sm_info_ptr[DS_RMSM_V4_SM_INSTANCE]))
        {
          ds_rmsm_ip_process_pdn_down_ev(rmsmi_info_ptr->sm_info_ptr[DS_RMSM_V4_SM_INSTANCE]);
        }

      }

      if (pdp_type == DS_PROFILE_PDP_IPV6 || pdp_type == DS_PROFILE_PDP_IPV4V6)
      {
        retval = ppp_event_deregister(
                  rmsmi_info_ptr->ppp_handle,
                  PPP_PROTOCOL_IPV6CP,
                  (ppp_event_mask_e_type) (PPP_UP_EV_MASK | PPP_RESYNC_EV_MASK));
        if (retval == -1)
        {
          DS_MSG0(MSG_LEGACY_ERROR,"Unable to deregister IPV6CP events");
        }
         
        /*---------------------------------------------------------------------
        Free any IPV6CP packets we have stored & set sm specific info
        dsm_free_packet will set the pointer to NULL
        ---------------------------------------------------------------------*/
        DS_RMSM_ENTER_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );

        if(info_ptr->v6_info.ipv6cp_cfg_pkt_ptr != NULL )
        {
          dsm_free_packet(&info_ptr->v6_info.ipv6cp_cfg_pkt_ptr);
        }

        DS_RMSM_LEAVE_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );

        ds_3gpp_rmsm_ip_common_reset_ipv6cp_creq_recd(info_ptr);
        if(DS_RMSM_IP_NULL_STATE != 
                ds_rmsm_ip_get_state(rmsmi_info_ptr->sm_info_ptr[DS_RMSM_V6_SM_INSTANCE]))
        {
          ds_rmsm_ip_process_pdn_down_ev(rmsmi_info_ptr->sm_info_ptr[DS_RMSM_V6_SM_INSTANCE]);
        }

      }

      retval = ppp_event_deregister( rmsmi_info_ptr->ppp_handle,
                                     PPP_PROTOCOL_LCP,
                                     PPP_DOWN_EV_MASK );

      if (retval == -1)
      {
        DS_MSG0(MSG_LEGACY_ERROR,"Unable to deregister LCP events");
      }
      
      retval = ppp_abort( ppp_dev );
      if (retval == -1)
      {
        DS_MSG0(MSG_LEGACY_ERROR,"PPP Abort failed");
      }

      DS_SYSTEM_HEAP_MEM_FREE(rmsmi_info_ptr->rm_ppp_config_ptr);

      rmsmi_info_ptr->rm_ppp_config_ptr = NULL;

      /*---------------------------------------------------------------------
        Cleanup local watermarks.
      ---------------------------------------------------------------------*/
      dsm_empty_queue(&rmsmi_info_ptr->sio_tx_wmk);
      dsm_empty_queue(&rmsmi_info_ptr->sio_rx_wmk);

      /*---------------------------------------------------------------------
        Indicate to siolib that we are not in call anymore, deregister
        all the event handlers and enable the flow to accept the next call
      ---------------------------------------------------------------------*/
      ds3g_siolib_ex_set_call_coming_up(rmsmi_info_ptr->port_id,FALSE );
      ds_rmsmi_siolib_deregister_callbacks(rmsmi_info_ptr->port_id);
      ds3g_siolib_ex_release_port ( rmsmi_info_ptr->port_id );

      ds3g_siolib_ex_set_inbound_flow( DS_FLOW_UMTS_RMSM_MASK|DS_FLOW_PKT_WM_MASK,
                                       DS_FLOW_ENABLE,
                                       rmsmi_info_ptr->port_id );

      ds_rmsmi_set_sio_port_id(rmsmi_info_ptr->instance,DS3G_SIOLIB_PORT_NONE);

      /* Deallocate rmsm info and related params since the call 
         needs to be cleaned up */
      ds_rmsmi_free_rmsm_instance(rm_dev_instance);   
    }
    break;
    
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE:
    {
    
      /*---------------------------------------------------------------------
        Send CONNECT result code immediately & Kick start the PPP on the 
        Rm interface & do not bring the call on the Um interface yet. 
        Start PPP in  Passive & INTERNAL_SNOOP mode. Register with PPP to 
        snoop for IPCP config request packet.       
        Note: If we know that we do not have PS service, we can send a 
        "NO CARRIER" result code immediately as an optimization. 
        Return the extended result code with CONNECT.
      ---------------------------------------------------------------------*/
      
      /* Always return PACKET for PS */
      connect_rate.call_type = DSAT_CALL_TYPE_PS;
      connect_rate.comm_prot = DSAT_COMM_PROT_PACKET;
      /* Return APN from the sm_ptr profile details */
      memscpy( (void*)connect_rate.rate.ps_call.apn,
               (DS_SYS_MAX_APN_LEN),
               (void*)rmsmi_info_ptr->profile_info.context.apn,
               (DS_SYS_MAX_APN_LEN) );


      /*Report Data Rate in bps*/
      if ( connect_rate.rate.ps_call.ul_rate == 0 )
      { 
        connect_rate.rate.ps_call.dl_rate = MAX_BIT_RATE_150000KBPS;
        connect_rate.rate.ps_call.ul_rate = MAX_BIT_RATE_50000KBPS;
      }

	    DS_SYSTEM_HEAP_MEM_ALLOC(rmsmi_info_ptr->rm_ppp_config_ptr, sizeof(ppp_dev_opts_type),
	                          ppp_dev_opts_type*);	  
      if(NULL == rmsmi_info_ptr->rm_ppp_config_ptr)
      {
        err_val = DS_LOCAL_ERR_VAL_3;
	      goto func_exit;
      }

      memset(rmsmi_info_ptr->rm_ppp_config_ptr, 0, sizeof(ppp_dev_opts_type));

      ppp_get_default_opts(rmsmi_info_ptr->rm_ppp_config_ptr);
  
      if (pdp_type == DS_PROFILE_PDP_IPV4 || pdp_type == DS_PROFILE_PDP_IPV4V6 ||
          (pdp_type == DS_PROFILE_PDP_IPV6 && 
           ds_pdnmgr_is_prof_clat_enabled(rmsmi_info_ptr->profile_id)))
      {
         /*---------------------------------------------------------------------
          We enable WINS in IPCP configuration explicitly to ensure that 1x 
          is not affected by WINS. 
         ---------------------------------------------------------------------*/
       /* if( ds_3gpp_nv_manager_get_nbns_enabled(rmsmi_info_ptr->subs_id))
        {
          rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
            will_mask[FSM_REMOTE] |= IPCP_N_PRIMARY_NBNS ;
          rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
            will_mask[FSM_LOCAL]  |= IPCP_N_PRIMARY_NBNS ; 
          rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
            want_mask[FSM_LOCAL]  |= IPCP_N_PRIMARY_NBNS ;
       
          rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
            will_mask[FSM_REMOTE] |= IPCP_N_SECONDARY_NBNS ;
          rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
            will_mask[FSM_LOCAL]  |= IPCP_N_SECONDARY_NBNS;
          rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
            want_mask[FSM_LOCAL]  |= IPCP_N_SECONDARY_NBNS;
        }*/
  
        /*---------------------------------------------------------------------
        Disable VJ compression on the Rm link for both Rx and Tx
        ---------------------------------------------------------------------*/
        rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
          will_mask[FSM_LOCAL]  &= ~IPCP_N_COMPRESS;
        rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
          will_mask[FSM_REMOTE] &= ~IPCP_N_COMPRESS;
        rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
          want_mask[FSM_LOCAL]  &= ~IPCP_N_COMPRESS;
        rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
          want_mask[FSM_REMOTE] &= ~IPCP_N_COMPRESS;
  
        rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.
          mode = PPP_CP_PASSIVE;
		
		if(v4_sm_info_ptr != NULL)
		{
          rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.fwk_inst_ptr =
			             v4_sm_info_ptr->fw_inst;
	      ds_rmsm_rm_disable_flow(v4_sm_info_ptr);
		}
      }

      if (pdp_type == DS_PROFILE_PDP_IPV6 || pdp_type == DS_PROFILE_PDP_IPV4V6)
      {
        /*VJ Compression is not supported for IPv6*/
        rmsmi_info_ptr->rm_ppp_config_ptr->ipv6cp_info.fwk_inst_ptr = 
          v6_sm_info_ptr->fw_inst;  
        rmsmi_info_ptr->rm_ppp_config_ptr->ipv6cp_info.mode = 
          PPP_CP_PASSIVE;
		
        ds_rmsm_rm_disable_flow(v6_sm_info_ptr);
      }

      if (pdp_type == DS_PROFILE_PDP_IPV4V6)
      {
        rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.mode = 
          PPP_CP_MODE_DISCARD;

        rmsmi_info_ptr->rm_ppp_config_ptr->ipv6cp_info.mode = 
          PPP_CP_MODE_DISCARD;
      }

      ds_rmsmi_setup_sio_watermarks (rm_dev_instance,
                                     &rmsmi_info_ptr->sio_tx_wmk,
                                     &rmsmi_info_ptr->sio_rx_wmk,
                                     &rmsmi_info_ptr->sio_tx_wmk_q,
                                     &rmsmi_info_ptr->sio_rx_wmk_q);
  
     rmsmi_info_ptr->rm_ppp_config_ptr->rx_f_ptr       
       = ds_rmsmi_sio_rx_ppp_data;
     rmsmi_info_ptr->rm_ppp_config_ptr->rx_user_data  
       = (void *)rmsmi_info_ptr;
     rmsmi_info_ptr->rm_ppp_config_ptr->tx_f_ptr       
       = ds_rmsmi_sio_tx_ppp_data;
     rmsmi_info_ptr->rm_ppp_config_ptr->tx_user_data   
       = (void *)rmsmi_info_ptr;
     
     rmsmi_info_ptr->rm_ppp_config_ptr->lcp_info.mode = PPP_CP_PASSIVE;
     rmsmi_info_ptr->rm_ppp_config_ptr->hdlc_mode     = HDLC_HW_PREF;

	   if(ds3g_siolib_get_sio_port(rmsmi_info_ptr->port_id) == SIO_PORT_A2_MDM)
	   {
	     rmsmi_info_ptr->rm_ppp_config_ptr->hdlc_mode = HDLC_A2_AUTONOMOUS;
	   }	 
     /* Setting bridge dev to 0 as MACRO FOR UM device has been removed. */
     rmsmi_info_ptr->rm_ppp_config_ptr->bridge_dev     = 0;
  
     /*---------------------------------------------------------------------
       Turn ON the authentication on the Rm link. Attempt Chap, If peer
       rejects it attempt PAP or fallback to NO auth
     ---------------------------------------------------------------------*/
     rmsmi_info_ptr->rm_ppp_config_ptr->lcp_info.
       will_mask[FSM_REMOTE] &= ~LCP_N_AP;
     rmsmi_info_ptr->rm_ppp_config_ptr->lcp_info.
       want_mask[FSM_REMOTE] &= ~LCP_N_AP;
     (void)ppp_set_auth_server_opts(rmsmi_info_ptr->rm_ppp_config_ptr,
                                    "UMTS_CHAP_SRVR");

#ifdef FEATURE_GSM_DTM  
       if (rr_dtm_get_max_LAPDm_octets() != 0) {
         rmsmi_info_ptr->rm_ppp_config_ptr->lcp_info.
           work_vals[FSM_LOCAL].authentication = PPP_PAP_PROTOCOL;
         rmsmi_info_ptr->rm_ppp_config_ptr->lcp_info.
           want_vals[FSM_LOCAL].authentication = PPP_PAP_PROTOCOL;
         DS_MSG0(MSG_LEGACY_HIGH,"Using PAP protocol");
       }
       else
#endif /* FEATURE_GSM_DTM */
       {                 
         if (rmsmi_info_ptr->profile_info.auth.auth_type == DS_PROFILE_AUTH_PAP)
         {
           DS_MSG0(MSG_LEGACY_HIGH,"Using PAP protocol");
           rmsmi_info_ptr->rm_ppp_config_ptr->lcp_info.work_vals[FSM_LOCAL].authentication 
                                                                       = PPP_PAP_PROTOCOL;
           rmsmi_info_ptr->rm_ppp_config_ptr->lcp_info.want_vals[FSM_LOCAL].authentication 
                                                                       = PPP_PAP_PROTOCOL;
         }
         else
         {
           DS_MSG0(MSG_LEGACY_HIGH,"Using CHAP protocol");
         }
       }

      if ( ppp_start( ppp_dev, rmsmi_info_ptr->rm_ppp_config_ptr ) < 0 )
      {
        DS_MSG0(MSG_LEGACY_ERROR,"RM PPP config failed, posting RM_PPP_DOWN");
        ds_3gpp_rmsm_ip_common_post_event( &inst , DS_CMD_3GPP_RMSM_RM_PPP_DOWN );
        break;
      }
  
      DS_MSG0(MSG_LEGACY_HIGH,"Changing PPP to internal SNOOP mode" ); 
      /*---------------------------------------------------------------------
        Change the PPP mode on Rm to internal SNOOP Mode 
      ---------------------------------------------------------------------*/
      ppp_set_mode( ppp_dev, PPP_INTERNAL_SNOOP_MODE );
  
      /*---------------------------------------------------------------------
        Register for PPP events (Up and Down events )
      ---------------------------------------------------------------------*/
      retval = ppp_event_register( rmsmi_info_ptr->ppp_handle,
                                   PPP_PROTOCOL_LCP, 
                                   PPP_DOWN_EV_MASK  );
      if (retval == -1)
      {
        DS_MSG0(MSG_LEGACY_ERROR,"Unable to register for LCP events"); 
        ds_3gpp_rmsm_ip_common_post_event( &inst , DS_CMD_3GPP_RMSM_RM_PPP_DOWN );
        break;
      }
  
      if (pdp_type == DS_PROFILE_PDP_IPV4 ||
          pdp_type == DS_PROFILE_PDP_IPV4V6 || 
          (pdp_type == DS_PROFILE_PDP_IPV6 && 
          ds_pdnmgr_is_prof_clat_enabled(rmsmi_info_ptr->profile_id)))
      {
        retval = ppp_event_register(
                    rmsmi_info_ptr->ppp_handle,
                    PPP_PROTOCOL_IPCP, 
                    (ppp_event_mask_e_type)(PPP_UP_EV_MASK | PPP_RESYNC_EV_MASK));               
        if (retval == -1)
        {
          DS_MSG0(MSG_LEGACY_ERROR,"Unable to register for IPCP events"); 
          ds_3gpp_rmsm_ip_common_post_event( &inst , DS_CMD_3GPP_RMSM_RM_PPP_DOWN );
          break;
        }
        
        /*---------------------------------------------------------------------
          Snoop for IPCP packets: Register with ps snoop registry
        ---------------------------------------------------------------------*/
        rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->v4_info.ipcp_snoop_tag
          = snoop_reg_event(ppp_dev, PPP_IPCP_PROTOCOL,
                            CONFIG_REQ, ds_3gpp_rmsm_ip_common_rm_ipcp_cfg_req_cb);
      }
      
      if (pdp_type == DS_PROFILE_PDP_IPV6 ||
          pdp_type == DS_PROFILE_PDP_IPV4V6)
      { 
      
        retval = ppp_event_register(
                    rmsmi_info_ptr->ppp_handle,
                    PPP_PROTOCOL_IPV6CP, 
                   (ppp_event_mask_e_type) (PPP_UP_EV_MASK | PPP_RESYNC_EV_MASK));
        if (retval == -1)
        {
          DS_MSG0(MSG_LEGACY_ERROR,"Unable to register for IPV6CP events"); 
          ds_3gpp_rmsm_ip_common_post_event( &inst , DS_CMD_3GPP_RMSM_RM_PPP_DOWN );
          break;
        }
      
        /*---------------------------------------------------------------------
           Snoop for IPV6CP packets: Register with ps snoop registry
        ---------------------------------------------------------------------*/
   	     rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->v6_info.ipv6cp_snoop_tag 
          = snoop_reg_event(ppp_dev, PPP_IPV6CP_PROTOCOL,
                            CONFIG_REQ, ds_3gpp_rmsm_ip_common_rm_ipv6cp_cfg_req_cb);
      }

      /*--------------------------------------------------------------------- 
        Send connect response now and immediately change to packet mode; on
        receiving connect TE can start sending PPP frames and we need to be
        ready to receive them
      ---------------------------------------------------------------------*/
#ifdef FEATURE_DATA_ATCOP
     dsat_report_rate_on_connect( connect_rate );
  
     dsat_send_result_code_ex( DSAT_CONNECT,rmsmi_info_ptr->port_id );
#endif /* FEATURE_DATA_ATCOP */

     (void)ds3g_siolib_ex_set_cd_state
             (DS3G_SIOLIB_REMOTE_CARRIER_ON,rmsmi_info_ptr->port_id);
  
      /*---------------------------------------------------------------------
        Rm configured : Set Rm to PKT mode & send result code to kick start
        PPP from the TE 
      ---------------------------------------------------------------------*/
     (void)ds3g_siolib_ex_change_mode(  SIO_DS_PKT_MODE,
                                        &rmsmi_info_ptr->sio_rx_wmk,
                                        &rmsmi_info_ptr->sio_tx_wmk,
                                        NULL,
                                        rmsmi_info_ptr->port_id  );
  
      SET_RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr);    
    }
    break;
    
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE:
      SET_RX_DATA_CAN_BE_PULLED(rmsmi_info_ptr);
      break;

    case DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_CLEANUP_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE:
      break;

    default:
      DS_MSG1(MSG_LEGACY_ERROR,"Cannot recognize the state %d", 
                         rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
      err_val = DS_LOCAL_ERR_VAL_4;
      break;
  }
  
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ip_common_transition_state :"
                            "local err %d",err_val);
  }
  return;
} /* ds_3gpp_rmsm_ip_common_transition_state */

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_PPP_UP_DELAY_TIMER_CMD_HDLR

 DESCRIPTION
   
   This function will be called when the PPP_UP_DELAY_TIMER expires

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_ppp_up_delay_timer_cmd_hdlr
( 
  ds_rmsm_instance_type* inst_ptr
)
{
  ds_rmsmi_sm_info_type  *sm_info_ptr = NULL;
  ds_rmsmi_info_type     *rmsmi_info_ptr = NULL;  
  ppp_dev_enum_type           ppp_dev;
  ds_3gpp_rmsm_ip_common_info_type* info_ptr = NULL;
  dsm_item_type                    **ppp_pkt_ptr = NULL;
  rex_crit_sect_type                *crit_sec_ptr = NULL;
  uint16                             proto = 0;
  
  if (inst_ptr == NULL) 
  {
    DATA_ERR_FATAL("inst_ptr = NULL");
    return;
  }
  
  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);
  
  if (rmsmi_info_ptr == NULL)
  { 
    DS_MSG0(MSG_LEGACY_HIGH,"RmSmi Info ptr is NULL !");
    return;
  }
  
  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  if (info_ptr == NULL)
  {
    return;
  }
  if (DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) == DS_PROFILE_PDP_IPV4V6)
  {
     if(IPCP_CREQ_RECD(info_ptr) && !IPV6CP_CREQ_RECD(info_ptr))
     {
       inst_ptr->sm_instance = DS_RMSM_V4_SM_INSTANCE;
     }
     else if(!IPCP_CREQ_RECD(info_ptr) && IPV6CP_CREQ_RECD(info_ptr))
     {
       inst_ptr->sm_instance = DS_RMSM_V6_SM_INSTANCE;
     }
     else
     {
       DS_MSG0(MSG_LEGACY_LOW,"Both config Packets Received!");
       return;
     }  
  }
  else
  {
    DS_MSG0(MSG_LEGACY_LOW,"Wait Delay Timer triggered for Incorrect PDP type!");
    return;
  }
  sm_info_ptr = ds_rmsmi_get_sm_info_ptr(inst_ptr);

  if (sm_info_ptr == NULL)
  { 
    DS_MSG0(MSG_LEGACY_LOW,"Sm Info ptr is NULL !");
    return;
  }
  ppp_dev = ds_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);
  
  if (ds_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
  {
    return;
  }

  if(V6_SM_INSTANCE(inst_ptr->sm_instance))
  {
    ppp_set_mode( ppp_dev, PPP_INTERNAL_MODE );

    crit_sec_ptr = &(info_ptr->v6_info.ipi_sm_info_crit_sec);
    proto = PPP_IPV6CP_PROTOCOL;

    DS_RMSM_ENTER_CRIT_SECTION( crit_sec_ptr );
    ppp_pkt_ptr = &info_ptr->v6_info.ipv6cp_cfg_pkt_ptr;
    if( *ppp_pkt_ptr != NULL &&
       !ppp_inject_packet(ppp_dev, proto, *ppp_pkt_ptr) )
    {
      *ppp_pkt_ptr= NULL;
      DS_RMSM_LEAVE_CRIT_SECTION( crit_sec_ptr );
      DS_MSG0(MSG_LEGACY_HIGH,"ppp pkt injected ");
      ds_3gpp_rmsm_ip_common_post_event( inst_ptr , DS_CMD_3GPP_RMSM_RM_PPP_CONFIGURED );
    }
    else
    {
      DS_RMSM_LEAVE_CRIT_SECTION( crit_sec_ptr );
      DS_MSG0(MSG_LEGACY_HIGH,"ppp pkt error ");
    }
  }
  else if (V4_SM_INSTANCE(inst_ptr->sm_instance))
  {
    ppp_set_mode( ppp_dev, PPP_INTERNAL_MODE );

    crit_sec_ptr = &(info_ptr->v4_info.ipi_sm_info_crit_sec);
    proto = PPP_IPCP_PROTOCOL;
  
    DS_RMSM_ENTER_CRIT_SECTION( crit_sec_ptr );
    ppp_pkt_ptr = &info_ptr->v4_info.ipcp_cfg_pkt_ptr;
    if( *ppp_pkt_ptr != NULL &&
       !ppp_inject_packet(ppp_dev, proto, *ppp_pkt_ptr) )
    {
      *ppp_pkt_ptr = NULL;
      DS_RMSM_LEAVE_CRIT_SECTION( crit_sec_ptr );
      DS_MSG0(MSG_LEGACY_HIGH,"ppp pkt injected ");
      ds_3gpp_rmsm_ip_common_post_event( inst_ptr , DS_CMD_3GPP_RMSM_RM_PPP_CONFIGURED );
    }
    else
    {
      DS_RMSM_LEAVE_CRIT_SECTION( crit_sec_ptr );
      DS_MSG0(MSG_LEGACY_HIGH,"ppp pkt error ");
    }
 }
 return;
} /* ds_3gpp_rmsm_ip_common_ppp_up_delay_timer_cmd_hdlr */

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_CONFIG_WAIT_TIMER_CMD_HDLR

 DESCRIPTION
   
   This function will be called when the PPP_UP_DELAY_TIMER expires

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
void ds_3gpp_rmsm_ip_common_config_wait_timer_cmd_hdlr
( 
  ds_rmsm_instance_type* inst_ptr
)
{
  ds_rmsmi_info_type                *rmsmi_info_ptr = NULL;
  ppp_dev_enum_type                 ppp_dev;
  ds_local_err_val_e_type           err_val  = DS_LOCAL_ERR_VAL_NONE;
  ds_3gpp_rmsm_ip_common_info_type  *info_ptr = NULL;
  /*----------------------------------------------------------------------*/

  DS_MSG0(MSG_LEGACY_HIGH,"Config wait timer expired. Bring down the DUN call");
  if(inst_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }
  
  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);
  if (rmsmi_info_ptr == NULL)
  { 
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  
  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  if(info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit; 
  }
  
  ppp_dev = ds_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);
  if (ds_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
  {
    err_val = DS_LOCAL_ERR_VAL_3;
    goto func_exit; 
  }
  
  ds_3gpp_rmsm_ip_common_reset_ipcp_creq_recd(info_ptr);
  ds_3gpp_rmsm_ip_common_reset_ipv6cp_creq_recd(info_ptr);
  
  if( !ppp_stop(ppp_dev, PPP_PROTOCOL_LCP) )
  {
    DS_MSG0(MSG_LEGACY_HIGH,"Stopping PPP ");
    ds_3gpp_rmsm_ip_common_transition_state(inst_ptr->rm_dev_instance, 
                      DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE);
  }
  else
  {
    DS_MSG0(MSG_LEGACY_HIGH,"PPP stop failed. Transition to NULL state");
    ds_3gpp_rmsm_ip_common_transition_state( inst_ptr->rm_dev_instance,
                                     DS_3GPP_RMSM_IP_COMMON_NULL_STATE );
  }

  ds_rmsm_ip_process_pdn_down_ev(rmsmi_info_ptr->sm_info_ptr[DS_RMSM_V4_SM_INSTANCE]);
  ds_rmsm_ip_process_pdn_down_ev(rmsmi_info_ptr->sm_info_ptr[DS_RMSM_V6_SM_INSTANCE]);

func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"ds_3gpp_rmsm_ip_common_config_wait_timer_cmd_hdlr err_val: %d",
                           err_val);
  }
  return;
}/*ds_3gpp_rmsm_ip_common_config_wait_timer_cmd_hdlr*/
/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          GLOBAL FUNCTION DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_INIT

  DESCRIPTION
    This function initializes the RmSm IP Common Data Structure
    
  PARAMETERS
    Rm Device Instance
    
  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_3gpp_rmsm_ip_common_init
(
  ds_rmsm_rm_dev_instance_type rm_dev_instance
)
{
  ds_rmsmi_info_type           *rmsmi_info_ptr = NULL;

  ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  DS_MSG1(MSG_LEGACY_HIGH,"Initializing RmSm IP Common, "
                   "rm_dev_instance: %d", rm_dev_instance);  

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(rm_dev_instance);

  if (rmsmi_info_ptr == NULL)
  { 
    DATA_ERR_FATAL("RmSmi Info ptr is NULL !");
    return FALSE;
  }
  
  DS_SYSTEM_HEAP_MEM_ALLOC(info_ptr, sizeof(ds_3gpp_rmsm_ip_common_info_type), 
                                     ds_3gpp_rmsm_ip_common_info_type* );

  if(info_ptr == NULL)
  {
    ERR_FATAL("MEM ALLOC Failure: RmSm ip common Info ptr is NULL during init",0,0,0);
    return FALSE;
  }

  rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr = info_ptr;

  /*-------------------------------------------------------------------------
    COMMON Initialization
  -------------------------------------------------------------------------*/

  info_ptr->state = DS_3GPP_RMSM_IP_COMMON_NULL_STATE;

  memset(&info_ptr->ppp_info, 0x0, sizeof(info_ptr->ppp_info));

  rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->configure_ppp_f_ptr = 
     ds_3gpp_rmsm_ip_common_configure_ppp_cb;

  /*-------------------------------------------------------------------------
    V4 Initialization
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Initialize the rex critical section for ip info sm structure.
  -------------------------------------------------------------------------*/
  memset(&(info_ptr->v4_info.ipi_sm_info_crit_sec),
           0,
           sizeof(rex_crit_sect_type));
  DS_RMSM_INIT_CRIT_SECTION( &(info_ptr->v4_info.ipi_sm_info_crit_sec) );


  info_ptr->v4_info.ipcp_cfg_pkt_ptr = NULL;
  info_ptr->v4_info.ipcp_snoop_tag   = 0;
  info_ptr->v4_info.send_ipcp_naks_flg = FALSE;
  info_ptr->v4_info.ipcp_res_del_timer = 
  ps_timer_alloc(ds_3gpp_rmsm_ip_common_ipcp_rsp_timer_cb,(void*)rmsmi_info_ptr);
  
  ds_3gpp_rmsm_ip_common_reset_ipcp_creq_recd(info_ptr);
  /*-------------------------------------------------------------------------
    V6 Initialization
  -------------------------------------------------------------------------*/
  memset(&(info_ptr->v6_info.ipi_sm_info_crit_sec),0,sizeof(rex_crit_sect_type));
  DS_RMSM_INIT_CRIT_SECTION( &(info_ptr->v6_info.ipi_sm_info_crit_sec) );

  info_ptr->v6_info.ipv6cp_cfg_pkt_ptr = NULL;
  info_ptr->v6_info.ipv6cp_snoop_tag   = 0;
  info_ptr->v6_info.send_ipv6cp_naks_flg = FALSE;                     
  info_ptr->v6_info.ipv6cp_res_del_timer = 
  	        ps_timer_alloc(ds_3gpp_rmsm_ip_common_ipv6cp_rsp_timer_cb,(void*)rmsmi_info_ptr);
  
  ds_3gpp_rmsm_ip_common_reset_ipv6cp_creq_recd(info_ptr);

  return TRUE;
} /* ds_3gpp_rmsm_ip_common_init */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_RM_WANTS_PKT_CALL_EV

  DESCRIPTION
    This function processes RM_WANTS_PKT_CALL_EV event. 
    
  PARAMETERS
    Instance ptr

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_rm_wants_pkt_call_ev
(
  ds_rmsm_instance_type* inst_ptr
)
{
  ds_rmsmi_info_type      *rmsmi_info_ptr = NULL;
  ds_local_err_val_e_type err_val = DS_LOCAL_ERR_VAL_NONE;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (inst_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);
  if (rmsmi_info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  DS_MSG3(MSG_LEGACY_HIGH,"Handle Rm Wants pkt call EV, "
                   "rm_dev_instance: %d, sm_instance: %d, state: %d ", 
                   inst_ptr->rm_dev_instance, 
                   inst_ptr->sm_instance, 
                   rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);

  switch( rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state )
  {
    case DS_3GPP_RMSM_IP_COMMON_NULL_STATE:
      ds_3gpp_rmsm_ip_common_transition_state(inst_ptr->rm_dev_instance, 
                      DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE );
      break;
      
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE:
    case DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_CLEANUP_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE:
    default:
      DS_MSG1(MSG_LEGACY_ERROR, "Invalid RM_WANTS_PKT_CALL_EV state:%d", 
           rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
      break;
  }

func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ip_common_process_rm_wants_pkt_call_ev"
                             "local err %d ",err_val);
  return;
  
} /* ds_3gpp_rmsm_ip_common_process_rm_wants_pkt_call_ev */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_RM_PPP_CONFIGURED_EV

  DESCRIPTION
    This function processes RM_WANTS_PKT_CALL_EV event. 
    
  PARAMETERS
    Instance ptr

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_rm_ppp_configured_ev
(
  ds_rmsm_instance_type* inst_ptr
)
{

  ds_rmsmi_info_type* rmsmi_info_ptr = NULL;
  ds_local_err_val_e_type err_val = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (inst_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  DS_MSG2(MSG_LEGACY_HIGH,"PPP configured event, move to rm configured state ,"
                          "rm_dev_instance: %d, sm_instance: %d ", 
                   inst_ptr->rm_dev_instance, inst_ptr->sm_instance);

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);
  if (rmsmi_info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  DS_MSG3(MSG_LEGACY_HIGH,"Handle Rm PPP Configured EV, "
                   "rm_dev_instance: %d, sm_instance: %d, state: %d ", 
                   inst_ptr->rm_dev_instance, 
                   inst_ptr->sm_instance, 
                   rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
  switch( rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state )
  {
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE:
      ds_3gpp_rmsm_ip_common_transition_state
      ( 
        inst_ptr->rm_dev_instance, 
        DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE 
      );
      break;

    case DS_3GPP_RMSM_IP_COMMON_NULL_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE:
    case DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_CLEANUP_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE:
      /*---------------------------------------------------------------------
        It's error to get this event in all other states.
      ---------------------------------------------------------------------*/
      DS_MSG1(MSG_LEGACY_ERROR, "Unexpected RM_WANTS_PKT_CALL_EV state:%d", 
        rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);

      break;

    default:
      DS_MSG1(MSG_LEGACY_ERROR, "Invalid RM_WANTS_PKT_CALL_EV state:%d", 
                      rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
      break;
  } 
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR, "ds_3gpp_rmsm_ip_common_process_rm_ppp_configured_ev err_val:%d", 
                      err_val);
  return;
} /*ds_3gpp_rmsm_ip_common_process_rm_ppp_configured_ev*/

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_RM_PPP_DOWN_EV

  DESCRIPTION
    This function processes RM_PPP_DOWN_EV event. 
    
  PARAMETERS
    Instance pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_rm_ppp_down_ev
(
  ds_rmsm_instance_type* inst_ptr
)
{
  ds_rmsmi_info_type* rmsmi_info_ptr = NULL;
  ds_rmsmi_sm_info_type* sm_info_ptr = NULL;
  ds_rmsm_sm_instance_type sm_inst;
  ds_rmsm_rm_dev_instance_type rm_dev_inst;
  ds_rmsm_instance_type        v4_inst;
  ds_rmsm_instance_type        v6_inst;
  ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;
  ds_profile_pdp_type_e           pdp_type;
  ds_local_err_val_e_type  err_val = DS_LOCAL_ERR_VAL_NONE;
  /*----------------------------------------*/

  DS_MSG2(MSG_LEGACY_HIGH,"Handle PPP Down, rm_dev_instance: %d, sm_instance: %d ",
                    inst_ptr->rm_dev_instance, inst_ptr->sm_instance);

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL)
  { 
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  if(info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  sm_inst = inst_ptr->sm_instance;
  rm_dev_inst = inst_ptr->rm_dev_instance;

  DS_MSG3(MSG_LEGACY_HIGH,"Handle Rm PPP Down EV, "
                   "rm_dev_instance: %d, sm_instance: %d, state: %d ", 
                   rm_dev_inst, sm_inst, 
                   rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);

  if ((V4_SM_INSTANCE(sm_inst)) || (ANY_SM_INSTANCE(sm_inst)))
  { 
    ds_3gpp_rmsm_ip_common_reset_ipcp_creq_recd(info_ptr);
  }

  if ((V6_SM_INSTANCE(sm_inst)) || (ANY_SM_INSTANCE(sm_inst)))
  {
    ds_3gpp_rmsm_ip_common_reset_ipv6cp_creq_recd(info_ptr);
  }

  pdp_type = DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr);

  switch (rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state)
  {
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE:
     /*Fall through*/

    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE:
      ds_3gpp_rmsm_ip_common_transition_state(rmsmi_info_ptr->instance, 
                                             DS_3GPP_RMSM_IP_COMMON_NULL_STATE);
      break;

    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE:
      ds_3gpp_rmsm_ip_common_clear_nak_timer(rmsmi_info_ptr);
      //Fallthrough

    case DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE:
    {
      if ((V4_SM_INSTANCE(sm_inst)) || (ANY_SM_INSTANCE(sm_inst)))
      {
        v4_inst.rm_dev_instance = inst_ptr->rm_dev_instance;
        v4_inst.sm_instance = DS_RMSM_V4_SM_INSTANCE;

        ds_3gpp_rmsm_ip_common_clear_nak_timer(rmsmi_info_ptr);
        sm_info_ptr = ds_rmsmi_get_sm_info_ptr(&v4_inst);
        ds_rmsm_ip_process_rm_ppp_down_ev(sm_info_ptr);
      }
	  
      if ((V6_SM_INSTANCE(sm_inst)) || (ANY_SM_INSTANCE(sm_inst)))
      {
        v6_inst.rm_dev_instance = inst_ptr->rm_dev_instance;
        v6_inst.sm_instance = DS_RMSM_V6_SM_INSTANCE;

        ds_3gpp_rmsm_ip_common_clear_nak_timer(rmsmi_info_ptr);
        sm_info_ptr = ds_rmsmi_get_sm_info_ptr(&v6_inst);
        ds_rmsm_ip_process_rm_ppp_down_ev(sm_info_ptr);
      }


      if ( !IPCP_CREQ_RECD(info_ptr) && !IPV6CP_CREQ_RECD(info_ptr) )
      {
        ds_3gpp_rmsm_ip_common_transition_state(rmsmi_info_ptr->instance,
                             DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_CLEANUP_STATE);
      }

      break;
    }
    default:
     DS_MSG1(MSG_LEGACY_ERROR, "PPP Down received in invalid state %d", 
                         rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
     break;
  }
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ip_common_process_rm_ppp_down_ev: err_val %d ",
                              err_val);
  return;
} /* ds_3gpp_rmsm_ip_common_process_rm_ppp_down_ev */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_RM_PPP_UP_EV

  DESCRIPTION
    This function processes RM_PPP_UP_EV event. 
    
  PARAMETERS
    Instance pointer

  RETURN VALUE
    None

  DEPENDENCIES
    none

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void ds_3gpp_rmsm_ip_common_process_rm_ppp_up_ev
(
  ds_rmsm_instance_type* inst_ptr
)
{
  ds_rmsmi_info_type* rmsmi_info_ptr = NULL;
  ds_rmsmi_sm_info_type* sm_info_ptr = NULL;
  ds_3gpp_rmsm_ip_common_info_type *info_ptr = NULL;

  /*----------------------------------------*/

  DS_MSG2(MSG_LEGACY_HIGH,"Handle PPP Up, rm_dev_instance: %d, sm_instance: %d ",
                    inst_ptr->rm_dev_instance, inst_ptr->sm_instance);

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL)
  { 
    DATA_ERR_FATAL("RmSmi Info ptr is NULL !");
  }

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  if(info_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_LOW,"Info ptr is NULL !");
    return;
  }

  sm_info_ptr = ds_rmsmi_get_sm_info_ptr(inst_ptr);

  if (sm_info_ptr == NULL)
  { 
    DATA_ERR_FATAL("Sm Info ptr is NULL !");
  }

  switch (rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state)
  {
    case DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE:
    { 
      ds_rmsm_ip_process_rm_ppp_up_ev(sm_info_ptr);
      break;
    }
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE:
    {
      DS_MSG2(MSG_LEGACY_HIGH,"Recieving PPP up in Invalid state, rm_dev_instance: %d, sm_instance: %d",
                       inst_ptr->rm_dev_instance,
                       inst_ptr->sm_instance);
      if ((DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) == DS_PROFILE_PDP_IPV4V6) && 
           ds_pdn_nv_manager_get_allow_single_IP_call_dualIP_pf())
      {
        /* -------------------------------------------------------------------------------
           For a Dual IP profile type, if PPP UP is received while the state is still 
           in DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE due to either one 
           of Creq pkt not received,a one second timer is started and on its expiry, PPP
           is brought up for the IP type for which Creq was received.
         ------------------------------------------------------------------------------- */
        
        if((IPCP_CREQ_RECD(info_ptr) && !IPV6CP_CREQ_RECD(info_ptr)) ||
          (!IPCP_CREQ_RECD(info_ptr) && IPV6CP_CREQ_RECD(info_ptr)))
        {
          DS_MSG1(MSG_LEGACY_HIGH,"PPP UP delay timer started with duration %d",
                       ds_pdn_nv_manager_get_ppp_wait_timer());
          ps_timer_start(rmsmi_info_ptr->ppp_up_delay_timer, (int64)ds_pdn_nv_manager_get_ppp_wait_timer());
        }
      }
    }

    default:
      DS_MSG1(MSG_LEGACY_ERROR, "PPP UP received in invalid state %d", 
        rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
      break;

  }
} /* ds_3gpp_rmsm_ip_common_process_rm_ppp_up_ev */


/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_HANDLE_RM_BRING_UP_UM_IFACE_EV
  
  DESCRIPTION
  This function processes DS_CMD_3GPP_RMSM_RM_BRING_UP_PDN event. 
  
  PARAMETERS
  instance ptr
  
  RETURN VALUE
  None
  
  DEPENDENCIES
  none
  
  SIDE EFFECTS
  None
 ===========================================================================*/
void ds_3gpp_rmsm_ip_common_process_rm_bring_up_pdn_ev 
(
  ds_rmsm_instance_type* inst_ptr
)
{ 
  ds_rmsmi_info_type                  *rmsmi_info_ptr = NULL;
  ds_rmsmi_sm_info_type               *sm_ptr = NULL;
  ds_profile_pdp_type_e                pdp_type;
  ds_3gpp_rmsm_ip_common_info_type    *info_ptr = NULL;
  ds_local_err_val_e_type             err_val = DS_LOCAL_ERR_VAL_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (inst_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);

  if (rmsmi_info_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  if(info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  sm_ptr = ds_rmsmi_get_sm_info_ptr(inst_ptr);
  if (sm_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_3;
    goto func_exit;
  }

  sm_ptr->instance_hdl = ds_rmsm_alloc_client_handle(inst_ptr);

  pdp_type = DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr);

  DS_MSG3(MSG_LEGACY_HIGH, "Recvd Rm Bring Up Um PDN EV, "
                     "Rm instance: %d, SM instance: %d, state:%d", 
                     rmsmi_info_ptr->instance,
                     sm_ptr->instance,
                     rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);

  switch( rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state )
  {

    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE:
    {
      ds_rmsm_ip_process_bring_up_pdn_ev(sm_ptr);
      
      /*Update RM config ptr with fw instance*/
      if(sm_ptr->instance == DS_RMSM_V4_SM_INSTANCE)
      {
        rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.fwk_inst_ptr = sm_ptr->fw_inst;
      }
      else if(sm_ptr->instance == DS_RMSM_V6_SM_INSTANCE)
      {  
        rmsmi_info_ptr->rm_ppp_config_ptr->ipv6cp_info.fwk_inst_ptr = sm_ptr->fw_inst;
      }
      else
      {
        DS_MSG0(MSG_LEGACY_HIGH, "Invalid SM instance type");
      }
      
      ds_3gpp_rmsm_ip_common_transition_state 
      ( 
        rmsmi_info_ptr->instance, 
        DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE
      );
      break;
    }
    //tbd_aks: Remove code duplication
    case DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE:
    {
      ds_rmsm_ip_process_bring_up_pdn_ev(sm_ptr);
      /*Update RM config ptr with fw instance*/
      if(sm_ptr->instance == DS_RMSM_V4_SM_INSTANCE)
      {
        rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.fwk_inst_ptr = sm_ptr->fw_inst;
      }
      else if(sm_ptr->instance == DS_RMSM_V6_SM_INSTANCE)
      {  
        rmsmi_info_ptr->rm_ppp_config_ptr->ipv6cp_info.fwk_inst_ptr = sm_ptr->fw_inst;
      }
      else
      {
        DS_MSG0(MSG_LEGACY_HIGH, "Invalid SM instance type");
      }
      break;
    }

    case DS_3GPP_RMSM_IP_COMMON_NULL_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_CLEANUP_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE:
       /*---------------------------------------------------------------------
        Ignore event in all other states.
        ---------------------------------------------------------------------*/
       DS_MSG2(MSG_LEGACY_HIGH, "Ignoring Rm Bring Up Um PDN Ev Rm Instance :%d state:%d",
                rmsmi_info_ptr->instance, rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
       break;

    default:
      /*---------------------------------------------------------------------
       Invalid state
       ---------------------------------------------------------------------*/
      DS_MSG1(MSG_LEGACY_ERROR, "Invalid RMSM state %d",
          rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
      break;

  } /* switch( sm_ptr->pdp_specific_info.pdp_ip.state */
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ip_common_process_rm_bring_up_pdn_ev :"
                            "local err %d",err_val);
  }
  return;
    
}

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_POST_EVENT()

  DESCRIPTION
    This function receives an event posted to PDP-IP type RMSM and enqueues the
    corresponding command to be processed in the DS task.

  PARAMETERS
 	  inst_ptr: Pointer to the Rm Dev Instance and SM Instance
      ds_command_enum_type: The command enum that is being posted to the DS task

  RETURN VALUE
    None

  DEPENDENCIES
    ds_rmsm_ip_sm_init() should have been called first.
    The top level state machine post function checks if we the state machine
    is initialized in the first place

  SIDE EFFECTS
    None
===========================================================================*/
void ds_3gpp_rmsm_ip_common_post_event
(
  ds_rmsm_instance_type* inst_ptr,
  ds_command_enum_type       cmd_id
)
{
  ds_command_type         *cmd_ptr = NULL;  /* DS Task cmd buf        */
  ds_rmsm_instance_type   *data_ptr = NULL;
  ds_local_err_val_e_type err_val = DS_LOCAL_ERR_VAL_NONE;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (inst_ptr == NULL) 
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_rmsm_instance_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  cmd_ptr->hdr.cmd_id = cmd_id;
  data_ptr = (ds_rmsm_instance_type*)cmd_ptr->cmd_payload_ptr;

  data_ptr->rm_dev_instance = (int)inst_ptr->rm_dev_instance;
  data_ptr->sm_instance = (int)inst_ptr->sm_instance;

  ds_put_cmd( cmd_ptr );
  
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ip_common_post_event : err %d",
                              err_val);
  return;
} /* ds_3gpp_rmsm_ip_common_post_event() */

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to DS_3GPP_RMSM (for pdp-ip calls)
    in DS task and calls an appropriate function to handle it

  PARAMETERS

  RETURN VALUE
    none

  DEPENDENCIES
    none  
    
  SIDE EFFECTS
    None
===========================================================================*/
/*lint -e818*/
void ds_3gpp_rmsm_ip_common_process_cmd
(
  ds_command_type *ds_cmd_ptr
)
{
  ds_rmsm_instance_type          *inst_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( (ds_cmd_ptr == NULL) || (ds_cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_ERR_FATAL("NULL cmd_ptr or null payload ptr");
    return;
  }

  inst_ptr = (ds_rmsm_instance_type*)ds_cmd_ptr->cmd_payload_ptr;
    
  /*-------------------------------------------------------------------------
    Take action based on event received and which state we are currently
    in.
  -------------------------------------------------------------------------*/
  switch(ds_cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_3GPP_RMSM_RM_WANTS_PKT_CALL:
      ds_3gpp_rmsm_ip_common_process_rm_wants_pkt_call_ev(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_RM_PPP_CONFIGURED:
      ds_3gpp_rmsm_ip_common_process_rm_ppp_configured_ev(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_RM_PPP_UP:
      ds_3gpp_rmsm_ip_common_process_rm_ppp_up_ev(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_RM_PPP_DOWN:
      ds_3gpp_rmsm_ip_common_process_rm_ppp_down_ev(inst_ptr);
      break;
    
    case DS_CMD_3GPP_RMSM_RM_BRING_UP_PDN:
      ds_3gpp_rmsm_ip_common_process_rm_bring_up_pdn_ev (inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_UM_PDN_UP:
      ds_3gpp_rmsm_ip_common_process_um_pdn_up_ev(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_UM_PDN_DOWN:
      ds_3gpp_rmsm_ip_common_process_um_pdn_down_ev(inst_ptr);
    break;

    case DS_CMD_3GPP_RMSM_RM_IPCP_CREQ_RECD:
      ds_3gpp_rmsm_ip_common_ipcp_rsp_timer_cmd_hdlr(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_RM_IPV6CP_CREQ_RECD:
      ds_3gpp_rmsm_ip_common_ipv6cp_rsp_timer_cmd_hdlr(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_PPP_UP_DELAY_TIMER:
      ds_3gpp_rmsm_ip_common_ppp_up_delay_timer_cmd_hdlr(inst_ptr);
      break;
    
    case DS_CMD_3GPP_RMSM_CONFIG_WAIT_TIMER:
      ds_3gpp_rmsm_ip_common_config_wait_timer_cmd_hdlr(inst_ptr);
	    break;
	  
    case DS_CMD_3GPP_RMSM_IPCP_RSP_TIMER:
      ds_3gpp_rmsm_ip_common_ipcp_rsp_timer_cmd_hdlr(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_IPV6CP_RSP_TIMER:
      ds_3gpp_rmsm_ip_common_ipv6cp_rsp_timer_cmd_hdlr(inst_ptr);
      break;

    case DS_CMD_3GPP_RMSM_ADDR_CFG_COMPLETE:
    case DS_CMD_3GPP_RMSM_ADDR_CFG_END:
    case DS_CMD_3GPP_RMSM_ADDR_CFG_FAIL:
    case DS_CMD_3GPP_RMSM_ADDR_CFG_CHANGED:
      ds_rmsm_ip_process_cmd(inst_ptr, ds_cmd_ptr->hdr.cmd_id);
    break;

    case DS_CMD_3GPP_RMSM_AUTOCONFIG_SM:
      ds_3gpp_rmsm_autoconfig_sm_process_ev_cmd(ds_cmd_ptr);
      break;
    case DS_CMD_3GPP_RMSM_AT_PDN_EV:
    case DS_CMD_3GPP_RMSM_AT_SEND_DATA:
      ds_3gpp_rmsm_at_process_cmd(ds_cmd_ptr);
      break;
    
    case DS_CMD_SIOLIB_RDM_OPEN:
    case DS_CMD_SIOLIB_RDM_CLOSE:
    case DS_CMD_SIOLIB_CLOSE_COMPLETE:
    case DS_CMD_SIOLIB_DTR_CHANGED:
#ifdef FEATURE_DSAT_MDM_FUSION
    case DS_CMD_SIOLIB_CD_CHANGED:
#endif
    case DS_CMD_SIOLIB_DSM_MEM_CTRL:
    case DS_CMD_SIOLIB_ENTER_ONLINE_CMD_TX_FLUSH:
    case DS_CMD_SIOLIB_COMPLETE_ONLINE_CMD_SWITCH:  /* Rx'd after a TX flush completes */
    case DS_CMD_SIOLIB_ENTER_ONLINE_DATA_TX_FLUSH:
    case DS_CMD_SIOLIB_ENTER_ONLINE_DATA_NO_TX_FLUSH:/* Enter online-cmd w/no TX flush */
    case DS_CMD_SIOLIB_COMPLETE_ONLINE_DATA_SWITCH:  /* Rx'd after a TX flush completes */
    case DS_CMD_SIOLIB_TIMER_EXPIRED:
      /*Dispatch siolib cmd to be handled separately*/
      ds3g_siolib_process_cmds(ds_cmd_ptr);
      break;
    
    default:
      DS_MSG3(MSG_LEGACY_ERROR,"Invalid UMTS RMSM sm instance: %d, "
                         "rm dev instance: %d, cmd %d, ignoring", 
                          inst_ptr->sm_instance, 
                          inst_ptr->rm_dev_instance, ds_cmd_ptr->hdr.cmd_id);
  } /* switch( cmd ) */
  return;
} /* ds_3gpp_rmsm_ip_common_process_cmd() */
/*lint +e818*/

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_IS_CALL_TEARABLE
  DESCRIPTION
    This function checks if the call is tearable.

  PARAMETERS
    inst: Rmsm Info type

  RETURN VALUE
    Boolean

  DEPENDENCIES
    none  
    
  SIDE EFFECTS
    None
===========================================================================*/

LOCAL boolean ds_3gpp_rmsm_ip_common_is_call_tearable
(
  ds_rmsmi_info_type* rmsmi_info_ptr
)
{
  ds_rmsmi_sm_info_type* sm_ptr = NULL;
  ds_profile_pdp_type_e pdp_type;
  ds_rmsm_instance_type inst;
  boolean ret1 = TRUE;
  boolean ret2 = TRUE;

  if (rmsmi_info_ptr == NULL)
  {
    DS_MSG3(MSG_LEGACY_ERROR,"Invalid RmSm info ptr, returning false",0,0,0);
    return FALSE;
  }

  inst.rm_dev_instance = rmsmi_info_ptr->instance;
  
  pdp_type = DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr);

  if (pdp_type == DS_PROFILE_PDP_IPV4 ||
      pdp_type == DS_PROFILE_PDP_IPV4V6)
  {  
    inst.sm_instance = DS_RMSM_V4_SM_INSTANCE;
    sm_ptr = ds_rmsmi_get_sm_info_ptr(&inst);
    ASSERT(sm_ptr);

    if (ds_rmsm_ip_get_state(sm_ptr) == DS_RMSM_IP_NULL_STATE)
    {
      ret1 = TRUE;
    }
    else
    {
      ret1 = FALSE;
    }
  }

  if (pdp_type == DS_PROFILE_PDP_IPV6 ||
      pdp_type == DS_PROFILE_PDP_IPV4V6)
  {
    inst.sm_instance = DS_RMSM_V6_SM_INSTANCE;
    sm_ptr = ds_rmsmi_get_sm_info_ptr(&inst);
    ASSERT(sm_ptr);

    if (ds_rmsm_ip_get_state(sm_ptr) == DS_RMSM_IP_NULL_STATE)
    {
      ret2 = TRUE;
    }
    else
    {
      ret2 = FALSE;
    }
  }

  return (ret1 && ret2);

}

/*===========================================================================
  FUNCTION DS_3GPP_RMSM_IP_COMMON_GET_STATE()

  DESCRIPTION
    This function retrieves the RMSM IP common state.

  PARAMETERS
    inst: Rm Dev Instance

  RETURN VALUE
    ds_3gpp_rmsm_ip_common_state_type

  DEPENDENCIES
    none  
    
  SIDE EFFECTS
    None
===========================================================================*/
ds_3gpp_rmsm_ip_common_state_type ds_3gpp_rmsm_ip_common_get_state
(
  ds_rmsm_rm_dev_instance_type  inst
)
{
  ds_rmsmi_info_type *rmsmi_info_ptr;
  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst);

  if (rmsmi_info_ptr == NULL)
  {
    DATA_ERR_FATAL("RmSmi info ptr is NULL");
    return 0;
  }

  return (rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
} /* ds_3gpp_rmsm_ip_common_get_state */

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_PPP_UP_DELAY_TIMER_CB

 DESCRIPTION
   
   This function will be called when the PPP_UP_DELAY_TIMER expires

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/
void ds_3gpp_rmsm_ip_common_ppp_up_delay_timer_cb
( 
  void* callback_data
)
{
  ds_command_type                *cmd_ptr;
  ds_rmsm_instance_type  *data_ptr = NULL;
  ds_rmsmi_info_type     *rmsmi_info_ptr = NULL;
  ds_local_err_val_e_type  err_val = DS_LOCAL_ERR_VAL_NONE;

  rmsmi_info_ptr = (ds_rmsmi_info_type*) callback_data;

  if (rmsmi_info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  DS_MSG0(MSG_LEGACY_HIGH,"PPP wait delay timer expired.");
  rmsmi_info_ptr->singleip_dual_pf_call = TRUE;
  
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_rmsm_instance_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_RMSM_PPP_UP_DELAY_TIMER;
    data_ptr = (ds_rmsm_instance_type*)cmd_ptr->cmd_payload_ptr;
    data_ptr->rm_dev_instance = rmsmi_info_ptr->instance;
    ds_put_cmd(cmd_ptr);
  }
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ip_common_process_um_pdn_up_ev: err_val %d ",
                              err_val);
  return;
} /* ds_3gpp_rmsm_ip_common_ppp_up_delay_timer_cb */

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_CONFIG_WAIT_TIMER_CB

 DESCRIPTION
   
   This function will be called when the config wait timer expires

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/

void ds_3gpp_rmsm_ip_common_config_wait_timer_cb
(
  void* callback_data
)
{
  ds_command_type        *cmd_ptr;
  ds_rmsm_instance_type  *data_ptr = NULL;
  ds_rmsmi_info_type     *rmsmi_info_ptr = NULL;
  ds_local_err_val_e_type  err_val = DS_LOCAL_ERR_VAL_NONE;

  rmsmi_info_ptr = (ds_rmsmi_info_type*) callback_data;

  if (rmsmi_info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  DS_MSG0(MSG_LEGACY_HIGH,"Config wait delay timer expired.");
  
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_rmsm_instance_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_RMSM_CONFIG_WAIT_TIMER;
    data_ptr = (ds_rmsm_instance_type*)cmd_ptr->cmd_payload_ptr;
    data_ptr->rm_dev_instance = rmsmi_info_ptr->instance;
    ds_put_cmd(cmd_ptr);
  }
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ip_common_config_wait_timer_cb: err_val %d ",
                              err_val);
  return;
}

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_PROCESS_UM_PDN_UP_EV

 DESCRIPTION

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/

void ds_3gpp_rmsm_ip_common_process_um_pdn_up_ev
(
 ds_rmsm_instance_type* inst_ptr
)
{
  ds_rmsmi_sm_info_type*   sm_info_ptr = NULL;
  ds_rmsmi_info_type*      rmsmi_info_ptr = NULL;
  ds_local_err_val_e_type  err_val = DS_LOCAL_ERR_VAL_NONE;

  /*-----------------------------------------------------------------------*/

  DS_MSG2(MSG_LEGACY_HIGH,"Handle PDN Up event, rm_dev_instance: %d, sm_instance: %d ",
                    inst_ptr->rm_dev_instance, inst_ptr->sm_instance);

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);
  if (rmsmi_info_ptr == NULL)
  { 
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  sm_info_ptr = ds_rmsmi_get_sm_info_ptr(inst_ptr);
  if (sm_info_ptr == NULL)
  { 
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  switch (rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state)
  {
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE:
    case DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE:
      ds_3gpp_rmsm_ip_common_clear_nak_timer(rmsmi_info_ptr);   
      ds_rmsm_ip_process_pdn_up_ev(sm_info_ptr); 
      break;
  
    case DS_3GPP_RMSM_IP_COMMON_NULL_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_CLEANUP_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE:
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      DS_MSG2(MSG_LEGACY_HIGH, "Ignoring UM_IFACE_UP_EV RmSm instance: %d state:%d",
                          rmsmi_info_ptr->instance, 
                          rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);

    default:
       DS_MSG0(MSG_LEGACY_ERROR,"Um Up received in incorrect state: %d ");
       break;
  
   }
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ip_common_process_um_pdn_up_ev: err_val %d ",
                              err_val);
  return;
}

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_COMMON_PROCESS_UM_PDN_DOWN_EV

 DESCRIPTION

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/

void ds_3gpp_rmsm_ip_common_process_um_pdn_down_ev
(
  ds_rmsm_instance_type* inst_ptr
)
{
  ds_rmsmi_sm_info_type*            sm_info_ptr = NULL;
  ds_rmsmi_info_type*               rmsmi_info_ptr = NULL;
  ds_rmsmi_sm_info_type             *companion_sm_info_ptr = NULL;
  ds_3gpp_rmsm_ip_common_info_type  *info_ptr = NULL;
  ppp_dev_enum_type                 ppp_dev;
  ppp_protocol_e_type               protocol = PPP_PROTOCOL_MAX;
  dsm_item_type                     **ppp_pkt_ptr = NULL;
  rex_crit_sect_type                *crit_sec_ptr = NULL;
  uint16                            proto = 0;
  ds_local_err_val_e_type           err_val = DS_LOCAL_ERR_VAL_NONE;

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);
  if (rmsmi_info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  if(info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit; 
  }

  if (inst_ptr->sm_instance >= DS_RMSM_MAX_SM_INSTANCES)
  { 
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit; 
  }

  sm_info_ptr = ds_rmsmi_get_sm_info_ptr(inst_ptr);
  if (sm_info_ptr == NULL)
  { 
    err_val = DS_LOCAL_ERR_VAL_3;
    goto func_exit; 
  }

  ppp_dev = ds_rmsmi_get_ppp_dev(rmsmi_info_ptr->instance);
  if (ds_rmsmi_validate_ppp_dev(ppp_dev) == FALSE)
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit; 
  }

  DS_MSG2(MSG_LEGACY_HIGH, "Processing UM PDN DOWN event sm inst :%d state:%d",
                         sm_info_ptr->instance,
                         rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
  
  switch (rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state)
  {
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE:
    case DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE:
    {

      companion_sm_info_ptr = ds_rmsmi_get_companion_sm_ptr(inst_ptr);

      if (V4_SM_INSTANCE(inst_ptr->sm_instance))
      {
        /*---------------------------------------------------------------------
          If PDN down recd for IPv4 and IPv6CP is received and 
          PDN down has not been recd for IPv6 instance then donot tear DUN call
        ---------------------------------------------------------------------*/
        if ( IPV6CP_CREQ_RECD(info_ptr) &&
             companion_sm_info_ptr != NULL &&
             companion_sm_info_ptr->recd_um_down == FALSE )
        {
          protocol = PPP_PROTOCOL_IPCP;
          ds_3gpp_rmsm_ip_common_clear_nak_timer(rmsmi_info_ptr);
          if(TRUE == ps_timer_is_running(rmsmi_info_ptr->ppp_up_delay_timer))
          {
            ps_timer_cancel(rmsmi_info_ptr->ppp_up_delay_timer);
          }

          sm_info_ptr->recd_um_down = TRUE;

          /*---------------------------------------------------------------------
            If PDP type is v4v6, we wait for both v4 and v6 config callbacks
            before completing RM PPP configuration. So, if an Instance 
            was torn down due to some reason, we should complete the 
            configuration for the companion Instance in case it is already UP.
          ---------------------------------------------------------------------*/
            DS_MSG0(MSG_LEGACY_HIGH,"Cleaning up V4 instance.");
            ds_3gpp_rmsm_ip_common_reset_ipcp_creq_recd(info_ptr);
            ppp_set_mode( ppp_dev, PPP_INTERNAL_MODE );

            crit_sec_ptr = &(info_ptr->v6_info.ipi_sm_info_crit_sec);
            proto = PPP_IPV6CP_PROTOCOL;

            DS_RMSM_ENTER_CRIT_SECTION( crit_sec_ptr );             
            ppp_pkt_ptr = &info_ptr->v6_info.ipv6cp_cfg_pkt_ptr;
            if( !ppp_inject_packet(ppp_dev, proto, *ppp_pkt_ptr) )
            {
              *ppp_pkt_ptr= NULL;
              DS_RMSM_LEAVE_CRIT_SECTION( crit_sec_ptr );     
              DS_MSG0(MSG_LEGACY_HIGH,"ppp pkt injected ");
              ds_3gpp_rmsm_ip_common_post_event( inst_ptr , DS_CMD_3GPP_RMSM_RM_PPP_CONFIGURED );
            }
            else
            {
              DS_RMSM_LEAVE_CRIT_SECTION( crit_sec_ptr );
              DS_MSG0(MSG_LEGACY_HIGH,"ppp pkt error ");
            }
          }
        else if (!IPV6CP_CREQ_RECD(info_ptr) && companion_sm_info_ptr != NULL &&
                 companion_sm_info_ptr->recd_um_down == FALSE && 
                 DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) == DS_PROFILE_PDP_IPV4V6)
        {
          DS_MSG0(MSG_LEGACY_HIGH,"config wait delay timer started- v4 inst down.");
          ps_timer_start(rmsmi_info_ptr->config_wait_timer, 1000);
          ds_rmsm_ip_process_pdn_down_ev(sm_info_ptr);
          sm_info_ptr->recd_um_down = TRUE;
          /* Bring up the companion instance on UM so that FWK indication isn't missed */
          if(ds_3gpp_rmsm_ip_bringup_companion_inst(inst_ptr) != -1)
          {
            DS_MSG0(MSG_LEGACY_HIGH,"Bring up the comp instance type:IPV6");
          break;
        }
          else
            protocol = PPP_PROTOCOL_LCP;
        }
        else
        {
          protocol = PPP_PROTOCOL_LCP;
        }
      }

      else if (V6_SM_INSTANCE(inst_ptr->sm_instance))
      {
        /*---------------------------------------------------------------------
          If Iface down recd for IPv6 and IPCP is received and 
          iface down has not been recd for IPv4 iface then donot tear DUN call
        ---------------------------------------------------------------------*/
        if ( IPCP_CREQ_RECD(info_ptr) &&
             companion_sm_info_ptr != NULL &&
             companion_sm_info_ptr->recd_um_down == FALSE )
        {
          protocol = PPP_PROTOCOL_IPV6CP;
          ds_3gpp_rmsm_ip_common_clear_nak_timer(rmsmi_info_ptr);
          if(TRUE == ps_timer_is_running(rmsmi_info_ptr->ppp_up_delay_timer))
          {
            ps_timer_cancel(rmsmi_info_ptr->ppp_up_delay_timer);
          }

          sm_info_ptr->recd_um_down = TRUE;

          /*---------------------------------------------------------------------
            If PDP type is v4v6, we wait for both v4 and v6 config callbacks
            before completing RM PPP configuration. So, if an Iface 
            was torn down due to some reason, we should complete the 
            configuration for the companion Iface in case it is already UP.
          ---------------------------------------------------------------------*/
            DS_MSG0(MSG_LEGACY_HIGH,"Cleaning up V6 instance.");
            ds_3gpp_rmsm_ip_common_reset_ipv6cp_creq_recd(info_ptr);
            ppp_set_mode( ppp_dev, PPP_INTERNAL_MODE );
          
            crit_sec_ptr = &(info_ptr->v4_info.ipi_sm_info_crit_sec);
            proto = PPP_IPCP_PROTOCOL;
          
            DS_RMSM_ENTER_CRIT_SECTION( crit_sec_ptr );             
            ppp_pkt_ptr = &info_ptr->v4_info.ipcp_cfg_pkt_ptr;
            if( !ppp_inject_packet(ppp_dev, proto, *ppp_pkt_ptr) )
            {
              *ppp_pkt_ptr = NULL;
              DS_RMSM_LEAVE_CRIT_SECTION( crit_sec_ptr );     
              DS_MSG0(MSG_LEGACY_HIGH,"ppp pkt injected ");
              ds_3gpp_rmsm_ip_common_post_event( inst_ptr , DS_CMD_3GPP_RMSM_RM_PPP_CONFIGURED );
            }
            else
            {
              DS_RMSM_LEAVE_CRIT_SECTION( crit_sec_ptr );
              DS_MSG0(MSG_LEGACY_HIGH,"ppp pkt error ");
            }          
        }
        else if (!IPCP_CREQ_RECD(info_ptr) && companion_sm_info_ptr != NULL &&
                 companion_sm_info_ptr->recd_um_down == FALSE && 
                 DS_RMSMI_GET_PDP_TYPE(rmsmi_info_ptr) == DS_PROFILE_PDP_IPV4V6)
        {
          DS_MSG0(MSG_LEGACY_HIGH,"config wait delay timer started- v6 inst down.");
          ps_timer_start(rmsmi_info_ptr->config_wait_timer, 1000);
          ds_rmsm_ip_process_pdn_down_ev(sm_info_ptr); 
          sm_info_ptr->recd_um_down = TRUE;
          /* Bring up the companion instance on UM so that FWK indication isn't missed */
          if(ds_3gpp_rmsm_ip_bringup_companion_inst(inst_ptr) != -1)
          {
            DS_MSG0(MSG_LEGACY_HIGH,"Bring up the comp instance type:IPV4");
          break;
          }
          else
            protocol = PPP_PROTOCOL_LCP;
        }  
        else
        {
          protocol = PPP_PROTOCOL_LCP;
        }
      }

      if (protocol == PPP_PROTOCOL_LCP)
      {
        ds_3gpp_rmsm_ip_common_reset_ipcp_creq_recd(info_ptr);
        ds_3gpp_rmsm_ip_common_reset_ipv6cp_creq_recd(info_ptr);
        
        if( !ppp_stop(ppp_dev, protocol) )
        {
          DS_MSG0(MSG_LEGACY_HIGH,"Stopping PPP ");
          ds_3gpp_rmsm_ip_common_transition_state(inst_ptr->rm_dev_instance, 
                            DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE);
        }
 
        else
        {
          DS_MSG0(MSG_LEGACY_HIGH,"PPP stop failed. Transition to NULL state");
          ds_3gpp_rmsm_ip_common_transition_state( inst_ptr->rm_dev_instance,
                                           DS_3GPP_RMSM_IP_COMMON_NULL_STATE );
        }
      } 
      ds_rmsm_ip_process_pdn_down_ev(sm_info_ptr); 
    
      break;
      
    }

    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_CLEANUP_STATE:
      ds_rmsm_ip_process_pdn_down_ev(sm_info_ptr);
      
      if (ds_3gpp_rmsm_ip_common_is_call_tearable(rmsmi_info_ptr))
      {
        ds_3gpp_rmsm_ip_common_transition_state( inst_ptr->rm_dev_instance, 
                                             DS_3GPP_RMSM_IP_COMMON_NULL_STATE );
      }

      break;

    case DS_3GPP_RMSM_IP_COMMON_NULL_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE:
    case DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE:
      /*---------------------------------------------------------------------
        Ignore event in all other states.
      ---------------------------------------------------------------------*/
      DS_MSG2(MSG_LEGACY_HIGH, "Ignoring UM_IFACE_DOWN_EV RmSm Instance :%d state:%d",
                         rmsmi_info_ptr->instance,
                         rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr->state);
      break;

    default:
      DS_MSG0(MSG_LEGACY_ERROR,"Um down received in incorrect state");
       break;
  }
  
func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ip_common_process_um_pdn_down_ev: err_val %d ",
                              err_val);
  return;
}

/*===========================================================================
 FUNCTION   DS_3GPP_RMSM_IP_BRINGUP_COMPANION_INST

 DESCRIPTION  Function to bringup the companion instance for the SM instace passed

 DEPENDENCIES
   none

 RETURN VALUE
   none
   
 SIDE EFFECTS
   
===========================================================================*/

int ds_3gpp_rmsm_ip_bringup_companion_inst
(
  ds_rmsm_instance_type* inst_ptr
)
{
  ds_rmsm_sm_instance_type          comp_sm_inst = DS_RMSM_INVALID_SM_INSTANCE;
  ds_rmsmi_info_type*               rmsmi_info_ptr = NULL;
  ds_local_err_val_e_type           err_val = DS_LOCAL_ERR_VAL_NONE;
  ds_3gpp_rmsm_ip_common_info_type  *info_ptr = NULL;
  ds_rmsmi_sm_info_type*            comp_sm_info_ptr = NULL;
  ds_fwk_clnt_info_s_type           clt_info = {0};
  ds_rmsm_instance_type             companion_inst = {0};
  int retval = -1;
  
  if(inst_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_0;
    goto func_exit; 
  }

  rmsmi_info_ptr = ds_rmsmi_get_rmsmi_info_ptr(inst_ptr->rm_dev_instance);
  if (rmsmi_info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }

  info_ptr = rmsmi_info_ptr->ds_3gpp_rmsm_ip_common_info_ptr;
  if(info_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_2;
    goto func_exit; 
  }

  if (inst_ptr->sm_instance >= DS_RMSM_MAX_SM_INSTANCES)
  { 
    err_val = DS_LOCAL_ERR_VAL_3;
    goto func_exit; 
  }

  if(inst_ptr->sm_instance == DS_RMSM_V4_SM_INSTANCE)
    comp_sm_inst = DS_RMSM_V6_SM_INSTANCE;
  else if(inst_ptr->sm_instance == DS_RMSM_V6_SM_INSTANCE)
    comp_sm_inst = DS_RMSM_V4_SM_INSTANCE;
  else
  {
    err_val = DS_LOCAL_ERR_VAL_4;
    goto func_exit;
  }

  companion_inst.rm_dev_instance = inst_ptr->rm_dev_instance;
  companion_inst.sm_instance = comp_sm_inst;

  comp_sm_info_ptr = ds_rmsmi_get_companion_sm_ptr(inst_ptr);
  if (comp_sm_info_ptr == NULL)
  { 
    err_val = DS_LOCAL_ERR_VAL_5;
    goto func_exit; 
  }
  
  comp_sm_info_ptr->instance_hdl = ds_rmsm_alloc_client_handle(&companion_inst);
  
  retval = ds_rmsm_bringup_pdn(comp_sm_info_ptr);
  if(retval == -1)
  {
    err_val = DS_LOCAL_ERR_VAL_6;
    goto func_exit;
  }
  
  ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_RMSM,
                         (ds_fwk_clnt_handle_type)comp_sm_info_ptr->instance_hdl,
                         &clt_info);
      
  /* Store the fw index in sm_info_ptr */
  comp_sm_info_ptr->fw_inst = clt_info.fw_inst;

  if(rmsmi_info_ptr->rm_ppp_config_ptr == NULL)
  {
    err_val = DS_LOCAL_ERR_VAL_6;
    goto func_exit;
  }

  if(comp_sm_info_ptr->instance == DS_RMSM_V4_SM_INSTANCE)
  {
    rmsmi_info_ptr->rm_ppp_config_ptr->ipcp_info.fwk_inst_ptr = comp_sm_info_ptr->fw_inst;
  }
  else if(comp_sm_info_ptr->instance == DS_RMSM_V6_SM_INSTANCE)
  {  
    rmsmi_info_ptr->rm_ppp_config_ptr->ipv6cp_info.fwk_inst_ptr = comp_sm_info_ptr->fw_inst;
  }

func_exit:
  if(err_val != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_3gpp_rmsm_ip_bringup_companion_inst: err_val %d ",
                              err_val);
  }
  return retval;
}
