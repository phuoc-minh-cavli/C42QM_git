
/*
$Header:

Change revision history maintained in version system
===============================================================================
Copyright (c) 2019 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "gpio_pwr_test_toggle.h"
#include "DDIGPIOInt.h"
#include "DalDevice.h"
#include "DALStdDef.h"
#include "malloc.h"
#include "DDIPlatformInfo.h"


/** GPIO control Handle Object */
typedef struct
{
   DalDeviceHandle    *h_tlmm;         /** < -- TLMM handle */
   DALGpioSignalType   gpio_pin;   	 /** < -- Holds the GPIO signal type */
} gpio_handle_ctrl_type;

static gpio_handle_ctrl_type *gpio_handle;
static DalDeviceHandle *hPlatforminfo;
/* ============================================================================
**  Function : gpio_init
** ============================================================================
*/
/**
 * Performs specific gpio initializations.
 *
 *
 * @return         void
 */
 static void gpio_pwr_init(void)
{
   DALResult        dal_result;
   
   gpio_handle = (gpio_handle_ctrl_type *)malloc(sizeof(gpio_handle_ctrl_type));
   

   /* TLMM configuration */
   if(TRUE == gpio_config.tlmm_cfg)
   {
      dal_result = DAL_DeviceAttach(DALDEVICEID_TLMM, &gpio_handle->h_tlmm);

      if(DAL_SUCCESS != dal_result || gpio_handle->h_tlmm == NULL)
      {
         return;
      }
   }



   gpio_handle->gpio_pin = DAL_GPIO_CFG(gpio_config.gpio_num,
                                                        gpio_config.func,
                                                        gpio_config.dir,
                                                        gpio_config.pull,
                                                        gpio_config.drive);

   /* Configure the GPIO with all configurations above */
   dal_result = DalTlmm_ConfigGpio(gpio_handle->h_tlmm,
                                   gpio_handle->gpio_pin,
                                   (DALGpioEnableType)gpio_config.init_enable);
   if(DAL_SUCCESS != dal_result)
   {
      return;
   }

   /* Set the output value if the gpio direction is Output */
   if(DAL_GPIO_OUTPUT == gpio_config.dir)
   {
      dal_result = DalTlmm_GpioOut(gpio_handle->h_tlmm,
                                   gpio_handle->gpio_pin,
                                   (DALGpioValueType)gpio_config.default_output_val);

      if(DAL_SUCCESS != dal_result)
      {
         return;
      }
   }
}



/* ============================================================================
**  Function : gpio_get_state
** ============================================================================
*/
/**
 * Reads the current gpio state
 *
 * @param[in]   gpio_id -  GPIO ID
 *
 * @return      gpio_pin_value_type
 *    DAL_GPIO_HIGH_VALUE - if GPIO pin is high
 *    DAL_GPIO_LOW_VALUE  - if GPIO pin is low
 *
 * @dependencies
 * None.
 */
static DALGpioValueType gpio_get_state(void)
{
   DALGpioValueType pin_state;
  DalTlmm_GpioIn(gpio_handle->h_tlmm,
                               gpio_handle->gpio_pin,
                               &pin_state);


   return(pin_state ? DAL_GPIO_HIGH_VALUE : DAL_GPIO_LOW_VALUE);
}

/* ============================================================================
**  Function : RCMPlatformCheck
** ============================================================================
*/
/**
 * Function return 1 if the Platform is RCM 0 if it is not or unable to get the DAL handle 
 *
 * 
 *
 * @return    int 0 or 1
 */
static int PlatformCheck (void)
{
	DalPlatformInfoPlatformInfoType PlatformInfo; 
	DALResult eDalResult = DAL_SUCCESS;
	if (!hPlatforminfo){ 
		/*Attach to platform info device and return a handle*/
		eDalResult = DAL_PlatformInfoDeviceAttach(DALDEVICEID_PLATFORMINFO, &hPlatforminfo);
	}
	if ( DAL_SUCCESS == eDalResult){
		eDalResult = DalPlatformInfo_GetPlatformInfo(hPlatforminfo,&PlatformInfo);
	}
	if ( DAL_SUCCESS == eDalResult){
		if (PlatformInfo.platform == DALPLATFORMINFO_TYPE_RCM || PlatformInfo.platform == DALPLATFORMINFO_TYPE_CDP){
			return 1;
		}
		else {
			return 0 ;
		}
	}
	return 0;			 
}


static void gpio_pwr_set_state(DALGpioValueType val)
{
   DalTlmm_GpioOut(gpio_handle->h_tlmm, gpio_handle->gpio_pin, val);
}

/* ============================================================================
**  Function : gpio_toggle
** ============================================================================
*/
/**
 * Toggle GPIO value if high it will toggle to low
 *
 * 
 *
 * @return      void
 */


void gpio_pwr_test_toggle(void)
{
	if(PlatformCheck()) {
		DALGpioValueType present_state;
		if(!gpio_handle)
		{
			gpio_pwr_init();
		}
		present_state = gpio_get_state();
		if(present_state == DAL_GPIO_HIGH_VALUE)
		{
			gpio_pwr_set_state(DAL_GPIO_LOW_VALUE);
		}
		else
		{
			gpio_pwr_set_state(DAL_GPIO_HIGH_VALUE);
		}
	}	   
}

