/***********************************************************************
 * ubi_nand.c
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

#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include "ubi_nand.h"
#include "ubi_util.h"

#include <string.h>

struct ubi_nand_info_type {
	EFI_NAND_IO_PROTOCOL *nand_io_protocol;

	struct nand_info info;
};
static struct ubi_nand_info_type ubi_nand_info;

int ubi_nand_init(void)
{
	struct ubi_nand_info_type *nand_info;
	EFI_STATUS status;

	nand_info = &ubi_nand_info;
	ubi_util_memset(nand_info, 0, sizeof(*nand_info));

	/* Locate the NAND partition GUID protocol. */
	status = gBS->LocateProtocol(&gEfiNandIoProtocolGuid,
			NULL, (VOID **) &ubi_nand_info.nand_io_protocol);
	ASSERT_EFI_ERROR(status);
	if (status != EFI_SUCCESS)
		return -1;

	ASSERT(ubi_nand_info.nand_io_protocol != NULL);
	ASSERT(ubi_nand_info.nand_io_protocol->Revision >=
		EFI_NAND_IO_PROTOCOL_REVISION);

	return 0;
}

int ubi_nand_open_partition(const char *partition_name,
	ubi_nand_handle *nand_hdl)
{
	EFI_NAND_IO_PROTOCOL *proto;
	NAND_STATUS status;

	proto = ubi_nand_info.nand_io_protocol;

	*nand_hdl = NULL;
	status = proto->Open(UBI_NAND_DEVICE_ID,
			(const uint8 *)partition_name, nand_hdl);
	if (status != NAND_DEVICE_DONE)
		return status;

	ASSERT(*nand_hdl != NULL);
	return 0;
}

int ubi_nand_close_partition(ubi_nand_handle nand_hdl)
{
	EFI_NAND_IO_PROTOCOL *proto;
	NAND_STATUS status;

	ASSERT(nand_hdl != NULL);

	proto = ubi_nand_info.nand_io_protocol;

	status = proto->Close(nand_hdl);
	if (status != NAND_DEVICE_DONE)
		return status;

	return 0;
}


int ubi_nand_get_page_size(ubi_nand_handle nand_hdl, uint32 *page_size)
{
	EFI_NAND_IO_PROTOCOL *proto;
	struct nand_info *info;
	NAND_STATUS status;

	ASSERT(nand_hdl != NULL);
	ASSERT(page_size != NULL);
	*page_size = 0;

	proto = ubi_nand_info.nand_io_protocol;
	info = &ubi_nand_info.info;

	ubi_util_memset(info, 0, sizeof(*info));
	status = proto->GetInfo(nand_hdl, info);
	if (status != NAND_DEVICE_DONE)
		return status;

	*page_size = (uint32 )info->page_size_bytes;

	return 0;
}

int ubi_nand_get_block_size(ubi_nand_handle nand_hdl, uint32 *block_size)
{
	EFI_NAND_IO_PROTOCOL *proto;
	struct nand_info *info;
	NAND_STATUS status;

	ASSERT(nand_hdl != NULL);
	ASSERT(block_size != NULL);
	*block_size = 0;

	proto = ubi_nand_info.nand_io_protocol;
	info = &ubi_nand_info.info;

	ubi_util_memset(info, 0, sizeof(*info));
	status = proto->GetInfo(nand_hdl, info);
	if (status != NAND_DEVICE_DONE)
		return status;

	*block_size = (uint32 )info->pages_per_block;

	return 0;
}

int ubi_nand_get_block_count(ubi_nand_handle nand_hdl, uint32 *block_count)
{
	EFI_NAND_IO_PROTOCOL *proto;
	struct nand_info *info;
	NAND_STATUS status;

	ASSERT(nand_hdl != NULL);
	ASSERT(block_count != NULL);
	*block_count = 0;

	proto = ubi_nand_info.nand_io_protocol;
	info = &ubi_nand_info.info;

	ubi_util_memset(info, 0, sizeof(*info));
	status = proto->GetInfo(nand_hdl, info);
	if (status != NAND_DEVICE_DONE)
		return status;

	*block_count = info->block_count;

	return 0;
}

int ubi_nand_read_pages(ubi_nand_handle nand_hdl,
	uint32 start_page, uint32 num_pages, uint8 *buf, uint32 buf_size)
{
	EFI_NAND_IO_PROTOCOL *proto;
	NAND_STATUS status;

	ASSERT(nand_hdl != NULL);
	ASSERT(buf != NULL);
	ASSERT(num_pages >= 1);

	ubi_util_memset(buf, 0, buf_size);

	proto = ubi_nand_info.nand_io_protocol;

	status = proto->ReadPages(nand_hdl, NAND_ACCESS_MAIN,
		start_page, num_pages, buf, buf_size);

	if (status != NAND_DEVICE_DONE)
		return status;

	return 0;

}

int ubi_nand_write_pages(ubi_nand_handle nand_hdl,
	uint32 start_page, uint32 num_pages, uint8 *buf, uint32 buf_size)
{
	EFI_NAND_IO_PROTOCOL *proto;
	NAND_STATUS status;

	ASSERT(nand_hdl != NULL);
	ASSERT(buf != NULL);
	ASSERT(num_pages >= 1);

	proto = ubi_nand_info.nand_io_protocol;

	status = proto->WritePages(nand_hdl, NAND_ACCESS_MAIN,
		start_page, num_pages, buf, buf_size);

	if (status != NAND_DEVICE_DONE)
		return status;

	return 0;
}

int ubi_nand_mark_block_bad(ubi_nand_handle nand_hdl, uint32 block_no)
{
	EFI_NAND_IO_PROTOCOL *proto;
	NAND_STATUS status;

	ASSERT(nand_hdl != NULL);

	proto = ubi_nand_info.nand_io_protocol;

	status = proto->BlockSetState(nand_hdl, block_no, NAND_BLOCK_BAD);
	if (status != NAND_DEVICE_DONE)
		return status;

	return 0;
}

int ubi_nand_is_block_bad(ubi_nand_handle nand_hdl, uint32 block_no)
{
	EFI_NAND_IO_PROTOCOL *proto;
	enum nand_block_state state;
	NAND_STATUS status;

	ASSERT(nand_hdl != NULL);

	proto = ubi_nand_info.nand_io_protocol;

	status = proto->BlockGetState(nand_hdl, block_no, &state);
	if (status != NAND_DEVICE_DONE)
		return status;

	if (state == NAND_BLOCK_OK)
		return 0;
	else
		return 1;
}

int ubi_nand_erase_blocks(ubi_nand_handle nand_hdl,
	uint32 start_block, uint32 num_blocks)
{
	EFI_NAND_IO_PROTOCOL *proto;
	NAND_STATUS status;

	ASSERT(nand_hdl != NULL);
	ASSERT(num_blocks >= 1);

	proto = ubi_nand_info.nand_io_protocol;

	status = proto->EraseBlocks(nand_hdl, start_block, num_blocks);
	if (status != NAND_DEVICE_DONE)
		return status;

	return 0;
}

