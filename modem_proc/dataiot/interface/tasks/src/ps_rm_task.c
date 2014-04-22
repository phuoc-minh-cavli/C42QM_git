/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              P S _ R M _ T A S K . C

DESCRIPTION
  This is the source file for the RM Data processing Task. Contained herein
  are the functions needed to initialize all the modules that execute in RM
  task context and the main task processing loop.


EXTERNALIZED FUNCTIONS
  ps_rm_task()
    RM Task entry point and main processing loop.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None of the RM services can be used unless MC has created the RM task
  and ps_rm_task() has been invoked.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/dataiot.mpss/2.0/interface/tasks/src/ps_rm_task.c#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/07/10    mkumarsa    Firtst version of the ps rm module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


#include "queue.h"
#include "err.h"
#include "dog_hb_rex.h"
#include "ps_rm_taski.h"
#include "rcinit_rex.h"

#include "data_msg.h"
#include "ds_system_heap.h"
#include "ttl_map.h"

rex_tcb_type    * rex_ps_rm_tcb;

/*--------------------------------------------------------------------------
  Defines for DOG. Assuming DOG_DYNAMIC_API feat is ON, since they (core bsp)
  dont allow anymore additiions to dog files.
---------------------------------------------------------------------------*/
dog_report_type   ps_rm_dog_rpt_id           = 0;      /* Initial Safety */

/*===========================================================================

/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------

Timers needed for the DataCommon Ctrl Task.
---------------------------------------------------------------------------*/
//rex_timer_type ps_rm_dog_rpt_timer;        /* for watchdog reporting  */

q_type ps_rm_cmd_q;

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/
ps_rm_sig_handler_info_type * ps_rm_sig_handler[PS_RM_MAX_SIGNALS] = {NULL};
ps_rm_cmd_handler_type ps_rm_cmd_handler[PS_RM_MAX_DEFINED_CMD_TYPES] = {ps_rm_default_cmd_handler};

/*--------------------------------------------------------------------------
  Pool of signals being currently enabled signals
---------------------------------------------------------------------------*/
rex_sigs_type ps_rm_enabled_sig_mask;

/*===========================================================================
                      EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================
FUNCTION PS_RM_INIT()

DESCRIPTION
  Initialization function for the rm data processing task. This function
  performs the functions needed for the data task to exit disabled state.

DEPENDENCIES
  RM task should have been started.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_rm_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_HIGH,"ps_rm_init(): "
          "PS RM Task Init");

  /*-------------------------------------------------------------------------
    Initialize the command queues for the PS_RM_CMD_Q_SIGNAL signal
  -------------------------------------------------------------------------*/
  (void) q_init( &ps_rm_cmd_q );
  

  /*------------------------------------------------------------------------
    Set signal mask for operation of all signals internal to PS_RM Task
  -------------------------------------------------------------------------*/
  ps_rm_enabled_sig_mask = ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) PS_RM_TASK_STOP_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) PS_RM_TASK_OFFLINE_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) PS_RM_CMD_Q_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) PS_RM_DOG_HB_REPORT_SIGNAL ) |
                          ( (rex_sigs_type) 1 <<
                            (rex_sigs_type) PS_RM_SIG_MASK_CHANGE_SIGNAL );
} /* ps_rm_init() */

/*===========================================================================
FUNCTION PS_RM_TASK_IS_RUNNING()

DESCRIPTION
  This api checks whether ps rm task is initialized or not and return TRUE if 
  it is running otherwise FALSE.

DEPENDENCIES
  RM task should have been started.

RETURN VALUE
  TRUE/FALSE.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ps_rm_task_is_running
(
  void
)
{

  if (rex_ps_rm_tcb != NULL)
    return TRUE;
  else 
    return FALSE;

} /* is_ps_rm_task_running() */

/*===========================================================================

                           LOCAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION PS_RM_DEFAULT_SIG_HANDLER()

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

boolean 
ps_rm_default_sig_handler
(
  ps_rm_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG_SPRINTF_1(MSG_LEGACY_ERROR,"ps_rm_default_sig_handler(): "

                  "Signal %d, no handler registered", sig);

  /*-------------------------------------------------------------------------
    Return TRUE since we don't want this signal to be enqueued again.
  -------------------------------------------------------------------------*/
  return TRUE;
} /* ps_rm_default_sig_handler() */

