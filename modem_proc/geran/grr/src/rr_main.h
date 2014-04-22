#ifndef RR_MAIN_H
#define RR_MAIN_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                               rr_main.h

GENERAL DESCRIPTION
   This module contains functions for the rr main function.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_main.h#4 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
02/10/12    sjw     Integrate DSDS controller with new message processing control
07/31/02    rmc     Added prototype for rr_main function

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "sys_cnst.h"
#include "rr_l1.h"
#include "rr_l1_g.h"
#include "mm_rr.h"
#include "timers.h"
#include "rr_defs.h"
#include "geran_grr_static_api.h"

/*===========================================================================

                         TYPE DECLARATIONS

===========================================================================*/
typedef struct
{
  boolean  valid;
  boolean  recovery_restart_enabled;
} rr_nv_recovery_restart_type_T;

typedef enum
{
  RR_PROCESS_MSG_NEVER = 0,
  RR_PROCESS_MSG_NOW,
  RR_PROCESS_MSG_LATER
} rr_process_msg_action_e;

//geran_static_grr_ext_api_s rr_vtbl;
extern geran_static_grr_ext_api_s rr_vtbl;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/**
  @brief Iterates over the pending message queue checking whether any messages
         may now be processed.

  This function will move any messages that are now processable from the
  pending message queue on to the RR command queue.
*/
extern void rr_check_for_pending_messages(
   
);

/**
  @brief Clear the pending message queue.

  This function will remove all entries from the pending message queue and
  delete them.  Each entry is returned to the free command queue.
*/
extern void rr_flush_pending_messages(
   
);

/**
  @brief Sleep for a number of milliseconds.

  Pauses the RR task until a sleep timer signal is received
*/
extern void rr_timed_wait(dword millisecs);

#ifdef FEATURE_SGLTE
/**
  @brief Check for L2G or T2G resel, CCO or redirection 
  messages on the pending message queue. If found reject the 
  procedure and delete the msg from the pending queue. 
*/
boolean rr_sglte_reject_pending_x2g_req(boolean abort_rcvd);
#endif /*FEATURE_SGLTE*/

/* Dynamically loaded APIs and vtable entries */
extern void geran_grr_init();

extern void rr_process_reset_signal(void);

extern void geran_grr_resume();
 
extern void geran_grr_suspend();
extern void geran_grr_deinit();

#endif /* #ifdef RR_MAIN_H */

