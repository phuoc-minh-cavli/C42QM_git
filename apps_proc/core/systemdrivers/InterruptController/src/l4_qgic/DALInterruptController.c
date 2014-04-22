/*==============================================================================

FILE:      DALInterruptController.c

DESCRIPTION: This file implements a Interrupt Controller for QGIC harware interrupt controller

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

                Copyright (c) 2016 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR

==============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/systemdrivers/InterruptController/src/l4_qgic/DALInterruptController.c#1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/28/07   an     Initial version. Conversion of Tramp to DALInterruptController

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <DALInterruptController.h>
#include <DALSys.h>
#include <DALSysCmn.h>
#include <string.h>
#include "DALInterruptControllerConfig.h"
#include "HALqgic.h"
#ifdef DALINTERRUPT_MPM_WAKEUP
#include "mpmint.h"
#endif
#include "queue.h"
#include "DDIHWIO.h"
#include "qurt_interrupt.h"
#include <err.h>
#include <stdlib.h>
#include "qurt_interrupt.h"

#ifdef TIMETEST
#include <timetest.h>
#include <timetest/timetest_l4.h>
#endif

/*=========================================================================

                           TYPE DEFINITIONS

===========================================================================*/

/*=========================================================================

                           DATA DEFINITIONS

===========================================================================*/


/*
 * This is the internal static data variable used for the functioning of the Interrupt controller.
 */
static InterruptDataType *pInterruptData;

/*=========================================================================

                           MACRO DEFINITIONS

===========================================================================*/

/*==========================================================================

                         FUNCTION  DECLARATIONS

==========================================================================*/
static DALResult GetPlatformData(InterruptControllerClientCtxt * pclientCtxt);
static void ConfigureQGIC(InterruptControllerClientCtxt *pclientCtxt);
#ifdef DALINTERRUPT_MPM_WAKEUP
static DALResult SetWakeupInterruptTrigger(uint32 IntID,uint32 nTrigger);
#endif
static void ClearInterruptFlag(uint32 nInterrupt,uint16 nFlag);
static void SetInterruptFlag(uint32 nInterrupt,uint16 nFlag);
static boolean GetInterruptFlag(uint32 nInterrupt,uint16 nFlag);
static void SetTrigger(uint32 nInterruptVector,uint32 nTrigger);
void InterruptController_SetDefaultIrq( void (*pfn)(uint32 nIrq) );


/*==========================================================================

                         FUNCTION  DEFINITIONS
=========================================================================*/

                         


/*======================================================================
  InterruptController_DefaultIRQ
=======================================================================*/

void InterruptController_DefaultIRQ ( uint32 nIRQ )
{

  DALISRCtx                  ClientParam;
  DALISR                     ClientIsr; 

  if( nIRQ < pInterruptData->pPlatformConfig->nMaxIRQ )
  {
     
    if (pInterruptData->pInterruptState[nIRQ].Isr == NULL)
	{
	  
	  HAL_qgic_Disable(nIRQ);
    }
    else
    {
	  ClientIsr = pInterruptData->pInterruptState[nIRQ].Isr;
      ClientParam = pInterruptData->pInterruptState[nIRQ].nParam;
	  
      /* Call ISR */
      ClientIsr(ClientParam);
    }
  }

}

/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/

DALResult InterruptController_DriverInit( InterruptControllerDrvCtxt *pclientCtxt )
{
	return DAL_SUCCESS;
}

DALResult InterruptController_DriverDeInit( InterruptControllerDrvCtxt *pclientCtxt )
{
	return DAL_SUCCESS;
}

/*=============================================================================

  Following functions are declared in IDevice Interface. 

===============================================================================

  FUNCTION      InterruptController_DeviceInit

  DESCRIPTION   This function is called when a driver tries to attach from the DALInterruptController.
  the purpose of this function is to initialize the device specific data for the interrupt controller.

  PARAMETERS    InterruptControllerClientCtxt * Pointer to the indivisual client context.

  DEPENDENCIES  DALSYS should be initialized.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

DALResult InterruptController_DeviceInit
( 
  InterruptControllerClientCtxt *pclientCtxt 
)
{

  InterruptControllerDevCtxt  *pDevice;
  InterruptControllerDrvCtxt  *pDrvCtxt;
  
  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  pDrvCtxt = pDevice->pInterruptControllerDrvCtxt;
  
  /* Initialize ULOG */
  ULogFront_RealTimeInit (&pDrvCtxt->hInterruptLog, "Interrupt Controller Log",
 		                            INTERRUPT_LOG_SIZE,
 		                            ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);
  ULogCore_HeaderSet (pDrvCtxt->hInterruptLog,
 		                      "Content-Type: text/tagged-log-1.0;");

  if(GetPlatformData(pclientCtxt) == DAL_ERROR)
  {
    return DAL_ERROR;
  }

  InterruptController_SetDefaultIrq(InterruptController_DefaultIRQ);


  ConfigureQGIC(pclientCtxt);


  return DAL_SUCCESS;

} /* END InterruptController_DeviceInit */


