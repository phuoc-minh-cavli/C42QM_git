/*===========================================================================

            S M D   L O O P B A C K   S E R V E R   T A S K

===========================================================================*/
/**
  @file smd_loopback_server_task.c
*/
/* 
  Defines the private API and structures used to implement the N-way SMD  
  loopback server layer on processors that have an SMD task.
*/
/*===========================================================================
    Copyright (c) 2012-2013 Qualcomm Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_loopback_server_task.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/13   bt      Fix KW warning for case label types.
10/05/12   bt      Initial implementation.
===========================================================================*/
/*===========================================================================
                   INCLUDE FILES
===========================================================================*/
#include "smd_os.h"
#include "smd_proc.h"
#include "smd_os_common.h"
#include "smd_loopback_server.h"
#include "smd_loopback_server_apis.h"

/*===========================================================================
                   CONSTANT AND VARIABLE DECLARATIONS
===========================================================================*/
extern smd_loopback_command_type *smd_lb_nway_cmd_reg;
extern smd_loopback_info_type     smd_loopback_info[];
extern void                      *smd_lb_nway_cs;

/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      smd_lb_server_lite_event_trigger
===========================================================================*/
/**
  Triggers a loopback event to a given loopback port.

  @param[in] port_idx   Index in the loopback structure of the relevant port.
  @param[in] event      The SMDL loopback event to handle.

  @return
  TRUE if the open was successful.
  FALSE if the open fails for any reason, based on the arguments provided in 
  the loopback command structure.

  @dependencies
  None.
*/
/*=========================================================================*/
boolean smd_lb_server_lite_event_trigger(uint32 port_idx, smdl_event_type event)
{
  boolean result = FALSE;
  
  /* On a processor with an SMD task, this is called from the SMD task context,
   * so just do the appropriate event right away. */
  switch((uint32)event)
  {
    case SMDL_EVENT_LOOPBACK_OPEN:
      if (smd_lb_server_lite_open(&smd_lb_nway_cmd_reg[port_idx],
                                  &smd_loopback_info[port_idx],
                                  port_idx))
      {
        smd_cs_lock(smd_lb_nway_cs, TRUE);
        /* Set server loopback state to OPEN. */
        SMD_WRITE_SMEM_4(&(smd_lb_nway_cmd_reg[port_idx].server_state), 
                         SMD_LOOPBACK_STATE_OPEN);
        /* Ensure the server state in shared memory is updated. */
        SMD_MEMORY_BARRIER();
        smd_cs_lock(smd_lb_nway_cs, FALSE);
        
        result = TRUE;
      }
      break;

    case SMDL_EVENT_LOOPBACK_CLOSE:
      smd_lb_server_lite_close(&smd_loopback_info[port_idx]);
      result = TRUE;
      break;
    default:
      break;
  }
  
  return result;
}

/*===========================================================================
  FUNCTION  smd_lb_lite_cb
===========================================================================*/
/**
  Callback for the SMD Lite N-way loopback data ports.

  When data is waiting to be read (event == SMDL_EVENT_READ), this function
  reads the first packet from the port and writes it to the return FIFO. If
  space is not immediately available in the return FIFO, the data will be
  stored until it can be transmitted (event == SMDL_EVENT_WRITE).  Also
  handles SMDL_EVENT_CLOSE_COMPLETE events appropriately by setting the server
  state to SMD_LOOPBACK_STATE_CLOSED and cleaning up the internal lite
  loopback structure.

  @param[in] port     The loopback data port handle.
  @param[in] event    The event signaled by SMD Lite.
  @param[in] data     Pointer to the loopback data port control structure,
                      of type smdl_loop_type.

  @return     None.
*/
/*==========================================================================*/
void smd_lb_lite_cb
(
  smdl_handle_type port,
  smdl_event_type  event,
  void            *data
)
{
  smdl_loop_type *handle = (smdl_loop_type *)data;
  
  /* On a processor with an SMD task, this is called from the SMD task context,
   * so just do the appropriate event right away. */
  switch(event)
  {
    case SMDL_EVENT_OPEN:
      /* Fall through to read case */
    case SMDL_EVENT_READ:
      smd_lb_lite_read_write(handle, handle->rx);
      break;

    case SMDL_EVENT_WRITE:
      smd_lb_lite_read_write(handle->rx, handle);
      break;

    case SMDL_EVENT_CLOSE_COMPLETE:
      /* The SMD Lite API was able to fully close the port on both sides. */
      smd_lb_gen_close_cb(handle->idx);
      /* Free the local data structure used by the SMDL callbacks. */
      smd_lb_lite_free_handle(handle);
      break;
    default:
      /* Ignore the other events.  Server will not react to remote close
       * or signaling events.  Local server opens, closes, and signal changes
       * can only occur by calling the appropriate client-side api. */
      break;
  }
}
