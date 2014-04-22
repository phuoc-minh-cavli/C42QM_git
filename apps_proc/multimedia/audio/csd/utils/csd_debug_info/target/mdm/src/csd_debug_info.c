/*============================================================================

   FILE:       csd_init_info.c

        Copyright © 2014 QUALCOMM Technologies Incorporated.
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
06/06/14   rp       added support for multiple CSD init and Deinit logging 
02/08/12   rp       creation
============================================================================*/

#include "csd_debug_info.h"
#include "time_svc.h"

  
/*define the CSD debug buffer  */
uint16_t  csd_debug_buf_pos = 0;
uint32_t  csd_debug_buf[CSD_DEBUG_BUF_SIZE];
time_type csd_debug_ts = {0};

