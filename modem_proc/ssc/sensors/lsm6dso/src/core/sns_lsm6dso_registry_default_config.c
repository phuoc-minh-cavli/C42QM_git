/*============================================================================
@file sns_lsm6dso_registry_default_config.c

Set default registry configuration in the absence of registry support

Copyright (c) 2018-2019 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

#include "sns_lsm6dso_sensor.h"
#include "sns_lsm6dso_sensor_instance.h"
#include "sns_com_port_types.h"
#include "sns_types.h"
#include "sns_mem_util.h"


#if LSM6DSO_REGISTRY_DISABLED
/** TODO Using 8996 Platform config as defaults. This is for
 *  test purpose only. All platform specific information will
 *  be available to the Sensor driver via Registry.
 *  otherwise, please change accordingly
 *  */

#if BUILD_DB
#define IRQ_NUM                    117
#define BUS_INSTANCE               0x01
#define BUS                        SNS_BUS_SPI
#define BUS_MIN_FREQ_KHZ           0  // using a value of 0 will introduce more errors in timing
#define BUS_MAX_FREQ_KHZ           3300 // 3.3MHz
#define SLAVE_CONTROL              0
#elif SDM_845_BUILDS
#define BUS_INSTANCE               0x02
#define BUS                        SNS_BUS_SPI
#define BUS_MIN_FREQ_KHZ           33*100  // using a value of 0 will introduce more errors in timing
#define BUS_MAX_FREQ_KHZ           96*100 // 9.6MHz
#define IRQ_NUM                    117
#define SLAVE_CONTROL              107
#elif SSC_TARGET_SM6150
#define BUS_INSTANCE               0x02
#define BUS                        SNS_BUS_SPI
#define BUS_MIN_FREQ_KHZ           33*100  // using a value of 0 will introduce more errors in timing
#define BUS_MAX_FREQ_KHZ           96*100 // 9.6MHz
#define IRQ_NUM                    81
#define SLAVE_CONTROL              0
#elif SSC_TARGET_SM7150
#define IRQ_NUM                    86
#define BUS_INSTANCE               0x01
#define BUS                        SNS_BUS_I3C_SDR
#define BUS_MIN_FREQ_KHZ           400    // using a value of 0 will introduce more errors in timing
#define BUS_MAX_FREQ_KHZ           12500 // 12.5MHz
#define SLAVE_CONTROL              107
#elif SSC_TARGET_SAIPAN
#define IRQ_NUM                    130
#define IRQ_NUM_1                  108
#define BUS_INSTANCE               0x01
#define BUS                        SNS_BUS_I3C_SDR
#define BUS_MIN_FREQ_KHZ           400    // using a value of 0 will introduce more errors in timing
#define BUS_MAX_FREQ_KHZ           12500 // 12.5MHz
#define SLAVE_CONTROL              106
#define SLAVE_CONTROL_1            107
#elif SSC_TARGET_RENNELL
#define IRQ_NUM                    86
#define IRQ_NUM_1                  87
#define BUS_INSTANCE               0x01
#define BUS                        SNS_BUS_I3C_SDR
#define BUS_MIN_FREQ_KHZ           400    // using a value of 0 will introduce more errors in timing
#define BUS_MAX_FREQ_KHZ           12500 // 12.5MHz
#define SLAVE_CONTROL              106
#define SLAVE_CONTROL_1            107
#else
#define IRQ_NUM                    132
#define BUS_INSTANCE               0x01
#define BUS                        SNS_BUS_I3C_SDR
#define BUS_MIN_FREQ_KHZ           400  // using a value of 0 will introduce more errors in timing
#define BUS_MAX_FREQ_KHZ           12500 // 12.5MHz
#define SLAVE_CONTROL              107
#endif

#define RAIL_1                     "/pmic/client/sensor_vddio"
#define NUM_OF_RAILS               1

#define I3C_ADDR                   10


