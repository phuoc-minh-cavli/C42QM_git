/** @file NandDxe.c

  FLASH NAND UEFI Driver code

  Copyright (c) 2017, 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential

  Copyright (c) 2008-2009, Apple Inc. All rights reserved.

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   -----   ----------------------------------------------------------- 
2018/06/14   svl     Add a check to know boot device
2017/03/03   eo      Initial version.

=============================================================================*/

#include "NandDxe.h"
#include "NandMibib.h"

#include "DALSys.h"

#include <NandFtl.h>
#include <NandFtlApi.h>
#include <Guid/EventGroup.h>
#include <Protocol/EFINandPartiGuid.h>
#include <Protocol/EFIChipInfo.h>

#include <Library/UefiCfgLib.h>
#include <Library/UefiRuntimeLib.h>
#include <BootConfig.h>

#define NAND_TPL TPL_CALLBACK

/* Boot services exit data types */
EFI_EVENT EfiExitBootServicesEvent = (EFI_EVENT)NULL;

flash_ftl_info_t      gCardInfo;
NandMibibPartiTable   gMibibPartiTable;

/* NAND Device Path */
typedef struct {
   VENDOR_DEVICE_PATH  Nand;
   EFI_DEVICE_PATH     End;
} NAND_DEVICE_PATH;

static NAND_DEVICE_PATH gNandDevicePath;

static VENDOR_DEVICE_PATH NandPath =
{
   {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
         (UINT8)(sizeof(VENDOR_DEVICE_PATH)),
         (UINT8)((sizeof(VENDOR_DEVICE_PATH)) >> 8)
      }
   },
   {0}
};

static EFI_DEVICE_PATH NandEndPath =
{
   END_DEVICE_PATH_TYPE,
   END_ENTIRE_DEVICE_PATH_SUBTYPE,
   {
      sizeof(EFI_DEVICE_PATH_PROTOCOL),
      0
   }
};

static NAND_DEV FlashDeviceFtl = {
   NAND_DEV_SIGNATURE,                       /* Signature */
   NULL,                                     /* Client Handle */
   NULL,                                     /* Device Handle */
   NULL,                                     /* Device Path */
   {0},                                      /* Partition Type Guid */
   NULL,                                     /* Partition Entry */
   {0},                                      /* BlockIo */
   {0},                                      /* CardInfo */
   {0},                                      /* Erase Block Protocol (Initialized to NULL) */
   {0}                                       /* Nand IO Protocol (Initialized to NULL) */
};

static NAND_DEV gNandIoDevice = {
   NAND_DEV_SIGNATURE,                       /* Signature */
   NULL,                                     /* Client Handle */
   NULL,                                     /* Device Handle */
   NULL,                                     /* Device Path */
   {0},                                      /* Partition Type Guid */
   NULL,                                     /* Partition Entry */
   {0},                                      /* BlockIo */
   {0},                                      /* CardInfo */
   {0},                                      /* Erase Block Protocol (Initialized to NULL) */
   {0}                                       /* Nand IO Protocol (Initialized to NULL) */
};

/* FLASH FTL Block IO's Media */
static EFI_BLOCK_IO_MEDIA gFtlMedia = {
   SIGNATURE_32('n','a','n','d'),            /* MediaId */
   FALSE,                                    /* RemovableMedia */
   TRUE,                                     /* MediaPresent */
   FALSE,                                    /* LogicalPartition */
   FALSE,                                    /* ReadOnly */
   FALSE,                                    /* WriteCaching */
   2048,                                     /* BlockSize */
   4,                                        /* IoAlign */
   0,                                        /* LastBlock */
   0,                                        /* LowestAlignedLba */
   0,                                        /* LogicalBlocksPerPhysicalBlock */
   0                                         /* OptimalTransferLengthGranularity */
};

static EFI_BLOCK_IO_MEDIA gMediaFtl[FLASH_NUM_PART_ENTRIES];

/* NAND FTL Card Info Protocol */
static EFI_MEM_CARDINFO_PROTOCOL gFtlCardInfoProtocol = {
   EFI_MEM_CARD_INFO_PROTOCOL_REVISION,       /* Revision */
   NandGetCardInfo                            /* GetCardInfo */
};

