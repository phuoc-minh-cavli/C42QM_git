#ifndef __DALPWM_H__
#define __DALPWM_H__

/*===========================================================================

                     I N T E R N A L   D A L   P W M
                          H E A D E R   F I L E

DESCRIPTION
  This file contains definitions for structures used by the DAL PWM

REFERENCES

===========================================================================
  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/systemdrivers/pwm/src/DalPWM.h#1 $
  $Author: pwbldsvc $
  $DateTime: 2020/01/30 22:49:35 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  02/28/19   shm     Initial version

===========================================================================*/


/*==========================================================================

                        INCLUDE FILES FOR MODULE

==========================================================================*/


#include <DALStdErr.h>
#include <DALFramework.h>
#include <DALSys.h>
#include <DALDeviceId.h>
#include "com_dtypes.h"
#include "qurt.h"
#include "err.h"
#include "ULogFront.h"

#include "HALpwm.h"
#include <DDIClock.h>

/*==========================================================================

                          EXPORTED STRUCTURES 

==========================================================================*/

#define DALPWM_MAX_STRING_ID_LENGTH 10

/*
 * Driver, device and client contexts
 */
typedef struct PWMDrvCtxt PWMDrvCtxt;
typedef struct PWMDevCtxt PWMDevCtxt;
typedef struct PWMClientCtxt PWMClientCtxt;

/*
 * Private "PWM" Vtable
 */
typedef struct PWMDALVtbl PWMDALVtbl;
struct PWMDALVtbl
{
  DALResult (*PWM_DriverInit)  (PWMDrvCtxt *);
  DALResult (*PWM_DriverDeInit)(PWMDrvCtxt *);
};


/*
 * PWM config type
 *
 *  nDuty:         Duty cycle in %
 *  nFreqHz:       Signal frequency in Hz
 */
typedef struct
{
  uint32 nDuty;
  uint32 nFreqHz;

} PWMConfigType;

/*
 * PWMDataType
 * Structure storing the details of a PWM instance
 */
typedef struct
{
  PWMConfigType      mConfig;
  HAL_pwm_ConfigType mHALConfig;
  boolean            bIsEnabled;
  boolean            bIsFreqSet;

} PWMDataType;

/*
 * PWM device context.
 */
struct PWMDevCtxt
{
  /*
   * Base Members - do not modify!
   */
  uint32           dwRefs;
  DALDEVICEID      DevId;
  uint32           dwDevCtxtRefIdx;
  PWMDrvCtxt      *pPWMDrvCtxt;
  DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
  uint32           Reserved[16];

  /* Private members */
  char             szDevName[DALPWM_MAX_STRING_ID_LENGTH];
  DalDeviceHandle *phClock;
  DalDeviceHandle *phHWIO;
  qurt_mutex_t    *hSyncObj;

  uint32           nPhyAddr;
  uint32           nVirtAddr;
  uint32           nTotalPWMInst;
  PWMDataType     *pState;

  ClockIdType      nAHBClkId;
  ClockIdType      nCoreClkId;

  uint32           nCoreFreqHz;

};


/*
 * PWM driver context.
 *
 * Data structure containing all information about an interrupt
 *
 *  PWMDALVtbl:      Base Members.
 *  dwNumDev:        The number of devices in PWM DAL driver.
 *  dwSizeDevCtxt:   Driver structure size for DALSYS.
 *  bInit:           Flag to indicate the driver has been initialized.
 *  dwRefs:          The number of clients using the driver.
 *  PWMDevCtxt:      The PWM device contexts array.
 */
struct PWMDrvCtxt
{
  PWMDALVtbl         PWMDALVtbl;
  uint32             dwNumDev;
  uint32             dwSizeDevCtxt;
  uint32             bInit;
  uint32             dwRefs;
  PWMDevCtxt         PWMDevCtxt[DALPWM_NUM_DEVICES];
};

