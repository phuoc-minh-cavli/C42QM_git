/*====================================================================
 *
 * FILE: flash_hal_common.c
 *
 * SERVICES: Functions used across all flash controller HALs
 *
 * DESCRIPTION: Functions used across all flash controller HALs
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2008-2009, 2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary

 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/FlashCommonLib/NandHalLib/flash_hal_reg_list.c#1 $ $DateTime: 2018/08/01 23:51:05 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/05/12     sv      Stub gen list API's
 * 03/12/09     eo      Clean up
 * 03/30/08     BB      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flashi_hal_common.h"
#include "flash_hal_reg_list.h"

/*====================================================================
 *
 *          S T A T I C   V A R I A B L E   D E F I N I T I O N S
 *
 ====================================================================*/

/*====================================================================
 *
 *                     F U N C T I O N S
 *
 ====================================================================*/

/* HAL uses the below APIs to build a list of register operations
 * corresponding to a flash operations. Driver makes use of this
 * when it uses Data mover for executing it.
 *
 * HAL uses the buffer passed by the driver to create the list.
 */

/*
 * Adds a register write command to the HAL list
 */
void flash_hal_gen_write_list(uintnt dest_addr, uint32 value, uint32 op,
  uint32 op_len)
{
}


/*
 * Adds a register read command to the HAL list
 */
void flash_hal_gen_read_list(uintnt dest_addr, uintnt src_addr, uint32 op,
  uint32 op_len)
{
}


