/*=============================================================================
 *
 * FILE:      flash_nand_dal_default.c
 *
 * DESCRIPTION: Functions specific to the NAND DAL default(full) mode driver
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/core.mpss/3.10/storage/flash/src/dal/flash_dal_nand_default.c#1 $ $DateTime: 2018/06/26 03:18:17 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 08/31/12     bb/sv      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_nand_entry.h"

flash_vtbl* nand_get_entry_vtbl(void)
{
  return nand_get_entry_vtbl_default();
}
