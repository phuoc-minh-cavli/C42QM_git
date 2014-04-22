/*===========================================================================

   D A T A   S E R V I C E S   C O N T R O L   P A T H     F I L E

DESCRIPTION

 The Data Services control path files contains support for call bring up/
 teardown implementation for the clients.

Copyright (c) 2018-2020 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
==============================================================================
08/08/18        sr              Initial Version
===========================================================================*/

#include "comdef.h"
#include "dss_control.h"
#include "ds_crit_sect.h"
#include "dss_ps.h"
#include "dsstcp.h"
#ifdef FEATURE_DATAMODEM_DNS
#include "dssdns.h"
#endif

#ifdef FEATURE_DATAMODEM_DSS_PING
#include "dss_ping_comm_mgr.h"
#endif /* FEATURE_DATAMODEM_DSS_PING */


/*---------------------------------------------------------------------------
  dss ping critical section
---------------------------------------------------------------------------*/
ds_crit_sect_type dss_ping_crit_section;

uint8 dss_handle_map[DSS_MAX_HANDLE] = {0};

#ifdef FEATURE_DATA_PS_QOS
uint8 dss_qos_handle_map[DSS_MAX_QOS_NET_INITIATED_REQUESTS] = {0};
#endif


/*===========================================================================
FUNCTION DSS_PDN_EVENT_CBACK()

DESCRIPTION

  This API is registered at DS task boot up with DS FWK.
  It is invoked whenever there is a DS PDN event which need to be processed by
  DSS

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
==============================================================================*/
void dss_pdn_event_cback
(
  ds_fwk_clnt_handle_type    client_handle,
  ds_pdn_event_enum_type     event,
  ds_pdn_event_info_u_type   event_info,
  void                      *user_data_ptr
)
{
  ds_command_type *cmd_ptr = NULL;
  dss_control_callback_info_s *callback_info = NULL;

  if (client_handle == DSS_INVALID_HANDLE)
  {
    DS_MSG1(MSG_LEGACY_ERROR,
            "dss_pdn_event_cback : No valid handle found for %d",
                   client_handle);
    return;
  }


  cmd_ptr = ds_allocate_cmd_buf(sizeof(dss_control_callback_info_s));
  if (cmd_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,
            "dss_pdn_event_cback: Not able to allocate memory for cmd ptr");
    return;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_DSS_CALL_CALLBACK_EVT;
	
    callback_info = (dss_control_callback_info_s*)cmd_ptr->cmd_payload_ptr;
    memset (callback_info, 0, sizeof (dss_control_callback_info_s));

    callback_info->client_handle = client_handle;
    callback_info->event = event;

    memscpy(&callback_info->event_info,
            sizeof(ds_pdn_event_info_u_type),
            &event_info, 
           sizeof(ds_pdn_event_info_u_type));

    /*For user_data_ptr we dont ve to explicitly allocate memory as this will
      be a valid memory passed. It would be sufficient to point user_data_ptr*/

    callback_info->user_data_ptr = user_data_ptr;

    ds_put_cmd(cmd_ptr);
  }
}



/*=====================================================================
FUNCTION DSS_BRINGUP_CALL()
DESCRIPTION
  Opens up the network library.  Assigns application ID and sets the
  application-defined callback functions to be called when library and
  socket calls would make progress. Stores the network policy info and
  brings up the data call with the policy info.
  Puts data services manager into "socket" mode.
  This function is called from the context of the socket client's task.
DEPENDENCIES
  None.
RETURN VALUE
  Returns application ID on success.
  On error, return DSS_SUCCESS and places the error condition value in
  *dss_errno.
  dss_errno Values
  DS_EMAPP      no more applications available - max apps exceeded.
  DS_EFAULT     Policy structure is uninitialized.
  DS_EWOULDBLOCK Call bringup is in process events will be notified
  DSS_SUCCESS call bringup successful
=====================================================================*/
sint15 dss_bringup_call
(
  dss_net_lib_info_type_s *net_params,
  sint15                  *dss_errno        /* error condition value */
)
{

  sint15 dss_nethdl = 0;
  int dss_handle = -1;
  acb_type *acb_ptr = NULL;
  ds_command_type *cmd_ptr = NULL;
  dss_control_bringup_type_s *bringup_info = NULL;
  int8 client_handle = DSS_INVALID_HANDLE;

  if (net_params == NULL || dss_errno == NULL)
  {
    DS_ERROR_LOG_0("dss_bringup_call: Net params or dss_errno are NULL");
    return DSS_ERROR;
  }

  if ((net_params->family != DSS_AF_INET) && (net_params->family != DSS_AF_INET6))
  {
    DS_ERROR_LOG_1("dss_bringup_call:Invalid IP type %d", net_params->family);
    *dss_errno = DS_EAFNOSUPPORT;
    return DSS_ERROR;
  }

 /*==============================================================================
  Create ACB, populate the fields and return the application handle(dss_nethandle)
==============================================================================*/
  dss_nethdl = dss_create_acb (net_params, dss_errno);
  if (dss_nethdl == DSS_ERROR)
  {
    DS_ERROR_LOG_0("dss_create_acb failed");
    return DSS_ERROR;
  }

  acb_ptr = dssocki_dss_nethandletoacb(dss_nethdl);
  if ( acb_ptr == NULL)
  {
    DS_ERROR_LOG_0("dss_bringup_call: ACB pointer is NULL");
    return DSS_ERROR;
  }

   DS_MSG1(MSG_LEGACY_HIGH,"dss_bringup_call : DSS Nethandle created %d",
   	       dss_nethdl);

/*==============================================================================
  Look up to see if a call is already present for the Policy and IP Type
==============================================================================*/
  dss_handle =  dss_perform_policy_match(net_params);

  if (dss_handle == DS_FWK_INVALID_CLNT_HANDLE)
  {

    /*Post a cmd to DS task and bring up the call in DS task context*/

    cmd_ptr = ds_allocate_cmd_buf(sizeof(dss_control_bringup_type_s));
    if (cmd_ptr == NULL)
    {
      DS_ERROR_LOG_0("dss_bringup_call: Not able to allocate memory for cmd ptr");
      *dss_errno = DS_ENOMEM;
      dssocki_freeacb(acb_ptr);
      return DSS_ERROR;
    }
    else
    {
       /*Allocate a free DSS Handle*/
      client_handle = dss_allocate_handle();

      if (client_handle == DSS_INVALID_HANDLE)
      {
        DS_ERROR_LOG_0("Unable to allocate DSS handle. Abort call bringup");
        *dss_errno = DS_EINVAL;
        dssocki_freeacb(acb_ptr);
        DS_SYSTEM_HEAP_MEM_FREE(cmd_ptr);
        return DSS_ERROR;
      }

      acb_ptr->dss_handle = client_handle;

      cmd_ptr->hdr.cmd_id = DS_CMD_DSS_BRINGUP_CALL_EVT;

      bringup_info = (dss_control_bringup_type_s*)cmd_ptr->cmd_payload_ptr;
      memset(bringup_info, 0, sizeof (dss_control_bringup_type_s));

      bringup_info->dss_nethandle = dss_nethdl;

      if (bringup_info->policy_info == NULL)
      {
        DS_SYSTEM_HEAP_MEM_ALLOC(bringup_info->policy_info,
                                 sizeof(ds_policy_info_type),
                                 ds_policy_info_type*);
        if(bringup_info->policy_info == NULL)
        {
          DS_ERROR_LOG_0("dss_bringup_call: Not able to allocate memory for cmd ptr");
          *dss_errno = DS_ENOMEM;
           dssocki_freeacb(acb_ptr);
           DS_SYSTEM_HEAP_MEM_FREE(cmd_ptr);
           return DSS_ERROR;
        }

        memset(bringup_info->policy_info, 0, sizeof(ds_policy_info_type));
        bringup_info->policy_info->family = net_params->family;
        bringup_info->policy_info->apn_type = net_params->apn_type;

        if (net_params->name != NULL)
        {
          bringup_info->policy_info->length = strlen((const char*)net_params->name);
		  if (bringup_info->policy_info->length != 0)
		  {
            strlcpy ((char*)bringup_info->policy_info->name,
                     (char*) net_params->name,
                     (bringup_info->policy_info->length + 1));
		  }
        }

        if (net_params->authentication_params.auth_param_valid)
        {
          bringup_info->policy_info->authentication_params.auth_param_valid = \
                             net_params->authentication_params.auth_param_valid;

          bringup_info->policy_info->authentication_params.auth_params.auth_type = \
                        net_params->authentication_params.auth_params.auth_type;

          strlcpy(bringup_info->policy_info->authentication_params.auth_params.password,
                  net_params->authentication_params.auth_params.password,
                  DS_PROFILE_MAX_AUTH_STRING_LEN+1);

          strlcpy(bringup_info->policy_info->authentication_params.auth_params.username,
                  net_params->authentication_params.auth_params.username,
                  DS_PROFILE_MAX_AUTH_STRING_LEN+1);
        }
        ds_put_cmd(cmd_ptr);
      }
    }
  }
  else
  {
    //Map the DSS Nethandle and dss handle and increment the reference count
    acb_ptr->dss_handle = dss_handle;
    dss_incr_handle_ref_cnt(dss_handle);
    dss_store_fw_inst_in_acb(acb_ptr, dss_handle);

    /*Indicate the call is already present using DSS ERRNO. We will not be
      posting NET events to DSS clients in this case. This will avoid processing
      overhead in DSS as well*/
    *dss_errno = DSS_SUCCESS;
    return dss_nethdl;
  }

  *dss_errno = DS_EWOULDBLOCK;

  return dss_nethdl;
}

