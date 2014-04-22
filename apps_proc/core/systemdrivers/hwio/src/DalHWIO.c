/*============================================================================

FILE:      DalHWIO.c

DESCRIPTION:
  This file contains the init and common routines for the HWIO DAL.

PUBLIC CLASSES:
  HWIO_DriverInit
  HWIO_DriverDeInit
  HWIO_DeviceInit
  HWIO_DeviceDeInit
  HWIO_PowerEvent
  HWIO_Open
  HWIO_Close
  HWIO_Info

============================================================================
        Copyright (c) 2016 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================
  $Header: //components/rel/core.tx/6.0/systemdrivers/hwio/src/DalHWIO.c#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "DalHWIO.h"
#include "DALDeviceId.h"
#include "qurt.h"
#include <stdlib.h>
/*=========================================================================
      Function Definitions
==========================================================================*/


/*==========================================================================

  FUNCTION      HWIO_DriverInit

  DESCRIPTION   See DDIHWIO.h

==========================================================================*/

DALResult HWIO_DriverInit
(
  HWIODrvCtxt *pCtxt
)
{
  DALSYS_PROPERTY_HANDLE_DECLARE(hProps);
  DALSYSPropertyVar PropVar;
  DALResult eResult; 
  uint32 nSize;
  HWIOPhysRegionType *pPhysEntry;

  /*-----------------------------------------------------------------------*/
  /* Create our map mutex object.                                          */
  /*-----------------------------------------------------------------------*/
   pCtxt->mMapSyncObj = (qurt_mutex_t *)malloc(sizeof(qurt_mutex_t));
   if (pCtxt->mMapSyncObj == NULL)
   {
     ERR_FATAL(
       "Failed to allocate memory to the syncronization object",
        0, 0, 0);
   }
   qurt_mutex_init(pCtxt->mMapSyncObj);
  
   //ULog Initialization
   if(DAL_SUCCESS == ULogFront_RealTimeInit (&pCtxt->hHWIOLog, "HWIO Log",
                            HWIO_LOG_SIZE,
                            ULOG_MEMORY_LOCAL, ULOG_LOCK_OS))
   {
     ULogCore_HeaderSet (pCtxt->hHWIOLog,
           "Content-Type: text/tagged-log-1.0;");
   }   

  /*-----------------------------------------------------------------------*/
  /* Get the HWIO map tables.                                              */
  /*-----------------------------------------------------------------------*/

  eResult = DALSYS_GetDALPropertyHandle(DALDEVICEID_HWIO, hProps);

  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  eResult = DALSYS_GetPropertyValue(hProps, "BaseMap", 0, &PropVar);

  if (eResult != DAL_SUCCESS)
    {
    return eResult;
  }

  pCtxt->PhysMap = (HWIOPhysRegionType *)PropVar.Val.pStruct;

  /*-----------------------------------------------------------------------*/
  /* Create the HWIO memory table.                                         */
  /*-----------------------------------------------------------------------*/

  nSize = 0;
  pPhysEntry = pCtxt->PhysMap;
  while (pPhysEntry->szName != NULL)
  {
    pPhysEntry++;
    nSize++;
  }

  nSize *= sizeof(HWIOVirtRegionType);

  pCtxt->VirtMap = (HWIOVirtRegionType*)malloc(nSize);

  if (pCtxt->VirtMap == NULL)
  {
    ERR_FATAL(
      "Failed to allocate memory to virtual map",
      0, 0, 0);
  }
  memset(pCtxt->VirtMap, 0, nSize);

  return DAL_SUCCESS;

} /* END HWIO_DriverInit */


/*==========================================================================

  FUNCTION      HWIO_DriverDeInit

  DESCRIPTION   See DDIHWIO.h

==========================================================================*/

DALResult HWIO_DriverDeInit
(
  HWIODrvCtxt *pCtxt
)
{
  /*-----------------------------------------------------------------------*/
  /* Destroy the mutex objects                                             */
  /*-----------------------------------------------------------------------*/

  qurt_mutex_destroy(pCtxt->mMapSyncObj);
  free(pCtxt->VirtMap);
  pCtxt->VirtMap = NULL;

          return DAL_SUCCESS;

} /* END HWIO_DriverDeInit */



DALResult 
HWIO_DeviceInit(HWIOClientCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

DALResult 
HWIO_DeviceDeInit(HWIOClientCtxt *pCtxt)
{
  return DAL_SUCCESS;
}

DALResult 
HWIO_PowerEvent(HWIOClientCtxt *pCtxt, DalPowerCmd PowerCmd, 
                    DalPowerDomain PowerDomain)
{
  return DAL_SUCCESS;
}

DALResult 
HWIO_Open(HWIOClientCtxt *pCtxt, uint32 dwaccessMode )
{
  return DAL_SUCCESS;
}

DALResult 
HWIO_Close(HWIOClientCtxt *pCtxt)
{
  return DAL_SUCCESS;
}

DALResult 
HWIO_Info(HWIOClientCtxt *pCtxt,DalDeviceInfo *pdeviceInfo, uint32 dwSize)
{
  return DAL_SUCCESS;
}



