/**
 * sns_gpio_service.c
 *
 * The GPIO Service implementation
 *
 * Copyright (c) 2017-2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * $Id: //components/rel/ssc.slpi/5.1/utils/gpio_service/hexagon_no_island/sns_gpio_service.c#1 $
 * $DateTime: 2019/05/28 05:03:16 $
 * $Change: 19336570 $
 *
 **/

#include <stdint.h>
#include <string.h>
#include "sns_assert.h"
#include "sns_gpio_service_internal.h"
#include "sns_gpio_service.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_printf_int.h"
#include "sns_types.h"

#include "DDITlmm.h"
#include "DalDevice.h"

/** Version: */
static const uint16_t sns_gpio_service_version SNS_SECTION(".rodata.sns") = 1;

struct sns_fw_gpio_service
{
  sns_gpio_service service;

  DalDeviceHandle *dal_device_handle;
  /* Number of SNS GPIOs with active/sleeep available config info */
  uint32_t sns_num_gpios;
  /* Active/Sleep config for all SSC TLMM GPIOs */
  sns_gpio_config *gpio_state;

  sns_osa_lock *lock;
};

/**
 * Private state of the GPIO Service.
 */
static sns_fw_gpio_service gpio_service SNS_SECTION(".data.sns");

  /** uTLMM and dalTLMM drive type enum values are identical. */
const DALGpioDriveType dal_drive_map[_sns_interrupt_drive_strength_ARRAYSIZE] SNS_SECTION(".rodata.sns") =
  {
    DAL_GPIO_2MA,
    DAL_GPIO_4MA,
    DAL_GPIO_6MA,
    DAL_GPIO_8MA,
    DAL_GPIO_10MA,
    DAL_GPIO_12MA,
    DAL_GPIO_14MA,
    DAL_GPIO_16MA,
  };

const DALGpioPullType dal_pull_map[_sns_interrupt_pull_type_ARRAYSIZE] SNS_SECTION(".rodata.sns") =
  {
    DAL_GPIO_NO_PULL,
    DAL_GPIO_PULL_DOWN,
    DAL_GPIO_KEEPER,
    DAL_GPIO_PULL_UP,
  };

/**
 * Return pull type value from DALGpioPullType enum.
 *
 * @param[i] pull   sns_irq_pull type
 *
 * @return DALGpioPullType
 */
SNS_SECTION(".text.sns")
static DALGpioPullType get_dal_pull_type(sns_interrupt_pull_type pull)
{
  /** uTLMM and dalTLMM pull type enum values are identical. */
  return dal_pull_map[pull];
}

/**
 * Return drive strength type from DALGpioDriveType enum.
 *
 * @param[i] drive   sns_irq_drive_strength type
 *
 * @return DALGpioDriveType
 */
SNS_SECTION(".text.sns")
static DALGpioDriveType get_dal_drive_type(sns_interrupt_drive_strength drive)
{
  return dal_drive_map[drive];
}

/**
 * Get DAL GPIO configuration for input request.
 *
 * @param[i] req    pointer to request config that contains IRQ
 *                  pin config
 *
 * @return uint32_t DAL GPIO config
 */
SNS_SECTION(".text.sns")
static uint32_t get_irq_pin_config(sns_interrupt_req const* req)
{
  DALGpioPullType pull = get_dal_pull_type(req->interrupt_pull_type);
  DALGpioDriveType drive = get_dal_drive_type(req->interrupt_drive_strength);

  /** Interrupts are always INPUT with func = 0 */
  /** uTLMM and dalTLMM GPIO CFG macros are identical. */
  /** Example: UTLMM_GPIO_CFG(gpio, func, dir, pull, drive) */

  return DAL_GPIO_CFG (req->interrupt_num, 0, DAL_GPIO_INPUT, pull, drive);
}

