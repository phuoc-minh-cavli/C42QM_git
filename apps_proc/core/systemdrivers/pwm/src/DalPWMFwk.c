/*
===============================================================================
*/
/**
  @file DalPWMFwk.c

  DAL framework implementation file for the PWM device driver.
*/
/*
  =============================================================================

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR.

  =============================================================================

  $Header: //components/rel/core.tx/6.0/systemdrivers/pwm/src/DalPWMFwk.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  02/28/19   shm     Initial version

  =============================================================================
*/


/*=============================================================================
      Include Files
===============================================================================*/


#include "DALFramework.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DDIPWM.h"
#include "DALPropDef.h"
#include "DalPWM.h"
#include "DDIHWIO.h"

#include <stringl/stringl.h>

/*=============================================================================
      Macro Definitions
=============================================================================*/

/*
 * Take a DAL driver handle and dereference into the driver context.
 */
#define HANDLE2CTXT(h) ((DalPWMHandle *)h)->pClientCtxt


/*=============================================================================
      Type Definitions
=============================================================================*/


/*=============================================================================
      Function Prototypes
=============================================================================*/

DALResult        PWM_DalPWM_Attach  (const char *, DALDEVICEID,
                                     DalDeviceHandle **);
static DALResult DALPWM_DriverInit  (PWMDrvCtxt *);
static DALResult DALPWM_DriverDeInit(PWMDrvCtxt *);


/*=============================================================================
      Data Declarations
=============================================================================*/

PWMDrvCtxt PWM_DriverCtxt =
{
  {
    PWM_DriverInit,
    PWM_DriverDeInit
  },
  DALPWM_NUM_DEVICES, sizeof(PWMDevCtxt)
};


/*=============================================================================
      Functions
=============================================================================*/

static uint32 PWM_AddRef
(
  DalPWMHandle *h
)
{
  return DALFW_AddRef((DALClientCtxt *)(h->pClientCtxt));
}

/*
 * Following functions are defined in DalDevice DAL interface.
 */

static uint32 PWM_DalPWM_Detach
(
  DalDeviceHandle *h
)
{
  uint32 dwref = DALFW_Release((DALClientCtxt *)(h->pClientCtxt));
  return dwref;
}

static DALResult PWM_DalPWM_Init
(
  DalDeviceHandle *h
)
{
  return PWM_DeviceInit(HANDLE2CTXT(h));
}

static DALResult PWM_DalPWM_DeInit
(
  DalDeviceHandle *h
)
{
  return PWM_DeviceDeInit(HANDLE2CTXT(h));
}

static DALResult PWM_DalPWM_PowerEvent
(
  DalDeviceHandle *h,
  DalPowerCmd      PowerCmd,
  DalPowerDomain   PowerDomain
)
{
  return DAL_SUCCESS;
}

static DALResult PWM_DalPWM_Open
(
  DalDeviceHandle *h,
  uint32           mode
)
{
  return DAL_SUCCESS;
}

static DALResult PWM_DalPWM_Close
(
  DalDeviceHandle *h
)
{
  return DAL_SUCCESS;
}

static DALResult PWM_DalPWM_Info
(
  DalDeviceHandle *h,
  DalDeviceInfo   *info,
  uint32           infoSize
)
{
  info->Version = DALPWM_INTERFACE_VERSION;
  return DAL_SUCCESS;
}

static DALResult PWM_DalPWM_SysRequest
(
  DalDeviceHandle *h,
  DalSysReq        ReqIdx,
  const void      *SrcBuf,
  uint32           SrcBufLen,
  void            *DestBuf,
  uint32           DestBufLen,
  uint32          *DestBufLenReq
)
{
  return DAL_SUCCESS;
}

/*
 * Following functions are extended in DalTimetick interface.
 */

static DALResult PWM_DalPWM_GetPWMId
(
  DalDeviceHandle *h,
  PWMInstanceType  eInst,
  DalPWMIdType    *pId
)
{
  return PWM_GetPWMId(HANDLE2CTXT(h), eInst, pId);
}

static DALResult PWM_DalPWM_ReleasePWMId
(
  DalDeviceHandle *h,
  DalPWMIdType     nId
)
{
  return PWM_ReleasePWMId(HANDLE2CTXT(h), nId);
}

static DALResult PWM_DalPWM_Enable
(
  DalDeviceHandle *h,
  DalPWMIdType     nId,
  boolean          bEnable
)
{
  return PWM_Enable(HANDLE2CTXT(h), nId, bEnable);
}

