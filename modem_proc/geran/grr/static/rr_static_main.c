/*!
  @file rr_static_main.c

  @brief
  implementation of statically loaded functions for RRC task.

  @detail

*/

/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/grr/static/rr_static_main.c#44 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
18/05/16   sc      Initial Revision

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include <stdlib.h>
#include "aconfig.h"
#include "task.h"
#include "rcinit.h"

#include "geran_tasks.h"
#include "rr_task.h"
#include "dog_hb_rex.h"
#include "gprs_mem.h"
#include "stringl/stringl.h"
#include "msgr.h"
#include "msgr_rex.h"
#include "ghdi_nvmem.h"
#include "ghdi_exp.h"
#include "ghdi_exp_v.h"
#include "mm_rr.h"
#include "rr_mm_send.h"
#include "rr_l1.h"
#include "rr_l1_send.h"
#include "rr_l1_g.h"
#include "rr_main.h"
#include "geran_nv.h"
#include "rr_static_main.h"
#include "rr_static_general.h"
#include "geran_grr_static_api.h"
#include "rr_main.h"
#include "rr_sim.h"
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
#include "geran_dmm.h"
#endif /* FEATURE_DYNAMIC_LOADING_GERAN */
#include "mm_rr.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/
static boolean initialised = FALSE;
boolean sd_task_has_started = FALSE;

rex_sigs_type       sigs;                     /* RR task REX signals */
rex_crit_sect_type  rr_q_lock_crit_sect;

q_type              rr_cmd_q;                 /* RR task message queue */
q_type              rr_msgr_cmd_q;
q_type              rr_msgr_cmd_free_q;
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
q_type              dyn_pend_q;               /* RR Q to pend messages before module is loaded */
#endif

static msgr_client_t   msgr_client;
static geran_grr_cmd_t rr_msgr_message_pool[RR_MSGR_QUEUE_SIZE];
static dog_report_type  rr_dog_rpt_var = 0;   /* RR Watchdog ID */

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
typedef struct
{
  boolean            dyn_unload_pending;
  rr_cmd_type       *saved_msg_ptr;
  rr_cmd_type       *processed_msg_ptr;
  byte               processed_msg_id;
  boolean            suspend_in_progress;
  boolean            resume_in_progress;
  boolean            proc_cnf_pending;
  proc_type_e        pending_proc;
  boolean            stop_gsm_mode_cnf_pending;
  uint32             rem_drx_time_ms;   //store DMM resumtion timer value recevied in MPH_PROC_IND.
} rr_static_data_t;

rr_static_data_t rr_static_data;

#endif
/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                                FUNCTIONS

===========================================================================*/
/**
  @brief Function called by General Services when gs_send_message() called to send a message to RR task
*/
gs_status_T rr_send_message(void *msg_p)
{
  gs_status_T   gs_status = GS_SUCCESS;
  rr_cmd_type  *cmd_ptr;

  // Allocate a command buffer from heap
  cmd_ptr = (rr_cmd_type *)GPRS_MEM_CALLOC(sizeof(rr_cmd_type));

  if (cmd_ptr)
  {
    word  message_length;
    rr_cmd_type  *head_cmd_ptr = NULL;

    // Get the message length
    GET_IMH_LEN(message_length, msg_p);
    message_length += sizeof(IMH_T);

    // Copy the new message to the allocated command buffer
    //replace memcpy with memscpy
    memscpy((void*)&cmd_ptr->msg,sizeof(cmd_ptr->msg), msg_p, message_length);

    // Ensure the link pointer is initialised
    (void) q_link( (void*)cmd_ptr, &cmd_ptr->link );

    RR_Q_LOCK();

    // If the new message MS_RR_RR, then scan from the head of the queue, and skip any which
    // are also MS_RR_RR. At the first non-MS_RR_RR message, insert the new message
    if (cmd_ptr->msg.message_header.message_set == MS_RR_RR)
    {
      // Get the item at the head of the queue
      head_cmd_ptr = q_check(&rr_cmd_q);

      // If the new message is a RR_IMSG_RR_EVENT_IND, then this is placed ahead of other RR-RR messages
      // (this is to maintain legacy RR-EVENT behaviour)
      if ((cmd_ptr->msg.message_header.message_set == MS_RR_RR) &&
          (cmd_ptr->msg.rr.header.rr_message_set == RR_IMSG) &&
          (cmd_ptr->msg.rr.rr.imsg.header.imh.message_id == RR_IMSG_RR_EVENT_IND) &&
          (cmd_ptr->msg.rr.rr.imsg.rr_event_ind.priority_over_other_imsg))
      {
        // If the current item is RR_IMSG_RR_EVENT_IND, move onto the next one
        while (head_cmd_ptr &&
               ((head_cmd_ptr->msg.message_header.message_set == MS_RR_RR) &&
                (head_cmd_ptr->msg.rr.header.rr_message_set == RR_IMSG) &&
                (head_cmd_ptr->msg.rr.rr.imsg.header.imh.message_id == RR_IMSG_RR_EVENT_IND)))
        {
          head_cmd_ptr = (rr_cmd_type *) q_next(&rr_cmd_q, &head_cmd_ptr->link);
        }
      }
      else
      {
        // If the current item is MS_RR_RR, move onto the next one
        while (head_cmd_ptr && (head_cmd_ptr->msg.message_header.message_set == MS_RR_RR))
        {
          head_cmd_ptr = (rr_cmd_type *) q_next(&rr_cmd_q, &head_cmd_ptr->link);
        }
      }
    }

    // This is the first queue item which is not MS_RR_RR
    if (head_cmd_ptr == NULL)
    {
      // Place the new message on the queue
      q_put(&rr_cmd_q, &cmd_ptr->link);
    }
    else
    {
      // Insert the new message into the queue
      q_insert(
               &rr_cmd_q,
               &cmd_ptr->link,
               &head_cmd_ptr->link);
    }

    RR_Q_UNLOCK();

    /* Set the command queue signal for receive task */
    (void)rex_set_sigs( geran_tcb_read(GERAN_TASK_ID_GRR), RR_CMD_Q_SIG );
  }
  else
  {
    MSG_GERAN_ERROR_0("No available heap memory");
    gs_status = GS_FAILURE;
  }

  return gs_status;
}