/* NAND FTL Erase Protocol */
static EFI_ERASE_BLOCK_PROTOCOL gFtlEraseProtocol = {
   EFI_ERASE_BLOCK_PROTOCOL_REVISION,         /* Revision */
   (128 * 1024),                              /* Erase Granularity */
   NandEraseBlocks                            /* FTLEraseBlocks */
};

/* NAND FTL Block IO Protocol */
EFI_BLOCK_IO_PROTOCOL gBlockIoFtl = {
   EFI_BLOCK_IO_PROTOCOL_REVISION3,           /* Revision */
   0,                                         /* Media */
   NandReset,                                 /* Reset */
   NandReadBlocks,                            /* ReadBlocks */
   NandWriteBlocks,                           /* WriteBlocks */
   NandFlushBlocks                            /* FlushBlocks */
};

/* Device Paths for all the NAND physical partitions */
static EFI_GUID NandDevicePathGuid = EFI_NAND_USER_PARTITION_GUID;

/* GUID generation for NAND partitions */
static EFI_NAND_PARTI_GUID_PROTOCOL *NandPartiGuidProtocol = NULL;

/* Get Chip info GUID protocol */
static EFI_CHIPINFO_PROTOCOL *ChipInfoProtocol = NULL;

/* Nand block set state wrapper prototype */
NAND_STATUS NandIoBlockSetStateWrapper(
   nand_handle_t handle,
   uint32        block,
   enum nand_block_state block_state
   );

/**  NAND IO UEFI Protocol implementation */
EFI_NAND_IO_PROTOCOL NandIoProtocolImplementation = {
   EFI_NAND_IO_PROTOCOL_REVISION,
   nand_open,
   nand_get_info,
   nand_erase,
   NandIoBlockSetStateWrapper,
   nand_block_get_state,
   nand_write,
   nand_read,
   nand_close
};

/**
   Set the Block state wrapper.

   @param  handle                 Handle to the nand driver's partition.
   @param  block                   The specified block to set its state value.
   @param  block_state          The Good or Bad state value

   @retval EFI_SUCCESS          The bad block set value operation is successful.
   @retval EFI_DEVICE_ERROR  Error in setting the blocks state value

**/
NAND_STATUS NandIoBlockSetStateWrapper(
   nand_handle_t handle,
   uint32        block,
   enum nand_block_state block_state
   )
{
   FLASH_FTL_STATUS	rc = FLASH_FTL_OK;

   /* update FTL's internal bad block table (bbt), if the partition is already
       opened in FTL. */
   rc = flash_ftl_api_set_block(handle, block, block_state);
   if ( FLASH_FTL_OK == rc)
   {
      /* Either FTL does not have same partition opened or other error ocurred. 
           mark the block directly using nand driver's API. */
      return nand_block_set_state(handle, block, block_state);
   }
   return NAND_DEVICE_FAIL;
}

/**
   Decode the error code returned from the NAND APIs

   @param  ecode     Incoming error code
**/
void 
EFIAPI NandDecodeErrorCode (
   IN FLASH_FTL_STATUS                   ecode
   )
{
   switch (ecode)
   {
      case FLASH_FTL_OK:
         /* Not really any error - No message required */
         break;
      case FLASH_FTL_FAIL:
         DEBUG ((EFI_D_ERROR, "FTL returns error: FLASH_FTL_ERROR\n"));
         break;
      case FLASH_FTL_NOT_INIT:
         DEBUG ((EFI_D_ERROR, "FTL returns error: FLASH_FTL_NOT_INIT\n"));
         break;
      case FLASH_FTL_INVALID_PARAM:
         DEBUG ((EFI_D_ERROR, "FTL returns error: FLASH_FTL_INVALID_PARAM\n"));
         break;
      case FLASH_FTL_OUT_OF_MEM:
         DEBUG ((EFI_D_ERROR, "FTL returns error: FLASH_FTL_OUT_OF_MEM\n"));
         break;
      case FLASH_FTL_OUT_OF_GOOD_BLOCKS:
         DEBUG ((EFI_D_ERROR, "FTL returns error: FLASH_FTL_OUT_OF_GOOD_BLOCKS\n"));
         break;
      default:
         DEBUG ((EFI_D_ERROR, "FTL returns error "
                              "that can not be decoded.\n"));
         break;
   }
}
 
