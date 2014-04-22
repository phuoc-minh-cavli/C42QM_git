/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     MBMS SERVICE MANAGER MAIN MODULE (MSMMAIN.C)

GENERAL DESCRIPTION

This module contains the implementation of the MBMS Service Manager module 

EXTERNALIZED FUNCTIONS
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001,2002,2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mbmsm/src/msmmain.c#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/17/06   vr      Initial version
03/18/08   rk      Added Support to merge MBMS SM in CB task

===========================================================================*/


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#include "mmcp_variation.h"
#include "sys.h"
#include "sys_v.h"
#include "sys_gw_v.h"
#include "cmwll.h"


#ifdef FEATURE_MBMS
#include "rrccmd.h"
#include "smtask.h"
#include "smtask_v.h"
#include "msmmain.h"

#define MSM_INVALID_LOG_CHAN_ID 0xFFFFFFFF
#define MSM_INVALID_INDEX       0xFFFFFFFF
#define MSM_MAX_BEARER_RATE     0xFE
#define MSM_INVALID_PLMN_ID     0xFFFFFFFF

// Global structure to hold MBMS service information
msm_mbms_services_s_type      msm_mbms_services;

// Global structure to hold the current MBMS capability
msm_mbms_capabililty_s_type   msm_mbms_capability;

// Var to hold PLMN Id sent by MM in the latest MM Cap indication
uint32 curr_plmn_id;

/*===========================================================================

FUNCTION MSMMAIN_INIT

DESCRIPTION

  This function initialized service db

DEPENDENCIES

  None.
  
RETURN VALUE

  None

SIDE EFFECTS

  None.
  
===========================================================================*/
void msmmain_init
(
  void
)
{
  uint32 i;

  //Initialize Service DB
  msm_mbms_services.num_services = 0;

  for(i = 0; i < MSM_MAX_MBMS_SERVICES; i++)
  {
    msm_mbms_services.services[i].service_state = MSM_MBMS_SERVICE_INACTIVE;
  }

  //Initialise mbms capability
  msm_mbms_capability.core_nw_capability = TRUE;
  msm_mbms_capability.cell_capability = TRUE;

  //Initialise PLMN id 
  curr_plmn_id = MSM_INVALID_PLMN_ID;
}

/*===========================================================================

FUNCTION MSMMAIN_CONVERT_PLMN_ID_TO_UINT32

DESCRIPTION
  This function converts the plmn id of array type  type to uint32
  
DEPENDENCIES
  None

RETURN VALUE
  uitn32 value
  
SIDE EFFECTS
  None

===========================================================================*/
uint32 msmmain_convert_plmn_id_to_uint32
(
  byte * plmn_id
)
{
  /* Local index */
  uint32 idx;                        

  /* PLMN ID return value */
  uint32 value = 0;

  for(idx = 0; idx < 3; idx++)
  {
    value = value << 8; 
    value = plmn_id[idx] | value;
  }

  return value;
}

/*===========================================================================

FUNCTION MSMMAIN_RETURN_DB_IDX_FOR_TMGI

DESCRIPTION

  This function returns a DB index for the given TMGI

DEPENDENCIES

  None.
  
RETURN VALUE

  DB index for the service. INVALID INDEX if srv is not found

SIDE EFFECTS

  None.
  
===========================================================================*/
uint32 msmmain_return_db_idx_for_tmgi
(
  sys_mbms_tmgi_uint_old_s_type srv_tmgi
)
{
  uint32 i;

  for(i = 0; i < msm_mbms_services.num_services ; i++)
  {
    if(msm_mbms_services.services[i].tmgi.service_id == srv_tmgi.service_id)
    {
      /* Check if the PLMN id also matches */
      if(FALSE == srv_tmgi.plmn_id_present)
      {
        MSG_HIGH("MBMS: Service ID %d already exists in DB at location %d",srv_tmgi.service_id,i,0);
        return i;
      }
      else if(msm_mbms_services.services[i].tmgi.plmn_id == srv_tmgi.plmn_id)
      {
        MSG_HIGH("MBMS: Service ID %d & PLMN id %d already exists in DB at location %d",
                 srv_tmgi.service_id,srv_tmgi.plmn_id,0);
        return i;
      }
    }
  }

  return MSM_INVALID_INDEX;
}


