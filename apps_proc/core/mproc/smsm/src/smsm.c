/*------------------------------------------------------------------------------
   SMSM.C  - Shared Memory State Machine
------------------------------------------------------------------------------*/
/*!
  @file
    smsm.c

  @brief
    This module contains a simply synchrnization mechnism via the shared
    memory. 
*/

/*------------------------------------------------------------------------------
     Copyright  2008 - 2014 Qualcomm Technologies Incorporated. 
     All rights reserved.
------------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smsm/src/smsm.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/26/14   bc      Remove ERR_FATAL in smsm_cb_register since it's not valid
                   any more
03/21/14   an      process SMSM_RESET first.
02/06/14   bc      Fixing kw issues in smsm_cb_register, replacing assert with
                   ERR_FATAL
04/03/13   bt      Generalize all SMSM critical sections.
01/31/13   bt      Simplify and sanitize smsm_set_reset, to avoid exceptions, 
                   errors and overhead after an error has already occurred.
11/20/12   bt      Switch assert order to resolve KW warnings in clients.
11/15/12   bt      Add more range-checking to resolve KW warning.
11/06/12   bt      Enable SMSM interrupts based on DevConfig.
09/14/12   bt      Use changed version mask name SMEM_FULL_VERSION_MASK.
09/12/12   bt      Moved static functions to top to avoid need for prototypes.
09/05/12   pa      Added prototypes for smsm_intr_mask_get and 
                   smsm_intr_mask_set to resolve compile errors.
06/25/12   pa      Cleaning up warnings.
06/21/12   bt      Make smsm_intr_mask_get/set static, to enforce as internal.
04/27/12   rs      Allow SMSM initialization to be re-entrant from multiple
                   processes by using the SMSM_INIT bit. 
04/13/12   bt      Split SMSM target header into processor-specific files.
02/12/12   bm      Adding SMSM Tx/Rx event logging support.
02/21/12   bt      Remove smsm_poll_timeout, no timetick calls in SMSM now.
12/21/11   rs      Defining memory barrier for SMSM as OS specific
12/13/11   rs      Removed DALSYS and DALHWIO dependencies: added interrupts 
                   base address initialization 
11/02/11   bt      Split SMD/SMSM interrupts Modem<->LPASS, clean up code.
10/03/11   bt      Prevent remote race when registering already-used bits.
09/06/11   bt      Made smsm_cb_tbl dynamically-allocated to optimize memory.
07/18/11   bm      Removal of deprecated smsm_shim API layer.
06/17/11   hwu     Seperate SMSM out of SMEM.
05/18/11   bm      Changed prototype for smsm_init to conform to RCInit 
                   requirement
04/25/11   tl      Cast smsm_host_type and smsm_entry_type to uint32 before
                   comparing with num_hosts and num_entries
04/13/11   rs      Use OS abstraction to implement SMSM critical sections 
04/12/11   bt      Removed non-modem procs' dependency on SMSM boot change.
03/24/11   bt      Use smsm size info from shared memory, set new SMSM version.
03/18/11   bt      In smsm_notify_other_hosts, check for invalid intr line.
01/06/11   bt      Upon SMSM init, clear all bits before setting on local host.
12/02/10   rs      Cleaned up compiler warnings
10/21/10   esh     Removed Proc-comm
09/16/10   bt      Protect race conditions with DALSys critical sections.
05/04/10   esh     Set interrupt mask based upon registered callbacks
03/18/10   rs      Zero initialize interrupt table used in host notifications
02/18/10   tl      Fixed interrupt mask in smsm_init()
02/03/10   esh     Compiler warning cleanup for Q6
02/02/10   esh     Fixed Q6 compilation error
01/20/10   hwu     enforce the usage of smsm_intr_mask_set.
01/19/10   esh     Fixed smsm_state_modify index error, assert cleanup,
                   unused code removal
01/06/10   esh     Removed return type for smsm_set_reset
11/24/09   rs      CMI updates
11/23/09   tl      Added smsm_shim_get_host_entry()
11/19/09   tl      Don't use proc_comm on 9600
11/19/09   sa      warning cleanup, sync-up with symbian
07/06/09   hwu     changed to update the shadow states when register callback.
06/10/09   bfc     CMI updates.
06/01/09   hwu     add barrier to make the memory is updated.
04/23/09   hwu     CMI phase I header split.
04/13/09   hwu     Remove smsm_notify_cb().
03/02/09   hwu     uncomment the change to send interrupt after set reset.
01/14/09   hwu     Added smsm_process_host_update. This can be called in ISR 
                   and task.
01/07/09   hwu     Fixed the compiler warnings.
11/20/08   bdh     Added support for Apps debug break/continue adding handling
                   SMSM_MODEM_BREAK and SMSM_MODEM_CONTINUE states. 
11/20/08   hwu     Fixed the cpu mask issue. Removed the running flag. 
10/30/08   jlk     fixed include to use smem_pcmod.h instead of 
                   smem_proc_comm.h 
10/30/08   tl      Fixed double mutex-lock in smsm_do_cb
10/21/08   hwu     added smem_target_init_interrupt.
01/05/08   hwu     Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smsm.h"
#include "smsm_internal.h"
#include "smsm_os_common.h"

#include "smem.h"
#ifdef FEATURE_SMSM_DEBUG_LOGGING
#include "smem_log.h"
#endif

/*===========================================================================
                    CONSTANT AND VARIABLE DECLARATIONS
===========================================================================*/
/**
   smsm_entry_table
     - Lists the smsm entries saved in the shared memory
*/
smsm_entry_info_type smsm_entry_tbl[SMSM_NUM_ENTRIES] = 
{
  { SMSM_APPS_STATE ,      SMSM_APPS },
  { SMSM_MODEM_STATE ,     SMSM_MODEM },
  { SMSM_Q6_STATE,         SMSM_Q6 },
  { SMSM_WCNSS_STATE,      SMSM_WCNSS },
  { SMSM_DSPS_STATE,       SMSM_INVALID_HOST },
  { SMSM_RESERVED_0,       SMSM_INVALID_HOST },
  { SMSM_RESERVED_1,       SMSM_INVALID_HOST },
  { SMSM_RESERVED_2,       SMSM_INVALID_HOST }
};

