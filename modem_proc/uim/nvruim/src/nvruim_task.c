/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            N V    R - U I M    S U B S Y S T E M    TASK


GENERAL DESCRIPTION

  This source file contains the main NVRUIM task along with supporting functions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2015 QUALCOMM Technologies, Incorporated and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/nvruim/src/nvruim_task.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/25/15   ar      Fix compilation warnings
01/19/15   stv     Mechanism to provide lock to lower priority task like diag
08/28/14   av      Change nvruim crit sect to mutex-based from pimutex-based
07/15/14   yt      Support for NV refresh
07/14/14   tl      Remove always ON feature FEATURE_MODEM_RCINIT
04/28/14   av      Fix to return appropriate error code if access denied
04/03/14   am      Fix compile error on SBM due to assert_on_crit_sect_owner
04/01/14   av      NVRUIM headers cleanup
02/20/14   av      Support for async processing of NV_RTRE_OP_CONFIG_I
01/28/14   df      Use q_cnt() instead directly accessing cnt
01/10/14   df      Use v2.0 diag macros
11/01/13   tkl     deregister watch dog when break from main task
11/01/13   at      Support for guaranteed response for each NVRUIM request
10/28/13   kb      Add offtarget support to check mutex is owned or not
10/17/13   at      SIM busy support in NVRUIM
10/01/13   vr      Move break statement of TASK_STOP_SIG to
                   FEATURE_MODEM_RCINIT flag
09/23/13   vr      qtf-crm MOB migration
07/02/13   vs      Reworked return values to NV API and clients
05/13/13   vs      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rex.h"
#include "task.h"
#include "dog_hb_rex.h"
#include "modem_mem.h"
#include "queue.h"
#include "nvruimi.h"
#include "nvruim_p.h"
#include "nvruim.h"
#include "nvruim_task.h"
#include "nvruim_request.h"
#include "uim_msg.h"
#include "rcevt_rex.h"
#include "rcinit.h"

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Queue to hold commands for NVRUIM */
q_type                     nvruim_task_cmd_q;

/* Global structure to general info for NVRUIM task */
nvruim_generic_data_type  *nvruim_generic_data_ptr  = NULL;

/*  NVRUIM Dog variables */
static dog_report_type     nvruim_dog_rpt_var       = 0;


/*===========================================================================
   FUNCTION:      NVRUIM_CMD

   DESCRIPTION:
     This function allows putting of command into the NVRUIM command queue.

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     nv_stat_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
nv_stat_enum_type nvruim_cmd
(
  nv_cmd_type *nv_cmd_ptr
)
{
  if(nv_cmd_ptr == NULL)
  {
    return NV_BADPARM_S;
  }

  /* Do not queue command if task has been stopped */
  if(nvruim_generic_data_ptr != NULL)
  {
    if(nvruim_generic_data_ptr->task_stopped)
    {
      UIM_MSG_ERR_1("NVRUIM Task stopped, cannot queue request for 0x%x",
                    nv_cmd_ptr->item);
      return NV_FAIL_S;
    }
  }

  if (q_cnt(&nvruim_task_cmd_q) >= NVRUIM_CMD_QUEUE_MAX_SIZE)
  {
    UIM_MSG_ERR_1("Maximum number of commands have been put onto cmd_q. Failed for item 0x%x",
                  nv_cmd_ptr->item);
    return NV_FAIL_S;
  }

  (void)q_link(nv_cmd_ptr, &nv_cmd_ptr->link);

  /* Put the message on the queue */
  q_put(&nvruim_task_cmd_q, &nv_cmd_ptr->link);

  /* Set the command queue signal */
  (void) rex_set_sigs(UIM_NVRUIM_TCB, NVRUIM_TASK_CMD_Q_SIG);
  UIM_MSG_HIGH_2("Successfully queued NV request 0x%x for item 0x%x",
                 nv_cmd_ptr->cmd, nv_cmd_ptr->item);
  return NV_DONE_S;
} /* nvruim_cmd */


