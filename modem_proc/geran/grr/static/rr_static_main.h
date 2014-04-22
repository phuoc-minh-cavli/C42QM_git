#ifndef RR_STATIC_MAIN_H
#define RR_STATIC_MAIN_H

/*!
  @file rr_static_main.h

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

$Header: //components/rel/geran.mpss/5.2.0/grr/static/rr_static_main.h#9 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
18/05/16   sc      Initial Revision

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "geran_grr_static_api.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
extern q_type      rr_cmd_q;

extern rex_crit_sect_type  rr_q_lock_crit_sect;
/*===========================================================================

                         INTERNAL VARIABLES

===========================================================================*/
#define RR_MSGR_QUEUE_SIZE 5
#define RR_MSG_QUEUE_SIZE (10 * NUM_GERAN_DATA_SPACES) /* cmd queue size */

/* Event to indicate that SD task is ready to process commands */
#define SD_TASK_RCEVT_READY "sdtask:ready"

/* SD task signals that it's running (so sd_misc_get_supp_band_pref can safely be called) */
#define SD_TASK_STARTED_SIG          0x20000

#define RR_Q_LOCK_INIT()  rex_init_crit_sect(&rr_q_lock_crit_sect)
#define RR_Q_LOCK()       REX_ISR_LOCK(&rr_q_lock_crit_sect)
#define RR_Q_UNLOCK()     REX_ISR_UNLOCK(&rr_q_lock_crit_sect)

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
extern void geran_grr_init();
extern void geran_grr_deinit();

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
extern void geran_grr_resume();
extern void geran_grr_suspend();

extern boolean rr_static_get_dyn_unload_pending();
extern void rr_static_set_dyn_unload_pending(boolean unload_status);
extern boolean rr_static_get_suspend_in_progress();
extern void rr_static_resume_completed();
extern void rr_static_send_proc_req(proc_type_e proc, rr_cmd_type *mm_msg);
extern boolean rr_static_get_stop_gsm_mode_cnf_pending();
extern void rr_static_set_stop_gsm_mode_cnf_pending(boolean status);
#endif

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

extern void rr_read_msgr_messages(void);
extern void grr_message_dispatcher(rr_cmd_type *cmd_ptr);
#endif /* RR_STATIC_MAIN_H */
