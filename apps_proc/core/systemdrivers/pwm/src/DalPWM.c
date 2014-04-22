/*=============================================================================

  M A I N   D A L   P W M   D R I V E R   F I L E

  DESCRIPTION

  Contains the majority of functionality for the DAL PWM driver. The APIs in
  this module MUST always remain target independent.


  =============================================================================
  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR
  ===========================================================================*/

/*=============================================================================

  EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/systemdrivers/pwm/src/DalPWM.c#1 $
  $Author: pwbldsvc $
  $DateTime: 2020/01/30 22:49:35 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  06/13/19   shm     Interchanged CTL1 & CTL2 programming for proper duty cycle
  02/28/19   shm     Initial version
  ===========================================================================*/

/*=============================================================================

  INCLUDE FILES FOR MODULE

  ===========================================================================*/


#include <DALStdDef.h>
#include <DDIPWM.h>
#include <stdlib.h>

#include "HALpwm.h"
#include "DalPWM.h"

#include <DDIHWIO.h>


/*==========================================================================

  LOCAL TYPE DEFINES FOR THIS MODULE

  ==========================================================================*/

typedef struct
{
  uint32 nPWMKey;

} DALPWMSecAccessType;

uint16 gnPMWAdder = 0;

/*
 * Helper macro to set the PWM ID
 *
 * First 4 bits are left free to encode the PWM frame for QAPI GetID API
 * This cannot be used in the DAL framework since each frame has its own
 * DAL device context.
 */
#define DAL_PWM_SET_KEY(nPWMInst) (nPWMInst << 4 | ((++gnPMWAdder) << 16))

#define DAL_PWM_GET_INST(nPWMId) ((nPWMId & 0x0000FFF0) >> 4)

#define XO_FREQ_HZ       19200000

#define MIN_DUTY_PERCENT 1
#define MAX_DUTY_PERCENT 99

/*==========================================================================

  LOCAL DATA FOR THIS MODULE

  ==========================================================================*/

static uint32 gnTotalPWM;

/*
 * ganPWMIdUsers array keeps track of which PWM instances are currently in use
 * and ensures they are not overwritten during this time. When a PWM instance
 * is available its entry in this array will be marked by a '0'; otherwise, it
 * will contain the PWM ID value of the current users.
 */
static DALPWMSecAccessType *ganPWMIdUsers;


/*=============================================================================

  GENERIC DAL FUNCTIONS NOT CURRENTLY IMPLEMENTED IN THIS MODULE

  ===========================================================================*/

DALResult PWM_DriverInit
(
  PWMDrvCtxt *pDrvCtxt
)
{
  return DAL_SUCCESS;
}

DALResult PWM_DriverDeInit
(
  PWMDrvCtxt *pDrvCtxt
)
{
  return DAL_SUCCESS;
}

/*=============================================================================

  GENERIC DAL FUNCTIONS IMPLEMENTED IN THIS MODULE

  ===========================================================================*/

