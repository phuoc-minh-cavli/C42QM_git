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

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/common/src/pdapi.c#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/01/20   ska     Changes done for MGP state to OFF when TBF>=5sec
02/17/17   skm     BLE and Barometer support for LPPE Phase 2 
01/09/17   rk      Fix a bug in processing zero wlan-ap measurements.
10/27/16   rk      Allow wlan-ap measurements with zero number of APs in error case.
05/01/15   rh      Added get gps lock synchrous API for LBS
02/24/15   ssu     Adding GM in memory logging.
09/05/14   ssu     GM LOWI Integration
03/10/14   ah      Added support for Get Avail WWAN Pos
06/26/13   mj      Wrap z_wiper_position_report_ex_struct_data with QWIP featurization
09/03/12   rh      Added support for Cell Info and NI message injection
04/18/12   ssu     Added pdsm_pd_get_best_avail_pos()
03/14/12   ss      Added pdsm_reset_location_service() API
03/02/02   rh      Added pdsm_pd_inject_external_position()
06/02/11   rh      Added pdsm_lcs_set_vx_config() API
05/18/11   rc      Lowering priority of some standalone messages from MSG_HIGH to MSG_LOW
09/30/10   rh      Changed ERR_OTHER to more meaningful error codes
08/18/09   gk      ingore the file name length in the efs data  set params
08/10/09   gk      Added support for Version
03/02/09   jd      Added support for Wiper logging over RPC.
02/06/09   atien   Add missing PDSM_CMD_TYPE_XTRA_T in pdsm_cmd_queue
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
12/07/06   cl      Moved pdsm_pool_init to TM-core
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
#include "pdapi.h"       /* Internal interface to pdsm.c */
#include "pdapidbg.h"    /* Interface to PDSM debug services */
#include "pdapiclient_lite.h" /* Interface to PDSM Client Interface Block */
#include "queue.h"      /* Interface to queue services */
#include "pdapibuf.h"
#include "sm_api.h"
#include "aries_os_api.h"
#include "tm_api.h"
#include "gm_api.h"
extern boolean pdsm_lite_cmd_queue( const pdsm_lite_cmd_s_type  *cmd_ptr);
extern boolean pdsm_lite_pd_inject_external_pos( pdsm_lite_pd_cmd_cb_f_type    cmd_cb_func,           /* Command callback */
   void                     *client_data_ptr,      /* user data pointer */
   pdsm_lite_pd_external_pos_info_ex_type    *ext_pos_ptr, /* external position */
   pdsm_lite_client_id_type      client_id              /* Client id of user */);

/* <EJECT> */
/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS

This section contains global definitions for constants, macros, types,
variables

===========================================================================*/

typedef enum
{
  PDSM_ACTIVE_TASK_NULL,

#ifdef FEATURE_GPSONE
  PDSM_ACTIVE_TASK_PDSM,
#endif
  PDSM_ACTIVE_TASK_MAX
} pdsm_active_task_type;

static pdsm_lite_cmd_s_type cmd_data;

/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

pdsm_active_task_type  pdsm_active_task;
/* test_data is used for initializing PE.
*/
/*===========================================================================

FUNCTION PDSM_CMD_QUEUE

DESCRIPTION
  This function is used queue commands to PDSM task.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

boolean pdsm_cmd_queue( const pdsm_lite_cmd_s_type  *cmd_ptr)
{
  return pdsm_lite_cmd_queue(cmd_ptr);
}
 /* End pdsm_cmd_queue( ) */


/*******************************************************************************
********************************************************************************
                          User Invoked Commands.
********************************************************************************
********************************************************************************/


/*===========================================================================

FUNCTION PDSM_GET_POSITION

DESCRIPTION
  This function is called by different clients to obtain Position Location
  Information.

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

boolean pdsm_get_position(
   pdsm_pd_cmd_cb_f_type    cmd_cb_func,         /* Command callback */
  /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,   /* user data pointer */
   pdsm_pd_option_s_type    *option_ptr,        /* PD option type */
  /*~ PARAM option_ptr POINTER */
   pdsm_pd_qos_type         *qos_ptr,           /* Ptr to user QOS for this request */
  /*~ PARAM qos_ptr POINTER */
   pdsm_client_id_type      client_id           /* Client id of user */
)
{
 return  pdsm_lite_get_position(cmd_cb_func,
   client_data_ptr,   
   option_ptr,        
   qos_ptr,client_id    );
} /* End pdsm_get_position() */