/*===========================================================================

FUNCTION PS_RM_DEFAULT_CMD_HANDLER()

DESCRIPTION
  This function is registered as the default command handler for all the
  commands which do not have a registered handler. It merely prints a warning
  message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void 
ps_rm_default_cmd_handler
(
  ps_rm_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG_SPRINTF_1(MSG_LEGACY_ERROR,"ps_rm_default_cmd_handler(): "
                  "Command %d, no handler registered", cmd );
} /* ps_rm_default_cmd_handler() */

/*===========================================================================
FUNCTION PS_RM_POWERUP()

DESCRIPTION
  Powerup (coldstart) initialization function for the PS_RM task. 
  
  This function performs initialization before PS_RM task is started.

DEPENDENCIES
  This function must be called only once at system powerup.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void 
ps_rm_powerup
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_HIGH,"ps_rm_powerup(): "
          "PS RM Task Powerup");

} /* ps_rm_powerup() */

/*===========================================================================

FUNCTION PS_RM_PROCESS_CMD()

DESCRIPTION
  This function is called in response to PS_RM_CMD_Q_SIGNAL.  It retrieves the
  next command from the NAT command queue and processes the command by calling 
  the registered cmd handler.

DEPENDENCIES
    An item must be on the NAT command queue or an LOG_MSG_ERROR will occur.

RETURN VALUE
  FALSE if cmd queue contains more commands to process and this function
  should be called again, TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean 
ps_rm_process_cmd
(
  void
)
{
  static byte    ps_rm_cmds_proc_hi_score = 0;
  ps_rm_cmd_type * cmd_ptr;
  byte           processed;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    LOG_MSG_ERROR if no command is available.
  -------------------------------------------------------------------------*/
  if( q_cnt( &ps_rm_cmd_q ) == 0)
  {
    DS_MSG0(MSG_LEGACY_ERROR,"ps_rm_process_cmd(): "
                    "No NAT command to process" );
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Process all commands in the command q.

    Get each command item in sequence (from the head) of the command queue
    and call the correponding handler with appropriate parameters.
  -------------------------------------------------------------------------*/
  processed = 0;
  while( (cmd_ptr = ( ps_rm_cmd_type * ) q_get( &ps_rm_cmd_q )) != NULL )
  {
    if( cmd_ptr->cmd >= PS_RM_MAX_DEFINED_CMD_TYPES )
    {
      DS_MSG_SPRINTF_1(MSG_LEGACY_ERROR,"ps_rm_process_cmd(): "
                      "Invalid cmd id 0x%x in Q", cmd_ptr->cmd);

      ps_rm_free_cmd_buf(cmd_ptr);
      continue;
    }
    processed++;
    ps_rm_cmd_handler[cmd_ptr->cmd]( cmd_ptr->cmd, cmd_ptr->user_data_ptr );
	ps_rm_free_cmd_buf(cmd_ptr);
  }

 
  /*-------------------------------------------------------------------------
    Clear the cmd queue signal (which may have been set if a command was
    issued after the rex_clr_sigs() in the outer NAT processing loop)
    Check if any commands are left before doing this, since a command may
    have been posted between closing brace and INTLOCK above.
  -------------------------------------------------------------------------*/
  if( q_cnt( &ps_rm_cmd_q ) == 0 )
  {
    PS_RM_CLR_SIGNAL(PS_RM_CMD_Q_SIGNAL);
  }

  /*-------------------------------------------------------------------------
    For debugging, show if we just processed a ton of commands
  -------------------------------------------------------------------------*/
  if( processed > ps_rm_cmds_proc_hi_score )
  {
    ps_rm_cmds_proc_hi_score = processed;
    DS_MSG1(MSG_LEGACY_HIGH,"ps_rm_process_cmd(): "
                     "New high # NAT commands processed: %d", processed);
  }

  /*-------------------------------------------------------------------------
    Always return true to clear NAT_CMD_SIG from loop_sigs in case command
    was sent after clearing signal above, so that we never get into the
    situation where we process a command due to loop_sigs, emptying the
    cmd queue, and yet the NAT_CMD_SIG is set, leading to LOG_MSG_ERROR above.
  -------------------------------------------------------------------------*/
  return TRUE;

} /* ps_rm_process_cmd() */


