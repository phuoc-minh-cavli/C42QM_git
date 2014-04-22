/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Position Determination Session Manager Module

GENERAL DESCRIPTION
  This module makes up the Position Determination Session Manager of the subscriber
  station software.

  The Session Manager provides Position Determination session control and event notification
  services to its clients.


EXTERNALIZED FUNCTIONS

  Copyright (c) 2007 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2019 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/pdapi/src/tm_pdapi_client.c#2 $
  $DateTime: 2020/12/14 10:03:21 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/08/20   sai     Change made to report AVAIL WWAN POS REPORT to QMI client even with Geofence concurrency
07/31/19   py      Send BestAvailFix Report even during internal session
02/17/17   skm     BLE and Barometer support for LPPE Phase 2 
01/13/16   rk      For zero wlan-ap meas report, the ack indication should still be sent out
05/26/16   muk     Update the Poly reporting to active Loc_MW session, stored in cachedId
05/01/16   muk     Change NV73542, to Enable Gnss Measurement by Default 
03/16/16   gk      LPPe enablement.
02/17/16   mj      Added NV support to enable/disable CPI injections and early exit logic 
08/26/15   muk     Handle TL1.0 messages to get the best available position estimate
06/22/15   ss      Change to allow assistance data deletion if there is no TM session but
                   engine is ON
01/07/15   jv      Added QZSS Support
10/29/14   ssu     GM LOWI Integration
09/10/14   gk      Added delete throttle timer to prevent B-b delete commands
05/12/14   ah      Added support for Set XTRA Version Check
03/10/14   ssu     Addressing a few KW errors.
03/10/14   ah      Added support for Set Premium Services Cfg & Get Avail WWAN Pos
01/30/14   ss      Allowing Set Param when Geofencing Session is running
06/26/13   mj      Wrap z_get_pos_param_options and qos with QWIP featurization
12/04/12   rk      CR 408753 - Initialize 'pd_data' before populating in tm_pdapi_xlate_use_pos_to_pd_ext_pos(). 
10/03/12   rh      Added a few PA commands for SUPL configuration 
09/03/12   rh      Added support for Cell Info and NI message injection 
07/22/12   rk      Added PDSM_LITE_PA_ASST_GLONASS_POS_PROTOCOL set/get parameter support.
06/12/12   rk      Update 'tm_pdapi_pa_event_callback' function with strict
                   PA event range check.
06/02/11   rh      Added PDSM_LITE_LCS_CMD_SET_VX_CONFIG and Get/Set SUPL security
09/30/10   rh      Changed ERR_OTHER to more meaningful error codes
4/02/10    gk      Added an event callback to inject position
01/29/10   pn      Updated DmLog structs with PACKED_POST.
09/01/09   gk      Added a check to validate the directory for EFS operation
08/23/09   gk      Added new event for guard timer expiry and MM ind
07/07/09   atien   Send a log of LOG_CGPS_PDSM_LITE_EXT_STATUS_POS_REPORT_C_type when
                   receiving position from app via pdsm_lite_use_this_position()
07/07/09   atien   Remove QOS Accuracy check for 0 for all cases.  0 is now used
                   for FEATURE_POSITION_SHOPPING
05/21/09   gk      removed checks for position modes
04/01/09   jlp     Added output of a log and f3 indicating the number of
                   successful fixes the device has had since power up.  Also
                   output an event for the first successful fix.
03/30/09   jd      Added a generic abort handling mechanism for SysD
04/02/09   ns      Support for Qwip: externed tm_pd_client_cmd_check
03/12/09   ns      SYSD triggered only for MSB/ Standalone sessions
03/02/09   jd      Added support for Wiper logging over RPC.
02/19/09   ns      Changed QWip Interface header call
02/04/09   atien   MOLR-AD call flow select
02/06/09   atien   Correct command error in tm_xtra_t_client_cmd_proc
01/02/09   atien   Support XTRA-T
01/25/09   ns      Added support for QWiP
08/08/08   cl      Enable non-encrypted lat/long when security is activated
06/09/08   gk      On-Demand Changes PDAPI events not generated correctly
4/28/08    gk      On-Demand changes
3/6/08     gk      if the server type is LOCAL and if MSA, check if the addr provided is NULL
1/16/08    gk      Dont check for qos=0 and operation mode is not msa
11/13/07    gk     Add SUPL Certificate injection through PDAPI
09/14/07    gk     NMEA CONFIG set parameters support
08/01/07   rw      Add support for XTRA feature
06/29/07   gk      set the pd_data pointers to NULL for all the pd_events
06/05/07   gk      support settign of URL
05/27/07   rw      Add check for app info setting URL paramters
05/23/07   gk      moved return the end session command status before sending the command to TM
5/23/07    gk      added DM event for notify verify
5/09/07    gk      KDDI related changes
04/25/07   gk      modified the pa log to match the new strucutre that needed to be changed for WM
04/4/07    gk      PC Time injection handling, modifing PD_EVENT call back handling, missing cmd callbacks
02/14/07   gk      Lint
02/06/07   gk      Renamed UMTS to UMTS_CP
01/31/07   cl      Change to pass LCS response to TM-Core
1/11/07    gk      Error in comparision, changed to logical or
12/06/06   gk      Updated set parms functionality, lcs functionality
12/04/06   gk      Updated LCS response, minor mods for tbf and num_fixes
                   code review changes, add new param to pd_event_callback
11/8/06    gk      Initial checkin for Converged GPS
#########################################################################################
*/
#include "pdapi_lite.h" //adding explicitly
#include "pdapiclient_lite.h" 
#include "gps_variation.h"
#include "customer.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "tm_pdapi_client.h"
#include "tm_pdapi_iface.h"
#include "pdapidbg.h"
#include "tm_data.h"
#include "pdapibuf.h"
#include "sm_log.h"
#include "sm_dm_event.h"
#include "time_svc.h"
#include "event.h"
#include "event_defs.h"
#include "log.h"
#include "log_codes.h"
#include "tm_common.h"
#include "gm_api.h"

#include "mgp_api.h"
#define XTRAT_CLIENT_COOKIE_RANGE 0XFFFFFFFF

static  pdsm_lite_pd_info_s_type z_pd_info;

static pdsm_lite_pd_option_s_type z_get_pos_param_options_type;
static pdsm_lite_pd_qos_type z_get_pos_params_qos_type;

static  tm_pdapi_get_pos_param_s_type   z_get_pos_param_type;

typedef enum
{
  XTRAT_SESSION_STATE_OFF,
  XTRAT_SESSION_STATE_ON
}xtrat_session_state;


static void tm_ext_client_cmd_status(
  pdsm_lite_cmd_s_type  *p_cmd_type
);

static uint32 cached_client_id;
static int32 cached_sess_type = TM_PDAPI_SESS_TYPE_NONE;

/*
   log.h says LOG_RECORD_DEFINE has been depricated.  log_hdr_type should be
   placed at the top of the structure and not accessed directly
*/
typedef PACKED struct PACKED_POST
{
   /* log header.  DO NOT ACCESS DIRECTLY! */
   log_hdr_type hdr;

   /* Version of this packet.  Currently only 0 is supported */
   uint8 u_Version;

   /* The number of successful fixes received */
   uint32 q_SuccessfulFixCount;

   /*
      The type of client that has received the successful fix.  This should be
      one of the values from pdsm_lite_client_type_e_type
   */
   uint16 w_ClientType;
} LOG_CGPS_DIAG_SUCCESSFUL_FIX_COUNT_C_type;

#define TM_DELETE_PARAM_THROTTLE_TIMER_VAL 1000 /* for now throttling timer is 1 sec */

