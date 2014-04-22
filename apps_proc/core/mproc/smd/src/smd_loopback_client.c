/*===========================================================================

 S M D   L O O P B A C K   C L I E N T   A P I   I M P L E M E N T A T I O N

===========================================================================*/
/**
  @file smd_loopback_client.c
*/
/* 
  Defines the private API and structures used to implement the N-way shared 
  memory driver loopback clients.
*/

/*===========================================================================
    Copyright (c) 2011-2014 Qualcomm Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_loopback_client.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/20/14   sm      Allow loopback port closing when state is other than closed.
01/31/14   rv      Replaced the memcpy and memmove functions with their 
                   respective secure memscpy and memsmove functions.
08/06/13   pa      Removing unreachable code warning.
03/21/13   bt      Move smd_get_other_host back to after edge check.
02/22/13   sm      + Change input param of smd_send_interrupt() to 
                     smem_host_type from smd_channel_type.
                   + Added other_host in smd_stream_info_type struct.
02/11/13   bt      Replace string copy macro with smd_string_copy.
11/15/12   bt      Change memcpy to SMD_STRNCPY to avoid buffer overflow.
08/23/12   bt      Reset Closed Nway structure with edge=SMD_NWAY_EDGE_UNUSED.
08/06/12   bt      Replace smd_host_type with smem_host_type.
04/05/12   bt      Remove smd_target.h, move processor defs to smd_proc.h.
05/04/11   bt      Initial implementation.
===========================================================================*/

/*===========================================================================
                   INCLUDE FILES
===========================================================================*/
#include "smd_proc.h"
#include "smd_loopback_client.h"
#include "smd_os_common.h"
#include "smd.h"
#include "smd_os.h"

/*===========================================================================
                   CONSTANT AND VARIABLE DECLARATIONS
===========================================================================*/
/* Handle structure used by the client to keep track of important data 
 * locally. */
typedef struct 
{
  smd_loopback_command_type *cmd_info;
  
  smd_loopback_cb_type  open_cb_fn;
  void                 *open_cb_data;
  smd_loopback_cb_type  close_cb_fn;
  void                 *close_cb_data;
  smd_loopback_cb_type  signal_cb_fn;
  void                 *signal_cb_data;
  
  /* Primarily used to determine correct callback to call with appropriate 
   * arguments when client is interrupted while in particular states. */
  smd_loopback_state_type server_prev_state;
  
  /* True if the client is expecting a response from the server on this 
   * channel, and will respond to a change in server state. */
  boolean                 exp_srvr_resp;
} smd_loopback_handle_struct;

static smd_loopback_handle_struct smd_loopback_handle[
                                      SMD_LOOPBACK_NUM_CHANNELS];
extern smd_loopback_command_type *smd_lb_nway_cmd_reg;
extern uint32                     smd_lb_nway_max_chnls;
extern void                      *smd_lb_nway_cs;

/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
  FUNCTION  smd_loopback_client_update