/**
  @brief Initialise RR task queue handling; Register with General Services; Create task timers
*/
static void rr_main_task_init(void)
{
  // Clear task REX signals
  (void) rex_clr_sigs(rex_self(), (rex_sigs_type)~0);

  // Initialise task queue & register

  (void) q_init(&rr_cmd_q);

  (void) gs_init(
           GS_QUEUE_RR,
           rex_self(),
           RR_CMD_Q_SIG,
           &rr_cmd_q,
           NULL,
           sizeof(rr_cmd_type),
           RR_MSG_QUEUE_SIZE,
           /* Use the default message handling routine. */
           (gs_get_handler_ptr) NULL);

  (void) gs_init_for_send_only (GS_QUEUE_RR, rr_send_message);

  /* Register with Dog HB */
  rr_dog_rpt_var = dog_hb_register_rex((rex_sigs_type)RR_RPT_TIMER_SIG);
} /* rr_main_task_init */

/**
  @brief Reads a message from the Message Router queue and reformats it
         as an IMH_T compatible message.
*/
void rr_read_msgr_messages(void)
{
  geran_grr_cmd_t *cmd_ptr;

  while ((cmd_ptr = (geran_grr_cmd_t *) q_get(&rr_msgr_cmd_q)) != NULL)
  {
    rr_cmd_bdy_type *rr_cmd_ptr;

    MSG_GERAN_LOW_0("Processing message received via MessageRouter...");

    rr_cmd_ptr = GPRS_MEM_MALLOC(sizeof(rr_cmd_bdy_type));

    if (rr_cmd_ptr != NULL)
    {
      rr_cmd_ptr->message_header.message_set = MS_MSGR_RR;
      rr_cmd_ptr->message_header.message_id = 0;
      rr_cmd_ptr->rr_msgr_msg.message = cmd_ptr->message;

      PUT_IMH_LEN(sizeof(rr_msgr_msg_t) - sizeof(IMH_T), &rr_cmd_ptr->message_header);

      (void) gs_send_message(GS_QUEUE_RR, rr_cmd_ptr, TRUE);

      GPRS_MEM_FREE(rr_cmd_ptr);
    }

    /* Return message to the free queue */
    q_put(&rr_msgr_cmd_free_q, &cmd_ptr->link);
  }
} /* end rr_read_msgr_messages() */

/**
  @brief Initialise Message Router
*/
static void rr_main_msgr_init(void)
{
  errno_enum_type msgr_error;
  msgr_id_t       msgr_id;
  uint32         i;

  (void) q_init(&rr_msgr_cmd_q);
  (void) q_init(&rr_msgr_cmd_free_q);

  for (i = 0; i < RR_MSGR_QUEUE_SIZE; i++)
  {
    q_link_type * link = q_link(&rr_msgr_message_pool[i], &rr_msgr_message_pool[i].link);
    q_put(&rr_msgr_cmd_free_q, link);
  }

  msgr_error = msgr_client_create(&msgr_client);
  ASSERT(msgr_error == E_SUCCESS);
  msgr_error = msgr_client_add_rex_q(
    &msgr_client,                           //!< Message Client Handle
    rex_self(),                             //!< REX TCB pointer
    RR_MSGR_Q_SIG,                          //!< REX TCB pointer
    &rr_msgr_cmd_q,                         //!< queue to place the msg
    &rr_msgr_cmd_free_q,                    //!< queue to get empty buffer
    offsetof(geran_grr_cmd_t, message),     //!< offset to msgr_hdr
    MSGR_NO_OFFSET,                         //!< offset to legacy cmd_type
    sizeof(geran_grr_cmd_t),                //!< max size of msgs
    &msgr_id                                //!< queue identifier
  );
  ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register_range(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    GERAN_GRR_REQ_FIRST + 1,
    GERAN_GRR_REQ_LAST
  );
  ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register_range(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    GERAN_GRR_IND_FIRST + 1,
    GERAN_GRR_IND_LAST
  );
  ASSERT(msgr_error == E_SUCCESS);

#ifdef FEATURE_LTE
  msgr_error = msgr_register(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    LTE_RRC_G_RESEL_FAILED_RSP
  );
  ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    LTE_RRC_G_REDIR_FAILED_RSP
  );
  ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    LTE_RRC_G_ABORT_RESEL_RSP
  );
  ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    LTE_RRC_G_ABORT_REDIR_RSP
  );
  ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    LTE_RRC_G_PLMN_SRCH_RSP
  );
  ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    LTE_RRC_G_ABORT_PLMN_SRCH_RSP
  );
  ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    LTE_RRC_CCO_NACC_COMPLETED_IND
  );
  ASSERT(msgr_error == E_SUCCESS);
#endif /* FEATURE_LTE */

#ifdef FEATURE_GSM_BAND_AVOIDANCE
  msgr_error = msgr_register(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    MCS_CXM_BAND_AVOID_BLIST_GSM1_IND
  );
  ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(
    MSGR_GERAN_GRR,
    &msgr_client,
    msgr_id,
    MCS_CXM_BAND_AVOID_BLIST_GSM2_IND
  );
  ASSERT(msgr_error == E_SUCCESS);
#endif /*FEATURE_GSM_BAND_AVOIDANCE*/

#ifdef FEATURE_LTE
#ifdef FEATURE_GERAN_X2G_ACQ_DB_SCAN
  msgr_error = msgr_register(
      MSGR_GERAN_GRR,
      &msgr_client,
      msgr_id,
      GERAN_GRR_LTE_GERAN_FREQ_INFO_IND
    );
    ASSERT(msgr_error == E_SUCCESS);
#endif /*FEATURE_GERAN_X2G_ACQ_DB_SCAN*/

  msgr_error = msgr_register(
      MSGR_GERAN_GRR,
      &msgr_client,
      msgr_id,
      GERAN_GRR_LTE_GERAN_ACTIVITY_IND
    );
    ASSERT(msgr_error == E_SUCCESS);
#endif /* FEATURE_LTE */
}

/**
  @brief Peform all initialisation required at task-start on static load

  The following are initialised here -
  RR task queues and registration with General Services;
  RR module task-start initialisation
*/
void gsm_rr_fn_init(void)
{
 
  /* Only use this mechanism to perform memory init, hardware init,
     or other startup init to satisfy dependency that follow. Avoid
     all HLOS mechanisms that will block. */

  /* initialise the critical section used for task queue handling */
  RR_Q_LOCK_INIT();

  geran_tcb_set(GERAN_TASK_ID_GRR, rex_self());

  // Create task queue; Register with GS services; Create task timers
  rr_main_task_init();

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
  rr_ms_task_start_init();
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

  rr_timer_init();

  // Read the classmark information so it is cached (the return value is ignored)
  {
    hw_classmark_T hw_classmark;
    (void) ghdi_read_hw_classmark(&hw_classmark);
  }

#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  /* Init global databases which we need to maintain in static context */
  geran_dal_init();
#endif

  // Configure EFS-NV for GERAN
  geran_nv_init();

  rr_public_store_task_start_init();
  rr_init_barred_cell_list();
  rr_clear_forbidden_lai_list_ptr();
  rr_static_general_init();

  // Initialise Message Router
  rr_main_msgr_init();

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
  (void)q_init(&dyn_pend_q);
#else
 geran_grr_init();
#endif /* FEATURE_DYNAMIC_LOADING_GERAN */

  /* Code in this function will add to startup latency. Do not use
     large loops that can be avoided.*/
}

