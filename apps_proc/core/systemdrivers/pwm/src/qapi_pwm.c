/*===========================================================================

             M A I N   P W M   D R I V E R   F I L E

DESCRIPTION

  Contains the majority of functionality for the PWM driver. The
  API in this module MUST always remain target independent.


===========================================================================
  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/systemdrivers/pwm/src/qapi_pwm.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/19   shm     Removed QAPI handler registration for FR57064
03/15/19   shm     Initial version.
===========================================================================*/

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "qapi_pwm.h"
#include "qurt_memory.h"
#include "qapi_txm_base.h"
#include "DalDevice.h"
#include "DDIPWM.h"
#include "DalPWM.h"
#include "DALFramework.h"


/*==========================================================================

                  LOCAL TYPE DEFINES FOR THIS MODULE

==========================================================================*/
DalDeviceHandle *pwm_handle[QAPI_PWM_NUM_FRAMES];
qbool_t          pwm_initialized = FALSE;

#define QAPI_PWM_CVT_TO_DAL_ID(nID) ((DalPWMIdType)(nID & 0xFFFFFFF0))

/*======================================================================
                          FUNCTIONS
=======================================================================*/

/*=============================================================================
  FUNCTION      qapi_pwm_handler
=============================================================================*/
qapi_Status_t qapi_pwm_handler(UINT id, UINT a, UINT b, UINT c, UINT d)
{
  qapi_Status_t retval = QAPI_ERROR;

  switch (id)
  {
    case TXM_QAPI_PWM_GET_ID:
      if (FALSE == qurt_check_if_module_address((qurt_addr_t)c, sizeof(qapi_PWM_ID_t)))
            break;
      retval = qapi_PWM_Get_ID((qapi_PWM_Frame_t)a, (qapi_PWM_Instance_t)b,(qapi_PWM_ID_t*)c);
      break;
    case TXM_QAPI_PWM_RELEASE_ID:
      retval = qapi_PWM_Release_ID((qapi_PWM_ID_t)a);
      break;
    case TXM_QAPI_PWM_ENABLE:
      retval = qapi_PWM_Enable((qapi_PWM_ID_t)a,(qbool_t)b);
      break;
    case TXM_QAPI_PWM_SET_FREQUENCY:
      retval = qapi_PWM_Set_Frequency((qapi_PWM_ID_t)a,(uint32_t)b);
      break;
    case TXM_QAPI_PWM_SET_DUTY_CYCLE:
      retval = qapi_PWM_Set_Duty_Cycle((qapi_PWM_ID_t)a,(uint32_t)b);
      break;
    case TXM_QAPI_PWM_GET_CLOCK_FREQUENCY:
      if (FALSE == qurt_check_if_module_address((qurt_addr_t)b, sizeof(uint32_t)))
            break;
      retval = qapi_PWM_Get_Clock_Frequency((qapi_PWM_ID_t)a,(uint32_t*)b);
      break;
    default:
      break;
  }
  return retval;
}

/*=============================================================================
  FUNCTION      __qapi_PWM_Module_Init
=============================================================================*/
qbool_t __qapi_PWM_Module_Init
(
void
)
{
  pwm_initialized = TRUE;
  memset(pwm_handle, 0x0, QAPI_PWM_NUM_FRAMES * sizeof(DalDeviceHandle*));

  return TRUE;

}

/*=============================================================================
  FUNCTION      __qapi_PWM_Get_DAL_DeviceHandle
=============================================================================*/
static DalDeviceHandle* __qapi_PWM_Get_DAL_DeviceHandle
(
  qapi_PWM_Frame_t    eFrame,
  qapi_PWM_ID_t       nID
)
{
  char*            szName;
  qapi_PWM_Frame_t eLocalFrame;

  if (eFrame >= QAPI_PWM_NUM_FRAMES)
  {
    return NULL;
  }

  /*
   * If an ID is passed, then get the frame number & look-up pwm_handle array
   */
  if (nID != NULL)
  {
    eLocalFrame = (qapi_PWM_Frame_t)(nID & 0xF);
    if (eLocalFrame >= QAPI_PWM_NUM_FRAMES)
    {
      return NULL;
    }

    return pwm_handle[eLocalFrame];
  }

  if (pwm_handle[eFrame] != NULL)
  {
    return pwm_handle[eFrame];
  }

  // TODO: Switch to strncat?
  switch (eFrame)
  {
    case QAPI_PWM_FRAME_0:
      szName = "PWM_0";
      break;
    case QAPI_PWM_FRAME_1:
      szName = "PWM_1";
      break;
    case QAPI_PWM_FRAME_2:
      szName = "PWM_2";
      break;
    default:
      szName = NULL;
      break;
  }

  if (szName != NULL)
  {
    if (DAL_SUCCESS == DAL_StringDeviceAttach(szName, &pwm_handle[eFrame]))
      return pwm_handle[eFrame];
  }

  return NULL;

}

/*=============================================================================
  FUNCTION      qapi_PWM_Get_ID
=============================================================================*/
qapi_Status_t qapi_PWM_Get_ID
(
  qapi_PWM_Frame_t    eFrame,
  qapi_PWM_Instance_t eInstance,
  qapi_PWM_ID_t*      pnID
)
{
  DalPWMIdType     nTempID;
  DalDeviceHandle *pwm_local_handle;

  if (!pwm_initialized)
  {
     return QAPI_ERROR;
  }

  if (eFrame >= QAPI_PWM_NUM_FRAMES)
  {
    return QAPI_ERR_INVALID_PARAM;
  }

  pwm_local_handle = __qapi_PWM_Get_DAL_DeviceHandle(eFrame, NULL);
  if (NULL == pwm_local_handle)
  {
    return QAPI_ERROR;
  }

  if ((eInstance < QAPI_PWM_NUM_INSTANCES) && (pnID != NULL))
  {
    if (DAL_SUCCESS == PWM_GetPWMId(
                         (PWMClientCtxt*)(pwm_local_handle->pClientCtxt),
                         (PWMInstanceType)eInstance,
                         (DalPWMIdType*)&nTempID))
    {
      *pnID = (qapi_PWM_ID_t)(nTempID | (eFrame & 0xF));
      return QAPI_OK;
    }
    else
    {
      return QAPI_ERROR;
    }
  }

  return QAPI_ERR_INVALID_PARAM;
}

