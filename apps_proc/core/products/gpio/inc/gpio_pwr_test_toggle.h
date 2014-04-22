
/**
 * @file pcie_gpio.h
 * @brief
 * Handles all PCIe GPIOs
 */
/*

===============================================================================
Copyright (c) 2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/


#include "com_dtypes.h"
#include "DDITlmm.h"
#include "TlmmDefs.h"


typedef struct _gpio_cfg_type
{
 
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
}gpio_cfg_type;

#ifndef GPIO_PWR_TEST
	#define GPIO_PWR_TEST     52                   /** 9205_PWR_TEST_GPIO */
#endif


const gpio_cfg_type gpio_config =
{
   /* configuration for TEST GPIO */
   
      1,                             /* TLMM cfg */
      0,                            /* Interrupt enbled */
      NULL,                         /* ISR */
      GPIO_PWR_TEST,            	/* GPIO number */
      0,                            /* GPIO func */
      DAL_GPIO_OUTPUT,              /* GPIO dir */
      DAL_GPIO_PULL_DOWN,          	/* GPIO pull */
      DAL_GPIO_2MA,                	/* GPIO drive */
      DAL_TLMM_GPIO_ENABLE,         /* GPIO default config enable/Disable */
      NULL,                         /* GPIO Alternate cofiguration */
      DAL_GPIO_LOW_VALUE            /* Default output value */
   
};



/* ============================================================================
**  Function : gpio_toggle
** ============================================================================
*/
/**
 * Sets the gpio state to the high/low
 *
 *
 * @return        void
 */
void gpio_pwr_test_toggle(void);


