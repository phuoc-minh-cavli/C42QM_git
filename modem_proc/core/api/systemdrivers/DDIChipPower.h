#ifndef DDICHIPPOWER_H
#define DDICHIPPOWER_H

/**
@file DDIChipPower.h
@brief
Public interface file for accessing the ChipPower DAL.
This is only supported for the requirements of modem page-only image.
*/

/*=============================================================================
                             Edit History

  $Header: //components/rel/core.mpss/3.10/api/systemdrivers/DDIChipPower.h#1 $
  $DateTime: 2019/02/18 01:53:06 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     ------------------------------------------------------------
  01/25/19   shm     Ported from TX image to support modem page-only image

===============================================================================
  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR
=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "DALSys.h"
#include "DALDeviceId.h"


/*=========================================================================
      Type Definitions
==========================================================================*/

/**
  @addtogroup chip_power Chip Power
  @{
*/
typedef void * ChipPowerCallbackFuncCtx;
typedef void * (*ChipPowerCallbackFunc)(void*, uint32, void*, uint32);
typedef struct
ChipPowerEventObj
{
  ChipPowerCallbackFuncCtx cbFuncCtx;
  ChipPowerCallbackFunc    cbFunc;
} ChipPowerEventObj;


typedef enum
{
  DALCHIPPOWER_EVENT_RESET    = 0, /**< -- Event is a reset */
  DALCHIPPOWER_EVENT_POWEROFF = 1, /**< -- Event is a power-off */
  /** @cond */
  DALCHIPPOWER_EVENT_32BITS   = 0x7FFFFFFF
  /** @endconf */
} ChipPowerEventType;


/**
  Defines the interface version.
*/
#define DALCHIPPOWER_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

/*=========================================================================
      DAL glue code
==========================================================================*/

/** @cond */
typedef struct DalChipPower DalChipPower;
struct DalChipPower
{
  struct DalDevice DalDevice;
  void      (*Reset)   (DalDeviceHandle *_h, uint32 nNotUsed);
  void      (*PowerOff)(DalDeviceHandle *_h, uint32 nNotUsed);
  DALResult (*RegisterCallback)
  (
    DalDeviceHandle         *_h,
    ChipPowerCallbackFunc    cbFunc,
    ChipPowerCallbackFuncCtx cbFuncCtx
  );
};

typedef struct DalChipPowerHandle DalChipPowerHandle;
struct DalChipPowerHandle
{
  uint32              dwDalHandleId;
  const DalChipPower *pVtbl;
  void               *pClientCtxt;
};

#define DAL_ChipPowerDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALCHIPPOWER_INTERFACE_VERSION,hDalDevice)

/** @endcond */


/*=========================================================================
      Function Definitions
==========================================================================*/

/*=========================================================================
FUNCTION  DalChipPower_Reset
===========================================================================*/
/**
  Resets the chip.
  This should only be called in STM and after global interrupts are disabled.

  @param[in] _h   Handle to the ChipPower DAL device.

  @return
  Does not return.

  @dependencies
  None.
*/
static __inline DALResult
DalChipPower_Reset(DalDeviceHandle *_h)
{
  ((DalChipPowerHandle *)_h)->pVtbl->Reset(_h, 0);

  /* shouldn't get here */
  return DAL_ERROR;
}


/*=========================================================================
FUNCTION  DalChipPower_PowerOff
===========================================================================*/
/**
  Powers off the chip.
  This should only be called in STM and after global interrupts are disabled.

  @param[in] _h   Handle to the ChipPower DAL device.
  
  @return
  Does not return.

  @dependencies
  None.
*/
static __inline DALResult
DalChipPower_PowerOff(DalDeviceHandle *_h)
{
  ((DalChipPowerHandle *)_h)->pVtbl->PowerOff(_h, 0);

  /* shouldn't get here */
  return DAL_ERROR;
}


/*=========================================================================
FUNCTION  DalChipPower_RegisterCallback
===========================================================================*/
/**
  Registers a callback event.

  Upon a reset or power-off, the ChipPower driver will perform all callbacks
  registered through this function. This allows drivers to perform any necessary
  cleanup before a reset or power-off occurs.
  This API uses mutex locks and should not be called in STM.

  @param[in] _h         Handle to the ChipPower DAL device.
  @param[in] cbFunc     Callback function to register. 
  @param[in] funcCtxt   Pointer to driver context for the callback.

  @return
  DAL_SUCCESS -- Callback was successfully registered. \n
  DAL_ERROR   -- Registration unsuccessful. 

  @dependencies
  None.
*/
static __inline DALResult
DalChipPower_RegisterCallback
(
  DalDeviceHandle         *_h,
  ChipPowerCallbackFunc    cbFunc,
  ChipPowerCallbackFuncCtx funcCtxt
)
{
  return ((DalChipPowerHandle *)_h)->pVtbl
    ->RegisterCallback(_h, cbFunc, funcCtxt);
}

/**
  @}
*/

#endif
