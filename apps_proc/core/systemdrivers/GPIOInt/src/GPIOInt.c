/*==============================================================================

FILE:      DalGPIOInt.c

DESCRIPTION
  This modules implements the API to utilize the Dal GPIO interrupt controller. 
  The definitions in this module are implemented internally and should not be 
  exposed for external client usage directly without using the accompanying DDI
  interface file for this module.

REFERENCES

               Copyright (c) 2016 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/systemdrivers/GPIOInt/src/GPIOInt.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
1/1/2010   aratin  First draft created. 
===========================================================================*/
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "DDITimetick.h"
#include "DDITlmm.h"
#include "busywait.h"
#include "DDIInterruptController.h"
#include "mpmint.h"
#ifdef QC_WINCE_VERSION
#include "ioctl_cfg.h"
#include "ioctl_intr.h"
#endif
#include "HALgpioint.h"
#include "DALGlbCtxt.h"
#include "GPIOInt.h"
#include "DDIHWIO.h"
#include <stdlib.h>
/*------------------------------------------------------------------------------
 GPIOInt Constants.
------------------------------------------------------------------------------*/ 
/*
 * Disable TCXO shutdown if any non MPM interrupts are enabled.
 */
#define GPIOINT_XO_SHUTDOWN_DISABLE 1

/*------------------------------------------------------------------------------
 GPIOInt Data Declarations.
------------------------------------------------------------------------------*/ 
/*
 * This is static GPIOInt state data. It can be accessed for debugging 
 * GPIOInterrupts to see what is the current registration state of the GPIO.
 */
static GPIOIntCntrlType GPIOIntData;
/*------------------------------------------------------------------------------
Following functions are internal MAO related functions that are turned off in 
WM until we have api support.
------------------------------------------------------------------------------*/
#ifdef GPIOINT_USE_MPM
mpmint_processor_type GPIOIntr_GetMPMProcId(GPIOIntProcessorType processor);
#endif
#ifdef GPIOINT_USE_NPA
static void GPIOIntr_UpdateNPARequest(GPIOIntDevCtxt* device);
static void GPIOIntr_Init_NPA(void *context,unsigned int  event_type, 
void *resource_name, unsigned int  resource_strlen);
#endif
void GPIOIntr_ReleaseDirectConnectInterrupt(GPIOIntDevCtxt *device, uint32 interrupt_id);
DALResult GPIOIntr_ConfigureGPIOInterrupt( GPIOIntClientCtxt * pCtxt,uint32 gpio,GPIOIntTriggerType trigger, uint32 nFlags);

/*==========================================================================

  FUNCTION      GPIOIntr_Init_NPA

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

#ifdef GPIOINT_USE_NPA
static void GPIOIntr_Init_NPA
( 
  void          *context,
  unsigned int  event_type, 
  void          *resource_name,
  unsigned int  resource_strlen
)
{
  GPIOIntDevCtxt* device;
  device = (GPIOIntDevCtxt*)context;
  device->gpioint_cntrl->npa_client = npa_create_sync_client(resource_name,"tramp_gpio", NPA_CLIENT_REQUIRED);
  if(device->gpioint_cntrl->npa_client != NULL)
  {
    GPIOIntr_UpdateNPARequest(device);
  }
  else
  {
    ERR_FATAL(
    "GPIO Interrupt Controller: Device could not create an NPA client",0,0,0);
  }

}/* END GPIOIntr_init_npa */


/*==========================================================================

  FUNCTION      GPIOIntr_UpdateNPARequest

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

static void GPIOIntr_UpdateNPARequest
(
  GPIOIntDevCtxt* device
)
{

  /* 
   * If the server is not ready we cannot make this request
   */
  if(device->gpioint_cntrl->npa_client == NULL)
  {
    return;
  }
  /*
   * Disable interrupts
   */
  qurt_mutex_lock(device->GPIOIntSyncObject);
  
  if (device->gpioint_cntrl->non_mpm_interrupts)
  {
    
    /*
     *  If there are are interrupts that are not wakeup sources we need to 
     *  request for xo clk source so that we can keep state of the interrupt 
     *  controller and not have it wiped out at a lower voltage.
     */
    npa_issue_required_request(device->gpioint_cntrl->npa_client, GPIOINT_XO_SHUTDOWN_DISABLE);
  }

  /*
   *  If  only interrupts that are wakeup sources are enabled we can cancel request 
   *  for xo clk and save power. We don't have to maintain state at 
   *  the interrupt controller as the MPM interrupt controller already has this state.
   */
  else 
  {
    npa_cancel_request(device->gpioint_cntrl->npa_client);
  }

  /*
   * Enable interrupts
   */
  qurt_mutex_unlock(device->GPIOIntSyncObject);

}  /* END GPIOIntr_update_npa_request */

#endif /* GPIOINT_USE_NPA */


/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult 
GPIOIntr_DriverInit(GPIOIntDrvCtxt *pCtxt)
{
	return DAL_SUCCESS;
}

DALResult 
GPIOIntr_DriverDeInit(GPIOIntDrvCtxt *pCtxt)
{
	return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are declared in DalDevice Interface. 
------------------------------------------------------------------------------*/

/*==========================================================================

  FUNCTION      GPIOIntr_DeviceInit

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_DeviceInit
(
  GPIOIntClientCtxt *pCtxt
)
{
  uint32 index;
  GPIOIntCntrlType *gpioint_cntrl;
  GPIOIntDevCtxt* device;
  DALResult result;

  device = pCtxt->pGPIOIntDevCtxt;
  device->gpioint_cntrl = &GPIOIntData;
  result = DAL_SUCCESS;
  
  //ULog Initialization
  if(DAL_SUCCESS == ULogFront_RealTimeInit (&pCtxt->hGPIOIntLog, "GPIOInt Log",
                            GPIOINT_LOG_SIZE,
                            ULOG_MEMORY_LOCAL, ULOG_LOCK_OS))
  ULogCore_HeaderSet (pCtxt->hGPIOIntLog,
                      "Content-Type: text/tagged-log-1.0;");

  /*
   * GPIOInt_Init has to be called first before anyone can attach to this 
   * driver. This is to ensure that GPIOInt dependencies of NPA and DALSYS 
   * are met.
   */
  if(GPIOIntData.GPIOInt_Init != 1)
  {
    ULOG_RT_PRINTF_0 (pCtxt->hGPIOIntLog, 
      "GPIO Interrupt Controller: Client cannot attach before GPIOInt_Init ");
    return DAL_ERROR;
  }
 
  if(!pCtxt->is_initialized)
  {
    /* 
     * Initialize the synchronization.
     */
   device->GPIOIntSyncObject = (qurt_mutex_t *)malloc(sizeof(qurt_mutex_t));
   if (device->GPIOIntSyncObject == NULL)
      {
         ERR_FATAL(
             "Failed to allocate memory to synchronization object"
             , 0, 0, 0);
      }
   qurt_mutex_init(device->GPIOIntSyncObject);
    /*
     * GPIOInt dal communicates with the main level1 interrupt controller
     * and the TLMM dal. So in Init we need to attach to the respective dals 
     * to get their handles in order to use their API.
     */
    if(DAL_SUCCESS != GPIOIntr_AttachToDals(device))
    { 
      return DAL_ERROR;
    }
   
    /*
     * Initialize the GPIOInt HAL
     */
    qurt_mutex_lock(device->GPIOIntSyncObject);
    if(DAL_SUCCESS != GPIOIntr_GetTargetData(pCtxt))
    {  
      qurt_mutex_unlock(device->GPIOIntSyncObject);
      return DAL_ERROR;
    }

    /* 
     * The internal pointer allocation is to make 
     * dereferencing easy to read
     */
    gpioint_cntrl = device->gpioint_cntrl;
    
    /* 
     *  Initialize all GPIOs as summary.
     */ 
    for(index =0;index < gpioint_cntrl->gpio_number; index++)
    {
       gpioint_cntrl->state[index].intr_type = HAL_GPIOINT_SUMMARY;
    }

    /*
     * Install the top-level interrupt handler for the SUMMARY class interrupts.
     */
    gpioint_cntrl->summary_param = (uint32)HAL_GPIOINT_GROUP1;
    result = DalInterruptController_RegisterISR(device->intr_handle, 
                     device->gpioint_cntrl->summary_intr_id,
                     (DALISR)GPIOIntr_Isr, (DALISRCtx)device, DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER);
    qurt_mutex_unlock(device->GPIOIntSyncObject);
  }
 
  if(result == DAL_SUCCESS)
  {
    /*
     * flag the driver as initialised.
     */
    pCtxt->is_initialized = 1;
    return result;
  }
  else
  {
    pCtxt->is_initialized = 0;
    return DAL_ERROR;
  }

} /* END GPIOIntr_DeviceInit */