===========================================================================*/
/**
  Checks every existing N-way SMD Loopback edge and appropriately performs a 
  client state transition or calls a client callback.
  
  Called from smd_main() after all other events have been serviced, if the 
  host processor is a valid client for any interprocessor edge.  Will exit 
  without doing anything if no outstanding response from any server is 
  expected.

  @return
  None.

  @dependencies
  None.
*/
/*==========================================================================*/
void smd_loopback_client_update( void )
{
/* Only Apps or Modem can be a loopback client. */
#if (defined(SMD_APPS_PROC) || defined(SMD_MODEM_PROC))
  smd_loopback_command_type  *lb_cmd_info;
  smd_loopback_handle_struct *handle;
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
    handle = &smd_loopback_handle[i];
    /* Loopback clients only need to service channels that they are 
     * expecting a response from. */
    if (handle->exp_srvr_resp)
    {
      lb_cmd_info = &smd_lb_nway_cmd_reg[i];
      edge = (smd_channel_type)SMD_READ_SMEM_4(&(lb_cmd_info->edge));
      /* Check loopback channels this processor is the client for. */
      if( (edge < SMD_CHANNEL_TYPE_LAST) && 
          (smdi_get_loopback_client(edge) == SMD_THIS_HOST) &&
          ((uint32)(other_host = smd_get_other_host(SMD_THIS_HOST, edge)) 
              < SMEM_NUM_HOSTS))
      {
        smd_cs_lock(smd_lb_nway_cs, TRUE);
        client_state = (smd_loopback_state_type)
                       SMD_READ_SMEM_4(&(lb_cmd_info->client_state));
        server_state = (smd_loopback_state_type)
                       SMD_READ_SMEM_4(&(lb_cmd_info->server_state));
        switch (client_state)
        {
        case SMD_LOOPBACK_STATE_CLOSED:
          if (server_state == SMD_LOOPBACK_STATE_CLOSED)
          {
            if (handle->server_prev_state == SMD_LOOPBACK_STATE_NAK)
            {
              /* smd_loopback_open() failed.  Pass success = FALSE to cb. */
              if (handle->open_cb_fn != NULL)
              {
                handle->open_cb_fn(FALSE, handle->open_cb_data);
              }
              handle->server_prev_state = SMD_LOOPBACK_STATE_CLOSED;
              /* Endpoint states are done transitioning. */
              handle->exp_srvr_resp = FALSE;
            }
            else if (handle->server_prev_state == SMD_LOOPBACK_STATE_OPEN)
            {
              /* smd_loopback_close() succeeded.  Pass success = TRUE to cb. */
              if (handle->close_cb_fn != NULL)
              {
                handle->close_cb_fn(TRUE, handle->close_cb_data);
              }
              /* Loopback channel has been closed, clear the command struct. */
              SMD_MEMSET_SMEM(lb_cmd_info, 0, 
                              sizeof(smd_loopback_command_type));
              SMD_WRITE_SMEM_4(&(lb_cmd_info->edge), SMD_NWAY_EDGE_UNUSED);
              /* Do not clear handle, it will be reset when reused. */
              handle->server_prev_state = SMD_LOOPBACK_STATE_CLOSED;
              /* Endpoint states are done transitioning. */
              handle->exp_srvr_resp = FALSE;
            }
            /* No other transitions are possible. */
          }
          /* Do nothing if server is in any other states (!= CLOSED). */
          break;
        case SMD_LOOPBACK_STATE_OPEN:
          if (server_state == SMD_LOOPBACK_STATE_NAK)
          {
            /* smd_loopback_open() failed.  Set the client side to CLOSED. */
            SMD_WRITE_SMEM_4(&(lb_cmd_info->client_state), 
                             SMD_LOOPBACK_STATE_CLOSED);
            handle->server_prev_state = SMD_LOOPBACK_STATE_NAK;
            SMD_MEMORY_BARRIER();
            /* Interrupt server so it goes to CLOSED state as well. */
            smd_send_interrupt(other_host);
            handle->exp_srvr_resp = TRUE;
          }
          else if (server_state == SMD_LOOPBACK_STATE_OPEN)
          {
            if (handle->server_prev_state == SMD_LOOPBACK_STATE_CLOSED)
            {
              /* smd_loopback_open() succeeded.  Pass success = TRUE to cb. */
              if (handle->open_cb_fn != NULL)
              {
                handle->open_cb_fn(TRUE, handle->open_cb_data);
              }
              handle->server_prev_state = SMD_LOOPBACK_STATE_OPEN;
              /* Endpoint states are done transitioning. */
              handle->exp_srvr_resp = FALSE;
            }
            else if (handle->server_prev_state == SMD_LOOPBACK_STATE_CHANGE_SIG)
            {
              /* smd_loopback_signal() succeeded.  Pass success = TRUE. */
              if (handle->signal_cb_fn != NULL)
              {
                handle->signal_cb_fn(TRUE, handle->signal_cb_data);
              }
              handle->server_prev_state = SMD_LOOPBACK_STATE_OPEN;
              /* Endpoint states are done transitioning. */
              handle->exp_srvr_resp = FALSE;
            }
            /* No other transitions are possible. */
          }
          /* Do nothing if server is in any other states (!= NAK/OPEN). */
          break;
        case SMD_LOOPBACK_STATE_CHANGE_SIG:
          if (server_state == SMD_LOOPBACK_STATE_CHANGE_SIG)
          {
            /* smd_loopback_signal() succeeded.  Set client side to OPEN. */
            SMD_WRITE_SMEM_4(&(lb_cmd_info->client_state), 
                             SMD_LOOPBACK_STATE_OPEN);
            handle->server_prev_state = SMD_LOOPBACK_STATE_CHANGE_SIG;
            SMD_MEMORY_BARRIER();
            /* Interrupt server so it goes back to OPEN state and can take 
             * more commands. */
            smd_send_interrupt(other_host);
            handle->exp_srvr_resp = TRUE;
          }
          /* Do nothing if server is in any other state (only OPEN possible).*/
          break;
        default:
          /* Client should never be in NAK state. */
          break;
        } /* switch ( client_state ) */
        smd_cs_lock(smd_lb_nway_cs, FALSE);
      } /* if ( handle.exp_srvr_resp ) */
    } /* if ( smdi_get_loopback_client(edge) == SMD_THIS_HOST ) */
  } /* for ( i < smd_lb_nway_max_chnls ) */
