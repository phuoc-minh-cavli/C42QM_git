/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER UIM ENTRIES

GENERAL DESCRIPTION
  This file contains the functions used to manage the phonebook task and
  queues

  Copyright (c) 2002 - 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/pbm/src/pbmtask.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/16   sp      Logging improvements
10/01/15   lm      Fix compilation issues on linux
09/30/15   nr      Cmd_ptr freeing twice
09/26/15   nr      Fix for pbm crash while MCFG NV refresh handling
08/05/15   nr      Operator specifc NV ECC numbers configuration
09/25/14   kr      Compilation error removal
07/07/14   NR      Use v2.0 diag macros
05/01/14   NR      FR 19033: Reduce Code Duplication pbm
04/04/14   NR      QTF CRM MOB Changes
03/17/14   kb      Add offtarget support to check mutex is owned or not
01/12/14   NR      TMC header deprecation changes
12/19/13   NR      PBM: LLVM compiler warnings
03/18/10   krishna Used pbmtask_i.h instead if pbmtask.h as part of CMI activity
07/10/09  ashwanik Support for CSIM and Dual Sim Features
04/01/09   kcp     Fixed CR 169091 - Terminal does not read PB files
                   during power up. Only the EF-PBR is read.
08/28/08   sg      Featurize MMGSDI seek/search calls based on FEATURE_MMGSDI_ENHANCED_SEARCH
07/10/08   sg      Handle the PBM_GSDI_SEEK_CB commands from GSDI
05/08/08   clm     Queue up pbm notify requests to avoid calling them while in critical sections of pbm.
01/28/08   clm     Remove posibility of entering infinite loop, if gargabe collect fails
                   to clean up properly when EFS is FULL.
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
11/02/07   clm     Add refresh okay and init complete support to pbm for mmgsdi.
10/10/06   cvs     Move messages to PBM's diag msg ID
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
02/23/06   cvs     rework memory allocation macros
01/09/06   cvs     Lint fixes
11/16/05   cvs     Lint fixes
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/12/05   AT      Added UCS2 support for strings.
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
03/08/05   PA      Support for PBM notifications
02/08/05   PA      Support for read lock
01/18/05   PA      Fixed compilation issues in non-MMGSDI builds
01/12/05   PA      PBM USIM Support
01/10/05   PA      Kick dog before NV writes.
12/08/04   PA      Kick dog before init to be safe.
11/19/04   PA      Support non-GSDI builds
11/18/04   PA      Support for write lock.
09/29/04   PA      Initial Revision
===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "pbm.h"
#include "pbmtask_i.h"
#include "pbmtask.h"
#include "pbmefs.h"
#include "pbmutils.h"
#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#endif /* FEATURE_MODEM_RCINIT */

#include "pbmgsdi.h"
#include "pbmuim.h"
#include "uim_msg.h"
#include "pbm_conversion.h"
#include "pbm_nv_refresh.h"
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
#include "stringl/stringl.h"

rex_crit_sect_type pbm_crit_sect;

static dog_report_type    pbm_reg_dog_rpt_var  = 0;
/* this is used during registration with dog */

q_type pbm_client_cmd_q;       /* Queue to manage client commands */

q_type pbm_gsdi_resp_q;        /* Queue to manage GSDI responses */

static q_type pbm_notify_evt_q;       /* Queue to manage notify events */


/*===========================================================================
FUNCTION PBM_KICK_DOG

DESCRIPTION
  Kick the watchdog and set the watchdog timer.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void pbm_kick_dog(void)
{
	dog_hb_report( pbm_reg_dog_rpt_var );
} /* pbm_kick_dog() */



/*===========================================================================

FUNCTION PBM_CMD_ALLOC

DESCRIPTION
  Allocate a PBM command buffer. The size of command is provided as input.

DEPENDENCIES
  Heap must have been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If Heap is empty a NULL is returned.

SIDE EFFECTS
==========================================================================*/
void *pbm_cmd_alloc(uint16 size)
{
   pbm_cmd_s_type *cmd = NULL;

   if (size < sizeof(pbm_cmd_s_type))
   {
      UIM_MSG_ERR_1("Invalid size for a PBM command %d", size);
      return NULL;
   }

   PBM_MEM_ALLOC(cmd, size);
   PBM_CHECK_PTR_RET(cmd, NULL);

   memset(cmd, 0, size);

   cmd->size = size;

   return(void *)cmd;
}


