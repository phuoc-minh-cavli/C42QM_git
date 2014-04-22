#pragma once
/*
 * Copyright (c) 2018-2020, ams AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __SNS_TMD2725_SENSOR___H__
#define __SNS_TMD2725_SENSOR___H__

#include "string.h"
#include <limits.h>
#include "msg.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_async_com_port.pb.h"
#include "sns_async_com_port_pb_utils.h"
#include "sns_attribute_util.h"
#include "sns_com_port_types.h"
#include "sns_data_stream.h"
#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#include "sns_event_service.h"
#include "sns_interrupt.pb.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_proximity.pb.h"
#include "sns_pb_util.h"
#include "sns_physical_sensor_test.pb.h"
#include "sns_pwr_rail_service.h"
#include "sns_rc.h"
#include "sns_register.h"
#include "sns_request.h"
#include "sns_sensor.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_uid.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_std.pb.h"
#include "sns_std_event_gated_sensor.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_stream_service.h"
#include "sns_suid.pb.h"
#include "sns_sync_com_port_service.h"
#include "sns_time.h"
#include "sns_timer.pb.h"
#include "sns_cal.pb.h"
#include "sns_types.h"
#include "sns_registry_util.h"
#include "sns_island_service.h"
#include "sns_tmd2725_version.h"
#include "sns_tmd2725_log.h"

#ifndef AMS_PORT_PLATFORM_H
#define AMS_PORT_PLATFORM_H

#define TMD2725_NUM_DEVICES 2

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ENDIAN_LITTLE
#define AMS_ENDIAN_1    0
#define AMS_ENDIAN_2    8
#else
#define AMS_ENDIAN_2    0
#define AMS_ENDIAN_1    8
#endif

#define AMS_BUILD /* define ams , undef custom */

typedef struct AMS_PORT_portHndl_s {
    sns_sync_com_port_service   *scp_service;
    sns_sync_com_port_handle    scp_handle;
    // for debug use:
    sns_diag_service            *diag;
} AMS_PORT_portHndl;

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
#endif
#ifndef CONFIG_AMS_OPTICAL_SENSOR_PROX
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
#define CONFIG_AMS_OPTICAL_SENSOR_PROX
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* AMS_PORT_PLATFORM_H */

#ifndef __AMS_DEVICE_ALG_ALS_API_H__
#define __AMS_DEVICE_ALG_ALS_API_H__

#define AMS_LUX_AVERAGE_COUNT    20

/* Default Configuration */
#define TMD2725_IS_DRI             TRUE
#define TMD2725_HW_ID              0
#define TMD2725_RES_IDX_ALS        3
#define TMD2725_RES_IDX_PROX       0
#define TMD2725_SYNC_STREAM        FALSE

#define TMD2725_BUS_TYPE           SNS_BUS_I2C
#define TMD2725_SLAVE_CONTROL      57              /* I2C address 0x39 */
#define TMD2725_BUS_MIN_FREQ       400
#define TMD2725_BUS_MAX_FREQ       400
#define TMD2725_REG_ADDRESS_TYPE   SNS_REG_ADDR_8_BIT
#if defined(SSC_TARGET_MDM9205)
#define TMD2725_BUS_INSTANCE       4
#define TMD2725_NUM_OF_RAILS       1
#define TMD2725_RAIL_1             "/pmic/client/dummy_vdd"
#define TMD2725_INT_NUM            27
#else
#define TMD2725_BUS_INSTANCE       1
#define TMD2725_NUM_OF_RAILS       2
#define TMD2725_RAIL_1             "/pmic/client/sensor_vddio"
#define TMD2725_INT_NUM            35
#endif
#define TMD2725_RAIL_2             "/pmic/client/sensor_vdd"
#define TMD2725_RAIL_ON_STATE      SNS_RAIL_ON_NPM

#define TMD2725_INT_PULL_TYPE      SNS_INTERRUPT_PULL_TYPE_NO_PULL
#define TMD2725_INT_IS_CHIP_PIN    TRUE
#define TMD2725_INT_DRIVE_STRENGTH SNS_INTERRUPT_DRIVE_STRENGTH_2_MILLI_AMP
#define TMD2725_INT_TRIGGER_TYPE   SNS_INTERRUPT_TRIGGER_TYPE_FALLING

#define TMD2725_RIGID_BODY_TYPE    SNS_STD_SENSOR_RIGID_BODY_TYPE_DISPLAY

typedef enum _amsRegistryDataWriteType {
    AMS_REGISTRY_WRITE_TYPE_NONE,
    AMS_REGISTRY_WRITE_TYPE_ALS_FAC_CAL,
    AMS_REGISTRY_WRITE_TYPE_PROX_FAC_CAL,
    AMS_REGISTRY_WRITE_TYPE_PROX_HW_CAL
}amsRegistryDataWriteType_t;

typedef enum _amsAlsAdaptive {
    ADAPTIVE_ALS_NO_REQUEST,
    ADAPTIVE_ALS_TIME_INC_REQUEST,
    ADAPTIVE_ALS_TIME_DEC_REQUEST,
    ADAPTIVE_ALS_GAIN_INC_REQUEST,
    ADAPTIVE_ALS_GAIN_DEC_REQUEST
}amsAlsAdaptive_t;

typedef enum _amsAlsStatus {
    ALS_STATUS_IRQ  = (1 << 0),
    ALS_STATUS_RDY  = (1 << 1),
    ALS_STATUS_OVFL = (1 << 2)
}amsAlsStatus_t;

typedef struct _alsData{
    uint16_t ch0ADC;
    uint16_t ch1ADC;
} alsData_t;

typedef struct _amsAlsCalibration {
    uint32_t Time_base; /* in uSec */
    uint32_t adcMaxCount;
    uint16_t calibrationFactor; /* default 1000 */
    uint8_t thresholdLow;
    uint8_t thresholdHigh;
    int32_t D_factor;
    int32_t L0_factor;
    int32_t L1_factor;
    int32_t L2_factor;
    int32_t L3_factor;
} amsAlsCalibration_t;

typedef struct _amsAlsDefaultConfig{
    uint32_t     aTime;
    uint32_t     wTime;
    bool         wLong;
    uint16_t     aGain;
    uint8_t      aPers;
    uint8_t      aUpperThrs;
    uint8_t      aLowerThrs;
    uint8_t      aThrsWidth;
} amsAlsDefaultConfig_t;

typedef struct _amsAlsInitData {
    bool adaptive;
    bool irRejection;
    uint32_t time_us;
    uint32_t gain;
    amsAlsCalibration_t calibration;
    amsAlsDefaultConfig_t alsDefaultConfig;
} amsAlsInitData_t;

typedef struct _amsALSConf {
    uint32_t time_us;
    uint32_t gain;
    uint8_t thresholdLow;
    uint8_t thresholdHigh;
} amsAlsConf_t;

typedef struct _amsAlsDataSet {
    alsData_t *datasetArray;
    uint64_t timeStamp;
    uint8_t status;
} amsAlsDataSet_t;

typedef struct _amsAlsResult {
    uint32_t    irrClear;
    uint32_t    irrRed;
    uint32_t    IR;
    uint32_t    mLux;
    uint32_t    mLux_ave;
    uint32_t    mMaxLux;
    uint32_t    CCT;
    amsAlsAdaptive_t adaptive;
    uint32_t    rawClear;
    uint32_t    rawIR;
    uint32_t    quality;
    /* 2 bits per slot = 4 quality states */
    #define ALS_QUALITY_LUX_MASK    0x00000003
    #define ALS_QUALITY_CCT_MASK    0x0000000c
    /* Define BAD & GOOD now leaving 2 states for future use */
    #define ALS_QUALITY_LUX_BAD     0x00000000
    #define ALS_QUALITY_LUX_GOOD    0x00000003
    #define ALS_QUALITY_CCT_BAD     0x00000000
    #define ALS_QUALITY_CCT_GOOD    0x0000000c
}amsAlsResult_t;

