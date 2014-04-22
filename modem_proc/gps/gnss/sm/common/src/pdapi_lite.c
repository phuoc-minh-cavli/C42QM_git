/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Position Determination Session Manager Module

GENERAL DESCRIPTION
  This module defines the Position Determination API for both the Location
  Services Manager (LSM) and the Position Determination Session Manager
  (PDSM).

  This API provides Position Determination session control and  event
  notification services to its clients.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015-2020 Qualcomm Technologies, Inc. All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/common/src/pdapi_lite.c#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/21/20   katta   Added new api to handle enhanced position request from the apps
10/01/2020 ska     Changes done for MGP state to OFF when TBF>=5sec
02/17/17   skm     BLE and Barometer support for LPPE Phase 2 
01/09/17   rk      Fix a bug in processing zero wlan-ap measurements.
10/27/16   rk      Allow wlan-ap measurements with zero number of APs in error case.
05/01/15   rh      Added get gps lock synchrous API for LBS
02/24/15   ssu     Adding GM in memory logging.
09/05/14   ssu     GM LOWI Integration
03/10/14   ah      Added support for Get Avail WWAN Pos
06/26/13   mj      Wrap z_wiper_position_report_ex_struct_data with QWIP featurization
09/03/12   rh      Added support for Cell Info and NI message injection
04/18/12   ssu     Added pdsm_lite_pd_get_best_avail_pos()
03/14/12   ss      Added pdsm_lite_reset_location_service() API
03/02/02   rh      Added pdsm_lite_pd_inject_external_position()
06/02/11   rh      Added pdsm_lite_lcs_set_vx_config() API
05/18/11   rc      Lowering priority of some standalone messages from MSG_HIGH to MSG_LOW
09/30/10   rh      Changed ERR_OTHER to more meaningful error codes
08/18/09   gk      ingore the file name length in the efs data  set params
08/10/09   gk      Added support for Version
03/02/09   jd      Added support for Wiper logging over RPC.
02/06/09   atien   Add missing PDSM_LITE_CMD_TYPE_XTRA_T in pdsm_lite_cmd_queue
01/12/09   atien   XTRA-T support
07/28/08   gk      Store user data pointer so it can be returned correctly
07/28/08   gk      Fixed Klocwork errors
06/15/08   kj      Compensate for de-featurization of pdapi.h
06/12/08   lt      Support for external coarse position injection.
11/13/07   gk      Add SUPL certificate injection
08/01/07   rw      Add support for XTRA feature
07/20/07   gk      Removed obsoleted functions, move pdapi_init to here
4/2/07     gk      PC Time injection API 
3/2/07     gk      get a buff for app_info
2/14/07    gk      fixed supl flags, some Lint warnings
12/07/06   cl      Moved pdsm_lite_pool_init to TM-core
12/06/06   gk      Fixed bug in get position
12/04/06   gk      Added LCS API and minor mods after code review
11/8/06    gk      PDAPI for converged GPS
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"     /* Definition for basic types and macros */

#include <string.h>     /* Interface to string library */
#include <stdlib.h>
#include "pdapi_lite.h"       /* Internal interface to pdsm.c */
#include "pdapidbg.h"    /* Interface to PDSM debug services */
#include "pdapiclient_lite.h" /* Interface to PDSM Client Interface Block */
#include "queue.h"      /* Interface to queue services */
#include "pdapibuf.h"
#include "sm_api.h"
#include "aries_os_api.h"
#include "tm_api.h"
#include "gm_api.h"
#include "lm_dmm.h"
/* <EJECT> */
/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS

This section contains global definitions for constants, macros, types,
variables

===========================================================================*/

typedef enum
{
  PDSM_LITE_ACTIVE_TASK_NULL,

#ifdef FEATURE_GPSONE
  PDSM_LITE_ACTIVE_TASK_PDSM,
#endif
  PDSM_LITE_ACTIVE_TASK_MAX
} pdsm_lite_active_task_type;

static pdsm_lite_cmd_s_type cmd_data;

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

pdsm_lite_active_task_type  pdsm_lite_active_task;
/* test_data is used for initializing PE.
*/
/*===========================================================================

FUNCTION PDSM_LITE_CMD_QUEUE

DESCRIPTION
  This function is used queue commands to PDSM task.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

boolean pdsm_lite_cmd_queue( const pdsm_lite_cmd_s_type  *cmd_ptr)
{
  os_IpcMsgType           *ipc_msg_ptr;
  pdsm_lite_cmd_s_type         *cmd_hdr_ptr;

  uint32  length = 0;
  boolean status = FALSE;

  if (cmd_ptr == NULL)
  {
    return status;
  }

  switch (cmd_ptr->cmd_type)
  {
  /* Only 3 types of commands are supported right now */
    case PDSM_LITE_CMD_TYPE_PD:           /* Position Session related command */
    case PDSM_LITE_CMD_TYPE_XTRA:
    case PDSM_LITE_CMD_TYPE_XTRA_T:
    case PDSM_LITE_CMD_TYPE_DPO_STATUS_RPT:
    case PDSM_LITE_CMD_TYPE_EXT_CELL:
      ipc_msg_ptr = os_IpcCreate(sizeof(pdsm_lite_cmd_s_type), IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);
      /* Queue the commands in Client command Queue.
      */
      if (ipc_msg_ptr == NULL)
      {
        PDSM_SWITCH_ERR( "IPC message allocation failed",(cmd_ptr->cmd_type),0,0 );
      }
      else
      {

        cmd_hdr_ptr = (pdsm_lite_cmd_s_type*)ipc_msg_ptr->p_Data; /*lint !e826 */
        *cmd_hdr_ptr = *cmd_ptr;
        ipc_msg_ptr->q_MsgId  = TM_CORE_MSG_ID_PDAPI;
        status    = os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM);
        if (status == FALSE)
        {
          PDSM_MSG_ERROR("Error in sending PDAPI msg_id=%d to TM thread",ipc_msg_ptr->q_MsgId,0,0);
          status = os_IpcDelete(ipc_msg_ptr);
          ipc_msg_ptr = NULL;
        } /* End status */
        else
        {
          PDSM_MSG_HIGH("sending PDAPI msg_id=%d to TM thread", ipc_msg_ptr->q_MsgId,0,0);
        } /* end else status */
      }
      break;
    case PDSM_LITE_CMD_TYPE_PARAM:
          
      {
        ipc_msg_ptr = os_IpcCreate(sizeof(pdsm_lite_cmd_s_type), IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);
      }

      /* Queue the commands in Client command Queue.
      */
      if (ipc_msg_ptr == NULL)
      {
        PDSM_SWITCH_ERR( "IPC message allocation failed",(cmd_ptr->cmd_type),0,0 );
      }
      else
      {

        cmd_hdr_ptr = (pdsm_lite_cmd_s_type*)ipc_msg_ptr->p_Data; /*lint !e826 */
        *cmd_hdr_ptr = *cmd_ptr;
        ipc_msg_ptr->q_MsgId  = TM_CORE_MSG_ID_PDAPI;
        status    = os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM);
        if (status == FALSE)
        {
          PDSM_MSG_ERROR("Error in sending PDAPI msg_id=%d to TM thread",ipc_msg_ptr->q_MsgId,0,0);
          status = os_IpcDelete(ipc_msg_ptr);
          ipc_msg_ptr = NULL;
        } /* End status */
        else
        {
          PDSM_MSG_HIGH("sending PDAPI msg_id=%d to TM thread", ipc_msg_ptr->q_MsgId,0,0);
        } /* end else status */
      }
      break;


    default:
      PDSM_SWITCH_ERR( "Command not supported by PD API",(cmd_ptr->cmd_type),0,0 );
      break;
  } /* End Switch for PDAPI command */
  return status;

} /* End pdsm_lite_cmd_queue( ) */


