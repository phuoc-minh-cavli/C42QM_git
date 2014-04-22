/** @file DeviceProgrammerSblNLib.c

  Stub functions for DeviceProgrammer SBLN.

  Copyright (c) 2019 Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


when       who       what, where, why
--------   ---       -----------------------------------------------------------
19-06-19   as        Initial revision.

=============================================================================*/

#include "boot_sbl_if.h"
#include "boot_cache_mmu.h"
#include "boot_page_table_armv7.h"
#include "boot_fastcall_tz.h"
#include "deviceprogrammer_initialize.h"

/*!
 * Stub for boot_hand_control_to_deviceprogrammer_ddr_main()
 *
 * @param bl_shared_data
 *    The shared bootloader information.
 *
*****************************************************************************/
void
boot_hand_control_to_deviceprogrammer_ddr_main (bl_shared_data_type *bl_shared_data)
{

  boot_fastcall_tz_no_rsp (TZ_UNLOCK_CMD,
                           TZ_UNLOCK_CMD_PARAM_ID,
                           1,0,0,0);

  deviceprogrammer_entry(bl_shared_data->sbl_shared_data->pbl_shared_data);
}
