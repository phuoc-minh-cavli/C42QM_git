#ifndef HAL_HSUSB_IMPL
#define HAL_HSUSB_IMPL
/*
===========================================================================

FILE:         HAL_hsusb_impl.h

DESCRIPTION:  
This is the HAL internal interface for the HS-USB core.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/hal/src/HAL_hsusb_impl.h#1 $

===========================================================================

===========================================================================
Copyright © 2008-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "HAL_hsusb.h"
//#include "Tx_port.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/*
* HAL_HSUSB_CoreIfType
*
* There is an USB core common interface.
*
*/
typedef struct HAL_HSUSB_CoreIfType
{
  uint16                  Version;
  /* The following functions are documented in HAL_hsusb_impl.c 
  where they have a default implementation */
  void    (*Reset)(uint8 instance);
  HAL_HSUSB_ErrorType (*ReadUlpiPhy)(uint8 instance, uint8 port, uint8 reg, uint8* data);
  HAL_HSUSB_ErrorType (*WriteUlpiPhy)(uint8 instance, uint8 port, uint8 reg, uint8 data);
  HAL_HSUSB_AhbBurstMode (*GetAhbBurstMode)(uint8 instance);
  void (*SetAhbBurstMode)(uint8 instance, HAL_HSUSB_AhbBurstMode eMode);
  boolean (*GetSe0GlitchFixCtrl)(uint8 instance);
  void    (*SetSe0GlitchFixCtrl)(uint8 instance, boolean state);
  boolean (*GetFs3Wire2WireSelect)(uint8 instance);
  void    (*SetFs3Wire2WireSelect)(uint8 instance, boolean state);
  boolean (*GetXtorBypass)(uint8 instance);
  void    (*SetXtorBypass)(uint8 instance, boolean state);
  boolean (*GetPostedWrites)(uint8 instance);
  void    (*SetPostedWrites)(uint8 instance, boolean state);
  boolean (*GetGrantStolen)(uint8 instance);
  void    (*SetGrantStolen)(uint8 instance, boolean state);
  HAL_HSUSB_HprotMode (*GetHprotMode)(uint8 instance);
  void    (*SetHprotMode)(uint8 instance, HAL_HSUSB_HprotMode eMode);
  boolean (*GetIncrOverride)(uint8 instance);
  void    (*SetIncrOverride)(uint8 instance, boolean state);
  boolean (*GetAsyncBridgesBypass)(uint8 instance);
  void    (*SetAsyncBridgesBypass)(uint8 instance, boolean state);
  boolean (*GetPeDpTxFifoIdleForce)(uint8 instance);
  void    (*SetPeDpTxFifoIdleForce)(uint8 instance, boolean state); 
  HAL_HSUSB_TestBusSelectorType (*GetTestBusSelector)(uint8 instance);
  void    (*SetTestBusSelector)(uint8 instance, HAL_HSUSB_TestBusSelectorType eMode);
  boolean (*GetTxBufCollisionMode)(uint8 instance);
  void    (*SetTxBufCollisionMode)(uint8 instance, boolean state);
  boolean (*GetStreamRxBypassMode)(uint8 instance);
  void    (*SetStreamRxBypassMode)(uint8 instance, boolean state);
  boolean (*GetStreamTxAutoMode)(uint8 instance);
  void    (*SetStreamTxAutoMode)(uint8 instance, boolean state);
  boolean (*GetHostSimTimersStdMode)(uint8 instance);
  void    (*SetHostSimTimersStdMode)(uint8 instance, boolean state);
  boolean (*GetHostSimTimersSuspMode)(uint8 instance);
  void    (*SetHostSimTimersSuspMode)(uint8 instance, boolean state);
  boolean (*GetDevicePEStateReset)(uint8 instance);
  void    (*SetDevicePEStateReset)(uint8 instance, boolean state);
  boolean (*GetIsochronousBugFix)(uint8 instance);
  void    (*SetIsochronousBugFix)(uint8 instance, boolean state);
  boolean (*GetAHBBufferableDataAccess)(uint8 instance);
  void    (*SetAHBBufferableDataAccess)(uint8 instance, boolean state);
  boolean (*GetLegacyMode)(uint8 instance);
  void    (*SetLegacyMode)(uint8 instance, boolean state);
  uint8   (*GetEndpointPipeID)(uint8 instance, uint8 ep, boolean direction);
  void    (*SetEndpointPipeID)(uint8 instance, uint8 ep, boolean direction, uint8 id);
  boolean (*GetSessVldOperation)(uint8 instance);
  void    (*SetSessVldOperation)(uint8 instance, boolean state);
  boolean (*GetSessVldStatus)(uint8 instance);
  void    (*SetSessVldStatus)(uint8 instance, boolean state);
  boolean (*GetSessVldChangeStatus)(uint8 instance);
  void    (*SetSessVldChangeStatus)(uint8 instance, boolean state);
  boolean (*GetSessVldChangeInterrupt)(uint8 instance);
  void    (*SetSessVldChangeInterrupt)(uint8 instance, boolean state);
  boolean (*GetSysClkSw)(uint8 instance);
  void    (*SetSysClkSw)(uint8 instance, boolean state);
  boolean (*GetResumeEndInterrupt)(uint8 instance);
  void    (*SetResumeEndInterrupt)(uint8 instance, boolean state);
  boolean (*GetSuspendWrongOpmode)(uint8 instance);
  void    (*SetSuspendWrongOpmode)(uint8 instance, boolean state);
  boolean (*GetNoSofRxFifoFull)(uint8 instance);
  void    (*SetNoSofRxFifoFull)(uint8 instance, boolean state);
  boolean (*GetIsoInZlpPrime)(uint8 instance);
  void    (*SetIsoInZlpPrime)(uint8 instance, boolean state);
  boolean (*GetDatalinePulsingReset)(uint8 instance);
  void    (*SetDatalinePulsingReset)(uint8 instance, boolean state);
  boolean (*GetDevicePortChirpKTime)(uint8 instance);
  void    (*SetDevicePortChirpKTime)(uint8 instance, boolean state);
  boolean (*GetDevicePortPreChirpKTime)(uint8 instance);
  void    (*SetDevicePortPreChirpKTime)(uint8 instance, boolean state);
  boolean (*GetSessVldControl)(uint8 instance);
  void    (*SetSessVldControl)(uint8 instance, boolean state);

  boolean (*GetTxEpPrimeL1Exit)(uint8 instance, uint8 ep);
  void    (*SetTxEpPrimeL1Exit)(uint8 instance, uint8 ep, boolean state);
  boolean (*GetTxEpPrimeL1Enable)(uint8 instance, uint8 ep);
  void    (*SetTxEpPrimeL1Enable)(uint8 instance, uint8 ep, boolean state);
  boolean (*GetL1PllPwrDwnEnable)(uint8 instance);
  void    (*SetL1PllPwrDwnEnable)(uint8 instance, boolean state);
  boolean (*GetL1PhyLPMEnable)(uint8 instance);
  void    (*SetL1PhyLPMEnable)(uint8 instance, boolean state);
  boolean (*GetL1GateAhbClkEnable)(uint8 instance);
  void    (*SetL1GateAhbClkEnable)(uint8 instance, boolean state);
  boolean (*GetL1GateFsXcvrClkEnable)(uint8 instance);
  void    (*SetL1GateFsXcvrClkEnable)(uint8 instance, boolean state);
  boolean (*GetL1GateSysClkEnable)(uint8 instance);
  void    (*SetL1GateSysClkEnable)(uint8 instance, boolean state);
  boolean (*GetL1GateXcvrClkEnable)(uint8 instance);
  void    (*SetL1GateXcvrClkEnable)(uint8 instance, boolean state);
  boolean (*GetL1RemoteWakeupEnable)(uint8 instance);
  void    (*SetL1RemoteWakeupEnable)(uint8 instance, boolean state);
  boolean (*GetL1LPMEnable)(uint8 instance);
  void    (*SetL1LPMEnable)(uint8 instance, boolean state);
  uint8   (*GetL1PllTurnoffMinHird)(uint8 instance);
  void    (*SetL1PllTurnoffMinHird)(uint8 instance, uint8 minHIRD);
  uint16  (*GetL1LongEntryCount)(uint8 instance);
  uint16  (*GetL1ShortEntryCount)(uint8 instance);
  void    (*ClearL1EntryCounts)(uint8 instance);
  uint8   (*GetL1RemoteWakeupTime)(uint8 instance);
  void    (*SetL1RemoteWakeupTime)(uint8 instance, uint8 timeInUS);
  boolean (*GetL1FPR)(uint8 instance);
  void    (*SetL1FPR)(uint8 instance, boolean state);
  boolean (*GetHSICClkPllBypassNL)(uint8 instance);
  boolean (*GetHSICClkPllReset)(uint8 instance);
  boolean (*GetHSICClkGate)(uint8 instance);
  boolean (*GetSysClkGate)(uint8 instance);
  boolean (*GetAHBClkGate)(uint8 instance);
  boolean (*GetL1State)(uint8 instance);
  boolean (*GetDebugL1Enable)(uint8 instance);
  void    (*SetDebugL1Enable)(uint8 instance, boolean state);
  boolean (*GetBRemoteWake)(uint8 instance);
  void    (*SetBRemoteWake)(uint8 instance, boolean state);
  uint8   (*GetHIRD)(uint8 instance);
  void    (*SetHIRD)(uint8 instance, uint8 HIRD);
} HAL_HSUSB_CoreIfType;

/*
* HAL_HSUSB_PortInfoType
*/
typedef struct
{
  HAL_HSUSB_PhyType       PhyType;
} HAL_HSUSB_PortInfoType;

/*
* HAL_HSUSB_CoreInfoType
*/
typedef struct
{
  HAL_HSUSB_CoreIfType*   If;
  uint32                  BaseAddress;
  HAL_HSUSB_ErrorType     LastError;
  HAL_HSUSB_PortInfoType* PortInfo;
  uint8                   PortsQty;
} HAL_HSUSB_CoreInfoType;

/*
* HAL_HSUSB_CoresType
*/
typedef struct
{
  HAL_HSUSB_CoreIfType*   CoreIf;
  HAL_HSUSB_CoreInfoType* CoreInfo;
  uint8                   MaxCoresQty;
  boolean                 Initialized;
} HAL_HSUSB_CoresType;

/* -----------------------------------------------------------------------
**                           EXTERNS
** ----------------------------------------------------------------------- */

/*
* HS-USB HAL Cores object.
*/
extern HAL_HSUSB_CoresType HAL_HSUSB_CoresObj;

#endif /* HAL_HSUSB_IMPL */
