/** @file DeviceProgrammerLiteLibNull.c
  
  Stub functions for DeviceProgrammerLiteLib

  Copyright (c) 2014, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/NullLibs/DevPrgLLibNull/DevPrgLLibNull.c#1 $
  $DateTime: 2018/08/01 23:51:05 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      -----------------------------------------------------------
12/02/14   wkimberl  Initial revision.


=============================================================================*/

#include "boot_sbl_if.h"

/*!
 * Stub for boot_hand_control_to_deviceprogrammer_lite_main
 *
 * @param bl_shared_data
 *    The shared bootloader information.
 *
*****************************************************************************/
void
boot_hand_control_to_deviceprogrammer_lite_main (bl_shared_data_type *bl_shared_data)
{
  (void)bl_shared_data;
}