/*******************************************************************************
********************************************************************************
                          User Invoked Commands.
********************************************************************************
********************************************************************************/


/*===========================================================================

FUNCTION PDSM_LITE_GET_POSITION

DESCRIPTION
  This function is called by different clients to obtain Position Location
  Information.

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

boolean pdsm_lite_get_position(
   pdsm_lite_pd_cmd_cb_f_type    cmd_cb_func,         /* Command callback */
  /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,   /* user data pointer */
   pdsm_lite_pd_option_s_type    *option_ptr,
/*~ PARAM option_ptr POINTER */
   pdsm_lite_pd_qos_type         *qos_ptr,           /* Ptr to user QOS for this request */
  /*~ PARAM qos_ptr POINTER */
   pdsm_lite_client_id_type      client_id           /* Client id of user */
)
{
  pdsm_lite_cmd_s_type         *cmd_ptr;
  pdsm_lite_pd_cmd_s_type      *pd_cmd_ptr;
  pdsm_lite_pd_cmd_info_s_type *cmd_info_ptr;
  pdsm_lite_pd_cmd_err_e_type  cmd_err = PDSM_LITE_PD_CMD_ERR_NOERR;
  pdsm_lite_cmd_s_type         z_cmd_data;
  boolean                 ret_value = FALSE;
  struct pdsm_lite_client_s  *client_ptr;

  /* verify option and qos pointers */
  if (qos_ptr == NULL) 
  {
    return ret_value;
  }

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return ret_value;
  }
  PDSM_MSG_LOW("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_get_position(). CDPtr %p", client_data_ptr,0,0);
  PDSM_MSG_LOW("------------------------------------------",0,0,0);


  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_lite_cmd_s_type));


  /* Pointer to Command header and information.
  */
  pd_cmd_ptr   = &cmd_ptr->cmd.pd;
  cmd_info_ptr = &pd_cmd_ptr->info;

  /* Initialize the command type field.
  */
  cmd_ptr->cmd_type = PDSM_LITE_CMD_TYPE_PD;

  /* Initialize the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = cmd_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_LITE_PD_CMD_GET_POS;

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;  
  /* check if the parametrs are valid or not. */
  if (cmd_err == PDSM_LITE_PD_CMD_ERR_NOERR )
  {
    memscpy((void *)&(cmd_info_ptr->option_type),
           sizeof(cmd_info_ptr->option_type),
           (void *)option_ptr,
           sizeof(*option_ptr));

    memscpy((void *)&(cmd_info_ptr->qos),
           sizeof(cmd_info_ptr->qos),
           (void *)qos_ptr,
           sizeof(*qos_ptr));

    /* Initialize the Command error field. In case of errors found in this function
    ** CM will inform the clients by calling their callbacks immediately.
    */
    cmd_info_ptr->cmd_err = cmd_err;

    /* Queue the command to PDSM Q */
  }
  ret_value = pdsm_lite_cmd_queue( cmd_ptr );
  
  if (ret_value == FALSE) 
  {
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_LITE_PD_CMD_GET_POS, PDSM_LITE_PD_CMD_ERR_NO_BUF_L );
    } /* End if no callback is provided. */
  }
  return ret_value;

} /* End pdsm_lite_get_position() */
/*===========================================================================

FUNCTION PDSM_LITE_END_SESSION

DESCRIPTION
  This function is used to end a PD session

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

boolean pdsm_lite_end_session
(
  pdsm_lite_pd_cmd_cb_f_type      pd_cb_func,    /* Command callback */
  /*~ PARAM pd_cb_func POINTER */
  void               *client_data_ptr,  /* Client specified ptr */
  pdsm_lite_pd_end_session_opt_s_type *end_session_opt,
  pdsm_lite_client_id_type        client_id      /* requesting client ID */
)
{
  pdsm_lite_cmd_s_type         *cmd_ptr;
  pdsm_lite_pd_cmd_s_type      *pd_cmd_ptr;
  pdsm_lite_pd_cmd_info_s_type *cmd_info_ptr;
  pdsm_lite_pd_cmd_err_e_type  cmd_err = PDSM_LITE_PD_CMD_ERR_NOERR;
  pdsm_lite_cmd_s_type         z_cmd_data;
  boolean                 ret_value = FALSE;
  struct pdsm_lite_client_s  *client_ptr;

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL) 
  {
    return ret_value;
  } 

  PDSM_MSG_LOW("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_end_session_ex(). CDPtr %p", client_data_ptr, 0, 0); 
  PDSM_MSG_LOW("------------------------------------------",0,0,0);
  
  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_lite_cmd_s_type));

  /* Get the Command and info pointer's.
  */
  pd_cmd_ptr = &cmd_ptr->cmd.pd;
  cmd_info_ptr = &pd_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_LITE_CMD_TYPE_PD;

  /* Init the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = pd_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_LITE_PD_CMD_END_SESSION_EX;  

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;
  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  /* Indicate user initiated end */
  cmd_info_ptr->end_status = PDSM_LITE_PD_END_CLIENT_END;
  cmd_info_ptr->session_end_info.receiver_off = end_session_opt->receiver_off;

  ret_value = pdsm_lite_cmd_queue( cmd_ptr );
  if (ret_value == FALSE) 
  {
    if ( pd_cb_func != NULL )
    {
      pd_cb_func( client_data_ptr, PDSM_LITE_PD_CMD_END_SESSION, PDSM_LITE_PD_CMD_ERR_NO_BUF_L );
    } /* End if no callback is provided. */
  }

  return ret_value;


} /* End pdsm_lite_end_session( ) */


