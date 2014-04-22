/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     C O M P   T A S K

GENERAL DESCRIPTION
  This file contains all the functions, definitions and data types needed 
  for other tasks to interface to the COMP Task.

  Signal and command definitions for the COMP Task, and functions
  that should be used by other tasks to send commands to the COMP 
  Task are defined here.

EXTERNALIZED FUNCTIONS

EXTERNALIZED FUNCTIONS INTERNAL TO COMP SUBSYSTEM

INITIALIZATION AND SEQUENCING REQUIREMENTS


 
Copyright (c) 2005-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/dsmgr/src/comptask.c#2 $ $DateTime: 2023/01/19 23:06:43 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/02/13   pkp     Dog heart beat implementation.
05/09/12   nd      Memory reduction effort in 3gpp.
03/22/12   nd      Fixing compilation error due to TCB variables undeclared.
01/24/12   vb	   Coan feature cleanup.
11/11/11   nd      Feature cleanup.
10/01/11   bvd     Added featurization for rcinit
08/24/11   sn      Added RLC to PDCP signals to support 4th PDP.
10/12/10   ss      DYNAMIC WDOG support for DATA tasks.
10/06/10   ttv     Added messages, which explain reason for asserting.
03/16/10   sy      Changed task_* api to tmc_task* api.
08/19/09   ss      SU level CMI modifications.
08/18/09   ars     Fixed lint warnings
03/04/09   sa      AU level CMI modifications.
11/06/06   vk      Added Data on Apps support.
08/31/06   rsl     Featurized PDCP
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ds_pdn_defs.h"
#include "comptask_api.h"
#include "rex.h"
#include "task.h"
#include "dog_hb_rex.h"
#include "rcinit_rex.h"
#include "data_msg.h"
#include "gsndcp.h"
#include "ds_system_heap.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
---------------------------------------------------------------------------*/ 

comp_sig_handler_info_type comp_sig_handler[COMP_MAX_SIGNALS] = {{NULL}};
q_type comp_cmd_q = {{NULL}};                       /* Compression Task command queue */

uint32 comp_tx_sig_res_mask = 0;    /* bit mask to track tx sigs reserved */ 


/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION COMP_DEFAULT_SIG_HANDLER()

DESCRIPTION
  This function is registered as the default signal handler for all the
  signals which do not have a registered handler. It merely prints a warning
  message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean comp_default_sig_handler
(
  comp_sig_enum_type sig,                  /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
  /*-------------------------------------------------------------------------
    Return TRUE since we don't want this signal to be enqueued again.
  -------------------------------------------------------------------------*/
  return TRUE;
} /* comp_default_sig_handler() */


/*===========================================================================

FUNCTION COMP_TASK_INIT

DESCRIPTION
  This function performs all the power-up initialization for the Compression
  Task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  comp_task_init( void )
{
  uint8    i;                        /* Loop index */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the command queue and the free command queue, and link the
    command items onto the free command queue.
  -------------------------------------------------------------------------*/  
  memset(&comp_cmd_q, 0, sizeof(q_type));
  if (q_init( &comp_cmd_q ) != &comp_cmd_q)
  {
    DS_ERR_FATAL("q_init comp_cmd_q failed");
    return;
  }

  /*------------------------------------------------------------------------
    Initialize all the signal handlers
  -------------------------------------------------------------------------*/
  /*lint -e641*/
  for( i = 0; i < COMP_MAX_SIGNALS; i++ )
  {
    comp_sig_handler[i].sig_handler = comp_default_sig_handler;
    comp_sig_handler[i].user_data_ptr = NULL;
  }

#ifdef FEATURE_GSM_GPRS
  /*------------------------------------------------------------------------
    Initialise the COMP hosted GPRS SNDCP module.
  -------------------------------------------------------------------------*/
  gsndcp_initialise();
#endif /* FEATURE_GSM_GPRS */
} /* comp_task_init() */


