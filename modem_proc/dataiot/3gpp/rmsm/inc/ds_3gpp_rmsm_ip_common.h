#ifndef DS_3GPP_RMSM_IP_COMMON_H
#define DS_3GPP_RMSM_IP_COMMON_H
/*===========================================================================

                      D S U M T S _ R M S M _ I P _ C O M M O N  . H

DESCRIPTION
  The Data Services RM State Machine(PDP-IP) specific header file. 
  This contains the state machine specific function definitions and 
  data definitions 
  
EXTERNALIZED FUNCTIONS
Copyright (c) 2002 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/rmsm/inc/ds_3gpp_rmsm_ip_common.h#3 $ 
$DateTime: 2020/08/28 01:39:46 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/11/11    vs      Initial file
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_headers.h"
#include "dstaski.h"
#include "ps_ip_netconfig.h"
#include "ps_ppp_defs.h"
#include "ds_pdn_manager.h"


/*---------------------------------------------------------------------------
  Enumeration of states of the RmSm IP Common State Machine.
---------------------------------------------------------------------------*/
typedef enum
{
  /*Initial State*/
  DS_3GPP_RMSM_IP_COMMON_NULL_STATE,    

  /*Waiting for initial IPCP or IPv6CP Creq packet*/
  DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_CFG_REQ_STATE,

  /*Waiting for PPP to be configured with IP address / IID information */
  DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_CONFIGURED_STATE,

  /*PPP configured with IP address/IID */
  DS_3GPP_RMSM_IP_COMMON_PPP_CONFIGURED_STATE,

  /* Waiting for Um to go down */
  DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_UM_CLEANUP_STATE,

  /* Waiting for PPP to go down */
  DS_3GPP_RMSM_IP_COMMON_WAITING_FOR_PPP_DOWN_STATE,

}ds_3gpp_rmsm_ip_common_state_type;

#define V4_SM_INSTANCE(sm_inst) \
  ((sm_inst == DS_RMSM_V4_SM_INSTANCE)? TRUE:FALSE)

#define V6_SM_INSTANCE(sm_inst) \
  ((sm_inst == DS_RMSM_V6_SM_INSTANCE)? TRUE:FALSE)

#define ANY_SM_INSTANCE(sm_inst) \
  ((sm_inst == DS_RMSM_ANY_SM_INSTANCE)? TRUE:FALSE)

/*---------------------------------------------------------------------------
  Data Structure which stores IPCP related information.
---------------------------------------------------------------------------*/
typedef struct 
{
  uint32                     ipcp_snoop_tag;  
  dsm_item_type              *ipcp_cfg_pkt_ptr;
  boolean                    send_ipcp_naks_flg;
  ps_timer_handle_type       ipcp_res_del_timer;
  uint8                      ipcp_creq_id; 
  boolean                    ipcp_creq_recd;
  rex_crit_sect_type         ipi_sm_info_crit_sec;  

}ds_3gpp_rmsm_ip_common_v4_info_type;

/*---------------------------------------------------------------------------
  Data Structure which stores IPV6CP related information.
---------------------------------------------------------------------------*/
typedef struct 
{
  uint32                     ipv6cp_snoop_tag;  
  dsm_item_type              *ipv6cp_cfg_pkt_ptr;
  boolean                    send_ipv6cp_naks_flg;
  ps_timer_handle_type       ipv6cp_res_del_timer; 
  uint8                      ipv6cp_creq_id;
  boolean                    ipv6cp_creq_recd;
  rex_crit_sect_type         ipi_sm_info_crit_sec;  

}ds_3gpp_rmsm_ip_common_v6_info_type;

/*---------------------------------------------------------------------------
  Data Structure which stores RmSm IP Common Information.
---------------------------------------------------------------------------*/
typedef struct
{
  ds_3gpp_rmsm_ip_common_state_type     state;
 
  ds_pdn_network_params_info_type       ppp_info;       
 
  ds_3gpp_rmsm_ip_common_v4_info_type   v4_info;
  
  ds_3gpp_rmsm_ip_common_v6_info_type   v6_info;

  boolean (*configure_ppp_f_ptr) (void*, ppp_protocol_e_type);

} ds_3gpp_rmsm_ip_common_info_type;

/*---------------------------------------------------------------------------
  This structure is used to read NV item single_ip_dun_call_in_dual_ip_profile
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  boolean  allow_single_ip_dun_call_dual_ip_profile;
                     /* NV item to allow single ip call in dual ip profile*/
  uint16   ppp_up_wait_timer_val;
                      /* wait timer (in ms) for IPCP/IPV6CP */
  uint16   extra_bits;
                     /* reserved space */
}ds_pdn_dun_call_config_type;

boolean ds_3gpp_rmsm_ip_common_ipcp_creq_was_received 
(
  ds_3gpp_rmsm_ip_common_info_type* info_ptr
);

#define IPCP_CREQ_RECD(info_ptr) \
  ds_3gpp_rmsm_ip_common_ipcp_creq_was_received(info_ptr)


boolean ds_3gpp_rmsm_ip_common_ipv6cp_creq_was_received 
(
  ds_3gpp_rmsm_ip_common_info_type* info_ptr
);            

#define IPV6CP_CREQ_RECD(info_ptr) \
  ds_3gpp_rmsm_ip_common_ipv6cp_creq_was_received(info_ptr)

#endif /*DS_3GPP_RMSM_IP_COMMON_H*/
