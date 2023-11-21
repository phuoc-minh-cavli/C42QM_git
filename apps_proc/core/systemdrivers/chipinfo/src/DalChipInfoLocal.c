/*============================================================================

FILE:      DalChipInfoLocal.c

DESCRIPTION:
  This file implements the portion of the ChipInfo DAL that reads information
  from the hardware.

PUBLIC CLASSES:
  ChipInfo_DriverInit
  ChipInfo_DriverDeInit

============================================================================
  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR
============================================================================
  $Header: //components/rel/core.tx/6.0/systemdrivers/chipinfo/src/DalChipInfoLocal.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $
 
when       who     what, where, why
--------   ---     --------------------------------------------------------
01/08/19   shm     Removing 9x07 chip IDs
09/25/18   shm     First version for 9205 codebase
============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "DALDeviceId.h"
#include "DalChipInfo.h"
#include "DDIHWIO.h"
#include <stringl/stringl.h>


/*=========================================================================
      Type Definitions
==========================================================================*/


/*
 * DalChipInfoDataLUTType
 *
 * Static per-chip data definitions.
 *
 * szChipId:  String representing the MSM name.
 * nFamily:   Chip Family enum.
 * nModem:    Bitmask of supported modems on this MSM.
 */
typedef struct
{
  const char           *szChipId;
  DalChipInfoFamilyType nFamily;
  DalChipInfoModemType  nModem;
} DalChipInfoDataLUTType;


/*
 * Define some modem combination shortcuts.
 */
#define DALCHIPINFO_MODEM_1XEVDO       (DALCHIPINFO_MODEM_1X | DALCHIPINFO_MODEM_EVDO)
#define DALCHIPINFO_MODEM_1XEVDO_REVA  (DALCHIPINFO_MODEM_1XEVDO | DALCHIPINFO_MODEM_EVDO_REVA)
#define DALCHIPINFO_MODEM_1XEVDO_REVB  (DALCHIPINFO_MODEM_1XEVDO_REVA | DALCHIPINFO_MODEM_EVDO_REVB)
#define DALCHIPINFO_MODEM_WWAN         (DALCHIPINFO_MODEM_1XEVDO_REVB | DALCHIPINFO_MODEM_GSM | DALCHIPINFO_MODEM_UMTS)


/*=========================================================================
      Data Definitions
==========================================================================*/

/*
 * DALCHIPINFO_MFGNUM_*
 *
 * Manufacturer IDs
 */
#define DALCHIPINFO_MFGNUM_QUALCOMM 0x170

/*
 * DALCHIPINFO_PARTNUM_*
 *
 * Definitions of part number fields.
 */

#define DALCHIPINFO_PARTNUM_MDM9205        0x107

/*
 * amPartMap
 *
 * Map of part numbers to logical parts.
 */
DalChipInfoPartMapType amPartMap[] =
{
  { DALCHIPINFO_PARTNUM_MDM9205,    DALCHIPINFO_ID_MDM9205   },
};

/*
 * chipIdMap
 *
 * Map of logical parts to DalChipInfoDataLUT indexes.
 */
DalChipInfoIdMapType chipIdMap[] =
{
  { DALCHIPINFO_ID_UNKNOWN,  0   },
  { DALCHIPINFO_ID_MDM9205,  1   },
};

#define DALCHIPINFO_ID_MAP_SIZE (sizeof(amPartMap) / sizeof(amPartMap[0]))
#define DALCHIPINFO_CHIPID_MAP_SIZE (sizeof(chipIdMap) / sizeof(chipIdMap[0]))
/*
 * DalChipInfoDataLUT
 *
 * Array of chip id strings, families and modem support indexed by MSM ID.
 */
static const DalChipInfoDataLUTType DalChipInfoDataLUT[DALCHIPINFO_CHIPID_MAP_SIZE] =
{
  { "UNKNOWN",     DALCHIPINFO_FAMILY_UNKNOWN,  0 },
  { "MDM9205",     DALCHIPINFO_FAMILY_MDM9205,  DALCHIPINFO_MODEM_WWAN | DALCHIPINFO_MODEM_CGPS | DALCHIPINFO_MODEM_LTE },
};

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ===========================================================================
**  ChipInfo_GetFamily
**
** ======================================================================== */

