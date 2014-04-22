/*
 * @file smp2p.c
 *
 * Public API and data structures for use with SMP2P.
 */

/*==============================================================================
     Copyright (c) 2012-2016 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smp2p/src/smp2p.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/04/16   rv      Added smp2p_read_ex, to support single threaded mode.
09/07/15   rv      Moved OS timetick init from smp2p_os_log() to smp2p_init()
08/21/15   bc      treat secure process edge as invalid as smp2p is not supported
                   in secure processor
04/07/15   db      Added SMP2P support for TZ
04/11/14   rv      Valid destination/source host parameter.
01/31/14   rv      Replaced the memcpy and memmove functions with their 
                   respective secure memscpy and memsmove functions.
09/16/13   pa/bt   Add RESET_LOCAL state and reset handshake.
08/06/13   pa      Added smp2p_modify_ex.
09/17/13   pa      Assign header_rx in smp2p_register within local lock.
06/26/13   pa      Fix race condition in smp2p_register.
03/28/13   rs      Moved generic queue functions to SMEM  
02/20/13   pa      Fixing bug in create() in which the context was not fully
                   initialized when the entry already exists (ie SSR case). 
01/25/13   pa      Fixing klocwork issues.
10/10/12   pa      Initial revision.
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smp2p_internal.h"
#include "smem.h"
#include "smem_list.h"
#include "assert.h"

/*===========================================================================
                        GLOBAL DATA
===========================================================================*/
/* Global info array
 * Contains all the info about the SMP2P from this processor's point of view. */
smp2p_info_type smp2p_info[SMEM_NUM_HOSTS];

/* Tx pending list.
 * Holds context structs for active entries */
smem_list_type smp2p_tx_active_list[SMEM_NUM_HOSTS];

/* Tx pending list.
 * Holds context structs which are pending the completion of the negotiation. */
smem_list_type smp2p_tx_pending_list[SMEM_NUM_HOSTS];

/* Rx pending list.
 * Holds context structs for active entries */
smem_list_type smp2p_rx_active_list[SMEM_NUM_HOSTS];

/* Rx pending list.
 * Holds context structs which are pending entry creation by remote endpoint. */
smem_list_type smp2p_rx_pending_list[SMEM_NUM_HOSTS];

/* Pointer to critical section for the SMP2P local log */
void * smp2p_log_cs;

/*===========================================================================
                        FUNCTION DEFINITONS
===========================================================================*/

/*===========================================================================
FUNCTION      smp2p_init
===========================================================================*/
/**
  Initializes the SMP2P internal structures.

  @return
  None.

  @dependencies
  SMEM is initialized.
*/
/*=========================================================================*/
void smp2p_init(void)
{
  uint32 i;

  /* zero init the info struct */
  memset(smp2p_info, 0, sizeof(smp2p_info));

  for (i=0; i<SMEM_NUM_HOSTS; i++)
  {
    /* initialize pending and active lists */
    smem_list_init(&smp2p_tx_active_list[i]);
    smem_list_init(&smp2p_tx_pending_list[i]);
    smem_list_init(&smp2p_rx_active_list[i]);
    smem_list_init(&smp2p_rx_pending_list[i]);

    /* initialize critical sections */
    smp2p_info[i].cs = smp2p_os_cs_create();
    ASSERT(smp2p_info[i].cs);
  }

  /* initialize the timetick */
  smp2p_os_timetick_init();
      
  /* initialize the critical section for the debug log */
  smp2p_log_cs = smp2p_os_cs_create();
  ASSERT(smp2p_log_cs);

  /* build up the smp2p interrupt table */
  smp2p_target_init_interrupts();

  /* enable interrupts */
  smp2p_os_register_interrupt();

  /* start loopback tasks */
  //smp2p_os_start_loopback_tasks();
}

