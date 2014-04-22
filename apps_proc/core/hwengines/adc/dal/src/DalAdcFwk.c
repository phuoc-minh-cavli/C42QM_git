/*============================================================================
  FILE:         DALAdcFwk.c

  OVERVIEW:     Implementation of the ADC DAL

  DEPENDENCIES: None

                Copyright (c) 2009-2012, 2015, 2018 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.tx/6.0/hwengines/adc/dal/src/DalAdcFwk.c#1 $$DateTime: 2020/01/30 22:49:35 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2018-12-28  ay   updated for MDM9205
  2015-03-27  jjo  Added tolerance API.
  2015-01-08  jjo  Added TM APIs.
  2012-04-19  jdt  Updated for 8974.
  2010-06-20 Purna klock work fix in Adc_DalAdc_Attach
  2009-03-16  jdt  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DALFramework.h"
#include "DALSys.h"
#include "DDIAdc.h"
#include "DalAdc.h"
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
AdcDrvCtxt gAdcDrvCtxt =
{
   .AdcDALVtbl    = {Adc_DriverInit, Adc_DriverDeInit},
   .dwNumDev      = ADC_MAX_NUM_DEVICES,
   .dwSizeDevCtxt = sizeof(AdcDevCtxt),
   .bInit         = 0,
   .dwRefs        = 0,
   .AdcDevCtxt    = {{0}}
};

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static DALBOOL init_done = FALSE;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static uint32
Adc_DalAdc_AddRef(DalAdcHandle *h)
{
   return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*------------------------------------------------------------------------------
Following functions are defined in DalDevice DAL Interface.
------------------------------------------------------------------------------*/
static uint32
Adc_DalAdc_Detach(DalDeviceHandle *h)
{
   uint32 dwref;

   /* Unregister the client */
   (void)Adc_DeregisterClient(h->pClientCtxt);

   dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
   if (0 == dwref)
   {
      /* Release the client context */
      free(h->pClientCtxt);
   }
   return dwref;
}

static DALResult
Adc_DalAdc_Init(DalDeviceHandle *h)
{
   DALResult result;

   if (init_done == TRUE)
   {
      return DAL_SUCCESS;
   }

   result = Adc_DeviceInit(h->pClientCtxt);
   if (result == DAL_SUCCESS)
   {
      init_done = TRUE;
   }

   return result;
}

static DALResult
Adc_DalAdc_DeInit(DalDeviceHandle *h)
{
   return Adc_DeviceDeInit(h->pClientCtxt);
}

static DALResult
Adc_DalAdc_PowerEvent(
   DalDeviceHandle *h,
   DalPowerCmd PowerCmd,
   DalPowerDomain PowerDomain
   )
{
   return Adc_PowerEvent(h->pClientCtxt, PowerCmd, PowerDomain);
}

static DALResult
Adc_DalAdc_Open(
   DalDeviceHandle *h,
   uint32 mode
   )
{
   return Adc_Open(h->pClientCtxt, mode);
}

static DALResult
Adc_DalAdc_Close(DalDeviceHandle *h)
{
   return Adc_Close(h->pClientCtxt);
}

static DALResult
Adc_DalAdc_Info(
   DalDeviceHandle *h,
   DalDeviceInfo *info,
   uint32 infoSize
   )
{
   info->Version = DALADC_INTERFACE_VERSION;
   return Adc_Info(h->pClientCtxt,info,infoSize);
}

