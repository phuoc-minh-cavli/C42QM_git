/***********************************************************************
 * ubi_crc.h
 *
 * UBI Flasher CRC module.
 * Copyright (C) 2017 QUALCOMM Technologies, Inc.
 *
 * Computes CRC for UBI data structures.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2017-09-08   rp    Create

===========================================================================*/
#ifndef __UBI_CRC_H__
#define __UBI_CRC_H__

#include "comdef.h"

uint32 ubi_crc32(uint32 crc, const uint8 *buf, uint32 size);

#endif /* __UBI_CRC_H__ */
