/*
 * @file smp2p_internal.c
 *
 * Private routines and data structures for use with SMP2P.
 */

/*==============================================================================
     Copyright (c) 2012-2015 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smp2p/src/smp2p_internal.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/24/15   db      protect smp2pi_negotiate by using locks to avoid race condition
                   from getting it called from different threads
04/08/15   db      Modified assertion in function 'smp2pi_process_new_tx_entries'
                   to remove compilation errors.
01/31/14   rv      Replaced the memcpy and memmove functions with their 
                   respective secure memscpy and memsmove functions.
10/14/13   bt      Process pending lists and notify client following handshake.
12/12/13   rv      SMEM Partition backwards compatibility is removed.
09/25/13   pa      Cleaning up use of SMP2P_LOG in smp2pi_negotiate(). 
09/16/13   pa/bt   Add RESET_LOCAL state and reset handshake.
08/09/13   pa      Remove ext_status from smem ext functions.
08/01/13   pa      Fixing another race condition bug in negotiation.
05/29/13   pa      Switch to smem_alloc_ex and smem_get_addr_ex.
05/01/13   pa      During negotiation, wait for remote side to set version num.
03/28/13   rs      Moved generic queue functions to SMEM  
02/13/13   pa      Write zeros to entry data if SMEM item already exists.
02/12/13   pa      Remove unneeded assert.
01/22/13   bt      Remove unused status variable to resolve warning.
10/10/12   pa      Initial revision.
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smp2p_internal.h"
#include "smem.h"
#include "smem_list.h"
#include "smem_v.h"
#include "assert.h"
#include "smp2p_os_common.h"

/*===========================================================================
                        EXTERNAL DATA
===========================================================================*/

extern smp2p_info_type smp2p_info[SMEM_NUM_HOSTS];
extern smem_list_type smp2p_tx_pending_list[SMEM_NUM_HOSTS];
extern smem_list_type smp2p_tx_active_list[SMEM_NUM_HOSTS];
extern smem_list_type smp2p_rx_pending_list[SMEM_NUM_HOSTS];
extern smem_list_type smp2p_rx_active_list[SMEM_NUM_HOSTS];

/*===========================================================================
                        GLOBAL DATA
===========================================================================*/
/* SMP2P version numbers supported.
 * Must be in high to low order, such that [0] is the greatest number
 * in the list. */
static const uint32 smp2p_supported_versions[] = {1};

/*===========================================================================
                        MACROS
===========================================================================*/
/** Macro to determine the number of elements in an array of any type */
#define N_ELEMENTS(x) (sizeof(x)/sizeof(*(x)))

/** Macro to determine if a processor's RESTART_DONE flag has been ACKed by 
 * the other processor yet.  The local and remote headers' flags will be passed 
 * as arguments to this macro based on which bit is to be read/written. */
#define SMP2P_RESET_ACKED(reset_header_flags, ack_header_flags) \
    (!(((ack_header_flags >> SMP2P_FLAG_RESTART_ACK_SHIFT) ^    \
        (reset_header_flags >> SMP2P_FLAG_RESTART_DONE_SHIFT)) & 1))

/*===========================================================================
                        LOCAL FUNCTION DEFINITONS
===========================================================================*/
/*===========================================================================
  FUNCTION  smp2pi_process_new_tx_entries
===========================================================================*/
/**
  This function moves all entries in the pending tx list to the active tx list, 
  creating each entry in SMEM if it is not already present, and then notifies 
  each of the clients.

  @param [in]   dest  Destination processor ID to check for new entries.

  @return
  None
*/
/*=========================================================================*/
static void smp2pi_process_new_tx_entries(smem_host_type dest)
{
  smp2p_context_type * pending_entry;
  smp2p_context_type * next_pending_entry;
  int32 create_status;

  pending_entry = smem_list_first(&smp2p_tx_pending_list[dest]);
  while (pending_entry)
  {
    /* get next entry */
    next_pending_entry = smem_list_next(pending_entry);

    /* remove this entry from the pending list */
    smem_list_delete(&smp2p_tx_pending_list[dest], pending_entry);

    /* add this entry to the active list */
    smem_list_append(&smp2p_tx_active_list[dest], pending_entry);

    /* create the entry in smem, if not already created. */
    create_status = smp2pi_add_tx_entry(pending_entry);
    if(SMP2P_STATUS_SUCCESS != create_status && 
       SMP2P_STATUS_DUPLICATE != create_status)
    {
	  ASSERT(0);
    }

    /* notify client */
    smp2p_os_send_event(&pending_entry->os_params);

    /* point to next entry */
    pending_entry = next_pending_entry;
  }
}

