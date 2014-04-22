/**
 * @file pcie_gpio.c
 * @brief
 * Handles all PCIe GPIOs
 */

/*
===============================================================================

Edit History

$Header:

Change revision history maintained in version system
===============================================================================
Copyright (c) 2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "pcie_gpio.h"
#include "DDITlmm.h"
#include "pcie_i.h"
#ifndef PCIE_BOOT
#include "DDIGPIOInt.h"
#endif

/** GPIO control Handle Object */
typedef struct
{
   DalDeviceHandle    *h_pcie_tlmm;         /** < -- TLMM handle */
   DalDeviceHandle    *h_pcie_gpio_int;     /** < -- Interrupt handle */
   DALGpioSignalType  pcie_gpio_pin;        /** < -- Holds the GPIO signal type */
} pcie_gpio_handle_ctrl_type;

#ifndef PCIE_BOOT
/** PCIe GPIO Control Handle */
static pcie_gpio_handle_ctrl_type *pcie_gpio_handle;
#endif

/** Prototypes */
#ifndef PCIE_BOOT
/* Performs specific gpio initializations */
static void pcie_gpioi_init(pcie_gpio_cfg_type *pcie_gpio_cfg_ptr, uint32 index);
#endif

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_gpioi_init
** ============================================================================
*/
/**
 * Performs specific gpio initializations.
 *
 * @param[in,out]  pcie_gpio_cfg_ptr    gpio configuration
 * @param[in]      index                ID of the gpio
 *
 * @return         void
 */
static void pcie_gpioi_init(pcie_gpio_cfg_type *pcie_gpio_cfg_ptr, uint32 index)
{
   DALResult           dal_result;

   /* TLMM configuration */
   if(TRUE == pcie_gpio_cfg_ptr->tlmm_cfg)
   {
      dal_result = DAL_DeviceAttach(DALDEVICEID_TLMM, &pcie_gpio_handle[index].h_pcie_tlmm);

      if(DAL_SUCCESS != dal_result)
      {
         pcie_osal_debug(MSG_FATAL, "DAL_DeviceAttach with DALDEVICEID_TLMM failed for gpio %d", pcie_gpio_cfg_ptr->gpio_num);
         return;
      }

      if(!pcie_gpio_handle[index].h_pcie_tlmm)
      {
         pcie_osal_debug(MSG_FATAL, "h_pcie_tlmm is NULL for gpio %d", pcie_gpio_cfg_ptr->gpio_num);
         return;
      }
   }

   /* Interrupt configuration */
   if(TRUE == pcie_gpio_cfg_ptr->int_cfg)
   {
      dal_result = DAL_DeviceAttach(DALDEVICEID_GPIOINT, &pcie_gpio_handle[index].h_pcie_gpio_int);

      if(DAL_SUCCESS != dal_result)
      {
         pcie_osal_debug(MSG_FATAL, "DAL_DeviceAttach with DALDEVICEID_GPIOINT failed for gpio %d", pcie_gpio_cfg_ptr->gpio_num);
         return;
      }

      if(!pcie_gpio_handle[index].h_pcie_gpio_int)
      {
         pcie_osal_debug(MSG_FATAL, "h_pcie_gpio_int is NULL for gpio %d", pcie_gpio_cfg_ptr->gpio_num);
         return;
      }
   }

   /* Inactive configuration */
   if(NULL != pcie_gpio_cfg_ptr->inactive_cfg)
   {
      dal_result = DalTlmm_SetInactiveConfig(
                                             pcie_gpio_handle[index].h_pcie_tlmm,
                                             pcie_gpio_cfg_ptr->gpio_num,
                                             pcie_gpio_cfg_ptr->inactive_cfg);
      if(DAL_SUCCESS != dal_result)
      {
         pcie_osal_debug(MSG_FATAL, "DalTlmm_SetInactiveConfig failed for gpio %d", pcie_gpio_cfg_ptr->gpio_num);
         return;
      }
   }

   pcie_gpio_handle[index].pcie_gpio_pin = DAL_GPIO_CFG(pcie_gpio_cfg_ptr->gpio_num,
                                                        pcie_gpio_cfg_ptr->func,
                                                        pcie_gpio_cfg_ptr->dir,
                                                        pcie_gpio_cfg_ptr->pull,
                                                        pcie_gpio_cfg_ptr->drive);

   /* Configure the GPIO with all configurations above */
   dal_result = DalTlmm_ConfigGpio(pcie_gpio_handle[index].h_pcie_tlmm,
                                   pcie_gpio_handle[index].pcie_gpio_pin,
                                   (DALGpioEnableType)pcie_gpio_cfg_ptr->init_enable);
   if(DAL_SUCCESS != dal_result)
   {
      pcie_osal_debug(MSG_FATAL, "DalTlmm_ConfigGpio failed for gpio %d", pcie_gpio_cfg_ptr->gpio_num);
      return;
   }

   /* Set the output value if the gpio direction is Output */
   if(DAL_GPIO_OUTPUT == pcie_gpio_cfg_ptr->dir)
   {
      dal_result = DalTlmm_GpioOut(pcie_gpio_handle[index].h_pcie_tlmm,
                                   pcie_gpio_handle[index].pcie_gpio_pin,
                                   (DALGpioValueType)pcie_gpio_cfg_ptr->default_output_val);

      if(DAL_SUCCESS != dal_result)
      {
         pcie_osal_debug(MSG_FATAL, "DalTlmm_GpioOut failed for gpio %d", pcie_gpio_cfg_ptr->gpio_num);
         return;
      }
   }
}
#endif /* PCIE_BOOT */