/*===========================================================================

FUNCTION  tm_pdapi_cmd_proc

DESCRIPTION
  Handles an incoming PDAPI cmd

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

void tm_pdapi_cmd_proc( void* p_data )
{
  pdsm_lite_cmd_s_type   *p_cmd_type;


  pdsm_lite_pd_cmd_err_e_type          e_cmd_err = PDSM_LITE_PD_CMD_ERR_NOERR;
  tm_pdapi_get_pos_param_s_type   *p_get_pos_type = &z_get_pos_param_type;
  
  if (p_data == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  p_cmd_type = (pdsm_lite_cmd_s_type*)p_data;


  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_pdapi_cmd_proc() getting called",0,0,0);

  switch( p_cmd_type->cmd_type )
  {

    case PDSM_LITE_CMD_TYPE_PD:

      if ( PDSM_LITE_PD_CMD_GET_POS == p_cmd_type->cmd.pd.cmd ) 
      {

        /* handle request for Position Determination*/
        /* First find out if the command is O.K, check the parameters 
                and the session state and other things.*/
        p_get_pos_type->client_id = p_cmd_type->cmd.pd.client_id;
        p_get_pos_type->client_type =  pdsmclient_lite_get_client_type_map(p_cmd_type->cmd.pd.client_id);
        p_get_pos_type->option_ptr = &z_get_pos_param_options_type;
        p_get_pos_type->qos_ptr = &z_get_pos_params_qos_type;
        z_get_pos_param_options_type = p_cmd_type->cmd.pd.info.option_type;
        z_get_pos_params_qos_type = p_cmd_type->cmd.pd.info.qos;

        e_cmd_err = tm_core_get_pos_para_check(p_get_pos_type);

        if (e_cmd_err == PDSM_LITE_PD_CMD_ERR_NOERR)
        {
          /* if ok to continue, do validity checks on the parameters */
          e_cmd_err = tm_pd_client_cmd_check( p_cmd_type );
        }

        tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
      }  /* GET_POS */

      if(PDSM_LITE_PD_CMD_ERR_NOERR == e_cmd_err)
      {
        /* Store the client pointer in the command buffer. */
        p_cmd_type->cmd.pd.client = pdsmclient_lite_get_client_ptr(p_cmd_type->cmd.pd.client_id);

        /* Now process this command from Client. It can either be a PD session command of
           request to terminate a session. */
        tm_pd_client_cmd_proc( p_cmd_type );
      }
      break;  /* switch case PDSM_LITE_CMD_TYPE_PD */

    case PDSM_LITE_CMD_TYPE_PARAM:      
      /* Store the client pointer in the command buffer.
      */
      p_cmd_type->cmd.pa.client = pdsmclient_lite_get_client_ptr(p_cmd_type->cmd.pa.client_id);

      /* Now handle the command for parameter setting.
      */
      tm_pa_cmd_proc( p_cmd_type );
      break;

    case PDSM_LITE_CMD_TYPE_XTRA:            /* XTRA related commands */
      /* Store the client pointer in the command buffer.
      */
      p_cmd_type->cmd.xtra.client = pdsmclient_lite_get_client_ptr(p_cmd_type->cmd.xtra.client_id);

      /* Process the XTRA cmd from client
      */
      tm_xtra_client_cmd_proc( p_cmd_type );
      break;

  case PDSM_LITE_CMD_TYPE_XTRA_T:            /* XTRA-T related commands */
    /* Store the client pointer in the command buffer.
    */
    p_cmd_type->cmd.xtra_t.client = pdsmclient_lite_get_client_ptr(p_cmd_type->cmd.xtra_t.client_id);

    /* Process the XTRA-T cmd from client
    */
    tm_xtra_t_client_cmd_proc( p_cmd_type );
    break;

  case PDSM_LITE_CMD_TYPE_DPO_STATUS_RPT:
    {
      boolean b_ret_val;
      /* Enable/Disable the DPO status reporting from MGP */
      if(p_cmd_type->cmd.dpo_status_rpt.enable > 0 ) 
      {
        b_ret_val = tm_core_enable_DpoStatusReport(TRUE);
      }
      else
      {
        b_ret_val = tm_core_enable_DpoStatusReport(FALSE);
      }
     
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"PDSM_LITE_CMD_TYPE_DPO_STATUS_RPT: Enable: %d, ret_val: %d",
               p_cmd_type->cmd.dpo_status_rpt.enable,
               b_ret_val,
               0);
    }
    break;
  
  case PDSM_LITE_CMD_TYPE_EXT_CELL:
	/* delegate handling of task to specific function */
	tm_ext_cell_client_cmd_proc( p_cmd_type );
	break;

  default:
    /* There is nothing we can do with this command
     */
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR," TM_PDAPI: Client command not handled ",p_cmd_type->cmd_type,0,0);
    break;
  } /* End Switch statement */

}

/*===========================================================================

FUNCTION TM_PD_CLIENT_CMD_CHECK

DESCRIPTION
  Checks for Command parameter errors and PDSM state.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

pdsm_lite_pd_cmd_err_e_type tm_pd_client_cmd_check( pdsm_lite_cmd_s_type *p_cmd_type )
{

  pdsm_lite_pd_cmd_s_type       *p_pd_cmd_type;
  pdsm_lite_pd_cmd_err_e_type   e_cmd_err = PDSM_LITE_PD_CMD_ERR_NOERR;
  pdsm_lite_pd_cmd_info_s_type  *p_cmd_info_type;

  if (p_cmd_type == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return PDSM_LITE_PD_CMD_ERR_INVALID_PARAM;
  }

  p_pd_cmd_type = &p_cmd_type->cmd.pd;
  p_cmd_info_type = &p_pd_cmd_type->info;


  /* Check if command is already in error.
  */
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_pd_client_cmd_check() getting called",0,0,0);

  if ( p_cmd_info_type->cmd_err != PDSM_LITE_PD_CMD_ERR_NOERR )
  {
    return  p_cmd_info_type->cmd_err;
  }

  /* There is no error, proceeed to check for command errors.
  */
  switch( p_pd_cmd_type->cmd )
  {
    case PDSM_LITE_PD_CMD_GET_POS:
    {

      /* now assuming session is active check if Transport modes match the new session and old session
      */
      if ((p_pd_cmd_type->info.option_type.session <= PDSM_LITE_PD_SESS_TYPE_MIN) ||
          (p_pd_cmd_type->info.option_type.session >= PDSM_LITE_PD_SESS_TYPE_MAX))
      {
        e_cmd_err = PDSM_LITE_PD_CMD_ERR_SESS_TYPE_P;
      }
      if ((p_pd_cmd_type->info.option_type.operation_mode <= PDSM_LITE_SESSION_OPERATION_MIN) ||
          (p_pd_cmd_type->info.option_type.operation_mode >= PDSM_LITE_SESSION_OPERATION_MAX))
      {
        e_cmd_err = PDSM_LITE_PD_CMD_ERR_OPERATION_P;
      }
      if (p_pd_cmd_type->info.option_type.fix_rate.num_fixes < 1)
      {
        e_cmd_err = PDSM_LITE_PD_CMD_ERR_NUM_FIXES_P;
      }
    } /* End if para_check returned success */

    break;

    case PDSM_LITE_PD_CMD_END_SESSION:
  case PDSM_LITE_PD_CMD_END_SESSION_EX:  

    break;

  default:
    /* Error in switch statement.
    */
    e_cmd_err = PDSM_LITE_PD_CMD_ERR_UNKNOWN_CMD;
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Bad PD command %d",p_pd_cmd_type->cmd,0,0);
    break;

  }/* End of PD switch for different commmands. */

  return e_cmd_err;

}/* End of pd_client_cmd_check( ) */

/*===========================================================================

FUNCTION TM_PD_CLIENT_CMD_STATUS

DESCRIPTION

  This function is used to notify clients of a specified  call command status.

RETURN VALUE

  None

DEPENDENCIES

===========================================================================*/