/*==========================================================================

  FUNCTION      GPIOIntr_GetTargetData

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_GetTargetData
(
  GPIOIntClientCtxt *pCtxt
)
{
  DALResult result;
  GPIOIntDevCtxt *device;

  /* 
   * structure which holds properties for dal.
   */
  DALSYSPropertyVar system_property_var;  
  HAL_gpioint_ContextType Target;
  HAL_gpioint_TargetDataType TargetData;
  #ifdef GPIOINT_USE_NPA
  char * xo_shutdown_rsrc;
  #endif
  /* 
   * declare property handle variable.
   */
  DALSYS_PROPERTY_HANDLE_DECLARE(interrupt_property); 

  result = DAL_SUCCESS;
  device = pCtxt->pGPIOIntDevCtxt;
  
  /* Get the property handle */
  DALSYS_GetDALPropertyHandle(DALDEVICEID_GPIOINT,interrupt_property);
  result = DALSYS_GetPropertyValue(interrupt_property,"PROCESSOR",
                           0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    device->gpioint_cntrl->processor = 
    (HAL_gpioint_ProcessorType)system_property_var.Val.dwVal;
  }
  else
  {
    ULOG_RT_PRINTF_0 (pCtxt->hGPIOIntLog, 
    "GPIO Interrupt Controller: Target Data is corrupt");
    return DAL_ERROR;
  }  

  result = DALSYS_GetPropertyValue(interrupt_property,"XO_SHUTDOWN_RSRC",
                           0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    #ifdef GPIOINT_USE_NPA
    xo_shutdown_rsrc = 
    (char *)system_property_var.Val.pszVal;

    /*
     * GPIOInt needs to communicate to the MPM for power saving features 
     * implemented.
     */
    npa_resource_available_cb(xo_shutdown_rsrc, GPIOIntr_Init_NPA, device);
    #endif
  }
  else
  {
    ULOG_RT_PRINTF_0 (pCtxt->hGPIOIntLog,
    "GPIO Interrupt Controller: Target Data is corrupt");
    return DAL_ERROR;
  }  

  TargetData.eProcessor = device->gpioint_cntrl->processor;
  Target.pTargetData = &TargetData;

  result = DALSYS_GetPropertyValue(interrupt_property,"GPIOINT_PHYSICAL_ADDRESS",
                           0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    device->gpioint_cntrl->gpioint_physical_address = (uint32 *)system_property_var.Val.dwVal;
  }
  else
  {
    ULOG_RT_PRINTF_0 (pCtxt->hGPIOIntLog, 
    "GPIO Interrupt Controller: could not get a base address for GPIOInt");
    return DAL_ERROR;
  } 
  if(DAL_DeviceAttach(DALDEVICEID_HWIO,&pCtxt->pGPIOIntDevCtxt->hwio_handle) == DAL_SUCCESS)
  {
    if(DalHWIO_MapRegionByAddress(device->hwio_handle,(uint8 *)device->gpioint_cntrl->gpioint_physical_address, 
                          (uint8 **)&device->gpioint_cntrl->gpioint_virtual_address) != DAL_SUCCESS)
    {
      return DAL_ERROR;
    }
    TargetData.nGPIOIntBase = device->gpioint_cntrl->gpioint_virtual_address;
    TargetData.nGPIOIntBasePhys = device->gpioint_cntrl->gpioint_physical_address;
   } 
  else
  {
    TargetData.nGPIOIntBase = device->gpioint_cntrl->gpioint_physical_address;
    TargetData.nGPIOIntBasePhys = device->gpioint_cntrl->gpioint_physical_address;
  }
  result = DALSYS_GetPropertyValue(interrupt_property,"INTERRUPT_PLATFORM",
                           0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    device->gpioint_cntrl->interrupt_controller = (uint8)system_property_var.Val.dwVal;
  }
  else
  {
    ULOG_RT_PRINTF_0 (pCtxt->hGPIOIntLog, 
    "GPIO Interrupt Controller: could not the main interrupt controller for GPIOInt");
    return DAL_ERROR;
  } 
  HAL_gpioint_Init(&Target);
  HAL_gpioint_GetNumber(&device->gpioint_cntrl->gpio_number);
  
  result = DALSYS_GetPropertyValue(interrupt_property,
  "NUMBER_OF_DIRECT_CONNECT_INTERRUPTS",0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    device->gpioint_cntrl->direct_intr_number = 
        system_property_var.Val.dwVal;
  }
  else
  {
    device->gpioint_cntrl->direct_intr_number = 0;
  }

  if(device->gpioint_cntrl->direct_intr_number > device->gpioint_cntrl->gpio_number)
  {
    ULOG_RT_PRINTF_0 (pCtxt->hGPIOIntLog, 
    "GPIO Interrupt Controller: Invalid Config Data ");
    return DAL_ERROR;
  }

  result = DALSYS_GetPropertyValue(interrupt_property,"SUMMARY_INTR_ID",
                           0,&system_property_var); 
  if(result == DAL_SUCCESS)
  {
    device->gpioint_cntrl->summary_intr_id = 
        system_property_var.Val.dwVal;
  }
  else
  {
    ULOG_RT_PRINTF_0 (pCtxt->hGPIOIntLog,
    "GPIO Interrupt Controller: summary intr id is corrupt");
    return DAL_ERROR;
  }
 
  if(device->gpioint_cntrl->direct_intr_number)
  {
    result = DALSYS_GetPropertyValue(interrupt_property,
    "DIRECT_CONNECT_CONFIG_MAP",0,&system_property_var); 
    if(result == DAL_SUCCESS)
    {
      device->gpioint_cntrl->gpioint_config_map = 
      (GPIOIntConfigMapType *)system_property_var.Val.pStruct;
    }
    else
    {
      ULOG_RT_PRINTF_0 (pCtxt->hGPIOIntLog,
      "GPIO Interrupt Controller: gpioint_config_map is corrupt");
      return result;
    }

  }  
  return result;

} /* END GPIOIntr_GetTargetData */


