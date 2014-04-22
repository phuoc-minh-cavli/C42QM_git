#ifndef SMD_PROC_H
#define SMD_PROC_H

/*---------------------------------------------------------------------------
            Shared Memory Driver Processor-Specific Definitions
---------------------------------------------------------------------------*/
/*!
  @file
    smd_proc.h

  @brief
    This file contains the SMD processor-specific definitions.
*/

/*--------------------------------------------------------------------------
     Copyright  2012-2014,2018 Qualcomm Technologies Incorporated. 
     All rights reserved.
---------------------------------------------------------------------------*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_proc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/06/13   an      Add cache management support.
01/22/13   bt      Add dummy SMD_RPM_OUT_M macro.
09/12/12   bt      Add SMEM access macros, from smd_os.h, and copy func choice.
08/06/12   bt      Change to use smem_host_type's.
08/01/12   bt      Update incoming intr from Modem for HW bringup flatfile.
05/04/12   pa      Adding APPS-RPM support for 9x25 sparrow
                   Removed HWIO macros in favor of DAL macros for use with
                   Sparrow
04/01/12   bt      Created processor-specific file for B-family Apps.
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smd_v.h"
#include "DDIIPCInt.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        MACRO DEFINITIONS
===========================================================================*/
/** \name SMD Host
 *
 * This macro defines the local processor in which SMD runs.
 *
 * @{
 */
#define SMD_THIS_HOST   ( SMEM_APPS )
#define SMD_APPS_PROC
#define SMD_NUM_CHANNEL_INFO SMD_NUM_CHANNELS
/*@}*/

/*---------------------------------------------------------------------------
  Interrupt Defines
---------------------------------------------------------------------------*/
/** NULL Interrupt definitions */
#define SMD_NULL_IN_INTERRUPT  ( DALIPCINT_INT_32BITS )
#define SMD_NULL_OUT_INTERRUPT ( (DalIPCIntInterruptType)SMD_NULL_IN_INTERRUPT)

/** \name SMD Target Apps
 *
 * These values represent the target specific IRQs and ISRs supported.
 *
 * @{
 */
/** No interrupts from/to self. */
#define SMD_APPS_IN           SMD_NULL_IN_INTERRUPT
#define SMD_APPS_OUT          SMD_NULL_OUT_INTERRUPT
#define SMD_APPS_IPCPROC      DALIPCINT_PROC_NULL

/** Apps<-ModemSW: mss_sw_to_kpss_ipc_irq0 -> CsrIrq4: 25 + 32 = 57 */
#define SMD_MPSS_IN           57
/* ##Out interrupts require mapping of appropriate register to compile.## */
#define SMD_MPSS_OUT          DALIPCINT_GP_0
#define SMD_MPSS_OUT_M        0 /* Not used with DAL */
#define SMD_MPSS_IPCPROC      DALIPCINT_PROC_MDSPSW

/** Apps<-LPASS: lpass_to_kpss_ipc_irq0 -> CsrIrq4: 156 + 32 = 188 */
#define SMD_ADSP_IN           188
#define SMD_ADSP_OUT          DALIPCINT_GP_0
#define SMD_ADSP_OUT_M        0 /* Not used with DAL */
#define SMD_ADSP_IPCPROC      DALIPCINT_PROC_ADSP

/** Apps<-RIVA: WcssAppsSmdMedIrq: 142 + 32 = 174 */
#define SMD_WCNSS_IN          174
/** Apps interrupt to send to RIVA: HWIO_ADDR(APCS_IPC) bit17 */
#define SMD_WCNSS_OUT         DALIPCINT_GP_1
#define SMD_WCNSS_OUT_M       0 /* Not used with DAL */
#define SMD_WCNSS_IPCPROC     DALIPCINT_PROC_WCN

/** Apps<-RPM incoming : rpm_to_kpss_ipc_irq0: 168 + 32 = 200 */
#define SMD_RPM_IN            202
/** Apps interrupt to RPM: kpss_to_rpm_ipc0 */
#define SMD_RPM_OUT           DALIPCINT_GP_2
#define SMD_RPM_OUT_M         0 /* Not used with DAL */
#define SMD_RPM_IPCPROC       DALIPCINT_PROC_RPM

/*@}*/

/* -------------------------------------------------------------
   Shared memory access macros for DAL Apps (Sparrow)
   ------------------------------------------------------------- */
/* Memory accesses must be volatile in order to enforce multibyte accesses, 
 * where required, and avoid compiler optimizations down to smaller accesses.
 */
#define SMD_READ_SMEM_1( addr ) ( *( ( volatile uint8* )( addr ) ) )

#define SMD_READ_SMEM_2( addr ) ( *( ( volatile uint16* )( addr ) ) )

#define SMD_READ_SMEM_4( addr ) ( *( ( volatile uint32* )( addr ) ) )

#define SMD_WRITE_SMEM_1( addr, val ) \
  ( *( ( volatile uint8 *)( addr ) )  = ( uint8 )( val ) )

#define SMD_WRITE_SMEM_2( addr, val ) \
  ( *( ( volatile uint16 *)( addr ) ) = ( uint16 )( val ) )

#define SMD_WRITE_SMEM_4( addr, val ) \
  ( *( ( volatile uint32 *)( addr ) ) = ( uint32 )( val ) )

#define SMD_MEMSET_SMEM( addr, val, size )     memset( addr, val, size )

#define SMD_MEMCPY_FROM_SMEM(  dst, dst_size, src, copy_size ) \
  smd_mem_copy(  dst, dst_size, src, copy_size )

#define SMD_MEMCPY_TO_SMEM(  dst, dst_size, src, copy_size )   \
  smd_mem_copy(  dst, dst_size, src, copy_size )

#define SMD_MEMCPY_FROM_TO_SMEM(  dst, dst_size, src, copy_size ) \
  smd_mem_copy(  dst, dst_size, src, copy_size )

/* Ensure the appropriate data copy implementation is used for copies in and
 * out of the SMD channels.  When the buffer resides in SMEM, the native 
 * memcpy can be used.  For buffers in RPM MSG RAM, word accesses are strictly 
 * required, but some implementations of memcpy do byte accesses under certain 
 * conditions, even if src and dest are word-aligned.
 * 
 * ARM memcpy works on all edges.
 */
#define SMD_GET_COPY_TO_SMD_FUNC(channel_type) memcpy
#define SMD_GET_COPY_FROM_SMD_FUNC(channel_type, cached) memcpy

/*===========================================================================
                        TYPE DEFINITIONS
===========================================================================*/
/** OS abstraction interrupt types */
typedef DalIPCIntProcessorType smd_os_proc_type;
typedef DalIPCIntInterruptType smd_os_intr_type;

#ifdef __cplusplus
}
#endif

#endif  /* SMD_PROC_H */