smsm_info_type   smsm_info = {0};

/** The critical section structure used to lock access to SMSM */
static void *smsm_cs = NULL;

/*===========================================================================
  FUNCTION  smsm_target_send_interrupt
===========================================================================*/
/*!
@brief
  This function sends out the interprocessor interrupts.

@param  info - pointer to the smsm_info structure
@param  host - the host to be interrupted
 
@return None

@note   None
*/
/*=========================================================================*/
static void smsm_target_send_interrupt
(
  smsm_info_type *info,
  uint32          host
)
{
  smsm_intr_info_type  *intr = NULL;

  intr = &(info->interrupt_tbl[host]);

  smsm_send_interrupt(intr);
}

/*===========================================================================

                    LOCAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
  FUNCTION  smsm_intr_mask_get
===========================================================================*/
/*!
@brief
  This function returns the interrupt mask for an entry on a certain processor.

@param  entry - which entry 
@param  host  - which host 

@return The entry interrupt mask of a host

@note   None
*/
/*=========================================================================*/
static uint32 smsm_intr_mask_get
(
 smsm_entry_type  entry,
 smsm_host_type   host
)
{
  smsm_info_type  *info = &smsm_info;
  uint32          *addr;
  uint32           mask;

  /* Do not attempt to access out of bounds of the shared memory structure. */
  ASSERT((uint32)entry < info->num_entries);
  ASSERT((uint32)host < info->num_hosts);
  
  addr = (uint32 *)info->intr_mask;
  mask = *(addr + (entry * info->num_hosts) + host);

  return(mask);
}

/*===========================================================================
  FUNCTION  smsm_intr_mask_set
===========================================================================*/
/*!
@brief
  This function sets the interrupt mask for an entry on a certain processor.

@param  entry - which entry 
@param  host  - which host 
@param  value  - the interrupt mask to be set

@return None

@note   None
*/
/*=========================================================================*/
static void smsm_intr_mask_set
(
 smsm_entry_type  entry,
 smsm_host_type   host,
 uint32           value
)
{
  smsm_info_type  *info = &smsm_info;
  uint32          *addr;
  uint32          *intr_mask_addr;
  
  /* only set the interrupt mask which belongs to this host */
  ASSERT(host == info->this_host);
  /* Do not attempt to access out of bounds of the shared memory structure. */
  ASSERT((uint32)entry < info->num_entries);
  ASSERT((uint32)host < info->num_hosts);

  addr = (uint32 *)info->intr_mask;
  intr_mask_addr  = (addr + (entry * info->num_hosts) + host);
  *intr_mask_addr = value;

  /* make sure the memory is updated */ 
  SMSM_MEMORY_BARRIER(); 
}

