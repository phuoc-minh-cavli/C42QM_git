#ifndef QDSS_THROW_H
#define QDSS_THROW_H

/*=============================================================================

FILE:         qdss_throw.h

DESCRIPTION:

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/common/src/qdss_throw.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
=============================================================================*/
#include "comdef.h"

typedef struct QDSSMemInfo QDSSMemInfo;
struct QDSSMemInfo
{
    uint32        VirtualAddr;
    uint32        PhysicalAddr;
    uint32        dwLen;
    uint32        dwMappedLen;
    uint32        dwProps;
};


#define TRY(nErr, func) \
   if (0 != (nErr = (func))) {\
      goto nErr##bail; \
   }

#define THROW(nErr, errno) \
   nErr = errno; \
   goto nErr##bail;

#define THROW_IF(nErr, condition, errno)        \
   if (condition) {                             \
      nErr = errno;                             \
      goto nErr##bail;                          \
   }

#define CATCH(nErr) nErr##bail: if (nErr != 0)

#define IGNORE(x)  ((void) x)

#endif //QDSS_THROW_H