static DALResult
Adc_DalAdc_SysRequest(
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
Following functions are extended in DalAdc Interface.
------------------------------------------------------------------------------*/
static DALResult
Adc_DalAdc_GetAdcInputProperties(
   DalDeviceHandle *h,
   const char *pInputName,
   uint32 nInputNameSize,
   AdcInputPropertiesType *pAdcInputProps
   )
{
   return Adc_GetAdcInputProperties(((DalAdcHandle *)h)->pClientCtxt,
                                    pInputName,
                                    nInputNameSize,
                                    pAdcInputProps);
}

static DALResult
Adc_DalAdc_Read(
   DalDeviceHandle *h,
   const AdcInputPropertiesType *pAdcInputProps,
   AdcResultType *pAdcResult
   )
{
   return Adc_Read(((DalAdcHandle *)h)->pClientCtxt,
                   pAdcInputProps,
                   pAdcResult);
}

//static DALResult
//Adc_DalAdc_RequestConversion(
//   DalDeviceHandle *h,
//   const AdcRequestParametersType *pAdcParams,
//   AdcRequestStatusType *pAdcRequestStatus
//   )
//{
//   return Adc_RequestConversion(((DalAdcHandle *)h)->pClientCtxt,
//                                pAdcParams,
//                                pAdcRequestStatus);
//}
//
//static DALResult
//Adc_DalAdc_RequestRecalibration(
//   DalDeviceHandle *h,
//   const AdcRequestParametersType *pAdcParams
//   )
//{
//   return Adc_RequestRecalibration(((DalAdcHandle *)h)->pClientCtxt, pAdcParams);
//}







static void
Adc_InitInterface(AdcClientCtxt *pClientCtxt)
{
   static const DalAdc vtbl =
   {
      {
         Adc_DalAdc_Attach,
         Adc_DalAdc_Detach,
         Adc_DalAdc_Init,
         Adc_DalAdc_DeInit,
         Adc_DalAdc_Open,
         Adc_DalAdc_Close,
         Adc_DalAdc_Info,
         Adc_DalAdc_PowerEvent,
         Adc_DalAdc_SysRequest
      },
      Adc_DalAdc_GetAdcInputProperties,
      
      Adc_DalAdc_Read
     
   };
   /*--------------------------------------------------------------------------
   Depending upon client type setup the vtables (entry points)
   --------------------------------------------------------------------------*/
   pClientCtxt->DalAdcHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
   pClientCtxt->DalAdcHandle.pVtbl = &vtbl;
   pClientCtxt->DalAdcHandle.pClientCtxt = pClientCtxt;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
DALResult
Adc_DalAdc_Attach(
   const char *pszArg,
   DALDEVICEID DeviceId,
   DalDeviceHandle **phDalDevice
   )
{
   DALResult nErr;
   AdcClientCtxt *pClientCtxt = NULL;

   /* Allocate memory for the client context */
   pClientCtxt = malloc(sizeof(AdcClientCtxt));
   if (NULL == pClientCtxt)
   {
      return DAL_ERROR;
   }

   memset(pClientCtxt, 0, sizeof(AdcClientCtxt));

   if (DeviceId != 0)
   {
      nErr = DALFW_AttachToDevice(DeviceId,
                                  (DALDrvCtxt *)&gAdcDrvCtxt,
                                  (DALClientCtxt *)pClientCtxt);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }

      /* Get a handle to the properties here since the device ID is available */
      nErr = DALSYS_GetDALPropertyHandle(DeviceId, pClientCtxt->pAdcDevCtxt->hProp);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }
   }
   else
   {
      nErr = DALFW_AttachToStringDevice(pszArg,
                                        (DALDrvCtxt *)&gAdcDrvCtxt,
                                        (DALClientCtxt *)pClientCtxt);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }

      /* Get a handle to the properties here since the string device name is available */
      nErr = DALSYS_GetDALPropertyHandleStr(pszArg, pClientCtxt->pAdcDevCtxt->hProp);
      if (nErr != DAL_SUCCESS)
      {
         goto error;
      }
   }

   Adc_InitInterface(pClientCtxt);
   (void)Adc_DalAdc_AddRef(&(pClientCtxt->DalAdcHandle));
   nErr = Adc_RegisterClient(pClientCtxt);
   if (nErr != DAL_SUCCESS)
   {
      goto error;
   }

   *phDalDevice = (DalDeviceHandle *)&(pClientCtxt->DalAdcHandle);

   return DAL_SUCCESS;

error:
   free(pClientCtxt);

   return nErr;
}