/*==========================================================================

  FUNCTION      GPIOIntr_Save (No implementation perceived as of now.)

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_Save
( 
  GPIOIntClientCtxt * pCtxt
)
{
    return DAL_ERROR;
}
/*==========================================================================

  FUNCTION      GPIOIntr_AttachToDals

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_AttachToDals
(
  GPIOIntDevCtxt *device
)
{
  /* attach to the Main Interrupt Controller dal*/
  if(DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, 
                                     &device->intr_handle))
      return DAL_ERROR;

  /*
   * Acquire system timer.
   */
  if(DalTimetick_Attach("SystemTimer", &device->timetick_handle) != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }
  return DAL_SUCCESS;

} /* END GPIOIntr_AttachToDals */


DALResult 
GPIOIntr_DeviceDeInit(GPIOIntClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult 
GPIOIntr_PowerEvent(GPIOIntClientCtxt *pCtxt, DalPowerCmd PowerCmd, 
                    DalPowerDomain PowerDomain)
{
    return DAL_SUCCESS;
}

DALResult 
GPIOIntr_Open(GPIOIntClientCtxt *pCtxt, uint32 dwaccessMode )
{
    return DAL_SUCCESS;
}

DALResult 
GPIOIntr_Close(GPIOIntClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult 
GPIOIntr_Info(GPIOIntClientCtxt *pCtxt,DalDeviceInfo *pdeviceInfo, uint32 dwSize)
{
    return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalGPIOInt Interface. 
------------------------------------------------------------------------------*/
/*==========================================================================

  FUNCTION      GPIOIntr_SetTrigger

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_SetTrigger
( 
  GPIOIntClientCtxt * pCtxt, 
  uint32  gpio, 
  GPIOIntTriggerType  trigger
) 
{
  GPIOIntDevCtxt *device;
  uint32 interrupt_id,direct_connect_trigger;
  device = pCtxt->pGPIOIntDevCtxt;

  /*
   * Validate the given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    ULOG_RT_PRINTF_1 (pCtxt->hGPIOIntLog,
    "GPIOIntr_SetTrigger : This GPIO %d is invalid",gpio);
    return DAL_ERROR;
  }

  /*
   * get the synchronization object.
   */
  qurt_mutex_lock(device->GPIOIntSyncObject);
  
  /*
   * If the interrupt is a direct connect interrupt we need to set the trigger
   * on the main level 1 interrupt controller and set the polarity on the 
   * secondary GPIOInt interrupt controller. In doing so there might be 
   * spurious interrupts generated hence we need to :
   * 1. Disable the interrupt.
   * 2. Set the trigger.
   * 3. Clear it.
   * 4. Enable it.
   */
  if(device->gpioint_cntrl->state[gpio].intr_type != HAL_GPIOINT_SUMMARY)
  {
    interrupt_id = GPIOIntr_GetInterruptId(device,gpio);
    direct_connect_trigger = GPIOIntr_GetDirectIntrTrigger(device,trigger);
    DalInterruptController_InterruptDisable(device->intr_handle,interrupt_id);
    HAL_gpioint_SetDirConnIntrPolarity((HAL_gpioint_TriggerType)trigger,gpio);
    DalInterruptController_SetTrigger(device->intr_handle,interrupt_id, 
    direct_connect_trigger);
    DalInterruptController_InterruptClear(device->intr_handle, interrupt_id);
    DalInterruptController_InterruptEnable(device->intr_handle,interrupt_id);
   
  }
  
  /*
   * The setting of the summary interrupt trigger is kept according to the 
   * legacy implementation.
   */
  else
  {
    HAL_gpioint_SetTrigger(gpio,(HAL_gpioint_TriggerType)trigger);
  }

  /*
   * Update static GPIOInt map with Trigger 
   */ 
  device->gpioint_cntrl->state[gpio].intr_trigger = (HAL_gpioint_TriggerType)trigger;

  /*
   * Setup mao interrup wake up.
   */ 
  #ifdef GPIOINT_USE_MPM
  if( device->gpioint_cntrl->state[gpio].gpio_intr_flags  & GPIOINTF_MPMINT)
  {
    device->gpioint_cntrl->state[gpio].gpio_intr_flags  |= GPIOINTF_WAKEUP;
    GPIOIntr_ConfigMPMWakeup(pCtxt,gpio,(GPIOIntProcessorType)device->gpioint_cntrl->processor);
  }
  #endif /* GPIOINT_USE_MPM */

  qurt_mutex_unlock(device->GPIOIntSyncObject);
  return DAL_SUCCESS;

} /* END GPIOIntr_SetTrigger */


/*==========================================================================

  FUNCTION      GPIOIntr_RegisterIsr

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_RegisterIsr
(
  GPIOIntClientCtxt * pCtxt, 
  uint32  gpio, 
  GPIOIntTriggerType trigger, 
  GPIOINTISR isr,
  GPIOINTISRCtx param
) 
{

  return GPIOIntr_RegisterIsrEx( pCtxt, gpio, trigger, isr, param, 0 );

} /* END GPIOIntr_RegisterIsr */


/*==========================================================================

  FUNCTION      GPIOIntr_RegisterIsrEx

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_RegisterIsrEx
(
  GPIOIntClientCtxt * pCtxt, 
  uint32  gpio, 
  GPIOIntTriggerType trigger, 
  GPIOINTISR isr,
  GPIOINTISRCtx param,
  uint32 nFlags
) 
{
  DALResult result;
  GPIOIntDevCtxt *device;
  
  device = pCtxt->pGPIOIntDevCtxt;

  /*
   * Validate the given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    ULOG_RT_PRINTF_1 (pCtxt->hGPIOIntLog, 
    "GPIOIntr_RegisterIsr : This GPIO %d is not supported",gpio);
    return DAL_ERROR;
  }
  qurt_mutex_lock(device->GPIOIntSyncObject);
  if(((device->gpioint_cntrl->state[gpio].isr != NULL) &&
     (device->gpioint_cntrl->state[gpio].isr != isr)) )
  {
    /*
     * Restore interrupts and return an error.
     */
    qurt_mutex_unlock(device->GPIOIntSyncObject);
    ULOG_RT_PRINTF_1 (pCtxt->hGPIOIntLog, 
    "GPIOIntr_RegisterIsr:This GPIO %d is already assigned",gpio);
    return DAL_ERROR;
  }
  device->gpioint_cntrl->state[gpio].isr = isr;
  device->gpioint_cntrl->state[gpio].isr_param = param; 
  result = GPIOIntr_ConfigureGPIOInterrupt(pCtxt, gpio,trigger,nFlags);
  if(result != DAL_SUCCESS)
  {
    device->gpioint_cntrl->state[gpio].isr = NULL;
    device->gpioint_cntrl->state[gpio].isr_param = 0; 
    ULOG_RT_PRINTF_1 (pCtxt->hGPIOIntLog,
    "GPIOIntr_RegisterIsr:GPIO Configuration failed! for gpio %d",gpio);
  }
  qurt_mutex_unlock(device->GPIOIntSyncObject);
  return result;

} /* END GPIOIntr_RegisterIsr */