/*===========================================================================
  FUNCTION  smsm_do_cb
===========================================================================*/
/*!
@brief
  This function processes the client callback.

@param  entry - a data entry
@param  prev_state - previous state of the entry
@param  curr_state - current state of the entry

@return None

@note   None
*/
/*=========================================================================*/
static void smsm_do_cb
(
  smsm_entry_type   entry,
  uint32            prev_state, 
  uint32            curr_state
)
{
  smsm_cb_link_type  *cb_link;
  smsm_cb_type        cb_func;
  void               *cb_data;
  uint32              mask;
  smsm_info_type     *info=&smsm_info;
  
  ASSERT(entry < SMSM_NUM_ENTRIES);

  /* Lock access to SMSM */
  smsm_cs_lock(smsm_cs);

  cb_link = info->smsm_cb_tbl[entry];
  /* Iterate through the SMSM callbacks linked list for this entry. */
  while (cb_link != NULL)
  {
    mask    = cb_link->cb_info.mask;
    cb_func = cb_link->cb_info.cb_func;
    cb_data = cb_link->cb_info.cb_data;

    /* call the callbacks if it is the state that it registered with. */
    if(cb_func && ((prev_state ^ curr_state) & mask ))
    {      
      cb_func( entry, prev_state, curr_state, cb_data );
    }
    cb_link = cb_link->next;
  }

  /* Unlock access to SMSM */
  smsm_cs_unlock(smsm_cs);

} /* smsm_do_cb */

/*===========================================================================
  FUNCTION  smsm_notify_other_hosts
===========================================================================*/
/*!
@brief
  This function sends interrupts to the other processors and notify them
  of its state changes.

@param  entry - a data entry
@param  prev_state - previous state of the entry
@param  curr_state - current state of the entry

@return None

@note   None
*/
/*=========================================================================*/
static void smsm_notify_other_hosts
(
  smsm_entry_type entry,
  uint32          prev_state,
  uint32          curr_state
)
{
  smsm_info_type  *info = &smsm_info;
  uint32           host;
  uint32           mask;

  ASSERT((uint32)entry < SMSM_NUM_ENTRIES && (uint32)entry < info->num_entries);
  
  for ( host = 0; host < SMSM_NUM_HOSTS && host < info->num_hosts; host++ )
  {
    /* interrupts the other hosts if needed, if an interrupt line exists */
    if( host != info->this_host && 
        info->interrupt_tbl[host].from != SMSM_INVALID_HOST && 
        info->interrupt_tbl[host].to   != SMSM_INVALID_HOST )
    {
      /* get the host interrupt mask */
      mask = smsm_intr_mask_get( entry , (smsm_host_type)host );

      /* send interrupt only if mask is enabled */
      if( (prev_state ^ curr_state) & mask ) 
      {
#ifdef FEATURE_SMSM_DEBUG_LOGGING
        /* Log the processor to which interrupt is being sent, own shadow and 
         * shared state */
        SMEM_LOG_EVENT(SMEM_SMSM_TX_EVENT, host, 
                       info->state_shadow[entry].state,
                       smsm_state_get(entry));
#endif /* FEATURE_SMSM_DEBUG_LOGGING */
        smsm_target_send_interrupt(info, host);
      }
    }
  }
}

/*===========================================================================

                    EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
  FUNCTION  smsm_state_get
===========================================================================*/
/*!
@brief
  This function reads in an entry state from the shared memory.

@param  entry - a data entry

@return The state of the specified entry

@note   This returns the state from the shared memory.
*/
/*=========================================================================*/
uint32 smsm_state_get(smsm_entry_type entry)
{
  uint32 state;

  /* make sure SMSM has been initialized */
  if(smsm_info.state_shared == NULL)
  {
    ERR_FATAL("smsm_state_get: smsm not initialized",0,0,0);
  }
  ASSERT((uint32)entry < smsm_info.num_entries);

  state = smsm_info.state_shared[entry]; 

  return state;
}

