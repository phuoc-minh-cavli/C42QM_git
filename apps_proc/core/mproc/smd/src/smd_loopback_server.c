/*===========================================================================

 S M D   L O O P B A C K   S E R V E R   A P I   I M P L E M E N T A T I O N

===========================================================================*/
/**
  @file smd_loopback_server.c
*/
/* 
  Defines the private API and structures used to implement the N-way shared 
  memory driver loopback servers.
*/

/*===========================================================================
    Copyright (c) 2011-2013 Qualcomm Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_loopback_server.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/20/14   sm      Allow loopback port closing when state is other than closed.
03/21/13   bt      Move smd_get_other_host back to after edge check.
02/22/13   sm      + Change input param of smd_send_interrupt() to 
                     smem_host_type from smd_channel_type.
                   + Added other_host in smd_stream_info_type struct.
10/05/12   bt      Allow tasked SMDL to return accurate error code.
07/31/12   pa      Enable loopback task for SMDL loopback.
04/05/12   bt      Remove smd_target.h, move processor defs to smd_proc.h.
02/23/12   bt      Resolve pointer-aliasing warning and fix dereference.
05/04/11   bt      Initial implementation.
===========================================================================*/

/*===========================================================================
                   INCLUDE FILES
===========================================================================*/
#include "smd_proc.h"
#include "smd_loopback_server.h"
#include "smd_loopback_server_apis.h"
#include "smd_os_common.h"

/*===========================================================================
                   CONSTANT AND VARIABLE DECLARATIONS
===========================================================================*/
/* Contains the handle information for each loopback server.  The actual type
 * and structure depends on the API used to open the server. */
smd_loopback_info_type smd_loopback_info[SMD_LOOPBACK_NUM_CHANNELS];

/* Used to prevent the server from processing open() and close() requests 
 * multiple times in a row for one port, before the server state is changed 
 * in the callback. */
static boolean state_transition_pending[SMD_LOOPBACK_NUM_CHANNELS];

extern smd_loopback_command_type *smd_lb_nway_cmd_reg;
extern uint32                     smd_lb_nway_max_chnls;
extern void                      *smd_lb_nway_cs;

/*===========================================================================
                   LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION  smd_lb_gen_close_cb
===========================================================================*/
/**
  This is the general server-side close callback operation, and is 
  agnostic to the SMD API used for the port.
  The only thing done is to set the server's state to 
  SMD_LOOPBACK_STATE_CLOSED, indicate that the close transition is done, and 
  notify the client.  Some APIs may require further cleanup after this 
  function is called.
  
  @param[in] idx  The index into the loopback command type register.

  @return         The smd_loopback_command_type structure at index idx.
  
  @sideeffects
  Interrupts the client processor.
*/
/*==========================================================================*/
void smd_lb_gen_close_cb ( uint32 idx )
{
  smem_host_type other_host;
  smd_loopback_command_type *lb_cmd_info;
  
  lb_cmd_info = &smd_lb_nway_cmd_reg[idx];
  
  other_host = smd_get_other_host(SMD_THIS_HOST, 
                (smd_channel_type)SMD_READ_SMEM_4(&(lb_cmd_info->edge)));
  if ((uint32)other_host >= SMEM_NUM_HOSTS)
  {
    ERR("SMD:%s:%d: Invalid host\n", __FUNCTION__, __LINE__, 0);
    return;
  }

  smd_cs_lock(smd_lb_nway_cs, TRUE);
  /* Set server loopback state to CLOSED. */
  SMD_WRITE_SMEM_4(&(lb_cmd_info->server_state), 
                   SMD_LOOPBACK_STATE_CLOSED);
  state_transition_pending[idx] = FALSE;
  /* Ensure the server state in shared memory is updated. */
  SMD_MEMORY_BARRIER();
  smd_cs_lock(smd_lb_nway_cs, FALSE);
  /* Interrupt client so it calls successful close() callback.  Client will 
   * clear the smd_loopback_command_type structure in smem. */
  smd_send_interrupt(other_host);
}

/*-------------------------------------------------------------------------
 *             SMD Loopback Command Local Function Definitions
 *-------------------------------------------------------------------------*/