/*===========================================================================
FUNCTION PBM_CMD_FREE

DESCRIPTION
  Deallocate a PBM command buffer.

DEPENDENCIES
  Heap must have already been initialized.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void pbm_cmd_free(void *ptr)
{
   PBM_MEM_FREEIF(ptr);
}


/*===========================================================================
FUNCTION PBM_CMD_Q_PUT

DESCRIPTION
  Add an item to the specified queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
static void pbm_cmd_q_put( q_type *q_ptr, pbm_cmd_s_type *cmd_ptr)
{
   PBM_CHECK_PTR2_RET(q_ptr, cmd_ptr, VOID);

   (void) q_link(cmd_ptr, &cmd_ptr->link);
   q_put(q_ptr, &cmd_ptr->link);
}


/*===========================================================================
FUNCTION PBM_CMD_Q_GET

DESCRIPTION
  Obtain the first item from the specified queue

DEPENDENCIES
  None

RETURN VALUE
  NULL if queue is empty
  pbm_cmd_s_type pointer if successful

SIDE EFFECTS

===========================================================================*/
pbm_cmd_s_type * pbm_cmd_q_get(q_type *q_ptr)
{
   PBM_CHECK_PTR_RET(q_ptr, NULL);

   return(pbm_cmd_s_type *) q_get(q_ptr);
}


/*===========================================================================
FUNCTION PBM_CLIENT_CMD_Q_PUT

DESCRIPTION
  Add an item to the client command queue and send a signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void pbm_client_cmd_q_put(pbm_cmd_s_type *cmd_ptr)
{
   PBM_CHECK_PTR_RET(cmd_ptr, VOID);

   pbm_cmd_q_put(&pbm_client_cmd_q, cmd_ptr);
   (void) rex_set_sigs(PBM_TCB, PBM_CLIENT_CMD_SIG);
}


/*===========================================================================
FUNCTION PBM_GSDI_RESP_Q_PUT

DESCRIPTION
  Add an item to the GSDI response queue and send a signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void pbm_mmgsdi_resp_q_put(pbm_cmd_s_type *cmd_ptr)
{
   PBM_CHECK_PTR_RET(cmd_ptr, VOID);

   pbm_cmd_q_put(&pbm_gsdi_resp_q, cmd_ptr);
   (void) rex_set_sigs(PBM_TCB, PBM_GSDI_RESPONSE_SIG);
}


/*===========================================================================
FUNCTION PBM_NOTIFY_EVT_Q_PUT

DESCRIPTION
  Add an item to the NOTIFY event queue and send a signal.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void pbm_notify_evt_q_put(pbm_cmd_s_type *cmd_ptr)
{
   PBM_CHECK_PTR_RET(cmd_ptr, VOID);

   pbm_cmd_q_put(&pbm_notify_evt_q, cmd_ptr);
   (void) rex_set_sigs(PBM_TCB, PBM_NOTIFY_EVT_SIG);
}


/*===========================================================================
FUNCTION PBM_NOTIFY_EVT_HANDLE

DESCRIPTION
  Read the Notify events from the queue and handle it.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void pbm_notify_evt_handle(void)
{
   pbm_cmd_s_type        *cmd_ptr = NULL;
   pbm_notify_evt_s_type *notify_evt = NULL;

   while ((cmd_ptr = pbm_cmd_q_get(&pbm_notify_evt_q)) != NULL)
   {
      if (PBM_CMD_NOTIFY_EVT == cmd_ptr->cmd_type)
      {
         /* PBM Notify Event */
         PBM_CMD_CHECK_SIZE(cmd_ptr, pbm_notify_evt_s_type, break);
         notify_evt = (pbm_notify_evt_s_type*)cmd_ptr;
         pbm_notify(&(notify_evt->notify_data));
      } else
      {
         UIM_MSG_ERR_1("Unknown command %d", cmd_ptr->cmd_type);
      }
      pbm_cmd_free(cmd_ptr);
   }
}




