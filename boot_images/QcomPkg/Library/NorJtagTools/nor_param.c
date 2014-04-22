/*=======================================================================
 * FILE:        nor_param.c
 *
 * SERVICES:    Flash Programming Support via Lauterbach FLASH_PROG ICD
 *
 * DESCRIPTION
 *   This file exist so that we can explicitly place our prog_param
 *   data in memory since ARM doesn't allow this within a file but rather by
 *   file basis.
 *
 * Copyright 2010, 2018, 2019 by Qualcomm Technologies, Inc. All Rights Reserved.
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/NorJtagTools/nor_param.c#2 $ $DateTime: 2019/04/23 17:39:55 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/10/10     sv      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "boot_comdef.h"
#include "nor_param.h"

/* Structure to communicate between cmm scripts and Tools code */
struct flash_prog_param prog_param SECTION(".bss.FLASH_PROG_PARAM");