DALResult PWM_DeviceInit
(
  PWMClientCtxt *pCtxt
)
{
  DALResult              nResult;
  uint32                 nClkDiv;
  DALSYSPropertyVar      tPropVar;
  boolean                bIsClockOn;
  PWMDevCtxt            *pDevCtxt = pCtxt->pDevCtxt;

  HAL_pwm_ContextType    Target;
  HAL_pwm_TargetDataType TargetData;

  /*
   * ULog initialization
   */
  if(DAL_SUCCESS == ULogFront_RealTimeInit(
                      &pCtxt->hPWMLog, "PWM Log",
                      PWM_LOG_SIZE,
                      ULOG_MEMORY_LOCAL, ULOG_LOCK_OS))
  {
    ULogCore_HeaderSet(
      pCtxt->hPWMLog,
      "Content-Type: text/tagged-log-1.0;");
  }

  /*
   * Initialize the synchronization
   */
  pDevCtxt->hSyncObj = (qurt_mutex_t*)malloc(sizeof(qurt_mutex_t));
  if (pDevCtxt->hSyncObj == NULL)
  {
    ERR_FATAL(
      "PWM_DeviceInit: Failed to allocate memory to synchronization object",
      0, 0, 0);
  }
  qurt_mutex_init(pDevCtxt->hSyncObj);

  /*
   * Ensure synchronization for critical initialization parameters.
   */
  qurt_mutex_lock(pDevCtxt->hSyncObj);

  /*
   * Attempt to retrieve a handle to the PWM properties file.
   */
  nResult = DALSYS_GetDALPropertyHandleStr(pDevCtxt->szDevName, pDevCtxt->hProp);

  if (nResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "PWM_DeviceInit: Failed to retrieve properties handle.", 0, 0, 0);

    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR;
  }

 /*
  * Attempt to retrieve the total supported PWM instances for the current
  * target.
  */
  nResult = DALSYS_GetPropertyValue(
              pDevCtxt->hProp, "PWM_TOTAL_INSTANCES", 0, &tPropVar);
  if (nResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "PWM_DeviceInit: Failed to retrieve number of PWM instances." ,0, 0, 0);

    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR;
  }

  pDevCtxt->nTotalPWMInst = gnTotalPWM = (uint32)tPropVar.Val.dwVal;

 /*
  * Allocate an array for PWM user tracking.
  */
  ganPWMIdUsers = (DALPWMSecAccessType*)malloc((gnTotalPWM * sizeof(DALPWMSecAccessType)));
  if (ganPWMIdUsers == NULL)
  {
    ERR_FATAL(
      "PWM_DeviceInit: Unable to create global array for PWM IDs.", 0, 0, 0);

    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR;
  }

  memset((void *)ganPWMIdUsers, NULL, gnTotalPWM * sizeof(DALPWMSecAccessType));

 /*
  * Allocate array for PWM state tracking.
  */
  pDevCtxt->pState = (PWMDataType*)malloc((gnTotalPWM * sizeof(PWMDataType)));
  if (pDevCtxt->pState == NULL)
  {
    ERR_FATAL(
      "PWM_DeviceInit: Unable to create array for PWM state array.", 0, 0, 0);

    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR;
  }

  memset((void *)pDevCtxt->pState, NULL, gnTotalPWM * sizeof(PWMDataType));

 /*
  * Attempt to retrieve the base address for PWM and invoke HWIO to get the
  * virtual address.
  */
  nResult = DALSYS_GetPropertyValue(pDevCtxt->hProp, "PWM_BASE", 0, &tPropVar);
  if (nResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "PWM_DeviceInit: Failed to retrieve the base address.", 0, 0, 0);

    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR;
  }
  else
  {
    pDevCtxt->nPhyAddr = (uint32)tPropVar.Val.dwVal;
  }

  nResult = DALSYS_GetPropertyValue(pDevCtxt->hProp, "PWM_OFFSET", 0, &tPropVar);
  if (nResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "PWM_DeviceInit: Failed to retrieve the offset.", 0, 0, 0);

    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR;
  }
  else
  {
    pDevCtxt->nPhyAddr += (uint32)tPropVar.Val.dwVal;
  }

  nResult = PWM_AttachToDals(pDevCtxt);
  if (nResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "PWM_DeviceInit: Failed to attach to DAL devices.", 0, 0, 0);

    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR;
  }

  nResult = DalHWIO_MapRegionByAddress(
              pDevCtxt->phHWIO,
              (uint8 *)pDevCtxt->nPhyAddr,
              (uint8 **)&pDevCtxt->nVirtAddr);

  if (nResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "PWM_DeviceInit: Failed to map to virtual address.", 0, 0, 0);

    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR;
  }

  /*
   * Initialize the HAL layer
   */
  Target.pTargetData = &TargetData;
  HAL_pwm_Init(&Target);

  /*
   * Get the clock IDs for AHB and core clocks
   */
  nResult = DALSYS_GetPropertyValue(
              pDevCtxt->hProp, "PWM_AHB_CLK", 0, &tPropVar);
  if (nResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "PWM_DeviceInit: Failed to retrieve AHB clock name.", 0, 0, 0);

    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR;
  }

  nResult = DalClock_GetClockId(
              pDevCtxt->phClock, (char*)tPropVar.Val.pszVal,
              &pDevCtxt->nAHBClkId);
  if (nResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "PWM_DeviceInit: Failed to get ID of AHB clock.", 0, 0, 0);

    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR;
  }

  nResult = DALSYS_GetPropertyValue(
              pDevCtxt->hProp, "PWM_CLK", 0, &tPropVar);
  if (nResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "PWM_DeviceInit: Failed to retrieve PWM core clock name.", 0, 0, 0);

    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR;
  }

  nResult = DalClock_GetClockId(
              pDevCtxt->phClock, (char*)tPropVar.Val.pszVal,
              &pDevCtxt->nCoreClkId);
  if (nResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "PWM_DeviceInit: Failed to get ID of PWM core clock.", 0, 0, 0);

    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR;
  }

  /*
   * De-assert the block reset for PWM clocks.
   * This is done here as its a safe step whether or not a PWM instance is
   * actually enabled or not.
   * Reset will be asserted during DAL device de-initialization.
   */
  nResult = DalClock_ResetClock(
              pDevCtxt->phClock, pDevCtxt->nAHBClkId,
              CLOCK_RESET_DEASSERT);
  if (nResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "PWM_DeviceInit: Failed to de-assert the block reset.", 0, 0, 0);

    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR;
  }

  /*
   * Enable the AHB clock to access PWM registers and then check if it was
   * turned on.
   * This is done here since clients could potentially call any PWM API
   * (SetFrequency/SetDutyCycle & Enable) first. It is not possible to enable
   * this clock in both functions.
   * The AHB clock will be disabled during DAL device de-initialization.
   */
  nResult = DalClock_EnableClock(
              pDevCtxt->phClock, pDevCtxt->nAHBClkId);
  if (nResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "PWM_DeviceInit: Failed to enable the AHB clock.", 0, 0, 0);

    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR;
  }

  bIsClockOn = DalClock_IsClockOn(
                 pDevCtxt->phClock, pDevCtxt->nAHBClkId);
  if (bIsClockOn == FALSE)
  {
    ERR_FATAL(
      "PWM_DeviceInit: AHB clock is not ON.", 0, 0, 0);

    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR;
  }

  /*
   * Set the PWM core clock divider after querying it from device config XML
   */
  nResult = DALSYS_GetPropertyValue(
              pDevCtxt->hProp, "PWM_CLK_DIV", 0, &tPropVar);
  if (nResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "PWM_DeviceInit: Failed to retrieve PWM core clock divider.", 0, 0, 0);

    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR;
  }

  nClkDiv = (uint32)tPropVar.Val.dwVal;

  nResult = DalClock_SetClockDivider(
              pDevCtxt->phClock, pDevCtxt->nCoreClkId, nClkDiv);
  if (nResult != DAL_SUCCESS)
  {
    ERR_FATAL(
      "PWM_DeviceInit: Failed to set the PWM core clock frequency to \
       %d Hz.", (uint32)tPropVar.Val.dwVal, 0, 0);

    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR;
  }

  pDevCtxt->nCoreFreqHz = XO_FREQ_HZ / nClkDiv;
  qurt_mutex_unlock(pDevCtxt->hSyncObj);

  return DAL_SUCCESS;
}