/*===========================================================================
  FUNCTION  smp2p_create
===========================================================================*/
/**
  Creates one SMP2P 32 bit entry on behalf of the client to the specified
  destination.

  Clients may call this routine multiple times to create multiple entries.

  If the SMP2P connection has not been opened, that process will be initiated
  and this function will return a status code indicating that initialization
  is in progress.

  The client may call smp2p_write when either of the following two has occurred:
    - This function returns success.
    - The client has called the wait API.

  @param [out]  ctx       Pointer to pointer of SMP2P context.
                          Context will be created and this pointer will be made
                          to point to it.
  @param [in]   dest      Destination processor ID.
  @param [in]   name      16 byte name string.
                          Must be unique for outbound entries to the specified
                          endpoint.

  @return
  SMP2P_STATUS_SUCCESS       - Entry is created and ready to accept write data.
  SMP2P_STATUS_NOT_INIT      - Connection is opening. Client must call wait API
                               before writing data.
  SMP2P_STATUS_DUPLICATE     - Duplicate name found.
  SMP2P_STATUS_INVALID_PARAM - Invalid parameter.
  Other non-success status   - Error occurred.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2p_create
(
  smp2p_context_type ** ctx,
  smem_host_type        dest,
  char *                name
)
{
  int32 status = SMP2P_STATUS_NOT_INIT;
  int32 name_len;
  char name_buf[SMP2P_NAME_STRING_LEN];
  void * ctx_temp;

  /* validate input param */
  if (dest >= SMEM_NUM_HOSTS || 
      (dest == SMP2P_THIS_HOST && smp2p_info[dest].header_rx == NULL)) 
  {
    return SMP2P_STATUS_INVALID_PARAM;
  }

  /* set up name buffer.
   * truncate the name string if it is longer than allowed. */
  memset(name_buf, 0, sizeof(name_buf));
  name_len = strlcpy(name_buf, name, sizeof(name_buf));

  /* don't allow zero length name strings */
  if (0 == name_len)
  {
    return SMP2P_STATUS_INVALID_PARAM;
  }

  SMP2P_LOG(SMP2P_LOG_EVENT_CREATE, dest, 0, 0);

  /* allocate the context buffer right away.  
   * this must be done outside of critical section on some procs (wcnss).
   * upon any failure, it will be freed outside of the lock. */
  ctx_temp = smp2p_os_malloc(sizeof(**ctx));
  if (NULL == ctx_temp)
  {
    return SMP2P_STATUS_FAILURE;
  }

  /* enter critical section to protect global lists and smem item */
  smp2p_os_cs_lock(smp2p_info[dest].cs);

  /* if state is open, check the active list for duplicate */
  if (SMP2P_STATE_OPEN == smp2p_info[dest].state)
  {
    smp2p_context_type * active_entry;

    /* scan the active list until the end, or a match is found */
    active_entry = smem_list_first(&smp2p_tx_active_list[dest]);
    while (active_entry)
    {
      if (!strncmp(active_entry->name, name_buf, SMP2P_NAME_STRING_LEN))
      {
        break;
      }

      /* point to next entry */
      active_entry = smem_list_next(active_entry);
    }

    /* if entry is already present, return duplicate status.
     * otherwise, just continue to create the context and put it on the 
     * active list. */
    if (active_entry)
    {
      smp2p_os_cs_unlock(smp2p_info[dest].cs);
      smp2p_os_free(ctx_temp);
      return SMP2P_STATUS_DUPLICATE;
    }
  }

  /* if the connection is not open, put the context on the pending list */
  else
  {
    smp2p_context_type * pending_entry;

    /* check up front if we have enough room in the queue */
    if (SMP2P_ENTRIES_TOTAL < smem_list_count(&smp2p_tx_pending_list[dest]))
    {
      smp2p_os_cs_unlock(smp2p_info[dest].cs);
      smp2p_os_free(ctx_temp);
      return SMP2P_STATUS_OUT_OF_RESOURCES;
    }

    /* scan the pending list until the end, or a match is found */
    pending_entry = smem_list_first(&smp2p_tx_pending_list[dest]);
    while (pending_entry)
    {
      if (!strncmp(pending_entry->name, name_buf, SMP2P_NAME_STRING_LEN))
      {
        break;
      }

      /* point to next entry */
      pending_entry = smem_list_next(pending_entry);
    }

    /* check that name does not appear in tx pending list.
     * if the entry is null, means we do not have a match */
    if (pending_entry)
    {
      smp2p_os_cs_unlock(smp2p_info[dest].cs);
      smp2p_os_free(ctx_temp);
      return SMP2P_STATUS_DUPLICATE;
    }

    /* call internal function to do the negotiation. */
    if (SMP2P_STATE_OPEN != smp2p_info[dest].state)
    {
      status = smp2pi_negotiate(dest);
    }
  }

  /* create context */
  *ctx = ctx_temp;

  /* init the context data */
  (*ctx)->index     = smp2pi_find_entry_index(smp2p_info[dest].header_tx, name_buf);
  (*ctx)->remote_id = dest;
  (*ctx)->dir       = SMP2P_DIR_TX;

  /* init the name */
  smp2p_mem_copy((*ctx)->name, sizeof((*ctx)->name), name_buf, 
                 sizeof(name_buf));

  /* initialize the OS event */
  smp2p_os_init_event(&(*ctx)->os_params);

  /* update the debug info */
  smp2p_info[dest].debug_info.size_local += sizeof(**ctx);

  if (SMP2P_STATE_OPEN == smp2p_info[dest].state)
  {
    /* put entry into the active list */
    smem_list_append(&smp2p_tx_active_list[dest], *ctx);

    /* add the entry into smem. 
     * negative index value means that the entry does not exist yet in smem.
     * create it and notify remote side with an interrupt. */
    if ((*ctx)->index < 0)
    {
      status = smp2pi_add_tx_entry(*ctx);
      ASSERT(SMP2P_STATUS_SUCCESS == status);

      /* send interrupt to notify remote endpoint of new entry */
      smp2p_os_send_interrupt(dest);
    }

    status = SMP2P_STATUS_SUCCESS;
  }

  /* if negotiation is not complete, put the entry on the pending list */
  else
  {
    ASSERT(SMP2P_STATE_INIT        == smp2p_info[dest].state || 
           SMP2P_STATE_RESET_LOCAL == smp2p_info[dest].state);

    /* put entry into the pending list */
    smem_list_append(&smp2p_tx_pending_list[dest], *ctx);
  }

  smp2p_os_cs_unlock(smp2p_info[dest].cs);

  return status;
}