typedef struct _amsAlsContext {
    uint64_t lastTimeStamp;
    uint32_t ave_lux[AMS_LUX_AVERAGE_COUNT];
    uint32_t ave_lux_index;
    uint32_t cpl;
    uint32_t time_us;
    amsAlsCalibration_t calibration;
    amsAlsDefaultConfig_t alsDefaultConfigData;
    amsAlsResult_t results;
    bool adaptive;
    uint16_t saturation;
    uint32_t gain;
    uint32_t changedAlsGain;
    uint32_t previousGain;
    uint32_t previousLux;
    bool notStableMeasurement;
    /* These pertain to reporting: theoretical max possible lux and
     * uW/cm^2, and their respective reporting resolutions
     */
    float maxLux;
    float luxResolution;
} amsAlsContext_t;

typedef struct _amsAlsAlgInfo {
    char * algName;
    amsAlsCalibration_t calibrationData;
    int (*initAlg) (amsAlsContext_t * ctx, amsAlsInitData_t * initData);
    int (*processData) (sns_sensor_instance *const this, amsAlsContext_t * ctx, amsAlsDataSet_t * inputData);
    int (*getResult) (amsAlsContext_t * ctx, amsAlsResult_t * outData);
    int (*setConfig) (amsAlsContext_t * ctx, amsAlsConf_t * inputData);
    int (*getConfig) (amsAlsContext_t * ctx, amsAlsConf_t * outputData);
} amsAlsAlgoInfo_t;

extern int amsAlg_als_getAlgInfo_tmd2725 (amsAlsAlgoInfo_t * info);
extern int amsAlg_als_initAlg_tmd2725 (amsAlsContext_t * ctx, amsAlsInitData_t * initData);
extern int amsAlg_als_processData_tmd2725(sns_sensor_instance *const this, amsAlsContext_t * ctx, amsAlsDataSet_t * inputData);
extern int amsAlg_als_getResult_tmd2725(amsAlsContext_t * ctx, amsAlsResult_t * outData);
extern int amsAlg_als_setConfig_tmd2725(amsAlsContext_t * ctx, amsAlsConf_t * inputData);
extern int amsAlg_als_getConfig_tmd2725(amsAlsContext_t * ctx, amsAlsConf_t * outputData);
extern void als_getReportingParams_tmd2725(amsAlsContext_t * ctx,
                              float *maxLux,
                              float *luxResolution);

#endif

#ifndef __AMS_DEVICE_ALG_ALS_CORE_H__
#define __AMS_DEVICE_ALG_ALS_CORE_H__

#ifndef ULLONG_MAX
#define ULLONG_MAX        ((uint64_t)-1)
#endif
#endif

#ifndef __AMS_CCB_CORE_CONROL_BLOCK_DYN_PROX_H__
#define __AMS_CCB_CORE_CONROL_BLOCK_DYN_PROX_H__

#define AMS_PRX_AVERAGE_COUNT                       8
#define AMS_PRX_DEFAULT_PULSE_COUNT                 17
#define AMS_PRX_DEFAULT_PULSE_LENGTH                1      /* 8 uSec */
#define AMS_PRX_DEFAULT_LED_CURRENT                 24    /* mA */
#define AMS_PRX_DEFAULT_GAIN                        4000
#define AMS_PRX_DEFAULT_PERS                        2
#define AMS_PRX_DEFAULT_PTIME                       49896  /* uSec, This is the maximum PTIME */
#define AMS_PRX_ADC_NO_BITS                         8
#define AMS_PRX_POFFSET_SIGN                        0
#define AMS_PRX_POFFSET_VALUE                       50
#define AMS_PRX_DEFAULT_BIN_SEARCH                  2
#define AMS_DEFAULT_WAIT_TIME                      28000  /*in us */
#define AMS_MIN_WAIT_TIME                          2800   /*in us */

#define AMS_PRX_DEFAULT_ADC_THRESHOLD               0
#define AMS_PRX_DEFAULT_ADAPTIVE_THRESHOLD_FACTOR   2

/* For parts with a prox ADC of less than 16 bits, these numbers still
 * apply given that prox data is put thru the AMS_NORMALISE_16() method.
 */
#define AMS_PRX_DEFAULT_NEARBY_HARD_THRESHOLD       4864
#define AMS_PRX_DEFAULT_MIN_THRESHOLD               255
#define AMS_PRX_DEFAULT_THRESHOLD_HYSTERESIS        2000
#define AMS_PRX_DEFAULT_HARD_THRESHOLD              14848

typedef enum{
    PRX_STATE_UNKNOWN,
    PRX_STATE_BEGIN,
    PRX_STATE_CAL,
    PRX_STATE_FILLUP,
    PRX_STATE_OPER,
    PRX_STATE_FIRST_MEASURE,
    PRX_STATE_STATIC_NO_DETECT,
    PRX_STATE_STATIC_DETECT,
    PRX_STATE_FACTORY_CALIBRATION,
    PRX_STATE_HARDWARE_CALIBRATION,
    PRX_STATE_LAST
}ams_prox_dyn_state_t;

typedef enum{
    PRX_ACCURACY_UNRELIABLE,
    PRX_ACCURACY_LOW,
    PRX_ACCURACY_MED,
    PRX_ACCURACY_HIGH,
}ams_prox_accuracy_t;

typedef struct _amsProxDefaultConfig{
    uint32_t     pTime;
    uint16_t     pPulseLen;
    uint8_t      pPulse;
    uint16_t     pGain;
    uint8_t      pLdrive;
    uint8_t      pPers;
    uint16_t     pUpperThrs;
    uint16_t     pLowerThrs;
    bool         pOffsetSign;
    int16_t      proxOffset;
} amsProxDefaultConfig_t;

typedef struct _amsProxResult {
    uint32_t    pData;
    bool        nearByState;
    int16_t     proxOffset;
}amsProxResult_t;

typedef struct _amsProxContext {
    amsProxDefaultConfig_t proxDefaultConfigData;
    amsProxResult_t results;
} amsProxContext_t;

typedef struct {
    uint32_t     pTime;
    uint32_t     pwTime_us;
    uint16_t     pPulseLen;
    uint8_t      pPulse;
    uint16_t     pGain;
    uint8_t      pLdrive;
    uint8_t      pPers;
    uint16_t     pUpperThrs;
    uint16_t     pLowerThrs;
    bool         pOffsetSign;
    int16_t      proxOffset;
    uint8_t      pBinSearch;
} ams_ccb_default_prox_config_data_t;

typedef struct {
    bool     dynamicMode;
    bool     calibrate;
    bool     proxPoffsetCalibrate;
    uint32_t ptime_us;
    uint32_t gain;
    uint16_t nearByHardThreshold;
    uint16_t nearByThresholdHysteresis;
    uint8_t  adcThreshold;
    uint8_t  numberAdcBits;
    uint32_t pTime;
    uint32_t pwTime_us;
    uint16_t pPulseLen;
    uint8_t  pPulse;
    uint16_t pGain;
    uint8_t  pLdrive;
    uint8_t  pPers;
    uint16_t pUpperThrs;
    uint16_t pLowerThrs;
    bool     pOffsetSign;
    int16_t  proxOffset;
    uint8_t  pBinSearch;
    ams_ccb_default_prox_config_data_t prox_default_config_values;
}ams_ccb_proximity_init_t;

typedef struct {
    amsProxContext_t ctxProx;
    ams_ccb_proximity_init_t initData;
    ams_prox_dyn_state_t state;
    bool nearBy;
    uint32_t pDataNorm;
    int16_t  proxOffset;
    uint32_t pDataNormLast;
    uint32_t pDataNormTrigger;
    uint32_t pDataAve;
    uint32_t pDataLow;
    uint32_t pDataMax;
    uint16_t ave[AMS_PRX_AVERAGE_COUNT];
    uint16_t pData;
    uint8_t  sensitivity;
    uint8_t  ave_index;
    uint8_t  adcThreshold;
    ams_prox_accuracy_t accuracy;
    bool     prox_is_running;
    bool     wait_for_clear;
}ams_ccb_proximity_ctx_t;

