/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D S   M U X   T A S K

GENERAL DESCRIPTION
  This file contains all the functions, definitions and data types needed 
  for other tasks to interface to the MUX Task.

  Signal and command definitions for the MUX Task, and functions
  that should be used by other tasks to send commands to the MUX 
  Task are defined here.

  Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.  
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/src/ds_mux_taski.c#1 $ 
  $DateTime: 2019/11/26 00:18:17 $ 
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/02/13   pkp     Dog heart beat implementation.
10/01/11   bvd     Adding featurization for rcinit
08/17/09   spn     Created Module
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "dog.h"
#include "ds_mux_taski.h"
#include "ds_mux_defs.h"
#include "ds_mux_svc.h"
#include "qurt_signal.h"
#include "qurt_thread.h"
#include "qurt_error.h"
#include "qurt_mutex.h"

extern qti_bridge_qurt_t  qti_bridge_qurt;



/*===========================================================================

                  GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------
  Timers needed for the DataCommon Ctrl Task.
---------------------------------------------------------------------------*/
//rex_timer_type ds_mux_dog_rpt_timer;        /* for watchdog reporting  */

q_type ds_mux_cmd_q;
q_type ds_mux_cmd_free_q;

/*--------------------------------------------------------------------------
  Data types to hold signal and cmd handlers
---------------------------------------------------------------------------*/
ds_mux_sig_handler_info_type ds_mux_sig_handler[DS_MUX_MAX_SIGNALS];
ds_mux_cmd_handler_type      ds_mux_cmd_handler[DS_MUX_MAX_DEFINED_CMD_TYPES];

/*--------------------------------------------------------------------------
  Protocol Stack Task Command links and queues
---------------------------------------------------------------------------*/
ds_mux_cmd_type ds_mux_cmd_buf[DS_MUX_CMD_BUF_CNT];

/*--------------------------------------------------------------------------
DS MUX critical section
---------------------------------------------------------------------------*/
qurt_mutex_t  global_ds_mux_crit_section;


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION DS_MUX_DEFAULT_SIG_HANDLER()

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
ds_mux_default_sig_handler
(
  ds_mux_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MUX_LOG_MSG_ERROR_1("ds_mux_default_sig_handler(): "
                  "Signal %d, no handler registered", sig);

  /*-------------------------------------------------------------------------
  Return TRUE since we don't want this signal to be enqueued again.
-------------------------------------------------------------------------*/
  return TRUE;
} /* ds_mux_default_sig_handler() */

/*===========================================================================

FUNCTION DS_MUX_DEFAULT_CMD_HANDLER()

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
ds_mux_default_cmd_handler
(
  ds_mux_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MUX_LOG_MSG_ERROR_1("ds_mux_default_cmd_handler(): "
                  "Command %d, no handler registered", cmd );
} /* ds_mux_default_cmd_handler() */

/*===========================================================================
FUNCTION DS_MUX_POWERUP()

DESCRIPTION
  Powerup (coldstart) initialization function for the DS_MUX task. 
  
  This function performs initialization before DS_MUX task is started.

DEPENDENCIES
  This function must be called only once at system powerup.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void 
ds_mux_powerup
(
  void
)
{    
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MUX_LOG_MSG_INFO2_0("ds_mux_powerup(): "
                         "Data Common Control Task Powerup");

  /*------------------------------------------------------------------------
    Perform the power up initialization for all other modules that run in
    NAT context
  -------------------------------------------------------------------------*/
  DS_MUX_INIT_CRIT_SECTION(&global_ds_mux_crit_section);

  /*-------------------------------------------------------------------------
    Initialize the command queues and link items onto the command free queue
  -------------------------------------------------------------------------*/
  (void) q_init( &ds_mux_cmd_q );
  (void) q_init( &ds_mux_cmd_free_q );

  for( i = 0; i < DS_MUX_CMD_BUF_CNT; i++ )
  {
    q_put( &ds_mux_cmd_free_q, q_link( &ds_mux_cmd_buf[i], &ds_mux_cmd_buf[i].link) );
  }

  /*------------------------------------------------------------------------
    Initialize all the signal and cmd handlers
  -------------------------------------------------------------------------*/
  for( i = 0; i < (int) DS_MUX_MAX_SIGNALS; i++ )
  {
    ds_mux_sig_handler[i].sig_handler = ds_mux_default_sig_handler;
    ds_mux_sig_handler[i].user_data_ptr = NULL;
  }

  for( i = 0; i < (int) DS_MUX_MAX_DEFINED_CMD_TYPES; i++ )
  {
    ds_mux_cmd_handler[i] = ds_mux_default_cmd_handler;
  }

  /*------------------------------------------------------------------------
    Set signal mask for operation of all signals internal to DS_MUX Task
  -------------------------------------------------------------------------*/
  bridge_mux_enabled_sig_mask = ((1 << DS_MUX_TASK_STOP_SIGNAL ) |
                                 (1 << DS_MUX_TASK_OFFLINE_SIGNAL ) |
                                 (1 << DS_MUX_CMD_Q_SIGNAL ) |
                                 (1 << DS_MUX_DOG_HB_REPORT_SIGNAL ) |
                                 (1 << DS_MUX_SIG_MASK_CHANGE_SIGNAL ));

} /* ds_mux_powerup() */