/*===========================================================================
  FUNCTION  smp2pi_process_new_rx_entries
===========================================================================*/
/**
  This function checks for an update to the entries_valid field by comparing
  against a saved value.  If entries_valid has increased, this function
  determines the new entry(s) name(s) and looks in the pending list for those
  entries. If found in the pending list, it is removed and the client is
  notified.

  @param [in]   source  Source processor ID to check for new entries.

  @return
  Returns the number of new entries found.

  @dependencies
  None.
*/
/*=========================================================================*/
static uint32 smp2pi_process_new_rx_entries(smem_host_type source)
{
  uint32 index;
  uint32 count = 0;
  smp2p_header_entry_count_type entry_count;
  smp2p_header_type * header_rx = smp2p_info[source].header_rx;

  /* read the entry count from smem */
  SMP2P_COPY_FROM_SMEM(&entry_count, sizeof(entry_count),
                       &header_rx->entry_count, sizeof(entry_count));

  /* check to see if a new entry was added.
   * also, don't go beyond the total number of entries as agreed upon in
   * negotiation. */
  while (smp2p_info[source].remote_entries_valid_shadow <
         entry_count.entries_valid &&
         smp2p_info[source].remote_entries_valid_shadow <=
         smp2p_info[source].remote_entries_total)
  {
    char name[SMP2P_NAME_STRING_LEN];
    smp2p_context_type * pending_ctx;

    /* new entry index will be at the end of the entry array */
    index = smp2p_info[source].remote_entries_valid_shadow;

    /* update local shadow */
    smp2p_info[source].remote_entries_valid_shadow++;

    /* update count */
    count++;

    /* copy name from smem */
    SMP2P_COPY_FROM_SMEM(name, sizeof(name), header_rx->entry[index].name,
                         sizeof(name));

    /* scan the pending list until the end, or a match is found */
    pending_ctx = smem_list_first(&smp2p_rx_pending_list[source]);
    while (pending_ctx)
    {
      if (!strncmp(pending_ctx->name, name, SMP2P_NAME_STRING_LEN))
      {
        break;
      }

      /* point to next entry */
      pending_ctx = smem_list_next(pending_ctx);
    }

    if (pending_ctx)
    {
      uint32 data;

      /* delete the item from the pending list */
      smem_list_delete(&smp2p_rx_pending_list[source], pending_ctx);

      /* set the index now that we know it */
      pending_ctx->index = index;

      /* set the data shadow */
      data = SMP2P_READ_SMEM_4(&header_rx->entry[index].data);
      pending_ctx->data_shadow = data;

      /* add the item to the active list */
      smem_list_append(&smp2p_rx_active_list[source], pending_ctx);

      /* notify client immediately for this entry */
      smp2p_os_send_event(&pending_ctx->os_params);
    }
  }

  return count;
}

/*===========================================================================
  FUNCTION  smp2pi_process_new_entries
===========================================================================*/
/**
  This function processes all pending local tx entries and new rx entries 
  in SMEM, and notifies the local clients for each, if any.  

  @param [in]   remote_host  Processor ID to check for new tx and rx entries.

  @return
  Returns the number of new rx entries found.
*/
/*=========================================================================*/
static uint32 smp2pi_process_new_entries(smem_host_type remote_host)
{
  /* Notify clients with pending tx entries and drain the pending list. */
  smp2pi_process_new_tx_entries(remote_host);

  /* Look for updates to remote entry count and process the new rx entries.
   * This will move any matching rx entries from the pending list to the rx 
   * active list and notify the registered clients. */
  return smp2pi_process_new_rx_entries(remote_host);
}

/*===========================================================================
                        EXTERNAL FUNCTION DEFINITONS
===========================================================================*/