typedef struct {
    uint8_t sensitivity;
}ams_ccb_proximity_config_t;

typedef struct {
    uint32_t alsData;
    uint16_t pData;
    uint8_t  statusReg;
}ams_ccb_proximity_dataSet_t;

typedef struct {
    bool nearBy;
    uint16_t proximity;
    int16_t  proxOffset;
    ams_prox_accuracy_t accuracy;
}ams_ccb_proximity_result_t;

typedef struct {
    uint32_t hardThreshold;
}ams_ccb_proximity_calibration_t;

typedef struct {
    char *algName;
    ams_ccb_proximity_calibration_t defaultCalibrationData;
}ams_ccb_proximity_info_t;

#endif

#ifndef __AMS_CCB_CORE_CONROL_BLOCK_ALS_H__
#define __AMS_CCB_CORE_CONROL_BLOCK_ALS_H__

#define MASK_HIGH    0xFF
#define MASK_LOW     0x00

#if !defined(CONFIG_ALS_CAL_TARGET)
#define CONFIG_ALS_CAL_TARGET          300 /* lux */
#endif
#if !defined(CONFIG_ALS_CAL_TARGET_TOLERANCE)
#define CONFIG_ALS_CAL_TARGET_TOLERANCE  15 /* lux */
#endif

typedef struct {
    uint32_t calibrationFactor;
    int32_t luxTarget;
    int8_t luxTargetError;
    uint32_t Time_base; /* in uSec */
    uint32_t adcMaxCount;
    uint8_t thresholdLow;
    uint8_t thresholdHigh;
    int32_t D_factor;
    int32_t L0_factor;
    int32_t L1_factor;
    int32_t L2_factor;
    int32_t L3_factor;
}ams_ccb_als_calibration_t;

typedef struct {
    uint32_t     aTime;
    uint32_t     wTime;
    bool         wLong;
    uint16_t     aGain;
    uint8_t      aPers;
    uint8_t      aUpperThrs;
    uint8_t      aLowerThrs;
    uint8_t      aThrsWidth;
} ams_ccb_default_als_config_data_t;

typedef struct {
    uint32_t uSecTime;
    uint32_t gain;
    uint8_t  threshold;
    uint8_t  clearChUpperThreshold;
    uint8_t  clearChLowerThreshold;
}ams_ccb_als_config_t;

typedef struct {
    bool calibrate;
    bool autoGain;
    uint8_t hysteresis;
    uint16_t  sampleRate;
    ams_ccb_als_config_t configData;
    ams_ccb_als_calibration_t calibrationData;
    ams_ccb_default_als_config_data_t als_default_config_values;
}ams_ccb_als_init_t;

typedef enum {
    AMS_CCB_ALS_INIT,
    AMS_CCB_ALS_RGB,
    AMS_CCB_ALS_AUTOGAIN,
    AMS_CCB_ALS_CALIBRATION_INIT,
    AMS_CCB_ALS_CALIBRATION_COLLECT_DATA,
    AMS_CCB_ALS_CALIBRATION_CHECK,
    AMS_CCB_ALS_GAIN,
    AMS_CCB_ALS_LAST_STATE
}ams_ccb_als_state_t;

typedef struct {
    char * algName;
    ams_ccb_als_calibration_t defaultCalibrationData;
}ams_ccb_als_info_t;

typedef struct {
    ams_ccb_als_state_t state;
    amsAlsContext_t ctxAlgAls;
    ams_ccb_als_init_t initData;
    uint16_t bufferCounter;
    uint16_t shadowAiltReg;
    uint16_t shadowAihtReg;
    bool wait_for_clear;
}ams_ccb_als_ctx_t;

typedef struct {
    uint8_t  statusReg;
}ams_ccb_als_dataSet_t;

typedef struct {
    uint32_t mLux;
    uint32_t colorTemp;
    uint32_t clear;
    uint32_t red;
    uint32_t ir;
    uint32_t rawClear;
    uint32_t rawIR;
    uint32_t quality;
}ams_ccb_als_result_t;

typedef struct _fifo{
    uint16_t AdcClear;
    uint16_t AdcIR;
} adcDataSet_t;

extern uint32_t changeAlsGain(sns_sensor_instance *const this, void * dcbCtx, adcDataSet_t adcData, ams_ccb_als_dataSet_t *alsData);

#endif

#ifndef __AMS_DEVICE_CONTROL_BLOCK_H__
#define __AMS_DEVICE_CONTROL_BLOCK_H__

/* This DCB code implements the ALWAYS_READ option unconditionally;
 * this symbol is for the benefit of, e.g., the ALS CCB which must
 * also work with other DCBs that do not (yet) support it.
 */
#define HAVE_OPTION__ALWAYS_READ  1

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
#define AMS_PRX_TIMEBASE (88) /* in usec, see data sheet */

typedef struct {
    bool     nearBy;
    uint16_t pData;
    int16_t  proxOffset;
    ams_prox_accuracy_t accuracy;
} ams_apiPrx_t;
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB

typedef struct {
    uint32_t mLux;
    uint32_t ir;
    uint32_t rawClear;
    uint16_t rawIR;
    uint32_t quality;
    /* 2 bits per slot can hold 4 quality states */
    #define ALS_QUALITY_LUX_MASK    0x00000003
    #define ALS_QUALITY_CCT_MASK    0x0000000c
    /* Define BAD & GOOD now leaving 2 states for future use */
    #define ALS_QUALITY_LUX_BAD     0x00000000
    #define ALS_QUALITY_LUX_GOOD    0x00000003
    #define ALS_QUALITY_CCT_BAD     0x00000000
    #define ALS_QUALITY_CCT_GOOD    0x0000000c
} ams_apiAls_t;
#endif

#define AMS_DEVICE_OFF_TO_IDLE_MS       10
#define AMS_USEC_PER_TICK               (2780)
#define AMS_ALS_MAX_COUNTS_PER_TICK     (1024)
#define AMS_ALS_MAX_COUNTS              (65535)
#define ACTUAL_USEC(x)                  (((x + AMS_USEC_PER_TICK / 2) / AMS_USEC_PER_TICK) * AMS_USEC_PER_TICK)
#define AMS_ALS_USEC_TO_REG(x)          (x / AMS_USEC_PER_TICK)
#define AMS_DEFAULT_REPORTTIME_US       (1000000) /* Max 8 seconds */
#define AMS_PRX_PGLD_TO_REG(x)          ((x-4)/8)

#define AMS_CALIBRATION_DONE                (-1)
#define AMS_CALIBRATION_DONE_BUT_FAILED     (-2)

typedef enum _deviceIdentifier_e {
    AMS_UNKNOWN_DEVICE,
    AMS_TMD2725,
    AMS_LAST_DEVICE
} tmd2725_deviceIdentifier_e;

#define AMS_DEVICE_ID       0xE4
#define AMS_DEVICE_ID_MASK  0xFC
#define AMS_REV_ID          0x02
#define AMS_REV_ID_MASK     0x07

#define AMS_PRX_PERS_TO_REG(x)         (x << 4)
#define AMS_PRX_REG_TO_PERS(x)         (x >> 4)
#define AMS_LED_MAX_mA                 192
#define AMS_LED_mA_PER_STEP            6
#define AMS_PRX_CURRENT_TO_REG(mA)     ((((mA) > 192) ? 31 : (((mA) / AMS_LED_mA_PER_STEP ) - 1 )) << 0)
#define AMS_ALS_PERS_TO_REG(x)         (x << 0)
#define AMS_ALS_REG_TO_PERS(x)         (x >> 0)