/**
  @brief Wrapper function for q_get() which adds mutex protection using RR_Q_LOCK.

*/
static rr_cmd_type *rr_q_get(void)
{
  rr_cmd_type *cmd_ptr;

  RR_Q_LOCK();
  cmd_ptr = (rr_cmd_type *)q_get(&rr_cmd_q);
  RR_Q_UNLOCK();

  return cmd_ptr;
}

/* mmcp_sd_task_running **************************************************/
/* this function will be provided by MMCP but for now it's declared here */
boolean mmcp_sd_task_running(void)
{
  return sd_task_has_started;
}
/* mmcp_sd_task_running **************************************************/

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
boolean rr_static_get_dyn_unload_pending()
{
  return rr_static_data.dyn_unload_pending;
}

void rr_static_set_dyn_unload_pending(boolean unload_status)
{
  rr_static_data.dyn_unload_pending = unload_status;
}

boolean rr_static_get_suspend_in_progress()
{
  return rr_static_data.suspend_in_progress;
}

boolean rr_static_get_stop_gsm_mode_cnf_pending()
{
  return rr_static_data.stop_gsm_mode_cnf_pending;
}

void rr_static_set_stop_gsm_mode_cnf_pending(boolean status)
{
  rr_static_data.stop_gsm_mode_cnf_pending = status;
}

static void rr_process_dyn_pend_queue(void)
{
  rr_cmd_type *cmd_ptr, *pend_queue_ptr;

  RR_Q_LOCK();  

  pend_queue_ptr = (rr_cmd_type*)q_get(&dyn_pend_q);
  cmd_ptr = (rr_cmd_type*)q_check(&rr_cmd_q);
  
  /*  CR2956894 :we can stop this timer here as GERAN loaded and starting to process pending msg here   *
   *  timer shall be started if PLMN_LIST req is kept in dyn_pend_q 									*
   */  
  rr_stop_unloaded_nas_msg_guard_timer();

  while(pend_queue_ptr != NULL)
  {
    if(cmd_ptr == NULL)
    {
      (void)q_link(pend_queue_ptr, &pend_queue_ptr->link);
      q_put(&rr_cmd_q, &pend_queue_ptr->link);
    }
    else
    {
      q_insert(&rr_cmd_q, &pend_queue_ptr->link, &cmd_ptr->link);
    }
    pend_queue_ptr = (rr_cmd_type*)q_get(&dyn_pend_q);
  }

  RR_Q_UNLOCK();
}

/**
  @brief Function to add messages to Dynamic pending queue when Dynamic loading is
         enabled but rr.so is not loaded yet.
*/
static void rr_send_to_dyn_pend_queue(rr_cmd_type *cmd_ptr)
{
  rr_cmd_type *pend_queue_ptr = (rr_cmd_type*) q_check(&dyn_pend_q);

  MSG_GERAN_HIGH_2("Msg (%d, %d) added to dyn_pend_q",
    cmd_ptr->msg.message_header.message_set, cmd_ptr->msg.message_header.message_id);
  
  if(cmd_ptr->msg.message_header.message_set == MS_MM_RR)
  {
#ifdef FEATURE_AWARE_CT21
    rr_static_debug_log_dyn_pend_q_message(
	  cmd_ptr,
	  pend_queue_ptr,
      cmd_ptr->msg.message_header.message_set,
      (mm_rr_message_id_enum_T)cmd_ptr->msg.message_header.message_id
    );
#endif
  }

  if(cmd_ptr->msg.message_header.message_set == MS_MM_RR && cmd_ptr->msg.message_header.message_id == RR_PLMN_LIST_REQ)
  {
	/*CR2956894 start timer*/
  	rr_start_unloaded_nas_msg_guard_timer(RR_UNLOADED_NAS_MSG_GUARD_TIMER_DURATION);
  }

  if (cmd_ptr->msg.message_header.message_set != MS_RR_RR)
  {
    while(pend_queue_ptr != NULL)
    {
      /* If message already present in Dynamic pending queue, delete the old instance */
      if (cmd_ptr->msg.message_header.message_set == pend_queue_ptr->msg.message_header.message_set)
      {
        if (cmd_ptr->msg.message_header.message_id == pend_queue_ptr->msg.message_header.message_id)
        {
          q_delete(&dyn_pend_q, &pend_queue_ptr->link);
		  
		  if((rr_static_data.saved_msg_ptr != NULL) && (cmd_ptr->msg.message_header.message_id == rr_static_data.saved_msg_ptr->msg.mm.header.message_id) &&
		     (cmd_ptr->msg.message_header.message_set == rr_static_data.saved_msg_ptr->msg.mm.header.message_set))
		  {
			MSG_GERAN_HIGH_0("Incoming NAS message same as in saved msg queue, update saved msg pointer");
            rr_static_data.saved_msg_ptr = cmd_ptr;
#ifdef FEATURE_AWARE_CT21
            rr_static_log_added_saved_q_message(rr_static_data.saved_msg_ptr,
              rr_static_data.saved_msg_ptr->msg.mm.header.message_set,
              rr_static_data.saved_msg_ptr->msg.mm.header.message_id);
#endif
		  }
		  GPRS_MEM_FREE(pend_queue_ptr);
          break;
        }
      }
      pend_queue_ptr = (rr_cmd_type*) q_next(&dyn_pend_q, &pend_queue_ptr->link);  
    }
  }

  /* Put recently received message in Dynamic pending Queue */
  (void)q_link(cmd_ptr, &cmd_ptr->link);
  q_put(&dyn_pend_q, &cmd_ptr->link);

  return;
}

void rr_static_resume_completed()
{
  rr_static_data.resume_in_progress = FALSE;
  rr_clear_static_backup();
  rr_process_dyn_pend_queue();
}