/*===========================================================================
  FUNCTION  smp2pi_isr
===========================================================================*/
/**
  Interrupt service routine for SMP2P.  Handles interrupts due to negotiation,
  new entries and received data.

  Negotiation interrupts will not trigger events to the client.
  New entries and new received data will trigger events to a client who is
  registered for that entry.

  @param [in]   source    Source processor ID who triggered the interrupt.

  @return
  None.

  @dependencies
  None.
*/
/*=========================================================================*/
void smp2pi_isr(smem_host_type source)
{
  if (source >= SMEM_NUM_HOSTS)
  {
    ERR_FATAL("Invalid source %d , max=%d", source, SMEM_NUM_HOSTS, 0);
    return;
  }

  SMP2P_LOG(SMP2P_LOG_EVENT_ISR, source, 0, 0);

  /* do state update if needed */
  smp2p_os_cs_lock(smp2p_info[source].cs);
  (void)smp2pi_negotiate(source);
  smp2p_os_cs_unlock(smp2p_info[source].cs);

  /* process entry updates if in valid state */
  if (SMP2P_STATE_OPEN == smp2p_info[source].state)
  {
    uint32 update_count;
    smp2p_header_type * header_tx = smp2p_info[source].header_tx;
    smp2p_header_type * header_rx = smp2p_info[source].header_rx;
    smp2p_context_type * ctx;
    uint32 data, local_header_flags, remote_header_flags;
    smp2p_header_version_type version_local;

    /* Read the flags before notifying client, so that if the remote subsystem
     * is reset after the clients are notified but before the RESTART_DONE 
     * check afterwards, the local host will not toggle RESTART_ACK until 
     * after the clients have been notified that the SMP2P entries were 
     * reset. */
    local_header_flags = SMP2P_READ_SMEM_4(&header_tx->flags);
    remote_header_flags = SMP2P_READ_SMEM_4(&header_rx->flags);

    /* look for updates to entry count and process them.
     * this will drain the rx pending list and notify registered clients */
    update_count = smp2pi_process_new_rx_entries(source);

    /* iterate through valid entries looking for data change */
    ctx = smem_list_first(&smp2p_rx_active_list[source]);
    while (ctx)
    {
      data = SMP2P_READ_SMEM_4(&header_rx->entry[ctx->index].data);
      if (data != ctx->data_shadow)
      {
        /* update shadow data */
        ctx->data_shadow = data;

        /* count how many times we received new data during this ISR */
        update_count++;

        /* update the receive count */
        smp2p_info[source].debug_info.count_rx++;

        /* notify client */
        smp2p_os_send_event(&ctx->os_params);
      }

      /* point to next entry */
      ctx = smem_list_next(ctx);
    }

    /* if no updates were found, log it */
    if (0 == update_count)
    {
      smp2p_info[source].debug_info.count_rx_no_update++;
    }

    SMP2P_COPY_FROM_SMEM(&version_local, sizeof(version_local), 
                         &header_rx->version, sizeof(version_local));
    if (version_local.flags & SMP2P_FFLAGS_SSR_HANDSHAKE)
    {
      /* If remote side has modified SMP2P_FLAG_RESTART_DONE bit, set 
       * SMP2P_FLAG_RESTART_ACK bit to same value.
       * Must do this after processing all entries and notifying clients.
       * Remote endpoint has restarted and is waiting for us to see that it 
       * has reset the entries before allowing its clients to write entries. */
      /* if remote RESTART_DONE bit value != local RESTART_ACK bit value */
      if (!SMP2P_RESET_ACKED(remote_header_flags, local_header_flags))
      {
        /* Ack to remote side by toggling RESTART_ACK (to match RESTART_DONE). */
        SMP2P_WRITE_SMEM_4(&header_tx->flags, 
                           (local_header_flags ^ SMP2P_FLAG_RESTART_ACK));
  
        /* make sure the memory is updated */
        SMP2P_MEMORY_BARRIER();
        smp2p_os_send_interrupt(source);
      }
    }
  }
}

/*===========================================================================
  FUNCTION  smp2pi_add_tx_entry
===========================================================================*/
/**
  This function initializes a new entry in the outbound SMEM item for the
  specified destination.  The name is copied to the entry and the top level
  entry count (entries_valid) is incremented.

  Very little param validation is done here.  Name should point to a valid 16
  byte null terminated string.  Ctx should be valid as well.

  @param [in]   ctx   Context for the entry to add.

  @return
  SMP2P_STATUS_DUPLICATE        - Name already exists in an entry
  SMP2P_STATUS_OUT_OF_RESOURCES - No more free entries
  SMP2P_STATUS_SUCCESS          - Entry added.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2pi_add_tx_entry(smp2p_context_type * ctx)
{
  smp2p_header_entry_count_type entry_count;
  smp2p_header_type * header_tx;
  int32 index;

  /* point to tx header */
  header_tx = smp2p_info[ctx->remote_id].header_tx;

  /* check for entry already in the array */
  index = smp2pi_find_entry_index(header_tx, ctx->name);

  /* if entry is already present, return duplicate status */
  if (index >= 0)
  {
    return SMP2P_STATUS_DUPLICATE;
  }

  /* lock here to protect the read-modify-write of entry_count as well as
   * the update to the entry itself */
  smp2p_os_cs_lock(smp2p_info[ctx->remote_id].cs);

  /* read the entry counts from the header */
  SMP2P_COPY_FROM_SMEM(&entry_count, sizeof(entry_count), 
                       &header_tx->entry_count, sizeof(entry_count));

  /* make sure we have room in the array */
  if (entry_count.entries_valid >=
      entry_count.entries_total)
  {
    smp2p_os_cs_unlock(smp2p_info[ctx->remote_id].cs);
    return SMP2P_STATUS_OUT_OF_RESOURCES;
  }

  /* set index to new position */
  index = entry_count.entries_valid;

  /* copy name identifier to entry */
  SMP2P_COPY_TO_SMEM(&header_tx->entry[index].name,
                     sizeof(header_tx->entry[index].name),
                     ctx->name, 
                     sizeof(ctx->name));

  /* ensure that entry is valid in memory before updating the count */
  SMP2P_MEMORY_BARRIER();

  /* update header */
  entry_count.entries_valid++;
  SMP2P_COPY_TO_SMEM(&header_tx->entry_count, 
                     sizeof(smp2p_header_entry_count_type),
                     &entry_count,
                     sizeof(entry_count));

  /* make sure the memory is updated */
  SMP2P_MEMORY_BARRIER();

  smp2p_os_cs_unlock(smp2p_info[ctx->remote_id].cs);

  ctx->index = index;

  return SMP2P_STATUS_SUCCESS;
}

