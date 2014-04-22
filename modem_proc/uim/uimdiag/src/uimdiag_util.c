/*===========================================================================

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.3.1/uimdiag/src/uimdiag_util.c#1 $$ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who   what, where, why
--------   ---   -----------------------------------------------------------
01/29/16   ar    Initial Revision


=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#include "uimdiag.h"
#include "uim_msg.h"
#include "uimdiag_util.h"
#include "modem_mem.h"

/*=============================================================================

                       GLOBAL DECLARATIONS

=============================================================================*/

/*=============================================================================

                              FUNCTIONS

=============================================================================*/

/*===========================================================================

FUNCTION UIMDIAG_MALLOC

DESCRIPTION
  The uimdiag_malloc called TMC alloc with different heap_size based the size
  of the allocation required.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  a pointer is allocated.
===========================================================================*/
void * uimdiag_malloc(dword size)
{
  return modem_mem_calloc(1,(size),MODEM_MEM_CLIENT_UIM);
}/* uimdiag_malloc */


/*===========================================================================

FUNCTION UIMDIAG_FREE

DESCRIPTION
  The uimdiag_free frees the pointer from the regular tmc_heap or the tmc_heap_small
  based on the pointer address, but only if FEATURE_UIMDIAG_USE_SMALL_HEAP is defined
  and FEATURE_LOW_MEMORY_USAGE IS NOT

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  the pointer is freed.
===========================================================================*/
void uimdiag_free(void * ptr)
{
  if (ptr == NULL)
  {
    UIMDIAG_MSG_ERROR_0("Attempt to free null ptr, return");
    return;
  }

  modem_mem_free((void*)(ptr),MODEM_MEM_CLIENT_UIM);
}/* uimdiag_free */


/*===========================================================================
FUNCTION UIMDIAG_GET_INDEX

DESCRIPTION
  Gets an available space from the req_table. This row is then marked as
  used. When it is no longer necessary,   the row should be freed by
  calling uimdiag_free_index.

DEPENDENCIES
  None

RETURN VALUE
  index of the request table

SIDE EFFECTS
  None
===========================================================================*/
int uimdiag_get_index(void)
{
  int i;

  if (uimdiag_req_table_ptr != NULL)
  {
    for (i = 0; i< UIMDIAG_MAX_CLIENT_REQ_TBL_SIZE; i++)
    {
      if (uimdiag_req_table_ptr[i].use_flag == UIMDIAG_REQ_TABLE_INDEX_FREE)
      {
        /* found an empty row. assign this */
        uimdiag_req_table_ptr[i].use_flag = UIMDIAG_REQ_TABLE_INDEX_USED;
        return i;
      }
    }
  }
  return UIMDIAG_GET_FN_ERR;
}/* uimdiag_get_index */


/*===========================================================================
FUNCTION UIMDIAG_FREE_INDEX

DESCRIPTION:
  Frees up space on req_table that is no longer being used.

INPUTS:
  int index: index into the request table

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
void uimdiag_free_index(int index)
{
   if ((uimdiag_req_table_ptr != NULL) &&
       (index >= 0) &&
       (index < UIMDIAG_MAX_CLIENT_REQ_TBL_SIZE))
   {
     /* Initialize the row entry */
     (void)memset (&uimdiag_req_table_ptr[index],
                   0x00, sizeof(uimdiag_req_table_elem_type));
   }
   else
   {
     UIMDIAG_MSG_ERROR_0("uimdiag_free_index: uimdiag_req_table_ptr is NULL or index out of range");
   }
}/* uimdiag_free_index */