static boolean rr_static_reject_mm_message(rr_cmd_type *msg)
{
  boolean rejected = FALSE;

  switch (msg->msg.message_header.message_id)
  {
    case RR_PLMN_SELECT_REQ:
    {
      if (is_geran_loaded())
      {
        rr_send_plmn_select_cnf_no_service(MM_AS_ABORTED);
      }
      else
      {
        rr_plmn_select_cnf_T rr_plmn_select_cnf;
    
        MSG_GERAN_HIGH_0("RR_PLMN_SELECT_CNF sent");
        RR_PREPARE_MM_MESSAGE(rr_plmn_select_cnf, RR_PLMN_SELECT_CNF);
        rr_plmn_select_cnf.error_code = MM_AS_ABORTED;
        rr_plmn_select_cnf.service_state = MM_AS_NO_SERVICE;
        (void)gs_send_message(GS_QUEUE_MM, (void*)&rr_plmn_select_cnf, TRUE);
#ifdef FEATURE_AWARE_CT21
		rr_debug_log_mm_rr_message(RR_PLMN_SELECT_CNF,0);
#endif
      }

      rejected = TRUE;
      break;
    }
    
    case RR_PLMN_LIST_REQ:
    {
      rr_PLMN_list_req_info_T *plmn_list_req = &msg->msg.rr_plmn_list_req;

      if (is_geran_loaded())
      {
        rr_send_plmn_list_cnf(NULL, MM_AS_ABORTED, plmn_list_req);
      }
      else
      {
        rr_plmn_list_cnf_T rr_plmn_list_cnf;
  
        MSG_GERAN_HIGH_0("RR_PLMN_LIST_CNF sent");
        RR_PREPARE_MM_MESSAGE(rr_plmn_list_cnf, RR_PLMN_LIST_CNF);
        rr_plmn_list_cnf.status = MM_AS_ABORTED;
        rr_plmn_list_cnf.available_plmn_list.plmn_list.length = 0;
        rr_plmn_list_cnf.trans_id = plmn_list_req->trans_id;
        (void)gs_send_message(GS_QUEUE_MM, (void*)&rr_plmn_list_cnf, TRUE);	
#ifdef FEATURE_AWARE_CT21
	    rr_debug_log_mm_rr_message(RR_PLMN_LIST_CNF,0);
#endif 
      }
      rejected = TRUE;
      break;
    }
    
    case RR_EST_REQ:
    {
      rr_abort_ind_T rr_abort_ind;
    
      MSG_GERAN_HIGH_0("RR_ABORT_IND sent");
      RR_PREPARE_MM_MESSAGE(rr_abort_ind, RR_ABORT_IND);
      rr_abort_ind.RR_abort_cause = OTHER_REASON;
      rr_abort_ind.failure_cause  = MM_RR_NO_RESOURCE_AVAILABLE;
      (void)gs_send_message(GS_QUEUE_MM, (void*)&rr_abort_ind, TRUE);
#ifdef FEATURE_AWARE_CT21
	  rr_debug_log_mm_rr_message(RR_ABORT_IND,0);
#endif
      rejected = TRUE;
      break;
    }
    
    case RR_STOP_GSM_MODE_REQ:
    case RR_DEACT_REQ:
    {
      /* STOP/DEACT message cannot be rejected. 
       * So, add it back to the normal queue, so that it will get processed again */
      rr_cmd_type *norm_cmd_ptr = q_check(&rr_cmd_q);
  
      MSG_GERAN_HIGH_0("STOP_GSM_MODE_REQ/DEACT_REQ moved to rr_cmd_q");
      if (norm_cmd_ptr == NULL)
      {
        q_put(&rr_cmd_q, &msg->link);
      }
      else
      {
        q_insert(&rr_cmd_q, &msg->link, &norm_cmd_ptr->link);
      }

      break;
    }

    default: break;
  }

  return rejected;
}

void rr_static_invalidate_saved_msg(void)
{
  if (rr_static_data.saved_msg_ptr)
  {
    MSG_GERAN_HIGH_1("Discarded saved MM msg %d", rr_static_data.saved_msg_ptr->msg.message_header.message_id);

    /* Remove the message from pending queue and send CNF to NAS */
    q_delete(&dyn_pend_q, &rr_static_data.saved_msg_ptr->link);
    if (rr_static_reject_mm_message(rr_static_data.saved_msg_ptr))
    {
#ifdef FEATURE_AWARE_CT21
      rr_static_log_freed_saved_q_message(
	  	rr_static_data.saved_msg_ptr,
	  	rr_static_data.saved_msg_ptr->msg.message_header.message_set,
	  	rr_static_data.saved_msg_ptr->msg.message_header.message_id
	  	);
#endif
      GPRS_MEM_FREE(rr_static_data.saved_msg_ptr);

    }
  }
  
  rr_static_data.proc_cnf_pending = FALSE;
  rr_static_data.saved_msg_ptr = NULL;
}

void rr_static_send_proc_req(proc_type_e proc, rr_cmd_type *cmd_ptr)
{
  if (cmd_ptr != NULL && 
      cmd_ptr == rr_static_data.processed_msg_ptr && 
      cmd_ptr->msg.message_header.message_id == rr_static_data.processed_msg_id)
  {
    MSG_GERAN_HIGH_0("PROC_REQ not sent as msg already processed");
  }
  else
  {
    rr_static_data.proc_cnf_pending = TRUE;
    rr_static_data.pending_proc = proc;
	
	if (cmd_ptr != NULL && 
		cmd_ptr->msg.mid_timer_expiry.timer_id == RR_DMM_RESUMPTION_TIMER && 
		rr_timer_validate_expiry_message(RR_DMM_RESUMPTION_TIMER))
	{ 
	  rr_static_data.resume_in_progress = TRUE;
	  MSG_GERAN_HIGH_1("After RR_DMM_RESUMPTION_TIMER expiry, resume_in_progress = %d", rr_static_data.resume_in_progress);
	}
	else
	{
	  rr_static_data.saved_msg_ptr = cmd_ptr;
	}

	if(rr_static_data.saved_msg_ptr != NULL)
	{
#ifdef FEATURE_AWARE_CT21
	  rr_static_log_added_saved_q_message(rr_static_data.saved_msg_ptr,
        rr_static_data.saved_msg_ptr->msg.mm.header.message_set,
        rr_static_data.saved_msg_ptr->msg.mm.header.message_id);
#endif
	}
	else
	{
      MSG_GERAN_HIGH_0("saved_msg_ptr is NULL, means PROC_STOP or PROC_NULL being sent to L1");
	}
#ifdef FEATURE_AWARE_CT21
/* Using pended count to log proc value */
	rr_debug_log_rr_l1_message(MPH_PROC_REQ, proc);
#endif
    rr_send_mph_proc_req(proc);
  }
}

