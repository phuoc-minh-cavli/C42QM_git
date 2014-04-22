
/*===========================================================================

               S M S M   Q U R T 

===========================================================================*/
/**
  @file smsm_qurt.c
*/
/* Abstracts the OS specific implementation of system function implemented by 
   DALSYS.
*/


/* NOTE: For the output PDF generated using Doxygen and Latex, all file and 
         group descriptions are maintained in the SMEM_mainpage file. To change
         any of the the file/group text for the PDF, edit the SMEM_mainpage
         file, or contact Tech Pubs.

         The above description for this file is part of the "smem" group
         description in the SMEM_mainpage file. 
*/

/*===========================================================================
Copyright (c) 2011-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smsm/src/smsm_qurt.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/11   rs      Initial revision of DALSYS critical sections used by 
                   SMSM.
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "stdlib.h"
#include "string.h" 
#include "stringl.h" 
#include "qurt.h"
#include "smsm_os.h"
#include "smsm_os_common.h"

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/*===========================================================================

                        LOCAL DATA DEFINITIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
  FUNCTION      smsm_cs_create
===========================================================================*/
/**
  Creates SMSM general-use critical section.

  @return
  Pointer to the critical section.
*/
/*=========================================================================*/
void *smsm_cs_create(void)
{
  qurt_mutex_t *mutex;

  mutex = smsm_malloc(sizeof(qurt_mutex_t));
  memset(mutex, 0, sizeof(qurt_mutex_t));

  qurt_mutex_init(mutex);
  
  return ((void *)mutex);
}

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
void smsm_cs_lock(void *cs)
{
  qurt_mutex_t *mutex = (qurt_mutex_t *)cs;

  qurt_mutex_lock(mutex);
}

/*===========================================================================
  FUNCTION      smsm_cs_unlock
===========================================================================*/
/**
  Unlocks this SMSM general-use critical section.
 
  @param[in] cs           Pointer to the critical section

  @return
  None.
*/
/*=========================================================================*/
void smsm_cs_unlock(void *cs)
{
  qurt_mutex_t *mutex = (qurt_mutex_t *)cs;

  qurt_mutex_unlock(mutex);
}

/*===========================================================================
  FUNCTION      smsm_cs_destroy
===========================================================================*/
/**
  Deletes this SMSM general-use critical section.
 
  @param[in] cs           Pointer to the critical section

  @return
  None.
*/
/*=========================================================================*/
void smsm_cs_destroy(void *cs)
{
  qurt_mutex_t *mutex = (qurt_mutex_t * )cs;
  
  qurt_mutex_destroy(mutex);
  
  smsm_free(mutex);
}

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
void *smsm_malloc(uint32 size)
{
  void *ptr;
  
  ptr = malloc(size);
  
  if (ptr == NULL)
  {
    ERR_FATAL("malloc failed! size=%d.", size, 0, 0);
  }
  
  return ptr;
}

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
int32 smsm_free(void *ptr)
{
  free(ptr);
  return 0;
}
