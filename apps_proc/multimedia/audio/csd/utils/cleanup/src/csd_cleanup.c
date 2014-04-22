/*============================================================================

   FILE:       csd_cleanup.c

        Copyright © 2012 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
 ===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

 $Header:
 $Author:

when       who      what, where, why
--------   ---      ---------------------------------------------------------- 
04/28/14    jk       Clean will now call vsm clean up and device cleanup as these track themselves 
07/03/13    vk       Changes to remove compiler warnings 
09/26/12    jk       fixed bug where device session where not getting cleared 
06/20/12    jk       creation
============================================================================*/

#include "csd_vsm.h"
#include "csd_dev_main.h"


uint32_t csd_cleanup_session( uint32_t ref_count )
{
  if ( 1 < ref_count )
  {
    /*cleanup voice session*/
    csd_vsm_cleanup();
    /* close dev session*/
    csd_dev_force_teardown_all_devices();
  }
  return CSD_EOK;
}