void tm_pd_client_cmd_status(
  pdsm_lite_cmd_s_type        *p_cmd_type,
      /* Pointer to PDSM command */
  pdsm_lite_pd_cmd_err_e_type e_cmd_err
     /* PDSM call command error */
)
{
  pdsm_lite_pd_cmd_s_type  *p_pd_cmd_type;

  if (p_cmd_type == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  p_pd_cmd_type = &p_cmd_type->cmd.pd;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_pd_client_cmd_status() getting called",0,0,0);

  /* Call the user supplied command callback.
  */
  if ( p_pd_cmd_type->cmd_cb_func != NULL )
  {
    p_pd_cmd_type->cmd_cb_func( p_pd_cmd_type->data_block_ptr,
      p_pd_cmd_type->cmd,
      e_cmd_err
      );
  } /* End if no callback is provided. */

  /* In case of errors notify client list.
  */
  if ( e_cmd_err != PDSM_LITE_PD_CMD_ERR_NOERR )
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "TM_PDAPI: pd cmd err, cmd = %d, err = %d, client = %d",
      p_pd_cmd_type->cmd, e_cmd_err, p_pd_cmd_type->client_id );

    pdsmclient_lite_list_pd_cmd_err_ntfy( e_cmd_err, p_pd_cmd_type );
    sm_report_event_pd_cmd_err((uint8) PD_GET_POS, e_cmd_err);
    sm_log_pd_cmd_err((uint32)p_pd_cmd_type->client_id, p_pd_cmd_type->cmd, e_cmd_err);
  }
} /* End pd_client_cmd_err() */
/*===========================================================================

FUNCTION TM_PD_CLIENT_CMD_PROC

DESCRIPTION
  This function is used handle client commands to PDSM.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

void tm_pd_client_cmd_proc(
  pdsm_lite_cmd_s_type *p_cmd_type
          /* Pointer to PDSM command */
  )
{
  /* Here commands are handled which are passed from a FIFO queue.
  ** Don't worry about priority and QOS for Alpha release.
  */
  pdsm_lite_pd_cmd_err_e_type          e_cmd_err = PDSM_LITE_PD_CMD_ERR_NOERR;
  tm_pdapi_get_pos_param_s_type   *p_get_pos_type = &z_get_pos_param_type;
  pdsm_lite_pd_info_s_type             *p_pd_info_type = &z_pd_info;
  tm_pdapi_end_session_param_s_type z_end_sess_type;
  boolean                         u_ret = TRUE;

  if (p_cmd_type == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_pd_client_cmd_proc() getting called",0,0,0);
  if ( ( pdsmclient_lite_valid_client_id( p_cmd_type->cmd.pd.client_id )) == FALSE )
  {
    e_cmd_err = PDSM_LITE_PD_CMD_ERR_CLIENT_ID_P;
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: PDSM_LITE_PD_CMD_ERR_CLIENT_ID_ID_P",0,0,0);
    tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
    return;
  }


  /* Now proceed with the command as ewerything looks just fine. In case of
  ** connection failure we will indicate an Abort (error) event to clients.
  */
  switch ( p_cmd_type->cmd.pd.cmd )
  {
    case PDSM_LITE_PD_CMD_END_SESSION:
  case PDSM_LITE_PD_CMD_END_SESSION_EX:  
      /* Handle the session end command first.
      */
      sm_log_end_session(p_cmd_type->cmd.pd.client_id);

      z_end_sess_type.client_id = p_cmd_type->cmd.pd.client_id;
      z_end_sess_type.receiver_off =  p_cmd_type->cmd.pd.info.session_end_info.receiver_off;
      e_cmd_err = tm_core_end_session_para_check(&z_end_sess_type);
      tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
      cached_client_id = p_cmd_type->cmd.pd.client_id;

      (void)tm_core_handle_end_session( &z_end_sess_type );  /* Handle the end session command */
      break;

    case PDSM_LITE_PD_CMD_GET_POS:

      sm_log_get_position_request(p_cmd_type);
      cached_client_id = p_cmd_type->cmd.pd.client_id;

      p_get_pos_type->client_id = p_cmd_type->cmd.pd.client_id;
      p_get_pos_type->client_type =  pdsmclient_lite_get_client_type_map(p_cmd_type->cmd.pd.client_id);
      p_get_pos_type->option_ptr = &p_cmd_type->cmd.pd.info.option_type;
      p_get_pos_type->qos_ptr = &p_cmd_type->cmd.pd.info.qos;

      /* handle request for Position Determination.
      */
      /* First find out if the command is O.K, check the parameters
      ** and the session state and other things.
      */
      e_cmd_err = tm_core_get_pos_para_check(p_get_pos_type);
      if (e_cmd_err == PDSM_LITE_PD_CMD_ERR_NOERR)
      {
        /* if ok to continue, do validity checks on the parameters */
        e_cmd_err = tm_pd_client_cmd_check( p_cmd_type );
      }
      /* Indicate to client about the status of this command, irrespective of the
      ** fact whether it was success or failure. In case of failures in connecting
      ** to PDE, it will be indicated as an END_EVENT callback.
      */

      tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );

      /* In case of error we need to return from here.
      */
      if (e_cmd_err == PDSM_LITE_PD_CMD_ERR_NOERR)
      {
        p_pd_info_type->client_id = p_cmd_type->cmd.pd.client_id;
        p_get_pos_type->internal_session = FALSE;

        u_ret = tm_core_handle_get_pos(p_get_pos_type);
      }

      if (!u_ret )
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"TM_PDAPI: TM core returned error for handle get pos", 0,0,0);
      }
      break;

    case PDSM_LITE_PD_CMD_INJECT_TIME:
      tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
      (void)tm_core_handle_inject_ext_time(&p_cmd_type->cmd.pd.info.ext_time_info);
      break;

    case PDSM_LITE_PD_CMD_INJECT_POS:
      tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
      (void)tm_core_handle_inject_ext_pos(p_cmd_type);
      break;

    case PDSM_LITE_PD_CMD_GET_GPS_STATE:
      sm_log_get_gps_state_info(p_cmd_type->cmd.pd.client_id);
      e_cmd_err = tm_core_get_gps_state_err_check();
      tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
      if (e_cmd_err == PDSM_LITE_PD_CMD_ERR_NOERR)
      {
        (void)tm_core_handle_get_gps_state(p_cmd_type);
      }
      break;

    case PDSM_LITE_PD_CMD_GET_BEST_AVAIL_POS:
      /* Request to get the best available position from the engine */
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"PDAPI cmd for best avail position", 0, 0, 0);
      sm_log_get_best_avail_pos_info(p_cmd_type->cmd.pd.client_id);
      e_cmd_err = tm_core_get_best_avail_pos_err_check();
      tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
      if (e_cmd_err == PDSM_LITE_PD_CMD_ERR_NOERR)
      {
        tm_core_get_best_avail_pos_param_s_type get_best_avail_pos_param;

        get_best_avail_pos_param.source = SM_GPS_STATE_INFO_REQ_TYPE_GET_BEST_AVAIL_POS_QMI;
        get_best_avail_pos_param.cmd.p_cmd_type = p_cmd_type;
        
        (void)tm_core_handle_best_avail_pos(&get_best_avail_pos_param);
      }
      break;

    case PDSM_LITE_PD_CMD_GET_AVAIL_WWAN_POS:
      /* Request to get the available WWAN position from the engine */
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"PDAPI cmd for avail WWAN position", 0, 0, 0);
      /* Currently no plans to generate a DM log message for Available WWAN Position */
      e_cmd_err = tm_core_get_best_avail_pos_err_check();
      tm_pd_client_cmd_status( p_cmd_type, e_cmd_err );
      if (e_cmd_err == PDSM_LITE_PD_CMD_ERR_NOERR)
      {
        tm_core_get_avail_wwan_pos_param_s_type get_avail_wwan_pos_param;

        get_avail_wwan_pos_param.source = SM_GPS_STATE_INFO_REQ_TYPE_GET_TLE_AVAIL_POS_QMI;
        get_avail_wwan_pos_param.cmd.p_cmd_type = p_cmd_type;
        
        (void)tm_core_handle_avail_wwan_pos(&get_avail_wwan_pos_param);
      }
      break;
    
  default:
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: command not suported by pd_client_cmd_proc()",p_cmd_type->cmd.pd.cmd,0,0);
      break;
  	}

  return;
} /* End pd_client_cmd_proc() */


/* ========================================================================

FUNCTION TM_PA_CMD_GET_PARAM_PARA_CHECK

DESCRIPTION
  Checks for parameter errors.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

static pdsm_lite_pa_cmd_err_e_type tm_pa_cmd_get_param_para_check( pdsm_lite_pa_cmd_s_type   *p_PaCmdType )
{
  pdsm_lite_pa_cmd_err_e_type      e_cmd_err = PDSM_LITE_PA_CMD_ERR_NOERR;

  /* check if this is a valid client (ID)handle.
  */

  if (p_PaCmdType == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return PDSM_LITE_PA_CMD_ERR_PARAM_TYPE_P;
  }
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_pa_cmd_get_param_para_check() getting called",0,0,0);

  if ( ( pdsmclient_lite_valid_client_id( p_PaCmdType->client_id )) == FALSE )
  {
    e_cmd_err = PDSM_LITE_PA_CMD_ERR_CLIENT_ID_P;
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: PDSM_LITE_PA_CMD_ERR_CLIENT_ID_ID_P",0,0,0);
  }
  if ( (p_PaCmdType->info.set_type <= PDSM_LITE_PA_NONE) ||
       (p_PaCmdType->info.set_type >= PDSM_LITE_PA_MAX))
  {
    /* App is not currently supported.
    */
    e_cmd_err = PDSM_LITE_PA_CMD_ERR_PARAM_TYPE_P;
  }
  return e_cmd_err;
} /* End pa_cmd_get_param_para_check() */

