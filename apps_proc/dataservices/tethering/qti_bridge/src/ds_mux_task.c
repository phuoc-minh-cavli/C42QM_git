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

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/src/ds_mux_task.c#1 $ 
  $DateTime: 2019/11/26 00:18:17 $ 
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/16   ad     Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "queue.h"
#include "err.h"
#include "ds_mux.h"
#include "ds_mux_task.h"
#include "ds_mux_taski.h"
#include "ds_mux_svc.h"
#include "ds_mux_platform.h"

/*--------------------------------------------------------------------------
  Defines for DOG. Assuming DOG_DYNAMIC_API feat is ON, since they (core bsp)
  dont allow anymore additions to dog files.
---------------------------------------------------------------------------*/

/*===========================================================================

                           LOCAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION DS_MUX_DOG_HB_SIG_HANDLER()

DESCRIPTION
  This function is registered as the DS_MUX_DOG_HB_REPORT_SIGNAL signal handler.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static boolean ds_mux_dog_hb_sig_handler
(
  ds_mux_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;
} /* ds_mux_dog_hb_sig_handler() */

/*===========================================================================

FUNCTION DS_MUX_CMD_Q_SIG_HANDLER()

DESCRIPTION
  This function is registered as the DS_MUX_CMD_Q_SIGNAL signal handler.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static boolean ds_mux_cmd_q_sig_handler
(
  ds_mux_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
  boolean ret = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ret = ds_mux_process_cmd();

  return ret;
} /* ds_mux_cmd_q_sig_handler() */

/*===========================================================================

FUNCTION DS_MUX_OFFLINE_SIG_HANDLER()

DESCRIPTION
  This function is registered as the DS_MUX_TASK_OFFLINE_SIGNAL signal handler.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static boolean ds_mux_offline_sig_handler
(
  ds_mux_sig_enum_type sig,                   /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MUX_LOG_MSG_INFO2_0("ds_mux_task(): "
                         "DS_MUX_TASK_OFFLINE_SIGNAL received");

  return TRUE;
} /* ds_mux_offline_sig_handler() */


/*===========================================================================
FUNCTION ds_mux_default_sig_init()

DESCRIPTION
  Initialization function for the DS MUX task. This function
  performs the functions needed for the data task to exit disabled state.

DEPENDENCIES
  RM task should have been started.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_mux_default_sig_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Set signal hanlder for operation of all signals internal to DS_MUX Task
  -------------------------------------------------------------------------*/

  DS_MUX_ENTER_CRIT_SECTION(&global_ds_mux_crit_section);

  ds_mux_set_sig_handler(DS_MUX_DOG_HB_REPORT_SIGNAL, ds_mux_dog_hb_sig_handler, NULL);
  ds_mux_set_sig_handler(DS_MUX_CMD_Q_SIGNAL, ds_mux_cmd_q_sig_handler, NULL);
  ds_mux_set_sig_handler(DS_MUX_TASK_OFFLINE_SIGNAL, ds_mux_offline_sig_handler, NULL);

  DS_MUX_LEAVE_CRIT_SECTION(&global_ds_mux_crit_section);

} /* ds_mux_default_sig_init() */

/*===========================================================================

                     EXTERNAL FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================
FUNCTION DS_MUX_TASK()

DESCRIPTION
  This function is the entry point and main processing loop for the DS MUX TASK.

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_mux_init()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
//  rex_ds_mux_tcb = rex_self();

  ds_mux_default_sig_init();

  ds_muxi_init();
}/*ds_mux_init*/

