/** @file DevPrgNLib.c
  
  Stub functions for DevPrgNLib

  Copyright (c) 2018 Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


when       who       what, where, why
--------   ---       -----------------------------------------------------------
18/03/20   svl       Initial revision.


=============================================================================*/
#include "boot_sbl_if.h"
#include "boot_cache_mmu.h"
#include "boot_page_table_armv7.h"
#include "boot_fastcall_tz.h"
#include "deviceprogrammer_initialize.h"

/*!
 * Stub for boot_hand_control_to_deviceprogrammer_lite_main
 *
 * @param bl_shared_data
 *    The shared bootloader information.
 *
*****************************************************************************/
void
boot_hand_control_to_deviceprogrammer_lite_main (boot_pbl_shared_data_type *pbl_shared_data)
{

  boot_fastcall_tz_no_rsp (TZ_UNLOCK_CMD,
                           TZ_UNLOCK_CMD_PARAM_ID,
                           1,0,0,0);
  /* Done setting up the MMU, run device programmer.  */
  deviceprogrammer_entry(pbl_shared_data);
}
