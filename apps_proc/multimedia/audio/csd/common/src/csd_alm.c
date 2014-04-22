
/*============================================================================

   FILE:       csd_alm.c

   DESCRIPTION: Contains Audio License Manager's dispatcher.

   PUBLIC CLASSES:  Not Applicable

   INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2011 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
 ===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

    This section contains comments describing changes made to this module.
    Notice that changes are listed in reverse chronological order.

    $Author: pwbldsvc $ 
    $Header: //components/rel/audio.tx/2.0/audio/csd/common/src/csd_alm.c#1 $
    $DateTime: 2018/12/25 22:39:07 $
    $Change: 17929171 $
    $Revision: #1 $

============================================================================*/

#include "csd_intf.h"

/*===========================================================================
FUNCTION csd_alm_init

DESCRIPTION

RETURN VALUE
   CSD return code

SIDE EFFECTS
===========================================================================*/
int32_t csd_alm_init(struct csd_vtable **vtable)
{
  return CSD_EOK;
}

/*===========================================================================
FUNCTION csd_alm_deinit

DESCRIPTION

RETURN VALUE
   CSD return code

SIDE EFFECTS
===========================================================================*/
int32_t csd_alm_deinit(void)
{
  return CSD_EOK;
}