/*===========================================================================

FUNCTION COMP_PROCESS_CMDS

DESCRIPTION
  This function de-queues commands from the COMP Task's command
  queue, and dispatches commands to the appropriate entity for further
  processing, if necessary. Commands are de-queued until the command queue is
  empty.

DEPENDENCIES
  This function should be called when the COMP_CMD_Q_SIG is set.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void comp_process_cmds( void )
{
  comp_cmd_type  *cmd_ptr;                             /* Pointer to command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get commands from the command queue until the queue is empty. For each
    command received, dispatch the command to the appropriate sub-task.
  -------------------------------------------------------------------------*/
  while( (cmd_ptr = (comp_cmd_type *)q_get( &comp_cmd_q )) != NULL )
  {
    if ( (cmd_ptr->hdr.cmd_id <= COMP_MIN_CMD) ||
         (cmd_ptr->hdr.cmd_id >= COMP_MAX_CMDS)
       )
    {
      DS_ERROR_LOG_1("invalid cmd id =d", cmd_ptr->hdr.cmd_id);
      return;
    }

    switch( cmd_ptr->hdr.cmd_id )
    {      
#ifdef FEATURE_GSM_GPRS
       /*-----------------------------------------------------------------------
        When mode handler registers TX and rx wm with sndcp.
      -----------------------------------------------------------------------*/
      case GSNDCP_REG_WM:
        /*lint -e534*/
        gsndcp_register_wm_hdlr ( cmd_ptr );
        /*lint +e534*/
        break;

      /*-----------------------------------------------------------------------
        When mode handler deregisters TX and rx wm with sndcp.
      -----------------------------------------------------------------------*/
      case GSNDCP_DEREG_WM:
        /*lint -e534*/
        gsndcp_deregister_wm_hdlr ( cmd_ptr );
        /*lint +e534*/
        break;
#endif /* FEATURE_GSM_GPRS */
      /*-----------------------------------------------------------------------
        INvalid COMP cmd is received.
      -----------------------------------------------------------------------*/
      default:
        break;
    } /* switch */

    /*-----------------------------------------------------------------------
      Free the command buffer that is allocated in comp_get_cmd_buff
    -----------------------------------------------------------------------*/
    DS_SYSTEM_HEAP_MEM_FREE(cmd_ptr);

  } /* while */
} /* comp_process_cmds() */
                          

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION COMP_TASK

DESCRIPTION
  This is the entry point for the COMP Task. This function contains
  the main processing loop that waits for events (signals or commands) and
  dispatches each event to the appropriate entity for further processing.

DEPENDENCIES
  None

RETURN VALUE
  This function does not return.

SIDE EFFECTS
  None