/**
   Reset the Block Device.

   @param  This                 Indicates a pointer to the calling context.
   @param  ExtendedVerification Driver may perform diagnostics on reset.

   @retval EFI_SUCCESS          The device was reset.
   @retval EFI_DEVICE_ERROR     The device is not functioning properly
                                and could not be reset.

**/
EFI_STATUS
EFIAPI
NandReset (
   IN EFI_BLOCK_IO_PROTOCOL   *This,
   IN BOOLEAN                 ExtendedVerification
   )
{
   return EFI_SUCCESS;
}


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
EFI_STATUS
EFIAPI
NandReadBlocks (
  IN EFI_BLOCK_IO_PROTOCOL   *This,
  IN UINT32                  MediaId,
  IN EFI_LBA                 Lba,
  IN UINTN                   BufferSize,
  OUT VOID                   *Buffer
  )
{
   NAND_DEV	          *NandDevice;
   flash_ftl_client_t  ftl_handle = NULL;
   FLASH_FTL_STATUS    rc = FLASH_FTL_OK;
   UINT32              total_lba_count = 0, block_size = 0, lba_count = 0;
   UINT8              *temp_buffer;

   /* Validate passed-in pointers are not NULL */
   if ((NULL == Buffer) || (NULL == This) || (NULL == This->Media))
   {
      return EFI_INVALID_PARAMETER;
   }

   /* Validate if buffer is multiple of block size */
   if ((BufferSize%(This->Media->BlockSize)) != 0 )
   {
      return EFI_BAD_BUFFER_SIZE;
   }

   /* Validate block size and access range */
   if ((Lba > This->Media->LastBlock) || (This->Media->BlockSize == 0))
   {
      return EFI_INVALID_PARAMETER;
   }

   if (This->Media->MediaId != MediaId)
   {
      return EFI_MEDIA_CHANGED;
   }

   NandDevice = NAND_DEV_FROM_BLOCKIO (This);

   ftl_handle = NandDevice->DeviceHandle;

   block_size = NandDevice->BlkIo.Media->BlockSize;

   total_lba_count = (BufferSize/block_size);

   /* Check if LBA plus the total sectors trying to access would exceed the */
   /* total size of the partition */
   if ((Lba + (BufferSize/block_size)) > (NandDevice->BlkIo.Media->LastBlock + 1))
   {
      return EFI_INVALID_PARAMETER;
   }

   /* Break transfer into smaller transfers.
      BAM has a limit of SDCC_MAX_BLOCK_TRANSFER_SIZE transfer size */
   temp_buffer = (UINT8 *)Buffer;

#ifdef ENABLE_DEBUG_OUTPUT
   DEBUG ((EFI_D_INFO, "RD: 0x%x    %d\n",Lba, total_lba_count));
#endif 
   
   while (total_lba_count > 0)
   {
      lba_count = (total_lba_count > NAND_MAX_BLOCK_TRANSFER_SIZE) ?
         NAND_MAX_BLOCK_TRANSFER_SIZE : total_lba_count;

      rc = flash_ftl_read_lba(ftl_handle, Lba, lba_count, temp_buffer);

      Lba = Lba + lba_count;
      total_lba_count = total_lba_count - lba_count;
      temp_buffer = temp_buffer + (lba_count * block_size);

      if (NAND_DEVICE_DONE != rc)
      {
         /* There's an error */
         NandDecodeErrorCode (rc);

         if (NAND_DEVICE_INVALID_PARAMETER == rc)
         {
            return EFI_INVALID_PARAMETER;
         }
         else
         {
            return EFI_DEVICE_ERROR;
         }
      }
   }

   return EFI_SUCCESS;
}

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
EFI_STATUS
EFIAPI
NandWriteBlocks (
   IN EFI_BLOCK_IO_PROTOCOL   *This,
   IN UINT32                  MediaId,
   IN EFI_LBA                 Lba,
   IN UINTN                   BufferSize,
   IN VOID                    *Buffer
   )
{
   NAND_DEV	          *NandDevice;
   flash_ftl_client_t  ftl_handle = NULL;
   FLASH_FTL_STATUS    rc = FLASH_FTL_OK;
   UINT32              total_lba_count = 0, lba_count = 0, block_size = 0;
   UINT8              *temp_buffer;

   /* Validate passed-in pointers are not NULL */
   if ((NULL == Buffer) || (NULL == This) || (NULL == This->Media))
   {
      return EFI_INVALID_PARAMETER;
   }

   /* Validate if buffer is multiple of block size */
   if (((BufferSize%(This->Media->BlockSize)) != 0))
   {
      return EFI_BAD_BUFFER_SIZE;
   }

   /* Validate block size and access range */
   if ((Lba > This->Media->LastBlock) || (This->Media->BlockSize == 0))
   {
      return EFI_INVALID_PARAMETER;
   }

   /* Validate if LBA plus the total sectors trying to access would exceed the */
   /* total size of the partition */
   if ((Lba + (BufferSize/This->Media->BlockSize)) > (This->Media->LastBlock + 1))
   {
      return EFI_INVALID_PARAMETER;
   }

   if (This->Media->MediaId != MediaId)
   {
      return EFI_MEDIA_CHANGED;
   }

   NandDevice = NAND_DEV_FROM_BLOCKIO (This);

   ftl_handle = NandDevice->DeviceHandle;

   if (NULL == ftl_handle)
   {
      return EFI_INVALID_PARAMETER;
   }

   /* Break transfer into smaller transfers.
      BAM has a limit of SDCC_MAX_BLOCK_TRANSFER_SIZE transfer size */
   temp_buffer = (UINT8 *)Buffer;

   block_size = NandDevice->BlkIo.Media->BlockSize;

   total_lba_count = (BufferSize/block_size);

#ifdef ENABLE_DEBUG_OUTPUT
   DEBUG ((EFI_D_INFO, "WR: 0x%x    %d\n",Lba, total_lba_count));
#endif 

   while (total_lba_count > 0)
   {
     lba_count = (total_lba_count > NAND_MAX_BLOCK_TRANSFER_SIZE) ?
        NAND_MAX_BLOCK_TRANSFER_SIZE : total_lba_count;

     rc = flash_ftl_write_lba(ftl_handle, Lba, lba_count, temp_buffer);

     Lba = Lba + lba_count;
     total_lba_count = total_lba_count - lba_count;
     temp_buffer = temp_buffer + (lba_count * block_size);

     if (rc != FLASH_FTL_OK)
     {
        NandDecodeErrorCode (rc);
        if (FLASH_FTL_INVALID_PARAM == rc)
        {
           return EFI_INVALID_PARAMETER;
        }
        else
        {
           return EFI_DEVICE_ERROR;
        }
     }
   }

   return EFI_SUCCESS;
}


