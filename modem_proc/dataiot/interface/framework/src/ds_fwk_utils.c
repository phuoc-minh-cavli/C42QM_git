/*==========================================================================*/
/*!
  @file 
  ds_fwk_utils.c

  @brief
  This file provides functions that are used to perform initializations 
  of netiface module.

  Copyright (c) 2017-2023 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*==========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/interface/framework/src/ds_fwk_utils.c#19 $
  $DateTime: 2023/01/23 07:54:55 $$Author: pwbldsvc $

  when            who what, where, why
  ---------- --- ------------------------------------------------------------
  2018 -08 -30 mk  Created module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Include Files
---------------------------------------------------------------------------*/
#include "ds_headers.h"
#include "ds_fwk.h"
#include "ds_pdn_ioctl.h"
#include "ds_ipfltr.h"
#include "ds_sys_eventi.h"
#include "ds_pdn_defs.h"
#include "ds_bearer_manager.h"
#include "ds_pdn_manager.h"
#include "ds_flow.h"
#include "ds_crit_sect.h"
#include "ps_addri_mgmt.h"
#include "ps_stat_logging.h"
#include "ds_fwki.h"
#include "ps_addr_v6.h"
#include "ds_rmnet_smi.h"
static ULogHandle ds_fwk_ulog_handle = NULL;
static ULogHandle ds_fwk_ulog_handle2 = NULL;

