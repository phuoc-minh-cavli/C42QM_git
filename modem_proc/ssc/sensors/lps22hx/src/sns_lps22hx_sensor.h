#pragma once
/**
 * @file sns_lps22hx_sensor.h
 *
 * Copyright (c) 2018-2019, STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the STMicroelectronics nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **/

#include "sns_sensor.h"
#include "sns_data_stream.h"
#include "sns_sensor_uid.h"
#include "sns_pwr_rail_service.h"
#include "sns_lps22hx_hal.h"
#include "sns_registry_util.h"

#include "sns_lps22hx_sensor_instance.h"
#include "sns_math_util.h"
#include "sns_diag_service.h"
#include "sns_lps22hx_ver.h"

#define PRESS_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0x47, 0xbc, 0x92, 0xa9, 0x0e, 0x42, 0x45, 0x5c, \
        0xae, 0xf9, 0x0f, 0x6f, 0x8b, 0xc7, 0xd8, 0xb5  \
      }  \
  }

#define PRESS_SENSOR_TEMP_SUID \
  {  \
    .sensor_uid =  \
      {  \
        0xc5, 0xcc, 0x51, 0xf2, 0x8c, 0x77, 0x46, 0xba, \
        0xae, 0x86, 0x36, 0x5c, 0xbd, 0x47, 0xb9, 0xbc  \
      }  \
  }

#ifndef SUID_IS_NULL
#define SUID_IS_NULL(suid_ptr) ( sns_memcmp( (suid_ptr),                \
                                             &(sns_sensor_uid){{0}},    \
                                             sizeof(sns_sensor_uid) ) == 0 )
#endif

#if LPS22HX_DEVICE_LPS22HB
#define PRESS_SENSOR_NAME   "lps22hb"
#else
#define PRESS_SENSOR_NAME   "lps22hh"
#endif
#if BUILD_DB
#define PRESS_SENSOR_VENDOR "template"
#else
#define PRESS_SENSOR_VENDOR "STMicro"
#endif

#ifndef LPS22HX_ENABLE_REGISTRY_ACCESS

#ifndef SSC_TARGET_SM8250
/** Using SM8150 Platform config as defaults. This is for
 *  test purpose only. All platform specific information will
 *  be available to the Sensor driver via Registry. */
#define LPS22HX_USE_I2C                1

#if LPS22HX_USE_I2C
#define LPS22HX_BUS_TYPE               SNS_BUS_I2C
#define LPS22HX_SLAVE_ADDRESS          0x5C
#define LPS22HX_BUS_INSTANCE           0x04
#define LPS22HX_BUS_MIN_FREQ           400
#define LPS22HX_BUS_MAX_FREQ           400
#else
#define LPS22HX_BUS_TYPE               SNS_BUS_SPI
#define LPS22HX_SLAVE_ADDRESS          0x0
#define LPS22HX_BUS_INSTANCE           0x01
#define LPS22HX_BUS_MIN_FREQ           0
#define LPS22HX_BUS_MAX_FREQ           33*100
#endif

#define LPS22HH_REG_ADDRESS_TYPE       SNS_REG_ADDR_8_BIT
#define LPS22HX_IRQ_NUM                118

#if defined(SSC_TARGET_MDM9205)
#define LPS22HX_NUM_OF_RAILS           2
#define LPS22HX_RAIL_1                 "/pmic/client/dummy_vdd"
#define LPS22HX_RAIL_2                 "/pmic/client/dummy_vdd"
#else
#define LPS22HX_NUM_OF_RAILS           2
#define LPS22HX_RAIL_1                 "/pmic/client/sensor_vddio"
#define LPS22HX_RAIL_2                 "/pmic/client/sensor_vdd"
#endif

#define LPS22HX_RAIL_ON_STATE          SNS_RAIL_ON_NPM

#else

/* Added for PreSil verification when registry is not available.
 */
