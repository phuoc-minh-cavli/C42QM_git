/*============================================================================
  FILE:         uimdrv_gpio_logging_stubs.c

  OVERVIEW:     This is a stubs file for uimdrv_gpio_logging.c

  DEPENDENCIES: N/A

                Copyright (c) 2016 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.  Please
use ISO format for dates.

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/hw/gpio_tlmm/uimdrv_gpio_logging_stubs.c#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

 when       who        what, where, why
------      ----       -----------------------------------------------------------
06/06/16    sam        Intial revision
============================================================================*/
#include "uimdrv_gpio.h"
#include "uimglobals.h"

/**
* This is a stubs function for uim_save_gpio_info
 */
void uim_save_gpio_info(
  uim_instance_global_type *uim_ptr,
  boolean                  is_internal_command
)
{
  (void)uim_ptr;
  (void)is_internal_command;
  return;
} /* uim_save_gpio_info */
