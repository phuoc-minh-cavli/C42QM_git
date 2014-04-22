/*======================================================================

               LM DMM main file

 GENERAL DESCRIPTION
  This file contains the implementation of Location Manager(LM) task.
  This task handles requests from various tasks like TM, MGP, GPS DIAG. 

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2020 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/lm/inc/lm_dmm.h#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/18/20   ska     Reducing proc_duration when last fix EPH is valid
12/30/18   AP      Added code for DMM feature support
======================================================================*/
/* DMM_SM code */
#ifndef LM_DMM_H
#define LM_DMM_H

#ifdef FEATURE_DYNAMIC_LOADING_GNSS
#include "marb.h"
#include "dmm.h"
#endif
#include "fs_public.h"

typedef enum
{
  LM_DMM_STATE_UNLOADED,
  LM_DMM_STATE_LOADED,
  LM_DMM_STATE_LOADING,
  LM_DMM_STATE_UNLOADING
} lm_dmm_state_e_type;

typedef enum
{
/*REV 12/05: Rename LM_DMM_EVENT_DMM_OPEN_MODULE to LM_DMM_EVENT_ARB_ACCEPT*/
  LM_DMM_EVENT_MGP_LOAD_REQ,                      /*Request to load MGP is received*/
  LM_DMM_EVENT_ARB_ACCEPT_IND,                    /*Arbitrator accepeted request to load MGP*/
  LM_DMM_EVENT_MGP_LOAD_DONE_IND,                 /*Loading and initilization of MGP is done*/
  LM_DMM_EVENT_GNSS_ACTIVATE_REQ,                 /*Request to grant permission to start GNSS receiver*/
  LM_DMM_EVENT_ABORT_MGP_LOAD_IND,                /*Indication to abort loading of MGP is received*/
  LM_DMM_EVENT_ARB_UNLOAD_IND,                    /*Indication to unload MGP is received*/
  LM_DMM_EVENT_RCVR_OFF_IND,                      /*Indication to notify that receiver is turned off is receiver*/
  LM_DMM_EVENT_MGP_UNLOAD_DONE_IND,               /*MGP unloading is complete*/
  LM_DMM_EVENT_MGP_UNLOAD_FAIL_IND,               /*MGP unload could not be completed sucessfully*/
  LM_DMM_EVENT_ARB_NOTIFY_IND                     /*Indication from Arbitrator to notify completion of backoff period */

} lm_dmm_state_machine_event_e_type;
typedef enum
{
  LM_DMM_EXCEPTION_LOAD_REQ_IN_INCOMPATIBLE_STATE,         /* EXCEPTION when MGP load request comes when it cannot be served*/
  LM_DMM_EXCEPTION_MGP_MODULE_OPEN_FAILURE,      /* EXCEPTION when MGP open fails after receiving ARB load accept*/
  LM_DMM_EXCEPTION_MARB_MGP_LOAD_FAILURE,        /* EXCEPTION when MARB rejects load request*/
  LM_DMM_EXCEPTION_MARB_GNSS_ACTIVATION_REJECT,  /* EXCEPTION when MARB rejects perimission to activate GNSS*/
  LM_DMM_EXCEPTION_MARB_UNKNOWN_RESPONSE,        /* EXCEPTION when MARB sends unknown response*/
  LM_DMM_EXCEPTION_ABORT_LOAD,                   /* EXCEPTION when MGP load doesnot happen on time and load timer times out*/
  LM_DMM_EXCEPTION_LOAD_REQ_OVER_LOADING,        /* EXCEPTION when one more load request is sent when DMM already in LOADING state*/
  LM_DMM_EXCEPTION_UNEXPECTED_EVENT,             /* EXCEPTION when DMM receives an event which is not expected in current DMM state machine */
  LM_DMM_EXCEPTION_TYPE_MAX,
}lm_dmm_exceptions_e_type;
#ifdef FEATURE_DYNAMIC_LOADING_GNSS
typedef struct 
{
  marb_result_enum_t             e_result;
  uint64              min_wait;
  uint64              load_wait;
} Arb_ResultStructType;
#endif
typedef void (lm_dmm_exception_handler_f_type)(
   lm_dmm_exceptions_e_type e_exception
   );

typedef void (lm_dmm_gnss_rsc_grant_handler_f_type)(void);

typedef struct
{
  lm_dmm_state_e_type e_lm_dmm_state;
  os_TimerMsgType* p_DmmArbReqWaitTimer;
  os_TimerMsgType* p_MgpLoadAttemptBackOffTimer;	  /*Timer to track the minimum time till which request to load timer cannot be made*/
  os_TimerMsgType* p_MgpLoadTimer;				  /*Timer to track the maximum time to load MGP */
  os_TimerMsgType* p_MgpUnLoadTimer;				  /*Timer to track the maximum time to unload MGP */
  lm_dmm_exception_handler_f_type *lm_dmm_exception_handler_func;
  lm_dmm_gnss_rsc_grant_handler_f_type * lm_dmm_gnss_rsc_grant_handler_func;
  uint32 u_gnss_proc_duration; 
  uint32 u_gnss_cleanup_time;
}lm_dmm_context_info_s_type;


