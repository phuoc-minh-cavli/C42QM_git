/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D S _  M U X _ S V C . C

DESCRIPTION


EXTERNALIZED FUNCTIONS
  ds_mux_enable_sig()
    Adds specific signal for DS_MUX task to process.

  ds_mux_disable_sig()
    Removes specific signal from DS_MUX task processing.

  ds_mux_set_sig_handler()
    Sets signal handler for specific signal processed in DS_MUX task.

  ds_mux_get_cmd_data_buf()
    Allocs a new command buffer to be used by caller.

  ds_mux_free_cmd_data_buf()
    Frees the DS_MUX command buffer.

  ds_mux_send_cmd()
    Post a command to be handled in DS_MUX task.

  ds_mux_set_cmd_handler()
    Sets command handler for command handled in DS_MUX task.


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/src/ds_mux_svc.c#1 $ $DateTime: 2019/11/26 00:18:17 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/07/10    am     created module.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#include "msg.h"
#include "err.h"
#include "dog.h"
#include "ds_mux_defs.h"
#include "ds_mux_taski.h"
#include "ds_mux_svc.h"

extern qti_bridge_qurt_t  qti_bridge_qurt;




/*===========================================================================

                  EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/
/*--------------------------------------------------------------------------
  Dog data structures [extern defs] for PS RM task
---------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION DS_MUX_ENABLE_SIG()

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void 
ds_mux_enable_sig
(
  ds_mux_sig_enum_type sig              /* Signal to be enabled               */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ds_mux_sig_handler[sig].sig_handler == ds_mux_default_sig_handler )
  {
    DS_MUX_LOG_MSG_ERROR_1("ds_mux_enable_sig(): "
                    "Signal %d enabled without a handler", sig );
  }

  bridge_mux_enabled_sig_mask |= (1 <<  sig );

  DS_MUX_SET_SIGNAL( DS_MUX_SIG_MASK_CHANGE_SIGNAL );
} /* ds_mux_enable_sig() */

/*===========================================================================

FUNCTION DS_MUX_DISABLE_SIG()

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
ds_mux_disable_sig
(
  ds_mux_sig_enum_type sig                       /* Signal to be disabled     */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bridge_mux_enabled_sig_mask &= ~( 1 << sig );
} /* ds_mux_disable_sig() */
    
/*===========================================================================

FUNCTION DS_MUX_SET_SIG_HANDLER()

DESCRIPTION
  This function sets a handler for the specified signal.

DEPENDENCIES
  None.

RETURN VALUE
  Previously registered signal handler

SIDE EFFECTS
  None.

===========================================================================*/

ds_mux_sig_handler_type 
ds_mux_set_sig_handler
(
  ds_mux_sig_enum_type sig,             /* Signal assocaited with handler     */
  ds_mux_sig_handler_type sig_handler,  /* Signal handler function pointer    */
  void *user_data_ptr                /* Param to be passed to the handler  */
)
{
  ds_mux_sig_handler_type old_handler = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sig > 0 && sig < DS_MUX_MAX_SIGNALS)
  {
    old_handler = ds_mux_sig_handler[sig].sig_handler;

    ds_mux_sig_handler[sig].sig_handler = ( sig_handler != NULL) ?
                                          sig_handler :
                                          ds_mux_default_sig_handler;

    ds_mux_sig_handler[sig].user_data_ptr = user_data_ptr;
  }

  return old_handler;
} /* ds_mux_set_sig_handler() */


/*===========================================================================

FUNCTION DS_MUX_SEND_CMD()

DESCRIPTION
  This function posts a cmd for processing in DS_MUX task context.  The cmd is
  processed by calling the registered cmd handler, if any.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void 
ds_mux_send_cmd
(
  ds_mux_cmd_enum_type cmd,         /* Actual command to be processed         */
  void *user_data_ptr            /* Command specific user parameters       */
)
{
  ds_mux_cmd_type *cmd_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get a cmd buffer from the free queue, call error routine if no links
    available, else copy buffer to cmd queue and signal DS_MUX task
  -------------------------------------------------------------------------*/
  if( ( cmd_ptr = ( ds_mux_cmd_type * ) q_get( &ds_mux_cmd_free_q ) ) == NULL )
  {
    DS_MUX_LOG_MSG_ERROR_0("ds_mux_send_cmd(): "
                    "DS_MUX task cmd buffers unavailable" );
  }
  else
  {
    cmd_ptr->cmd = cmd;
    cmd_ptr->user_data_ptr = user_data_ptr;

    /*-----------------------------------------------------------------------
      Making the operation atomic so that race conditions do not occur
    -----------------------------------------------------------------------*/
    DS_MUX_ENTER_CRIT_SECTION(&global_ds_mux_crit_section);
    q_put( &ds_mux_cmd_q, &cmd_ptr->link );
    DS_MUX_SET_SIGNAL( DS_MUX_CMD_Q_SIGNAL );
    DS_MUX_LEAVE_CRIT_SECTION(&global_ds_mux_crit_section);
  }
} /* ds_mux_send_cmd() */


/*===========================================================================

FUNCTION DS_MUX_SET_CMD_HANDLER()

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

ds_mux_cmd_handler_type 
ds_mux_set_cmd_handler
(
  ds_mux_cmd_enum_type cmd,              /* Command name                      */
  ds_mux_cmd_handler_type cmd_handler    /* Cmd handler function pointer      */
)
{
  ds_mux_cmd_handler_type old_handler;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  old_handler = ds_mux_cmd_handler[cmd];

  ds_mux_cmd_handler[cmd] = ( cmd_handler != NULL) ?
                          cmd_handler :
                          ds_mux_default_cmd_handler;

  return old_handler;
} /* ds_mux_set_cmd_handler() */