/** See sns_gpio_service_internal.h */
void sns_gpio_service_set_sns_gpio_config(bool sleep_config)
{
  DALGpioSignalType slpi_gpio_config;
  uint32_t i = 0;

  if(sleep_config == true)
  {
    for(i = 0; i < gpio_service.sns_num_gpios; i++)
    {
      slpi_gpio_config = DAL_GPIO_CFG(gpio_service.gpio_state[i].gpio_num,
                           gpio_service.gpio_state[i].sleep_fs,
                           gpio_service.gpio_state[i].sleep_dir,
                           gpio_service.gpio_state[i].sleep_pull,
                           gpio_service.gpio_state[i].sleep_drive);
      if(!DalTlmm_ConfigGpio(gpio_service.dal_device_handle, slpi_gpio_config, DAL_TLMM_GPIO_ENABLE))
      {
        SNS_PRINTF(ERROR, sns_fw_printf, "Error when setting sleep config for sns gpio: %d",
          gpio_service.gpio_state[i].gpio_num);
      }
      else
      {
        SNS_PRINTF(LOW, sns_fw_printf, "Setting Sleep config for GPIO:%d, Drive:%d",
                   gpio_service.gpio_state[i].gpio_num,
                   gpio_service.gpio_state[i].sleep_drive);
      }
    }
  }
  else
  {
    for(i = 0; i < gpio_service.sns_num_gpios; i++)
    {
      slpi_gpio_config = DAL_GPIO_CFG(gpio_service.gpio_state[i].gpio_num,
                           gpio_service.gpio_state[i].active_fs,
                           gpio_service.gpio_state[i].active_dir,
                           gpio_service.gpio_state[i].active_pull,
                           gpio_service.gpio_state[i].active_drive);
      if(!DalTlmm_ConfigGpio(gpio_service.dal_device_handle, slpi_gpio_config, DAL_TLMM_GPIO_ENABLE))
      {
        SNS_PRINTF(ERROR, sns_fw_printf, "Error when setting active config for sns gpio: %d",
          gpio_service.gpio_state[i].gpio_num);
      }
      else
      {
        SNS_PRINTF(LOW, sns_fw_printf, "Setting Active config for GPIO:%d, Drive:%d",
                   gpio_service.gpio_state[i].gpio_num,
                   gpio_service.gpio_state[i].active_drive);
      }
    }
  }
}

/** See sns_gpio_service_internal.h */
void sns_gpio_service_store_gpio_config(sns_gpio_config *gpio_config)
{
  uint32_t i;

  sns_osa_lock_acquire(gpio_service.lock);
  for(i = 0; i < gpio_service.sns_num_gpios && i < SNS_MAX_GPIO; i++)
  {
    if(gpio_service.gpio_state[i].gpio_num == gpio_config->gpio_num)
    {
      gpio_service.gpio_state[i].active_fs = gpio_config->active_fs;
      gpio_service.gpio_state[i].active_dir = gpio_config->active_dir;
      gpio_service.gpio_state[i].active_pull = gpio_config->active_pull;
      gpio_service.gpio_state[i].active_drive = gpio_config->active_drive;
      gpio_service.gpio_state[i].sleep_fs = gpio_config->sleep_fs;
      gpio_service.gpio_state[i].sleep_dir = gpio_config->sleep_dir;
      gpio_service.gpio_state[i].sleep_pull = gpio_config->sleep_pull;
      gpio_service.gpio_state[i].sleep_drive = gpio_config->sleep_drive;
      break;
    }
  }

  if(i >= gpio_service.sns_num_gpios && gpio_service.sns_num_gpios < SNS_MAX_GPIO)
  {
    sns_gpio_config *cfg = &gpio_service.gpio_state[gpio_service.sns_num_gpios++];
    cfg->gpio_num = gpio_config->gpio_num;
    cfg->active_fs = gpio_config->active_fs;
    cfg->active_dir = gpio_config->active_dir;
    cfg->active_pull = gpio_config->active_pull;
    cfg->active_drive = gpio_config->active_drive;
    cfg->sleep_fs = gpio_config->sleep_fs;
    cfg->sleep_dir = gpio_config->sleep_dir;
    cfg->sleep_pull = gpio_config->sleep_pull;
    cfg->sleep_drive = gpio_config->sleep_drive;

    SNS_PRINTF(LOW, sns_fw_printf, "Stored GPIO:%d, Num of GPIOs:%d",
               gpio_config->gpio_num, gpio_service.sns_num_gpios);

    if(gpio_service.sns_num_gpios == SNS_MAX_GPIO)
    {
      SNS_PRINTF(ERROR, sns_fw_printf, "Excess number of SNS GPIOs registered with GPIO service");
    }
  }

  sns_osa_lock_release(gpio_service.lock);
}

/** See sns_gpio_service.h */
SNS_SECTION(".text.sns")
sns_rc sns_read_gpio(uint32_t gpio,
                     bool is_chip_pin,
                     sns_gpio_state *state)
{
  if(is_chip_pin)
  {
    DALGpioSignalType gpio_config =
       DAL_GPIO_CFG(gpio, 0, DAL_GPIO_INPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA);
    DALGpioValueType gpio_val = DAL_GPIO_LOW_VALUE;

    sns_osa_lock_acquire(gpio_service.lock);

    if(DAL_SUCCESS != DalTlmm_GpioIn(gpio_service.dal_device_handle,
                                     gpio_config,
                                     &gpio_val))
    {
      sns_osa_lock_release(gpio_service.lock);
      SNS_PRINTF(ERROR, sns_fw_printf, "DalTlmm_GpioIn gpio %d", gpio);
      return SNS_RC_NOT_SUPPORTED;
    }

    *state = (gpio_val == DAL_GPIO_LOW_VALUE) ? SNS_GPIO_STATE_LOW : SNS_GPIO_STATE_HIGH;

    //SNS_PRINTF(HIGH, sns_fw_printf, "read_gpio gpio %d gpio_val %d  state = %d", gpio, gpio_val, *state);

    sns_osa_lock_release(gpio_service.lock);
  }

  return SNS_RC_SUCCESS;
}