typedef enum _deviceRegisters {
    DEVREG_ENABLE,      /* 0x80 */
    DEVREG_ATIME,
    DEVREG_PTIME,           /*DEVREG_PRATE,*/
    DEVREG_WTIME,
    DEVREG_AILTL,
    DEVREG_AILTH,
    DEVREG_AIHTL,
    DEVREG_AIHTH,
    DEVREG_PILT,
    DEVREG_PIHT,
    DEVREG_PERS,
    DEVREG_CFG0,
    DEVREG_PCFG0,
    DEVREG_PCFG1,
    DEVREG_CFG1,
    DEVREG_REVID,
    DEVREG_ID,
    DEVREG_STATUS,      /* 0x93 */
    DEVREG_CDATAL,
    DEVREG_CDATAH,
    DEVREG_ALSIRL,
    DEVREG_ALSIRH,
    DEVREG_PDATA,
    DEVREG_CFG2,
    DEVREG_CFG3,
    DEVREG_POFFSET_L,
    DEVREG_POFFSET_H,
    DEVREG_CALIB,
    DEVREG_CALIBCFG,
    DEVREG_CALIBSTAT,
    DEVREG_INTENAB,
    DEVREG_POFFSET_MAG,
    DEVREG_POFFSET_SIGN,

    DEVREG_REG_MAX
}ams_tmd2725_deviceRegister_t;

typedef enum _2725_regOptions {

    PON                = 0x01,  /* register 0x80 */
    AEN                = 0x02,
    PEN                = 0x04,
    WEN                = 0x08,

    WLONG              = 0x04,  /* register 0x8D */

    PPLEN_4uS          = 0x00,  /* register 0x8E */
    PPLEN_8uS          = 0x40,
    PPLEN_16uS         = 0x80,
    PPLEN_32uS         = 0xC0,

    PGAIN_1            = 0x00,  /* register 0x8F */
    PGAIN_2            = 0x40,
    PGAIN_4            = 0x80,
    PGAIN_8            = 0xC0,

    AGAIN_1            = 0x00,  /* register 0x90 */
    AGAIN_4            = 0x01,
    AGAIN_16           = 0x02,
    AGAIN_64           = 0x03,

    PSAT_AMBIENT       = 0x01,  /* register 0x93 */
    PSAT_REFLECTIVE    = 0x02,  /* register 0x93 */
    ZINT               = 0x04,
    CINT               = 0x08,
    AINT               = 0x10,
    PINT               = 0x20,
    PSAT               = 0x40,
    ASAT               = 0x80,
    ALS_INT_ALL        = (AINT),

    AGAINL             = 0x04,  /* register 0x9F */

    INT_READ_CLEAR     = 0x80,  /* register 0xAB */
    SAI                = 0x10,

    ELECT_CAL          = 0x20,  /* register 0xD7 */
    START_OFFSET_CALIB = 0x01,

    BINSRCH_TARGET_0   = 0x00,  /* register 0xD9 */
    BINSRCH_TARGET_1   = 0x20,
    BINSRCH_TARGET_3   = 0x40,
    BINSRCH_TARGET_7   = 0x60,
    BINSRCH_TARGET_15  = 0x80,
    BINSRCH_TARGET_31  = 0xA0,
    BINSRCH_TARGET_63  = 0xC0,
    BINSRCH_TARGET_127 = 0xE0,

    CALIB_FINISHED     = 0x01,  /* register 0xDC */

    CIEN               = 0x08,  /* register 0xDD */
    AIEN               = 0x10,
    PIEN               = 0x20,
    PSIEN              = 0x40,
    ASIEN              = 0x80,
#ifdef CONFIG_AMS_USE_ASIEN
    AMS_ALL_IENS       = (AIEN+PIEN+ASIEN+CIEN),
#else
    AMS_ALL_IENS       = (AIEN | PIEN | CIEN),
#endif

    LAST_IN_ENUM_LIST
}ams_regOptions_t;

typedef enum _2725_regMasks {
    MASK_PON             = 0x01,   /* register 0x80 */
    MASK_AEN             = 0x02,
    MASK_PEN             = 0x04,
    MASK_WEN             = 0x08,

    MASK_ATIME           = 0xFF,   /* register 0x81 */

    MASK_PRATE           = 0xFF,   /* register 0x82 */

    MASK_WTIME           = 0xFF,   /* register 0x83 */

    MASK_AILTL           = 0xFFFF, /* register 0x84 */

    MASK_AIHTL           = 0xFFFF, /* register 0x86 */

    MASK_PILT            = 0xFF,   /* register 0x88 */

    MASK_PILH            = 0xFF,   /* register 0x8A */

    MASK_APERS           = 0x0F,   /* register 0x8C */
    MASK_PPERS           = 0xF0,

    MASK_WLONG           = 0x04,   /* register 0x8D */

    MASK_PPULSE_LEN      = 0xC0,   /* register 0x8E */
    MASK_PPULSE          = 0x3F,

    MASK_PGAIN           = 0xC0,   /* register 0x8F */
    MASK_PLDRIVE         = 0x1F,

    MASK_AGAIN           = 0x03,   /* register 0x90 */

    MASK_REV_ID          = 0x07,   /* register 0x91 */

    MASK_ID              = 0xFC,   /* register 0x92 */

    MASK_PSAT_REFLECTIVE = 0x01,   /* register 0x93 */
    MASK_CINT            = 0x08,
    MASK_AINT            = 0x10,
    MASK_PINT            = 0x20,
    MASK_PSAT            = 0x40,
    MASK_ASAT            = 0x80,
    MASK_ALS_INT_ALL     = MASK_AINT,
    MASK_RELEVANT_INTS  = (0
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
                             | 0x68
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
                             | 0x10
#endif
                          ),

    MASK_CDATA           = 0xFFFF, /* register 0x94 */

    MASK_RDATA           = 0xFFFF, /* register 0x96 */

    MASK_GDATA           = 0xFFFF, /* register 0x98 */

    MASK_BDATA           = 0xFFFF, /* register 0x9A */

    MASK_PDATA           = 0xFFFF, /* register 0x9C */

    MASK_AGAINL          = 0x01,   /* register 0x9F */

    MASK_INT_READ_CLEAR  = 0x80,   /* register 0xAB */
    MASK_SAI             = 0x10,

    MASK_POFFSET_L       = 0xFF,   /* register 0xC0 */

    MASK_POFFSET_H       = 0x01,   /* register 0xC1 */

    MASK_ELECT_CAL       = 0x20,   /* register 0xD7 */
    MASK_OFFSET_CALIB    = 0x01,

    MASK_BINSRCH_TARGET  = 0xE0,   /* register 0xD9 */
    MASK_AUTO_OFFSET_ADJ = 0x08,
    MASK_PRX_DATA_AVG    = 0x07,

    MASK_CALIB_FINISHED  = 0x01,   /* register 0xDC */

    MASK_CIEN            = 0x08,   /* register 0xDD */
    MASK_AIEN            = 0x10,
    MASK_PIEN            = 0x20,
    MASK_PSIEN           = 0x40,
    MASK_ASIEN           = 0x80,
#ifdef CONFIG_AMS_USE_ASIEN
    MASK_ALS_IEN_ALL     = (MASK_AIEN | MASK_ASIEN),
#else
    MASK_ALS_IEN_ALL     = (MASK_AIEN),
#endif

    MASK_POFFSET_MAG     = 0xFF,   /* register 0xE6 */

    MASK_POFFSET_SIGN    = 0x01,   /* register 0xE7 */

    MASK_LAST_IN_ENUMLIST
}ams_regMask_t;


#define AMS_ENABLE_ALS_EX()         {ctx->shadowEnableReg |= (AEN); \
    ams_setByte (ctx->portHndl, DEVREG_ENABLE, ctx->shadowEnableReg); \
    ams_setField (ctx->portHndl, DEVREG_INTENAB, MASK_HIGH, (MASK_AIEN | MASK_ASIEN));\
    }