/*===========================================================================
  FUNCTION  smp2pi_negotiate
===========================================================================*/
/**
  Performs the negotiation process of the SMP2P connection.

  If negotiation has not started, this function first looks for the SMP2P SMEM
  item for the specified destination processor. If the SMEM item exists, this
  function uses the remote processor version number and feature flags as the
  starting point for negotiation. If the SMEM item does not exist, this
  function advertises the local processor's version number and feature flags.

  If negotiation is in progress, this function will attempt to match the remote
  processor's version number and feature flags, or gear down to a version or
  feature set that both processors support.

  At each step, the remote processor is notified of the change with an SMP2P
  interrupt being triggered.

  When negotiation is complete, this function returns success. If any entries
  are in the Tx pending list, their clients are notified and the Tx pending
  list is emptied.

  @param [in]   dest   Destination processor ID.

  @return
  SMP2P_STATUS_SUCCESS  - Negotiation complete and connection is open.
  SMP2P_STATUS_NOT_INIT - Negotiation is in progress and is waiting for
                          response from remote endpoint.
  SMP2P_STATUS_FAILURE  - Failure occurred.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2pi_negotiate(smem_host_type dest)
{
  int32 status = SMP2P_STATUS_FAILURE;
  int32 smem_status;
  smem_alloc_params_type params;

  if (SMP2P_STATE_OPEN == smp2p_info[dest].state)
  {
    /* if connection is open, just return success */
    status = SMP2P_STATUS_SUCCESS;
  }
  else if (SMP2P_STATE_RESET_LOCAL == smp2p_info[dest].state)
  {
    uint32 local_header_flags = SMP2P_READ_SMEM_4(
        &smp2p_info[dest].header_tx->flags);
    uint32 remote_header_flags = SMP2P_READ_SMEM_4(
        &smp2p_info[dest].header_rx->flags);
    /* Wait for remote side to ack our reset bit. */
    /* if local RESTART_DONE bit value == remote RESTART_ACK bit value */
    if (SMP2P_RESET_ACKED(local_header_flags, remote_header_flags))
    {
      /* once bits are equal, go directly to open */
      smp2p_info[dest].state = SMP2P_STATE_OPEN;
      status = SMP2P_STATUS_SUCCESS;

      smp2pi_process_new_entries(dest);

      SMP2P_LOG(SMP2P_LOG_EVENT_NEGOTIATE, dest, SMP2P_NEGOTIATION_EVENT_1, 0);
    }
    else
    {
      status = SMP2P_STATUS_NOT_INIT;
    }
  }
  else if (SMP2P_STATE_CLOSED == smp2p_info[dest].state)
  {
    uint32 size;
    uint32 smem_id;
    smp2p_header_type * header_tx;

    /* allocate SMEM item */
    /* set rx valid entries to 0. on the first ISR we get after negotiation,
     * we will find any entries that have been created on the remote side */
    smp2p_info[dest].remote_entries_valid_shadow = 0;

    /* allocate outbound smem item.
     * for loopback, allocate a local buffer instead of smem. */
    size = SMP2P_SMEM_ITEM_SIZE(SMP2P_ENTRIES_TOTAL);
    smem_id = SMP2P_GET_SMEM_ID(SMP2P_THIS_HOST, dest);
    if (SMP2P_LPBK_HOST == dest)
    {
      smp2p_info[dest].header_tx = smp2p_os_malloc(size);
      smp2p_info[dest].debug_info.size_local += size;
    }
    else
    {
      void * smem_addr_tx;
      void * smem_addr_rx;
      smp2p_header_version_type version_remote = {0};
      smp2p_header_version_type version_local = {0};

      /* look up the location of the tx and rx smp2p smem items */
      params.remote_host = dest;
      params.smem_type   = (smem_mem_type)smem_id;
      params.flags       = 0;
      smem_status = smem_get_addr_ex(&params);
      ASSERT(smem_status == SMEM_STATUS_SUCCESS ||
             smem_status == SMEM_STATUS_NOT_FOUND);
      smem_addr_tx = SMEM_STATUS_SUCCESS == smem_status ? params.buffer : NULL;

      params.remote_host = dest;
      params.smem_type   = (smem_mem_type)SMP2P_GET_SMEM_ID(dest, SMP2P_THIS_HOST);
      params.flags       = 0;
      smem_status = smem_get_addr_ex(&params);
      ASSERT(smem_status == SMEM_STATUS_SUCCESS ||
             smem_status == SMEM_STATUS_NOT_FOUND);
      smem_addr_rx = SMEM_STATUS_SUCCESS == smem_status ? params.buffer : NULL;

      /* set pointers if either smem item exists */
      if (smem_addr_tx)
      {
        smp2p_info[dest].header_tx = smem_addr_tx;
        smp2p_info[dest].debug_info.size_smem += size;
        SMP2P_COPY_FROM_SMEM(&version_local, sizeof(version_local),
                             &smp2p_info[dest].header_tx->version, 
                             sizeof(version_local));
      }
      if (smem_addr_rx)
      {
        /* Existence of the remote smem item does not necessarily mean that the
         * remote side has populated the header.  Non-zero version number 
         * indicates that the header is fully written */
        smp2p_info[dest].header_rx = smem_addr_rx;

        SMP2P_COPY_FROM_SMEM(&version_remote, sizeof(version_remote), 
                             &smp2p_info[dest].header_rx->version,
                             sizeof(version_remote));
        if (!version_remote.version)
        {
          smp2p_info[dest].header_rx = NULL;
          smem_addr_rx = NULL;
        }
      }

      /* check if the smem allocation has already occurred.
       * this allows us to support recovery/detection, where the proc is reset
       * but the contents of smem are retained.
       * in this case, we will try to restore state from smem. */
      if (smem_addr_tx && smem_addr_rx)
      {
        smp2p_header_type * header_tx = smp2p_info[dest].header_tx;
        smp2p_header_type * header_rx = smp2p_info[dest].header_rx;

        /* if versions match, that means negotiation was completed sometime
         * in the past.  use those values.
         * if this fails, fall through and restart negotiation. */
        if (version_local.version == version_remote.version &&
            version_local.flags   == version_remote.flags)
        {
          smp2p_header_entry_count_type entries_remote;
          smp2p_header_entry_count_type entries_local;
          smp2p_entry_type * entry;
          uint32 ii;
          uint32 local_header_flags, remote_header_flags;

          SMP2P_COPY_FROM_SMEM(&entries_remote, sizeof(entries_remote), 
                               &header_rx->entry_count, sizeof(entries_remote));
          SMP2P_COPY_FROM_SMEM(&entries_local, sizeof(entries_local),
                               &header_tx->entry_count, sizeof(entries_local));

          smp2p_info[dest].remote_entries_total =
              entries_remote.entries_total;

          /* zero out entries to provide clean slate after SSR */
          for (ii=0; ii<entries_local.entries_valid; ii++)
          {
            entry = &header_tx->entry[ii];
            SMP2P_WRITE_SMEM_4(&entry->data, 0);
          }

          if (version_local.flags & SMP2P_FFLAGS_SSR_HANDSHAKE)
          {
            local_header_flags = SMP2P_READ_SMEM_4(&header_tx->flags);
            remote_header_flags = SMP2P_READ_SMEM_4(&header_rx->flags);
            /* if local RESTART_DONE bit value == remote RESTART_ACK bit */
            if (SMP2P_RESET_ACKED(local_header_flags, remote_header_flags))
            {
              /* Toggle RESTART_DONE (to be opposite of remote RESTART_ACK). */
              SMP2P_WRITE_SMEM_4(&header_tx->flags, 
                  (local_header_flags ^ SMP2P_FLAG_RESTART_DONE));
            }
            
            /* make sure the memory is updated */
            SMP2P_MEMORY_BARRIER();
            
            smp2p_info[dest].state = SMP2P_STATE_RESET_LOCAL;
            /* notify remote processor */
            smp2p_os_send_interrupt(dest);
  
            SMP2P_LOG(SMP2P_LOG_EVENT_NEGOTIATE, dest, SMP2P_NEGOTIATION_EVENT_2, 0);
            return SMP2P_STATUS_NOT_INIT;
          }
          else
          {
            /* make sure the memory is updated */
            SMP2P_MEMORY_BARRIER();
            
            smp2p_info[dest].state = SMP2P_STATE_OPEN;
            /* notify remote processor */
            smp2p_os_send_interrupt(dest);
  
            SMP2P_LOG(SMP2P_LOG_EVENT_NEGOTIATE, dest, SMP2P_NEGOTIATION_EVENT_3, 0);
            return SMP2P_STATUS_SUCCESS;
          }
        }
      }
      else
      {
        /* allocate smp2p smem item */
        params.remote_host = dest;
        params.smem_type   = (smem_mem_type)smem_id;
        params.flags       = 0;
        params.size        = size;
        smem_status = smem_alloc_ex(&params);
        ASSERT(smem_status == SMEM_STATUS_SUCCESS);
        smp2p_info[dest].header_tx = params.buffer;
        smp2p_info[dest].debug_info.size_smem += size;
      }
    }

    /* point to header */
    header_tx = smp2p_info[dest].header_tx;

    /* set up header */
    SMP2P_WRITE_SMEM_4(&header_tx->identifier,                SMP2P_IDENTIFIER);
    SMP2P_WRITE_SMEM_2(&header_tx->procs.proc_local,          SMP2P_THIS_HOST);
    SMP2P_WRITE_SMEM_2(&header_tx->procs.proc_remote,         dest);
    SMP2P_WRITE_SMEM_2(&header_tx->entry_count.entries_total, SMP2P_ENTRIES_TOTAL);
    SMP2P_WRITE_SMEM_2(&header_tx->entry_count.entries_valid, 0);
    SMP2P_WRITE_SMEM_4(&header_tx->flags,                     0);
    SMP2P_MEMORY_BARRIER();

    /* check if rx item is available, and attempt to find a supported version */
    if (smp2p_info[dest].header_rx)
    {
      uint32 i;
      smp2p_header_entry_count_type entries_remote;
      smp2p_header_version_type version_remote;
      smp2p_header_version_type version_local;
      smp2p_header_type * header_rx = smp2p_info[dest].header_rx;

      /* read entry count from smem */
      SMP2P_COPY_FROM_SMEM(&entries_remote, sizeof(entries_remote), 
                           &header_rx->entry_count, sizeof(entries_remote));

      if (SMP2P_ENTRIES_MAX < entries_remote.entries_total)
      {
        /* TODO: Error fatal: remote endpoint has invalid number of entries */
        version_local.flags = 0;
        version_local.version = 0xff;
        SMP2P_COPY_TO_SMEM(&header_tx->version, 
                           sizeof(smp2p_header_version_type), 
                           &version_local, sizeof(version_local));
        SMP2P_MEMORY_BARRIER();
        /* notify remote endpoint of update */
        smp2p_os_send_interrupt(dest);
        ERR_FATAL("remote endpoint has invalid number of entries dest=%d, entries_total=%d.", 
                dest, entries_remote.entries_total, 0);
        return SMP2P_STATUS_FAILURE;
      }
      else
      {
        /* capture initial value of entries total, and do not reference the
         * value in smem after this point so that we always adhere to the value
         * presented to us in negotiation (in the unlikely case that the value
         * in smem changes after that).*/
        smp2p_info[dest].remote_entries_total = entries_remote.entries_total;
      }

      /* set local version number based on remote values */
      SMP2P_COPY_FROM_SMEM(&version_remote, sizeof(version_remote), 
                           &header_rx->version, sizeof(version_remote));
      version_local.version = 0;
      for (i=0; i<N_ELEMENTS(smp2p_supported_versions); i++)
      {
        if (smp2p_supported_versions[i] <= version_remote.version)
        {
          version_local.version = smp2p_supported_versions[i];
          break;
        }
      }

      if (!version_local.version)
      {
        /* TODO: Error fatal: could not find a supported version */
        version_local.flags = 0;
        version_local.version = 0xff;
        SMP2P_COPY_TO_SMEM(&header_tx->version, 
                           sizeof(smp2p_header_version_type),
                           &version_local, sizeof(version_local));
        SMP2P_MEMORY_BARRIER();
        /* notify remote endpoint of update */
        smp2p_os_send_interrupt(dest);
        ERR_FATAL("Could not find a supported version dest=%d.", dest, 0, 0);
        
        return SMP2P_STATUS_FAILURE;
      }

      /* set local flags based on remote values */
      version_local.flags = version_remote.flags & SMP2P_FFLAGS_SUPPORTED;

      /* set any flags that we'd like to request.
       * do this only on the first try, so that we don't keep requesting
       * the same flags over and over */
      version_local.flags |= SMP2P_FFLAGS_REQUESTED;

      /* Write the value to SMEM */
      SMP2P_COPY_TO_SMEM(&header_tx->version, 
                         sizeof(smp2p_header_version_type),
                         &version_local, sizeof(version_local));

      /* if everything matches, negotiation is complete.
       * otherwise, we expect to have another round of negotiation */
      smp2p_info[dest].state = (version_local.version == version_remote.version &&
                                version_local.flags   == version_remote.flags) ?
                                SMP2P_STATE_OPEN :
                                SMP2P_STATE_INIT;
      SMP2P_LOG(SMP2P_LOG_EVENT_NEGOTIATE, dest, SMP2P_NEGOTIATION_EVENT_4, 0);

      if (SMP2P_STATE_OPEN == smp2p_info[dest].state)
      {
        smp2pi_process_new_entries(dest);

        status = SMP2P_STATUS_SUCCESS;
      }
      else
      {
        status = SMP2P_STATUS_NOT_INIT;
      }
    }
    else
    {
      smp2p_header_version_type version_local;

      /* Remote endpoint has not created SMEM item yet.
       * Advertise local processor latest version and preferred flags. */
      version_local.version = smp2p_supported_versions[0];
      version_local.flags   = SMP2P_FFLAGS_REQUESTED;

      /* update state */
      smp2p_info[dest].state = SMP2P_STATE_INIT;
      SMP2P_LOG(SMP2P_LOG_EVENT_NEGOTIATE, dest, SMP2P_NEGOTIATION_EVENT_5, 0);

      /* Write the value to SMEM */
      SMP2P_COPY_TO_SMEM(&header_tx->version,
                         sizeof(smp2p_header_version_type),
                         &version_local, sizeof(version_local));

      status = SMP2P_STATUS_NOT_INIT;
    }

    SMP2P_MEMORY_BARRIER();

    /* notify remote endpoint of update */
    smp2p_os_send_interrupt(dest);
  }
  else if (SMP2P_STATE_INIT == smp2p_info[dest].state)
  {
    uint32 i;
    smp2p_header_entry_count_type entries_remote;
    smp2p_header_version_type version_remote;
    smp2p_header_version_type version_local;
    smp2p_header_type * header_tx;
    smp2p_header_type * header_rx;

    /* locate the incoming SMEM item if we haven't yet.
     * for loopback, the the loopback code should malloc the rx item
     * and set the rx header pointer before getting here. */
    if (SMP2P_LPBK_HOST == dest)
    {
      ASSERT(smp2p_info[dest].header_rx);
    }
    else if (!smp2p_info[dest].header_rx)
    {
      params.remote_host = dest;
      params.smem_type   = (smem_mem_type)SMP2P_GET_SMEM_ID(dest, SMP2P_THIS_HOST);
      params.flags       = 0;
      smem_status = smem_get_addr_ex(&params);
      ASSERT(smem_status == SMEM_STATUS_SUCCESS ||
             smem_status == SMEM_STATUS_NOT_FOUND);
      smp2p_info[dest].header_rx = SMEM_STATUS_SUCCESS == smem_status ?
                                   params.buffer : NULL;

      /* if still not found, we have nothing to do, so exit */
      if (!smp2p_info[dest].header_rx)
      {
        return SMP2P_STATUS_NOT_INIT;
      }
    }

    header_tx = smp2p_info[dest].header_tx;
    header_rx = smp2p_info[dest].header_rx;

    SMP2P_COPY_FROM_SMEM(&entries_remote, sizeof(entries_remote), 
                         &header_rx->entry_count, sizeof(entries_remote));
    if (SMP2P_ENTRIES_MAX < entries_remote.entries_total)
    {
      /* TODO: Error fatal: remote endpoint has invalid number of entries */
      version_local.flags = 0;
      version_local.version = 0xff;
      SMP2P_COPY_TO_SMEM(&header_tx->version,
                         sizeof(smp2p_header_version_type),
                         &version_local, sizeof(version_local));
      SMP2P_MEMORY_BARRIER();
      /* notify remote endpoint of update */
      smp2p_os_send_interrupt(dest);
      ERR_FATAL("remote endpoint has invalid number of entries dest=%d, entries_total=%d", 
            dest, entries_remote.entries_total, 0);
      return SMP2P_STATUS_FAILURE;
    }
    else
    {
      /* capture initial value of entries total, and do not reference the
       * value in smem after this point so that we always adhere to the value
       * presented to us in negotiation (in the unlikely case that the value
       * in smem changes after that).*/
      smp2p_info[dest].remote_entries_total = entries_remote.entries_total;
    }

    /* if everything matches, negotiation is complete.
     * otherwise, we expect to have another round of negotiation */
    SMP2P_COPY_FROM_SMEM(&version_remote, sizeof(version_remote), 
                         &header_rx->version, sizeof(version_remote));
    SMP2P_COPY_FROM_SMEM(&version_local, sizeof(version_local),
                         &header_tx->version, sizeof(version_local));
    smp2p_info[dest].state = (version_local.version == version_remote.version &&
                              version_local.flags   == version_remote.flags) ?
                                    SMP2P_STATE_OPEN :
                                    SMP2P_STATE_INIT;
    SMP2P_LOG(SMP2P_LOG_EVENT_NEGOTIATE, dest, SMP2P_NEGOTIATION_EVENT_6, 0);

    if (SMP2P_STATE_OPEN != smp2p_info[dest].state)
    {
      version_local.version = 0;
      for (i=0; i<N_ELEMENTS(smp2p_supported_versions); i++)
      {
        if (smp2p_supported_versions[i] <= version_remote.version)
        {
          version_local.version = smp2p_supported_versions[i];
          break;
        }
      }

      if (0 == version_local.version)
      {
        /* TODO: Error fatal: could not find a supported version */
        version_local.flags = 0;
        version_local.version = 0xff;
        SMP2P_COPY_TO_SMEM(&header_tx->version,
                           sizeof(smp2p_header_version_type),
                           &version_local, sizeof(version_local));
        SMP2P_MEMORY_BARRIER();
        /* notify remote endpoint of update */
        smp2p_os_send_interrupt(dest);
        ERR_FATAL("could not find a supported version.", 0, 0, 0);
        return SMP2P_STATUS_FAILURE;
      }

      /* mask local flags based on remote values */
      version_local.flags = version_remote.flags & SMP2P_FFLAGS_SUPPORTED;

      /* Write the value to SMEM */
      SMP2P_COPY_TO_SMEM(&header_tx->version,
                         sizeof(smp2p_header_version_type),
                         &version_local, sizeof(version_local));

      /* if everything matches, negotiation is complete.
       * otherwise, we expect to have another round of negotiation */
      smp2p_info[dest].state = (version_local.version == version_remote.version &&
                                version_local.flags   == version_remote.flags) ?
                                      SMP2P_STATE_OPEN :
                                      SMP2P_STATE_INIT;
      SMP2P_LOG(SMP2P_LOG_EVENT_NEGOTIATE, dest, SMP2P_NEGOTIATION_EVENT_7, 0);

      SMP2P_MEMORY_BARRIER();

      /* notify remote endpoint of update */
      smp2p_os_send_interrupt(dest);
    }

    /* re-do check if the connection is open, as state may have changed above */
    if (SMP2P_STATE_OPEN == smp2p_info[dest].state)
    {
      smp2pi_process_new_entries(dest);

      status = SMP2P_STATUS_SUCCESS;
    }
    else
    {
      status = SMP2P_STATUS_NOT_INIT;
    }
  }

  return status;
}