/*==========================================================================

  FUNCTION      GPIOIntr_UnConfigureGPIOInterrupt

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

void 
GPIOIntr_UnConfigureGPIOInterrupt
(
  GPIOIntDevCtxt * device, 
  uint32 gpio
)
{
  uint32 interrupt_id;
  HAL_gpioint_InterruptType interrupt;
  interrupt = device->gpioint_cntrl->state[gpio].intr_type;
 
  if(interrupt != HAL_GPIOINT_SUMMARY)
  {
    interrupt_id = GPIOIntr_GetInterruptId(device, gpio);

    /* The Main interrupt controller handles the direct connects */
    DalInterruptController_Unregister(device->intr_handle,interrupt_id);
  }
  
  /*
   * Disable this interrupt be it Direct or Summary and default the information
   * back to summary.
   */
  HAL_gpioint_Disable(gpio);
  device->gpioint_cntrl->state[gpio].intr_type = HAL_GPIOINT_SUMMARY;
  device->gpioint_cntrl->state[gpio].gpio_intr_flags &= ~GPIOINTF_REGISTERED;

  /*
   * Disable mao interrupt wake up.
   */ 
  #ifdef GPIOINT_USE_MPM
  if( device->gpioint_cntrl->state[gpio].gpio_intr_flags  & GPIOINTF_MPMINT)
  {
    device->gpioint_cntrl->state[gpio].gpio_intr_flags  &= ~GPIOINTF_WAKEUP;
    mpmint_disable_gpio_wakeup(gpio);
  }
  #ifdef GPIOINT_USE_NPA
  else
  {
    device->gpioint_cntrl->non_mpm_interrupts --;
    GPIOIntr_UpdateNPARequest(device);
  }
  #endif /* GPIOINT_USE_NPA*/
  #endif /* GPIOINT_USE_MPM */

} /* END GPIOIntr_UnConfigureGPIOInterrupt */


/*==========================================================================

  FUNCTION      GPIOIntr_ConfigureGPIOInterrupt

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_ConfigureGPIOInterrupt
(
  GPIOIntClientCtxt * pCtxt, 
  uint32 gpio,
  GPIOIntTriggerType trigger,
  uint32 nFlags
)
{
  GPIOIntDevCtxt * device;
  uint32 interrupt_id,direct_connect_trigger;
  
  device = pCtxt->pGPIOIntDevCtxt;
  if(trigger == GPIOINT_TRIGGER_DUAL_EDGE)
  {
    device->gpioint_cntrl->state[gpio].intr_type = HAL_GPIOINT_SUMMARY;
    interrupt_id = GPIOINT_NONE;
  }
  else
  {
    interrupt_id =  GPIOIntr_GetFreeDirectConnectInterruptId(device,gpio);
  }

  if( GPIOINT_PRIORITY_ISSET(nFlags) && ( interrupt_id == GPIOINT_NONE ) )
  {
    /* Cannot set priority since there are no available Direct Connects */
    return DAL_ERROR;
  }

  if(device->gpioint_cntrl->state[gpio].intr_type == HAL_GPIOINT_SUMMARY)
  {

    HAL_gpioint_SetTargetProcessor(device->gpioint_cntrl->processor,gpio,
    (HAL_gpioint_InterruptType)device->gpioint_cntrl->state[gpio].intr_type);
    HAL_gpioint_EnableRawStatus(gpio);

    /*
     * Set SUMMARY interrupt trigger if the gpio is not a DIRECT CONNECT.
     */ 
    HAL_gpioint_SetTrigger(gpio,(HAL_gpioint_TriggerType)trigger);

    /*
     * Update static GPIOInt map with Trigger 
     */ 
    device->gpioint_cntrl->state[gpio].intr_trigger = (HAL_gpioint_TriggerType)trigger;

    /*
     * Wait for the configuration to set in. 
     */ 
    busywait(5);
    /*
     * Clear and enable the interrupt
     */
    HAL_gpioint_Clear(gpio);
    HAL_gpioint_Enable(gpio);
  } 
  else
  { 

    HAL_gpioint_SetDirConnIntr(gpio, 
    (HAL_gpioint_InterruptType)device->gpioint_cntrl->state[gpio].intr_type);
    HAL_gpioint_SetDirConnIntrPolarity((HAL_gpioint_TriggerType)trigger,gpio);

    /*
     * Install the client event if there is a client registration or an ISR if the client registers one.
     */
    direct_connect_trigger = GPIOIntr_GetDirectIntrTrigger(device,trigger);
    if( GPIOINT_PRIORITY_ISSET( nFlags ) )
    {
      if(DAL_SUCCESS != DalInterruptController_RegisterISR(device->intr_handle, 
          (DALInterruptID)interrupt_id,(DALISR)device->gpioint_cntrl->state[gpio].isr,
          (DALISRCtx)device->gpioint_cntrl->state[gpio].isr_param,
          DALINTRCTRL_ENABLE_CONFIG(direct_connect_trigger, GPIOINT_GET_PRIORITY(nFlags))))
      {
        return DAL_ERROR;
      }
    }
    else if(DAL_SUCCESS != DalInterruptController_RegisterISR(device->intr_handle, 
    (DALInterruptID)interrupt_id,(DALISR)device->gpioint_cntrl->state[gpio].isr,
    (DALISRCtx)device->gpioint_cntrl->state[gpio].isr_param,direct_connect_trigger))
    {
      return DAL_ERROR;
    }

    /*
     * Update static GPIOInt map with Trigger 
     */ 
    device->gpioint_cntrl->state[gpio].intr_trigger = (HAL_gpioint_TriggerType)trigger;
  }
  
  /*
   * Setup mao interrup wake up.
   */ 
  #ifdef GPIOINT_USE_MPM
  if( device->gpioint_cntrl->state[gpio].gpio_intr_flags  & GPIOINTF_MPMINT)
  {
    device->gpioint_cntrl->state[gpio].gpio_intr_flags  |= GPIOINTF_WAKEUP;
    GPIOIntr_ConfigMPMWakeup(pCtxt,gpio,(GPIOIntProcessorType)device->gpioint_cntrl->processor);
  }
  #ifdef GPIOINT_USE_NPA
  else
  {
    device->gpioint_cntrl->non_mpm_interrupts ++;
    GPIOIntr_UpdateNPARequest(device);
  }
  #endif /* GPIOINT_USE_NPA*/
  #endif /* GPIOINT_USE_MPM */

  device->gpioint_cntrl->state[gpio].gpio_intr_flags |= GPIOINTF_REGISTERED;
  return DAL_SUCCESS;

} /* END GPIOIntr_ConfigureGPIOInterrupt */