/*===========================================================================

FUNCTION pdsm_lite_pd_inject_external_time

DESCRIPTION
  Command PDSM to inject time information from an external clock.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  pdsm_lite_init( ) must have been already done.

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
boolean pdsm_lite_pd_inject_external_time(
   pdsm_lite_pd_cmd_cb_f_type    cmd_cb_func,         /* Command callback */
   void                     *client_data_ptr,   /* user data pointer */
   pdsm_lite_pd_external_time_info_type    *ext_time_ptr,        /* external time */
   pdsm_lite_client_id_type      client_id           /* Client id of user */
)
{
  pdsm_lite_cmd_s_type         *cmd_ptr;
  pdsm_lite_pd_cmd_s_type      *pd_cmd_ptr;
  pdsm_lite_pd_cmd_info_s_type *cmd_info_ptr;
  pdsm_lite_pd_cmd_err_e_type  cmd_err = PDSM_LITE_PD_CMD_ERR_NOERR;
  pdsm_lite_cmd_s_type         z_cmd_data;
  boolean                 ret_value = FALSE;
  struct pdsm_lite_client_s  *client_ptr;

  /* verify time Info pointer */
  if (ext_time_ptr == NULL)
  {
    return ret_value;
  }

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return ret_value;
  }

  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_pd_inject_external_time(). CDPtr %p", client_data_ptr, 0, 0); 
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);
  
  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_lite_cmd_s_type));

  /* Get the Command and info pointer's.
  */
  pd_cmd_ptr = &cmd_ptr->cmd.pd;
  cmd_info_ptr = &pd_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_LITE_CMD_TYPE_PD;

  /* Init the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = cmd_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_LITE_PD_CMD_INJECT_TIME;
  memscpy(&cmd_info_ptr->ext_time_info, sizeof(cmd_info_ptr->ext_time_info), ext_time_ptr, sizeof(*ext_time_ptr));
  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;
  ret_value = pdsm_lite_cmd_queue( cmd_ptr );
  if (ret_value == FALSE) 
  {
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_LITE_PD_CMD_INJECT_TIME, PDSM_LITE_PD_CMD_ERR_NO_BUF_L );
    } /* End if no callback is provided. */
  }

  return ret_value;

}
/*===========================================================================

FUNCTION pdsm_lite_pd_inject_external_pos_ex

DESCRIPTION
  !! DEPRECATED API !!, use pdsm_lite_pd_inject_external_position() instead.
  Command PDSM to inject coarse position information from an external source.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  pdsm_lite_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
boolean pdsm_lite_pd_inject_external_pos(
   pdsm_lite_pd_cmd_cb_f_type    cmd_cb_func,           /* Command callback */
   void                     *client_data_ptr,      /* user data pointer */
   pdsm_lite_pd_external_pos_info_ex_type    *ext_pos_ptr, /* external position */
   pdsm_lite_client_id_type      client_id              /* Client id of user */
)
{
  pdsm_lite_pd_external_position_info_type pos_info;

  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("Warning ! Deprecated API called: pdsm_lite_pd_inject_external_pos_ex()",0,0,0);
  PDSM_MSG_HIGH("redirecting to pdsm_lite_pd_inject_external_position()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  if (NULL == ext_pos_ptr) {
    PDSM_MSG_ERROR("PDSM: Null external pos info ptr()",0,0,0);
    return FALSE;
  }

  memset(&pos_info, 0, sizeof(pos_info));

  pos_info.opt_field_mask = ext_pos_ptr->pos_info.opt_field_mask; 
  pos_info.time_info_type = ext_pos_ptr->pos_info.time_info_type;
  pos_info.timestamp_sec = ext_pos_ptr->pos_info.timestamp_sec;

  pos_info.alt_def = ext_pos_ptr->pos_info.alt_def;

  pos_info.source = ext_pos_ptr->pos_info.source;

  pos_info.latitude = (double)ext_pos_ptr->pos_info.latitude * (double)PDSM_LITE_COARSE_LAT_RAD_TO_DEG_SCALE_FACTOR;
  pos_info.longitude = (double)ext_pos_ptr->pos_info.longitude * (double)PDSM_LITE_COARSE_LON_RAD_TO_DEG_SCALE_FACTOR;

  pos_info.loc_unc_hor = (float)ext_pos_ptr->pos_info.loc_unc_hor;
  pos_info.loc_unc_hor_raw = ext_pos_ptr->pos_info.loc_unc_hor_raw;
  pos_info.altitude = (float)ext_pos_ptr->pos_info.altitude;
  pos_info.loc_unc_vert = (float)ext_pos_ptr->pos_info.loc_unc_vert;

  pos_info.confidence_hor = ext_pos_ptr->pos_info.confidence_hor; 
  pos_info.confidence_hor_raw = ext_pos_ptr->pos_info.confidence_hor_raw; 
  pos_info.confidence_vert = ext_pos_ptr->pos_info.confidence_vert; 

  pos_info.e_HoriRelIndicator = ext_pos_ptr->e_HoriRelIndicator;
  pos_info.e_VertRelIndicator = ext_pos_ptr->e_VertRelIndicator;
  pos_info.e_AltitudeSource   = ext_pos_ptr->e_AltitudeSource;
  pos_info.e_AltUncCoverage   = (pdsm_lite_pd_altitude_unc_coverage_e_type)(ext_pos_ptr->e_AltHorLinkage);
  pos_info.e_AltHorLinkage    = (pdsm_lite_pd_altitude_horizontal_linkage_e_type)(ext_pos_ptr->e_AltUncCoverage);

  return pdsm_lite_pd_inject_external_position(cmd_cb_func, client_data_ptr,
    &pos_info, client_id);
}


/*===========================================================================
DESCRIPTION
  Command PDSM to inject coarse position information from an external source
  with reliability associated with the injected position.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
DEPENDENCIES
  pdsm_lite_client_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
boolean pdsm_lite_pd_inject_external_position(
   pdsm_lite_pd_cmd_cb_f_type    cmd_cb_func,           /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,      /* user data pointer */
   pdsm_lite_pd_external_position_info_type  *ext_pos_ptr, /* external position */
   /*~ PARAM ext_pos_ptr POINTER */
   pdsm_lite_client_id_type      client_id              /* Client id of user */
)
{
  pdsm_lite_cmd_s_type         *cmd_ptr;
  pdsm_lite_pd_cmd_s_type      *pd_cmd_ptr;
  pdsm_lite_pd_cmd_info_s_type *cmd_info_ptr;
  pdsm_lite_pd_cmd_err_e_type  cmd_err = PDSM_LITE_PD_CMD_ERR_NOERR;
  pdsm_lite_cmd_s_type         z_cmd_data;
  boolean                 ret_value = TRUE;

  struct pdsm_lite_client_s  *client_ptr;

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL) {
    PDSM_MSG_ERROR("PDSM: client_id not found: %d",client_id,0,0);
    return FALSE;
  }
  if (NULL == ext_pos_ptr) {
    PDSM_MSG_ERROR("PDSM: Null external pos info ptr()",0,0,0);
    return FALSE;
  }

  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_pd_inject_external_position(). CDPtr %p", client_data_ptr, 0, 0); 
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_lite_cmd_s_type));

  /* Get the Command and info pointer's.
  */
  pd_cmd_ptr = &cmd_ptr->cmd.pd;
  cmd_info_ptr = &pd_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_LITE_CMD_TYPE_PD;

  /* Init the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = cmd_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_LITE_PD_CMD_INJECT_POS;
  memscpy(&cmd_info_ptr->ext_pos_info, sizeof(cmd_info_ptr->ext_pos_info), ext_pos_ptr, sizeof(*ext_pos_ptr));
  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;
  ret_value = pdsm_lite_cmd_queue( cmd_ptr );
  if (ret_value == FALSE) 
  {
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_LITE_PD_CMD_INJECT_POS, PDSM_LITE_PD_CMD_ERR_NO_BUF_L );
    } /* End if no callback is provided. */
  }

  return ret_value;
}  



/*===========================================================================
FUNCTION pdsm_lite_pd_query_gps_state
DESCRIPTION
  Command PDSM to return the state information of the GPS engine.
  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
DEPENDENCIES
  pdsm_lite_init( ) must have been already done.
RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request
SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM extended status events.
===========================================================================*/
boolean pdsm_lite_pd_query_gps_state(
   pdsm_lite_pd_cmd_cb_f_type     cmd_cb_func,       /* Command callback */
   /*~ PARAM cmd_cb_func POINTER */
   void                      *client_data_ptr,  /* Not sure if this is required */
   pdsm_lite_client_id_type       client_id          /* Requesting client id */
)
{
  pdsm_lite_cmd_s_type         *cmd_ptr;
  pdsm_lite_pd_cmd_s_type      *pd_cmd_ptr;
  pdsm_lite_pd_cmd_info_s_type *cmd_info_ptr;
  pdsm_lite_pd_cmd_err_e_type  cmd_err = PDSM_LITE_PD_CMD_ERR_NOERR;
  pdsm_lite_cmd_s_type         z_cmd_data;
  boolean                 ret_value = FALSE;
  struct pdsm_lite_client_s  *client_ptr;

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return ret_value;
  }

  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_pd_query_gps_state(). CDPtr %p", client_data_ptr, 0, 0); 
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_lite_cmd_s_type));

  /* Get the Command and info pointer's.
  */
  pd_cmd_ptr = &cmd_ptr->cmd.pd;
  cmd_info_ptr = &pd_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_LITE_CMD_TYPE_PD;

  /* Init the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = cmd_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_LITE_PD_CMD_GET_GPS_STATE;
  
  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;
    
  ret_value = pdsm_lite_cmd_queue( cmd_ptr );
  if (ret_value == FALSE) 
  {
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_LITE_PD_CMD_GET_GPS_STATE, PDSM_LITE_PD_CMD_ERR_NO_BUF_L );
    } /* End if no callback is provided. */
  }

  return ret_value;
  

}