/*===========================================================================
  FUNCTION  smp2p_write
===========================================================================*/
/**
  Writes data to the SMP2P entry and notifies remote endpoint. Entry location is
  determined by contents of context parameter.

  @param [in]   ctx       Pointer to SMP2P context.
  @param [in]   data      Data to be written.

  @return
  SMP2P_STATUS_SUCCESS       - Operation successful.
  SMP2P_STATUS_NOT_INIT      - Connection is opening and is not ready for data.
  SMP2P_STATUS_INVALID_PARAM - Invalid parameter.

  @dependencies
  Context must be initialized.
*/
/*=========================================================================*/
int32 smp2p_write
(
  smp2p_context_type * ctx,
  uint32               data
)
{
  return smp2p_modify_ex(ctx, 0xffffffff, data, SMP2P_FLAGS_NONE);
}

/*===========================================================================
  FUNCTION  smp2p_modify
===========================================================================*/
/**
  Performs read-modify-write bits in the entry specified, then notifies remote
  endpoint.  Clear mask is applied before set mask.

  @param[in]  context   Pointer to SMP2P context.
  @param[in]  clr_mask  Bit mask to use when clearing bits.
  @param[in]  set_mask  Bit mask to use when setting bits.

  @return
  SMP2P_STATUS_SUCCESS       - Operation successful.
  SMP2P_STATUS_NOT_INIT      - Connection is opening and is not ready for data.
  SMP2P_STATUS_INVALID_PARAM - Invalid parameter.

  @dependencies
  Context must be initialized.
*/
/*=========================================================================*/
int32 smp2p_modify
(
  smp2p_context_type * ctx,
  uint32               clr_mask,
  uint32               set_mask
)
{
  return smp2p_modify_ex(ctx, clr_mask, set_mask, SMP2P_FLAGS_NONE);
}