/*===========================================================================
  FUNCTION  smsm_get_local_state
===========================================================================*/
/*!
@brief
  This function reads in an entry state cached locally. This may not be
  the same as what is in the shared memory.

@param  entry - a data entry 

@return The state of the specified entry

@note   This returns the locally cached state for the specified entry
*/
/*=========================================================================*/
uint32 smsm_get_local_state(smsm_entry_type entry)
{
  uint32 state;
 
  ASSERT(entry < SMSM_NUM_ENTRIES);
  state = smsm_info.state_shadow[entry].state; 

  return state;
}

/*===========================================================================
  FUNCTION  smsm_state_set
===========================================================================*/
/*!
@brief
  This function sets the local processor states and interrupts the other
  processors as needed. It then processes the local callbacks associated
  with this state changes.

@param  entry  - a data entry 
@param  states - states to be set

@return None

@note
  Depending on the interrupt mask, the state chanages may or maynot be
  forwarded to the other hosts.
*/
/*=========================================================================*/
void smsm_state_set
(
  smsm_entry_type entry,
  uint32          states
)
{
  smsm_state_modify(entry, 0, states); 
} /* smsm_state_set */

/*===========================================================================
  FUNCTION  smsm_state_clr
===========================================================================*/
/*!
@brief
  This function clears the local processor states and interrupts the other
  processors as needed. It then processes the local callbacks associated
  with this state changes.
  
@param  entry  - a data entry
@param  states - states to be cleared

@return None

@note
  Depending on the interrupt mask, the state chanages may or maynot be
  forwarded to the other hosts.
*/
/*=========================================================================*/
void smsm_state_clr
(
  smsm_entry_type entry,
  uint32          states
)
{
  smsm_state_modify(entry, states, 0);
} /* smsm_state_clr */

/*===========================================================================
  FUNCTION  smsm_state_modify
===========================================================================*/
/*!
@brief
  This function sets/clears the local processor state and interrupts the
  other processors as needed. It then processes the local callbacks
  associated with this state changes.

@param  entry - a data entry
@param  clr_states - states to be cleared
@param  set_states - states to be set

@return None

@note
  Depending on the interrupt mask, the state chanages may or maynot be
  forwarded to the other hosts.
*/
/*=========================================================================*/
void smsm_state_modify
(
  smsm_entry_type entry,
  uint32          clr_states,
  uint32          set_states
)
{
  uint32           prev_state;
  uint32           curr_state;
  smsm_info_type  *info = &smsm_info;
  
  ASSERT((uint32)entry < SMSM_NUM_ENTRIES && (uint32)entry < info->num_entries);
  /* make sure only owner can set its state */
  ASSERT(info->state_shadow[entry].owner == info->this_host);
  
  /* Look access to SMSM */
  smsm_cs_lock(smsm_cs);

  prev_state = info->state_shared[entry];
  curr_state = (prev_state & (~clr_states)) | set_states;
  info->state_shared[entry] = curr_state ;
  info->state_shadow[entry].state = curr_state;
  
  /* make sure the memory is updated */ 
  SMSM_MEMORY_BARRIER(); 

  /* Unlock access to SMSM */
  smsm_cs_unlock(smsm_cs);

  /* Interrupt the other hosts so it can deal with the state changes */  
  smsm_notify_other_hosts(entry, prev_state, curr_state);
  
  /* Notify registered clients on local processor */
  smsm_do_cb( entry, prev_state, curr_state );
}