#define LPS22HX_BUS_TYPE               SNS_BUS_I2C
#define LPS22HX_SLAVE_ADDRESS          0x5C
#define LPS22HX_BUS_INSTANCE           0x5
#define LPS22HX_BUS_MIN_FREQ           400
#define LPS22HX_BUS_MAX_FREQ           400
#define LPS22HX_NUM_OF_RAILS           1
#define LPS22HX_RAIL_1                 "/pmic/client/sensor_vddio"
#define LPS22HH_REG_ADDRESS_TYPE       SNS_REG_ADDR_8_BIT
#define LPS22HX_RAIL_ON_STATE          SNS_RAIL_ON_NPM
#define LPS22HX_IRQ_NUM                129
#endif  // SSC_TARGET_SM8250

#endif  //LPS22HX_ENABLE_REGISTRY_ACCESS



/**
 * LPS22HX ODR definitions
 */
#define LPS22HX_ODR_0                 0.0
#define LPS22HX_ODR_1                 1.0
#define LPS22HX_ODR_10                10.0
#define LPS22HX_ODR_25                25.0
//#define LPS22HX_ODR_50               50.0
//#define LPS22HX_ODR_75               75.0  // LPS22HB
//#define LPS22HX_ODR_100              100.0 // LPS22HH
//#define LPS22HX_ODR_200              200.0 // LPS22HH
#define LPS22HX_ODR_MAX               LPS22HX_ODR_25

/**
 * Pressure ranges(in hectoPascal)
 */
#define LPS22HX_PRESS_RANGE_MIN    260.0
#define LPS22HX_PRESS_RANGE_MAX    1260

/**
 * Pressure resolutions (in hectoPascal/LSB)
 */
#define LPS22HX_PRESS_RESOLUTION    (0.0002) // 1/4096 lsb/hPa

/**
 * Sensor Temprature resolution in deg Celsius/LSB
 */
#define LPS22HX_TEMP_RESOLUTION     (0.01) //100lsb/C

/**
 * Sensor Temperature range in deg Celsius
 */
#define LPS22HX_SENSOR_TEMPERATURE_RANGE_MIN  (-40.0)
#define LPS22HX_SENSOR_TEMPERATURE_RANGE_MAX  (85.0)


/** Supported opertating modes */
#define LPS22HX_MODE_PWRDWN   "PWRDNWN"
#define LPS22HX_ONE_SHOT      "ONE_SHOT"
#define LPS22HX_CONTINUOUS    "CONTINUOUS"

/** Power rail timeout States for the LPS22HX Sensors.*/
typedef enum
{
  LPS22HX_POWER_RAIL_PENDING_NONE,
  LPS22HX_POWER_RAIL_PENDING_INIT,
  LPS22HX_POWER_RAIL_PENDING_SET_CLIENT_REQ,
} lps22hx_power_rail_pending_state;

/** Interrupt Sensor State. */
typedef struct lps22hx_state
{
  sns_data_stream         *reg_data_stream;
  sns_data_stream         *fw_stream;
  sns_data_stream         *timer_stream;
  sns_sensor_uid          reg_suid;
  sns_sensor_uid          irq_suid;
  sns_sensor_uid          timer_suid;
  sns_sensor_uid          acp_suid; // Asynchronous COM Port
  lps22hx_sensor_type     sensor;
  sns_sensor_uid          my_suid;
  lps22hx_com_port_info   com_port_info;
#if !(LPS22HX_POLLING)
  sns_interrupt_req       irq_config;
#endif
  sns_pwr_rail_service    *pwr_rail_service;
  sns_rail_config         rail_config;
  sns_power_rail_state    registry_rail_on_state;

  bool                    hw_is_present;
  bool                    available:1;
  bool                    sensor_client_present:1;

  // sensor configuration
  bool                    is_dri:1;
  bool                    supports_sync_stream:1;
  int64_t                 hardware_id;
  uint8_t                 resolution_idx;

  // registry sensor config
  int8_t                  outstanding_reg_requests;
  uint8_t                 rigid_body_type;
  sns_registry_phy_sensor_cfg registry_cfg;
  uint32_t                registry_persist_version;

  // registry sensor platform config
  bool registry_pf_cfg_received;
  sns_registry_phy_sensor_pf_cfg registry_pf_cfg;

  // axis conversion
  bool                    registry_fac_cal_received;

  // factory calibration
  matrix3                 fac_cal_corr_mat;
  float                   fac_cal_bias[TRIAXIS_NUM];
  float                   fac_cal_scale[TRIAXIS_NUM];

  // placement
  float                   placement[12];

  lps22hx_power_rail_pending_state    power_rail_pend_state;

  // debug
  uint16_t                who_am_i;
  sns_diag_service        *diag_service;
  sns_sync_com_port_service *scp_service;
  size_t                  encoded_event_len;
} lps22hx_state;

