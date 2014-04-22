#ifndef QDSS_UTILS_H
#define QDSS_UTILS_H

/*=============================================================================

FILE:         qdss_utils.h

DESCRIPTION:

=============================================================================*/
/*=============================================================================
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include "DALFramework.h"

DALResult qdss_dal_memmap(uint32 physAddr,
                          uint32 size,
                          uint32 dwAttribs,
                          void *phMem,
                          QDSSMemInfo *pMemInfo);

#endif //QDSS_UTILS_H