/*==============================================================================
FUNCTION DSS_OPEN_NETWORK_LIBRARY()

DESCRIPTION

  Opens up the network library.  Assigns application ID and sets the
  application-defined callback functions to be called when library and
  socket calls would make progress. Stores the network policy info and
  uses it in further calls.

  Puts data services manager into "socket" mode.

  This function is called from the context of the socket client's task.

DEPENDENCIES
  None.

RETURN VALUE
  Returns application ID on success.

  On error, return DSS_SUCCESS and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EMAPP      no more applications available - max apps exceeded.
  DS_EFAULT     Policy structure is uninitialized.

SIDE EFFECTS
  Puts data services manager into "socket" mode.
==============================================================================*/
sint15 dss_create_acb
(
  dss_net_lib_info_type_s *net_params,
  sint15                  *dss_errno        /* error condition value */
)
{
  acb_type *acb_ptr;           /* ptr to application control block */

  /*-------------------------------------------------------------------------
    Allocate ACB
  -------------------------------------------------------------------------*/
  if ( (acb_ptr = dssocki_allocacb()) == NULL)
  {
    *dss_errno = DS_EMAPP;
    return (DSS_ERROR);
  }

  /*-------------------------------------------------------------------------
    Set callback functions
  -------------------------------------------------------------------------*/
  acb_ptr->net_cb            = net_params->net_cb;
  acb_ptr->net_cb_user_data  = net_params->net_cb_user_data;
  acb_ptr->sock_cb           = net_params->sock_cb;
  acb_ptr->sock_cb_user_data = net_params->sock_cb_user_data;
  acb_ptr->dss_handle        = DSS_INVALID_HANDLE;

  /*-------------------------------------------------------------------------
    Initialize the netdown reason in the acb.
  -------------------------------------------------------------------------*/
  acb_ptr->netdown_reason    = DS_NET_DOWN_REASON_NOT_SPECIFIED;

  return (acb_ptr->dss_nethandle);
}


/*=============================================================================
FUNCTION DSS_PERFORM_POLICY_MATCH()

DESCRIPTION

  This API checks if a call is already present in with the Policy information
  passed.


DEPENDENCIES
  None.

RETURN VALUE
  If a call is already present DSS Handle is returned by DS FWK
  If a call is not present -1 is returned


SIDE EFFECTS
  None
==============================================================================*/
int dss_perform_policy_match
(
  dss_net_lib_info_type_s *net_params
)
{
  ds_policy_info_type policy_info = {0};
  ds_fwk_clnt_handle_type client_hdl = DSS_INVALID_HANDLE;

  memset(&policy_info, 0, sizeof(ds_policy_info_type));

  policy_info.apn_type = net_params->apn_type;
  policy_info.family   = net_params->family;

  if(net_params->name != NULL)
  {
    policy_info.length   = strlen ((const char*)net_params->name);
    if (policy_info.length != 0)
    {
      strlcpy(policy_info.name, (const char*)net_params->name, policy_info.length+1);
    }
  }

  client_hdl = ds_fwk_look_up_pdn_by_policy(DS_FWK_CLIENT_SOCKETS, policy_info);

  return client_hdl;
  
}


/*===========================================================================
FUNCTION DSS_GET_HANDLE()

DESCRIPTION

  This API gets the DSS handle corresponding to the Nethandle stores in the ACB

DEPENDENCIES
  None.

RETURN VALUE
  None


SIDE EFFECTS
  None
==============================================================================*/
sint15 dss_get_handle
(
  sint15 dss_nethandle
)
{
  acb_type *acb_ptr;

/*-------------------------------------------------------------------------
  Get acb_ptr and check for invalid application ID
-------------------------------------------------------------------------*/
  acb_ptr = dssocki_dss_nethandletoacb(dss_nethandle);

  if ( acb_ptr == NULL)
  {
    DS_ERROR_LOG_0("dss_get_handle: ACB pointer is NULL");
    return (DSS_INVALID_HANDLE);
  }

  return (acb_ptr->dss_handle);
}