#endif
} /* smd_loopback_client_update */

/*===========================================================================
  FUNCTION  smd_loopback_open
===========================================================================*/
/**
  Tells the server to open an SMD loopback port identified by the name and 
  edge, with the provided characteristics (buffer size, API, transfer type, 
  and loopback mode).

  When the client host is notified that the server has finished the open 
  operation, the client host will call the client-provided cb_func, using 
  cb_data as a parameter.

  @param[in] name        The name of the SMD loopback port to open.  The 
                         length of the name is defined as
                         SMD_CHANNEL_NAME_SIZE_MAX, where the last character 
                         is always null. Do not use whitespace in port_name.
  @param[in] edge        Indicates which two processors will be communicating
                         on this port.
  @param[in] fifo_size   The SMD FIFO size, in bytes.  Must be a multiple
                         of 32 and must not be greater than #SMD_MAX_FIFO.
                         Some processors may have certain other restrictions.
  @param[in] server_api  Indicates whether SMD memcpy, SMD Lite, or SMD DSM 
                         should be used to implement the server's open, close, 
                         signaling, and read/write functionality.
  @param[in] xfr_type    Packet, Streaming, or legacy mode, if supported by 
                         the server_api.
  @param[in] mode        The server will Echo, Source, or Sink data.
  @param[in] cb_func     The client callback to run once the open finishes 
                         (on failure or success).
  @param[in] cb_data     The client data to pass to the callback, if necessary.

  @return       A handle pointing to the internal data structures used by the
                loopback code on the client side.
                NULL if an error occurred.

  @sideeffect   Initializes a loopback command structure in shared memory.

  @dependencies
              - Shared memory must have been initialized on this processor by
                calling smem_init().
              - The Shared Memory Driver must be up and running. The startup
                process is platform-specific.
*/
/*=========================================================================*/
smd_loopback_handle_type smd_loopback_open
(
  const char               *name,
  smd_channel_type          edge,
  uint32                    fifo_size,
  smd_loopback_api_type     server_api,
  smd_xfrflow_type          xfr_type,
  smd_loopback_mode_type    mode,
  smd_loopback_cb_type      cb_func,
  void                     *cb_data
)
{
  uint32 i;
  /* The first free control struct in smem to use for this new channel. */
  smd_loopback_command_type  *new_cmd_info;
  /* A temporary local copy for ease of copying to smem all at once. */
  smd_loopback_command_type   new_cmd_local;
  smd_loopback_handle_struct *new_lb_handle;
  smem_host_type other_host;
  
  if (smd_lb_nway_max_chnls <= 0 || smd_lb_nway_cmd_reg == NULL || 
      edge >= SMD_CHANNEL_TYPE_LAST || fifo_size > SMD_MAX_FIFO)
  {
    return NULL;
  }

  other_host = smd_get_other_host(SMD_THIS_HOST, edge);
  if ((uint32)other_host >= SMEM_NUM_HOSTS)
  {
    ERR("SMD:%s:%d: Invalid host\n", __FUNCTION__, __LINE__, 0);
    return NULL;
  }

  smd_cs_lock(smd_lb_nway_cs, TRUE);
  /* Find the smd_loopback_command_type struct to use for this name-edge 
   * combination. */
  for (i = 0; i < smd_lb_nway_max_chnls; i++)
  {
    /* If the loopback channel name is empty (NULL), use this control struct.*/
    if (!(*smd_lb_nway_cmd_reg[i].name))
    {
      break;
    }
  }
  if (i == smd_lb_nway_max_chnls)
  {
    /* Ran out of SMD Loopback control structures to allocate! */
    smd_cs_lock(smd_lb_nway_cs, FALSE);
    return NULL;
  }
  /* For the rest of this function, it is important that the value of i does 
   * not change at all, as it is used later for indexing purposes. */
  new_cmd_info = &smd_lb_nway_cmd_reg[i];
  ASSERT(SMD_READ_SMEM_4(&(new_cmd_info->client_state)) == 
          SMD_LOOPBACK_STATE_CLOSED);
  ASSERT(SMD_READ_SMEM_4(&(new_cmd_info->server_state)) == 
          SMD_LOOPBACK_STATE_CLOSED);
  
  memset(&new_cmd_local, 0, sizeof(smd_loopback_command_type));
  /* Set the values in the local control struct based on the arguments. */
  new_cmd_local.client_state = SMD_LOOPBACK_STATE_OPEN;
  new_cmd_local.server_state = SMD_LOOPBACK_STATE_CLOSED;
  (void)smd_string_copy(new_cmd_local.name, name, SMD_CHANNEL_NAME_SIZE_MAX);
  new_cmd_local.edge = edge;
  new_cmd_local.fifo_size = fifo_size;
  new_cmd_local.server_api = server_api;
  new_cmd_local.xfer_type = xfr_type;
  new_cmd_local.loopback_mode = mode;
  /* Server side will set the signals in the control structure when it 
   * performs the open that results from this command, so don't bother with
   * them here on client side. */
  
  /* Then copy the constructed control struct into its place in smem. */
  SMD_MEMCPY_TO_SMEM(new_cmd_info, sizeof(smd_loopback_command_type), 
                     &new_cmd_local, sizeof(smd_loopback_command_type));
  /* We're done with new_cmd_local now. */
  
  /* Ensure the control struct in shared memory is updated. */
  SMD_MEMORY_BARRIER();
  smd_cs_lock(smd_lb_nway_cs, FALSE);
  
  /* Save the cb_func and cb_data and set up the loopback_handle fields. */
  new_lb_handle = &smd_loopback_handle[i];
  new_lb_handle->cmd_info = new_cmd_info;
  new_lb_handle->open_cb_fn = cb_func;
  new_lb_handle->open_cb_data = cb_data;
  new_lb_handle->close_cb_fn = NULL;
  new_lb_handle->close_cb_data = NULL;
  new_lb_handle->signal_cb_fn = NULL;
  new_lb_handle->signal_cb_data = NULL;
  new_lb_handle->server_prev_state = SMD_LOOPBACK_STATE_CLOSED;
  new_lb_handle->exp_srvr_resp = TRUE;
  
  /* Interrupt the server, so it knows there is something for it to do. */
  smd_send_interrupt(other_host);
  
  return (smd_loopback_handle_type)new_lb_handle;
} /* smd_loopback_open */