/* ============================================================================
**  Function : pcie_gpio_init
** ============================================================================
*/
/**
 * Performs gpio initializations.
 *
 * @param[in]      void
 *
 * @return         pcie_return_status
 *    Status code.
 */
pcie_return_status pcie_gpio_init(void)
{
#ifndef PCIE_BOOT
   uint32 pcie_gpio_config_index;
#endif /* PCIE_BOOT */

#ifndef PCIE_BOOT
   pcie_gpio_handle = (pcie_gpio_handle_ctrl_type*)pcie_osal_malloc(sizeof(pcie_gpio_handle_ctrl_type) * PCIE_MAX_GPIO);

   if(NULL == pcie_gpio_handle)
   {
      pcie_osal_debug(MSG_FATAL, "Allocation failed for pcie_gpio_handle");
      return(PCIE_FAILED);
   }

   pcie_osal_memset(pcie_gpio_handle, 0, sizeof(pcie_gpio_handle_ctrl_type) * PCIE_MAX_GPIO);

   /* Initalize all GPIOs */
   for(pcie_gpio_config_index = 0;pcie_gpio_config_index < PCIE_MAX_GPIO;pcie_gpio_config_index++)
   {
      pcie_gpioi_init(&pcie_dev->pcie_gpio_config_ptr[pcie_gpio_config_index], pcie_gpio_config_index);
   }
#endif /* PCIE_BOOT */
   return(PCIE_SUCCESS);
}

/* ============================================================================
**  Function : pcie_gpio_get_state
** ============================================================================
*/
/**
 * Reads the current gpio state
 *
 * @param[in]   gpio_id -  GPIO ID
 *
 * @return      pcie_gpio_pin_value_type
 *    PCIE_GPIO_HIGH - if GPIO pin is high
 *    PCIE_GPIO_LOW  - if GPIO pin is low
 *
 * @dependencies
 * None.
 */
pcie_gpio_pin_value_type pcie_gpio_get_state(pcie_gpio_id_type gpio_id)
{
#ifndef PCIE_BOOT
   DALResult dal_result;
   DALGpioValueType pin_state;

   if(NULL == pcie_gpio_handle[gpio_id].h_pcie_tlmm)
   {
      pcie_osal_debug(MSG_FATAL, "h_pcie_tlmm is NULL for gpio %d", pcie_dev->pcie_gpio_config_ptr[gpio_id].gpio_num);
   }

   dal_result = DalTlmm_GpioIn(pcie_gpio_handle[gpio_id].h_pcie_tlmm,
                               pcie_gpio_handle[gpio_id].pcie_gpio_pin,
                               &pin_state);

   if(DAL_SUCCESS != dal_result)
      pcie_osal_debug(MSG_FATAL, "Failed to retrieve gpio state for gpio ");

   return(pin_state ? PCIE_GPIO_HIGH : PCIE_GPIO_LOW);
#else
   return(PCIE_GPIO_HIGH);
#endif /* PCIE_BOOT */
}

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_gpio_set_state
** ============================================================================
*/
/**
 * Sets the gpio state to the high/low
 *
 * @param[in]   gpio_id     GPIO ID
 * @param[in]   val         Value to be set
 *
 * @return      void
 */