/*===========================================================================

FUNCTION PS_RM_ENABLE_SIG()

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void 
ps_rm_enable_sig
(
  ps_rm_sig_enum_type sig              /* Signal to be enabled               */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (( ps_rm_sig_handler[sig] == NULL || ps_rm_sig_handler[sig]->sig_handler == ps_rm_default_sig_handler))
  {
     DS_MSG_SPRINTF_1(MSG_LEGACY_ERROR,"ps_rm_enable_sig(): "
                    "Signal %d enabled without a handler", sig );
  }

  ps_rm_enabled_sig_mask |= ( (rex_sigs_type) 1 << (rex_sigs_type) sig );

  PS_RM_SET_SIGNAL( PS_RM_SIG_MASK_CHANGE_SIGNAL );
} /* ps_rm_enable_sig() */

/*===========================================================================

FUNCTION PS_RM_DISABLE_SIG()

DESCRIPTION
  This function disables a signal handling by removing it from the
  currently enabled signal mask.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void 
ps_rm_disable_sig
(
  ps_rm_sig_enum_type sig                       /* Signal to be disabled     */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ps_rm_enabled_sig_mask &= ~( (rex_sigs_type) 1 << (rex_sigs_type) sig );
} /* ps_rm_disable_sig() */


/*===========================================================================

FUNCTION PS_RM_SET_SIG_HANDLER()

DESCRIPTION
  This function sets a handler for the specified signal.

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered signal handler

SIDE EFFECTS
  None.

===========================================================================*/

ps_rm_sig_handler_type 
ps_rm_set_sig_handler
(
  ps_rm_sig_enum_type sig,             /* Signal assocaited with handler     */
  ps_rm_sig_handler_type sig_handler,  /* Signal handler function pointer    */
  void *user_data_ptr                /* Param to be passed to the handler  */
)
{
  ps_rm_sig_handler_type old_handler = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  if (sig > 0 && sig < PS_RM_MAX_SIGNALS)
  {
   if(ps_rm_sig_handler[sig] == NULL)
   {
   /*-------------------------------------------------------------------------
    Allocate memory for PS_RM command type
   -------------------------------------------------------------------------*/
    DS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(ps_rm_sig_handler[sig], sizeof(ps_rm_sig_handler_info_type),
                            ps_rm_sig_handler_info_type*);
   }

   old_handler = ps_rm_sig_handler[sig]->sig_handler;
   
   ps_rm_sig_handler[sig]->sig_handler = ( sig_handler != NULL) ?
                                          sig_handler :
                                          ps_rm_default_sig_handler;

   ps_rm_sig_handler[sig]->user_data_ptr = user_data_ptr;

  }
  return old_handler;
} /* ps_rm_set_sig_handler() */

/*===========================================================================
	
FUNCTION PS_RM_GET_CMD_BUF()
	
DESCRIPTION
  This function allocates a command buffer .  

DEPENDENCIES
  None.

RETURN VALUE
  ps_rm_cmd_type * Buffer

SIDE EFFECTS
  None.
	
===========================================================================*/

ps_rm_cmd_type* ps_rm_get_cmd_buf()
{
  ps_rm_cmd_type  *ps_rm_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate memory for PS_RM command type
  -------------------------------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(ps_rm_cmd_ptr, sizeof(ps_rm_cmd_type),
                            ps_rm_cmd_type*);

  if(ps_rm_cmd_ptr == NULL)
  {
     DS_MSG_SPRINTF_1(MSG_LEGACY_ERROR,"ps_rm_get_cmd_buf: Can't get cmd buf for ps_rm task."
		             "Heap Memory Allocation failed for client: %d size: %d" ,
                     (sizeof(ps_rm_cmd_type)));
  }
  else
  {
    memset(ps_rm_cmd_ptr , 0, sizeof(ps_rm_cmd_type));
    /*-----------------------------------------------------------------------
      Ensure command payload pointer points to the payload allocated
    -----------------------------------------------------------------------*/
  }
  return( ps_rm_cmd_ptr );
}

/*===========================================================================


FUNCTION PS_RM_FREE_CMD_BUF()
	
DESCRIPTION
  This function allocates a command buffer .  

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
	
===========================================================================*/

void ps_rm_free_cmd_buf(ps_rm_cmd_type *cmd_ptr)
{

 #ifdef FEATURE_Q_NO_SELF_QPTR
	 q_delete(&ps_rm_cmd_q, &(cmd_ptr->link));
 #else
	 q_delete(&(cmd_ptr->link));
 #endif
 
  DS_SYSTEM_HEAP_MEM_FREE(cmd_ptr);
}