/*---------------------------------------------------------------------

                      DS _FWK_UTILS API
                      
-----------------------------------------------------------------------*/
/*===========================================================================
FUNCTION DS_FWK_GET_DEFAULT_BEARER()
 
DESCRIPTION
   gets the defualt bearer from the given framework instance
  
PARAMETERS
   ds_fwk_inst_ptr
   ip_type

RETURN VALUE
  bearer_ptr

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ds_bearer_mgr_s* ds_fwk_get_default_bearer
(
  ds_fwk_s_type*     ds_fwk_inst_ptr,
  ip_addr_enum_type  ip_type
)
{
  if( ds_fwk_inst_ptr == NULL )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"fW inst is NULL");	 
    return NULL;
  }
  if( ip_type == IPV4_ADDR && ds_fwk_inst_ptr->v4_pdn_ptr != NULL )
  {
    return ds_fwk_inst_ptr->v4_pdn_ptr->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
  }
  else if( ip_type == IPV6_ADDR && ds_fwk_inst_ptr->v6_pdn_ptr != NULL )
  {
    return ds_fwk_inst_ptr->v6_pdn_ptr->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
  }
  else if( ip_type == NON_IP_ADDR && ds_fwk_inst_ptr->non_ip_pdn_ptr != NULL )
  {
    return ds_fwk_inst_ptr->non_ip_pdn_ptr->bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
  }
  return NULL;
}


/*===========================================================================
FUNCTION DS_FWK_IS_NON_IP_BEARER()

DESCRIPTION
   checks whether the bearer is ip or non_ip type
  
PARAMETERS
  bearer_ptr

RETURN VALUE
  TRUE - if bearer is non_ip
  FALSE - otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_fwk_is_non_ip_bearer
(
  ds_bearer_mgr_s       *bearer_ptr
)
{
  if( bearer_ptr != NULL && bearer_ptr->pdn_mgr_ptr != NULL)
  {
    if( bearer_ptr->pdn_mgr_ptr->inst_ptr[DS_PDN_INSTANCE_NON_IP] != NULL )
  return TRUE;
}
  return FALSE;
}

/*===========================================================================
FUNCTION DS_FWK_GET_NUM_FILTERS()

DESCRIPTION
   returns the number of filters on the framework instance
  
PARAMETERS
  ds_fwk_inst_ptr - framework instance
  client_type - type of ip filter
  
RETURN VALUE
  no of filters

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 ds_fwk_get_num_filters
(
  ds_fwk_s_type*                ds_fwk_inst_ptr,
  ds_ipfltr_client_id_enum_type client
)
{
  if( ds_fwk_inst_ptr == NULL )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"fW inst is NULL");	 
    return 0;
  }

  return (q_cnt(&((ds_fwk_inst_ptr)->ipfltr_info[client]))+\
           q_cnt(&global_ipfltr_info[client]));
}

/*===========================================================================
  FUNCTION DS_FWK_IS_FLOW_ENABLED()

  DESCRIPTION
   checks whether the flow is enabled or not on the given flow ptr.
      
  PARAMETERS
   flow_ptr

  RETURN VALUE
       0  -- if flow is disabled
       1 -- when flow is enabled

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_fwk_is_flow_enabled
(
  ds_flow_type*  flow_ptr
)
{
  /* first check whether bearer flow is enabled or not and then check flow state*/
  if( ds_flow_is_valid( flow_ptr ))
  {
    if( ds_bearer_mgr_is_tx_flow_enabled(flow_ptr->bearer_ptr) && 
       ( flow_ptr->state == DS_FLOW_STATE_UP))
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*===========================================================================
  FUNCTION DS_FWK_IS_FLOW_ENABLED_EX()

  DESCRIPTION
   checks whether the flow is enabled or not on the given flow ptr.
      
  PARAMETERS
   flow_ptr
   is_mox_data_pkt

  RETURN VALUE
       0  -- if flow is disabled
       1 -- when flow is enabled

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_fwk_is_flow_enabled_ex
(
  ds_flow_type*  flow_ptr,
  boolean        is_mox_data_pkt
)
{
  /* first check whether bearer flow is enabled or not and then check flow state*/
  DS_MSG1(MSG_LEGACY_HIGH,"is_mox_data_pkt %d",is_mox_data_pkt);
  if( ds_flow_is_valid( flow_ptr ))
  {
    if( ds_bearer_mgr_is_tx_flow_enabled_ex(flow_ptr->bearer_ptr,is_mox_data_pkt) && 
       ( flow_ptr->state == DS_FLOW_STATE_UP))
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*===========================================================================
  FUNCTION DS_FWK_GET_DATA_FLOW

  DESCRIPTION
   checks if packet matches with any of the bearers and verifies whether flow is enabled or not.
      
  PARAMETERS
   ds_fwk_inst_ptr -- instance on which the packet needs to be sent
   ip_type -- ip type of client
   pkt_info -- packet which needs to be sent
   flow_ptr -- out param - flow ptr on which the packet will be sent

  RETURN VALUE
       0  -- if flow is disabled for the packet
       1 -- when flow is enabled for the packet

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

boolean ds_fwk_get_data_flow
(
  ds_fwk_s_type             *ds_fwk_inst_ptr,
  ip_addr_enum_type          ip_type,
  ip_pkt_info_type          *pkt_info,
  ds_flow_type             **flow_ptr
)
{
  return ds_fwk_get_data_flow_ex(ds_fwk_inst_ptr,
                          ip_type,
                          pkt_info,
                          flow_ptr,
                          FALSE);

}


/*===========================================================================
  FUNCTION DS_FWK_GET_DEFAULT_BEARER_FLOW_EXT

  DESCRIPTION
   Checks if packet matches with any of the bearers and verifies whether flow 
   is enabled or not.

   This api does not check the DS_FLOW_CAPABILITY_DATA_DISALLOWED 
   mask on the flows
      
  PARAMETERS
   ds_fwk_inst_ptr -- instance on which the packet needs to be sent
   ip_type -- ip type of client
   flow_ptr -- flow pointer 
   is_mox_data_pkt -- is mo exception packet


  RETURN VALUE
       0  -- if flow is disabled for the packet
       1 -- when flow is enabled for the packet

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_fwk_get_default_bearer_flow_ext
(
  ds_fwk_s_type            *ds_fwk_inst_ptr,
  ip_addr_enum_type         ip_type,
  ds_flow_type            **flow_ptr,
  boolean                   is_mox_data_pkt
)
{
  ds_flow_type                 *ds_flow_ptr = NULL;
  ds_bearer_mgr_s              *def_bearer = NULL;
  ds_pdn_mgr_instance_type_e    inst_type = NULL;

  if( ds_fwk_inst_ptr == NULL )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Invalid args");
    return FALSE;
  }

  def_bearer = ds_fwk_get_default_bearer(ds_fwk_inst_ptr,ip_type);
  if( ip_type == IPV4_ADDR )
  {
    inst_type = DS_PDN_INSTANCE_IPV4;
  }
  else if( ip_type == IPV6_ADDR )
  {
    inst_type = DS_PDN_INSTANCE_IPV6;
  }
  else if( ip_type == NON_IP_ADDR )
  {
    inst_type = DS_PDN_INSTANCE_NON_IP;
  }
  if( def_bearer != NULL )
  {
    ds_flow_ptr = ds_flow_mgr_get_default_flow_from_bearer(def_bearer,inst_type);
   *flow_ptr = ds_flow_ptr;
    if( (ds_flow_ptr != NULL) && 
        (ds_fwk_is_flow_enabled_ex(ds_flow_ptr,is_mox_data_pkt) == TRUE))
    {
       DS_MSG1(MSG_LEGACY_HIGH,"default flow 0x%x",ds_flow_ptr);
       return TRUE;
    }
  }
  return FALSE;
}

/*===========================================================================
  FUNCTION DS_FWK_GET_DATA_FLOW_EXT

  DESCRIPTION
   Checks if packet matches with any of the bearers and verifies whether flow 
   is enabled or not.
      
  PARAMETERS
   ds_fwk_inst_ptr -- instance on which the packet needs to be sent
   ip_type -- ip type of client
   pkt_info -- packet which needs to be sent
   flow_ptr -- out param - flow ptr on which the packet will be sent

  RETURN VALUE
       0  -- if flow is disabled for the packet
       1 -- when flow is enabled for the packet

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_fwk_get_data_flow_ext
(
  ds_fwk_s_type             *ds_fwk_inst_ptr,
  ip_addr_enum_type          ip_type,
  ip_pkt_info_type          *pkt_info,
  ds_flow_type             **flow_ptr
)
{			
  ds_flow_type				   *ds_flow_ptr = NULL;
	  
  if( ds_fwk_inst_ptr == NULL || pkt_info == NULL || flow_ptr == NULL )
  {
     DS_MSG0(MSG_LEGACY_ERROR,"Invalid args");
     return FALSE;
  }
  *flow_ptr = NULL;
  if (DS_FWK_NUM_FILTERS(ds_fwk_inst_ptr,FLTR_CLIENT_QOS_OUTPUT) > 0)
  {
     ds_flow_ptr = (ds_flow_type *)ds_ipfltr_execute(ds_fwk_inst_ptr->ds_fwk_index,
		                                             FLTR_CLIENT_QOS_OUTPUT,
		                                             DS_IPFLTR_SUBSET_ID_DEFAULT,
                                                     pkt_info);
   *flow_ptr = ds_flow_ptr;
	if( ds_flow_ptr != NULL && 
		ds_fwk_is_flow_enabled_ex(ds_flow_ptr,FALSE) == TRUE )
	{
	  DS_MSG1(MSG_LEGACY_HIGH,"Pkt matched with dedicated flow 0x%x",ds_flow_ptr);
	  return TRUE;
	}
  }
  if(ds_fwk_get_default_bearer_flow_ext(ds_fwk_inst_ptr,ip_type,
									   flow_ptr,FALSE))
  {  
     DS_MSG1(MSG_LEGACY_HIGH,"Pkt matched with dedicated flow 0x%x",*flow_ptr);
     return TRUE;
  }
  return FALSE;
}

/*===========================================================================
  FUNCTION DS_FWK_GET_DATA_FLOW_EX

  DESCRIPTION
   Checks if packet matches with any of the bearers and verifies whether flow is enabled or not.
      
  PARAMETERS
   ds_fwk_inst_ptr -- instance on which the packet needs to be sent
   ip_type -- ip type of client
   pkt_info -- packet which needs to be sent
   flow_ptr -- out param - flow ptr on which the packet will be sent
   is_mox_data_pkt -- mo exception packet or not.

  RETURN VALUE
       0  -- if flow is disabled for the packet
       1 -- when flow is enabled for the packet

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

boolean ds_fwk_get_data_flow_ex
(
  ds_fwk_s_type             *ds_fwk_inst_ptr,
  ip_addr_enum_type          ip_type,
  ip_pkt_info_type          *pkt_info,
  ds_flow_type             **flow_ptr,
  boolean                    is_mox_data_pkt
)
{

  ds_flow_type                 *ds_flow_ptr = NULL;
  
  if( ds_fwk_inst_ptr == NULL || pkt_info == NULL || flow_ptr == NULL )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Invalid args");
    return FALSE;
  }
  *flow_ptr = NULL;
  if (DS_FWK_NUM_FILTERS(ds_fwk_inst_ptr,FLTR_CLIENT_QOS_OUTPUT) > 0)
  {
    ds_flow_ptr = (ds_flow_type *)ds_ipfltr_execute(ds_fwk_inst_ptr->ds_fwk_index,
                                     FLTR_CLIENT_QOS_OUTPUT,
                                     DS_IPFLTR_SUBSET_ID_DEFAULT,
                                     pkt_info);
   *flow_ptr = ds_flow_ptr;
    if( ds_flow_ptr != NULL && 
        ds_fwk_is_flow_enabled_ex(ds_flow_ptr,is_mox_data_pkt) == TRUE )
    {
      DS_MSG1(MSG_LEGACY_HIGH,"Pkt matched with dedicated flow 0x%x",ds_flow_ptr);
      return TRUE;
    }
  }
  if(ds_fwk_get_default_bearer_flow_ex(ds_fwk_inst_ptr,ip_type,
                                       flow_ptr,is_mox_data_pkt))
  {  
    DS_MSG1(MSG_LEGACY_HIGH,"Pkt matched with dedicated flow 0x%x",*flow_ptr);
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================
  FUNCTION DS_FWK_GET_DEFAULT_BEARER_FLOW

  DESCRIPTION
   checks if packet matches with any of the bearers and verifies whether flow is enabled or not.
      
  PARAMETERS
   ds_fwk_inst_ptr -- instance on which the packet needs to be sent
   ip_type -- ip type of client
   flow_ptr -- out param - flow ptr on which the packet will be sent

  RETURN VALUE
       0  -- if flow is disabled for the packet
       1 -- when flow is enabled for the packet

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_fwk_get_default_bearer_flow
(
  ds_fwk_s_type              *ds_fwk_inst_ptr,
  ip_addr_enum_type           ip_type,
  ds_flow_type              **flow_ptr
)
{
  return ds_fwk_get_default_bearer_flow_ex(
                        ds_fwk_inst_ptr,
                        ip_type,
                        flow_ptr,
                        FALSE);
}

/*===========================================================================
  FUNCTION DS_FWK_GET_DEFAULT_BEARER_FLOW_EX

  DESCRIPTION
   checks if packet matches with any of the bearers and verifies whether flow is enabled or not.
      
  PARAMETERS
   ds_fwk_inst_ptr -- instance on which the packet needs to be sent
   ip_type -- ip type of client
   flow_ptr -- flow pointer 
   is_mox_data_pkt -- is mo exception packet


  RETURN VALUE
       0  -- if flow is disabled for the packet
       1 -- when flow is enabled for the packet

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean ds_fwk_get_default_bearer_flow_ex
(
  ds_fwk_s_type            *ds_fwk_inst_ptr,
  ip_addr_enum_type         ip_type,
  ds_flow_type            **flow_ptr,
  boolean                   is_mox_data_pkt
)
{
  ds_flow_type                 *ds_flow_ptr = NULL;
  ds_bearer_mgr_s              *def_bearer = NULL;
  ds_pdn_mgr_instance_type_e    inst_type = NULL;

  if( ds_fwk_inst_ptr == NULL )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Invalid args");
    return FALSE;
  }

  def_bearer = ds_fwk_get_default_bearer(ds_fwk_inst_ptr,ip_type);
  if( ip_type == IPV4_ADDR )
  {
    inst_type = DS_PDN_INSTANCE_IPV4;
  }
  else if( ip_type == IPV6_ADDR )
  {
    inst_type = DS_PDN_INSTANCE_IPV6;
  }
  else if( ip_type == NON_IP_ADDR )
  {
    inst_type = DS_PDN_INSTANCE_NON_IP;
  }
  if( def_bearer != NULL )
  {
    ds_flow_ptr = ds_flow_mgr_get_default_flow_from_bearer(def_bearer,inst_type);
   *flow_ptr = ds_flow_ptr;
    if( ds_flow_ptr != NULL && 
        ds_fwk_is_flow_enabled_ex(ds_flow_ptr,is_mox_data_pkt) == TRUE &&
       (!ds_flow_get_capability(ds_flow_ptr, DS_FLOW_CAPABILITY_DATA_DISALLOWED)))
    {
      DS_MSG1(MSG_LEGACY_HIGH,"default flow 0x%x",ds_flow_ptr);
      return TRUE;
    }
  }
  return FALSE;
}

/*===========================================================================
FUNCTION DS_FWKI_GET_LINKLOCAL_ADDR()

DESCRIPTION
  This function is used to get the link local address of the V6 interface.
  The function sets the addr type to invalid if the call fails.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr_ptr:    value return - the address will be will be stored here

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Copies the ipv6 addr from ps_iface_ptr to ip_addr_ptr.
===========================================================================*/
void ds_fwki_get_linklocal_addr
(
  ds_fwk_s_type         *fwk_inst,
  ps_ip_addr_type       *ip_addr_ptr,  
  ip_addr_enum_type       ip_ver
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ip_addr_ptr == NULL || fwk_inst == NULL)
  {
    return;
  }

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  if(ip_ver == IPV6_ADDR)
  {
    if ((NULL != fwk_inst) &&
               (NULL != fwk_inst->v6_net_info.ipv6_addrs[DS_DEFAULT_V6_INDEX]))
    {
        ip_addr_ptr->type = IPV6_ADDR;
        ip_addr_ptr->addr.v6.ps_s6_addr32[0] = ps_htonl(0xFE800000UL);
        ip_addr_ptr->addr.v6.ps_s6_addr32[1] = 0;
        ip_addr_ptr->addr.v6.ps_s6_addr64[1] =
        fwk_inst->v6_net_info.ipv6_addrs[DS_DEFAULT_V6_INDEX]->iid;
        DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        return; //return here for success case
    }
  }

  ip_addr_ptr->type = IP_ADDR_INVALID;
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return;
} /* ps_ifacei_get_linklocal_addr() */

/*===========================================================================
FUNCTION DS_FWK_GET_V4_DNS_ADDRS

DESCRIPTION
  This function returns the primary and secondary DNS addr's on the
  IPV4 PDN.

PARAMETERS
  pdn_ptr_info: ptr to interface control block on which to operate on.
  prim_dns_ptr:  storage for primary dns address
  sec_dns_ptr:  storage for secondary dns address

RETURN VALUE
  None.  However, if the addr family is not IPV4, then the input
  parameters are stored with zero.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_fwk_get_v4_dns_addrs
(
  ds_fwk_s_type         *fwk_inst,
  ip_addr_type          *prim_dns_ptr,
  ip_addr_type          *sec_dns_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (fwk_inst == NULL || prim_dns_ptr == NULL || sec_dns_ptr == NULL)
  {
    DS_MSG3(MSG_LEGACY_ERROR,"ds_fwk_get_v4_dns_addrs(): "
                    "NULL parameters are passed, fwk_inst %p, prim_dns_ptr, 0x%p, "
                    "sec_dns_ptr, 0x%p", fwk_inst, prim_dns_ptr, sec_dns_ptr);
    DS_ASSERT(0);
    return;
  }

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Ensure that the addr family is consistent through the logica/assoc
    PDN chain.
  -------------------------------------------------------------------------*/

  prim_dns_ptr->addr.v4 = fwk_inst->v4_net_info.primary_dns.ps_s_addr;
  sec_dns_ptr->addr.v4  = fwk_inst->v4_net_info.secondary_dns.ps_s_addr;

  /*-------------------------------------------------------------------------
    Set appropriate ip address types.
  -------------------------------------------------------------------------*/
  if(prim_dns_ptr->addr.v4 == 0)
  {
    prim_dns_ptr->type    = IP_ADDR_INVALID;
  }
  else
  {
    prim_dns_ptr->type    = IPV4_ADDR;
  }

  if(sec_dns_ptr->addr.v4 == 0)
  {
    sec_dns_ptr->type     = IP_ADDR_INVALID;
  }
  else
  {
    sec_dns_ptr->type     = IPV4_ADDR;
  }
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
} /* ds_fwk_get_v4_dns_addrs() */


#if  defined (FEATURE_DATA_PS_IPV6)
/*===========================================================================
FUNCTION DS_FWK_GET_V6_DNS_ADDRS

DESCRIPTION
  This function returns the primary and secondary DNS addr's on the
  IPV6 PDN.

PARAMETERS
  pdn_ptr_info: ptr to interface control block on which to operate on.
  prim_dns_ptr:  storage for primary dns address
  sec_dns_ptr:  storage for secondary dns address

RETURN VALUE
  None.  However, if the addr family is not IPV6, then the input
  parameters are stored with zero.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_fwk_get_v6_dns_addrs
(
  ds_fwk_s_type         *fwk_inst,
  ip_addr_type          *prim_dns_ptr,
  ip_addr_type          *sec_dns_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (fwk_inst == NULL || prim_dns_ptr == NULL || sec_dns_ptr == NULL)
  {
    DS_MSG3(MSG_LEGACY_ERROR,"ds_fwk_get_v6_dns_addrs(): "
                    "NULL parameters are passed, fwk_inst %p, prim_dns_ptr, 0x%p, "
                    "sec_dns_ptr, 0x%p", fwk_inst, prim_dns_ptr, sec_dns_ptr);
    DS_ASSERT(0);
    return;
  }

  prim_dns_ptr->addr.v6[0] =
                        fwk_inst->v6_net_info.primary_dns.ps_s6_addr64[0];
  prim_dns_ptr->addr.v6[1] =
                        fwk_inst->v6_net_info.primary_dns.ps_s6_addr64[1];

  sec_dns_ptr->addr.v6[0] =
                      fwk_inst->v6_net_info.secondary_dns.ps_s6_addr64[0];
  sec_dns_ptr->addr.v6[1] =
                      fwk_inst->v6_net_info.secondary_dns.ps_s6_addr64[1];

  /*-------------------------------------------------------------------------
    Set appropriate ip address types.
  -------------------------------------------------------------------------*/
  if( (prim_dns_ptr->addr.v6[0] == 0) &&
      (prim_dns_ptr->addr.v6[1] == 0))
  {
    prim_dns_ptr->type = IP_ADDR_INVALID;
  }
  else
  {
    prim_dns_ptr->type = IPV6_ADDR;
  }

  if( (sec_dns_ptr->addr.v6[0] == 0) &&
      (sec_dns_ptr->addr.v6[1] == 0))
  {
    sec_dns_ptr->type  = IP_ADDR_INVALID;
  }
  else
  {
    sec_dns_ptr->type  = IPV6_ADDR;
  }

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* ds_fwk_get_v6_dns_addrs() */
#endif
/*===========================================================================
LOCAL FUNCTION DS_FWK_CALC_GATEWAY_ADDR()

DESCRIPTION
  This function calculates the netmask and server/router IP address
  from the client IP address. This function tries to figure out the
  smallest netmask without handing out the broadcast address.

DEPENDENCIES
  Client IP address must be valid
  Currently only supports IPV4 addrs.  Of course, with IPV6, this
  whole thing is obsolete.

PARAMETERS
  dhcp_am_ps_iface_info * - Handle to the glue needed to get at the connection
                   details.

RETURN VALUE
  None.
  Calculations put into handle structure.

SIDE EFFECTS
  None
===========================================================================*/
void ds_fwk_calc_gateway_addr
(
  uint32    client_ip,
  uint32   *gateway_ip,
  uint32   *netmask
)
{
  uint32 bits = 1;
  uint32 mask = ~bits;
  uint32 ip;
  uint32 host;
  uint32 network;

  if(client_ip == 0)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_calc_gateway_addr(): client ip is been 0 in ");
    return;
  }  /* Do the calculation in host byte oder */

  ip = dss_ntohl(client_ip);

  /*------------------------------------------------------------------------ 
    If LSB is 1, then skip all 1's till 0 to generate netmask.
    For Example:
    IP      = 10.0.0.3        ( 0000 1010 0000 0000 0000 0000 0000 0011 )
    netmask = 255.255.255.248 ( 1111 1111 1111 1111 1111 1111 1111 1000 )
   
    If LSB is 0, then skip all 0's till 1 to genrate netmask.
    For Example:
    IP      = 10.0.0.8        ( 0000 1010 0000 0000 0000 0000 0000 1000 )
    netmask = 255.255.255.240 ( 1111 1111 1111 1111 1111 1111 1111 0000 )
  ------------------------------------------------------------------------*/
  if (bits & ip) 
  {
    while( (bits & ip) )
    {
      bits <<= 1;
      mask <<= 1;
    }
  }
  else
  {
    while( !(bits & ip) )
    {
      bits <<= 1;
      mask <<= 1;
    }
  }

#define DHCP_AM_MIN_MASK ((uint32)(0xfffffffc))
  mask &= DHCP_AM_MIN_MASK;

#if defined(PS_DHCP_IP_ADDR_CLASS_BASED_NETMASK_CALC)

    /*
     * Netmask is calculated on the basis of IP address class.
     * This is because if we choose MIN_MASK the chances of broadcast address
     * coinciding with other addresses, the n/w assigns like - DNS, SIP etc.
     * will increase.
     * Gateway address calculation will be still based on MIN_MASK.
     * This is because if we calculate it based on the calculated netmask value,
     * the value will always be netmask+1 or netmask+2, which increases the
     * chances of this address coinciding with the ip address of another
     * rmnet instance. So we choose MIN_MASK to calculate gateway address.
     * TBD: Not a fool proof solution re-visit in future if needed.
     */
  
    /* class C address */
    if ( ( ip & 0xE0000000 ) == 0xC0000000 )
    {
      *netmask  = dss_htonl(0xFFFFFF00);
    }
    else if( ( ip & 0xC0000000 ) == 0x80000000 ) /* class B address */
    {
      *netmask  = dss_htonl (0xFFFF0000);
    }
    else if( ( ip & 0x80000000 ) == 0x00000000 )   /* class A address */
    {
      *netmask  = dss_htonl (0xFF000000);
    } 
#elif defined(PS_DHCP_FIXED_NETMASK_255_255_255_255)


    /*
     * IP address class based netmask calculation has few limitations.
     * In Qualnet all the IP addresses are Class A addresses.
     * When a QMI call is made in lab, the netmask will be 255.0.0.0,
     * in that case Windows routes all the packets through QMI interface.
     * In case of Multi-Rmnet calls, all the QMI ip addresses will fall into a
     * single subnet. Then in that case we cannot test the data transfers
     * individually on different PDN's. To overcome these limitations
     * we are fixing the netmask to 255.255.255.255
     * But this may have other limitations like the ones encountered in 
     * field testing.
     * TBD: Not a fool proof solution, more testing is needed.
     */
    *netmask  = dss_htonl(0xFFFFFFFF);
#else

    *netmask = dss_htonl (mask);
#endif /* PS_DHCP_IP_ADDR_CLASS_BASED_NETMASK_CALC */

  
  /*------------------------------------------------------------------------ 
    Generate Server IP by adding 1 to IP address.
    If by adding 1 the host address becomes broadcast address
    then subtract 1 to generate server address.
   
    For Example (Subtract):
    IP      =  10.0.0.10       (0000 1010 0000 0000 0000 0000 0000 1010)
    netmask =  255.255.255.252 (1111 1111 1111 1111 1111 1111 1111 1100)
    Server  =  10.0.0.9        (0000 1010 0000 0000 0000 0000 0000 1001)
   
    For Example (Addition):
    IP      =  10.0.0.11       (0000 1010 0000 0000 0000 0000 0000 1011)
    netmask =  255.255.255.248 (1111 1111 1111 1111 1111 1111 1111 1000)
    Server  =  10.0.0.12       (0000 1010 0000 0000 0000 0000 0000 1100)
  ------------------------------------------------------------------------*/
  if ( (~mask & (ip + 1)) == ~mask )
  {
    host = ~mask & (ip - 1);
  }
  else
  {
    host = ~mask & (ip + 1);
  }

  network = mask & ip;

  /*------------------------------------------------------------------------ 
    Combine Network & Host address to form server address.
  ------------------------------------------------------------------------*/
  ip = network | host;

  *gateway_ip = dss_htonl(ip);
}

