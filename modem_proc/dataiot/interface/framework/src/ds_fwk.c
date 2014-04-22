/*==========================================================*/
/*!
  @file 
  ds_fwk.c

  @brief
  This file provides functions that are used to perform initializations 
  of netiface module.

  Copyright (c) 2018-2021 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*================= =========================================*/
/*============================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/interface/framework/src/ds_fwk.c#16 $
  $DateTime: 2022/05/27 04:18:53 $$Author: pwbldsvc $

  when           who    what, where, why
  ---------- --- ----------------------------------------------------------------
  2020-05-15 mkumarsa  Added fix to check the bearer state before pick packet from a2 watermark.
  2018-05-14 svaka  Created module.

============================================================*/

/*--------------------------------------------------------------------------- 
  Include Files
---------------------------------------------------------------------------*/
#include "ds_headers.h"
#include "ds_fwki.h"
#include "ds_pdn_ioctl.h"
#include "ds_ipfltr.h"
#include "ds_sys_eventi.h"
#include "ds_bearer_manager.h"
#include "ds_pdn_manager.h"
#include "ds_flow.h"
#include "ps_svc.h"
#include "a2_dl_per.h"
#include "a2_log_ext.h"
#include "ps_logging.h"
#include "ps_addri_mgmt.h"
#include "ps_addri_v6.h"
#include "ds_qos_net_initiated_req.h"
#include "ps_logging_diag.h"
#include "ds_fwk_diag_test_tool.h"
#include "ds_crit_sect.h"
#ifdef FEATURE_DATAMODEM_DNS
#include "ps_dns_cache.h"
#endif
#include "ds_fwk_opt_defs.h"
#include "ps_pkt_info_utils.h"
#include "ds_a2_api.h"
#include "ds_rmnet_meta_sm.h"
#include "rmnet_opt_defs.h"
#include "ps_metai_info.h"
#include "ds_pdn_cp_data_hdlr.h"


#define   DS_FWK_MAX_BEARER     DS_3GPP_MAX_BEARER_CONTEXT
#define   DS_FWK_MAX_DPL_INST  DS_3GPP_MAX_INSTANCE
#define   DS_FWK_HW_PS_WM_DNE   305000
#define   DS_FWK_HW_PS_WM_HI    225000
#define   DS_FWK_HW_PS_WM_LO    120000
/*Discard  received DSM item when DNE packet count hit*/
#define   DS_FWK_HW_PS_WM_DNE_PKT_COUNT 1000
#define   DS_FWK_DL_THROUGHPUT_ENHANCEMENT_DISABLED     1

/*---------------------------------------------------------------------------
  Static declarations.
---------------------------------------------------------------------------*/
static ds_fw_mh_info_type       * ds_fw_mh_info[DS_FWK_MH_MAX];
static ds_fwk_event_buf_type    * ds_fwk_event_buf[DS_FWK_CLIENT_MAX];
static ds_fwk_s_type            * ds_fwk_inst[DS_FWK_MAX_INST];
static ds_fwk_bearer_info_type    ds_fwk_dl_sig_in_use_tbl[DS_FWK_MAX_BEARER];
static boolean                    ds_fwk_event_trigger_flag;
static dpl_cp_iid_info_type       dpl_ifname_by_rm_instance[DS_FWK_MAX_DPL_INST];
static dpl_cp_iid_info_type       dpl_ifname_by_um_pdn[DS_FWK_MAX_DPL_INST];
static ds_fwk_tx_buf_type       * ds_fwk_tx_buf[DS_FWK_CLIENT_MAX];
static ds_fwk_tx_nipd_buf_type  * ds_fwk_tx_nipd_buf[DS_FWK_CLIENT_MAX];
ds_fwk_global_info_type           ds_fwk_global_info;
static boolean   ds_fwk_dl_tpt_val = DS_FWK_DL_THROUGHPUT_ENHANCEMENT_DISABLED;
/*============================================================
FUNCTION DS_FWK_INIT()

DESCRIPTION
  This function initializes the framework related varibales as part of bootup.
  
PARAMETERS
  none
  
RETURN VALUE
  void

DEPENDENCIES
  none

SIDE EFFECTS
  none
============================================================*/
void ds_fwk_init()
{
  int j = 0;
/*-------------------------------------------------------------------------*/
   ds_sys_init();
  
  for(j = 0 ;j < DS_FWK_MAX_DPL_INST;j++)
  {
     dpl_ifname_by_rm_instance[j].dpl_id = DPL_IID_IFNAME_SIO_IFACE_RMNET_1 + j;
     dpl_ifname_by_rm_instance[j].in_use = FALSE;
  }
  for(j = 0 ;j < DS_FWK_MAX_DPL_INST;j++)
  {
     dpl_ifname_by_um_pdn[j].dpl_id= DPL_IID_IFNAME_3GPP_CONTEXT_0 + j;
     dpl_ifname_by_um_pdn[j].in_use = FALSE;
  }

#ifndef TEST_FRAMEWORK
    a2_dl_per_register_wm_dereg_done_cb(&ds_fwk_dereg_ds_pdcp_wm_cb);
#endif                                 
  
  memset(&ds_fwk_global_info,0,sizeof(ds_fwk_global_info_type));

  ds_fwk_diag_init();
}

/*============================================================
FUNCTION DS_FWK_INST_IS_VALID_EX()

DESCRIPTION
  Returns whether the pdn  of fraemwork instance pointer passed is valid or not
  
PARAMETERS
  ds_fwk_inst_ptr - framework instance
  ip_ver - ip version
  
RETURN VALUE
  TRUE if instance is valid
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
boolean ds_fwk_inst_is_valid_ex
(
  ds_fwk_s_type*          ds_fwk_inst_ptr,
  ip_addr_enum_type       ip_ver	
)
{
  uint8 index = 0;
  
  if ( ds_fwk_inst_ptr == NULL )
  {
    return FALSE;
  }
  while( index < DS_FWK_MAX_INST )
  {
    if( ds_fwk_inst[index] == ds_fwk_inst_ptr )
    {
      return ds_fwk_inst_is_valid(ds_fwk_inst_ptr,ip_ver);
    }
    index++;
  }
  return FALSE;
}

/*============================================================
FUNCTION DS_FWK_GET_INST_PTR_FROM_INDEX()

DESCRIPTION
  gets the framework instance ptr of passed index.
  
PARAMETERS
  framework index
  
RETURN VALUE
  pointer to framework instance if instance is allocated
  NULL otherwise

DEPENDENCIES
  none

SIDE EFFECTS
  none
============================================================*/
ds_fwk_s_type* ds_fwk_get_inst_ptr_from_index
(
  ds_fwk_index_type index
)
{
  if( ds_fwk_inst[index] != NULL)
  {
     return ds_fwk_inst[index];
  }
  return NULL;
}

/*============================================================
FUNCTION DS_FWK_INST_IS_VALID()

DESCRIPTION
  returns whether the pdn instance of fraemwork instance pointer passed is valid or not
  
PARAMETERS
  ds_fwk_inst_ptr - framework instance
  ip_ver - ip version
  
RETURN VALUE
  TRUE if instance is valid
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
boolean ds_fwk_inst_is_valid
(
  ds_fwk_s_type*          ds_fwk_inst_ptr,
  ip_addr_enum_type       ip_ver	
)
{
  if ( ds_fwk_inst_ptr == NULL )
  {
    return FALSE;
  }
  if(ip_ver == IPV4_ADDR)
  {
    if(ds_fwk_inst_ptr->v4_pdn_ptr != NULL)
       return TRUE;
  }
  else if(ip_ver == IPV6_ADDR)
  {
    if(ds_fwk_inst_ptr->v6_pdn_ptr != NULL)
       return TRUE;
  }
  else if(ip_ver == NON_IP_ADDR)
  {
    if(ds_fwk_inst_ptr->non_ip_pdn_ptr != NULL)
       return TRUE;
  }
  return FALSE;
}


/*============================================================
FUNCTION DS_FWK_VALIDATE_MH_INFO()

DESCRIPTION
  returns whether the Mode handler is registered to framework or not
  
PARAMETERS
  mh_id - mode handler ID 
    
RETURN VALUE
  TRUE if MH is valid
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
static boolean ds_fwk_validate_mh_info(uint8 mh_id)
{
  DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_validate_mh_info:mh_id %d",mh_id);
  if( mh_id < DS_FWK_MH_3GPP || mh_id >= DS_FWK_MH_MAX )
  {
    return FALSE;
  }
  if( ds_fw_mh_info[mh_id] == NULL )
  {
    return FALSE;
  }
  return TRUE;
}


/*============================================================
FUNCTION DS_FWK_VALIDATE_CLIENT_ID()

DESCRIPTION
  returns whether the client is valid or not
  
PARAMETERS
  client_id
  
RETURN VALUE
  TRUE if client is valid
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
=============================================================*/
static boolean ds_fwk_validate_client_id(ds_fwk_client_type client_id)
{
  DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_validate_client_id:client_id %d ",client_id);
  if( client_id >= DS_FWK_CLIENT_MAX|| client_id <= DS_FWK_CLIENT_INVALID)
  {
    return FALSE;
  }
  return TRUE;
}


/*============================================================
FUNCTION DS_FWK_VALIDATE_CLIENT_HANDLE()

DESCRIPTION
  returns whether the client handle passed is in the proper range or not
  
PARAMETERS
  client handle
  
RETURN VALUE
  TRUE if client handle is valid
  FALSE otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
=============================================================*/
static ds_fwk_index_type ds_fwk_validate_client_handle
(
  ds_fwk_clnt_handle_type  clnt_handle
)
{
   DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_validate_client_handle clnt_handle %d",
                            clnt_handle);
  if( clnt_handle < DS_FWK_MIN_CLNT_HANDLE || 
      clnt_handle > DS_FWK_MAX_CLNT_HANDLE )
  {
    return FALSE;
  }
  return TRUE;
}


/*============================================================
FUNCTION DS_FWK_GET_CLIENT_INFO_FROM_HANDLE()

DESCRIPTION
   fills the framework instance and ip version of passed client handle
  
PARAMETERS
  clnt_type - type of client
  clnt_handle - client handle
  clnt_info_ptr - information of client
  
RETURN VALUE
  void

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
void ds_fwk_get_clnt_info_from_handle
(
  ds_fwk_client_type        clnt_type,
  ds_fwk_clnt_handle_type   clnt_handle,
  ds_fwk_clnt_info_s_type  *clnt_info_ptr
)
{
  /*------------------------------Local Declarations-----------------------------*/
  uint8                    index = 0;
  /*--------------------------------------------------------------------------*/

  clnt_info_ptr->fw_inst = NULL;
  clnt_info_ptr->ip_type = IP_ADDR_INVALID;

  if( !ds_fwk_validate_client_handle(clnt_handle))
  {
    return;
  }
  
  while( index < DS_FWK_MAX_INST )
  {
    if( ds_fwk_inst[index] != NULL)
    {
      if(ds_fwk_inst[index]->client_info[clnt_type].\
                       v4_client_handle == clnt_handle )
      {
        clnt_info_ptr->fw_inst = ds_fwk_inst[index];
        clnt_info_ptr->ip_type = IPV4_ADDR;
        return;
      }
      else if(ds_fwk_inst[index]->client_info[clnt_type].\
                       v6_client_handle == clnt_handle )
      {
        clnt_info_ptr->fw_inst = ds_fwk_inst[index];
        clnt_info_ptr->ip_type = IPV6_ADDR;
        return;
      }
      else if(ds_fwk_inst[index]->client_info[clnt_type].\
                       nipd_client_handle == clnt_handle )
      {
        clnt_info_ptr->fw_inst = ds_fwk_inst[index];
        clnt_info_ptr->ip_type = NON_IP_ADDR;
        return;
      }
    }
    index++;
  }
  
  DS_MSG2(MSG_LEGACY_ERROR,"no matching instance"
                           "clnt_handle %d fw_inst %d",clnt_handle,index);
}


/*============================================================
FUNCTION DS_FWK_GET_INDEX_FROM_CLIENT_HANDLE()

DESCRIPTION
  returns FW instance index based on client type and handle
  
PARAMETERS
  clnt_type
  clnt_handle
  
RETURN VALUE
  framework instance index

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
static ds_fwk_index_type ds_fwk_get_index_from_client_handle
(
  ds_fwk_client_type      clnt_type,
  ds_fwk_clnt_handle_type clnt_handle
)
{
  /*---------------------------Local Declarations--------------------------------*/
  uint8 index = 0;
  /*--------------------------------------------------------------------------*/
  if( !ds_fwk_validate_client_handle(clnt_handle))
  {
    return -1;
  }
  
  while( index < DS_FWK_MAX_INST )
  {
    if( ds_fwk_inst[index] != NULL)
    {
      if( (ds_fwk_inst[index]->client_info[clnt_type].\
                        v4_client_handle == clnt_handle )||
          (ds_fwk_inst[index]->client_info[clnt_type].\
                        nipd_client_handle == clnt_handle )||
          (ds_fwk_inst[index]->client_info[clnt_type].\
                        v6_client_handle == clnt_handle ))
      {
        DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_get_index_from_client_handle:"
                                 "clnt_handle %d fw_inst %d",clnt_handle,index);
        return index;
      }
    }
    index++;
  }
  
  return DS_FWK_INST_INVALID;
}