#ifdef TEST_FRAMEWORK
/*===========================================================================
FUNCTION DS_MUX_POWERDOWN()

DESCRIPTION
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_mux_powerdown
(
  void
)
{
  DS_MUX_DESTROY_CRIT_SECTION(&global_ds_mux_crit_section);
}
#endif /*TEST_FRAMEWORK*/


/*===========================================================================

FUNCTION DS_MUX_PROCESS_CMD()

DESCRIPTION
  This function is called in response to DS_MUX_CMD_Q_SIGNAL.  It retrieves the
  next command from the NAT command queue and processes the command by calling 
  the registered cmd handler.

DEPENDENCIES
    An item must be on the NAT command queue or an DS_MUX_LOG_MSG_ERROR will occur.

RETURN VALUE
  FALSE if cmd queue contains more commands to process and this function
  should be called again, TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean 
ds_mux_process_cmd
(
  void
)
{
  static byte       ds_mux_cmds_proc_hi_score = 0;
  ds_mux_cmd_type * cmd_ptr;
  byte              processed;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    DS_MUX_LOG_MSG_ERROR if no command is available.
  -------------------------------------------------------------------------*/
  if( q_cnt( &ds_mux_cmd_q ) == 0)
  {
    DS_MUX_LOG_MSG_ERROR_0("ds_mux_process_cmd(): "
                    "No command to process" );
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Process all commands in the command q.

    Get each command item in sequence (from the head) of the command queue
    and call the correponding handler with appropriate parameters.
  -------------------------------------------------------------------------*/
  processed = 0;
  while( (cmd_ptr = ( ds_mux_cmd_type * ) q_get( &ds_mux_cmd_q )) != NULL )
  {
    if( cmd_ptr->cmd >= DS_MUX_MAX_DEFINED_CMD_TYPES )
    {
      DS_MUX_LOG_MSG_ERROR_1("ds_mux_process_cmd(): "
                      "Invalid cmd id 0x%x in Q", cmd_ptr->cmd);
      q_put( &ds_mux_cmd_free_q, &cmd_ptr->link );
      continue;
    }
    processed++;
    ds_mux_cmd_handler[cmd_ptr->cmd]( cmd_ptr->cmd, cmd_ptr->user_data_ptr );

    q_put( &ds_mux_cmd_free_q, &cmd_ptr->link );
  }

  /*-------------------------------------------------------------------------
    Intlock to assure atomic operation below.

    Note this also ensures no other task will be scheduled since
    q_cnt() and rex_clr_sigs() cannot cause task context switch.
  -------------------------------------------------------------------------*/
  DS_MUX_ENTER_CRIT_SECTION(&global_ds_mux_crit_section);

  /*-------------------------------------------------------------------------
    Clear the cmd queue signal (which may have been set if a command was
    issued after the rex_clr_sigs() in the outer NAT processing loop)
    Check if any commands are left before doing this, since a command may
    have been posted between closing brace and INTLOCK above.
  -------------------------------------------------------------------------*/
  if( q_cnt( &ds_mux_cmd_q ) == 0 )
  {
    DS_MUX_CLR_SIGNAL(DS_MUX_CMD_Q_SIGNAL);
  }

  /*-------------------------------------------------------------------------
    End atomic operation - Restore interrupts.
  -------------------------------------------------------------------------*/
  DS_MUX_LEAVE_CRIT_SECTION(&global_ds_mux_crit_section);

  /*-------------------------------------------------------------------------
    For debugging, show if we just processed a ton of commands
  -------------------------------------------------------------------------*/
  if( processed > ds_mux_cmds_proc_hi_score )
  {
    ds_mux_cmds_proc_hi_score = processed;
    DS_MUX_LOG_MSG_INFO2_1 ("ds_mux_process_cmd(): "
                     "New high #  commands processed: %d", processed);
  }

  /*-------------------------------------------------------------------------
    Always return true to clear NAT_CMD_SIG from loop_sigs in case command
    was sent after clearing signal above, so that we never get into the
    situation where we process a command due to loop_sigs, emptying the
    cmd queue, and yet the NAT_CMD_SIG is set, leading to DS_MUX_LOG_MSG_ERROR above.
  -------------------------------------------------------------------------*/
  return TRUE;

} /* ds_mux_process_cmd() */