#define MAX_SUPPORTED_SENSORS 2

typedef struct {
  lps22hx_sensor_type sensor;
  sns_sensor_api* sensor_api;
  sns_sensor_instance_api* instance_api;
} lps22hx_sensors;

/** Global const tables */
extern const lps22hx_sensors lps22hx_supported_sensors[MAX_SUPPORTED_SENSORS];

/** Functions shared by all LPS22HX Sensors */
/**
 * This function parses the client_request list per Sensor and
 * determines final config for the Sensor Instance.
 *
 * @param[i] this          Sensor reference
 * @param[i] instance      Sensor Instance to config
 * @param[i] sensor_type   Sensor type
 *
 * @return none
 */
void lps22hx_reval_instance_config(sns_sensor *this,
                                   sns_sensor_instance *instance,
                                   lps22hx_sensor_type sensor_type);

/**
 * Sends a request to the SUID Sensor to get SUID of a dependent
 * Sensor.
 *
 * @param[i] this          Sensor reference
 * @param[i] data_type     data_type of dependent Sensor
 * @param[i] data_type_len Length of the data_type string
 */
void lps22hx_send_suid_req(sns_sensor *this, char *const data_type,
                           uint32_t data_type_len);

/**
 * Processes events from SUID Sensor.
 *
 * @param[i] this   Sensor reference
 *
 * @return none
 */
void lps22hx_process_suid_events(sns_sensor *const this);

/**
 * notify_event() Sensor API common between all LPS22HX Sensors.
 *
 * @param this    Sensor reference
 *
 * @return sns_rc
 */
sns_rc lps22hx_sensor_notify_event(sns_sensor *const this);

/**
 * set_client_request() Sensor API common between all LPS22HX
 * Sensors.
 *
 * @param this            Sensor reference
 * @param exist_request   existing request
 * @param new_request     new request
 * @param remove          true to remove request
 *
 * @return sns_sensor_instance*
 */
sns_sensor_instance* lps22hx_set_client_request(sns_sensor *const this,
                                                struct sns_request const *exist_request,
                                                struct sns_request const *new_request,
                                                bool remove);

/**
 * Initializes  Sensor attributes.
 *
 * @param this   Sensor reference
 *
 * @return none
 */
void lps22hx_publish_def_attributes(sns_sensor *const this);
void lps22hx_init_dependencies(sns_sensor *const this);
sns_rc lps22hx_process_registry_events(sns_sensor *const this);
void lps22hx_discover_hw(sns_sensor *const this);
void lps22hx_update_siblings(sns_sensor *const this);
sns_sensor_uid const* lps22hx_get_sensor_uid(sns_sensor const *const this);

sns_rc lps22hx_press_init(sns_sensor *const this);
sns_rc lps22hx_sensor_temp_init(sns_sensor *const this);
sns_rc lps22hx_press_deinit(sns_sensor *const this);
sns_rc lps22hx_sensor_temp_deinit(sns_sensor *const this);

