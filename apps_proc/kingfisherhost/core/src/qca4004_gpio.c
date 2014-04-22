/*****************************************************************************
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "qca4004_gpio.h"
#include "qapi_tlmm.h"
#include "qca4004_internal.h"

/*-------------------------------------------------------------------------
 * Macros
 *-----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * The GPIO pins of MDM9205 which control the power supply of QCA4004 pin
 *-------------------------------------------------------------------------
 * MDM GPIO	|		QCA4004 pin
 *-------------------------------------------------------------------------
 * GPIO_45	|		WAKEUP
 * GPIO_47	|		WLAN_EN / CHIP_PWD_L
 * GPIO_50	|		QCA_3V3_EN / 3.3V
 * GPIO_51	|		QCA_1P8_EN
 *-----------------------------------------------------------------------*/
#define PIN_QCA_WLAN_EN	47
#define PIN_QCA_3V3_EN	50
#define PIN_QCA_1P8_EN	51
#define PIN_QCA_WAKEUP	45

#define NUM_PIN_QCA		3


/*-------------------------------------------------------------------------
 * Static & global Variable Declarations
 *-----------------------------------------------------------------------*/
static const uint32_t pin_list[NUM_PIN_QCA] = {PIN_QCA_WLAN_EN, PIN_QCA_1P8_EN, PIN_QCA_3V3_EN};
static qapi_GPIO_ID_t gpio_id_list[NUM_PIN_QCA];


/*-------------------------------------------------------------------------
 * Local Function Definitions
 *-----------------------------------------------------------------------*/

/*
 * The code snippet below will required the lock on a GPIO pin
 * and configure pin to a discrete GPIO with default configuration 
 * configured as an output with a pull-up
 */
static qapi_Status_t qca4004_config_get_gpio(uint32_t pin,qapi_GPIO_ID_t* gpio_id)
{
	qapi_Status_t      status = QAPI_OK;
	qapi_TLMM_Config_t tlmm_config;
	
	tlmm_config.pin = pin; 
	
	tlmm_config.func = 0; 								
	tlmm_config.dir = QAPI_GPIO_OUTPUT_E;
	tlmm_config.pull = QAPI_GPIO_PULL_UP_E;
	tlmm_config.drive = QAPI_GPIO_10MA_E; 
	QCA4004_DEBUG_LOG("qca4004_config_get_gpio pin=%d, ", pin);
	status = qapi_TLMM_Get_Gpio_ID(&tlmm_config, gpio_id);
	QCA4004_DEBUG_LOG("get gpio status=%d , ", status);
	if (status == QAPI_OK)
	{
		status = qapi_TLMM_Config_Gpio(*gpio_id, &tlmm_config);
		QCA4004_DEBUG_LOG("config gpio status=%d , ", status);
		if (status != QAPI_OK)
		{
			qapi_TLMM_Release_Gpio_ID(&tlmm_config, *gpio_id);
			// Handle failed case here
		}
		#if 1
		/* when QCA4004 initialize, 
		 * the default value of pins PIN_QCA_3V3_EN and PIN_QCA_1P8_EN should set to high 
		 */ 
		else if(pin != PIN_QCA_WLAN_EN)
		{
			status = qapi_TLMM_Drive_Gpio(*gpio_id,pin,QAPI_GPIO_HIGH_VALUE_E);
		}
		#endif
	}
	return status;
} 

/*
 * The code snippet below will relinquish the lock on a GPIO pin
 */
static qapi_Status_t qca4004_config_release_gpio(uint32_t pin,qapi_GPIO_ID_t gpio_id)
{
	qapi_Status_t      status = QAPI_OK;
	qapi_TLMM_Config_t tlmm_config;
	
	tlmm_config.pin = pin; 
	
	tlmm_config.func = 0; 								
	tlmm_config.dir = QAPI_GPIO_OUTPUT_E;
	tlmm_config.pull = QAPI_GPIO_PULL_UP_E;
	tlmm_config.drive = QAPI_GPIO_10MA_E; 

	//status = qapi_TLMM_Drive_Gpio(gpio_id, pin, QAPI_GPIO_LOW_VALUE_E);
	if(status == QAPI_OK)
		status = qapi_TLMM_Release_Gpio_ID(&tlmm_config, gpio_id);
	
	return status;
} 


/*-------------------------------------------------------------------------
 * Public Function Definitions
 *-----------------------------------------------------------------------*/

/*
 * The code snippet below will required the lock on all GPIO pins for QCA4004
 * and configure pins to GPIOs with default configuration 
 */
uint8_t qca4004_get_gpio_all(void)
{	
	uint8_t i;
	for(i = 0;i < NUM_PIN_QCA;i++)
	{
		if(qca4004_config_get_gpio(pin_list[i],&(gpio_id_list[i])) != QAPI_OK)
		{
			while(i)
			{
				qca4004_config_release_gpio(pin_list[i-1], gpio_id_list[i-1]);
				i--;
			}

			return 1;
		}
		QCA4004_DEBUG_LOG("get gpio all pin=%d,gpio_id=%d ", pin_list[i],gpio_id_list[i]);
	}
	return 0;
}

/*
 * The code snippet below will relinquish the lock on all GPIO pins for QCA4004
 */
uint8_t qca4004_release_gpio_all(void)
{
	uint8_t i;
	for(i = 0;i < NUM_PIN_QCA;i++)
	{
		qca4004_config_release_gpio(pin_list[i], gpio_id_list[i]);
	}

	return 0;
}

/*
 * The code snippet below will set all GPIO pins for QCA4004 to a specified value.
 */
uint8_t qca4004_set_gpio_all(uint8_t value)
{
	qapi_GPIO_Value_t value_t = QAPI_GPIO_LOW_VALUE_E;
	uint8_t i = 0;

	QCA4004_DEBUG_LOG("qca4004_set_gpio_all value=%d ,  ", value);

	if(value == 1)
		value_t = QAPI_GPIO_HIGH_VALUE_E;

	#if 0
	for(i = 0;i < NUM_PIN_QCA;i++)
	{	
		QCA4004_DEBUG_LOG("pin=%d ,gpio_id=%d ", pin_list[i],gpio_id_list[i]);
		if(qapi_TLMM_Drive_Gpio(gpio_id_list[i],pin_list[i],value_t) != QAPI_OK)
		{
			QCA4004_DEBUG_LOG("drive gpio error pin=%d ,gpio_id=%d ", pin_list[i],gpio_id_list[i]);
			return QCA4004_ERR_GPIO;
		}
	}
	#else
	/* QCA4004 chip can be power on or off by pin PIN_QCA_WLAN_EN (the first pin in the array) in hostless mode */
	if(qapi_TLMM_Drive_Gpio(gpio_id_list[0],pin_list[0],value_t) != QAPI_OK)
	{
		QCA4004_DEBUG_LOG("drive gpio error pin=%d ,gpio_id=%d ", pin_list[i],gpio_id_list[i]);
		return 1;
	}
	#endif

	return 0;
}