/* ========================================================================

FUNCTION TM_PA_CMD_DELETE_PARAM_PARA_CHECK

DESCRIPTION
  Helper function to massage the parameters of pdsm_lite_set_parameters called with
  cmd type set to PDSM_LITE_PA_DELETE_PARAMS_EX or deprecated PDSM_LITE_PA_DELETE_PARAMS.

RETURN VALUE
  PDSM_LITE_PA_CMD_ERR_NOERR:         If no errors
  PDSM_LITE_PA_CMD_ERR_DELETE_PARM_P: In case parameter deletion cant continue

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

static pdsm_lite_pa_cmd_err_e_type tm_pa_cmd_delete_param_para_check( pdsm_lite_delete_parms_type   *parms )
{
  pdsm_lite_pa_cmd_err_e_type      e_cmd_err = PDSM_LITE_PA_CMD_ERR_NOERR;

  /*Accept the delete param command if MGP is in OFF state or check if on-demand LPM is running */
  /* do not accept delete if the on-demand READY mode is ON */
  boolean delete_parms = FALSE;
  uint32 q_throttle_time_remaining =0;

  if(NULL == parms)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Null Parms in Delete Param Check",0,0,0);
    return PDSM_LITE_PA_CMD_ERR_NOT_ALLOWED;
  }
  /*Add code to check if the MGP is loaded or not*/
  if (tm_core_info.e_cached_rcvr_state == C_GPS_RCVR_STATE_OFF)
  {
    delete_parms = TRUE;
  }
  else if(FALSE == tm_core_session_active())
  {
    /*Engine is ON but there is no TM session. Example: LM apptrack logic.
      Allow Delete params to go through*/
    delete_parms = TRUE;
  }

  q_throttle_time_remaining = os_TimerCheck(tm_core_info.tm_delete_param_throttle_timer); 
  if (q_throttle_time_remaining > 0)
  {
    delete_parms = FALSE;
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"Delete all throttle timer running. remaining timeout %d", q_throttle_time_remaining,0,0);

  }

  if (delete_parms == TRUE)
  {
    if ((parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_TIME) &&
        ((parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_TIME_OFFSET) ||
         (parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_TIME_UNC)))
    {
      /* if both delete time and inject offset or time unc are set, just honor the delete time */
      parms->pdsm_lite_delete_parms_flags &=  ~(PDSM_LITE_PA_TIME_OFFSET); 
      parms->pdsm_lite_delete_parms_flags &=  ~(PDSM_LITE_PA_TIME_UNC); 
    }

    if ((parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_POS) &&
        ((parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_POS_OFFSET)||
         (parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_POS_UNC)))
    {
      /* if both delete time and inject offset are set, just honor the delete time */
      parms->pdsm_lite_delete_parms_flags &=  ~(PDSM_LITE_PA_POS_OFFSET); 
      parms->pdsm_lite_delete_parms_flags &=  ~(PDSM_LITE_PA_POS_UNC); 
    }

    /* if ephemeris is to be deleted and no SV is marked, mark all the SVs from that
       constellation to be deleted */
    if (parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_EPH )
    {
      parms->eph_sv_mask      = (parms->eph_sv_mask == 0)      ? 0xffffffff : parms->eph_sv_mask;
    }

    if (parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_EPH_GLO )
    {
      parms->glo_eph_sv_mask  = (parms->glo_eph_sv_mask == 0)  ? 0xffffffff : parms->glo_eph_sv_mask;
    }

    if (parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_EPH_BDS )
    {
      parms->bds_eph_sv_mask = (parms->bds_eph_sv_mask == 0) ? 0x1fffffffffULL : parms->bds_eph_sv_mask;
    }

    if (parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_EPH_QZSS )
    {
      parms->qzss_eph_sv_mask  = (parms->qzss_eph_sv_mask == 0)  ? 0xffffffff : parms->qzss_eph_sv_mask;
    }

    if (parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_EPH_GAL )
    {
      parms->gal_eph_sv_mask = (parms->gal_eph_sv_mask == 0) ? 0xfffffffffULL : parms->gal_eph_sv_mask;
    }

    /* if almanac is to be deleted and no SV is marked, mark all the SVs to be deleted */
    if (parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_ALM )
    {
      parms->alm_sv_mask      = (parms->alm_sv_mask == 0)      ? 0xffffffff : parms->alm_sv_mask;
    }

    if (parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_ALM_GLO )
    {
      parms->glo_alm_sv_mask  = (parms->glo_alm_sv_mask == 0)  ? 0xffffffff : parms->glo_alm_sv_mask;
    }

    if (parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_ALM_BDS )
    {
      parms->bds_alm_sv_mask = (parms->bds_alm_sv_mask == 0) ? 0x1fffffffffULL : parms->bds_alm_sv_mask;
    }

    if (parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_ALM_QZSS )
    {
      parms->qzss_alm_sv_mask  = (parms->qzss_alm_sv_mask == 0)  ? 0xffffffff : parms->qzss_alm_sv_mask;
    }

    if (parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_ALM_GAL )
    {
      parms->gal_alm_sv_mask = (parms->gal_alm_sv_mask == 0) ? 0xfffffffffULL : parms->gal_alm_sv_mask;
    }

    if (parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_GPS )
    {
      parms->gps_gnss_sv_blacklist_mask  = (parms->gps_gnss_sv_blacklist_mask == 0)  ? 0xffffffff : parms->gps_gnss_sv_blacklist_mask;
    }

    if (parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_GLO )
    {
      parms->glo_gnss_sv_blacklist_mask  = (parms->glo_gnss_sv_blacklist_mask == 0)  ? 0xffffffff : parms->glo_gnss_sv_blacklist_mask;
    }

    if (parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_BDS )
    {
      parms->bds_gnss_sv_blacklist_mask  = (parms->bds_gnss_sv_blacklist_mask == 0)  ? 0x1fffffffffULL : parms->bds_gnss_sv_blacklist_mask;
    }

    if (parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_QZSS )
    {
      parms->qzss_gnss_sv_blacklist_mask  = (parms->qzss_gnss_sv_blacklist_mask == 0)  ? 0xffffffff : parms->qzss_gnss_sv_blacklist_mask;
    }

    if (parms->pdsm_lite_delete_parms_flags & PDSM_LITE_PA_DELETE_GNSS_SV_BLACKLIST_GAL )
    {
      parms->gal_gnss_sv_blacklist_mask  = (parms->gal_gnss_sv_blacklist_mask == 0)  ? 0xfffffffffULL : parms->gal_gnss_sv_blacklist_mask;
    }
    (void)os_TimerStart(tm_core_info.tm_delete_param_throttle_timer,
                        TM_DELETE_PARAM_THROTTLE_TIMER_VAL, 
                        NULL);

  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: MGP engine state %d,throttle timer %d. Cannot delete aiding data", 
                tm_core_info.e_cached_rcvr_state,  q_throttle_time_remaining,0);
    e_cmd_err = PDSM_LITE_PA_CMD_ERR_NOT_ALLOWED;
  }
  return e_cmd_err;
}
/* ========================================================================

FUNCTION TM_PA_CMD_SET_PARAM_PARA_CHECK

DESCRIPTION
  Checks for parameter errors.

RETURN VALUE
  pdsm_lite_pa_cmd_err_e_type

DEPENDENCIES
  Called by registered clients.

===========================================================================*/


static pdsm_lite_pa_cmd_err_e_type tm_pa_cmd_set_param_para_check( pdsm_lite_pa_cmd_s_type   *p_pa_cmd_type )
{
  pdsm_lite_pa_cmd_err_e_type      e_cmd_err = PDSM_LITE_PA_CMD_ERR_NOERR;

  /* check if this is a valid client (ID)handle.
  */

  if (p_pa_cmd_type == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return PDSM_LITE_PA_CMD_ERR_PARAM_TYPE_P;
  }

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_pa_cmd_set_param_para_check() getting called",0,0,0);

  if ( (p_pa_cmd_type->info.set_type <= PDSM_LITE_PA_NONE) || (p_pa_cmd_type->info.set_type >= PDSM_LITE_PA_MAX) )
  {
    e_cmd_err = PDSM_LITE_PA_CMD_ERR_PARAM_TYPE_P;
  }
  switch( p_pa_cmd_type->info.set_type)
  {
    case PDSM_LITE_PA_GPS_LOCK:           /* GPS LOCK */
      if ((p_pa_cmd_type->info.pa_set.gps_lock < PDSM_LITE_GPS_LOCK_NONE) ||
          (p_pa_cmd_type->info.pa_set.gps_lock > PDSM_LITE_GPS_LOCK_ALL))
      {
        e_cmd_err = PDSM_LITE_PA_CMD_ERR_GPS_LOCK_P;
      }
      break;
    case PDSM_LITE_PA_DELETE_PARAMS_EX1: /* Delete GPS/ GNSS related parameters from the database */
      e_cmd_err = tm_pa_cmd_delete_param_para_check(&p_pa_cmd_type->info.pa_set.delete_params);
      break;

    case PDSM_LITE_PA_DPO_CONTROL:
      break;

#ifdef FEATURE_CGPS_XTRA_T
     case PDSM_LITE_PA_XTRAT_CLIENT_TOKEN:
      // check that the value is within the range 0-0xFFFFFFFF
      if(p_pa_cmd_type->info.pa_set.client_cookie > ((XTRAT_CLIENT_COOKIE_RANGE)))
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"TM_PDAPI: PDSM_LITE_PA_XTRAT_CLIENT_TOKEN: parameter out of range (0-0xFFFFFFFF)", 0,0,0);
        e_cmd_err = PDSM_LITE_PA_CMD_ERR_PARAM_TYPE_P;
      }
      break;

    case PDSM_LITE_PA_XTRAT_USER_SESSION_CONTROL:
      /* do nothing here. always allow it to pass without errors */
      break;
#endif /* FEATURE_CGPS_XTRA_T */

    case PDSM_LITE_PA_PE_CONFIG_PARAM:
      if (p_pa_cmd_type->info.pa_set.pe_config_info.peConfigMask == 0) // config nothing?
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Invalid PE config mask 0x%x",p_pa_cmd_type->info.pa_set.pe_config_info.peConfigMask,0,0);
        e_cmd_err = PDSM_LITE_PA_CMD_ERR_PARAM_TYPE_P;
      }
      break;

    case PDSM_LITE_PA_XTRA_VERSION_CHECK:
      if (p_pa_cmd_type->info.pa_set.xtra_version_check >= PDSM_LITE_PA_XTRA_VERSION_CHECK_MAX)
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Invalid XTRA Version Check type=%d", p_pa_cmd_type->info.pa_set.xtra_version_check, 0, 0);
        e_cmd_err = PDSM_LITE_PA_CMD_ERR_PARAM_TYPE_P;
      }
      break;
       
    case PDSM_LITE_PA_UAX3_CONFIG:
      if (p_pa_cmd_type->info.pa_set.uax3_config_info > PDSM_LITE_PA_UAX3_MASK_ALL)
      {
        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Invalid PDSM_LITE_PA_UAX3_CONFIG value=%d", p_pa_cmd_type->info.pa_set.uax3_config_info);
        e_cmd_err = PDSM_LITE_PA_CMD_ERR_PARAM_TYPE_P;
      }
      break;

    default:
    e_cmd_err = PDSM_LITE_PA_CMD_ERR_PARAM_TYPE_P;
    break;
  }
  return e_cmd_err;
}