/*===========================================================================

FUNCTION MSM_CREATE_NEW_SERVICE_ENTRY

DESCRIPTION

  This function creates new service entry

DEPENDENCIES

  None.
  
RETURN VALUE

  Index

SIDE EFFECTS

  None.
  
===========================================================================*/
uint32 msm_create_new_service_entry
(
  sys_gw_mbms_activate_req_entry_s_type * mbms_act_req
)
{
  uint32 srv_index = MSM_INVALID_INDEX;
  sys_mbms_tmgi_uint_old_s_type local_tmgi;

  if(FALSE == mbms_act_req->tmgi.present)
  {
    MSG_ERROR("TMGI absent in MBMS Act Req",0,0,0);
    return MSM_INVALID_INDEX;
  }

  /* Check if PLMN Id is part of TMGI from SDP. If not append it */
  if(mbms_act_req->tmgi.tmgi > 0xFFFFFF)
  {
    MSG_HIGH("PLMN Id already part of TMGI in MBMS Act Req",0,0,0);
    local_tmgi.service_id = (uint32)(mbms_act_req->tmgi.tmgi >> 24);
    local_tmgi.plmn_id_present = TRUE;
    local_tmgi.plmn_id = (uint32)(mbms_act_req->tmgi.tmgi & 0xFFFFFF);
  }
  else
  {
    MSG_HIGH("PLMN Id not present in TMGI in MBMS Act Req",0,0,0);
    local_tmgi.service_id = (uint32) mbms_act_req->tmgi.tmgi;

    if(curr_plmn_id != MSM_INVALID_PLMN_ID)
    {
      MSG_HIGH("Using stored curr PLMN Id %d",curr_plmn_id,0,0);

      /* Use the stored PLMN id */
      local_tmgi.plmn_id_present = TRUE;
      local_tmgi.plmn_id = curr_plmn_id;
    }
    else
    {
      local_tmgi.plmn_id_present = FALSE;
      MSG_ERROR("Stored PLMN Id is invalid, not populating PLMN Id field in TMGI",0,0,0);
    }
  }

  /* Check if this service is already present in the db */
  if(MSM_INVALID_INDEX == msmmain_return_db_idx_for_tmgi(local_tmgi))
  {
    /* Check if DB is full */
    if(msm_mbms_services.num_services >= MSM_MAX_MBMS_SERVICES)
    {
      MSG_ERROR("MBMS: Cannot create new service since DB is full. Num srvs %d, Max srvs %d",
                 msm_mbms_services.num_services, MSM_MAX_MBMS_SERVICES,0);
    }
    else
    {
      /* Store the TMGI that was passaed in the command. This is the TMGI that should be used 
         in all Req Rsp commands with higher layers */
      msm_mbms_services.services[msm_mbms_services.num_services].tmgi_sdp = mbms_act_req->tmgi.tmgi;

      /* Store the TMGI in local format */
      msm_mbms_services.services[msm_mbms_services.num_services].tmgi = local_tmgi;

      /* Store MIP and port number */
      msm_mbms_services.services[msm_mbms_services.num_services].mip = mbms_act_req->mip;
      msm_mbms_services.services[msm_mbms_services.num_services].port = (uint16) mbms_act_req->port;

      /* Store service priority if present */
      if(mbms_act_req->priority.present)
      {
        msm_mbms_services.services[msm_mbms_services.num_services].priority = (uint8) mbms_act_req->priority.priority;          
      }

      /* Initialise LC id for this service */
      msm_mbms_services.services[msm_mbms_services.num_services].log_chan_id = MSM_INVALID_LOG_CHAN_ID;
  
      srv_index = msm_mbms_services.num_services;

      MSG_HIGH("MBMS: Added service 0x%x to MSM MBMS Services DB, Num services %d",
               msm_mbms_services.services[msm_mbms_services.num_services].tmgi.service_id,msm_mbms_services.num_services,0);

      /* Incremenet num services */
      msm_mbms_services.num_services++;
    }
  }

  return srv_index;
}

/*===========================================================================

FUNCTION MSM_DELETE_SERVICE_ENTRY

DESCRIPTION

  This function creates deletes service entry

DEPENDENCIES

  None.
  
RETURN VALUE

  Index

SIDE EFFECTS

  None.
  
===========================================================================*/
void msm_delete_service_entry
(
  uint32 srv_index
)
{
  
  msm_mbms_services.services[srv_index].service_state = MSM_MBMS_SERVICE_INACTIVE;
  msm_mbms_services.services[srv_index].log_chan_id = MSM_INVALID_LOG_CHAN_ID;

  if(msm_mbms_services.num_services > 0)
  {
    msm_mbms_services.num_services--;
  }

  MSG_HIGH("MBMS: Deleted service %d from MSM MBMS Services DB, Num services %d ",
             msm_mbms_services.services[srv_index].tmgi.service_id,
             msm_mbms_services.num_services,0);

  /* Compact holes */
  if(srv_index < msm_mbms_services.num_services)
  {
    memsmove(&msm_mbms_services.services[srv_index], (sizeof(msm_mbms_service_entry_s_type)*(MSM_MAX_MBMS_SERVICES-srv_index)),
           &msm_mbms_services.services[srv_index+1],
           ((sizeof(msm_mbms_service_entry_s_type)) * (msm_mbms_services.num_services - srv_index)));
  }
}

/*===========================================================================

FUNCTION MSM_RETURN_INDEX_FOR_SERVICE

DESCRIPTION

  This function returns Index for a service

DEPENDENCIES

  None.
  
RETURN VALUE

  Index

SIDE EFFECTS

  None.
  
===========================================================================*/
uint32 msm_return_index_for_service
(
  uint32 service_id
)
{
  uint32 i;

  for(i = 0; i < msm_mbms_services.num_services; i++)
  {
    if(service_id == msm_mbms_services.services[i].tmgi.service_id)
    {
      return i;
    }
  }

  return MSM_INVALID_INDEX;
}


/*===========================================================================

FUNCTION MSM_SEND_MBMS_CAPABILITY_IND

DESCRIPTION

  This function sends the latest MBMS capability information to MBMS Apps

DEPENDENCIES

  None.
  
RETURN VALUE

  None

SIDE EFFECTS

  None.
  
===========================================================================*/
void msm_send_mbms_capability_ind
(
  void
)
{
  /* CM rpt ptr*/
  cm_rpt_type *cm_cmd_ptr = NULL;

  if((cm_cmd_ptr = cm_get_cmd_buf()) == NULL)
  {
    ERR_FATAL("MBMS:Failed to get cm rpt buffer", 0, 0, 0 );
  }   
  else
  {
    /* Set hdr to indicate this is a generic MBMS rpt*/
    cm_cmd_ptr->hdr.cmd = CM_RPT_MBMS_GENERIC;

    /* Set rpt type to capability ind */
    cm_cmd_ptr->cmd.mbms_generic_rpt.rpt_type = SYS_GW_MBMS_RESP_CAPABILITY_IND;

    /* Populate the latest core nw and cell capabilities in the command */
    cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.capability_ind.network_capability = 
      msm_mbms_capability.core_nw_capability;

    cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.capability_ind.cell_capability = 
      msm_mbms_capability.cell_capability;
    
    MSG_HIGH("MBMS: Sending SYS_GW_MBMS_RESP_CAPABILITY_IND. core_nw_capab %d, cell_capab %d",
             msm_mbms_capability.core_nw_capability,msm_mbms_capability.cell_capability,0);
  
    /* Put CM command */
    cm_put_cmd(cm_cmd_ptr);          
  }
}

