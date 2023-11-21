/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Position Determination Session Manager Module

GENERAL DESCRIPTION
  This module makes up the Position Determination Session Manager of the subscriber
  station software.

  The Session Manager provides Position Determination session control and event notification
  services to its clients.


EXTERNALIZED FUNCTIONS

  Copyright (c) 2000 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc. 
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.
 
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/pdapi/inc/tm_pdapi_client.h#1 $ $DateTime: 2020/07/21 21:23:44 $ $Author: pwbldsvc $
  
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/17/17   skm     BLE and Barometer support for LPPE Phase 2 
03/14/12   ss      Added support to send error in PA callback
09/03/12   rh      Added support for Cell Info and NI message injection
04/02/09   ns      Support for Qwip: externed tm_pd_client_cmd_check 
01/02/09   atien   XTRA-T support
01/27/09   ns      Changes for qWip integration.
04/28/08   gk      On-Demand changes
08/01/07   rw      Add support for XTRA feature
05/27/07   rw      Add definition of range check for URL parameters for JGPS
4/03/07    gk      PC Time injection handling, modifing PD_EVENT call back handling, missing cmd callbacks  
12/06/06   gk      modified pa_event call back API 
12/04/06   gk      Updated LCS response, minor mods for tbf and num_fixes
11/8/06    gk      initial checkin for converged GPS
#########################################################################################
*/

#ifndef TM_PDAPI_CLIENT_H
#define TM_PDAPI_CLIENT_H

#include "pdapi_lite.h"      /* External interface to pd.c */
#include "pdapiclient_lite.h"  /* Interface to this file */
#include "aries_os_api.h"
#include "sm_dm_event.h"
#include "sm_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  TM_PDAPI_SESS_TYPE_NONE = -1,
  TM_PDAPI_SESS_TYPE_MI,
  TM_PDAPI_SESS_TYPE_MT
} tm_pdapi_sess_type;

/*===========================================================================

FUNCTION  tm_pdapi_cmd_proc

DESCRIPTION 
  Handles an incoming PDAPI cmd

RETURN VALUE 
  None

DEPENDENCIES

===========================================================================*/

extern void tm_pdapi_cmd_proc( void* p_Data );

/*===========================================================================

FUNCTION tm_pd_client_cmd_status

DESCRIPTION
  

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

extern void tm_pd_client_cmd_status(
  pdsm_lite_cmd_s_type        *p_cmd_type,
      /* Pointer to PDSM command */
  pdsm_lite_pd_cmd_err_e_type e_cmd_err
     /* PDSM call command error */
  );

/*===========================================================================

FUNCTION TM_PD_CLIENT_CMD_PROC

DESCRIPTION
  This function is used handle client commands to PDSM.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
extern void tm_pd_client_cmd_proc(
  pdsm_lite_cmd_s_type *p_CmdType
          /* Pointer to PDSM command */
 );
 
/*===========================================================================

FUNCTION TM_PA_CMD_PROC

DESCRIPTION
   This function is used to process PA related commands.

RETURN VALUE
  None

DEPENDENCIES


===========================================================================*/
 
extern void tm_pa_cmd_proc(
   pdsm_lite_cmd_s_type *p_CmdType
        /* Pointer to PDSM command */
);

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
);


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
  pdsm_lite_cmd_s_type *p_cmd_type
     /* Pointer to PDSM command */
);

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
  pdsm_lite_cmd_s_type *p_cmd_type
     /* Pointer to PDSM command */
);
/*===========================================================================
FUNCTION  TM_PDAPI_PD_POS EVENT_CALLBACK 

DESCRIPTION
  Sends a PD_EVENT POSITION event to client

Parameters:

  pd_event: the event to be sent to PDAPI
===========================================================================*/
void tm_pdapi_pos_event_callback( pdsm_lite_pd_event_type   e_pd_event,
                                            pdsm_lite_pd_info_s_type* p_pd_info_type,
                                           tm_pdapi_sess_type   sess_type
                                );

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
                                );

/*===========================================================================
FUNCTION  TM_PDAPI_PD_EVENT_CALLBACK 

DESCRIPTION
  Sends a PD_EVENT to client and logs the event in DM.

Parameters:

  pd_event: the event to be sent to PDAPI
===========================================================================*/
extern void tm_pdapi_pd_event_callback ( tm_pdapi_pd_event_s_type *p_pd_info_type,
                                         tm_pdapi_sess_type  sess_type                                  
                                         );
                                      
/*===========================================================================
FUNCTION  TM_PDAPI_PA_EVENT_CALLBACK 

DESCRIPTION
  Sends a PD_EVENT to client and logs the event in DM.
 
Parameters:
  
  pd_event: the event to be sent to PDAPI
===========================================================================*/

extern void tm_pdapi_pa_event_callback( pdsm_lite_pa_info_s_type *p_pa_info_type,pdsm_lite_pa_cmd_err_e_type pa_cmd_err);

/*===========================================================================
FUNCTION  TM_PDAPI_PA_EVENT_CALLBACK_EX

DESCRIPTION
  Sends a PDA_EVENT to client and logs the event in DM.

Parameters:

  pa_event: the event to be sent to PDAPI
  pa_cmd_err: Any error to be sent to PDAPI corresponding to PA event
===========================================================================*/

extern void tm_pdapi_pa_event_callback_ex( pdsm_lite_pa_info_s_type *p_pa_info_type, pdsm_lite_pa_cmd_err_e_type   pa_cmd_err);
                                      



/*===========================================================================
FUNCTION  TM_PDAPI_XTRA_EVENT_CALLBACK

DESCRIPTION
  Sends a XTRA event to client and logs the event in DM.

Parameters:

  xtra_event: the event to be sent to PDAPI
===========================================================================*/
extern void tm_pdapi_xtra_event_callback
( 
  pdsm_lite_xtra_event_type   e_xtra_event,
  pdsm_lite_xtra_info_s_type  *p_xtra_info_type
);

/*===========================================================================
FUNCTION  TM_PDAPI_XTRA_T_EVENT_CALLBACK

DESCRIPTION
  Sends a XTRA-T event to client and logs the event in DM.

Parameters:

  xtra_t_session_type:  xtra-t session type 
===========================================================================*/
extern void tm_pdapi_xtra_t_event_callback
( 
  pdsm_lite_xtra_t_info_s_type     *xtra_t_info_ptr,
  pdsm_lite_xtra_t_session_e_type   xtra_t_session_type
);

/*===========================================================================

FUNCTION TM_PD_CLIENT_CMD_CHECK

DESCRIPTION
  Checks for Command parameter errors and PDSM state.

RETURN VALUE
  None

DEPENDENCIES
  Called by registered clients.

===========================================================================*/

pdsm_lite_pd_cmd_err_e_type tm_pd_client_cmd_check( pdsm_lite_cmd_s_type *p_cmd_type );


#ifdef __cplusplus
}
#endif

#endif

 
