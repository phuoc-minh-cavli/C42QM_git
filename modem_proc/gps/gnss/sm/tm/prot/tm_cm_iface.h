#ifndef TM_CM_IFACE_H
#define TM_CM_IFACE_H


/*===========================================================================
  
                            TM_CM_IFACE  h e a d e r
                   
DESCRIPTION

  
Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
Copyright (c) 2013 - 2019 Qualcomm Technologies, Inc
Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/prot/tm_cm_iface.h#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
11/07/17    kpn     L+L: Handling 3rd party E911 call
08/01/17    kpn     L+L: Use AS_ID while calling tm_cm_process_ext_emergency_enter()
06/07/17    kpn     L+L: Use Srv Sys mode while handling E911 Orig call event
04/10/17    kpn     L+L: New macro definition to get AS ID as per the index value
04/10/17    kpn     L+L: API to get number of configured Subs
02/21/17    kpn     Support of 1x CP on Sub2
01/24/17    kpn     Support E911 CP session on non-DD sub
11/16/16    mj      E911 and apptracking concurrency support
03/11/16    mj      Changes to check as_sys_mode from cell info first for CELL DCH request to CM
10/19/15    mj      Add changes to provide srv_status of selected sys_mode through phone status
07/20/15    mj      Add support for MultiMode config for MultiSim devices
06/24/15    rh      Support Cell ID update from CM  
04/15/15    mj     Add a new bool to tm_cm_stack_ss_info_s_type to indicate a CM transistion from operational to non-operational
03/18/15    sj      Add sys mode to call events 
06/04/14    mj      Handle MultiSIM subscription feature SRLTE
01/24/14    mj      Remove featurization FEATURE_DUAL_SIM to fix SGLTE featured out issues
10/25/13    mj      Add definitions to support policy manager changes that retrieve device mode
09/27/13    mj      TSTS Support: Define variables for third SUB
01/04/12    ssu     Obtaining th CELL id information in CELL_DCH from CM APIs 
06/23/11    rh      Added cdma_mt_end_call_immed parameter for CR 280070
01/20/11    rh      Fix for CR 258513
03/03/10    ns      DSDS Support
04/02/09    ns      support for Qwip
04/24/07    ank     Added support for CELL_ID_DB feature.
11/30/06    ank     Initial Release
===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"

#if !defined (WIN32) && !defined (FEATURE_CGPS_PLAYBACK)
#include "queue.h"
#include "task.h"
#endif


#include "aries_os_api.h"
#include "cm.h"

#ifndef FEATURE_GNSS_LEAN_BUILD
#include "cm_v.h"
#endif

#include "tm_prtl_iface.h"
#include "tm_common.h"

#ifndef FEATURE_GNSS_LEAN_BUILD
#include "cm_dualsim.h"
#endif

#include "sys.h"

#include "gps_common.h"

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                               DEFINES                                   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define AS_ID(index) SYS_MODEM_AS_ID_1

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                              DATA TYPES                                 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*---------------------------------
  Possible states that the phone call could be in.
 *-------------------------------*/

typedef struct
{
  cgps_SrvSystemType srv_system;
  sys_srv_status_e_type srv_status;
} tm_cm_phone_state_info_s_type;

typedef struct {
  cm_ph_state_e_type           ph_state;
  boolean                      lpm_on;
  boolean                      edrx_enabled;
  uint8                        edrx_cycle_length;
} tm_cm_ph_info_s_type;


typedef union 
{
  cm_ph_event_e_type     ph_event;
  cm_dbm_event_e_type    dbm_event;
  cm_ss_event_e_type     ss_event;
} tm_cm_event_cb_u_type;


typedef struct {
  uint64                     changed_fields;
  sys_srv_status_e_type      srv_status;
  sys_srv_domain_e_type      srv_domain;
  sys_sys_mode_e_type        sys_mode;

  /* Mode info*/
  sys_mm_information_s_type  mm_information;

  /*Roaming info*/
  sys_roam_status_e_type     roam_status;
  
  /*Sys id info*/
  sys_sys_id_s_type          sys_id;
  sys_plmn_id_s_type         sys_id_id_plmn;
    
  /*Cell info*/
  sys_cell_info_s_type       cell_info;

#ifdef FEATURE_CGPS_TLE_IOT
  /*CIOT info*/
  sys_sys_mode_e_type        ciot_lte_mode;
#endif /* FEATURE_CGPS_TLE_IOT */
} tm_cm_mm_ss_stack_info_s_type;

typedef struct {
  uint64                              changed_fields;
  tm_cm_mm_ss_stack_info_s_type      stack_info;
} tm_cm_mm_ss_info_s_type;

typedef union
{
  tm_cm_ph_info_s_type      ph_info;
  tm_cm_mm_ss_info_s_type   ss_info;
} tm_cm_event_cb_info_u_type;

typedef struct
{
  tm_cm_event_cb_u_type      cm_event_type;
  tm_cm_event_cb_info_u_type cm_event_info;
} tm_cm_event_cb_info_s_type;

/* Data structures needed for Event CB */
typedef enum
{
  TM_CM_PH_EVENT,
  TM_CM_DBM_EVENT,
  TM_CM_PD_EVENT,
  TM_CM_SS_EVENT,
} tm_cm_event_cb_e_type;
  
typedef enum
{
  TM_CM_PH_CMD,
  TM_CM_DBM_CMD,
  TM_CM_PD_CMD,
  TM_CM_SS_CMD
} tm_cm_cmd_cb_e_type;