/*===========================================================
FUNCTION DS_FWK_GET_IP_ADDR

DESCRIPTION
  This function returns the primary IP address based on the ip 
  version

PARAMETERS
  ds_fwk_inst_ptr: ptr to fwk instance control block on which to
                   operate on.
  ip_ver:          version of the ip address to be looked 

RETURN VALUE
  ip_addr_ptr.  Ip address pointr in which the address will be
                 stored

DEPENDENCIES
  None

SIDE EFFECTS
  None
=============================================================*/
void ds_fwk_get_ip_addr
(
  ds_fwk_s_type*          ds_fwk_inst_ptr,
  ip_addr_enum_type       ip_ver,
  ps_ip_addr_type         *ip_addr_ptr
)
{
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if((ds_fwk_inst_ptr !=NULL)&&(ip_addr_ptr != NULL))
  {  
     ip_addr_ptr->type = IP_ADDR_INVALID;
   /* Validate the passed framework instance. */
     if(!ds_fwk_inst_is_valid_ex(ds_fwk_inst_ptr, ip_ver))
     {
       DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_get_ip_addr(): Invalid Fwk Instance.");
       DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
       return;  
     }  
     if((ip_ver == IPV6_ADDR)&&(ds_fwk_inst_ptr->v6_pdn_ptr !=NULL))
     {
       if(ds_fwk_inst_ptr->v6_net_info.ipv6_addrs[0] != NULL)
       {
         memset(ip_addr_ptr, 0, sizeof(ps_ip_addr_type));
         ip_addr_ptr->type = IPV6_ADDR;
         memscpy
         (
           ip_addr_ptr->addr.v6.ps_s6_addr64,
           sizeof(uint64),
           &(ds_fwk_inst_ptr->v6_net_info.ipv6_addrs[0]->prefix),
           sizeof(uint64)
         );
         memscpy
         (
           ip_addr_ptr->addr.v6.ps_s6_addr64 + 1,
           sizeof(uint64),
           &(ds_fwk_inst_ptr->v6_net_info.ipv6_addrs[0]->iid),
           sizeof(uint64)
         );
       }
     }
     else if ((ip_ver == IPV4_ADDR)&&(ds_fwk_inst_ptr->v4_pdn_ptr !=NULL))
     {
       if(ds_fwk_inst_ptr->v4_net_info.ipv4_addr != 0 )
       {
         memset(ip_addr_ptr, 0, sizeof(ps_ip_addr_type));
         ip_addr_ptr->type = IPV4_ADDR;
         ip_addr_ptr->addr.v4.ps_s_addr = 
         ds_fwk_inst_ptr->v4_net_info.ipv4_addr;
       }
     }
     else if(ip_ver == NON_IP_ADDR)
     {
       memset(ip_addr_ptr, 0, sizeof(ps_ip_addr_type));
     } 
     else
     {
       DS_MSG0(MSG_LEGACY_ERROR,"Invalid IP type or the pdn pointer is NULL");
     }
  } 
  else if(ip_addr_ptr != NULL)
  {
    ip_addr_ptr->type = IP_ADDR_INVALID;
  }

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return;
}


/*===========================================================================
FUNCTION DS_FWK_GET_IPV6_ADDR()

DESCRIPTION
  This function will return IPv6 address for the interface to be used by
  transport layers. If the interface is a v6 interface and IPv6 is enabled
  then this function returns the source address of the interface with
  appropriate scope based on the passed destination address.
  Otherwise it just returns a v4 mapped v6 address by making a call to
  ds_fwk_get_addr().

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  dest_ip_ptr:    Destination IPv6 address.
  ip_addr_ptr:    value return - the address will be  will be stored here

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_fwk_get_ipv6_addr
(
  ds_fwk_s_type              *fw_ptr,  
  ip_addr_enum_type           ip_ver,
  const struct ps_in6_addr *dest_ip_ptr,
  ps_ip_addr_type           *ip_addr_ptr 
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(fw_ptr == NULL || dest_ip_ptr == NULL || ip_addr_ptr == NULL)
  {
    DS_ERR_FATAL("NULL paraemter in ds_fwk_get_ipv6_addr()");
    return;
  }

  /* Validate the Passed Framework instance. */
  if(!ds_fwk_inst_is_valid_ex(fw_ptr, ip_ver))
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_get_ipv6_addr(): Invalid Fwk Instance.");
    ip_addr_ptr->type = IP_ADDR_INVALID;
    return;  
  }

  /*-----------------------------------------------------------------------
    Check is the destination address is of link-local or global scope.
    If the destination address is of link-local scope, get the link-local
    address otherwise get the global address.
  -----------------------------------------------------------------------*/
  ip_addr_ptr->type = IPV6_ADDR;

#ifdef FEATURE_DATA_PS_IPV6
  if (PS_IN6_IS_ADDR_MC_LINKLOCAL(dest_ip_ptr) ||
      PS_IN6_IS_ADDR_LINKLOCAL(dest_ip_ptr))
  {
     ds_fwki_get_linklocal_addr(fw_ptr,ip_addr_ptr,ip_ver);
  }
  else
#endif /* FEATURE_DATA_PS_IPV6 */
  {
    ds_fwk_get_ip_addr(fw_ptr,ip_ver, ip_addr_ptr);
  }

  return;
}/* ps_iface_get_ipv6_addr() */

/*===========================================================================
FUNCTION DS_FWK_SET_IP_ADDR

DESCRIPTION
  This function returns the ip address of the call to ds_fwk 

PARAMETERS
  fwk_inst : ptr to interface control block on which to operate on.
  ip_ver    : IP type of the fwk
  ip_addr_ptr :pbuffer where the ip address will be returned 

RETURN VALUE
  ip address
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

void ds_fwk_set_ip_addr
(
  ds_fwk_s_type*          ds_fwk_inst_ptr,
  ip_addr_enum_type       ip_ver,
  ps_ip_addr_type         *ip_addr_ptr
)
{

  if(ds_fwk_inst_ptr == NULL ||ip_addr_ptr == NULL)
  {
    DS_ERR_FATAL("Invalid fwk inst or ip_addr_ptr pointer is NULL");
    return;
  }

  if(ip_addr_ptr !=NULL)
  {
   if((ip_ver == IPV6_ADDR)&&(ip_addr_ptr !=NULL))
   {
      ip_addr_ptr->type = IPV6_ADDR;
    
     memscpy
     (
       &(ds_fwk_inst_ptr->v6_net_info.ipv6_addrs[0]->prefix),
       sizeof(uint64),
       ip_addr_ptr->addr.v6.ps_s6_addr64,
       sizeof(uint64)
     );
     memscpy
     (
       &(ds_fwk_inst_ptr->v6_net_info.ipv6_addrs[0]->iid),
       sizeof(uint64),
       ip_addr_ptr->addr.v6.ps_s6_addr64 + 1,
       sizeof(uint64)

     );
   }
   else if ((ip_ver == IPV4_ADDR)&&(ip_addr_ptr !=NULL))
   {
     ip_addr_ptr->type = IPV4_ADDR;
     ds_fwk_inst_ptr->v4_net_info.ipv4_addr =
	 	           ip_addr_ptr->addr.v4.ps_s_addr;
   }
   else
   {
     DS_ERR_FATAL("Invalid IP type or the pdn pointer is NULL");
   } 
  }

  return;
}

void ds_fwk_get_all_dns_addrs
(
  ds_fwk_s_type                 *fwk_inst,
  ip_addr_type          *dns_addrs_ptr,
  uint8                         *num_dns_addrs_ptr,
  ds_pdn_mgr_instance_type_e	 pdn_instance
)
{
  ip_addr_type		prim_dns_addr, sec_dns_addr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(fwk_inst ==NULL)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_get_all_dns_addrs(): "
                             "Invalid fwk_inst 0x%p", fwk_inst);
    DS_ASSERT(0);
    return;
  }

  if(pdn_instance ==DS_PDN_INSTANCE_NON_IP)
    return;

  if( (dns_addrs_ptr == NULL) || (num_dns_addrs_ptr == NULL) )
  {
     DS_MSG2(MSG_LEGACY_ERROR,"ds_fwk_get_all_dns_addrs(): "
                     "NULL param, dns_addrs_ptr: 0x%p or num_dns_addrs_ptr: 0x%p",
                     dns_addrs_ptr, num_dns_addrs_ptr);
     DS_ASSERT(0);
     return;
  }

  /*-------------------------------------------------------------------------
    This function to be changed once more than 2 DNS addresses are supported.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);  
  if(pdn_instance == DS_PDN_INSTANCE_IPV4)
  {
     ds_fwk_get_v4_dns_addrs(fwk_inst, &prim_dns_addr, &sec_dns_addr);
	 
  }
#ifdef FEATURE_DATA_PS_IPV6
  else if(pdn_instance ==DS_PDN_INSTANCE_IPV6)
  {
    ds_fwk_get_v6_dns_addrs(fwk_inst, &prim_dns_addr, &sec_dns_addr);
  }
#endif /* FEATURE_DATA_PS_IPV6 */
  else
  {  
    DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    DS_ASSERT(0);
    return;
  }

   /*-------------------------------------------------------------------------
     This to be changed once multiple DNS addresses are supported.
   -------------------------------------------------------------------------*/
  if(*num_dns_addrs_ptr >= 2)
  {
    memscpy(&dns_addrs_ptr[0], 
            sizeof(ip_addr_type), 
            &prim_dns_addr, 
            sizeof(ip_addr_type) );

    memscpy(&dns_addrs_ptr[1], 
            sizeof(ip_addr_type), 
            &sec_dns_addr, 
            sizeof(ip_addr_type) );

    *num_dns_addrs_ptr = 2;
  }
  else
  {
     memscpy(&dns_addrs_ptr[0], 
             sizeof(ip_addr_type), 
             &prim_dns_addr, 
             sizeof(ip_addr_type) );
	 
     *num_dns_addrs_ptr = 1;
  }  
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
}