/**
   Flush the Block Device.

   @param  This              Indicates a pointer to the calling context.

   @retval EFI_SUCCESS       All outstanding data was written to the device
   @retval EFI_DEVICE_ERROR  The device reported an error while
                             writting back the data
   @retval EFI_NO_MEDIA      There is no media in the device.

**/
EFI_STATUS
EFIAPI
NandFlushBlocks (
   IN EFI_BLOCK_IO_PROTOCOL  *This
   )
{
   return EFI_SUCCESS;
}

/**
   Function: MMCHSGetCardInfo

   This function retrieves the card information.

   @param  card_info              Pointer to a MEM_CARD_INFO
                                  variable that is used to store
                                  the retrieved card information

   @retval EFI_SUCCESS            The function completed successfully.
   @retval EFI_INVALID_PARAMETER  The pass-in parameter is invalid.
**/
EFI_STATUS
EFIAPI
NandGetCardInfo (
   EFI_MEM_CARDINFO_PROTOCOL   *This,
   MEM_CARD_INFO               *card_info
   )
{
   NAND_DEV           *NandDevice;
   flash_ftl_client_t  ftl_handle = NULL;
   FLASH_FTL_STATUS    rc = FLASH_FTL_OK;
   EFIChipInfoSerialNumType chip_ser_num;
   EFI_STATUS          EfiStatus = EFI_UNSUPPORTED;
   
   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(NAND_TPL);

   if ((NULL == card_info) || (NULL == This))
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto NandGetCardInfoExit;
   }

   NandDevice = NAND_DEV_FROM_CARD_INFO (This);

   ftl_handle = NandDevice->DeviceHandle;

   if (NULL == ftl_handle)
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto NandGetCardInfoExit;
   }

   /* Get NAND Flash Info */
   rc = flash_ftl_get_info(ftl_handle, &gCardInfo);

   if (FLASH_FTL_OK != rc)
   {
      DEBUG ((EFI_D_ERROR, "Get Card Info FAILED. %x\n\n\n", rc));
      EfiStatus = EFI_DEVICE_ERROR;
      goto NandGetCardInfoExit;
   }

   card_info->mfr_id = gCardInfo.maker_id;
   card_info->oem_id = gCardInfo.device_id;
   card_info->reliable_write_count = 0;
   card_info->rpmb_size_in_byte = 0;

   /* Copy the information out */
   card_info->card_type[0] = 'N';
   card_info->card_type[1] = 'A';
   card_info->card_type[2] = 'N';
   card_info->card_type[3] = 'D';

   EfiStatus = ChipInfoProtocol->GetSerialNumber(ChipInfoProtocol, &chip_ser_num);
   if (EFI_SUCCESS != EfiStatus)
   {
      goto NandGetCardInfoExit;
   }
   card_info->reliable_write_count = 0;
   card_info->rpmb_size_in_byte = 0;

   card_info->serial_num_len = sizeof(EFIChipInfoSerialNumType);

   ZeroMem(&(card_info->mfr_date[0]), sizeof(card_info->mfr_date));
   ZeroMem(&(card_info->product_serial_num[0]), sizeof(card_info->product_serial_num));
   CopyMem(card_info->product_serial_num, &chip_ser_num, card_info->serial_num_len);  

   EfiStatus = EFI_SUCCESS;

