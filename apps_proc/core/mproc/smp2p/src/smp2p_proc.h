#ifndef SMP2P_PROC_H
#define SMP2P_PROC_H

/*---------------------------------------------------------------------------
          Shared Memory State Manager Processor-Specific Definitions
---------------------------------------------------------------------------*/
/*!
  @file
    SMP2P_proc.h

  @brief
    This file contains the SMP2P processor-specific definitions.
*/

/*--------------------------------------------------------------------------
     Copyright  2013,2019 Qualcomm Technologies Incorporated.
     All rights reserved.
---------------------------------------------------------------------------*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smp2p/src/smp2p_proc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/15   db      Correctly set Apps->Modem destination processor
09/25/13   bt      Change ASSERT_BRK to ASSERT by default.
09/16/13   bt      Add RESET_LOCAL state and reset handshake.
08/19/13   pa      Added SMP2P_LOG_EX.
01/22/13   bt      Created processor-specific file for B-family TN Apps.
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "DDIIPCInt.h"
#include "assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        MACRO DEFINITIONS
===========================================================================*/
#ifdef FEATURE_SMP2P_ASSERT_BRK
/*  To enable this, define FEATURE_SMP2P_ASSERT_BRK, or just change the above
 *  line to #if 1. */
/** Breakpoint assert.  Halts the processor at the assert location rather
 *  than going through the error handling code.
 *  Useful for bringup of new code.
 */
#define ASSERT_BRK(cond) \
  _Pragma("push") \
  _Pragma("arm") \
  if (!(cond)) { __asm { bkpt 0 } } \
  _Pragma("pop")
#else
#define ASSERT_BRK  ASSERT
#endif

/** \name SMP2P Host
 *
 * This macro defines the local processor in which SMP2P runs.
 *
 * @{
 */
#define SMP2P_THIS_HOST        SMEM_APPS
/*@}*/

/* Feature flags (smp2p_header_version_type.flags, 24 bits) */
#define SMP2P_FFLAGS_SSR_HANDSHAKE      0x000001

#define SMP2P_FFLAGS_SUPPORTED  (SMP2P_FFLAGS_SSR_HANDSHAKE)
#define SMP2P_FFLAGS_REQUESTED  (SMP2P_FFLAGS_SSR_HANDSHAKE)

/* Max entries */
#define SMP2P_ENTRIES_TOTAL   16

/** SMP2P log macro.
 *  This macro defines the log routines used by SMP2P to track events that
 *  occur at runtime.
 *  This is defined in the processor specific header file so that each
 *  processor may define it's own policy. */
#define SMP2P_LOG(a,b,c,d)      smp2p_os_log(a,b,c,d, SMP2P_FLAGS_NONE)

/** SMP2P log macro with flags */
#define SMP2P_LOG_EX(a,b,c,d,e) smp2p_os_log(a,b,c,d,e)

/** Defines the number of entries in the SMP2P local log */
#define SMP2P_LOG_NUM_ENTRIES 16

/** This define, if present, enables local logging */
#define SMP2P_LOG_LOCAL

/** This define, if present, enables logging to SMEM log */
/* #define SMP2P_LOG_SMEM */

/*-------------------------------------------------------------------------
  Interrupt Defines
-------------------------------------------------------------------------*/
/** NULL Interrupt definitions */
#define SMP2P_NULL_IN_INTERRUPT  ( DALIPCINT_INT_32BITS )
#define SMP2P_NULL_OUT_INTERRUPT SMP2P_NULL_IN_INTERRUPT

/** \name SMP2P Target Modem
 *
 * These values represent the target specific IRQs and ISRs supported.
 *
 * @{
 */
/** No interrupts from/to self. */
#define SMP2P_APPS_IN          SMP2P_NULL_IN_INTERRUPT
#define SMP2P_APPS_OUT         SMP2P_NULL_OUT_INTERRUPT
#define SMP2P_APPS_IPCPROC     DALIPCINT_PROC_NULL

#define SMP2P_MPSS_IN          59
#define SMP2P_MPSS_OUT         DALIPCINT_GP_2
#define SMP2P_MPSS_OUT_M       0 /* Not used with DAL. */
#define SMP2P_MPSS_IPCPROC     DALIPCINT_PROC_MDSPSW

#define SMP2P_ADSP_IN          190
#define SMP2P_ADSP_OUT         DALIPCINT_GP_2
#define SMP2P_ADSP_OUT_M       0
#define SMP2P_ADSP_IPCPROC     DALIPCINT_PROC_ADSP

#define SMP2P_WCNSS_IN         175
#define SMP2P_WCNSS_OUT        DALIPCINT_GP_2
#define SMP2P_WCNSS_OUT_M      0
#define SMP2P_WCNSS_IPCPROC    DALIPCINT_PROC_WCN

/** No interrupts from/to DSPS. */
#define SMP2P_DSPS_IN          SMP2P_NULL_IN_INTERRUPT
#define SMP2P_DSPS_OUT         SMP2P_NULL_OUT_INTERRUPT
#define SMP2P_DSPS_OUT_M       0
#define SMP2P_DSPS_IPCPROC     DALIPCINT_PROC_NULL

/** No interrupts from/to Q6FW. */
#define SMP2P_Q6FW_IN          SMP2P_NULL_IN_INTERRUPT
#define SMP2P_Q6FW_OUT         SMP2P_NULL_OUT_INTERRUPT
#define SMP2P_Q6FW_OUT_M       0
#define SMP2P_Q6FW_IPCPROC     DALIPCINT_PROC_NULL

/** No interrupts from/to RPM. */
#define SMP2P_RPM_IN           SMP2P_NULL_IN_INTERRUPT
#define SMP2P_RPM_OUT          SMP2P_NULL_OUT_INTERRUPT
#define SMP2P_RPM_OUT_M        0
#define SMP2P_RPM_IPCPROC      DALIPCINT_PROC_NULL
/*@}*/

/*===========================================================================
                        TYPE DEFINITIONS
===========================================================================*/
/** OS abstraction over DAL types */
typedef DalIPCIntProcessorType smp2p_os_proc_type;
typedef DalIPCIntInterruptType smp2p_os_intr_type;

#ifdef __cplusplus
}
#endif

#endif  /* SMP2P_PROC_H */