int ds_fwk_get_op_reserved_pco
(
  ds_pdn_mgr_s                       *pdn_ptr_info,
  ds_pdn_op_reserved_pco_type        *operator_reserved_pco_info_ptr
)
{
  if ((pdn_ptr_info ==NULL) ||(operator_reserved_pco_info_ptr == NULL))
  {
     DS_MSG2(MSG_LEGACY_ERROR,"ds_fwk_get_op_reserved_pco(): "
             "NULL parameters are passed "
             "operator_reserved_pco_info_ptr, 0x%p, PDN 0x%p",
             operator_reserved_pco_info_ptr,pdn_ptr_info);
     DS_ASSERT(0);
     return -1;
   }

  if(pdn_ptr_info->state != DS_PDN_MGR_STATE_UP)
  {
   return -1;
  }
  /*-------------------------------------------------------------------------
     Copy the PCO information saved in the PDN
  -------------------------------------------------------------------------*/
  
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);  

  memscpy(operator_reserved_pco_info_ptr,
          sizeof(ds_pdn_op_reserved_pco_type),
          &pdn_ptr_info->op_reserved_pco,
          sizeof(ds_pdn_op_reserved_pco_type));

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);  
  return 0;
}

/*===========================================================================
FUNCTION DS_FWK_GET_MODEM_RESERVED_PCO

DESCRIPTION
  This function returns the modem reserved PCO information in the pdn_ptr.

PARAMETERS
  ds_pdn_mgr_s:               ptr to PDN control block on which to operate on.
  operator_reserved_pco_info_ptr: pointer to the operator reserved PCO info
 
RETURN VALUE
  0 if successful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

int ds_fwk_get_modem_reserved_pco
(
  ds_pdn_mgr_s                       *pdn_ptr_info,
  ds_pdn_mgr_modem_reserved_pco_type *modem_reserved_pco_info_ptr
)
{
  if ((pdn_ptr_info ==NULL) ||(modem_reserved_pco_info_ptr == NULL))
  {
     DS_MSG2(MSG_LEGACY_ERROR,"ds_fwk_get_modem_reserved_pco(): "
             "NULL parameters are passed " 
             "operator_reserved_pco_info_ptr, 0x%p, PDN 0x%p", 
             modem_reserved_pco_info_ptr,pdn_ptr_info);
     return -1;
  }

  if(pdn_ptr_info->state != DS_PDN_MGR_STATE_UP)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_get_modem_reserved_pco():"
            "pdn state is not in up state");
    return -1;
  }
  /*-------------------------------------------------------------------------
     Copy the PCO information saved in the PDN
  -------------------------------------------------------------------------*/
  
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);  
  memscpy(modem_reserved_pco_info_ptr,
          sizeof(ds_pdn_mgr_modem_reserved_pco_type),
          &pdn_ptr_info->modem_reserved_pco,
          sizeof(ds_pdn_mgr_modem_reserved_pco_type));
  
 DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);  
 return 0;
}

void ds_fwk_get_peer_addr
(
  ds_pdn_mgr_s                 *pdn_ptr_info,
  ps_ip_addr_type              *ip_addr_ptr,  
  ds_pdn_mgr_instance_type_e    pdn_instance,
  ds_fwk_s_type                *ds_fwk_inst_ptr 
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   if ((pdn_ptr_info ==NULL) || (ip_addr_ptr == NULL))
   {
     DS_ASSERT(0);
     return;
   }
   if (ds_fwk_inst_ptr == NULL)
   {
     DS_ASSERT(0);
     return;
   }
   if(pdn_instance == DS_PDN_INSTANCE_NON_IP)
      return;
   
   DS_ENTER_CRIT_SECTION(&global_ps_crit_section);   
  /*-------------------------------------------------------------------------
     The address is only valid if the state of the interface is UP - in all
     other cases return invalid address.
   -------------------------------------------------------------------------*/
   switch(pdn_ptr_info->state)
   {   
     case DS_PDN_MGR_STATE_DOWN:
     case DS_PDN_MGR_STATE_COMING_UP:
	
     /*---------------------------------------------------------------------
        Return the invalid address, the address is invalid in this state
      ---------------------------------------------------------------------*/ 
     ip_addr_ptr->type = IP_ADDR_INVALID;
     break;
	
     case DS_PDN_MGR_STATE_UP:
     case DS_PDN_MGR_STATE_GOING_DOWN:	
       break;
	
     default:
       DS_ASSERT(0);
    } /* switch(PDN state) */
	
    if(ip_addr_ptr->type == IP_ANY_ADDR)
    {
      if(pdn_instance == DS_PDN_INSTANCE_IPV4)
        ip_addr_ptr->type = IPV4_ADDR;
      else if(pdn_instance == DS_PDN_INSTANCE_IPV6)
        ip_addr_ptr->type = IPV6_ADDR;
    }
	
    switch(ip_addr_ptr->type)
    {
      case IPV4_ADDR:
       /*-----------------------------------------------------------------------
         return the v4 gateway address if available
       -----------------------------------------------------------------------*/
      if(ds_fwk_inst_ptr->v4_net_info.gateway.ps_s_addr)
      {
        ip_addr_ptr->addr.v4.ps_s_addr =
                     ds_fwk_inst_ptr->v4_net_info.gateway.ps_s_addr;
      }
      else
      {
        ip_addr_ptr->type = IP_ADDR_INVALID;
      }
      break;

	 case IPV6_ADDR:
      /*-----------------------------------------------------------------------
         return the link local IPv6 address, if gateway iid is available
       -----------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6
     if(//(pdn_ptr_info->inst_ptr[pdn_instance]->v6 ipv6_addrs[DEFAULT_V6_INDEX] != NULL) &&
        ds_fwk_inst_ptr->v6_net_info.gateway_iid)
     {
        ip_addr_ptr->addr.v6.ps_s6_addr32[0] = ps_htonl(0xFE800000UL);
        ip_addr_ptr->addr.v6.ps_s6_addr32[1] = 0;
        ip_addr_ptr->addr.v6.ps_s6_addr64[1] =
         ds_fwk_inst_ptr->v6_net_info.gateway_iid;
     }
     else
#endif /* FEATURE_DATA_PS_IPV6 */
     {
       ip_addr_ptr->type = IP_ADDR_INVALID;
     }
     break;
	
    case IP_ADDR_INVALID:
     break;
	
     default:
      DS_ASSERT(0);
    } /* switch(requested address type) */
    DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    if(ip_addr_ptr->type == IP_ADDR_INVALID)
    {
       memset(ip_addr_ptr, 0, sizeof(ps_ip_addr_type));
       ip_addr_ptr->type = IP_ADDR_INVALID;
    }
}

int ds_fwk_get_sip_serv_addr_ex
(
  ds_fwk_s_type                 *fwk_inst,
  ip_addr_type                  *sip_serv_addr_ptr_array,
  uint32                        *count,
  ds_pdn_mgr_instance_type_e     pdn_instance
)
{

  ds_fwk_sip_serv_addr_type	                       *curr_sip_serv_addr;
  uint32						 temp_count = 0;
  uint32						 result = -1;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(pdn_instance == DS_PDN_INSTANCE_NON_IP)
      return 0;

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);  
  do
  {
    if (count == NULL || (sip_serv_addr_ptr_array == NULL && *count != 0))
    {
	 DS_MSG2(MSG_LEGACY_ERROR,"Count 0x%p or sip_serv_addr 0x%p is NULL",
                               count,sip_serv_addr_ptr_array);
	 DS_ASSERT(0);
     break;
	} 
    if(*count == 0)
    {
      break;
   }	  
   if (fwk_inst ==NULL)
   {
     DS_MSG0(MSG_LEGACY_ERROR,"fW inst info is NULL");	 
	 DS_ASSERT(0);
     break;
   }
   /*-------------------------------------------------------------------------
     Traverse the list and copy the SIP server addresses in the array.
   -------------------------------------------------------------------------*/
   if(pdn_instance == DS_PDN_INSTANCE_IPV4)
   {
    curr_sip_serv_addr = list_peek_front
              (&fwk_inst->v4_net_info.sip_serv_addr);

   while (temp_count < *count && curr_sip_serv_addr != NULL)
   {
     sip_serv_addr_ptr_array[temp_count].type =
      curr_sip_serv_addr->sip_addr.type;
  
     if (curr_sip_serv_addr->sip_addr.type == IPV4_ADDR)
     {
       sip_serv_addr_ptr_array[temp_count].addr.v4 =
        curr_sip_serv_addr->sip_addr.addr.v4.ps_s_addr;
     }
         curr_sip_serv_addr = 
         list_peek_next(&fwk_inst->v4_net_info.sip_serv_addr,
                                             &curr_sip_serv_addr->link);
         temp_count++;
        }
     *count = temp_count;
     result = 0;
   }
   else if(pdn_instance == DS_PDN_INSTANCE_IPV6)
   {
      curr_sip_serv_addr = list_peek_front
                        (&fwk_inst->v6_net_info.sip_serv_addr);
   
    while (temp_count < *count && curr_sip_serv_addr != NULL)
    {
      sip_serv_addr_ptr_array[temp_count].type =
       curr_sip_serv_addr->sip_addr.type;
  
      if(curr_sip_serv_addr->sip_addr.type == IPV6_ADDR)
     {
       sip_serv_addr_ptr_array[temp_count].addr.v6[0] =
        curr_sip_serv_addr->sip_addr.addr.v6.ps_s6_addr64[0];
       sip_serv_addr_ptr_array[temp_count].addr.v6[1] =
        curr_sip_serv_addr->sip_addr.addr.v6.ps_s6_addr64[1];
     }
     curr_sip_serv_addr = 
     list_peek_next(&fwk_inst->v6_net_info.sip_serv_addr,
                                         &curr_sip_serv_addr->link);
     temp_count++;
    }
	  
   *count = temp_count;
    result = 0;
    }
  } while(0);	
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
 return result;
} /* ds_fwk_get_sip_serv_addr_ex() */


int ds_fwk_get_pdn_stats
(
  ps_stat_fwk_enum_type      stat_type,
  ds_fwk_s_type             *fwk_inst,
  ip_addr_enum_type          ip_type,
  ds_stats_type             *stats
)
{
  if(ds_fwk_get_all_stats
      (
       stat_type,
       fwk_inst,
       stats,
       sizeof(ds_stats_type),
       ip_type
      ) == E_SUCCESS)
  {
     return 0;
  }
  return -1;
}