void ChipInfo_GetFamily
(
  ChipInfoDrvCtxt       *pCtxt,
  DalChipInfoFamilyType *peFamily,
  uint32                nHWRevNum
)
{
  DalChipInfoIdType ePart;
  uint32             nPart=0,n;

  /*
   * Read part number from hardware.
   */
  ChipInfo_GetPart(pCtxt, &ePart, nHWRevNum);

  /*
   * Look up family.
   */
  if (ePart < DALCHIPINFO_NUM_IDS)
  {
    /*
     * Look up using the table.
     */
    for (n = 0; n < DALCHIPINFO_CHIPID_MAP_SIZE; n++)
    {
      if (ePart == chipIdMap[n].ePart)
      {
        nPart = chipIdMap[n].nPart;
        break;
      }
    }
  
    if (n == DALCHIPINFO_CHIPID_MAP_SIZE)
    {
      nPart = 0;
    }
    *peFamily = DalChipInfoDataLUT[nPart].nFamily;
  }
  else
  {
    *peFamily = DALCHIPINFO_FAMILY_UNKNOWN;
  }

} /* END ChipInfo_GetFamily */


/* ===========================================================================
**  ChipInfo_GetFamilyByPart
**
** ======================================================================== */

void ChipInfo_GetFamilyByPart
(
  DalChipInfoIdType    ePart,
  DalChipInfoFamilyType *peFamily
)
{
  uint32             nPart=0,n;

  /*
   * Look up family.
   */
  if (ePart < DALCHIPINFO_NUM_IDS)
  {
    /*
     * Look up using the table.
     */
    for (n = 0; n < DALCHIPINFO_CHIPID_MAP_SIZE; n++)
    {
      if (ePart == chipIdMap[n].ePart)
      {
        nPart = chipIdMap[n].nPart;
        break;
      }
    }
  
    if (n == DALCHIPINFO_CHIPID_MAP_SIZE)
    {
      nPart = 0;
    }
    *peFamily = DalChipInfoDataLUT[nPart].nFamily;
  }
  else
  {
    *peFamily = DALCHIPINFO_FAMILY_UNKNOWN;
  }

} /* END ChipInfo_GetFamilyByPart */


/* ===========================================================================
**  ChipInfo_GetPart
**
** ======================================================================== */

void ChipInfo_GetPart
(
  ChipInfoDrvCtxt     *pCtxt,
  DalChipInfoIdType   *pePart,
  uint32              nHWRevNum
)
{
  uint32 nPart, n;

  /*
   * Read the part number.
   */
  ChipInfo_GetRawPart(pCtxt, &nPart, nHWRevNum);

  /*
   * Look up using the table.
   */
  for (n = 0; n < DALCHIPINFO_ID_MAP_SIZE; n++)
  {
    if (nPart == amPartMap[n].nPart)
    {
      *pePart = amPartMap[n].ePart;
      break;
    }
  }

  if (n == DALCHIPINFO_ID_MAP_SIZE)
  {
    *pePart = DALCHIPINFO_ID_UNKNOWN;
  }

} /* END ChipInfo_GetPart */


/* ===========================================================================
**  ChipInfo_GetVersion
**
** ======================================================================== */

void ChipInfo_GetVersion
(
  ChipInfoDrvCtxt        *pCtxt,
  DalChipInfoVersionType *pnVersion,
  uint32                 nSOCHWVersion
)
{
  uint32 majNumBmsk, majNumShft, minNumBmsk, minNumShft, majorNum;
  DALSYSPropertyVar PropVar;

 
  DALSYS_GetPropertyValue(pCtxt->hProps,"MAJOR_VERSION_BMSK",0,&PropVar);
  majNumBmsk = PropVar.Val.dwVal;

  DALSYS_GetPropertyValue(pCtxt->hProps,"MAJOR_VERSION_SHFT",0,&PropVar);
  majNumShft = PropVar.Val.dwVal;

  DALSYS_GetPropertyValue(pCtxt->hProps,"MINOR_VERSION_BMSK",0,&PropVar);
  minNumBmsk = PropVar.Val.dwVal;

  DALSYS_GetPropertyValue(pCtxt->hProps,"MINOR_VERSION_SHFT",0,&PropVar);
  minNumShft = PropVar.Val.dwVal;

  /* If MAJOR VERSION is 0x0 we treat it as 0x1 to provide correct versioninfo on Dino*/
  majorNum = (nSOCHWVersion & majNumBmsk) >> majNumShft;
  if( majorNum == 0x0)
  {
    majorNum = 0x1;
  }
  *pnVersion = DALCHIPINFO_VERSION(
     (majorNum),
      ((nSOCHWVersion & minNumBmsk) >> minNumShft));

} /* END ChipInfo_GetVersion */


/* ===========================================================================
**  ChipInfo_GetManufacturer
**
** ======================================================================== */