/*===========================================================================
FUNCTION NVRUIM_WAIT

DESCRIPTION
  Main wait routine for the NVRUIM task.

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals). The calling unit can then check for the reason for
  exiting the wait.

SIDE EFFECTS
  None
===========================================================================*/
rex_sigs_type nvruim_wait
(
  rex_sigs_type requested_mask
)
{
  rex_sigs_type rex_signals_mask = 0x00;  /* Mask of signals returned by REX */

  if (rex_self() != UIM_NVRUIM_TCB)                                            
  {
    return rex_signals_mask;
  }

  /* Stop listening to signals corresponding to the NVRUIM command
     queue if NVRUIM task has been stopped */
  if(nvruim_generic_data_ptr != NULL)
  {
    if(nvruim_generic_data_ptr->task_stopped)
    {
      requested_mask &= ~NVRUIM_TASK_CMD_Q_SIG ;
    }
  }

  /* --------------------------------------------------------------------
  Loop until the current signals from REX match with one or more of
  the requested signals.
  -------------------------------------------------------------------- */
  do
  {
    (void) rex_clr_sigs( rex_self(), NVRUIM_RPT_TIMER_SIG );
    dog_hb_report(nvruim_dog_rpt_var);

    /* ---------------------------------------------------------------
    Perform the real wait to wait for a command...or to force a
     context switch to allow higher priority tasks to run.
    ---------------------------------------------------------------- */
    rex_signals_mask = rex_wait(requested_mask | NVRUIM_RPT_TIMER_SIG);

  /* do while mask requested does not match current mask */
  } while ((rex_signals_mask & requested_mask) == 0);


  return (rex_signals_mask);
} /* nvruim_wait() */


/*===========================================================================
FUNCTION NVRUIM_HANDLE_SIM_BUSY_SIG

DESCRIPTION
  This function, called in nvruim_task, processes the SIM busy signal.

DEPENDENCIES
  mmgsdi_task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void nvruim_handle_sim_busy_sig(void)
{
  UIM_MSG_HIGH_0("NVRUIM: GOT SIM_BUSY_SIG");

  /* Clear signal */
  (void) rex_clr_sigs(rex_self(), NVRUIM_SIM_BUSY_SIG);

  /* Also tell MMGSDI to cancel any pending requests */
  nvruim_send_mmgsdi_cancel();
} /* nvruim_handle_sim_busy_sig */


/*===========================================================================
FUNCTION NVRUIM_WAIT_FOR_MMGSDI_ACCESS

DESCRIPTION
  Wait routine called when MMGSDI access is in progress

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void nvruim_wait_for_mmgsdi_access (
  void
)
{
  rex_sigs_type rex_signals_mask = 0x00;  /* Mask of signals returned by REX */

  /* --------------------------------------------------------------------
  Loop until the current signals from REX match with one or more of
  the signals of interest
  -------------------------------------------------------------------- */
  do
  {
    (void) rex_clr_sigs( rex_self(), NVRUIM_RPT_TIMER_SIG );
    dog_hb_report(nvruim_dog_rpt_var);

    rex_signals_mask = rex_wait(NVRUIM_MMGSDI_ACCESS_SIG |
                                NVRUIM_RPT_TIMER_SIG     |
                                NVRUIM_SIM_BUSY_SIG);

    /* If there was a SIM_BUSY event while waiting for the MMGSDI response,
       we need to cancel that request and expect an error callback */
    if (rex_signals_mask & NVRUIM_SIM_BUSY_SIG)
    {
      nvruim_handle_sim_busy_sig();
    }

  /* do while mask requested does not match current mask */
  } while ((rex_signals_mask & NVRUIM_MMGSDI_ACCESS_SIG) == 0);
} /* nvruim_wait_for_mmgsdi_access */


/*===========================================================================
FUNCTION NVRUIM_HANDLE_OFFLINE_SIG

DESCRIPTION
  This function, called in nvruim_task, is called to process an offline signal.

DEPENDENCIES
  mmgsdi_task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void nvruim_handle_offline_sig(void)
{
  UIM_MSG_HIGH_0("NVRUIM: GOT TASK_OFFLINE_SIG");

  /* Clear signal */
  (void) rex_clr_sigs(rex_self(), TASK_OFFLINE_SIG);
} /* nvruim_handle_offline_sig */


/*===========================================================================
FUNCTION NVRUIM_HANDLE_STOP_SIG

DESCRIPTION
  This function, called in nvruim_task, is called to process an stop signal.

DEPENDENCIES
  mmgsdi_task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void nvruim_handle_stop_sig(void)
{
  UIM_MSG_HIGH_0("NVRUIM: GOT TASK_STOP_SIG");

   /* Clear signal */
  (void) rex_clr_sigs(rex_self(), TASK_STOP_SIG);

  /* Don't process items in Queue */
  /* Sort of flushes the Queue    */
  if(nvruim_generic_data_ptr != NULL)
  {
    nvruim_generic_data_ptr->task_stopped = TRUE;
  }
} /* nvruim_handle_stop_sig */