/*===========================================================================
  FUNCTION  smd_loopback_close
===========================================================================*/
/**
  Tells the server to close an SMD loopback port identified by the handle.

  When the client host is notified that the server has finished the close 
  operation, the client host will call the client-provided cb_func, using 
  cb_data as a parameter.

  @param[in] handle   The handle returned for this port by smd_loopback_open().
  @param[in] cb_func  The client callback to run once the close finishes.
  @param[in] cb_data  The client data to pass to the callback, if necessary.

  @return             True if the command is successfully sent to the server.
                      False if the port is in an inappropriate state to close.

  @dependencies
  The port must have been opened on the server side already.  Both the client
  and server-side loopback states should be in SMD_LOOPBACK_STATE_OPEN.
*/
/*=========================================================================*/
boolean smd_loopback_close
(
  smd_loopback_handle_type  handle,
  smd_loopback_cb_type      cb_func,
  void                     *cb_data
)
{
  smem_host_type other_host;
  smd_loopback_command_type  *cmd_info;
  smd_loopback_handle_struct *handle_struct = 
      (smd_loopback_handle_struct *)handle;
  if (handle_struct->cmd_info == NULL)
  {
    return FALSE;
  }
  cmd_info = handle_struct->cmd_info;

  other_host = smd_get_other_host(SMD_THIS_HOST, 
                   (smd_channel_type)SMD_READ_SMEM_4(&(cmd_info->edge)));
  if ((uint32)other_host >= SMEM_NUM_HOSTS)
  {
    ERR("SMD:%s:%d: Invalid host\n", __FUNCTION__, __LINE__, 0);
    return FALSE;
  }

  smd_cs_lock(smd_lb_nway_cs, TRUE);
  if ((SMD_READ_SMEM_4(&(cmd_info->client_state))==SMD_LOOPBACK_STATE_CLOSED)||
      (SMD_READ_SMEM_4(&(cmd_info->server_state))==SMD_LOOPBACK_STATE_CLOSED))
  {
    smd_cs_lock(smd_lb_nway_cs, FALSE);
    return FALSE;
  }
  SMD_WRITE_SMEM_4(&(cmd_info->client_state), SMD_LOOPBACK_STATE_CLOSED);
  
  /* Ensure the control struct in shared memory is updated. */
  SMD_MEMORY_BARRIER();
  smd_cs_lock(smd_lb_nway_cs, FALSE);
  
  handle_struct->close_cb_fn = cb_func;
  handle_struct->close_cb_data = cb_data;
  handle_struct->exp_srvr_resp = TRUE;
  
  /* Interrupt the server, so it knows there is something for it to do. */
  smd_send_interrupt(other_host);
  
  return TRUE;
} /* smd_loopback_close */