#ifdef FEATURE_MODEM_RCINIT
/*===========================================================================
FUNCTION PBM_INIT_BEFORE_TASK_START

DESCRIPTION
  Initialization of PBM task before the task has been started
  critical sections heap and globals for conversion arrays are initialized.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void pbm_init_before_task_start( void )
{
   rex_init_crit_sect(&pbm_crit_sect);


   /* Initialize the Conversion tables. */
   pbmconvert_init_conversion();

   /* Initialize queues */
   (void) q_init(&pbm_client_cmd_q);
   (void) q_init(&pbm_gsdi_resp_q);
   (void) q_init(&pbm_notify_evt_q);
}

#else /* FEATURE_MODEM_RCINIT */

/*===========================================================================
FUNCTION PBM_TASK_INIT

DESCRIPTION
  Initialization of PBM task structures

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
static void pbm_task_init(void)
{
   rex_init_crit_sect(&pbm_crit_sect);

   /* Initialize the Conversion tables. */
   pbmconvert_init_conversion();

   /* Initialize queues */
   (void) q_init(&pbm_client_cmd_q);
   (void) q_init(&pbm_gsdi_resp_q);
   (void) q_init(&pbm_notify_evt_q);

  /*register to dog*/
   pbm_reg_dog_rpt_var = dog_hb_register_rex((rex_sigs_type) PBM_RPT_TIMER_SIG);

   /* Wait for start signal from Main Control task */
   tmc_task_start();
   (void) rex_clr_sigs( rex_self(), TASK_START_SIG );
}

#endif /* FEATURE_MODEM_RCINIT */

/*===========================================================================
FUNCTION PBM_WAIT

DESCRIPTION
  Suspend PBM from executing until at least one of the specified signals
  in the input mask is set.

DEPENDENCIES
  pbm_rpt_timer must have been initialized

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type pbm_wait(rex_sigs_type mask)
{
   rex_sigs_type    sigs;

   for (;;)
   {

      /* Wait for specified signal or watchdog timer */
      sigs = rex_wait(mask | PBM_RPT_TIMER_SIG);

      if (sigs & PBM_RPT_TIMER_SIG)
      {
         sigs &= ~PBM_RPT_TIMER_SIG;
         (void) rex_clr_sigs(PBM_TCB, PBM_RPT_TIMER_SIG);
         pbm_kick_dog();
      }

      if (sigs & mask)
         return sigs;

   } /* forever */
}


#if defined FEATURE_MODEM_RCINIT && defined FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_MODEM_RCINIT && FEATURE_UIM_TEST_FRAMEWORK */


#ifdef FEATURE_MODEM_RCINIT
/*===========================================================================
FUNCTION PBM_TASK

DESCRIPTION
  The main PBM task.

DEPENDENCIES
  REX is running.

RETURN VALUE
  Never exits.

SIDE EFFECTS

===========================================================================*/
void pbm_task(dword dummy)
{
   rex_sigs_type sigs;
#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

   /*register to dog*/
   pbm_reg_dog_rpt_var = dog_hb_register_rex((rex_sigs_type) PBM_RPT_TIMER_SIG);

   rcinit_handshake_startup(); // required rcinit handshake

#ifdef FEATURE_UIM_TEST_FRAMEWORK
   #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

   pbm_kick_dog();

   (void)pbm_init();
   for (;;)
   {
      /* Wait forever for rex signals */
      sigs = pbm_wait( PBM_CLIENT_CMD_SIG |
                       PBM_GSDI_RESPONSE_SIG |
                       TASK_OFFLINE_SIG |
                       PBM_TASK_STOP_SIG |
#ifdef FEATURE_UIM_TEST_FRAMEWORK
                       #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
                       PBM_NOTIFY_EVT_SIG
                     );


      /* Kick watchdog */
      if (sigs & PBM_RPT_TIMER_SIG)
      {
         (void) rex_clr_sigs(rex_self(), PBM_RPT_TIMER_SIG);
         pbm_kick_dog();
      }

      /* OFFLINE signal */
      if (sigs & TASK_OFFLINE_SIG)
      {
         (void) rex_clr_sigs(rex_self(), TASK_OFFLINE_SIG);
         UIM_MSG_MED_0(" TASK_OFFLINE_SIG received");
      }
      /* GSDI command */
      if (sigs & PBM_GSDI_RESPONSE_SIG)
      {
         (void) rex_clr_sigs(rex_self(), PBM_GSDI_RESPONSE_SIG);
         UIM_MSG_MED_0(" PBM_GSDI_RESPONSE_SIG received");
         pbm_mmgsdi_resp_handle();
      }

      /* Client command */
      if (sigs & PBM_CLIENT_CMD_SIG)
      {
         (void) rex_clr_sigs(rex_self(), PBM_CLIENT_CMD_SIG);
         UIM_MSG_MED_0(" PBM_CLIENT_CMD_SIG received");
         pbm_client_cmd_handle();
      }

      /* Notify event command */
      if (sigs & PBM_NOTIFY_EVT_SIG)
      {
         (void) rex_clr_sigs(rex_self(), PBM_NOTIFY_EVT_SIG);
         UIM_MSG_MED_0(" PBM_NOTIFY_EVT_SIG received");
         pbm_notify_evt_handle();
      }
  
      /* STOP signal */
      if (sigs & PBM_TASK_STOP_SIG)
      {
         (void) rex_clr_sigs(rex_self(), PBM_TASK_STOP_SIG);
         UIM_MSG_MED_0("PBM_TASK_STOP_SIG received" );
#ifdef FEATURE_UIM_TEST_FRAMEWORK
        #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
      }

#ifdef FEATURE_UIM_TEST_FRAMEWORK
	  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
   } /* for (;;) */
#if defined FEATURE_MODEM_RCINIT && defined FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_MODEM_RCINIT && FEATURE_UIM_TEST_FRAMEWORK */
}