/*===========================================================================
  FUNCTION  smp2p_modify_ex
===========================================================================*/
/**
  Performs read-modify-write bits in the entry specified, then notifies remote
  endpoint.  Clear mask is applied before set mask.

  @param[in]  context   Pointer to SMP2P context.
  @param[in]  clr_mask  Bit mask to use when clearing bits.
  @param[in]  set_mask  Bit mask to use when setting bits.
  @param[in]  flags     Flags parameter.
                        SMP2P_FLAGS_NO_INTR disables sending an interrupt 
                          following the SMP2P entry update.
                        SMP2P_FLAGS_STM is used in single threaded mode and 
                          no OS locks are taken.
                        SMP2P_FLAGS_NONE is normal behavior.

  @return
  SMP2P_STATUS_SUCCESS       - Operation successful.
  SMP2P_STATUS_NOT_INIT      - Connection is opening and is not ready for data.
  SMP2P_STATUS_INVALID_PARAM - Invalid parameter.

  @dependencies
  Context must be initialized.
*/
/*=========================================================================*/
int32 smp2p_modify_ex
(
  smp2p_context_type * ctx,
  uint32               clr_mask,
  uint32               set_mask,
  uint32               flags
)
{
  int32 status;

  /* do param check on context */
  status = smp2pi_validate_context(ctx);

  if (SMP2P_STATUS_SUCCESS == status)
  {
    smp2p_entry_type * entry =
        &smp2p_info[ctx->remote_id].header_tx->entry[ctx->index];
    uint32 data;

    SMP2P_LOG_EX(SMP2P_LOG_EVENT_MODIFY, ctx->remote_id, set_mask, clr_mask, flags);

    if (!(flags & SMP2P_FLAGS_STM))
    {
      smp2p_os_cs_lock(smp2p_info[ctx->remote_id].cs);
    }

    data  = SMP2P_READ_SMEM_4(&entry->data);
    data &= ~clr_mask;
    data |=  set_mask;

    /* write the data back to smem */
    SMP2P_WRITE_SMEM_4(&entry->data, data);

    /* make sure the memory is updated */
    SMP2P_MEMORY_BARRIER();

    if (!(flags & SMP2P_FLAGS_NO_INTR))
    {
      /* notify remote processor */
      smp2p_os_send_interrupt(ctx->remote_id);
    }

    /* update write count */
    smp2p_info[ctx->remote_id].debug_info.count_tx++;

    /* save last data written in the context, for debug */
    ctx->data_shadow = data;

    if (!(flags & SMP2P_FLAGS_STM))
    {
      smp2p_os_cs_unlock(smp2p_info[ctx->remote_id].cs);
    }
  }

  return status;
}