int ds_fwk_reset_pdn_stats
(
  ds_fwk_s_type             *fwk_inst,
  ip_addr_enum_type          ip_type,  
  sint15                     *return_value
)
{
  if(fwk_inst)
  {
    if(ip_type == IPV4_ADDR)
    {
      memset(&(fwk_inst->rm_stats[DS_FWK_IP_TYPE_V4]), 0, sizeof(ds_stats_type));
      memset(&(fwk_inst->um_stats[DS_FWK_IP_TYPE_V4]), 0, sizeof(ds_stats_type));
    }
    else if(ip_type == IPV6_ADDR)
    {
       memset(&(fwk_inst->rm_stats[DS_FWK_IP_TYPE_V6]), 0, sizeof(ds_stats_type));
       memset(&(fwk_inst->um_stats[DS_FWK_IP_TYPE_V6]), 0, sizeof(ds_stats_type));
    }     
  }
  return 0;
}

int ds_fwk_get_sip_domain_names
(
  ds_fwk_s_type      *fwk_inst,
  ds_domain_name_type    *sip_serv_addr_ptr_array,
  uint8               *count
)
{
  return 0;
}

int ds_fwk_get_subnet_mask
(
  ds_pdn_mgr_s                         *pdn_ptr_info,
  ds_pdn_ioctl_subnet_mask_type        *subnet_info,
  ds_pdn_mgr_instance_type_e            pdn_instance,
  ds_fwk_s_type                        *fwk_inst,
  sint15                               *return_value  
)
{
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);  
  if(((pdn_ptr_info ==NULL )||(fwk_inst ==NULL)))
  {  
     DS_MSG0(MSG_LEGACY_ERROR,"pdn_ptr or fwk_inst is NULL so crashing");
     DS_ASSERT(0);
  }
  switch(pdn_ptr_info->state)
  {
  /*-------------------------------------------------------------------
    Return the invalid address, the subnet mask is invalid in this
    state
   -------------------------------------------------------------------*/
    case DS_PDN_MGR_STATE_DOWN:
    case DS_PDN_MGR_STATE_COMING_UP:
        memset(subnet_info, 0, sizeof(ds_pdn_ioctl_subnet_mask_type));
        subnet_info->type = IP_ADDR_INVALID;
	    DS_MSG0(MSG_LEGACY_ERROR,"Subnet mask is invalid due to PDN state");
        break;
        /*-------------------------------------------------------------------
          In following states, PDN has a valid IP address, and hence a
          valid subnet mask
        -------------------------------------------------------------------*/
    case DS_PDN_MGR_STATE_UP:
    case DS_PDN_MGR_STATE_GOING_DOWN:
    case DS_PDN_MGR_STATE_PEND_REDIAL:
      if (pdn_instance == DS_PDN_INSTANCE_IPV4)
      {
          subnet_info->type = IPV4_ADDR;
          subnet_info->addr.v4 =
            fwk_inst->v4_net_info.net_mask.ps_s_addr;          
          DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
          return 0;
      }
      else
      {
        DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        DS_MSG2(MSG_LEGACY_ERROR,"ds_fwk_get_subnet_mask is not supported "
                                  "on IPv6 PDN or NON-IP PDN 0x%p: inst is %d",
                                  pdn_ptr_info,pdn_instance);
       *return_value = DS_EAFNOSUPPORT;
        return -1;
      }
    default:
    {
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      DS_MSG3(MSG_LEGACY_ERROR,"Unknown PDN state %d, PDN 0x%p inst is %d",
                                pdn_ptr_info->state,
                                pdn_ptr_info,
                                pdn_instance);
	  
      *return_value = DS_EINVAL;
       return -1;    
    }	
    DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return 0;		
 }
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return -1;	     
}

/*============================================================
FUNCTION DS_FWK_GET_DPL_PTR()

DESCRIPTION
  This function will return the address of the dpl structure 

PARAMETERS
  fw_inst:             ptr to fwk control block on which to operate on.
  ip_type:             the address will be stored here
  client_id	       client_id RM or UM

RETURN VALUE
  dpl_net_cb

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/

dpl_net_cb *ds_fwk_get_dpl_ptr
(
  ds_fwk_s_type        *fw_inst,
  ip_version_enum_type  ip_type,
  ds_dpl_client_type    client_id
)
{
  DS_MSG2(MSG_LEGACY_HIGH,"ip_type %d client_id %d ",ip_type,client_id);

  if((fw_inst != NULL)&&(client_id == DS_DPL_CLIENT_UM))
  {
   DS_MSG2(MSG_LEGACY_HIGH,"fw_inst %p client_id %d ",fw_inst,client_id);
   if(ip_type == IP_V4)
    return &(fw_inst->dpl_um[DS_FWK_IP_TYPE_V4]);
   else if(ip_type == IP_V6)
    return &(fw_inst->dpl_um[DS_FWK_IP_TYPE_V6]);
  }
  else if((fw_inst !=NULL)&&(client_id == DS_DPL_CLIENT_RM))
  {
    if(ip_type == IP_V4)
      return &(fw_inst->dpl_rm[DS_FWK_IP_TYPE_V4]);
   else if(ip_type == IP_V6)
     return &(fw_inst->dpl_rm[DS_FWK_IP_TYPE_V6]);
  }  
  return NULL;
}

/*============================================================
FUNCTION DS_FWK_GENERATE_PRIV_IPV6_ADDR()

DESCRIPTION
  This function will generate the private ipv6 address requested by the client

PARAMETERS
  pdn_ptr_info:        ptr to fwk control block on which to operate on.
  ip_addr_ptr:         the address will be stored here
  iid_param_ptr	       IID params
  ps_errno             Error no

RETURN VALUE
  uint 

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
int ds_fwk_generate_priv_ipv6_addr
(
  ds_pdn_mgr_s            *pdn_ptr_info,
  ps_ip_addr_type         *ip_addr_ptr,
  ps_ipv6_iid_params_type *iid_param_ptr,
  int16                   *ps_errno
)
{
  return 0;
}

/*============================================================
FUNCTION DS_FWK_GET_MTU()

DESCRIPTION
  This function will generate mtu requested by the client

PARAMETERS
  fwk_inst:        ptr to fwk control block on which to operate on.
  ip_type:         the address for which the MTU is to be given

RETURN VALUE
  uint16

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
uint16  ds_fwk_get_mtu
(
   ds_fwk_s_type            *fwk_inst,
   ip_addr_enum_type   ip_type
)
{
  if(fwk_inst !=NULL)
  {
     if(ip_type == IPV4_ADDR)
     {
      return   fwk_inst->v4_net_info.mtu;
     }
     else if(ip_type == IPV6_ADDR)
     {
      return fwk_inst->v6_net_info.mtu;
     }
     else if(ip_type == NON_IP_ADDR)
     {
       return  fwk_inst->non_ip_net_info.mtu; 
      }     
       return 1420;
    }
  return  0;
}
/*============================================================
FUNCTION DS_FWK_GET_PDN_PTR_FROM_FWK_INST()

DESCRIPTION
  This function will generate the pdn pointer from th efwk inst passed

PARAMETERS
  fwk_inst:        ptr to fwk control block on which to operate on.
  ip_type:         the address for which the MTU is to be given

RETURN VALUE
  uint16

DEPENDENCIES
  ds_pdn_mgr_s

SIDE EFFECTS
  None
============================================================*/
ds_pdn_mgr_s * ds_fwk_get_pdn_ptr_from_fwk_inst
(
  ds_fwk_s_type        *fwk_inst,
  ip_addr_enum_type     ip_ver
)
{
   if(fwk_inst ==NULL)
   	return NULL;
   if(ip_ver == IPV4_ADDR)
   	return fwk_inst->v4_pdn_ptr ?fwk_inst->v4_pdn_ptr:NULL;
   else if(ip_ver == IPV6_ADDR)
   	return fwk_inst->v6_pdn_ptr?fwk_inst->v6_pdn_ptr:NULL;
   else if(ip_ver == NON_IP_ADDR)
   	return fwk_inst->non_ip_pdn_ptr?fwk_inst->non_ip_pdn_ptr:NULL;
   else 
   	return NULL;
}

/*===========================================================================
FUNCTION DS_FWK_GET_EX_IP6_CONFIG()

DESCRIPTION
  This function copies secondary ip addresses into the v6 address list 
  from the given fwk_inst ptr.

PARAMETERS 
  fwk_inst                  fwk_instance ptr
  ps_v6_addr_list           IPv6 address list

DEPENDENCIES
  None

RETURN VALUE
   0 on successful
  -1 on failure

SIDE EFFECTS
  None
===========================================================================*/
int ds_fwk_get_private_v6_config
(
  ds_fwk_s_type      *fwk_inst,
  q_type             *ps_v6_addr_list
)
{
  ds_fwk_v6_addr_s_type  *v6_src_addr_ptr  = NULL;
  ds_v6_addr_type        *v6_dst_addr_ptr  = NULL;
  uint8                  index = 0;
  struct ps_in6_addr     ipv6_addr;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (fwk_inst == NULL || ps_v6_addr_list == NULL)
  {
    DS_MSG0(MSG_LEGACY_HIGH,"ds_fwk_get_private_v6_config(): "
                    "Invalid Args");
    return -1;
  }

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /*---------------------------------------------------------------------
     Copy all secondary IPv6 address from fwk inst.
  ---------------------------------------------------------------------*/
  for(index = DS_MAX_IPV6_PREFIXES; index < DS_MAX_IPV6_ADDRS; index++)
  {
    v6_src_addr_ptr = fwk_inst->v6_net_info.ipv6_addrs[index];

    if(v6_src_addr_ptr == NULL)
    {
      break;
    }

    DS_SYSTEM_HEAP_MEM_ALLOC(v6_dst_addr_ptr, sizeof(ds_v6_addr_type), ds_v6_addr_type*);
   
   if (v6_dst_addr_ptr == NULL )
   {    
      DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_0("ds_fwk_get_private_v6_config(): "
                      "Alloc Error");
      return -1;
    }

    memset(v6_dst_addr_ptr, 0, sizeof(ds_v6_addr_type));

    if (v6_src_addr_ptr->addr_type == DS_IPV6_ADDR_TYPE_PRIV_SHARED || 
        v6_src_addr_ptr->addr_type == DS_IPV6_ADDR_TYPE_PRIV_UNIQUE ||
        v6_src_addr_ptr->addr_type == DS_IPV6_ADDR_TYPE_EXTERNAL)
    {
      /*---------------------------------------------------------------------
         Add to destination queue.
      ---------------------------------------------------------------------*/
      v6_dst_addr_ptr->prefix           = v6_src_addr_ptr->prefix;
      v6_dst_addr_ptr->iid              = v6_src_addr_ptr->iid;
      v6_dst_addr_ptr->addr_type        = v6_src_addr_ptr->addr_type;

      if (v6_src_addr_ptr->addr_type != DS_IPV6_ADDR_TYPE_EXTERNAL)
      {
       v6_dst_addr_ptr->pref_lifetime    =
               ps_timer_remaining(v6_src_addr_ptr->pref_lifetimer_handle)/1000;
       v6_dst_addr_ptr->valid_lifetime   =
               ps_timer_remaining(v6_src_addr_ptr->valid_lifetimer_handle)/1000;
      }

      v6_dst_addr_ptr->unused_addr_time =
             ps_timer_remaining(v6_src_addr_ptr->unused_addr_timer_handle)/1000;
      q_put(ps_v6_addr_list, q_link(v6_dst_addr_ptr, &v6_dst_addr_ptr->link));

      DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_get_private_v6_config(): "
                      "Returning Following Address from fwk_inst %d",
                       fwk_inst->v6_pdn_ptr);

      ipv6_addr.in6_u.u6_addr64[0] = v6_dst_addr_ptr->prefix;
      ipv6_addr.in6_u.u6_addr64[1] = v6_dst_addr_ptr->iid;
      IPV6_ADDR_MSG(ipv6_addr.ps_s6_addr64);
    }
  }     
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return 0;
}