/*===========================================================================
  FUNCTION  smsm_init
===========================================================================*/
/*!
@brief
  This function allocates the shared states from the shared memory and
  initialize the local data structure. It then hooks up the interrupt
  handler.
  
@param  None

@return None

@note   None
*/
/*=========================================================================*/
void smsm_init( void )
{
  smsm_info_type     *info = &smsm_info;
  smsm_entry_type     entry;
  uint32              i    = 0;
  uint32              state;
  smsm_size_info_type *size_info;
  
  /* Check to see if SMSM has been initialized already */
  if(info->num_hosts != 0)
  {
    return;
  }
  
  memset(info, 0, sizeof(smsm_info_type));
  
  info->this_host = SMSM_HOST_PROC;
  size_info = smem_alloc( SMEM_SMSM_SIZE_INFO, 
      sizeof(smsm_size_info_type));
  if ( size_info == NULL )
  {
    ERR_FATAL( "smsm_init: Bad pointer from smem_alloc of SMSM size info.", 
               0, 0, 0 );
  }
  if (size_info->num_hosts == 0)
  {
    info->num_hosts   = SMSM_NUM_HOSTS;
    info->num_entries = SMSM_NUM_ENTRIES;
  }
  else
  {
    /* Just set these values locally as before since they don't change, so there
     * is no need to reread from shared memory after the first time. */
    info->num_hosts   = size_info->num_hosts;
    info->num_entries = size_info->num_entries;
  }
  ASSERT(info->num_hosts   <= SMSM_HOSTS_MAX);
  ASSERT(info->num_hosts   >  0);
  ASSERT(info->num_entries >= info->num_hosts);

  /* Initialize the SMSM critical section */
  smsm_cs = smsm_cs_create();
  
  /* allocate the shared info from the shared memory */
  info->state_shared = ( uint32 * )
    ( smem_alloc( SMEM_SMSM_SHARED_STATE, sizeof(uint32) * info->num_entries));

  /* allocate the interrupt mask from the shared memory.
     For each shared word, allocate N interrupt masks where N is the
     number of processors.
  */
  info->intr_mask =
    ( uint32 * ) smem_alloc( SMEM_SMSM_CPU_INTR_MASK,
            sizeof( uint32 ) * info->num_hosts * info->num_entries);

  /* make sure smem_alloc didn't return NULL */
  if ( ( info->state_shared == NULL ) ||
       ( info->intr_mask    == NULL ) )
  {
    ERR_FATAL( "Bad pointer from smem_alloc.", 0, 0, 0 );
  }
  
  /* Initialize the interrupt masks belonging to this host */
  for ( i=0; i < info->num_entries && i < SMSM_NUM_ENTRIES; i++ )
  {
    smsm_intr_mask_set(smsm_entry_tbl[i].entry, info->this_host, 0x0);
  }

  /* Initialize the interrupt table */
  memset((void *)info->interrupt_tbl,
         0x00,
         SMSM_NUM_HOSTS * sizeof(smsm_intr_info_type));

  /* build up the smsm interrupt table */
  smsm_target_init_interrupt(info);

  /* build up smsm local table, a table for this host only */
  for ( i = 0; i < SMSM_NUM_ENTRIES; i++ )
  {
    entry = smsm_entry_tbl[i].entry;
    
    info->state_shadow[entry].entry   = entry; /* for ease of debugging */
    info->state_shadow[entry].owner   = smsm_entry_tbl[i].owner;
    info->state_shadow[entry].state   = 0;
  }
  
  /* register interrupts */
  smsm_register_interrupt( info );
  
  /* read all the other host states after registering interrupt
     so we will not miss any upcoming hosts
   */ 
  for( i = 0; i < SMSM_NUM_ENTRIES && i < info->num_entries; i++ )
  {
    state = smsm_state_get((smsm_entry_type)i);
    info->state_shadow[i].state = state;
  }

  /**************************************************************** 
   * Allow SMSM initialization to be re-entrant from multiple
   * processes by using the SMSM_INIT bit. If not set, then
   * all states are cleared and SMSM_INIT and SMSM_MODEM_CONTINUE
   * are set.
   ****************************************************************/
  state = smsm_state_get( ( smsm_entry_type )info->this_host );
  if( 0 == ( state & SMSM_INIT ) )
  {
    smsm_state_modify( ( smsm_entry_type )info->this_host, 0xFFFFFFFF, 
                        SMSM_INIT | SMSM_MODEM_CONTINUE);
  }

  if (smem_version_set(SMEM_VERSION_SMSM, SMSM_VERSION_ID, 
                       SMEM_FULL_VERSION_MASK) == FALSE)
  {
    ERR_FATAL("smsm_init: version (%d) does not match all procs!",
        SMSM_VERSION_ID, 0, 0);
  }
}