NandGetCardInfoExit:
   gBS->RestoreTPL (PrevTPL);
   
   return EfiStatus;
}

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
EFI_STATUS 
EFIAPI 
NandEraseBlocks (
   IN EFI_BLOCK_IO_PROTOCOL *This,
   IN UINT32 MediaId,
   IN EFI_LBA LBA,
   IN OUT EFI_ERASE_BLOCK_TOKEN *Token,
   IN UINTN Size
   )
{
   NAND_DEV           *NandDevice;
   flash_ftl_client_t  ftl_handle = NULL;
   FLASH_FTL_STATUS    rc = FLASH_FTL_OK;
   EFI_STATUS          EfiStatus = EFI_UNSUPPORTED;
   UINT32              start_block = 0, block_count = 0;
   UINT32              total_size = 0, lbas_per_erase_block = 0;
   UINT32              LBA32 = LBA;

   EFI_TPL PrevTPL;
   PrevTPL = gBS->RaiseTPL(NAND_TPL);

   if (NULL == This)
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto NandEraseBlocksExit;
   }

   NandDevice = NAND_DEV_FROM_BLOCKIO (This);
   ftl_handle = NandDevice->DeviceHandle;

   if (NULL == ftl_handle)
   {
      EfiStatus = EFI_INVALID_PARAMETER;
      goto NandEraseBlocksExit;   
   }
   
   /* Validate if LBA plus the total sectors trying to access would exceed the */
   /* total size of the partition */
   if ((LBA + (Size/NandDevice->BlkIo.Media->BlockSize)) > (NandDevice->BlkIo.Media->LastBlock + 1))
   {
      return EFI_INVALID_PARAMETER;
   }
   
   lbas_per_erase_block = 
      gFtlEraseProtocol.EraseLengthGranularity / NandDevice->BlkIo.Media->BlockSize;
   start_block  = (UINT32)(LBA32 / lbas_per_erase_block);
   total_size   = (UINT32)((LBA32 % lbas_per_erase_block) * NandDevice->BlkIo.Media->BlockSize);
   total_size  += (UINT32)Size; 
   block_count  = (UINT32)(total_size / gFtlEraseProtocol.EraseLengthGranularity);
   block_count += (UINT32)((total_size % gFtlEraseProtocol.EraseLengthGranularity)?1:0);
   
   rc = flash_ftl_erase_block(ftl_handle, start_block, block_count);

   if (FLASH_FTL_INVALID_PARAM == rc)
   {
      DEBUG ((EFI_D_ERROR, "Nand FTL Erase:: Invalid Parameter. SectorStart = 0x%x"
             "Count = 0x%x, handle = 0x%x\n", start_block, block_count, ftl_handle));
      EfiStatus = EFI_INVALID_PARAMETER;
      goto NandEraseBlocksExit;
   }

   if (FLASH_FTL_OK != rc)
   {
      DEBUG ((EFI_D_ERROR, "Failed to Erase. SectorStart = 0x%x. "
             "Count = 0x%x. Error = 0x%x\n\n", start_block, block_count, rc));
      EfiStatus = EFI_DEVICE_ERROR;
      goto NandEraseBlocksExit;
   }

   EfiStatus = EFI_SUCCESS;