#define AMS_DISABLE_ALS_EX()        {ctx->shadowEnableReg &= ~(AEN); \
    ams_setByte (ctx->portHndl, DEVREG_ENABLE, ctx->shadowEnableReg); \
    ams_setField (ctx->portHndl, DEVREG_INTENAB, MASK_LOW, (MASK_AIEN | MASK_ASIEN));\
    }
/* REENABLE only enables those that were on record as being enabled */
#define AMS_REENABLE()              ams_setEnable(ctx)
/* DISABLE_ALS disables ALS w/o recording that as its new state */
#define AMS_DISABLE_ALS()           {ams_setField(ctx->portHndl, DEVREG_ENABLE,  MASK_LOW,  (MASK_AEN));}
#define AMS_REENABLE_ALS()          {ams_setField(ctx->portHndl, DEVREG_ENABLE,  MASK_HIGH, (MASK_AEN));}

#define AMS_CLR_ALS_INT()           {ams_setByte (ctx->portHndl, DEVREG_STATUS, (AINT | ASAT));}
#define AMS_GET_ALS_TIME(unsigned32_mSec)   {uint8_t atime; \
    ams_getByte (ctx->portHndl, DEVREG_ATIME, &atime); \
    (unsigned32_mSec) = ((((uint32_t)atime) + 1) * AMS_USEC_PER_TICK) / 1000; \
}
#define AMS_SET_ALS_TIME(mSec)      {ams_setByte (ctx->portHndl, DEVREG_ATIME,   alsTimeUsToReg(mSec));}
/* gainLevel is an index into an array of available
 * gains, not (necessarily) the register code for that gain; the array
 * is private to the DCB.
 */
#define AMS_GET_ALS_GAIN(scaledGain, gainLevel) {scaledGain = ctx->alsGain; \
    gainLevel = alsGainToLevel(scaledGain);}


#define AMS_SET_ALS_GAIN(mGain)     { \
    uint8_t enbl, cfg1, cfg2; \
    ctx->alsGain = mGain; \
    alsGainLevelToRegs(alsGainToLevel(mGain), &cfg1, &cfg2); \
    ams_getByte(ctx->portHndl, DEVREG_ENABLE, &enbl); \
    if(enbl & MASK_AEN) ams_setByte(ctx->portHndl, DEVREG_ENABLE, (enbl & ~(MASK_AEN))); \
    ams_setByte(ctx->portHndl, DEVREG_CFG1, cfg1); \
    ams_setByte(ctx->portHndl, DEVREG_CFG2, cfg2); \
    if(enbl & MASK_AEN) ams_setByte(ctx->portHndl, DEVREG_ENABLE, enbl); \
    }

#define AMS_SET_ALS_THRESHOLDS(tl,th) {uint16_t tvals[2]; tvals[0]=(tl); tvals[1]=(th); ams_setBuf (ctx->portHndl, DEVREG_AILTL, ((uint8_t*)tvals), 4);}
#define AMS_SET_ALS_PERS(x)         {ams_setField(ctx->portHndl, DEVREG_PERS, (x), MASK_APERS);}
#define AMS_ALS_GET_CR(x)           {memset((uint8_t *)&adcData, 0, sizeof(adcData)); ams_getBuf(ctx->portHndl, DEVREG_CDATAL, (uint8_t *)&adcData, 4);}
#define AMS_SET_WTIME(x)         {ams_setByte(ctx->portHndl, DEVREG_WTIME, (x));}
#define AMS_DISABLE_WTIME()           {ams_setField(ctx->portHndl, DEVREG_ENABLE,  MASK_LOW,  (MASK_WEN));}
#define AMS_REENABLE_WTIME()          {ams_setField(ctx->portHndl, DEVREG_ENABLE,  MASK_HIGH, (MASK_WEN));}

#define AMS_ENABLE_PROX_EX()        {ctx->shadowEnableReg |= (PEN); \
    ams_setByte (ctx->portHndl, DEVREG_ENABLE, ctx->shadowEnableReg); \
    ams_setField (ctx->portHndl, DEVREG_INTENAB, MASK_HIGH, (MASK_PIEN | MASK_PSIEN));\
    }
#define AMS_DISABLE_PROX_EX()       {ctx->shadowEnableReg &= ~(PEN); \
    ams_setByte (ctx->portHndl, DEVREG_ENABLE, ctx->shadowEnableReg); \
    ams_setField (ctx->portHndl, DEVREG_INTENAB, MASK_LOW, (MASK_PIEN | MASK_PSIEN));\
    }

/* REMINDER:  PTIME (aka PRATE) determines ONLY the rep rate of prox
 * cycles performed for prox averaging (as set by CALIBCFG[0:2], which
 * is in effect during the mainstream too, not just during cal).
 */
#define AMS_SET_PROX_TIME(uSec)     {ams_setByte(ctx->portHndl,  DEVREG_PTIME/*DEVREG_PRATE*/,   proxTimeToReg(uSec));}

#define AMS_CLR_PROX_INT()                  {ams_setByte (ctx->portHndl, DEVREG_STATUS, (PINT | CINT | PSAT | PSAT_REFLECTIVE));}
#define AMS_ENABLE_PROX_INT()               {ams_setField(ctx->portHndl, DEVREG_INTENAB, MASK_HIGH, MASK_PIEN);}
#define AMS_SET_PROX_GAIN(mGain)            {ams_setField(ctx->portHndl, DEVREG_PCFG1,  proxGainToReg(mGain),   MASK_PGAIN);}
#define AMS_SET_PROX_THRS_LOW(x)            {ams_setByte (ctx->portHndl, DEVREG_PILT, (uint8_t)(x));}
#define AMS_SET_PROX_THRS_HIGH(x)           {ams_setByte (ctx->portHndl, DEVREG_PIHT, (uint8_t)(x));}
#define AMS_SET_PROX_THRS_LOW_HIGH(x, y)    { uint8_t buf[2] = {(uint8_t)(x), (uint8_t)(y)}; \
                                              sns_port_vector port_vec[2] = { \
                                                { .is_write = true, .reg_addr = deviceRegisterDefinition[DEVREG_PILT].address, .bytes = 1, .buffer = &buf[0] }, \
                                                { .is_write = true, .reg_addr = deviceRegisterDefinition[DEVREG_PIHT].address, .bytes = 1, .buffer = &buf[1] }  \
                                              }; \
                                              ams_rwVec (ctx->portHndl, port_vec, 2); \
                                            }

#define AMS_SET_PROX_PERS(reps)             {ams_setField(ctx->portHndl, DEVREG_PERS,    (uint8_t)(AMS_PRX_PERS_TO_REG(reps)), MASK_PPERS);}
#define AMS_SET_PROX_LEDDRIVE(mA)           {ams_setField(ctx->portHndl, DEVREG_PCFG1,   AMS_PRX_CURRENT_TO_REG(mA),  MASK_PLDRIVE);}
#define AMS_SET_PROX_PULSE_LEN(x)           {ams_setField(ctx->portHndl, DEVREG_PCFG0, (x << 6), MASK_PPULSE_LEN);}
#define AMS_SET_PROX_PULSE_COUNT(x)         {ams_setField(ctx->portHndl, DEVREG_PCFG0, ((x) - 1) << 0, MASK_PPULSE);}
#define AMS_SET_PROX_POFFSET_MAGNITUDE(x)   {ams_setField(ctx->portHndl, DEVREG_POFFSET_L, x, MASK_POFFSET_L);}
#define AMS_SET_PROX_POFFSET_SIGN(x)        {ams_setField(ctx->portHndl, DEVREG_POFFSET_H, x, MASK_POFFSET_H);}

