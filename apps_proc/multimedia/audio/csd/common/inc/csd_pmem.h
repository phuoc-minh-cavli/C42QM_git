#ifndef _CSD_PMEM_H_
#define _CSD_PMEM_H_
/*! \file csd_pmem.h
    \brief pmem pool for CSD

    Copyright (C) 2010 - 2012 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Proprietary and Confidential.
*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/audio.tx/2.0/audio/csd/common/inc/csd_pmem.h#1 $
$DateTime: 2018/12/25 22:39:07 $
$Author: pwbldsvc $
$Change: 17929171 $
$Revision: #1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/10   zj      initial release

============================================================================*/
#include "mmdefs.h"
#include "csd_list.h"

#define CSD_PMEM_PAGE_SIZE			4096

int32_t csd_pmem_malloc(void** buf);
int32_t csd_pmem_free(void* buf);

#endif /* _CSD_PMEM_H_ */