NandEraseBlocksExit:
   if (Token != NULL) 
   {
      Token->Event = NULL;
      Token->TransactionStatus = EfiStatus;
   }
   
   gBS->RestoreTPL (PrevTPL);

   return EfiStatus;
}

/*
  Function: NandExitBootService
 
  This function deinits the nand driver and its structures
  which also deinits BAM driver and qpic pipe initializations
  on exit when gBS->ExitBootServices() is called.

  @param Event         A pointer to this event. 
  @param Context      A pointer to event handler private data 
**/
VOID
EFIAPI
NandExitBootService (
  EFI_EVENT  Event,
  VOID       *Context
  )
{
   // Close nand partitions and deinit nand/bam drivers
   if (FLASH_FTL_OK != flash_ftl_api_deinit())
   {
      DEBUG ((EFI_D_ERROR, "NandExitBootService: nand/bam driver deinit. Failed. "));
   }
}

EFI_STATUS
EFIAPI
NandDxeEntryPoint (
   IN EFI_HANDLE         ImageHandle,
   IN EFI_SYSTEM_TABLE   *SystemTable
   )
{
   EFI_STATUS Status;
   EFI_GUID   PartiTypeGuid;
   UINT32     Idx = 0, StrNameIdx = 0, StrNameLen = 0;
   HARDDRIVE_DEVICE_PATH   HdDev;
   EFI_PARTITION_ENTRY    *NandPartiEntry = NULL;
   NAND_DEV               *NandPartiHandle = NULL;
   CHAR16                  NandMibibPartiName[PARTITON_NAME_SIZE_MAX];
   
   if(FALSE == boot_from_nand())
   {
      return EFI_UNSUPPORTED;
   }

   /* Locate the NAND partition GUID protocol. */
   Status = gBS->LocateProtocol(&gEfiNandPartiGuidProtocolGuid, NULL,
       (VOID **) &NandPartiGuidProtocol);

   ASSERT_EFI_ERROR (Status);

   /* Locate the Chip Info GUID protocol. */
   Status = gBS->LocateProtocol(&gEfiChipInfoProtocolGuid, NULL,
       (VOID **) &ChipInfoProtocol);

   ASSERT_EFI_ERROR (Status);

   /* Initialize the Nand IO API Interface */
   gNandIoDevice.NandIo = NandIoProtocolImplementation;

   Status = gBS->InstallMultipleProtocolInterfaces(
               &gNandIoDevice.ClientHandle,
               &gEfiNandIoProtocolGuid,
               &gNandIoDevice.NandIo,
               NULL
               );

   ASSERT_EFI_ERROR (Status);

   ZeroMem(&gCardInfo, sizeof (gCardInfo));
   
   if (EFI_SUCCESS != NandInitMibibPartiTable(&gMibibPartiTable))
   {
      DEBUG ((EFI_D_ERROR, "Init Mibib Parti Table:: returned failure\n"));
      Status = EFI_DEVICE_ERROR;
      goto NandDxeEntryExit;
   }
   /* Update Block Size and Erase Block Size based on NAND device detected */
   gFtlMedia.BlockSize         = gMibibPartiTable.LbaSize * 1024;
   gFtlEraseProtocol.EraseLengthGranularity = gMibibPartiTable.EraseBlockSize * 1024;

   /* External UEFI applications depend on this order */
   FlashDeviceFtl.Signature    = NAND_DEV_SIGNATURE;
   FlashDeviceFtl.DeviceHandle = gMibibPartiTable.PartiEntries[0].PartiHandle;
   FlashDeviceFtl.CardInfo     = gFtlCardInfoProtocol;

   FlashDeviceFtl.BlkIo        = gBlockIoFtl;
   FlashDeviceFtl.BlkIo.Media  = &gMediaFtl[0];
   CopyMem(FlashDeviceFtl.BlkIo.Media, &gFtlMedia, sizeof(EFI_BLOCK_IO_MEDIA));

   FlashDeviceFtl.BlkIo.Media->LastBlock = gMibibPartiTable.PartiEntries[0].BlockCount - 1;

   FlashDeviceFtl.EraseBlk    = gFtlEraseProtocol;

   gNandDevicePath.Nand       = NandPath;
   gNandDevicePath.Nand.Guid  = NandDevicePathGuid;
   gNandDevicePath.End        = NandEndPath;

   /* Install device nand blockIO API interface */
   Status = gBS->InstallMultipleProtocolInterfaces (
               &FlashDeviceFtl.ClientHandle,
               &gEfiDevicePathProtocolGuid, &gNandDevicePath,
               &gEfiBlockIoProtocolGuid, &FlashDeviceFtl.BlkIo,
               &gEfiMemCardInfoProtocolGuid, &FlashDeviceFtl.CardInfo,
               &gEfiEraseBlockProtocolGuid, &FlashDeviceFtl.EraseBlk,
               NULL
               );
   if (EFI_SUCCESS != Status)
   {
      DEBUG ((EFI_D_ERROR, "InstallMultipleProtocolInterfaces:: returned ERROR Status 0x%08x \n", Status));
      goto NandDxeEntryExit;
   }

   // Register for boot services exit event.
   Status = gBS->CreateEventEx (
               EVT_NOTIFY_SIGNAL, 
               TPL_CALLBACK,
               NandExitBootService, 
               NULL, 
               &gEfiEventExitBootServicesGuid,
               &EfiExitBootServicesEvent
               );
   
   if (EFI_ERROR (Status)) {
     DEBUG ((EFI_D_ERROR, "CreateEventEx: ExitBootServicesEvent creation failed 0x%08x \n", Status));
     EfiExitBootServicesEvent = NULL;
   }
   
   /* Install and open MIBIB logical partitions */
   for (Idx = 1; Idx <= gMibibPartiTable.PartiNumEntries; Idx++)
   {
      NandPartiHandle = NULL;
      NandPartiHandle = AllocateZeroPool (sizeof (NAND_DEV));
      if (NandPartiHandle == NULL) 
      {
         Status = EFI_OUT_OF_RESOURCES;
         goto NandDxeEntryExit;
      }
      ZeroMem (NandPartiHandle, sizeof (NAND_DEV));
      NandPartiHandle->Signature    = NAND_DEV_SIGNATURE;
      NandPartiHandle->DeviceHandle = gMibibPartiTable.PartiEntries[Idx].PartiHandle;
      NandPartiHandle->CardInfo     = gFtlCardInfoProtocol;
      NandPartiHandle->BlkIo        = gBlockIoFtl;

      NandPartiHandle->BlkIo.Media  = &gMediaFtl[Idx];
      CopyMem (NandPartiHandle->BlkIo.Media, &gFtlMedia, sizeof (EFI_BLOCK_IO_MEDIA));
      NandPartiHandle->BlkIo.Media->LogicalPartition = TRUE;
      if (gMibibPartiTable.PartiEntries[Idx].BlockCount)
      {
         NandPartiHandle->BlkIo.Media->LastBlock = 
            MultU64x32 ((gMibibPartiTable.PartiEntries[Idx].BlockCount - 1), 1);
      }
      else
      {
         NandPartiHandle->BlkIo.Media->LastBlock = 
            MultU64x32 ((gMibibPartiTable.PartiEntries[Idx].BlockCount), 1);
      }

      NandPartiHandle->BlkIo.Reset        = NandReset;
      NandPartiHandle->BlkIo.ReadBlocks   = NandReadBlocks;
      NandPartiHandle->BlkIo.WriteBlocks  = NandWriteBlocks;
      NandPartiHandle->BlkIo.FlushBlocks  = NandFlushBlocks;
      NandPartiHandle->EraseBlk           = gFtlEraseProtocol;

      NandPartiEntry = AllocateZeroPool (sizeof (EFI_PARTITION_ENTRY));
      if (NandPartiEntry == NULL) 
      {
         Status = EFI_OUT_OF_RESOURCES;
         goto NandDxeEntryExit;
      }
      ZeroMem (NandPartiEntry, sizeof (EFI_PARTITION_ENTRY));
	  NandPartiHandle->PartitionEntry = NandPartiEntry;
      NandPartiHandle->PartitionEntry->Attributes  = 0;
      NandPartiHandle->PartitionEntry->StartingLBA = 0;

      if (gMibibPartiTable.PartiEntries[Idx].BlockCount)
      {
         NandPartiHandle->PartitionEntry->EndingLBA = (gMibibPartiTable.PartiEntries[Idx].BlockCount - 1);
      }
      else
      {
         NandPartiHandle->PartitionEntry->EndingLBA   = gMibibPartiTable.PartiEntries[Idx].BlockCount;
      }

      StrNameLen = (AsciiStrLen((CONST CHAR8 *)gMibibPartiTable.PartiEntries[Idx].PartiName) + 1);
      StrNameLen = (StrNameLen < PARTITON_NAME_SIZE_MAX)? 
                      StrNameLen : PARTITON_NAME_SIZE_MAX;
      ZeroMem(&NandMibibPartiName, sizeof (NandMibibPartiName));
      for (StrNameIdx = 0; StrNameIdx < StrNameLen; StrNameIdx++)
      {
         NandPartiHandle->PartitionEntry->PartitionName[StrNameIdx] =
            (CHAR16)gMibibPartiTable.PartiEntries[Idx].PartiName[StrNameIdx];
         NandMibibPartiName[StrNameIdx] = 
            (CHAR16)gMibibPartiTable.PartiEntries[Idx].PartiName[StrNameIdx];
      }

      ZeroMem (&HdDev, sizeof (HdDev));
      HdDev.Header.Type         = MEDIA_DEVICE_PATH;
      HdDev.Header.SubType      = MEDIA_HARDDRIVE_DP;
      SetDevicePathNodeLength (&HdDev.Header, sizeof (HdDev));

      HdDev.PartitionNumber     = (UINT32) Idx + 1;
      HdDev.MBRType             = MBR_TYPE_EFI_PARTITION_TABLE_HEADER;
      HdDev.SignatureType       = SIGNATURE_TYPE_GUID;
      HdDev.PartitionStart      = 0;
      HdDev.PartitionSize       = gMibibPartiTable.PartiEntries[Idx].BlockCount;

      Status = NandPartiGuidProtocol->GenGuid(NandPartiGuidProtocol,
                  (CONST CHAR16 *)&NandMibibPartiName, 
                  (CONST UINT32)StrLen((CONST CHAR16 *)&NandMibibPartiName),
                  &PartiTypeGuid);
      if (EFI_SUCCESS != Status)
      {
         DEBUG ((EFI_D_ERROR, "NandGetPartitionGuid:: returned failure\n"));
         Status = EFI_DEVICE_ERROR;
         goto NandDxeEntryExit;
      }

      CopyGuid (&NandPartiHandle->PartitionEntry->PartitionTypeGUID, &PartiTypeGuid); 
      CopyGuid (&NandPartiHandle->PartitionEntry->UniquePartitionGUID, &PartiTypeGuid);

      CopyGuid (&NandPartiHandle->PartitionTypeGuid, &PartiTypeGuid);
      CopyMem (HdDev.Signature, &PartiTypeGuid, sizeof (EFI_GUID));
      NandPartiHandle->DevicePath  = AppendDevicePathNode(
                                        (CONST EFI_DEVICE_PATH_PROTOCOL *)&gNandDevicePath, 
                                        (CONST EFI_DEVICE_PATH_PROTOCOL *)&HdDev);

      /* Initialize the NAND BLOCK IO API Interface */
      Status = gBS->InstallMultipleProtocolInterfaces (
                  &NandPartiHandle->ClientHandle,
                  &gEfiDevicePathProtocolGuid, NandPartiHandle->DevicePath,
                  &gEfiPartitionTypeGuid, &NandPartiHandle->PartitionTypeGuid,
                  &gEfiPartitionRecordGuid, NandPartiHandle->PartitionEntry,
                  &gEfiBlockIoProtocolGuid, &NandPartiHandle->BlkIo,
                  &gEfiMemCardInfoProtocolGuid, &NandPartiHandle->CardInfo,
                  &gEfiEraseBlockProtocolGuid, &NandPartiHandle->EraseBlk,
                  NULL
                  );
      if (EFI_SUCCESS != Status)
      {
         DEBUG ((EFI_D_ERROR, "InstallMultipleProtocolInterfaces:: returned ERROR Status 0x%08x, Idx %d\n", Status, Idx));
         goto NandDxeEntryExit;
      }
   }
         
NandDxeEntryExit:
   ASSERT_EFI_ERROR (Status);

   return EFI_SUCCESS;
}