/*==========================================================================

  FUNCTION      InterruptController_DeviceDeInit

  DESCRIPTION   This function is called when a driver tries to detach from the DALInterruptController.
  the purpose of this function is to cleanup all dynamically allocated data and reset the DALInterruptController state.

  PARAMETERS    InterruptControllerClientCtxt * Pointer to the indivisual client context.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

DALResult InterruptController_DeviceDeInit
( 
  InterruptControllerClientCtxt *pclientCtxt
)
{
  
  return DAL_SUCCESS;

} /* END InterruptController_DeviceDeInit */



void InterruptController_SetDefaultIrq( void (*pfn)(uint32 nIrq) )
{
  qurt_interrupt_register_default_irq(pfn);
}



/*=============================================================================

  Following functions are extended in IInterruptController Interface.

===============================================================================

=============================================================================*/




/*==========================================================================

  FUNCTION      InterruptController_RegisterISR
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/

DALResult  InterruptController_RegisterISR
(
  InterruptControllerClientCtxt * pclientCtxt,   
  DALInterruptID nInterruptVector, 
  const DALISR isr,
  const DALISRCtx ctx, 
  uint32  IntrFlags
) 
{
  unsigned long    interrupt_flag=0;
  uint32 nInterruptPriority,nTrigger;
  InterruptControllerDevCtxt *pDevice;
  InterruptControllerDrvCtxt *pDrvCtxt;
  DALISR OldIsr;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  pDrvCtxt = pDevice->pInterruptControllerDrvCtxt;

  if((nInterruptVector == INVALID_INTERRUPT) ||(isr == NULL))
  {
    ULOG_RT_PRINTF_1(pDrvCtxt->hInterruptLog,
                 "Interrupt Controller: Invalid interrupt. Could not Register Interrupt for interrupt id %d",nInterruptVector);
    return DAL_ERROR;
  }

  INTLOCK_SAVE(interrupt_flag);

  OldIsr = (DALISR)pInterruptData->pInterruptState[nInterruptVector].Isr;
  nInterruptPriority = (IntrFlags>>24) & 0x000000ff;

  /*
   *  If the client has set up the priority we use this value. Otherwise we use the value set up in the BSP data.
   *  If the BSP data too does not have this listed then we fallback to a median priority level of 8.
   */
  if(!nInterruptPriority)
  {
    nInterruptPriority = pInterruptData->pInterruptState[nInterruptVector].nPriority;
  }
  else
  {
    pInterruptData->pInterruptState[nInterruptVector].nPriority = nInterruptPriority;
  }

  nTrigger = IntrFlags & DAL_INTERRUPT_TRIGGER_MASK;
  if(!nTrigger)
  {
    nTrigger = pInterruptData->pInterruptState[nInterruptVector].nTrigger;
  }

  #ifdef DALINTERRUPT_MPM_WAKEUP

  /*
   * Configure the interrupt as a wakeup source if necessary.
   */
  if (GetInterruptFlag(nInterruptVector, INTERRUPTF_MPM))
  {
 
    SetWakeupInterruptTrigger(nInterruptVector,nTrigger);  
    mpmint_config_wakeup(
      (mpmint_isr_type)pInterruptData->pInterruptState[nInterruptVector].nMPMID, 
      MPMINT_MODEM);
  }
  #endif /* DALINTERRUPT_MPM_WAKEUP */

  /*
   * Here TBD if the interrupt is to be unregistered with L4 and reregistered with the right handler according 
   * to the changed priority.
   */
  if(OldIsr == NULL)
  {
    HAL_qgic_SetPriority(nInterruptVector, nInterruptPriority);
  }

  pInterruptData->pInterruptState[nInterruptVector].Isr = isr;
  pInterruptData->pInterruptState[nInterruptVector].nParam = ctx;

  SetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED);  
  SetTrigger(nInterruptVector,nTrigger);
  HAL_qgic_SetTargets(nInterruptVector,0x1);
  HAL_qgic_ClearPending(nInterruptVector);
  HAL_qgic_Enable(nInterruptVector);

  INTFREE_RESTORE(interrupt_flag);

  return DAL_SUCCESS;

} /* END InterruptController_RegisterISR */


/*==========================================================================

  FUNCTION      InterruptController_Unregister
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/

DALResult  InterruptController_Unregister
( 
  InterruptControllerClientCtxt * pclientCtxt, 
  DALInterruptID  nInterruptVector
) 
{

  unsigned long    interrupt_flag=0;
  InterruptControllerDevCtxt *pDevice;
  InterruptControllerDrvCtxt *pDrvCtxt;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  pDrvCtxt = pDevice->pInterruptControllerDrvCtxt;
  
  if(nInterruptVector == INVALID_INTERRUPT)
  {
    ULOG_RT_PRINTF_0(pDrvCtxt->hInterruptLog,
    "Interrupt Controller: Invalid interrupt data. Could not Unregister Interrupt");
    return DAL_ERROR;
  }

  INTLOCK_SAVE(interrupt_flag);

  HAL_qgic_ClearPending(nInterruptVector);
  HAL_qgic_Disable(nInterruptVector);
  pInterruptData->pInterruptState[nInterruptVector].Isr = NULL;
  pInterruptData->pInterruptState[nInterruptVector].nParam = 0;  

  #ifdef DALINTERRUPT_MPM_WAKEUP

  /*
   * De-configure the interrupt as a wakeup source if necessary.
   */
  if (GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED))
  {
    mpmint_disable_wakeup(
      (mpmint_isr_type)pInterruptData->pInterruptState[nInterruptVector].nMPMID);
  }

  #endif /* DALINTERRUPT_MPM_WAKEUP */

  ClearInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED);  
  INTFREE_RESTORE(interrupt_flag);

  return DAL_SUCCESS;

} /* END InterruptController_Unregister */