void pcie_gpio_set_state(pcie_gpio_id_type gpio_id, pcie_gpio_pin_value_type val)
{
   DALResult dRes;

   if(NULL == pcie_gpio_handle[gpio_id].h_pcie_tlmm)
   {
      pcie_osal_debug(MSG_FATAL, "h_pcie_tlmm is NULL for gpio %d", pcie_dev->pcie_gpio_config_ptr[gpio_id].gpio_num);
   }

   dRes = DalTlmm_GpioOut(pcie_gpio_handle[gpio_id].h_pcie_tlmm,
                          pcie_gpio_handle[gpio_id].pcie_gpio_pin,
                          (DALGpioValueType)val);
   if(DAL_SUCCESS != dRes)
   {
      pcie_osal_debug(MSG_FATAL, "DalTlmm_GpioOut failed");
   }
}
#endif /* PCIE_BOOT */

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_gpio_interrupt_set_trigger
** ============================================================================
*/
/**
 * Sets the trigger for the gpio interrupt.
 *
 * @param[in]      gpio_id     ID of the gpio
 * @param[in]      trigger     Trigger for the ISR
 *
 * @return         void
 */
void pcie_gpio_interrupt_set_trigger(pcie_gpio_id_type gpio_id, pcie_gpio_int_trigger_type trigger)
{
   DALResult           dal_result;

   dal_result = GPIOInt_SetTrigger(
                                   pcie_gpio_handle[gpio_id].h_pcie_gpio_int,
                                   pcie_dev->pcie_gpio_config_ptr[gpio_id].gpio_num,
                                   (GPIOIntTriggerType)trigger);

   if(DAL_SUCCESS != dal_result)
   {
      pcie_osal_debug(MSG_FATAL, "GPIOInt_SetTrigger failed for gpio %d", pcie_dev->pcie_gpio_config_ptr[gpio_id].gpio_num);
   }
}
#endif /* PCIE_BOOT */

/* ============================================================================
**  Function : pcie_gpio_register_isr
** ============================================================================
*/
/**
 * Register PCIe interrupt handler and sets the trigger.
 *
 * @param[in]        gpio_id     ID of the gpio
 * @param[in,out]    isr         ISR to be registered
 * @param[in]        trigger     Trigger for the ISR
 * @param[in]        param       Parameter for the ISR
 *
 * @return           void
 */
void pcie_gpio_register_isr(pcie_gpio_id_type gpio_id, void *isr, pcie_gpio_int_trigger_type trigger, uint32 param)
{
#ifndef PCIE_BOOT
   DALResult           dal_result;

   if(NULL == pcie_gpio_handle[gpio_id].h_pcie_gpio_int)
   {
      pcie_osal_debug(MSG_FATAL, "h_pcie_gpio_int is NULL for gpio %d", pcie_dev->pcie_gpio_config_ptr[gpio_id].gpio_num);
   }

   dal_result = GPIOInt_RegisterIsr(pcie_gpio_handle[gpio_id].h_pcie_gpio_int,
                                    pcie_dev->pcie_gpio_config_ptr[gpio_id].gpio_num,
                                    (GPIOIntTriggerType)trigger,
                                    (GPIOINTISR)isr,
                                    param);

   if(DAL_SUCCESS != dal_result)
   {
      pcie_osal_debug(MSG_FATAL, "GPIOInt_RegisterIsr failed for gpio %d", pcie_dev->pcie_gpio_config_ptr[gpio_id].gpio_num);
   }

   /* Update the GPIO interrupts in the pcie_gpio_config_ptr */
   pcie_dev->pcie_gpio_config_ptr[gpio_id].isr = isr;
#endif /* PCIE_BOOT */
}

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_gpio_deregister_isr
** ============================================================================
*/
/**
 * De-register PCIe interrupt handler.
 *
 * @param[in]      gpio_id   - ID of the gpio
 *
 * @return         void
 */
