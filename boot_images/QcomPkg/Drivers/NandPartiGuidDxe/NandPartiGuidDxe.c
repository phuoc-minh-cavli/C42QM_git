/**
* @file NandPartiGuidDxe.c
* @brief   This file implements NAND Partition GUID EFI protocol 
*          interface.

  Copyright (c) 2017,2018 Qualcomm Technologies, Inc. All rights reserved.
*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/14/18   svl     Add a check to know boot device
03/03/17   eo      Created.
==============================================================================*/

/*==============================================================================

                       INCLUDE FILES FOR THIS MODULE

==============================================================================*/

#include <Uefi.h>

#include <Protocol/Hash.h>
#include <Protocol/EFINandPartiGuid.h>

#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/HashSw.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <BootConfig.h>

/*==============================================================================

                        DEFINES FOR THIS MODULE

==============================================================================*/
#define GUID_MAX_SIZE_IN_BYTES          (sizeof(EFI_GUID) - 1)

#define NAND_GEN_PARTITION_GUID_HASH     0x1
#define NAND_GEN_PARTITION_GUID_NAME     0x2

/*==============================================================================

                     GLOBAL VARIABLES FOR THIS MODULE

==============================================================================*/
EFI_HASH_PROTOCOL     *pEfiNandHashProtocol = NULL;
EFI_SHA1_HASH         Sha1HashBuffer;

/*
 * If hashlib/hashdxe not available, use dummy GUID generated from partition name itself. 
 */
static EFI_GUID gDummyGuid;

/*
 * Flag to determine method used to generate GUID. 
 */

static UINT8    gNandGenPartitionGuidFlag = NAND_GEN_PARTITION_GUID_HASH;

/*==============================================================================

                             API IMPLEMENTATION

==============================================================================*/

/*==============================================================================

 Generate NAND partition GUID hashed with NAND partition name. 
  
 @param[in]    This               Pointer to the EFI_NAND_PARTI_GUID_PROTOCOL instance.
 @param[in]    *PartiName         Buffer pointer to the NAND partition name
 @param[in]    PartiNameLen       Size of the string NAND partition name.
 @param[out]   *PartiGuid         Returned partition GUID value hashed from NAND partition name.

 @retval EFI_SUCCESS             The partition GUID was successfully generated.
 @retval EFI_DEVICE_ERROR        Partition GUID generation failed.

==============================================================================*/
EFI_STATUS
EFIAPI NandGenPartiGuid (
   IN EFI_NAND_PARTI_GUID_PROTOCOL   *This,
   IN CONST CHAR16                   *PartiName,
   IN CONST UINT32                   PartiNameLen,
   OUT EFI_GUID                      *PartiGuid 
   )
{
   EFI_HASH_OUTPUT	   Hash;
   EFI_GUID           *AlgoGuid;
   EFI_STATUS		   EfiStatus = EFI_SUCCESS;
   UINT32              HashLen = 0;
   UINT8              *pHash = NULL;
   
   /* Validate passed-in pointers are not NULL */
   if ((NULL == PartiName) || (NULL == This) || (0 == PartiNameLen))
   {
      return EFI_INVALID_PARAMETER;
   }

   if (gNandGenPartitionGuidFlag == NAND_GEN_PARTITION_GUID_HASH)
   {
      /* Locate protocol */
      EfiStatus = gBS->LocateProtocol(&gEfiHashProtocolGuid, NULL, (VOID **)&pEfiNandHashProtocol);
      if (EFI_SUCCESS != EfiStatus)
      {
         if (EFI_NOT_FOUND != EfiStatus)
         {
            DEBUG ((EFI_D_ERROR, "ERROR: LocateProtocol failed to find hash protocol: EfiStatus = 0x%x. \n\n", EfiStatus));
         }
      }

      if (EFI_SUCCESS == EfiStatus)
      {
         Hash.Sha1Hash = &Sha1HashBuffer;
         pHash    = (UINT8 *)&Sha1HashBuffer;
         HashLen  = sizeof(EFI_GUID);
         AlgoGuid = &gEfiHashAlgorithmSha1Guid;

         EfiStatus = pEfiNandHashProtocol->Hash(pEfiNandHashProtocol, AlgoGuid, 0, (CONST UINT8 *)PartiName, PartiNameLen, &Hash);
         if (EFI_SUCCESS != EfiStatus)
         {
            DEBUG ((EFI_D_ERROR, "ERROR: Hash failed. EfiStatus = 0x%x. \n\n", EfiStatus));
            EfiStatus = EFI_DEVICE_ERROR;
         }

         if (EFI_SUCCESS == EfiStatus )
         {
            CopyMem((UINT8 *)PartiGuid, Hash.Sha1Hash, HashLen);
         }
      }
   }
   //Hash lib not available: add temp dummy GUID for now   
   else
   {
      UINT8 Idx = 0;
      UINT8 TotalCharCount = 0;
      UINT8 *GuidPtr = NULL;

      ZeroMem(&gDummyGuid, sizeof(EFI_GUID));
      GuidPtr = (UINT8 *)&gDummyGuid;

      TotalCharCount = (PartiNameLen < sizeof(EFI_GUID))? PartiNameLen : sizeof(EFI_GUID);

      for (Idx = 0; Idx < TotalCharCount; Idx++)
      {
         if (Idx == GUID_MAX_SIZE_IN_BYTES)
         {
            break;
         }
         GuidPtr[GUID_MAX_SIZE_IN_BYTES-Idx] = (UINT8)*(PartiName+Idx);
      }
      DEBUG ((EFI_D_VERBOSE, "debug: PartiName %a, Partition GUID: %g \n", PartiName, &gDummyGuid)); 
      CopyMem((UINT8 *)PartiGuid, (UINT8 *)&gDummyGuid, sizeof(EFI_GUID));

      EfiStatus = EFI_SUCCESS;
   }

   return EfiStatus;
}

static EFI_NAND_PARTI_GUID_PROTOCOL NandPartiGuidProtocol =
{
   EFI_NAND_PARTI_GUID_PROTOCOL_REVISION,
   NandGenPartiGuid
};

/*==============================================================================

  FUNCTION      NandPartitionGuidEntryPoint

  DESCRIPTION   Entry point for Nand Partition Guid access.

==============================================================================*/ 

EFI_STATUS
EFIAPI
NandPartitionGuidEntryPoint (IN EFI_HANDLE ImageHandle, 
                    IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_HANDLE  handle = NULL;
  EFI_STATUS  status = EFI_UNSUPPORTED;

  if(FALSE == boot_from_nand())
  {
    return EFI_UNSUPPORTED;
  }

  /* Locate protocol */
  status = gBS->LocateProtocol(&gEfiHashProtocolGuid, NULL, (VOID **)&pEfiNandHashProtocol);
  if (EFI_SUCCESS != status)
  {
     /* If hash lib/hash dxe is not available, fallback to secondary method of pseudo GUID generation. */
     if (EFI_NOT_FOUND != status)
     {
       DEBUG ((EFI_D_ERROR, "ERROR: LocateProtocol failed. status = 0x%x. \n\n", status)); 
     }
     gNandGenPartitionGuidFlag = NAND_GEN_PARTITION_GUID_NAME;
  }

  status = gBS->InstallMultipleProtocolInterfaces(&handle, &gEfiNandPartiGuidProtocolGuid, 
                                                  (void **)&NandPartiGuidProtocol, 
                                                  NULL, NULL, NULL );

  return status;

} /* ChipInfoEntryPoint */