/*===========================================================================

FUNCTION PDSM_END_SESSION

DESCRIPTION
  This function is used to end a PD session

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

boolean pdsm_end_session
(
  pdsm_lite_pd_cmd_cb_f_type      pd_cb_func,    /* Command callback */
  /*~ PARAM pd_cb_func POINTER */
  void               *client_data_ptr,  /* Client specified ptr */
  pdsm_lite_client_id_type        client_id      /* requesting client ID */
)

{
  pdsm_lite_pd_end_session_opt_s_type end_session_opt;
  end_session_opt.receiver_off = FALSE;
  return pdsm_lite_end_session(pd_cb_func,client_data_ptr,&end_session_opt,client_id);
}
 /* End pdsm_end_session( ) */



/*===========================================================================

FUNCTION pdsm_pd_inject_external_time

DESCRIPTION
  Command PDSM to inject time information from an external clock.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  pdsm_init( ) must have been already done.

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
boolean pdsm_pd_inject_external_time(
   pdsm_pd_cmd_cb_f_type    cmd_cb_func,         /* Command callback */
   void                     *client_data_ptr,   /* user data pointer */
   pdsm_pd_external_time_info_type    *ext_time_ptr,        /* external time */
   pdsm_client_id_type      client_id           /* Client id of user */
)
{
  return pdsm_lite_pd_inject_external_time(cmd_cb_func,client_data_ptr,ext_time_ptr,client_id);
}

/*===========================================================================

FUNCTION pdsm_pd_inject_external_pos

DESCRIPTION
  !! DEPRECATED API !!, use pdsm_pd_inject_external_position() instead.
  Command PDSM to inject coarse position information from an external source.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  pdsm_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
boolean pdsm_pd_inject_external_pos(
   pdsm_pd_cmd_cb_f_type    cmd_cb_func,           /* Command callback */
   void                     *client_data_ptr,      /* user data pointer */
   pdsm_pd_external_pos_info_ex_type    *ext_pos_ptr, /* external position */
   pdsm_client_id_type      client_id              /* Client id of user */
)
{
  return pdsm_lite_pd_inject_external_pos(cmd_cb_func,client_data_ptr,ext_pos_ptr,client_id);
}


/*===========================================================================
DESCRIPTION
  Command PDSM to inject coarse position information from an external source
  with reliability associated with the injected position.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
DEPENDENCIES
  pdsm_client_init( ) must have been already done.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM event.

===========================================================================*/
boolean pdsm_pd_inject_external_position(
   pdsm_pd_cmd_cb_f_type    cmd_cb_func,           /* Command callback */
   /*~ PARAM pd_cb_func POINTER */
   void                     *client_data_ptr,      /* user data pointer */
   pdsm_pd_external_position_info_type  *ext_pos_ptr, /* external position */
   /*~ PARAM ext_pos_ptr POINTER */
   pdsm_client_id_type      client_id              /* Client id of user */
)
{
  return pdsm_lite_pd_inject_external_position(cmd_cb_func,client_data_ptr,ext_pos_ptr,client_id);
}



/*===========================================================================
FUNCTION pdsm_pd_query_gps_state
DESCRIPTION
  Command PDSM to return the state information of the GPS engine.
  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.
DEPENDENCIES
  pdsm_init( ) must have been already done.
RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request
SIDE EFFECTS
  If command is successful, PDSM clients will be notified of the
  corresponding PDSM extended status events.
===========================================================================*/
boolean pdsm_pd_query_gps_state(
   pdsm_pd_cmd_cb_f_type     cmd_cb_func,       /* Command callback */
   /*~ PARAM cmd_cb_func POINTER */
   void                      *client_data_ptr,  /* Not sure if this is required */
   pdsm_client_id_type       client_id          /* Requesting client id */
)
{
  return pdsm_lite_pd_query_gps_state(cmd_cb_func,client_data_ptr,client_id);
}

/*===========================================================================

FUNCTION PDSM_SET_PARAMETERS

DESCRIPTION
  This function is used to set PDSM Phone parameters.

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

boolean pdsm_set_parameters
(
  pdsm_pa_cmd_cb_f_type    cmd_cb_func,        /* Command callback fn */
  void                     *client_data_ptr,   /* Client provided data pointer */
  pdsm_pa_e_type           pa_set,             /* Type of parameter to Set */
  pdsm_pa_info_type         *pa_ptr,           /* Pointer to Parameter values to bet set to */

  pdsm_client_id_type      client_id
)
{
  return  pdsm_lite_set_parameters(cmd_cb_func,client_data_ptr,pa_set,pa_ptr,client_id);
}
 /* End pdsm_set_parameters( ) */