/*===========================================================================
FUNCTION PS_FWK_SET_PRIVATE_IP6_CONFIG()

DESCRIPTION
  Set secondary ip addresses to the given fwk inst ptr.

PARAMETERS 
  fwk_inst                  fwk_instance ptr
  ps_v6_addr_list           IPv6 address list
  is_psm_restore            PSM restoration

DEPENDENCIES
  None

RETURN VALUE
   0 on successful
  -1 on failure

SIDE EFFECTS
  None
===========================================================================*/
int ds_fwk_set_private_v6_config
(
  ds_fwk_s_type           *fwk_inst,
  q_type                  *ps_v6_addr_list,
  boolean                  is_psm_restore
)
{
#ifdef FEATURE_DATA_PS_ADDR_MGMT
  ps_addr_mgmt_alloc_type     alloc_info;
  struct ps_in6_addr          ip_addr;
  ps_addr_mgmt_handle_type    handle;
#endif /* FEATURE_DATA_PS_ADDR_MGMT */
  
  ds_v6_addr_type             *v6_src_addr_ptr  = NULL;
  ds_fwk_v6_addr_s_type       *v6_dst_addr_ptr  = NULL;
  ds_fwk_v6_addr_s_type       *public_addr_ptr  = NULL;
  struct ps_in6_addr           ipv6_addr;
  ps_ipv6_iid_params_type      iid_param;
  int16                        error_num;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (fwk_inst == NULL || ps_v6_addr_list == NULL)
  {
    LOG_MSG_ERROR_0("ds_fwk_set_private_v6_config(): "
                    "Invalid Args");
    return -1;
  }

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /*------------------------------------------------------------------------
    Get Public address to obtain prefix length.
  -------------------------------------------------------------------------*/
  public_addr_ptr = fwk_inst->v6_net_info.ipv6_addrs[0];

  /*---------------------------------------------------------------------
     Copy all secondary IPv6 address to ps fwk_inst.
  ---------------------------------------------------------------------*/
  for(v6_src_addr_ptr  = (ds_v6_addr_type*) q_check(ps_v6_addr_list);
      v6_src_addr_ptr != NULL;
      v6_src_addr_ptr  = q_next(ps_v6_addr_list, &v6_src_addr_ptr->link))
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(v6_dst_addr_ptr, sizeof(ds_fwk_v6_addr_s_type),
                             ds_fwk_v6_addr_s_type*);

    if (v6_dst_addr_ptr == NULL )
    {    
      DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_0("ds_fwk_set_private_v6_config(): "
                      "Alloc Error");
      return -1;
    }

    if (v6_src_addr_ptr->addr_type == DS_IPV6_ADDR_TYPE_PRIV_SHARED || 
        v6_src_addr_ptr->addr_type == DS_IPV6_ADDR_TYPE_PRIV_UNIQUE)
    {
      /*---------------------------------------------------------------------
         Add to destination queue.
      ---------------------------------------------------------------------*/
      v6_dst_addr_ptr->prefix           = v6_src_addr_ptr->prefix;
      v6_dst_addr_ptr->iid              = v6_src_addr_ptr->iid;
      v6_dst_addr_ptr->ref_cnt          = 0;
      v6_dst_addr_ptr->prefix_len       = public_addr_ptr->prefix_len;
      iid_param.app_request             = TRUE;
      iid_param.is_unique               = 
         (v6_src_addr_ptr->addr_type == DS_IPV6_ADDR_TYPE_PRIV_UNIQUE) ? TRUE : FALSE;

      DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_set_private_v6_config(): "
                      "Setting Following Address in fwk inst :%d",
                       fwk_inst->v6_pdn_ptr);

      ipv6_addr.in6_u.u6_addr64[0] = v6_dst_addr_ptr->prefix;
      ipv6_addr.in6_u.u6_addr64[1] = v6_dst_addr_ptr->iid;
      IPV6_ADDR_MSG(ipv6_addr.ps_s6_addr64);

     if (ps_alloc_priv_ipv6_addr(fwk_inst, &v6_dst_addr_ptr, &iid_param,
                                           FALSE, TRUE, &error_num) < 0)
     {     
       DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
       LOG_MSG_ERROR_0("ds_fwk_set_private_v6_config(): "
                       "Privacy address allocation failed.");
       return -1;
     }

     if(v6_src_addr_ptr->pref_lifetime > 0)
     {
       /*-------------------------------------------------------------------
         Start Preferred lifetime if value is > 0.
       -------------------------------------------------------------------*/
       (void)ps_timer_start(v6_dst_addr_ptr->pref_lifetimer_handle,
                            v6_src_addr_ptr->pref_lifetime*1000);
     }
     else if(v6_src_addr_ptr->unused_addr_time > 0)
     {
       /*-------------------------------------------------------------------
         Start the unused activity timer if value > 0.
       -------------------------------------------------------------------*/
       (void)ps_timer_start(v6_dst_addr_ptr->unused_addr_timer_handle,
                            v6_src_addr_ptr->unused_addr_time*1000);
     } 
     else
     {
       /*-------------------------------------------------------------------
         Both Unused and Preferred lifetime are < 0, thus delete the address
         to generate proper event for application.
       -------------------------------------------------------------------*/
       ps_delete_priv_ipv6_addr(fwk_inst, v6_dst_addr_ptr);
     }
    }
    else if (v6_src_addr_ptr->addr_type == DS_IPV6_ADDR_TYPE_EXTERNAL)
    {
#ifdef FEATURE_DATA_PS_ADDR_MGMT
      ip_addr.ps_s6_addr64[0] = v6_src_addr_ptr->prefix;
      ip_addr.ps_s6_addr64[1] = v6_src_addr_ptr->iid;
      memset(&alloc_info, 0, sizeof(alloc_info));
      alloc_info.ip_addr.type    = IPV6_ADDR;
      alloc_info.ip_addr.addr.v6 = ip_addr;
      alloc_info.addr_type       = DS_IPV6_ADDR_TYPE_EXTERNAL;
      alloc_info.is_psm_restore  = is_psm_restore;
      if (ps_addr_mgmt_alloc_unique_addr(fwk_inst, &handle, &alloc_info,
                                           DS_FWK_CLIENT_RMNET, &error_num) < 0)
      {      
         DS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
         DS_MSG0(MSG_LEGACY_HIGH,"ds_fwk_set_external_v6_config(): "
                 "Privacy address allocation failed.");
         return -1;
       }

#endif /* FEATURE_DATA_PS_ADDR_MGMT */
    }
  }

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return 0;
}

/*===========================================================================
FUNCTION PS_FWK_SET_IP_CONFIG()

DESCRIPTION
  Set secondary ip addresses to the given fwk inst ptr.

PARAMETERS 
  ds_fwk_inst_ptr      fwk_inst_ptr
  ip_ver               IP version
  ip_config_ptr        IP config ptr


DEPENDENCIES
  None

RETURN VALUE
   0 on successful
  -1 on failure

SIDE EFFECTS
  None
===========================================================================*/

int ds_fwk_set_ip_config
(
  ds_fwk_s_type*          ds_fwk_inst_ptr,
  ip_addr_enum_type       ip_ver,
  ds_ip_config_type       *ip_config_ptr
)
{
  int                           ret_val       = -1;
  uint8                         prefix_length = 8;
  ds_pdn_event_info_u_type      event_info;
  ds_pdn_mgr_instance_type_e    pdn_instance  = DS_PDN_INSTANCE_MAX;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ds_fwk_inst_ptr == NULL || ip_config_ptr == NULL)
  {
    DATA_ERR_FATAL("ds_fwk_set_ip_config(): Invalid FWK_INST or "
                    "ip_config_ptr passed");
    return ret_val;
  }

  /*-------------------------------------------------------------------------
    Check address family
  -------------------------------------------------------------------------*/
  if (ip_ver !=  ip_config_ptr->ip_addr.type)
  {
    LOG_MSG_ERROR_0("ds_fwk_set_ip_config(): "
                    "IP addr family not matching");
    return ret_val;
  }

  if(ip_ver == IPV4_ADDR)
  {
    pdn_instance = DS_PDN_INSTANCE_IPV4;
  }
  else if(ip_ver == IPV6_ADDR)
  {
    pdn_instance = DS_PDN_INSTANCE_IPV6;
  }
  else if(ip_ver == NON_IP_ADDR)
  {
    pdn_instance = DS_PDN_INSTANCE_NON_IP;
  }
  else
  {
    pdn_instance = DS_PDN_INSTANCE_MAX;
  }

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);  
 
  memset(&event_info,0x0,sizeof(ds_pdn_event_info_u_type));
  do
  {
    /*-----------------------------------------------------------------------
      Update Sip Server addresses in the fwk_inst
    -----------------------------------------------------------------------*/
    if (ip_config_ptr->valid_sip_serv_addr_count > 0)
    {    
      if(!(ds_pdn_mgr_set_sip_serv_addr
                                (ds_fwk_inst_ptr,
                                 ip_config_ptr->sip_serv_addr_array, 
                                 ip_config_ptr->valid_sip_serv_addr_count,
                                 pdn_instance)))
      {
        LOG_MSG_ERROR_0("ds_fwk_set_ip_config(): "
                        "Error setting SIP Serv addrs to fwk_inst:");
        break;
      }
    }
  
    /*-----------------------------------------------------------------------
      Update V4 IP Config details
    -----------------------------------------------------------------------*/
    if (ip_config_ptr->ip_addr.type == IFACE_IPV4_ADDR_FAMILY)
    {
      /*---------------------------------------------------------------------
        Set V4 DNS Addr to fwk_inst_ptr
      ---------------------------------------------------------------------*/
      ds_fwk_inst_ptr->v4_net_info.primary_dns.ps_s_addr =
                                   ip_config_ptr->dns_addrs[0].addr.v4;
      ds_fwk_inst_ptr->v4_net_info.secondary_dns.ps_s_addr =
                                   ip_config_ptr->dns_addrs[1].addr.v4;

      /*---------------------------------------------------------------------
        Set V4 IP Addr to fwk_inst_ptr
      ---------------------------------------------------------------------*/
        ds_fwk_set_ip_addr(ds_fwk_inst_ptr,ip_ver, &(ip_config_ptr->ip_addr));

      /*---------------------------------------------------------------------
        Set V4 Gateway Addr to fwk_inst_ptr
      ---------------------------------------------------------------------*/
      ds_fwk_inst_ptr->v4_net_info.gateway.ps_s_addr = 
                                   ip_config_ptr->v4_gateway_addr;
      ret_val = 0;
      break;
    }

    /*-----------------------------------------------------------------------
      Update V6 IP Config details
    -----------------------------------------------------------------------*/
    else if (ip_config_ptr->ip_addr.type == IFACE_IPV6_ADDR_FAMILY)
    {
      /*---------------------------------------------------------------------
       Set V6 DNS Addr to fwk_inst_ptr
      ---------------------------------------------------------------------*/
      if (-1 == ps_set_v6_dns_addrs
                          (ds_fwk_inst_ptr, 
                          ((ps_ip_addr_type*)&(ip_config_ptr->dns_addrs[0])),
                          ((ps_ip_addr_type*)&(ip_config_ptr->dns_addrs[1]))))
      {
        LOG_MSG_ERROR_0("ds_fwk_set_ip_config(): "
                        "Set v6 DNS addrs to fwk_inst_ptr failed");
        break;
      }

      /*---------------------------------------------------------------------
        Set V6 IP Addr to fwk_inst_ptr
      ---------------------------------------------------------------------*/
      if (-1 == ps_apply_v6_prefix(
                         ds_fwk_inst_ptr,
                         ip_config_ptr->v6_gateway_iid,   
                         ip_config_ptr->ip_addr.addr.v6.ps_s6_addr64[0],
                         ip_config_ptr->prefix_valid_life_time,
                         ip_config_ptr->prefix_pref_life_time, 
                         prefix_length))
      {
        LOG_MSG_ERROR_0("ds_fwk_set_ip_config(): "
                        "Apply V6 Prefix to fwk_inst_ptr failed");
        break;
      }

      /*--------------------------------------------------------------------
       Applied prefix successfully Generate a valid RA indication
      --------------------------------------------------------------------*/
      event_info.ra_lifetime = ip_config_ptr->router_life_time;
      ip6_sm_valid_ra_cback
                      (PDN_VALID_RA_EV, event_info,
                       ds_fwk_inst_ptr->v6_pdn_ptr->\
                       inst_ptr[DS_PDN_INSTANCE_IPV6]->ip6_sm
                       );
      ret_val = 0;
      break;
    }
    else
    {
      LOG_MSG_ERROR_0("ds_fwk_set_ip_config(): "
                      "Unsupported IP type in IP cfg container");
      break;
    }
  }while (0);
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return ret_val;

} /* ds_fwk_set_ip_config() */