#else /* FEATURE_MODEM_RCINIT */

/*===========================================================================
FUNCTION PBM_TASK

DESCRIPTION
  The main PBM task.

DEPENDENCIES
  REX is running.

RETURN VALUE
  Never exits.

SIDE EFFECTS

===========================================================================*/
void pbm_task(dword dummy)
{
   rex_sigs_type sigs;
   pbm_return_type ret;

   pbm_task_init();

   pbm_kick_dog();

   ret = pbm_init();
   PBM_CHECK_ERR(ret, "Failed to initialize PBM");

   for (;;)
   {      /* Wait forever for rex signals */

      sigs = pbm_wait( PBM_CLIENT_CMD_SIG |
                       PBM_GSDI_RESPONSE_SIG |
                       TASK_OFFLINE_SIG |
                       PBM_TASK_STOP_SIG |
                       PBM_NOTIFY_EVT_SIG
                     );

      /* Kick watchdog */
      if (sigs & PBM_RPT_TIMER_SIG)
      {
         (void) rex_clr_sigs(PBM_TCB, PBM_RPT_TIMER_SIG);
         UIM_MSG_MED_0("PBM_RPT_TIMER_SIG received");
         pbm_kick_dog();
      }

      /* OFFLINE signal */
      if (sigs & TASK_OFFLINE_SIG)
      {
         (void) rex_clr_sigs(PBM_TCB, TASK_OFFLINE_SIG);
         UIM_MSG_MED_0(" TASK_OFFLINE_SIG received");
         tmc_task_offline();
      }
      /* GSDI command */
      if (sigs & PBM_GSDI_RESPONSE_SIG)
      {
         (void) rex_clr_sigs(PBM_TCB, PBM_GSDI_RESPONSE_SIG);
         UIM_MSG_MED_0(" PBM_GSDI_RESPONSE_SIG received");
         pbm_mmgsdi_resp_handle();
      }

      /* Client command */
      if (sigs & PBM_CLIENT_CMD_SIG)
      {
         (void) rex_clr_sigs(PBM_TCB, PBM_CLIENT_CMD_SIG);
         UIM_MSG_MED_0(" PBM_CLIENT_CMD_SIG received");
         pbm_client_cmd_handle();
      }

      /* Notify event command */
      if (sigs & PBM_NOTIFY_EVT_SIG)
      {
         (void) rex_clr_sigs(PBM_TCB, PBM_NOTIFY_EVT_SIG);
         UIM_MSG_MED_0(" PBM_NOTIFY_EVT_SIG received");
         pbm_notify_evt_handle();
      }

      /* STOP signal */
      if (sigs & PBM_TASK_STOP_SIG)
      {
         (void) rex_clr_sigs(PBM_TCB, PBM_TASK_STOP_SIG);
         UIM_MSG_MED_0("PBM_TASK_STOP_SIG received");
         tmc_task_stop();
      }

   } /* for (;;) */
}
#endif /* FEATURE_MODEM_RCINIT */