/*============================================================
FUNCTION DS_FWKI_GET_FWK_INSTANCE_FROM_BEARER()

DESCRIPTION
  returns associated framework instance ptr for bearer.
  
PARAMETERS
  bearer_ptr
  
RETURN VALUE
  framework instance ptr

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
ds_fwk_s_type* ds_fwki_get_fwk_instance_from_bearer
(
  ds_bearer_mgr_s * bearer_ptr
)
{
  if(TRUE == ds_bearer_mgr_validate_bearer_ptr(bearer_ptr))
  {
    if( bearer_ptr != NULL && bearer_ptr->pdn_mgr_ptr != NULL )
    {
      if( bearer_ptr->pdn_mgr_ptr->fw_index >= 0 &&
          bearer_ptr->pdn_mgr_ptr->fw_index < DS_FWK_MAX_INST )
      {
        DS_MSG1(MSG_LEGACY_HIGH,"ds_fwki_get_fwk_instance_from_bearer index %d",
                               bearer_ptr->pdn_mgr_ptr->fw_index);
        return ds_fwk_inst[bearer_ptr->pdn_mgr_ptr->fw_index];
      }
    }
  }  
  return NULL;
}

/*============================================================
FUNCTION DS_FWK_GET_PDN_INFO_FROM_CLIENT_HANDLE()

DESCRIPTION
  Gets the PDN info from client handle
  
PARAMETERS
  Clnt_type - type of client
  clnt_handle - client handle
  index -- framework index
  pdn ptr(out)  -- pdn information
  
RETURN VALUE
    0 if success
  -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
static int ds_fwk_get_pdn_info_from_client_handle
(
  ds_fwk_client_type       clnt_type,
  ds_fwk_clnt_handle_type  clnt_handle,
  ds_fwk_index_type        index,
  ds_fwk_pdn_info            * pdn_ptr
)
{
  DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_get_pdn_info_from_client_handle clnt_handle %d",
                           clnt_handle);
  if( !ds_fwk_validate_client_handle(clnt_handle))
  {
    return -1;
  }
  
  if( ds_fwk_inst[index] != NULL)
  {
    if(ds_fwk_inst[index]->client_info[clnt_type].\
                       v4_client_handle == clnt_handle )
    {
      pdn_ptr->pdn_ptr = ds_fwk_inst[index]->v4_pdn_ptr;
      pdn_ptr->ip_type = IPV4_ADDR;
      return 0;
    }
    else if(ds_fwk_inst[index]->client_info[clnt_type].\
                       v6_client_handle == clnt_handle )
    {
      pdn_ptr->pdn_ptr = ds_fwk_inst[index]->v6_pdn_ptr;
      pdn_ptr->ip_type = IPV6_ADDR;
      return 0;
    }
    else if(ds_fwk_inst[index]->client_info[clnt_type].\
                       nipd_client_handle == clnt_handle )
    {
      pdn_ptr->pdn_ptr = ds_fwk_inst[index]->non_ip_pdn_ptr;
      pdn_ptr->ip_type = NON_IP_ADDR;
      return 0;
    }
  }
  
  return -1;
}

/*============================================================
FUNCTION DS_FWK_SET_PDN_INFO()

DESCRIPTION
  sets the pdn information to framework instance
  
PARAMETERS
  fwk_index
  pdn_resp_info - response information PDN managet.
  
RETURN VALUE
    0 if success
  -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
static int ds_fwk_set_pdn_info
(
  ds_fwk_index_type            index,
  ds_fwk_response_info_s_type *pdn_resp_info
)
{
  DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_set_pdn_info index %d",index);
  if( ds_fwk_inst[index] != NULL && pdn_resp_info != NULL )
  {
    if(pdn_resp_info->ip_type == IPV4_ADDR)
    {
      ds_fwk_inst[index]->v4_pdn_ptr = pdn_resp_info->pdn_ptr;
      return 0;
    }
    else if(pdn_resp_info->ip_type == IPV6_ADDR)
    {
      ds_fwk_inst[index]->v6_pdn_ptr = pdn_resp_info->pdn_ptr;
      return 0;
    }
    else if(pdn_resp_info->ip_type == NON_IP_ADDR )
    {
      ds_fwk_inst[index]->non_ip_pdn_ptr = pdn_resp_info->pdn_ptr;
      return 0;
    }
  }
  
  return -1;
}



/*---------------------------------------------------------------------------
external functions
---------------------------------------------------------------------------*/
/*============================================================
FUNCTION DS_FWK_REGISTER_MH()

DESCRIPTION
 regsiters MH operations with ds_fwk
 
PARAMETERS
  client_type

RETURN VALUE
    0  -- if reg is success
   -1 -- if failed

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
int ds_fwk_register_mh
(
  ds_fwk_mh_e_type                mh_id,
  ds_fwk_pdn_operations_s_type*   operations
)
{
  /*Check whether MH ID is in the range*/
  /*check if MH is already registered or passed info is NULL*/
  if( ( mh_id < DS_FWK_MH_3GPP   || mh_id >= DS_FWK_MH_MAX )||
        operations == NULL )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_register_mh:invalid param mh_id %d ",mh_id);
    return -1;
  }

  if(ds_fw_mh_info[mh_id] == NULL)
  {
    /*allocate the memory for MH*/
    DS_SYSTEM_HEAP_MEM_ALLOC(ds_fw_mh_info[mh_id],
                             sizeof(ds_fw_mh_info_type),
                             ds_fw_mh_info_type*)
    if( ds_fw_mh_info[mh_id] == NULL)
    {  
      DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_register_mh:"
                               " memory not available for %d ",mh_id);
      return -1;
    }
  }

  /*store the info passed by MH*/
  ds_fw_mh_info[mh_id]->pdn_operations.bringup_f_ptr = operations->bringup_f_ptr;
  ds_fw_mh_info[mh_id]->pdn_operations.teardown_f_ptr = operations->teardown_f_ptr;
  ds_fw_mh_info[mh_id]->pdn_operations.look_up_f_ptr = operations->look_up_f_ptr;
  ds_fw_mh_info[mh_id]->pdn_operations.ioctl_f_ptr = operations->ioctl_f_ptr;
  return 0;
}

/*============================================================
  FUNCTION DS_FWK_REGISTER_MH()

  DESCRIPTION
   gets the current MH
   
  PARAMETERS
   none

  RETURN VALUE
     MH index

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
uint8 ds_fwk_get_current_mh()
{
  /*Currently we are supporting only 3GPP*/
  /*TODO: we have return based on the RAT*/
  return DS_FWK_MH_3GPP;
}

/*============================================================
  FUNCTION DS_FWK_LOOK_UP_PDN_BY_POLICY()

  DESCRIPTION
    Invokes corresponding Mode handler look up api to find the mathcing PDN for given policy.

  PARAMETERS
    client_type
    apn 
    ip type

  RETURN VALUE
    Client handle - if matches to any existing policy
    -1 - if not matches with any existing PDN policy.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
ds_fwk_clnt_handle_type  ds_fwk_look_up_pdn_by_policy
(
  ds_fwk_client_type       client_id,
  ds_policy_info_type      policy_info
)
{
  /*------------------------------Local Declarations-------------------------- --*/
   uint8                     mh_id = ds_fwk_get_current_mh();
   ds_fwk_index_type         fwk_inst = -1;
   ds_fwk_clnt_handle_type   clnt_handle = -1;
  /*--------------------------------------------------------------------------*/
  
   DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_look_up_pdn_by_policy" 
                           "client ID:%d lookup family %d",
                           client_id,policy_info.family);
   if((ds_fwk_validate_mh_info(mh_id) == TRUE) && 
   	  (ds_fw_mh_info[mh_id]->pdn_operations.look_up_f_ptr != NULL))
   {
      fwk_inst = ds_fw_mh_info[mh_id]->pdn_operations.look_up_f_ptr(policy_info);
   }
   if( fwk_inst < 0 || fwk_inst >= DS_FWK_MAX_INST)
   {
     DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_look_up_pdn_by_policy" 
                              "client ID:%d lookup failed",
                              client_id);
     return -1;
   }
   
   DS_MSG3(MSG_LEGACY_HIGH,"ds_fwk_look_up_pdn_by_policy:" 
             "v4 handle %d v6 handle %d nipd handle %d",
             ds_fwk_inst[fwk_inst]->client_info[client_id].v4_client_handle,
             ds_fwk_inst[fwk_inst]->client_info[client_id].v6_client_handle,
             ds_fwk_inst[fwk_inst]->client_info[client_id].nipd_client_handle);
   
   if ( policy_info.family == IPV4_ADDR)
   {
     clnt_handle = ds_fwk_inst[fwk_inst]->client_info[client_id].v4_client_handle;
   }
   else if ( policy_info.family == IPV6_ADDR)
   {
     clnt_handle = ds_fwk_inst[fwk_inst]->client_info[client_id].v6_client_handle;
   }
   else if( policy_info.family == NON_IP_ADDR)
   {
     clnt_handle = ds_fwk_inst[fwk_inst]->client_info[client_id].nipd_client_handle;
   }
   return clnt_handle;
}

/*============================================================
  FUNCTION DS_FWK_CLEAR_V6_ADDRESSES()

  DESCRIPTION
    Frees the v6 addresses corresponding to the client

  PARAMETERS
    fwk_inst -- framework instance
    client  -- client whos address needs to be cleared.

  RETURN VALUE
    0  - if addresses are deletd success
    -1 - if address are not deleted successfully

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
int ds_fwk_clear_v6_addresses
(
  ds_fwk_s_type            *fwk_inst,
  ds_fwk_client_type		client_id
)
{
  ds_fwk_v6_addr_s_type    *v6_addr_ptr;
  uint8	                    addr_index;
  ds_fwk_v6_addr_s_type    *ipv6_addrs[DS_MAX_IPV6_ADDRS] ={0};
  int8                      i = 0;

/*--------------------------------------------------*/
  DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_clear_v6_addresses" 
                           "client_id:%d fwk_inst %p",
                            client_id,fwk_inst);

  if(!DS_FWK_INST_IS_VALID( fwk_inst, IPV6_ADDR ))
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_clear_v6_addresses"
                             "Invalid Framewrk Instance passed %p",fwk_inst);
    return -1;
  }

  switch(client_id)
  {
   case DS_FWK_CLIENT_RMNET:
   case DS_FWK_CLIENT_RMSM:
   case DS_FWK_CLIENT_ATCOP:
   case DS_FWK_CLIENT_PROTO_V6:
#ifdef FEATURE_MODE_TLB
   case DS_FWK_CLIENT_TLB:
#endif
      ps_addr_mgmt_free_ext_v6_addresses(fwk_inst);
   break;
   
   case DS_FWK_CLIENT_SOCKETS:
     for( addr_index = (DS_MAX_IPV6_PREFIXES -1); addr_index < DS_MAX_IPV6_ADDRS; addr_index++)
     {
       ipv6_addrs[addr_index] = fwk_inst->v6_net_info.ipv6_addrs[addr_index];
     }  
     for(i =(DS_MAX_IPV6_PREFIXES);i < DS_MAX_IPV6_ADDRS; i++)
     {    
        v6_addr_ptr = ipv6_addrs[i];
        if((v6_addr_ptr) && (v6_addr_ptr->addr_type == DS_IPV6_ADDR_TYPE_PRIV_UNIQUE))
        {      
          (void) ps_delete_priv_ipv6_addr(fwk_inst, v6_addr_ptr);
        }
     }
     break;
   case DS_FWK_CLIENT_INVALID:
      ps_addr_mgmt_free_ext_v6_addresses(fwk_inst);
      for( addr_index = (DS_MAX_IPV6_PREFIXES -1); addr_index < DS_MAX_IPV6_ADDRS; addr_index++)
      { 
         ipv6_addrs[addr_index] = fwk_inst->v6_net_info.ipv6_addrs[addr_index];
      }
      for(i =(DS_MAX_IPV6_PREFIXES);i < DS_MAX_IPV6_ADDRS; i++)
      {    
        v6_addr_ptr = ipv6_addrs[i];
        if(v6_addr_ptr)
        {      
             (void) ps_delete_priv_ipv6_addr(fwk_inst, v6_addr_ptr);
        }
      }
     break;
   default:
    break;
  }

 return 0;
}

/*============================================================
  FUNCTION DS_FWK_DEALLOC_IID()

  DESCRIPTION
    DeaLlocs the unique iid assigne to thefwk instance

  PARAMETERS
    dpl_cb
    family 

  RETURN VALUE
   None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
void ds_fwk_dealloc_iid
(
  dpl_net_cb            *dpl_cb,
  ip_addr_enum_type      family
)
{
  uint8     i =0;
  int index =0;
  /*--------------------------------------------------*/

  DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_dealloc_iid family %d ",family);
  if(family == IPV4_ADDR)
  {
     index =0;
  }
  else if(family == IPV6_ADDR)
  {
    index =1;
  }

  for(i =0 ; i< DS_FWK_MAX_INST;i++)
  {
    if((dpl_cb->dpl_id.ifname == dpl_ifname_by_rm_instance[index].dpl_id)&&
    (dpl_ifname_by_rm_instance[index].in_use ==TRUE))
    {
        dpl_ifname_by_rm_instance[index].in_use = FALSE;
        DS_MSG2(MSG_LEGACY_HIGH,"Found dplid in rmnet pool for clearing index %d"
                              " dpl_cb->dpl_id.ifname%d ",index,
                dpl_ifname_by_rm_instance[index].dpl_id);          
        break;
    }
    else if((dpl_cb->dpl_id.ifname == dpl_ifname_by_um_pdn[index].dpl_id)&&
    (dpl_ifname_by_um_pdn[index].in_use == TRUE))
    {
      dpl_ifname_by_um_pdn[index].in_use = FALSE;
      DS_MSG2(MSG_LEGACY_HIGH,"Found dplid in um pool for clearing index %d" 
                              "dpl_cb->dpl_id.ifname%d ",index,
                dpl_ifname_by_rm_instance[index].dpl_id); 
      break;
    }
    index = index +2;
  }
}

/*============================================================
  FUNCTION DS_FWK_ASSIGN_IID()

  DESCRIPTION
    Assigns the unique iid to a fwk instance

  PARAMETERS
    dpl_cb
    family 
    client_id

  RETURN VALUE
   None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/

void ds_fwk_assign_iid
(
  dpl_net_cb       *dpl_cb,
  ip_addr_enum_type family,
  ds_dpl_client_type client_id
)
{
  uint8 index =0;
  int   j =0;
/*--------------------------------------------------*/
  DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_assign_iid family %d client_id %d",
                           family,client_id);
  if(family == IPV4_ADDR)
  {
    index =0;
  }
  else if(family == IPV6_ADDR)
  {
    index =1;
  }
  else 
    return;

  for(j = 0;j < DS_FWK_MAX_INST ; j++)
  {
    if(client_id == DS_DPL_CLIENT_RM)
    {
      //Check from the global table which iid are free for the client
      if(dpl_cb->dpl_id.ifname == 0)
      {
        if(dpl_ifname_by_rm_instance[index].in_use == FALSE)       
        {
          dpl_cb->dpl_id.ifname = dpl_ifname_by_rm_instance[index].dpl_id;
          dpl_ifname_by_rm_instance[index].in_use = TRUE;         
          DS_MSG2(MSG_LEGACY_HIGH,"Found dplid in rmnet pool" 
                                  "index %d dpl_cb->dpl_id.ifname %d ",
                                   index,dpl_cb->dpl_id.ifname);
         break;
        }
      }
    }
    else if(client_id == DS_DPL_CLIENT_UM)
    {
      //Check from the global table which iid are free for the client
      if(dpl_ifname_by_um_pdn[index].in_use == FALSE)
      {
        dpl_cb->dpl_id.ifname = dpl_ifname_by_um_pdn[index].dpl_id;
        dpl_ifname_by_um_pdn[index].in_use = TRUE;        
        DS_MSG2(MSG_LEGACY_HIGH,"Found dplid in um pool" 
                                "index %d dpl_cb->dpl_id.ifname %d",
                                index,dpl_cb->dpl_id.ifname);
        break;
      }
    }
   index = index +2;
  }   

}