/*===========================================================================
  FUNCTION  smd_loopback_signal
===========================================================================*/
/**
  Tells the server to set a particular signal on its side of an SMD loopback 
  port identified by the handle.  The value may not necessarily be different 
  from the current server signal's value.

  When the client host is notified that the server has finished the signaling 
  operation, the client host will call the client-provided cb_func, using 
  cb_data as a parameter.

  @param[in] handle   The handle returned for this port by smd_loopback_open().
  @param[in] signal   The signal that should be set.
  @param[in] value    The value the signal should be set to.
  @param[in] cb_func  The client callback to run once the signal finishes.
  @param[in] cb_data  The client data to pass to the callback, if necessary.

  @return             True if the command is successfully sent to the server.
                      False if the port is in an inappropriate state to close.

  @dependencies
  The port must have been opened on the server side already.  Both the client
  and server-side loopback states should be in SMD_LOOPBACK_STATE_OPEN.
*/
/*=========================================================================*/
boolean smd_loopback_signal
(
  smd_loopback_handle_type  handle,
  smd_loopback_signal_type  signal,
  boolean                   value,
  smd_loopback_cb_type      cb_func,
  void                     *cb_data
)
{
  smem_host_type other_host;
  smd_loopback_command_type *cmd_info;
  smd_loopback_handle_struct *handle_struct = 
      (smd_loopback_handle_struct *)handle;
  if (handle_struct->cmd_info == NULL)
  {
    return FALSE;
  }
  cmd_info = handle_struct->cmd_info;

  other_host = smd_get_other_host(SMD_THIS_HOST, 
                   (smd_channel_type)SMD_READ_SMEM_4(&(cmd_info->edge)));
  if ((uint32)other_host >= SMEM_NUM_HOSTS)
  {
    ERR("SMD:%s:%d: Invalid host\n", __FUNCTION__, __LINE__, 0);
    return FALSE;
  }

  smd_cs_lock(smd_lb_nway_cs, TRUE);
  if ((SMD_READ_SMEM_4(&(cmd_info->client_state))!= SMD_LOOPBACK_STATE_OPEN) ||
      (SMD_READ_SMEM_4(&(cmd_info->server_state))!= SMD_LOOPBACK_STATE_OPEN))
  {
    smd_cs_lock(smd_lb_nway_cs, FALSE);
    return FALSE;
  }
  
  /* Set the indicated signal to the requested value. */
  switch(signal)
  {
  case SMD_LOOPBACK_SIGNAL_DTR:
    SMD_WRITE_SMEM_1(&(cmd_info->sig_dtr), value);
    break;
  case SMD_LOOPBACK_SIGNAL_CTS:
    SMD_WRITE_SMEM_1(&(cmd_info->sig_cts), value);
    break;
  case SMD_LOOPBACK_SIGNAL_CD:
    SMD_WRITE_SMEM_1(&(cmd_info->sig_cd), value);
    break;
  case SMD_LOOPBACK_SIGNAL_RI:
    SMD_WRITE_SMEM_1(&(cmd_info->sig_ri), value);
    break;
  default:
    break;
  }
  /* Ensure the signals in shared memory are updated before the state is. */
  SMD_MEMORY_BARRIER();
  
  SMD_WRITE_SMEM_4(&(cmd_info->client_state), SMD_LOOPBACK_STATE_CHANGE_SIG);
  /* Ensure the control struct in shared memory is updated. */
  SMD_MEMORY_BARRIER();
  
  smd_cs_lock(smd_lb_nway_cs, FALSE);
  
  handle_struct->signal_cb_fn = cb_func;
  handle_struct->signal_cb_data = cb_data;
  handle_struct->exp_srvr_resp = TRUE;
  
  /* Interrupt the server, so it knows there is something for it to do. */
  smd_send_interrupt(other_host);
  
  return TRUE;
} /* smd_loopback_signal */
