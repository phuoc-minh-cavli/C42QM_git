/** @file NandDxe.h
   
  NAND Driver header file

  Copyright (c) 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved. 
  Qualcomm Technologies Proprietary and Confidential
   
  Portions Copyright (c) 2006-2008, 2014-2015 Intel Corporation 

  All rights reserved. This program and the accompanying materials 
  are licensed and made available under the terms and conditions of the BSD License 
  which accompanies this distribution. The full text of the license may be found at 
  http://opensource.org/licenses/bsd-license.php 

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, 
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

/*=============================================================================
                              EDIT HISTORY


when         who     what, where, why
----------   ---     ----------------------------------------------------------- 
2017/01/10   eo      Initial version.

=============================================================================*/

#ifndef _NANDDXE_H_
#define _NANDDXE_H_

#include <Uefi.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/EFICardInfo.h>
#include <Protocol/EFIHWIO.h>
#include <Protocol/EFIEraseBlock.h>
#include <Protocol/EFINandIo.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/QcomLib.h>
#include <Library/BaseMemoryLib.h>

#define NAND_DEV_SIGNATURE SIGNATURE_32 ('n', 'a', 'n', 'd')

/**
  Maximum number of blocks for read or write
**/
#define NAND_MAX_BLOCK_TRANSFER_SIZE   6400

typedef struct {
  UINT32                     Signature;
  EFI_HANDLE                 ClientHandle;
  EFI_HANDLE                 DeviceHandle;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_GUID                   PartitionTypeGuid;
  EFI_PARTITION_ENTRY       *PartitionEntry;
  EFI_BLOCK_IO_PROTOCOL      BlkIo;
  EFI_MEM_CARDINFO_PROTOCOL  CardInfo;  
  EFI_ERASE_BLOCK_PROTOCOL   EraseBlk;
  EFI_NAND_IO_PROTOCOL       NandIo;
} NAND_DEV;

/**
   Reset the Block Device.
   
   @param  This                 Indicates a pointer to the calling context.
   @param  ExtendedVerification Driver may perform diagnostics on reset.
   
   @retval EFI_SUCCESS          The device was reset.
   @retval EFI_DEVICE_ERROR     The device is not functioning properly
                                and could not be reset.

**/
EFI_STATUS EFIAPI NandReset (
   IN EFI_BLOCK_IO_PROTOCOL   *This,
   IN BOOLEAN                 ExtendedVerification
   );

/**
   Read BufferSize bytes from Lba into Buffer.
   
   @param  This       Indicates a pointer to the calling context.
   @param  MediaId    Id of the media, changes every time the media is replaced.
   @param  Lba        The starting Logical Block Address to read from
   @param  BufferSize Size of Buffer, must be a multiple of device block size.
   @param  Buffer     A pointer to the destination buffer for the data.
                      The caller is responsible for either having implicit or
                      explicit ownership of the buffer.
   
   @retval EFI_SUCCESS           The data was read correctly from the device.
   @retval EFI_DEVICE_ERROR      The device reported an error while
                                 performing the read.
   @retval EFI_NO_MEDIA          There is no media in the device.
   @retval EFI_MEDIA_CHANGED     The MediaId does not matched the
                                 current device.
   @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block
                                 size of the device.
   @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not
                                 valid,or the buffer is not on proper alignment.
EFI_STATUS
**/
EFI_STATUS EFIAPI NandReadBlocks (
   IN EFI_BLOCK_IO_PROTOCOL   *This,
   IN UINT32                  MediaId,
   IN EFI_LBA                 Lba,
   IN UINTN                   BufferSize,
   OUT VOID                   *Buffer
   );