/*===========================================================================
  FUNCTION  smd_loopback_server_open
===========================================================================*/
/**
  Opens an SMD loopback port, associated with the loopback command structure
  at the indicated index, using the appropriate exported APIs.  The fields in
  the loopback command structure determine the port properties.

  Sets the appropriate server callbacks.  Also sets the signal fields in the 
  loopback command structure based on the expected signals to be set when the 
  port is first opened, which depends on the SMD API used.  Then it changes 
  the server loopback state to SMD_LOOPBACK_STATE_OPEN and notifies the client.

  @param[in] idx  The index at which to find the loopback command structure 
                  for the port to be opened.

  @return    TRUE if the open was successful.
             FALSE if the open fails for any reason, based on the arguments 
             provided in the loopback command structure.

  @sideeffects
  Allocates an internal data structure from heap memory.
*/
/*=========================================================================*/
static boolean smd_loopback_server_open( uint32 idx )
{
  smd_loopback_command_type *lb_cmd_info;
  smd_loopback_info_type    *lb_port_info;
  uint32                     max_fifo_size;
  boolean                    result = FALSE;
  
  lb_cmd_info = &smd_lb_nway_cmd_reg[idx];
  lb_port_info = &smd_loopback_info[idx];
  
#if defined (SMD_MODEM_PROC)
  max_fifo_size = SMD_MAX_FIFO;
#elif defined (SMD_LPASS_PROC) || defined (SMD_RIVA_PROC)
  max_fifo_size = SMD_STANDARD_FIFO;
#else
  max_fifo_size = SMD_MIN_FIFO;
#endif
  
  if (SMD_READ_SMEM_4(&(lb_cmd_info->fifo_size)) > max_fifo_size)
  {
    return FALSE;
  }
  
  switch( SMD_READ_SMEM_1(&(lb_cmd_info->server_api)) )
  {
  case SMD_LOOPBACK_API_MEMCPY:
  {
    if (smd_lb_server_memcpy_open(lb_cmd_info, lb_port_info, idx))
    {
      smd_cs_lock(smd_lb_nway_cs, TRUE);
      /* Set server loopback state to OPEN. */
      SMD_WRITE_SMEM_4(&(lb_cmd_info->server_state), SMD_LOOPBACK_STATE_OPEN);
      /* Ensure the server state in shared memory is updated. */
      SMD_MEMORY_BARRIER();
      smd_cs_lock(smd_lb_nway_cs, FALSE);

      result = TRUE;
    }
    break;
  }
  case SMD_LOOPBACK_API_SMDLITE:
  {
    if (smd_lb_server_lite_event_trigger(idx, 
            (smdl_event_type)SMDL_EVENT_LOOPBACK_OPEN))
    {
      result = TRUE;
    }
    break;
  }
  case SMD_LOOPBACK_API_DSM:
  {
    if (smd_lb_server_dsm_open(lb_cmd_info, lb_port_info, idx))
    {
      smd_cs_lock(smd_lb_nway_cs, TRUE);
      /* Set server loopback state to OPEN. */
      SMD_WRITE_SMEM_4(&(lb_cmd_info->server_state), SMD_LOOPBACK_STATE_OPEN);
      /* Ensure the server state in shared memory is updated. */
      SMD_MEMORY_BARRIER();
      smd_cs_lock(smd_lb_nway_cs, FALSE);

      result = TRUE;
    }
    break;
  }
  default:
    /* Unsupported API type! */
    result = FALSE;
    break;
  }
  
  return result;
}

/*===========================================================================
  FUNCTION  smd_loopback_server_close
===========================================================================*/
/**
  Closes an SMD loopback port, associated with the loopback command structure
  at the indicated index.

  When the server has finished the close operation, the local close callback 
  will run, which sets the server-side loopback state to 
  SMD_LOOPBACK_STATE_CLOSED, notifies the client, and does any needed cleanup.

  @param[in] idx  The index at which to find the loopback command structure 
                  for the port to be closed.

  @return    None.
  
  @sideeffects
  None.
*/
/*=========================================================================*/
static void smd_loopback_server_close( uint32 idx )
{
  smd_loopback_command_type *lb_cmd_info;
  smd_loopback_info_type    *lb_port_info;
  
  lb_cmd_info = &smd_lb_nway_cmd_reg[idx];
  lb_port_info = &smd_loopback_info[idx];
  
  switch( SMD_READ_SMEM_1(&(lb_cmd_info->server_api)) )
  {
  case SMD_LOOPBACK_API_MEMCPY:
  {
    smd_lb_server_memcpy_close(lb_port_info);
    break;
  }
  case SMD_LOOPBACK_API_SMDLITE:
  {
    (void)smd_lb_server_lite_event_trigger(idx, 
              (smdl_event_type)SMDL_EVENT_LOOPBACK_CLOSE);
    break;
  }
  case SMD_LOOPBACK_API_DSM:
  {
    smd_lb_server_dsm_close(lb_port_info);
    break;
  }
  default:
    /* Unsupported API type!  Do nothing. */
    break;
  }
}

