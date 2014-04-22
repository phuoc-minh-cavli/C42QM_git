/*!
  @file
  clade_heap.h

  @brief
  Contains the modem heap manager public interface.

*/

/*===========================================================================

  Copyright (c) 2019 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/perf.mpss/2.1.2.0/clade_heap/inc/clade_heap.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/19   xx      Initial Version
===========================================================================*/



#ifndef CLADE_HEAP_H
#define CLADE_HEAP_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <stddef.h>            /* standard definition file (for size_t) */
#include "memheap.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/* --------------------------------------------------------------------------
** Functions
** ----------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif


void* clade_heap_alloc(
  size_t               size       /*!< Number of bytes to allocate */
);

void clade_heap_free(
  void                *ptr        /*!< Memory to free */
);

 void* clade_heap_calloc(
   size_t				 elt_count,
   size_t				 elt_size
 
 );

 void *clade_heap_realloc
(
  void   *ptr,
  size_t size
);

#ifdef __cplusplus
}
#endif

#endif /* CLADE_HEAP_H */

