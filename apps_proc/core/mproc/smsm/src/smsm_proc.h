#ifndef SMSM_PROC_H
#define SMSM_PROC_H

/*---------------------------------------------------------------------------
          Shared Memory State Manager Processor-Specific Definitions
---------------------------------------------------------------------------*/
/*!
  @file
    smsm_proc.h

  @brief
    This file contains the SMSM processor-specific definitions.
*/

/*--------------------------------------------------------------------------
     Copyright  2012 Qualcomm Technologies Incorporated. 
     All rights reserved.
---------------------------------------------------------------------------*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smsm/src/smsm_proc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/12   bt      Correct SMSM Dalipcint to WCNSS (just in case).
08/06/12   bt      Use DALIPCINT_PROC_NULL, since Sparrow has DAL.
08/01/12   bt      Update incoming intr from Modem for HW bringup flatfile.
05/04/12   pa      Adding APPS-RPM support for 9x25 sparrow
                   Removed HWIO macros in favor of DAL macros for use with
                   Sparrow
04/02/12   bt      Created processor-specific file for B-family Apps.
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "DDIIPCInt.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        MACRO DEFINITIONS
===========================================================================*/
/** \name SMSM Host
 *
 * This macro defines the local processor in which SMSM runs.
 *
 * @{
 */
#define SMSM_HOST_PROC   ( SMSM_APPS )
#define SMSM_APPS_PROC
/*@}*/

/*-------------------------------------------------------------------------
  Interrupt Defines
-------------------------------------------------------------------------*/
/** NULL Interrupt definitions */
#define SMSM_NULL_IN_INTERRUPT  ( DALIPCINT_INT_32BITS )
#define SMSM_NULL_OUT_INTERRUPT SMSM_NULL_IN_INTERRUPT

/** \name SMSM Target Apps
 *
 * These values represent the target specific IRQs and ISRs supported.
 *
 * @{
 */
/** No interrupts from/to self. */
#define SMSM_APPS_IN          SMSM_NULL_IN_INTERRUPT
#define SMSM_APPS_OUT         SMSM_NULL_OUT_INTERRUPT
#define SMSM_APPS_IPCPROC     DALIPCINT_PROC_NULL

/** Apps<-ModemSW: mss_sw_to_kpss_ipc_irq1 -> CsrIrq5: 26 + 32 = 50 */
#define SMSM_MPSS_IN          58
/* ##Out interrupts require mapping of appropriate register to compile.## */
#define SMSM_MPSS_OUT         DALIPCINT_GP_1
#define SMSM_MPSS_OUT_M       0 /* Not used with DAL */
#define SMSM_MPSS_IPCPROC     DALIPCINT_PROC_MDSPSW

/** Apps<-LPASS: lpass_to_kpss_ipc_irq1 -> CsrIrq5: 157 + 32 = 189 */
#define SMSM_ADSP_IN          189
#define SMSM_ADSP_OUT         DALIPCINT_GP_1
#define SMSM_ADSP_OUT_M       0 /* Not used with DAL */
#define SMSM_ADSP_IPCPROC     DALIPCINT_PROC_ADSP

/** Apps<-RIVA: RivaAppsWlanSmsmIrq: 144 + 32 = 176 */
#define SMSM_WCNSS_IN         176
#define SMSM_WCNSS_OUT        DALIPCINT_GP_3
#define SMSM_WCNSS_OUT_M      0 /* Not used with DAL */
#define SMSM_WCNSS_IPCPROC    DALIPCINT_PROC_WCN
/*@}*/

/*===========================================================================
                        TYPE DEFINITIONS
===========================================================================*/
/** OS abstraction interrupt types */
typedef DalIPCIntProcessorType smsm_os_proc_type;
typedef DalIPCIntInterruptType smsm_os_intr_type;

#ifdef __cplusplus
}
#endif

#endif  /* SMSM_PROC_H */