/*===========================================================================

FUNCTION TM_PA_CLIENT_CMD_CHECK

DESCRIPTION
  Checks for Position Determination Command parameter errors and whether command
  is allowed in this State or not.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

static pdsm_lite_pa_cmd_err_e_type tm_pa_client_cmd_check( pdsm_lite_cmd_s_type *p_cmd_type )
{
  pdsm_lite_pa_cmd_s_type      *p_pa_cmd_type;
  pdsm_lite_pa_cmd_info_s_type *p_cmd_info_type;
  pdsm_lite_pa_cmd_err_e_type   e_cmd_err = PDSM_LITE_PA_CMD_ERR_NOERR;
  /* Call this macro to obtain Sesion tage pointer */

  /* Check if command is already in error.
  */
  if (p_cmd_type == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return PDSM_LITE_PA_CMD_ERR_PARAM_TYPE_P;
  }

  p_pa_cmd_type = &p_cmd_type->cmd.pa;
  p_cmd_info_type = &p_pa_cmd_type->info;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_pa_client_cmd_check () getting called",0,0,0);

  if ( p_cmd_info_type->cmd_err != PDSM_LITE_PA_CMD_ERR_NOERR )
  {
    return  p_cmd_info_type->cmd_err;
  }

  /* There is no error, proceeed to check for command errors.
  */
  switch( p_pa_cmd_type->cmd )
  {
    case PDSM_LITE_PA_CMD_GET_PARAM:
      if ( (e_cmd_err = tm_pa_cmd_get_param_para_check( p_pa_cmd_type )) != PDSM_LITE_PA_CMD_ERR_NOERR )
      {
        return e_cmd_err;
      }
      break;

    case PDSM_LITE_PA_CMD_SET_PARAM:  /* Same parameter checking for get_param() and set_param() */
      /* Write a function which checks if pa_set ( add field in session_tag )
      ** is already set ( indicating param set in progress).
      */
      if ( (e_cmd_err = tm_pa_cmd_set_param_para_check( p_pa_cmd_type )) != PDSM_LITE_PA_CMD_ERR_NOERR)
      {
        return e_cmd_err;
      }
      break;

    default:
    /* Error in switch statement.
      */
      e_cmd_err = PDSM_LITE_PA_CMD_ERR_UNKNOWN_CMD;
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Bad PD command %d",p_pa_cmd_type->cmd,0,0);
      return e_cmd_err;

  } /* End of PD switch for different commmands. */



  return e_cmd_err;

}/* End of pa_client_cmd_check( ) */

/*===========================================================================

FUNCTION TM_PA_CLIENT_CMD_STATUS

DESCRIPTION
  This function is used for invoking user provided command callback.


RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

static void tm_pa_client_cmd_status(
  pdsm_lite_cmd_s_type  *p_cmd_type,
  pdsm_lite_pa_cmd_err_e_type e_cmd_err
)
{
  pdsm_lite_pa_cmd_s_type  *p_pa_cmd_type;

  if (p_cmd_type == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }
  p_pa_cmd_type = &p_cmd_type->cmd.pa;

  /* Call the user supplied command callback.
  */
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_pa_cmd_status () getting called",0,0,0);

  if ( p_pa_cmd_type->cmd_cb_func != NULL )
  {
    p_pa_cmd_type->cmd_cb_func( p_pa_cmd_type->data_block_ptr,
                               p_pa_cmd_type->cmd,
                               e_cmd_err
                             );
  } /* End if no callback is provided. */

  if ( e_cmd_err != PDSM_LITE_PA_CMD_ERR_NOERR )
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, " pa cmd err, cmd = %d, err = %d, client = %d",
        p_pa_cmd_type->cmd, e_cmd_err, p_pa_cmd_type->client_id );
    pdsmclient_lite_list_pa_cmd_err_ntfy( e_cmd_err, p_pa_cmd_type );
    sm_report_event_pa_cmd_err((uint8)p_pa_cmd_type->cmd, e_cmd_err);
    sm_log_pa_cmd_err((uint32)p_pa_cmd_type->client_id, p_pa_cmd_type->cmd, e_cmd_err);
  }
} /* End pd_client_cmd_err() */
/*===========================================================================

FUNCTION TM_XTRA_CLIENT_CMD_STATUS

DESCRIPTION
  This function is used for invoking user provided command callback.


RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

static void tm_xtra_client_cmd_status(
  pdsm_lite_cmd_s_type           *p_cmd_type,
  pdsm_lite_xtra_cmd_err_e_type  e_cmd_err
)
{
  pdsm_lite_xtra_cmd_s_type  *p_xtra_cmd_type;

  if (p_cmd_type == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  p_xtra_cmd_type = &p_cmd_type->cmd.xtra;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_xtra_cmd_status () getting called",0,0,0);

  /* Only one XTRA client registered at a time */
  if ( e_cmd_err != PDSM_LITE_XTRA_CMD_ERR_NOERR )
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, " XTRA cmd err, cmd = %d, err = %d, client = %d",
        p_xtra_cmd_type->cmd, e_cmd_err, p_xtra_cmd_type->client_id );
  }

  /* Call the user supplied command callback */
  if ( p_xtra_cmd_type->cmd_cb_func != NULL )
  {
    p_xtra_cmd_type->cmd_cb_func( p_xtra_cmd_type->data_block_ptr,
                                  p_xtra_cmd_type->cmd,
                                  e_cmd_err
                                );
  } /* End if no callback is provided. */


} /* End tm_xtra_client_cmd_status() */


/*===========================================================================

FUNCTION TM_XTRA_T_CLIENT_CMD_STATUS

DESCRIPTION
  This function is used for invoking user provided command callback.


RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

static void tm_xtra_t_client_cmd_status(
  pdsm_lite_cmd_s_type            *p_cmd_type,
  pdsm_lite_xtra_t_cmd_err_e_type  e_cmd_err
)
{
  pdsm_lite_xtra_t_cmd_s_type  *p_xtra_t_cmd_type;

  if (p_cmd_type == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  p_xtra_t_cmd_type = &p_cmd_type->cmd.xtra_t;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_xtra_t_client_cmd_status () getting called",0,0,0);

  /* Only one XTRA client registered at a time */
  if ( e_cmd_err != PDSM_LITE_XTRA_T_CMD_ERR_NONE )
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, " XTRA-T cmd err, cmd = %d, err = %d, client = %d",
        p_xtra_t_cmd_type->cmd, e_cmd_err, p_xtra_t_cmd_type->client_id );
  }

  /* Call the user supplied command callback */
  if ( p_xtra_t_cmd_type->cmd_cb_func != NULL )
  {
    p_xtra_t_cmd_type->cmd_cb_func( p_xtra_t_cmd_type->data_block_ptr,
                                    p_xtra_t_cmd_type->cmd,
                                    e_cmd_err
                                  );
  } /* End if no callback is provided. */


} /* End tm_xtra_t_client_cmd_status() */


/*===========================================================================

FUNCTION TM_PA_CMD_PROC

DESCRIPTION
   This function is used to process PA related commands.

RETURN VALUE
  None

DEPENDENCIES


===========================================================================*/