static void rr_static_send_deact_cnf(void)
{
  rr_deact_cnf_T rr_deact_cnf;
  RR_PREPARE_MM_MESSAGE(rr_deact_cnf, RR_DEACT_CNF);
  (void)gs_send_message(GS_QUEUE_MM, (void*)&rr_deact_cnf, TRUE);
  MSG_GERAN_HIGH_0("RR_DEACT_CNF sent");
#ifdef FEATURE_AWARE_CT21
  rr_debug_log_mm_rr_message(RR_DEACT_CNF,0);
#endif
}

static void rr_static_send_stop_gsm_mode_cnf(void)
{
  rr_stop_gsm_mode_cnf_T rr_stop_gsm_mode_cnf;
  RR_PREPARE_MM_MESSAGE(rr_stop_gsm_mode_cnf, RR_STOP_GSM_MODE_CNF);
  rr_stop_gsm_mode_cnf.status = MM_AS_SUCCESS;
  (void)gs_send_message(GS_QUEUE_MM, (void*)&rr_stop_gsm_mode_cnf, TRUE);
  MSG_GERAN_HIGH_0("RR_STOP_GSM_MODE_CNF sent");
  /* Fix for CR 2876405*/
#ifdef FEATURE_AWARE_CT21
  rr_debug_log_mm_rr_message(RR_STOP_GSM_MODE_CNF,0);
#endif
  rr_static_set_stop_gsm_mode_cnf_pending(FALSE);
}

static void rr_static_send_abort_cnf(void)
{
  rr_abort_cnf_T rr_abort_cnf;
  RR_PREPARE_MM_MESSAGE(rr_abort_cnf, RR_ABORT_CNF);
  rr_abort_cnf.status = MM_AS_SUCCESS;
  (void)gs_send_message(GS_QUEUE_MM, (void*)&rr_abort_cnf, TRUE);
  MSG_GERAN_HIGH_0("RR_ABORT_CNF sent");
#ifdef FEATURE_AWARE_CT21
  rr_debug_log_mm_rr_message(RR_ABORT_CNF,0);
#endif
}

/**
 * Discard mm message from dyn_pend_q and send responce if needed
 *
 * @param mm_message_id    msg_id of the NAS msg whihc needs to be discarded
 *
 * @param send_resp        whether a resp needs to be sent or not 
 *
 */
static void rr_static_discard_mm_message(const mm_rr_message_id_enum_T mm_message_id , boolean send_resp, boolean match_msg)
{
  rr_cmd_type *pend_queue_ptr = (rr_cmd_type*) q_check(&dyn_pend_q);

  while(pend_queue_ptr != NULL)
  {
    /* If message present in Dynamic pending queue, delete the instance */
    if (MS_MM_RR == pend_queue_ptr->msg.message_header.message_set)
    {
      if (!match_msg || mm_message_id == pend_queue_ptr->msg.message_header.message_id)
      {
        MSG_GERAN_HIGH_2("Discarding pending NAS msg from dyn_pend_q (if any) msg_id = %d, pend_queue_ptr = 0x%08X",
		mm_message_id, &pend_queue_ptr);
		
      	if(send_resp)
      	{
      		rr_static_reject_mm_message(pend_queue_ptr);
      	}
		if((rr_static_data.saved_msg_ptr != NULL) &&(rr_static_data.saved_msg_ptr->msg.message_header.message_id  == pend_queue_ptr->msg.message_header.message_id))
	    {
          rr_static_data.saved_msg_ptr = NULL;
		}
      	q_delete(&dyn_pend_q, &pend_queue_ptr->link);
      	GPRS_MEM_FREE(pend_queue_ptr);
      	break;
      }
    }
    pend_queue_ptr = (rr_cmd_type*) q_next(&dyn_pend_q, &pend_queue_ptr->link);  
  }
  return;
}

static boolean rr_static_process_stop_and_deact_req(rr_cmd_type *cmd_ptr)
{
  geran_dyn_state_e geran_dyn_state = geran_get_dyn_state();

  if (geran_dyn_state == GERAN_INACTIVE)
  {
    if (cmd_ptr->msg.message_header.message_id == RR_DEACT_REQ)
    {
      rr_static_send_deact_cnf();

      if (cmd_ptr->msg.rr_deact_req.deact_reason == RR_MM_PSM)
      {
        mph_psm_entry_ind_T  mph_psm_entry_ind;
        RR_PREPARE_L1_MESSAGE(mph_psm_entry_ind, MPH_PSM_ENTRY_IND);
        (void)gs_send_message(GS_QUEUE_LAYER_1, (void*)&mph_psm_entry_ind, TRUE);
      }
    }
    else
    {
      rr_static_send_stop_gsm_mode_cnf();
    }
	rr_static_data.resume_in_progress = FALSE;
	rr_clear_static_backup();
    rr_stop_dmm_resumption_timer();
    rr_static_data.rem_drx_time_ms = 0;
    return FALSE;
  }
  else
  {
    if (cmd_ptr->msg.message_header.message_id == RR_STOP_GSM_MODE_REQ)
    {
      set_rr_mm_gsm_stop_reason(cmd_ptr->msg.rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason);
    }

    /* Save the new message and send PROC_REQ */
    if (geran_dyn_state == GERAN_UNLOADED)
    {
      if (cmd_ptr->msg.message_header.message_id == RR_STOP_GSM_MODE_REQ &&
          cmd_ptr->msg.rr_stop_gsm_mode_req.rr_mm_gsm_stop_reason == RR_MM_DEEP_SLEEP)
      {
        rr_static_send_stop_gsm_mode_cnf();
      }
      else if (!(cmd_ptr->msg.message_header.message_id == RR_DEACT_REQ &&
                 rr_static_data.proc_cnf_pending &&	rr_static_data.pending_proc == PROC_STOP))
      {
        /* If any previous PROC is pending then invalidate it */
        rr_static_invalidate_saved_msg();
        rr_static_send_proc_req(PROC_STOP, NULL);
      }
    }
    else if (TRUE == geran_get_mm_high_priority())
    {
      /* If any previous PROC is pending then invalidate it */
      rr_static_invalidate_saved_msg();

      rr_static_send_proc_req(PROC_HIGH, cmd_ptr);

    }
  }

  rr_static_resume_completed();
  rr_stop_dmm_resumption_timer();
  rr_static_data.rem_drx_time_ms = 0;
  return TRUE;
}
#endif