/*===========================================================================
FUNCTION DSS_BRINGUP_PDN()

DESCRIPTION

  This API brings up the data call as per the policy passed by the DSS client


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
==============================================================================*/
void dss_bringup_pdn
(
  dss_control_bringup_type_s     *bringup_info
)
{
  int                                result = 0;
  ds_policy_info_type                policy_info = { 0 };
  ds_fwk_response_info_s_type        resp_info;
  acb_type                           *acb_ptr = NULL;
  ds_pdn_event_info_u_type           event_info;

  memset(&event_info, 0, sizeof(ds_pdn_event_info_u_type));

  acb_ptr = dssocki_dss_nethandletoacb(bringup_info->dss_nethandle);
  if ( acb_ptr == NULL)
  {
    DS_ERROR_LOG_0("dss_bringup_pdn: ACB pointer is NULL.Abort call bringup");
    goto bail;
  }

  /*Initialize the Policy info params which are passed by DSS client*/
  policy_info.apn_type = bringup_info->policy_info->apn_type;
  policy_info.family = bringup_info->policy_info->family;
  policy_info.length = bringup_info->policy_info->length;

  if (policy_info.length != 0)
  {
    strlcpy(policy_info.name, bringup_info->policy_info->name,
            policy_info.length+1);
  }

  if(bringup_info->policy_info->authentication_params.auth_param_valid)
  {
      policy_info.authentication_params.auth_param_valid = \
            bringup_info->policy_info->authentication_params.auth_param_valid;

    policy_info.authentication_params.auth_params.auth_type = \
           bringup_info->policy_info->authentication_params.auth_params.auth_type;

    strlcpy(policy_info.authentication_params.auth_params.password,
            bringup_info->policy_info->authentication_params.auth_params.password,
            DS_PROFILE_MAX_AUTH_STRING_LEN+1);

    strlcpy(policy_info.authentication_params.auth_params.username,
            bringup_info->policy_info->authentication_params.auth_params.username,
            DS_PROFILE_MAX_AUTH_STRING_LEN+1);

  }

  /*Bring up the data call*/
  result = ds_fwk_bringup_pdn_by_policy(DS_FWK_CLIENT_SOCKETS,
                                        policy_info,
                                        acb_ptr->dss_handle,
                                        &resp_info);

  /*Call bring up on Additional PDN.OTA will be sent.PDN Manager will send 
    EWOULDBLOCK
 .*/
  if (result == DS_EWOULDBLOCK)
  {
    DS_MSG1(MSG_LEGACY_HIGH,
            "Triggered call bring up for DSS handle %d,waiting for NETISCONN ",
            acb_ptr->dss_handle);
  }
  /*Call Bring up on Default PDN. No OTA will be send when DSS requests for call
    bring up as attach procedure is already done. PDN Manager should just incr
    their ref count. DSS should post NETISCONN to the DSS client though because
    from DSS perspective this is the first call bring up for this APN and IP Type
  */
  else if (result == DSS_SUCCESS)
  {
    DS_MSG0(MSG_LEGACY_HIGH,
            "dss_bringup_pdn : NETISCONN to DSS Client will be"
            " posted from FWK");
  }
  else if (result == DSS_ERROR) // call failed
  {
    DS_ERROR_LOG_3("dss_bringup_pdn: Call bring up failed, "
                   "DSS handle: %d, Netdown reason type %u,Netdown reason %u",
                   acb_ptr->dss_handle, resp_info.netdown_reason_type,
                   resp_info.netdown_reason);
    goto bail;
  }

    dss_store_fw_inst_in_acb(acb_ptr, acb_ptr->dss_handle);

bail:

  if (bringup_info->policy_info != NULL)
  {
    DS_SYSTEM_HEAP_MEM_FREE(bringup_info->policy_info);
  }

  return;
}


/*===========================================================================
FUNCTION DSS_ALLOCATE_HANDLE()

DESCRIPTION

  This API allocates an available handle from the pool of DSS handles

DEPENDENCIES
  None.

RETURN VALUE
      DSS handle from the DSS handle pool


SIDE EFFECTS
  None
==============================================================================*/
int8 dss_allocate_handle(void)
{
  int8 index = 0;

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  for (index=0 ; index < DSS_MAX_HANDLE; index++)
  {
    if (dss_handle_map[index] == DSS_HANDLE_FREE)
    {
      dss_handle_map[index] = DSS_HANDLE_ASSIGNED;
      break;
    }
  }

  if (index == DSS_MAX_HANDLE)
  {
    index = DSS_INVALID_HANDLE;
  }
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  DS_MSG1(MSG_LEGACY_HIGH,"dss handle allocated %d",index);

  return index;
}


/*===========================================================================
FUNCTION DSS_INCR_HANDLE_REF_CNT()

DESCRIPTION

  This API increments the reference count for the DSS handle if a call is already
  present for the requested Policy

DEPENDENCIES
  None.

RETURN VALUE
  None


SIDE EFFECTS
  None
==============================================================================*/
void dss_incr_handle_ref_cnt
(
  sint15 dss_handle
)
{
  
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  dss_handle_map[dss_handle]++;
  DS_MSG2(MSG_LEGACY_HIGH,"dss_incr_handle_ref_cnt: DSS Handle %u ref count %u",
          dss_handle, dss_handle_map[dss_handle]);

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
}

/*===========================================================================
FUNCTION DSS_DECR_HANDLE_REF_CNT()

DESCRIPTION

  This API decrements the reference count for the DSS handle 

DEPENDENCIES
  None.

RETURN VALUE
  None


SIDE EFFECTS
  None
==============================================================================*/

void dss_decr_handle_ref_cnt
(
  sint15 dss_handle
)
{
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  if (dss_handle_map[dss_handle] > DSS_HANDLE_FREE)
  {
    dss_handle_map[dss_handle]--;
  }
  DS_MSG2(MSG_LEGACY_HIGH,"dss_decr_handle_ref_cnt: DSS Handle %u ref count %u",
          dss_handle, dss_handle_map[dss_handle]);
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
}