/*===========================================================================

FUNCTION PDSM_LITE_SET_PARAMETERS

DESCRIPTION
  This function is used to set PDSM Phone parameters.

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

boolean pdsm_lite_set_parameters
(
  pdsm_lite_pa_cmd_cb_f_type    cmd_cb_func,        /* Command callback fn */
  void                     *client_data_ptr,   /* Client provided data pointer */
  pdsm_lite_pa_e_type           pa_set,             /* Type of parameter to Set */
  pdsm_lite_pa_info_type         *pa_ptr,           /* Pointer to Parameter values to bet set to */

  pdsm_lite_client_id_type      client_id
)
{
  pdsm_lite_cmd_s_type         *cmd_ptr;
  pdsm_lite_pa_cmd_s_type      *pa_cmd_ptr;
  pdsm_lite_pa_cmd_info_s_type *cmd_info_ptr;
  pdsm_lite_pa_cmd_err_e_type  cmd_err = PDSM_LITE_PA_CMD_ERR_NOERR;
  boolean                 ret_value = FALSE;

  struct pdsm_lite_client_s  *client_ptr;

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL) 
  {
    return ret_value;
  }

  if (pa_ptr == NULL)
  {
    PDSM_MSG_ERROR("PDSM: Null parameter pointer", 0,0,0);
    return ret_value;
  }
  PDSM_MSG_LOW("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_set_parameters()",0,0,0);
  PDSM_MSG_LOW("------------------------------------------",0,0,0);
  
  cmd_ptr = &cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_lite_cmd_s_type));
  
  pa_cmd_ptr = &cmd_ptr->cmd.pa;
  cmd_info_ptr = &pa_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_LITE_CMD_TYPE_PARAM;

  /* Initalize the header fields.
  */
  pa_cmd_ptr->cmd_cb_func = cmd_cb_func;
  pa_cmd_ptr->client_id = client_id;
  pa_cmd_ptr->data_block_ptr = client_data_ptr;
  pa_cmd_ptr->cmd = PDSM_LITE_PA_CMD_SET_PARAM;

  /* use the latest client data pointer.
  */
  client_ptr->pa_data_ptr = client_data_ptr;
  /* Indicate what type of parameter is required to bet set */
  cmd_info_ptr->set_type = pa_set;
  
  /* Copy the new paramater */
  {
    cmd_info_ptr->pa_set = *pa_ptr;
  }

  cmd_info_ptr->cmd_err = cmd_err;
  /* Now queue the command for pdsm task handling.
  */
  ret_value = pdsm_lite_cmd_queue( cmd_ptr );
  if (ret_value == FALSE) 
  {
    cmd_err = PDSM_LITE_PA_CMD_ERR_NO_BUF_L;
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_LITE_PA_CMD_SET_PARAM, cmd_err );
    } /* End if no callback is provided. */
  }
  
  return ret_value;

} /* End pdsm_lite_set_parameters( ) */

/*===========================================================================

FUNCTION PDSM_LITE_GET_PARAMETERS

DESCRIPTION
  This function is used to set PDSM Phone parameters.

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

boolean pdsm_lite_get_parameters (
    pdsm_lite_pa_cmd_cb_f_type    cmd_cb_func,        /* Command callback fn */
    void                     *client_data_ptr,   /* Client provided data pointer */
    pdsm_lite_pa_e_type           pa_get,             /* Type of parameter to Get */
    pdsm_lite_client_id_type      client_id
)
{
  pdsm_lite_cmd_s_type             *cmd_ptr;
  pdsm_lite_pa_cmd_s_type          *pa_cmd_ptr;
  pdsm_lite_pa_cmd_info_s_type     *cmd_info_ptr;
  pdsm_lite_pa_cmd_err_e_type      cmd_err = PDSM_LITE_PA_CMD_ERR_NOERR;
  pdsm_lite_cmd_s_type             z_cmd_data;
  boolean                     ret_value = FALSE;
  

  struct pdsm_lite_client_s  *client_ptr;

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL) 
  {
    return ret_value;
  }
  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_get_parameters()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);
  
  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_lite_cmd_s_type));
  
  pa_cmd_ptr                  = &cmd_ptr->cmd.pa;
  cmd_info_ptr                = &pa_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type           = PDSM_LITE_CMD_TYPE_PARAM;

  /* Initalize the header fields.
  */
  pa_cmd_ptr->cmd_cb_func     = cmd_cb_func;
  pa_cmd_ptr->client_id       = client_id;
  pa_cmd_ptr->data_block_ptr  = client_data_ptr;
  pa_cmd_ptr->cmd             = PDSM_LITE_PA_CMD_GET_PARAM;

  /* Indicate what type of parameter is required to bet set */
  cmd_info_ptr->set_type      = pa_get;

  cmd_info_ptr->cmd_err       = cmd_err;

  /* use the latest client data pointer.
  */
  client_ptr->pa_data_ptr = client_data_ptr;
  /* Now queue the command for pdsm task handling.
  */
  ret_value = pdsm_lite_cmd_queue( cmd_ptr );
  if (ret_value == FALSE) 
  {
    cmd_err = PDSM_LITE_PA_CMD_ERR_NO_BUF_L;
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_LITE_PA_CMD_GET_PARAM, cmd_err );
    } /* End if no callback is provided. */
  }

  return ret_value;

} /* End pdsm_lite_get_parameters( ) */
/*===========================================================================

FUNCTION pdsm_lite_get_pdapi_version
DESCRIPTION
  Gets the pdapi version number
DEPENDENCIES

RETURN VALUE
  PDSM Version number as a UINT32

SIDE EFFECTS

===========================================================================*/
uint32 pdsm_lite_get_pdapi_version(void)
{
  return 0;
}


/*===========================================================================

FUNCTION pdsm_lite_xtra_set_data

DESCRIPTION
  gpsOneXTRA client calls this function to send parts of the XTRA file to GPS 
  engine.  
  
DEPENDENCIES

RETURN VALUE
  TRUE - Data part has been accepted
  FALSE - Data part has been rejected

SIDE EFFECTS

===========================================================================*/
boolean pdsm_lite_xtra_set_data
(
  pdsm_lite_xtra_cmd_cb_f_type    xtra_cb_func,      /* Command callback */
  pdsm_lite_client_id_type        client_id,         /* Client id of user */
  void                       *client_data_ptr,  /* user data pointer */
  uint32                     xtra_data_len,     /* Length of XTRA data */
  unsigned char              *xtra_data_ptr,    /* XTRA data */
  uint8                      part_number,       /* data part number */
  uint8                      total_parts,       /* total data parts */
  pdsm_lite_xtra_dc_status_e_type xtra_dc_status     /* status of download */
)
{
#ifdef FEATURE_CGPS_XTRA
  pdsm_lite_cmd_s_type           *cmd_ptr;
  pdsm_lite_xtra_cmd_s_type      *xtra_cmd_ptr;
  pdsm_lite_xtra_cmd_info_s_type *cmd_info_ptr;
  pdsm_lite_xtra_cmd_err_e_type  cmd_err = PDSM_LITE_XTRA_CMD_ERR_NOERR;
  pdsm_lite_cmd_s_type           z_cmd_data;
  struct pdsm_lite_client_s      *client_ptr;

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return FALSE;
  }
  
  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_xtra_set_data()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  
  /* Get the Command and info pointers.
  */
  xtra_cmd_ptr = &cmd_ptr->cmd.xtra;
  cmd_info_ptr = &xtra_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_LITE_CMD_TYPE_XTRA;

  /* Init the command header fields.
  */
  xtra_cmd_ptr->cmd_cb_func = xtra_cb_func;
  xtra_cmd_ptr->client_id = client_id;
  xtra_cmd_ptr->data_block_ptr = client_data_ptr;
  xtra_cmd_ptr->cmd = PDSM_LITE_XTRA_CMD_SET_DATA;

  client_ptr->xtra_data_ptr = client_data_ptr;
  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  if (PDSM_LITE_XTRA_CMD_ERR_NOERR != tm_xtra_copy_data(part_number,xtra_data_ptr,xtra_data_len))
  {
    return FALSE;
  }

  cmd_info_ptr->xtra_data_ptr = xtra_data_ptr;
  cmd_info_ptr->xtra_data_len = xtra_data_len;
  cmd_info_ptr->xtra_dc_status = xtra_dc_status;
  cmd_info_ptr->part_number = part_number;
  cmd_info_ptr->total_parts = total_parts;
  /* Send the command to TM
  */
  (void) pdsm_lite_cmd_queue( cmd_ptr );
 
  return TRUE;