/*===========================================================================
  FUNCTION DS_FWK_ALLOC_ATTACH_INST()

  DESCRIPTION
   allocates attach framework instance when invoked from MH

  PARAMETERS
    pdn_ptr 
    ip type

  RETURN VALUE
    fwk instance index

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
ds_fwk_index_type ds_fwk_alloc_attach_inst
(
  ds_pdn_mgr_s*      pdn_ptr,
  ip_addr_enum_type  ip_type,
  boolean            is_default
)
{
  ds_fwk_index_type ds_fwk_index = 0;
  if(pdn_ptr->fw_index == DS_FWK_INST_INVALID)
  {
    ds_fwk_index = ds_fwk_alloc_inst();
  }
  else
  {
    ds_fwk_index = pdn_ptr->fw_index;
  }
  DS_MSG3(MSG_LEGACY_HIGH,"ds_fwk_alloc_attach_inst:" 
                          "family %d fw_index %d is_default %d",
                          ip_type,ds_fwk_index,is_default);
  ds_fwk_inst[ds_fwk_index]->attach_inst = is_default;
  if(ip_type == IPV4_ADDR)
    ds_fwk_inst[ds_fwk_index]->v4_pdn_ptr = pdn_ptr;
  if(ip_type == IPV6_ADDR)
    ds_fwk_inst[ds_fwk_index]->v6_pdn_ptr = pdn_ptr;
  if(ip_type == NON_IP_ADDR)
    ds_fwk_inst[ds_fwk_index]->non_ip_pdn_ptr = pdn_ptr;

  if(is_default)
  {
    dpli_log_iid_desc_and_status(ds_fwk_inst[ds_fwk_index], IPV4_ADDR, DS_DPL_CLIENT_UM,NULL);  
    dpli_log_iid_desc_and_status(ds_fwk_inst[ds_fwk_index], IPV6_ADDR, DS_DPL_CLIENT_UM,NULL);
  }
  return ds_fwk_index;	
}

/*============================================================
  FUNCTION DS_FWK_ALLOC_INST()

  DESCRIPTION
   allocates free framework instance when invoked from MH

  PARAMETERS
    none

  RETURN VALUE
    fwk instance index

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
ds_fwk_index_type ds_fwk_alloc_inst()
{
  /*------------------------------Local Declarations-----------------------------*/
  ds_fwk_index_type ds_fwk_index = 0;
  ds_fwk_client_type clnt_id = 0;
  uint8 index = 0;
  /*--------------------------------------------------------------------------*/
  
  //loop in to find the free instance
  while(ds_fwk_inst[ds_fwk_index] != NULL)
  {
    ds_fwk_index++;
  };
  
  if(ds_fwk_index >=DS_FWK_MAX_INST)
  {  
     DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_alloc_inst: failed" 
                              "max instance already in use",ds_fwk_index);
     return -1;
  }
  
  /*allocate the memory for MH*/
  DS_SYSTEM_HEAP_MEM_ALLOC(ds_fwk_inst[ds_fwk_index],
                           sizeof(ds_fwk_s_type),
                           ds_fwk_s_type*)
  if( ds_fwk_inst[ds_fwk_index] == NULL)
  {  
    DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_alloc_inst:"
                             " memory not available for %d ",
                             ds_fwk_index);
    return -1;
  }
  
  memset(ds_fwk_inst[ds_fwk_index],0x00,sizeof(ds_fwk_s_type));
  ds_fwk_inst[ds_fwk_index]->ds_fwk_index = ds_fwk_index;

  while(clnt_id < DS_FWK_CLIENT_MAX)
  {
    ds_fwk_inst[ds_fwk_index]->client_info[clnt_id].v4_client_handle = 
                                                    DS_FWK_INVALID_CLNT_HANDLE;
    ds_fwk_inst[ds_fwk_index]->client_info[clnt_id].v6_client_handle =
                                                    DS_FWK_INVALID_CLNT_HANDLE;
    ds_fwk_inst[ds_fwk_index]->client_info[clnt_id].nipd_client_handle = 
                                                    DS_FWK_INVALID_CLNT_HANDLE;
    clnt_id++;
  }
  
  for(index = 0; index < FLTR_CLIENT_MAX; index++)
  {
    (void) q_init(&(ds_fwk_inst[ds_fwk_index]->ipfltr_info[index]));
  }

#ifdef FEATURE_DATAMODEM_DNS
  (void) q_init(&(ds_fwk_inst[ds_fwk_index]->dns_cache_q));
#endif

  DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_alloc_inst:"
                          "Allocated at index for %d inst 0x%p",
                           ds_fwk_index,ds_fwk_inst[ds_fwk_index]);
  ds_fwk_assign_iid(&(ds_fwk_inst[ds_fwk_index]->dpl_um[0]),IPV4_ADDR,DS_DPL_CLIENT_UM);
  ds_fwk_assign_iid(&(ds_fwk_inst[ds_fwk_index]->dpl_um[1]),IPV6_ADDR,DS_DPL_CLIENT_UM);
  /*-----------------------------------------------------------------------
  Initialize QoS Net initiated requests on this instance
-----------------------------------------------------------------------*/
  list_init(&(ds_fwk_inst[ds_fwk_index]->net_initiated_qos_req_list));
  list_init(&(ds_fwk_inst[ds_fwk_index]->rx_qos_fltr_node_list));
  return ds_fwk_index;
}


void ds_fwk_free_iids(ds_fwk_s_type  *fw_inst)
{
 /*----------------------------------------------*/
    ds_fwk_dealloc_iid(&(fw_inst->dpl_rm[0]),IPV4_ADDR);
    ds_fwk_dealloc_iid(&(fw_inst->dpl_rm[1]),IPV6_ADDR);    
    ds_fwk_dealloc_iid(&(fw_inst->dpl_um[0]),IPV4_ADDR);
    ds_fwk_dealloc_iid(&(fw_inst->dpl_um[1]),IPV6_ADDR);    
}

/*===========================================================================
  FUNCTION DS_FWK_DELETE_INST()

  DESCRIPTION
   allocated attach framework instance when invoked from MH

  PARAMETERS
    pdn_ptr 
    ip type

  RETURN VALUE
    fwk instance index

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
void ds_fwk_delete_inst(ds_fwk_index_type fw_index)
{
  ds_ipfltr_client_id_enum_type filter_client = FLTR_CLIENT_SOCKETS;
  int16                         ps_errno;
  if( ds_fwk_inst[fw_index] != NULL )
  {
    DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_delete_inst:"
                            "deallocated at index for %d inst 0x%p",
                             fw_index,ds_fwk_inst[fw_index]);

    for(filter_client = 0; filter_client < FLTR_CLIENT_MAX; filter_client++)
    {
      ds_ipfltri_delete(ds_fwk_inst[fw_index],
                        IP_ANY_ADDR,
                        filter_client,
                        DS_IPFLTR_DELETE_ALL_HANDLE,
                        &ps_errno
                      );
      (void) q_destroy(&(ds_fwk_inst[fw_index]->ipfltr_info[filter_client]));
    }

#ifdef FEATURE_DATAMODEM_DNS
    /*-------------------------------------------------------------------------
      Clear DNS cache
    -------------------------------------------------------------------------*/
    (void) ps_dns_cache_flush(ds_fwk_inst[fw_index], &ps_errno);
    (void) q_destroy(&(ds_fwk_inst[fw_index]->dns_cache_q));
#endif

    ds_fwk_free_iids(ds_fwk_inst[fw_index]);
    if( ds_fwk_inst[fw_index]->dl_opt_fltr_arr != NULL )
    {
      DS_SYSTEM_HEAP_MEM_FREE(ds_fwk_inst[fw_index]->dl_opt_fltr_arr);
    }
    
    DS_SYSTEM_HEAP_MEM_FREE(ds_fwk_inst[fw_index]);	
    ds_fwk_inst[fw_index] = NULL;
  }
}

/*===========================================================
  FUNCTION DS_FWK_DEALLOC_ATTACH_INST()

  DESCRIPTION
   deallocates attach framework instance when invoked from MH

  PARAMETERS
    ds_fwk_index

  RETURN VALUE
    none

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
void ds_fwk_dealloc_attach_inst
(
  ds_fwk_index_type ds_fwk_index
)
{
  if(ds_fwk_inst[ds_fwk_index] != NULL)
  {
    DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_dealloc_attach_inst clnt mask %d",
                             ds_fwk_inst[ds_fwk_index]->client_mask);
    if( ds_fwk_inst[ds_fwk_index]->client_mask != 0 )  
    {
      if( ds_fwk_inst[ds_fwk_index]->v4_pdn_ptr == NULL &&
          ds_fwk_inst[ds_fwk_index]->v6_pdn_ptr == NULL &&
          ds_fwk_inst[ds_fwk_index]->non_ip_pdn_ptr == NULL )
      {
        DS_ASSERT(0);
      }
      return;
    }
    else
    {
      ds_fwk_delete_inst(ds_fwk_index);
    }
  }
}

/*===========================================================
  FUNCTION DS_FWK_ASSIGN_DPL_DESC()

  DESCRIPTION
   deallocates attach framework instance when invoked from MH

  PARAMETERS
    ds_fwk_index
    client_id
    policy_info

  RETURN VALUE
    none

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/

void ds_fwk_assign_dpl_desc
(
  uint8                        fwk_inst,
  ds_dpl_client_type           client_id,
  ds_policy_info_type          policy_info
)
{
  char *apn = "default_apn";
/*-----------------------------------------------------------*/
  DS_MSG_SPRINTF_2(MSG_LEGACY_HIGH,"ds_fwk_assign_dpl_desc %d, apn %s",
                    client_id,policy_info.name);
  if(policy_info.length !=0)
  {
    dpli_log_iid_desc_and_status(ds_fwk_inst[fwk_inst], IPV4_ADDR,
                                 client_id,policy_info.name);  
    dpli_log_iid_desc_and_status(ds_fwk_inst[fwk_inst], IPV6_ADDR,
                                 client_id,policy_info.name);  
  }
  else
  {
    dpli_log_iid_desc_and_status(ds_fwk_inst[fwk_inst], IPV4_ADDR,
                                 client_id,apn);
    dpli_log_iid_desc_and_status(ds_fwk_inst[fwk_inst], IPV6_ADDR,
                                  client_id,apn);  
  }
}

/*============================================================
  FUNCTION DS_FWK_UPDATE_CLIENT_INFO()

  DESCRIPTION
   deallocates attach framework instance when invoked from MH

  PARAMETERS
    IP family
    fw index
    client ID
    client handle
    reset flag

  RETURN VALUE
    none

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
static void ds_fwk_update_client_info
(
  ip_addr_enum_type        family,
  ds_fwk_index_type        fw_index,
  ds_fwk_client_type       client_id,
  ds_fwk_clnt_handle_type  client_handle,
  boolean                  reset
)
{
  /*assign client handle */
  /*update the client mask on who is using this instance*/
  #define  MAX_IP_TYPE  3
  
  ds_fwk_client_type client_index  = DS_FWK_CLIENT_RMSM;
  
  DS_MSG5(MSG_LEGACY_HIGH,"ds_fwk_update_client_info:" 
                          "family %d fw_index %d client_handle %d"
                          "reset %d client_mask %d",
                          family,fw_index,client_handle,
                          reset,ds_fwk_inst[fw_index]->client_mask);
  if(!reset)
  {
    if(family == IPV4_ADDR)
    {
      ds_fwk_inst[fw_index]->client_info[client_id].v4_client_handle 
                 = client_handle;
      ds_fwk_inst[fw_index]->client_mask |= (1<<(client_id*MAX_IP_TYPE));	
    }
    else if(family == IPV6_ADDR)
    {
      ds_fwk_inst[fw_index]->client_info[client_id].v6_client_handle 
                = client_handle;
	  // to update the V6 bit in the mask for that client
      ds_fwk_inst[fw_index]->client_mask |= (1<<((client_id*MAX_IP_TYPE)+1));	
    }
    else if(family == NON_IP_ADDR)
    {
      ds_fwk_inst[fw_index]->client_info[client_id].nipd_client_handle = 
                 client_handle;
      // to update the NIPD bit in the mask for that client
      ds_fwk_inst[fw_index]->client_mask |= (1<<((client_id*MAX_IP_TYPE)+2));
    }
    else
    {
      DS_MSG1(MSG_LEGACY_HIGH,"invalid IP type %d",family);
      DS_ASSERT(0);
    }
  }
  else
  {
    //special case: incase of pdn down event all the client info should be cleared
    if( ds_fwk_event_trigger_flag != TRUE)
    {
      while( client_index < DS_FWK_CLIENT_PROTO_V6 )
     {
       if(family == IPV4_ADDR)
       {
         ds_fwk_inst[fw_index]->client_info[client_index].v4_client_handle = 
                                                   DS_FWK_INVALID_CLNT_HANDLE;
         ds_fwk_inst[fw_index]->client_mask &= ~ (1<<(client_index*MAX_IP_TYPE));	
         ds_fwk_inst[fw_index]->v4_pdn_ptr = NULL;
       }
       else if(family == IPV6_ADDR)
       {
         ds_fwk_inst[fw_index]->client_info[client_index].v6_client_handle =
                                                   DS_FWK_INVALID_CLNT_HANDLE;
         // to update the V6 bit in the mask for that client
         ds_fwk_inst[fw_index]->client_mask &= ~ (1<<((client_index*MAX_IP_TYPE)+1));
         ds_fwk_inst[fw_index]->v6_pdn_ptr = NULL;
       }
       else if(family == NON_IP_ADDR)
       {
         ds_fwk_inst[fw_index]->client_info[client_index].nipd_client_handle = 
                                                   DS_FWK_INVALID_CLNT_HANDLE;
         // to update the NIPD bit in the mask for that client
         ds_fwk_inst[fw_index]->client_mask &= ~ (1<<((client_index*MAX_IP_TYPE)+2));
         ds_fwk_inst[fw_index]->non_ip_pdn_ptr = NULL;
       }
       else
       {
         DS_MSG1(MSG_LEGACY_HIGH,"invalid IP type %d",family);
         DS_ASSERT(0);
       }
       client_index++;
     }
      
     DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_update_client_info:" 
                             "client_handle %d client_mask %d",
                             client_handle,ds_fwk_inst[fw_index]->client_mask );
      return;
    }
    else
    {
      ds_fwk_event_trigger_flag = FALSE;
    }
    
    if(family == IPV4_ADDR)
    {
      ds_fwk_inst[fw_index]->client_info[client_id].v4_client_handle = 
                                                DS_FWK_INVALID_CLNT_HANDLE;
      ds_fwk_inst[fw_index]->client_mask &= ~ (1<<(client_id*MAX_IP_TYPE));	
    }
    else if(family == IPV6_ADDR)
    {
      ds_fwk_inst[fw_index]->client_info[client_id].v6_client_handle =
                                                DS_FWK_INVALID_CLNT_HANDLE;
      // to update the V6 bit in the mask for that client
      ds_fwk_inst[fw_index]->client_mask &= ~ (1<<((client_id*MAX_IP_TYPE)+1));
    }
    else if(family == NON_IP_ADDR)
    {
      ds_fwk_inst[fw_index]->client_info[client_id].nipd_client_handle = 
                                                DS_FWK_INVALID_CLNT_HANDLE;
      // to update the NIPD bit in the mask for that client
      ds_fwk_inst[fw_index]->client_mask &= ~ (1<<((client_id*MAX_IP_TYPE)+2));
    }
    else
    {
      DS_MSG1(MSG_LEGACY_HIGH,"invalid IP type %d",family);
      DS_ASSERT(0);
  }
}

  DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_update_client_info:" 
                          "client_handle %d client_mask %d",
                          client_handle,ds_fwk_inst[fw_index]->client_mask );
}