DALResult PWM_DeviceDeInit
(
  PWMClientCtxt *pCtxt
)
{
  DALResult   nResult;
  PWMDevCtxt *pDevCtxt = pCtxt->pDevCtxt;

  /*
   * Disable the AHB clock.
   * Cannot check for the clock to be OFF as there might be other active PWM
   * instances which are keeping the clocks on.
   */
  nResult = DalClock_DisableClock(pDevCtxt->phClock, pDevCtxt->nAHBClkId);
  if (nResult != DAL_SUCCESS)
  {
    ULOG_RT_PRINTF_0(
      pCtxt->hPWMLog,
      "PWM_DeviceDeInit: Disabling the AHB clock failed.");
  }

  free(pDevCtxt->pState);
  pDevCtxt->pState = NULL;

  free(ganPWMIdUsers);
  ganPWMIdUsers = NULL;

  qurt_mutex_destroy(pDevCtxt->hSyncObj);
  free(pDevCtxt->hSyncObj);
  pDevCtxt->hSyncObj = NULL;

  return DAL_SUCCESS;
}

/*=============================================================================

  FUNCTION      PWM_AttachToDals

  DESCRIPTION   See DalPWM.h

=============================================================================*/

DALResult PWM_AttachToDals
(
  PWMDevCtxt *pDevCtxt
)
{
  /* Attach to the clock DAL device */
  if (DAL_SUCCESS != DAL_DeviceAttach(
                       DALDEVICEID_CLOCK, &pDevCtxt->phClock))
  {
    return DAL_ERROR;
  }

  if (DAL_SUCCESS != DAL_DeviceAttach(
                       DALDEVICEID_HWIO, &pDevCtxt->phHWIO))
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;

} /* END PWM_AttachToDals */