/*===========================================================================
  FUNCTION  smsm_process_host_update
===========================================================================*/
/*!
@brief
  This function goes through entries owned by the host and locates the entry 
  that has changed. It then processes the client registered callbacks for 
  that entry.

@param  host - the interrupt source(host)

@return None

@note   This function can be called in interrupt context or task context.
*/
/*=========================================================================*/
void smsm_process_host_update(smsm_host_type host)
{
  smsm_shadow_type  *shadow;
  uint32             prev_state;
  uint32             curr_state;
  smsm_info_type    *info = &smsm_info;  
  uint32             entry;

  for(entry = 0; entry < SMSM_NUM_ENTRIES && entry < info->num_entries; 
      entry++)
  {
    shadow = &(info->state_shadow[entry]);
    
    /* read in the entry if it is owned by the host */
    if(shadow->owner == host)
    {
      /* Look access to SMSM */
      smsm_cs_lock(smsm_cs);

      prev_state = shadow->state;
      curr_state = smsm_state_get((smsm_entry_type)entry);

      /* Process SMSM_RESET first if changed. It may result in 
       * subsystem restart by system monitor so it's better 
       * not to spend time on logging or processing other events*/
      if (((prev_state ^ curr_state) & SMSM_RESET) != 0)
      {
        smsm_do_cb( (smsm_entry_type)entry, prev_state, prev_state^SMSM_RESET );
        prev_state ^= SMSM_RESET;
      }

#ifdef FEATURE_SMSM_DEBUG_LOGGING
      /* Log the host from which the interrupt came, Shadow and Shared state
       * for that host*/
      SMEM_LOG_EVENT(SMEM_SMSM_RX_EVENT, host, prev_state, curr_state);
#endif /* FEATURE_SMSM_DEBUG_LOGGING */    

      if (curr_state != prev_state)
      {
        shadow->state = curr_state;

        /* Unlock access to SMSM */
        smsm_cs_unlock(smsm_cs);

        /* call the callbacks registered for this entry state changes. */
        smsm_do_cb( (smsm_entry_type)entry, prev_state, curr_state );
      }
      else
      {
        /* Unlock access to SMSM */
        smsm_cs_unlock(smsm_cs);
      }
      
    }
  }
}

/*===========================================================================
  FUNCTION  smsm_cb_register
===========================================================================*/
/*!
@brief
  This function registers a client callback function which will be
  called when the specified bits in a specified entry changed.

  If the function already exists, then the existing mask is bit-wise OR'ed
  with \c mask.  If the function already exists, the callback data, 
  \c cb_data, must be identical.

@param  entry - a data entry
@param  mask - the bits in the entry
@param  cb_func - client specified callback function
@param  cb_data - client specified callback data

@return 0 if unable to register callback
@return 1 if new callback registered
@return 2 if existing callback updated

@note   None
*/
/*=========================================================================*/
int smsm_cb_register
(
 smsm_entry_type      entry,
 uint32               mask,
 smsm_cb_type         cb_func,
 void                *cb_data
)
{
  smsm_info_type     *info=&smsm_info;
  smsm_cb_info_type  *smsm_cb = NULL;
  smsm_cb_link_type  *cb_link = NULL;
  smsm_cb_link_type **last_link = NULL;
  int32               rc = 0;
  smsm_shadow_type   *shadow = NULL;
  boolean             found = FALSE;

  ASSERT((uint32)entry < SMSM_NUM_ENTRIES && (uint32)entry < info->num_entries);
  
  /* Lock access to SMSM */
  smsm_cs_lock(smsm_cs);

  /* Find an existing, matching callback, if any. */
  last_link = &(info->smsm_cb_tbl[entry]);
  cb_link = info->smsm_cb_tbl[entry];
  while (cb_link != NULL && !found)
  {
    smsm_cb = &cb_link->cb_info;
    
    if (smsm_cb->cb_func == cb_func && smsm_cb->cb_data == cb_data)
    {
      /* Update existing function registration. */
      smsm_cb->mask |= mask;
      rc = 2;
      found = TRUE;
    }
    else
    {
      last_link = &cb_link->next;
      cb_link = cb_link->next;
    }
  }

  if (!found)
  {
    ASSERT(cb_link == NULL);
    ASSERT(last_link != NULL && *last_link == NULL);
    /* Register new callback by appending to the end of the linked list. */
    cb_link = smsm_malloc(sizeof(smsm_cb_link_type));
    if (cb_link == NULL)
    {
      /* Unlock access to SMSM */
      smsm_cs_unlock(smsm_cs);
      ERR_FATAL("smsm_cb_register: malloc of cb_link failed.", 0, 0, 0);
    }
    memset(cb_link, 0, sizeof(smsm_cb_link_type));
    smsm_cb = &cb_link->cb_info;
    smsm_cb->mask    = mask;
    smsm_cb->cb_func = cb_func;
    smsm_cb->cb_data = cb_data;
    cb_link->next    = NULL;
    *last_link       = cb_link;

    /* Update the bit states masked by this cb, in order to avoid servicing 
     * state changes that occurred before it was registered.
     * Still has a data race where other local callbacks masked by the same 
     * bit(s) may ignore outstanding state changes, if any. */
    shadow = &(info->state_shadow[entry]);
    shadow->state = (shadow->state & ~mask) | 
          (smsm_state_get((smsm_entry_type)entry) & mask);
    rc = 1;
  }
  /* Enable requested interrupts */
  smsm_intr_mask_set(entry, info->this_host, smsm_cb->mask | 
                                smsm_intr_mask_get(entry, info->this_host));

  /* Unlock access to SMSM */
  smsm_cs_unlock(smsm_cs);

  return rc;

} /* smsm_cb_register */