/*===========================================================================
  FUNCTION  smp2p_register
===========================================================================*/
/**
  Registers an OS event to be triggered when data arrives on the specified
  entry.

  If the specified entry is not yet created by the remote endpoint,
  SMP2P will save the params and trigger the specified OS event upon
  creation of the entry.

  If the specified entry is already created by the remote endpoint,
  SMP2P will trigger the event immediately to inform the client to call
  smp2p_read() to read the data there.

  Clients should call the wait API to wait for an event.

  @param [out]  ctx       Pointer to pointer of SMP2P context.
                          Context will be created and this pointer will be made
                          to point to it.
  @param [in]   source    Source processor ID.
  @param [in]   name      16 byte name string of the entry.

  @return
  SMP2P_STATUS_SUCCESS          - Operation successful.
  SMP2P_STATUS_NOT_FOUND        - Entry not found in Rx data.
                                  Client may call wait API to be notified when
                                  the Tx side to creates the entry.
  SMP2P_STATUS_INVALID_PARAM    - Invalid parameter.
  SMP2P_STATUS_DUPLICATE        - Duplicate name already present.
  SMP2P_STATUS_OUT_OF_RESOURCES - Out of room in pending list.

  @dependencies
  none.
*/
/*=========================================================================*/
int32 smp2p_register
(
  smp2p_context_type ** ctx,
  smem_host_type        source,
  char *                name
)
{
  smp2p_header_type * header_rx = NULL;
  int32 index;
  int32 status;
  int32 name_len;
  char name_buf[SMP2P_NAME_STRING_LEN];
  void * ctx_temp;

  /* validate params */
  if (source >= SMEM_NUM_HOSTS || 
      (source == SMP2P_THIS_HOST && smp2p_info[source].header_rx == NULL)) 
  {
    return SMP2P_STATUS_INVALID_PARAM;
  }

  /* set up name buffer.
   * truncate the name string if it is longer than allowed. */
  memset(name_buf, 0, sizeof(name_buf));
  name_len = strlcpy(name_buf, name, sizeof(name_buf));

  /* don't allow zero length name strings */
  if (0 == name_len)
  {
    return SMP2P_STATUS_INVALID_PARAM;
  }

  SMP2P_LOG(SMP2P_LOG_EVENT_REGISTER, source, 0, 0);

  /* allocate the context buffer right away.
   * this must be done outside of critical section on some procs (wcnss).
   * upon any failure, it will be freed outside of the lock. */
  ctx_temp = smp2p_os_malloc(sizeof(**ctx));
  if (NULL == ctx_temp)
  {
    return SMP2P_STATUS_FAILURE;
  }

  /* lock here to protect the active and pending lists and other globals */
  smp2p_os_cs_lock(smp2p_info[source].cs);
  
  /* see if item exists already.
   * allow clients to register for entries which don't exist yet
   * even if the connection is not open */
   if(SMP2P_STATE_OPEN == smp2p_info[source].state)
   {
     /* set local header_rx pointer here to make sure it is not NULL */   
     header_rx = smp2p_info[source].header_rx;
     index = smp2pi_find_entry_index(header_rx, name_buf);
   }
   else
   {
     index = -1;
   } 

  /* at this point, there are several possible conditions:
   * - the entry exists (index is 0 or greater)
   * - the entry does not exist (smp2pi_find_entry_index returned -1 in index)
   * - the connection is not open (index is -1) */
  if (index >= 0)
  {
    smp2p_context_type * pending_ctx;

    /* item exists, check if anyone has already registered for it.
     * we do not support multiple clients registering for the same entry.
     * scan the active list looking for the name */
    pending_ctx = smem_list_first(&smp2p_rx_active_list[source]);
    while (pending_ctx)
    {
      if (!strncmp(pending_ctx->name, name_buf, SMP2P_NAME_STRING_LEN))
      {
        break;
      }

      /* point to next entry */
      pending_ctx = smem_list_next(pending_ctx);
    }

    if (pending_ctx)
    {
      smp2p_os_cs_unlock(smp2p_info[source].cs);
      smp2p_os_free(ctx_temp);
      return SMP2P_STATUS_DUPLICATE;
    }
  }
  else
  {
    smp2p_context_type * pending_ctx;

    /* make sure this name is not already in the pending list
     * scan the pending list until the end, or a match is found */
    pending_ctx = smem_list_first(&smp2p_rx_pending_list[source]);
    while (pending_ctx)
    {
      if (!strncmp(pending_ctx->name, name_buf, SMP2P_NAME_STRING_LEN))
      {
        break;
      }

      /* point to next entry */
      pending_ctx = smem_list_next(pending_ctx);
    }

    /* check that name does not appear in rx pending list.
     * pending entry will be null if no match was found. */
    if (pending_ctx)
    {
      smp2p_os_cs_unlock(smp2p_info[source].cs);
      smp2p_os_free(ctx_temp);
      return SMP2P_STATUS_DUPLICATE;
    }
    /* check that the pending list is not too big already */
    else if (SMP2P_ENTRIES_MAX < smem_list_count(&smp2p_rx_pending_list[source]))
    {
      smp2p_os_cs_unlock(smp2p_info[source].cs);
      smp2p_os_free(ctx_temp);
      return SMP2P_STATUS_OUT_OF_RESOURCES;
    }
  }

  /* init the context */
  *ctx = ctx_temp;

  /* initialize the OS event */
  smp2p_os_init_event(&(*ctx)->os_params);

  /* init with invalid index value, so we don't try to use this ctx except
   * for the os params */
  (*ctx)->index     = index;
  (*ctx)->remote_id = source;
  (*ctx)->dir       = SMP2P_DIR_RX;

  /* init the name */
  smp2p_mem_copy((*ctx)->name, sizeof((*ctx)->name), name_buf, 
                 sizeof((*ctx)->name));

  /* update global size inside lock */
  smp2p_info[source].debug_info.size_local += sizeof(**ctx);

  if (index >= 0)
  {
    /* init the shadow data to current contents */
    (*ctx)->data_shadow = SMP2P_READ_SMEM_4(&header_rx->entry[index].data);

    /* put entry on the list */
    smem_list_append(&smp2p_rx_active_list[source], (*ctx));

    /* be sure to unlock before calling the OS function */
    smp2p_os_cs_unlock(smp2p_info[source].cs);

    /* notify client */
    smp2p_os_send_event(&(*ctx)->os_params);

    status = SMP2P_STATUS_SUCCESS;
  }
  else
  {
    /* put entry on the list */
    smem_list_append(&smp2p_rx_pending_list[source], (*ctx));

    smp2p_os_cs_unlock(smp2p_info[source].cs);

    status = SMP2P_STATUS_NOT_FOUND;
  }

  return status;
}

/*===========================================================================
  FUNCTION  smp2p_read
===========================================================================*/
/**
  Reads data from an entry. Entry location is determined by contents of context
  parameter.

  Client should call this routine after SMP2P has triggered a receive event
  as registered by smp2p_register().

  Clients may call this routine at any time to read the current data value
  of the entry.

  @param [in]   ctx       Pointer to SMP2P context.
  @param [out]  data      Data read by SMP2P.

  @return
  SMP2P_STATUS_SUCCESS       - Operation successful.
  SMP2P_STATUS_NOT_INIT      - Connection is opening and is not ready for data.
  SMP2P_STATUS_INVALID_PARAM - Invalid parameter.

  @dependencies
  Context must be initialized.
*/
/*=========================================================================*/
int32 smp2p_read
(
  smp2p_context_type * ctx,
  uint32 *             data
)
{
  return smp2p_read_ex(ctx, data, SMP2P_FLAGS_NONE);
}