void ChipInfo_GetManufacturer
(
  ChipInfoDrvCtxt             *pCtxt,
  DalChipInfoManufacturerType *peManufacturer,
  uint32                      nHWRevNum
)
{
  uint32 pnRawManufacturer;

  ChipInfo_GetRawManufacturer(pCtxt, &pnRawManufacturer, nHWRevNum);
  switch(pnRawManufacturer)
    {
    case DALCHIPINFO_MFGNUM_QUALCOMM:
    {
      *peManufacturer = DALCHIPINFO_MFG_QUALCOMM;
      break;
    }

    default:
    {
      *peManufacturer = DALCHIPINFO_MFG_UNKNOWN;
      break;
    }
  }

} /* END ChipInfo_GetManufacturer */


/* ===========================================================================
**  ChipInfo_GetRawManufacturer
**
** ======================================================================== */

void ChipInfo_GetRawManufacturer
(
  ChipInfoDrvCtxt     *pCtxt,
  uint32 *pnRawManufacturer,
  uint32 nHWRevNum
)
{

  uint32 mfgIdBmsk, mfgIdShft;
  DALSYSPropertyVar PropVar;


  DALSYS_GetPropertyValue(pCtxt->hProps,"QUALCOMM_MFG_ID_BMSK",0,&PropVar);
  mfgIdBmsk = PropVar.Val.dwVal;

  DALSYS_GetPropertyValue(pCtxt->hProps,"QUALCOMM_MFG_ID_SHFT",0,&PropVar);
  mfgIdShft = PropVar.Val.dwVal;

  *pnRawManufacturer = (nHWRevNum & mfgIdBmsk) >> mfgIdShft;

} /* END ChipInfo_GetRawManufacturer */


/* ===========================================================================
**  ChipInfo_GetRawPart
**
** ======================================================================== */

void ChipInfo_GetRawPart
(
  ChipInfoDrvCtxt     *pCtxt,
  uint32 *pnRawPart,
  uint32 nHWRevNum
)
{

  uint32 partNumBmsk, partNumShft;
  DALSYSPropertyVar PropVar;

  DALSYS_GetPropertyValue(pCtxt->hProps,"PARTNUM_BMSK",0,&PropVar);
  partNumBmsk = PropVar.Val.dwVal;

  DALSYS_GetPropertyValue(pCtxt->hProps,"PARTNUM_SHFT",0,&PropVar);
  partNumShft = PropVar.Val.dwVal;

  *pnRawPart = (nHWRevNum & partNumBmsk) >> partNumShft;

} /* END ChipInfo_GetRawPart */


/* ===========================================================================
**  ChipInfo_GetRawVersion
**
** ======================================================================== */

void ChipInfo_GetRawVersion
(
  ChipInfoDrvCtxt     *pCtxt,
  uint32 *pnRawVersion,
  uint32 nHWRevNum
)
{
  uint32 versionIdBmsk, versionIdShft;
  DALSYSPropertyVar PropVar;

  DALSYS_GetPropertyValue(pCtxt->hProps,"VERSION_ID_BMSK",0,&PropVar);
  versionIdBmsk = PropVar.Val.dwVal;

  DALSYS_GetPropertyValue(pCtxt->hProps,"VERSION_ID_SHFT",0,&PropVar);
  versionIdShft = PropVar.Val.dwVal;

  *pnRawVersion = (nHWRevNum & versionIdBmsk) >> versionIdShft;

} /* END ChipInfo_GetRawVersion */



/*=========================================================================
      Implementation
==========================================================================*/


/*==========================================================================

  FUNCTION      ChipInfo_DriverInit

  DESCRIPTION   See DDIChipInfo.h

==========================================================================*/