#define  FILEPATH_GNSS_UNLOAD            "/gps/gnss/unload"   /* EFS file path */
#define  FILEPATH_GNSS_PROC_DURATION     "/gps/gnss/gnssProcDuration"      /* File path for efs file proc_duration  */
#define  FILEPATH_GNSS_CLEANUP_DURATION  "/gps/gnss/gnssCleanupDuration"   /* File path for efs file cleanup_duration */

/*  LM_DMM macro mapping to TM_UTIL macros*/
#define LM_DMM_EXPECTED_SESS_DURATION_SYS_STATUS_VALID_EPH                 6000  /* in milliseconds */
#define LM_DMM_EXPECTED_SESS_DURATION_SYS_STATUS_VALID_ALL                 8000  /* in milliseconds */ 
#define LM_DMM_EXPECTED_SESS_DURATION_SYS_STATUS_VALID_NONE                40000 /* in milliseconds */
#define LM_DMM_EXPECTED_SESS_DURATION_SYS_STATUS_VALID_ONLY_XTRA           25000 /* in milliseconds */

#define LM_DMM_GNSS_CLEANUP_TIME       350  /* in milliseconds */
#define LM_DMM_LOAD_WAIT_GAURD_TIME    5000
#define LM_DMM_MGP_LOAD_TIME           4000
#define LM_DMM_MGP_UNLOAD_TIME         4000
#define LM_DMM_GNSS_PROC_DURATION      LM_DMM_EXPECTED_SESS_DURATION_SYS_STATUS_VALID_ALL  


#ifdef FEATURE_DYNAMIC_LOADING_GNSS 
/*
 ******************************************************************************
 * lm_dmm_marb_deregister
 *
 * Function description:
 *
 *  DeRegisters LM DMM as a client for MARB
 *   
 *
 * Parameters: 
 *
 * Return value: 
 ******************************************************************************
*/
void lm_dmm_marb_deregister(void);

/*
 ******************************************************************************
 * lm_dmm_marb_deregister
 *
 * Function description:
 *
 *  Registers LM DMM as a client for MARB
 *   
 *
 * Parameters: 
 *
 * Return value: 
 ******************************************************************************
*/

void lm_dmm_marb_register(void);
/*
 ******************************************************************************
 * lm_dmm_marb_cb
 *
 * Function description:
 *
 *  Callback called by MARB to provide inputs to DMM 
 * 
 *
 * Parameters: 
 *   marb_async_callback_input_t *input
 * Return value: 
 *
 ******************************************************************************
*/
void lm_dmm_marb_cb(marb_async_callback_input_t *input);
#endif

/*
 ******************************************************************************
 * lm_dmm_state_machine
 *
 * Function description:
 *
 *  Implementation of DMM state machine 
 *
 * Parameters: 
 *   lm_dmm_state_machine_event_e_type event
 * Return value: 
 *   lm_dmm_state_e_type : state of DMM state machine
 ******************************************************************************
*/

lm_dmm_state_e_type lm_dmm_state_machine(lm_dmm_state_machine_event_e_type event);

#ifdef FEATURE_DYNAMIC_LOADING_GNSS
/*
 ******************************************************************************
 * lm_dmm_mgp_load_backoff_timer_timeout
 *
 * Function description:
 *
 *  Handles load backoff timer time out
 * 
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
*/

void lm_dmm_mgp_load_backoff_timer_timeout(void);

/*
 ******************************************************************************
 * lm_dmm_load_timeout
 *
 * Function description:
 *
 *  Handles load timer timeout
 * 
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
*/

void lm_dmm_load_timeout(void);

/*
 ******************************************************************************
 * lm_dmm_unlLoad_timeout
 *
 * Function description:
 *
 *  Handles unload timer timeout
 * 
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
*/

void lm_dmm_unlLoad_timeout(void);
#endif
/*
 ******************************************************************************
 * lm_dmm_init
 *
 * Function description:
 *
 * Initializes the variables in z_lm_dmm_context_info structure
 *
 * Parameters: 
 *
 * Return value: 
 *
 ******************************************************************************
*/

void lm_dmm_init(void);

/*
 ******************************************************************************
 * lm_dmm_register_cb
 *
 * Function description:
 *
 *  Registers callback to function that handles exceptions thrown by 
 *  DMM state machine.
 *
 * Parameters: 
 *   lm_dmm_exception_handler_f_type *: pointer to function 
 *                                              that handles exceptions
 *                                              thrown by DMM state machine
 * Return value: 
 *
 ******************************************************************************
*/

void lm_dmm_register_cb(lm_dmm_exception_handler_f_type *p_lm_dmm_exception_handler_func, 
              lm_dmm_gnss_rsc_grant_handler_f_type *p_lm_dmm_gnss_rsc_grant_handler_func);

void lm_dmm_initialize_mgp();

#endif