#define AMS_GET_PROX(unsigned32_result) {uint8_t pdata; \
    ams_getByte(ctx->portHndl,  DEVREG_PDATA, &pdata); \
    (unsigned32_result) = (uint32_t)pdata; \
}

#define AMS_SET_CAL_BINSRCH(x)      {ams_setField(ctx->portHndl, DEVREG_CALIBCFG, (x << 5), MASK_BINSRCH_TARGET);}
#define AMS_SET_CAL_PROX_AUTO_OFFSET_ADJUST()  {ams_setField(ctx->portHndl, DEVREG_CALIBCFG, MASK_HIGH, MASK_AUTO_OFFSET_ADJ);}

#define AMS_ENABLE_CAL_INT()        {if (ctx->allowIens) ams_setField(ctx->portHndl, DEVREG_INTENAB, MASK_HIGH, MASK_CINT);}
#define AMS_DISABLE_CAL_INT()       {if (ctx->allowIens) ams_setField(ctx->portHndl, DEVREG_INTENAB, MASK_LOW,  MASK_CINT);}

typedef struct _deviceRegisterTable {
    uint8_t address;
    uint8_t resetValue;
}deviceRegisterTable_t;

typedef enum _tmd2725_config_options {
    AMS_CONFIG_ENABLE,
    AMS_CONFIG_THRESHOLD,
    AMS_CONFIG_ALWAYS_READ,     /* see .alwaysReadXxx in ams_deviceCtx_t */
    AMS_CONFIG_ALLOW_INTS,      /* tell dev whether OS has connected the IRQ line */
    AMS_CONFIG_OPTION_LAST
}deviceConfigOptions_t;

typedef enum _2725_report_type {
    AMS_REPORT_NONE        = 0x00,
    AMS_REPORT_ON_CHANGE   = 0x01,
    AMS_REPORT_STREAMING   = 0x02,
    AMS_REPORT_OC_AND_STRM = 0x03,
    AMS_REPORT_TYPE_LAST
}ams_reportType_t;

typedef enum _2725_als_cal_type {
    AMS_ALS_CAL_TYPE_ON_CHANGE   = 0x01,
    AMS_ALS_CAL_TYPE_STREAMING   = 0x02,
}ams_alsCalType_t;

typedef enum _tmd2725_mode {
    MODE_OFF            = (0),
    MODE_ALS_LUX        = (1 << 0),
    MODE_ALS_ALL        = (MODE_ALS_LUX),
    MODE_PROX           = (1 << 5),
    MODE_UNKNOWN    /* must be in last position */
} ams_mode_t;

typedef enum _tmd2725_configureFeature {
    AMS_CONFIG_PROX,
    AMS_CONFIG_ALS_LUX,
    AMS_CONFIG_GLOBAL,
    AMS_CONFIG_FEATURE_LAST
}ams_configureFeature_t;

typedef enum calibration_data_index_e {
    DEVICE_NAME_IDX = 0,
    TIMEBASE_US_IDX,
    ADCMAXCOUNT_IDX,
    ALSTHRESHOLDHIGH_IDX,
    ALSTHRESHOLDLOW_IDX,
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
    ALSCALIBRATIONLUXTARGETHIGH_IDX,
    ALSCALIBRATIONLUXTARGETLOW_IDX,
    ALSCALIBRATIONLUXTARGET_IDX,
    ALSCALIBRATIONLUXTARGETERROR_IDX,
    ALSCALIBRATIONFACTOR_IDX,
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
    PROXADAPTIVETHRESHOLDFACTOR_IDX,
    PROXADAPTIVETHRESHOLD_IDX,
    PROXHARDTHRESHOLD_IDX,
    PROXOFFSET_IDX,
    PROXOFFSETSIGN_IDX,
#endif
    CALIBRATION_DATA_INDEX_MAX
} calibration_data_index_e;

typedef struct _calibrationData {
    uint32_t    timeBase_us;    /* in uSec */
    uint32_t    adcMaxCount;
    uint8_t     alsThresholdHigh; /* in % */
    uint8_t     alsThresholdLow;  /* in % */
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
    uint16_t    alsCalibrationLuxTargetHigh;
    uint16_t    alsCalibrationLuxTargetLow;
    uint16_t    alsCalibrationLuxTarget;
    uint16_t    alsCalibrationLuxTargetError;
    uint16_t    alsCalibrationFactor;        /* multiplicative factor default 1000 */
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
    /* TODO: keep adaptive threshold for now */
    uint16_t    proxAdaptiveThresholdFactor;
    uint8_t     proxAdaptiveThreshold; /* in bits */
    uint16_t    proxHardThreshold;
    uint16_t    proxMinThreshold;
    uint16_t    proxHardThresholdResetValue;
    uint16_t    proxMinThresholdResetValue;
    uint8_t     proxOffset;
    uint8_t     proxOffsetSign;
#endif
    char        deviceName[8];
    int32_t     alsCoefC;
    int32_t     alsCoefR;
    int32_t     alsCoefG;
    int32_t     alsCoefB;
    int16_t     alsDfg;
    uint16_t    alsCctOffset;
    uint16_t    alsCctCoef;
    int32_t     D_factor;
    int32_t     L0_factor;
    int32_t     L1_factor;
    int32_t     L2_factor;
    int32_t     L3_factor;
} ams_calibrationData_t;

typedef struct _default_als_config_data {
    char         deviceName[8];
    uint32_t     aTime;
    uint32_t     wTime;
    bool         wLong;
    uint16_t     aGain;
    uint8_t      aPers;
    uint8_t      aUpperThrs;
    uint8_t      aLowerThrs;
    uint8_t      aThrsWidth;
} ams_default_als_config_data_t;

typedef struct _default_prox_config_data {
    char         deviceName[8];
    uint32_t     pTime;
    uint32_t     pwTime_us;
    uint16_t     pPulseLen;
    uint8_t      pPulse;
    uint16_t     pGain;
    uint8_t      pLdrive;
    uint8_t      pPers;
    uint16_t     pUpperThrs;
    uint16_t     pLowerThrs;
    bool         pOffsetSign;
    int16_t      proxOffset;
    uint8_t      pBinSearch;
} ams_default_prox_config_data_t;

typedef struct _tmd2725Context {
    tmd2725_deviceIdentifier_e deviceId;
    uint64_t timeStamp;
    AMS_PORT_portHndl * portHndl;
    ams_mode_t mode;
#ifdef AMS_PHY_SUPPORT_SHADOW
    uint8_t shadow[DEVREG_REG_MAX];
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
    ams_ccb_proximity_ctx_t ccbProxCtx;
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
    ams_ccb_als_ctx_t ccbAlsCtx;
#endif
    ams_calibrationData_t * systemCalibrationData;
    ams_default_als_config_data_t *alsDefaultConfigData;
    ams_default_prox_config_data_t *proxDefaultConfigData;
    bool alwaysReadAls;       /* read ADATA every tmd2725_deviceEventHandler call
                                 regardless of xINT bits */
    bool alwaysReadProx;      /* ditto PDATA */
    uint32_t updateAvailable;
    uint8_t shadowEnableReg;
    uint8_t shadowEnableRegPrevious;
    uint8_t shadowIntenabReg;
    uint8_t shadowIntenabRegPrevious;
    uint8_t shadowStatusReg;
    uint8_t shadowStatusCalibReg;
    uint8_t shadowPiltReg;
    uint8_t shadowPihtReg;
    uint32_t alsGain; /*  gain x 1000; store the value so
                       * _GET_ALS_GAIN need not do a multi-register lookup
                       */
    bool allowIens;
    int32_t hwid;
    bool reset_als;
    bool reset_prox;
}ams_deviceCtx_t;

typedef enum _sensorType {
    AMS_NO_SENSOR_AVAILABLE,
    AMS_AMBIENT_SENSOR,
    AMS_PROXIMITY_SENSOR,
    AMS_ALS_GAIN_CHANGED,
    AMS_LAST_SENSOR
}ams_sensorType_t;

