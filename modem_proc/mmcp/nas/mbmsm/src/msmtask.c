/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     MBMS SERVICE MANAGER TASK MODULE (MSMTASK.C)

GENERAL DESCRIPTION

 This file contains types, function definitions and data definitions for
 MBMS Service Manager Task

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All 
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mmcp.mpss/6.1.10/nas/mbmsm/src/msmtask.c#1 $ $DateTime: 2023/04/25 14:39:18 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/17/06   vr      Initial version
03/18/08   rk      Added Support to merge MBMS SM in CB task
09/12/11   HC      Replacing the #include "ms_timer.h" with #include "ms_timer_v.h" 
                   as a part of CMI activity
                   Replacing the #include "gs.h" with #include "gs_v.h" 
                   as a part of CMI activity

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "task.h"
#include "dog_hb_rex.h"
#include "queue.h"
#include "err.h"
#include "gs_v.h"
#include "ms_timer_v.h"
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

#if defined(FEATURE_MBMS) || defined (FEATURE_MODEM_MBMS)

#include "msmtask.h"
#include "msmmain.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Command queue for the MSM task */
q_type msm_cmd_q;                                       

extern rex_tcb_type* cb_tcb_ptr;

/*===========================================================================

FUNCTION MSM_GET_CMD_BUF

DESCRIPTION

  This function returns a command buffer of type msm_ext_cmd_type
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling msm_free_cmd_buf(). 

  Calling function MUST check for a NULL return value and take necessary
  action.
 
DEPENDENCIES

  None.
  
RETURN VALUE

  A buffer of type msm_ext_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.
  
===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
msm_ext_cmd_type * msm_get_cmd_buf
(
  void
)
{
  /* Allocate command buffer */
#ifndef FEATURE_MODEM_HEAP
   return (msm_ext_cmd_type*) gs_alloc( sizeof( msm_ext_cmd_type ));
#else
   return (msm_ext_cmd_type*) modem_mem_calloc( 1, sizeof( msm_ext_cmd_type ), MODEM_MEM_CLIENT_NAS);
#endif
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION MSM_FREE_CMD_BUF

DESCRIPTION

  This function frees a command buffer removed from any MSM 
  command queue. Only commands removed from MSM's command queues
  can be passed into this function.

DEPENDENCIES

  None.
  
RETURN VALUE

  None.

SIDE EFFECTS

  None.
  
===========================================================================*/
void msm_free_cmd_buf
(
  msm_ext_cmd_type *cmd_buf 
)
{
  if(cmd_buf == NULL)
  {
    MSG_ERROR(" Trying to free an empty MSM pointer", 0,0,0);
  }
  else
  {
#ifndef FEATURE_MODEM_HEAP
    gs_free(cmd_buf);
#else
    modem_mem_free(cmd_buf, MODEM_MEM_CLIENT_NAS);
#endif
  }
}

/*===========================================================================

FUNCTION MSM_PUT_CMD

DESCRIPTION

  This function puts a command buffer into MSM's command queue. Note 
  that the command buffer must have been requested through a call to 
  msm_get_cmd_buf(). Also,The calling function must have filled 
  all the necessary data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the MSM
  task to indicate that a command has been placed in the 
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the MSM task and this could cause a context
  switch.

===========================================================================*/
void msm_put_cmd
(
  msm_ext_cmd_type *msm_cmd_ptr  /* The command buffer to be put in the queue */
)
{

  /* Check for Null pointer */
  if (msm_cmd_ptr != NULL)
  {
    /* First initialize the queue link. This is always done 
       before putting an item in a queue.*/
    (void)q_link(msm_cmd_ptr, &(msm_cmd_ptr->cmd_hdr.link));

    /* Put the message on the queue */
    q_put(&msm_cmd_q, &msm_cmd_ptr->cmd_hdr.link);

    /* Set the command queue signal for receive task. */
    (void)rex_set_sigs(cb_tcb_ptr, MSM_CMD_Q_SIG);
  }
  else
  {
    ERR_FATAL("Attempt to put NULL msm_ext_cmd_type on queue", 0,0,0);
  }
}

/*===========================================================================

FUNCTION MSM_INIT

DESCRIPTION
  This function does the initialization for MSM layer. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void msm_init( void )
{
  /* Initialize command queue */
  (void)q_init(&msm_cmd_q);

}

#endif

#ifdef FEATURE_MBMS
/*===========================================================================

FUNCTION MSM_TASK

DESCRIPTION
  This function is the entry point for MSM Task. It initializes MSM state
  and individual procedures by calling msm_init(). The ACK signal is sent back
  to TMC and a wait is done on Task Start Signal from TMC. Once a Task Start
  Signal is received, msm_dispatcher() is called that takes control now onwards.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void msm_task (dword param)
{

  msm_ext_cmd_type * msm_cmd_ptr;

  static rex_sigs_type sigs;
   
  /* Initialize global data, queues, and signals */
  msm_init();

  tmc_task_start();

  /* Loop forever */
  for(;;)
  {
    /* Wait on signal */
    sigs = rex_wait(MSM_CMD_Q_SIG);

    /* If this signal is for the command queue */
    if (sigs & MSM_CMD_Q_SIG)
    {
      /* Clear signals */
      (void) rex_clr_sigs( rex_self(),MSM_CMD_Q_SIG);

      /* Read all commands from command queue */
      while ((msm_cmd_ptr = (msm_ext_cmd_type*)q_get(&msm_cmd_q) ) != NULL)
      {
        /* Send command to main state machine */
        msm_command_handler(msm_cmd_ptr); 

        /* Deallocate command */
        msm_free_cmd_buf(msm_cmd_ptr);
      }   
    }
  }
}

#endif