/*===========================================================================
  FUNCTION  smd_loopback_server_signal
===========================================================================*/
/**
  Sets all RS-232 signals of an SMD loopback port, associated with the 
  loopback command structure at the indicated index.  The values to set the 
  signals to are obtained from this loopback command structure, which may 
  not necessarily be different from the port's current values.

  Changes the server loopback state to SMD_LOOPBACK_STATE_CHANGE_SIG and
  notifies the client when done.

  @param[in] idx  The index at which to find the loopback command structure 
                  for the port whose signals should change.

  @return    None.
  
  @sideeffects
  Interrupts the client processor immediately.
*/
/*=========================================================================*/
static void smd_loopback_server_signal( uint32 idx )
{
  smem_host_type other_host;
  smd_loopback_command_type *lb_cmd_info;
  smd_loopback_info_type    *lb_port_info;
  
  lb_cmd_info = &smd_lb_nway_cmd_reg[idx];
  lb_port_info = &smd_loopback_info[idx];
  
  other_host = smd_get_other_host(SMD_THIS_HOST, 
                (smd_channel_type)SMD_READ_SMEM_4(&(lb_cmd_info->edge)));
  if ((uint32)other_host >= SMEM_NUM_HOSTS)
  {
    ERR("SMD:%s:%d: Invalid host\n", __FUNCTION__, __LINE__, 0);
    return;
  }

  switch( SMD_READ_SMEM_1(&(lb_cmd_info->server_api)) )
  {
  case SMD_LOOPBACK_API_MEMCPY:
  {
    smd_lb_server_memcpy_signal(lb_cmd_info, lb_port_info);
    break;
  }
  case SMD_LOOPBACK_API_SMDLITE:
  {
    smd_lb_server_lite_signal(lb_cmd_info, lb_port_info);
    break;
  }
  case SMD_LOOPBACK_API_DSM:
  {
    smd_lb_server_dsm_signal(lb_cmd_info, lb_port_info);
    break;
  }
  default:
    /* Unsupported API type!  Do nothing. */
    return;
  }
  smd_cs_lock(smd_lb_nway_cs, TRUE);
  SMD_WRITE_SMEM_4(&(lb_cmd_info->server_state), SMD_LOOPBACK_STATE_CHANGE_SIG);
  /* Ensure the server state in shared memory is updated. */
  SMD_MEMORY_BARRIER();
  smd_cs_lock(smd_lb_nway_cs, FALSE);
  /* Interrupt client to let it know signal() succeeded. */
  smd_send_interrupt(other_host);
}

