/***********************************************************************
 * ubi_flasher_protocol.c
 *
 * UBI Flasher protocol module.
 * Copyright (C) 2017, 2018 QUALCOMM Technologies, Inc.
 *
 * UBI flasher protocol functions.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2018-06-14   as    Added check in entry point to make sure boot is from nand.
2017-09-08   rp    Create

===========================================================================*/

#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFIUbiFlasher.h>

#include "ubi_nand.h"
#include "ubi_flasher.h"
#include "ubi_util.h"
#include "BootConfig.h"

#include <string.h>

struct ubi_flasher_info {
	EFI_HANDLE ubi_flasher_handle;
};
static struct ubi_flasher_info ubi_flasher_info_inst;



EFI_STATUS EFIAPI efi_ubi_flasher_open(
	IN  CONST CHAR8               *PartitionName,
	OUT UBI_FLASHER_HANDLE        *UbiFlasherHandle,
	OUT UINT32                    *NandFlashPageSizeInBytes,
	OUT UINT32                    *NandFlashBlockSizeInBytes);

EFI_STATUS EFIAPI efi_ubi_flasher_write(
	IN  UBI_FLASHER_HANDLE        UbiFlasherHandle,
	IN  UINT32                    FrameNumber,
	IN  VOID                      *Buffer,
	IN  UINT32                    BufferSize);

EFI_STATUS EFIAPI efi_ubi_flasher_close(
	IN  UBI_FLASHER_HANDLE        UbiFlasherHandle);


static struct _EFI_UBI_FLASHER_PROTOCOL efi_ubi_flasher_protocol_inst = {
	EFI_UBI_FLASHER_PROTOCOL_REVISION,
	efi_ubi_flasher_open,
	efi_ubi_flasher_write,
	efi_ubi_flasher_close
};


/* GUID generation for NAND partitions */

EFI_STATUS EFIAPI
UbiFlasherEntryPoint
(
	IN EFI_HANDLE ImageHandle,
	IN EFI_SYSTEM_TABLE   *SystemTable
)
{
	struct ubi_flasher_info *ubif;
	EFI_STATUS status;
	int result;

    if(FALSE == boot_from_nand())
        return EFI_UNSUPPORTED;

	ubif = &ubi_flasher_info_inst;
	ubi_util_memset(ubif, 0, sizeof(*ubif));

	result = ubi_nand_init();
	if (result != 0)
		return EFI_LOAD_ERROR;

	status = gBS->InstallMultipleProtocolInterfaces(
		&ubif->ubi_flasher_handle,
		&gEfiUbiFlasherProtocolGuid,
		&efi_ubi_flasher_protocol_inst,
		NULL);
	ASSERT_EFI_ERROR(status);

	return EFI_SUCCESS;
}
EFI_STATUS EFIAPI efi_ubi_flasher_open(
	IN  CONST CHAR8               *PartitionName,
	OUT UBI_FLASHER_HANDLE        *UbiFlasherHandle,
	OUT UINT32                    *NandFlashPageSizeInBytes,
	OUT UINT32                    *NandFlashBlockSizeInBytes)
{
	int result;

	if (PartitionName == NULL) {
		DEBUG((EFI_D_ERROR, "Partition name is null\n"));
		return EFI_INVALID_PARAMETER;
	}
	if (UbiFlasherHandle == NULL) {
		DEBUG((EFI_D_ERROR, "Handle is null\n"));
		return EFI_INVALID_PARAMETER;
	}
	if (NandFlashPageSizeInBytes == NULL) {
		DEBUG((EFI_D_ERROR, "PageSize param is null\n"));
		return EFI_INVALID_PARAMETER;
	}
	if (NandFlashBlockSizeInBytes == NULL) {
		DEBUG((EFI_D_ERROR, "BlockSize param is null\n"));
		return EFI_INVALID_PARAMETER;
	}

	*UbiFlasherHandle = NULL;

	DEBUG((EFI_D_ERROR, "UBI flashing: [%s]\n", PartitionName));

	result = ubi_flasher_open((const char *)PartitionName,
			(ubi_flasher_handle *)UbiFlasherHandle,
			(uint32 *)NandFlashPageSizeInBytes,
			(uint32 *)NandFlashBlockSizeInBytes);

	return result;
}

EFI_STATUS EFIAPI efi_ubi_flasher_write(
	IN  UBI_FLASHER_HANDLE        UbiFlasherHandle,
	IN  UINT32                    FrameNumber,
	IN  VOID                      *Buffer,
	IN  UINT32                    BufferSize)
{
	int result;

	if (UbiFlasherHandle == NULL) {
		DEBUG((EFI_D_ERROR, "Handle is null\n"));
		return EFI_INVALID_PARAMETER;
	}
	if (Buffer == NULL) {
		DEBUG((EFI_D_ERROR, "Buffer is null, %d\n", BufferSize));
		return EFI_INVALID_PARAMETER;
	}
	if (BufferSize < UBI_NAND_MIN_PAGE_SIZE) {
		DEBUG((EFI_D_ERROR, "Buffer is too small, %d\n", BufferSize));
		return EFI_INVALID_PARAMETER;
	}

	DEBUG((EFI_D_ERROR, "UBI flashing: [%d, %d]\n",
		FrameNumber, BufferSize));

	result = ubi_flasher_write((ubi_flasher_handle)UbiFlasherHandle,
		(uint32)FrameNumber, (uint8 *)Buffer, (uint32)BufferSize);

	return result;
}

EFI_STATUS EFIAPI efi_ubi_flasher_close(
	IN  UBI_FLASHER_HANDLE        UbiFlasherHandle)
{
	int result;

	if (UbiFlasherHandle == NULL) {
		DEBUG((EFI_D_ERROR, "Handle is null\n"));
		return EFI_INVALID_PARAMETER;
	}

	result = ubi_flasher_close((ubi_flasher_handle)UbiFlasherHandle);

	return result;
}