/*==========================================================================

  FUNCTION      GPIOIntr_DeregisterIsr

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_DeregisterIsr
( 
  GPIOIntClientCtxt * pCtxt, 
  uint32  gpio, 
  GPIOINTISR isr
) 
{
  GPIOIntDevCtxt *device;

  device = pCtxt->pGPIOIntDevCtxt;
  if (!HAL_gpioint_IsSupported(gpio))
  {
    ULOG_RT_PRINTF_1 (pCtxt->hGPIOIntLog, 
    "GPIOIntr_DeregisterIsr : invalid GPIO %d",gpio);
    return DAL_ERROR;
  }
  
  /*
   * Disable interrupts by accessing the processor's internal sync variable
   */
  qurt_mutex_lock(device->GPIOIntSyncObject);

  /*
   * Check that the ISR is correct
   */
  if (device->gpioint_cntrl->state[gpio].isr != isr)
  {
    ULOG_RT_PRINTF_1 (pCtxt->hGPIOIntLog, 
    "GPIO Interrupt Controller: : invalid GPIO %d",gpio);
    /*
     * Restore interrupts
     */
    qurt_mutex_unlock(device->GPIOIntSyncObject);
    return DAL_ERROR;
  }

  /*
   * Clear out the handler and remove the event. Here the direct connect 
   * interrupt handler is just saved for error checking purposes as it is 
   * largely handled by the Main GPIO interrupt controller.
   */
  device->gpioint_cntrl->state[gpio].isr = NULL;
  GPIOIntr_UnConfigureGPIOInterrupt(device,gpio); 

  /*
   * Restore interrupts
   */
 qurt_mutex_unlock(device->GPIOIntSyncObject);

  return DAL_SUCCESS;

} /* END GPIOIntr_DeregisterIsr */

/*==========================================================================

  FUNCTION      GPIOIntr_Deregister

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_Deregister
( 
  GPIOIntClientCtxt * pCtxt, 
  uint32  gpio
) 
{
  GPIOIntDevCtxt *device;

  device = pCtxt->pGPIOIntDevCtxt;
  if (!HAL_gpioint_IsSupported(gpio))
  {
    ULOG_RT_PRINTF_1 (pCtxt->hGPIOIntLog, 
    "GPIOIntr_DeregisterIsr : invalid GPIO %d",gpio);
    return DAL_ERROR;
  }
  
  /*
   * Disable interrupts by accessing the processor's internal sync variable
   */
  qurt_mutex_lock(device->GPIOIntSyncObject);


  /*
   * Clear out the handler and remove the event. Here the direct connect 
   * interrupt handler is just saved for error checking purposes as it is 
   * largely handled by the Main GPIO interrupt controller.
   */
  device->gpioint_cntrl->state[gpio].isr = NULL;
  GPIOIntr_UnConfigureGPIOInterrupt(device,gpio); 

  /*
   * Restore interrupts
   */
 qurt_mutex_unlock(device->GPIOIntSyncObject);

  return DAL_SUCCESS;

} /* END GPIOIntr_Deregister */

/*==========================================================================

  FUNCTION      GPIOIntr_IsInterruptEnabled

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_IsInterruptEnabled
(
  GPIOIntClientCtxt *pCtxt, 
  uint32  gpio, 
  uint32 *  state
) 
{
  uint32 irq,enabled;
  HAL_gpioint_InterruptType interrupt;
  GPIOIntDevCtxt *device;

  device = pCtxt->pGPIOIntDevCtxt;

  /*
   * Validate the given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    ULOG_RT_PRINTF_1 (pCtxt->hGPIOIntLog, 
      "GPIOIntr_IsInterruptEnabled : invalid GPIO %d",gpio);
    return DAL_ERROR;
  }

  interrupt = device->gpioint_cntrl->state[gpio].intr_type;
  if(interrupt != HAL_GPIOINT_SUMMARY)
  {
    irq = GPIOIntr_GetInterruptId(device,gpio);
    DalInterruptController_IsInterruptEnabled(device->intr_handle, 
                                     (DALInterruptID)irq,&enabled);
    *state = enabled;
    return DAL_SUCCESS;
  } 
  /*
   * Check the HAL for summary interrupts.
   */
  *state = (uint32)HAL_gpioint_IsEnabled(gpio);
  
  return DAL_SUCCESS;

} /* GPIOIntr_IsInterruptEnabled */


/*==========================================================================

  FUNCTION      GPIOIntr_IsInterruptPending

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_IsInterruptPending
( 
  GPIOIntClientCtxt *pCtxt, 
  uint32  gpio, 
  uint32 *  state 
) 
{  
  uint32 irq,pending;
  HAL_gpioint_InterruptType interrupt;
  GPIOIntDevCtxt *device;

  device = pCtxt->pGPIOIntDevCtxt;

  /*
   * Validate the given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    ULOG_RT_PRINTF_1 (pCtxt->hGPIOIntLog, 
    "GPIOIntr_IsInterruptPending : invalid GPIO %d",gpio);
    return DAL_ERROR;
  }

  interrupt = device->gpioint_cntrl->state[gpio].intr_type; 
  if(interrupt != HAL_GPIOINT_SUMMARY)
  {
    irq = GPIOIntr_GetInterruptId(device,gpio);
    DalInterruptController_IsInterruptPending(device->intr_handle, 
                                     (DALInterruptID)irq,&pending);
    *state = pending;
     return DAL_SUCCESS;
  }
  else
  {

    /*
     * Check the HAL for summary interrupts.
     */
    *state = (uint32)HAL_gpioint_IsPending(gpio);
  }
  return DAL_SUCCESS;

} /* END GPIOIntr_IsInterruptPending */


/*==========================================================================

  FUNCTION      GPIOIntr_IsInterruptSet

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_IsInterruptSet
( 
  GPIOIntClientCtxt *pCtxt, 
  uint32  gpio, 
  uint32 *state 
) 
{  
  uint32 irq;
  DALResult bSet;
  HAL_gpioint_InterruptType interrupt;
  GPIOIntDevCtxt *device;

  device = pCtxt->pGPIOIntDevCtxt;

  /*
   * Validate the given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    ULOG_RT_PRINTF_1 (pCtxt->hGPIOIntLog, 
    "GPIOIntr_IsInterruptPending : invalid GPIO %d",gpio);
    return DAL_ERROR;
  }

  interrupt = device->gpioint_cntrl->state[gpio].intr_type; 
  if(interrupt != HAL_GPIOINT_SUMMARY)
  {
    irq = GPIOIntr_GetInterruptId(device,gpio);
    bSet = DalInterruptController_InterruptStatus(device->intr_handle, 
                                     (DALInterruptID)irq);
    if(bSet == DAL_INTERRUPT_SET)
    {
      *state = 1;
    }
    else
    {
      *state = 0;
    }
     return DAL_SUCCESS;
  }
  else
  {

    /*
     * Check the HAL for summary interrupts.
     */
    *state = (uint32)HAL_gpioint_IsSet(gpio);
  }
  return DAL_SUCCESS;

} /* END GPIOIntr_IsInterruptSet */


