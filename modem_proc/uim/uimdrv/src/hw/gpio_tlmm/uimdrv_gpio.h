#ifndef UIMDRV_GPIO_H
#define UIMDRV_GPIO_H
/*============================================================================
  FILE:          uimdrv_gpio.h

  OVERVIEW:     File containing the inclusions and definitions necessary for
                the gpio interface.

  DEPENDENCIES: N/A

                Copyright (c) 2012 - 2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.  Please
use ISO format for dates.

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/hw/gpio_tlmm/uimdrv_gpio.h#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

 when       who        what, where, why
------      ----       -----------------------------------------------------------
06/07/16    sam        Stub out Hotswap related functionality
06/07/16    sam        Stub out GPIO Info logging
06/06/16    sam        Stub out external LDO related functionality
01/08/14    ks         Compile warnings
06/16/14    ll         Switch to new string based TLMM APIs
11/27/13    akv        Initialization of hw enumeration based on dal chip family
10/22/13    ll         Check UIM GPIOs as inputs upon timeout w/recovery disabled
10/08/13    nmb        HW header file cleanup
09/16/13    yk         Removed the code for old targets and for old features
07/12/13    nmb        Global Reorganization
07/12/13    akv        HW Enumeration - modifying target specific hardware values
                       and organizing them in value-key pairs
04/10/13    nmb        8x10 bring up changes
03/18/13    js         8x26 UIM3 interface bring up fixes.
02/22/13    js         8x26 bring up changes
02/21/13    ak         Use UIM instance number for GPIO configuration logic
12/05/12    js         UIM Parallel Processing changes
11/09/12    akv        BATT ALARM configuration for Dime
10/26/12    akv/ssr    HW Enumeration changes
============================================================================*/
#include "comdef.h"
#include "uimdrv_main.h"

#define UIM_INVALID_GPIO_NUM                   0xFFFF /* Indicates that there is no GPIO number for the item */

#define UIM_MAX_GPIO_NAME_LEN     16
#define UIM_TLMM_GPIO_INST_INDEX  3
#define UIM_TLMM_IO_MODE          DAL_GPIO_MODE_IO
#define UIM_TLMM_UIMCTRL_MODE     DAL_GPIO_MODE_PRIMARY

typedef struct
{
  char DataGpioName[UIM_MAX_GPIO_NAME_LEN];
  char ClkGpioName[UIM_MAX_GPIO_NAME_LEN];
  char ResetGpioName[UIM_MAX_GPIO_NAME_LEN];
  char PresentGpioName[UIM_MAX_GPIO_NAME_LEN];
  char BattAlarmGpioName[UIM_MAX_GPIO_NAME_LEN];
  char UimExtLdoEnGpioName[UIM_MAX_GPIO_NAME_LEN];
  char UimExtLdoVselGpioName[UIM_MAX_GPIO_NAME_LEN];
}uim_gpios_name_type;

extern DALGpioIdType disabled_uim_card_presence_gpio_id;

void updateGpioNameWithInstance
(
  uim_gpios_name_type *pGpiosName,
  uim_slot_type uim_slot
);

/* Function to initialize GPIO */
void initialize_gpio(uim_instance_enum_type uim_instance);

void UIM_SET_RESET_AS_OUTPUT_SLOT(uim_instance_global_type *uim_ptr);
void ENABLE_UART_INTERFACE_SLOT(uim_instance_global_type *uim_ptr);
void UIM_CONFIGURE_RESET_DEFAULT_GPIO(uim_instance_global_type *uim_ptr);
void UIM_CONFIGURE_DATA_DEFAULT_GPIO(uim_instance_global_type *uim_ptr);
void UIM_CONFIGURE_CLK_DEFAULT_GPIO(uim_instance_global_type *uim_ptr);
void UIM_CONFIGURE_DATA_FOR_UIM_CONTROLLER(uim_instance_global_type *uim_ptr);
void UIM_CONFIGURE_CLK_FOR_UIM_CONTROLLER(uim_instance_global_type *uim_ptr);
void UIM_CONFIGURE_BATT_ALARM_FOR_UICC_SHUTDOWN(uim_instance_global_type *uim_ptr);
void UIM_ASSERT_RESET_SLOT(uim_instance_global_type *uim_ptr);
void UIM_DEASSERT_RESET_SLOT(uim_instance_global_type *uim_ptr);

void uim_config_external_ldo(uim_gpios_name_type   *pGpiosName,  uim_instance_enum_type uim_instance);
boolean uimConfigureUimExtLdoToClassC(uim_instance_enum_type uim_instance);
boolean uimConfigureUimExtLdoToClassB(uim_instance_enum_type uim_instance);
boolean uimConfigureUimExtLdoToZero(uim_instance_enum_type uim_instance);
#endif /* UIMDRV_GPIO_H */