/*============================================================
  FUNCTION DS_FWK_CHECK_CALL_ALLOWED()

  DESCRIPTION

  PARAMETERS

  RETURN VALUE
      
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
boolean ds_fwk_check_call_allowed
(
  ds_fwk_s_type             *fwk_ptr ,
  ds_fwk_client_type         client_id
)
{
  boolean ret_val = TRUE;
  
  if(fwk_ptr != NULL)
  {
    if(client_id == DS_FWK_CLIENT_RMNET)
    {
      if((fwk_ptr->client_info[DS_FWK_CLIENT_ATCOP].v4_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_ATCOP].v6_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_ATCOP].nipd_client_handle != DS_FWK_INVALID_CLNT_HANDLE))
            ret_val = FALSE;
#ifdef FEATURE_MODE_TLB
      if((fwk_ptr->client_info[DS_FWK_CLIENT_TLB].v4_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_TLB].v6_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_TLB].nipd_client_handle != DS_FWK_INVALID_CLNT_HANDLE))
            ret_val = FALSE;
#endif
      if(((fwk_ptr->client_info[DS_FWK_CLIENT_SOCKETS].v4_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
           (fwk_ptr->client_info[DS_FWK_CLIENT_SOCKETS].v6_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
           (fwk_ptr->client_info[DS_FWK_CLIENT_SOCKETS].nipd_client_handle != DS_FWK_INVALID_CLNT_HANDLE)) &&
          ((fwk_ptr->client_info[DS_FWK_CLIENT_RMSM].v4_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
           (fwk_ptr->client_info[DS_FWK_CLIENT_RMSM].v6_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
           (fwk_ptr->client_info[DS_FWK_CLIENT_RMSM].nipd_client_handle != DS_FWK_INVALID_CLNT_HANDLE)))
      {
         ret_val = FALSE;
      }

    }
    else if(client_id == DS_FWK_CLIENT_RMSM )
    {   
      if((fwk_ptr->client_info[DS_FWK_CLIENT_ATCOP].v4_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_ATCOP].v6_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_ATCOP].nipd_client_handle != DS_FWK_INVALID_CLNT_HANDLE))
            ret_val = FALSE;
#ifdef FEATURE_MODE_TLB
      if((fwk_ptr->client_info[DS_FWK_CLIENT_TLB].v4_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_TLB].v6_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_TLB].nipd_client_handle != DS_FWK_INVALID_CLNT_HANDLE))
            ret_val = FALSE;
#endif
     if(((fwk_ptr->client_info[DS_FWK_CLIENT_RMNET].v4_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_RMNET].v6_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_RMNET].nipd_client_handle != DS_FWK_INVALID_CLNT_HANDLE)) &&
        ((fwk_ptr->client_info[DS_FWK_CLIENT_SOCKETS].v4_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_SOCKETS].v6_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_SOCKETS].nipd_client_handle != DS_FWK_INVALID_CLNT_HANDLE)))
       {
          ret_val = FALSE;
       } 

    }
    else if(client_id == DS_FWK_CLIENT_ATCOP)
    {
      if((fwk_ptr->client_info[DS_FWK_CLIENT_RMSM].v4_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_RMSM].v6_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_RMSM].nipd_client_handle != DS_FWK_INVALID_CLNT_HANDLE))
            ret_val = FALSE;
      if((fwk_ptr->client_info[DS_FWK_CLIENT_RMNET].v4_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_RMNET].v6_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_RMNET].nipd_client_handle != DS_FWK_INVALID_CLNT_HANDLE))
            ret_val = FALSE;
#ifdef FEATURE_MODE_TLB
      if((fwk_ptr->client_info[DS_FWK_CLIENT_TLB].v4_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_TLB].v6_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_TLB].nipd_client_handle != DS_FWK_INVALID_CLNT_HANDLE))
            ret_val = FALSE;
#endif
    }
#ifdef FEATURE_MODE_TLB
    else if(client_id == DS_FWK_CLIENT_TLB)
    {
      if((fwk_ptr->client_info[DS_FWK_CLIENT_RMSM].v4_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_RMSM].v6_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_RMSM].nipd_client_handle != DS_FWK_INVALID_CLNT_HANDLE))
            ret_val = FALSE;
      if((fwk_ptr->client_info[DS_FWK_CLIENT_RMNET].v4_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_RMNET].v6_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_RMNET].nipd_client_handle != DS_FWK_INVALID_CLNT_HANDLE))
            ret_val = FALSE;
      if((fwk_ptr->client_info[DS_FWK_CLIENT_ATCOP].v4_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_ATCOP].v6_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
         (fwk_ptr->client_info[DS_FWK_CLIENT_ATCOP].nipd_client_handle != DS_FWK_INVALID_CLNT_HANDLE))
            ret_val = FALSE;
    }
#endif
    else if(client_id == DS_FWK_CLIENT_SOCKETS)
    {
      if(((fwk_ptr->client_info[DS_FWK_CLIENT_RMNET].v4_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
          (fwk_ptr->client_info[DS_FWK_CLIENT_RMNET].v6_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
          (fwk_ptr->client_info[DS_FWK_CLIENT_RMNET].nipd_client_handle != DS_FWK_INVALID_CLNT_HANDLE)) &&
         ((fwk_ptr->client_info[DS_FWK_CLIENT_RMSM].v4_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
          (fwk_ptr->client_info[DS_FWK_CLIENT_RMSM].v6_client_handle != DS_FWK_INVALID_CLNT_HANDLE) ||
          (fwk_ptr->client_info[DS_FWK_CLIENT_RMSM].nipd_client_handle != DS_FWK_INVALID_CLNT_HANDLE)))
         {
           ret_val = FALSE;
         }
	  }
  }
  else  /*Fwk_ptr is NUll. Dont allow call*/
  {
    ret_val = FALSE;
  }
  
  DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_check_call_allowed:" 
                          "ret_val %d client_id %d",
                          ret_val,client_id);
  return ret_val;
}

/*============================================================
  FUNCTION DS_FWK_BRINGUP_PDN_BY_POLICY()

  DESCRIPTION
    Invokes corresponding Mode handler bringup up api and bringups the PDN.

  PARAMETERS
    client_type
    apn 
    ip type
    client_handle - we use this as client handle to identiy and send as part of further indications.
    clients can be RMNET/RMSM/DSS.
    incase of PDN sharing scenario to indicate only to the requested client dsfwk passes this 
    user_data to pdn mansger, PDN manager provides this info as part of event so that DSFWK 
    can invoke client call back with this user data.

  RETURN VALUE
      0  -- if bringup is successful.
     -1 -- if bringup is failed.
      102 -Operation would block.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
int  ds_fwk_bringup_pdn_by_policy
(
  ds_fwk_client_type           client_id,
  ds_policy_info_type          policy_info,
  ds_fwk_clnt_handle_type      client_handle,
  ds_fwk_response_info_s_type *response_info
)
{
  /*---------------------------Local Declarations--------------------------------*/
  uint8             mh_id = ds_fwk_get_current_mh();
  ds_fwk_index_type fwk_inst = DS_FWK_INST_INVALID;
  int result = 0;
  ds_fwk_clnt_handle_type      clnt_handle = -1;
  ds_pdn_event_info_u_type     event_info;
  /*--------------------------------------------------------------------------*/
  
   DS_MSG3(MSG_LEGACY_HIGH,"ds_fwk_bringup_pdn_by_policy" 
                           "client ID:%d lookup family %d client_handle %d",
                            client_id,policy_info.family,client_handle);
  /*-----------------------------------------------------------
    Algo:
    1) check whether MH is registered with FW or not.if not return failure.
    2)do the lookup with policy_infomration.MH returns a instance if policy
       matches with any PDN.
    3)if MH returns valid instance then invoke bringup API.of MH and attach
       the clienthandle to FW instance.
    4)if MH returns invalid instance then allocate new FW instance and assign
       the client handle to FW instance.invoke MH bringup API to bring up the PDN.
     ------------------------------------------------------------*/
  if((ds_fwk_validate_mh_info(mh_id) != TRUE) || 
     (ds_fw_mh_info[mh_id]->pdn_operations.bringup_f_ptr == NULL))
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_bringup_pdn_by_policy:" 
                             "invalid mh_id %d passed ",mh_id);
    return -1;
  }

  clnt_handle = ds_fwk_look_up_pdn_by_policy(client_id,policy_info);
  if(clnt_handle != DS_FWK_INVALID_CLNT_HANDLE)
  {
    //call is already Up with same policy. client needs to take care of ref count.
    return 0;
  }
  
  if(ds_fw_mh_info[mh_id]->pdn_operations.look_up_f_ptr != NULL) 
  {
    fwk_inst = ds_fw_mh_info[mh_id]->pdn_operations.look_up_f_ptr(policy_info);
  }

  DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_bringup_pdn_by_policy:"
                           "fw_inst %d returned from lookup",fwk_inst);
  if( fwk_inst == DS_FWK_INST_INVALID )
  {
    fwk_inst = ds_fwk_alloc_inst();
  }

  memset(&event_info,0x0,sizeof(ds_pdn_event_info_u_type));
  if( fwk_inst == DS_FWK_INST_INVALID || fwk_inst >= DS_FWK_MAX_INST )
  {
    event_info.pdn_down_info.netdown_reason = DS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES;	
    event_info.pdn_down_info.addr_family = policy_info.family;
    goto send_result;
  }


  if(ds_fwk_check_call_allowed(ds_fwk_inst[fwk_inst],client_id) == FALSE)
  {  
    event_info.pdn_down_info.netdown_reason = DS_NET_DOWN_REASON_PDN_IN_USE;
    event_info.pdn_down_info.addr_family = policy_info.family;
    goto send_result;
  }

  if(client_id == DS_FWK_CLIENT_RMNET)
  {
    ds_fwk_assign_iid(&(ds_fwk_inst[fwk_inst]->dpl_rm[0]),IPV4_ADDR,DS_DPL_CLIENT_RM);
    ds_fwk_assign_iid(&(ds_fwk_inst[fwk_inst]->dpl_rm[1]),IPV6_ADDR,DS_DPL_CLIENT_RM);

    ds_fwk_assign_dpl_desc(fwk_inst,DS_DPL_CLIENT_RM,policy_info);
    ds_fwk_assign_dpl_desc(fwk_inst,DS_DPL_CLIENT_UM,policy_info);
  }
  
  ds_fwk_update_client_info(policy_info.family,
                            fwk_inst,
                            client_id,
                            client_handle,
                            FALSE);
  result = ds_fw_mh_info[mh_id]->pdn_operations.bringup_f_ptr(
                                       fwk_inst,
                                       policy_info,
                                       response_info);
  DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_bringup_pdn_by_policy: result %d,fwk_inst %d",
                           result,fwk_inst);
  /*invoke MH API to bringup the PDN*/
  if( result == 0 )
  {
    response_info->ip_type = policy_info.family;
    //call is successful, no need to wait for any events.
    ds_fwk_set_pdn_info(fwk_inst,response_info);
    ds_fwk_notify_events_ex( fwk_inst,
                               policy_info.family,
                               client_id,
                            PDN_UP_EV,
                            event_info);//no need to fill event info for PDN UP Event.
    return 0;
  }
  else if(result == DS_EWOULDBLOCK )
  { 
    response_info->ip_type = policy_info.family;
    ds_fwk_set_pdn_info(fwk_inst,response_info);
    return DS_EWOULDBLOCK;
  }
  else
  {
    //PDN bringup failed.
    DS_MSG2(MSG_LEGACY_ERROR,"ds_fwk_bringup_pdn_by_policy:"
                             "bringup failed type %d reason %d.",
                              response_info->netdown_reason_type,
                              response_info->netdown_reason);
    ds_fwk_event_trigger_flag = TRUE;
    event_info.pdn_down_info.addr_family = policy_info.family;
    event_info.pdn_down_info.netdown_reason = response_info->netdown_reason;
    ds_fwk_notify_events_ex( fwk_inst,
                            policy_info.family,
                            client_id,
                               PDN_DOWN_EV,
                               event_info);//no need to fill event info 	
    return -1;
  }

 send_result:
 DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_bringup_pdn_by_policy:"
               "netdown_reason %d",event_info.pdn_down_info.netdown_reason);
 if( (ds_fwk_event_buf[client_id]!= NULL) && 
     (ds_fwk_event_buf[client_id]->event_cback_f_ptr != NULL)&&
      client_handle != DS_FWK_INVALID_CLNT_HANDLE )
  {
     ds_fwk_event_buf[client_id]->event_cback_f_ptr(
                      client_handle,
                      PDN_DOWN_EV,
                      event_info,
                      ds_fwk_event_buf[client_id]->user_data_ptr);
       
  }
  return -1;
}

/*============================================================
  FUNCTION DS_FWK_CHECK_IF_TEARDOWN_ALLOWED()

  DESCRIPTION
   Checks if the network teardown  is allowed for the client 
   
  PARAMETERS
   client_type                fwk client 
   ip_type                    ip type of the call 
   fw_index                   index to fw inst
 
  RETURN VALUE
     Boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
boolean ds_fwk_check_if_teardown_allowed
(
  ds_fwk_client_type             client_type,
  ip_addr_enum_type	          ip_type,
  ds_fwk_index_type              fw_index
)
{
  uint16                        client_mask = 0;  
  ds_fwk_client_type            client_id = DS_FWK_CLIENT_RMSM;
 /*----------------------------------------------------------------*/

 //Logic should be to check if any client has started a call with same IP TYPE
   while(client_id < DS_FWK_CLIENT_PROTO_V6)
   {
     if(client_id != client_type)
     {
        //Check if it pdn sharing scenario
        if(ip_type == IPV4_ADDR)
            client_mask = client_id*MAX_IP_TYPE;
         else if(ip_type == IPV6_ADDR)
             client_mask = (client_id*MAX_IP_TYPE)+1;
         else if(ip_type == NON_IP_ADDR)
             client_mask = (client_id*MAX_IP_TYPE)+2;

         if((ds_fwk_inst[fw_index]->client_mask & (1<< client_mask)) != 0)
         {
            DS_MSG2(MSG_LEGACY_ERROR,"ds_fwk_tear_down_pdn:"
                   " Mask set for the client %d client_mask %d.",
                     client_id,client_mask );
              return FALSE;
          }     
        }	
        client_id++;
   }
  return TRUE;
}

