/***********************************************************************
 * ubi_util.c
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

#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/QcomBaseLib.h>

#include "ubi_util.h"

#include <stdlib.h>
#include <string.h>

struct ubi_util_info_type {
	uint32 total_mallocs;
	uint32 failed_mallocs;

	uint32 total_malloced_size;
	uint32 total_freed_size;
};
static struct ubi_util_info_type ubi_util_info;

void ubi_util_init(void)
{
	struct ubi_util_info_type *util_info;

	util_info = &ubi_util_info;
	ubi_util_memset(util_info, 0, sizeof(*util_info));
}

void* ubi_util_malloc(uint32 size)
{
	void *ptr;

	ASSERT(size >= 1);

	++ubi_util_info.total_mallocs;

	ptr = AllocatePool(size);
	if (ptr != NULL) {
		ubi_util_memset(ptr, 0, size);
		ubi_util_info.total_malloced_size += size;
	} else {
		++ubi_util_info.failed_mallocs;
	}

	return ptr;
}

void ubi_util_free(void *ptr, uint32 size)
{
	ASSERT(ptr != NULL);
	ASSERT(size >= 1);

  	ubi_util_info.total_freed_size += size;
	FreePool(ptr);
}

uint32 ubi_util_le_to_be32(uint32 le_num)
{
	uint32 be_num;


	be_num = 0;
	be_num |= (((le_num >> 0)  & 0xFF) << 24);
	be_num |= (((le_num >> 8)  & 0xFF) << 16);
	be_num |= (((le_num >> 16) & 0xFF) << 8);
	be_num |= ((le_num >> 24)  & 0xFF);

	return be_num;
}

uint64 ubi_util_le_to_be64(uint64 le_num)
{
	uint64 be_num;

	be_num = 0;
	be_num |= (((le_num >> 0)  & 0xFF) << 56);
	be_num |= (((le_num >> 8)  & 0xFF) << 48);
	be_num |= (((le_num >> 16) & 0xFF) << 40);
	be_num |= (((le_num >> 24) & 0xFF) << 32);
	be_num |= (((le_num >> 32) & 0xFF) << 24);
	be_num |= (((le_num >> 40) & 0xFF) << 16);
	be_num |= (((le_num >> 48) & 0xFF) << 8);
	be_num |= ((le_num >> 56)  & 0xFF);

	return be_num;
}


/**
 * check_pattern - check if buffer contains only a certain byte pattern.
 * @buf: buffer to check
 * @pattern: the pattern to check
 * @size: buffer size in bytes
 *
 * This function returns %1 if there are only @patt bytes in @buf, and %0 if
 * something else was also found.
 */
int ubi_util_check_pattern(const uint8 *buf, uint8 pattern, uint32 size)
{
	uint32 i;

	ASSERT(buf != NULL);
	ASSERT(size >= 1);

	for (i = 0; i < size; i++)
		if ((buf)[i] != pattern)
			return 0;
	return 1;
}

/**
 * calc_data_len - calculate how much real data is stored in the buffer
 * @page_size: min I/O of the device
 * @buf: a buffer with the contents of the physical eraseblock
 * @len: the buffer length
 *
 * This function calculates how much "real data" is stored in @buf and
 * returns the length (in number of pages). Continuous 0xFF bytes at the end
 * of the buffer are not considered as "real data".
 */
int ubi_util_calc_data_len(const uint8 *buf, uint32 buf_len, uint32 page_size)
{
	uint32 i;

	ASSERT(buf != NULL);
	ASSERT(buf_len >= 1);
	ASSERT(page_size >= 1);

	for (i = buf_len - 1; i >= 0; i--)
		if ((buf)[i] != 0xFF)
			break;

	/* The resulting length must be aligned to the minimum flash I/O size*/
	buf_len = i + 1;
	buf_len = (buf_len + page_size - 1) / page_size;
	return buf_len;
}


void ubi_util_memset(void *dst, int val, uint32 dst_size)
{
	ASSERT(dst != NULL);
	ASSERT(dst_size > 0);

	//(void) SetMem32(dst, dst_size, (uint32)val);
	(void) memset(dst, val, dst_size);
}

void ubi_util_memscpy(uint8 *dst, uint32 dst_size,
	const uint8 *src, uint32 src_size)
{
	uint32 copied_bytes = 0;

	ASSERT(dst != NULL);
	ASSERT(src != NULL);
	ASSERT(dst_size >= src_size);

	if ((((uint8*)src <= (uint8*)dst) &&
		((uint8*)dst < ((uint8*)src + src_size))) ||
		(((uint8*)dst <= (uint8*)src) &&
		((uint8*)src < ((uint8*)dst + src_size)))) {
		DEBUG((EFI_D_ERROR, "Memscpy on overlapping regions!\n"));
		ASSERT(0);
	}

	copied_bytes = (uint32) CopyMemS(dst, dst_size, src, src_size);
	if(copied_bytes != src_size) {
		DEBUG((EFI_D_ERROR, "CopyMemS failed!\n"));
		ASSERT(0);
	}
}