typedef struct _sensorInfo {
    uint32_t    standbyCurrent_uA;
    uint32_t    activeCurrent_uA;
    uint32_t    rangeMin;
    uint32_t    rangeMax;
    uint8_t     maxPolRate;
    uint8_t     adcBits;
    float_t     alsResolution;
    float_t     proxResolution;
    char *      driverName;
} ams_SensorInfo_t;

typedef struct _deviceInfo {
    uint32_t    memorySize;
    ams_calibrationData_t defaultCalibrationData;
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
    ams_SensorInfo_t proxSensor;
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
    ams_SensorInfo_t alsSensor;
#endif
    ams_sensorType_t tableSubSensors[10];
    uint8_t     numberOfSubSensors;
    char *      deviceModel;
    char *      deviceName;
}ams_deviceInfo_t;


extern tmd2725_deviceIdentifier_e tmd2725_validateDevice(AMS_PORT_portHndl * portHndl);
extern bool tmd2725_getDeviceInfo(ams_deviceInfo_t * info, int32_t hwid);
extern bool tmd2725_deviceInit(ams_deviceCtx_t * ctx, AMS_PORT_portHndl * portHndl, ams_calibrationData_t * calibrationData, \
                               ams_default_als_config_data_t * als_default_config_values, ams_default_prox_config_data_t * proxDefaultConfigValues, int32_t hwid);
extern bool tmd2725_deviceEventHandler(sns_sensor_instance *const this, ams_deviceCtx_t * ctx);
extern uint32_t tmd2725_getResult(ams_deviceCtx_t * ctx);
extern bool tmd2725_deviceSetConfig(sns_sensor_instance *const this, ams_deviceCtx_t * ctx, ams_configureFeature_t feature, deviceConfigOptions_t option, uint32_t data);
extern bool ams_deviceGetConfig(ams_deviceCtx_t * ctx);
extern bool tmd2725_deviceCalibrateLux(sns_sensor_instance *instance, ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData, ams_alsCalType_t cal_type);
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
extern bool tmd2725_deviceGetAls(ams_deviceCtx_t * ctx, ams_apiAls_t * exportData);
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
extern bool tmd2725_deviceGetPrx(ams_deviceCtx_t * ctx, ams_apiPrx_t * exportData);
extern bool tmd2725_deviceCalibrateProx(sns_sensor_instance *instance, ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData);
extern bool tmd2725_deviceCalibrateProxHardware(sns_sensor_instance *instance, ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData);
#endif
extern void tmd2725_restore_enable_settings(ams_deviceCtx_t *devCtx);

#ifdef __cplusplus
}
#endif

#else

#endif /* __AMS_DEVICE_CONTROL_BLOCK_H__ */

#include "sns_tmd2725_sensor_instance.h"

#ifndef AMS_ALS_CALLIBRATION_H
#define AMS_ALS_CALLIBRATION_H

#define CPU_FRIENDLY_FACTOR_1024        1
#define AMS_ALS_TIMEBASE                (2780) /* in uSec, see data sheet */
#define AMS_ALS_FACTOR                  (1000)
#define AMS_ALS_L0_FACTOR               (1 * AMS_ALS_FACTOR)
#define AMS_ALS_L1_FACTOR               (0.26 * AMS_ALS_FACTOR)
#define AMS_ALS_L2_FACTOR               (0.8 * AMS_ALS_FACTOR)
#define AMS_ALS_L3_FACTOR               (0.27 * AMS_ALS_FACTOR)
#define AMS_ALS_D_FACTOR                (41)
#define AMS_ALS_THRESHOLD_HIGH          (5) /* in % */
#define AMS_ALS_THRESHOLD_LOW           (5) /* in % */
#define AMS_ALS_ADC_MAX_COUNT           (1023) /* in uSec, see data sheet */
#define AMS_ALS_THRESHOLD_HALF_WIDTH    (10) /* in % */
#define AMS_ALS_AGAIN                   (16000)
#define AMS_ALS_ATIME                   (90000)
#define AMS_ALS_PERS                    (0x01)
#define AMS_ALS_CLEAR_CHANNEL_UPPER_THRESHOLD (80) /* in % */
#define AMS_ALS_CLEAR_CHANNEL_LOWER_THRESHOLD (10) /* in % */

#endif /* AMS_ALS_CALLIBRATION_H */

/* For turning a compiler option value into a string */
#define STRINGIFY(s) STRINGIFY2(s)
#define STRINGIFY2(s) #s
#define AMS_REGISTRY_ENTRY_NAME(s) STRINGIFY(CONFIG_AMS_PART_NAME)s

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
/** Forward Declaration of ALS Sensor API */
extern sns_sensor_api tmd2725_als_sensor_api;
extern sns_sensor_api tmd2725_als_streaming_sensor_api;
#endif

/** Forward Declaration of Prox Sensor API */
extern sns_sensor_api tmd2725_prox_sensor_api;
extern sns_sensor_api tmd2725_prox_streaming_sensor_api;
/**
 * AMS customary device ODR (Hz) definitions
 * Note 1 Hz entry is polled mode only - many devices can't interrupt that slow
 */
//#if ! CONFIG_AMS_USE_DRI - the test sensor is hardcoded to request 1 Hz so
//                           let's support it even in DRI mode
#define AMS_DEVICE_ODR_1                 1.0
#define AMS_DEVICE_ODR_2                 2.0
#define AMS_DEVICE_ODR_5                 5.0
#define AMS_DEVICE_ODR_10                10.0
#define AMS_DEVICE_ODR_15                15.0
#define AMS_DEVICE_ODR_20                20.0

/* Length of float[] array in std. report event */
#ifdef CONFIG_ALS_INCLUDE_IR_RESULTS
#define ALS_PROTO_STD_ARRAY_LEN     3
#else
#define ALS_PROTO_STD_ARRAY_LEN     2
#endif
#ifdef CONFIG_PROX_INCLUDE_PROXIMITY_RESULTS
#define PROX_PROTO_STD_ARRAY_LEN    3
#else
#define PROX_PROTO_STD_ARRAY_LEN    2
#endif

#define ALS_FAC_CAL_NUM             2
#define ALS_CUSTOM_CAL_NUM          5
#define ALS_DEFAULT_CONFIG_VAL      8
#define ALS_CAL_TARGET_NUM          2

#define PROX_FAC_CAL_NUM            2
#define PROX_OFFSET_HW_CAL_NUM      2
#define PROX_DEFAULT_CONFIG_VAL     12
/** Supported opertating modes */
#define AMS_OPMODE_LPM       "LPM"
#define AMS_OPMODE_NORMAL    "NORMAL"

/** Power rail timeout States for the AMS Sensors.*/
#define AMS_POWER_RAIL_OFF_TIMEOUT_NS 1000000000ULL /* 1 second */
typedef enum
{
  AMS_POWER_RAIL_PENDING_NONE,
  AMS_POWER_RAIL_PENDING_INIT,
  AMS_POWER_RAIL_PENDING_SET_CLIENT_REQ,
  AMS_POWER_RAIL_PENDING_OFF
} ams_power_rail_pending_state;


typedef struct pb_arg_calibration_common {
  const char* name;
  sns_sensor *sensor;
} pb_arg_calibration_common;

typedef struct pb_custom_float_parse_arg {
  float *data_array;
  uint32_t *version_array;
  uint8_t *index;
  uint8_t size;
} pb_custom_float_parse_arg;

typedef struct pb_custom_calibration_values {
  float_t L0factor;
  float_t L1factor;
  float_t L2factor;
  float_t L3factor;
  float_t Dfactor;
  float_t *data_array;
  uint32_t *version_array;
  uint8_t *index;
  uint8_t size;
  bool has_L0factor;
  bool has_L1factor;
  bool has_L2factor;
  bool has_L3factor;
  bool has_Dfactor;
} pb_custom_calibration_values;

