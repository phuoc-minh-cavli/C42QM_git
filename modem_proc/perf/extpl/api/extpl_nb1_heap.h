/*!
  @file
  extpl_nb1_heap.h

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

$Header: //components/rel/perf.mpss/2.1.2.0/extpl/api/extpl_nb1_heap.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/08/20   xx      Initial Version
===========================================================================*/



#ifndef EXTPL_NB1_HEAP_H
#define EXTPL_NB1_HEAP_H


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


void* extpl_nb1_heap_alloc(
  size_t               size       /*!< Number of bytes to allocate */
);

void extpl_nb1_heap_free(
  void                *ptr        /*!< Memory to free */
);

void* nb1_scan_heap_alloc
(
  size_t               size       /*!< Number of bytes to allocate */
);

void nb1_scan_heap_free
(
  void                *ptr        /*!< Memory to free */
);

#ifdef __cplusplus
}
#endif

#endif /* EXTPL_NB1_HEAP_H */

