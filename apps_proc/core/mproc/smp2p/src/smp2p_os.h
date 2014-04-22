/*
 * @file smp2p_os.h
 *
 * OS specific routines and definitions for SMP2P.
 */

/*==============================================================================
     Copyright (c) 2012 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smp2p/src/smp2p_os.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/10/12   pa      Initial revision.
===========================================================================*/
#ifndef SMP2P_OS_H_
#define SMP2P_OS_H_

#include "qurt.h"
#include "qurt_memory.h"

/** SMP2P OS parameters
 * Contains any OS-specific variables needed by the SMP2P API.
 * Currently only defined for DAL, though other OS's may be added.
 */
typedef struct
{
  qurt_mutex_t     mutex;
  qurt_signal_t    own_signal;
  qurt_signal_t    *signal;        /* Signal used for setting and waiting */
  boolean          event_set;
  boolean          waiting;
} smp2p_os_params_type;

/* -------------------------------------------------------------
   Memory barrier for REX
------------------------------------------------------------- */
#define SMP2P_MEMORY_BARRIER() memory_barrier()

#endif /* SMP2P_OS_H_ */