/*============================================================
  FUNCTION DS_FWK_TEARDOWN_PDN()

  DESCRIPTION
    Invokes corresponding Mode handler bringup up api and bringups the PDN.

  PARAMETERS
    client_type

  RETURN VALUE
      0  -- if bringup is successful.
     -1 -- if bringup is failed.
      102 -Operation would block.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
int  ds_fwk_tear_down_pdn
(
  ds_fwk_client_type             client_id,
  ds_fwk_clnt_handle_type        client_handle,
  ds_fwk_response_info_s_type   *response_info
)
{
  /*------------------------------Local Declarations-----------------------------*/
  ds_fwk_index_type           fw_index = DS_FWK_INST_INVALID;
  ds_fwk_pdn_info             pdn_info;
  int                         result = 0;
  ds_pdn_event_info_u_type    event_info;
  uint8                       mh_id = ds_fwk_get_current_mh();
  /*--------------------------------------------------------------------------*/
  DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_tear_down_pdn" 
                          "client ID:%d lookup client_handle %d",
                           client_id,client_handle);
  #define  MAX_IP_TYPE  3
  if((ds_fwk_validate_mh_info(mh_id) != TRUE) && 
     (ds_fw_mh_info[mh_id]->pdn_operations.teardown_f_ptr != NULL))
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_tear_down_pdn:"
                             " invalid mh_id %d passed ",mh_id);
    return -1;
  }
  
  fw_index = ds_fwk_get_index_from_client_handle(client_id,client_handle);
  if( fw_index == DS_FWK_INST_INVALID )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_tear_down_pdn:"
                             " invalid client_handle %d passed ",client_handle);
    return -1;
  }
  
  memset(&pdn_info,0x00,sizeof(pdn_info));
  if(ds_fwk_get_pdn_info_from_client_handle(
     client_id,client_handle,fw_index,&pdn_info) == -1)
  {
    return -1;
  }
  
  if(FALSE == ds_fwk_check_if_teardown_allowed(client_id,pdn_info.ip_type,fw_index))
  {
     DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_tear_down_pdn:"
                  "ds_fwk_check_if_teardown_allowed return FALSE" );
     goto send_result;
   }
  memset(&event_info,0x0,sizeof(ds_pdn_event_info_u_type));	
  result = ds_fw_mh_info[mh_id]->pdn_operations.teardown_f_ptr(pdn_info.pdn_ptr,
                                                            pdn_info.ip_type,
                                                            response_info);
  /*invoke MH API to Teardown the PDN*/
   DS_MSG3(MSG_LEGACY_HIGH,"ds_fwk_tear_down_pdn:" 
                           "result %d,fwk_inst %d family %d",
                            result,fw_index,pdn_info.ip_type);
   if ( result == 0)
   { 
     goto send_result;
   }
   else if(result == DS_EWOULDBLOCK )
   {
     //wait for PDN events in this case., dont clear the client mask
     //client mask is updated as part of PDN down events.
     return DS_EWOULDBLOCK;
   }
   else
   { 
     //PDN teardown failed.
      DS_MSG2(MSG_LEGACY_ERROR,"ds_fwk_tear_down_pdn:"
                " failed type %d reason %d.",
                  response_info->netdown_reason_type,
                  response_info->netdown_reason);
      return -1;
   }

 send_result:
  /*assign client handle */
  /*update the client mask on who is using this instance*/
  //NO events will be recieved from PDN MGR in this case
  event_info.pdn_down_info.addr_family = response_info->ip_type;
  event_info.pdn_down_info.netdown_reason = DS_NET_DOWN_REASON_CLIENT_END;
  ds_fwk_event_trigger_flag = TRUE;
  ds_fwk_notify_events_ex( fw_index,
                                    pdn_info.ip_type,
                                    client_id,
                                    PDN_DOWN_EV,
                                    event_info);
  return 0;
}


/*============================================================
  FUNCTION DS_FWK_REGISTER_EVENTS()

  DESCRIPTION
   regsiters client for all the PDN events. 
   this callback is invoked for all the PDN events if the client is active.
      
  PARAMETERS
    client_id: client type
    callback_type: user callback 
    user-data_ptr: data that needs to be passed to client as part of PDN event invocation.

  RETURN VALUE
      0  -- if reg is success
     -1 -- if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
int ds_fwk_reg_events
(
  ds_fwk_client_type       client_id,
  ds_fwk_event_cback_type  callback_type,
  void                    *user_data_ptr
)
{
  /*check whether the client ID passed is valid or not*/
  if( !ds_fwk_validate_client_id(client_id))
  	return FALSE;
  
  if(ds_fwk_event_buf[client_id] != NULL || callback_type == NULL )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"invlid CB type");
    return FALSE;
  }

  /*allocate the memory for client*/
  DS_SYSTEM_HEAP_MEM_ALLOC(ds_fwk_event_buf[client_id],
                           sizeof(ds_fwk_event_buf_type),
                           ds_fwk_event_buf_type*)
  if( ds_fwk_event_buf[client_id] == NULL)
  {  
    DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_reg_events:"
                             " memory not available for %d ",client_id);
    return -1;
  }
  
  memset(ds_fwk_event_buf[client_id],0x00,sizeof(ds_fwk_event_buf_type));
  ds_fwk_event_buf[client_id]->event_cback_f_ptr = callback_type;
  ds_fwk_event_buf[client_id]->user_data_ptr = user_data_ptr;
  return 0;
}


/*============================================================
  FUNCTION ds_fwk_notify_events()

  DESCRIPTION
    invokes event callbacks of all the client present on the framework instance
      
  PARAMETERS
    ds_fwk_inst: instance of ds_fwk.
    event: event name
    event_info: event_payload

  RETURN VALUE
       0  -- if events invocation is successful
     -1 -- if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
int ds_fwk_notify_events
(
  ds_fwk_index_type            fw_index,
  ip_addr_enum_type            family,
  ds_pdn_event_enum_type       event,
  ds_pdn_event_info_u_type     event_info
)
{
   return ds_fwk_notify_events_ex(fw_index,
                            family,
                            DS_FWK_CLIENT_INVALID,
                            event,
                            event_info
                            );
}

/*===========================================================================
  FUNCTION DS_FWK_REGISTER_TX_CB()

  DESCRIPTION
   regsiters client for all the PDN events. this callback is invoked for all the PDN events if the client is active.
      
  PARAMETERS
    client_id: client type
    callback_type: user callback 
    user-data_ptr: data that needs to be passed to client as part of PDN event invocation.

  RETURN VALUE
      0  -- if reg is success
     -1 -- if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int ds_fwk_reg_tx_cb
(
  ds_fwk_client_type       client_id,
  ds_fwk_tx_cback_type     callback_type,
  void                    *user_data_ptr
)
{
  /*check whether the client ID passed is valid or not*/
  if( !ds_fwk_validate_client_id(client_id))
  	return FALSE;
  
  if(ds_fwk_tx_buf[client_id] != NULL || callback_type == NULL )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"invlid CB type");
    return FALSE;
  }
  /*allocate the memory for client*/
  DS_SYSTEM_HEAP_MEM_ALLOC(ds_fwk_tx_buf[client_id],
                           sizeof(ds_fwk_tx_buf_type),
                           ds_fwk_tx_buf_type*)
  if( ds_fwk_tx_buf[client_id] == NULL)
  {  
    DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_reg_tx_cb:"
                             " memory not available for %d ",client_id);
    return -1;
  }
  memset(ds_fwk_tx_buf[client_id],0x00,sizeof(ds_fwk_tx_buf_type));
  ds_fwk_tx_buf[client_id]->tx_cback_f_ptr = callback_type;
  ds_fwk_tx_buf[client_id]->user_data_ptr = user_data_ptr;
  return 0;
}

/*===========================================================================
  FUNCTION DS_FWK_REGISTER_NIPD_TX_CB()

  DESCRIPTION
   regsiters client for all the PDN events. this callback is invoked for all the PDN events if the client is active.
      
  PARAMETERS
    client_id: client type
    callback_type: user callback 
    user-data_ptr: data that needs to be passed to client as part of PDN event invocation.

  RETURN VALUE
      0  -- if reg is success
     -1 -- if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int ds_fwk_reg_nipd_tx_cb
(
  ds_fwk_client_type       client_id,
  ds_fwk_tx_cback_type     callback_type,
  void                    *user_data_ptr
)
{
  /*check whether the client ID passed is valid or not*/
  if( !ds_fwk_validate_client_id(client_id))
    return FALSE;
  
  if(ds_fwk_tx_nipd_buf[client_id] != NULL || callback_type == NULL )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"invlid CB type");
    return FALSE;
  }
  /*allocate the memory for client*/
  DS_SYSTEM_HEAP_MEM_ALLOC(ds_fwk_tx_nipd_buf[client_id],
                           sizeof(ds_fwk_tx_nipd_buf_type),
                           ds_fwk_tx_nipd_buf_type*)
  if( ds_fwk_tx_nipd_buf[client_id] == NULL)
  {  
    DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_reg_tx_cb:"
                             " memory not available for %d ",client_id);
    return -1;
  }
  memset(ds_fwk_tx_nipd_buf[client_id],0x00,sizeof(ds_fwk_tx_nipd_buf_type));
  ds_fwk_tx_nipd_buf[client_id]->tx_cback_f_ptr = callback_type;
  ds_fwk_tx_nipd_buf[client_id]->user_data_ptr = user_data_ptr;
  return 0;
}

/*===========================================================================
  FUNCTION DS_FWK_INVOKE_TX_CB()

  DESCRIPTION
   invokes tx function pointer fo the client id passed.
      
  PARAMETERS
    client_id: client type
    callback_type: user callback 
    user-data_ptr: data that needs to be passed to client as part of PDN event invocation.

  RETURN VALUE
      0  -- if reg is success
     -1 -- if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int ds_fwk_invoke_tx_cb
(
  ds_fwk_client_type          client_id,  
  ds_fwk_clnt_handle_type     client_handle,  
  dsm_item_type              *dsm_item
)
{
  DS_MSG2(MSG_LEGACY_HIGH,"Passed client handle %d and client id %d",
                 client_handle, client_id);
  /*check whether the client ID passed is valid or not*/
  if( (!ds_fwk_validate_client_id(client_id)) ||
      (ds_fwk_tx_buf[client_id] == NULL) )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"Dropping packet invalid Client ID");
    dsm_free_packet(&dsm_item);
    return -1;
  }
  ds_fwk_tx_buf[client_id]->tx_cback_f_ptr(client_handle,
                                           dsm_item,
                                           ds_fwk_tx_buf[client_id]->user_data_ptr);
  return 0;
}

/*===========================================================================
  FUNCTION DS_FWK_INVOKE_TX_NIPD_CB()

  DESCRIPTION
    Invokes the nipd callback for client       
    
  PARAMETERS
    client_id: client type
    client_handle: user callback 
    user-dsm_item: data that needs to be passed to client as part of PDN event invocation.

  RETURN VALUE
      0  -- if invocation is success
     -1 -- if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

int ds_fwk_invoke_tx_nipd_cb
(
  ds_fwk_client_type          client_id,  
  ds_fwk_clnt_handle_type     client_handle,  
  dsm_item_type              *dsm_item
)
{
  DS_MSG2(MSG_LEGACY_HIGH,"Passed client handle %d and client id %d",
                 client_handle, client_id);
  /*check whether the client ID passed is valid or not*/
  if( !ds_fwk_validate_client_id(client_id))
  	return FALSE;
  
  if(ds_fwk_tx_nipd_buf[client_id] == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"invlid CB type");
    return FALSE;

  }
  ds_fwk_tx_nipd_buf[client_id]->tx_cback_f_ptr(client_handle,
                                           dsm_item,
                                           ds_fwk_tx_nipd_buf[client_id]->user_data_ptr);
  return 0;
}