===========================================================================*/
void  comp_task
(
  dword ignored
)
{
  rex_sigs_type   requested_sigs;   /* Signal mask to suspend on           */
  rex_sigs_type   set_sigs;         /* Signals set upon return from wait   */
  dog_report_type comp_dog_rpt_id = 0;
  /*----------------------------------------------------------------------- 
    We get the comp tcb pointer locally.
  ----------------------------------------------------------------------*/
  rex_tcb_type* comp_tcb_ptr;
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  RCEVT_SIGEX_SIGREX term_sig; /* Termination Signal to wait on*/
  comp_tcb_ptr = rex_self();
  term_sig.signal = comp_tcb_ptr;
  term_sig.mask = 1 << COMP_TASK_STOP_SIG;

  /* Register for the STOP Signal */
  rcinit_register_term_group(RCINIT_GROUP_5, 
                             RCEVT_SIGEX_TYPE_SIGREX, 
                             &term_sig);
#else
  comp_tcb_ptr = &comp_tcb;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  rcinit_handshake_startup();
  comp_task_init();
#else
  comp_task_init();
#endif

  /*-------------------------------------------------------------------------
    Dog heart beat registration.
  -------------------------------------------------------------------------*/
  comp_dog_rpt_id = dog_hb_register_rex(
                 (rex_sigs_type)1 << (rex_sigs_type)COMP_DOG_HB_REPORT_SIG);


  /*-------------------------------------------------------------------------
    Signal mask to suspend on is the combination of all the signals requested
    by each of the sub-tasks.
  -------------------------------------------------------------------------*/
  requested_sigs = 
    ( (rex_sigs_type) 1 <<  COMP_SNDCP_CMD_Q_SIGNAL ) |
    ( (rex_sigs_type) 1 <<  COMP_SNDCP_LL_DL_Q_SIGNAL)|
    ( (rex_sigs_type) 1 <<  COMP_CMD_Q_SIG)        | 
    ( (rex_sigs_type) 1 <<  COMP_TASK_STOP_SIG)    | 
    ( (rex_sigs_type) 1 <<  COMP_TASK_OFFLINE_SIG) |
    ( (rex_sigs_type) 1 <<  COMP_DOG_HB_REPORT_SIG) |
    ( (rex_sigs_type) 1 <<  COMP0_TX_DATA_SIG) |
    ( (rex_sigs_type) 1 <<  COMP1_TX_DATA_SIG) |
    ( (rex_sigs_type) 1 <<  COMP2_TX_DATA_SIG) |
    ( (rex_sigs_type) 1 <<  COMP3_TX_DATA_SIG);
  
  /*-------------------------------------------------------------------------
    Main task loop, never exits.
  -------------------------------------------------------------------------*/
  for( ;; )
  {

    /*----------------------------------------------------------------------
      Wait for one of the specified signals to be set.
    -----------------------------------------------------------------------*/
    set_sigs = rex_wait( requested_sigs );
    
    /*----------------------------------------------------------------------
     Clear comp_tcb_ptr with set_sigs.  set_sigs is not altered.
    ----------------------------------------------------------------------*/
    /*lint -e534*/
    (void)rex_clr_sigs( comp_tcb_ptr, set_sigs );
    /*lint +e534*/

    if( (set_sigs & ( (rex_sigs_type) 1 <<  COMP_DOG_HB_REPORT_SIG)) != 0 )
    {
      dog_hb_report( comp_dog_rpt_id ); /* Play ping<->pong */
    }

    /*----------------------------------------------------------------------
      If any of the task signals were received, invoke the function to ACK
      task conroller.
    -----------------------------------------------------------------------*/
    if( (set_sigs & ( (rex_sigs_type) 1 <<  COMP_TASK_STOP_SIG)) != 0 )
    {
#ifdef FEATURE_MODEM_RCINIT_PHASE2
      dog_hb_deregister( comp_dog_rpt_id);
      rcinit_unregister_term_group(RCINIT_GROUP_5, RCEVT_SIGEX_TYPE_SIGREX, &term_sig);
      rcinit_handshake_term();
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */

#endif
    }

    /*----------------------------------------------------------------------
      If the command queue signal was set, invoke the function that 
      processes the commands for COMP task.
    -----------------------------------------------------------------------*/
    if( (set_sigs & ( (rex_sigs_type) 1 <<  COMP_CMD_Q_SIG)) != 0 )
    {
      comp_process_cmds();
    }

    if ( (set_sigs & ( (rex_sigs_type) 1 <<  COMP_SNDCP_CMD_Q_SIGNAL)) != 0)
    {
      (void)comp_sig_handler[COMP_SNDCP_CMD_Q_SIGNAL].sig_handler
     (
       COMP_SNDCP_CMD_Q_SIGNAL,
       comp_sig_handler[COMP_SNDCP_CMD_Q_SIGNAL].user_data_ptr
      );
    }
    if ( (set_sigs & ( (rex_sigs_type) 1 <<  COMP_SNDCP_LL_DL_Q_SIGNAL)) != 0)
    {
      (void)comp_sig_handler[COMP_SNDCP_LL_DL_Q_SIGNAL].sig_handler
     (
       COMP_SNDCP_LL_DL_Q_SIGNAL,
       comp_sig_handler[COMP_SNDCP_LL_DL_Q_SIGNAL].user_data_ptr
      );
    }

    /*----------------------------------------------------------------------
      Process Transmit queues next. If the tx-hdlr returns false, then 
      there is more data sitting on the tx queue, so set the tx signal again.
    -----------------------------------------------------------------------*/
    if ( (set_sigs & ( (rex_sigs_type) 1 <<  COMP0_TX_DATA_SIG)) != 0)
    {
      (void)comp_sig_handler[COMP0_TX_DATA_SIG].sig_handler
     (
       COMP0_TX_DATA_SIG,
       comp_sig_handler[COMP0_TX_DATA_SIG].user_data_ptr
      );
    }
    if ( (set_sigs & ( (rex_sigs_type) 1 <<  COMP1_TX_DATA_SIG)) != 0)
    {
      (void)comp_sig_handler[COMP1_TX_DATA_SIG].sig_handler
     (
       COMP1_TX_DATA_SIG,
       comp_sig_handler[COMP1_TX_DATA_SIG].user_data_ptr
      );
    }
    if ( (set_sigs & ( (rex_sigs_type) 1 <<  COMP2_TX_DATA_SIG)) != 0)
    {
      (void)comp_sig_handler[COMP2_TX_DATA_SIG].sig_handler
     (
       COMP2_TX_DATA_SIG,
       comp_sig_handler[COMP2_TX_DATA_SIG].user_data_ptr
      );
    }
    if ( (set_sigs & ( (rex_sigs_type) 1 <<  COMP3_TX_DATA_SIG)) != 0)
    {
      (void)comp_sig_handler[COMP3_TX_DATA_SIG].sig_handler
     (
       COMP3_TX_DATA_SIG,
       comp_sig_handler[COMP3_TX_DATA_SIG].user_data_ptr
      );
    }
  } /* forever */
} /* comp_task() */