void pcie_gpio_deregister_isr(pcie_gpio_id_type gpio_id)
{
   DALResult           dal_result;

   if(NULL == pcie_gpio_handle[gpio_id].h_pcie_gpio_int)
   {
      pcie_osal_debug(MSG_FATAL, "h_pcie_gpio_int is NULL for gpio %d", pcie_dev->pcie_gpio_config_ptr[gpio_id].gpio_num);
   }
   /* Deregister a GPIO interrupt handler. This also disables the interrupt */
   dal_result = GPIOInt_DeregisterIsr(pcie_gpio_handle[gpio_id].h_pcie_gpio_int,
                                      pcie_dev->pcie_gpio_config_ptr[gpio_id].gpio_num,
                                      (GPIOINTISR)pcie_dev->pcie_gpio_config_ptr[gpio_id].isr);
   if(DAL_SUCCESS != dal_result)
   {
      pcie_osal_debug(MSG_FATAL, "GPIOInt_DeregisterIsr failed for gpio %d", pcie_dev->pcie_gpio_config_ptr[gpio_id].gpio_num);
   }

   /* Update the GPIO interrupts in the pcie_gpio_config_ptr to NULL */
   pcie_dev->pcie_gpio_config_ptr[gpio_id].isr = NULL;
}

/* ============================================================================
**  Function : pcie_gpio_trigger_perst_int
** ============================================================================
*/
/**
 * Trigger PERST GPIO interrupt.. Used for internal testing purpose only.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_gpio_trigger_perst_int(void)
{
   DALResult           dal_result;
   dal_result = GPIOInt_TriggerInterrupt(pcie_gpio_handle[PCIE_GPIO_PERST].h_pcie_gpio_int,
                                         pcie_dev->pcie_gpio_config_ptr[PCIE_GPIO_PERST].gpio_num);
   if(DAL_SUCCESS != dal_result)
   {
      pcie_osal_debug(MSG_FATAL, "GPIOInt_TriggerInterrupt failed");
   }
}

/* ============================================================================
**  Function : pcie_gpio_switch_function
** ============================================================================
*/
/**
 * Switches the GPIO function to the value specified
 *
 * @param[in]      gpio_id      GPIO ID
 * @param[in]      func         Function to be switched to
 *
 * @return         void
 */
void pcie_gpio_switch_function(pcie_gpio_id_type gpio_id, uint32 func)
{
   DALResult           dal_result;

   if(NULL == pcie_gpio_handle[gpio_id].h_pcie_tlmm)
   {
      pcie_osal_debug(MSG_FATAL, "h_pcie_tlmm is NULL for gpio %d", pcie_dev->pcie_gpio_config_ptr[gpio_id].gpio_num);
   }

   pcie_gpio_handle[gpio_id].pcie_gpio_pin = DAL_GPIO_CFG(pcie_dev->pcie_gpio_config_ptr[gpio_id].gpio_num,
                                                        func,
                                                        pcie_dev->pcie_gpio_config_ptr[gpio_id].dir,
                                                        pcie_dev->pcie_gpio_config_ptr[gpio_id].pull,
                                                        pcie_dev->pcie_gpio_config_ptr[gpio_id].drive);

   /* Configure the GPIO with all configurations above */
   dal_result = DalTlmm_ConfigGpio(pcie_gpio_handle[gpio_id].h_pcie_tlmm,
                                   pcie_gpio_handle[gpio_id].pcie_gpio_pin,
                                   (DALGpioEnableType)pcie_dev->pcie_gpio_config_ptr[gpio_id].init_enable);
   if(DAL_SUCCESS != dal_result)
   {
      pcie_osal_debug(MSG_FATAL, "DalTlmm_ConfigGpio failed for gpio %d", pcie_dev->pcie_gpio_config_ptr[gpio_id].gpio_num);
   }
}
#endif /* PCIE_BOOT */
