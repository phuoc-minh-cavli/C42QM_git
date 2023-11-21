#ifndef TM_PDAPI_IFACE_H
#define TM_PDAPI_IFACE_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TM - PD API Interface Module

GENERAL DESCRIPTION
  This module handles IS801 specific routines

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/inc/tm_pdapi_iface.h#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/17/17   skm     BLE and Barometer support for LPPE Phase 2 
04/06/15   mj      Modify "tm_core_param_check" to make it a general PARAM check (SET and GET) function 
05/29/12   ss      Removed tm_core_handle_best_avail_pos  
04/18/12   ssu     Added tm_core_handle_best_avail_pos  
01/06/09   atien   Support XTRA-T
06/12/08   lt      Support for external coarse position injection.
01/21/08   cl      Allow overlapping dload and fix (fix and dload) sessions
11/13/07   gk      Add supl certificate injection throuh PDAPI
08/01/07   rw      Change for XTRA feature
04/3 /07   gk      Time injection API and new struct to report PD events
01/31/07   cl      Modified function for LCS handling
12/07/06   cl      Added set/get parameter handling
12/6/06    gk      modified the set param structure name
12/4/06    gk      modified the set param structure name
09/15/06   cl      Initial version

============================================================================*/
#include "gps_variation.h"
#include "pdapi_lite.h"
#include "pdapiclient_lite.h"
#include "sm_nv.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
  MT_NA, /* getpos not through NI client */
  MT_CP, /* getpos through NI client from a control plane protocol */
  MT_UP  /* getpos through NI client from a user plance protocol */
} pdsm_lite_mt_initiator_e_type;  /* this is not an NV_item and not exposed to client */


/* Struct for get pos request*/ 
typedef struct
{
  pdsm_lite_client_type_e_type       client_type;
  pdsm_lite_client_id_type           client_id;
  pdsm_lite_pd_option_s_type         *option_ptr;        /* PD option type */
  pdsm_lite_pd_qos_type              *qos_ptr;           /* Ptr to user QOS for this request */
  pdsm_lite_mt_initiator_e_type      mt_initiator;       /* 0 (not applicable), (1) CP, or (2) UP */
  boolean                       internal_session; /*If the session is internal*/
} tm_pdapi_get_pos_param_s_type;

/* Struct for end session request*/ 
typedef struct
{
  pdsm_lite_client_id_type           client_id;
  boolean 						receiver_off; /*Receiver off flag*/
} tm_pdapi_end_session_param_s_type;

typedef struct
{
  pdsm_lite_client_id_type          client_id;
  pdsm_lite_pa_e_type               e_pa_type;            /* Parameter mask to be set */
  pdsm_lite_pa_info_type            *pa_info_type_ptr;
} tm_pdapi_param_s_type;

typedef struct
{
  pdsm_lite_pd_info_s_type  pd_info_type;
  tm_pdapi_get_pos_param_s_type pd_get_pos_data;
} tm_pdapi_pd_event_payload_s_type;

typedef struct tm_pdapi_pd_event_s
{
  pdsm_lite_pd_event_type               e_pd_event;
  tm_pdapi_pd_event_payload_s_type pd_event_data;
} tm_pdapi_pd_event_s_type;




/*===========================================================================

FUNCTION tm_core_get_pos_para_check

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern pdsm_lite_pd_cmd_err_e_type tm_core_get_pos_para_check
(
  const tm_pdapi_get_pos_param_s_type *get_pos_param_ptr
);

/*===========================================================================

FUNCTION tm_core_resume_get_pos

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_resume_get_pos(void);

/*===========================================================================

FUNCTION tm_core_handle_get_pos

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_handle_get_pos
(
  const tm_pdapi_get_pos_param_s_type *get_pos_param_ptr
);

/*===========================================================================

FUNCTION tm_core_handle_use_pos

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
#ifdef FEATURE_POSITION_SHOPPING
#error code not present
#endif

/*===========================================================================

FUNCTION tmCore_EndSessionParaCheck

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern pdsm_lite_pd_cmd_err_e_type tm_core_end_session_para_check
(
  const tm_pdapi_end_session_param_s_type  *end_session_param_ptr                                   
);


/*===========================================================================

FUNCTION tm_core_handle_end_session

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_handle_end_session
(
  const tm_pdapi_end_session_param_s_type  *end_session_param_ptr                                          
);

/*===========================================================================

FUNCTION tm_core_handle_inject_ext_time

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_handle_inject_ext_time
(
  pdsm_lite_pd_external_time_info_type *ext_time_ptr
);


/*===========================================================================

FUNCTION tm_core_handle_inject_ext_pos

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_handle_inject_ext_pos
(
  pdsm_lite_cmd_s_type *p_cmd_type
);

/*===========================================================================

FUNCTION tm_core_handle_get_gps_state

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_handle_get_gps_state
(
  pdsm_lite_cmd_s_type *p_cmd_type
);










/*===========================================================================

FUNCTION tm_core_param_check

DESCRIPTION
  This function decides if a PA command is allowed or not based on the type of command and 
  if there is an ongoing session in progress 
DEPENDENCIES
 
PARAMETERS
 set_param_ptr: Pointer to SET/GET PARAM command structure
 pa_cmd       : PA command Type

RETURN VALUE
  TRUE: The requested PA command is allowed to execute
  FALSE: The requested PA command is denied
  
SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_param_check(tm_pdapi_param_s_type *set_param_ptr, pdsm_lite_pa_cmd_e_type pa_cmd);

/*===========================================================================

FUNCTION tm_core_handle_set_param

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_handle_set_param(tm_pdapi_param_s_type *set_param_ptr);

/*===========================================================================

FUNCTION tm_core_handle_get_param

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_handle_get_param(tm_pdapi_param_s_type *get_param_ptr);


/*===========================================================================

FUNCTION tm_core_handle_get_param

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern pdsm_lite_xtra_cmd_err_e_type tm_core_xtra_req_check
(
  const pdsm_lite_xtra_cmd_s_type *xtra_clnt_req_ptr
);


/*===========================================================================

FUNCTION tm_core_handle_xtra_client_cmd

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_handle_xtra_client_cmd
(
  pdsm_lite_xtra_cmd_s_type *cmd_ptr
);

/*===========================================================================

FUNCTION tm_core_xtra_t_req_check

DESCRIPTION
   Check the current TM-Core state to determine if XTRA-T request is allowed.
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern pdsm_lite_xtra_t_cmd_err_e_type tm_core_xtra_t_req_check
(
  const pdsm_lite_xtra_t_cmd_s_type *xtra_t_clnt_req_ptr
);


/*===========================================================================

FUNCTION tm_core_handle_xtra_t_client_cmd

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern boolean tm_core_handle_xtra_t_client_cmd
(
  pdsm_lite_xtra_t_cmd_s_type *cmd_ptr
);



#ifdef __cplusplus
}
#endif

#endif /* TM_PDAPI_IFACE_H */