/*===========================================================================
  FUNCTION  smp2pi_validate_context
===========================================================================*/
/**
  Validates that the context structure contents are within range of the
  valid SMP2P destination and entry counts.

  Note that this routine only performs bounds checking and cannot catch
  corruption or attacks which cause the context data to fall within valid range.

  @param [in]   ctx    Pointer to context to check.

  @return
  SMP2P_STATUS_INVALID_PARAM - Context contains invalid data
  SMP2P_STATUS_NOT_INIT      - Context contains valid port_id, but the SMP2P
                               connection is not yet complete.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2pi_validate_context(smp2p_context_type * ctx)
{
  smp2p_header_type * header;
  uint16 entries_valid;

  if (ctx->remote_id >= SMEM_NUM_HOSTS)
  {
    return SMP2P_STATUS_INVALID_PARAM;
  }

  if (ctx->dir != SMP2P_DIR_TX &&
      ctx->dir != SMP2P_DIR_RX)
  {
    return SMP2P_STATUS_INVALID_PARAM;
  }

  /* make sure the connection is open */
  if (SMP2P_STATE_OPEN != smp2p_info[ctx->remote_id].state)
  {
    return SMP2P_STATUS_NOT_INIT;
  }

  /* choose the appropriate header to validate against */
  header = SMP2P_DIR_TX == ctx->dir ?
                smp2p_info[ctx->remote_id].header_tx :
                smp2p_info[ctx->remote_id].header_rx;

  /* finally, check if the index is in the valid range */
  entries_valid = SMP2P_READ_SMEM_2(&header->entry_count.entries_valid);
  if (ctx->index >= entries_valid)
  {
    return SMP2P_STATUS_INVALID_PARAM;
  }

  return SMP2P_STATUS_SUCCESS;
}

/*===========================================================================
  FUNCTION  smp2pi_find_entry_index
===========================================================================*/
/**
  Scans the SMP2P entries for the specified name.

  @param [in]   header  Pointer to SMP2P header to scan.
  @param [in]   name    Name of the entry to search for.

  @return
  Returns the index of the entry with the given name.
  If not found, -1 is returned.

  @dependencies
  None.
*/
/*=========================================================================*/
int32 smp2pi_find_entry_index(smp2p_header_type * header, char * name)
{
  uint32 i;
  char name_buf[SMP2P_NAME_STRING_LEN];
  uint16 entries_valid;

  entries_valid = SMP2P_READ_SMEM_2(&header->entry_count.entries_valid);

  for (i=0; i<entries_valid; i++)
  {
    SMP2P_COPY_FROM_SMEM(name_buf, SMP2P_NAME_STRING_LEN, header->entry[i].name,
                         SMP2P_NAME_STRING_LEN);
    if (0 == strncmp(name, name_buf, SMP2P_NAME_STRING_LEN))
    {
      return i;
    }
  }

  return -1;
}