/**
   Write BufferSize bytes from Lba into Buffer.
   
   @param  This       Indicates a pointer to the calling context.
   @param  MediaId    The media ID that the write request is for.
   @param  Lba        The starting logical block address to be 
                      written. The   caller is responsible for
                      writing to only legitimate locations.
   @param  BufferSize Size of Buffer, must be a multiple of device block size.
   @param  Buffer     A pointer to the source buffer for the data.
   
   @retval EFI_SUCCESS           The data was written correctly to the device.
   @retval EFI_WRITE_PROTECTED   The device can not be written to.
   @retval EFI_DEVICE_ERROR      The device reported an error 
                                 while performing the write.
   @retval EFI_NO_MEDIA          There is no media in the device.
   @retval EFI_MEDIA_CHNAGED     The MediaId does not matched 
                                 the current device.
   @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple 
                                 of the block size of the device.
   @retval EFI_INVALID_PARAMETER The write request contains LBAs that are not
                                 valid or the buffer is not on proper
                                 alignment.
**/ 
EFI_STATUS EFIAPI NandWriteBlocks (
   IN EFI_BLOCK_IO_PROTOCOL   *This,
   IN UINT32                  MediaId,
   IN EFI_LBA                 Lba,
   IN UINTN                   BufferSize,
   IN VOID                    *Buffer
   );

/**
   Flush the Block Device.
   
   @param  This              Indicates a pointer to the calling context.
   
   @retval EFI_SUCCESS       All outstanding data was written to the device
   @retval EFI_DEVICE_ERROR  The device reported an error while 
                             writting back the data
   @retval EFI_NO_MEDIA      There is no media in the device.

**/
EFI_STATUS EFIAPI NandFlushBlocks (
   IN EFI_BLOCK_IO_PROTOCOL  *This
   );

/**
  Function: NandGetCardInfo
 
   This function retrieves the card information. 
  
   @param  This              Indicates a pointer to the calling context.
   @param  card_info       Pointer to a MEM_CARD_INFO
                                  variable that is used to store
                                  the retrieved card information
    
   @retval EFI_SUCCESS            The function completed successfully. 
   @retval EFI_INVALID_PARAMETER  The pass-in parameter is invalid.
**/
EFI_STATUS EFIAPI NandGetCardInfo (
   EFI_MEM_CARDINFO_PROTOCOL   *This,
   MEM_CARD_INFO               *card_info
   );

/*
  Function: NandEraseBlocks
 
  This function erases the NAND sectors specified by LBA and
  Size. Both the LBA and Size must align to the 
  EraseLengthGranularity. If either is not aligned, 
  EFI_INVALID_PARAMETER is returned. If the parameters are 
  valid, this function sends the commands to the NAND part to 
  apply ERASE erase to the specified sectors. 

  @param This         A pointer to the calling context. 
  @param MediaId      Current media ID 
  @param LBA          The start sector for the erase operation 
  @param Token        Pointer to the token associated with the 
                      transaction
  @param Size         The number of bytes to apply erase 

  @return EFI_SUCCESS            The function returned successfully.
  @return EFI_INVALID_PARAMETER  A parameter is invalid, including invalid
                                 alignment of the start and count values.
  @return EFI_DEVICE_ERROR       The physical device reported an error.
**/
EFI_STATUS EFIAPI NandEraseBlocks (
   IN EFI_BLOCK_IO_PROTOCOL *This,
   IN UINT32 MediaId,
   IN EFI_LBA LBA,
   IN OUT EFI_ERASE_BLOCK_TOKEN *Token,
   IN UINTN Size
   );

#define NAND_DEV_FROM_BLOCKIO(a)  CR (a, NAND_DEV, BlkIo, NAND_DEV_SIGNATURE)

#define NAND_DEV_FROM_CARD_INFO(a) CR (a, NAND_DEV, CardInfo, NAND_DEV_SIGNATURE)

#define NAND_DEV_FROM_ERASE(a) CR (a, NAND_DEV, EraseBlk, NAND_DEV_SIGNATURE)

#define NAND_DEV_FROM_NANDIO(a) CR (a, NAND_DEV, NandIo, NAND_DEV_SIGNATURE)

#define EFI_NAND_USER_PARTITION_GUID \
    { 0x03ef84a9, 0x60ce, 0x4371, { 0x97, 0xcf, 0x04, 0x84, 0x5a, 0x86, 0x5b, 0x79 } }

extern GUID gEfiPartitionTypeGuid;
extern EFI_GUID gEfiNandUserPartitionGuid;

#endif /* _NANDDXE_H_ */