DALResult ChipInfo_DriverInit
(
  ChipInfoDrvCtxt     *pCtxt
)
{
  DALResult eResult = DAL_SUCCESS;
  DALSYSPropertyVar PropVar;
  uint32 nHWIORegAddr = 0, nSOCHWVerRegAddr = 0;
  uint32 nHWRevNum, nSOCHWVerNum;
  uint32 nHWRevNumPhysAddress, nSOCHWVerPhysAddress;
  uint32 n,nPart=0;
    
  /* Get the property handle */
  DALSYS_GetDALPropertyHandle(DALDEVICEID_CHIPINFO,pCtxt->hProps);

  DALSYS_GetPropertyValue(pCtxt->hProps,"HWREVNUM_PHYS_ADDR",0,&PropVar);
  nHWRevNumPhysAddress = PropVar.Val.dwVal;

  DALSYS_GetPropertyValue(pCtxt->hProps,"SOC_HW_VERSION_PHYS_ADDR",0,&PropVar);
  nSOCHWVerPhysAddress = PropVar.Val.dwVal;

  /*-----------------------------------------------------------------------*/
  /* Attach to the HWIO DAL.                                               */
  /*-----------------------------------------------------------------------*/
  eResult = DAL_DeviceAttach(DALDEVICEID_HWIO, &pCtxt->phDalHWIO);
  if (eResult == DAL_SUCCESS)
  {
     eResult = DalHWIO_MapRegionByAddress(pCtxt->phDalHWIO, (uint8*)nHWRevNumPhysAddress,(uint8 **)&nHWIORegAddr);
     eResult |= DalHWIO_MapRegionByAddress(pCtxt->phDalHWIO, (uint8*)nSOCHWVerPhysAddress,(uint8 **)&nSOCHWVerRegAddr);
  }
  else
  {
    nHWIORegAddr = nHWRevNumPhysAddress;
    nSOCHWVerRegAddr = nSOCHWVerPhysAddress;
  }
  DALSYS_GetPropertyValue(pCtxt->hProps,"HWREVNUM_OFFSET",0,&PropVar);
  nHWIORegAddr = PropVar.Val.dwVal  + nHWIORegAddr;
  DALSYS_GetPropertyValue(pCtxt->hProps,"SOC_HW_VERSION_OFFSET",0,&PropVar);
  nSOCHWVerRegAddr = PropVar.Val.dwVal  + nSOCHWVerRegAddr;

  nHWRevNum = *(uint32*)nHWIORegAddr;
  nSOCHWVerNum = *(uint32*)nSOCHWVerRegAddr;

  /*-----------------------------------------------------------------------*/
  /* Read chip information from HW.                                        */
  /*-----------------------------------------------------------------------*/

  ChipInfo_GetVersion(pCtxt,(DalChipInfoVersionType *)&pCtxt->nChipVersion, nSOCHWVerNum);
  ChipInfo_GetPart(pCtxt,(DalChipInfoIdType *)&pCtxt->eChipId, nHWRevNum);
  ChipInfo_GetRawVersion(pCtxt,&pCtxt->nRawChipVersion, nHWRevNum);
  ChipInfo_GetRawPart(pCtxt,&pCtxt->nRawChipId, nHWRevNum);
  ChipInfo_GetFamily(pCtxt,(DalChipInfoFamilyType *)&pCtxt->eChipFamily, nHWRevNum);

  /*-----------------------------------------------------------------------*/
  /* Check for override of the ChipId from the properties.                 */
  /*-----------------------------------------------------------------------*/

  if (pCtxt->eChipId == DALCHIPINFO_ID_UNKNOWN)
  {
    eResult = DALSYS_GetPropertyValue(pCtxt->hProps, "ChipIdOverride", 0, &PropVar);

    if (eResult == DAL_SUCCESS)
    {
      pCtxt->eChipId = (DalChipInfoIdType)PropVar.Val.dwVal;

      ChipInfo_GetFamilyByPart(
        (DalChipInfoIdType)pCtxt->eChipId,
        (DalChipInfoFamilyType *)&pCtxt->eChipFamily);

      pCtxt->nChipVersion = 0x00010000;
    }
  }

  /*-----------------------------------------------------------------------*/
  /* Fill in derived information.                                          */
  /*-----------------------------------------------------------------------*/

  /*
   * Look up using the table.
   */
  for (n = 0; n < DALCHIPINFO_CHIPID_MAP_SIZE; n++)
  {
    if (pCtxt->eChipId == chipIdMap[n].ePart)
    {
      nPart = chipIdMap[n].nPart;
      break;
    }
  }

  if (n == DALCHIPINFO_CHIPID_MAP_SIZE)
  {
    nPart = 0;
  }
  pCtxt->nModemSupport = DalChipInfoDataLUT[nPart].nModem;
  strlcpy(
    pCtxt->szChipIdString, DalChipInfoDataLUT[nPart].szChipId,
    DALCHIPINFO_MAX_ID_LENGTH);


  /*-----------------------------------------------------------------------*/
  /* Success.                                                              */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END ChipInfo_DriverInit */


/*==========================================================================

  FUNCTION      ChipInfo_DriverDeInit

  DESCRIPTION   See DDIChipInfo.h

==========================================================================*/

DALResult ChipInfo_DriverDeInit
(
  ChipInfoDrvCtxt *pCtxt
)
{
  if (pCtxt->phDalHWIO != NULL)
  {
    DalDevice_Close(pCtxt->phDalHWIO);
  }

  /*-----------------------------------------------------------------------*/
  /* Success.                                                              */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END ChipInfo_DriverDeInit */