/**
 * ALS default config values
 */
typedef struct pb_als_default_config_values {
  float_t aTime;
  float_t wTime;
  float_t wLong;
  float_t aGain;
  float_t aPers;
  float_t aUpperThrs;
  float_t aLowerThrs;
  float_t aThrsWidth;
  float_t *data_array;
  uint32_t *version_array;
  uint8_t *index;
  uint8_t size;
  bool hasAtime;
  bool hasWtime;
  bool hasWlong;
  bool hasAgain;
  bool hasApers;
  bool hasUpperThrs;
  bool hasLowerThrs;
  bool hasThrsWidth;
} pb_als_default_config_values;

typedef struct pb_als_calibration_target_values {
  float_t luxTarget;
  float_t luxTargetTolerance;
  float_t *data_array;
  uint32_t *version_array;
  uint8_t *index;
  uint8_t size;
  bool has_luxTarget;
  bool has_luxTargetTolerance;
} pb_als_calibration_target_values;

/**
 * PROX default config values
 */
typedef struct pb_prox_default_config_values {
  float_t pTime;
  float_t pwTime_us;
  float_t pPulseLen;
  float_t pPulse;
  float_t pGain;
  float_t pLdrive;
  float_t pPers;
  float_t pUpperThrs;
  float_t pLowerThrs;
  float_t pOffsetSign;
  float_t pProxOffset;
  float_t pBinSearch;
  float_t *data_array;
  uint32_t *version_array;
  uint8_t *index;
  uint8_t size;
  bool hasPtime;
  bool hasPwtime;
  bool hasPpulseLen;
  bool hasPpulse;
  bool hasPpGain;
  bool hasPldrive;
  bool hasPpers;
  bool hasPupperThrs;
  bool hasPlowerThrs;
  bool hasPoffsetSign;
  bool hasProxOffset;
  bool hasPbinSearch;
} pb_prox_default_config_values;

/** Interrupt Sensor State. */
typedef struct ams_state
{
    void                    *priv;
    sns_data_stream         *reg_data_stream;
    sns_data_stream         *fw_stream;
    sns_data_stream         *timer_stream;
    sns_sensor_uid          reg_suid;
    sns_sensor_uid          irq_suid;
    sns_sensor_uid          timer_suid;
    ams_sensor_type         sensor_type;
    sns_sensor_uid          my_suid;
    ams_com_port_info       com_port_info;
    sns_interrupt_req       irq_config;

    sns_pwr_rail_service    *pwr_rail_service;
    sns_rail_config         rail_config;
    sns_power_rail_state    registry_rail_on_state;

    bool                    tmd2725_hw_is_present;
    bool                    sensor_client_present;
    bool                    use_dri;

    ams_power_rail_pending_state    power_rail_pend_state;

    // debug
    tmd2725_deviceIdentifier_e  who_am_i;
    sns_diag_service        *diag_service;
    sns_sync_com_port_service *scp_service;
    size_t                  encoded_event_len;

    AMS_PORT_portHndl       port_handle;
    AMS_PORT_portHndl       *port_handle_ptr;

    // sensor configuration
    bool is_dri;
    int64_t hardware_id;
    bool supports_sync_stream;
    uint8_t resolution_idx;
    float odr_for_selftest;

    // registry sensor config
    bool registry_cfg_received;
    sns_registry_phy_sensor_cfg registry_cfg;

    // registry sensor platform config
    bool registry_pf_cfg_received;
    sns_registry_phy_sensor_pf_cfg registry_pf_cfg;

    // placement
    bool                    registry_placement_received;
    float                   placement[12];

    // registry calibration data
    ams_calibrationData_t   calibration_data_tmd2725;

    // als default config data
    ams_default_als_config_data_t als_default_config_data;

    // prox default config data
    ams_default_prox_config_data_t prox_default_config_data;

    uint32_t                cal_version[CALIBRATION_DATA_INDEX_MAX];
} ams_state;

/** Functions shared by all AMS Sensors */
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
void tmd2725_reval_instance_config(sns_sensor *this,
                              sns_sensor_instance *instance,
                              ams_sensor_type sensor_type);

/**
 * Sends a request to the SUID Sensor to get SUID of a dependent
 * Sensor.
 *
 * @param[i] this          Sensor reference
 * @param[i] data_type     data_type of dependent Sensor
 * @param[i] data_type_len Length of the data_type string
 */
void tmd2725_send_suid_req(sns_sensor *this, char *const data_type,
                      uint32_t data_type_len);

/**
 * Processes events from SUID Sensor.
 *
 * @param[i] this   Sensor reference
 *
 * @return none
 */
void tmd2725_process_suid_events(sns_sensor *const this);

/**
 * notify_event() Sensor API common between all AMS Sensors.
 *
 * @param this    Sensor reference
 *
 * @return sns_rc
 */
sns_rc tmd2725_sensor_notify_event(sns_sensor *const this);
sns_rc tmd2725_streaming_sensor_notify_event(sns_sensor *const this);

/**
 * set_client_request() Sensor API common between all AMS
 * Sensors.
 *
 * @param this            Sensor reference
 * @param exist_request   existing request
 * @param new_request     new request
 * @param remove          true to remove request
 *
 * @return sns_sensor_instance*
 */
sns_sensor_instance* tmd2725_set_client_request(sns_sensor *const this,
                                           struct sns_request const *exist_request,
                                           struct sns_request const *new_request,
                                           bool remove);

void tmd2725_start_power_rail_timer(sns_sensor *const this,
                                sns_time timeout_ticks,
                                ams_power_rail_pending_state pwr_rail_pend_state);
/**
 * Sets instance config to run a self test.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Instance reference
 *
 * @return none
 */
void tmd2725_set_self_test_inst_config(sns_sensor *this,
                              sns_sensor_instance *instance);

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
sns_rc tmd2725_als_init(sns_sensor *const this);
sns_rc tmd2725_als_streaming_init(sns_sensor *const this);
sns_rc tmd2725_als_deinit(sns_sensor *const this);
void tmd2725_publish_als_resolution_attribute(sns_sensor *const this, float res);
uint32_t getAlsAdcMaxCount(uint32_t atime_usec);
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
sns_rc tmd2725_prox_init(sns_sensor *const this);
sns_rc tmd2725_prox_streaming_init(sns_sensor *const this);
sns_rc tmd2725_prox_deinit(sns_sensor *const this);
void tmd2725_publish_prox_active_current_attribute(sns_sensor *const this, int32_t ac);
#endif

void tmd2725_sensor_send_registry_request(sns_sensor *const this,
                                             char *reg_group_name);

void tmd2725_publish_available(sns_sensor *const this);

extern sns_sensor_uid const* tmd2725_get_als_oc_sensor_uid(sns_sensor const *const this);
extern sns_sensor_uid const* tmd2725_get_als_strm_sensor_uid(sns_sensor const *const this);
extern sns_sensor_uid const* tmd2725_get_prox_oc_sensor_uid(sns_sensor const *const this);
extern sns_sensor_uid const* tmd2725_get_prox_strm_sensor_uid(sns_sensor const *const this);
extern sns_sensor_uid const* tmd2725_get_suid_with_sensortype_and_hwid(ams_sensor_type type, int64_t hwid);
extern uint8_t ams_setEnable(ams_deviceCtx_t * ctx);

void * amsLibrary_open_tmd2725(int32_t hwid, AMS_PORT_portHndl* port_handle_ptr, ams_calibrationData_t *calibration_data, ams_deviceCtx_t *amsLibrary, \
                                ams_default_als_config_data_t *alsDefaultConfigData, ams_default_prox_config_data_t *proxDefaultConfigData);
int32_t tmd2725_calculate_prox_active_current_from_setting(sns_sensor *const this);

#endif //__SNS_TMD2725_SENSOR___H__