/*==========================================================================

  FUNCTION      GPIOIntr_GetFreeDirectConnectInterruptId

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

uint32 
GPIOIntr_GetFreeDirectConnectInterruptId
(
  GPIOIntDevCtxt *device, 
  uint32 gpio
) 
{

  uint32 index;

  /* 
   * We first check whether we already have a free direct connect interrupt
   * Assigned. if not we take the next available direct connect interrupt.
   */

  for(index =0;index < device->gpioint_cntrl->direct_intr_number; index++)
  {
    if(device->gpioint_cntrl->gpioint_config_map[index].gpio == gpio)
    {
      device->gpioint_cntrl->state[gpio].intr_type = 
      device->gpioint_cntrl->gpioint_config_map[index].interrupt;
      return device->gpioint_cntrl->gpioint_config_map[index].interrupt_id;
    }
  }
  for(index =0;index < device->gpioint_cntrl->direct_intr_number; index++)
  {
    if(device->gpioint_cntrl->gpioint_config_map[index].gpio == GPIOINT_NONE)
    {
      device->gpioint_cntrl->state[gpio].intr_type = 
      device->gpioint_cntrl->gpioint_config_map[index].interrupt;
      device->gpioint_cntrl->gpioint_config_map[index].gpio = gpio;
      return device->gpioint_cntrl->gpioint_config_map[index].interrupt_id;
    }
  }

  return (uint32)GPIOINT_NONE;

} /* END GPIOIntr_GetFreeDirectConnectInterruptId */


/*==========================================================================

  FUNCTION      GPIOIntr_GetInterruptId

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

uint32 
GPIOIntr_GetInterruptId
(
  GPIOIntDevCtxt *device, 
  uint32 gpio
) 
{

  uint32 index;
  /* 
   * We first check whether we have a free direct connect interrupt 
   */
  if(device->gpioint_cntrl->state[gpio].intr_type != HAL_GPIOINT_SUMMARY)
  {
    for(index =0;index < device->gpioint_cntrl->direct_intr_number; index++)
    {
      if(device->gpioint_cntrl->gpioint_config_map[index].gpio == gpio)
      {
        return device->gpioint_cntrl->gpioint_config_map[index].interrupt_id;
      }
    }
  }
  return (uint32)GPIOINT_NONE;

} /* END GPIOIntr_GetInterruptId */

/*==========================================================================

  FUNCTION      GPIOIntr_ReleaseDirectConnectInterrupt

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

void 
GPIOIntr_ReleaseDirectConnectInterrupt
(
  GPIOIntDevCtxt *device, 
  uint32 interrupt_id
) 
{
  uint32 index;

 for(index =0;index < device->gpioint_cntrl->direct_intr_number; index++)
 {
   if(device->gpioint_cntrl->gpioint_config_map[index].interrupt_id == 
    device->gpioint_cntrl->gpioint_config_map[index].interrupt_id)
   {
      device->gpioint_cntrl->gpioint_config_map[index].gpio = GPIOINT_NONE;
      break;
   }
 }
  return;

} /* GPIOIntr_ReleaseDirectConnectInterrupt */


/*==========================================================================

  FUNCTION      GPIOIntr_Isr

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

void 
GPIOIntr_Isr
(
  void *pCtxt
)
{
  uint32 gpio, group;
  GPIOIntDevCtxt *device = (GPIOIntDevCtxt *)pCtxt;
  
    /*
     * Retrieve the interrupt from the hardware
     */
    group = device->gpioint_cntrl->summary_param;
    HAL_gpioint_GetPending((HAL_gpioint_GroupType)group, &gpio);

    /*
     * If nothing pending we are done
     */
    while (gpio != HAL_GPIOINT_NONE)
    {
      /*
       * Run the ISR
       */
      GPIOIntr_RunIsr(device,gpio);

      /*
       * Retrieve the next interrupt from the hardware
       */
      HAL_gpioint_GetPending((HAL_gpioint_GroupType)group, &gpio);
    }


} /* END GPIOIntr_isr */


/*==========================================================================

  FUNCTION      GPIOIntr_RunIsr

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

void 
GPIOIntr_RunIsr
(
  GPIOIntDevCtxt *device,
  uint32 gpio
)
{ 
  boolean bIsEdgeTriggered = TRUE;
  HAL_gpioint_TriggerType nTrigger = HAL_GPIOINT_TRIGGER_HIGH;

  /* 
   * Get the trigger type to determine when to clear the interrupt
   */
  HAL_gpioint_GetTrigger( gpio, &nTrigger );

  if( nTrigger == HAL_GPIOINT_TRIGGER_HIGH ||
      nTrigger == HAL_GPIOINT_TRIGGER_LOW )
  {
    bIsEdgeTriggered = FALSE;
  }

  /*
   * Log the GPIO that fired
   */
  GPIOIntr_LogEvent(device,gpio);

  /*
   * Clear the interrupt before if it's edge triggered
   */
  if( bIsEdgeTriggered )
  {
    HAL_gpioint_Clear(gpio);
  }

  /*
   * Get the ISR and run it
   */
  if(device->gpioint_cntrl->state[gpio].isr == NULL)
  {
    GPIOIntr_DefaultIsr(device,gpio);
  }
  else 
  {
    device->gpioint_cntrl->state[gpio].isr(device->gpioint_cntrl->state[gpio].isr_param);
  }

  /*
   * Clear the gpio interrupt if it is not owned by the apps processor.
   * During power collapse, the apps interrupt will be preserved to trigger
   * an apps interrupt once gpios are switched back to the apps processor.
   * The apps GPIO monitoring ISR (GPIOIntr_monitor_apps_isr) has disabled
   * the interrupt at this point. 
   * Clear the interrupt after the ISR is called if it's level triggered. 
   */
  if( !bIsEdgeTriggered )
  {
    HAL_gpioint_Clear(gpio);
  }

} /* END GPIOIntr_run_isr */


/*==========================================================================

  FUNCTION      GPIOIntr_LogEvent

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

void 
GPIOIntr_LogEvent
(
  GPIOIntDevCtxt *device,
  uint32 gpio
)
{

  GPIOIntCntrlType *gpioint_cntrl = device->gpioint_cntrl;
  uint32 index = gpioint_cntrl->log.index;
  gpioint_cntrl->log.entries[index].gpio = gpio;
  DalTimetick_GetTimetick64(device->timetick_handle, &gpioint_cntrl->log.entries[index].timestamp);

  gpioint_cntrl->state[gpio].last =
  gpioint_cntrl->log.entries[index].timestamp;
  gpioint_cntrl->state[gpio].cnt++;

  if (++gpioint_cntrl->log.index >= MAX_INTERRUPT_LOG_ENTRIES)
  {
    gpioint_cntrl->log.index = 0;
  }

} /* END GPIOIntr_log_event */