/*==========================================================================

  FUNCTION      InterruptController_InterruptDone
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/

DALResult  InterruptController_InterruptDone
( 
  InterruptControllerClientCtxt * pclientCtxt,   
  DALInterruptID  nInterruptVector
) 
{

  unsigned long    interrupt_flag=0;

  InterruptControllerDevCtxt *pDevice;
  InterruptControllerDrvCtxt *pDrvCtxt;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  pDrvCtxt = pDevice->pInterruptControllerDrvCtxt;

  if(nInterruptVector == INVALID_INTERRUPT)
  {
    ULOG_RT_PRINTF_0(pDrvCtxt->hInterruptLog,
    "Interrupt Controller: Invalid interrupt. Could not clear it.");
    return DAL_ERROR;
  }
  INTLOCK_SAVE(interrupt_flag);

  HAL_qgic_ClearPending(nInterruptVector);

  INTFREE_RESTORE(interrupt_flag);

  return DAL_SUCCESS;

} /* END InterruptController_InterruptDone */


/*==========================================================================

  FUNCTION      InterruptController_InterruptEnable
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/

DALResult  InterruptController_InterruptEnable
( 
  InterruptControllerClientCtxt * pclientCtxt,
  DALInterruptID  nInterruptVector
) 
{

  unsigned long    interrupt_flag=0;

  InterruptControllerDevCtxt *pDevice;
  InterruptControllerDrvCtxt  *pDrvCtxt;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  pDrvCtxt = pDevice->pInterruptControllerDrvCtxt;

  if(nInterruptVector == INVALID_INTERRUPT)
  {
    ULOG_RT_PRINTF_0(pDrvCtxt->hInterruptLog,
    "Interrupt Controller: Invalid interrupt. Could not Enable it");
    return DAL_ERROR;
  }
  
  INTLOCK_SAVE(interrupt_flag);

  HAL_qgic_Enable(nInterruptVector);
  
  INTFREE_RESTORE(interrupt_flag);

  return DAL_SUCCESS;

} /* END InterruptController_InterruptEnable */


/*==========================================================================

  FUNCTION      InterruptController_InterruptDisable
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/

DALResult  InterruptController_InterruptDisable
( 
  InterruptControllerClientCtxt * pclientCtxt,
  DALInterruptID  nInterruptVector
) 
{
  unsigned long    interrupt_flag=0;

  InterruptControllerDevCtxt *pDevice;
  InterruptControllerDrvCtxt  *pDrvCtxt;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  pDrvCtxt = pDevice->pInterruptControllerDrvCtxt;

  if(nInterruptVector == INVALID_INTERRUPT)
  {
    ULOG_RT_PRINTF_0(pDrvCtxt->hInterruptLog,
    "Interrupt Controller: Invalid interrupt. Could not Disable it");
    return DAL_ERROR;
  }
  INTLOCK_SAVE(interrupt_flag);

  HAL_qgic_Disable(nInterruptVector);
  
  INTFREE_RESTORE(interrupt_flag);

  return DAL_SUCCESS;

} /* InterruptController_InterruptDisable */


/*==========================================================================

  FUNCTION      InterruptController_InterruptTrigger
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/

DALResult  InterruptController_InterruptTrigger
( 
  InterruptControllerClientCtxt * pclientCtxt,
  DALInterruptID  nInterruptVector
) 
{
  unsigned long    interrupt_flag=0;

  InterruptControllerDevCtxt *pDevice;
  InterruptControllerDrvCtxt  *pDrvCtxt;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  pDrvCtxt = pDevice->pInterruptControllerDrvCtxt;

  if(nInterruptVector == INVALID_INTERRUPT)
  {
    ULOG_RT_PRINTF_0(pDrvCtxt->hInterruptLog,
    "Interrupt Controller: Invalid interrupt. Could not Disable it.");
    return DAL_ERROR;
  }

  INTLOCK_SAVE(interrupt_flag);

  /*
   * Set interrupt in hardware, SGI use Generate, and peripheral interrupts
   * just mark as Pending.
   * For this API we generate non-secure interrupts to the current processor.
   */
  if (HAL_qgic_IsSGI(nInterruptVector))
  {
    HAL_qgic_Generate(nInterruptVector, HAL_QGIC_NON_SECURE, HAL_QGIC_TARGET_SELF);
  }
  else
  {
    HAL_qgic_SetPending(nInterruptVector);
  }
  
  INTFREE_RESTORE(interrupt_flag);


  return DAL_SUCCESS;

} /* InterruptController_InterruptTrigger */