#else
  return FALSE;
#endif /* FEATURE_CGPS_XTRA */
  
} /* End pdsm_lite_xtra_set_data( ) */


/*===========================================================================

FUNCTION pdsm_lite_xtra_client_initiate_download_request

DESCRIPTION
   gpsOneXTRA client calls this function when it needs to download an extended 
   assistance file but it does not have necessary information 
   (i.e. server URL, etc). Upon getting the request, TM should send 
   gpsOneXTRA PD API event which contains required information to the 
   gpsOneXTRA client.
   
DEPENDENCIES

RETURN VALUE
   TRUE - Initiate download request success
   FALSE - Initiate download request failed

SIDE EFFECTS

===========================================================================*/
boolean pdsm_lite_xtra_client_initiate_download_request
(
  pdsm_lite_xtra_cmd_cb_f_type   xtra_cb_func,       /* Command callback */
  pdsm_lite_client_id_type       client_id,         /* Client id of user */
  void                      *client_data_ptr   /* user data pointer */
)
{
#ifdef FEATURE_CGPS_XTRA
  pdsm_lite_cmd_s_type           *cmd_ptr;
  pdsm_lite_xtra_cmd_s_type      *xtra_cmd_ptr;
  pdsm_lite_xtra_cmd_info_s_type *cmd_info_ptr;
  pdsm_lite_xtra_cmd_err_e_type  cmd_err = PDSM_LITE_XTRA_CMD_ERR_NOERR;
  pdsm_lite_cmd_s_type           z_cmd_data;
  struct pdsm_lite_client_s      *client_ptr;

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return FALSE;
  }
  
  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_xtra_client_initiate_download_request()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  /* Get the Command and info pointers.
  */
  xtra_cmd_ptr = &cmd_ptr->cmd.xtra;
  cmd_info_ptr = &xtra_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_LITE_CMD_TYPE_XTRA;

  /* Init the command header fields.
  */
  xtra_cmd_ptr->cmd_cb_func = xtra_cb_func;
  xtra_cmd_ptr->client_id = client_id;
  xtra_cmd_ptr->data_block_ptr = client_data_ptr;
  xtra_cmd_ptr->cmd = PDSM_LITE_XTRA_CMD_INITIATE_DOWNLOAD;
  client_ptr->xtra_data_ptr = client_data_ptr;

  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  (void) pdsm_lite_cmd_queue( cmd_ptr );

  return TRUE;
#else  
  return FALSE;
#endif /* FEATURE_CGPS_XTRA */
} /* End pdsm_lite_xtra_client_initiate_download_request( ) */


/*===========================================================================

FUNCTION pdsm_lite_xtra_set_auto_download_params

DESCRIPTION
  gpsOneXTRA client calls this function to modify parameters related to the 
  automatic download functionality.  enable_auto_download flag is used 
  to enable or disable the functionality.  The download_interval parameter 
  specifies the number of hours between automatic downloads and its range 
  is 1-168 (168 = number of hours in 1 week).
  
DEPENDENCIES

RETURN VALUE
  TRUE - Auto Download Parameters are updated
  FALSE - Auto Download Parameters are NOT updated

SIDE EFFECTS

===========================================================================*/
boolean pdsm_lite_xtra_set_auto_download_params
(
  pdsm_lite_xtra_cmd_cb_f_type  xtra_cb_func,         /* Command callback */
  pdsm_lite_client_id_type      client_id,            /* Client id of user */
  void                     *client_data_ptr,     /* user data pointer */
  boolean                  enable_auto_download, /* turn on/off auto download */
  uint16                   download_interval     /* time between downloads */
)
{
#ifdef FEATURE_CGPS_XTRA
  pdsm_lite_cmd_s_type           *cmd_ptr;
  pdsm_lite_xtra_cmd_s_type      *xtra_cmd_ptr;
  pdsm_lite_xtra_cmd_info_s_type *cmd_info_ptr;
  pdsm_lite_cmd_s_type           z_cmd_data;
  pdsm_lite_xtra_cmd_err_e_type  cmd_err = PDSM_LITE_XTRA_CMD_ERR_NOERR;
  
  struct pdsm_lite_client_s  *client_ptr;

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if ( (client_ptr == NULL) || (client_ptr->init_mask != PDSM_LITE_INIT_MASK) )
  {
    return FALSE;
  }
  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_xtra_set_auto_download_params()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;

  /* Get the Command and info pointers.
  */
  xtra_cmd_ptr = &cmd_ptr->cmd.xtra;
  cmd_info_ptr = &xtra_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_LITE_CMD_TYPE_XTRA;

  /* Init the command header fields.
  */
  xtra_cmd_ptr->cmd_cb_func = xtra_cb_func;
  xtra_cmd_ptr->client_id = client_id;
  xtra_cmd_ptr->data_block_ptr = client_data_ptr;
  xtra_cmd_ptr->cmd = PDSM_LITE_XTRA_CMD_SET_AUTO_DOWNLOAD_PARAMS;

  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  cmd_info_ptr->enable_auto_download = enable_auto_download;
  /* use the latest client data pointer.
  */
  client_ptr->xtra_data_ptr = client_data_ptr;
  cmd_info_ptr->download_interval = download_interval;

  (void) pdsm_lite_cmd_queue( cmd_ptr );

  return TRUE;
#else
  return FALSE;
#endif /* FEATURE_CGPS_XTRA */
} /* End pdsm_lite_xtra_set_auto_download_params( ) */