/*==========================================================================

  FUNCTION      GPIOIntr_ConfigMaointWakeup

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

void
GPIOIntr_ConfigMPMWakeup
(
  GPIOIntClientCtxt * pCtxt, 
  uint32 gpio, 
  GPIOIntProcessorType processor
)
{
  #ifdef GPIOINT_USE_MPM
  HAL_gpioint_TriggerType  trigger;
  mpmint_processor_type    mpm_proc;
  mpmint_detect_type       mao_int_detect;
  mpmint_polarity_type     mao_int_polarity;
  GPIOIntDevCtxt *         device;
  
  device   = pCtxt->pGPIOIntDevCtxt;
  mpm_proc = GPIOIntr_GetMPMProcId((GPIOIntProcessorType)processor);
  
  trigger = device->gpioint_cntrl->state[gpio].intr_trigger;
  pCtxt->pGPIOIntDevCtxt->gpioint_cntrl->state[gpio].gpio_intr_flags |= GPIOINTF_MPMINT;
  /*
   * Determine the MAO interrupt configuration
   */
  switch (trigger)
  {

    case HAL_GPIOINT_TRIGGER_DUAL_EDGE:
      mao_int_detect = MPMINT_EDGE_DETECT;
      mao_int_polarity = MPMINT_ACTIVE_DUAL;
      break;

    case HAL_GPIOINT_TRIGGER_HIGH:
      mao_int_detect = MPMINT_LEVEL_DETECT;
      mao_int_polarity = MPMINT_ACTIVE_HIGH;
      break;

    case HAL_GPIOINT_TRIGGER_LOW:
      mao_int_detect = MPMINT_LEVEL_DETECT;
      mao_int_polarity = MPMINT_ACTIVE_LOW;
      break;

    case HAL_GPIOINT_TRIGGER_RISING:
      mao_int_detect = MPMINT_EDGE_DETECT;
      mao_int_polarity = MPMINT_ACTIVE_HIGH;
      break;

    case HAL_GPIOINT_TRIGGER_FALLING:
      mao_int_detect = MPMINT_EDGE_DETECT;
      mao_int_polarity = MPMINT_ACTIVE_LOW;
      break;

    default:
      mao_int_detect = MPMINT_LEVEL_DETECT;
      mao_int_polarity = MPMINT_ACTIVE_HIGH;
      break;
  }

  /*
   * Inform MAO the GPIO interrupt is enabled
   */
  mpmint_config_gpio_wakeup(gpio, mao_int_detect,mao_int_polarity, mpm_proc);
  #endif

} /* END GPIOIntr_config_maoint_wakeup */


/*==========================================================================

  FUNCTION      GPIOIntr_DefaultIsr

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

void 
GPIOIntr_DefaultIsr
(
  GPIOIntDevCtxt *device,
  uint32 gpio
)
{
  qurt_mutex_lock(device->GPIOIntSyncObject);
  device->gpioint_cntrl->state[gpio].unhandled++ ;
  HAL_gpioint_Disable(gpio);
  qurt_mutex_unlock(device->GPIOIntSyncObject);
} /* END GPIOIntdefault_isr */


/*==========================================================================

  FUNCTION      GPIOIntr_GetDirectIntrTrigger

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

uint32 
GPIOIntr_GetDirectIntrTrigger
(
  GPIOIntDevCtxt *device,
  GPIOIntTriggerType trigger
)
{
  uint32 direct_intr_trigger;

  switch(trigger)
  {
    case GPIOINT_TRIGGER_HIGH:
      direct_intr_trigger = DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER;
      break;
    case GPIOINT_TRIGGER_LOW:
      /*
       * This check is added because Polarity for the  main interrupt 
       * controller may not be negative. Hence a trigger translation is 
       * needed. 
       */
      if((device->gpioint_cntrl->interrupt_controller == GPIOINT_QGIC) ||
      (device->gpioint_cntrl->interrupt_controller == GPIOINT_L2VIC))
      {
        direct_intr_trigger = DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER;
      }
      else
      {
        direct_intr_trigger = DALINTRCTRL_ENABLE_LEVEL_LOW_TRIGGER;
      }
      break;
    case GPIOINT_TRIGGER_RISING:
      direct_intr_trigger = DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER;
      break;
    case GPIOINT_TRIGGER_FALLING:
       if((device->gpioint_cntrl->interrupt_controller == GPIOINT_QGIC) ||
      (device->gpioint_cntrl->interrupt_controller == GPIOINT_L2VIC))
      {
        direct_intr_trigger = DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER;
      }
      else
      {
        direct_intr_trigger = DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER;
      }
      break;
    default:
      direct_intr_trigger = DALINTRCTRL_ENABLE_DEFAULT_SETTINGS;
      break;
  }
  return direct_intr_trigger;

} /* END GPIOIntr_GetDirectIntrTrigger */


/*==========================================================================

  FUNCTION      GPIOIntr_Restore

  DESCRIPTION   See GPIOInt.h

==========================================================================*/
DALResult GPIOIntr_Restore
( 
  GPIOIntClientCtxt * pCtxt
)
{
    return DAL_SUCCESS;
}

/*==========================================================================

  FUNCTION      GPIOIntr_GetMPMProcId

  DESCRIPTION   See GPIOInt.h

==========================================================================*/
#ifdef GPIOINT_USE_MPM
mpmint_processor_type
GPIOIntr_GetMPMProcId(GPIOIntProcessorType processor)
{ 
  switch(processor)
  {
    case GPIOINT_DEVICE_MODEM :
      return MPMINT_MODEM;  
    case GPIOINT_DEVICE_APPS :
      return MPMINT_APPS;   
    default :
      return MPMINT_MODEM;   
  }

} /* END GPIOIntr_GetMPMProcId */
#endif


/*==========================================================================

  FUNCTION      GPIOIntr_DisableInterrupt

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_DisableInterrupt
( 
  GPIOIntClientCtxt * pCtxt, 
  uint32 gpio
)
{
  GPIOIntDevCtxt *device;
  uint32 interrupt_id;
  device = pCtxt->pGPIOIntDevCtxt;

  /*
   *  Validate given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    ULOG_RT_PRINTF_1 (pCtxt->hGPIOIntLog, 
    "GPIOIntr_DisableInterrupt :This GPIO %d to be triggered is not supported",gpio);
    return DAL_ERROR;
  }

  qurt_mutex_lock(device->GPIOIntSyncObject);
  if(device->gpioint_cntrl->state[gpio].intr_type != HAL_GPIOINT_SUMMARY)
  {
    HAL_gpioint_DisableDirConnIntr(gpio);
    interrupt_id = GPIOIntr_GetInterruptId(device,gpio);
    DalInterruptController_InterruptDisable(device->intr_handle,interrupt_id);
  }
  else
  {
    HAL_gpioint_Disable(gpio);
  }

  /*
   * Disable mao interrup wake up.
   */ 
  #ifdef GPIOINT_USE_MPM
  if( device->gpioint_cntrl->state[gpio].gpio_intr_flags  & GPIOINTF_MPMINT)
  {
    device->gpioint_cntrl->state[gpio].gpio_intr_flags  &= ~GPIOINTF_WAKEUP;
    mpmint_disable_gpio_wakeup(gpio);
  }
  #ifdef GPIOINT_USE_NPA
  else
  {
    device->gpioint_cntrl->non_mpm_interrupts --;
    GPIOIntr_UpdateNPARequest(device);
  }
  #endif /* GPIOINT_USE_NPA*/
  #endif /* GPIOINT_USE_MPM */

  qurt_mutex_unlock(device->GPIOIntSyncObject);
  return DAL_SUCCESS;

} /* END GPIOIntr_DisableInterrupt */


