/*==============================================================================

                              ds_appsrv_task.h

GENERAL DESCRIPTION
  Utility functions for appsrv modules to send commands to dstask

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/05/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_APPSRV_TASK_H
#define DS_APPSRV_TASK_H

#include "comdef.h"
#include "data_msg.h"

/* Callback function type to process the dstask command. */
typedef void (*ds_appsrv_process_fcn)(uint32* msg_content_ptr);


/*==============================================================================
FUNCTION ds_appsrv_cmd_init

DESCRIPTION
  Register appsrv command process function to DSTask.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None
==============================================================================*/
void ds_appsrv_cmd_init();


/*==============================================================================
FUNCTION ds_appsrv_init

DESCRIPTION
  Initialize appsrv modules.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None
==============================================================================*/
void ds_appsrv_init();

/*==============================================================================
FUNCTION ds_appsrv_put_cmd

DESCRIPTION
  Puts a command to dstask's command queue. It will call the given process
  function instead of a registered module's process function when dstask sends
  a command to appsrv task.

PARAMETERS
  [In] process_fcn     - Process function to call when dstask sends a command
                         to appsrv task
  [In] msg_content_ptr - Pointer with the object to return in the process
                         function

RETURN VALUE
  Returns true if it successfully registers a command to dstask; otherwise false.

DEPENDENCIES
  None
==============================================================================*/
boolean ds_appsrv_put_cmd(ds_appsrv_process_fcn process_fcn, uint32* msg_content_ptr);


#endif /* DS_APPSRV_TASK_H */
