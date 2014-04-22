#ifndef CSD_DEV_GPIO_DEFS_H_
#define CSD_DEV_GPIO_DEFS_H_
/*==============================================================================
  @file csd_dev_gpio_defs.h

  This file defines the GPIO specific MACROS used by the target. Modification to
  GPIO mapping, tlmm functionality, gpio drive strength, gpio pull and other
  constants can be made here provided the target support them.

  Copyright (c) 2008-13 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to this module.
 Notice that changes are listed in reverse chronological order.

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 06/16/14   jyuan   GPIO Protection using TLMM 2.0 API
 05/11/10   unni    Initial Creation
==============================================================================*/


typedef struct {
  DALGpioIdType   gpio_id;
  uint32_t        ref_count;
} csd_gpio_info;

/*=======================================?=======================================
  Constants used in GPIO controls
=========================================?=====================================*/
/* Codec Reset init period - hold down codec reset for this duration */
#define CODEC_RESET_INIT_PERIOD 10
#define GPIO_FUNCTION_NAME_MAX_SIZE 30

typedef enum
{
  CODEC_RESET     = 0,
  CODEC_INIT      = 1,
  CODEC_INTERRUPT = 2,
  MCLK            = 3,
  PRI_SYNC_CLK    = 4,
  PRI_DATA_IN     = 5,
  PRI_DATA_OUT    = 6,
  PRI_BIT_CLK     = 7,
  SEC_SYNC_CLK    = 8,
  SEC_DATA_IN     = 9,
  SEC_DATA_OUT    = 10,
  SEC_BIT_CLK     = 11,
  CDC_BUCK_EN     = 12,
  GPIO_USECASE_MAX
} csd_gpio_use_case;

/*=============================================================================

  FUNCTION      csd_dev_tlmm_get_gpio_setting

  DESCRIPTION   enable GPIO use case

  DEPENDENCIES  none

  PARAMETERS    @use_case : use case defined in csd_gpio_use_case
                @gpio_function_name: name associated with the GPIO
                @config: GPIO configuration including direction, pull and strength

  RETURN VALUE  csd return status @see csd_status.h

  SIDE EFFECTS  none

=============================================================================*/
static __inline int32_t csd_dev_tlmm_get_gpio_setting(
  csd_gpio_use_case use_case,
  char_t* gpio_function_name,
  DalTlmm_GpioConfigIdType* config )
{
  int32_t rc = CSD_EOK;

  if( !gpio_function_name || !config )
  {
    return CSD_EBADPARAM;
  }

  switch( use_case )
  {
    case CODEC_RESET:
      strlcpy( gpio_function_name, "codec_en", sizeof("codec_en") );
      config->eDirection = DAL_GPIO_OUTPUT;
      config->ePull = DAL_GPIO_PULL_UP;
      config->eDriveStrength = DAL_GPIO_12MA;
	  //config->eOutputDrive = DAL_GPIO_LOW_VALUE;

    break;

    case CODEC_INIT:
      strlcpy( gpio_function_name, "codec_en", sizeof("codec_en") );
      config->eDirection = DAL_GPIO_OUTPUT;
      config->ePull = DAL_GPIO_PULL_UP;
      config->eDriveStrength = DAL_GPIO_16MA;
	  //config->eOutputDrive = DAL_GPIO_HIGH_VALUE;
    break;

    case CODEC_INTERRUPT:
      strlcpy( gpio_function_name, "codec_int", sizeof("codec_int") );
      config->eDirection = DAL_GPIO_INPUT;
      config->ePull = DAL_GPIO_NO_PULL;
      config->eDriveStrength = DAL_GPIO_2MA;
    break;

    case MCLK:
      strlcpy( gpio_function_name, "pri_mi2s_mclk_a", sizeof("pri_mi2s_mclk_a") );
      config->eDirection = DAL_GPIO_OUTPUT;
      config->ePull = DAL_GPIO_NO_PULL;
      config->eDriveStrength = DAL_GPIO_12MA;
    break;

    case PRI_SYNC_CLK:
      strlcpy( gpio_function_name, "pri_mi2s_ws_a", sizeof("pri_mi2s_ws_a") );
      config->eDirection = DAL_GPIO_OUTPUT;
      config->ePull = DAL_GPIO_NO_PULL;
      config->eDriveStrength = DAL_GPIO_16MA;
    break;

    case PRI_DATA_IN:
      strlcpy( gpio_function_name, "pri_mi2s_data0_a", sizeof("pri_mi2s_data0_a") );
      config->eDirection = DAL_GPIO_INPUT;
      config->ePull = DAL_GPIO_NO_PULL;
      config->eDriveStrength = DAL_GPIO_16MA;
    break;

    case PRI_DATA_OUT:
      strlcpy( gpio_function_name, "pri_mi2s_data1_a", sizeof("pri_mi2s_data1_a") );
      config->eDirection = DAL_GPIO_OUTPUT;
      config->ePull = DAL_GPIO_NO_PULL;
      config->eDriveStrength = DAL_GPIO_16MA;
    break;

    case PRI_BIT_CLK:
      strlcpy( gpio_function_name, "pri_mi2s_sck_a", sizeof("pri_mi2s_sck_a") );
      config->eDirection = DAL_GPIO_OUTPUT;
      config->ePull = DAL_GPIO_NO_PULL;
      config->eDriveStrength = DAL_GPIO_16MA;
    break;

    case SEC_SYNC_CLK:
      strlcpy( gpio_function_name, "sec_mi2s_ws_a", sizeof("sec_mi2s_ws_a") );
      config->eDirection = DAL_GPIO_OUTPUT;
      config->ePull = DAL_GPIO_NO_PULL;
      config->eDriveStrength = DAL_GPIO_6MA;
    break;

    case SEC_DATA_IN:
      strlcpy( gpio_function_name, "sec_mi2s_data0_a", sizeof("sec_mi2s_data0_a") );
      config->eDirection = DAL_GPIO_INPUT;
      config->ePull = DAL_GPIO_NO_PULL;
      config->eDriveStrength = DAL_GPIO_6MA;
    break;

    case SEC_DATA_OUT:
      strlcpy( gpio_function_name, "sec_mi2s_data1_a", sizeof("sec_mi2s_data1_a") );
      config->eDirection = DAL_GPIO_OUTPUT;
      config->ePull = DAL_GPIO_NO_PULL;
      config->eDriveStrength = DAL_GPIO_6MA;
    break;

    case SEC_BIT_CLK:
      strlcpy( gpio_function_name, "sec_mi2s_sck_a", sizeof("sec_mi2s_sck_a") );
      config->eDirection = DAL_GPIO_OUTPUT;
      config->ePull = DAL_GPIO_NO_PULL;
      config->eDriveStrength = DAL_GPIO_6MA;
    break;

    case CDC_BUCK_EN:
      strlcpy( gpio_function_name, "cdc_buck_en", sizeof("cdc_buck_en") );
      config->eDirection = DAL_GPIO_OUTPUT;
      config->ePull = DAL_GPIO_PULL_UP;
      config->eDriveStrength = DAL_GPIO_16MA;
	    //config->eOutputDrive = DAL_GPIO_HIGH_VALUE;
      //CSD_MSG( CSD_PRIO_ERROR, "cdc_buck_enable in usecase");
    break;



    default:
      rc = CSD_EBADPARAM;
      CSD_MSG( CSD_PRIO_ERROR, "csd_dev_tlmm_get_gpio_setting: Unknown usecase[%d]", use_case );
    break;
  }

  return rc;
}

#endif /* CSD_DEV_GPIO_DEFS_H_ */