/*===========================================================================
  FUNCTION DS_FWK_INVOKE_EVENT_CB_EX()

  DESCRIPTION
    invokes event callbacks of all the client present on the framework instance
      
  PARAMETERS
    ds_fwk_inst: instance of ds_fwk.
    family: ip type
    client id: client due to which callback is invoked.
    event: event name
    event_info: event_payload

  RETURN VALUE
       0  -- if events invocation is successful
     -1 -- if failed

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/

int ds_fwk_notify_events_ex
(
  ds_fwk_index_type            fw_index,
  ip_addr_enum_type            family,
  int8                        client_id,
  ds_pdn_event_enum_type       event,
  ds_pdn_event_info_u_type     event_info
)
{
  /*---------------------------Local Declarations--------------------------------*/
  ds_fwk_clnt_handle_type clnt_handle = -1;
  uint8                   temp_client_id = 0;
  /*--------------------------------------------------------------------------*/
  DS_MSG4(MSG_LEGACY_HIGH,"ds_fwk_notify_events_ex:"
                          "fw_index %d family %d.event %d client_id %d",
                           fw_index,family,event,client_id);
  if( fw_index < 0 || fw_index >= DS_FWK_MAX_INST)
  {
    return -1;
  }
  if( ds_fwk_inst[fw_index] == NULL)
   return -1;
  /*check each clients handle if valid raise events to respective callback with the client handle*/

  if( client_id == DS_FWK_CLIENT_INVALID )
  {
    while ( temp_client_id < DS_FWK_CLIENT_MAX )
    {
      if(family == IPV4_ADDR)
      {
        if(ds_fwk_inst[fw_index]->client_info[temp_client_id].v4_client_handle != 
                                                   DS_FWK_INVALID_CLNT_HANDLE)
        {
          clnt_handle = ds_fwk_inst[fw_index]->client_info[temp_client_id].\
                        v4_client_handle;
        }
      }
      else if(family == IPV6_ADDR)
      {
        if(ds_fwk_inst[fw_index]->client_info[temp_client_id].v6_client_handle != 
                                                   DS_FWK_INVALID_CLNT_HANDLE)
        {
          clnt_handle = ds_fwk_inst[fw_index]->client_info[temp_client_id].\
                        v6_client_handle;
        }
      }
      else if(family == NON_IP_ADDR)
      {
        if(ds_fwk_inst[fw_index]->client_info[temp_client_id].nipd_client_handle != 
                                                   DS_FWK_INVALID_CLNT_HANDLE)
        {
          clnt_handle = ds_fwk_inst[fw_index]->client_info[temp_client_id].\
                        nipd_client_handle;
        }
      }
      if( (ds_fwk_event_buf[temp_client_id]!= NULL) && 
  		(ds_fwk_event_buf[temp_client_id]->event_cback_f_ptr != NULL)&&
  		 clnt_handle != DS_FWK_INVALID_CLNT_HANDLE )
      {
        DS_MSG3(MSG_LEGACY_HIGH,"ds_fwk_notify_events_ex:"
                                "client_id %d clnt_handle %d.event %d",
                                temp_client_id,clnt_handle,event);
        ds_fwk_event_buf[temp_client_id]->event_cback_f_ptr(
                        clnt_handle,
                        event,
                        event_info,
                        ds_fwk_event_buf[temp_client_id]->user_data_ptr);
        
      }
      clnt_handle = DS_FWK_INVALID_CLNT_HANDLE;
      temp_client_id++;
    }
  }
  else
  {
    if(client_id < DS_FWK_CLIENT_MAX )
    {
      if(family == IPV4_ADDR)
      {
        if(ds_fwk_inst[fw_index]->client_info[client_id].v4_client_handle != 
                                                   DS_FWK_INVALID_CLNT_HANDLE)
        {
          clnt_handle = ds_fwk_inst[fw_index]->client_info[client_id].\
                        v4_client_handle;
        }
      }
      else if(family == IPV6_ADDR)
      {
        if(ds_fwk_inst[fw_index]->client_info[client_id].v6_client_handle != 
                                                   DS_FWK_INVALID_CLNT_HANDLE)
        {
          clnt_handle = ds_fwk_inst[fw_index]->client_info[client_id].\
                        v6_client_handle;
        }
      }
      else if(family == NON_IP_ADDR)
      {
        if(ds_fwk_inst[fw_index]->client_info[client_id].nipd_client_handle !=  
                                                   DS_FWK_INVALID_CLNT_HANDLE)
        {
          clnt_handle = ds_fwk_inst[fw_index]->client_info[client_id].\
                        nipd_client_handle;
        }
      }
      if( (ds_fwk_event_buf[client_id]!= NULL) && 
          (ds_fwk_event_buf[client_id]->event_cback_f_ptr != NULL)&&
           clnt_handle != -1 )
      {
        DS_MSG3(MSG_LEGACY_HIGH,"ds_fwk_notify_events_ex:"
                                "client_id %d clnt_handle %d.event %d",
                                 client_id,clnt_handle,event);
         ds_fwk_event_buf[client_id]->event_cback_f_ptr(
                        clnt_handle,
                        event,
                        event_info,
                        ds_fwk_event_buf[client_id]->user_data_ptr);
        
      }
    }
  }
  if( event == PDN_FLOW_ADDED_EV  ||
      event == PDN_FLOW_MODIFIED_EV ||
      event == PDN_FLOW_DELETED_EV )
  {
    ds_fwk_qos_net_initiated_handle_event( event, event_info);
  }
  /*incase of PDN down events clear the client info*/
  if( event == PDN_DOWN_EV)
  {
    if(family == IPV6_ADDR) 
        ds_fwk_clear_v6_addresses(ds_fwk_inst[fw_index],client_id);
	
    ds_fwk_update_client_info(family,fw_index,
                              client_id,
                              clnt_handle,
                              TRUE);
    /*check if there are any  clients on this instance*/
    /*if there are no  clients delete the instance*/
    DS_MSG3(MSG_LEGACY_HIGH,"ds_fwk_notify_events_ex:"
                            "fw_index %d attach_inst %d.fw_index %d",
                            ds_fwk_inst[fw_index]->client_mask,
                            ds_fwk_inst[fw_index]->attach_inst,
                            fw_index);
    if( ds_fwk_inst[fw_index]->v4_pdn_ptr == NULL &&
        ds_fwk_inst[fw_index]->v6_pdn_ptr == NULL &&
        ds_fwk_inst[fw_index]->non_ip_pdn_ptr == NULL && 
        ds_fwk_inst[fw_index]->attach_inst != TRUE )
    {
      ds_fwk_delete_inst(fw_index);
    }
  }
   DS_MSG2(MSG_LEGACY_HIGH,"Recieved  event %d, flow mask 0x%x ",
              event,event_info.bearer_flow_mask);
  if(event == BEARER_FLOW_ENABLED_EV)
  {
    if (ds_pdn_cp_data_is_mo_exception_data_supported())
    {
      ds_fwk_inst[fw_index]->tx_flow_mask = event_info.bearer_flow_mask;
    }
  }
  if(event == BEARER_FLOW_DISABLED_EV)
  {
    if (ds_pdn_cp_data_is_mo_exception_data_supported())
    {
      if (event_info.bearer_flow_mask == DS_BEARER_APN_RATE_CTRL_MOX_ONLY_MASK ||
          event_info.bearer_flow_mask == DS_BEARER_TECH_SUSPENSION_MASK)
      {
        ds_fwk_inst[fw_index]->tx_flow_mask = event_info.bearer_flow_mask;
      }
    }
  }
  
  return 0;
}


/**********************data path setup**********************/

/*============================================================
  FUNCTION DS_FWK_DL_SIG_HDLR()

  DESCRIPTION
    DL packets signal handler from PS task.
      
  PARAMETERS
    sig - signal from PS task.
    user_data_ptr - user data

  RETURN VALUE
    TRUE - if packet processing is success
    FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
static boolean ds_fwk_dl_sig_hdlr
(
  ps_sig_enum_type      sig,
  void                * user_data_ptr
)
{
  ds_bearer_mgr_s   * bearer_ptr = NULL;
  dsm_item_type     * dsm_item_ptr = NULL;
  boolean             retval = FALSE;  
  q_type            * q_ptr = NULL;
  
  /*extract the bearer from user data*/
  bearer_ptr   = (ds_bearer_mgr_s*) user_data_ptr;
  if(TRUE == ds_bearer_mgr_validate_bearer_ptr(bearer_ptr))
  {
    q_ptr = bearer_ptr->a2_to_ps_wm.wm_item.q_ptr;
    if(q_ptr != NULL)
    {
      dsm_item_ptr = (dsm_item_type *)  dsm_dequeue(
                                        &(bearer_ptr->a2_to_ps_wm.wm_item));
      if (NULL == dsm_item_ptr)
      {
        return TRUE;
      }
    }
    else
    {    
      goto bail;
    }
  }
  else
  {
    goto bail;
  }
  /* invoke bearer input*/
  retval = ds_fwk_bearer_input(bearer_ptr,&dsm_item_ptr);  
  return retval;
 
 bail:       
    DS_MSG1(MSG_LEGACY_HIGH, "ds_fwk_dl_sig_hdlr Dropping packet"
            "as bearer is not valid %p",bearer_ptr);
    return TRUE;

}

/*============================================================
  FUNCTION DS_FWK_HW_TO_PS_NON_EMPTY_CBACK()

  DESCRIPTION
    invoked when HW pushes packets to hw_to_ps WM.
      
  PARAMETERS
   dsm_wm_ptr
   user_data_ptr

  RETURN VALUE
   none

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
static void ds_fwk_hw_to_ps_non_empty_cback
(
  dsm_watermark_type  * dsm_wm_ptr,
  void                * user_data_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DS_MSG0(MSG_LEGACY_HIGH, "ds_fwk_hw_to_ps_non_empty_cback");
  ps_set_signal((ps_sig_enum_type) user_data_ptr);
  return;
} /* ds_fwk_hw_to_ps_non_empty_cback() */

/*===========================================================================
FUNCTION DS_FWK_IS_PDN_SHARED()

DESCRIPTION
   Checks if the framework instance is shared between A7 and DUN clients
  
PARAMETERS
  fwk_inst    -  Framework instance
  pkt_version -  Pkt version
  
RETURN VALUE
  Boolean

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
boolean ds_fwk_is_pdn_shared
(
  ds_fwk_s_type *fwk_inst,
  uint8          pkt_version
)
{
#define  MAX_IP_TYPE  3

  if(fwk_inst != NULL)
  {
  	if(pkt_version == DS_FWK_OPT_V4_VERSION)
    {
      if((fwk_inst->client_mask & (1<<(DS_FWK_CLIENT_RMNET*MAX_IP_TYPE)))&&
         (fwk_inst->client_mask & (1<<(DS_FWK_CLIENT_RMSM*MAX_IP_TYPE))))
      {
          DS_MSG0(MSG_LEGACY_HIGH,"ds_fwk_is_pdn_shared:"
                  "pdn_sharing for V4 call %d" );
          return TRUE;
      }
    }
    else if(pkt_version == DS_FWK_OPT_V6_VERSION)
    {
      if((fwk_inst->client_mask & (1<<((DS_FWK_CLIENT_RMNET*MAX_IP_TYPE)+1)))&&
          (fwk_inst->client_mask & (1<<((DS_FWK_CLIENT_RMSM*MAX_IP_TYPE)+1))))
      {
         DS_MSG0(MSG_LEGACY_HIGH,"ds_fwk_is_pdn_shared:"
                 "pdn_sharing for V6 call %d" );
	     return TRUE; 
      }
    }
  }
  return FALSE;
}

/*============================================================
  FUNCTION DS_FWK_RESET_WM_COUNTERS()

  DESCRIPTION
    resets the WM counter
    
  PARAMETERS
   dsm_wm_ptr
   
  RETURN VALUE
   none

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
static void ds_fwk_reset_wm_counters
(
  dsm_watermark_type  * dsm_wm_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsm_wm_ptr->highest_cnt           = 0;
  dsm_wm_ptr->total_rcvd_cnt        = 0;
  dsm_wm_ptr->dne_discard_pkt_cnt   = 0;
  dsm_wm_ptr->dne_discard_bytes_cnt = 0;
  return;
} /* ds_fwk_reset_wm_counters() */


/*============================================================
  FUNCTION DS_FWK_SETUP_HW_TO_PS_WM()

  DESCRIPTION
    Sets hw_to_ps WM levels
    
  PARAMETERS
   bearer info which holds hw_to_ps wm.
   
  RETURN VALUE
   none

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
static void ds_fwk_setup_hw_to_ps_wm
(
  ds_fwk_bearer_info_type  * bearer_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    1. Initiatlize the queue
    2. Set WM levels
    3. Set non empty func ptr
    4. Reset WM counters
  -------------------------------------------------------------------------*/

  dsm_queue_init(&(bearer_info->bearer_ptr->a2_to_ps_wm.wm_item),
                 DS_FWK_HW_PS_WM_DNE,
                 &(bearer_info->bearer_ptr->a2_to_ps_wm.wm_queue));

  dsm_set_low_wm(&(bearer_info->bearer_ptr->a2_to_ps_wm.wm_item), 
                   DS_FWK_HW_PS_WM_LO);
  dsm_set_hi_wm(&(bearer_info->bearer_ptr->a2_to_ps_wm.wm_item), 
                   DS_FWK_HW_PS_WM_HI);
  dsm_set_dne(&(bearer_info->bearer_ptr->a2_to_ps_wm.wm_item),
                   DS_FWK_HW_PS_WM_DNE);
  dsm_set_dne_q_cnt( &(bearer_info->bearer_ptr->a2_to_ps_wm.wm_item), 
                   DS_FWK_HW_PS_WM_DNE_PKT_COUNT );

  bearer_info->bearer_ptr->a2_to_ps_wm.wm_item.non_empty_func_ptr =
                    ds_fwk_hw_to_ps_non_empty_cback;
  bearer_info->bearer_ptr->a2_to_ps_wm.wm_item.non_empty_func_data =
                   (void *) (bearer_info->signal);

  ds_fwk_reset_wm_counters(&(bearer_info->bearer_ptr->a2_to_ps_wm.wm_item));
} /* ps_dpmi_setup_hw_to_ps_wm() */


/*============================================================
  FUNCTION DS_FWK_REG_BEARER_WITH_HW()

  DESCRIPTION
    registers bearer with HW for data path.
    
  PARAMETERS
   bearer ptr
   
  RETURN VALUE
   0 - if succses
   -1 - otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
int ds_fwk_reg_bearer_with_hw
(
  ds_bearer_mgr_s * bearer_ptr
)
{
  uint8 dl_sig_index;
  /* Allocate singal for this bearer*/
  /*extract watermarks from bearer*/
  /*---------------------------------------------------------------------
    Setup DL sig for bearer
  ---------------------------------------------------------------------*/
  for (dl_sig_index = 0;
  	 dl_sig_index < DS_FWK_MAX_BEARER;
  	 dl_sig_index++)
  {
    if (FALSE == ds_fwk_dl_sig_in_use_tbl[dl_sig_index].in_use)
    {
      ds_fwk_dl_sig_in_use_tbl[dl_sig_index].in_use = TRUE;
      ds_fwk_dl_sig_in_use_tbl[dl_sig_index].signal = 
        (ps_sig_enum_type) (PS_FWK_DL_SIGNAL_0 + dl_sig_index);
      ds_fwk_dl_sig_in_use_tbl[dl_sig_index].bearer_ptr = bearer_ptr;
      ps_set_sig_handler(ds_fwk_dl_sig_in_use_tbl[dl_sig_index].signal,
                         ds_fwk_dl_sig_hdlr,
                        (void *) bearer_ptr);
      ps_enable_sig( ds_fwk_dl_sig_in_use_tbl[dl_sig_index].signal );
      break;
    }
  }
  
  if (DS_FWK_MAX_BEARER <= dl_sig_index)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_reg_bearer_with_hw():" 
                             "Couldn't find DL sig");
    return -1;
  }
  
  DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_reg_bearer_with_hw():"
                          " Assigned DL sig %d bearer id %d",
                          ds_fwk_dl_sig_in_use_tbl[dl_sig_index].signal,
                          bearer_ptr->bearer_id);
  
  /*-----------------------------------------------------------------------
    Setup HW to PS WM
  -----------------------------------------------------------------------*/
  ds_fwk_setup_hw_to_ps_wm(&ds_fwk_dl_sig_in_use_tbl[dl_sig_index]);

#ifndef TEST_FRAMEWORK
 DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_reg_bearer_with_hw(): PDCP WM:0x%p PS WM:0x%p",
			   &bearer_ptr->rx_wm.wm_item,&bearer_ptr->a2_to_ps_wm.wm_item);
 /*-----------------------------------------------------------------------
    Register l2_to_ps wmk
  -----------------------------------------------------------------------*/
  a2_dl_per_register_pdcp_watermark(bearer_ptr->bearer_id,
                                    &bearer_ptr->rx_wm.wm_item);
  /*-----------------------------------------------------------------------
    Register a2 to ps with A2
  -----------------------------------------------------------------------*/
  a2_dl_per_register_ds_watermark(bearer_ptr->bearer_id,
                                  &bearer_ptr->a2_to_ps_wm.wm_item);
#endif                                 

 return 0;
}


/*============================================================
  FUNCTION DS_FWK_DEREG_BEARER_WITH_HW()

  DESCRIPTION
    deregisters bearer with HW for data path.
    
  PARAMETERS
   bearer ptr
   
  RETURN VALUE
   0 - if succses
   -1 - otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
int ds_fwk_dereg_bearer_with_hw
(
  ds_bearer_mgr_s * bearer_ptr
)
{
  uint8 dl_sig_index;
  
  DS_ENTER_CRIT_SECTION(&ps_ds_crit_section);
  for (dl_sig_index = 0;
       dl_sig_index < DS_FWK_MAX_BEARER;
       dl_sig_index++)
  {
    if (TRUE == ds_fwk_dl_sig_in_use_tbl[dl_sig_index].in_use &&
        bearer_ptr == ds_fwk_dl_sig_in_use_tbl[dl_sig_index].bearer_ptr )
    {
      ds_fwk_dl_sig_in_use_tbl[dl_sig_index].in_use = FALSE;
      ds_fwk_dl_sig_in_use_tbl[dl_sig_index].bearer_ptr = NULL;
      break;
    }
  }
  
  if (DS_FWK_MAX_BEARER <= dl_sig_index)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_dereg_bearer_with_hw():"
                             " Couldn't find DL sig");
    DS_LEAVE_CRIT_SECTION(&ps_ds_crit_section);
    return -1;
  }
  
  ps_clr_signal(ds_fwk_dl_sig_in_use_tbl[dl_sig_index].signal);
    /*-------------------------------------------------------------------------
        Reset the signal to default handler.This way we avoid a race condiotion
        where we will end up in a loop when downlink packets are being 
        processed in dl_opt handler and bearer is deleted(we will always return
        TRUE and that signal is set again resulting in a loop). Setting it to 
        NULL might result in NULL pointer access By using default sig handler 
        we will process the signal at max once. 
      -----------------------------------------------------------------------*/
  ps_reset_sig_handler( ds_fwk_dl_sig_in_use_tbl[dl_sig_index].signal );

  DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_dereg_bearer_with_hw():" 
                          " Reclaimed DL sig %d bearer id %d",
                          ds_fwk_dl_sig_in_use_tbl[dl_sig_index].signal,
                          bearer_ptr->bearer_id);
  
  

