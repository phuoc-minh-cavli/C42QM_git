#ifndef SMD_OS_H
#define SMD_OS_H

/*===========================================================================

                      Shared Memory OS Header File


 Copyright (c) 2008-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_os.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/17/14   rv      Timer undef is added.
01/10/14   bc      SMD port searching optimization
                   Added signals for remote event processing
                   Removed SMD_EVENT_SIG to handle race condition in 
                   smd_task_loop function in smd_rex.c
02/11/13   bt      Remove OS string copy macro.
08/17/12   bt      Move SMEM memory access macros to smd_proc.h.
08/13/12   bt      Make memory access volatile to prevent optimizing to smaller 
                   accesses - RPM requires word-access, cannot byte-access.
04/05/12   bt      Moved interrupt defs, headers to smd_proc.h.
02/01/12   pa      Removing inclusion of deprecated customer.h
01/06/12   bt      Remove headers deprecated in 8974, use DAL memory_barrier().
12/14/11   bt      Replace memcpy_v with memcpy, which is fully supported by 
                   Q6 v4, for faster copy speeds, esp. involving unaligned data.
11/07/11   bt      Remove smd_smsm_isr() as all interrupts now separated. 
07/11/11   bm      + Moved NULL interrupt definitions to this file
                   + Typedefined smd_os_proc_type and smd_os_intr_type.
03/25/11   hwu     Merge in 8960 change. 
02/11/11   bt      Remove SMD_DSM_INIT_MUTEX from smd_dsm.c and smd_os.h files.
12/02/10   tl/rs   Removed unused cross process memory access for WM6 
09/27/10   tl/ih   Added SMD_CMD_SIG to signal SMD to process the command queue
06/08/10   tl      Split SMD task into multiple tasks with different priorities
03/11/10   rs      + Refactored SMD interrupts
                   + Cleared compiler warnings
01/20/10   hwu     Added change to support interrupt bits.
11/24/09   tl      Added 9600 target support
04/13/09   jlk     Added smd_sio timer macros
02/20/09   hwu     Remove the DEM macros.
02/04/09    sa     cleanup msg macros
11/18/08   hwu     Changed to call dem_notification_reg_smd(). 
09/15/08   hwu     Added smd_smsm_isr.
09/15/08   jlk     added SMD_DSM_INIT_MUTEX() stub
08/19/08   bfc     Fixed target/OS SMD abstraction for rex.
08/08/08   bfc     Added support for a block port loopback.
06/11/08   sa      Removing smd_start_ist and using ist table instread
06/05/08   sa      Initial smd rex header file split up from smd_target.h. 
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "stdlib.h"
#include "string.h"
#include "err.h"
#include "assert.h"
#include "qurt.h"
#include "timer.h"  /* Clock services definitions           */
#include "qurt_memory.h"

/*===========================================================================
                        MACRO DEFINITIONS
===========================================================================*/
/* -------------------------------------------------------------
   Memory barrier for REX
------------------------------------------------------------- */
#define SMD_MEMORY_BARRIER()   memory_barrier()

/*-------------------------------------------------------------------------
 *                    SMD TASK signal definitions
 *-------------------------------------------------------------------------*/
/* Upper bits are used to represent processor for smd port search 
 * Processor bit will be shifted and transform to smem_host_type */
#define SMD_EVENT_HOST_BIT_SHIFT   16         /* Number of bits to shift */
#define SMD_ALL_PROC_MASK          0x00000057 /* To safely get proc bit only */

/* Processor bits are based on BIT MASK Macros defined in smd_internal.h */
#define SMD_PROC_SIG_MASK          0x00000001 /* To check if proc sig is on */
#define SMD_EVENT_APPS_SIG         0x00010000 
#define SMD_EVENT_MODEM_SIG        0x00020000
#define SMD_EVENT_ADSP_SIG         0x00040000
#define SMD_EVENT_WCN_SIG          0x00100000
#define SMD_EVENT_RPM_SIG          0x00400000

#define SMD_CMD_SIG                0x00000002 /* Signal when queued commands */

#define SMD_SMSM_EVENT_SIG         0x00000010

/* Message macros */
#define SMD_MSG_HIGH( m, a1, a2, a3 ) MSG_HIGH( m, a1, a2, a3 )
#define SMD_MSG_MED( m, a1, a2, a3 )  MSG_MED( m, a1, a2, a3 )
#define SMD_MSG_LOW( m, a1, a2, a3 )  MSG_LOW( m, a1, a2, a3 )
#define SMD_MSG_ERROR( m, a1, a2, a3 ) MSG_ERROR( m, a1, a2, a3 )
#define SMD_MSG_FATAL( m, a1, a2, a3 ) ERR_FATAL( m, a1, a2, a3 )

/* timer functions used in smd_sio */
#define SMD_SIO_TIMER_TYPE timer_type 

#define SMD_SIO_TIMER_DEF(timer, func, data) \
    timer_def_osal(timer, NULL, TIMER_FUNC1_CB_TYPE, func, data)

#define SMD_SIO_TIMER_CLR(timer) timer_clr(timer, T_NONE)

#define SMD_SIO_TIMER_UNDEF(timer) timer_undef(timer)

#define SMD_SIO_TIMER_SET(timer, time) \
     timer_set(timer, time, 0, T_MSEC)

/*===========================================================================
                        TYPE DEFINITIONS
===========================================================================*/
/**
 * QuRT-specific information for SMD task context
 */
typedef struct smd_os_context_struct
{
  boolean        is_ist;
  qurt_signal_t *signal;
} smd_os_context_type;

#endif /* SMD_OS_H */
