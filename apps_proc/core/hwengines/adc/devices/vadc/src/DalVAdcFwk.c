/*============================================================================
  FILE:         DALVAdcFwk.c

  OVERVIEW:     Implementation of a Physical ADC device DAL for the PMIC4
                VADC module.

  DEPENDENCIES: None

                Copyright (c) 2009-2015, 2018 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.tx/6.0/hwengines/adc/devices/vadc/src/DalVAdcFwk.c#1 $$DateTime: 2020/01/30 22:49:35 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2018-12-28  ay   updated for MDM9205
  2015-03-27  jjo  Added tolerance API.
  2015-01-12  jjo  Add TM APIs.
  2014-01-15  jjo  KW fix to free memory on error.
  2013-06-19  jjo  Added support for multiple VADCs.
  2012-17-10  jjo  Added synchronous call stubs.
  2012-06-13  jjo  Updated to use a DAL device ID string name.
  2009-03-16  jdt  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DALFramework.h"
#include "DALSys.h"
#include "DDIAdcDevice.h"
#include "DalVAdc.h"
#include "DALSys.h"
#include <stdlib.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
VAdcDrvCtxt gVAdcDrvCtxt =
{
   .VAdcDALVtbl   = {VAdc_DriverInit, VAdc_DriverDeInit},
   .dwNumDev      = VADC_MAX_NUM_DEVICES,
   .dwSizeDevCtxt = sizeof(VAdcDevCtxt),
   .bInit         = 0,
   .dwRefs        = 0,
   .VAdcDevCtxt   = {{0}}
};

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static uint32
VAdc_DalAdcDevice_AddRef(DalAdcDeviceHandle *h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/
static uint32
VAdc_DalAdcDevice_Detach(DalDeviceHandle *h)
{
   uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   return dwref;
}

static DALResult
VAdc_DalAdcDevice_Init(DalDeviceHandle *h)
{
   VAdcClientCtxt *pCtxt = h->pClientCtxt;
   DALResult result;

   result = VAdc_DeviceInit(pCtxt);
   if (result != DAL_SUCCESS)
   {
      return result;
   }

   return result;
}

static DALResult
VAdc_DalAdcDevice_DeInit(DalDeviceHandle *h)
{
   VAdcClientCtxt *pCtxt = h->pClientCtxt;
   DALResult retResult = DAL_SUCCESS;
   DALResult result;

   result = VAdc_DeviceDeInit(pCtxt);
   if (result != DAL_SUCCESS)
   {
      retResult = result;
   }

   return retResult;
}

static DALResult
VAdc_DalAdcDevice_PowerEvent(
   DalDeviceHandle *h,
   DalPowerCmd PowerCmd,
   DalPowerDomain PowerDomain
   )
{
   return DAL_SUCCESS;
}

static DALResult
VAdc_DalAdcDevice_Open(
   DalDeviceHandle *h,
   uint32 mode
   )
{
   return VAdc_Open(h->pClientCtxt, mode);
}

static DALResult
VAdc_DalAdcDevice_Close(DalDeviceHandle *h)
{
   return VAdc_Close(h->pClientCtxt);
}

static DALResult
VAdc_DalAdcDevice_Info(
   DalDeviceHandle *h,
   DalDeviceInfo* info,
   uint32 infoSize
   )
{
   info->Version = DALADCDEVICE_INTERFACE_VERSION;
   return DAL_SUCCESS;
}

static DALResult
VAdc_DalAdcDevice_SysRequest(
   DalDeviceHandle *h,
   DalSysReq ReqIdx,
   const void *SrcBuf,
   uint32 SrcBufLen,
   void *DestBuf,
   uint32 DestBufLen,
   uint32 *DestBufLenReq
   )
{
   return DAL_ERROR;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalAdcDevice Interface.
------------------------------------------------------------------------------*/
static DALResult
VAdc_DalAdcDevice_GetInputProperties(
   DalDeviceHandle *h,
   const char *pChannelName,
   uint32 *puChannelIdx
   )
{
   VAdcClientCtxt *pCtxt = h->pClientCtxt;

   return VAdc_GetChannel(pCtxt,
                          pChannelName,
                          puChannelIdx);
}