/*===========================================================================

FUNCTION pdsm_lite_xtra_query_data_validity

DESCRIPTION
  gpsOneXTRA client calls this function to request validity information. 
  Upon getting the request, the starting GPS week and GPS minutes 
  along with the duration of the current extended assistance data 
  will be sent to the gpsOneXTRA client.  
  
  If there is no data in the subsystem, the values of the start time and 
  duration will be set to 0.
  
DEPENDENCIES

RETURN VALUE
  TRUE - request is a success
  FALSE - request failed

SIDE EFFECTS

===========================================================================*/
boolean pdsm_lite_xtra_query_data_validity
(
  pdsm_lite_xtra_cmd_cb_f_type  xtra_cb_func,       /* Command callback */
  pdsm_lite_client_id_type      client_id,          /* Client id of user */
  void                     *client_data_ptr    /* user data pointer */
)
{
#ifdef FEATURE_CGPS_XTRA
  pdsm_lite_cmd_s_type           *cmd_ptr;
  pdsm_lite_xtra_cmd_s_type      *xtra_cmd_ptr;
  pdsm_lite_xtra_cmd_info_s_type *cmd_info_ptr;
  pdsm_lite_xtra_cmd_err_e_type  cmd_err = PDSM_LITE_XTRA_CMD_ERR_NOERR;
  pdsm_lite_cmd_s_type           z_cmd_data;
  
  struct pdsm_lite_client_s  *client_ptr;

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return FALSE;
  }
  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_xtra_query_data_validity()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  
  /* Get the Command and info pointers.
  */
  xtra_cmd_ptr = &cmd_ptr->cmd.xtra;
  cmd_info_ptr = &xtra_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_LITE_CMD_TYPE_XTRA;

  /* Init the command header fields.
  */
  xtra_cmd_ptr->cmd_cb_func = xtra_cb_func;
  xtra_cmd_ptr->client_id = client_id;
  xtra_cmd_ptr->data_block_ptr = client_data_ptr;
  xtra_cmd_ptr->cmd = PDSM_LITE_XTRA_CMD_QUERY_DATA_VALIDITY;

  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  /* use the latest client data pointer.
  */
  client_ptr->xtra_data_ptr = client_data_ptr;
  (void) pdsm_lite_cmd_queue( cmd_ptr );

  return TRUE;
#else
  return FALSE;
#endif /* FEATURE_CGPS_XTRA */
} /* End pdsm_lite_xtra_query_data_validity( ) */


/*===========================================================================

FUNCTION pdsm_lite_xtra_inject_time_info

DESCRIPTION
   gpsOneXTRA client calls this function to inject XTRA Time
   
DEPENDENCIES

RETURN VALUE
    TRUE - Time injection success
    FALSE - Time injection failed

SIDE EFFECTS

===========================================================================*/
boolean pdsm_lite_xtra_inject_time_info
(
  pdsm_lite_xtra_cmd_cb_f_type  xtra_cb_func,      /* Command callback */
  pdsm_lite_client_id_type      client_id,         /* Client id of user */
  void                     *client_data_ptr,  /* user data pointer */
  pdsm_lite_xtra_time_info_type *time_info_ptr     /* pointer to time information */
)
{
#ifdef FEATURE_CGPS_XTRA
  pdsm_lite_cmd_s_type           *cmd_ptr;
  pdsm_lite_xtra_cmd_s_type      *xtra_cmd_ptr;
  pdsm_lite_xtra_cmd_info_s_type *cmd_info_ptr;
  pdsm_lite_xtra_cmd_err_e_type  cmd_err = PDSM_LITE_XTRA_CMD_ERR_NOERR;
  pdsm_lite_cmd_s_type           z_cmd_data;
  struct pdsm_lite_client_s  *client_ptr;

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return FALSE;
  }

  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_xtra_inject_time_info()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  
  /* Get the Command and info pointers.
  */
  xtra_cmd_ptr = &cmd_ptr->cmd.xtra;
  cmd_info_ptr = &xtra_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_LITE_CMD_TYPE_XTRA;

  /* Init the command header fields.
  */
  xtra_cmd_ptr->cmd_cb_func = xtra_cb_func;
  xtra_cmd_ptr->client_id = client_id;
  xtra_cmd_ptr->data_block_ptr = client_data_ptr;
  xtra_cmd_ptr->cmd = PDSM_LITE_XTRA_CMD_TIME_INFO_INJ;
  client_ptr->xtra_data_ptr = client_data_ptr;

  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = cmd_err;

  if (time_info_ptr)
  {
    cmd_info_ptr->d_TimeMsec     = (double)time_info_ptr->d_TimeMsec;
    cmd_info_ptr->f_TimeUncMsec  = (float)time_info_ptr->f_TimeUncMsec;
    cmd_info_ptr->b_RefToUtcTime = time_info_ptr->b_RefToUtcTime;
    cmd_info_ptr->b_ForceFlag    = time_info_ptr->b_ForceFlag;
  }

  (void) pdsm_lite_cmd_queue( cmd_ptr );

  return TRUE;
#else  
  return FALSE;
#endif /* FEATURE_CGPS_XTRA s*/
} /* End pdsm_lite_xtra_inject_time_info( ) */
/*===========================================================================

FUNCTION pdsm_lite_xtra_query_xtra_config_info

DESCRIPTION
  gpsOneXTRA client calls this function to query Xtra config info. 
  

DEPENDENCIES

RETURN VALUE
  TRUE - request is a success
  FALSE - request failed
  
SIDE EFFECTS

===========================================================================*/

boolean pdsm_lite_xtra_query_config_info
(
  pdsm_lite_xtra_cmd_cb_f_type  xtra_cb_func,       /* Command callback */
  pdsm_lite_client_id_type      client_id,          /* Client id of user */
  void                     *client_data_ptr,    /* user data pointer */
  pdsm_lite_xtra_config_req_info_type *pXtraCfgReqInfo
)
{
#ifdef FEATURE_CGPS_XTRA
  pdsm_lite_cmd_s_type           *cmd_ptr;
  pdsm_lite_xtra_cmd_s_type      *xtra_cmd_ptr;
  pdsm_lite_xtra_cmd_info_s_type *cmd_info_ptr;
  pdsm_lite_cmd_s_type           z_cmd_data = {0};
  
  struct pdsm_lite_client_s  *client_ptr;

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return FALSE;
  }
  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_xtra_query_config_info()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  
  /* Get the Command and info pointers.
  */
  xtra_cmd_ptr = &cmd_ptr->cmd.xtra;
  cmd_info_ptr = &xtra_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_LITE_CMD_TYPE_XTRA;

  /* Init the command header fields.
  */
  xtra_cmd_ptr->cmd_cb_func = xtra_cb_func;
  xtra_cmd_ptr->client_id = client_id;
  xtra_cmd_ptr->data_block_ptr = client_data_ptr;
  xtra_cmd_ptr->cmd = PDSM_LITE_XTRA_CMD_QUERY_XTRA_CFG_INFO;

  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  cmd_info_ptr->cmd_err = PDSM_LITE_XTRA_CMD_ERR_NOERR;

  /* use the latest client data pointer.
  */
  client_ptr->xtra_data_ptr = client_data_ptr;
  
  if ( NULL != pXtraCfgReqInfo)
  {
    xtra_cmd_ptr->info.enable_auto_download = (pXtraCfgReqInfo->config_mask & 0x01 ) ? FALSE : TRUE;
  }

  (void) pdsm_lite_cmd_queue( cmd_ptr );

  return TRUE;
#else
  return FALSE;
