#ifndef DS_MUX_SVC_H
#define DS_MUX_SVC_H
/*===========================================================================

                        D S _ M U X _ S V C . H

DESCRIPTION

  Rm Data processing Task services header file. All definitions, structures,
  and functions needed for performing DS_MUX task specific services.
  
EXTERNALIZED FUNCTIONS  
  
  ds_mux_enable_sig()
    Adds specific signal for DS_MUX task to process.

  ds_mux_disable_sig()
    Removes specific signal from DS_MUX task processing.

  ds_mux_set_sig_handler()
    Sets signal handler for specific signal processed in DS_MUX task.

  ds_mux_send_cmd()
    Post a command to be handled in DS_MUX task.

  ds_mux_set_cmd_handler()
    Sets command handler for command handled in DS_MUX task.

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2016 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/inc/ds_mux_svc.h#1 $ 
  $DateTime: 2019/11/26 00:18:17 $ $Author: pwbldsvc $

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "ds_mux_defs.h"
#include "qti_bridge.h"


/*===========================================================================

                      EXTERN DECLARATIONS

===========================================================================*/



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DS_MUX_ENABLE_SIG()

DESCRIPTION
  This function enables the specified signal by adding it to the mask
  which the DS_MUXtask uses for rex_wait().

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void 
ds_mux_enable_sig
(
  ds_mux_sig_enum_type sig                       /* Signal to be enabled      */
);

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
);

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
);

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
);

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
  ds_mux_cmd_enum_type cmd,             /* Command name                      */
  ds_mux_cmd_handler_type cmd_handler   /* Cmd handler function pointer      */
);

#ifdef __cplusplus
}
#endif


#endif /* DS_MUX_SVC_H */