#ifndef TEST_FRAMEWORK
  /*-----------------------------------------------------------------------
    Dereg l2 to ps with A2
  -----------------------------------------------------------------------*/
  DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_dereg_bearer_with_hw(): PDCP WM:0x%p PS WM:0x%p",
          &bearer_ptr->rx_wm.wm_item,&bearer_ptr->a2_to_ps_wm.wm_item);
  a2_dl_per_deregister_ds_pdcp_watermark(bearer_ptr->bearer_id);

  DS_MSG1(MSG_LEGACY_HIGH,"ds_fwk_dereg_bearer_with_hw invoked for the id %d"
            "Going to wait for signal DS_A2_DEREG_SIG ",bearer_ptr->bearer_id);
   
  (void)ds_wait( (rex_sigs_type) 1 <<
                 (rex_sigs_type)DS_A2_DEREG_SIG ); /* Wait for completion*/
  rex_clr_sigs( ds_get_ds_task_ptr(), 1 <<(rex_sigs_type)DS_A2_DEREG_SIG); 
#endif 

 if ( NULL != bearer_ptr->a2_to_ps_wm.wm_item.non_empty_func_ptr )
 {
   bearer_ptr->a2_to_ps_wm.wm_item.non_empty_func_ptr = NULL;
   bearer_ptr->a2_to_ps_wm.wm_item.non_empty_func_data = NULL;
   dsm_queue_destroy(&(bearer_ptr->a2_to_ps_wm.wm_item));
 }
  DS_LEAVE_CRIT_SECTION(&ps_ds_crit_section);
 return 0;
}

/**********************data path**********************/

/*============================================================
  FUNCTION DS_FWK_DATA_ACTIVITY_CLIENT_STATUS_IND()

  DESCRIPTION 
     Sets client status information in FWK Global stats structure.
     based on client status FWK decides whether
     to invoke data activity resumed system event or not.   

  PARAMETERS
   client_status    cient status if enalbe or not 
 
  RETURN VALUE
     None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
void ds_fwk_data_activity_client_status_ind
(
  boolean  client_status
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*----------------------------------------------------------------------
        1.reg_clnt_cnt is maintained to send DS_SYS_EVENT_DATA_ACTIVITY_RESUMED event
          when the count is >= 1.
        2.Each time when a client is registered this count is incremented
          When a client is deregistered this count will be decremented if current count is not 0.
       -----------------------------------------------------------------------*/
  if( client_status == TRUE )
  {
    ds_fwk_global_info.fwk_global_stats.reg_clnt_cnt++;
  }
  else
  {
    /*safe check: dont decrement if the count is already zero*/
    if( ds_fwk_global_info.fwk_global_stats.reg_clnt_cnt != 0 )
    {
      ds_fwk_global_info.fwk_global_stats.reg_clnt_cnt--;
    }
  }
  DS_MSG2(MSG_LEGACY_HIGH, "ds_fwk_data_activity_client_status_ind"
          "client status %d client count %d ",
           client_status,
           ds_fwk_global_info.fwk_global_stats.reg_clnt_cnt
           );

}/* ds_fwk_data_activity_client_status_ind */

/*============================================================
  FUNCTION DS_FWK_GET_MH()

  DESCRIPTION
   gets the mode handler ptr from passed ID
      
  PARAMETERS
   mh_id

  RETURN VALUE
    ds_fw_mh_info_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
ds_fw_mh_info_type * ds_fwk_get_mh(uint8 mh_id)
{
   if(ds_fwk_validate_mh_info(mh_id) ==TRUE)
   {
     return ds_fw_mh_info[mh_id];
   }
   else
   {
   	return NULL;
   }
}

/*============================================================
  FUNCTION DS_FWK_GET_GLOBAL_STATS()

  DESCRIPTION 
     Gets the global stats from framework

  PARAMETERS
   global_stats    global stats pointer to get the stats
 
  RETURN VALUE
     None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
void ds_fwk_get_global_stats
(
  ds_sys_ioctl_global_stats_type  *global_stats
)
{
  if(global_stats != NULL)
  {
    global_stats->global_pkt_cnt = 
               ds_fwk_global_info.fwk_global_stats.global_stats.global_pkt_cnt;
  }
  return;
}

/*============================================================
  FUNCTION DS_FWK_GLOBAL_STATS_INC()

  DESCRIPTION 
    increments global stats count in fwk global info

  PARAMETERS
   None   
 
  RETURN VALUE
     None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
void ds_fwk_global_stats_inc
(
)
{
  boolean  data_activity_flag   = TRUE;/*to send as event info*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*increment the packet count on particular subscription*/
  ds_fwk_global_info.fwk_global_stats.global_stats.global_pkt_cnt++;
    /*-------------------------------------------------------------------------
     1.If registered client count is >= 1 send PS_SYS_EVENT_DATA_ACTIVITY_RESUMED to 
       all regisred clients.
     2.Make sure no events are sent for further packets by initializing reg_clnt_cnt to zero.
      .reg_clnt_cnt is incremented next time when a client is registered again.
       --------------------------------------------------------------------------*/
  if( ds_fwk_global_info.fwk_global_stats.reg_clnt_cnt )
  {
    ds_fwk_global_info.fwk_global_stats.reg_clnt_cnt = 0;
    if( ds_sys_event_ind(DS_SYS_TECH_ALL,DS_SYS_DATA_ACTIVITY_RESUMED_EV,
                         &data_activity_flag /*to comply with ds_sys layer */
                         ) == -1 )
    {
      DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_global_stats_inc:"
              "Error in sending DS_SYS_DATA_ACTIVITY_RESUMED_EV"
              );
    }
  }
}
/*===========================================================
  FUNCTION DS_FWK_QTF_GET_HW_TO_PS_WM()

  DESCRIPTION
    gets the hw_to_ps wm for QTF
      
  PARAMETERS
   clnt_type
   client_handle

  RETURN VALUE
   watermark

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
dsm_watermark_type* ds_fwk_qtf_get_hw_to_ps_wm
(
  ds_fwk_client_type        clnt_type,
  ds_fwk_clnt_handle_type   client_handle
)
{
  ds_fwk_clnt_info_s_type  clnt_info_ptr;
  ds_bearer_mgr_s*         bearer_ptr = NULL;
  
  ds_fwk_get_clnt_info_from_handle(clnt_type,client_handle,&clnt_info_ptr);
  if( clnt_info_ptr.fw_inst != NULL )
  {
    if( clnt_info_ptr.ip_type == IPV4_ADDR )
    {
      bearer_ptr = clnt_info_ptr.fw_inst->v4_pdn_ptr->\
                      bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
    }
    else if( clnt_info_ptr.ip_type == IPV6_ADDR )
    {
      bearer_ptr = clnt_info_ptr.fw_inst->v6_pdn_ptr->\
                      bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
    }
    else if( clnt_info_ptr.ip_type == NON_IP_ADDR )
    {
      bearer_ptr = clnt_info_ptr.fw_inst->non_ip_pdn_ptr->\
                      bearer_ptr_tbl[DS_3GPP_DEFAULT_BEARER_INDEX];
    }
  }
  
  if( bearer_ptr != NULL )
  {
    return &(bearer_ptr->a2_to_ps_wm.wm_item);
  }
  
  return NULL;
}
    
/*===========================================================
FUNCTION DS_FWK_DEREG_DS_PDCP_WM_CB

DESCRIPTION
  This function sets the signal to DS task to clear the watermarks

PARAMETERS
  epsid     The epsid on which this watermark should be deregistered. 

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
=============================================================*/
void  ds_fwk_dereg_ds_pdcp_wm_cb
(
  uint8 epsid
)
{
  DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_dereg_ds_pdcp_wm_cb invoked for the id %d"
          "setting the signal to unblock the ds task ",epsid);
  
  ds_set_signal(DS_A2_DEREG_SIG);
  return;
}

/*===========================================================
  FUNCTION DS_FWK_GET_CLIENT_INFO()

  DESCRIPTION
    gets the hw_to_ps wm for QTF
      
  PARAMETERS
   clnt_type
   client_handle

  RETURN VALUE
   watermark

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
boolean ds_fwk_get_client_info
(
  ds_fwk_s_type             * fw_inst,
  ip_addr_enum_type           ip_type,
  ds_fwk_client_type        * clnt_type,
  ds_fwk_clnt_handle_type   * client_handle
)
{
  if( clnt_type == NULL || client_handle == NULL )
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_get_client_info: Invalid params");
    return FALSE;
  }
  *client_handle = DS_FWK_INVALID_CLNT_HANDLE;
  /*ipv4*/
  if( ip_type ==  IPV4_ADDR)
  {
    if(fw_inst->client_info[DS_FWK_CLIENT_RMNET].v4_client_handle != 
                 DS_FWK_INVALID_CLNT_HANDLE )
    {
      *clnt_type =  DS_FWK_CLIENT_RMNET;
      *client_handle = fw_inst->client_info[DS_FWK_CLIENT_RMNET].v4_client_handle;
    }
    else if( fw_inst->client_info[DS_FWK_CLIENT_RMSM].v4_client_handle != 
                 DS_FWK_INVALID_CLNT_HANDLE )
    {
      *clnt_type =  DS_FWK_CLIENT_RMSM;
      *client_handle = fw_inst->client_info[DS_FWK_CLIENT_RMSM].v4_client_handle;
    }
    else if( fw_inst->client_info[DS_FWK_CLIENT_ATCOP].v4_client_handle != 
                 DS_FWK_INVALID_CLNT_HANDLE )
    {
      *clnt_type =  DS_FWK_CLIENT_ATCOP;
      *client_handle = fw_inst->client_info[DS_FWK_CLIENT_ATCOP].v4_client_handle;
    }
#ifdef FEATURE_MODE_TLB
    else if( fw_inst->client_info[DS_FWK_CLIENT_TLB].v4_client_handle != 
                 DS_FWK_INVALID_CLNT_HANDLE )
    {
      *clnt_type =  DS_FWK_CLIENT_TLB;
      *client_handle = fw_inst->client_info[DS_FWK_CLIENT_TLB].v4_client_handle;
    }
#endif
  }
    /*ipv6*/
  else if( ip_type ==  IPV6_ADDR)
  {
    if(fw_inst->client_info[DS_FWK_CLIENT_RMNET].v6_client_handle != 
                 DS_FWK_INVALID_CLNT_HANDLE )
    {
      *clnt_type =  DS_FWK_CLIENT_RMNET;
      *client_handle = fw_inst->client_info[DS_FWK_CLIENT_RMNET].v6_client_handle;
    }
    else if( fw_inst->client_info[DS_FWK_CLIENT_RMSM].v6_client_handle != 
                 DS_FWK_INVALID_CLNT_HANDLE )
    {
      *clnt_type =  DS_FWK_CLIENT_RMSM;
      *client_handle = fw_inst->client_info[DS_FWK_CLIENT_RMSM].v6_client_handle;
    }
    else if( fw_inst->client_info[DS_FWK_CLIENT_ATCOP].v6_client_handle != 
                 DS_FWK_INVALID_CLNT_HANDLE )
    {
      *clnt_type =  DS_FWK_CLIENT_ATCOP;
      *client_handle = fw_inst->client_info[DS_FWK_CLIENT_ATCOP].v6_client_handle;
    }
#ifdef FEATURE_MODE_TLB
    else if( fw_inst->client_info[DS_FWK_CLIENT_TLB].v6_client_handle != 
                 DS_FWK_INVALID_CLNT_HANDLE )
    {
      *clnt_type =  DS_FWK_CLIENT_TLB;
      *client_handle = fw_inst->client_info[DS_FWK_CLIENT_TLB].v6_client_handle;
    }
#endif
  }
  /*nipd*/
  else if( ip_type ==  NON_IP)
  {
    if(fw_inst->client_info[DS_FWK_CLIENT_RMNET].nipd_client_handle != 
                 DS_FWK_INVALID_CLNT_HANDLE )
    {
      *clnt_type =  DS_FWK_CLIENT_RMNET;
      *client_handle = fw_inst->client_info[DS_FWK_CLIENT_RMNET].nipd_client_handle;
    }
    else if( fw_inst->client_info[DS_FWK_CLIENT_RMSM].nipd_client_handle != 
                 DS_FWK_INVALID_CLNT_HANDLE )
    {
      *clnt_type =  DS_FWK_CLIENT_RMSM;
      *client_handle = fw_inst->client_info[DS_FWK_CLIENT_RMSM].nipd_client_handle;
    }
    else if( fw_inst->client_info[DS_FWK_CLIENT_ATCOP].nipd_client_handle != 
                 DS_FWK_INVALID_CLNT_HANDLE )
    {
      *clnt_type =  DS_FWK_CLIENT_ATCOP;
      *client_handle = fw_inst->client_info[DS_FWK_CLIENT_ATCOP].nipd_client_handle;
    }
#ifdef FEATURE_MODE_TLB
    else if( fw_inst->client_info[DS_FWK_CLIENT_TLB].nipd_client_handle != 
                 DS_FWK_INVALID_CLNT_HANDLE )
    {
      *clnt_type =  DS_FWK_CLIENT_TLB;
      *client_handle = fw_inst->client_info[DS_FWK_CLIENT_TLB].nipd_client_handle;
    }
#endif
  }
  if ( *client_handle != DS_FWK_INVALID_CLNT_HANDLE )
  {
    DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_get_client_info():" 
                            " client type %d client handle %d",
                             *clnt_type,
                             *client_handle);
    return TRUE;
  }
  else
    return FALSE;
}

