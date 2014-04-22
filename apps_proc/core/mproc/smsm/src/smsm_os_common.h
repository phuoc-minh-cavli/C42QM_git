/*---------------------------------------------------------------------------
   SMSM_OS_COMMON.H  - Common Header for SMSM OS Implementations
---------------------------------------------------------------------------*/
/*!
  @file
    smsm_os_common.h

  @brief
    This file contains the common prototypes for SMSM functions that must be
    implemented by all OSes.
*/

/*--------------------------------------------------------------------------
     Copyright  2011-2013 Qualcomm Technologies Incorporated. 
     All rights reserved.
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smsm/src/smsm_os_common.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/03/13   bt      Generalize all SMSM critical sections.
02/01/13   bt      Add generic SMSM locking functions.
04/05/12   bt      Move cs prototypes to smsm_os_common.h.
08/30/11   bt      Initial version.
===========================================================================*/
#ifndef SMSM_OS_COMMON_H
#define SMSM_OS_COMMON_H

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

/*--------------------------------------------------------------------------
  Type definitions
---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
  Function declarations
---------------------------------------------------------------------------*/
/*===========================================================================
  FUNCTION  smsm_malloc
===========================================================================*/
/**
  Dynamically allocate a region of memory from the heap.  The region should be
  freed using \c smsm_free when no longer used.

  @param[in]  size   The number of bytes to be allocated.

  @return    The pointer to the region of memory that was allocated.
             NULL if the allocation failed.
*/
/*=========================================================================*/
void *smsm_malloc( uint32 size );

/*===========================================================================
  FUNCTION  smsm_malloc
===========================================================================*/
/**
  Free a region of memory that was allocated by \c smsm_malloc.

  @param[in] pMem    A reference to the region of memory to be freed.

  @return    0 if succesful.
             Negative value if an error occurred.
*/
/*=========================================================================*/
int32 smsm_free( void *pMem );

/*===========================================================================
  FUNCTION      smsm_cs_gen_create
===========================================================================*/
/**
  Creates SMSM general-use critical section.

  @return
  Pointer to the critical section.
*/
/*=========================================================================*/
void *smsm_cs_create(void);

/*===========================================================================
  FUNCTION      smsm_cs_lock
===========================================================================*/
/**
  Locks this SMSM general-use critical section.

  This function can protect access to any code in SMSM.
 
  @param[in] cs           Pointer to the critical section

  @return
  None.
*/
/*=========================================================================*/
void smsm_cs_lock( void *cs );

/*===========================================================================
  FUNCTION      smsm_cs_gen_unlock
===========================================================================*/
/**
  Unlocks this SMSM general-use critical section.
 
  @param[in] cs           Pointer to the critical section

  @return
  None.
*/
/*=========================================================================*/
void smsm_cs_unlock( void *cs );

/*===========================================================================
  FUNCTION      smsm_cs_gen_destroy
===========================================================================*/
/**
  Deletes this SMSM general-use critical section.
 
  @param[in] cs           Pointer to the critical section

  @return
  None.
*/
/*=========================================================================*/
void smsm_cs_destroy( void *cs );

#endif /* SMSM_OS_COMMON_H */
