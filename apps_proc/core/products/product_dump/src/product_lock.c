
/*===========================================================================

	      C R I T I C A L  S E C T I O N S

===========================================================================*/
/**
  @file spinlock_cs.c
*/
/* Abstracts the OS specific implementation of Critical Section
*/


/*===========================================================================
Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/01/15   rss     First cut for 9x55.

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "product_dump.h"
#include "qurt_mutex.h"
/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/*===========================================================================

                        LOCAL DATA DEFINITIONS

===========================================================================*/

/** The critical section structure for the lock */
static qurt_mutex_t productlock_cs;

/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION  productlock_cs_init
===========================================================================*/
/**
  Creates the critical section used to acquire the lock
  from different tasks.
 
  @return
  None

  @dependencies 
  None.

  @sideeffects
  None.
*/
/*==========================================================================*/
void productlock_cs_init( void )
{
  /* Initialize the critical section */
 qurt_mutex_init(&productlock_cs);

}

/*===========================================================================
  FUNCTION  productlock_cs_lock
===========================================================================*/
/**
  Locks the critical section.
 
 
  @return
  None

  @dependencies 
  None.

  @sideeffects
  None.
*/
/*==========================================================================*/
void productlock_cs_lock( void ) 
{
  /* Lock the non-preemtable critical section */
  qurt_mutex_lock(&productlock_cs);
}

/*===========================================================================
  FUNCTION  productlock_cs_unlock
===========================================================================*/
/**
  Releases the critical section.
 
 
  @return
  None

  @dependencies 
  None.

  @sideeffects
*/
/*==========================================================================*/
void productlock_cs_unlock( void )
{
  /* Unlock the critical section */
  qurt_mutex_unlock(&productlock_cs );
}