/*===========================================================================
FUNCTION DS_FWK_DELETE_PDN_SHARING_FILTERS()

DESCRIPTION
  Deletes the pdn sharing filters from the framework instance 
  
PARAMETERS
  fwk_index - Framework index to fetch the framework instance
  ip_ver    - Ip address verion of the filter to delete

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
void ds_fwk_delete_pdn_sharing_filters
(
  uint8              fwk_index,
  ip_addr_enum_type  ip_ver
)
{
  int16                         ps_errno;
  /*-----------------------------------------------------------------*/

  if( ds_fwk_inst[fwk_index] != NULL )
  {
    DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_delete_pdn_sharing_filters:"
                             "index %d inst 0x%p",
                              fwk_index,ds_fwk_inst[fwk_index]);	 

     ds_ipfltri_delete(ds_fwk_inst[fwk_index],
                       ip_ver,
                       FLTR_CLIENT_PDN_SHARING,
                       DS_IPFLTR_DELETE_ALL_HANDLE,
                       &ps_errno
                       );

  }

}

/*============================================================
  FUNCTION DS_FWK_LOOK_UP_PDN_BY_POLICY()

  DESCRIPTION
    Invokes corresponding Mode handler look up api to find the mathcing PDN for given policy.

  PARAMETERS
    client_id        Fwk client type
    policy_info      Policy on which the loop up is performed

  RETURN VALUE
    Client handle - if matches to any existing policy
    -1 - if not matches with any existing PDN policy.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
ds_fwk_clnt_handle_type  ds_fwk_get_fwk_inst_by_policy
(
  ds_fwk_client_type       client_id,
  ds_policy_info_type      policy_info
)
{
  /*------------------------------Local Declarations-------------------------- --*/
   uint8                     mh_id = ds_fwk_get_current_mh();
   ds_fwk_index_type         fwk_inst = DS_FWK_INST_INVALID;
  /*--------------------------------------------------------------------------*/
  
   DS_MSG2(MSG_LEGACY_HIGH,"ds_fwk_get_fwk_inst_by_policy" 
                           "client ID:%d lookup family %d",
                            client_id,policy_info.family);
   if((ds_fwk_validate_mh_info(mh_id) == TRUE) && 
   	  (ds_fw_mh_info[mh_id]->pdn_operations.look_up_f_ptr != NULL))
   {
      fwk_inst = ds_fw_mh_info[mh_id]->pdn_operations.look_up_f_ptr(policy_info);
   }
   if( fwk_inst < 0 || fwk_inst >= DS_FWK_MAX_INST)
   {
     DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_get_fwk_inst_by_policy" 
                              "client ID:%d lookup failed",
                              client_id);
     return DS_FWK_INST_INVALID;
   }
   
   DS_MSG3(MSG_LEGACY_HIGH,"ds_fwk_get_fwk_inst_by_policy:" 
             "v4 handle %d v6 handle %d nipd handle %d",
             ds_fwk_inst[fwk_inst]->client_info[client_id].v4_client_handle,
             ds_fwk_inst[fwk_inst]->client_info[client_id].v6_client_handle,
             ds_fwk_inst[fwk_inst]->client_info[client_id].nipd_client_handle);
   
  
   return fwk_inst;
}

/*===========================================================================
FUNCTION DS_FWK_IS_PDN_MO_EX

DESCRIPTION
  Function to determine whether MO-Exception is enabled or not
  
PARAMETERS
  fwk_inst - Framework instance
  ip_type - Ip Type for which Mo-exception is enabled
  
RETURN VALUE
  TRUE - MO-Exception is enabled for the PDN
  FALSE - MO-Exception is disabled for the PDN

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================*/
boolean ds_fwk_is_pdn_mo_ex 
(
  ds_fwk_s_type     *fwk_inst,
  ip_addr_enum_type ip_type
)
{
  if (fwk_inst == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_is_pdn_mo_ex - Fwk inst is NULL");
    return FALSE;
  }

  if((ip_type != IPV4_ADDR) && (ip_type != IPV6_ADDR) &&
     (ip_type != NON_IP_ADDR))
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_is_pdn_mo_ex - Invalid Ip Type %d",
            ip_type);
    return FALSE;
  }

  if (ip_type == IPV4_ADDR)
  {
    return ds_pdn_cp_data_is_mo_exception_data_supported_on_pdn(fwk_inst->v4_pdn_ptr);
  }
  else if (ip_type == IPV6_ADDR)
  {
    return ds_pdn_cp_data_is_mo_exception_data_supported_on_pdn(fwk_inst->v6_pdn_ptr);
  }
  else if (ip_type == NON_IP_ADDR)
  {
     return ds_pdn_cp_data_is_mo_exception_data_supported_on_pdn(fwk_inst->non_ip_pdn_ptr);
  }
  else
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_is_pdn_mo_ex - Unable to determine MO-Ex"
            "status %d", ip_type);
    return FALSE;
  }
}

/*===========================================================================
FUNCTION DS_IS_PKT_MO_EXCEPTION()

DESCRIPTION
  This function determines whether the packet MO-Exception or not

DEPENDENCIES
  None

RETURN VALUE
  TRUE for a successful match
  FALSE for no match

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_is_pkt_mo_exception
(
  sio_stream_id_type      stream_id,
  dsm_item_type           *rx_pkt
)
{
  uint8                          ip_hdr_first_byte;
  uint16                         version = 0;
  ip_addr_enum_type              ip_type;
  rmnet_instance_e_type          rmnet_instance = RMNET_INSTANCE_MAX;
  ds_fwk_clnt_info_s_type        clnt_info_ptr ={0};
  ds_fwk_s_type                  *ds_fwk_inst = NULL;
  boolean                        pdn_mo_ex_cap = FALSE;
  ps_tx_meta_info_type           *mo_tx_meta_info_ptr = NULL;
  errno_enum_type                err_no;
  boolean                        is_mox_data_pkt = FALSE;
  rmnet_smi_info_type            *info = NULL;
  rmnet_smi_ip_info_type         *ip_info = NULL;

  if (!ds_pdn_cp_data_is_mo_exception_data_supported())
  {
    /*
      MO-EX is valid only for NB1. For CATM1 and G, we can dont ve to do checks
    */
     DS_MSG0(MSG_LEGACY_ERROR, "CP MO_EX not supported ");
    return FALSE;
  }

  if (stream_id == SIO_NO_STREAM_ID || rx_pkt == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR, "streamID is invalid or DSM is NULL");
    return FALSE;
  }

  (void) dsm_extract(rx_pkt, 0, &ip_hdr_first_byte, 1);

  version = ip_hdr_first_byte & RMNET_OPT_IP_VERSION_MASK;

  if(version == RMNET_OPT_V4_VERSION)
  {
    ip_type = IPV4_ADDR;
  }
  else if (version == RMNET_OPT_V6_VERSION)
  {
    ip_type = IPV6_ADDR;
  }
  else 
  {
    return FALSE;
  }


  rmnet_instance = rmnet_get_rmnet_inst_from_stream_id(stream_id,ip_type,
                                                       (void**)&info);
  
  if((rmnet_instance == RMNET_INSTANCE_MAX) || (info == NULL))
  {
    DS_MSG0(MSG_LEGACY_ERROR, "Cannot get RmNET instance");
    return FALSE;
  }

    /* Check if the PDN supports MO-EX*/
  ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_RMNET,rmnet_instance,
                                   &clnt_info_ptr);

  if(clnt_info_ptr.fw_inst == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"No framework "
            "instance could be found for this call");
    return FALSE;
  }
  else
  {
    ds_fwk_inst = clnt_info_ptr.fw_inst;
  }

  pdn_mo_ex_cap = (ip_type == IPV4_ADDR)?\
                   ds_fwk_is_pdn_mo_ex(ds_fwk_inst, IPV4_ADDR):\
                   ds_fwk_is_pdn_mo_ex(ds_fwk_inst, IPV6_ADDR);

  DS_MSG1(MSG_LEGACY_HIGH,"pdn_mo_ex_cap %d",pdn_mo_ex_cap);

  if(pdn_mo_ex_cap == FALSE)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"MO-exception is not enabled for the PDN");
    return FALSE;
  }	

  if(DS_FWK_NUM_FILTERS(ds_fwk_inst, FLTR_CLIENT_MO_EXCP) == 0)
  {
    DS_MSG1(MSG_LEGACY_ERROR,"NO-exception filters installed on this fwk"
            "instance %p",ds_fwk_inst);
    return FALSE;
  }

  DS_MSG0(MSG_LEGACY_HIGH,"ds_is_pkt_mo_exception:"
          "MO exceptional filters registered");

  PS_TX_META_INFO_AND_RT_META_INFO_GET(mo_tx_meta_info_ptr);

  if((mo_tx_meta_info_ptr != NULL )&&
     (mo_tx_meta_info_ptr->rt_meta_info_ptr != NULL))
  {
    (&(PS_TX_META_GET_PKT_INFO( mo_tx_meta_info_ptr)))->\
                  is_fraghdl_allocated = 2;//INVALID;
    mo_tx_meta_info_ptr->rt_meta_info_ptr->is_mo_ex = TRUE;
    if(ps_pkt_info_filter_tx_pkt(ds_fwk_inst,
                                 FLTR_CLIENT_MO_EXCP,
                                 &rx_pkt,
                                 &mo_tx_meta_info_ptr,
                                 &err_no) < 0)
    {
      DS_MSG1(MSG_LEGACY_ERROR,"ds_is_pkt_mo_exception: "
              "ps_pkt_info_filter_tx_pkt returned errno = %d", err_no);
     goto bail;
    }
    else
    {
      is_mox_data_pkt = (mo_tx_meta_info_ptr != NULL) &&
      (PS_TX_META_GET_FILTER_RESULT( mo_tx_meta_info_ptr, FLTR_CLIENT_MO_EXCP));

       DS_MSG1(MSG_LEGACY_HIGH,"ds_is_pkt_mo_exception: "
               "is_mox_data_pkt %d", is_mox_data_pkt);

       if (is_mox_data_pkt)
       {
         if (ds_fwk_inst->tx_flow_mask == DS_BEARER_TECH_SUSPENSION_MASK ||
             ds_fwk_inst->tx_flow_mask ==DS_BEARER_APN_RATE_CTRL_MOX_ONLY_MASK)
         {
           ip_info = rmnet_get_ip_info_from_inst(info->constants.ip_inst);
           if(ip_info != NULL)
           {
             PS_RM_SET_SIGNAL(ip_info->rmnet_ul_sig);
             if(!ip_info->is_mo_ex_pkt_queued)
             {
               ip_info->is_mo_ex_pkt_queued = TRUE;
             }
           }
         }
       }
       if(mo_tx_meta_info_ptr != NULL)
       {
         if(mo_tx_meta_info_ptr->rt_meta_info_ptr != NULL)
         {
            mo_tx_meta_info_ptr->rt_meta_info_ptr->is_mo_ex = FALSE;
         }
         PS_TX_META_INFO_FREE(&mo_tx_meta_info_ptr);
       }
       return is_mox_data_pkt;
    }
  }

bail:

  if (mo_tx_meta_info_ptr != NULL)  
  {
    if (mo_tx_meta_info_ptr->rt_meta_info_ptr != NULL)
    {
      mo_tx_meta_info_ptr->rt_meta_info_ptr->is_mo_ex = FALSE;
    }
    PS_TX_META_INFO_FREE(&mo_tx_meta_info_ptr);
  }
  return FALSE;
}


#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*============================================================
  FUNCTION DS_FWK_GET_DL_THROUGHPUT_STATUS()

  DESCRIPTION
   Returns the DL throughput status.
      
  PARAMETERS
   None

  RETURN VALUE
    boolean
    0 - DL Throughput enhancement is enabled.
    1 - DL Throughput enhancement is disabled.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
boolean ds_fwk_get_dl_throughput_status ()
{
  boolean ret_val = FALSE;
  
  DS_ENTER_CRIT_SECTION(&ps_ds_crit_section);
  ret_val = ds_fwk_dl_tpt_val;
  DS_LEAVE_CRIT_SECTION(&ps_ds_crit_section);

  return ret_val;
}

/*============================================================
  FUNCTION DS_FWK_SET_DL_THROUGHPUT_STATUS()

  DESCRIPTION
   Sets DL throughput status.
      
  PARAMETERS
   boolean

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
void ds_fwk_set_dl_throughput_status
(
  boolean dl_tpt_status
)
{
  DS_ENTER_CRIT_SECTION(&ps_ds_crit_section);
  ds_fwk_dl_tpt_val = dl_tpt_status;
  DS_LEAVE_CRIT_SECTION(&ps_ds_crit_section);
  
  return;
}



/*============================================================
  FUNCTION DS_FWK_IS_MODEM_EMBEDDED_CALL_ACTIVE()

  DESCRIPTION
   This API determines if there is a modem embedded call active
   on the fwk instance.
      
  PARAMETERS
   fwk_inst - Framework instance
   ip_type  - IP type of the call

  RETURN VALUE
    boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
boolean ds_fwk_is_modem_embedded_call_active
(
  ds_fwk_s_type     *fwk_inst,
  ip_addr_enum_type ip_type
)
{
  if (fwk_inst == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ds_fwk_is_modem_embedded_call_active"
            " - Fwk inst is NULL");
    return FALSE;
  }

  if((ip_type != IPV4_ADDR) && (ip_type != IPV6_ADDR) )
  {
    DS_MSG1(MSG_LEGACY_ERROR,"ds_fwk_is_modem_embedded_call_active "
            " - Invalid Ip Type %d", ip_type);
    return FALSE;
  }

  DS_MSG3(MSG_LEGACY_ERROR,"ds_fwk_is_modem_embedded_call_active"
            " - ip type %d , v4 handle %d, v6 handle %d",
            ip_type, 
            fwk_inst->client_info[DS_FWK_CLIENT_SOCKETS].v4_client_handle,
            fwk_inst->client_info[DS_FWK_CLIENT_SOCKETS].v6_client_handle);

  if ((ip_type == IPV4_ADDR) &&
      (fwk_inst->client_info[DS_FWK_CLIENT_SOCKETS].v4_client_handle != \
       DS_FWK_INVALID_CLNT_HANDLE))
  {
    return TRUE;
  }
  else if ((ip_type == IPV6_ADDR) &&
      (fwk_inst->client_info[DS_FWK_CLIENT_SOCKETS].v6_client_handle != \
       DS_FWK_INVALID_CLNT_HANDLE))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/*============================================================
  FUNCTION DS_FWK_GET_TX_CB()

  DESCRIPTION
   This API retrieves the TX callback registered for a particular framework client
      
  PARAMETERS
   Client Id

  RETURN VALUE
    Function pointer of the registered callback

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
============================================================*/
ds_fwk_tx_buf_type*  ds_fwk_get_tx_cb( ds_fwk_client_type   client_id)
{
  /*check whether the client ID passed is valid or not*/
  if( !ds_fwk_validate_client_id(client_id))
    return NULL;
  
  if(ds_fwk_tx_buf[client_id] != NULL)
  {
    DS_MSG1(MSG_LEGACY_ERROR," CB registered for clientId %d", client_id);
    return ds_fwk_tx_buf[client_id];
  }
  else 
  {
    DS_MSG1(MSG_LEGACY_ERROR," No callback reg for client id %d", client_id);
    return NULL;
  }


}