/**
  @brief Called when there is a message to be processed in the RR task queue.
  If dynamic loading feature is enabled, this function will call SOmgr API
  to  load RR and process messages further.
  Else the module will be loaded statically.

  This is triggered by RR_CMD_Q_SIG being set, causing RR task to run
*/
#ifdef PERLUTF
#error code not present
#else
static void rr_read_messages(void)
#endif // PERLUTF
{
  rr_cmd_type *cmd_ptr;

  while ((cmd_ptr = rr_q_get()) != NULL)
  {
#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
    boolean process_msg = TRUE;
    
    MSG_GERAN_HIGH_2("Read message (%d, %d)", cmd_ptr->msg.message_header.message_set, cmd_ptr->msg.message_header.message_id);
    switch (cmd_ptr->msg.message_header.message_set)
    {
      case MS_RR_L1:
      {
        if (cmd_ptr->msg.message_header.message_id == MPH_PROC_IND && rr_static_data.suspend_in_progress == FALSE)
        {
          MSG_GERAN_HIGH_2("recieved MPH_PROC_IND (proc=%d, proc_cnf_pending=%d)", 
            cmd_ptr->msg.mph_proc_ind.proc, rr_static_data.proc_cnf_pending);

          if (cmd_ptr->msg.mph_proc_ind.proc == PROC_SUSPEND)
          {
            /* Discard any saved msg and let the dispatched handle MPH_PROC_IND */
            rr_static_invalidate_saved_msg();
            rr_static_data.suspend_in_progress = TRUE;
			rr_static_data.rem_drx_time_ms = cmd_ptr->msg.mph_proc_ind.rem_drx_time_ms;
		    /* Start DMM resumption timer with timeout passed by GL1 */
            if (cmd_ptr->msg.mph_proc_ind.rem_drx_time_ms > 0)
            {
              rr_start_dmm_resumption_timer((rex_timer_cnt_type)cmd_ptr->msg.mph_proc_ind.rem_drx_time_ms);
            }
            else
            {
              MSG_GERAN_HIGH_0("INVALID dmm_resumption_timer in MPH_PROC_IND (suspend)");
            }

            /* WWAN tech status whether tech is loaded or unloaded */
            mm_wwan_set_tech_status(FALSE);
          }
          else if (cmd_ptr->msg.mph_proc_ind.proc == PROC_RESUME)
          {
            /* If PROC_CNF is not pending, then move the pending messages to main queue,
               otherwise messages will be moved while handling PROC_CNF. */
            if (rr_static_data.proc_cnf_pending == FALSE)
            {
              rr_process_dyn_pend_queue(); 
            }
            process_msg = FALSE;
          }
          else
          {
            MSG_GERAN_ERROR_0("Unexpected proc in MPH_PROC_IND");
            process_msg = FALSE;
          }		  
#ifdef FEATURE_AWARE_CT21
/* Using pended count to log proc value */
		  rr_debug_log_rr_l1_message(MPH_PROC_IND,cmd_ptr->msg.mph_proc_ind.proc);
#endif
        }
        else if (cmd_ptr->msg.message_header.message_id == MPH_PROC_CNF)
        {
          MSG_GERAN_HIGH_4("recieved MPH_PROC_CNF (status=%d, proc=%d, proc_cnf_pending=%d, pending_proc=%d)",
            cmd_ptr->msg.mph_proc_cnf.status, cmd_ptr->msg.mph_proc_cnf.proc, rr_static_data.proc_cnf_pending, rr_static_data.pending_proc);

          process_msg = FALSE;

          if (rr_static_data.proc_cnf_pending == TRUE &&
              cmd_ptr->msg.mph_proc_cnf.proc == rr_static_data.pending_proc)
          {
            if (cmd_ptr->msg.mph_proc_cnf.proc == PROC_STOP)
            {
              /* Check if a CNF need to be send to NAS */
              rr_deact_reason_t deact_reason = get_rr_deact_reason();
              if (deact_reason == RR_DEACT_REASON_DEACT_REQ)
              {
                rr_static_send_deact_cnf();
              }
              else if (deact_reason == RR_DEACT_REASON_STOP_GSM_MODE_REQ)
              {
                rr_static_send_stop_gsm_mode_cnf();
              }
            }
            
            if (cmd_ptr->msg.mph_proc_cnf.status == PROC_SUCCESS)
            {	  
              /* Move all the messages from Dynamic pending queue to front of RR main queue.
               * When cmd_ptr loop iterates, all pend messages will be processed in order
               */
              if (rr_static_data.resume_in_progress)
              {
                MSG_GERAN_HIGH_0("DMMSUSREM:: Send DMM_Resume_ind");
                rr_send_imsg_dmm_resume_ind();

                /* resume other modules as well */
                (void)geran_resume();
              }
              else
              {
                rr_process_dyn_pend_queue();
              }
              rr_static_data.suspend_in_progress = FALSE;
              rr_static_data.processed_msg_ptr = rr_static_data.saved_msg_ptr;
              if (rr_static_data.saved_msg_ptr != NULL)
              {
                rr_static_data.processed_msg_id  = rr_static_data.saved_msg_ptr->msg.message_header.message_id;
              }
            }
            else
            {
              rr_static_invalidate_saved_msg();
            }

            rr_static_data.proc_cnf_pending = FALSE;
            rr_static_data.saved_msg_ptr = NULL;
#ifdef FEATURE_AWARE_CT21
/* Using pended count to log proc cnf status */
		    rr_debug_log_rr_l1_message(MPH_PROC_CNF,cmd_ptr->msg.mph_proc_cnf.status);
#endif
          }
        }
        break;
      }
      case MS_MM_RR:
      {
        geran_dyn_state_e geran_dyn_state = geran_get_dyn_state();
        boolean mm_high_priority = geran_get_mm_high_priority();
        
        MSG_GERAN_HIGH_7("Recieved NAS msg %d (dyn_state=%d, proc_cnf_pending=%d, pending_proc=%d, mm_high_priority=%d, suspend_in_progress=%d, resume_in_progress = %d)", 
          cmd_ptr->msg.message_header.message_id, geran_dyn_state, rr_static_data.proc_cnf_pending, 
          rr_static_data.pending_proc, mm_high_priority, rr_static_data.suspend_in_progress, 
          rr_static_data.resume_in_progress);
  
        if (rr_static_data.resume_in_progress)
        {
          if(cmd_ptr->msg.message_header.message_id == RR_ABORT_REQ)
          {
#ifdef FEATURE_AWARE_CT21
		  	 rr_debug_log_mm_rr_message(RR_ABORT_REQ,0);
#endif
             rr_static_send_abort_cnf();
             rr_static_discard_mm_message(RR_EST_REQ,FALSE,TRUE);
             process_msg = FALSE;
          }
          else if(((cmd_ptr->msg.message_header.message_id == RR_DEACT_REQ)
                 || (cmd_ptr->msg.message_header.message_id == RR_STOP_GSM_MODE_REQ))
                 && rr_static_data.proc_cnf_pending)
          {
             rr_static_discard_mm_message(cmd_ptr->msg.message_header.message_id,TRUE,FALSE);
             process_msg = rr_static_process_stop_and_deact_req(cmd_ptr);
          }
          else if(cmd_ptr->msg.message_header.message_id == RR_WWAN_SET_PRIORITY_IND)
          {
             geran_set_mm_high_priority(cmd_ptr->msg.mm.rr_wwan_set_prioirity_ind.high_priority);
             process_msg = FALSE;
          }
          else
          {
            /* During Resume other NAS messages are not processed, they are moved to pending queue */
          }
        }
        else if (rr_static_data.suspend_in_progress)
        {
          /* During suspend NAS messages are not processed, they are moved to pending queue */
        }
        else if (cmd_ptr->msg.message_header.message_id == RR_PLMN_SELECT_REQ ||
                 cmd_ptr->msg.message_header.message_id == RR_PLMN_LIST_REQ)
        {
          /* If any previous PROC is pending then invalidate it */
          rr_static_invalidate_saved_msg();

          if (rr_timer_is_running(RR_DMM_RESUMPTION_TIMER) 
#ifdef FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH
               || (get_rr_mm_gsm_stop_reason() == RR_SUSPEND_STACK && cmd_ptr->msg.message_header.message_id == RR_PLMN_SELECT_REQ && rr_static_get_scell_backup_ptr() != NULL)
#endif /* FEATURE_GERAN_GPS_PLMN_SCAN_SEARCH */
             )
          {
            MSG_GERAN_HIGH_0("DMMSUSREM:: resume in progress is set to TRUE");
            rr_timer_cancel(RR_DMM_RESUMPTION_TIMER);
            rr_static_data.resume_in_progress = TRUE;
          }

          /* Save the new message and send PROC_REQ */
          rr_static_send_proc_req(PROC_IDLE, cmd_ptr);
        }
        else if (cmd_ptr->msg.message_header.message_id == RR_EST_REQ)
        {
          /* If any previous PROC is pending then invalidate it */
          rr_static_invalidate_saved_msg();

          if (rr_timer_is_running(RR_DMM_RESUMPTION_TIMER))
          {
            MSG_GERAN_HIGH_0("DMMSUSREM:: RR_DMM_RESUMPTION_TIMER is cancled");
            rr_timer_cancel(RR_DMM_RESUMPTION_TIMER);
            rr_static_data.resume_in_progress = TRUE;
            rr_static_send_proc_req(PROC_IDLE, cmd_ptr);
          }
          else
          {
            /* Save the new message and send PROC_REQ */
            rr_static_send_proc_req(PROC_CONNECT, cmd_ptr);
          }
        }
        else if (cmd_ptr->msg.message_header.message_id == RR_STOP_GSM_MODE_REQ ||
                 cmd_ptr->msg.message_header.message_id == RR_DEACT_REQ)
        {
           process_msg = rr_static_process_stop_and_deact_req(cmd_ptr);
        }
        else if (cmd_ptr->msg.message_header.message_id == RR_ABORT_REQ && 
                 rr_static_data.proc_cnf_pending && 
                 (rr_static_data.pending_proc == PROC_CONNECT ||
                 (rr_static_data.pending_proc == PROC_IDLE && rr_static_data.resume_in_progress == TRUE)))
        {
          rr_static_invalidate_saved_msg();
#ifdef FEATURE_AWARE_CT21
		  rr_debug_log_mm_rr_message(RR_ABORT_REQ,0);
#endif
          rr_static_send_abort_cnf();
          process_msg = FALSE;
        }
        else if (cmd_ptr->msg.message_header.message_id == RR_FORBIDDEN_LAI_UPDATE_REQ)
        {
          rr_store_forbidden_lai_list((rr_forbidden_lai_update_req_T*)&cmd_ptr->msg);
          process_msg = FALSE;
        }
        else if(cmd_ptr->msg.message_header.message_id == RR_WWAN_SET_PRIORITY_IND)
        {
          geran_set_mm_high_priority(cmd_ptr->msg.mm.rr_wwan_set_prioirity_ind.high_priority);
          process_msg = FALSE;
        }
        break;
      }	

      case MS_TIMER:
      {
        if(cmd_ptr->msg.message_header.message_id == MID_TIMER_EXPIRY)
        {
          if (cmd_ptr->msg.mid_timer_expiry.timer_id == RR_DMM_RESUMPTION_TIMER)
          {
            MSG_GERAN_HIGH_0("DMMSUSREM:: DMM_Resumption timer expired");
          }

          if (is_geran_loaded() == FALSE)
          {
            process_msg = FALSE;

            if (cmd_ptr->msg.mid_timer_expiry.timer_id == RR_CELL_BARRED_TIMER && rr_timer_validate_expiry_message(RR_CELL_BARRED_TIMER))
            {
              rr_process_barred_cell_timer_expiry();
            }
            else if (cmd_ptr->msg.mid_timer_expiry.timer_id == RR_DMM_RESUMPTION_TIMER && rr_timer_validate_expiry_message(RR_DMM_RESUMPTION_TIMER))
            {
              rr_static_send_proc_req(PROC_IDLE, cmd_ptr);
            }
			else if(cmd_ptr->msg.mid_timer_expiry.timer_id == RR_UNLOADED_NAS_MSG_GUARD_TIMER && rr_timer_validate_expiry_message(RR_UNLOADED_NAS_MSG_GUARD_TIMER))
			{
			  rr_static_discard_mm_message(RR_PLMN_LIST_REQ,TRUE,FALSE);				
			}
            else
            {
              MSG_GERAN_HIGH_1("Timer %d expiry ignored in unloaded state", cmd_ptr->msg.mid_timer_expiry.timer_id);
            }
          }
		  else if (cmd_ptr->msg.mid_timer_expiry.timer_id == RR_DMM_RESUMPTION_TIMER && rr_timer_validate_expiry_message(RR_DMM_RESUMPTION_TIMER))
		  {
		    process_msg = FALSE;
		    MSG_GERAN_HIGH_0("DMMSUSREM:: DMM_Resumption timer expired in geran_loaded state, restarting");
            rr_start_dmm_resumption_timer((rex_timer_cnt_type)rr_static_data.rem_drx_time_ms);
		  }
        }
        break;
      }
      case MS_MSGR_RR:
      {	
#ifdef FEATURE_GSM_CELL_SELECT_INFO      
        if(cmd_ptr->msg.rr_msgr_msg.message.hdr.id == GERAN_GRR_CELL_SELECT_INFO_REQ)
        {
        	MSG_GERAN_HIGH_0("Recieved GERAN_GRR_CELL_SELECT_INFO_REQ ");
        	boolean is_ue_camped     = FALSE;
			byte    rxlev_access_min = 0;
        	if (is_geran_loaded() && rr_is_camped())
        	{
        		gprs_scell_info_t *scell_info_ptr = rr_gprs_get_scell_info();
				is_ue_camped     = TRUE;
				rxlev_access_min = scell_info_ptr->gsm.cell_selection_parameters.RXLEV_ACCESS_MIN;
        	}
       		rr_send_cell_select_info_rsp(is_ue_camped,rxlev_access_min);
			process_msg = FALSE ;
        }
#endif		
        break;
      }      
      default:
        break;
    }

    if (cmd_ptr != NULL && process_msg == TRUE)
    {
      /* In below cases put the message in the dyn_pend_q. 
       * 1. If we are not loaded
       * 2. If we are waiting for PROC_CNF
       * 3. If suspend is ongoing, and received message is a NAS message */
      if (!is_geran_loaded() ||
          rr_static_data.proc_cnf_pending ||
          ((rr_static_data.suspend_in_progress || rr_static_data.resume_in_progress) && cmd_ptr->msg.message_header.message_set == MS_MM_RR))
      {
        rr_send_to_dyn_pend_queue(cmd_ptr);
      }
      else
      {
        grr_message_dispatcher(cmd_ptr);

        if (rr_static_get_dyn_unload_pending() == TRUE)
        {
          rr_static_send_proc_req(PROC_STOP, NULL);
          rr_static_set_dyn_unload_pending(FALSE);
        }
        else if (rr_static_data.suspend_in_progress)
        {
          if (rr_is_inactive())
          {
            rr_ps_domain_backup();
            rr_static_send_proc_req(PROC_STOP, NULL);
          }
        }
      }
    }
    /* Free the memory if message is already processed */
    else if (cmd_ptr != NULL)
    {
      GPRS_MEM_FREE(cmd_ptr);
    }
#else
    grr_message_dispatcher(cmd_ptr);
#endif
  }
  return;
} /* end rr_read_messages() */