/*===========================================================================
FUNCTION DSS_POST_CLIENT_NET_EVT()

DESCRIPTION

  This API will invoke the NET callback registered by DSS clients

DEPENDENCIES
  None.

RETURN VALUE
  None


SIDE EFFECTS
  None
==============================================================================*/
void dss_post_client_net_evt
(
  uint8 dss_handle,
  uint8 pdn_evt,
  ds_pdn_event_info_u_type   event_info
)
{
  DS_MSG2(MSG_LEGACY_HIGH,"dss_post_client_net_evt, event %d, dss_handle %d",
          pdn_evt,dss_handle);

  if (pdn_evt == DS_ENETISCONN)
  {
    dss_process_pdn_up_ev_cback(dss_handle);
  }
  else if (pdn_evt == DS_ENETNONET)
  {
    dss_process_pdn_down_ev_cback(dss_handle,event_info.pdn_down_info);
  }
}


/*===========================================================================
FUNCTION DSS_BRINGDOWN_CALL()
DESCRIPTION
  Closes the data call for the application.  All sockets must have
  been closed for the application, prior to closing.  .  If this
  is the last active application using the network library, this function
  takes the data services manager out of "socket" mode.
  This function is called from the context of the socket client's task.
  It also closes all the associated sockets to this network library.
DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP        invalid application ID
  DS_SOCKEXIST     there are existing sockets
 

SIDE EFFECTS
  Puts data services manager into "autodetect" mode.
===========================================================================*/
sint15 dss_bringdown_call
(
  sint15 dss_nethandle,                         /* application ID */
  sint15 *dss_errno                               /* error condition value */
)
{
  sint15 dss_handle = DSS_INVALID_HANDLE;
  uint8 dss_handle_ref_cnt = DSS_INVALID_HANDLE;
  acb_type *acb_ptr = NULL;
  ds_command_type *cmd_ptr = NULL;
  sint15 *dss_hdl_ptr = NULL;

  DS_MSG1 (MSG_LEGACY_HIGH,"dss_bringdown_call for APP ID %u", dss_nethandle);

  if (dss_errno == NULL)
  {
    DS_ERROR_LOG_0("dss_bringdown_call dss_errno is NULL");
    return(DSS_ERROR);
  }

  dss_handle = dss_get_handle(dss_nethandle);

  if (dss_handle == DSS_INVALID_HANDLE)
  {
    DS_ERROR_LOG_1("dss_bringdown_call: Unable to fetch dss handle for "
                   "DSS Net handle %d ", dss_nethandle);
    *dss_errno = DS_EFAULT;
    return(DSS_ERROR);
  }
  

  acb_ptr = dssocki_dss_nethandletoacb(dss_nethandle);
  if ( acb_ptr == NULL)
  {
    DS_ERROR_LOG_0("dss_bringdown_call: ACB pointer is NULL");
    *dss_errno = DS_EBADAPP;
    return DSS_ERROR;
  }

  if(acb_ptr->scb_cnt != 0)
  {
    DS_ERROR_LOG_0("dss_bringdown_call: Please close sockets associated with"
                   " this call before tearing down the call ");
    *dss_errno = DS_SOCKEXIST;
    return DSS_ERROR;
  }

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  dss_handle_ref_cnt = dss_handle_map[dss_handle];
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /*
    This indicates there are multiple DSS clients with call on same APN and
    IP type. Hence return a call bringdown sucess
  */
  if (dss_handle_ref_cnt > DSS_HANDLE_ASSIGNED)
  {
    if (dssocki_check_sock_exists(acb_ptr) == DSS_ERROR)
    {
      DS_MSG0(MSG_LEGACY_ERROR,"Sockets open for ACB,"
              " Application should call dss_close on open sockets for clean up");
    }
		
    DS_MSG2(MSG_LEGACY_HIGH,"dss_bringdown_call : DSS Handle %d ref count %d "
            " Returning call teardown success from DSS",
            dss_handle, dss_handle_ref_cnt);

    dss_decr_handle_ref_cnt(dss_handle);
    dssocki_freeacb(acb_ptr);
    return DSS_SUCCESS;
  }
  else if (dss_handle_ref_cnt == DSS_HANDLE_ASSIGNED)
  {
    /*Post a cmd to DS task and bring down the call in DS task context*/

    cmd_ptr = ds_allocate_cmd_buf(sizeof(sint15));
    if (cmd_ptr == NULL)
    {
      DS_ERROR_LOG_0("dss_bringdown_call: Not able to allocate memory for cmd ptr");
      *dss_errno = DS_ENOMEM;
      return DSS_ERROR;
    }
    else
    {
      cmd_ptr->hdr.cmd_id = DS_CMD_DSS_BRINGDOWN_CALL_EVT;

      dss_hdl_ptr = (sint15*)cmd_ptr->cmd_payload_ptr;
      *dss_hdl_ptr = dss_handle;
      ds_put_cmd(cmd_ptr);
    }
  }

  *dss_errno = DS_EWOULDBLOCK;
  return DSS_SUCCESS;

}


/*===========================================================================
FUNCTION DSS_BRINGDOWN_PDN()

DESCRIPTION

  This API brings down the data call for the DSS handle passed

DEPENDENCIES
  None.

RETURN VALUE
      0  -- if bringup is successful.
     -1 -- if bringup is failed.
      102 -Operation would block.


SIDE EFFECTS
  None
==============================================================================*/
void dss_bringdown_pdn
(
  sint15 *dss_handle
)
{
  int call_status;
  ds_fwk_response_info_s_type resp_info;
  ds_pdn_event_info_u_type event_info;

  memset (&resp_info, 0, sizeof(ds_fwk_response_info_s_type));
  memset (&event_info, 0, sizeof(ds_pdn_event_info_u_type));

  call_status =  ds_fwk_tear_down_pdn(DS_FWK_CLIENT_SOCKETS,
                                      *dss_handle,
                                      &resp_info);

  /*
   This would be a case of call bring down on additional PDN where OTA will 
   be sent from PDN Manager and would return EWOULDBLOCK to DSS.
  */
  if (call_status ==  DS_EWOULDBLOCK)
  {
    DS_MSG1(MSG_LEGACY_HIGH,
            "dss_bringdown_pdn:Triggered bring down call for DSS handle %d "
            "Waiting for NO_NET", *dss_handle);
  }
  /*
    This would be a case of call bring down on ATTACH PDN. No OTA will be sent
    by PDN Manager. But from DSS Client perspective, we need to send a 
    DS_ENETNONET to the DSS Client
  */
  else if (call_status == DSS_SUCCESS)
  {
    DS_MSG1(MSG_LEGACY_HIGH,
            "dss_bringdown_pdn:Call bring down for DSS handle %d. FWK to "
            " NO_NET evt to DSS client", *dss_handle);
  }
  /*
    Case 1: There could be some issue in NAS in handling call bring down in which 
            case PDN Mgr will do internal clean up and post PDN_DOWN_EV. 

    Case 2: In case Network rejects the call teardown, even in that case 
            PDN Mgr will post PDN_DOWN_EV

    Case 3: In case FWK instance is not available (for attach PDN, fwk inst may 
            have been de-allocated when processing Detach from NAS), 
            DSS needs to spoof PDN_DOWN_EV . This way IMS SM would not be stuck 
            in a wrong state.
  */
  else if (call_status == DSS_ERROR)
  {
    DS_ERROR_LOG_3("dss_bringdown_pdn: Call bring down failed, "
                   "DSS handle: %d, Netdown reason type %u,Netdown reason %u",
                   dss_handle, resp_info.netdown_reason_type,
                   resp_info.netdown_reason);

    if (resp_info.netdown_reason != DS_NET_DOWN_REASON_NOT_SPECIFIED)
    {
      event_info.pdn_down_info.netdown_reason = resp_info.netdown_reason;
    }
    else
    {
      event_info.pdn_down_info.netdown_reason = DS_NET_DOWN_REASON_NOT_SPECIFIED;
    }

    dss_pdn_event_cback(*dss_handle,
                        PDN_DOWN_EV,
                        event_info,
                        NULL);
  }

  return;
}