static DALResult
VAdc_DalAdcDevice_GetDeviceProperties(
   DalDeviceHandle *h,
   AdcDeviceDevicePropertiesType *pAdcDeviceProp
   )
{
   VAdcClientCtxt *pCtxt = h->pClientCtxt;

   return VAdc_GetDeviceProperties(pCtxt, pAdcDeviceProp);
}



static DALResult
VAdc_DalAdcDevice_ReadChannel(
   DalDeviceHandle *h,
   uint32 uChannelIdx,
   AdcDeviceResultType *pAdcDeviceReadResult
   )
{
   VAdcClientCtxt *pCtxt = h->pClientCtxt;

   return VAdc_ReadChannel(pCtxt,
                           uChannelIdx,
                           pAdcDeviceReadResult);
}













static void
VAdc_InitInterface(VAdcClientCtxt *pclientCtxt)
{
   static const DalAdcDevice vtbl =
   {
      {
         VAdc_DalAdcDevice_Attach,
         VAdc_DalAdcDevice_Detach,
         VAdc_DalAdcDevice_Init,
         VAdc_DalAdcDevice_DeInit,
         VAdc_DalAdcDevice_Open,
         VAdc_DalAdcDevice_Close,
         VAdc_DalAdcDevice_Info,
         VAdc_DalAdcDevice_PowerEvent,
         VAdc_DalAdcDevice_SysRequest
      },
      VAdc_DalAdcDevice_GetInputProperties,
      VAdc_DalAdcDevice_GetDeviceProperties,

      VAdc_DalAdcDevice_ReadChannel
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pclientCtxt->DalAdcDeviceHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pclientCtxt->DalAdcDeviceHandle.pVtbl  = &vtbl;
   pclientCtxt->DalAdcDeviceHandle.pClientCtxt = pclientCtxt;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
DALResult
VAdc_DalAdcDevice_Attach(
   const char *pszArg,
   DALDEVICEID DeviceId,
   DalDeviceHandle **phDalDevice
   )
{
   DALResult nErr;
   VAdcClientCtxt *pClientCtxt = NULL;

   /* Allocate memory for the client context */
   pClientCtxt = malloc(sizeof(VAdcClientCtxt));
   if (NULL == pClientCtxt)
   {
      return DAL_ERROR;
   }

   memset(pClientCtxt, 0, sizeof(VAdcClientCtxt));

   if (DeviceId != 0)
   {
      nErr = DALFW_AttachToDevice(DeviceId,
                                  (DALDrvCtxt *)&gVAdcDrvCtxt,
                                  (DALClientCtxt *)pClientCtxt);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }

      /* Get a handle to the properties here since the device ID is available */
      nErr = DALSYS_GetDALPropertyHandle(DeviceId, pClientCtxt->pVAdcDevCtxt->hProp);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }
   }
   else
   {
      nErr = DALFW_AttachToStringDevice(pszArg,
                                        (DALDrvCtxt *)&gVAdcDrvCtxt,
                                        (DALClientCtxt *)pClientCtxt);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }

      /* Get a handle to the properties here since the string device name is available */
      nErr = DALSYS_GetDALPropertyHandleStr(pszArg, pClientCtxt->pVAdcDevCtxt->hProp);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }
   }

   VAdc_InitInterface(pClientCtxt);
   (void)VAdc_DalAdcDevice_AddRef(&(pClientCtxt->DalAdcDeviceHandle));
   *phDalDevice = (DalDeviceHandle *)&(pClientCtxt->DalAdcDeviceHandle);

   return DAL_SUCCESS;

error:
   if (pClientCtxt != NULL)
   {
      free(pClientCtxt);
   }

   return nErr;
}

