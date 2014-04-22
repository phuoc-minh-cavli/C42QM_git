#ifndef DDIPWM_H
#define DDIPWM_H

/*
===============================================================================
*/
/**
  @file DDIPWM.h

  Public interface include file for accessing the PWM DAL device driver.

  The DDIPWM.h file is the public API interface to the PWM (Pulse Width
  Modulation) Device Access Library (DAL).

  Depending on the chipset, there can be multiple PWM frames, with each frame
  having multiple programmable PWM instances.
  Each PWM frame is a separate DAL string device.
  The name of the device is "PWM_n", when "n" is the frame number, indexed
  from 0.
  The client needs to attach to the appropriate DAL string device before
  invoking the DAL APIs published here.
*/
/*
  =============================================================================

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR

  =============================================================================
  $Header: //components/rel/core.tx/6.0/api/systemdrivers/DDIPWM.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $
  =============================================================================
*/

/*=============================================================================
      Include Files
=============================================================================*/

#include "DalDevice.h"
#include "DALStdErr.h"
#include "com_dtypes.h"
#include "PWMDefs.h"

/*=============================================================================
      Type Definitions
=============================================================================*/

/**
  Defines the interface version.
 */
#define DALPWM_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

/*=============================================================================
      Interface Definitions - Internally used
=============================================================================*/

/** @cond */

typedef struct DalPWM DalPWM;
struct DalPWM
{
  struct DalDevice DalDevice;
  DALResult (*GetPWMId)          (DalDeviceHandle *_h, PWMInstanceType eInst,
                                  DalPWMIdType *pId);
  DALResult (*ReleasePWMId)      (DalDeviceHandle *_h, DalPWMIdType nId);
  DALResult (*Enable)            (DalDeviceHandle *_h, DalPWMIdType nId,
                                  boolean bEnable);
  DALResult (*SetFrequency)      (DalDeviceHandle *_h, DalPWMIdType nId,
                                  uint32 nFreqHz);
  DALResult (*SetDutyCycle)      (DalDeviceHandle *_h, DalPWMIdType nId,
                                  uint32 nDuty);
  DALResult (*GetClockFrequency) (DalDeviceHandle *_h, DalPWMIdType nId,
                                  uint32 *pnFreqHz);
};

typedef struct DalPWMHandle DalPWMHandle;
struct DalPWMHandle
{
  uint32        dwDalHandleId;
  const DalPWM *pVtbl;
  void         *pClientCtxt;
};

/** @endcond */

/*=============================================================================
      Interface Declarations
=============================================================================*/


/* ============================================================================
**  Function : DalPWM_GetPWMId
** ==========================================================================*/
/**
  Retrieves a access ID corresponding to the input PWM instance.

  The input PWM instance should be valid for the target and should not be
  already used by some other client.
  This API that needs to be invoked before any other DAL PWM API.

  @param *_h         [in] -- Handle to the PWM DAL device
  @param  eInst      [in] -- PWM instance (enumeration in PWMDefs.h)
  @param *pId       [out] -- Pointer to fill in with the ID

  @return
  DAL_SUCCESS -- The PWM ID was found\n
  DAL_ERROR   -- The PWM ID was not found

  @dependencies
  None.

  @sa
  DalPWM_Enable, DalPWM_SetFrequency, DalPWM_SetDutyCycle, DalReleasePWMId
*/

static __inline DALResult DalPWM_GetPWMId
(
  DalDeviceHandle *_h,
  PWMInstanceType  eInst,
  DalPWMIdType    *pId
)
{
  return ((DalPWMHandle *)_h)->pVtbl->GetPWMId(_h, eInst, pId);
}


/* ============================================================================
**  Function : DalPWM_ReleasePWMId
** ==========================================================================*/
/**
  Releases the input PWM ID.

  This API releases a lock which is automatically applied when the PWM ID is
  acquired. The lock can only be released by the owner of PWM ID.

  @param *_h         [in] -- Handle to the PWM DAL device
  @param  nId       [out] -- PWM ID to be released

  @return
  DAL_SUCCESS -- The lock was released.
  DAL_ERROR   -- The PWM ID is not allowed to unlock this PWM instance.

  @dependencies
  None.

  @sa
  DalPWM_GetPWMId
*/

static __inline DALResult DalPWM_ReleasePWMId
(
  DalDeviceHandle *_h,
  DalPWMIdType     nId
)
{
  return ((DalPWMHandle *)_h)->pVtbl->ReleasePWMId(_h, nId);
}


/* ============================================================================
**  Function : DalPWM_Enable
** ==========================================================================*/
/**
  Enables the clock(s) required for the requested PWM ID.
  The ID should be held by the client requesting to enable the PWM instance.

  @param *_h         [in] -- Handle to the PWM DAL device
  @param  nId        [in] -- PWM instance's ID
  @param  bEnable    [in] -- TRUE to enable and FALSE to disable the PWM
                             instance

  @return
  DAL_SUCCESS -- Resources for PWM instance were enabled/disabled\n
  DAL_ERROR   -- Resources for PWM instance were not enabled/disabled

  @dependencies
  None.

  @sa
  DalPWM_GetPWMId
*/

static __inline DALResult DalPWM_Enable
(
  DalDeviceHandle *_h,
  DalPWMIdType     nId,
  boolean          bEnable
)
{
  return ((DalPWMHandle *)_h)->pVtbl->Enable(_h, nId, bEnable);
}