/*===========================================================================

FUNCTION PS_RM_SEND_CMD()

DESCRIPTION
  This function posts a cmd for processing in PS_RM task context.  The cmd is
  processed by calling the registered cmd handler, if any.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void 
ps_rm_send_cmd
(
  ps_rm_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{
  ps_rm_cmd_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get a cmd buffer from the free queue, call error routine if no links
    available, else copy buffer to cmd queue and signal PS_RM task
  -------------------------------------------------------------------------*/

  cmd_ptr = ps_rm_get_cmd_buf();
  if(cmd_ptr == NULL)
  {
     DS_MSG0(MSG_LEGACY_ERROR,"ps_rm_send_cmd(): "
                    "PS_RM task cmd buffers unavailable" );
  }
  else
  {  
    q_link(cmd_ptr, &cmd_ptr->link);
    cmd_ptr->cmd = cmd;
    cmd_ptr->user_data_ptr = user_data_ptr;

    q_put( &ps_rm_cmd_q, &cmd_ptr->link );
    PS_RM_SET_SIGNAL( PS_RM_CMD_Q_SIGNAL );
  }
} /* ps_rm_send_cmd() */


/*===========================================================================

FUNCTION PS_RM_SET_CMD_HANDLER()

DESCRIPTION
  This function sets a handler for the specified cmd type.  The cmd handling
  can be disabled by setting a NULL handler

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered cmd handler

SIDE EFFECTS
  None.

===========================================================================*/

ps_rm_cmd_handler_type 
ps_rm_set_cmd_handler
(
  ps_rm_cmd_enum_type cmd,              /* Command name                      */
  ps_rm_cmd_handler_type cmd_handler    /* Cmd handler function pointer      */
)
{
  ps_rm_cmd_handler_type old_handler;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  old_handler = ps_rm_cmd_handler[cmd];

  ps_rm_cmd_handler[cmd] = ( cmd_handler != NULL) ?
                          cmd_handler :
                          ps_rm_default_cmd_handler;

  return old_handler;
} /* ps_rm_set_cmd_handler() */


/*===========================================================================

FUNCTION PS_RM_WAIT()

DESCRIPTION
  This function performs the PS_RM Task waiting function. This function will
  not return until at least one signal from the requested mask is set.
  The watchdog is also kicked at least once.

DEPENDENCIES
  None

RETURN VALUE
  The signal mask actually set out of the requested mask which terminated
  the wait.

SIDE EFFECTS
  All the set signals in the requested mask are cleared

===========================================================================*/

rex_sigs_type
ps_rm_wait
(
  rex_sigs_type requested_mask             /* mask of signals to wait for */
)
{
  rex_sigs_type set_sigs;                         /* Currently set signals */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    First check to see if one of the signals in the mask is already set.  If
    yes, then clear those signals and return. Otherwise enter into the wait
    only if no requested signal events have occurred
  -------------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  set_sigs = rex_get_sigs( rex_ps_rm_tcb );
#else
  set_sigs = rex_get_sigs( &ps_rm_tcb );
#endif

  while ( (set_sigs & requested_mask) == 0 )
  {
    set_sigs = rex_wait( requested_mask |
                         (  1 << (rex_sigs_type)PS_RM_DOG_HB_REPORT_SIGNAL ) );

    /*-----------------------------------------------------------------------
      If dog heart beat received, ack the beat. Do ping<->pong.
    -----------------------------------------------------------------------*/
    if( set_sigs & ( 1 << (rex_sigs_type)PS_RM_DOG_HB_REPORT_SIGNAL ) )
    {
#ifdef FEATURE_MODEM_RCINIT_PHASE2
      (void)rex_clr_sigs( rex_ps_rm_tcb,
                    (  1 << (rex_sigs_type)PS_RM_DOG_HB_REPORT_SIGNAL ) );
#else
      (void)rex_clr_sigs( &ps_rm_tcb,

                    (  1 << (rex_sigs_type)PS_RM_DOG_HB_REPORT_SIGNAL ) );
#endif
      dog_hb_report( ps_rm_dog_rpt_id );
    }
  }

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  (void)rex_clr_sigs( rex_ps_rm_tcb, ( set_sigs & requested_mask ) );
#else
  (void)rex_clr_sigs( &ps_rm_tcb, ( set_sigs & requested_mask ) );
#endif
  return ( set_sigs & requested_mask );
} /* ps_rm_wait() */