/*===========================================================================

FUNCTION PDSM_GET_PARAMETERS

DESCRIPTION
  This function is used to set PDSM Phone parameters.

RETURN VALUE
  TRUE - Command Accepted; FALSE - Command Rejected due to an error 

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

boolean pdsm_get_parameters (
    pdsm_pa_cmd_cb_f_type    cmd_cb_func,        /* Command callback fn */
    void                     *client_data_ptr,   /* Client provided data pointer */
    pdsm_pa_e_type           pa_get,             /* Type of parameter to Get */
    pdsm_client_id_type      client_id
)
{
  return pdsm_lite_get_parameters(cmd_cb_func,client_data_ptr,pa_get,client_id);
}
 /* End pdsm_get_parameters( ) */

/*===========================================================================

FUNCTION pdsm_get_pdapi_version
DESCRIPTION
  Gets the pdapi version number
DEPENDENCIES

RETURN VALUE
  PDSM Version number as a UINT32

SIDE EFFECTS

===========================================================================*/
uint32 pdsm_get_pdapi_version(void)
{
  return 0;
}


/*===========================================================================

FUNCTION pdsm_xtra_set_data

DESCRIPTION
  gpsOneXTRA client calls this function to send parts of the XTRA file to GPS 
  engine.  
  
DEPENDENCIES

RETURN VALUE
  TRUE - Data part has been accepted
  FALSE - Data part has been rejected

SIDE EFFECTS

===========================================================================*/
boolean pdsm_xtra_set_data
(
  pdsm_xtra_cmd_cb_f_type    xtra_cb_func,      /* Command callback */
  pdsm_client_id_type        client_id,         /* Client id of user */
  void                       *client_data_ptr,  /* user data pointer */
  uint32                     xtra_data_len,     /* Length of XTRA data */
  unsigned char              *xtra_data_ptr,    /* XTRA data */
  uint8                      part_number,       /* data part number */
  uint8                      total_parts,       /* total data parts */
  pdsm_xtra_dc_status_e_type xtra_dc_status     /* status of download */
)
{
 return  pdsm_lite_xtra_set_data(xtra_cb_func,client_id,client_data_ptr,xtra_data_len,xtra_data_ptr,part_number,total_parts,xtra_dc_status);
}
 /* End pdsm_xtra_set_data( ) */


/*===========================================================================

FUNCTION pdsm_xtra_client_initiate_download_request

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
boolean pdsm_xtra_client_initiate_download_request
(
  pdsm_xtra_cmd_cb_f_type   xtra_cb_func,       /* Command callback */
  pdsm_client_id_type       client_id,         /* Client id of user */
  void                      *client_data_ptr   /* user data pointer */
)
{
  return pdsm_xtra_client_initiate_download_request(xtra_cb_func,client_id,client_data_ptr);
}
 /* End pdsm_xtra_client_initiate_download_request( ) */


/*===========================================================================

FUNCTION pdsm_xtra_set_auto_download_params

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
boolean pdsm_xtra_set_auto_download_params
(
  pdsm_xtra_cmd_cb_f_type  xtra_cb_func,         /* Command callback */
  pdsm_client_id_type      client_id,            /* Client id of user */
  void                     *client_data_ptr,     /* user data pointer */
  boolean                  enable_auto_download, /* turn on/off auto download */
  uint16                   download_interval     /* time between downloads */
)
{
  return pdsm_lite_xtra_set_auto_download_params(xtra_cb_func,client_id,client_data_ptr,enable_auto_download,download_interval);
}
 /* End pdsm_xtra_set_auto_download_params( ) */


/*===========================================================================

FUNCTION pdsm_xtra_query_data_validity

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
boolean pdsm_xtra_query_data_validity
(
  pdsm_xtra_cmd_cb_f_type  xtra_cb_func,       /* Command callback */
  pdsm_client_id_type      client_id,          /* Client id of user */
  void                     *client_data_ptr    /* user data pointer */
)
{
  return pdsm_lite_xtra_query_data_validity(xtra_cb_func,client_id,client_data_ptr);
}
 /* End pdsm_xtra_query_data_validity( ) */


/*===========================================================================

FUNCTION pdsm_xtra_inject_time_info

DESCRIPTION
   gpsOneXTRA client calls this function to inject XTRA Time
   
DEPENDENCIES

RETURN VALUE
    TRUE - Time injection success
    FALSE - Time injection failed

SIDE EFFECTS

===========================================================================*/
boolean pdsm_xtra_inject_time_info
(
  pdsm_xtra_cmd_cb_f_type  xtra_cb_func,      /* Command callback */
  pdsm_client_id_type      client_id,         /* Client id of user */
  void                     *client_data_ptr,  /* user data pointer */
  pdsm_xtra_time_info_type *time_info_ptr     /* pointer to time information */
)
{
  return pdsm_lite_xtra_inject_time_info(xtra_cb_func,client_id,client_data_ptr,time_info_ptr);
}
 /* End pdsm_xtra_inject_time_info( ) */