#endif /* FEATURE_CGPS_XTRA */
} /* End pdsm_lite_xtra_query_xtra_config_info( ) */
/*===========================================================================

FUNCTION pdsm_lite_xtra_t_get_data

DESCRIPTION
   XTRA-T client calls this function to get CellDB data from GPS Engine.
   
DEPENDENCIES

RETURN VALUE
    TRUE  - XTRA-T get data command success
    FALSE - XTRA-T get data command failure

SIDE EFFECTS

===========================================================================*/
boolean pdsm_lite_xtra_t_get_data
(
  pdsm_lite_xtra_t_cmd_cb_f_type   xtra_t_cb_func,      /* Command callback */
  void                       *client_data_ptr,     /* Not sure if this is required */
  pdsm_lite_xtra_t_get_s_type      xtra_t_get_type,     /* Parameter mask to be set */
  pdsm_lite_client_id_type         client_id            /* Requesting client id */
)
{
#ifdef FEATURE_CGPS_XTRA_T
  pdsm_lite_cmd_s_type             *cmd_ptr;
  pdsm_lite_xtra_t_cmd_s_type      *xtra_t_cmd_ptr;
  pdsm_lite_xtra_t_cmd_info_s_type *xtra_t_cmd_info_ptr;
  pdsm_lite_xtra_t_cmd_err_e_type   cmd_err = PDSM_LITE_XTRA_T_CMD_ERR_NONE;
  pdsm_lite_cmd_s_type              z_cmd_data;

  PDSM_MSG_HIGH("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_xtra_t_get_data()",0,0,0);
  PDSM_MSG_HIGH("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  
  /* Get the Command and info pointers.
  */
  xtra_t_cmd_ptr = &cmd_ptr->cmd.xtra_t;
  xtra_t_cmd_info_ptr = &xtra_t_cmd_ptr->info;

  /* Init the command type.
  */
  cmd_ptr->cmd_type = PDSM_LITE_CMD_TYPE_XTRA_T;

  /* Init the command header fields.
  */
  xtra_t_cmd_ptr->cmd_cb_func = xtra_t_cb_func;
  xtra_t_cmd_ptr->data_block_ptr = client_data_ptr;
  xtra_t_cmd_ptr->client_id = client_id;
  /* NOTE: xtra_t_cmd_ptr->client is updated in tm_pdapi_cmd_proc() */
  xtra_t_cmd_ptr->cmd = PDSM_LITE_XTRA_T_CMD_GET_DATA;

  /* Initialize the Information fields. cmd_err field is
  ** initialized so that the interface of passing command's to Session
  ** manager in spite of errors is available.
  */
  xtra_t_cmd_info_ptr->cmd_err  = cmd_err;
  xtra_t_cmd_info_ptr->get_data = xtra_t_get_type;

  (void) pdsm_lite_cmd_queue( cmd_ptr );

  return TRUE;
#else  
  if ( xtra_t_cb_func != NULL )
   {
     xtra_t_cb_func(  client_data_ptr, 
                      PDSM_LITE_XTRA_T_CMD_GET_DATA, 
                      PDSM_LITE_XTRA_T_CMD_FEATURE_NOT_ENABLED);
   }
  return FALSE;
#endif /* FEATURE_CGPS_XTRA_T */
}
/* End pdsm_lite_xtra_t_get_data ( ) */
/*===========================================================================

FUNCTION pdsm_lite_pd_get_best_avail_pos

DESCRIPTION
  This API initiates a command to the engine to return the best available position. 
The response to this request is an asynchronous fix report sent to PDAPI. There are 
no assumptions made on the response delay for this fix .  

DEPENDENCIES

RETURN VALUE
   TRUE if the request was successfully queued
   FALSE otherwise

SIDE EFFECTS

===========================================================================*/
boolean pdsm_lite_pd_get_best_avail_pos
(
   pdsm_lite_pd_cmd_cb_f_type cmd_cb_func, /* command result callback function */
   /*~ PARAM pa_motion_data_cb_func POINTER */
   pdsm_lite_client_id_type   client_id,
   void                 *client_data_ptr /* client provided reference data pointer */
)
{
  pdsm_lite_cmd_s_type         *cmd_ptr;
  pdsm_lite_pd_cmd_s_type      *pd_cmd_ptr;
  //pdsm_lite_pd_cmd_info_s_type *cmd_info_ptr;  /* Removing set-but-not-used variable */
  pdsm_lite_cmd_s_type         z_cmd_data;
  boolean                 ret_value = FALSE;
  struct pdsm_lite_client_s  *client_ptr;

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return ret_value;
  }

  PDSM_MSG_LOW("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_pd_get_best_avail_pos(). CDPtr %p", client_data_ptr, 0, 0); 
  PDSM_MSG_LOW("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_lite_cmd_s_type));

  /* Pointer to Command header and information.
  */
  pd_cmd_ptr   = &cmd_ptr->cmd.pd;
  //cmd_info_ptr = &pd_cmd_ptr->info;  /* Removing set-but-not-used variable */
  
  /* Initialize the command type field.
  */
  cmd_ptr->cmd_type = PDSM_LITE_CMD_TYPE_PD;

  /* Initialize the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = cmd_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_LITE_PD_CMD_GET_BEST_AVAIL_POS;

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;
  ret_value = pdsm_lite_cmd_queue( cmd_ptr );
  
  if (ret_value == FALSE) 
  {
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_LITE_PD_CMD_GET_BEST_AVAIL_POS, PDSM_LITE_PD_CMD_ERR_NO_BUF_L );
    } /* End if no callback is provided. */
  }
  return ret_value;
}

/*===========================================================================

FUNCTION pdsm_lite_pd_get_avail_wwan_pos

DESCRIPTION
  This API initiates a command to the engine to return the available WWAN position. 
The response to this request is an asynchronous fix report sent to PDAPI. There are 
no assumptions made on the response delay for this fix.

DEPENDENCIES

RETURN VALUE
   TRUE if the request was successfully queued
   FALSE otherwise

SIDE EFFECTS

===========================================================================*/
boolean pdsm_lite_pd_get_avail_wwan_pos
(
  pdsm_lite_pd_cmd_cb_f_type   cmd_cb_func, /* command result callback function */
  /*~ PARAM pa_motion_data_cb_func POINTER */
  pdsm_lite_client_id_type     client_id,
  void                   *client_data_ptr /* client provided reference data pointer */
)
{
  pdsm_lite_cmd_s_type         *cmd_ptr;
  pdsm_lite_pd_cmd_s_type      *pd_cmd_ptr;
  pdsm_lite_pd_cmd_info_s_type *cmd_info_ptr;
  pdsm_lite_cmd_s_type         z_cmd_data;
  boolean                 ret_value = FALSE;
  struct pdsm_lite_client_s   *client_ptr;

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return ret_value;
  }

  PDSM_MSG_LOW("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_pd_get_avail_wwan_pos(). CDPtr %p", client_data_ptr, 0, 0); 
  PDSM_MSG_LOW("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_lite_cmd_s_type));

  /* Pointer to Command header and information.
  */
  pd_cmd_ptr   = &cmd_ptr->cmd.pd;
  cmd_info_ptr = &pd_cmd_ptr->info;

  /* Initialize the command type field.
  */
  cmd_ptr->cmd_type = PDSM_LITE_CMD_TYPE_PD;

  /* Initialize the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = cmd_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_LITE_PD_CMD_GET_AVAIL_WWAN_POS;

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;
  ret_value = pdsm_lite_cmd_queue( cmd_ptr );
  
  if (ret_value == FALSE) 
  {
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_LITE_PD_CMD_GET_AVAIL_WWAN_POS, PDSM_LITE_PD_CMD_ERR_NO_BUF_L );
    } /* End if no callback is provided. */
  }
  return ret_value;
}