/*
 * PWMPropertyValueType
 *
 * Generic PWM property data. Actual data type will depend on the property
 * and will not even necessarily be a pointer (i.e. could be straight uint32).
 */
typedef const void *PWMPropertyValueType;

struct PWMDALClientCtxt
{
  DalPWMHandle   DalPWMHandle;
};


/*
 * PWM client context
 *
 * Data structure containing top-level PWM access structures
 * base members.
 *
 *  dwRefs:              References used internally.
 *  dwAccessMode:        SHARED for PWM.
 *  pPortCtxt:           Handle to a port context for RPC.
 *  pPWMDevCtxt:         Handle to the PWM device context.
 *  DalPWMHandle:        Handle to the PWM configuration structures.
 */
struct PWMClientCtxt
{
  /*
   * Base members - do not modify!
   */
  uint32              dwRefs;
  uint32              dwAccessMode;
  void               *pPortCtxt;
  PWMDevCtxt         *pDevCtxt;
  DalPWMHandle        DalPWMHandle;

  /*
   * PWM client state can be added by developers here
   */
  ULogHandle          hPWMLog;
};


/*==========================================================================

                     APPLICATION PROGRAMMING INTERFACE

==========================================================================*/

/* =========================================================================
**  Function : PWM_DriverInit
** =========================================================================*/
/**
  Initialize the PWM driver.

  This function initializes the PWM driver, it is the main init entry
  point.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context

  @return
  DAL_SUCCESS -- Init was successful\n
  DAL_ERROR   -- Initialization failed

  @dependencies
  None
*/

DALResult PWM_DriverInit
(
  PWMDrvCtxt *pDrvCtxt
);

/* =========================================================================
**  Function : PWM_DriverDeInit
** =========================================================================*/
/**
  De-initialize the PWM driver.

  This function shuts-down the PWM driver. It is not expected to ever
  be called.

  @param *pDrvCtxt [in] -- Handle to the DAL driver context

  @return
  DAL_SUCCESS -- De-init was successful\n
  DAL_ERROR   -- De-init failed

  @dependencies
  None
*/

DALResult PWM_DriverDeInit
(
  PWMDrvCtxt *pDrvCtxt
);

DALResult PWM_DeviceInit  (PWMClientCtxt *);
DALResult PWM_DeviceDeInit(PWMClientCtxt *);

/*=============================================================================

  FUNCTION
    PWM_AttachToDals

  DESCRIPTION
    This is a default function that consolidates all the other DAL attaches that
    are needed. For example, PWM driver uses the functionality of clock driver.
    We need to make sure we have handles to access these.

  PARAMETERS
    PWMDevCtxt *pDevCtxt: Device context of the PWM DAL.

  DEPENDENCIES
    None

  RETURN VALUE
    DAL_SUCCESS: This indicates if all the DAL attaches were a success.
    DAL_ERROR  : Any of the DAL attaches failed.

  SIDE EFFECTS
    None

=============================================================================*/

DALResult PWM_AttachToDals
(
  PWMDevCtxt *pDevCtxt
);

DALResult PWM_GetPWMId
(
  PWMClientCtxt   *pCtxt,
  PWMInstanceType  eInst,
  DalPWMIdType    *pId
);

DALResult PWM_ReleasePWMId
(
  PWMClientCtxt   *pCtxt,
  DalPWMIdType     nId
);

DALResult PWM_Enable
(
  PWMClientCtxt   *pCtxt,
  DalPWMIdType     nId,
  boolean          bEnable
);

DALResult PWM_SetFrequency
(
  PWMClientCtxt   *pCtxt,
  DalPWMIdType     nId,
  uint32           nFreqHz
);

DALResult PWM_SetDutyCycle
(
  PWMClientCtxt   *pCtxt,
  DalPWMIdType     nId,
  uint32           nDuty
);

DALResult PWM_GetClockFrequency
(
  PWMClientCtxt   *pCtxt,
  DalPWMIdType     nId,
  uint32          *pnFreqHz
);

#endif /* __DALPWM_H__ */