static DALResult PWM_DalPWM_SetFrequency
(
  DalDeviceHandle *h,
  DalPWMIdType     nId,
  uint32           nFreqHz
)
{
  return PWM_SetFrequency(HANDLE2CTXT(h), nId, nFreqHz);
}

static DALResult PWM_DalPWM_SetDutyCycle
(
  DalDeviceHandle *h,
  DalPWMIdType     nId,
  uint32           nDuty
)
{
  return PWM_SetDutyCycle(HANDLE2CTXT(h), nId, nDuty);
}

static DALResult PWM_DalPWM_GetClockFrequency
(
  DalDeviceHandle *h,
  DalPWMIdType     nId,
  uint32          *pnFreqHz
)
{
  return PWM_GetClockFrequency(HANDLE2CTXT(h), nId, pnFreqHz);
}


/* ============================================================================
**  Function : PWM_InitInterface
** ==========================================================================*/
/**
  Initialize the client context for communicating to the DAL.

  This function initializes the given client context. It is invoked
  internally when a client attaches to the DAL device.

  @param *pClientCtxt [in] -- Pointer to the DAL client context.

  @return
  None.

  @dependencies
  None.
*/
static void PWM_InitInterface
(
  PWMClientCtxt* pClientCtxt
)
{
  static DalPWM vtbl =
  {
    {
      PWM_DalPWM_Attach,
      PWM_DalPWM_Detach,
      PWM_DalPWM_Init,
      PWM_DalPWM_DeInit,
      PWM_DalPWM_Open,
      PWM_DalPWM_Close,
      PWM_DalPWM_Info,
      PWM_DalPWM_PowerEvent,
      PWM_DalPWM_SysRequest
    },

    PWM_DalPWM_GetPWMId,
    PWM_DalPWM_ReleasePWMId,
    PWM_DalPWM_Enable,
    PWM_DalPWM_SetFrequency,
    PWM_DalPWM_SetDutyCycle,
    PWM_DalPWM_GetClockFrequency,
  };

  pClientCtxt->DalPWMHandle.dwDalHandleId = DALDEVICE_INTERFACE_HANDLE_ID;
  pClientCtxt->DalPWMHandle.pVtbl = &vtbl;
  pClientCtxt->DalPWMHandle.pClientCtxt = pClientCtxt;

} /* END PWM_InitInterface */


/* ============================================================================
**  Function : PWM_DalPWM_Attach
** ==========================================================================*/
/**
  Internal DAL framework function for attaching to the PWM DAL.

  This function attaches to the PWM DAL and initializes the given
  device handle.

  @param *pszArg      [in] -- String passed by user during attach.
  @param  DeviceId    [in] -- The device id, should be NULL if an integer
                              device ID is not defined in DalDeviceId.h
  @param  phDalDevice[out] -- DAL device handle to fill in.

  @return
  None.

  @dependencies
  None.
*/

DALResult PWM_DalPWM_Attach
(
  const char       *pszArg,
  DALDEVICEID       DeviceId,
  DalDeviceHandle **phDalDevice
)
{
  DALResult      nErr;
  PWMClientCtxt *pClientCtxt;
  static         PWMClientCtxt ClientCtxt[DALPWM_NUM_DEVICES];

  /*
   * Sanity checks
   */
  if (phDalDevice == NULL || pszArg == NULL)
  {
    return DAL_ERROR;
  }

  /*
   * Get the PWM frame being attached to - PWM_0/PWM_1/PWM_2 and so on
   */
  pClientCtxt = &ClientCtxt[pszArg[4]-'0'];

  *phDalDevice = NULL;

  nErr = DALFW_AttachToStringDevice(
           pszArg, (DALDrvCtxt *)&PWM_DriverCtxt,
           (DALClientCtxt *)pClientCtxt);

  if (nErr == DAL_SUCCESS)
  {
    strlcpy(
      pClientCtxt->pDevCtxt->szDevName, pszArg, DALPWM_MAX_STRING_ID_LENGTH);
    PWM_InitInterface(pClientCtxt);
    PWM_AddRef(&(pClientCtxt->DalPWMHandle));
    *phDalDevice = (DalDeviceHandle *)&(pClientCtxt->DalPWMHandle);
  }

  return nErr;

} /* END PWM_DalPWM_Attach */