/** See sns_gpio_service.h */
SNS_SECTION(".text.sns")
sns_rc sns_write_gpio(uint32_t gpio,
                      bool is_chip_pin,
                      sns_gpio_drive_strength drive_strength,
                      sns_gpio_pull_type pull,
                      sns_gpio_state state)
{
  if(is_chip_pin)
  {
    DALGpioValueType out_val =
       (state == SNS_GPIO_STATE_LOW) ? DAL_GPIO_LOW_VALUE : DAL_GPIO_HIGH_VALUE;
    DALGpioSignalType gpio_config =
       DAL_GPIO_CFG_OUT(gpio, 0, DAL_GPIO_OUTPUT,
                       (DALGpioPullType)pull, (DALGpioDriveType)drive_strength, out_val);

    sns_osa_lock_acquire(gpio_service.lock);

    //SNS_PRINTF(sns_fw_printf, (HIGH, 2, "write gpio %d out_val = %d", gpio, out_val);

    if(DAL_SUCCESS != DalTlmm_ConfigGpio(gpio_service.dal_device_handle, gpio_config, DAL_TLMM_GPIO_ENABLE))
    {
      sns_osa_lock_release(gpio_service.lock);
      SNS_PRINTF(ERROR, sns_fw_printf, "DalTlmm_ConfigGpio gpio %d", gpio);
      return SNS_RC_NOT_SUPPORTED;
    }
    sns_osa_lock_release(gpio_service.lock);
  }

  return SNS_RC_SUCCESS;
}

/* See sn_gpio_service_internal.h */
SNS_SECTION(".text.sns")
bool sns_gpio_service_update_irq_pin(sns_interrupt_req const *req,
                                     bool enable, bool set_inactive_cfg)
{
  DALResult err;
  bool      rv  = false;
  uint32_t  cfg = get_irq_pin_config(req);

  if(req->is_chip_pin)
  {
    /** Configuring chip level TLMM wake up interrupts may not be
     *  needed. The default config of these pins is done in the
     *  boot image. This functinality is no-op in older DDF
     *  architecture as well. */

    /* Configure INACTIVE CFG for this GPIO */
    if(set_inactive_cfg)
    {
      SNS_PRINTF(HIGH, sns_fw_printf,
                 "DalTlmm_SetInactiveConfig gpio %d", req->interrupt_num);
      DALGpioSignalType inactive_port_config =
          DAL_GPIO_CFG(req->interrupt_num, 0, DAL_GPIO_INPUT,
                       req->interrupt_pull_type,
                       req->interrupt_drive_strength);
      DalTlmm_SetInactiveConfig(gpio_service.dal_device_handle,
                                req->interrupt_num, inactive_port_config);
    }

    err = DalTlmm_ConfigGpio(gpio_service.dal_device_handle,
                             (DALGpioSignalType)cfg,
                             (enable ? DAL_TLMM_GPIO_ENABLE : DAL_TLMM_GPIO_DISABLE));
    rv = (err == DAL_SUCCESS);
  }


  return rv;
}

/**
 * GPIO Service API.
 */
static sns_gpio_service_api  gpio_service_api SNS_SECTION(".data.sns") =
{
  .struct_len = sizeof(sns_gpio_service_api),
  .read_gpio = &sns_read_gpio,
  .write_gpio = &sns_write_gpio
};

/**
 * See sns_gpio_service_internal.h
 */
sns_fw_gpio_service* sns_gpio_service_init(void)
{
  sns_osa_lock_attr   lock_attr;
  sns_rc return_code = SNS_RC_SUCCESS;

  gpio_service.service.api = &gpio_service_api;
  gpio_service.service.service.type = SNS_GPIO_SERVICE;
  gpio_service.service.service.version = sns_gpio_service_version;

  gpio_service.sns_num_gpios = 0;

  gpio_service.gpio_state = (sns_gpio_config *)sns_malloc(SNS_HEAP_MAIN,
                               sizeof(sns_gpio_config) * SNS_MAX_GPIO);

  /** Initialize rail_mgr_lock. */
  sns_osa_lock_attr_init(&lock_attr);
  sns_osa_lock_attr_set_memory_partition(&lock_attr, SNS_OSA_MEM_TYPE_NORMAL);
  return_code = sns_osa_lock_create(&lock_attr, &gpio_service.lock);
  SNS_ASSERT(SNS_RC_SUCCESS == return_code);

  if(DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_TLMM, &gpio_service.dal_device_handle))
  {
    SNS_ASSERT(0);
  }

  return &gpio_service;
}
