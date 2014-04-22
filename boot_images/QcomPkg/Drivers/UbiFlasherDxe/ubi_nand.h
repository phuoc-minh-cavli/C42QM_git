/***********************************************************************
 * ubi_nand.h
 *
 * UBI Nand interface.
 * Copyright (C) 2017 QUALCOMM Technologies, Inc.
 *
 * UBI Nand interface wrapper over the Nand-I/O protocol.
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
#ifndef __UBI_NAND_H__
#define __UBI_NAND_H__

#include "comdef.h"
#include <Protocol/EFINandIo.h>

#ifndef UBI_NAND_MIN_PAGE_SIZE
#define UBI_NAND_MIN_PAGE_SIZE  (512)
#endif

#ifndef UBI_NAND_DEVICE_ID
#define UBI_NAND_DEVICE_ID  (0)
#endif


typedef void* ubi_nand_handle;

int ubi_nand_init(void);
int ubi_nand_open_partition(const char *partition_name,
	ubi_nand_handle *nand_hdl);
int ubi_nand_close_partition(ubi_nand_handle nand_hdl);
int ubi_nand_get_page_size(ubi_nand_handle nand_hdl, uint32 *page_size);
int ubi_nand_get_block_size(ubi_nand_handle nand_hdl, uint32 *block_size);
int ubi_nand_get_block_count(ubi_nand_handle nand_hdl, uint32 *block_count);

int ubi_nand_read_pages(ubi_nand_handle nand_hdl,
	uint32 start_page, uint32 num_pages, uint8 *buf, uint32 buf_size);

int ubi_nand_write_pages(ubi_nand_handle nand_hdl,
	uint32 start_page, uint32 num_pages, uint8 *buf, uint32 buf_size);

int ubi_nand_mark_block_bad(ubi_nand_handle nand_hdl, uint32 block_no);
int ubi_nand_is_block_bad(ubi_nand_handle nand_hdl, uint32 block_no);
int ubi_nand_erase_blocks(ubi_nand_handle nand_hdl,
	uint32 start_block, uint32 num_blocks);

#endif /* __UBI_NAND_H__ */