/*==========================================================================

  FUNCTION      GPIOIntr_EnableInterrupt

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_EnableInterrupt
(
  GPIOIntClientCtxt * pCtxt, 
  uint32 gpio
)
{
  GPIOIntDevCtxt *device;
  uint32 interrupt_id;

  device = pCtxt->pGPIOIntDevCtxt;

  /*
   *  Validate given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    ULOG_RT_PRINTF_1 (pCtxt->hGPIOIntLog, 
    "GPIO Interrupt Controller: This GPIO %d to be triggered is not supported",gpio);
    return DAL_ERROR;
  }
  qurt_mutex_lock(device->GPIOIntSyncObject);
  if(device->gpioint_cntrl->state[gpio].intr_type != HAL_GPIOINT_SUMMARY)
  {
    HAL_gpioint_SetDirConnIntr(gpio,device->gpioint_cntrl->state[gpio].intr_type);
    interrupt_id = GPIOIntr_GetInterruptId(device,gpio);
    DalInterruptController_InterruptEnable(device->intr_handle,interrupt_id);
  }
  else
  {  
    HAL_gpioint_Enable(gpio);
  }

  /*
   * Setup mao interrup wake up.
   */ 
  #ifdef GPIOINT_USE_MPM
  if( device->gpioint_cntrl->state[gpio].gpio_intr_flags  & GPIOINTF_MPMINT)
  {
    device->gpioint_cntrl->state[gpio].gpio_intr_flags  |= GPIOINTF_WAKEUP;
    GPIOIntr_ConfigMPMWakeup(pCtxt,gpio,(GPIOIntProcessorType)device->gpioint_cntrl->processor);
  }
  #ifdef GPIOINT_USE_NPA
  else
  {
    device->gpioint_cntrl->non_mpm_interrupts ++;
    GPIOIntr_UpdateNPARequest(device);
  }
  #endif /* GPIOINT_USE_NPA*/
  #endif /* GPIOINT_USE_MPM */

  qurt_mutex_unlock(device->GPIOIntSyncObject);
  return DAL_SUCCESS;

} /* END GPIOIntr_EnableInterrupt */


/*==========================================================================

  FUNCTION      GPIOIntr_MapMPMInterrupts

  DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_MapMPMInterrupt
(
  GPIOIntClientCtxt * pCtxt,
  uint32 gpio, 
  uint32 mpm_interrupt_id
) 
{
   GPIOIntDevCtxt *device;
   device = (GPIOIntDevCtxt *)pCtxt->pGPIOIntDevCtxt;

  /*
   * Validate the given gpio interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    ULOG_RT_PRINTF_1 (pCtxt->hGPIOIntLog, 
    "GPIOIntr_MapMPMInterrupts: This GPIO %d is not supported",gpio);
     return DAL_ERROR;
  }
  else
  {
    device->gpioint_cntrl->state[gpio].mpm_intr_id = mpm_interrupt_id;                      
    device->gpioint_cntrl->state[gpio].gpio_intr_flags  |= GPIOINTF_MPMINT;     
    if(device->gpioint_cntrl->state[gpio].gpio_intr_flags  & GPIOINTF_REGISTERED)
    {
      #ifdef GPIOINT_USE_MPM
      GPIOIntr_ConfigMPMWakeup(pCtxt,gpio,(GPIOIntProcessorType)device->gpioint_cntrl->processor);
      #ifdef GPIOINT_USE_NPA
      device->gpioint_cntrl->non_mpm_interrupts --;
      GPIOIntr_UpdateNPARequest(device);
      #endif /* GPIOINT_USE_NPA*/
      #endif /* GPIOINT_USE_MPM */
    }
  }                            
  return DAL_SUCCESS;
} /* END GPIOIntr_MapMPMInterrupts */


/*==========================================================================

  FUNCTION      GPIOIntr_InterruptNotify

  DESCRIPTION   See GPIOInt.h

==========================================================================*/
DALResult 
GPIOIntr_InterruptNotify(GPIOIntClientCtxt * pCtxt, uint32  gpio) 
{
  
  return DAL_SUCCESS;
}


/*==========================================================================

   FUNCTION      GPIOIntr_ClearInterrupt

   DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_ClearInterrupt
(
  GPIOIntClientCtxt * pCtxt, 
  uint32 gpio
)
{
  GPIOIntDevCtxt *device;
  uint32 interrupt_id;
  
  device = pCtxt->pGPIOIntDevCtxt;

  /*
   *  Validate the given GPIO interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    ULOG_RT_PRINTF_1 (pCtxt->hGPIOIntLog, 
    "GPIOIntr_ClearInterrupt : This GPIO %d is not supported",gpio);
    return DAL_ERROR;
  }
  qurt_mutex_lock(device->GPIOIntSyncObject);
  if(device->gpioint_cntrl->state[gpio].intr_type != HAL_GPIOINT_SUMMARY)
  {
    interrupt_id = GPIOIntr_GetInterruptId(device,gpio);
    DalInterruptController_InterruptClear(device->intr_handle,interrupt_id);
  }
  else
  {  
    HAL_gpioint_Clear(gpio);
  }
  qurt_mutex_unlock(device->GPIOIntSyncObject);
  return DAL_SUCCESS;

} /* END GPIOIntr_ClearInterrupt */


/*==========================================================================

   FUNCTION      GPIOIntr_TriggerInterrupt

   DESCRIPTION   See GPIOInt.h

==========================================================================*/

DALResult 
GPIOIntr_TriggerInterrupt
(
  GPIOIntClientCtxt * pCtxt, 
  uint32  gpio
)
{
  uint32 interrupt_id;
  GPIOIntDevCtxt *device;

  device = (GPIOIntDevCtxt *)pCtxt->pGPIOIntDevCtxt;

  /*
   * Validate the given gpio interrupt
   */
  if (!HAL_gpioint_IsSupported(gpio))
  {
    ULOG_RT_PRINTF_1 (pCtxt->hGPIOIntLog, 
    "GPIO Interrupt Controller: This GPIO %d is not supported",gpio);
    return DAL_ERROR;
  }

  if(device->gpioint_cntrl->state[gpio].isr == NULL)
    
  {
    /*
     * This GPIO is not registered for a handler.
     */
    ULOG_RT_PRINTF_1 (pCtxt->hGPIOIntLog, 
    "GPIOIntr_TriggerInterrupt:This GPIO %d does not have a client ",gpio);
    return DAL_ERROR;
  }
  /*
   * Disable interrupts
   */
  qurt_mutex_lock(device->GPIOIntSyncObject);
  if(device->gpioint_cntrl->state[gpio].intr_type == HAL_GPIOINT_SUMMARY)
  {
    HAL_gpioint_TriggerInterrupt(gpio);
  }
  else
  {
    interrupt_id = GPIOIntr_GetInterruptId(device,gpio);
    DalInterruptController_InterruptTrigger( device->intr_handle,interrupt_id);
  }

  /* 
   * Restore interrupts
   */
  qurt_mutex_unlock(device->GPIOIntSyncObject);
  return DAL_SUCCESS;
                                        
} /* END GPIOIntr_TriggerInterrupt */


/*==========================================================================

   FUNCTION      GPIOInt_Init

   DESCRIPTION   See DDIGPIOInt.h

==========================================================================*/
void GPIOInt_Init(void)
{

  DalDeviceHandle *  gpioint_handle;

  GPIOIntData.GPIOInt_Init = TRUE;
  /* 
   * attach to the gpioint dal.
   */
  DAL_DeviceAttach(DALDEVICEID_GPIOINT, &gpioint_handle);
     
  
} /* END GPIOInt_Init */