/*=========================================================================

FUNCTION PS_RM_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the RM data task.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_rm_task
(
  uint32 ignored    /* Parameter received from Main Control task - ignored */
)
{
  rex_sigs_type     loop_sigs;      /* Signals to be processed within a task
                                       loop */
  rex_sigs_type     set_sigs = 0;   /* Currently set signals               */
  uint8             loop_cnt;       /* # of times task loop is iterated
                                       on a signal mask                    */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef TEST_FRAMEWORK
  #error code not present
#endif

 //Add TTL 
#ifdef FEATURE_TTL
  TTLBOOTUP(STARTUP_PS_RM_TASK);
#endif

  rex_ps_rm_tcb = rex_self();

#ifdef TEST_FRAMEWORK
  #error code not present
#endif
  rcinit_handshake_startup();

  ps_rm_dog_rpt_id = dog_hb_register_rex(
                (rex_sigs_type)1 << (rex_sigs_type)PS_RM_DOG_HB_REPORT_SIGNAL);

  ps_rm_init();

#ifdef FEATURE_TTL
       TTLBOOTUP(STARTUP_PS_RM_TASK_DONE);
#endif

  /*-------------------------------------------------------------------------
    Main task loop. Never exit!
  -------------------------------------------------------------------------*/
  for( ;;)
  {
    /*-----------------------------------------------------------------------
      Wait for work to do. No return from wait until one of the enabled
      signals is set. Even if we come out of wait, process only those
      signals which are currently enabled.
    -----------------------------------------------------------------------*/

    set_sigs |= rex_get_sigs( rex_ps_rm_tcb );

    set_sigs &= ps_rm_enabled_sig_mask;

    /*-----------------------------------------------------------------------
      Enter into the Wait only if no requested signal events have occurred
    -----------------------------------------------------------------------*/
    if( set_sigs == 0 )
    {
      set_sigs = rex_wait( ps_rm_enabled_sig_mask );
      set_sigs &= ps_rm_enabled_sig_mask;
    }

    (void)rex_clr_sigs( rex_ps_rm_tcb, set_sigs );

    loop_sigs = set_sigs & ~( (rex_sigs_type) 1
                             << (rex_sigs_type) PS_RM_SIG_MASK_CHANGE_SIGNAL );
    set_sigs = 0;

    /*-----------------------------------------------------------------------
      Now handle the active signals one by one in the order of priority.
      If the loop iterates for more than DCC_MAX_SIGNALS times, some signal
      is not handled by this loop. So instead of going in to infinite loop
      perform a check
    -----------------------------------------------------------------------*/
    loop_cnt = 0;
    while( loop_sigs && loop_cnt++ < (rex_sigs_type) PS_RM_MAX_SIGNALS )
    {
      /*---------------------------------------------------------------------
        If dog heart beat received, ack the beat. Do ping<->pong.
      ---------------------------------------------------------------------*/
      if( loop_sigs & ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_DOG_HB_REPORT_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type)PS_RM_DOG_HB_REPORT_SIGNAL );
        dog_hb_report( ps_rm_dog_rpt_id );
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_1 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_1 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_1]->sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_1 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_1]->user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_1);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_2 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_2 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_2]->sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_2 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_2]->user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_2);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_3 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_3 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_3]->sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_3 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_3]->user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_3);
        }
      }
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_4 ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_4 );

        if ( ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_4]->sig_handler
             (
               PS_RM_RMNET_RX_DATA_Q_SIGNAL_4 ,
               ps_rm_sig_handler[PS_RM_RMNET_RX_DATA_Q_SIGNAL_4]->user_data_ptr
             ) == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_RMNET_RX_DATA_Q_SIGNAL_4);
        }
      }      
      /*---------------------------------------------------------------------
        PS Task command queue signal
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_CMD_Q_SIGNAL ) )
      {
        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_CMD_Q_SIGNAL );

        if (ps_rm_process_cmd() == FALSE )
        {
          set_sigs |= ( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_CMD_Q_SIGNAL );
        }
      }

#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      /*---------------------------------------------------------------------
        Check if offline command signal was set.  If set then clear signal,
        and process task offline procedures.
      ---------------------------------------------------------------------*/
      else if( loop_sigs & ( (rex_sigs_type) 1 <<
                             (rex_sigs_type) PS_RM_TASK_OFFLINE_SIGNAL ) )
      {
         DS_MSG0(MSG_LEGACY_ERROR,"ps_rm_task(): "
                                  "PS_RM_TASK_OFFLINE_SIGNAL received");

        loop_sigs &= ~( (rex_sigs_type) 1 <<
                        (rex_sigs_type) PS_RM_TASK_OFFLINE_SIGNAL );
      }
    }

    ASSERT(0 == loop_sigs);
  } /* forever */

} /* ps_rm_task() */