#define LSM6DSO_DEFAULT_REG_CFG_RAIL_ON         SNS_RAIL_ON_LPM
#define LSM6DSO_DEFAULT_REG_CFG_ISDRI           1
#define LSM6DSO_DEFAULT_REG_CFG_HW_ID           0
#define LSM6DSO_DEFAULT_REG_ACCEL_RESOLUTION_IDX 2
#define LSM6DSO_DEFAULT_REG_GYRO_RESOLUTION_IDX 3
#define LSM6DSO_DEFAULT_REG_CFG_SUPPORT_SYN_STREAM 0

/**
 * Sensor platform resource configuration with hrad coded values
 * @param this -- pointer to sensor
 * @param cfg -- pointer to cfg structure which will be filled in
 *               Caller should pass this to sensor_save_registry_pf_cfg
 */
void sns_lsm6dso_registry_def_config(sns_sensor *const this,
                                     sns_registry_phy_sensor_pf_cfg *cfg)
{
  lsm6dso_state *state = (lsm6dso_state*)this->state->state;
  lsm6dso_shared_state *shared_state = lsm6dso_get_shared_state(this);

  sns_registry_phy_sensor_pf_cfg def_cfg = {
    .slave_config =       SLAVE_CONTROL,
    .min_bus_speed_khz =  BUS_MIN_FREQ_KHZ,
    .max_bus_speed_khz =  BUS_MAX_FREQ_KHZ,
    .dri_irq_num =        IRQ_NUM,
#if LSM6DSO_ODR_REGISTRY_FEATURE_ENABLE
    .max_odr =            500,
    .min_odr =            20,
#endif
    .bus_type =           BUS,
    .bus_instance =       BUS_INSTANCE,
    .reg_addr_type =      SNS_REG_ADDR_8_BIT,
    .irq_pull_type =      2,
    .irq_drive_strength = 0,
    .irq_trigger_type =   3,
    .num_rail =           NUM_OF_RAILS,
    .rail_on_state =      LSM6DSO_DEFAULT_REG_CFG_RAIL_ON,
    .rigid_body_type =    RIGID_BODY_TYPE,
,
#if LSM6DSO_USE_I3C
    .i3c_address =        I3C_ADDR,
#endif
    .irq_is_chip_pin =    1,
    .vddio_rail = RAIL_1,
    .vdd_rail = "",
  };

#if defined(SSC_TARGET_SM8250) || defined(SSC_TARGET_SAIPAN) || defined(SSC_TARGET_RENNELL)
  if( shared_state->hw_idx == 1 )
  {
    def_cfg.slave_config    = SLAVE_CONTROL_1;
    def_cfg.dri_irq_num     = IRQ_NUM_1;
    def_cfg.rigid_body_type = RIGID_BODY_TYPE_1;
    def_cfg.i3c_address     = I3C_ADDR_1;
  }
#endif

  *cfg = def_cfg;

  state->is_dri               = LSM6DSO_DEFAULT_REG_CFG_ISDRI;
  state->hardware_id          = LSM6DSO_DEFAULT_REG_CFG_HW_ID;
  state->supports_sync_stream = LSM6DSO_DEFAULT_REG_CFG_SUPPORT_SYN_STREAM;
  shared_state->inst_cfg.accel_resolution_idx = LSM6DSO_DEFAULT_REG_ACCEL_RESOLUTION_IDX;
  shared_state->inst_cfg.gyro_resolution_idx = LSM6DSO_DEFAULT_REG_GYRO_RESOLUTION_IDX;

  sns_memset(shared_state->placement, 0, sizeof(shared_state->placement));

#if defined(SSC_TARGET_SM6150) || defined(SSC_TARGET_SM7150)
  shared_state->inst_cfg.ag_stream_mode = DRI;
#endif
  lsm6dso_init_inst_config(this, shared_state);
}
#endif //LSM6DSO_REGISTRY_DISABLED