/*=============================================================================

  APPLICATION PROGRAMMING INTERFACE

  ===========================================================================*/

DALResult PWM_GetPWMId
(
  PWMClientCtxt   *pCtxt,
  PWMInstanceType  eInst,
  DalPWMIdType    *pId
)
{
  /*
   * Sanity checks
   */
  if (pId == NULL || eInst >= PWM_NUM_INSTANCES)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  if (eInst >= gnTotalPWM)
  {
    return DAL_ERROR_NOT_SUPPORTED;
  }

  qurt_mutex_lock(pCtxt->pDevCtxt->hSyncObj);

  /*
   * Check if the PWM instance is already in use
   */
  if (ganPWMIdUsers[eInst].nPWMKey == 0)
  {
    ganPWMIdUsers[eInst].nPWMKey = DAL_PWM_SET_KEY(eInst);
    *pId = ganPWMIdUsers[eInst].nPWMKey;

    qurt_mutex_unlock(pCtxt->pDevCtxt->hSyncObj);
    return DAL_SUCCESS;
  }
  else
  {
    *pId = NULL;
    ULOG_RT_PRINTF_1(
      pCtxt->hPWMLog,
      "PWM_GetPWMId: PWM ID for instance %d is already allocated to another \
      client.", eInst);
  }

  qurt_mutex_unlock(pCtxt->pDevCtxt->hSyncObj);

  return DAL_ERROR;
}

DALResult PWM_ReleasePWMId
(
  PWMClientCtxt   *pCtxt,
  DalPWMIdType     nId
)
{
  PWMInstanceType eInst = (PWMInstanceType)DAL_PWM_GET_INST(nId);

  /*
   * Sanity check
   */
  if (nId == NULL)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  qurt_mutex_lock(pCtxt->pDevCtxt->hSyncObj);

  if (eInst < gnTotalPWM)
  {
    /*
     * Only the holder of the PWM ID for this PWM instance can
     * unlock access to it for others to use.
     */
    if (ganPWMIdUsers[eInst].nPWMKey == nId)
    {
      ganPWMIdUsers[eInst].nPWMKey = NULL;
      qurt_mutex_unlock(pCtxt->pDevCtxt->hSyncObj);
      return DAL_SUCCESS;
    }
  }

  qurt_mutex_unlock(pCtxt->pDevCtxt->hSyncObj);
  return DAL_ERROR;

} /* PWM_ReleasePWMId */

DALResult PWM_Enable
(
  PWMClientCtxt   *pCtxt,
  DalPWMIdType     nId,
  boolean          bEnable
)
{
  DALResult       nResult;
  boolean         bIsClockOn;
  PWMDevCtxt     *pDevCtxt  = pCtxt->pDevCtxt;
  PWMDataType    *pPWMState = pCtxt->pDevCtxt->pState;
  PWMInstanceType eInst     = (PWMInstanceType)DAL_PWM_GET_INST(nId);

  if (eInst >= gnTotalPWM)
  {
    return DAL_ERROR_INVALID_PARAMETER;
  }

  qurt_mutex_lock(pDevCtxt->hSyncObj);

  /*
   * Ensure that only the holder of the PWM instance's ID can operate on it.
   */
  if (nId != ganPWMIdUsers[eInst].nPWMKey)
  {
    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR_NOT_ALLOWED;
  }

  /*
   * Short-circuit if the PWM instance is already at the state which the client
   * has requested for.
   */
  if (pPWMState[eInst].bIsEnabled == bEnable)
  {
    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_SUCCESS;
  }

  if (bEnable == TRUE)
  {
    /*
     * Enable the clocks and check if they are turned ON
     */
    nResult = DalClock_EnableClock(pDevCtxt->phClock, pDevCtxt->nCoreClkId);
    if (nResult != DAL_SUCCESS)
    {
      ULOG_RT_PRINTF_1(
        pCtxt->hPWMLog,
        "PWM_Enable (Instance %d): Enabling the PWM core clock failed.", eInst);

      qurt_mutex_unlock(pDevCtxt->hSyncObj);
      return nResult;
    }

    bIsClockOn = DalClock_IsClockOn(
                   pDevCtxt->phClock, pDevCtxt->nCoreClkId);
    if (bIsClockOn == FALSE)
    {
      ULOG_RT_PRINTF_1(
        pCtxt->hPWMLog,
        "PWM_Enable (Instance %d): PWM core clock is not ON.", eInst);

      qurt_mutex_unlock(pDevCtxt->hSyncObj);
      return DAL_ERROR_INTERNAL;
    }

    /*
     * Enable the PWM instance
     */
    HAL_pwm_Enable(pDevCtxt->nVirtAddr, (uint32)eInst);
  }
  else
  {
    /*
     * Disable the PWM instance
     */
    HAL_pwm_Disable(pDevCtxt->nVirtAddr, (uint32)eInst);

    nResult = DalClock_DisableClock(pDevCtxt->phClock, pDevCtxt->nCoreClkId);
    if (nResult != DAL_SUCCESS)
    {
      ULOG_RT_PRINTF_1(
        pCtxt->hPWMLog,
        "PWM_Enable (Instance %d): Disabling the PWM core clock failed.", eInst);

      qurt_mutex_unlock(pDevCtxt->hSyncObj);
      return nResult;
    }
  }

  pPWMState[eInst].bIsEnabled = bEnable;

  qurt_mutex_unlock(pDevCtxt->hSyncObj);
  return DAL_SUCCESS;
}