/*===========================================================================

FUNCTION MSM_HANDLE_MBMS_CM_GENERIC_CMD

DESCRIPTION

  This function handles MBMS_CM_GENERIC_CMD which comes from MBMS Apps (via CM)

DEPENDENCIES

  None.
  
RETURN VALUE

  None

SIDE EFFECTS

  None.
  
===========================================================================*/
void msm_handle_mbms_cm_generic_cmd
(
  msm_ext_cmd_type *msm_cmd_ptr
)
{
  uint32 i = 0;
  uint32 srv_index;

  switch(msm_cmd_ptr->cmd.cm_mbms_generic_cmd.cmd_type)
  {
    /* MBMS Service Activate command */
    case SYS_GW_MBMS_REQ_ACTIVATE:

      MSG_HIGH("Recvd SYS_GW_MBMS_REQ_ACTIVATE for %d services",
               msm_cmd_ptr->cmd.cm_mbms_generic_cmd.cmd.activate_req.num_services,0,0);

      for(i = 0; i < msm_cmd_ptr->cmd.cm_mbms_generic_cmd.cmd.activate_req.num_services; i++)
      {
        rrc_cmd_type * rrc_cmd_ptr = NULL;
        
        MSG_HIGH("MBMS: Recvd MBMS ACTIVATED REQ for TMGI 0x%x MIP 0x%x Port %d",
                 msm_cmd_ptr->cmd.cm_mbms_generic_cmd.cmd.activate_req.services[i].tmgi.tmgi,
                 msm_cmd_ptr->cmd.cm_mbms_generic_cmd.cmd.activate_req.services[i].mip.addr.v4, 
                 msm_cmd_ptr->cmd.cm_mbms_generic_cmd.cmd.activate_req.services[i].port);

        if(MSM_INVALID_INDEX != 
            (srv_index = msm_create_new_service_entry(&msm_cmd_ptr->cmd.cm_mbms_generic_cmd.cmd.activate_req.services[i])))
        {
          /* Send a command to RRC to activate service */
          if(NULL == (rrc_cmd_ptr = rrc_get_msm_cmd_buf()))
          {
            ERR_FATAL("MBMS: No memory for RRC cmd",0,0,0);
          }
          else
          {
            /* Send Activate request to RRC */
            rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_MBMS_SERVICE_ACTIVATED_REQ;
            rrc_cmd_ptr->cmd.mbms_service_activated_req.service_id = msm_mbms_services.services[srv_index].tmgi.service_id;

            /* TMGI that is passed to RRC will the XORed value of service id and plmn id */
            if(msm_mbms_services.services[srv_index].tmgi.plmn_id_present)
            {
              rrc_cmd_ptr->cmd.mbms_service_activated_req.tmgi = 
                msm_mbms_services.services[srv_index].tmgi.service_id ^ msm_mbms_services.services[srv_index].tmgi.plmn_id;
            }
            else
            { 
               rrc_cmd_ptr->cmd.mbms_service_activated_req.tmgi = msm_mbms_services.services[srv_index].tmgi.service_id;
            }

            /* For now populate the service type as Broadcast */
            rrc_cmd_ptr->cmd.mbms_service_activated_req.service_type = MBMS_BROADCAST;

            MSG_HIGH("MBMS: Sent RRC_MBMS_SERVICE_ACTIVATED_REQ for Service id 0x%x TMGI %d",
                     rrc_cmd_ptr->cmd.mbms_service_activated_req.service_id,
                     rrc_cmd_ptr->cmd.mbms_service_activated_req.tmgi,0);

            /* Set service state apprpriately */
            msm_mbms_services.services[srv_index].service_state = MSM_MBMS_SERVICE_PENDING_ACT;

            rrc_put_msm_cmd(rrc_cmd_ptr);
          }
        }
        else
        {
          MSG_ERROR("MBMS: Cannot create entry for TMGI %d",
                     msm_cmd_ptr->cmd.cm_mbms_generic_cmd.cmd.activate_req.services[i].tmgi.tmgi,0,0);
        }
      }
      break;

    /* MBMS Service Deactivate command */
    case SYS_GW_MBMS_REQ_DEACTIVATE:

      MSG_HIGH("Recvd SYS_GW_MBMS_REQ_DEACTIVATE for %d services", 
               msm_cmd_ptr->cmd.cm_mbms_generic_cmd.cmd.deactivate_req.num_services,0,0);

      /* Deactivate all services in the cmd */
      for(i = 0; i < msm_cmd_ptr->cmd.cm_mbms_generic_cmd.cmd.deactivate_req.num_services; i++)
      {
        rrc_cmd_type * rrc_cmd_ptr;
        sys_mbms_tmgi_uint_old_s_type local_tmgi;

        MSG_HIGH("Recvd MBMS DEACTIVATED REQ for TMGI %d",
                 msm_cmd_ptr->cmd.cm_mbms_generic_cmd.cmd.deactivate_req.services[i].tmgi.tmgi,0,0);

        if(FALSE == msm_cmd_ptr->cmd.cm_mbms_generic_cmd.cmd.deactivate_req.services[i].tmgi.present)
        {
          MSG_ERROR("TMGI absent in MBMS Deact Req",0,0,0);
          continue;
        }
      
        /* Check if PLMN Id is part of TMGI from SDP. If not append it */
        if(msm_cmd_ptr->cmd.cm_mbms_generic_cmd.cmd.deactivate_req.services[i].tmgi.tmgi > 0xFFFFFF)
        {
          MSG_HIGH("PLMN Id already part of TMGI in MBMS Deact Req",0,0,0);
          local_tmgi.service_id = (uint32)(msm_cmd_ptr->cmd.cm_mbms_generic_cmd.cmd.deactivate_req.services[i].tmgi.tmgi >> 24);
          local_tmgi.plmn_id_present = TRUE;
          local_tmgi.plmn_id = (uint32)(msm_cmd_ptr->cmd.cm_mbms_generic_cmd.cmd.deactivate_req.services[i].tmgi.tmgi & 0xFFFFFF);
        }
        else
        {
          MSG_HIGH("PLMN Id not present in TMGI in MBMS Deact Req",0,0,0);
          local_tmgi.service_id = (uint32) (msm_cmd_ptr->cmd.cm_mbms_generic_cmd.cmd.deactivate_req.services[i].tmgi.tmgi);
          local_tmgi.plmn_id_present = FALSE;
          /* To Do: Query NAS and append PLMN ID to the TMGI value */
        }

        if(MSM_INVALID_INDEX != (srv_index = msmmain_return_db_idx_for_tmgi(local_tmgi)))
        {
          if(msm_mbms_services.services[srv_index].service_state != MSM_MBMS_SERVICE_PENDING_DEACT)
          {
            /* Send a command to RRC to activate service */
            if(NULL == (rrc_cmd_ptr = rrc_get_msm_cmd_buf()))
            {
              ERR_FATAL("MBMS: No memory for RRC cmd",0,0,0);
            }
            else
            {
              rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_MBMS_SERVICE_DEACTIVATED_REQ;
              rrc_cmd_ptr->cmd.mbms_service_deactivated_req.service_id = msm_mbms_services.services[srv_index].tmgi.service_id;
  
              MSG_HIGH("MBMS: Sent RRC_MBMS_SERVICE_DEACTIVATED_REQ for Service id %d",
                       rrc_cmd_ptr->cmd.mbms_service_deactivated_req.service_id,0,0);
  
              /* Set service state apprpriately */
              msm_mbms_services.services[srv_index].service_state = MSM_MBMS_SERVICE_PENDING_DEACT; 
  
              rrc_put_msm_cmd(rrc_cmd_ptr);
            }
          }
          else
          {
            MSG_HIGH("MBMS: Service already pending Deactivation",0,0,0);
          }
        }
        else
        {
          ERR("Service not present in DB, Dropping Deact Req",0,0,0);
        }
      }
      break;

    /* MBMS Session Complete Ind */
    case SYS_GW_MBMS_REQ_SESSION_COMPLETE_IND:
      ERR("SYS_GW_MBMS_REQ_SESSION_COMPLETE_IND currently not handled",0,0,0);
      break;

    /* MBMS Abort Req  (to abort MBMS contex activation procedure in NAS) */
    case SYS_GW_MBMS_REQ_ABORT:
      ERR("SYS_GW_MBMS_REQ_ABORT currently not handled",0,0,0);
      break;

    /* MBMS Service Suspend command */
    case SYS_GW_MBMS_REQ_SUSPEND:
      ERR("SYS_GW_MBMS_REQ_SUSPEND currently not handled",0,0,0);
      break;  

    /* MBMS Service Resume command */
    case SYS_GW_MBMS_REQ_RESUME:
      ERR("SYS_GW_MBMS_REQ_RESUME currently not handled",0,0,0);
      break;

    /* MBMS Available Services List Request */
    case SYS_GW_MBMS_REQ_AVAIL_SERVICES_LIST:
      ERR("SYS_GW_MBMS_REQ_AVAIL_SERVICES_LIST currently not handled",0,0,0);
      break;

    /* MBMS Capability enquiry */
    case SYS_GW_MBMS_REQ_GET_CAPABILITY:
      /* MBMS App is asking for MBMS capability, send the latest capability */
      MSG_HIGH("Recvd SYS_GW_MBMS_REQ_GET_CAPABILITY from CM",0,0,0);
      msm_send_mbms_capability_ind();
      break;

    /* MBMS Set Attribute */
    case SYS_GW_MBMS_REQ_SET_ATTR:
      ERR("SYS_GW_MBMS_REQ_SET_ATTR currently not handled",0,0,0);
      break;

    default:
      ERR("Unknown cmd from CM",0,0,0);
      break;
  }
}