/*===========================================================================
  FUNCTION  smp2p_read_ex
===========================================================================*/
/**
  Reads data from an entry. Entry location is determined by contents of context
  parameter.

  Client should call this routine after SMP2P has triggered a receive event
  as registered by smp2p_register().

  Clients may call this routine at any time to read the current data value
  of the entry.

  @param [in]   ctx     Pointer to SMP2P context.
  @param [out]  data    Data read by SMP2P.
  @param [in]   flags   Flags parameter.
                        SMP2P_FLAGS_STM single threaded mode, no locks are taken.
                        SMP2P_FLAGS_NONE is normal behavior.

  @return
  SMP2P_STATUS_SUCCESS       - Operation successful.
  SMP2P_STATUS_NOT_INIT      - Connection is opening and is not ready for data.
  SMP2P_STATUS_INVALID_PARAM - Invalid parameter.

  @dependencies
  Context must be initialized.
*/
/*=========================================================================*/
int32 smp2p_read_ex
(
  smp2p_context_type * ctx,
  uint32 *             data,
  uint32               flags
)
{
  /* do param check on context */
  int32 status = smp2pi_validate_context(ctx);

  if (SMP2P_STATUS_SUCCESS == status)
  {
    /* point to the appropriate entry */
    smp2p_entry_type * entry =
        &smp2p_info[ctx->remote_id].header_rx->entry[ctx->index];

    /* read out the data */
    *data = SMP2P_READ_SMEM_4(&entry->data);

    SMP2P_LOG_EX(SMP2P_LOG_EVENT_READ, ctx->remote_id, *data, 0, flags);
  }
  else
  {
    SMP2P_LOG_EX(SMP2P_LOG_EVENT_READ, SMEM_INVALID_HOST, 0, status, flags);
  }

  return status;
}

/*===========================================================================
  FUNCTION  smp2p_wait
===========================================================================*/
/**
  Calls OS specific wait function to wait for an event specified in the context.

  For a transmit/outbound context, the following scenarios may trigger an event:
    - A call to smp2p_create() returned status indicating that the
      connection must be negotiated.  SMP2P will trigger the event when
      the connection is open and ready to use.

  For a receive/inbound context, the following scenarios may trigger an event:
    - An entry has new data available to read.
    - A call to smp2p_register() has found the requested entry.
      Client should read the data value after this event since data may be
      updated.
    - A call to smp2p_register() returned status indicating that the entry
      is not found.  SMP2P will trigger the event when the Tx side notifies
      the local side that the entry is created.
      Client should read the data value after this event since data may be
      updated.

  This function will sleep (ie. not return) until the event triggers.

  @param [in]   ctx       Pointer to SMP2P context.

  @return
  SMP2P_STATUS_SUCCESS  - Entry is created and ready to accept write data.
  SMP2P_STATUS_FAILURE  - Error occurred.

  @dependencies
  Context must initialized by a call to SMP2P.
*/
/*=========================================================================*/
int32 smp2p_wait
(
  smp2p_context_type * ctx
)
{
  /* call OS specific wait */
  return smp2p_os_wait(ctx);
}


/*===========================================================================
  FUNCTION  smp2p_wait_multiple
===========================================================================*/
/**
  Calls OS specific wait function to wait for an event specified in the context.
  This function waits for multiple events and returns when any one of them
  has been triggered.

  This function will sleep (ie. not return) until the event triggers.

  @param [in]   ctx_arr     Array of one or more pointers to SMP2P contexts
                            to wait on.
  @param [in]   ctx_count   Number of valid contexts in ctx array
  @param [out]  trigger_idx Index of context which was triggered

  @return
  SMP2P_STATUS_SUCCESS  - Entry is created and ready to accept write data.
  SMP2P_STATUS_FAILURE  - Error occurred.

  @dependencies
  Context must initialized by a call to SMP2P.
*/
/*=========================================================================*/
int32 smp2p_wait_multiple
(
  smp2p_context_type ** ctx_arr,
  uint32                ctx_count,
  uint32              * trigger_idx
)
{
  /* call OS specific wait */
  return smp2p_os_wait_multiple(ctx_arr, ctx_count, trigger_idx);
}
