/*==========================================================================*/
/*!
  @file 
  ds_crit_sect.c

  @brief
  This file provides REX specific critical section implementation.

  Copyright (c) 2009 - 2018 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*==========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/interface/utils/src/ds_crit_sect_rex.c#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
10/08/18  mks  Created the file for the critical section api
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "ds_crit_sect.h"
#include "ds_system_heap.h"
#include "amssassert.h"
#include "data_msg.h"

/*===========================================================================

                          PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
#if !defined(TEST_FRAMEWORK) || !defined(FEATURE_QUBE)
#include "rex.h"
/*---------------------------------------------------------------------------
  REX specific critical section implementation
---------------------------------------------------------------------------*/
void ds_init_crit_section
(
  ds_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == crit_sect_ptr)
  {
    DS_ERR_FATAL("DS_INIT_CRIT_SECTION(): critical section pointer is NULL");
    return;
  }

  DS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(crit_sect_ptr->handle,
                                  sizeof (rex_crit_sect_type),
                                  rex_crit_sect_type*);

  if (NULL == crit_sect_ptr->handle )
  {
    return;
  }


  (void) memset (crit_sect_ptr->handle, 0, sizeof (rex_crit_sect_type));

  rex_init_crit_sect ((rex_crit_sect_type *) crit_sect_ptr->handle);


  
} /* DS_INIT_CRIT_SECTION() */


void ds_enter_crit_section
(
  ds_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == crit_sect_ptr || NULL == crit_sect_ptr->handle)
  {
    DS_ERR_FATAL("DS_ENTER_CRIT_SECTION(): critical section or handle is NULL");
    return;
  }

  rex_enter_crit_sect ((rex_crit_sect_type *) crit_sect_ptr->handle);

} /* DS_ENTER_CRIT_SECTION() */

void ds_leave_crit_section
(
  ds_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == crit_sect_ptr || NULL == crit_sect_ptr->handle)
  {
    DS_ERR_FATAL("DS_LEAVE_CRIT_SECTION(): critical section or handle is NULL");
    return;
  }

  rex_leave_crit_sect ((rex_crit_sect_type *) crit_sect_ptr->handle);

} /* DS_LEAVE_CRIT_SECTION() */

void ds_destroy_crit_section
(
  ds_crit_sect_type*  crit_sect_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == crit_sect_ptr || NULL == crit_sect_ptr->handle)
  {
    DS_ERR_FATAL("DS_DESTROY_CRIT_SECTION(): critical section or handle is NULL");
    return;
  }

  (void) rex_del_crit_sect ((rex_crit_sect_type *) crit_sect_ptr->handle);

  DS_SYSTEM_HEAP_MEM_FREE (crit_sect_ptr->handle);
  
} /* DS_DESTROY_CRIT_SECTION() */

#endif /* if !defined(TEST_FRAMEWORK) || !defined(FEATURE_QUBE) */

