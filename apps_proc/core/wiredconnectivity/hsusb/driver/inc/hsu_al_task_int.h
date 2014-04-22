#ifndef HSU_AL_TASK_INT_H
#define HSU_AL_TASK_INT_H

/*=============================================================================

                          H S U _ A L _ T A S K _ I N T . H

=============================================================================*/
/**
@file hsu_al_task_int.h

  Contians the High-Speed USB implementation of the Adaptation Layer
  Command Task.

  The Adaptation Layer Command Task is a REX task in charge of executing 
  High-Speed USB adaptation layer commands. An adaptation layer command is a
  code section that must be executed after the occurrence of an interrupt, but
  cannot be executed within the interrupt context itself (for example, when
  this code section is blocking). Therefore, the interrupt service routine
  queues a command in the commands queue. Later, another task (the Adaptation
  Layer task) retrieves this item from the command queue and executes the
  command outside the interrupt context.

  This task manages a cyclic queue of commands. Each queue entry contains a
  function address and an actual parameter for this function, which is of the
  type void*. When commands are inserted into the queue (using the
  hsu_al_task_enqueue_cmd function), a signal is sent to this task to awaken
  it. This task then processes all the queued commands in a FIFO order. For
  each queued command, this task calls the supplied function, passing it its
  argument.

  While this task is processing command queue items, the Giant mutex is locked.
  When this task finishes processing the queue (i.e., it becomes empty again),
  this task releases the Giant mutex and resumes waiting for its signal.

  The command queue is implemented using a cyclic buffer that has a head index
  and a tail index. The tail index points to the location of the next inserted
  item, and the head index points to the location of the next popped item. When
  head index == tail index, the queue is empty, and when head index == tail
  index + 1, the queue is full. This way, the queue can be managed without
  mutex protection, since no item counter is needed and there is no
  simultaneous access to the same end of the queue.

  Externalized functions
  - hsu_al_task_main is externalized outside of the HS-USB stack
    adaptation layer.

  - hsu_al_task_enqueue_cmd is externalized internally in the HS-USB
    stack adaptation layer.
   
  Initialization and sequencing requirements:\n
  The HS-USB adaptation layer task must be created before using the
  API defined in this file.
*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_al_task_int.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who     what, where, why
  --------  ---     -----------------------------------------------------------
  12/20/10  rg      Created.
     
=============================================================================*/

/*=============================================================================
Copyright (c) 2006-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hsu_al_task.h"

#include "dsm.h"
#include "hsu_al_mem.h"


/** @ingroup hsusb_qc_api
    @{
*/

/*=============================================================================

                  LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================
  Constants and Macros
=============================================================================*/

/*=============================================================================
  Externalized Variables
=============================================================================*/

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================
FUNCTION    hsu_al_task_enqueue_cmd_ex
=============================================================================*/
/**
  Enters a new command into the HS-USB adaptation layer command queue
  for execution by the High-Speed USB adaptation layer task.

@param command_func         [IN] -- Command to handle.
@param *command_arg         [IN] -- Parameter to provide to the handler.
@param acquire_giant_mutex  [IN] -- Whether to lock the giant mutex before calling the command

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_al_task_enqueue_cmd_ex(hsu_al_task_cmd_func_type  command_func,
                             void*                      command_arg,
                             boolean                    acquire_giant_mutex);


/*===========================================================================
FUNCTION hsu_al_task_free_dsm

DESCRIPTION
  Put dsm item into dsm free queue in AL task. 

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_free_dsm(dsm_item_type * dsm_ptr);

/*===========================================================================
FUNCTION hsu_al_task_forward_rx_dsm

DESCRIPTION
  Put dsm item into dsm rx queue in AL task. 

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_forward_rx_dsm(rb_t * rb);

/*===========================================================================
FUNCTION hsu_al_task_forward_ecm_control_rx_dsm

DESCRIPTION
  Put dsm item into ecm control rx queue in AL task. 

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_forward_ecm_control_rx_dsm(dsm_item_type * dsm_ptr);

/*===========================================================================
FUNCTION hsu_al_task_forward_ser_rx_dsm

DESCRIPTION
  Put dsm item into SER rx queue in AL task. 

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_forward_ser_rx_dsm(dsm_item_type * dsm_ptr);


/*===========================================================================
FUNCTION hsu_al_task_forward_acm_rx_dsm

DESCRIPTION
  Put dsm item into ACM rx queue in AL task. 

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_forward_acm_rx_dsm(dsm_item_type * dsm_ptr);

/*===========================================================================
FUNCTION hsu_al_task_signal_remote_wakeup

DESCRIPTION
  Sends signal to AL TASK to trigger remote wake-up

DEPENDENCIES
  None.

RETURN VALUE
  None.  

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_al_task_signal_remote_wakeup(void);

/** @} */ /* endgroup hsusb_qc_api */

#endif /* HSU_AL_TASK_H */