DALResult PWM_SetFrequency
(
  PWMClientCtxt   *pCtxt,
  DalPWMIdType     nId,
  uint32           nFreqHz
)
{
  uint16              nCycCfg;
  PWMDevCtxt         *pDevCtxt  = pCtxt->pDevCtxt;
  PWMDataType        *pPWMState = pCtxt->pDevCtxt->pState;
  PWMInstanceType     eInst     = (PWMInstanceType)DAL_PWM_GET_INST(nId);

  /*
   * Sanity checks
   */
  if (eInst >= gnTotalPWM)
  {
    ULOG_RT_PRINTF_1(
      pCtxt->hPWMLog,
      "PWM_SetFrequency: Invalid instance %d requetsed.", eInst);
    return DAL_ERROR_INVALID_PARAMETER;
  }

  qurt_mutex_lock(pDevCtxt->hSyncObj);

  /*
   * Ensure that only the holder of the PWM instance's ID can operate on it.
   */
  if (nId != ganPWMIdUsers[eInst].nPWMKey)
  {
    ULOG_RT_PRINTF_1(
      pCtxt->hPWMLog,
      "PWM_SetFrequency: Instance %d ID locked by another client.", eInst);
    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR_NOT_ALLOWED;
  }

  /*
   * Ensure that the requested frequency is within stipulated range
   */
  if (nFreqHz > pDevCtxt->nCoreFreqHz >> 1 ||
      nFreqHz < pDevCtxt->nCoreFreqHz >> HAL_pwm_GetCounterWidth())
  {
    ULOG_RT_PRINTF_2(
      pCtxt->hPWMLog,
      "PWM_SetFrequency: Invalid frequency %d requested for instance %d.",
      nFreqHz, eInst);
    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR_NOT_ALLOWED;
  }

  /*
   * Ensure that the user is not changing the frequency.
   * This is a limitation of PWM H/W - CYC_CFG cannot be modified to a value
   * lower than the current PERIOD_CNT value.
   * Users can only modify the duty cycle.
   */
  if (pPWMState[eInst].bIsFreqSet == TRUE)
  {
    if (nFreqHz != pPWMState[eInst].mConfig.nFreqHz)
    {
      ULOG_RT_PRINTF_1(
        pCtxt->hPWMLog,
        "PWM_SetFrequency: Frequency cannot be changed for instance %d.",
        eInst);
      qurt_mutex_unlock(pDevCtxt->hSyncObj);
      return DAL_ERROR_NOT_ALLOWED;
    }
    else
    {
      qurt_mutex_unlock(pDevCtxt->hSyncObj);
      return DAL_SUCCESS;
    }
  }

  /*
   * Calculate the CYC_CFG to be programmed
   */
  nCycCfg = (uint16)HAL_PWM_CVT_TO_CYC_CFG(nFreqHz, pDevCtxt->nCoreFreqHz);

  pPWMState[eInst].mConfig.nFreqHz    = nFreqHz;
  pPWMState[eInst].mHALConfig.nCycCfg = nCycCfg;

  HAL_pwm_WriteCycCfg(pDevCtxt->nVirtAddr, eInst, nCycCfg);

  /*
   * Trigger the UPDATE.
   * Only poll for the UPDATE to clear if PWM instance is enabled through the
   * driver.
   */
  HAL_pwm_Update(
    pDevCtxt->nVirtAddr, eInst, pPWMState[eInst].bIsEnabled);

  pPWMState[eInst].bIsFreqSet = TRUE;

  qurt_mutex_unlock(pDevCtxt->hSyncObj);

  return DAL_SUCCESS;
}