/*===========================================================================
FUNCTION NVRUIM_HANDLE_QUEUE_SIG

DESCRIPTION
  This function, called in nvruim_task, is called to process a signals of
  command queue. This function is called only for the processing ASYNC
  NV requests.

DEPENDENCIES
  nvruim_task must be finished initialization.

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
===========================================================================*/
static void nvruim_handle_queue_sig(rex_sigs_type sigs_rcvd)
{
  nv_cmd_type                   *nv_cmd_ptr                       = NULL;
  nv_rtre_control_type          dummy_rtre_control_value;
  nv_rtre_polling_control_type  dummy_rtre_polling_control_value;

  if(sigs_rcvd & NVRUIM_TASK_CMD_Q_SIG)
  {
    /* Process ASYNC NVRUIM Command */
    nv_cmd_ptr = (nv_cmd_type*)(q_get(&nvruim_task_cmd_q));

    /* If there is a command on the queues */
    if(nv_cmd_ptr != NULL)
    {
      UIM_MSG_HIGH_0("NVRUIM_TASK_CMD_Q_SIG received");

      switch (nv_cmd_ptr->cmd)
      {
        case NV_RTRE_OP_CONFIG_F:
          (void)nvruim_process_rtre_configuration(nv_cmd_ptr,
                                                  &dummy_rtre_control_value,
                                                  &dummy_rtre_polling_control_value);
          break;
        case NV_READ_F:
        case NV_WRITE_F:
          (void)nvruim_process_nv_access(nv_cmd_ptr);
          break;
        default:
          UIM_MSG_ERR_1("NVRUIM does not support 0x%x cmd type", nv_cmd_ptr->cmd);
          nv_cmd_ptr->status = NV_BADPARM_S;
          break;
      }

      if (nv_cmd_ptr->done_q_ptr)
      {
        /* Put the data on done q provided by async client */
        (void)q_link(nv_cmd_ptr, &nv_cmd_ptr->link);
        q_put(nv_cmd_ptr->done_q_ptr, &nv_cmd_ptr->link);
      }

      if (nv_cmd_ptr->tcb_ptr)
      {
        /* Let the client know the NV command/request completed */
        (void) rex_set_sigs(nv_cmd_ptr->tcb_ptr, nv_cmd_ptr->sigs);
      }

      /* Since we do not own/allocate nv_cmd_ptr, DO NOT TRY TO FREE
       it here */
    }

    (void) rex_clr_sigs(UIM_NVRUIM_TCB, NVRUIM_TASK_CMD_Q_SIG);

    /* Set the signal if there are one or more commands to look at */
    if (q_cnt(&nvruim_task_cmd_q) > 0)
    {
      /*  Items on the queue. Set the signal */
      (void) rex_set_sigs(UIM_NVRUIM_TCB, NVRUIM_TASK_CMD_Q_SIG);
    }
  } /* end if NVRUIM_TASK_CMD_Q_SIG */
} /* nvruim_handle_queue_sig */


/*===========================================================================
FUNCTION NVRUIM_INIT_POST_STARTUP

DESCRIPTION
  Function called to define timers, dog registrations and other initialization
  that requires NVRUIM task to be up as a prerequisite.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
===========================================================================*/
static void nvruim_init_post_startup(
  void
)
{
  /* --------------------------------------------------------
      Clear all signals
  -------------------------------------------------------- */
  (void) rex_clr_sigs(UIM_NVRUIM_TCB, (rex_sigs_type)~0);

  /* --------------------------------------------------------
      Register for dog heartbeat mechanism
  -------------------------------------------------------- */
  nvruim_dog_rpt_var =
    dog_hb_register_rex((rex_sigs_type) NVRUIM_RPT_TIMER_SIG);
} /* nvruim_init_post_startup */


/*===========================================================================
FUNCTION NVRUIM_INIT

DESCRIPTION
  Function called to initialize NVRUIM queues and global variables.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
  On targets with RCInit enabled in place of TMC, this function is called
  in RCInit context. Therefore, it should not include references to TCBs, NV
  operations, etc.
===========================================================================*/
void nvruim_init (
  void
)
{
  /* If for whatever reasons this init funtion is called more than
     once, we put this safeguard in place to avoid initializing
     multiple times */
  if(nvruim_generic_data_ptr != NULL)
  {
    return;
  }
  /* ---------------------------------------------------------
      INITIALIZE ALL QUEUES
  --------------------------------------------------------- */
  (void) q_init(&nvruim_task_cmd_q);

  /* --------------------------------------------------------
      INITIALIZE GLOBALS
  -------------------------------------------------------- */
  nvruim_generic_data_ptr =
    modem_mem_calloc(1, sizeof(nvruim_generic_data_type), MODEM_MEM_CLIENT_UIM);
  if (nvruim_generic_data_ptr == NULL)
  {
    UIM_MSG_ERR_0("NVRUIM_INIT: Error allocating memory; not registering with NV");
    return;
  }

  nvruim_generic_data_ptr->task_stopped                      = FALSE;
  nvruim_generic_data_ptr->rtre_config_init                  = FALSE;
  nvruim_generic_data_ptr->rtre_control_value                =
    NV_RTRE_CONTROL_NO_RUIM;
  nvruim_generic_data_ptr->rtre_polling_control_value =
    NV_RTRE_POLLING_CONTROL_NO_POLL;

  qurt_mutex_init(&(nvruim_generic_data_ptr->crit_sect));
  rex_init_crit_sect(&(nvruim_generic_data_ptr->timer_crit_sect));

  nv_ruim_register(nvruim_process_nv_req);
} /* nvruim_init */


