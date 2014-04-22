/*------------------------------------------------------------------------------
   SMSM_INTERNAL.H  - SMSM Internal Header 
------------------------------------------------------------------------------*/
/*!
  @file
    smsm_internal.h

  @brief
    This file contains the SMSM internal data structure and APIs. 
*/

/*------------------------------------------------------------------------------
     Copyright  2008 - 2013 Qualcomm Technologies Incorporated. 
     All rights reserved.
------------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smsm/src/smsm_internal.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/01/13   bt      Add required smsm.h inclusion.
11/06/12   bt      Remove prototypes for now-static target function.
06/21/12   bt      Make smsm_intr_mask_get/set static, to enforce as internal.
04/13/12   bt      Move smsm_target function prototypes to smsm_internal.h.
02/12/12   bm      Adding SMSM interrupt Tx/Rx events.
11/02/11   bt      Split SMD/SMSM interrupts Modem<->LPASS, clean up code.
10/31/11   bm      Moved smsm_size_info_type structure definition into
                   smem_boot.h as smem_smsm_size_info_type.
09/06/11   bt      Made smsm_cb_tbl dynamically-allocated to optimize memory.
07/20/11   bm      Correction in smsm_intr_info_type struct.
07/12/11   bm      Use smsm_os_proc/intr_type, abstraced by smsm_os.h
06/29/11   tl      Integrated WoA support: Don't use DALIpcInt on WoA
06/17/11   hwu     Seperate out SMSM from SMEM.
05/18/11   tl      Don't include DDIIPCInt.h directly; rather, include
                   smem_target.h, which will include DDIIPCInt.h if available
04/08/11   rs      Use OS abstraction to implement SMSM critical sections 
03/24/11   bt      Added defines for smsm_size_info_type and SMSM_VERSION_ID.
10/13/10   bt      Added processor field in intr_info to support DALIPCInt.
01/20/10   hwu     Added the irq_out_mask to specify the bit in irq_out reg.
01/19/10   esh     Unused code removal
11/25/09   rs      CMI updates
08/27/09   hwu     Moved smsm_mutex_xxx to smem_os.h. 
04/13/09   hwu     Remove smsm_notify_cb().
12/05/08   hwu     Correct a typo.
11/20/08   hwu     Remove the running flag.
10/21/08   hwu     added smem_target_init_interrupt.
01/05/08   hwu     Initial version
===========================================================================*/
#ifndef SMSM_INTERNAL_H
#define SMSM_INTERNAL_H

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smsm_os.h"
#include "smsm.h"
#include "smsm_proc.h"
#include "smem_boot.h"
/*-------------------------------------------------------------------------
  Defines
-------------------------------------------------------------------------*/
#define SMSM_VERSION_ID         0x00000001

#define SMSM_MAX_CALLBACKS      20 /* max callbacks for each entry */
#define SMSM_HOSTS_MAX          16  /* max number of processors supported */

#define SMEM_SMSM_RX_EVENT     (SMEM_LOG_DEBUG_EVENT_BASE +  1)
#define SMEM_SMSM_TX_EVENT     (SMEM_LOG_DEBUG_EVENT_BASE +  2)

/*--------------------------------------------------------------------------
  Enumerated types
---------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
  Structure definitions
--------------------------------------------------------------------------*/

/**
   State Entry definition
*/
typedef struct
{
  smsm_entry_type  entry;
  smsm_host_type   owner;
}smsm_entry_info_type;

/**
   Callback related info
*/
typedef struct
{
  uint32            mask;
  smsm_cb_type      cb_func;
  void             *cb_data;
}smsm_cb_info_type;

/**
   Linked node for callback info
*/
typedef struct smsm_cb_link_struct
{
  smsm_cb_info_type           cb_info;
  struct smsm_cb_link_struct *next;
}smsm_cb_link_type;

/**
   Callback related info
*/
typedef struct 
{
  smsm_host_type          from;
  smsm_host_type          to;
  /** destination host type */
  smsm_os_proc_type       processor;
  /** outgoing interrupt register */
  smsm_os_intr_type       irq_out;
  /** interrupt bits in irq_out register */
  uint32                  irq_out_mask;
  /** incoming interrupt register */
  uint32                  irq_in;
}smsm_intr_info_type;

/**
   Shadow Entry info
*/
typedef struct
{
  smsm_entry_type   entry;
  smsm_host_type    owner;
  smsm_state_type   state;
}smsm_shadow_type;

/**
   SMSM hosts and entries size information.

   These values are set in smem_boot_init() in the Apps' sbl2, before any 
   processor calls smem_init().  In each smem_init(), the SMSM shared memory
   items will be allocated with a size based on these values.  E.g., the
   state_shared has num_entries words, and the intr_mask is a 2D array of 
   dimensions num_hosts * num_entries.  This will prevent future interprocessor
   dependencies on these values because they must only be changed once when 
   set in smem_boot_init().

   The actual structure is defined in smem_boot.h to make SMEM boot code
   independent of SMSM drivers.
*/
typedef smem_smsm_size_info_type smsm_size_info_type;

/**
   SMSM info 
*/
typedef struct 
{
  uint32                   num_hosts;
  smsm_host_type           this_host;
  uint32                   num_entries;
  volatile uint32         *state_shared;
  volatile uint32         *intr_mask;
  smsm_intr_info_type      interrupt_tbl[SMSM_NUM_HOSTS];
  smsm_shadow_type         state_shadow[SMSM_NUM_ENTRIES];
  smsm_cb_link_type       *smsm_cb_tbl[SMSM_NUM_ENTRIES];
}smsm_info_type;

/*--------------------------------------------------------------------------
  Function declarations
---------------------------------------------------------------------------*/
/*===========================================================================
  FUNCTION  smsm_register_interrupt
===========================================================================*/
/*!
@brief
  This function registers SMSM interrupt.

@param  info - pointer to the smsm data struct.

@return None

@note   None
*/
/*=========================================================================*/
void smsm_register_interrupt(smsm_info_type *info);

/*===========================================================================
  FUNCTION  smsm_send_interrupt
===========================================================================*/
/*!
@brief
  This function sends out the interprocessor interrupts.

@param  intr - pointer to the interrupt info.

@return None

@note   None
*/
/*=========================================================================*/
void smsm_send_interrupt(smsm_intr_info_type *intr);

/*===========================================================================
  FUNCTION  smsm_process_host_update
===========================================================================*/
/*!
@brief
  This function goes through entries owend by the host and locate the entry 
  that has changed. It then processes the client registered callbacks for 
  that entry.

@param  param - the interrupt source

@return None

@note   None
*/
/*=========================================================================*/
void smsm_process_host_update(smsm_host_type host);

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
uint32 smsm_get_local_state( smsm_entry_type entry );

/*===========================================================================
  FUNCTION  smsm_get_host_entry
===========================================================================*/
/*!
@brief
  This function returns the host entry.

@param  None

@return the host entry.

@note   None
*/
/*=========================================================================*/
smsm_entry_type smsm_get_host_entry(smsm_host_type host );

/*===========================================================================
  FUNCTION  smsm_target_init_interrupt
===========================================================================*/
/*!
@brief
  This function initialize the interprocessor interrupts for this target.

@param  info - pointer to the smsm_info structure
 
@return None

@note   None
*/
/*=========================================================================*/
void smsm_target_init_interrupt(smsm_info_type *info);

#endif /* SMSM_INTERNAL_H */