void tm_pa_cmd_proc(
    pdsm_lite_cmd_s_type *p_cmd_type /* Pointer to PDSM command */
)
{
  pdsm_lite_pa_cmd_info_s_type *p_cmd_info_type;
  pdsm_lite_pa_cmd_err_e_type  e_cmd_err;
  pdsm_lite_pa_cmd_s_type      *p_pa_cmd_type;
  pdsm_lite_pa_event_type      e_pa_event =0;
  pdsm_lite_pa_info_s_type     z_pa_info_type;
  pdsm_lite_pa_info_s_type     *p_pa_info_type = &z_pa_info_type;
  boolean                 u_ret = FALSE;
  tm_pdapi_param_s_type z_pa_param_type;
  tm_pdapi_param_s_type *p_pa_param_type = &z_pa_param_type;
  pdsm_lite_client_s_type    *client_ptr;

  if (p_cmd_type  == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  p_cmd_info_type = &p_cmd_type->cmd.pa.info;
  p_pa_cmd_type = &p_cmd_type->cmd.pa;
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_pa_cmd_proc () getting called",0,0,0);
  /* Here commands are handled which are passed from a FIFO queue.
  ** Don't worry about priority and QOS for Alpha release.
  */
  e_cmd_err = PDSM_LITE_PA_CMD_ERR_NOERR;

  /* Now proceed with the command as ewerything looks just fine. In case of
  ** connection failure we will indicate an Abort (error) event to clients.
  */
  if ( ( pdsmclient_lite_valid_client_id( p_cmd_type->cmd.pa.client_id )) == FALSE )
  {
    e_cmd_err = PDSM_LITE_PA_CMD_ERR_CLIENT_ID_P;
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: PDSM_LITE_PA_CMD_ERR_CLIENT_ID_ID_P",0,0,0);
    tm_pa_client_cmd_status( p_cmd_type, e_cmd_err );
    return;
  }
   
  client_ptr = pdsmclient_lite_get_client_ptr(p_cmd_type->cmd.pa.client_id);
  p_pa_info_type->client_id = p_cmd_type->cmd.pa.client_id;

  switch ( p_pa_cmd_type->cmd )
  {
    case PDSM_LITE_PA_CMD_SET_PARAM:
      /* First find out if the command is O.K, check the parameters
      ** and the session state and other things.
      */
      sm_log_set_parameters_request(p_cmd_type);

      p_pa_param_type->client_id = p_cmd_type->cmd.pa.client_id;
      p_pa_param_type->e_pa_type = p_cmd_type->cmd.pa.info.set_type;
      p_pa_param_type->pa_info_type_ptr = &p_cmd_type->cmd.pa.info.pa_set;

      u_ret = tm_core_param_check(p_pa_param_type, PDSM_LITE_PA_CMD_SET_PARAM);
      if( u_ret )
      {
        e_cmd_err = tm_pa_client_cmd_check( p_cmd_type );
      }
      else
      {
        e_cmd_err = PDSM_LITE_PA_CMD_ERR_NOT_ALLOWED;
        /* Set cmd is not allowed when TM task is not active or there is an active session */
      }

      /* Indicate to client about the status of this command, irrespective of the
      ** fact whether it was success or failure.
      */
        tm_pa_client_cmd_status( p_cmd_type, e_cmd_err );

      if (e_cmd_err == PDSM_LITE_PA_CMD_ERR_NOERR)
      {
      /* call the tmcore function to set the parameters */
        u_ret = tm_core_handle_set_param(p_pa_param_type );
      }
      break;

    case PDSM_LITE_PA_CMD_GET_PARAM:
      p_pa_param_type->client_id = p_cmd_type->cmd.pa.client_id;
      p_pa_param_type->e_pa_type = p_cmd_type->cmd.pa.info.set_type;
      p_pa_param_type->pa_info_type_ptr =&p_cmd_type->cmd.pa.info.pa_set;

      u_ret = tm_core_param_check(p_pa_param_type, PDSM_LITE_PA_CMD_GET_PARAM);
      if (u_ret)
      {
        e_cmd_err = tm_pa_client_cmd_check( p_cmd_type );
      }
      else
      {
        e_cmd_err = PDSM_LITE_PA_CMD_ERR_NOT_ALLOWED;
        /* Get cmd is not allowed when TM task is not active or there is an active session */
      }


      /* Indicate to client about the status of this command, irrespective of the
      ** fact whether it was success or failure.
      */
      tm_pa_client_cmd_status( p_cmd_type, e_cmd_err );

      if (e_cmd_err != PDSM_LITE_PA_CMD_ERR_NOERR)
      {
        return;
      }
      /* call the tmcore function to set the parameters */
      u_ret = tm_core_handle_get_param(p_pa_param_type );

      if (p_cmd_info_type->set_type == PDSM_LITE_PA_DELETE_PARAMS_EX1)  /* PDSM_LITE_PA_DELETE_PARAMS is deprecated */
      {
        e_pa_event = PDSM_LITE_PA_EVENT_DELETE_PARAMS; /* PDSM_LITE_PA_DELETE_PARAMS_EX and PDSM_LITE_PA_DELETE_PARAMS
                                                     use the same event since payload data is congruent */
        p_pa_info_type->pa_info.delete_params = p_pa_param_type->pa_info_type_ptr->delete_params;
      }
      else if (p_cmd_info_type->set_type == PDSM_LITE_PA_GPS_LOCK)
      {
        e_pa_event = PDSM_LITE_PA_EVENT_GPS_LOCK;
        p_pa_info_type->pa_info.gps_lock = p_pa_param_type->pa_info_type_ptr->gps_lock;
      }
      else if (p_cmd_info_type->set_type == PDSM_LITE_PA_DPO_CONTROL)
      {
        e_pa_event = PDSM_LITE_PA_EVENT_DPO_CONTROL;
        p_pa_info_type->pa_info.dpo_control = p_pa_param_type->pa_info_type_ptr->dpo_control;
      }
      p_pa_info_type->pa_event =  p_cmd_info_type->set_type;
      sm_log_pa_event_callback(e_pa_event, p_pa_info_type);
      sm_report_event_gps_pa_event_callback(p_pa_info_type->pa_event);

      /* inform the client requesting the get parameters */
      pdsm_lite_client_pa_event_ntfy( client_ptr, e_pa_event, p_pa_info_type);
      break;

    default:
      break;

  }
  return;

} /* End pa_cmd_proc( ) */

/*===========================================================================

FUNCTION tm_xtra_client_cmd_proc

DESCRIPTION
  This function is used handle XTRA client commands.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

void tm_xtra_client_cmd_proc
(
  pdsm_lite_cmd_s_type *p_cmd_type
     /* Pointer to PDSM command */
)
{
  /* Local Variables */
  boolean u_ret = FALSE;
  pdsm_lite_xtra_cmd_err_e_type e_cmd_err = PDSM_LITE_XTRA_CMD_ERR_NOERR;

  /*----------------------------------------------------------------------
     Check input parameters
  ----------------------------------------------------------------------*/
  if (p_cmd_type  == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_xtra_client_cmd_proc() getting called",0,0,0);

  /*----------------------------------------------------------------------
     Handle XTRA client request
  ----------------------------------------------------------------------*/
 
    /* First check the command and session state.
    */
    e_cmd_err = tm_core_xtra_req_check( &p_cmd_type->cmd.xtra );

  /* Indicate to client about the status of this command, irrespective of the
  ** fact whether it was success or failure.
  */
  tm_xtra_client_cmd_status( p_cmd_type, e_cmd_err );

  /* In case of error we need to return from here.
  */
  if (e_cmd_err == PDSM_LITE_XTRA_CMD_ERR_NOERR)
  {
    /* Handle the XTRA client request
    */
    u_ret = tm_core_handle_xtra_client_cmd(&p_cmd_type->cmd.xtra);
  }

  if (!u_ret )
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"TM_PDAPI: TM core returned error for handling XTRA request", 0,0,0);
  }

  return;
} /* End tm_xtra_client_cmd_proc()*/

/*===========================================================================
FUNCTION tm_pdapi_event_first_successful_fix

DESCRIPTION
  This function generates a DM event to report the first successful fix that
  the diag task has received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_pdapi_event_first_successful_fix(void)
{
  event_report(EVENT_CGPS_DIAG_FIRST_SUCCESSFUL_FIX);
} /* gpsdiag_LogSuccessfulFixCount */

/*===========================================================================
FUNCTION tm_pdapi_log_successful_fix_count

DESCRIPTION
   This function generates a DM log message to report the number of successful
   fixes that the diag task has received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tm_pdapi_log_successful_fix_count(const uint32 cq_SuccessfulFixCount,
                                   const pdsm_lite_client_type_e_type ce_ClientType)
{
  LOG_CGPS_DIAG_SUCCESSFUL_FIX_COUNT_C_type * pz_LogPkt;

  /* Try to allocate memory for the log packet */
  pz_LogPkt = (LOG_CGPS_DIAG_SUCCESSFUL_FIX_COUNT_C_type *)log_alloc(
   LOG_CGPS_DIAG_SUCCESSFUL_FIX_COUNT_C,
   sizeof(LOG_CGPS_DIAG_SUCCESSFUL_FIX_COUNT_C_type));

  if(pz_LogPkt != 0)
  {
    /* log is enabled and space is available */
    pz_LogPkt->u_Version = 0;
    pz_LogPkt->q_SuccessfulFixCount = cq_SuccessfulFixCount;
     pz_LogPkt->w_ClientType = (uint16)ce_ClientType;

    log_commit(pz_LogPkt);
  }
} /* gpsdiag_LogSuccessfulFixCount */


/*===========================================================================

FUNCTION tm_xtra_t_client_cmd_proc

DESCRIPTION
  This function is used handle XTRA-T client commands.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_xtra_t_client_cmd_proc
(
  pdsm_lite_cmd_s_type *p_cmd_type /* Pointer to PDSM command */
)
{
  /* Local Variables */
  boolean u_ret = FALSE;
  pdsm_lite_xtra_t_cmd_err_e_type e_cmd_err = PDSM_LITE_XTRA_T_CMD_ERR_NONE;

  /*----------------------------------------------------------------------
     Check input parameters
  ----------------------------------------------------------------------*/
  if (p_cmd_type  == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_xtra_t_client_cmd_proc() getting called",0,0,0);

  /* Send a DM log about XTRA-T session type */
#if (0)
  switch (p_cmd_type->cmd.xtra_t.cmd)
  {
    case PDSM_LITE_XTRA_T_CMD_GET_DATA:
      sm_log_xtra_t_session_type((uint8) PDSM_LITE_XTRA_T_SESSION_UPLOAD_DTR);
      break;

    default:
      sm_log_xtra_t_session_type((uint8) PDSM_LITE_XTRA_T_SESSION_NONE);
      break;
  }
#endif

  /*----------------------------------------------------------------------
     Handle XTRA-T client request
  ----------------------------------------------------------------------*/
 
    /* First check the command and session state.
    */
    e_cmd_err = tm_core_xtra_t_req_check( &p_cmd_type->cmd.xtra_t );
  

  /* Indicate to client about the status of this command, irrespective of the
  ** fact whether it was success or failure.
  */
  tm_xtra_t_client_cmd_status( p_cmd_type, e_cmd_err );

  /* In case of error we need to return from here.
  */
  if (e_cmd_err == PDSM_LITE_XTRA_T_CMD_ERR_NONE)
  {
    /* Set the client ID in the XTRA-T request struct
    */
    p_cmd_type->cmd.xtra_t.client_id = p_cmd_type->cmd.xtra_t.client_id;

    /* Handle the XTRA-T client request
    */
    u_ret = tm_core_handle_xtra_t_client_cmd( &p_cmd_type->cmd.xtra_t );
  }

  if (!u_ret )
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW,"TM_PDAPI: TM core returned error for handling XTRA-T request", 0,0,0);
  }

  return;
} /* tm_xtra_t_client_cmd_proc */