/* ============================================================================
**  Function : DalPWM_SetFrequency
** ==========================================================================*/
/**
  Configures the frequency (period) of the requested PWM ID's signal.
  NOTE: The frequency can only be set once owing to H/W limitations.
        This API needs to be called before setting the duty cycle.

  The PWM output frequency range (outlined below) is determined by the PWM core
  clock frequency. This can be statically configured through device config XML.

  Clients can invoke DalPWM_GetClockFrequency to obtain the frequency and check
  if it meets their requirements.

  Min PWM output frequency is (PWM clock frequency) / 2^16.
  Max PWM output frequency is (PWM clock frequency) / 2.

  For example, if the PWM clock frequency is 19.2 Mhz, then:
  Min 292 Hz (19.2 MHz/2^16), max 9.6 MHz (4.8/2)

  @param *_h            [in] -- Handle to the PWM DAL device
  @param  nId           [in] -- PWM instance's ID
  @param  nFreqHz       [in] -- Frequency of the PWM signal in Hz

  @return
  DAL_SUCCESS                 -- PWM instance succesfully configured\n
  DAL_ERROR_INVALID_PARAMETER -- Invalid arguments\n
  DAL_ERROR_NOT_ALLOWED       -- Following scenarios could return this error:\n
                                 a) Client does not have ID for PWM instance\n
                                 b) Requested frequency is not within range\n
                                 c) Client tried to change the frequency\n
  DAL_ERROR                   -- PWM instance not configured

  @dependencies
  None.

  @sa
  DalPWM_GetPWMId
*/

static __inline DALResult DalPWM_SetFrequency
(
  DalDeviceHandle *_h,
  DalPWMIdType     nId,
  uint32           nFreqHz
)
{
  return ((DalPWMHandle *)_h)->pVtbl->SetFrequency(_h, nId, nFreqHz);
}

/* ============================================================================
**  Function : DalPWM_SetDutyCycle
** ==========================================================================*/
/**
  Configures the duty cycle (active period) of the requested PWM ID's signal.

  NOTE: The duty cycle can only be set to quantized levels.
        The specific levels depend upon the PWM output frequency and the PWM
        core clock frequency.
        Clients are responsible to check this and request the duty cycle
        accordingly.
        If the step size is not an integer, then use the rounded up integer
        duty cycle.

        For example, if step size is 6.25%, then 12.5%, 18.75% can be supported.
        Client needs to pass 19% as the parameter for 18.75%.
        Client needs to pass 13% as the parameter for 12.5%.
        Client needs to pass 7% as the parameter for 6.25%.

        If client passes a duty cycle that cannot be achieved, then the duty
        cycle will be set to the max achievable duty cycle lesser than the
        requested one.
        As per above example, if 21% is passed, then 18.75% would be set.

  Duty cycle step size in %:
  100 * (PWM output frequency)/(PWM clock frequency)

  For example, if PWM core clock frequency is 19.2 MHz and output is 1.92 MHz,
  then the duty cycle step size will be:
  (1.92/19.2) * 100 = 10%
  Hence, only duty cycles of 10%, 20%, 30% and so on can be obtained.
  In this example, for 1% step size, the user has to set the output frequency
  to 48 KHz or lower.

  The PWM clock is running at the frequency specified device config XML.
  This can be obtained from DalPWM_GetClockFrequency.

  @param *_h            [in] -- Handle to the PWM DAL device
  @param  nId           [in] -- PWM instance's ID
  @param  nDuty         [in] -- Duty cycle of the PWM signal in %
                                Min % is 1 and max % is 99

  @return
  DAL_SUCCESS                 -- PWM instance succesfully configured\n
  DAL_ERROR_INVALID_PARAMETER -- Invalid arguments (duty cycle not within\n
                                 above range)\n
  DAL_ERROR_NOT_ALLOWED       -- Following scenarios could return this error:\n
                                 a) Client does not have ID for PWM instance\n
                                 b) Frequency has not been set\n
                                 c) Requested duty cycle is lesser than the\n
                                    least achievable one
  DAL_ERROR                   -- PWM instance not configured

  @dependencies
  None.

  @sa
  DalPWM_GetPWMId
*/

static __inline DALResult DalPWM_SetDutyCycle
(
  DalDeviceHandle *_h,
  DalPWMIdType     nId,
  uint32           nDuty
)
{
  return ((DalPWMHandle *)_h)->pVtbl->SetDutyCycle(_h, nId, nDuty);
}

/* ============================================================================
**  Function : DalPWM_GetClockFrequency
** ==========================================================================*/
/**
  Query the PWM core clock frequency so that the frequency range and duty cycle
  step size can be calculated.
  The frequency is configurable through the device config XML.

  @param *_h            [in] -- Handle to the PWM DAL device
  @param  nId           [in] -- PWM instance's ID
  @param  pnFreqHz     [out] -- Pointer to fill in with the frequency. A value
                                of zero indicates that the frequency could not
                                be determined.

  @return
  DAL_SUCCESS                 -- The frequency was determined and the pointer
                                 filled in (see function description). \n
  DAL_ERROR_INVALID_PARAMETER -- Invalid arguments\n
  DAL_ERROR                   -- The frequency could not be determined.

  @dependencies
  None.

  @sa
  DalPWM_GetPWMId
*/
static __inline DALResult DalPWM_GetClockFrequency
(
  DalDeviceHandle *_h,
  DalPWMIdType     nId,
  uint32          *pnFreqHz
)
{
  return ((DalPWMHandle *)_h)->pVtbl->GetClockFrequency(_h, nId, pnFreqHz);
}

/**
  @}
*/ /* end_group Functions */

#endif /* DDIPWM_H */