/*==========================================================================

  FUNCTION      InterruptController_InterruptClear
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/

DALResult  InterruptController_InterruptClear
( 
  InterruptControllerClientCtxt * pclientCtxt,
  DALInterruptID  nInterruptVector
) 
{
  unsigned long    interrupt_flag=0;

  InterruptControllerDevCtxt *pDevice;
  InterruptControllerDrvCtxt  *pDrvCtxt;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  pDrvCtxt = pDevice->pInterruptControllerDrvCtxt;

  if(nInterruptVector == INVALID_INTERRUPT)
  {
    ULOG_RT_PRINTF_0(pDrvCtxt->hInterruptLog,
    "Interrupt Controller: Invalid interrupt. Could not clear it.");
    return DAL_ERROR;
  }
  INTLOCK_SAVE(interrupt_flag);

  HAL_qgic_ClearPending(nInterruptVector);
  
  INTFREE_RESTORE(interrupt_flag);


  return DAL_SUCCESS;

} /* END InterruptController_InterruptClear */


/*==========================================================================

  FUNCTION      InterruptController_InterruptStatus
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/

DALResult InterruptController_InterruptStatus
( 
  InterruptControllerClientCtxt * pclientCtxt,   
  DALInterruptID  nInterruptVector
) 
{
  unsigned long    interrupt_flag=0;

  DALResult nResult;
  InterruptControllerDevCtxt *pDevice;
  InterruptControllerDrvCtxt  *pDrvCtxt;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;  
  pDrvCtxt = pDevice->pInterruptControllerDrvCtxt;
  
  if(nInterruptVector == INVALID_INTERRUPT)
  {
    ULOG_RT_PRINTF_0(pDrvCtxt->hInterruptLog,
    "Interrupt Controller: Invalid interrupt. Could not get its status.");
    return DAL_ERROR;
  }

  INTLOCK_SAVE(interrupt_flag);
  if(HAL_qgic_IsPending(nInterruptVector) == TRUE)
  {
    nResult = DAL_INTERRUPT_SET;
  }
  else
  {
    nResult = DAL_ERROR;
  }
   
  INTFREE_RESTORE(interrupt_flag);

  return nResult;

} /* END InterruptController_InterruptStatus */


/*==========================================================================

   FUNCTION      SetTrigger

  DESCRIPTION   This function sets the trigger for the interrupt vector

  PARAMETERS    \
  uint32 nInterrupt  : The interrupt vector number.
  uint32 nTrigger : The interrupt trigger type.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

static void SetTrigger
(
  uint32 nInterruptVector,
  uint32 nTrigger
)
{
  switch (nTrigger)
  {
    case DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER:
      HAL_qgic_SetTrigger(nInterruptVector,HAL_QGIC_TRIGGER_EDGE);
      break;
    
    case DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER:
      HAL_qgic_SetTrigger(nInterruptVector,HAL_QGIC_TRIGGER_EDGE);
      break;
  
    case DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER:
      HAL_qgic_SetTrigger(nInterruptVector,HAL_QGIC_TRIGGER_LEVEL);
      break;

    case DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER:
      HAL_qgic_SetTrigger(nInterruptVector,HAL_QGIC_TRIGGER_LEVEL);
      break;

    default:
      break;    
  }

  /* Update the trigger information */
  pInterruptData->pInterruptState[nInterruptVector].nTrigger = nTrigger;

  return;

} /* END SetTrigger */


/*==========================================================================

  FUNCTION      InterruptController_SetTrigger
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/

DALResult InterruptController_SetTrigger
(
  InterruptControllerClientCtxt * pclientCtxt, 
  DALInterruptID  nInterruptVector,
  uint32 nTrigger
)
{
  unsigned long    interrupt_flag=0;

  InterruptControllerDevCtxt *pDevice;
  InterruptControllerDrvCtxt  *pDrvCtxt;

  uint32 bInterruptEnabled;
  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  pDrvCtxt = pDevice->pInterruptControllerDrvCtxt;

  if(nInterruptVector == INVALID_INTERRUPT)
  {
    ULOG_RT_PRINTF_0(pDrvCtxt->hInterruptLog,
    "Interrupt Controller: Invalid interrupt. Could not set the trigger for it");
    return DAL_ERROR;
  }

  INTLOCK_SAVE(interrupt_flag);


  bInterruptEnabled = (uint32)HAL_qgic_IsEnabled(nInterruptVector);
   
  if(bInterruptEnabled)
  {
    HAL_qgic_Disable(nInterruptVector);
  }
 
  nTrigger = nTrigger & DAL_INTERRUPT_TRIGGER_MASK;
  SetTrigger(nInterruptVector,nTrigger);
  if(bInterruptEnabled)
  {
    HAL_qgic_ClearPending(nInterruptVector);
    HAL_qgic_Enable(nInterruptVector);
  }
  pInterruptData->pInterruptState[nInterruptVector].nTrigger = nTrigger;
  
  INTFREE_RESTORE(interrupt_flag);

  #ifdef DALINTERRUPT_MPM_WAKEUP
  SetWakeupInterruptTrigger(nInterruptVector,nTrigger);  
  #endif /* if DALINTERRUPT_MPM_WAKEUP*/

  return DAL_SUCCESS;

} /* END InterruptController_SetTrigger */


