#ifndef SMEM_TARGET_H
#define SMEM_TARGET_H

/*===========================================================================

                    Shared Memory Target Specific Header File


 Copyright (c) 2014-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smem/src/smem_target.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/15   db      Moved target specific dependencies to smem_hwio.h 
                   (Included under smem/config/$MSM_ID/inc)
03/06/15   db      Added 9x55 support 
04/10/14   rv      Use HWIO_TCSR_TZ_WONCE_n macros for SMEM_TARG_INFO_ADDR.
03/31/14   bm      IMEM location varies across target. Use target exported 
                   macro for IMEM base
12/18/13   bt      Rename IMEM defines to more-generic target info.
12/13/13   bt      Remove faulty and incorrect featurization of comdef.h.
11/09/13   bt      Add SMEM_PAGE_ALIGN_MASK define.
11/05/13   bt      Support 64-bit architectures.
06/06/13   pa      Add SMEM_INVALID_ADDR.
05/14/13   bt      Add SMEM_INFO_IMEM_ADDR, per family.
04/24/13   pa      Added SMEM_THIS_HOST.
01/08/13   pa      Added spin_lock_internal_asm, spin_unlock_internal_asm and
                   spin_lock_get_asm.
10/29/12   bt      Remove unneeded and/or redundant err.h.
06/20/12   bt      Update SMEM Address failsafe to new 0x0FA00000.
06/14/12   bm      Update macros for RPM MSG ram.
03/19/12   bm      Define macros for RPM mgs ram phys/virt base addr and size
03/13/12   bt      Unfeaturize spin_lock_get declaration.
01/20/12   pa      Cleaned up header files for bootloader.
01/18/12   bm      Add SMEM support for RPM processor.
12/19/11   bt      Created target specific file for 8974, based on 8960.
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smem_hwio.h"

/*--------------------------------------------------------------------------
  Variable Declarations
---------------------------------------------------------------------------*/

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/

#if defined(IMAGE_RPM_PROC)
#define SMEM_RPM_PROC
#define SMEM_THIS_HOST SMEM_RPM
#elif defined(RIVA_BRINGUP_VIRTIO)
#define SMEM_RIVA_PROC
#define SMEM_THIS_HOST SMEM_WCN
#elif defined(IMAGE_MODEM_PROC)
#define SMEM_MODEM_PROC
#define SMEM_THIS_HOST SMEM_MODEM
#elif defined(IMAGE_APPS_PROC)
#define SMEM_APPS_PROC
#define SMEM_THIS_HOST SMEM_APPS
#elif defined(MPROC_LPASS_PROC)
#define SMEM_LPASS_PROC
#define SMEM_THIS_HOST SMEM_ADSP
#endif

/* -------------------------------------------------------------
   Shared memory access macros for Modem, Scorpion and QDSP6
   ------------------------------------------------------------- */
#define SMEM_MEMSET_SMEM( addr, val, size )     memset( addr, val, size )

/** Defines the spinlock identifier for each processor */
#if defined(SMEM_APPS_PROC)
#define SMEM_SPINLOCK_PID             ( 1 )
#elif defined(SMEM_MODEM_PROC)
#define SMEM_SPINLOCK_PID             ( 2 )
#elif defined(SMEM_LPASS_PROC)
#define SMEM_SPINLOCK_PID             ( 3 )
#elif defined(SMEM_RIVA_PROC)
#define SMEM_SPINLOCK_PID             ( 5 )
#elif defined(SMEM_RPM_PROC)
#define SMEM_SPINLOCK_PID             ( 6 )
#else
#error "SMEM_SPINLOCK_PID not defined"
#endif

/** Defines invalid address */
#define SMEM_INVALID_ADDR               ((void *)(-1))

/** Mask for smallest possible page mapping size. */
#define SMEM_PAGE_ALIGN_MASK            0x00000FFF

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
  FUNCTION  spin_lock_internal
===========================================================================*/
/**
  Acquires spinlock by writing a non-zero value to the hardware mutex.
 
  Mutex characteristics:
   - Each Mutex has a separate read/write Mutex-register
   - Each Mutex-register can hold an integer value
   - '0' value = Mutex is free/unlocked
   - Non-zero value = Mutex is locked
 
  Locking the Mutex:
   - If Mutex is unlocked and PID is written, then the mutex is locked
   - If Mutex is locked and PID is written, then mutex register is not updated and
     the mutex remains locked
 
  A more detailed description can be found in System FPB (80-VR139-1A Rev. A)
  documentation.

  @param[in] lock      Spinlock id.

  @return
  None

  @dependencies 
  None.

  @sideeffects
  Locks the hardware mutex.
*/
/*==========================================================================*/
void spin_lock_internal( uint32 lock );

/*===========================================================================
  FUNCTION  spin_unlock_internal
===========================================================================*/
/**
  Releases spinlock by writing zero to the hardware mutex.
 
  Mutex characteristics:
   - Each Mutex has a separate read/write Mutex-register
   - Each Mutex-register can hold an integer value
   - '0' value = Mutex is free/unlocked
   - Non-zero value = Mutex is locked
 
  Locking the Mutex:
   - If '0' is written, then mutex register is updated and the mutex is
     unlocked
 
  A more detailed description can be found in System FPB (80-VR139-1A Rev. A)
  documentation.

  @param[in] lock      Spinlock id.

  @return
  None

  @dependencies 
  None.

  @sideeffects
  Unlocks the hardware mutex.
*/
/*==========================================================================*/
void spin_unlock_internal( uint32 lock );

/*===========================================================================
  FUNCTION  spin_lock_get
===========================================================================*/
/**
  This function retrieves the HW mutex value of the specified lock.
 
  Mutex characteristics:
   - Each Mutex has a separate read/write Mutex-register
   - Each Mutex-register can hold an integer value
   - '0' value = Mutex is free/unlocked
   - Non-zero value = Mutex is locked
 
  A more detailed description can be found in System FPB (80-VR139-1A Rev. A)
  documentation.

  @param[in] lock      Spinlock id.

  @return
  None

  @dependencies 
  None.

  @sideeffects
  None.
*/
/*==========================================================================*/
uint32 spin_lock_get( uint32 lock );

/*===========================================================================
  FUNCTION  spin_lock_internal_asm
===========================================================================*/
/**
  Acquires spinlock by performing an exclusive memory operation on the lock 
  location in shared memory.
 
  @param[in] lock      Spinlock id.

  @return
  None

  @dependencies 
  None.
*/
/*==========================================================================*/
void spin_lock_internal_asm( uint32 lock );

/*===========================================================================
  FUNCTION  spin_unlock_internal_asm
===========================================================================*/
/**
  Releases spinlock by writing 0 to the lock location in shared memory.
  
  @param[in] lock      Spinlock id.

  @return
  None

  @dependencies 
  None.
*/
/*==========================================================================*/
void spin_unlock_internal_asm( uint32 lock );

/*===========================================================================
  FUNCTION  spin_lock_get_asm
===========================================================================*/
/**
  This function retrieves the value of the specified lock.

  @param[in] lock      Spinlock id.

  @return
  Returns the value at the specified lock location.

  @dependencies 
  None.
*/
/*==========================================================================*/
uint32 spin_lock_get_asm( uint32 lock );

#endif /* SMEM_TARGET_H */