/*===========================================================================
FUNCTION PS_FWK_GET_IP_CONFIG()

DESCRIPTION
  Set secondary ip addresses to the given fwk inst ptr.

PARAMETERS 
  ds_fwk_inst_ptr      fwk_inst_ptr
  ip_ver               IP version
  ip_config_ptr        IP config ptr


DEPENDENCIES
  None

RETURN VALUE
   0 on successful
  -1 on failure

SIDE EFFECTS
  None
===========================================================================*/

int ds_fwk_get_ip_config
(
  ds_fwk_s_type*          ds_fwk_inst_ptr,
  ip_addr_enum_type       ip_ver,
  ds_ip_config_type       *ip_config_ptr
)
{
  uint8                      num_dns_addrs = DS_NUM_DNS_ADDRS;
  uint32                     num_sip_serv_addrs = DS_MAX_SIP_SERVER_ADDRESSES;
  ds_fwk_v6_addr_s_type      *v6_addr = NULL;
  ds_pdn_mgr_instance_type_e pdn_instance = DS_PDN_INSTANCE_MAX;
  ds_pdn_mgr_s               *pdn_mgr_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ds_fwk_inst_ptr == NULL)
  {
    DATA_ERR_FATAL("ds_fwk_get_ip_config(): Invalid fwk_inst_ptr passed");
    return -1;
  }

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    1. Copy IP address
  -------------------------------------------------------------------------*/
  ds_fwk_get_ip_addr(ds_fwk_inst_ptr, ip_ver, &(ip_config_ptr->ip_addr));
   
  if (ip_config_ptr->ip_addr.type == IFACE_INVALID_ADDR_FAMILY)
  {
    LOG_MSG_ERROR_0("source interface IP Address is invalid");	
    DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return -1;
  }

  /*-------------------------------------------------------------------------
    2. Copy DNS addresses
  -------------------------------------------------------------------------*/
  if(ip_ver == IPV4_ADDR)
  {
    pdn_instance = DS_PDN_INSTANCE_IPV4;
    pdn_mgr_p = ds_fwk_inst_ptr->v4_pdn_ptr;
  }
  else if(ip_ver == IPV6_ADDR)
  {
    pdn_instance = DS_PDN_INSTANCE_IPV6;
    pdn_mgr_p = ds_fwk_inst_ptr->v6_pdn_ptr;
  }
  else if(ip_ver == NON_IP_ADDR)
  {
    pdn_instance = DS_PDN_INSTANCE_NON_IP;
    pdn_mgr_p = ds_fwk_inst_ptr->non_ip_pdn_ptr;
  }
  else
  {
    pdn_instance = DS_PDN_INSTANCE_MAX;
  }
  memset(ip_config_ptr->dns_addrs, 0, 
         sizeof(ip_config_ptr->dns_addrs));
  ds_fwk_get_all_dns_addrs(ds_fwk_inst_ptr,
                             ip_config_ptr->dns_addrs,
                             &num_dns_addrs, pdn_instance);

  /*-------------------------------------------------------------------------
    3. Copy Sip Server addresses
  -------------------------------------------------------------------------*/
  memset(ip_config_ptr->sip_serv_addr_array, 0, 
         sizeof(ip_config_ptr->sip_serv_addr_array));

  ds_pdn_mgr_get_sip_serv_addr(pdn_mgr_p, pdn_instance,
                                (ps_ip_addr_type*)ip_config_ptr->sip_serv_addr_array, 
                                (uint8*)&num_sip_serv_addrs);

  ip_config_ptr->valid_sip_serv_addr_count = num_sip_serv_addrs;

  /*-------------------------------------------------------------------------
    4. If IP type is V4, copy Gateway addr
  -------------------------------------------------------------------------*/
  if (ip_config_ptr->ip_addr.type == IFACE_IPV4_ADDR_FAMILY)
  {
    ip_config_ptr->v4_gateway_addr = 
                              ds_fwk_inst_ptr->v4_net_info.gateway.ps_s_addr;
  }

  /*-------------------------------------------------------------------------
    5. If IP type is V6, copy timer values & gateway IID
  -------------------------------------------------------------------------*/
  else if (ip_config_ptr->ip_addr.type == IFACE_IPV6_ADDR_FAMILY)
  {
    v6_addr = ds_fwk_inst_ptr->v6_net_info.ipv6_addrs[DS_DEFAULT_V6_INDEX];

    /*----------------------------------------------------------------------
       Init Life time values to Infinite. If timer is not running or 
       timer handle not allocated, assume time to be infinite 
    ----------------------------------------------------------------------*/
    ip_config_ptr->prefix_valid_life_time = 0xFFFFFFFFUL;
    ip_config_ptr->prefix_pref_life_time  = 0xFFFFFFFFUL;

    if (PS_TIMER_INVALID_HANDLE != v6_addr->valid_lifetimer_handle &&
        TRUE == ps_timer_is_running(v6_addr->valid_lifetimer_handle)
       )
    {
     /*----------------------------------------------------------------------
       get the valid life time from handle 
     ----------------------------------------------------------------------*/
     ip_config_ptr->prefix_valid_life_time =
            (uint32) ps_timer_remaining(v6_addr->valid_lifetimer_handle) / 1000;
    }

    if (PS_TIMER_INVALID_HANDLE != v6_addr->pref_lifetimer_handle &&
        TRUE == ps_timer_is_running(v6_addr->pref_lifetimer_handle)
       )
    {
     /*----------------------------------------------------------------------
       get the pref life time value from handle
     ----------------------------------------------------------------------*/
     ip_config_ptr->prefix_pref_life_time =
             (uint32) ps_timer_remaining(v6_addr->pref_lifetimer_handle) / 1000;
    }
    
    ip_config_ptr->v6_gateway_iid = ds_fwk_inst_ptr->v6_net_info.gateway_iid;
  }
  
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return 0;

} /* ds_fwk_get_ip_config() */

/*===========================================================================
  FUNCTION DS_FWK_GET_ALL_STATS


  DESCRIPTION
    Gets all the packet stats on the PDN
        
  PARAMETERS
   stat_type      --  stat type
   ds_fwk_inst_ptr -- instance n which the packet needs to be sent
   return_value    -- structure where stats needs to be given
   ret_len         -- length of the strut 
   call_type       -- ip type of client

  RETURN VALUE
   E_SUCCESS      if the packets stats are found

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

errno_enum_type   ds_fwk_get_all_stats
(
  ps_stat_fwk_enum_type      stat_type,
  ds_fwk_s_type             *fwk_inst,
  void                      *return_value,
  uint16                     ret_len,
  ip_addr_enum_type          call_type
)
{
  ds_stats_type           *stats_ptr;
  ds_stats_type           *fwk_stats = NULL;
  errno_enum_type          ret_val = E_SUCCESS;
/*----------------------------------------------------*/  
  if((fwk_inst == NULL)||(return_value == NULL))
  {
    return E_INVALID_ARG;
  }
  
  if(call_type == IPV4_ADDR)
    fwk_stats = &(fwk_inst->um_stats[DS_FWK_IP_TYPE_V4]);
  else if(call_type == IPV6_ADDR)
    fwk_stats = &(fwk_inst->um_stats[DS_FWK_IP_TYPE_V6]);
  else
  {
    DS_MSG1(MSG_LEGACY_ERROR,"Invalid call type %d", call_type);
    return E_INVALID_ARG;
  }

  switch(stat_type)
  {
     case PS_STAT_FWK_ALL:
     {
        stats_ptr = (ds_stats_type *)return_value;
        stats_ptr->pkts_rx    =  fwk_stats->pkts_rx;      
        stats_ptr->pkts_tx =     fwk_stats->pkts_tx;
        stats_ptr->bytes_rx = fwk_stats->bytes_rx;
        stats_ptr->bytes_tx = fwk_stats->bytes_tx;
        break;
     }
     case PS_STAT_FWK_PKTS_RX:
        (*(uint32 *)return_value) =  fwk_stats->pkts_rx;
      break;
         
     case PS_STAT_FWK_PKTS_TX:
       (*(uint32 *)return_value) = fwk_stats->pkts_tx;
      break;
         
     case PS_STAT_FWK_BYTES_RX:
       (*(uint32 *)return_value) =  fwk_stats->bytes_rx;
      break;
     
    case PS_STAT_FWK_BYTES_TX:
       (*(uint32 *)return_value) = fwk_stats->bytes_tx;
     break;
   
     case PS_STAT_FWK_PKTS_DROPPED_RX:
        (*(uint32 *)return_value) = fwk_stats->pkts_dropped_rx;
       break;
    
     case PS_STAT_FWK_PKTS_DROPPED_TX:
        (*(uint32 *)return_value) = fwk_stats->pkts_dropped_tx;
       break;

    default:
      break;
  }
  return ret_val;

}

/*===========================================================================
FUNCTION ds_fwk_get_ulog_handle

DESCRIPTION
  Returns ULOG Handle for DS FWK handler
  
DEPENDENCIES
  None

RETURN VALUE
  ulog_handle

SIDE EFFECTS
  None
===========================================================================*/
ULogHandle ds_fwk_get_ulog_handle()
{
  return ds_fwk_ulog_handle;
}

/*===========================================================================
FUNCTION ds_fwk_get_ulog_handle2

DESCRIPTION
  Returns ULOG Handle 2 for DS FWK handler
  
DEPENDENCIES
  None

RETURN VALUE
  ulog_handle

SIDE EFFECTS
  None
===========================================================================*/
ULogHandle ds_fwk_get_ulog_handle2()
{
  return ds_fwk_ulog_handle2;
}

/*===========================================================================
FUNCTION ds_fwk_ulog_register

DESCRIPTION
  This function call registers DS FWK handler with ULOG framework
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_fwk_ulog_register(void)
{
  ULogResult result = ULogFront_RealTimeInit(&ds_fwk_ulog_handle, 
                               "DS_FWK_ULOG",	  
                               8192,
                               ULOG_MEMORY_LOCAL,
                               ULOG_LOCK_OS);

  DS_MSG1(MSG_LEGACY_HIGH,"DS FWK ULOG REGISTER %d",result);

  //Register 2nd handle
  result = ULogFront_RealTimeInit(&ds_fwk_ulog_handle2, 
                               "DS_FWK_ULOG2",   
                               16384,
                               ULOG_MEMORY_LOCAL,
                               ULOG_LOCK_OS);

  DS_MSG1(MSG_LEGACY_HIGH,"DS FWK ULOG REGISTER %d",result);
}


/*===========================================================================
FUNCTION DS_FWK_IS_RMNET_EMBEDDED_ICMP_PACKET()

DESCRIPTION
  Retrieves the ICMP packet information from RMNET

PARAMETERS
  meta_pkt_info_ptr  : Meta info of the icmp
  ip_vsn             : Ip version of the icmp packet
  pkt_ptr            : Packet pointer
  rm_client_handle   : Rmnet client
  
RETURN VALUE
 Boolean  
 
DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_fwk_is_rmnet_embedded_icmp_packet
(
  ip_pkt_info_type      *meta_pkt_info_ptr,
  ip_version_enum_type   ip_vsn,
  dsm_item_type         *pkt_ptr,
  uint8                  rm_client_handle
)
{
  uint16                 id = 0;
  rmnet_smi_info_type   *ip_info = NULL;
  uint16                 sequence_num = 0;
  uint8                  i = 0;
 /*-----------------------------------------------------------*/

  if((meta_pkt_info_ptr == NULL)||(pkt_ptr == NULL))
    DS_ASSERT(0);

  //Find out the RMNET instance from rm client_handle
  ip_info = rmnet_get_smi_info_ptr(rm_client_handle);
  
  if(ip_info == NULL )
    return FALSE;

  id = meta_pkt_info_ptr->ptcl_info.icmp.id;
  sequence_num = meta_pkt_info_ptr->ptcl_info.icmp.sequence_num;  
  
  DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_get_rmnet_embedded_icmp_packet "
          "ICMP id %d sequnece number %d", id,sequence_num);

  for(i = 0; i < RMNET_MAX_ICMP_OUTSTANDING_REQ; i++)
  {
    if((id == ip_info->icmp_data[i].identifier) && 
       (sequence_num == ip_info->icmp_data[i].sequence_num))
    {
      ip_info->icmp_data[i].identifier = -1;
      ip_info->icmp_data[i].sequence_num = -1;
      return TRUE;
    }
  }
  return FALSE;
}