DALResult PWM_SetDutyCycle
(
  PWMClientCtxt   *pCtxt,
  DalPWMIdType     nId,
  uint32           nDuty
)
{
  uint64              nCtl1, nCtl2;
  PWMDevCtxt         *pDevCtxt  = pCtxt->pDevCtxt;
  PWMDataType        *pPWMState = pCtxt->pDevCtxt->pState;
  PWMInstanceType     eInst     = (PWMInstanceType)DAL_PWM_GET_INST(nId);

  /*
   * Sanity checks
   */
  if (eInst >= gnTotalPWM      ||
      nDuty < MIN_DUTY_PERCENT ||
      nDuty > MAX_DUTY_PERCENT)
  {
    ULOG_RT_PRINTF_2(
      pCtxt->hPWMLog,
      "PWM_SetDutyCycle: Invalid duty %d or invalid instance %d.",
      nDuty, eInst);
    return DAL_ERROR_INVALID_PARAMETER;
  }

  qurt_mutex_lock(pDevCtxt->hSyncObj);

  /*
   * Ensure that only the holder of the PWM instance's ID can operate on it.
   */
  if (nId != ganPWMIdUsers[eInst].nPWMKey)
  {
    ULOG_RT_PRINTF_1(
      pCtxt->hPWMLog,
      "PWM_SetDutyCycle: Instance %d ID locked by another client.", eInst);
    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR_NOT_ALLOWED;
  }

  /*
   * Ensure that the PWM instance's CYC_CFG is configured
   */
  if (pPWMState[eInst].bIsFreqSet != TRUE)
  {
    ULOG_RT_PRINTF_1(
      pCtxt->hPWMLog,
      "PWM_SetDutyCycle: Frequency not set for instance %d.", eInst);
    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR_NOT_ALLOWED;
  }

  /*
   * Calculate the CTL1 to be programmed
   * CTL2 is always set to 0 for convenience
   */
  nCtl2 = 0;
  nCtl1 = HAL_PWM_CVT_TO_CTL1(
            nDuty, pPWMState[eInst].mConfig.nFreqHz, pDevCtxt->nCoreFreqHz);

  if (nCtl1 == 0)
  {
    ULOG_RT_PRINTF_2(
      pCtxt->hPWMLog,
      "PWM_SetDutyCycle: Duty cycle %d requested is lesser than the least \
      possible duty cycle for instance.", nDuty, eInst);
    qurt_mutex_unlock(pDevCtxt->hSyncObj);
    return DAL_ERROR_NOT_ALLOWED;
  }

  pPWMState[eInst].mConfig.nDuty    = nDuty;
  pPWMState[eInst].mHALConfig.nCtl1 = nCtl1;
  pPWMState[eInst].mHALConfig.nCtl2 = nCtl2;

  HAL_pwm_WriteDutyCtlRegs(pDevCtxt->nVirtAddr, eInst, nCtl1, nCtl2);

  /*
   * Trigger the UPDATE.
   * Only poll for the UPDATE to clear if PWM instance is enabled through the
   * driver.
   */
  HAL_pwm_Update(
    pDevCtxt->nVirtAddr, eInst, pPWMState[eInst].bIsEnabled);

  qurt_mutex_unlock(pDevCtxt->hSyncObj);

  return DAL_SUCCESS;
}

DALResult PWM_GetClockFrequency
(
  PWMClientCtxt   *pCtxt,
  DalPWMIdType     nId,
  uint32          *pnFreqHz
)
{
  PWMInstanceType eInst = (PWMInstanceType)DAL_PWM_GET_INST(nId);

  /*
   * Sanity checks
   */
  if (eInst >= gnTotalPWM || pnFreqHz == NULL)
  {
    ULOG_RT_PRINTF_1(
      pCtxt->hPWMLog,
      "PWM_GetClockFrequency: Invalid instance %d or pointer to frequency NULL.",
      eInst);
    return DAL_ERROR_INVALID_PARAMETER;
  }

  *pnFreqHz = pCtxt->pDevCtxt->nCoreFreqHz;

  return DAL_SUCCESS;
}