/*===========================================================================

FUNCTION pdsm_lite_pd_get_avail_wwan_pos_ex

DESCRIPTION
  This API initiates a command to the engine to return the available WWAN position. 
The response to this request is an asynchronous fix report sent to PDAPI. There are 
no assumptions made on the response delay for this fix.

DEPENDENCIES

RETURN VALUE
   TRUE if the request was successfully queued
   FALSE otherwise

SIDE EFFECTS

===========================================================================*/
boolean pdsm_lite_pd_get_avail_wwan_pos_ex
(
  pdsm_lite_pd_cmd_cb_f_type   cmd_cb_func, /* command result callback function */
  /*~ PARAM pa_motion_data_cb_func POINTER */
  pdsm_lite_client_id_type     client_id,
  void                   *client_data_ptr,  /* client provided reference data pointer */
  pdsm_lite_gtp_wwan_info_type *info_ptr
)
{
  pdsm_lite_cmd_s_type         *cmd_ptr;
  pdsm_lite_pd_cmd_s_type      *pd_cmd_ptr;
  pdsm_lite_pd_cmd_info_s_type *cmd_info_ptr;
  pdsm_lite_cmd_s_type         z_cmd_data;
  boolean                 ret_value = FALSE;
  struct pdsm_lite_client_s   *client_ptr;

  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  /* verify client pointer */
  if (client_ptr == NULL)
  {
    return ret_value;
  }

  PDSM_MSG_LOW("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_pd_get_avail_wwan_pos_ex(). CDPtr %p", client_data_ptr, 0, 0); 
  PDSM_MSG_LOW("------------------------------------------",0,0,0);

  cmd_ptr = &z_cmd_data;
  memset(cmd_ptr,0,sizeof(pdsm_lite_cmd_s_type));

  /* Pointer to Command header and information.
  */
  pd_cmd_ptr   = &cmd_ptr->cmd.pd;
  cmd_info_ptr = &pd_cmd_ptr->info;

  /* Initialize the command type field.
  */
  cmd_ptr->cmd_type = PDSM_LITE_CMD_TYPE_PD;
  
  /* Initialize the Information fields. */
  cmd_info_ptr->qos.gps_session_timeout = info_ptr->u_TimeOutSec;

  /* Initialize the command header fields.
  */
  pd_cmd_ptr->cmd_cb_func = cmd_cb_func;
  pd_cmd_ptr->client_id = client_id;
  pd_cmd_ptr->data_block_ptr = client_data_ptr;
  pd_cmd_ptr->cmd = PDSM_LITE_PD_CMD_GET_AVAIL_WWAN_POS;

  /* use the latest client data pointer.
  */
  client_ptr->pd_data_ptr = client_data_ptr;
  ret_value = pdsm_lite_cmd_queue( cmd_ptr );
  
  if (ret_value == FALSE) 
  {
    if ( cmd_cb_func != NULL )
    {
      cmd_cb_func( client_data_ptr, PDSM_LITE_PD_CMD_GET_AVAIL_WWAN_POS, PDSM_LITE_PD_CMD_ERR_NO_BUF_L );
    } /* End if no callback is provided. */
  }
  return ret_value;
}

boolean pdsm_lite_enable_dpo_status_report(boolean enable)
{
#ifdef FEATURE_GNSS_DPO_STATUS_REPORT
  pdsm_lite_cmd_s_type z_cmd_data;

  PDSM_MSG_MED("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_enable_dpo_status_report() enable %d",enable,0,0);
  PDSM_MSG_MED("------------------------------------------",0,0,0);

  /*  pack data for pdsm_lite_cmd_queue */
  z_cmd_data.cmd_type = PDSM_LITE_CMD_TYPE_DPO_STATUS_RPT;
  z_cmd_data.cmd.dpo_status_rpt.enable = enable;
  return(pdsm_lite_cmd_queue(&z_cmd_data));
#else
  return FALSE;
#endif /* FEATURE_GNSS_DPO_STATUS_REPORT */
}
/*===========================================================================

FUNCTION pdsm_pd_inject_external_cell_info

DESCRIPTION
  PDSM client calls this function to inject external cell information to SM
  Typically this is the cell info updated from HLOS for external/3rd-party
  modem. This external cell info will be used in SUPL if it's valid, otherwise
  CM's cell info will be used (typically for internal modem)
 
  The supplied callback function pd_cb_func will be called to report this
  command's success/failure status. There will be no PDAPI event for this command.
 
DEPENDENCIES

RETURN VALUE
  TRUE - request is a success
  FALSE - request failed
  
SIDE EFFECTS

===========================================================================*/

boolean pdsm_lite_pd_inject_external_cell_info(
  pdsm_lite_pd_cmd_cb_f_type    pd_cb_func,          /* Command callback */
  void                     *client_data_ptr,    /* user data pointer */
  pdsm_lite_client_id_type      client_id,           /* Client id of user */
  pdsm_lite_cell_info_s_type    *cell_info           /* Pointer to external cell info */
)
{
  boolean b_retval = TRUE;
  pdsm_lite_cmd_s_type        z_cmd_data;
  struct pdsm_lite_client_s  *client_ptr = NULL;

  memset(&z_cmd_data, 0, sizeof(z_cmd_data));
  client_ptr = pdsmclient_lite_get_client_ptr( client_id );

  if((NULL == client_ptr) || (NULL == cell_info))
  {
    b_retval = FALSE;
  }
  else
  {
    PDSM_MSG_MED("--------- GPS Command --------------------",0,0,0);
    PDSM_MSG_MED("pdsm_pd_inject_external_cell_info()",0,0,0);
    PDSM_MSG_MED("------------------------------------------",0,0,0);

    /* pack data for pdsm_cmd_queue */
    z_cmd_data.cmd_type = PDSM_LITE_CMD_TYPE_EXT_CELL;
    z_cmd_data.cmd.ext_cell.client_id = client_id;
    z_cmd_data.cmd.ext_cell.client_data_ptr = client_data_ptr;
    z_cmd_data.cmd.ext_cell.cmd_cb_func = pd_cb_func;
    z_cmd_data.cmd.ext_cell.cell_info = *cell_info;

    b_retval = pdsm_lite_cmd_queue(&z_cmd_data);

    if (b_retval == FALSE) 
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Failed to queue pdsm command INJECT_CELL_INFO",0,0,0);
      if ( pd_cb_func != NULL )
      {
        pd_cb_func( client_data_ptr, PDSM_LITE_PD_CMD_INJECT_CELL_INFO, PDSM_LITE_PD_CMD_ERR_NO_BUF_L );
      } /* End if no callback is provided. */
    }

  }

  return b_retval;
}



/*
******************************************************************************
* pdsm_lite_read_qmi_services_config
*
* Function description:
*
*  This synchronous API is called by QMI services to read QMI configuration at 
*  bootup. This function may be called only after PDAPI is initialized. The QMI 
*  configuration is used by the caller to register with PDAPI 
*
* Parameters:
*  qmi_Config : Configuration for QMI initialization
*
* Return value:
*  TRUE  - Request processed successfully
*  FALSE - Request could not be processed
*
* ******************************************************************************
*/
boolean pdsm_lite_read_qmi_services_config( pdsm_lite_QMIServiceConfig *qmi_Config )
{
  PDSM_MSG_MED("--------- GPS Command --------------------",0,0,0);
  PDSM_MSG_HIGH("pdsm_lite_read_qmi_services_config() Query",0,0,0);
  PDSM_MSG_MED("------------------------------------------",0,0,0);

  if( NULL == qmi_Config )
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"qmi_Config is NULL", 0, 0, 0);
    return FALSE;
  }

  /* zero out the input struct */
  memset(qmi_Config, 0, sizeof(*qmi_Config));


  tm_read_qmi_services_configuration( qmi_Config );

  return TRUE;
}



/*===========================================================================
FUNCTION pdsm_lite_get_gps_lock_status()

DESCRIPTION
  PDSM client calls this function to get current GPS lock status
 
  This is a synchronous API, will return immediately
 
DEPENDENCIES

RETURN VALUE
  Current GPS lock status
  
SIDE EFFECTS

===========================================================================*/
pdsm_lite_gps_lock_e_type pdsm_lite_get_gps_lock_status(void)
{
  return tm_api_get_gps_lock_status();
}