/*===========================================================================
FUNCTION NVRUIM_TASK_ENDED

DESCRIPTION
  Function called to cleanup when task is ended.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called when task ends and break from the
  task loop.
===========================================================================*/
static void nvruim_task_ended (
  void
)
{
  /* Deregister dog if leaving the task so that we don't ERR_FATAL */
  dog_hb_deregister(nvruim_dog_rpt_var);
} /* nvruim_task_ended */


/*===========================================================================
FUNCTION NVRUIM_TASK

DESCRIPTION
  Entry point for the NVRUIM task.  This function performs task initialization,
  then sits in an infinite loop, waiting on an input queue, and responding
  to messages received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void nvruim_task
(
  dword dummy
)
{
  rex_sigs_type             sigs =  NVRUIM_TASK_CMD_Q_SIG       |
                                    NVRUIM_SIM_BUSY_SIG         |
                                    TASK_STOP_SIG               |
                                    TASK_OFFLINE_SIG;
  rex_sigs_type             sigs_rcvd = 0x00;
  RCEVT_SIGEX_SIGREX        stop_sig;
  RCINIT_GROUP              nvruim_group;

  (void)dummy;

  stop_sig.signal = UIM_NVRUIM_TCB;
  stop_sig.mask = TASK_STOP_SIG;

  nvruim_group = rcinit_lookup_group_rextask(UIM_NVRUIM_TCB);

  rcinit_register_term_group(nvruim_group, RCEVT_SIGEX_TYPE_SIGREX, &stop_sig);

  /* Handshake with RCInit framework */
  rcinit_handshake_startup();

  (void)rcevt_signal_name("NVRUIM:READY");

  nvruim_init_post_startup();

  /* ------------------------------------------------------------------------
  ** Main Task Loop
  ** ----------------------------------------------------------------------*/
  for (;;)
  {
    /*-----------------------------------------------------------------------------
                            Wait on signal
    -----------------------------------------------------------------------------*/
    sigs_rcvd = nvruim_wait( sigs );

    /*-----------------------------------------------------------------------------
                          Process Signal - TASK_OFFLINE_SIG
    -----------------------------------------------------------------------------*/

    if (sigs_rcvd & TASK_OFFLINE_SIG)
    {
      nvruim_handle_offline_sig();
    } /* end if TASK_OFFLINE_SIG */

    /*-----------------------------------------------------------------------------
                          Process Signal - TASK_STOP_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & TASK_STOP_SIG)
    {
      nvruim_handle_stop_sig();
      rcinit_unregister_term_group(nvruim_group, RCEVT_SIGEX_TYPE_SIGREX, &stop_sig);
      rcinit_handshake_term();
      break;
    } /* end if TASK_STOP_SIG */

    /*-----------------------------------------------------------------------------
            Process Signal - NVRUIM_TASK_CMD_Q_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & sigs & NVRUIM_TASK_CMD_Q_SIG)
    {
      /* Do not handle command if NVRUIM task has been stopped */
      if(!nvruim_generic_data_ptr->task_stopped)
      {
        nvruim_handle_queue_sig(sigs_rcvd);
      }
    } /* end if NVRUIM_TASK_CMD_Q_SIG */
    /*-----------------------------------------------------------------------------
            Process Signal - NVRUIM_SIM_BUSY_SIG
    -----------------------------------------------------------------------------*/
    else if (sigs_rcvd & NVRUIM_SIM_BUSY_SIG)
    {
      nvruim_handle_sim_busy_sig();
    } /* end if NVRUIM_SIM_BUSY_SIG */
    else
    {
      UIM_MSG_ERR_1("NVRUIM TASK INVALID SIGS: %x", sigs_rcvd);

      /* Reset the Sigs by Masking out Good Sigs and
      ** Clearing out Bad SIGS
      */
      (void)rex_clr_sigs(UIM_NVRUIM_TCB, (sigs_rcvd & ~sigs) );
    } /* end else - Invalid signal in NVRUIM task */
  } /* end for(;;) */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

  nvruim_task_ended();
} /* nvruim_task */