/*===========================================================================
FUNCTION DS_CMD_DSS_PROCESS_FUNC()

DESCRIPTION

  This API is registered with DS task during task init to handle command events
  which will be handled by DSS module

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
==============================================================================*/
void dss_cmd_process_func( ds_command_type *cmd_ptr )
{
  ds_command_enum_type cmd_id;

  if (cmd_ptr == NULL)
  {
    DS_ERROR_LOG_0("dss_cmd_process_func: cmd_ptr is NULL. Pls check");
    return;
  }

  cmd_id = cmd_ptr->hdr.cmd_id;

  DS_MSG1(MSG_LEGACY_HIGH,"dss_cmd_process_func, cmd_id %d",cmd_id);

  switch (cmd_id)
  {
    case DS_CMD_DSS_BRINGUP_CALL_EVT:
       dss_bringup_pdn((dss_control_bringup_type_s*)cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_DSS_BRINGDOWN_CALL_EVT:
       dss_bringdown_pdn((sint15*)cmd_ptr->cmd_payload_ptr);
     break;

    case DS_CMD_DSS_CALL_CALLBACK_EVT:
       dss_process_callback_hdlr((dss_control_callback_info_s*)cmd_ptr->cmd_payload_ptr);
     break;

    default:
      break;
  }
}

/*===========================================================================
FUNCTION DSS_POWERUP_INIT()

DESCRIPTION

  This API does the task init functionality for DSS

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
==============================================================================*/

void dss_init()
{

/*------------------------------------------------------------------------------
  Register a functional handler when DSS command events are handled
  by DS task
----------------------------------------------------------------------------*/
 if ( !(ds_register_cmd_hdlr(DS_MODULE_DSS,dss_cmd_process_func)))
 {
   DS_ERROR_LOG_0("Not able to register DSS function handle with DS task"
                  "ABORT DSS Task INIT ");
   return;
 }

/*------------------------------------------------------------------------------
  Initialize the Queues
-----------------------------------------------------------------------------*/
  (void) q_init(&sock_config_cb.tcp_cb_q);
  (void) q_init(&sock_config_cb.udp_cb_q);
  (void) q_init(&sock_config_cb.icmp_cb_q);
  (void) q_init(&sock_config_cb.rsvp_cb_q);
  (void) q_init(&sock_config_cb.app_cb_q);
  (void) q_init(&sock_config_cb.sock_cb_q);

/*-------------------------------------------------------------------------
  Register the DS_FWK callback function
-------------------------------------------------------------------------*/
  ds_fwk_reg_events(DS_FWK_CLIENT_SOCKETS,&dss_pdn_event_cback,NULL);

  /*-------------------------------------------------------------------------
    Initialize DNS subsystem during powerup
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATAMODEM_DNS
  dss_dns_init();
#endif

  DS_INIT_CRIT_SECTION(&dss_ping_crit_section);

/*-------------------------------------------------------------------------
  Initialize ping engine.
------------------------------------------------------------------------*/
#ifdef FEATURE_DATAMODEM_DSS_PING
  dss_ping_comm_mgr_init_ping_engine();
#endif /* FEATURE_DATAMODEM_DSS_PING */


  DS_MSG0(MSG_LEGACY_HIGH,"DSS Task Init Successful");

  return;
}


/*===========================================================================
FUNCTION DSS_PROCESS_CALLBACK_HDLR()

DESCRIPTION

  This API Handles the functionality for the event posted in the DSS callback
  registered with DS_FWK at DSS task init.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
==============================================================================*/
void dss_process_callback_hdlr
(
  dss_control_callback_info_s *callback_info
)
{
  if(callback_info == NULL)
  {
    DS_ERROR_LOG_0("dss_process_callback_hdlr : Callback info pointer is NULL");
    return;
  }

  DS_MSG1(MSG_LEGACY_HIGH,"dss_process_callback_hdlr, event %d",
          callback_info->event);

  switch(callback_info->event)
  {
    case PDN_UP_EV:
        dss_post_client_net_evt(callback_info->client_handle, DS_ENETISCONN,
                                callback_info->event_info);
      break;

    case PDN_DOWN_EV:
        dss_post_client_net_evt(callback_info->client_handle, DS_ENETNONET,
                                callback_info->event_info);
     break;

    case BEARER_FLOW_ENABLED_EV:
    case BEARER_UP_EV:
       dssocki_update_write_ev();
      break;
	case BEARER_FLOW_DISABLED_EV:
    case PDN_ADDR_CHANGED_EV:
    case PDN_PREFIX_UPDATE_EV:
    case PDN_VALID_RA_EV:
    case PDN_FLOW_ADDED_EV:
    case PDN_FLOW_DELETED_EV:
    case PDN_IPV6_PRIV_ADDR_GENERATED_EV:
    case PDN_IPV6_PRIV_ADDR_DEPRECATED_EV:
    case PDN_IPV6_PRIV_ADDR_EXPIRED_EV:
    case PDN_IPV6_PRIV_ADDR_DELETED_EV:
    case PDN_IPV6_EXTERNAL_ADDR_DELETED_EV:
    case PDN_EXTENDED_IP_CONFIG_EX_EV:
    case PDN_NET_INITIATED_QOS_AVAILABLE_EV:
    case PDN_NET_AVAILABLE_QOS_MODIFIED_EV:
    case PDN_NET_INITIATED_QOS_UNAVAILABLE_EV:
    case PDN_WWAN_TECH_SUSPENSION_EV:
      dssocki_process_pdn_ev_cback(callback_info->event,
                                   callback_info->event_info);
    break;

    default:
       DS_ERROR_LOG_1("dss_process_callback_hdlr : Invalid event %d",
                      callback_info->event);
     break;
    }
  	
}

/*===========================================================================
FUNCTION DSS_PROCESS_CALLBACK_HDLR()

DESCRIPTION

  This API Stores the DS FWK instance pointer for the DSS handle in the ACB

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None
==============================================================================*/
void dss_store_fw_inst_in_acb
(
  acb_type  *acb_ptr,
  uint8     dss_handle
)
{
  ds_fwk_clnt_info_s_type               clnt_info = {0};

  ds_fwk_get_clnt_info_from_handle(DS_FWK_CLIENT_SOCKETS, dss_handle,&clnt_info);

  if (clnt_info.fw_inst == NULL)
  {
    DS_ERROR_LOG_0("Not able to fetch DS FWK Inst from DSS Handle");
    return;
  }
  else
  {
    acb_ptr->fw_inst = clnt_info.fw_inst;
  }
}


/*===========================================================================
FUNCTION      DSS_PROCESS_PDN_UP_EV_CBACK()

DESCRIPTION
  Called when the PDN comes up. This calls the
  net_callback_fcn() for the specified application.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dss_process_pdn_up_ev_cback
(
  uint8 dss_handle
)
{
  acb_type *curr_acb_ptr=NULL;
  dss_net_callback_info_s *net_cb_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Loop through all the apps and notify apps that have the same iface_ptr
    as the one on which the UP event occurred.
  -------------------------------------------------------------------------*/

  curr_acb_ptr = (acb_type *) q_check( &sock_config_cb.app_cb_q );
  while(curr_acb_ptr != NULL)
  {
    DS_ASSERT(APPAVAIL != curr_acb_ptr->dss_nethandle);

    if(curr_acb_ptr->dss_handle == dss_handle)
    {
      /*---------------------------------------------------------------------
        Notify app
      ---------------------------------------------------------------------*/
      if(curr_acb_ptr->net_cb != NULL)
      {
        DS_MSG1(MSG_LEGACY_HIGH,
                "Calling net cback for app %d", curr_acb_ptr->dss_nethandle);
        /*------------------------------------------------------------------
          Network Callback's should be called under TASKLOCK's. In the
          code without L4 feature these TASKLOCK's are taken care by
          DS_ENTER_CRIT_SECTION above. Using ifdef here so that with L4
          feature callback is under TASKLOCK.
        ------------------------------------------------------------------*/
        DS_SYSTEM_HEAP_MEM_ALLOC(net_cb_info,
                                 sizeof(dss_net_callback_info_s),
                                 dss_net_callback_info_s*);

        if(net_cb_info == NULL)
        {
          DS_ERROR_LOG_1("Unable to allocate memory for Invoking Net Callback"
                         " For Net Handle %d",curr_acb_ptr->dss_nethandle);
          DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
          DS_ASSERT(0);
          return;
        }

        memset(net_cb_info, 0, sizeof(dss_net_callback_info_s));
        net_cb_info->dss_nethandle = curr_acb_ptr->dss_nethandle;
        net_cb_info->dss_errno = DS_ENETISCONN;
        net_cb_info->net_cb_user_data = curr_acb_ptr->net_cb_user_data;

        curr_acb_ptr->net_cb(net_cb_info);

        DS_SYSTEM_HEAP_MEM_FREE(net_cb_info);

      }
      else
      {
        DS_ERROR_LOG_1("NOT calling net cback for app %d",
                       curr_acb_ptr->dss_nethandle);
      }

      dssocki_socket_event_occurred(NULL, curr_acb_ptr);
    }

    curr_acb_ptr
      = (acb_type *) q_next(&sock_config_cb.app_cb_q, &curr_acb_ptr->link);
  } /* while */
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* dss_process_pdn_up_ev_cback() */

/*===========================================================================
FUNCTION      DSS_RESET_SOCK_ROUTING_CACHE()

DESCRIPTION
  Loop through all the sockets and set the routing cache of all the sockets
  that are using the interface that has gone down to NULL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void dss_reset_sock_routing_cache(acb_type  *acb_ptr )
{
  scb_type               * curr_scb_ptr = NULL;
  scb_type               * next_scb_ptr = NULL;
  dssocki_app_to_ps_cmd_type     * ps_cmd;

  if (acb_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"dss_reset_sock_routing_cache,"
            "acb_ptr is NULL");
    return;
  }

  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  curr_scb_ptr = (scb_type *) q_check( &sock_config_cb.sock_cb_q );
  while ( curr_scb_ptr != NULL )
  {
    DS_ASSERT(SOCKAVAIL != curr_scb_ptr->sockfd);
    next_scb_ptr
      = (scb_type *) q_next(&sock_config_cb.sock_cb_q, &curr_scb_ptr->link);

    if (curr_scb_ptr->acb_ptr == acb_ptr)
    {
      switch (curr_scb_ptr->protocol)
      {
        case PS_IPPROTO_TCP:
          if( (curr_scb_ptr->protocol_ctl_blk.tcb) &&
               ( (curr_scb_ptr->protocol_ctl_blk.tcb->ipcb.routing_cache ==
                  curr_scb_ptr->acb_ptr->fw_inst)
               )
            )
          {
            curr_scb_ptr->protocol_ctl_blk.tcb->ipcb.routing_cache = NULL;
            curr_scb_ptr->cached_info.ip.routing_cache = NULL;

            IP_INFO_CHANGE_UPCALL(
              &curr_scb_ptr->protocol_ctl_blk.tcb->ipcb,
              curr_scb_ptr->sockfd,
              curr_scb_ptr,
              curr_scb_ptr->protocol_ctl_blk.tcb->info_change_upcall);


            if(curr_scb_ptr->protocol_ctl_blk.tcb->state != TCP_CLOSED)
            {
              ps_cmd = dssocki_alloc_ps_cmd_buffer(APP_TO_PS_SOCKET_CLOSE);
              if(NULL == ps_cmd)
              {
                DS_ERROR_LOG_0( "Command buffers could not be allocated");
                return;
              }
              dssocki_send_ps_cmd_buffer(curr_scb_ptr, (void *)TRUE, NULL, ps_cmd);
            }
            dssocki_set_sock_state(curr_scb_ptr, DSSOCKI_CLOSED);
          }
          break;

          case PS_IPPROTO_UDP:
            if(curr_scb_ptr->protocol_ctl_blk.ucb &&
               curr_scb_ptr->protocol_ctl_blk.ucb->ipcb.routing_cache ==
                 curr_scb_ptr->acb_ptr->fw_inst)
            {
              curr_scb_ptr->protocol_ctl_blk.ucb->ipcb.routing_cache =  NULL;
              curr_scb_ptr->cached_info.ip.routing_cache = NULL;

              IP_INFO_CHANGE_UPCALL(
                &curr_scb_ptr->protocol_ctl_blk.ucb->ipcb,
                curr_scb_ptr->sockfd,
                curr_scb_ptr,
                curr_scb_ptr->protocol_ctl_blk.ucb->info_change_upcall);


            }
            break;

          case PS_IPPROTO_ICMP:
          case PS_IPPROTO_ICMP6: /* ICMP and ICMPv6 handled in the same fashion */
            if(curr_scb_ptr->protocol_ctl_blk.icb &&
               curr_scb_ptr->protocol_ctl_blk.icb->ipcb.routing_cache ==
               curr_scb_ptr->acb_ptr->fw_inst)
            {
              curr_scb_ptr->protocol_ctl_blk.icb->ipcb.routing_cache =  NULL;
              curr_scb_ptr->cached_info.ip.routing_cache = NULL;

              IP_INFO_CHANGE_UPCALL(
                &curr_scb_ptr->protocol_ctl_blk.icb->ipcb,
                curr_scb_ptr->sockfd,
                curr_scb_ptr,
                curr_scb_ptr->protocol_ctl_blk.icb->info_change_upcall);

            }
            break;

          default:
            DS_MSG1(MSG_LEGACY_ERROR,"Unknown protocol type %d",
                    curr_scb_ptr->protocol);
            break;
        }
      }
    curr_scb_ptr = next_scb_ptr;
  } /* while */

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
}

