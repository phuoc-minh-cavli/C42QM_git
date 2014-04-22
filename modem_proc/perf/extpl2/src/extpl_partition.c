/*==========================================================================
 * FILE:         extpl_partition.c
 *
 * SERVICES:     EXTERNAL PAGER  Partition select
 *
 * DESCRIPTION:  This file allow to select primary vs backup partition by OEM.
 *
=============================================================================*/

/*===========================================================================

  Copyright (c) 2020 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/perf.mpss/2.1.2.0/extpl2/src/extpl_partition.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/24/2020   rm     Initial Version

===========================================================================*/

#include "msg.h"

#include "extpl.h"

#define PAGING_PRIMARY_PARTITION_NAME "qdsp_paging"
#define PAGING_BACKUP_PARTITION_NAME  "qdsp_paging_bkp"
#define M1_PRIMARY_PARTITION_NAME "qdsp"
#define M1_BACKUP_PARTITION_NAME "qdsp_bkp"

const char* qdsp_paging_partition_select_update()
{
    return PAGING_PRIMARY_PARTITION_NAME;
}

const char* qdsp_m1_partition_select_update()
{
    return M1_PRIMARY_PARTITION_NAME;
}