/*==========================================================================

  FUNCTION      InterruptController_IsInterruptPending
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/

DALResult InterruptController_IsInterruptPending
(
  InterruptControllerClientCtxt * pclientCtxt, 
  DALInterruptID  nInterruptVector,
  uint32* bState
)
{

  unsigned long    interrupt_flag=0;

  InterruptControllerDevCtxt *pDevice;
  InterruptControllerDrvCtxt  *pDrvCtxt;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  pDrvCtxt = pDevice->pInterruptControllerDrvCtxt;

  if(nInterruptVector == INVALID_INTERRUPT)
  {
    ULOG_RT_PRINTF_0(pDrvCtxt->hInterruptLog,
    "Interrupt Controller: Invalid interrupt. Could not get its pending status.");
    return DAL_ERROR;
  }
  
  INTLOCK_SAVE(interrupt_flag);

  *bState = (HAL_qgic_IsPending(nInterruptVector) && HAL_qgic_IsEnabled(nInterruptVector));
  
  INTFREE_RESTORE(interrupt_flag);
  return DAL_SUCCESS;

} /* END InterruptController_IsInterruptPending */


/*==========================================================================

  FUNCTION      InterruptController_IsInterruptEnabled
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/

DALResult InterruptController_IsInterruptEnabled
(
  InterruptControllerClientCtxt * pclientCtxt, 
  DALInterruptID  nInterruptVector,
  uint32* bState
)
{
  unsigned long    interrupt_flag=0;

  InterruptControllerDevCtxt *pDevice;
  InterruptControllerDrvCtxt  *pDrvCtxt;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  pDrvCtxt = pDevice->pInterruptControllerDrvCtxt;
  
  if(nInterruptVector == INVALID_INTERRUPT)
  {
    ULOG_RT_PRINTF_0(pDrvCtxt->hInterruptLog,
    "Interrupt Controller: Invalid interrupt. Could not get its enabled status.");
    return DAL_ERROR;
  }
  
  INTLOCK_SAVE(interrupt_flag);

  *bState = (uint32)HAL_qgic_IsEnabled(nInterruptVector);
  
  INTFREE_RESTORE(interrupt_flag);


  return DAL_SUCCESS;

} /* END InterruptController_IsInterruptEnabled */


/*==========================================================================

  FUNCTION      InterruptController_MapWakeupInterrupt
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/

DALResult InterruptController_MapWakeupInterrupt
(
  InterruptControllerClientCtxt * pclientCtxt, 
  DALInterruptID nInterruptVector,
  uint32 nWakeupIntID
)
{
  unsigned long    interrupt_flag=0;

  #ifdef DALINTERRUPT_MPM_WAKEUP
  HAL_qgic_TriggerType eTrigger;
  uint32 nInterruptTrigger;
  InterruptControllerDevCtxt *pDevice;
  InterruptControllerDrvCtxt  *pDrvCtxt;

  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  pDrvCtxt = pDevice->pInterruptControllerDrvCtxt;

  if(nInterruptVector == INVALID_INTERRUPT)
  {
    ULOG_RT_PRINTF_1(pDrvCtxt->hInterruptLog,
    "Interrupt Controller: Invalid interrupt. Could not map it as an MPM interrupt for interrupt id %d.",nInterruptVector);
    return DAL_ERROR;
  }

  INTLOCK_SAVE(interrupt_flag);

  pInterruptData->pInterruptState[nInterruptVector].nMPMID = nWakeupIntID;                      
  SetInterruptFlag(nInterruptVector, INTERRUPTF_MPM);  
  HAL_qgic_GetTrigger(nInterruptVector,&eTrigger);
  switch (eTrigger)
  {
    case HAL_QGIC_TRIGGER_EDGE:
      nInterruptTrigger = DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER;
      break;

	case HAL_QGIC_TRIGGER_LEVEL:
      nInterruptTrigger = DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER;
      break;

    default:
         INTFREE_RESTORE(interrupt_flag);
         return DAL_ERROR;    
  }                                                                            
  SetWakeupInterruptTrigger(nInterruptVector, nInterruptTrigger);
             
  if (GetInterruptFlag(nInterruptVector, INTERRUPTF_REGISTERED))
  {
    mpmint_config_wakeup(
      (mpmint_isr_type) pInterruptData->pInterruptState[nInterruptVector].nMPMID, 
      MPMINT_MODEM);
  }                   

  INTFREE_RESTORE(interrupt_flag);
 
  #endif /* DALINTERRUPT_MPM_WAKEUP */

  return DAL_SUCCESS;

} /* InterruptController_MapWakeupInterrupt */


/*==========================================================================

  FUNCTION      InterruptController_IsAnyInterruptPending
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult InterruptController_IsAnyInterruptPending
(
  InterruptControllerClientCtxt * pclientCtxt, 
  uint32* bState
)
{
  uint32 nInterruptVector;
  HAL_qgic_CPUIdType eCPU;

  /*
   * Check pending status in hardware
   */
  HAL_qgic_GetHighestPending(&nInterruptVector, &eCPU);

  if (nInterruptVector != HAL_QGIC_IRQ_NA && nInterruptVector != HAL_QGIC_IRQ_SECURE_NA)
  {
    *bState = 1;
  }
  else
  {
    /*
     * Nothing pending.
     */
    *bState = 0;
  }
  return DAL_SUCCESS;

} /* InterruptController_IsAnyInterruptPending */


/*==========================================================================

  FUNCTION      InterruptController_Sleep
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult 
InterruptController_Sleep
(
  InterruptControllerClientCtxt * pclientCtxt,
  InterruptControllerSleepType sleep
)
{
  /*
   * Save the hardware state
   */
  if (sleep == DALINTCNTLR_POWER_COLLAPSE)
  {
    HAL_qgic_Save();
  }


  return DAL_SUCCESS;

} /* END InterruptController_Sleep */


