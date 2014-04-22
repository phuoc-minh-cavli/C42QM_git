/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/27/010  jk     Initial Version
===========================================================================*/

#include "csd_intf.h"

/* ac stubs */
int32_t csd_as_init(struct csd_vtable ** vtable)
{
  return CSD_EOK;
}

int32_t csd_as_dinit(void)
{
  return CSD_EOK;
}

int32_t csd_as_debug_init(struct csd_vtable **vtable)
{
  return CSD_EOK;
}

int32_t csd_as_debug_dinit( void )
{
  return CSD_EOK;
}