/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION  smd_loopback_server_update
===========================================================================*/
/**
  Checks every existing N-way SMD Loopback edge and performs any outstanding 
  commands from clients, or appropriate server state transitions.
  
  Called from smd_main() after all other events have been serviced, if the 
  host processor is a valid server for any interprocessor edge.  Will exit 
  without doing anything if no connected client has an outstanding command 
  it wishes the (host) server to perform.  Will not do anything for loopback
  edges for which there is a pending state transition (such as 
  Opened -> Closed).

  @return
  None.

  @dependencies
  None.
*/
/*==========================================================================*/
void smd_loopback_server_update( void )
{
  smd_loopback_command_type *lb_cmd_info;
  uint32 i;
  smd_channel_type edge;
  smem_host_type other_host;
  smd_loopback_state_type client_state, server_state;
  
  if (smd_lb_nway_max_chnls <= 0 || smd_lb_nway_cmd_reg == NULL)
  {
    /* Nway Loopback not initialized, possibly due to version mismatch. */
    return;
  }
  /* Iterate through loopback command registers to see if any changed. */
  for( i = 0; i < smd_lb_nway_max_chnls; i++ )
  {
    lb_cmd_info = &smd_lb_nway_cmd_reg[i];
    edge = (smd_channel_type)SMD_READ_SMEM_4(&(lb_cmd_info->edge));
    /* Check loopback channels this processor is the server for (that exist).
     * Doesn't read whole name, just checks if first 4 bytes are NULL.
     * Also, don't do anything if a state transition is pending and the 
     * callback hasn't finished yet (for close()). */
    if ( ( edge < SMD_CHANNEL_TYPE_LAST ) &&
         ( smdi_get_loopback_server(edge) == SMD_THIS_HOST ) &&
         ( (uint32)(other_host = smd_get_other_host(SMD_THIS_HOST, edge))
            < SMEM_NUM_HOSTS ) &&
         ( SMD_READ_SMEM_1(lb_cmd_info->name) ) && 
         ( !state_transition_pending[i] ) )
    {
      client_state = (smd_loopback_state_type)
                     SMD_READ_SMEM_4(&(lb_cmd_info->client_state));
      server_state = (smd_loopback_state_type)
                     SMD_READ_SMEM_4(&(lb_cmd_info->server_state));
      /* Loopback servers will service channels where the client state is 
       * different from the server's state, except if waiting when client 
       * is OPEN and server is in NAK state. */
      if (client_state != server_state)
      {
        /* Service the loopback command depending on the current client and 
         * server state. */
        switch (client_state)
        {
        case SMD_LOOPBACK_STATE_CLOSED:
          if (server_state == SMD_LOOPBACK_STATE_NAK)
          {
            /* smd_loopback_open() failed.  Return server state to CLOSED. */
            SMD_WRITE_SMEM_4(&(lb_cmd_info->server_state), 
                             SMD_LOOPBACK_STATE_CLOSED);
            /* Ensure the server state in shared memory is updated. */
            SMD_MEMORY_BARRIER();
            /* Interrupt client so it calls failed open() callback. */
            smd_send_interrupt(other_host);
          }
          else if (server_state == SMD_LOOPBACK_STATE_OPEN ||
                   server_state == SMD_LOOPBACK_STATE_CHANGE_SIG)
          {
            /* Client requested a close.  Close callback will change state. */
            state_transition_pending[i] = TRUE;
            smd_loopback_server_close(i);
          }
          /* Do nothing if server is in any other states (!= NAK/OPEN). */
          break;
        case SMD_LOOPBACK_STATE_OPEN:
          if (server_state == SMD_LOOPBACK_STATE_CLOSED)
          {
            /* Client requested an open.  Next state depends on success. */
            state_transition_pending[i] = TRUE;
            if (!(smd_loopback_server_open(i)))
            {
              /* open() failed because of invalid argument combination. */
              SMD_WRITE_SMEM_4(&(lb_cmd_info->server_state), 
                             SMD_LOOPBACK_STATE_NAK);
              SMD_MEMORY_BARRIER();
            }
            state_transition_pending[i] = FALSE;
            /* Interrupt client so it changes to the appropriate state. */
            smd_send_interrupt(other_host);
          }
          else if (server_state == SMD_LOOPBACK_STATE_CHANGE_SIG)
          {
            /* smd_loopback_signal() succeeded. Return server state to OPEN. */
            SMD_WRITE_SMEM_4(&(lb_cmd_info->server_state), 
                             SMD_LOOPBACK_STATE_OPEN);
            /* Ensure the server state in shared memory is updated. */
            SMD_MEMORY_BARRIER();
            /* Interrupt client so it calls successful signal() callback. */
            smd_send_interrupt(other_host);
          }
          /* Do nothing if server is in any other states (!= CLOSED/SIGNAL). */
          break;
        case SMD_LOOPBACK_STATE_CHANGE_SIG:
          if (server_state == SMD_LOOPBACK_STATE_OPEN)
          {
            /* Client requested to change signals.  Transition to SIGNAL. */
            smd_loopback_server_signal(i);
          }
          /* Do nothing if server is in any other states (!= OPEN). */
          break;
        default:
          /* Client should never be in NAK state. */
          break;
        } /* switch ( client_state ) */
      } /* if ( client_state != server_state ) */
    } /* if ( smdi_get_loopback_server(edge) == SMD_THIS_HOST ) && ... */
  } /* for( i < smd_lb_nway_max_chnls ) */
} /* smd_loopback_server_update */