/*==========================================================================

  FUNCTION      InterruptController_GetInterruptTrigger
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult InterruptController_GetInterruptTrigger
(
  InterruptControllerClientCtxt *pclientCtxt, 
  DALInterruptID nInterruptVector,
  uint32* eTrigger
)
{
  unsigned long    interrupt_flag=0;

  InterruptControllerDevCtxt *pDevice;
  InterruptControllerDrvCtxt  *pDrvCtxt;
  HAL_qgic_TriggerType eInterruptTrigger;
  
  pDevice = pclientCtxt->pInterruptControllerDevCtxt;
  pDrvCtxt = pDevice->pInterruptControllerDrvCtxt;
  
  if((nInterruptVector == INVALID_INTERRUPT) ||(eTrigger ==NULL))
  {
    ULOG_RT_PRINTF_1(pDrvCtxt->hInterruptLog,
    "Interrupt Controller: Invalid interrupt data Could not get the trigger for interrupt id %d",nInterruptVector);
    return DAL_ERROR;
  }
  INTLOCK_SAVE(interrupt_flag);
  HAL_qgic_GetTrigger(nInterruptVector,&eInterruptTrigger);
  switch (eInterruptTrigger)
  {
    case HAL_QGIC_TRIGGER_EDGE:
      *eTrigger = DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER;
      break;

	case HAL_QGIC_TRIGGER_LEVEL:
      *eTrigger = DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER;
      break;

    default:
       INTFREE_RESTORE(interrupt_flag);
       return DAL_ERROR;    
  }
  INTFREE_RESTORE(interrupt_flag);
  return DAL_SUCCESS;

} /* END InterruptController_GetInterruptTrigger */


/*==========================================================================

  FUNCTION      InterruptController_RestoreWakeupConfigs
  DESCRIPTION   See DDIInterruptController.h

==========================================================================*/
DALResult 
InterruptController_Wakeup
(
  InterruptControllerClientCtxt * pclientCtxt,
  InterruptControllerSleepType sleep
)
{

  /*
   * Restore the hardware
   */
  if (sleep == DALINTCNTLR_POWER_COLLAPSE)
  {
    HAL_qgic_Restore();
  }


  return DAL_SUCCESS;

}/* END InterruptController_Wakeup */


/*=============================================================================

  FUNCTION      GetPlatformData

  DESCRIPTION   This function queries DALSYS to get platform specific information for the interrupt Controller
  GPIO interrupt. The interrupt is disabled.

  PARAMETERS     
  InterruptControllerClientCtxt * :     The client context pointer of the interrupt 
                            controller DAL.
  DEPENDENCIES  None.

  RETURN VALUE  
  DALResult.  : 
  DAL_ERROR : 
  If the interrupt controller was not able to query the target specific information.
  DAL_SUCCESS :
  Returns a success if all relevant data was 

  SIDE EFFECTS  None.

=============================================================================*/