/*===========================================================================

FUNCTION tm_ext_cell_client_cmd_proc

DESCRIPTION
  This function is used handle External Cell Injection client commands.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
void tm_ext_cell_client_cmd_proc
(
  pdsm_lite_cmd_s_type *p_cmd_type /* Pointer to PDSM command */
)
{
  pdsm_lite_pd_cmd_err_e_type cmd_status = PDSM_LITE_PD_CMD_ERR_NOERR;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_ext_cell_client_cmd_proc() getting called",0,0,0);

  /*----------------------------------------------------------------------
     Check input parameters
  ----------------------------------------------------------------------*/
  if(NULL == p_cmd_type)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    cmd_status = PDSM_LITE_PD_CMD_ERR_INVALID_PARAM;
  }
  else 
  {
    // Below function needs to do CellInfo validity check and then return cmd_status
    if ( tm_core_external_cell_db_proc(&p_cmd_type->cmd.ext_cell.cell_info) == FALSE)
    {
      // invalid Cell info passed in?
      cmd_status = PDSM_LITE_PD_CMD_ERR_INVALID_PARAM;  
    }
  }
  /* issue callback with appropriate status */
  if(p_cmd_type!= NULL && p_cmd_type->cmd.ext_cell.cmd_cb_func)
  {
    p_cmd_type->cmd.ext_cell.cmd_cb_func(p_cmd_type->cmd.ext_cell.client_data_ptr, PDSM_LITE_PD_CMD_INJECT_CELL_INFO, cmd_status);
  }

  return;
} /* tm_ext_cell_client_cmd_proc */
/*===========================================================================
FUNCTION  TM_PDAPI_PD_POS EVENT_CALLBACK

DESCRIPTION
  Sends a PD_EVENT POSITION event to client and logs the event in DM.

Parameters:

  pd_event: the event to be sent to PDAPI
===========================================================================*/
void tm_pdapi_pos_event_callback( pdsm_lite_pd_event_type   e_pd_event,
                                            pdsm_lite_pd_info_s_type* p_pd_info_type,
                                           tm_pdapi_sess_type   sess_type
                                )

{
  static uint32 q_successful_fix_count = 0;

  if (p_pd_info_type == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return;

  }  

  if (e_pd_event & PDSM_LITE_PD_EVENT_POSITION)
  {
    const pdsm_lite_client_type_e_type ce_client_type = pdsmclient_lite_get_client_type_map(p_pd_info_type->client_id);

    q_successful_fix_count++;
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"tm_pdapi_pos_event_callback: PDSM_LITE_PD_EVENT_POSITION, Successful Fixes %u",
     q_successful_fix_count, 0, 0);

    if(q_successful_fix_count == 1)
    {
      /* This is the first successful fix */
      tm_pdapi_event_first_successful_fix();
    }

    tm_pdapi_log_successful_fix_count(q_successful_fix_count,
     ce_client_type);
  }
//TODO:[APP][TBD] Not sure what is the use of this function so commenting for now 

  (void) time_get(p_pd_info_type->time_stamp);

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_pdapi_pos_event_callback () getting called",0,0,0);


  pdsmclient_lite_list_pd_event_ntfy( e_pd_event, (pdsm_lite_pd_info_s_type *)p_pd_info_type);

}

/*===========================================================================
FUNCTION  TM_PDAPI_PD_POS_LOG_GENERATION

DESCRIPTION
  Log PD_EVENT POSITION in DM.

Parameters:

  pd_event: the event to be sent to PDAPI
===========================================================================*/
void tm_pdapi_pos_log_generation( pdsm_lite_pd_event_type   e_pd_event,
                                  pdsm_lite_client_id_type         client_id,
                                  sm_FixInfoStructType *lr_data,
                                  boolean              is_sft,
                                  boolean              is_kf
                                )
{
  if (lr_data == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }  
#if 0 /*This information has already been filled in calling function*/
(void) time_get(p_pd_info_type->time_stamp);
#endif 
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_pdapi_pos_log_generation () getting called",0,0,0);
  if ((e_pd_event & PDSM_LITE_PD_EVENT_POSITION) ||
      (e_pd_event & PDSM_LITE_PD_EVENT_VELOCITY) ||
      (e_pd_event & PDSM_LITE_PD_EVENT_HEIGHT))
  {
    sm_log_report_position(client_id,lr_data, is_sft, is_kf);
    sm_report_event_gps_pd_position();
  }
}

/*===========================================================================
FUNCTION  TM_PDAPI_PD_EVENT_CALLBACK

DESCRIPTION
  Sends other PD_EVENT  events to client and logs the event in DM. All events
  other than the Position event use this callback

Parameters:

  pd_pd_info_type: the event and the event data to be sent to PDAPI
===========================================================================*/
void tm_pdapi_pd_event_callback ( tm_pdapi_pd_event_s_type *p_pd_info_type ,
                                  tm_pdapi_sess_type  sess_type)
{
  boolean send_event = TRUE;

  if (p_pd_info_type == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI pd-event_callback: Null pointer to function ", 0,0,0);
    return;
  }
  else if((tm_core_is_internal_session_on() == TRUE)&&
         ((!(p_pd_info_type->e_pd_event & PDSM_LITE_PD_EVENT_COARSE_POS_INJ_MASK)) && 
          (PDSM_LITE_PD_EVENT_BEST_AVAIL_POS_REPORT != p_pd_info_type->e_pd_event) &&
          (PDSM_LITE_PD_STATUS_EVENT_AVAIL_WWAN_POS_REPORT != p_pd_info_type->e_pd_event))
         )
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"TM_PDAPI: Gps session is internal %d ", p_pd_info_type->e_pd_event,0,0);
    return;
  }

  (void) time_get(p_pd_info_type->pd_event_data.pd_info_type.time_stamp);

  if (sess_type != TM_PDAPI_SESS_TYPE_NONE)
  {
    cached_sess_type = sess_type;
  }
  switch (p_pd_info_type->e_pd_event)
  {

     case PDSM_LITE_PD_EVENT_DONE:
        sm_report_event_gps_pd_session_done();
        break;

      case PDSM_LITE_PD_EVENT_END:
        sm_report_event_gps_pd_event_end((uint8)p_pd_info_type->pd_event_data.pd_info_type.pd_info.end_status);
        break;

      case PDSM_LITE_PD_EVENT_BEGIN:
        if(sess_type == TM_PDAPI_SESS_TYPE_MI)
        {
          sm_report_event_gps_pd_session_start(&p_pd_info_type->pd_event_data.pd_get_pos_data,(uint8)MOBILE_INIT_SESSION);
        }
        else
        {
          sm_report_event_gps_pd_session_start(&p_pd_info_type->pd_event_data.pd_get_pos_data,(uint8)NETWORK_INIT_SESSION);
        }
        break;

      case PDSM_LITE_PD_EVENT_UPDATE_FAILURE:
        if (p_pd_info_type->pd_event_data.pd_info_type.client_id == -1)
        {
          p_pd_info_type->pd_event_data.pd_info_type.client_id  = cached_client_id;
        }
        break;

      case PDSM_LITE_PD_EVENT_GPS_BEGIN:

      case PDSM_LITE_PD_EVENT_GPS_DONE:
        /* this event can be called when the tm_core has cleaned up. use the cahced_client_id */
        if (p_pd_info_type->pd_event_data.pd_info_type.client_id == -1)
        {
          p_pd_info_type->pd_event_data.pd_info_type.client_id  = cached_client_id;
        }
        break;
      case PDSM_LITE_PD_EVENT_BEST_AVAIL_POS_REPORT:
           sm_log_report_ext_status_best_avail_pos_info(p_pd_info_type->pd_event_data.pd_info_type.client_id,
                                                       &(p_pd_info_type->pd_event_data.pd_info_type.pd_info.pz_BestAvailPosInd));
       break;
      case PDSM_LITE_PD_STATUS_GPS_STATE_INFO:
          sm_log_report_ext_status_gps_state_info((uint32)p_pd_info_type->pd_event_data.pd_info_type.client_id, 
                                             &(p_pd_info_type->pd_event_data.pd_info_type.pd_info.pz_gps_state_info));
      break;
  
      case PDSM_LITE_PD_STATUS_NMEA_POS_REPORT:
      {
        sm_log_nmea_position_report((uint32)p_pd_info_type->pd_event_data.pd_info_type.client_id, &p_pd_info_type->pd_event_data.pd_info_type.pd_info.nmea_pos_report_type);
       break;
      }
    default:
      break;
  }
  sm_log_pd_event_callback(p_pd_info_type->e_pd_event, &p_pd_info_type->pd_event_data.pd_info_type);

  if (send_event == TRUE)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"PDAPI event 0x %lx sent to PDAPI clients", p_pd_info_type->e_pd_event,0, 0);
    pdsmclient_lite_list_pd_event_ntfy(p_pd_info_type->e_pd_event, &p_pd_info_type->pd_event_data.pd_info_type);
  }

}