/*===========================================================================
FUNCTION      DSS_PROCESS_PDN_DOWN_EV_CBACK()

DESCRIPTION
  Called when the PDN goes down. This calls the
  net_callback_fcn() for the specified application.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dss_process_pdn_down_ev_cback
(
  uint8 dss_handle,
  ds_net_down_event_info_type  pdn_down_info
)
{
  acb_type               * curr_acb_ptr = NULL;
  acb_type               * acb_ptr_to_del = NULL;
  dss_net_callback_info_s *net_cb_info = NULL;


  /*-------------------------------------------------------------------------
    Loop through all the apps and notify apps that have the same iface_ptr
    as the one on which the DOWN event occurred.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  curr_acb_ptr = (acb_type *) q_check( &sock_config_cb.app_cb_q );
  while( curr_acb_ptr != NULL)
  {
    DS_ASSERT(APPAVAIL != curr_acb_ptr->dss_nethandle);

    if (curr_acb_ptr->dss_handle == dss_handle)
    {
       /*Leave critical section as the critical section is locked 
        inside dss_decr_handle_ref_cnt */

        DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        dss_reset_sock_routing_cache(curr_acb_ptr);
        dss_decr_handle_ref_cnt(dss_handle);
        if (dssocki_check_sock_exists(curr_acb_ptr) == DSS_ERROR)
        {
          DS_MSG0(MSG_LEGACY_ERROR,"Sockets open for ACB,"
            " Application should call dss_close on open sockets for clean up");
        }
        DS_ENTER_CRIT_SECTION(&global_ps_crit_section);

      /*---------------------------------------------------------------------
        Notify app
      ---------------------------------------------------------------------*/
      if(curr_acb_ptr->net_cb != NULL)
      {
        DS_MSG1(MSG_LEGACY_HIGH,
                "Calling net cback for app %d",curr_acb_ptr->dss_nethandle);

        /*-------------------------------------------------------------------
          Network Callback's should be called under TASKLOCK's. In the
          code without L4 feature these TASKLOCK's are taken care by
          DS_ENTER_CRIT_SECTION above. Using ifdef here so that with L4
          feature callback is under TASKLOCK.
        -------------------------------------------------------------------*/
        DS_SYSTEM_HEAP_MEM_ALLOC(net_cb_info,
                                 sizeof(dss_net_callback_info_s),
                                 dss_net_callback_info_s*);

        if(net_cb_info == NULL)
        {
          DS_MSG1(MSG_LEGACY_ERROR,"Unable to allocate memory for Invoking"
                  "  Net CallbackFor Net Handle %d",curr_acb_ptr->dss_nethandle);
          DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
          DS_ASSERT(0);
          return;
        }

        memset(net_cb_info, 0, sizeof(dss_net_callback_info_s));
        net_cb_info->dss_nethandle = curr_acb_ptr->dss_nethandle;
        net_cb_info->dss_errno = DS_ENETNONET;
        net_cb_info->net_cb_user_data = curr_acb_ptr->net_cb_user_data;
        net_cb_info->pdn_down_info = pdn_down_info;

        curr_acb_ptr->net_cb(net_cb_info);

        DS_SYSTEM_HEAP_MEM_FREE(net_cb_info);
      }
      else
      {
        DS_MSG1(MSG_LEGACY_ERROR,"NOT calling net cback for app %d",
                       curr_acb_ptr->dss_nethandle);
      }

      dssocki_socket_event_occurred(NULL, curr_acb_ptr);

      curr_acb_ptr->fw_inst = NULL;

      //Store the Current ACB pointer which is needed to be deleted
      acb_ptr_to_del = curr_acb_ptr;
    }

    curr_acb_ptr =
      (acb_type *) q_next(&sock_config_cb.app_cb_q, &curr_acb_ptr->link);

    //Free the ACB
    if (acb_ptr_to_del != NULL)
    {
      DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      dssocki_freeacb(acb_ptr_to_del);
      DS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      acb_ptr_to_del = NULL;
    }

  }  /* while */

  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* dss_process_iface_down_ev_cback() */