static DALResult 
GetPlatformData
(
  InterruptControllerClientCtxt * pclientCtxt
)
{
  uint32 nIdx;
  /* 
   * Structure which holds properties 
  */
  DALSYSPropertyVar system_property_var;
  HAL_qgic_BaseAddressType  sQGICBaseAddressData;
  uint8* pQGICVirtualBase;
  InterruptConfigType *pIRQConfigs;

  /*
   *  Declare property handle variable 
   */
  DALSYS_PROPERTY_HANDLE_DECLARE(interrupt_property); 
    
  /* Get the property handle */
  DALSYS_GetDALPropertyHandle(DALDEVICEID_INTERRUPTCONTROLLER,interrupt_property); 
  if(DALSYS_GetPropertyValue(interrupt_property,"INTERRUPT_CONFIG_DATA",
                           0,&system_property_var) == DAL_SUCCESS)
  {
    pInterruptData = (InterruptDataType *) malloc(sizeof(InterruptDataType));

    if(pInterruptData == NULL)
    {
      ERR_FATAL
        ("Interrupt Controller: could not allocate data for InterruptController", 0, 0, 0);

      return DAL_ERROR;
    }

    /*
     * Allways initialize malloced data to 0.
     */
    memset(pInterruptData, 0, sizeof(InterruptDataType));
    pInterruptData->pPlatformConfig  = (InterruptPlatformDataType *)system_property_var.Val.pdwVal;
  }
  else
  {
     ERR_FATAL("Interrupt Controller: could not get configuration data for Device", 0, 0, 0);

    return DAL_ERROR;
  }
  
  /*
   * Check for NULL platform config before we dereference it
   */
  if(pInterruptData->pPlatformConfig == NULL)
  {
    ERR_FATAL("Interrupt Controller: Target Config data is corrupt.", 0, 0, 0);

    return DAL_ERROR;
  }
   
  if(pInterruptData->pPlatformConfig->nMaxIRQ)
  {
     pInterruptData->pInterruptState =  (InterruptStateType *) malloc(sizeof(InterruptStateType)*(pInterruptData->pPlatformConfig->nMaxIRQ));

    if(pInterruptData->pInterruptState == NULL)
    {
     ERR_FATAL
        ("Interrupt Controller: could not allocate state data for InterruptController", 0, 0, 0);

      return DAL_ERROR;
    }

    /*
     * Allways initialize malloced data to 0.
     */
    memset(pInterruptData->pInterruptState, 0, 
      (sizeof(InterruptStateType)*pInterruptData->pPlatformConfig->nMaxIRQ));
  }
  else
  {
     ERR_FATAL
        ("Interrupt Controller: Supported interrupts is not defined", 0, 0, 0);

    return DAL_ERROR;
  }

  pInterruptData->pClientCtxt = pclientCtxt;

  /* 
   * We first default to a median priority and level high trigger.
   */
  for(nIdx = 0; nIdx < pInterruptData->pPlatformConfig->nMaxIRQ; nIdx ++)
  { 
    pInterruptData->pInterruptState[nIdx].nPriority = INTERRUPT_MEDIAN_PRIORITY + INTERRUPT_START_PRIORITY;
    pInterruptData->pInterruptState[nIdx].nTrigger = DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER;
  }

  pIRQConfigs =  (InterruptConfigType*)pInterruptData->pPlatformConfig->pIRQConfigs;

  /* 
   * We then only save the interrupt properties if this is supported in the BSP
   */
  if(pIRQConfigs != NULL)
 {

    /*
     * Go over supported interrupts from the BSP Interrupt Config Data.
     */
    for(nIdx = 0; pIRQConfigs[nIdx].nQGICNumber != INVALID_INTERRUPT; nIdx ++)
    {
      pInterruptData->pInterruptState[pIRQConfigs[nIdx].nQGICNumber].nPriority = pIRQConfigs[nIdx].nPriority;
      pInterruptData->pInterruptState[pIRQConfigs[nIdx].nQGICNumber].pInterruptName = pIRQConfigs[nIdx].pInterruptName;
    }
  }
  if(DAL_DeviceAttach(DALDEVICEID_HWIO,&pInterruptData->pHWIOHandle) == DAL_SUCCESS)
  {
    if(DalHWIO_MapRegionByAddress(pInterruptData->pHWIOHandle,
        (uint8 *)pInterruptData->pPlatformConfig->psQgicAddr->pQGICBaseAddr, 
        (uint8 **)&pQGICVirtualBase) != DAL_SUCCESS)
	{
      ERR_FATAL
        ("Interrupt Controller: Error in mapping hwio addresses. ", 0, 0, 0);

      return DAL_ERROR;
    }
  } 
  else
  {
    /*
     * If for some reason HWIO dal is not present on the platform then the 
     * virtual address is the same as the physical address.
     */
     pQGICVirtualBase = pInterruptData->pPlatformConfig->psQgicAddr->pQGICBaseAddr;
  }

  /* 
   * The rest of the bases are offsets from the main GIC base address.
   */
  sQGICBaseAddressData.nGICDBaseAddress = (uint32)pQGICVirtualBase + 
  pInterruptData->pPlatformConfig->psQgicAddr->nQGICDistributorOffset;
  sQGICBaseAddressData.nGICCBaseAddress = (uint32)pQGICVirtualBase + 
   pInterruptData->pPlatformConfig->psQgicAddr->nQGICCPUOffset;
  sQGICBaseAddressData.nGICHBaseAddress = (uint32)pQGICVirtualBase + 
    pInterruptData->pPlatformConfig->psQgicAddr->nHypervisorOffset;
  sQGICBaseAddressData.nGICVBaseAddress = (uint32)pQGICVirtualBase + 
   pInterruptData->pPlatformConfig->psQgicAddr->nVirtualizationCPUOffset;

  /*
   * Initialized the QGIC
   */
  HAL_qgic_Init(&sQGICBaseAddressData);
  
  return DAL_SUCCESS;

} /* END InterruptController_GetPlatformData */

/*==========================================================================

  FUNCTION      ConfigureQGIC

  DESCRIPTION   This function sets up the QGIC interruptcontroller at initialization.

  PARAMETERS    None

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  none.

==========================================================================*/
static void ConfigureQGIC(InterruptControllerClientCtxt *pclientCtxt)
{
  

 /*
   * Clear and Mask all the Interrupts
   */

   HAL_qgic_ClearInterrupts();
   HAL_qgic_MaskInterrupts();
  
  
  /*
   * If we are running in legacy mode (i.e. always Secure context with no
   * trustzone) then enable the non-secure interrupts and direct secure
   * to FIQ.
   */
  if (pInterruptData->pPlatformConfig->eMode == QGIC_MODE_LEGACY)
  {
    HAL_qgic_SetSecureDest(HAL_QGIC_PORT_FIQ);
    HAL_qgic_SetSecureAckNS(TRUE);
    HAL_qgic_EnableDistributorNS();
    HAL_qgic_EnableInterruptsNS();
  }

  /*
   * Kick things off now.
   */
  HAL_qgic_EnableDistributor();
  HAL_qgic_EnableInterrupts();

  /*
   * Set priority mask to the lowest to allow all interrupts.
   */
  HAL_qgic_SetPriorityMask(HAL_QGIC_PRIORITY_LOWEST+1);

  return;

} /* END ConfigureQGIC */