/*===========================================================================

FUNCTION MSM_COMMAND_HANDLER

DESCRIPTION

  This function handles all the commands from other layers

DEPENDENCIES

  None.
  
RETURN VALUE

  None

SIDE EFFECTS

  None.
  
===========================================================================*/
void msm_command_handler
(
  msm_ext_cmd_type * msm_cmd_ptr
)
{
  uint32 i;
  uint32 srv_index;
  cm_rpt_type *cm_cmd_ptr = NULL;
  sm_cmd_type *sm_cmd_ptr = NULL;
  sys_gw_mbms_tmgi_s_type tmgi;
  sys_gw_mbms_tmgi_uint_s_type tmgi_uint;

  switch(msm_cmd_ptr->cmd_hdr.cmd_id)
  {
    case MBMS_CM_GENERIC_CMD:
      msm_handle_mbms_cm_generic_cmd(msm_cmd_ptr);
      break;

    case RRC_MBMS_SERVICE_ACTIVATED_CNF:

      MSG_HIGH("Recvd RRC_MBMS_SERVICE_ACTIVATED_CNF for service %d, status %d, lc_id %d", 
               msm_cmd_ptr->cmd.rrc_mbms_service_act_cnf.service_id,
               msm_cmd_ptr->cmd.rrc_mbms_service_act_cnf.status,
               msm_cmd_ptr->cmd.rrc_mbms_service_act_cnf.lc_id);

      if (MSM_INVALID_INDEX != (srv_index = msm_return_index_for_service(msm_cmd_ptr->cmd.rrc_mbms_service_act_cnf.service_id)))
      {
        if(TRUE == msm_cmd_ptr->cmd.rrc_mbms_service_act_cnf.status)
        {
          /* Update the service state and LC info. If service state is pending deact, leave it as it is
             bcos we would be waiting for deact cnf */
          if(msm_mbms_services.services[srv_index].service_state != MSM_MBMS_SERVICE_PENDING_DEACT)
          {
            msm_mbms_services.services[srv_index].service_state = MSM_MBMS_SERVICE_ACTIVE_TRAFFIC;
          }

          msm_mbms_services.services[srv_index].log_chan_id = msm_cmd_ptr->cmd.rrc_mbms_service_act_cnf.lc_id;
        }

        /* Get CM cmd buf to send cnf to CM */
        if((cm_cmd_ptr = cm_get_cmd_buf()) == NULL)
        {
          ERR_FATAL("MBMS:Failed to get cm rpt buffer", 0, 0, 0 );
        }   
        else
        {
          /* Set hdr to indicate this is a generic MBMS rpt*/
          cm_cmd_ptr->hdr.cmd = CM_RPT_MBMS_GENERIC;
      
          /* Set rpt type to Activate Cnf */
          cm_cmd_ptr->cmd.mbms_generic_rpt.rpt_type = SYS_GW_MBMS_RESP_ACTIVATE_CNF;
      
          /* Set num services to 1. Revisit this code if in future there is a provision 
             to send multiple cnfs in one report */
          cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.activate_cnf.num_services = 1;

          /* Fill in the fields */
          cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.activate_cnf.services[0].tmgi.present = TRUE;
          cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.activate_cnf.services[0].tmgi.tmgi = msm_mbms_services.services[srv_index].tmgi_sdp;
          cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.activate_cnf.services[0].mip = msm_mbms_services.services[srv_index].mip;
          cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.activate_cnf.services[0].port = msm_mbms_services.services[srv_index].port;  
          cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.activate_cnf.services[0].cnf_status = msm_cmd_ptr->cmd.rrc_mbms_service_act_cnf.status;

          if(TRUE == cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.activate_cnf.services[0].cnf_status)
          {
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.activate_cnf.services[0].srv_status = SYS_GW_MBMS_SRV_STATUS_AVAILABLE;
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.activate_cnf.services[0].log_chan_id.present = TRUE;
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.activate_cnf.services[0].log_chan_id.log_chan_id = 
              msm_mbms_services.services[srv_index].log_chan_id;
          }
          else
          {
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.activate_cnf.services[0].srv_status = SYS_GW_MBMS_SRV_STATUS_UNAVAILABLE_FAILURE_OTHER;
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.activate_cnf.services[0].log_chan_id.present = FALSE;
          }

          MSG_HIGH("Sending SYS_GW_MBMS_RESP_ACTIVATE_CNF Service id 0x%x, TMGI %d, Status %d",
                   msm_mbms_services.services[srv_index].tmgi.service_id,
                   cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.activate_cnf.services[0].tmgi.tmgi,
                   cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.activate_cnf.services[0].cnf_status);

          /* If service cnf with status FALSE, remove service from DB because we dont expect a deactivate from 
             upper layers to clean up */
          if(FALSE == cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.activate_cnf.services[0].cnf_status)
          {
            /* Clear service entry */
            MSG_HIGH("Clearing service entry on Act_Cnf failure",0,0,0);
            msm_delete_service_entry(srv_index);
          }
                   
          /* Put CM command */
          cm_put_cmd(cm_cmd_ptr);          
        }       
      }
      else
      {
        MSG_ERROR("MBMS: Service index not found for service id 0x%x",msm_cmd_ptr->cmd.rrc_mbms_service_act_cnf.service_id,0,0);
      }
      break;  

    case RRC_MBMS_SERVICE_IND:

      MSG_HIGH("MBMS: Recvd RRC_MBMS_SERVICE_IND for service 0x%x, status %d, lc_id %d ", 
               msm_cmd_ptr->cmd.rrc_mbms_service_ind.service_id,
               msm_cmd_ptr->cmd.rrc_mbms_service_ind.status,
               msm_cmd_ptr->cmd.rrc_mbms_service_ind.lc_id);

      if(MSM_INVALID_INDEX != (srv_index = msm_return_index_for_service(msm_cmd_ptr->cmd.rrc_mbms_service_ind.service_id)))
      {
        if(TRUE == msm_cmd_ptr->cmd.rrc_mbms_service_ind.status)
        {
          msm_mbms_services.services[srv_index].service_state = MSM_MBMS_SERVICE_ACTIVE_TRAFFIC;
          msm_mbms_services.services[srv_index].log_chan_id = msm_cmd_ptr->cmd.rrc_mbms_service_ind.lc_id;
        }

        /* Get CM cmd buf to send cnf to CM */
        if((cm_cmd_ptr = cm_get_cmd_buf()) == NULL)
        {
          ERR_FATAL("MBMS:Failed to get cm rpt buffer", 0, 0, 0 );
        }   
        else
        {
          /* Set hdr to indicate this is a generic MBMS rpt*/
          cm_cmd_ptr->hdr.cmd = CM_RPT_MBMS_GENERIC;
      
          /* Set rpt type to Activate Cnf */
          cm_cmd_ptr->cmd.mbms_generic_rpt.rpt_type = SYS_GW_MBMS_RESP_SERVICE_IND;
      
          /* Set num services to 1. Revisit this code if in future there is a provision 
             to send multiple cnfs in one report */
          cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.service_ind.num_services = 1;

          /* Fill in the fields */
          cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.service_ind.services[0].tmgi.present = TRUE;
          cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.service_ind.services[0].tmgi.tmgi = msm_mbms_services.services[srv_index].tmgi_sdp;
          cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.service_ind.services[0].mip = msm_mbms_services.services[srv_index].mip;
          cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.service_ind.services[0].port = msm_mbms_services.services[srv_index].port;  
          //cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.service_ind.services[0].cnf_status = msm_cmd_ptr->cmd.rrc_mbms_service_ind.status;

          if(TRUE == msm_cmd_ptr->cmd.rrc_mbms_service_ind.status)
          {
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.service_ind.services[0].srv_status = SYS_GW_MBMS_SRV_STATUS_AVAILABLE;
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.service_ind.services[0].log_chan_id.present = TRUE;
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.service_ind.services[0].log_chan_id.log_chan_id = 
              msm_mbms_services.services[srv_index].log_chan_id;
          }
          else
          {
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.service_ind.services[0].srv_status = SYS_GW_MBMS_SRV_STATUS_UNAVAILABLE_FAILURE_OTHER;
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.service_ind.services[0].log_chan_id.present = FALSE;
          }

          MSG_HIGH("Sending SYS_GW_MBMS_RESP_SERVICE_IND Service id 0x%x, TMGI %d, Srv Status %d",
                   msm_mbms_services.services[srv_index].tmgi.service_id,
                   cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.service_ind.services[0].tmgi.tmgi,
                   cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.service_ind.services[0].srv_status);

          /* If service cnf with status FALSE, remove service from DB because we dont expect a deactivate from 
             upper layers to clean up */
          if(FALSE == msm_cmd_ptr->cmd.rrc_mbms_service_ind.status)
          {
            /* Clear service entry */
            MSG_HIGH("Clearing service entry on Service Ind failure",0,0,0);
            msm_delete_service_entry(srv_index);
          }
                   
          /* Put CM command */
          cm_put_cmd(cm_cmd_ptr);          
        }  
      }
      else
      {
        MSG_ERROR("Service index not found for service id %d",msm_cmd_ptr->cmd.rrc_mbms_service_ind.service_id,0,0);
      }
      break;  

    case RRC_MBMS_SERVICE_DEACTIVATED_CNF:
      MSG_HIGH("MBMS: Recvd RRC_MBMS_SERVICE_DEACTIVATED_CNF for service %d, status %d ", 
               msm_cmd_ptr->cmd.rrc_mbms_service_deact_cnf.service_id,
               msm_cmd_ptr->cmd.rrc_mbms_service_deact_cnf.status,0 );

      if(MSM_INVALID_INDEX != (srv_index = msm_return_index_for_service(msm_cmd_ptr->cmd.rrc_mbms_service_deact_cnf.service_id)))
      {
        if(TRUE == msm_cmd_ptr->cmd.rrc_mbms_service_deact_cnf.status)
        {
          MSG_HIGH("MBMS: Service deactivation Cnf with Success",0,0,0);     

          /* Get CM cmd buf to send cnf to CM */
          if((cm_cmd_ptr = cm_get_cmd_buf()) == NULL)
          {
            ERR_FATAL("MBMS:Failed to get cm rpt buffer", 0, 0, 0 );
          }   
          else
          {
            /* Set hdr to indicate this is a generic MBMS rpt*/
            cm_cmd_ptr->hdr.cmd = CM_RPT_MBMS_GENERIC;
        
            /* Set rpt type to Activate Cnf */
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt_type = SYS_GW_MBMS_RESP_DEACTIVATE_CNF;
        
            /* Set num services to 1. Revisit this code if in future there is a provision 
               to send multiple cnfs in one report */
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.deactivate_cnf.num_services = 1;
  
            /* Fill in the fields */
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.deactivate_cnf.services[0].tmgi.present = TRUE;
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.deactivate_cnf.services[0].tmgi.tmgi = msm_mbms_services.services[srv_index].tmgi_sdp;
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.deactivate_cnf.services[0].mip = msm_mbms_services.services[srv_index].mip;
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.deactivate_cnf.services[0].port = msm_mbms_services.services[srv_index].port;  
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.deactivate_cnf.services[0].cnf_status = msm_cmd_ptr->cmd.rrc_mbms_service_deact_cnf.status;
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.deactivate_cnf.services[0].srv_status = SYS_GW_MBMS_SRV_STATUS_UNAVAILABLE_FAILURE_OTHER;
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.deactivate_cnf.services[0].log_chan_id.present = TRUE;
            cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.deactivate_cnf.services[0].log_chan_id.log_chan_id = msm_mbms_services.services[srv_index].log_chan_id;
            
            MSG_HIGH("Sending SYS_GW_MBMS_RESP_DEACTIVATE_CNF Service id 0x%x, TMGI %d, Status %d",
                     msm_mbms_services.services[srv_index].tmgi.service_id,
                     cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.deactivate_cnf.services[0].tmgi.tmgi,
                     cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.deactivate_cnf.services[0].cnf_status);
  
            /* Put CM command */
            cm_put_cmd(cm_cmd_ptr);          
  
            /* Clear service entry */
            msm_delete_service_entry(srv_index);
          }
        }  
        else
        {
          MSG_ERROR("MBMS: Service De-activation Cnf with Failure",0,0,0);  

          /* Update the service state */
          msm_mbms_services.services[srv_index].service_state = MSM_MBMS_SERVICE_ACTIVE_ACQ;
        }
      }
      else
      {
        MSG_ERROR("MBMS: Service index not found for service id %d",msm_cmd_ptr->cmd.rrc_mbms_service_deact_cnf.service_id,0,0);
      }
      break;        

    case MSM_MM_MBMS_SUPP_IND: 
      MSG_HIGH("Recvd MSM_MM_MBMS_SUPP_IND from MM with core_nw capab %d",
               msm_cmd_ptr->cmd.mm_msm_supp_ind.mbms_support,0,0);

      /* Update the curr plmn id from command */
      curr_plmn_id = msmmain_convert_plmn_id_to_uint32(msm_cmd_ptr->cmd.mm_msm_supp_ind.plmn_id);

      MSG_HIGH("Current PLMN Id from MM_SUPP_IND is %d", curr_plmn_id,0,0);

      /* Check is core_nw capability from the cmd is diff from stored one */
      if(msm_cmd_ptr->cmd.mm_msm_supp_ind.mbms_support != 
         msm_mbms_capability.core_nw_capability)
      {
        MSG_HIGH("MBMS Core nw capability change. old capab %d, new capab %d",
                 msm_mbms_capability.core_nw_capability,
                 msm_cmd_ptr->cmd.mm_msm_supp_ind.mbms_support,0);

        /* Update latest capability */
        msm_mbms_capability.core_nw_capability = msm_cmd_ptr->cmd.mm_msm_supp_ind.mbms_support;

        /* Send capability update indication to upper layers */
        msm_send_mbms_capability_ind();
      }
      else
      {
        MSG_HIGH("No change in MBMS Core nw capability. capab %d",
                 msm_mbms_capability.core_nw_capability,0,0);
      }
      break;

    case RRC_MBMS_CELL_CAPABILITY_IND:
      MSG_HIGH("Recvd RRC_MBMS_CELL_CAPABILITY_IND from RRC with cell capab %d",
               msm_cmd_ptr->cmd.rrc_mbms_cell_capability_ind.cell_mbms_capable,0,0);

      /* Check is core_nw capability from the cmd is diff from stored one */
      if(msm_cmd_ptr->cmd.rrc_mbms_cell_capability_ind.cell_mbms_capable != 
         msm_mbms_capability.cell_capability)
      {
        MSG_HIGH("MBMS Cell capability change. old capab %d, new capab %d",
                 msm_mbms_capability.cell_capability,
                 msm_cmd_ptr->cmd.rrc_mbms_cell_capability_ind.cell_mbms_capable,0);

        /* Update latest capability */
        msm_mbms_capability.cell_capability = msm_cmd_ptr->cmd.rrc_mbms_cell_capability_ind.cell_mbms_capable;

        /* Send capability update indication to upper layers */
        msm_send_mbms_capability_ind();
      }
      else
      {
        MSG_HIGH("No change in MBMS cell capability. capab %d",
                 msm_mbms_capability.cell_capability,0,0);
      }
      break;

    case RRC_MBMS_LL_READY_IND:
      MSG_HIGH("MBMS: Recvd RRC_MBMS_LL_READY_IND from RRC",0,0,0); 
               
      /* Send a command to CM */
      if ((cm_cmd_ptr = cm_get_cmd_buf()) == NULL)
      {
        ERR_FATAL("MBMS:Failed to get cm rpt buffer", 0, 0, 0 );
      }   
      else
      {
        cm_cmd_ptr->hdr.cmd = CM_RPT_MBMS_GENERIC;
        cm_cmd_ptr->cmd.mbms_generic_rpt.rpt_type = SYS_GW_MBMS_RESP_LL_READY_IND;

        MSG_HIGH("MBMS: Sending SYS_GW_MBMS_RESP_LL_READY_IND ",0,0,0);
                     
        /* Put CM command */
        cm_put_cmd(cm_cmd_ptr);          
      } 
      break;

    case MSM_SM_ACTIVATION_IND:
      MSG_HIGH("Recvd MSM_SM_ACTIVATION_IND from SM for MIP 0x%x",
               msm_cmd_ptr->cmd.sm_mbms_act_ind.multicast_addr.addr.v4,0,0);

      /* For now, do not maintain any db. Just send an Activate req 
         to SM to proceed with the MBMS context activation */  
      
      /* Get the SM command */
      sm_cmd_ptr = sm_get_cmd_buf();

      if(sm_cmd_ptr != NULL)
      {
        /* Set the msg set */
        sm_cmd_ptr->header.message_set = MS_MSM_SM;  

        /* Set header appropriately */
        sm_cmd_ptr->header.cmd_id = MSM_SM_ACTIVATION_REQ;

        /* Copy the other params from the incoming cmd */
        sm_cmd_ptr->cmd.mbms_act_req.linked_ti = 
          msm_cmd_ptr->cmd.sm_mbms_act_ind.linked_ti;

        sm_cmd_ptr->cmd.mbms_act_req.multicast_addr = 
          msm_cmd_ptr->cmd.sm_mbms_act_ind.multicast_addr;

        sm_cmd_ptr->cmd.mbms_act_req.apn_name = 
          msm_cmd_ptr->cmd.sm_mbms_act_ind.apn_name; 

        sm_cmd_ptr->cmd.mbms_act_req.connection_id = 
          msm_cmd_ptr->cmd.sm_mbms_act_ind.connection_id;

        sm_cmd_ptr->cmd.mbms_act_req.nsapi = 
          msm_cmd_ptr->cmd.sm_mbms_act_ind.nsapi;

        sm_cmd_ptr->cmd.mbms_act_req.mbms_config_options = 
          msm_cmd_ptr->cmd.sm_mbms_act_ind.mbms_config_options;

        /* Set bearer capabilities */
        sm_cmd_ptr->cmd.mbms_act_req.mbms_bearer_cap.valid = TRUE;
        sm_cmd_ptr->cmd.mbms_act_req.mbms_bearer_cap.max_bit_rare_downlink = MSM_MAX_BEARER_RATE; 

        MSG_HIGH("Sending MSM_SM_ACTIVATION_REQ to SM for MIP 0x%x",
                 sm_cmd_ptr->cmd.mbms_act_req.multicast_addr.addr.v4,0,0);

        /* Send the Command to SM task */
        sm_put_cmd(sm_cmd_ptr);
      }
      else
      {
        ERR_FATAL("No memory for SM cmd",0,0,0);
      }
      break;
      
    case MSM_SM_ACTIVATION_CNF:
    
      MSG_HIGH("Recvd MSM_SM_ACTIVATION_CNF from SM for MIP 0x%x",
               msm_cmd_ptr->cmd.sm_mbms_act_conf.multicast_addr.addr.v4,0,0);

      /* Send indication to MBMS Apps that MBMS context has been successfully activated.*/
      if ((cm_cmd_ptr = cm_get_cmd_buf()) == NULL)
      {
        ERR_FATAL("MBMS:Failed to get cm rpt buffer", 0, 0, 0 );
      }   
      else
      {
        cm_cmd_ptr->hdr.cmd = CM_RPT_MBMS_GENERIC;
        cm_cmd_ptr->cmd.mbms_generic_rpt.rpt_type = SYS_GW_MBMS_RESP_CONTEXT_ACTIVATED_IND;

        /* Initialise TMGI structure */
        tmgi.present = TRUE;
        tmgi.length = 0;

        /* Fill the foll info */
        for(i = 0; i < 3; i++)
        {
          tmgi.value[tmgi.length++] = msm_cmd_ptr->cmd.sm_mbms_act_conf.tmgi.mbms_serv_id[i];
        }

        /* Convert TMGI to uint64 */
        (void)sys_gw_mbms_conv_tmgi_to_tmgi_uint64(&tmgi,&tmgi_uint);

        MSG_HIGH("TMGI value is %d",tmgi_uint.tmgi,0,0);

        cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_activated_ind.tmgi = tmgi_uint;
        
        cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_activated_ind.mip = 
          msm_cmd_ptr->cmd.sm_mbms_act_conf.multicast_addr;
        
        cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_activated_ind.act_status = TRUE;  

        cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_activated_ind.context_failure_reason = 
          SYS_GW_MBMS_CONTEXT_FAILURE_REASON_NONE;

        MSG_HIGH("MBMS: Sending SYS_GW_MBMS_RESP_CONTEXT_ACTIVATED_IND for MIP 0x%x, TMGI %d, Status %d",
                 cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_activated_ind.mip.addr.v4,
                 cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_activated_ind.tmgi.tmgi,
                 cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_activated_ind.act_status);
                 
        /* Put CM command */
        cm_put_cmd(cm_cmd_ptr);          
      }
      break;

    case MSM_SM_ACTIVATION_REJ:
      MSG_HIGH("Recvd MSM_SM_ACTIVATION_REJ from SM for MIP 0x%x",
               msm_cmd_ptr->cmd.sm_mbms_act_rej.multicast_addr.addr.v4,0,0);

      /* Send indication to MBMS Apps that MBMS context has been successfully activated.*/
      if ((cm_cmd_ptr = cm_get_cmd_buf()) == NULL)
      {
        ERR_FATAL("MBMS:Failed to get cm rpt buffer", 0, 0, 0 );
      }   
      else
      {
        cm_cmd_ptr->hdr.cmd = CM_RPT_MBMS_GENERIC;
        cm_cmd_ptr->cmd.mbms_generic_rpt.rpt_type = SYS_GW_MBMS_RESP_CONTEXT_ACTIVATED_IND;

        /* Fill the foll info */
        cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_activated_ind.mip = 
          msm_cmd_ptr->cmd.sm_mbms_act_rej.multicast_addr;

        cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_activated_ind.act_status = FALSE;  

        cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_activated_ind.context_failure_reason = 
          SYS_GW_MBMS_CONTEXT_FAILURE_REASON_OTHER;

        MSG_HIGH("MBMS: Sending SYS_GW_MBMS_RESP_CONTEXT_ACTIVATED_IND for MIP 0x%x, Status %d, Failure reason %d",
                 cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_activated_ind.mip.addr.v4,
                 cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_activated_ind.act_status,
                 cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_activated_ind.context_failure_reason);
                 
        /* Put CM command */
        cm_put_cmd(cm_cmd_ptr);          
      }
      break;

    case MSM_SM_DEACTIVATION_CNF:
      MSG_HIGH("Recvd MSM_SM_DEACTIVATION_CNF from SM for MIP 0x%x",
               msm_cmd_ptr->cmd.sm_mbms_deact_conf.multicast_addr.addr.v4,0,0);

      /* Send indication to MBMS Apps that MBMS context has been successfully activated.*/
      if ((cm_cmd_ptr = cm_get_cmd_buf()) == NULL)
      {
        ERR_FATAL("MBMS:Failed to get cm rpt buffer", 0, 0, 0 );
      }   
      else
      {
        cm_cmd_ptr->hdr.cmd = CM_RPT_MBMS_GENERIC;
        cm_cmd_ptr->cmd.mbms_generic_rpt.rpt_type = SYS_GW_MBMS_RESP_CONTEXT_DEACTIVATED_IND;

        /* Fill the foll info */
        //To Do: cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_deactivated_ind.tmgi
        cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_deactivated_ind.mip = 
          msm_cmd_ptr->cmd.sm_mbms_deact_conf.multicast_addr;
        
        cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_deactivated_ind.deact_status = TRUE;  

        cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_deactivated_ind.context_failure_reason = 
          SYS_GW_MBMS_CONTEXT_FAILURE_REASON_NONE;

        MSG_HIGH("MBMS: Sending SYS_GW_MBMS_RESP_CONTEXT_DEACTIVATED_IND for MIP 0x%x, TMGI %d, Status %d",
                 cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_deactivated_ind.mip.addr.v4,
                 cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_deactivated_ind.tmgi.tmgi,
                 cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_deactivated_ind.deact_status);
                 
        /* Put CM command */
        cm_put_cmd(cm_cmd_ptr);          
      }
      break;

    case MSM_SM_DEACTIVATION_IND:
      MSG_HIGH("Recvd MSM_SM_DEACTIVATION_IND from SM for MIP 0x%x",
               msm_cmd_ptr->cmd.sm_mbms_deact_ind.multicast_addr.addr.v4,0,0);

      /* Send indication to MBMS Apps that MBMS context has been successfully activated.*/
      if ((cm_cmd_ptr = cm_get_cmd_buf()) == NULL)
      {
        ERR_FATAL("MBMS:Failed to get cm rpt buffer", 0, 0, 0 );
      }   
      else
      {
        cm_cmd_ptr->hdr.cmd = CM_RPT_MBMS_GENERIC;
        cm_cmd_ptr->cmd.mbms_generic_rpt.rpt_type = SYS_GW_MBMS_RESP_CONTEXT_DEACTIVATED_IND;

        /* Fill the foll info */
        //To Do: cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_deactivated_ind.tmgi
        cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_deactivated_ind.mip = 
          msm_cmd_ptr->cmd.sm_mbms_deact_ind.multicast_addr;
        
        cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_deactivated_ind.deact_status = TRUE;  

        cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_deactivated_ind.context_failure_reason = 
          SYS_GW_MBMS_CONTEXT_FAILURE_REASON_NONE;

        MSG_HIGH("MBMS: Sending SYS_GW_MBMS_RESP_CONTEXT_DEACTIVATED_IND for MIP 0x%x, TMGI %d, Status %d",
                 cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_deactivated_ind.mip.addr.v4,
                 cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_deactivated_ind.tmgi.tmgi,
                 cm_cmd_ptr->cmd.mbms_generic_rpt.rpt.context_deactivated_ind.deact_status);
                 
        /* Put CM command */
        cm_put_cmd(cm_cmd_ptr);          
      }
      break;

    case MSM_SM_ABORT_CNF:
      ERR("MSM_SM_ABORT_CNF not handled yet",0,0,0);
      break;

    case MBMS_SERVICE_ACTIVATED_REQ:
      MSG_ERROR("MBMS_SERVICE_ACTIVATED_REQ is obsolete",0,0,0);
      break;

    case MBMS_SERVICE_DEACTIVATED_REQ:
      MSG_ERROR("MBMS_SERVICE_DEACTIVATED_REQ is obsolete",0,0,0);
      break;

    default:
      MSG_HIGH("MBMS: Invalid cmd %d not handled", msm_cmd_ptr->cmd_hdr.cmd_id,0,0);
      break;
  }
}



#endif