typedef struct 
{
  tm_cm_event_cb_e_type            event_type;
  tm_cm_event_cb_info_s_type       event_info;
} tm_cm_event_info_s_type;

typedef union 
{
  cm_dbm_cmd_e_type    dbm_cmd;
  cm_pd_cmd_e_type     pd_cmd;
  cm_ss_cmd_e_type     ss_cmd;
} tm_cm_cmd_cb_u_type;

typedef union
{
  cm_dbm_cmd_err_e_type        dbm_cmd_err;
  cm_pd_cmd_err_e_type         pd_cmd_err;
  cm_ss_cmd_err_e_type         ss_cmd_err;
} tm_cm_cmd_cb_err_u_type;

typedef struct
{
  tm_cm_cmd_cb_u_type        cm_cmd_type;
  tm_cm_cmd_cb_err_u_type    cm_cmd_err;
  void                       *data_block_ptr;
} tm_cm_cmd_cb_info_s_type;

typedef struct
{
  tm_cm_cmd_cb_e_type            cmd_type;
  tm_cm_cmd_cb_info_s_type       cmd_info;
} tm_cm_cmd_info_s_type;

/* Structure for holding internal information */

typedef struct
{
  sys_sys_mode_e_type        sys_mode;//New Sys Mode for this AS ID
  sys_sys_mode_e_type        prev_sys_mode; //Prev Sys Mode for this AS ID
  sys_srv_status_e_type      srv_status; //New SRV Status for this AS ID
  sys_srv_domain_e_type      srv_domain; //CS/PS
  uint64                     changed_fields;
  sys_sys_mode_e_type        as_sys_mode; //sys_mode updated by lower layers
#ifdef FEATURE_CGPS_TLE_IOT
  /*CIOT info*/
  sys_sys_mode_e_type        ciot_lte_mode;
#endif /* FEATURE_CGPS_TLE_IOT */
}tm_cm_stack_ss_info_s_type;

typedef struct
{
  /*---------------------------------
    Call Manager related
   *------------------------------ */

  cm_client_id_type             cm_client;

  /*---------------------------------
    Timers
   *------------------------------ */
//#9x05 check
  os_TimerMsgType*              inactivity_timer;
  boolean                       inactivity_timer_active;

  /*---------------------------------
    Misc.
   *------------------------------ */

  boolean is_init;             /* Has tm_cm_init been called? */

  boolean msg_ack_pending;         /* We are awaiting the status of a msg */
  q_type  msg_waiting_q;       /* Queue for messages waiting to be sent */


  sys_sys_mode_e_type       sys_mode;
  sys_sys_mode_e_type       prev_sys_mode;

  /*Flag to check for RAT changes when UE has Main and GWL stack active in SVLTE type targets.*/
  boolean					prev_gw_mode;
  byte                      p_rev_in_use;

  tm_cm_stack_ss_info_s_type  ss_info;

  tm_cm_stack_ss_info_s_type  external_stack_ss_info;
  
  sys_srv_status_e_type       prev_srv_status; /* Holds the previous serving status of the selected sys_mod */
  sys_srv_status_e_type       srv_status; /* Holds the current serving status of the selected sys_mod */
  uint32                      curr_mcc;   /*MCC of current serving cell*/
} tm_cm_info_s_type;

//#ifdef FEATURE_GPSONE_NEW_L2_ACK
typedef boolean (*tm_cm_pd_resp_is_within_t2m_f_type)(
 byte*  start,
 /* msg buffer pointer */
 uint32 length
 /* msg length */
 );
//#endif /* FEATURE_GPSONE_NEW_L2_ACK */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                              FUNCTION DECLARATIONS                      */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================

FUNCTION tm_cm_iface_get_phone_state

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void tm_cm_iface_get_phone_state(tm_cm_phone_state_info_s_type *phone_state_info_ptr);

/*===========================================================================

FUNCTION tm_cm_client_init

DESCRIPTION
  Initialializes the TM_CM, registers callbacks, enables the auto-answer
  feature. This is a synchronous function.

DEPENDENCIES
  None.

RETURN VALUE
  Any of the status_e enums.

SIDE EFFECTS
  None 

===========================================================================*/
extern boolean tm_cm_client_init(void);

/*===========================================================================

FUNCTION  tm_cm_iface_event_proc

DESCRIPTION 
  Handles an incoming CM event

RETURN VALUE 
  None

DEPENDENCIES

===========================================================================*/
extern void tm_cm_iface_event_proc( void* p_data );

/*===========================================================================

FUNCTION  tm_cm_iface_cmd_proc

DESCRIPTION 
  Handles CM cmd cb.

RETURN VALUE 
  None

DEPENDENCIES

===========================================================================*/
extern void tm_cm_iface_cmd_proc( void* p_data );

/*===========================================================================

FUNCTION tm_cm_get_sys_mode_for_as_id

DESCRIPTION
  This function gets the current serving system mode for the requested sub id

DEPENDENCIES

RETURN VALUE
  sys_sys_mode_e_type: Requested sub's serving system mode

SIDE EFFECTS
  None

===========================================================================*/
extern sys_sys_mode_e_type tm_cm_get_sys_mode_for_as_id(void);

/*===========================================================================

FUNCTION tm_cm_process_external_srv_changed

DESCRIPTION
  This function processes srv changes from external modem

DEPENDENCIES


RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
extern void tm_cm_process_external_srv_changed(const pdsm_lite_cell_info_s_type *external_cell_db_info_ptr);

extern void tm_cm_request_plmn_search(void *p_data);

#endif /* TM_CM_IFACE_H */