/*===========================================================================

FUNCTION pdsm_xtra_query_xtra_config_info

DESCRIPTION
  gpsOneXTRA client calls this function to query Xtra config info. 
  

DEPENDENCIES

RETURN VALUE
  TRUE - request is a success
  FALSE - request failed
  
SIDE EFFECTS

===========================================================================*/

boolean pdsm_xtra_query_config_info
(
  pdsm_xtra_cmd_cb_f_type  xtra_cb_func,       /* Command callback */
  pdsm_client_id_type      client_id,          /* Client id of user */
  void                     *client_data_ptr,    /* user data pointer */
  pdsm_xtra_config_req_info_type *pXtraCfgReqInfo
)
{
  return pdsm_xtra_query_config_info(xtra_cb_func,client_id,client_data_ptr,pXtraCfgReqInfo);
}
 /* End pdsm_xtra_query_xtra_config_info( ) */
/*===========================================================================

FUNCTION pdsm_xtra_t_get_data

DESCRIPTION
   XTRA-T client calls this function to get CellDB data from GPS Engine.
   
DEPENDENCIES

RETURN VALUE
    TRUE  - XTRA-T get data command success
    FALSE - XTRA-T get data command failure

SIDE EFFECTS

===========================================================================*/
boolean pdsm_xtra_t_get_data
(
  pdsm_xtra_t_cmd_cb_f_type   xtra_t_cb_func,      /* Command callback */
  void                       *client_data_ptr,     /* Not sure if this is required */
  pdsm_xtra_t_get_s_type      xtra_t_get_type,     /* Parameter mask to be set */
  pdsm_client_id_type         client_id            /* Requesting client id */
)
{
  return pdsm_lite_xtra_t_get_data(xtra_t_cb_func,client_data_ptr,xtra_t_get_type,client_id);
}
/* End pdsm_xtra_t_get_data ( ) */
/*===========================================================================

FUNCTION pdsm_pd_get_best_avail_pos

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
boolean pdsm_pd_get_best_avail_pos
(
   pdsm_pd_cmd_cb_f_type cmd_cb_func, /* command result callback function */
   /*~ PARAM pa_motion_data_cb_func POINTER */
   pdsm_client_id_type   client_id,
   void                 *client_data_ptr /* client provided reference data pointer */
)
{
  return pdsm_lite_pd_get_best_avail_pos(cmd_cb_func,client_id,client_data_ptr);
}

/*===========================================================================

FUNCTION pdsm_pd_get_avail_wwan_pos

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
boolean pdsm_pd_get_avail_wwan_pos
(
  pdsm_pd_cmd_cb_f_type   cmd_cb_func, /* command result callback function */
  /*~ PARAM pa_motion_data_cb_func POINTER */
  pdsm_client_id_type     client_id,
  void                   *client_data_ptr /* client provided reference data pointer */
)
{
   return pdsm_lite_pd_get_avail_wwan_pos(cmd_cb_func,client_id,client_data_ptr);
}

boolean pdsm_enable_dpo_status_report(boolean enable)
{
  return pdsm_lite_enable_dpo_status_report(enable);
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
boolean pdsm_pd_inject_external_cell_info(
  pdsm_pd_cmd_cb_f_type    pd_cb_func,          /* Command callback */
  void                     *client_data_ptr,    /* user data pointer */
  pdsm_client_id_type      client_id,           /* Client id of user */
  pdsm_cell_info_s_type    *cell_info           /* Pointer to external cell info */
)
{
  return pdsm_lite_pd_inject_external_cell_info(pd_cb_func,client_data_ptr,client_id,cell_info);
}



/*
******************************************************************************
* pdsm_read_qmi_services_config
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
boolean pdsm_read_qmi_services_config( pdsm_QMIServiceConfig *qmi_Config )
{
  return pdsm_lite_read_qmi_services_config(qmi_Config);
}



/*===========================================================================
FUNCTION pdsm_get_gps_lock_status()

DESCRIPTION
  PDSM client calls this function to get current GPS lock status
 
  This is a synchronous API, will return immediately
 
DEPENDENCIES

RETURN VALUE
  Current GPS lock status
  
SIDE EFFECTS

===========================================================================*/
pdsm_gps_lock_e_type pdsm_get_gps_lock_status(void)
{
  return tm_api_get_gps_lock_status();
}