/*==========================================================================

  FUNCTION      SetWakeupInterruptTrigger

  DESCRIPTION   This function configures the interrupt trigger for the 
                relevant MPM interrupt.

  PARAMETERS    IntID   - The interrupt whose isr is needed.
                nTrigger -  The trigger to be set.

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  See implementation header file.


==========================================================================*/

#ifdef DALINTERRUPT_MPM_WAKEUP
static DALResult SetWakeupInterruptTrigger
(
  uint32 nInterrupt,
  uint32 nTrigger
)
{

  mpmint_detect_type   mao_int_detect_type;
  mpmint_polarity_type mao_int_polarity_type;

  /*
   * If this interrupt has not been mapped from the MPM then return an ERROR.
   */
  if (!GetInterruptFlag(nInterrupt, INTERRUPTF_MPM))
  {
    return DAL_ERROR;
  }

  switch (nTrigger)
  {
    default:
    return DAL_ERROR;

    case DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER:
      mao_int_detect_type = MPMINT_LEVEL_DETECT;
      mao_int_polarity_type = MPMINT_ACTIVE_HIGH;
      break;

    case DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER:
      mao_int_detect_type = MPMINT_LEVEL_DETECT;
      mao_int_polarity_type = MPMINT_ACTIVE_LOW;
      break;

    case DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER:
      mao_int_detect_type = MPMINT_EDGE_DETECT;
      mao_int_polarity_type = MPMINT_ACTIVE_HIGH;
      break;

    case DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER:
      mao_int_detect_type = MPMINT_EDGE_DETECT;
      mao_int_polarity_type = MPMINT_ACTIVE_LOW;
      break;
  }

  /*
   * Configure MAO.
   */
  mpmint_config_int(
    (mpmint_isr_type)pInterruptData->pInterruptState[nInterrupt].nMPMID,
    mao_int_detect_type,
    mao_int_polarity_type);

  return DAL_SUCCESS;
 
} /* END SetWakeupInterruptTrigger */

#endif /* DALINTERRUPT_MPM_WAKEUP */


/*==========================================================================

  FUNCTION      GetInterruptFlag

==========================================================================*/

static boolean GetInterruptFlag
(
  uint32 nInterrupt,
  uint16         nFlag
)
{
  return (pInterruptData->pInterruptState[nInterrupt].nFlags & nFlag) ? TRUE : FALSE;

} /* END GetInterruptFlag */


/*==========================================================================

  FUNCTION      SetInterruptFlag

==========================================================================*/

static void SetInterruptFlag
(
  uint32 nInterrupt,
  uint16         nFlag
)
{
  pInterruptData->pInterruptState[nInterrupt].nFlags |= nFlag;

} /* END SetInterruptFlag */


/*==========================================================================

  FUNCTION      ClearInterruptFlag

==========================================================================*/

static void ClearInterruptFlag
(
  uint32 nInterrupt,
  uint16         nFlag
)
{

  pInterruptData->pInterruptState[nInterrupt].nFlags &= ~nFlag;

} /* END ClearInterruptFlag */


/*==========================================================================

  FUNCTION      InterruptController_GetInterruptID

==========================================================================*/
DALResult InterruptController_GetInterruptID
(
  InterruptControllerClientCtxt  *pclientCtxt,
  const char* szIntrName,
  uint32* pnIntrID
)
{
  uint32 nIdx;
  DALResult nResult;

  nResult = DAL_ERROR;
  if (pnIntrID == NULL) 
  {
    return nResult;
  }
  *pnIntrID = INVALID_INTERRUPT;
  for (nIdx=0;nIdx < pInterruptData->pPlatformConfig->nMaxIRQ;nIdx++) 
  {
    if (strcmp(szIntrName, pInterruptData->pInterruptState[nIdx].pInterruptName) == 0)
    {
      nResult = DAL_SUCCESS;
      *pnIntrID = nIdx;
      break;
    }
  }
  return nResult;
} /* END InterruptController_GetInterruptID */


/*==========================================================================

  FUNCTION      InterruptController_LogState

==========================================================================*/
DALResult InterruptController_LogState
(
  InterruptControllerClientCtxt  *pclientCtxt,
  void *pULog
)
{
  uint32 nIdx,nTrigger,bIsEnabled,bIsFired;
  
  ULOG_RT_PRINTF_0((ULogHandle)pULog,"Interrupt State log ");
  ULOG_RT_PRINTF_0((ULogHandle)pULog,"This log contains Interrupt state of those interrupts that are either fired (i.e. Raw hw status shows the interrupt has fired) or enabled");
 
  for(nIdx = 0; nIdx < pInterruptData->pPlatformConfig->nMaxIRQ; nIdx ++)
  {
    bIsEnabled = HAL_qgic_IsEnabled(nIdx);
    bIsFired = HAL_qgic_IsPending(nIdx);
    if ((bIsFired != 0)||(bIsEnabled != 0)) 
    {
      InterruptController_GetInterruptTrigger(pclientCtxt,nIdx,&nTrigger);
      ULOG_RT_PRINTF_4((ULogHandle)pULog,"Interrupt: %d Enabled:%d Fired: %d Trigger: %d ",nIdx,bIsEnabled,
                     bIsFired,nTrigger);
    }
    
  }
  
  return DAL_SUCCESS;

} /* END InterruptController_LogState */
