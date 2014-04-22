#ifndef DS_MUX_TASK_H
#define DS_MUX_TASK_H
/*===========================================================================

                                 D S   M U X   T A S K

                                 H E A D E R   F I L E

DESCRIPTION
  This is the external header file for the MUX Task. This file
  contains all the functions, definitions and data types needed for other
  tasks to interface to the MUX Task.

  Signal and command definitions for the MUX Task, and functions
  that should be used by other tasks to send commands to the MUX
  Task are defined here.

  Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/inc/ds_mux_task.h#1 $
  $DateTime: 2019/11/26 00:18:17 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/16   ad      initial version
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#include "comdef.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*=========================================================================*/
/*!
  @function 
  ds_mux_task

  @brief
  This function is the entry point and main processing loop for the Data
  Service(s) DS_MUX task.

  @details
  This function can only called by the Task main controller to start the 
  DS_MUX task processing loop. 

  @param[in]  dummy - Ignored.

  @dependencies
  None.

  @return
  Does not Return.
*/
/*=========================================================================*/
void ds_mux_task
(
  uint32 dummy
);


#endif /* DS_MUX_TASK_H */