/*===========================================================================
FUNCTION DS_FWK_SET_RMNET_EMBEDDED_ICMP_PACKET_INFO()

DESCRIPTION
  Stores the ICMP packet information

PARAMETERS
  meta_pkt_info_ptr  : Meta info of the icmp
  ip_vsn             : Ip version of the icmp packet
  pkt_ptr            : Packet pointer
  rm_client_handle   : Rmnet client

RETURN VALUE
 Boolean  
 
DEPENDENCIES  
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_fwk_set_rmnet_embedded_icmp_packet_info
(
  ip_pkt_info_type      *meta_pkt_info_ptr,
  ip_version_enum_type   ip_vsn,
  dsm_item_type         *pkt_ptr,
  uint8                  rm_client_handle
)
{
  uint16                 id = 0;
  rmnet_smi_info_type  * ip_info = NULL;
  uint16                 sequence_num = 0;
  uint8                  i = 0;
 /*-----------------------------------------------------------*/

  if((meta_pkt_info_ptr == NULL)||(pkt_ptr == NULL))
    DS_ASSERT(0);

  //Find out the RMNET instance from rm client_handle
  ip_info = rmnet_get_smi_info_ptr(rm_client_handle);
  
  if(ip_info == NULL )
    return FALSE;

  id = meta_pkt_info_ptr->ptcl_info.icmp.id;
  sequence_num = meta_pkt_info_ptr->ptcl_info.icmp.sequence_num;
  
  for(i = 0; i < RMNET_MAX_ICMP_OUTSTANDING_REQ; i++)
  {
    if((-1 == ip_info->icmp_data[i].identifier) && 
       (-1 == ip_info->icmp_data[i].sequence_num))
    {
      ip_info->icmp_data[i].identifier = id;
      ip_info->icmp_data[i].sequence_num = sequence_num;
      return TRUE;
    }
  }
  
  DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_set_rmnet_embedded_icmp_packet "
          "ICMP packet info could not be stored in cache. "
          "Identifier %d Sequence Number %d",
           id,sequence_num);
  return FALSE;
}

/*============================================================
FUNCTION DS_FWK_GET_EXTERNAL_IPV6_ADDRS()

DESCRIPTION
  This function will get the external address
  
PARAMETERS
  ds_fwk_inst_ptr:   - IN ptr to PDN control block on which to operate on.
  ip_addr_ptr:    value return - the address will be stored here

RETURN VALUE
  0 on successful get of ipv6 extrenal address
 -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
boolean ds_fwk_get_external_ipv6_addrs
(
  ds_fwk_s_type           *ds_fwk_inst_ptr,
  struct ps_in6_addr      *ip_addr_ptr
  
)
{
  ds_fwk_v6_addr_s_type       *external_addr = NULL;
  uint8                        addr_index = 0;
/*----------------------------------------------------------------*/
  /* Validate the Passed Framework instance. */
  if(!ds_fwk_inst_is_valid_ex(ds_fwk_inst_ptr, IPV6_ADDR))
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_get_ipv6_addr(): Invalid Fwk Instance.");
    return FALSE;  
  }
  if(ip_addr_ptr == NULL)
  {
    return FALSE;
  }
  ip_addr_ptr->ps_s6_addr64[0] = ds_fwk_inst_ptr->\
              v6_net_info.ipv6_addrs[DS_DEFAULT_V6_INDEX]->prefix;
           
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 /*-------------------------------------------------------------------------
    If we hit NULL there is space available. Start from 1 as 0 is always the
    public addr.
  -------------------------------------------------------------------------*/
  for(addr_index = DS_MAX_IPV6_PREFIXES; addr_index < DS_MAX_IPV6_ADDRS; addr_index++)
  {
    external_addr = ds_fwk_inst_ptr->v6_net_info.ipv6_addrs[addr_index];
    if(external_addr != NULL)
    {
      if((external_addr->addr_type == DS_IPV6_ADDR_TYPE_EXTERNAL)&&
        (external_addr->client_id == DS_FWK_CLIENT_RMNET) &&
        (external_addr->is_psm_restore == TRUE))
      {
        ip_addr_ptr->ps_s6_addr64[1] = external_addr->iid;
        DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_get_external_addrs %d",
                external_addr->is_psm_restore);
  
        return TRUE;
      }
    }
  } /* end for */
  return FALSE;
}

#ifdef FEATURE_DATA_PS_464XLAT
/*============================================================
  FUNCTION DS_FWK_CLAT_REG_PDN_CONTEXT()

  DESCRIPTION
   Reg the CLAT PDN context
      
  PARAMETERS
   reg_info_ptr  : CLAT PDN context

  RETURN VALUE
    int

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
int ds_fwk_clat_reg_pdn_context 
(
  ds_fwk_clat_reg_pdn_cntxt_info_type *reg_info_ptr
)
{ 
  ds_fwk_s_type                         *ds_fwk_inst = NULL;
  ds_fwk_error_enum_type                 result;
  ds_fwk_clat_pdn_cntxt_info_type       *clat_pdn_cntxt = NULL;
/*-------------------------------------------------------------------*/  
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  do
  {
    if(reg_info_ptr == NULL) 
    {
      DS_ASSERT(0);
      result = DS_FWK_ERROR_1;
      break;
    }
    if(reg_info_ptr->fwk_inst_ptr == NULL) 
    {
      result = DS_FWK_ERROR_2;
      break;
    }

    if(FALSE == (ds_fwk_inst_is_valid_ex(reg_info_ptr->fwk_inst_ptr, IPV6_ADDR)))
    {
      result =  DS_FWK_ERROR_3;
      break;
    }
    ds_fwk_inst = reg_info_ptr->fwk_inst_ptr;
  /*-------------------------------------------------------------------------
        Allocate memory for clat context
        Move it to dpm structure for 9x45. It is currently being stored in
        iface because 3gpp2 doesnt register with DPM
     -------------------------------------------------------------------------*/
    DS_SYSTEM_HEAP_MEM_ALLOC(clat_pdn_cntxt, 
                             sizeof(ds_fwk_clat_pdn_cntxt_info_type),
                            ds_fwk_clat_pdn_cntxt_info_type*);
    if (NULL == clat_pdn_cntxt)
    {
      result = DS_FWK_ERROR_5;
      break;
    }
    clat_pdn_cntxt->v4_client_addr = 
       reg_info_ptr->v4_client_addr;
    clat_pdn_cntxt->v6_dev_prefix_len = 
       reg_info_ptr->v6_dev_prefix_len;
    clat_pdn_cntxt->v6_plat_prefix_len = 
       reg_info_ptr->v6_plat_prefix_len;
    clat_pdn_cntxt->client_pdn_handle = 
       reg_info_ptr->client_pdn_handle;	   
    clat_pdn_cntxt->v6_own_addr_len = 
       reg_info_ptr->v6_own_addr_len;
    clat_pdn_cntxt->v6_te_addr_len = 
       reg_info_ptr->v6_te_addr_len;
    clat_pdn_cntxt->tos_ignore_bit_flag =
      reg_info_ptr->tos_ignore_bit_flag;
    clat_pdn_cntxt->tos_override_val =
      reg_info_ptr->tos_override_val;

    memscpy(&(clat_pdn_cntxt->v6_dev_prefix),
              sizeof(struct ps_in6_addr),
             &(reg_info_ptr->v6_dev_prefix),
             sizeof(struct ps_in6_addr));
    memscpy(&(clat_pdn_cntxt->v6_plat_prefix),
             sizeof(struct ps_in6_addr),
            &(reg_info_ptr->v6_plat_prefix),
             sizeof(struct ps_in6_addr));	 
    memscpy(&(clat_pdn_cntxt->v6_own_addr),
               sizeof(struct ps_in6_addr),
             &(reg_info_ptr->v6_own_addr),
             sizeof(struct ps_in6_addr));  
    memscpy(&(clat_pdn_cntxt->v6_te_addr),
              sizeof(struct ps_in6_addr),
            &(reg_info_ptr->v6_te_addr),
             sizeof(struct ps_in6_addr));  

    IPV6_ADDR_MSG(clat_pdn_cntxt->v6_dev_prefix.ps_s6_addr64);
    IPV6_ADDR_MSG(clat_pdn_cntxt->v6_plat_prefix.ps_s6_addr64);   
    IPV6_ADDR_MSG(clat_pdn_cntxt->v6_own_addr.ps_s6_addr64);	
    IPV6_ADDR_MSG(clat_pdn_cntxt->v6_te_addr.ps_s6_addr64);
    //clat_pdn_cntxt->client_pdn_handle = clat_pdn_cntxt;
    ds_fwk_inst->clat_pdn_cntxt = (void *)clat_pdn_cntxt;
    ds_fwk_inst->is_clat_capable = TRUE;
    result = DS_FWK_SUCCESS;
  } while(0);
  
  if (result != DS_FWK_SUCCESS)
  {
    DS_SYSTEM_HEAP_MEM_FREE(clat_pdn_cntxt);
    if (ds_fwk_inst)
    { 
      ds_fwk_inst->clat_pdn_cntxt = NULL;
      ds_fwk_inst->is_clat_capable = FALSE;
    }  
  }
  
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section); 
  DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_clat_reg_pdn_context() : result %d ds_fwk_inst %x", 
          result, ds_fwk_inst);    
  return result;
} /* ps_dpm_clati_reg_pdn_cntxt_cmd_hdlr */

/*============================================================
  FUNCTION DS_FWK_CLAT_DEREG_PDN_CONTEXT()

  DESCRIPTION
   Deregs the CLAT PDN context
 
  PARAMETERS
   dereg_info_ptr : Pointer to clat pdn contxt

  RETURN VALUE
    int

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
int ds_fwk_clat_dereg_pdn_context
(
  ds_fwk_clat_dereg_pdn_cntxt_info_type    *dereg_info_ptr
)
{
  ds_fwk_s_type                          *ds_fwk_inst = NULL;
  ds_fwk_error_enum_type                  result;
  ds_fwk_clat_pdn_cntxt_info_type        *clat_pdn_cntxt = NULL;
/*-------------------------------------------------------------------*/
  if (NULL == dereg_info_ptr)
  {
    DS_ASSERT(0);
    return DS_FWK_ERROR_1;
  }
  DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_clat_dereg_pdn_context(): fwk_inst 0x%x client_pdn_handle; %x", 
          dereg_info_ptr->fwk_inst_ptr,
          dereg_info_ptr->client_pdn_handle);

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);  
  do
  {
    if (NULL == dereg_info_ptr->fwk_inst_ptr)
    {
      result = DS_FWK_ERROR_2;
      break;
    }
    if (NULL == dereg_info_ptr->client_pdn_handle)
    {
      result =  DS_FWK_ERROR_3;
      break;
    }
    if(FALSE == (ds_fwk_inst_is_valid_ex(dereg_info_ptr->fwk_inst_ptr, IPV6_ADDR)))
    {
      result =  DS_FWK_ERROR_4;
      break;
    }
    ds_fwk_inst = dereg_info_ptr->fwk_inst_ptr;
    clat_pdn_cntxt = 
    (ds_fwk_clat_pdn_cntxt_info_type *)ds_fwk_inst->clat_pdn_cntxt;
    if( clat_pdn_cntxt != NULL && 
        clat_pdn_cntxt->client_pdn_handle != dereg_info_ptr->client_pdn_handle)
    {
      DS_ASSERT(0);  
      result =  DS_FWK_ERROR_5;
    }
   /*-----------------------------------------------------------------------
     For CLAT case, only v6 iface would be up and used. 
     1. Deregister CLAT PDN context with hardware 
     2. Delete v6 destination filter for CLAT  
     3. Free CLAT pdn context
   -----------------------------------------------------------------------*/ 
    DS_SYSTEM_HEAP_MEM_FREE(clat_pdn_cntxt);
    ds_fwk_inst->clat_pdn_cntxt = NULL;
    ds_fwk_inst->is_clat_capable = FALSE;
    result =  DS_FWK_SUCCESS;
  } while(0);
  
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_clat_dereg_pdn_context() : result %d ",  
          result);	  
  return DS_FWK_SUCCESS;
}
#endif