/**
  @brief RR task top-level function

  Called at start-up to setup & initialise RR task, then enter wait loop
*/
void rr_main(dword rex_parm)
{

  (void) rex_parm;

  if (initialised == FALSE)
  {
    /* Perform startup init here. Satisfy dependency of other tasks
       provided by this task. Do not linger here, startup latency is
       impacted. Higher priority tasks in the same group will preempt
       this task at this point. Do not rely upon priority alone as the
       sole mechanism to arbitrate resource contention with other tasks. */

    rcinit_handshake_startup(); /* required rcinit handshake */
    /* Optional dog registration here */
    /* mmcp_sd_task_running **************************************************/
    /* Register for signal that SD has started up */
    rcevt_register_name(SD_TASK_RCEVT_READY, SD_TASK_STARTED_SIG);
    /* mmcp_sd_task_running **************************************************/
    gsm_rr_fn_init();

    initialised = TRUE;
  }

#ifdef PERLUTF
  #error code not present
#else
  for (;;)
  {
    // wait for either a CMD queue signal or a watchdog timer signal
    sigs = rex_wait(
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      RR_RPT_TIMER_SIG |
      RR_CMD_Q_SIG     |
      RR_MSGR_Q_SIG    |
      RR_RESET_SIG     |
      /* mmcp_sd_task_running **************************************************/
      SD_TASK_STARTED_SIG
      /* mmcp_sd_task_running **************************************************/
    );

    // if a watchdog signal is received
    if (sigs & RR_RPT_TIMER_SIG)
    {
      (void) rex_clr_sigs( rex_self(), RR_RPT_TIMER_SIG );

      /* Report to the Dog HB */
      dog_hb_report(rr_dog_rpt_var);
    }

    /* Initiate lower layer failure recovery */
    if (sigs & RR_RESET_SIG)
    {
      (void) rex_clr_sigs(rex_self(), RR_RESET_SIG);

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
      if (is_geran_loaded() == TRUE)
      {
        DM_VOIDFUNC_CALL( GERAN_DMM_GRR_INTF_ID, geran_static_grr_ext_api_s, grr_process_reset_signal);
      }
      else
      {
        MSG_GERAN_ERROR_0("Geran dynamic module not loaded (RR_RESET_SIG)");
      }

#else
        rr_process_reset_signal();
      
#endif
    }

    /* check for the signal that indicates a new message in the task queue */
    if (sigs & RR_CMD_Q_SIG)
    {
      (void) rex_clr_sigs(rex_self(), RR_CMD_Q_SIG);
      rr_read_messages();
    }

    if (sigs & RR_MSGR_Q_SIG)
    {
      (void) rex_clr_sigs(rex_self(), RR_MSGR_Q_SIG);
      rr_read_msgr_messages();
    }

    /* mmcp_sd_task_running **************************************************/
    /* this functionality will be provided by MMCP but for now it's declared here */
    if (sd_task_has_started == FALSE && (sigs & SD_TASK_STARTED_SIG))
    {
      sd_task_has_started = TRUE;
      /* Clear the OS signal */
      (void)rex_clr_sigs( rex_self(),
                          SD_TASK_STARTED_SIG
                         );
      /* It's now OK to call sd_misc_get_supp_band_pref() because SD task has signalled that it's running */
    }

#ifdef TEST_FRAMEWORK
    #error code not present
#endif /* TEST_FRAMEWORK */
  } /* end for */
#endif /* PERLUTF */

  /* Deregister if leaving the task so that we don't ERR_FATAL */
  dog_hb_deregister(rr_dog_rpt_var);
  /*UNREACHED*/
}

/**
  @brief Alternative entry-point for RR task top-level function
*/
void gsm_rr_task(dword dummy)
{
   rr_main(dummy);
}

