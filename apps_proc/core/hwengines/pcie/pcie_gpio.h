/**
 * @file pcie_gpio.h
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
#ifndef __PCIE_GPIO_H__
#define __PCIE_GPIO_H__

#include "com_dtypes.h"
#ifndef PCIE_BOOT
#include "TlmmDefs.h"
#else
#include "DDITlmm.h"
#endif /* PCIE_BOOT */

#define PCIE_GPIO_CFG_OUT DAL_GPIO_CFG_OUT

/** GPIO pin value */
typedef enum _pcie_gpio_pin_value_type
{
   PCIE_GPIO_LOW  = 0,    /**< -- GPIO Pin value is low */
   PCIE_GPIO_HIGH = 1     /**< -- GPIO Pin value is high */
}pcie_gpio_pin_value_type;

/** GPIO Enable Control type */
typedef enum
{
  PCIE_GPIO_DISABLE,  /**< -- Use the internal inactive configuration. */
  PCIE_GPIO_ENABLE,   /**< -- Use the configuration passed as parameter. */
}pcie_gpio_enable_type;

/** GPIO Direction Control type */
typedef enum
{
   PCIE_GPIO_INPUT  = 0,     /**< -- Set to INPUT. */
   PCIE_GPIO_OUTPUT = 1,     /**< -- Set to OUTPUT. */
}pcie_gpio_dir_type;

/** GPIO Pull Control type */
typedef enum
{
  PCIE_GPIO_NO_PULL    = 0x0, /**< -- Do not specify a pull. */
  PCIE_GPIO_PULL_DOWN  = 0x1, /**< -- Pull the GPIO down. */
  PCIE_GPIO_KEEPER     = 0x2, /**< -- Designate as a Keeper. */
  PCIE_GPIO_PULL_UP    = 0x3, /**< -- Pull the GPIO up. */
}pcie_gpio_pull_type;

/** GPIO Drive Strength Control type */
typedef enum
{
  PCIE_GPIO_2MA     = 0,    /**< -- Specify a 2 mA drive. */
  PCIE_GPIO_4MA     = 0x1,  /**< -- Specify a 4 mA drive. */
  PCIE_GPIO_6MA     = 0x2,  /**< -- Specify a 6 mA drive. */
  PCIE_GPIO_8MA     = 0x3,  /**< -- Specify a 8 mA drive. */
  PCIE_GPIO_10MA    = 0x4,  /**< -- Specify a 10 mA drive. */
  PCIE_GPIO_12MA    = 0x5,  /**< -- Specify a 12 mA drive. */
  PCIE_GPIO_14MA    = 0x6,  /**< -- Specify a 14 mA drive. */
  PCIE_GPIO_16MA    = 0x7,  /**< -- Specify a 16 mA drive. */
}pcie_gpio_drive_type;

/**
 * This enum is used to define the trigger condition of the GPIO interrupt.
 */
typedef enum {
   LEVEL_HIGH = 0x0,           /**< -- The GPIO interrupt will trigger only if the input signal is high */
   LEVEL_LOW,                  /**< -- The GPIO interrupt will trigger only if the input signal is low */
   RISING_EDGE,                /**< -- The GPIO interrupt will trigger only if the input signal level transitions from low to high */
   FALLING_EDGE,               /**< -- The GPIO interrupt will trigger only if the input signal level transitions from high to low */
   DUAL_EDGE,                  /**< -- The GPIO interrupt will trigger only if the input signal level transitions from high to low or from low to high. */
   TRIGGER_MAX = 0x7fffffff    /**< -- RESERVED. */
}pcie_gpio_int_trigger_type;

/** ALL PCIE GPIO IDs */
typedef enum
{
   PCIE_GPIO_PERST    = 0x0,         /** < -- ID for PERST GPIO */
   PCIE_GPIO_WAKE     = 0x1,         /** < -- ID for WAKE GPIO */
   PCIE_GPIO_CLKREQ   = 0x2,         /** < -- ID for CLKREQ GPIO */
   PCIE_MAX_GPIO                     /** < -- Reserved */
}pcie_gpio_id_type;

/** Structure for PCIe GPIO configuration */
typedef struct _pcie_gpio_cfg_type
{
   pcie_gpio_id_type pcie_gpio_id;             /** < -- GPIO ID */
   boolean tlmm_cfg;                           /** < -- Specify if TLMM config is enabled */
   boolean int_cfg;                            /** < -- Specify if Interrupt is enabled for the GPIO */
   void *isr;                                  /** < -- Registerd ISR for the GPIO */
   uint32 gpio_num;                            /** < -- GPIO pin number */
   uint32 func;                                /** < -- GPIO pin function */
   uint32 dir;                                 /** < -- GPIO pin direction */
   uint32 pull;                                /** < -- GPIO Pull */
   uint32 drive;                               /** < -- GPIO Drive */
   uint32 init_enable;                         /** < -- GPIO default configuration enable */
   uint32 inactive_cfg;                        /** < -- GPIO Alternate configurtion */
   uint32 default_output_val;                  /** < -- GPIO default Output value if direction is output */
}pcie_gpio_cfg_type;

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
enum _pcie_return_status pcie_gpio_init(void);

/* ============================================================================
**  Function : pcie_gpio_get_state
** ============================================================================
*/
/**
 * Reads the current gpio state
 *
 * @param[in]      gpio_id     ID of the gpio
 *
 * @return         pcie_gpio_pin_value_type
 */
pcie_gpio_pin_value_type pcie_gpio_get_state(pcie_gpio_id_type gpio_id);

/* ============================================================================
**  Function : pcie_gpio_set_state
** ============================================================================
*/
/**
 * Sets the gpio state to the high/low
 *
 * @param[in]     gpio_id     ID of the gpio
 * @param[in]     val         Value to be set
 *
 * @return        void
 */
void pcie_gpio_set_state(pcie_gpio_id_type gpio_id, pcie_gpio_pin_value_type val);

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
void pcie_gpio_interrupt_set_trigger(pcie_gpio_id_type gpio_id, pcie_gpio_int_trigger_type trigger);

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
void pcie_gpio_register_isr(pcie_gpio_id_type gpio_id, void *isr, pcie_gpio_int_trigger_type trigger, uint32 param);

/* ============================================================================
**  Function : pcie_gpio_deregister_isr
** ============================================================================
*/
/**
 * De-register PCIe interrupt handler.
 *
 * @param[in]      gpio_id     ID of the gpio
 *
 * @return          void
 */
void pcie_gpio_deregister_isr(pcie_gpio_id_type gpio_id);

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
void pcie_gpio_trigger_perst_int(void);

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
void pcie_gpio_switch_function(pcie_gpio_id_type gpio_id, uint32 func);

#endif /* __PCIE_GPIO_H__ */