/*===========================================================================
FUNCTION  TM_PDAPI_PA_EVENT_CALLBACK

DESCRIPTION
  Sends a PD_EVENT to client and logs the event in DM.

Parameters:

  p_Client -
  e_pd_event: the event to be sent to PDAPI
===========================================================================*/

void tm_pdapi_pa_event_callback( pdsm_lite_pa_info_s_type *p_pa_info_type,pdsm_lite_pa_cmd_err_e_type pa_cmd_err )
{
  pdsm_lite_pa_event_type e_pa_event = PDSM_LITE_PA_EVENT_NONE;

  if (p_pa_info_type == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  // pa event enum range check
  if ( (p_pa_info_type->pa_event < PDSM_LITE_PA_NONE) || (p_pa_info_type->pa_event >= PDSM_LITE_PA_MAX) )
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_PDAPI: PA event %d is out of range. tm_pdapi_pa_event_callback() not called.", p_pa_info_type->pa_event);
    return;
  }

  (void) time_get(p_pa_info_type->time_stamp);

  /* No PA event conversion (enum to bitmask style) required after 'PDSM_LITE_PA_LPP_CONFIG_INFO' enumerator.
   *  Bitmask style PA event processing is discontinued from 'PDSM_LITE_PA_LPP_CONFIG_INFO' onwards.
   */
  switch (p_pa_info_type->pa_event)
  {
    case PDSM_LITE_PA_GPS_LOCK:           /* GPS LOCK */
      e_pa_event = PDSM_LITE_PA_EVENT_GPS_LOCK;
      break;

    case PDSM_LITE_PA_DELETE_PARAMS_EX1:/* Delete GPS / GNSS related parameters from the database */
      e_pa_event = PDSM_LITE_PA_EVENT_DELETE_PARAMS; /* PDSM_LITE_PA_DELETE_PARAMS and PDSM_LITE_PA_DELETE_PARAMS_EX use
                                                the same event since data payload is congruent */
      break;

    case PDSM_LITE_PA_DPO_CONTROL:
      e_pa_event = PDSM_LITE_PA_EVENT_DPO_CONTROL;
      break;

    case PDSM_LITE_PA_XTRAT_CLIENT_TOKEN:
      e_pa_event = PDSM_LITE_PA_EVENT_XTRAT_CLIENT_TOKEN;
      break;

    case PDSM_LITE_PA_XTRAT_USER_SESSION_CONTROL:
      e_pa_event = PDSM_LITE_PA_EVENT_XTRAT_SESSION_CONTROL;
      break;

    case PDSM_LITE_PA_XTRA_APN_PROFILE:
      e_pa_event = PDSM_LITE_PA_EVENT_XTRA_APN_PROFILE_SELECT;
      break;

      // ----- No more new PA event conversion ----- //
    default:
      break;  // This line shouldn't be executed. Only to resolve warning msg. 
  }

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_pdapi_pa_event_callback () getting called",0,0,0);
  sm_log_pa_event_callback(e_pa_event, p_pa_info_type);
  sm_report_event_gps_pa_event_callback(p_pa_info_type->pa_event);

  pdsmclient_lite_list_pa_event_ntfy(e_pa_event, p_pa_info_type,pa_cmd_err);
}

/*===========================================================================
FUNCTION  tm_pdapi_pa_event_callback

DESCRIPTION
  Sends a PD_EVENT to client and logs the event in DM.

Parameters:

  p_Client -
  e_pd_event: the event to be sent to PDAPI
===========================================================================*/
void tm_pdapi_pa_event_callback_ex( pdsm_lite_pa_info_s_type *p_pa_info_type, pdsm_lite_pa_cmd_err_e_type   pa_cmd_err)
{
  pdsm_lite_pa_event_type e_pa_event = PDSM_LITE_PA_EVENT_NONE;

  if (p_pa_info_type == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  // pa event enum range check
  if ( (p_pa_info_type->pa_event < PDSM_LITE_PA_NONE) || (p_pa_info_type->pa_event >= PDSM_LITE_PA_MAX) )
  {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "TM_PDAPI: PA event %d is out of range. tm_pdapi_pa_event_callback() not called.", p_pa_info_type->pa_event);
    return;
  }

  (void) time_get(p_pa_info_type->time_stamp);


  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_pdapi_pa_event_callback_ex () getting called",0,0,0);
  sm_log_pa_event_callback(e_pa_event, p_pa_info_type);
  sm_report_event_gps_pa_event_callback(p_pa_info_type->pa_event);

  pdsmclient_lite_list_pa_event_ntfy(e_pa_event, p_pa_info_type, pa_cmd_err);
}
/*===========================================================================
FUNCTION  TM_PDAPI_XTRA_EVENT_CALLBACK

DESCRIPTION
  Sends a XTRA event to client and logs the event in DM.

Parameters:

  xtra_event: the event to be sent to PDAPI
===========================================================================*/
void tm_pdapi_xtra_event_callback
(
  pdsm_lite_xtra_event_type   e_xtra_event,
  pdsm_lite_xtra_info_s_type  *p_xtra_info_type
)
{
  /* Check the input parameters */
  if (p_xtra_info_type == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_pdapi_xtra_event_callback () getting called",0,0,0);

  if( e_xtra_event != PDSM_XTRA_EVENT_NONE )
  {
    /* log the event information to diag */
    //sm_log_xtra_event(p_xtra_info_type);

    /* Send the event to gpsOneXTRA client */
    (void)pdsm_lite_xtra_client_request(e_xtra_event, p_xtra_info_type);
  }
} /* End tm_pdapi_xtra_event_callback() */

/*===========================================================================
FUNCTION  TM_PDAPI_XTRA_T_EVENT_CALLBACK

DESCRIPTION
  Sends a XTRA-T event to client and logs the event in DM.

Parameters:

  xtra_t_event: the event to be sent to PDAPI
  xtra_t_session_type:  xtra-t session type
===========================================================================*/
void tm_pdapi_xtra_t_event_callback
(
  pdsm_lite_xtra_t_info_s_type     *xtra_t_info_ptr,
  pdsm_lite_xtra_t_session_e_type   xtra_t_session_type
)
{
  boolean  send_event = TRUE;

  /* Check the input parameters */
  if (xtra_t_info_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI: Null pointer to function ", 0,0,0);
    return;
  }

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_LOW, "TM_PDAPI: tm_pdapi_xtra_t_event_callback () getting called",0,0,0);

  if( xtra_t_info_ptr->xtra_t_event != PDSM_LITE_XTRA_T_EVENT_NONE )
  {
    /* send the DM log information to diag */
#if (0)
    sm_log_xtra_t_event(xtra_t_info_ptr, xtra_t_session_type);
#endif
    /* send the DM event information to diag */
    switch (xtra_t_info_ptr->xtra_t_event)
    {
      case PDSM_LITE_XTRA_T_EVENT_SESS_BEGIN:
#if (0)
        sm_report_event_gps_xtra_t_session_begin(xtra_t_session_type);
#endif
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"XTRA-T Event: Session BEGIN: %d",xtra_t_session_type,0,0);
        break;

      case PDSM_LITE_XTRA_T_EVENT_DATA:
#if (0)
        sm_report_event_gps_xtra_t_session_data(
          xtra_t_info_ptr->xtra_t_event_payload.data_report.xtra_t_data_len,
          xtra_t_info_ptr->xtra_t_event_payload.data_report.part_number,
          xtra_t_info_ptr->xtra_t_event_payload.data_report.total_parts);
#endif
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"XTRA-T Event: DATA:  len %ld, part %ld, total %ld",
                 xtra_t_info_ptr->xtra_t_event_payload.data_report.xtra_t_data_len,
                 xtra_t_info_ptr->xtra_t_event_payload.data_report.part_number,
                 xtra_t_info_ptr->xtra_t_event_payload.data_report.total_parts);
        break;

      case PDSM_LITE_XTRA_T_EVENT_SESS_DONE:
#if (0)
        sm_report_event_gps_xtra_t_session_done(xtra_t_session_type);
#endif
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"XTRA-T Event: Session DONE: %d",xtra_t_session_type,0,0);
        break;

      case PDSM_LITE_XTRA_T_EVENT_SESS_END:
#if (0)
        sm_report_event_gps_xtra_t_session_end(xtra_t_info_ptr->xtra_t_event_payload.end_reason);
#endif
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"XTRA-T Event: Session END %d",xtra_t_info_ptr->xtra_t_event_payload.end_reason, 0,0);
        break;

      case PDSM_LITE_XTRA_T_EVENT_DOWNLOAD_REQ:    /* Aries 5.0 */
      case PDSM_LITE_XTRA_T_EVENT_UPLOAD_REQ:     /* Aries 5.0 */
      case PDSM_LITE_XTRA_T_EVENT_NONE:
      default:
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_PDAPI_CLIENT: unsupported XTRA-T event",0,0,0);
        send_event = FALSE;
        break;
    }

    /* Send the event to XTRA-T client */
    if (send_event == TRUE)
    {
      (void)pdsm_lite_xtra_t_client_request(xtra_t_info_ptr->xtra_t_event, xtra_t_info_ptr);
    }
  }
} /* End tm_pdapi_xtra_t_event_callback() */
