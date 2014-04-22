/*---------------------------------------------------------------------------
   SMEM_OS.H  - REX Specific Header for SMEM
---------------------------------------------------------------------------*/
/*!
  @file
    smsm_os.h

  @brief
    This file contains the REX specific SMSM definitions.
*/

/*--------------------------------------------------------------------------
     Copyright  2008 - 2012 Qualcomm Technologies Incorporated.
     All rights reserved.
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smsm/src/smsm_os.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/05/12   bt      Moved interrupt defs, headers to smsm_proc.h.
02/21/12   bt      Remove smsm_poll_timeout, no timetick calls in SMSM now.
02/02/12   pa      Removed deprecated header file
01/19/23   bt      Featurize DALFramework, not needed on DSPS.
12/21/11   rs      Redefined SMSM memory barrier as OS specific. 
08/31/11   bt      Remove redefinition of ERR_FATAL, now provided by Err.
08/08/11   bt      Remove cache_mmu.h from DSPS too, move comdef.h first.
07/11/11   bm      + Moved SMSM NULL interrupt definitions to this file
                   + Typedefined smsm_os_proc_type.
06/17/11   hwu     Initial version.
===========================================================================*/
#ifndef SMSM_OS_H
#define SMSM_OS_H

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "err.h"
#include "assert.h"
#include "qurt_memory.h"
#ifndef IMAGE_DSPS_PROC  /* DSPS uses smem_generic_memory_barrier() instead */
#include "DALFramework.h"
#endif

/*--------------------------------------------------------------------------
  Type definitions
---------------------------------------------------------------------------*/

/** OS Macro redefinitions for DSPS target 
 */
#ifdef IMAGE_DSPS_PROC
#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT( cond ) \
      if( !( cond ) ) \
      {                 \
        while(1);         \
      }
#endif

/* -------------------------------------------------------------
   Memory barrier for REX
------------------------------------------------------------- */
#ifndef IMAGE_DSPS_PROC
#define SMSM_MEMORY_BARRIER() memory_barrier()
#else
#define SMSM_MEMORY_BARRIER() smem_generic_memory_barrier()
#endif

#endif /* SMSM_OS_H */
