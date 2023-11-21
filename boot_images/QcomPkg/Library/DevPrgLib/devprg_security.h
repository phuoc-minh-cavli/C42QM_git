/**************************************************************************
 * FILE: devprg_security.h
 *
 * Abstraction layer for the security APIs.
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Proprietary
 *
 *************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/DevPrgLib/devprg_security.h#1 $
  $DateTime: 2018/08/01 23:51:05 $
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-07-14   sl    Move functions that change between targets to target file.
2016-11-22   wek   Create.

===========================================================================*/

#ifndef __DEVPRG_SECURITY_H__
#define __DEVPRG_SECURITY_H__

#include "comdef.h"

/* Compute the sha256 of a given buffer.
 *
 * param input   input data to compute the SHA256
 * param len     Length of input
 * param sha_out SHA256 output, the buffer MUST be at least 32 bytes.
 *
 */
void devprg_sha256_direct(uint8* input, uint32 len, uint8* sha_out);

#endif /* __DEVPRG_SECURITY_H__ */