/*===========================================================================
FUNCTION COMP_SET_SIGNAL()

DESCRIPTION 
  Sets a signal for the COMP task and notifies the scheduler that there
  is processing to be performed in comp task. 
 
PARAMETERS 
  sig   : Signal to be set.
 
DEPENDENCIES
  Signal handler must be set in advance. Otherwise signal handling is 
  defaulted (printing an err msg).
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void comp_set_signal
(
  comp_sig_enum_type sig                       
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  (void)rex_set_sigs(rcinit_lookup_rextask("comp"), (rex_sigs_type) 1 << ( sig ) );
#else
  (void)rex_set_sigs(&comp_tcb, (rex_sigs_type) 1 << ( sig ) );
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
} /* comp_set_signal() */

/*===========================================================================
FUNCTION COMP_CLEAR_SIGNAL()

DESCRIPTION
  Clear a signal for the COMP task.
  
PARAMETERS 
  sig   : Signal to be cleared.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void comp_clear_signal
(
  comp_sig_enum_type sig                      /* Signal to be disabled     */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  (void)rex_clr_sigs(rcinit_lookup_rextask("comp"), (rex_sigs_type) 1 << ( sig ) );
#else
  (void)rex_clr_sigs(&comp_tcb, (rex_sigs_type) 1 << ( sig ) );
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
} /* comp_disable_sig() */


/*===========================================================================

FUNCTION COMP_GET_CMD_BUF

DESCRIPTION
  This function allocates a command buffer from the COMP Task. The
  The caller of this function must check for a NULL return value, and either
  perform some error recovery or ERR_FATAL, as appropriate.

DEPENDENCIES
  None

RETURN VALUE
  Returns a pointer to a command buffer, if available, or NULL if no command
  buffers are available.

SIDE EFFECTS
  None

===========================================================================*/