/*=============================================================================
  FUNCTION      qapi_PWM_Release_ID
=============================================================================*/
qapi_Status_t qapi_PWM_Release_ID
(
  qapi_PWM_ID_t      nID
)
{
  DalDeviceHandle *pwm_local_handle;

  pwm_local_handle = __qapi_PWM_Get_DAL_DeviceHandle(
                       (qapi_PWM_Frame_t)NULL, nID);

  if (!pwm_initialized)
  {
    return QAPI_ERROR;
  }

  if ((nID != NULL) && pwm_local_handle)
  {
    if (DAL_SUCCESS == PWM_ReleasePWMId(
                         (PWMClientCtxt*)(pwm_local_handle->pClientCtxt),
                         QAPI_PWM_CVT_TO_DAL_ID(nID)))
    {
      return QAPI_OK;
    }
    else
    {
      return QAPI_ERROR;
    }
  }

  return QAPI_ERR_INVALID_PARAM;
}

/*=============================================================================
  FUNCTION      qapi_PWM_Enable
=============================================================================*/
qapi_Status_t qapi_PWM_Enable
(
  qapi_PWM_ID_t      nID,
  qbool_t            bEnable
)
{
  DalDeviceHandle *pwm_local_handle;

  pwm_local_handle = __qapi_PWM_Get_DAL_DeviceHandle(
                       (qapi_PWM_Frame_t)NULL, nID);

  if (!pwm_initialized)
  {
    return QAPI_ERROR;
  }

  if ((nID != NULL) && pwm_local_handle)
  {
    if (DAL_SUCCESS == PWM_Enable(
                         (PWMClientCtxt*)(pwm_local_handle->pClientCtxt),
                         QAPI_PWM_CVT_TO_DAL_ID(nID), bEnable))
    {
      return QAPI_OK;
    }
    else
    {
      return QAPI_ERROR;
    }
  }

  return QAPI_ERR_INVALID_PARAM;

}

/*=============================================================================
  FUNCTION      qapi_PWM_Set_Frequency
=============================================================================*/
qapi_Status_t qapi_PWM_Set_Frequency
(
  qapi_PWM_ID_t      nID,
  uint32_t           nFreqHz
)
{
  DalDeviceHandle *pwm_local_handle;

  pwm_local_handle = __qapi_PWM_Get_DAL_DeviceHandle(
                       (qapi_PWM_Frame_t)NULL, nID);

  if (!pwm_initialized)
  {
    return QAPI_ERROR;
  }

  if ((nID != NULL) && pwm_local_handle)
  {
    if (DAL_SUCCESS == PWM_SetFrequency(
                         (PWMClientCtxt*)(pwm_local_handle->pClientCtxt),
                         QAPI_PWM_CVT_TO_DAL_ID(nID), nFreqHz))
    {
      return QAPI_OK;
    }
    else
    {
      return QAPI_ERROR;
    }
  }

  return QAPI_ERR_INVALID_PARAM;
}

/*=============================================================================
  FUNCTION      qapi_PWM_Set_Duty_Cycle
=============================================================================*/
qapi_Status_t qapi_PWM_Set_Duty_Cycle
(
  qapi_PWM_ID_t      nID,
  uint32_t           nDuty
)
{
  DalDeviceHandle *pwm_local_handle;

  pwm_local_handle = __qapi_PWM_Get_DAL_DeviceHandle(
                       (qapi_PWM_Frame_t)NULL, nID);

  if (!pwm_initialized)
  {
    return QAPI_ERROR;
  }

  if ((nID != NULL) && pwm_local_handle)
  {
    if (DAL_SUCCESS == PWM_SetDutyCycle(
                         (PWMClientCtxt*)(pwm_local_handle->pClientCtxt),
                         QAPI_PWM_CVT_TO_DAL_ID(nID), nDuty))
    {
      return QAPI_OK;
    }
    else
    {
      return QAPI_ERROR;
    }
  }

  return QAPI_ERR_INVALID_PARAM;
}

/*=============================================================================
  FUNCTION      qapi_PWM_Get_Clock_Frequency
=============================================================================*/
qapi_Status_t qapi_PWM_Get_Clock_Frequency
(
  qapi_PWM_ID_t    nID,
  uint32_t*        pnFreqHz
)
{
  DalDeviceHandle *pwm_local_handle;

  pwm_local_handle = __qapi_PWM_Get_DAL_DeviceHandle(
                       (qapi_PWM_Frame_t)NULL, nID);

  if (!pwm_initialized)
  {
    return QAPI_ERROR;
  }

  if ((nID != NULL) && pwm_local_handle)
  {
    if (DAL_SUCCESS == PWM_GetClockFrequency(
                         (PWMClientCtxt*)(pwm_local_handle->pClientCtxt),
                         QAPI_PWM_CVT_TO_DAL_ID(nID), (uint32*)pnFreqHz))
    {
      return QAPI_OK;
    }
    else
    {
      return QAPI_ERROR;
    }
  }

  return QAPI_ERR_INVALID_PARAM;
}