#ifdef FEATURE_DATA_PS_QOS

/*===========================================================================
FUNCTION      DSS_QOS_IOCTL_HANDLERS()

DESCRIPTION
  This API handles the QOS IOCLT's posted from DS clients

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean dss_qos_ioctl_handler
(
  sint15                   dss_nethandle,
  ds_pdn_ioctl_type        ioctl_name,
  void                    *argval_ptr,
  sint15                  *ps_errno
)
{
  acb_type *acb_ptr = NULL;
  ds_pdn_ioctl_qos_net_initiated_request_type *net_initiated_req_ptr = NULL;
  ds_pdn_ioctl_qos_net_initiated_release_type *net_initiated_rel_ptr = NULL;
  ds_pdn_ioctl_qos_get_granted_flow_spec_type *granted_flow_spec_ptr = NULL;
  ds_fwk_qos_net_initiated_request_type       net_initiated_req;
  ds_fwk_qos_net_initiated_check_type         net_initiated_check;
  ds_fwk_qos_net_initiated_release_type       net_initiated_rel;
  ds_fwk_qos_get_granted_flow_spec_type       granted_flow_spec;
  list_type *list_ptr = NULL;
  ds_fwk_qos_matching_flow_type *item_ptr = NULL;
  ds_pdn_event_info_u_type event_info;

  if (argval_ptr == NULL)
  {
    *ps_errno = DS_EFAULT;
    return FALSE;
  }

  acb_ptr = dssocki_dss_nethandletoacb(dss_nethandle);
  if ( acb_ptr == NULL)
  {
    DS_MSG0(MSG_LEGACY_ERROR, "dss_qos_ioctl_handler: ACB pointer is NULL");
    *ps_errno = DS_EBADAPP;
    return FALSE;
  }

  memset(&net_initiated_req, 0, sizeof(ds_fwk_qos_net_initiated_request_type));
  memset(&net_initiated_check, 0, sizeof(ds_fwk_qos_net_initiated_check_type));
  memset(&net_initiated_rel, 0, sizeof(ds_fwk_qos_net_initiated_release_type));
  memset(&granted_flow_spec, 0, sizeof(ds_fwk_qos_get_granted_flow_spec_type));
  memset(&event_info, 0, sizeof(ds_pdn_event_info_u_type));

  DS_MSG3(MSG_LEGACY_HIGH, "DSS Nethandle %d, DSS Handle %d, Ioctl %d",
          acb_ptr->dss_nethandle, acb_ptr->dss_handle, ioctl_name);
  switch (ioctl_name)
  {
    case DS_PDN_IOCTL_QOS_NET_INITIATED_REQUEST:

      net_initiated_req_ptr =(ds_pdn_ioctl_qos_net_initiated_request_type*)\
                                                                    argval_ptr;

     /*
       Initiate a new NIQ request
     */
      net_initiated_req.net_initiated_qos_spec = \
                               net_initiated_req_ptr->net_initiated_qos_spec;

      if (DSS_ERROR == ds_fwk_create_qos_net_initiated_request(acb_ptr->fw_inst,
                                                               &net_initiated_req,
                                                               ps_errno))
      {
        DS_MSG1(MSG_LEGACY_ERROR, "Cannot create Net Initiated QOS request"
                "ps_errno %d", *ps_errno);
        return FALSE;
      }

      /*
         Assign the handle which will be used by clients for future
         interaction (This is not QOS Handle)
      */
      net_initiated_req_ptr->req_handle = net_initiated_req.req_handle;

      /*
         Check if the new NIQ request is already present. If it is present
         raise a PDN_NET_INITIATED_QOS_AVAILABLE_EV from DSS
      */
      list_init(&net_initiated_check.matching_flow_list);
      net_initiated_check.net_initiated_qos_spec = \
                               net_initiated_req_ptr->net_initiated_qos_spec;
							   
      net_initiated_check.req_handle = net_initiated_req_ptr->req_handle;

      if (DSS_ERROR == ds_fwk_qos_net_initiated_check(DS_FWK_CLIENT_SOCKETS,
                                                      acb_ptr->dss_handle,
                                                      &net_initiated_check,
                                                      ps_errno))
      {
        /*
          DS_EWOULDBLOCK represents that no flows matching to the NIQ
          were found.
        */
        if (list_is_initialized(&net_initiated_check.matching_flow_list))
        {
          list_destroy(&net_initiated_check.matching_flow_list);
        }

        if (*ps_errno == DS_EWOULDBLOCK)
        {
          return TRUE;
        }
        else
        {
        DS_MSG1(MSG_LEGACY_ERROR, "Error in checking the NIQ requested is"
                "present ps_errno %d", *ps_errno);
        return FALSE;
        }
      }
      else
      {
        list_ptr= &net_initiated_check.matching_flow_list;
         /*
          Iterate through list which would indicate the number of
          matching flows for the NIQ. Internally Post 
          PDN_NET_INITIATED_QOS_AVAILABLE_EV event to DS task to indicate the
          qos_handle(flow_ptr) to the clients
        */

        if (list_is_initialized(list_ptr))
        {
          item_ptr = (ds_fwk_qos_matching_flow_type*)list_peek_front(list_ptr);
           while(item_ptr != NULL)
          {
            event_info.net_initiated_qos_info.req_handle = \
                                                net_initiated_req_ptr->req_handle;
            event_info.net_initiated_qos_info.qos_handle = \
                                        (ds_qos_handle_type)item_ptr->flow_ptr;
			
             dss_pdn_event_cback(acb_ptr->dss_handle,
                                PDN_NET_INITIATED_QOS_AVAILABLE_EV,
                                event_info,
                                NULL);
            item_ptr = list_peek_next(list_ptr, &(item_ptr->link));
          }
          list_destroy(&net_initiated_check.matching_flow_list);
        }
      }

      break;
    case DS_PDN_IOCTL_QOS_NET_INITIATED_RELEASE:
      net_initiated_rel_ptr = (ds_pdn_ioctl_qos_net_initiated_release_type*)\
                                                                  argval_ptr;

      net_initiated_rel.req_handle = net_initiated_rel_ptr->req_handle;

      if (DSS_ERROR == ds_fwk_release_qos_net_initiated(
                                              acb_ptr->fw_inst,
                                              &net_initiated_rel,
                                              ps_errno))
      {
        DS_MSG2(MSG_LEGACY_ERROR, "Error in Releasing the NIQ request "
                "handle %d ps_errno %d", 
                net_initiated_rel_ptr->req_handle,*ps_errno);
        return FALSE;
      }
      break;

    case DS_PDN_IOCTL_QOS_GET_GRANTED_FLOW_SPEC:
      granted_flow_spec_ptr = (ds_pdn_ioctl_qos_get_granted_flow_spec_type*)\
                                                                 argval_ptr;

      granted_flow_spec.qos_handle = granted_flow_spec_ptr->qos_handle;

      if (DSS_ERROR == ds_fwk_qos_get_granted_flow_spec(&granted_flow_spec))
      {
        DS_MSG2(MSG_LEGACY_ERROR, "Error in Getting granted flow spec "
                "QOS handle %d ps_errno %d",
                granted_flow_spec.qos_handle,*ps_errno);
        return FALSE;
      }
      else
      {
        granted_flow_spec_ptr->rx_flow = granted_flow_spec.rx_flow;
        granted_flow_spec_ptr->tx_flow = granted_flow_spec.tx_flow;
        granted_flow_spec_ptr->qos_status = granted_flow_spec.qos_status;
        granted_flow_spec_ptr->bearer_id = granted_flow_spec.bearer_id;
        
      }

      break;

    default:
      return FALSE;
      break;
  }

  return TRUE;
}
#endif