/*===========================================================================
  FUNCTION  smsm_cb_deregister
===========================================================================*/
/*!
@brief
  This function deregisters a client callback function.
  
  To completely deregister a function, either pass in the identical mask
  used to register it or pass in all 1's.

@param  entry - a data entry
@param  mask - the bits to deregister (1 = deregister, 0 = no change)
@param  cb_func - client specified callback function

@return 0 if callback not found
@return 1 if callback found and mask adjusted
@return 2 if callback found and deregistered

@note   None
*/
/*=========================================================================*/
int smsm_cb_deregister
(
 smsm_entry_type     entry,
 uint32              mask,
 smsm_cb_type        cb_func
)
{
  smsm_info_type     *info    = &smsm_info;
  smsm_cb_info_type  *smsm_cb = NULL;
  smsm_cb_link_type  *cb_link = NULL;
  smsm_cb_link_type **prev_link = NULL;
  int32               rc = 0;
  uint32              new_mask = 0;
  
  ASSERT((uint32)entry < SMSM_NUM_ENTRIES && (uint32)entry < info->num_entries);
  
  /* Lock access to SMSM */
  smsm_cs_lock(smsm_cs);

  prev_link = &(info->smsm_cb_tbl[entry]);
  cb_link = info->smsm_cb_tbl[entry];
  while (cb_link != NULL)
  {
    smsm_cb = &cb_link->cb_info;

    if(smsm_cb->cb_func == cb_func)
    {
      smsm_cb->mask &= ~mask;
      
      if(!smsm_cb->mask)
      {
        /* Since bit mask is empty, the callback can be completely removed. */
        *prev_link = cb_link->next;
        cb_link->next = NULL;
        if (smsm_free(cb_link) != 0)
        {
          ERR("smsm_cb_deregister: Free of cb_link failed.", 0, 0, 0);
        }
        smsm_cb = NULL;
        rc = 2;
      }
      else
      {
        new_mask |= smsm_cb->mask;
        prev_link = &cb_link->next;
        rc = 1;
      }
    }
    else
    {
      new_mask |= smsm_cb->mask;
      prev_link = &cb_link->next;
    }
    cb_link = *prev_link;
  }
  /* Set the new interrupt mask for this entry. */
  smsm_intr_mask_set(entry, info->this_host, new_mask);
  
  /* Unlock access to SMSM */
  smsm_cs_unlock(smsm_cs);
  
  return rc;

} /* smsm_cb_dereg */