comp_cmd_type  *comp_get_cmd_buf( void )
{
  comp_cmd_type    *cmd_ptr;                 /* Pointer to command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate command buffer from the heap.
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(cmd_ptr, sizeof(comp_cmd_type), comp_cmd_type*);
  if(cmd_ptr == NULL)
  {
    DS_ERROR_FATAL("comp_get_cmd_buf: Memory allocation failed");
  }

  return( cmd_ptr );
} /* comp_get_cmd_buf() */

/*===========================================================================

FUNCTION COMP_PUT_CMD

DESCRIPTION
  This function puts a command on the COMP Task's command queue, and
  sets the COMP_CMD_Q_SIG.

DEPENDENCIES
  The caller of this function should have previously allocated a command
  buffer by calling comp_get_cmd_buf().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  comp_put_cmd
(
  comp_cmd_type    *cmd_ptr                 /* Pointer to command to be sent */
)
{
  DS_MSG1(MSG_LEGACY_HIGH, "comp_put_cmd cmd_id = %d", cmd_ptr->hdr.cmd_id);

  if ( (cmd_ptr->hdr.cmd_id <= COMP_MIN_CMD) ||
       (cmd_ptr->hdr.cmd_id >= COMP_MAX_CMDS)
     )
  {
    DS_ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Put the command on the command queue, and set the command queue signal.
  -------------------------------------------------------------------------*/
  q_put( &comp_cmd_q, &cmd_ptr->hdr.link );
  COMP_SET_SIGNAL(COMP_CMD_Q_SIG);

  return;
} /* comp_put_cmd() */

/*===========================================================================

FUNCTION COMP_SET_SIG_HANDLER()

DESCRIPTION
  This function sets a handler for the specified signal  
  
DEPENDENCIES
  None.

RETURN VALUE
  Previously registered signal handler

SIDE EFFECTS
  None.

===========================================================================*/
comp_sig_handler_type comp_set_sig_handler
(
  comp_sig_enum_type sig,              /* Signal assocaited with handler     */
  comp_sig_handler_type sig_handler,   /* Signal handler function pointer    */ 
  void *user_data_ptr                /* Param to be passed to the handler  */
)
{
  comp_sig_handler_type old_handler;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_ASSERT( sig_handler != NULL );

  old_handler = comp_sig_handler[sig].sig_handler;

  comp_sig_handler[sig].sig_handler = sig_handler;  
  comp_sig_handler[sig].user_data_ptr = user_data_ptr;

  return old_handler;
} /* comp_set_sig_handler() */

/*===========================================================================

FUNCTION COMP_RESERVE_TO_COMP_TX_SIG()

DESCRIPTION
  This function reserves a tx signal for GSNDCP/PDCP and returns it. 
  
DEPENDENCIES
  None.

RETURN VALUE
  Signal reserved

SIDE EFFECTS
  None.

===========================================================================*/

comp_sig_enum_type comp_reserve_to_comp_tx_sig(void)
{
  uint32 sig = (uint32)COMP0_TX_DATA_SIG;
  uint32 maxsig = (uint32)COMP3_TX_DATA_SIG;
  uint32 sigbit = 1 << sig;

  for (; sig <= maxsig; ++sig) {
      if (!(sigbit & comp_tx_sig_res_mask)) {
          /* we found an unreserved tx signal */
          comp_tx_sig_res_mask |= sigbit;
          return (comp_sig_enum_type)sig;
      }
      sigbit <<= 1;
  }
  
  DS_ERROR_FATAL("Cannot reserve tx signal!");
  return COMP_MAX_SIGNALS;
}

/*===========================================================================

FUNCTION COMP_UNRESERVE_TO_COMP_TX_SIG()

DESCRIPTION
  This function unreserves a tx signal for GSNDCP/PDCP. 
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void comp_unreserve_to_comp_tx_sig(comp_sig_enum_type sig)
{
  uint32 sigbit = 1 << ((uint32)sig);

  comp_tx_sig_res_mask &= ~sigbit;
  return;
}

