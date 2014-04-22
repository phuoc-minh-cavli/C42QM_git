/***********************************************************************
 * ubi_util.h
 *
 * UBI util functions.
 * Copyright (C) 2017 QUALCOMM Technologies, Inc.
 *
 * UBI util functions.
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
#ifndef __UBI_UTIL_H__
#define __UBI_UTIL_H__

#include "comdef.h"

void* ubi_util_malloc(uint32 size);
void ubi_util_free(void *ptr, uint32 size);

uint32 ubi_util_le_to_be32(uint32 le_num);
uint64 ubi_util_le_to_be64(uint64 le_num);

int ubi_util_check_pattern(const uint8 *buf, uint8 pattern, uint32 size);
int ubi_util_calc_data_len(const uint8 *buf, uint32 buf_len, uint32 page_size);


void ubi_util_memset(void *dst, int val, uint32 dst_size);
void ubi_util_memscpy(uint8 *dst, uint32 dst_size,
	const uint8 *src, uint32 src_size);


#endif /* __UBI_UTIL_H__ */
