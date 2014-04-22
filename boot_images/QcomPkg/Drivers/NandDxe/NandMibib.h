/** @file NandMibib.h
   
  This file provides NAND MIBIB Partition Table feature internal definitions. 

  Copyright (c) 2017 Qualcomm Technologies, Inc. 
  All Rights Reserved. 
  Qualcomm Technologies Proprietary and Confidential

**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   ---     -----------------------------------------------------------
2017-03-03   eo      Initial version

=============================================================================*/

#ifndef _NAND_MIBIB_H_
#define _NAND_MIBIB_H_

#include <Library/UefiLib.h>

#include <flash_mibib.h>
#include <flash_micrc.h>
#include <flash_miparti.h>

#define PARTITON_NAME_SIZE_MAX       (32)

/* MIBIB partition table */
typedef struct{
  VOID                     *PartiHandle;
  UINT8                     PartiName[PARTITON_NAME_SIZE_MAX];
  UINT32                    BlockCount;
} NandMibibPartiEntry;


typedef struct {
UINT32                      PartiNumEntries;
UINT32                      LbaSize;
UINT32                      EraseBlockSize;
NandMibibPartiEntry         PartiEntries[FLASH_NUM_PART_ENTRIES];
} NandMibibPartiTable;

/******************************************************************************
* Name: NandInitMibibPartiTable
*
* Description:
*    Initialize the MIBIB partition table
*
* Arguments:
*    Buffer             [OUT] : MIBIB partition table
*
* Returns:
*    EFI_SUCCESS or error status
*
******************************************************************************/
EFI_STATUS 
NandInitMibibPartiTable (
  OUT NandMibibPartiTable    *MibibPartiTable
  );

#endif /* ifdef NAND_MIBIB_H_ */