/*==============================================================================
  FUNCTION:  smsm_set_reset
===============================================================================*/
/**
  This function sets the SMSM_RESET state and clears the specified states.
  
  It then sends an SMSM interrupt to all processors registered for changes in
  the local SMSM_RESET bit.  This is usually Apps, to handle Error cases and
  Subsystem Restart.

  @param[in]    SMSM bit states to be cleared

  @return       None

  @note
  1.) This function should only be called from single-threaded environment, 
  it's NOT thread-safe.
  2.) It must not cause any further fatal exceptions, errors, or assertions.
  3.) It does no logging or other extraneous overhead.

  @sideeffects
  May notify remote subsystems that the local processor has hit an error 
  condition, which may trigger either a system crash or a subsystem restart.
*/
/*============================================================================*/
void smsm_set_reset
(
  uint32 state 
)
{
  uint32   prev_state; 
  uint32   curr_state;
  smsm_info_type   *info = &smsm_info;
  uint32   host;
  uint32   mask;

  /* If SMSM has not been initialized yet, just return to avoid null references
   * or recursive failure conditions.  Warning: this means the interrupt will
   * not be sent and the remote processors will not be notified of the failure
   * via SMSM.
   */
  if(info->num_hosts == 0)
  {
    return;
  }

  /* This function is not thread-safe. It's assumed that interrupts are
     locked if more than one thread could call into this function concurrently
  */
  
  prev_state = ( uint32 )info->state_shared[info->this_host];
  curr_state = ( prev_state & ~state ) | SMSM_RESET ;
  info->state_shared[info->this_host] = curr_state;  
  info->state_shadow[info->this_host].state = curr_state;
  
  /* make sure the memory is updated */ 
  SMSM_MEMORY_BARRIER(); 

  for ( host = 0; host < SMSM_NUM_HOSTS && host < info->num_hosts; host++ )
  {
    /* interrupts the other hosts if needed, if an interrupt line exists */
    if( host != info->this_host && 
        info->interrupt_tbl[host].from != SMSM_INVALID_HOST && 
        info->interrupt_tbl[host].to   != SMSM_INVALID_HOST )
    {
      /* get the host interrupt mask */
      mask = *((uint32 *)info->intr_mask + 
          ((smsm_entry_type)info->this_host * info->num_hosts) + 
          (smsm_host_type)host);

      /* send interrupt only if mask is enabled */
      if( (prev_state ^ curr_state) & mask ) 
      {
        smsm_target_send_interrupt(info, host);
      }
    }
  }
} /* smsm_set_reset */

/*==============================================================================
   The following are for backward compatibility
===============================================================================*/

/*===========================================================================
  FUNCTION  smsm_shim_get_host_entry
===========================================================================*/
/*!
@brief
  Returns the smsm_entry_type corresponding to a host's primary entry. This
  is used by the SMSM_THIS_HOST and SMSM_OTHER_HOST macros.

@param  this_host - Is the requested host SMSM_THIS_HOST?

@return Per the following table:

        Host     this_host  Return
        ----     ---------  ------
        Apps     FALSE      SMSM_MODEM_STATE
        Apps     TRUE       SMSM_APPS_STATE
        Modem    FALSE      SMSM_APPS_STATE
        Modem    TRUE       SMSM_MODEM_STATE
        Other               SMSM_INVALID_ENTRY

@note   SMSM must have been initialized via smsm_init().
*/
/*=========================================================================*/
smsm_entry_type smsm_shim_get_host_entry
(
  boolean             this_host
)
{
  /* Assert: SMSM has been initialized */
  ASSERT(smsm_info.num_hosts > 0);

  switch(smsm_info.this_host)
  {
    case SMSM_APPS:
      if(this_host)
      {
        return SMSM_APPS_STATE;
      }
      else
      {
        return SMSM_MODEM_STATE;
      }
    case SMSM_MODEM:
      if(this_host)
      {
        return SMSM_MODEM_STATE;
      }
      else
      {
        return SMSM_APPS_STATE;
      }
    default:
      return SMSM_INVALID_ENTRY;
  }
}

/*===========================================================================
  FUNCTION  smsm_host_entry_get
===========================================================================*/
/*!
@brief
  This function returns the host entry.

@param  None

@return The host entry.

@note   None
*/
/*=========================================================================*/
smsm_entry_type smsm_get_host_entry(smsm_host_type host)
{
  smsm_entry_type entry = SMSM_INVALID_ENTRY;
  
  switch( host )
  {
  case SMSM_MODEM:
    entry = SMSM_MODEM_STATE;
    break;
  
  case SMSM_APPS:
    entry = SMSM_APPS_STATE;
    break;
    
  case SMSM_Q6:
    entry = SMSM_Q6_STATE;
    break;

  case SMSM_WCNSS:
    entry = SMSM_WCNSS_STATE;
    break;

  default:
    ERR_FATAL( "Invalid host type.", 0, 0, 0 );
    break;
  }

  return entry;
} /* smsm_get_host_entry */
