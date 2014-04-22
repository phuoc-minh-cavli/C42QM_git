/*
 * Copyright (c) 2019-2020, ams AG
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

#include "sns_tmd2725_sensor.h"


//static ams_deviceCtx_t * amsLibraries[TMD2725_NUM_DEVICES] = { NULL, NULL};
static ams_deviceCtx_t tmd2725_device_contexts[TMD2725_NUM_DEVICES];


/*
 * Forward declaration of the function
 */

static void ccb_alsGetResult(void * dcbCtx, ams_ccb_als_result_t * result);
static bool ccb_alsHandle(sns_sensor_instance *const this, void * dcbCtx, ams_ccb_als_dataSet_t * data);
static void ccb_alsInfo(ams_ccb_als_info_t* infoData);
static void ccb_alsInit(void * dcbCtx, ams_ccb_als_init_t * initData);
static void ccb_alsSetConfig(void * dcbCtx, ams_ccb_als_config_t * configData);
static void ccb_proximityGetResult(void *dcbCtx, ams_ccb_proximity_result_t *result);
static bool ccb_proximityHandle(sns_sensor_instance *const this, void *dcbCtx, ams_ccb_proximity_dataSet_t *data);
static void ccb_proximityInfo(ams_ccb_proximity_info_t *infoData);
static void ccb_proximityInit(void *dcbCtx, ams_ccb_proximity_init_t *initData);
static void ccb_proximitySetConfig(void *dcbCtx, ams_ccb_proximity_config_t *configData);
static deviceRegisterTable_t deviceRegisterDefinition[DEVREG_REG_MAX];


/**
 * Function to exit the island mode on demand
 * @param this
 */
#ifdef ENABLE_DEBUG
static void inst_exit_island(sns_sensor_instance *this);
#endif

#define AMS_ROUND_SHFT_VAL              4
#define AMS_ROUND_ADD_VAL               (1 << (AMS_ROUND_SHFT_VAL - 1))
#define AMS_ALS_GAIN_FACTOR             1000
#define CPU_FRIENDLY_FACTOR_1024        1

amsAlsAdaptive_t tmd2725_als_adaptive(amsAlsContext_t * ctx, amsAlsDataSet_t * inputData){
    UNUSED_VAR(ctx);
    UNUSED_VAR(inputData);
    return ADAPTIVE_ALS_NO_REQUEST;
}

static void als_ave_LUX (amsAlsContext_t * ctx) {

    /* if average queue isn't full (at startup), fill it */
    if(ctx->ave_lux[AMS_LUX_AVERAGE_COUNT - 1] == 0) {
        ctx->results.mLux_ave = 0;
        ctx->ave_lux_index = 0;
        do {
            ctx->ave_lux[ctx->ave_lux_index++] = ctx->results.mLux;
            ctx->results.mLux_ave += ctx->results.mLux;
        } while (ctx->ave_lux_index < AMS_LUX_AVERAGE_COUNT);
        ctx->ave_lux_index = 1;
    } else {
        /* replace the oldest LUX value with the new LUX value */
        ctx->results.mLux_ave -= ctx->ave_lux[ctx->ave_lux_index];
        ctx->ave_lux[ctx->ave_lux_index] = ctx->results.mLux;
        ctx->results.mLux_ave += ctx->ave_lux[ctx->ave_lux_index];
        ctx->ave_lux_index++;
    }

    if (ctx->ave_lux_index == AMS_LUX_AVERAGE_COUNT) {
        ctx->ave_lux_index = 0;
    }
}

static void als_compute_data_clear (sns_sensor_instance *const this, amsAlsContext_t * ctx, amsAlsDataSet_t * inputData) {
    uint16_t ch0 = 0, ch1 = 0, cf = 0;
    int32_t L0_factor = 0, L1_factor = 0, L2_factor = 0, L3_factor = 0;
    uint32_t cpl = 0;
    int64_t lux1 = 0, lux2 = 0;

    if (ctx == NULL || inputData == NULL) {
        SNS_INST_PRINTF(ERROR, this, "als_compute_data_clear(): ERROR, NULL arg(s) detected\n");
        return;
    }

    ctx->results.rawClear = inputData->datasetArray->ch0ADC;
    ctx->results.rawIR = inputData->datasetArray->ch1ADC;

    ch0 = inputData->datasetArray->ch0ADC;
    ch1 = inputData->datasetArray->ch1ADC;
    L0_factor = ctx->calibration.L0_factor;
    L1_factor = ctx->calibration.L1_factor;
    L2_factor = ctx->calibration.L2_factor;
    L3_factor = ctx->calibration.L3_factor;
    cf = ctx->calibration.calibrationFactor;

    AMS_DEV_INST_PRINTF(MED, this, "ch0: %d, ch1: %d, l0_factor: %d, l1_factor: %d, l2_factor: %d, l3_factor: %d, d_factor: %d, Calibration Factor: %d, cpl = %d",
                        (int32_t)ch0,
                        (int32_t)ch1,
                        (int32_t)ctx->calibration.L0_factor,
                        (int32_t)ctx->calibration.L1_factor,
                        (int32_t)ctx->calibration.L2_factor,
                        (int32_t)ctx->calibration.L3_factor,
                        (int32_t)ctx->calibration.D_factor,
                        (int32_t)ctx->calibration.calibrationFactor,
                        ctx->cpl);

    cpl = ctx->cpl;

    if (ch0 < (uint16_t)USHRT_MAX){
        ctx->results.mLux = 0;

        if (cpl > 0) {
            /*
             * ams design note 28 second order lux equation format
             * Lux1 = (Ch0 - (coefB * Ch1)) / CPL
             * Lux2 = ((coefC * Ch0) - (coefD * Ch1)) / CPL
             * Lux  = MAX(Lux1, Lux2, 0)
             * CPL = (ATIME_ms * AGAINx) / (DF * GA) = (ATIME_ms * AGAINx) / (DGF)
             */
            lux1 = (L0_factor * ch0) - (L1_factor * ch1);
            lux2 = (L2_factor * ch0) - (L3_factor * ch1);

            /* mult/div here to handle negative lux */
            lux1 = lux1 * cf / cpl;
            lux2 = lux2 * cf / cpl;

            AMS_DEV_INST_PRINTF(MED, this, "lux1 %d, lux2 = %d", (int32_t)lux1, (int32_t)lux2);

            if (lux1 < 0) {
                lux1 = 0;
            }
            if (lux2 < 0) {
                lux2 = 0;
            }
            ctx->results.mLux = (lux1 > lux2) ? lux1 : lux2;
        } else {
            AMS_DEV_INST_PRINTF(HIGH, this, "als_compute_data_clear(): cpl=0\n");
            return;
        }

        if (ctx->results.mLux > (uint32_t)0xFFFFFFFE) {
            AMS_DEV_INST_PRINTF(HIGH, this, "als_compute_data_clear(): overflow detected\n");
            ctx->results.mLux = ctx->previousLux;
        }
    }
}

static void als_update_statics(amsAlsContext_t * ctx) {
    uint64_t tempCpl;
    uint64_t tempTime_us = ctx->time_us;
    uint64_t tempGain = ctx->gain;

    /* test for the potential of overflowing */
    uint32_t maxOverFlow;
    maxOverFlow = (uint64_t)ULLONG_MAX / ctx->time_us;

    if (maxOverFlow < ctx->gain) {
        tempCpl = ((tempTime_us / 2) * (tempGain / 2)) / (AMS_ALS_GAIN_FACTOR/4);
    } else {
        tempCpl = (tempTime_us * tempGain) / AMS_ALS_GAIN_FACTOR;
    }
    if (tempCpl > (uint32_t)ULONG_MAX){
        /* if we get here, we have an problem */
        return;
    }

    ctx->cpl = tempCpl / ctx->calibration.D_factor;
    ctx->saturation = ctx->calibration.adcMaxCount;
    ctx->previousGain = ctx->gain;

    {
        float a, b;
        als_getReportingParams_tmd2725(ctx, &a, &b);
        ctx->maxLux = a;
        ctx->luxResolution = b;
    }
}

static uint8_t AMS_PORT_setByte(AMS_PORT_portHndl * portHndl, uint8_t reg_addr, uint8_t * data, uint8_t length) {
    uint32_t write_count;
    sns_rc status;
    sns_port_vector port_vec;
    port_vec.buffer = data;
    port_vec.bytes = length;
    port_vec.is_write = true;
    port_vec.reg_addr = reg_addr;

    if (portHndl == NULL)
        return 0;

    status = portHndl->scp_service->api->sns_scp_register_rw(portHndl->scp_handle,
                                                            &port_vec,
                                                            1,
                                                            false,
                                                            &write_count);

    if (status == SNS_RC_SUCCESS) {
        return (uint8_t)write_count;
    }
    return 0;
}

static uint8_t ams_rwVec(AMS_PORT_portHndl * portHndl, sns_port_vector *port_vec, int32_t num_vec) {
    uint32_t rw_count;
    sns_rc status;

    if (portHndl == NULL)
        return 0;

    status = portHndl->scp_service->api->sns_scp_register_rw(portHndl->scp_handle,
                                                            port_vec,
                                                            num_vec,
                                                            false,
                                                            &rw_count);

    if (status == SNS_RC_SUCCESS) {
        return (uint8_t)rw_count;
    }
    return 0;
}

static uint8_t AMS_PORT_getByte(AMS_PORT_portHndl * portHndl, uint8_t reg_addr, uint8_t * data, uint8_t length){
    uint32_t read_count;
    sns_rc status;
    sns_port_vector port_vec;
    port_vec.buffer = data;
    port_vec.bytes = length;
    port_vec.is_write = false;
    port_vec.reg_addr = reg_addr;

    if (portHndl == NULL)
        return 0;

    status = portHndl->scp_service->api->sns_scp_register_rw(portHndl->scp_handle,
                                                            &port_vec,
                                                            1,
                                                            false,
                                                            &read_count);

    if (status == SNS_RC_SUCCESS) {
        return (uint8_t)read_count;
    }
    return 0;
}

static uint8_t ams_getByte(AMS_PORT_portHndl * portHndl, ams_tmd2725_deviceRegister_t reg, uint8_t * readData)
{
    uint8_t read_count = 0;
    uint8_t length = 1;

    /* Sanity check input param */
    if(reg > DEVREG_REG_MAX)
    {
        return 0;
    }

    read_count = AMS_PORT_getByte(portHndl,
                                deviceRegisterDefinition[reg].address,
                                readData,
                                length);

    return read_count;
}

static uint8_t ams_setByte(AMS_PORT_portHndl * portHndl, ams_tmd2725_deviceRegister_t reg, uint8_t data)
{
    uint8_t write_count = 0;
    uint8_t length = 1;

    /* Sanity check input param */
    if(reg > DEVREG_REG_MAX)
    {
        return 0;
    }

    write_count = AMS_PORT_setByte(portHndl,
                                deviceRegisterDefinition[reg].address,
                                &data,
                                length);

    return write_count;
}

static uint8_t ams_getBuf(AMS_PORT_portHndl * portHndl, ams_tmd2725_deviceRegister_t reg, uint8_t * readData, uint8_t length)
{
    uint8_t read_count = 0;

    /* Sanity check input param */
    if(reg > DEVREG_REG_MAX)
    {
        return 0;
    }

    read_count = AMS_PORT_getByte(portHndl,
                                deviceRegisterDefinition[reg].address,
                                readData,
                                length);

    return read_count;
}

static uint8_t ams_setBuf(AMS_PORT_portHndl * portHndl, ams_tmd2725_deviceRegister_t reg, uint8_t * data, uint8_t length)
{
    uint8_t write_count = 0;

    /* Sanity check input param */
    if((reg > DEVREG_REG_MAX) || (reg >= DEVREG_POFFSET_L && reg <= DEVREG_CALIBCFG))
    {
        return 0;
    }

    write_count = AMS_PORT_setByte(portHndl,
                                deviceRegisterDefinition[reg].address,
                                data,
                                length);

    return write_count;
}

static uint8_t ams_setWord(AMS_PORT_portHndl * portHndl, ams_tmd2725_deviceRegister_t reg, uint16_t data)
{
    uint8_t write_count = 0;
    uint8_t length = sizeof(uint16_t);
    uint8_t buffer[sizeof(uint16_t)];

    /* Sanity check input param */
    if(reg > DEVREG_REG_MAX)
    {
        return 0;
    }

    buffer[0] = ((data >> AMS_ENDIAN_1) & 0xff);
    buffer[1] = ((data >> AMS_ENDIAN_2) & 0xff);

    write_count = AMS_PORT_setByte(portHndl,
                                deviceRegisterDefinition[reg].address,
                                &buffer[0],
                                length);
    return write_count;
}

static uint8_t ams_setField(AMS_PORT_portHndl * portHndl, ams_tmd2725_deviceRegister_t reg, uint8_t data, ams_regMask_t mask)
{
    uint8_t write_count = 1;
    uint8_t length = 1;
    uint8_t original_data;
    uint8_t new_data;

    AMS_PORT_getByte(portHndl,
                        deviceRegisterDefinition[reg].address,
                        &original_data,
                        length);

    new_data = original_data & ~mask;
    new_data |= (data & mask);

    if (new_data != original_data){
        write_count = AMS_PORT_setByte(portHndl,
                        deviceRegisterDefinition[reg].address,
                        &new_data,
                        length);
    }

    return write_count;
}

#define AMS_INVALID_GAIN  0
#define AMS_ALS_MIN_GAIN  500
#define MAX_ALS_GAIN_LEVEL 5
#define MIN_ALS_GAIN_LEVEL 0

static deviceRegisterTable_t deviceRegisterDefinition[DEVREG_REG_MAX] = {
    { 0x80, 0x00 },          /* DEVREG_ENABLE */
    { 0x81, 0x00 },          /* DEVREG_ATIME */
    { 0x82, 0x1F },          /* DEVREG_PTIME */ /*DEVREG_PRATE  */
    { 0x83, 0x00 },          /* DEVREG_WTIME  */
    { 0x84, 0x00 },          /* DEVREG_AILTL  */
    { 0x85, 0x00 },          /* DEVREG_AILTH  */
    { 0x86, 0x00 },          /* DEVREG_AIHTL  */
    { 0x87, 0x00 },          /* DEVREG_AIHTH  */
    { 0x88, 0x00 },          /* DEVREG_PILT  */
    { 0x8A, 0x00 },          /* DEVREG_PIHT  */
    { 0x8C, 0x00 },          /* DEVREG_PERS  */
    { 0x8D, 0x80 },          /* DEVREG_CFG0  */
    { 0x8E, 0x4F },          /* DEVREG_PCFG0  */
    { 0x8F, 0x80 },          /* DEVREG_PCFG1  */
    { 0x90, 0x00 },          /* DEVREG_CFG1  */
    { 0x91, AMS_REV_ID },    /* DEVREG_REVID  */
    { 0x92, AMS_DEVICE_ID }, /* DEVREG_ID  */
    { 0x93, 0x00 },          /* DEVREG_STATUS  */
    { 0x94, 0x00 },          /* DEVREG_CDATAL  - AKA CH0 - AKA Photopic*/
    { 0x95, 0x00 },          /* DEVREG_CDATAH  */
    { 0x96, 0x00 },          /* DEVREG_ALSIRL  - AKA CH1 - AKA IR*/
    { 0x97, 0x00 },          /* DEVREG_ALSIRL  */
    { 0x9C, 0x00 },          /* DEVREG_PDATA  */
    { 0x9F, 0x04 },          /* DEVREG_CFG2  */
    { 0xAB, 0x4C },          /* DEVREG_CFG3  */  /* disable clear-on-read, this cause trouble while streaming and on-change work the same time */
    { 0xC0, 0x00 },          /* DEVREG_POFFSET_L  */
    { 0xC1, 0x00 },          /* DEVREG_POFFSET_H  */
    { 0xD7, 0x00 },          /* DEVREG_CALIB  */
    { 0xD9, 0x50 },          /* DEVREG_CALIBCFG  */
    { 0xDC, 0x00 },          /* DEVREG_CALIBSTAT  */
    { 0xDD, 0x00 },          /* DEVREG_INTENAB  */
    { 0xE6, 0x00 },          /* DEVREG_POFFSET_MAG  */
    { 0xE7, 0x00 },          /* DEVREG_POFFSET_SIGN  */
};

/*
 * In SEE interrupts should only be enabled after receiving
 * SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT.
 *
 * Therefore, ctx->allowIens is a bool where
 *
 * true ==  SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT has been received
 *          and it is okay to enable interrupts from the TMD2725.
 *
 * false == SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT has *NOT* been received
 *          and it is *NOT* okay to enable interrupts.
 *
 * Set ENABLE reg to ctx->shadowEnableReg unless INT enables have to be
 * be deferred, in which case omit the IEN bits and set the rest
*/
uint8_t ams_setEnable(ams_deviceCtx_t * ctx)
{
    uint8_t allowedIntenabBits = 0;
    sns_port_vector port_vec[2];
    uint8_t byte_count;

    if (ctx->allowIens)
    {
        allowedIntenabBits = (ctx->shadowIntenabReg);
    }
    else
    {
        allowedIntenabBits = ctx->shadowIntenabReg & ~(AMS_ALL_IENS);
    }
    port_vec[0].is_write = true;
    port_vec[0].reg_addr = deviceRegisterDefinition[DEVREG_ENABLE].address;
    port_vec[0].bytes    = 1;
    port_vec[0].buffer   = &ctx->shadowEnableReg;

    port_vec[1].is_write = true;
    port_vec[1].reg_addr = deviceRegisterDefinition[DEVREG_INTENAB].address;
    port_vec[1].bytes    = 1;
    port_vec[1].buffer   = &allowedIntenabBits;

    byte_count = ams_rwVec(ctx->portHndl, port_vec, 2);
    return (byte_count);
}

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
/* this array is no longer public, and the index is an
 * abstract gainLevel number, not a register value, since the gain is
 * split between 2 regs in the 27xx family. Gains must be in monotonically
 * increasing order.
 */
static uint32_t alsGain_conversion[] = {
    1000 / 2,   // AGAIN=0, AGAINMAX = 0, AGAINL=0
    1 * 1000,   // AGAIN=0, AGAINMAX = 0, AGAINL=1
    4 * 1000,   // AGAIN=1, AGAINMAX = 0, AGAINL=1
    16 * 1000,  // AGAIN=2, AGAINMAX = 0, AGAINL=1
    64 * 1000,  // AGAIN=3, AGAINMAX = 0, AGAINL=1
    128 * 1000  // AGAIN=3, AGAINMAX = 1, AGAIN =1
};

/* This is a simplified version. It converts a precise numerical gain to one of
 * the discrete levels supported by the chip, in the form of an index into
 * alsGain_conversion[]. Input gain must be scaled by 1000x ("milligain" units).
*/
static uint32_t alsGainToLevel(uint32_t mGain){
    int i;

    for (i = sizeof(alsGain_conversion)/sizeof(uint32_t)-1; i != 0; i--) {
        if (mGain >= alsGain_conversion[i]) break;
    }
    return (i << 0);
}

/* This is the more detailed version actually needed to set the HW */
static void alsGainLevelToRegs(uint32_t level, uint8_t *cfg1, uint8_t *cfg2){
    uint8_t _cfg1, _cfg2;
    uint32_t maxLevel = sizeof(alsGain_conversion)/sizeof(uint32_t) - 1;

    /* if level is invalid, default to gain=1x */
    if (level > maxLevel) {
        _cfg1 = 0;
        _cfg2 = 0x04;
    }
    else {
        if (level == maxLevel) {
            _cfg1 = 0x03;
            _cfg2 = 0x14;
        } else {
            if(level != 0x00){
                _cfg1 = level - 1;
                _cfg2 = 0x04;
            }
            else{
                _cfg1 = 0x00;
                _cfg2 = 0x00;
            }
        }
    }
    if (cfg1)
        *cfg1 = _cfg1;
    if (cfg2)
        *cfg2 = _cfg2;
}

/* Get the max ALS ADC count as a function of ATIME */
static uint8_t alsCurrentGainLevel(uint32_t x);
static uint8_t alsCurrentGainLevel(uint32_t x){
    uint8_t i;

    for (i = sizeof(alsGain_conversion)/sizeof(uint32_t)-1; i != 0; i--) {
        if (x == alsGain_conversion[i]) break;
    }
    return i;
}

static uint8_t alsTimeUsToReg(uint32_t x){
    uint8_t ticks;

    if(x < AMS_USEC_PER_TICK)
        return 0;

    /* Actual number of ticks will be (ticks + 1); if x is an exact multiple,
    * we want the next lower number, so bias it down by 1
    */
    x -= 1;

    if(x >= (255 * AMS_USEC_PER_TICK))
        return 0xff;

    ticks = (x / AMS_USEC_PER_TICK);
    return ticks;
}

uint32_t getAlsAdcMaxCount(uint32_t atime_usec){
    uint32_t counts;
    uint32_t ticks = 1 + (uint32_t)alsTimeUsToReg(atime_usec);
    counts = (ticks * AMS_ALS_MAX_COUNTS_PER_TICK) - 1;
    if (counts > AMS_ALS_MAX_COUNTS)
        return AMS_ALS_MAX_COUNTS;
    return counts;
}
#endif


#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
static uint32_t proxGain_conversion[] = { 1000, 2000, 4000, 8000};
static uint8_t proxGainToReg(uint32_t x){
    int i;

    for (i = sizeof(proxGain_conversion)/sizeof(uint32_t)-1; i != 0; i--) {
        if (x >= proxGain_conversion[i]) break;
    }
    return (i << 6);
}

// PTIME(also PRATE) defines the duration of 1 prox sample (PRATE+1)*88us
static uint8_t proxTimeToReg (uint32_t usec){
    if (usec > (255 * AMS_PRX_TIMEBASE))
        return (255 - 1);
    return ((usec / AMS_PRX_TIMEBASE) - 1);
}
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB

static bool _tmd2725_proxSetThreshold(ams_deviceCtx_t * ctx, int32_t threshold){
    ams_ccb_proximity_config_t configData;
    configData.sensitivity = (threshold & 0xff);
    ccb_proximitySetConfig(&ctx->ccbProxCtx, &configData);
    return false;
}

static void _tmd2725_handleProxEvent(sns_sensor_instance *const this, ams_deviceCtx_t * ctx){
    ams_ccb_proximity_dataSet_t proxData;
    uint8_t pData;
    proxData.statusReg = ctx->shadowStatusReg;
    ams_getByte(ctx->portHndl, DEVREG_PDATA, &pData);
    proxData.pData = pData;
    ccb_proximityHandle(this, ctx, &proxData);
}

bool tmd2725_deviceGetPrx(ams_deviceCtx_t * ctx, ams_apiPrx_t * exportData){
    ams_ccb_proximity_result_t outData;
    ccb_proximityGetResult(ctx, &outData);
    exportData->nearBy = outData.nearBy;
    exportData->pData = outData.proximity;
    exportData->proxOffset = outData.proxOffset;
    exportData->accuracy = outData.accuracy;
    return false;
}
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
static bool _tmd2725_alsSetThreshold(ams_deviceCtx_t * ctx, int32_t threshold){
    ams_ccb_als_config_t configData;
    configData.threshold = threshold;
    ccb_alsSetConfig(ctx, &configData);
    return false;
}

bool tmd2725_deviceGetAls(ams_deviceCtx_t * ctx, ams_apiAls_t * exportData){
    ams_ccb_als_result_t result;
    ccb_alsGetResult(ctx, &result);
    exportData->mLux        = result.mLux;
    exportData->rawClear    = result.rawClear;
    exportData->rawIR      = result.rawIR;
    exportData->quality     = result.quality;

    return false;
}

static void _tmd2725_handleAlsEvent(sns_sensor_instance *const this, ams_deviceCtx_t * ctx){
    ams_ccb_als_dataSet_t ccbAlsData;
    ccbAlsData.statusReg = ctx->shadowStatusReg;
    ccb_alsHandle(this, ctx, &ccbAlsData);
}

#endif

/*
* In SEE interrupts should only be enabled after receiving
* SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT.
*
* Therefore, if SNS_INTERRUPT_MSGID_SNS_INTERRUPT_REG_EVENT has been
* received in ams_inst_notify_event() then
*   ...
*   tmd2725_deviceSetConfig(state->priv, AMS_CONFIG_GLOBAL, AMS_CONFIG_ALLOW_INTS, 1);
*   ...
*
* When there are no more SEE clients _tmd2725_setup_clean_irq_stream() will called and
* it will call
*   ...
*   tmd2725_deviceSetConfig(state->priv, AMS_CONFIG_GLOBAL, AMS_CONFIG_ALLOW_INTS, 0);
*   ...
*
*/

bool tmd2725_deviceSetConfig(sns_sensor_instance *const this, ams_deviceCtx_t * ctx, ams_configureFeature_t feature, deviceConfigOptions_t option, uint32_t data){
    int ret = 0;
    uint8_t byte_count = 0;

    AMS_DEV_INST_PRINTF(HIGH, this, "DEVICE SET CONFIG ENTERED\n");
    if (feature == AMS_CONFIG_GLOBAL){
        AMS_DEV_INST_PRINTF(HIGH, this, "ams_configureFeature_t  AMS_CONFIG_GLOBAL(%d)\n", data);
        switch (option)
        {
            case AMS_CONFIG_ALLOW_INTS:
                if (data == 0) {
                    ctx->allowIens = false;
                } else {
                    ctx->allowIens = true;
                }
                break;

            default:
                SNS_INST_PRINTF(ERROR, this, "tmd2725_deviceSetConfig _GLOBAL: ERROR, unk. option %d\n", option);
                return true;
        }
    }

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
    if (feature == AMS_CONFIG_PROX){
        AMS_DEV_INST_PRINTF(HIGH, this, "ams_configureFeature_t  AMS_CONFIG_PROX\n");
        switch (option)
        {
            case AMS_CONFIG_ENABLE: /* power on */
                AMS_DEV_INST_PRINTF(HIGH, this, "deviceConfigOptions_t AMS_CONFIG_ENABLE(%u), current mode %d", data, ctx->mode);
                if (data == 0) {
                    if (ctx->mode == MODE_PROX) {
                        AMS_DEV_INST_PRINTF(HIGH, this, "deviceConfigOptions_t   AMS_CONFIG_ENABLE MODE_PROX");
                        /* if no other active features, turn off device */
                        ctx->shadowEnableReg = 0;
                        ctx->mode = MODE_OFF;
                        ccb_proximityInit(ctx, &ctx->ccbProxCtx.initData);
                    } else {
                        AMS_DEV_INST_PRINTF(HIGH, this, "deviceConfigOptions_t   AMS_CONFIG_ENABLE NO MODE_PROX");
                        ctx->mode &= ~MODE_PROX;
                        ctx->shadowEnableReg &= (~PEN);
                        ctx->shadowIntenabReg &= (~PIEN);
                    }
                } else {
                    /*  this will reinitialize the
                        proximity in a way that will force interrupt
                        before we were missing this interrupt because when initialliy other
                        feature was enabled the interrupt was generated but it was missed
                        since the interrupt was not configured because it wasn't needed
                    **/
                    ccb_proximityInit(ctx, &ctx->ccbProxCtx.initData);
                    ctx->shadowEnableReg |= (PEN | PON);
                    ctx->shadowIntenabReg |= PIEN;
                    ctx->mode |= MODE_PROX;
                }
                break;
            case AMS_CONFIG_THRESHOLD: /* set threshold */
                AMS_INST_PRINTF(HIGH, this,  "deviceConfigOptions_t   AMS_CONFIG_THRESHOLD\n");
                ret |= _tmd2725_proxSetThreshold(ctx, data);
                break;
            case AMS_CONFIG_ALWAYS_READ:
                AMS_DEV_INST_PRINTF(HIGH, this, "deviceConfigOptions_t   AMS_CONFIG_ALWAYS_READ(%d)\n", data);
                ctx->alwaysReadProx = !!data;
                break;
            default:
                break;
        }
    }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
    if (feature == AMS_CONFIG_ALS_LUX){
        AMS_DEV_INST_PRINTF(HIGH, this, "ams_configureFeature_t  AMS_CONFIG_ALS_LUX\n");
        switch (option)
        {
            case AMS_CONFIG_ENABLE: /* ON / OFF */
                AMS_DEV_INST_PRINTF(HIGH, this, "deviceConfigOptions_t AMS_CONFIG_ENABLE(%u), current mode %d", data, ctx->mode);
                if (data == 0) {
                    if (ctx->mode == MODE_ALS_LUX) {
                        /* if no other active features, turn off device */
                        ctx->shadowEnableReg = 0;
                        ctx->shadowIntenabReg = 0;
                        ctx->mode = MODE_OFF;
                    } else {
                        if ((ctx->mode & MODE_ALS_ALL) == MODE_ALS_LUX) {
                            ctx->shadowEnableReg &= ~MASK_AEN;
                            ctx->shadowIntenabReg &= ~(MASK_ALS_IEN_ALL);
                        }
                        ctx->mode &= ~(MODE_ALS_LUX);
                    }
                } else {
                    if ((ctx->mode & MODE_ALS_ALL) == 0) {
                        AMS_DEV_INST_PRINTF(HIGH, this,  "ALS RECONFIG\n");
                        ccb_alsInit(ctx, &ctx->ccbAlsCtx.initData);
                        if (ctx->ccbAlsCtx.state == AMS_CCB_ALS_INIT)
                            ccb_alsHandle(this, ctx, NULL);
                        ctx->shadowEnableReg |= (AEN | PON);
                        ctx->shadowIntenabReg |= MASK_ALS_IEN_ALL;
                    } else {
                        /* force interrupt */
                        AMS_DEV_INST_PRINTF(HIGH, this, "ALS FORCE INTERRUPT\n");
                        byte_count = ams_setWord(ctx->portHndl, DEVREG_AIHTL, 0x00);
                        AMS_DEV_INST_PRINTF(HIGH, this, "deviceConfigOptions_t  ams_setWord  write_count = %d", byte_count);
                    }
                    ctx->mode |= MODE_ALS_LUX;
                }
                break;
            case AMS_CONFIG_THRESHOLD: /* set threshold */
                AMS_DEV_INST_PRINTF(HIGH, this, "deviceConfigOptions_t   AMS_CONFIG_THRESHOLD, data = %d", data);
                ret |= _tmd2725_alsSetThreshold(ctx, data);
                break;
            case AMS_CONFIG_ALWAYS_READ:
                AMS_DEV_INST_PRINTF(HIGH, this, "deviceConfigOptions_t   AMS_CONFIG_ALWAYS_READ(%d)\n", data);
                ctx->alwaysReadAls = !!data;
                break;

            default:
                break;
        }
    }
#endif

    byte_count = ams_setEnable(ctx);
    AMS_INST_PRINTF(HIGH, this, "deviceConfigOptions_t  ams_setEnable  write_count = %d", byte_count);
    AMS_DEV_INST_PRINTF(HIGH, this, "DEVICE SET CONFIG COMPLETED\n");

    return 0;
}

void tmd2725_restoreThresholds(ams_deviceCtx_t * ctx){

    ams_ccb_als_ctx_t * ccbCtx = &ctx->ccbAlsCtx;
    uint8_t enable_reg_mask = 0;

    if (ctx->reset_als == true)
    {
        enable_reg_mask |= MASK_AEN;
    }

    if (ctx->reset_prox == true)
    {
        enable_reg_mask |= MASK_PEN;
    }

    /* Disable before writing any configuration changes */
    ams_setField(ctx->portHndl, DEVREG_ENABLE,  MASK_LOW,  enable_reg_mask);

    /* Restore the threshold values */
    if (ctx->reset_als == true)
    {
        AMS_SET_ALS_THRESHOLDS(ccbCtx->shadowAiltReg, ccbCtx->shadowAihtReg);        
    }

    if (ctx->reset_prox == true)
    {
        AMS_SET_PROX_THRS_LOW_HIGH(ctx->shadowPiltReg, ctx->shadowPihtReg);
    }
}

bool tmd2725_deviceEventHandler(sns_sensor_instance *const this, ams_deviceCtx_t * ctx){

    bool ret = true;
    sns_port_vector port_vecs[1];
    uint8_t byte_count;
    ams_instance_state *inst_state = (ams_instance_state*)this->state->state;

#ifdef ENABLE_DEBUG
    inst_exit_island((sns_sensor_instance *)this);
#endif

#if 0
#ifdef AMS_DEV_DEBUG_ENABLE
{
   // read wait register and enable, log execution timestamp
   uint8 temp_vec[4];
   uint64_t  temp_time, temp_resu;
   AMS_PORT_getByte(ctx->portHndl, 0x80, temp_vec, 4);
   AMS_INST_PRINTF(HIGH, this, "AMS: tmd2725_deviceEventHandler: enter: ENABLE=0x%02x, ATIME 0x%02x,PTIME 0x%02x,WTIME 0x%02x",
                  temp_vec[0], temp_vec[1], temp_vec[2], temp_vec[3]);
   temp_time = sns_get_system_time();  // in ticks
   temp_resu = sns_get_time_tick_resolution();  // in nano seconds
   SNS_INST_PRINTF(HIGH, this, "AMS: tmd2725_deviceEventHandler: enter: time is =0x%x, 0x%x", (uint32_t)(temp_time>>32), (uint32_t)temp_time);
   SNS_INST_PRINTF(HIGH, this, "AMS: tmd2725_deviceEventHandler: enter: reso is =0x%x, 0x%x", (uint32_t)(temp_resu>>32), (uint32_t)temp_resu);
}
#endif
#endif

    /* Setup STATUS register port vector */
    port_vecs[0].buffer = &ctx->shadowStatusReg;
    port_vecs[0].bytes = 1;
    port_vecs[0].is_write = false;
    port_vecs[0].reg_addr = deviceRegisterDefinition[DEVREG_STATUS].address;

    ams_rwVec(ctx->portHndl, port_vecs, 1);

    AMS_INST_PRINTF(HIGH, this, "AMS: tmd2725_deviceEventHandler: enter: STATUS=0x%02x", ctx->shadowStatusReg);

    /* If forced interrupt flag was set, the threshold values for the
     * functions must be restored.
     */
    if ((ctx->reset_als == true) || (ctx->reset_prox == true))
    {
        /* restore threhold values */
        tmd2725_restoreThresholds(ctx);
        
        ctx->reset_als = false;
        ctx->reset_prox = false;
    }

    /* If no bits are set then there is no need to continue */
    if(ctx->shadowStatusReg == 0)
    {
        AMS_INST_PRINTF(HIGH, this, "AMS: tmd2725_deviceEventHandler: no STATUS bits set \n");
        return ret;
    }

    /* Clear the status bits in the sensor HW */
    AMS_DEV_INST_PRINTF(HIGH, this, "AMS: write 0xFF to STATUS");
    ams_setByte(ctx->portHndl, DEVREG_STATUS, 0xFF);

    /* Handle prox calibration done */
    if(ctx->shadowStatusReg & MASK_CINT)
    {
        ams_getByte(ctx->portHndl, DEVREG_CALIBSTAT, &ctx->shadowStatusCalibReg);
        //make sure to clear zero_weight bits (7:4)
        ams_setByte(ctx->portHndl, DEVREG_CALIBSTAT, (ctx->shadowStatusCalibReg)&0x0f);
    }
    else
    {
        ctx->shadowStatusCalibReg = 0;
    }

    /*-----------------------------------------------------------*/
    /*                  Handle Proximity                         */
    /*-----------------------------------------------------------*/
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
    if ((ctx->shadowStatusReg & (PSAT | PINT | CINT)) || ctx->alwaysReadProx)
    {
        if (ctx->mode & MODE_PROX)
        {
            AMS_DEV_INST_PRINTF(MED, this, "tmd2725_deviceEventHandler: calling _tmd2725_handleProxEvent()\n");
            _tmd2725_handleProxEvent(this, ctx);
            AMS_DEV_INST_PRINTF(HIGH, this, "AMS: updateAvailableFlags = %x\n", ctx->updateAvailable);
        }
    }
#endif
    /*-----------------------------------------------------------*/
    /*                  Handle Ambient Light                     */
    /*-----------------------------------------------------------*/

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
    if (ctx->shadowStatusReg & (ALS_INT_ALL) || ctx->alwaysReadAls)
    {
        if (ctx->mode & MODE_ALS_ALL)
        {
            /* If ctx->updateAvailable has the AMS_ALS_GAIN_CHANGED bit
             * set it means that the last lux result has not yet been
             * reported. Do not compute a new lux .
             */
            AMS_DEV_INST_PRINTF(HIGH, this, "AMS: updateAvailableFlags = %x\n", ctx->updateAvailable);
            if (!(ctx->updateAvailable & (1 << AMS_ALS_GAIN_CHANGED)))
            {
                /* If the gain was NOT changed */
                AMS_DEV_INST_PRINTF(MED, this, "tmd2725_deviceEventHandler: calling _tmd2725_handleAlsEvent()\n");
                _tmd2725_handleAlsEvent(this, ctx);
            }
            else
            {
                AMS_DEV_INST_PRINTF(MED, this, "tmd2725_deviceEventHandler: _tmd2725_handleAlsEvent skipped because gain changed. updateAvailable = %08x\n",ctx->updateAvailable);
            }
        }
        else
        {
            AMS_DEV_INST_PRINTF(MED, this, "AMS: tmd2725_deviceEventHandler: no ALS bits in STATUS reg\n");
        }
    }
#endif

    if(ctx->shadowStatusReg & (0x04))
    {
        uint8_t enable_reg_mask = 0;

        AMS_DEV_INST_PRINTF(MED, this, "AMS: tmd2725_deviceEventHandler: ZINT\n");

        ctx->reset_als = false;
        ctx->reset_prox = false;
        
        if(inst_state->publish_sensors & (AMS_ALS_OC | AMS_ALS_STRM))
        {
            ctx->reset_als = true;
            enable_reg_mask |= MASK_AEN;
        }
        if(inst_state->publish_sensors & AMS_PROX_OC)
        {
            ctx->reset_prox = true;
            enable_reg_mask |= MASK_PEN;
        }

        if ((ctx->reset_als == true) || (ctx->reset_prox == true))
        {
            ams_setField(ctx->portHndl, DEVREG_ENABLE,  MASK_LOW,  enable_reg_mask);

            if (ctx->reset_als == true)
            {
                /* Save ALS threshold values */
                sns_port_vector als_reg_vecs[2];
                ams_ccb_als_ctx_t * ccbCtx = &ctx->ccbAlsCtx;

                als_reg_vecs[0].buffer = (uint8_t *)&ccbCtx->shadowAiltReg;
                als_reg_vecs[0].bytes = 2;
                als_reg_vecs[0].is_write = false;
                als_reg_vecs[0].reg_addr = deviceRegisterDefinition[DEVREG_AILTL].address;

                als_reg_vecs[1].buffer = (uint8_t *)&ccbCtx->shadowAihtReg;
                als_reg_vecs[1].bytes = 2;
                als_reg_vecs[1].is_write = false;
                als_reg_vecs[1].reg_addr = deviceRegisterDefinition[DEVREG_AIHTL].address;
    
                ams_rwVec(ctx->portHndl, als_reg_vecs, 2);
                
                /* force interrupt */
                AMS_SET_ALS_THRESHOLDS(0xffff, 0);
            }

            if (ctx->reset_prox == true)
            {
                /* Save PROX threshold values */
                sns_port_vector prox_reg_vecs[2];

                prox_reg_vecs[0].buffer = &ctx->shadowPiltReg;
                prox_reg_vecs[0].bytes = 1;
                prox_reg_vecs[0].is_write = false;
                prox_reg_vecs[0].reg_addr = deviceRegisterDefinition[DEVREG_PILT].address;

                prox_reg_vecs[1].buffer = &ctx->shadowPihtReg;
                prox_reg_vecs[1].bytes = 1;
                prox_reg_vecs[1].is_write = false;
                prox_reg_vecs[1].reg_addr = deviceRegisterDefinition[DEVREG_PIHT].address;
    
                ams_rwVec(ctx->portHndl, prox_reg_vecs, 2);
                
                // force prox interrupt if it is active
                AMS_SET_PROX_THRS_LOW_HIGH(0xff, 0x00);
            }

            ams_setField(ctx->portHndl, DEVREG_ENABLE,  MASK_HIGH,  enable_reg_mask);
        }
    }

    byte_count = ams_setEnable(ctx);

    return ret;
}

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
bool tmd2725_deviceCalibrateLux(sns_sensor_instance *instance, ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData, ams_alsCalType_t cal_type){
    ams_ccb_als_info_t infoData;
    sns_port_vector port_vec[2];

    AMS_INST_PRINTF(HIGH, instance, "tmd2725_deviceCalibrateLux: enter");

    if ((cal_type != AMS_ALS_CAL_TYPE_ON_CHANGE) &&
        (cal_type != AMS_ALS_CAL_TYPE_STREAMING))
    {
        SNS_INST_PRINTF(ERROR, instance, "tmd2725_deviceCalibrateLux: error: invalid calibration type");

        return false;
    }

    if (calibrationData){
        ctx->ccbAlsCtx.initData.calibrationData.calibrationFactor = calibrationData->alsCalibrationFactor;
    } else {
        ccb_alsInfo(&infoData);
        ctx->ccbAlsCtx.initData.calibrationData.calibrationFactor = infoData.defaultCalibrationData.calibrationFactor;
    }
    ctx->ccbAlsCtx.initData.configData.gain = 16000;
    ctx->ccbAlsCtx.initData.configData.uSecTime = 97200;
    ctx->ccbAlsCtx.initData.calibrate = true;
    ccb_alsInit(ctx, &ctx->ccbAlsCtx.initData);

    if (ctx->ccbAlsCtx.state != AMS_CCB_ALS_CALIBRATION_INIT)
    {
        AMS_INST_PRINTF(HIGH, instance, "tmd2725_deviceCalibrateLux: WARNING: state is not AMS_CCB_ALS_CALIBRATION_INIT, ccb state = %d", ctx->ccbAlsCtx.state);
    }

    AMS_SET_ALS_PERS(0x1);

    /* Save the ENABLE and INTENAB register settings so they
     * can be restored when the calibration is complete
     */
    ctx->shadowEnableRegPrevious = ctx->shadowEnableReg;
    ctx->shadowIntenabRegPrevious = ctx->shadowIntenabReg;

    /* Enable ALS */
    ctx->shadowEnableReg = (AEN | PON);

    /* Only enable ALS interrupt if on change mode */
    if (cal_type == AMS_ALS_CAL_TYPE_ON_CHANGE)
    {
        ctx->shadowIntenabReg = AIEN;
    }
    else
    {
        ctx->shadowIntenabReg = 0;

    }

    ctx->mode |= MODE_ALS_LUX;

    port_vec[0].is_write = true;
    port_vec[0].reg_addr = deviceRegisterDefinition[DEVREG_ENABLE].address;
    port_vec[0].bytes    = 1;
    port_vec[0].buffer   = &ctx->shadowEnableReg;

    port_vec[1].is_write = true;
    port_vec[1].reg_addr = deviceRegisterDefinition[DEVREG_INTENAB].address;
    port_vec[1].bytes    = 1;
    port_vec[1].buffer   = &ctx->shadowIntenabReg;

    ams_rwVec(ctx->portHndl, port_vec, 2);

    return true;
}
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
bool tmd2725_deviceCalibrateProx(sns_sensor_instance *instance, ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData){
    ams_ccb_proximity_info_t infoData;
    sns_port_vector port_vec[2];

    AMS_INST_PRINTF(HIGH, instance, "tmd2725_deviceCalibrateProx: We are here");

    if (calibrationData){
        ctx->ccbProxCtx.initData.nearByHardThreshold = calibrationData->proxHardThreshold;
        if (ctx->ccbProxCtx.initData.nearByHardThreshold < calibrationData->proxMinThreshold)
        {
            ctx->ccbProxCtx.initData.nearByHardThreshold = calibrationData->proxMinThreshold;
        }
    } else {
        ccb_proximityInfo(&infoData);
        ctx->ccbProxCtx.initData.nearByHardThreshold = infoData.defaultCalibrationData.hardThreshold;
    }

    ctx->ccbProxCtx.initData.calibrate = true;
    ctx->ccbProxCtx.initData.proxPoffsetCalibrate = false;
    ccb_proximityInit(ctx, &ctx->ccbProxCtx.initData);

    ctx->shadowEnableReg |= (PEN | PON);
    ctx->shadowIntenabReg |= PIEN;
    ctx->mode |= MODE_PROX;

    port_vec[0].is_write = true;
    port_vec[0].reg_addr = deviceRegisterDefinition[DEVREG_ENABLE].address;
    port_vec[0].bytes    = 1;
    port_vec[0].buffer   = &ctx->shadowEnableReg;

    port_vec[1].is_write = true;
    port_vec[1].reg_addr = deviceRegisterDefinition[DEVREG_INTENAB].address;
    port_vec[1].bytes    = 1;
    port_vec[1].buffer   = &ctx->shadowIntenabReg;

    ams_rwVec(ctx->portHndl, port_vec, 2);

    return true;
}

bool tmd2725_deviceCalibrateProxHardware(sns_sensor_instance *instance, ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData){
    AMS_INST_PRINTF(HIGH, instance, "tmd2725_deviceCalibrateProxHardware: We are here");
    ams_ccb_proximity_info_t infoData;
    sns_port_vector port_vec[2];

    if (calibrationData){
        ctx->ccbProxCtx.initData.nearByHardThreshold = calibrationData->proxHardThreshold;
        ctx->ccbProxCtx.initData.proxOffset = calibrationData->proxOffset;
        ctx->ccbProxCtx.initData.pOffsetSign = calibrationData->proxOffsetSign;

        if (ctx->ccbProxCtx.initData.nearByHardThreshold < calibrationData->proxMinThreshold)
        {
            ctx->ccbProxCtx.initData.nearByHardThreshold = calibrationData->proxMinThreshold;
        }
    } else {
        ccb_proximityInfo(&infoData);
        ctx->ccbProxCtx.initData.nearByHardThreshold = infoData.defaultCalibrationData.hardThreshold;
    }

    ctx->ccbProxCtx.initData.proxPoffsetCalibrate = true;
    ctx->ccbProxCtx.initData.calibrate = false;
    ccb_proximityInit(ctx, &ctx->ccbProxCtx.initData);

    ctx->shadowEnableReg |= (PEN | PON);
    ctx->shadowIntenabReg |= PIEN;
    ctx->mode |= MODE_PROX;

    port_vec[0].is_write = true;
    port_vec[0].reg_addr = deviceRegisterDefinition[DEVREG_ENABLE].address;
    port_vec[0].bytes    = 1;
    port_vec[0].buffer   = &ctx->shadowEnableReg;

    port_vec[1].is_write = true;
    port_vec[1].reg_addr = deviceRegisterDefinition[DEVREG_INTENAB].address;
    port_vec[1].bytes    = 1;
    port_vec[1].buffer   = &ctx->shadowIntenabReg;

    ams_rwVec(ctx->portHndl, port_vec, 2);

    return true;
}
#endif

uint32_t tmd2725_getResult(ams_deviceCtx_t * ctx) {
    uint32_t returnValue = ctx->updateAvailable;
    ctx->updateAvailable = 0;
    return returnValue;
}

typedef struct{
    uint8_t                 deviceId;
    uint8_t                 deviceIdMask;
    uint8_t                 deviceRef;
    uint8_t                 deviceRefMask;
    tmd2725_deviceIdentifier_e  device;
}tmd2725_deviceIdentifier_t;

static tmd2725_deviceIdentifier_t tmd2725_deviceIdentifier[]={
    {AMS_DEVICE_ID, MASK_ID, 0,          MASK_REV_ID, AMS_TMD2725},
    {AMS_DEVICE_ID, MASK_ID, AMS_REV_ID, MASK_REV_ID, AMS_TMD2725},
    {0, 0, 0, 0, AMS_LAST_DEVICE}
};

static void _tmd2725_resetAllRegisters(AMS_PORT_portHndl * portHndl);
static void _tmd2725_resetAllRegisters(AMS_PORT_portHndl * portHndl){
    ams_tmd2725_deviceRegister_t i, base;
    uint8_t rvals[DEVREG_REG_MAX - DEVREG_STATUS];
    int j;
    int addr, prevAddr, maxAddr;

    /* Initial consecutive regs up to just before _REVID */
    for (j=0, i = DEVREG_ENABLE; i <= DEVREG_CFG1; i++) {
        rvals[j++] = deviceRegisterDefinition[i].resetValue;
    }

    ams_setBuf(portHndl, DEVREG_ENABLE, rvals, j);

    /* after _ID reg, transfer each stretch of consecutive regs as a block */
    j = 0;
    i = base = DEVREG_STATUS;
    maxAddr = deviceRegisterDefinition[DEVREG_REG_MAX - 1].address;
    prevAddr = deviceRegisterDefinition[DEVREG_STATUS].address - 1;
    while ((maxAddr  >=  (addr = deviceRegisterDefinition[i].address))
            && (i < DEVREG_REG_MAX))
    {
        if (addr != (prevAddr + 1)) {
            ams_setBuf(portHndl, base, rvals, j);
            j = 0;
            base = i;
        }
        prevAddr = addr;
        rvals[j++] = deviceRegisterDefinition[i++].resetValue;
    }
    if (j) {
        ams_setBuf(portHndl, base, rvals, j);
    }
}

void * amsLibrary_open_tmd2725(int32_t hwid, AMS_PORT_portHndl* port_handle_ptr, \
                                ams_calibrationData_t *calibration_data_tmd2725, ams_deviceCtx_t *amsLibrary, \
                                ams_default_als_config_data_t *alsDefaultConfigData, ams_default_prox_config_data_t *proxDefaultConfigData){
    tmd2725_deviceIdentifier_e chipId = AMS_UNKNOWN_DEVICE;
    ams_deviceCtx_t *p_devctx = &amsLibrary[hwid];;

    if (NULL == amsLibrary) {
        chipId = tmd2725_validateDevice(port_handle_ptr);
        if (chipId != AMS_UNKNOWN_DEVICE){
            p_devctx = &tmd2725_device_contexts[hwid];
            tmd2725_deviceInit(p_devctx, port_handle_ptr, calibration_data_tmd2725, alsDefaultConfigData, proxDefaultConfigData, hwid);
        } else {
            p_devctx = NULL;
        }
    }
    else{
        p_devctx = NULL;
    }
    return (void*)p_devctx;
}

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
bool _tmd2725_proxInit(ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData, ams_default_prox_config_data_t * prox_default_config_values);
bool _tmd2725_proxInit(ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData, ams_default_prox_config_data_t * prox_default_config_values){

    ams_ccb_proximity_init_t proxInitData;

    proxInitData.calibrate = false;
    proxInitData.proxPoffsetCalibrate = false;

#ifdef CONFIG_AMS_PROX_STATIC_MODE
    /* this is fully static mode */
    proxInitData.dynamicMode = false;
#else
    /* this is semi-dynamic mode (using HW auto-offset-adjust) */
    proxInitData.dynamicMode = true;
#endif

    ctx->ccbProxCtx.initData.prox_default_config_values.pTime = prox_default_config_values->pTime;
    ctx->ccbProxCtx.initData.prox_default_config_values.pwTime_us = prox_default_config_values->pwTime_us;
    ctx->ccbProxCtx.initData.prox_default_config_values.pPulseLen = prox_default_config_values->pPulseLen;
    ctx->ccbProxCtx.initData.prox_default_config_values.pPulse = prox_default_config_values->pPulse;
    ctx->ccbProxCtx.initData.prox_default_config_values.pGain = prox_default_config_values->pGain;
    ctx->ccbProxCtx.initData.prox_default_config_values.pLdrive = prox_default_config_values->pLdrive;
    ctx->ccbProxCtx.initData.prox_default_config_values.pPers = prox_default_config_values->pPers;
    ctx->ccbProxCtx.initData.prox_default_config_values.pUpperThrs = prox_default_config_values->pUpperThrs;
    ctx->ccbProxCtx.initData.prox_default_config_values.pLowerThrs = prox_default_config_values->pLowerThrs;
    ctx->ccbProxCtx.initData.prox_default_config_values.pOffsetSign = prox_default_config_values->pOffsetSign;
    ctx->ccbProxCtx.initData.prox_default_config_values.proxOffset = prox_default_config_values->proxOffset;
    ctx->ccbProxCtx.initData.prox_default_config_values.pBinSearch = prox_default_config_values->pBinSearch;

    proxInitData.numberAdcBits = AMS_PRX_ADC_NO_BITS;
    proxInitData.adcThreshold = AMS_PRX_DEFAULT_ADC_THRESHOLD;
    proxInitData.nearByHardThreshold = AMS_PRX_DEFAULT_NEARBY_HARD_THRESHOLD;
    proxInitData.nearByThresholdHysteresis = AMS_PRX_DEFAULT_THRESHOLD_HYSTERESIS;

    proxInitData.ptime_us = prox_default_config_values->pTime;
    proxInitData.pwTime_us = prox_default_config_values->pwTime_us;
    proxInitData.pPulseLen = prox_default_config_values->pPulseLen;
    proxInitData.pPulse = prox_default_config_values->pPulse;
    proxInitData.pGain = prox_default_config_values->pGain;
    proxInitData.pLdrive = prox_default_config_values->pLdrive;
    proxInitData.pPers = prox_default_config_values->pPers;
    proxInitData.pUpperThrs = prox_default_config_values->pUpperThrs;
    proxInitData.pLowerThrs = prox_default_config_values->pLowerThrs;
    proxInitData.pOffsetSign = prox_default_config_values->pOffsetSign;
    proxInitData.proxOffset = prox_default_config_values->proxOffset;
    proxInitData.pBinSearch = prox_default_config_values->pBinSearch;

    AMS_SET_CAL_PROX_AUTO_OFFSET_ADJUST();
    if (calibrationData){
        proxInitData.nearByHardThreshold = calibrationData->proxHardThreshold;
        proxInitData.proxOffset = calibrationData->proxOffset;
        proxInitData.pOffsetSign = calibrationData->proxOffsetSign;
    }

    ccb_proximityInit(ctx, &proxInitData);
    return false;
}
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
bool _tmd2725_alsInit(ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData, ams_default_als_config_data_t * als_default_config_values);
bool _tmd2725_alsInit(ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData, ams_default_als_config_data_t * als_default_config_values){

    if (calibrationData == NULL) {
        ams_ccb_als_info_t infoData;
        ccb_alsInfo(&infoData);
        /* ALS calibration data */
        ctx->ccbAlsCtx.initData.calibrationData.luxTarget = infoData.defaultCalibrationData.luxTarget;
        ctx->ccbAlsCtx.initData.calibrationData.luxTargetError = infoData.defaultCalibrationData.luxTargetError;
        ctx->ccbAlsCtx.initData.calibrationData.calibrationFactor = infoData.defaultCalibrationData.calibrationFactor;
        ctx->ccbAlsCtx.initData.calibrationData.Time_base = infoData.defaultCalibrationData.Time_base;
        ctx->ccbAlsCtx.initData.calibrationData.thresholdLow = infoData.defaultCalibrationData.thresholdLow;
        ctx->ccbAlsCtx.initData.calibrationData.thresholdHigh = infoData.defaultCalibrationData.thresholdHigh;
        ctx->ccbAlsCtx.initData.calibrationData.calibrationFactor = infoData.defaultCalibrationData.calibrationFactor;
        ctx->ccbAlsCtx.initData.calibrationData.D_factor = infoData.defaultCalibrationData.D_factor;
        ctx->ccbAlsCtx.initData.calibrationData.L0_factor = infoData.defaultCalibrationData.L0_factor;
        ctx->ccbAlsCtx.initData.calibrationData.L1_factor = infoData.defaultCalibrationData.L1_factor;
        ctx->ccbAlsCtx.initData.calibrationData.L2_factor = infoData.defaultCalibrationData.L2_factor;
        ctx->ccbAlsCtx.initData.calibrationData.L3_factor = infoData.defaultCalibrationData.L3_factor;
    } else {
        /* ALS calibration data */
        ctx->ccbAlsCtx.initData.calibrationData.luxTarget = calibrationData->alsCalibrationLuxTarget;
        ctx->ccbAlsCtx.initData.calibrationData.luxTargetError = calibrationData->alsCalibrationLuxTargetError;
        ctx->ccbAlsCtx.initData.calibrationData.calibrationFactor = calibrationData->alsCalibrationFactor;
        ctx->ccbAlsCtx.initData.calibrationData.Time_base = calibrationData->timeBase_us;
        ctx->ccbAlsCtx.initData.calibrationData.thresholdLow = calibrationData->alsThresholdLow;
        ctx->ccbAlsCtx.initData.calibrationData.thresholdHigh = calibrationData->alsThresholdHigh;
        ctx->ccbAlsCtx.initData.calibrationData.calibrationFactor = calibrationData->alsCalibrationFactor;
        ctx->ccbAlsCtx.initData.calibrationData.D_factor = calibrationData->D_factor;
        ctx->ccbAlsCtx.initData.calibrationData.L0_factor = calibrationData->L0_factor;
        ctx->ccbAlsCtx.initData.calibrationData.L1_factor = calibrationData->L1_factor;
        ctx->ccbAlsCtx.initData.calibrationData.L2_factor = calibrationData->L2_factor;
        ctx->ccbAlsCtx.initData.calibrationData.L3_factor = calibrationData->L3_factor;

        ctx->ccbAlsCtx.initData.als_default_config_values.aTime = als_default_config_values->aTime;
        ctx->ccbAlsCtx.initData.als_default_config_values.aGain = als_default_config_values->aGain;
        ctx->ccbAlsCtx.initData.als_default_config_values.aPers = als_default_config_values->aPers;
        ctx->ccbAlsCtx.initData.als_default_config_values.wTime = als_default_config_values->wTime;
        ctx->ccbAlsCtx.initData.als_default_config_values.wLong = als_default_config_values->wLong;
        ctx->ccbAlsCtx.initData.als_default_config_values.aLowerThrs = als_default_config_values->aLowerThrs;
        ctx->ccbAlsCtx.initData.als_default_config_values.aUpperThrs = als_default_config_values->aUpperThrs;
        ctx->ccbAlsCtx.initData.als_default_config_values.aThrsWidth = als_default_config_values->aThrsWidth;
    }

    ctx->ccbAlsCtx.initData.calibrate = false;
    ctx->ccbAlsCtx.initData.configData.gain = als_default_config_values->aGain;
    ctx->ccbAlsCtx.initData.configData.uSecTime = als_default_config_values->aTime;
    ctx->ccbAlsCtx.initData.configData.threshold = als_default_config_values->aThrsWidth;
    ctx->ccbAlsCtx.initData.configData.clearChUpperThreshold = AMS_ALS_CLEAR_CHANNEL_UPPER_THRESHOLD;
    ctx->ccbAlsCtx.initData.configData.clearChLowerThreshold = AMS_ALS_CLEAR_CHANNEL_LOWER_THRESHOLD;
    ctx->alwaysReadAls = false;
    ctx->alwaysReadProx = false;
    ctx->ccbAlsCtx.initData.calibrationData.adcMaxCount = getAlsAdcMaxCount(ctx->ccbAlsCtx.initData.configData.uSecTime);
    ctx->ccbAlsCtx.initData.autoGain = true;

    ccb_alsInit(ctx, &ctx->ccbAlsCtx.initData);
    return false;
}
#endif


bool tmd2725_deviceInit(ams_deviceCtx_t * ctx, AMS_PORT_portHndl * portHndl, ams_calibrationData_t * calibrationData, \
                        ams_default_als_config_data_t * alsDefaultConfigValues, ams_default_prox_config_data_t * proxDefaultConfigValues, int32_t hwid){

    int ret = 0;

    memset((void*)ctx, 0, sizeof(ams_deviceCtx_t));
    ctx->hwid = hwid;
    ctx->portHndl = portHndl;
    ctx->mode = MODE_OFF;
    ctx->systemCalibrationData = calibrationData;
    ctx->alsDefaultConfigData = alsDefaultConfigValues;
    ctx->proxDefaultConfigData = proxDefaultConfigValues;
    ctx->deviceId = tmd2725_validateDevice(ctx->portHndl);
    ctx->shadowEnableReg = deviceRegisterDefinition[DEVREG_ENABLE].resetValue;
    _tmd2725_resetAllRegisters(ctx->portHndl);

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
    ret |= _tmd2725_proxInit(ctx, calibrationData, proxDefaultConfigValues);
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
    ret |= _tmd2725_alsInit(ctx, calibrationData, alsDefaultConfigValues);
#endif
    ams_setByte(ctx->portHndl, DEVREG_ENABLE, ctx->shadowEnableReg);
    return ret;
}

tmd2725_deviceIdentifier_e tmd2725_validateDevice(AMS_PORT_portHndl * portHndl){
    uint8_t tmd2725ChipId;
    uint8_t tmd2725RevId;
    uint8_t i = 0;
    sns_port_vector port_vec[2];
    uint8_t byte_count = 0;

    port_vec[0].is_write = false;
    port_vec[0].reg_addr = deviceRegisterDefinition[DEVREG_ID].address;
    port_vec[0].bytes    = 1;
    port_vec[0].buffer   = &tmd2725ChipId;

    port_vec[1].is_write = false;
    port_vec[1].reg_addr = deviceRegisterDefinition[DEVREG_REVID].address;
    port_vec[1].bytes    = 1;
    port_vec[1].buffer   = &tmd2725RevId;

    byte_count = ams_rwVec(portHndl, port_vec, 2);
    if (byte_count == 0)
    {
        return AMS_UNKNOWN_DEVICE;
    }

    do{
        if (((tmd2725ChipId & tmd2725_deviceIdentifier[i].deviceIdMask) ==
            (tmd2725_deviceIdentifier[i].deviceId & tmd2725_deviceIdentifier[i].deviceIdMask)) &&
            ((tmd2725RevId & tmd2725_deviceIdentifier[i].deviceRefMask) ==
             (tmd2725_deviceIdentifier[i].deviceRef & tmd2725_deviceIdentifier[i].deviceRefMask))){
                return tmd2725_deviceIdentifier[i].device;
        }
        i++;
    }while (tmd2725_deviceIdentifier[i].device != AMS_LAST_DEVICE);

    return AMS_UNKNOWN_DEVICE;
}

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
/*
 Proximity Active Current Calculation

Return Value is in microamps.

 ac = ((((ppulse_len + 0.5) * ppulse)/ptime(ms)) * pdrive(mA));

 */
static uint32_t ppulse_len_conversion[] = { 4, 8, 16, 32};
static int _tmd2725_calculate_prox_active_current(int32_t hwid)
{
    float ac = 0;                // active current
    float ppulse_len = 0;        // pulse length
    int ppulse = 0;              // number of pulses
    float ptime_ms = 0;          // ptime in ms
    int pldrive_mA = 0;          // drive current in mA, 6 * (pldrive + 1)
    uint8_t ptime;               // ptime register data
    uint8_t pcfg0;               // pcfg0 register data
    uint8_t pcfg1;               // pcfg1 register data
    sns_port_vector port_vec[3];
    ams_deviceCtx_t *p_devctx = &tmd2725_device_contexts[hwid];

    /* Read the current configuration from the registers */
    port_vec[0].is_write = false;
    port_vec[0].reg_addr = deviceRegisterDefinition[DEVREG_PTIME].address;
    port_vec[0].bytes    = 1;
    port_vec[0].buffer   = &ptime;

    port_vec[1].is_write = false;
    port_vec[1].reg_addr = deviceRegisterDefinition[DEVREG_PCFG0].address;
    port_vec[1].bytes    = 1;
    port_vec[1].buffer   = &pcfg0;

    port_vec[2].is_write = false;
    port_vec[2].reg_addr = deviceRegisterDefinition[DEVREG_PCFG1].address;
    port_vec[2].bytes    = 1;
    port_vec[2].buffer   = &pcfg1;

    ams_rwVec(p_devctx->portHndl, port_vec, 3);

    /* Evaluate the register data, refer to TMD2725 datasheet */
    ppulse_len = ppulse_len_conversion[(pcfg0 & MASK_PPULSE_LEN) >> 6];
    ppulse = (pcfg0 & MASK_PPULSE) + 1;
    ptime_ms = ((ptime + 1) * AMS_PRX_TIMEBASE) / 1000;
    pldrive_mA = 6 * ((pcfg1 & MASK_PLDRIVE) + 1);

    /* Calculate active current */
    ac = ((((ppulse_len + 0.5) * ppulse) / ptime_ms) * pldrive_mA);

    return (int)ac;
}

int32_t tmd2725_calculate_prox_active_current_from_setting(sns_sensor *const this)
{
    float ac = 0;                // active current
    float ppulse_len = 0;        // pulse length
    int ppulse = 0;              // number of pulses
    float ptime_ms = 0;          // ptime in ms
    int pldrive_mA = 0;          // drive current in mA, 6 * (pldrive + 1)
    uint8_t ptime_reg_val;       // ptime register data
    uint8_t pldrive_reg_val;     // pcfg0 register data
    uint8_t pulse_len_reg_val;
    ams_state *state = (ams_state*)this->state->state;

    /* Evaluate the register data, refer to TMD2725 datasheet */
    // we convert setting to register value, and then convert it back to
    // avoid the invalid setting and ensure the accuracy of calculated value
    pulse_len_reg_val = state->prox_default_config_data.pPulseLen&0x3;
    ppulse_len = ppulse_len_conversion[pulse_len_reg_val];
    ppulse = state->prox_default_config_data.pPulse & MASK_PPULSE;
    ptime_reg_val = proxTimeToReg(state->prox_default_config_data.pTime);
    ptime_ms = ((ptime_reg_val + 1) * AMS_PRX_TIMEBASE) / 1000;

    pldrive_reg_val = AMS_PRX_CURRENT_TO_REG(state->prox_default_config_data.pLdrive);
    pldrive_mA = 6 * ((pldrive_reg_val & MASK_PLDRIVE) + 1);

    /* Calculate active current */
    ac = ((((ppulse_len + 0.5) * ppulse) / ptime_ms) * pldrive_mA);

    return (int32_t)ac;
}
#endif

bool tmd2725_getDeviceInfo(ams_deviceInfo_t * info, int32_t hwid){
    memset(info, 0, sizeof(ams_deviceInfo_t));

    info->defaultCalibrationData.timeBase_us = AMS_USEC_PER_TICK;
    info->numberOfSubSensors = 0;
    info->memorySize =  sizeof(ams_deviceCtx_t);
    sns_memscpy(info->defaultCalibrationData.deviceName,
                sizeof(info->defaultCalibrationData.deviceName),
                "TMD2725", sizeof(info->defaultCalibrationData.deviceName));
    info->deviceModel = info->defaultCalibrationData.deviceName;
    info->deviceName  = "ALS/PRX";
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
    {
        info->defaultCalibrationData.proxHardThreshold = AMS_PRX_DEFAULT_NEARBY_HARD_THRESHOLD;
        info->proxSensor.driverName = "PROX";

        info->tableSubSensors[info->numberOfSubSensors]= AMS_PROXIMITY_SENSOR;
        info->numberOfSubSensors++;
        info->proxSensor.adcBits = 8;
        info->proxSensor.maxPolRate = 50;
        info->proxSensor.activeCurrent_uA = _tmd2725_calculate_prox_active_current(hwid);
        /* For the proximity active current to be accurate it must be calculated
         * from configuration values that have been programmed into the sensor
         * registers. However, attributes are published before the initial
         * configuration is written to the registers. We're waiting on a response
         * from Qualcomm about this. Therefore we are hard coding a value of
         * 1300 uA. This is accurate for our open air reference configuration.
         */
        info->proxSensor.standbyCurrent_uA = 5;
        info->proxSensor.rangeMax = 5; /* 0.05 units = meters */
        info->proxSensor.rangeMin = 0.0;
    }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
    {
        ams_deviceCtx_t *p_devctx = &tmd2725_device_contexts[hwid];
        UNUSED_VAR(p_devctx);
        ams_ccb_als_info_t infoData;
        ccb_alsInfo(&infoData);
        info->tableSubSensors[info->numberOfSubSensors]= AMS_AMBIENT_SENSOR;
        info->numberOfSubSensors++;

        info->alsSensor.driverName = infoData.algName;
        info->alsSensor.adcBits = 16;
        info->alsSensor.maxPolRate = 50;
        info->alsSensor.activeCurrent_uA = 80;
        info->alsSensor.standbyCurrent_uA = 5;
        /* max lux ( AGAIN = 16x ; ATIME = 102.86ms ; ADCs: ch0 = 37887) */
        info->alsSensor.rangeMax = 23020;
        info->alsSensor.rangeMin = 0;

        // ALS default Lux Resolution
        // ALS Lux Resolution = (lux reading / ALs Count Value w.r.t the ATIME)
        // Calibration is done at 300 lux, hence set the ALS lux Reading to 300
        // ATIME is set to 102.86, the max count with this ATIME is 37887
        // Hence, Default ALS lux Resolution = (300 / 37887)
        info->alsSensor.alsResolution = (float)(300.00f / 37887.00f);

        info->defaultCalibrationData.alsCalibrationFactor = infoData.defaultCalibrationData.calibrationFactor;
        info->defaultCalibrationData.alsCalibrationLuxTarget = infoData.defaultCalibrationData.luxTarget;
        info->defaultCalibrationData.alsCalibrationLuxTargetError = infoData.defaultCalibrationData.luxTargetError;

        info->defaultCalibrationData.D_factor = infoData.defaultCalibrationData.D_factor;
        info->defaultCalibrationData.L0_factor = infoData.defaultCalibrationData.L0_factor;
        info->defaultCalibrationData.L1_factor = infoData.defaultCalibrationData.L1_factor;
        info->defaultCalibrationData.L2_factor = infoData.defaultCalibrationData.L2_factor;
        info->defaultCalibrationData.L3_factor = infoData.defaultCalibrationData.L3_factor;
    }
#endif
    return false;
}

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS

int amsAlg_als_setConfig_tmd2725(amsAlsContext_t * ctx, amsAlsConf_t * inputData){
    int ret = 0;

    if (inputData != NULL) {
        ctx->gain = inputData->gain;
        ctx->time_us = inputData->time_us;
    }
    als_update_statics(ctx);
    return ret;
}

/*
 * getConfig: is used to quarry the algorithm's configuration
 */
int amsAlg_als_getConfig_tmd2725(amsAlsContext_t * ctx, amsAlsConf_t * outputData){
    int ret = 0;

    outputData->gain = ctx->gain;
    outputData->time_us = ctx->time_us;

    return ret;
}
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS

/*
 * getAlgInfo: Intended to be called by a wrapper, allowing the wrapper to
 * allocate memory for the algorithm.
 * It's not needed to call this API, but it could be useful in certain OS'es
 */
int amsAlg_als_getAlgInfo_tmd2725 (amsAlsAlgoInfo_t * info) {
    int ret = 0;

    info->algName = "AMS_ALS";
    info->initAlg = &amsAlg_als_initAlg_tmd2725;
    info->processData = &amsAlg_als_processData_tmd2725;
    info->getResult = &amsAlg_als_getResult_tmd2725;
    info->setConfig = &amsAlg_als_setConfig_tmd2725;
    info->getConfig = &amsAlg_als_getConfig_tmd2725;

    return ret;
}
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS

extern void als_calc_cpl(amsAlsContext_t * ctx);

/*
 * initAlg: is used to initialize the algorithm.
 */
int amsAlg_als_initAlg_tmd2725 (amsAlsContext_t * ctx, amsAlsInitData_t * initData){
    int ret = 0;

    memset(ctx, 0, sizeof(amsAlsContext_t));

    if (initData != NULL) {
        ctx->calibration.Time_base = initData->calibration.Time_base;
        ctx->calibration.adcMaxCount = initData->calibration.adcMaxCount;
        ctx->calibration.thresholdLow = initData->calibration.thresholdLow;
        ctx->calibration.thresholdHigh = initData->calibration.thresholdHigh;
        ctx->calibration.calibrationFactor = initData->calibration.calibrationFactor;
        ctx->calibration.D_factor  = initData->calibration.D_factor;
        ctx->calibration.L0_factor = initData->calibration.L0_factor;
        ctx->calibration.L1_factor = initData->calibration.L1_factor;
        ctx->calibration.L2_factor = initData->calibration.L2_factor;
        ctx->calibration.L3_factor = initData->calibration.L3_factor;

        ctx->alsDefaultConfigData.aTime = initData->alsDefaultConfig.aTime;
        ctx->alsDefaultConfigData.aGain = initData->alsDefaultConfig.aGain;
        ctx->alsDefaultConfigData.aPers = initData->alsDefaultConfig.aPers;
        ctx->alsDefaultConfigData.wLong = initData->alsDefaultConfig.wLong;
        ctx->alsDefaultConfigData.wTime = initData->alsDefaultConfig.wTime;
        ctx->alsDefaultConfigData.aLowerThrs = initData->alsDefaultConfig.aLowerThrs;
        ctx->alsDefaultConfigData.aUpperThrs = initData->alsDefaultConfig.aUpperThrs;
        ctx->alsDefaultConfigData.aThrsWidth = initData->alsDefaultConfig.aThrsWidth;
    }

    if (initData != NULL) {
        ctx->gain = initData->gain;
        ctx->time_us = initData->time_us;
        ctx->adaptive = initData->adaptive;
    }

    als_update_statics(ctx);
    return ret;
}
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS

void als_getReportingParams_tmd2725(amsAlsContext_t * ctx,
                              float *maxLux,
                              float *luxResolution)
{
    int64_t a;
    int64_t result;
    int64_t result_1;
    union _qbytes {int64_t sqword; uint32_t udword[2]; uint8_t ubyte[8];} qwd; /* dbg */

    /*-----  ILLUMINANCE  -----*/

    /* Max lux is attained when clear & channel 1 are max */
    if (maxLux) {
        /* .calibrationFactor is scaled up by 1000x (AMS_ALS_GAIN_FACTOR)
         * so we descale here to give lux in lieu of millilux
         */
        result = (int64_t)ctx->saturation * (AMS_ALS_L0_FACTOR + AMS_ALS_L1_FACTOR);
        result_1 = (int64_t)ctx->saturation * (AMS_ALS_L1_FACTOR + AMS_ALS_L2_FACTOR);
        a = (result > result_1) ? result : result_1;
        a *= (int64_t)ctx->calibration.calibrationFactor;
        a /= AMS_ALS_GAIN_FACTOR * (int64_t)ctx->cpl;
        qwd.sqword = a;
        *maxLux = (float)a;
    }

    /* L1 has smallest coefficient so 1 count of red gives smallest
     * resolvable change
     */
    if (luxResolution) {
        a = AMS_ALS_L1_FACTOR * (int64_t)ctx->calibration.calibrationFactor;
        a /= (int64_t)ctx->cpl;
        /* units = millilux here */
        if (a < 0) a = -a;
        /* It's limited by our 1 mLux quantization: */
        if (a < 1) a = 1;
        qwd.sqword = a;
        *luxResolution = 0.001 * (float)a;
    }
}

int amsAlg_als_processData_tmd2725(sns_sensor_instance *const this, amsAlsContext_t * ctx, amsAlsDataSet_t * inputData){
    int ret = 0;

    if (inputData->status & ALS_STATUS_RDY){
        AMS_DEV_INST_PRINTF(HIGH, this, "ALS Status = ALS_READY\n");
        ctx->previousLux = ctx->results.mLux;
        if (ctx->previousGain != ctx->gain){
            AMS_DEV_INST_PRINTF(HIGH, this, "Previous_gain = %d not equal to Context_gain = %d\n", ctx->previousGain, ctx->gain);
            als_update_statics(ctx);
        }

        als_compute_data_clear(this, ctx, inputData);
        als_ave_LUX(ctx);
    }

    if (ctx->adaptive){
        AMS_DEV_INST_PRINTF(HIGH, this, "ALS Adaptive\n");
        if (inputData->status & ALS_STATUS_OVFL){
            ctx->results.adaptive = ADAPTIVE_ALS_GAIN_DEC_REQUEST;
        } else {
            ctx->results.adaptive = tmd2725_als_adaptive(ctx, inputData);
        }
    } else {
        ctx->results.adaptive = ADAPTIVE_ALS_NO_REQUEST;
    }
    return ret;
}
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS

/*
 * getResult: will return the output generated by the algorithm
 */

int amsAlg_als_getResult_tmd2725(amsAlsContext_t * ctx, amsAlsResult_t * outData){
    int ret = 0;

    outData->rawClear  = ctx->results.rawClear;
    outData->rawIR  = ctx->results.rawIR;
    outData->irrClear = ctx->results.irrClear;
    outData->irrRed   = ctx->results.irrRed;
    outData->mLux_ave  = ctx->results.mLux_ave / AMS_LUX_AVERAGE_COUNT;
    outData->IR  = ctx->results.IR;
    outData->CCT = ctx->results.CCT;
    outData->adaptive = ctx->results.adaptive;
    outData->mLux = ctx->results.mLux;

    return ret;
}
#endif

static void ccb_alsInit(void * dcbCtx, ams_ccb_als_init_t* initData){
    ams_deviceCtx_t * ctx = (ams_deviceCtx_t*)dcbCtx;
    ams_ccb_als_ctx_t * ccbCtx = &ctx->ccbAlsCtx;
    amsAlsInitData_t initAlsData;
    amsAlsAlgoInfo_t infoAls;

    if (initData){
        sns_memscpy(&ccbCtx->initData, sizeof(ccbCtx->initData), initData, sizeof(ams_ccb_als_init_t));
    } else {
        ccbCtx->initData.calibrate = false;
    }

    initAlsData.adaptive = false;
    initAlsData.irRejection = false;
    initAlsData.gain = ccbCtx->initData.configData.gain;
    initAlsData.time_us = ccbCtx->initData.configData.uSecTime;
    initAlsData.calibration.adcMaxCount = ccbCtx->initData.calibrationData.adcMaxCount;

    initAlsData.calibration.calibrationFactor = ccbCtx->initData.calibrationData.calibrationFactor;
    initAlsData.calibration.Time_base = ccbCtx->initData.calibrationData.Time_base;
    initAlsData.calibration.thresholdLow = ccbCtx->initData.calibrationData.thresholdLow;
    initAlsData.calibration.thresholdHigh = ccbCtx->initData.calibrationData.thresholdHigh;
    initAlsData.calibration.calibrationFactor = ccbCtx->initData.calibrationData.calibrationFactor;
    initAlsData.calibration.D_factor = ccbCtx->initData.calibrationData.D_factor;
    initAlsData.calibration.L0_factor = ccbCtx->initData.calibrationData.L0_factor;
    initAlsData.calibration.L1_factor = ccbCtx->initData.calibrationData.L1_factor;
    initAlsData.calibration.L2_factor = ccbCtx->initData.calibrationData.L2_factor;
    initAlsData.calibration.L3_factor = ccbCtx->initData.calibrationData.L3_factor;

    initAlsData.alsDefaultConfig.aTime = ccbCtx->initData.als_default_config_values.aTime;
    initAlsData.alsDefaultConfig.aPers = ccbCtx->initData.als_default_config_values.aPers;
    initAlsData.alsDefaultConfig.aGain = ccbCtx->initData.als_default_config_values.aGain;
    initAlsData.alsDefaultConfig.wTime = ccbCtx->initData.als_default_config_values.wTime;
    initAlsData.alsDefaultConfig.wLong = ccbCtx->initData.als_default_config_values.wLong;
    initAlsData.alsDefaultConfig.aLowerThrs = ccbCtx->initData.als_default_config_values.aLowerThrs;
    initAlsData.alsDefaultConfig.aUpperThrs = ccbCtx->initData.als_default_config_values.aUpperThrs;
    initAlsData.alsDefaultConfig.aThrsWidth = ccbCtx->initData.als_default_config_values.aThrsWidth;

    amsAlg_als_getAlgInfo_tmd2725(&infoAls);

    amsAlg_als_initAlg_tmd2725(&ccbCtx->ctxAlgAls, &initAlsData);

    //set Wait Time
    if(ccbCtx->ctxAlgAls.alsDefaultConfigData.wTime >= AMS_MIN_WAIT_TIME)
    {
        uint8_t temp_i = (ccbCtx->ctxAlgAls.alsDefaultConfigData.wTime/AMS_MIN_WAIT_TIME)-1;
        AMS_SET_WTIME(temp_i);
        ctx->shadowEnableReg |= WEN;
    }
    else
    {
        ctx->shadowEnableReg &= (~WEN);
    }

    ccbCtx->shadowAiltReg = 0xffff;
    ccbCtx->shadowAihtReg = 0;
    AMS_SET_ALS_THRESHOLDS(ccbCtx->shadowAiltReg, ccbCtx->shadowAihtReg);

    if (ccbCtx->initData.calibrate){
        ccbCtx->state = AMS_CCB_ALS_CALIBRATION_INIT;
    } else {
        ccbCtx->state = AMS_CCB_ALS_INIT;
    }
    ccbCtx->wait_for_clear = false;
}

static void ccb_alsSetConfig(void * dcbCtx, ams_ccb_als_config_t * configData){
    ams_ccb_als_ctx_t * ccbCtx = &((ams_deviceCtx_t*)dcbCtx)->ccbAlsCtx;

    ccbCtx->initData.configData.threshold = configData->threshold;
}

static bool ccb_alsHandle(sns_sensor_instance *const this, void * dcbCtx, ams_ccb_als_dataSet_t * alsData){

    ams_deviceCtx_t * ctx = (ams_deviceCtx_t*)dcbCtx;
    ams_ccb_als_ctx_t * ccbCtx = &((ams_deviceCtx_t*)dcbCtx)->ccbAlsCtx;
    amsAlsDataSet_t inputDataAls;
    static adcDataSet_t adcData;

    if(ccbCtx->wait_for_clear)
    {
        AMS_DISABLE_ALS();
        AMS_DEV_INST_PRINTF(MED, this,"ccb_alsHandle on hold, wait_for_clear is true");
        return true;
    }

    AMS_DEV_INST_PRINTF(MED, this,
                      "ccb_alsHandle: case = %d\n"
                      "ctx->systemCalibrationData->alsCalibrationFactor = %d, "
                      "ctx->systemCalibrationData->D_factor = %d, "
                      "ctx->systemCalibrationData->L0_factor = %d, "
                      "ctx->systemCalibrationData->L1_factor = %d, "
                      "ctx->systemCalibrationData->L2_factor = %d, "
                      "ctx->systemCalibrationData->L3_factor = %d, "
                      "ctx->alsDefaultConfigData->aTime = %d, "
                      "ctx->alsDefaultConfigData->aGain = %d"
                      , ccbCtx->state
                      , ctx->systemCalibrationData->alsCalibrationFactor
                      , ctx->systemCalibrationData->D_factor
                      , ctx->systemCalibrationData->L0_factor
                      , ctx->systemCalibrationData->L1_factor
                      , ctx->systemCalibrationData->L2_factor
                      , ctx->systemCalibrationData->L3_factor
                      , ctx->alsDefaultConfigData->aTime
                      , ctx->alsDefaultConfigData->aGain);
    AMS_DEV_INST_PRINTF(MED, this,
                      "ctx->alsDefaultConfigData->aPers = %d, "
                      "ctx->alsDefaultConfigData->athrsWidth = %d",
                        ctx->alsDefaultConfigData->aPers
                       ,ctx->alsDefaultConfigData->aThrsWidth);

    if ((alsData == NULL) && (ccbCtx->state != AMS_CCB_ALS_INIT)){
        /* The initial call (only) is expected to have no alsData because
         * it occurs before any conditions in statusReg have happened
         */
        return true;
    }
    switch (ccbCtx->state){
    case AMS_CCB_ALS_INIT:
        ctx->ccbAlsCtx.ctxAlgAls.changedAlsGain = AMS_INVALID_GAIN;
        AMS_DISABLE_ALS();
        AMS_SET_ALS_TIME(ctx->alsDefaultConfigData->aTime);
        AMS_SET_ALS_PERS(ctx->alsDefaultConfigData->aPers);
        AMS_SET_ALS_GAIN(ctx->alsDefaultConfigData->aGain);
        /* force interrupt */
        ccbCtx->shadowAiltReg = 0xffff;
        ccbCtx->shadowAihtReg = 0;
        AMS_SET_ALS_THRESHOLDS(ccbCtx->shadowAiltReg, ccbCtx->shadowAihtReg);
        ccbCtx->state = AMS_CCB_ALS_GAIN;
        AMS_REENABLE_ALS();
        break;
    case AMS_CCB_ALS_GAIN:
        AMS_ALS_GET_CR(&adcData);
        if(ccbCtx->initData.autoGain == true)
        {
            ctx->ccbAlsCtx.ctxAlgAls.changedAlsGain = changeAlsGain(this, ctx, adcData, alsData);
        }
        /* force interrupt */
        AMS_DISABLE_ALS();
        ccbCtx->shadowAiltReg = 0xffff;
        ccbCtx->shadowAihtReg = 0;
        AMS_SET_ALS_THRESHOLDS(ccbCtx->shadowAiltReg, ccbCtx->shadowAihtReg);
        ccbCtx->state = AMS_CCB_ALS_RGB;
        AMS_REENABLE_ALS();
        break;
    case AMS_CCB_ALS_RGB: /* state to measure RGB */
#ifdef HAVE_OPTION__ALWAYS_READ
    if ((alsData->statusReg & (AINT)) || ctx->alwaysReadAls)
#else
    if (alsData->statusReg & (AINT))
#endif
    {
        AMS_ALS_GET_CR(&adcData);

        inputDataAls.status = ALS_STATUS_RDY;
        inputDataAls.datasetArray = (alsData_t*)&adcData;
        AMS_DEV_INST_PRINTF(HIGH, this, "Configdata ALS Threshold value = %d\n", ccbCtx->initData.configData.threshold);
        if (ctx->alsDefaultConfigData->aThrsWidth){
            uint32_t tempThresh;
            tempThresh = (((uint32_t)adcData.AdcClear * (100 - (uint32_t)(ctx->alsDefaultConfigData->aThrsWidth))) + 50) / 100;
            ccbCtx->shadowAiltReg = (uint16_t)tempThresh;
            tempThresh = (((uint32_t)adcData.AdcClear * (100 + (uint32_t)(ctx->alsDefaultConfigData->aThrsWidth))) + 50) / 100;
            ccbCtx->shadowAihtReg = (uint16_t)tempThresh;
            AMS_SET_ALS_THRESHOLDS(ccbCtx->shadowAiltReg, ccbCtx->shadowAihtReg);
                        AMS_DEV_INST_PRINTF(HIGH, this, "ccb_alsHandle change interrupt threshold 1 = %d, 2= %d", (uint32_t)ccbCtx->shadowAiltReg, (uint32_t)ccbCtx->shadowAihtReg);
        }

        /*------------------ Process ADC Data -----------------*/
        AMS_DEV_INST_PRINTF(HIGH, this, "ccb_alsHandle: calling amsAlg_als_processData_tmd2725 updateAvailable = %x\n", ctx->updateAvailable);

        amsAlg_als_processData_tmd2725(this, &ctx->ccbAlsCtx.ctxAlgAls, &inputDataAls);

        /* Evaluate the quality of the result */
        if(!(alsData->statusReg & ASAT)){
            ctx->ccbAlsCtx.ctxAlgAls.results.quality &= ~(ALS_QUALITY_LUX_MASK | ALS_QUALITY_CCT_MASK);
            ctx->ccbAlsCtx.ctxAlgAls.results.quality |= (ALS_QUALITY_LUX_GOOD | ALS_QUALITY_CCT_GOOD);
        }
        else{
            ctx->ccbAlsCtx.ctxAlgAls.results.quality &= ~(ALS_QUALITY_LUX_MASK | ALS_QUALITY_CCT_MASK);
            ctx->ccbAlsCtx.ctxAlgAls.results.quality |= (ALS_QUALITY_LUX_GOOD | ALS_QUALITY_CCT_BAD);
            AMS_SET_ALS_THRESHOLDS(0xffff, 0);
        }

        ctx->updateAvailable |= (1 << AMS_AMBIENT_SENSOR);
        ccbCtx->state = AMS_CCB_ALS_RGB;

        /* Software AGC */
        if(ccbCtx->initData.autoGain == true)
        {
            ctx->ccbAlsCtx.ctxAlgAls.changedAlsGain = changeAlsGain(this, ctx, adcData, alsData);
        }

        break;
    }

    case AMS_CCB_ALS_CALIBRATION_INIT:
        if (alsData->statusReg & (AINT))
        {
            AMS_DISABLE_ALS();
            AMS_SET_ALS_TIME(ctx->alsDefaultConfigData->aTime);
            AMS_SET_ALS_PERS(0);
            AMS_SET_ALS_GAIN(ctx->alsDefaultConfigData->aGain);
            ccbCtx->shadowAiltReg = 0xffff;
            ccbCtx->shadowAihtReg = 0;
            AMS_SET_ALS_THRESHOLDS(ccbCtx->shadowAiltReg, ccbCtx->shadowAihtReg);

            ccbCtx->state = AMS_CCB_ALS_CALIBRATION_COLLECT_DATA;
            ccbCtx->ctxAlgAls.ave_lux[AMS_LUX_AVERAGE_COUNT - 1] = 0;
            AMS_REENABLE_ALS();
        }
    break;
    case AMS_CCB_ALS_CALIBRATION_COLLECT_DATA:
        if (alsData->statusReg & (AINT))
        {
            AMS_ALS_GET_CR(&adcData);

            inputDataAls.status = ALS_STATUS_RDY;
            inputDataAls.datasetArray = (alsData_t*)&adcData;
            amsAlg_als_processData_tmd2725(this, &ctx->ccbAlsCtx.ctxAlgAls, &inputDataAls);
            ccbCtx->bufferCounter += 1;
            AMS_DEV_INST_PRINTF(HIGH, this, "AMS_CCB_ALS_CALIBRATION_COLLECT_DATA = %d\n", ccbCtx->bufferCounter);

            /* Software Autogain */
            if (ccbCtx->initData.autoGain == true)
            {
                AMS_DEV_INST_PRINTF(HIGH, this, "ccb_alsHandle 3 calling changeAlsGain()\n");
                changeAlsGain(this, ctx, adcData, alsData);
            }

            /* If all calibration samples have been collected
             * compute the average lux and new calibration scale factor
             */
            if (ccbCtx->bufferCounter == AMS_LUX_AVERAGE_COUNT)
            {
                uint32_t avaLux = ctx->ccbAlsCtx.ctxAlgAls.results.mLux_ave / AMS_LUX_AVERAGE_COUNT;
                uint32_t factor = (ccbCtx->initData.calibrationData.luxTarget * 1000) / (avaLux / 1000);
                ctx->ccbAlsCtx.ctxAlgAls.calibration.calibrationFactor = factor;

                AMS_DEV_INST_PRINTF(HIGH, this,
                                    "ccb_alsHandle: target = %d\n ccb_alsHandle: mLux   = %d\n"
                                    "ccb_alsHandle: aveLux = %d\n ccb_alsHandle: factor = %d\n",
                                    ccbCtx->initData.calibrationData.luxTarget,
                                    ctx->ccbAlsCtx.ctxAlgAls.results.mLux,
                                    avaLux, factor);

                ccbCtx->ctxAlgAls.ave_lux[AMS_LUX_AVERAGE_COUNT - 1] = 0;
                ccbCtx->state = AMS_CCB_ALS_CALIBRATION_CHECK;
            }
        }
        break;
    case AMS_CCB_ALS_CALIBRATION_CHECK:
        AMS_DEV_INST_PRINTF(HIGH, this, "AMS_CCB_ALS_CALIBRATION_CHECK\n");

        if (alsData->statusReg & (AINT))
        {
            AMS_ALS_GET_CR(&adcData);
            inputDataAls.status = ALS_STATUS_RDY;
            inputDataAls.datasetArray = (alsData_t*)&adcData;
            amsAlg_als_processData_tmd2725(this, &ctx->ccbAlsCtx.ctxAlgAls, &inputDataAls);

            ccbCtx->bufferCounter += 1;
            if (ccbCtx->bufferCounter > AMS_LUX_AVERAGE_COUNT)
            {
                ccbCtx->bufferCounter = AMS_LUX_AVERAGE_COUNT;
            }
            if (ccbCtx->bufferCounter == AMS_LUX_AVERAGE_COUNT)
            {
                uint32_t avaLux = ctx->ccbAlsCtx.ctxAlgAls.results.mLux_ave / AMS_LUX_AVERAGE_COUNT;
                ccbCtx->bufferCounter = 0;
                AMS_DEV_INST_PRINTF(HIGH, this,"ccb_alsHandle: target = %d\n"
                                    "ccb_alsHandle: mLux   = %d\n"
                                    "ccb_alsHandle: aveLux = %d\n"
                                    "ccb_alsHandle_tmd2725: factor = %d\n"
                                    , ccbCtx->initData.calibrationData.luxTarget
                                    , ctx->ccbAlsCtx.ctxAlgAls.results.mLux
                                    , ctx->ccbAlsCtx.ctxAlgAls.results.mLux_ave / AMS_LUX_AVERAGE_COUNT
                                    , ctx->ccbAlsCtx.ctxAlgAls.calibration.calibrationFactor);
                ccbCtx->state = AMS_CCB_ALS_INIT;
                ccbCtx->initData.calibrate = false;
                if(abs((signed)(avaLux / 1000) - ccbCtx->initData.calibrationData.luxTarget) < ccbCtx->initData.calibrationData.luxTargetError)
                {
                    AMS_DEV_INST_PRINTF(HIGH, this, "AMS_CCB_ALS_CALIBRATION_CHECK pass\n");
                    ctx->updateAvailable = AMS_CALIBRATION_DONE;
                    ctx->systemCalibrationData->alsCalibrationFactor = ctx->ccbAlsCtx.ctxAlgAls.calibration.calibrationFactor;
                }
                else
                {
                    AMS_DEV_INST_PRINTF(HIGH, this, "AMS_CCB_ALS_CALIBRATION_CHECK fail:  %u not in range %d +/- %d\n"
                                        , (avaLux / 1000)
                                        , ccbCtx->initData.calibrationData.luxTarget
                                        , ccbCtx->initData.calibrationData.luxTargetError
                        );
                    ctx->updateAvailable = AMS_CALIBRATION_DONE_BUT_FAILED;
                    ctx->ccbAlsCtx.ctxAlgAls.calibration.calibrationFactor = ctx->systemCalibrationData->alsCalibrationFactor;
                }
            }
        }
        break;
    default:
        ccbCtx->state = AMS_CCB_ALS_RGB;
    break;
    }
    return false;
}

uint32_t changeAlsGain(sns_sensor_instance *const this, void * dcbCtx, adcDataSet_t adcData, ams_ccb_als_dataSet_t *alsData)
{
    ams_deviceCtx_t * ctx = (ams_deviceCtx_t*)dcbCtx;
    ams_ccb_als_ctx_t * ccbCtx = &((ams_deviceCtx_t*)dcbCtx)->ccbAlsCtx;
    ams_instance_state *state = (ams_instance_state*)this->state->state;

    uint8_t currentAlsGainLevel = 0;
    uint8_t again = 0;
    static uint8_t previousAlsGainLevel;
    uint8_t setAlsGainLevel = 0;
    uint32_t currentClearChRawCountValue = 0;
    uint32_t maxRawChCountValue = ctx->ccbAlsCtx.ctxAlgAls.saturation;
    bool need_to_decrement = false;
    amsAlsContext_t  *als_ctx = &ctx->ccbAlsCtx.ctxAlgAls;

    AMS_DEV_INST_PRINTF(HIGH, this, "changeAlsGain(): maxRawChCountValue = %d changeAlsGain(): CLEAR Channel Raw Count = %d\n", \
                                    maxRawChCountValue, adcData.AdcClear);
    currentAlsGainLevel = alsCurrentGainLevel(ctx->ccbAlsCtx.ctxAlgAls.gain);
    AMS_DEV_INST_PRINTF(HIGH, this, "changeAlsGain(): currentAlsGainLevel = %d\n", currentAlsGainLevel);
    currentClearChRawCountValue = adcData.AdcClear;
    /*
     * Set ALS gain to the next smaller gain
     * This section is to handle the Analog gain
     * In case of Analog saturation reduce the ALS
     * gain.
     * If the clear channel raw count is more than
     * 80% of the max raw count, decrease the ALS gain.
     */
    /* if((alsData->statusReg & ASAT) || (currentClearChRawCountValue > ((maxRawChCountValue * ccbCtx->initData.configData.clearChUpperThreshold) / 100))) */
    if(alsData->statusReg & ASAT)
    {
        AMS_DEV_INST_PRINTF(HIGH, this, "changeAlsGain(): ASAT true\n");
        need_to_decrement = true;
    }
    if (currentClearChRawCountValue > ((maxRawChCountValue * ccbCtx->initData.configData.clearChUpperThreshold) / 100))
    {
        AMS_DEV_INST_PRINTF(HIGH, this, "changeAlsGain(): current raw %d > (max * clearChUpperThreshold) / 1000) %d\n",
                            currentClearChRawCountValue, ((maxRawChCountValue * ccbCtx->initData.configData.clearChUpperThreshold) / 100) );
        need_to_decrement = true;
    }
    if (need_to_decrement == true)
    {
        if(currentAlsGainLevel != MIN_ALS_GAIN_LEVEL){
            setAlsGainLevel = currentAlsGainLevel - 1;
        }
        else{
            setAlsGainLevel = currentAlsGainLevel;
        }
        AMS_DEV_INST_PRINTF(HIGH, this, "changeAlsGain(): currentAlsGainLevel = %d, setAlsGainLevel = %d\n", currentAlsGainLevel, setAlsGainLevel);
        ctx->ccbAlsCtx.ctxAlgAls.gain = alsGain_conversion[setAlsGainLevel];
        AMS_DISABLE_ALS();
        AMS_SET_ALS_GAIN(ctx->ccbAlsCtx.ctxAlgAls.gain);
        AMS_REENABLE_ALS();
        currentAlsGainLevel = alsCurrentGainLevel(ctx->ccbAlsCtx.ctxAlgAls.gain);
        previousAlsGainLevel = currentAlsGainLevel;
        ctx->updateAvailable |= (1 << AMS_ALS_GAIN_CHANGED);
        state->als_stream_config_timestamp = sns_get_system_time();
        state->als_oc_config_timestamp = sns_get_system_time();
        als_update_statics(als_ctx);
        return alsGain_conversion[currentAlsGainLevel];
    }
    /*
     * In case of low light the ALS Gain has to be increased.
     * The Clear channel data is used to increase the ALS gain
     * level.
     * if the Clear channel data reduces to 10% of its maximum
     * value then increase the ALS gain till the Clear channel
     * data increases upto 80% of it maximum value.
     */
    AMS_DEV_INST_PRINTF(HIGH, this, "changeAlsGain(): ccbCtx->initData.configData.clearChLowerThreshold = %d\n", ccbCtx->initData.configData.clearChLowerThreshold);
    if(currentClearChRawCountValue < ((maxRawChCountValue * ccbCtx->initData.configData.clearChLowerThreshold) / 100)){
        if(currentAlsGainLevel < MAX_ALS_GAIN_LEVEL){
            setAlsGainLevel = currentAlsGainLevel + 1;
        }
        else{
            setAlsGainLevel = currentAlsGainLevel;
        }
        AMS_DEV_INST_PRINTF(HIGH, this, "changeAlsGain(): currentAlsGainLevel = %d, setAlsGainLevel = %d\n", currentAlsGainLevel, setAlsGainLevel);
        ctx->ccbAlsCtx.ctxAlgAls.gain = alsGain_conversion[setAlsGainLevel];
        AMS_DISABLE_ALS();
        AMS_SET_ALS_GAIN(ctx->ccbAlsCtx.ctxAlgAls.gain);
        AMS_REENABLE_ALS();
        currentAlsGainLevel = alsCurrentGainLevel(ctx->ccbAlsCtx.ctxAlgAls.gain);
        previousAlsGainLevel = currentAlsGainLevel;
        ctx->updateAvailable |= (1 << AMS_ALS_GAIN_CHANGED);
        state->als_stream_config_timestamp = sns_get_system_time();
        state->als_oc_config_timestamp = sns_get_system_time();
        als_update_statics(als_ctx);
        return alsGain_conversion[currentAlsGainLevel];
    }

    ams_getByte(ctx->portHndl,  DEVREG_CFG1, &again);
    AMS_DEV_INST_PRINTF(HIGH, this, "changeAlsGain AGAIN value = 0x%02x" , again);
    return alsGain_conversion[currentAlsGainLevel];
}

static void ccb_alsGetResult(void * dcbCtx, ams_ccb_als_result_t * exportData){
    ams_ccb_als_ctx_t * ccbCtx = &((ams_deviceCtx_t*)dcbCtx)->ccbAlsCtx;

    /* export data */
    exportData->mLux = ccbCtx->ctxAlgAls.results.mLux;
    exportData->colorTemp = ccbCtx->ctxAlgAls.results.CCT;
    exportData->clear = ccbCtx->ctxAlgAls.results.irrClear;
    exportData->red = ccbCtx->ctxAlgAls.results.irrRed;
    exportData->ir = ccbCtx->ctxAlgAls.results.IR;
    exportData->rawClear = ccbCtx->ctxAlgAls.results.rawClear;
    exportData->rawIR = ccbCtx->ctxAlgAls.results.rawIR;
    exportData->quality = ccbCtx->ctxAlgAls.results.quality;
}

static void als_getDefaultCalibrationData(ams_ccb_als_calibration_t * data)
{
    if (data != NULL) {
        data->D_factor  = AMS_ALS_D_FACTOR;
        data->L0_factor = AMS_ALS_L0_FACTOR;
        data->L1_factor = AMS_ALS_L1_FACTOR;
        data->L2_factor = AMS_ALS_L2_FACTOR;
        data->L3_factor = AMS_ALS_L3_FACTOR;
        data->Time_base = AMS_ALS_TIMEBASE;
        data->thresholdLow = AMS_ALS_THRESHOLD_LOW;
        data->thresholdHigh = AMS_ALS_THRESHOLD_HIGH;
        data->calibrationFactor = 1000;
        data->luxTarget = CONFIG_ALS_CAL_TARGET;
        data->luxTargetError = CONFIG_ALS_CAL_TARGET_TOLERANCE;
    }
}

static void ccb_alsInfo(ams_ccb_als_info_t* infoData){
    if (infoData != NULL) {
        infoData->algName = "ALS";
        infoData->defaultCalibrationData.calibrationFactor = 1000;
        infoData->defaultCalibrationData.luxTarget = CONFIG_ALS_CAL_TARGET;
        infoData->defaultCalibrationData.luxTargetError = CONFIG_ALS_CAL_TARGET_TOLERANCE;
        als_getDefaultCalibrationData(&infoData->defaultCalibrationData);
    }
}

static uint32_t AMS_NORMALISE_16(uint32_t data, uint32_t dataSize);
static uint32_t AMS_NORMALISE_16(uint32_t data, uint32_t dataSize){
    uint32_t temp = (16 - dataSize);
    return (data << temp);
}

static uint32_t AMS_DENORMALISE_16(uint32_t data, uint32_t dataSize);
static uint32_t AMS_DENORMALISE_16(uint32_t data, uint32_t dataSize){
    uint32_t temp = (16 - dataSize);
    return (data >> temp);
}

static void ccb_proximityInit(void *dcbCtx, ams_ccb_proximity_init_t *initData){
    ams_deviceCtx_t *ctx = (ams_deviceCtx_t *)dcbCtx;
    ams_ccb_proximity_ctx_t *ccbCtx = &((ams_deviceCtx_t *)dcbCtx)->ccbProxCtx;

    ccbCtx->nearBy = false;
    ccbCtx->prox_is_running = false;
    if (ccbCtx->initData.numberAdcBits > 8){
        ccbCtx->sensitivity = 3;
    } else {
        ccbCtx->sensitivity = 0;
    }
    if (initData){
         sns_memscpy(&ccbCtx->initData, sizeof(ccbCtx->initData), initData, sizeof(ams_ccb_proximity_init_t));
    } else {
        ccbCtx->initData.adcThreshold = AMS_PRX_DEFAULT_ADC_THRESHOLD;
        ccbCtx->initData.nearByHardThreshold = AMS_PRX_DEFAULT_NEARBY_HARD_THRESHOLD;
        ccbCtx->initData.nearByThresholdHysteresis = AMS_PRX_DEFAULT_THRESHOLD_HYSTERESIS;
        ccbCtx->initData.dynamicMode = true;
        ccbCtx->initData.numberAdcBits = 16;
        ccbCtx->initData.ptime_us = AMS_PRX_DEFAULT_PTIME;
        ccbCtx->initData.pwTime_us = AMS_DEFAULT_WAIT_TIME;
    }

    ccbCtx->adcThreshold = ccbCtx->initData.adcThreshold;
    AMS_SET_PROX_TIME(ccbCtx->initData.ptime_us);
    AMS_SET_PROX_PERS(ccbCtx->initData.pPers);
    AMS_SET_PROX_PULSE_LEN(ccbCtx->initData.pPulseLen);
    AMS_SET_PROX_PULSE_COUNT(ccbCtx->initData.pPulse);
    AMS_SET_PROX_GAIN(ccbCtx->initData.pGain);
    AMS_SET_PROX_LEDDRIVE(ccbCtx->initData.pLdrive);
    /* The below 2 values are read from the registry and updated*/
    AMS_SET_PROX_POFFSET_MAGNITUDE(ccbCtx->initData.proxOffset);
    AMS_SET_PROX_POFFSET_SIGN(ccbCtx->initData.pOffsetSign);
    /* force interrupt */
    AMS_SET_PROX_THRS_HIGH(ccbCtx->initData.pUpperThrs);
    AMS_SET_PROX_THRS_LOW(ccbCtx->initData.pLowerThrs);

    if (ccbCtx->initData.dynamicMode == true) {
        AMS_SET_CAL_PROX_AUTO_OFFSET_ADJUST();
    }
    if(ccbCtx->initData.pwTime_us >= AMS_MIN_WAIT_TIME)
    {
        uint8_t temp_i = (ccbCtx->initData.pwTime_us/AMS_MIN_WAIT_TIME)-1;
        AMS_SET_WTIME(temp_i);
        ctx->shadowEnableReg |= WEN;
    }
    else
    {
        ctx->shadowEnableReg &= (~WEN);
    }
    ccbCtx->state = PRX_STATE_BEGIN;
    ccbCtx->wait_for_clear = false;
}

static void ccb_proximitySetConfig(void *dcbCtx, ams_ccb_proximity_config_t *configData){
    ams_ccb_proximity_ctx_t *ccbCtx = &((ams_deviceCtx_t *)dcbCtx)->ccbProxCtx;

    if (configData->sensitivity > 0){
        ccbCtx->sensitivity = configData->sensitivity;
    }
}

static bool ccb_proximityHandle(sns_sensor_instance *const this, void *dcbCtx, ams_ccb_proximity_dataSet_t *proxData){

    ams_deviceCtx_t *ctx = (ams_deviceCtx_t *)dcbCtx;
    ams_ccb_proximity_ctx_t *ccbCtx = &((ams_deviceCtx_t *)dcbCtx)->ccbProxCtx;
    uint32_t thresh;

    if(ccbCtx->wait_for_clear)
    {
        ctx->shadowEnableReg &= ~(PEN);
        ams_setEnable(ctx);
        AMS_DEV_INST_PRINTF(MED, this,"ccb_proximityHandle streaming on hold, wait_for_clear is true");
        return true;
    }

    /* The accuracy will be high unless we are in
     * state PRX_STATE_UNKNOWN or PRX_STATE_BEGIN.
     * In those cases we will set accuracy to
     * PRX_ACCURACY_UNRELIABLE. (see those cases
     * below)
     */
    ccbCtx->accuracy = PRX_ACCURACY_HIGH;
    ccbCtx->pData = proxData->pData;
    ccbCtx->pDataNorm = AMS_NORMALISE_16(proxData->pData, ccbCtx->initData.numberAdcBits);
    AMS_DEV_INST_PRINTF(HIGH, this,
                      "ccbCtx->initData.nearByHardThreshold = %d\n"
                      "ccbCtx->initData.pwTime_us = %d\n"
                      "ctx->systemCalibrationData->proxMinThreshold = %d\n"
                      "ctx->proxDefaultConfigData->pTime = %d\n"
                      "ctx->proxDefaultConfigData->pPulseLen = %d\n"
                      "ctx->proxDefaultConfigData->pPulse = %d",
                      ccbCtx->initData.nearByHardThreshold,
                      ccbCtx->initData.pwTime_us,
                      ctx->systemCalibrationData->proxMinThreshold,
                      ctx->proxDefaultConfigData->pTime,
                      ctx->proxDefaultConfigData->pPulseLen,
                      ctx->proxDefaultConfigData->pPulse);

    AMS_DEV_INST_PRINTF(HIGH, this, "ctx->proxDefaultConfigData->pGain = %d\n"
                      "ctx->proxDefaultConfigData->pLdrive = %d\n"
                      "ctx->proxDefaultConfigData->pPers = %d\n"
                      "ctx->proxDefaultConfigData->pUpperThrs = %d\n"
                      "ctx->proxDefaultConfigData->pLowerThrs = %d",
                      ctx->proxDefaultConfigData->pGain,
                      ctx->proxDefaultConfigData->pLdrive,
                      ctx->proxDefaultConfigData->pPers,
                      ctx->proxDefaultConfigData->pUpperThrs,
                      ctx->proxDefaultConfigData->pLowerThrs);

     AMS_DEV_INST_PRINTF(HIGH, this,
                      "ctx->proxDefaultConfigData->pOffsetSign = %d\n"
                      "ctx->proxDefaultConfigData->pProxOffset = %d\n"
                      "ctx->proxDefaultConfigData->pBinSearch = %d",
                      ctx->proxDefaultConfigData->pOffsetSign,
                      ctx->proxDefaultConfigData->proxOffset,
                      ctx->proxDefaultConfigData->pBinSearch);
    AMS_DEV_INST_PRINTF(HIGH, this,
                        "ccbCtx->initData.proxOffset = %d\n"
                        "ccbCtx->initData.pOffsetSign = %d\n"
                        ,ccbCtx->initData.proxOffset
                        ,ccbCtx->initData.pOffsetSign);

    switch (ccbCtx->state)
    {
    case PRX_STATE_FACTORY_CALIBRATION:

        AMS_DEV_INST_PRINTF(HIGH, this, "PRX_STATE_FACTORY_CALIBRATION done\n");
        ccbCtx->initData.calibrate = false;
        ccbCtx->nearBy = true;
        /* signal to DCB that calibration is completed */
        ctx->updateAvailable = AMS_CALIBRATION_DONE;
        thresh = ccbCtx->pDataNorm + (ccbCtx->pDataNorm>>2);
        thresh = (thresh > 65024) ? 65024 : thresh;
        thresh = (thresh < ctx->systemCalibrationData->proxMinThreshold) ?
                    ctx->systemCalibrationData->proxMinThreshold : thresh;
        ccbCtx->initData.nearByThresholdHysteresis = thresh;
        thresh = ccbCtx->pDataNorm;
        thresh = (thresh > 65024) ? 65024 : thresh;
        thresh = (thresh < ctx->systemCalibrationData->proxMinThreshold) ?
                    ctx->systemCalibrationData->proxMinThreshold : thresh;
        ctx->systemCalibrationData->proxHardThreshold = (uint16_t)thresh;
        ccbCtx->initData.nearByHardThreshold = (uint16_t)thresh;
        ccbCtx->state = PRX_STATE_STATIC_NO_DETECT;
        AMS_DEV_INST_PRINTF(HIGH, this, "Setting PRX_STATE to farAway\n");
        AMS_SET_PROX_THRS_LOW_HIGH(
           0x00,
           AMS_DENORMALISE_16(ccbCtx->initData.nearByThresholdHysteresis, ccbCtx->initData.numberAdcBits));
        ccbCtx->nearBy = false;
        ctx->updateAvailable |= (1 << AMS_PROXIMITY_SENSOR);
        //remove this. Going back to state_begin will trigger another interrupt and fail test case PRX_733
        //if (ccbCtx->prox_is_running == true) {
        //    ccbCtx->state = PRX_STATE_BEGIN;
        //}
        break;
    case PRX_STATE_FIRST_MEASURE:
        AMS_DEV_INST_PRINTF(MED, this, "PRX_STATE_FIRST_MEASURE, ccbCtx->initData.nearByHardThreshold = %d\n",
            ccbCtx->initData.nearByHardThreshold);
        //initialize nearByThresholdHysteresis
        thresh = ccbCtx->initData.nearByHardThreshold + (ccbCtx->initData.nearByHardThreshold>>2);
        thresh = (thresh > 65024) ? 65024 : thresh;
        thresh = (thresh < ctx->systemCalibrationData->proxMinThreshold) ?
                    ctx->systemCalibrationData->proxMinThreshold : thresh;
        ccbCtx->initData.nearByThresholdHysteresis = thresh;
        if (ccbCtx->pDataNorm > ccbCtx->initData.nearByThresholdHysteresis) {
            ccbCtx->state = PRX_STATE_STATIC_DETECT;
            AMS_DEV_INST_PRINTF(HIGH, this, "Setting PRX_STATE to nearBy\n");
            AMS_SET_PROX_THRS_LOW_HIGH(
               AMS_DENORMALISE_16((ccbCtx->initData.nearByHardThreshold - (ccbCtx->initData.nearByHardThreshold >> 2)), ccbCtx->initData.numberAdcBits),
               0xff);
            ccbCtx->nearBy = true;
            ctx->updateAvailable |= (1 << AMS_PROXIMITY_SENSOR);
        } else {
            ccbCtx->state = PRX_STATE_STATIC_NO_DETECT;
            AMS_DEV_INST_PRINTF(HIGH, this, "Setting PRX_STATE farAway\n");
            AMS_SET_PROX_THRS_LOW_HIGH(
               0x0000,
               AMS_DENORMALISE_16(ccbCtx->initData.nearByThresholdHysteresis, ccbCtx->initData.numberAdcBits));
            ccbCtx->nearBy = false;
            ctx->updateAvailable |= (1 << AMS_PROXIMITY_SENSOR);
        }
        break;
    case PRX_STATE_STATIC_NO_DETECT:
        AMS_DEV_INST_PRINTF(MED, this, "PRX_STATE_STATIC_NO_DETECT\n");
        if (ccbCtx->pDataNorm > ccbCtx->initData.nearByThresholdHysteresis) {
            ccbCtx->state = PRX_STATE_STATIC_DETECT;
            AMS_DEV_INST_PRINTF(HIGH, this, "Changing PRX_STATE to nearBy\n");
            AMS_SET_PROX_THRS_LOW_HIGH(
                AMS_DENORMALISE_16((ccbCtx->initData.nearByHardThreshold - (ccbCtx->initData.nearByHardThreshold >> 2)), ccbCtx->initData.numberAdcBits),
                0xff);
            ccbCtx->nearBy = true;
            ctx->updateAvailable |= (1 << AMS_PROXIMITY_SENSOR);
        }
        break;
    case PRX_STATE_STATIC_DETECT:
        AMS_DEV_INST_PRINTF(MED, this, "PRX_STATE_STATIC_DETECT\n");
        if (ccbCtx->pDataNorm < (ccbCtx->initData.nearByHardThreshold - (ccbCtx->initData.nearByHardThreshold >> 2))) {
            ccbCtx->state = PRX_STATE_STATIC_NO_DETECT;
            AMS_DEV_INST_PRINTF(HIGH, this, "Changing PRX_STATE to farAway\n");
            AMS_SET_PROX_THRS_LOW_HIGH(
               0x0000,
               AMS_DENORMALISE_16(ccbCtx->initData.nearByThresholdHysteresis, ccbCtx->initData.numberAdcBits));
            ccbCtx->nearBy = false;
            ctx->updateAvailable |= (1 << AMS_PROXIMITY_SENSOR);
        }
        break;
    default:
    case PRX_STATE_UNKNOWN:
    case PRX_STATE_BEGIN:
        AMS_DEV_INST_PRINTF(HIGH, this, "PRX_STATE_BEGIN\n");

        ccbCtx->prox_is_running = true;
        ccbCtx->nearBy = false;
        ccbCtx->accuracy = PRX_ACCURACY_UNRELIABLE;
        ccbCtx->pDataAve = 0;
        ccbCtx->ave_index = 0;
        ccbCtx->pDataLow = -1;
        ccbCtx->pDataMax = 0;
        ccbCtx->pDataNormLast = 0xffff;
        ccbCtx->pDataNormTrigger = 0xffff;
        thresh = ccbCtx->initData.nearByHardThreshold + (ccbCtx->initData.nearByHardThreshold>>2);
        thresh = (thresh > 0xfe00) ? 0xfe00 : thresh;
        ccbCtx->initData.nearByThresholdHysteresis = thresh;
        AMS_SET_PROX_PERS(ctx->proxDefaultConfigData->pPers);
        AMS_SET_PROX_PULSE_LEN(ctx->proxDefaultConfigData->pPulseLen);
        AMS_SET_PROX_PULSE_COUNT(ctx->proxDefaultConfigData->pPulse);

        if (true == ccbCtx->initData.proxPoffsetCalibrate) {
            AMS_DEV_INST_PRINTF(HIGH, this, "PRX_STATE_HARDWARE_CALIBRATION is true\n");
            /* setup registers for HW calibration */
            ctx->shadowEnableReg &= ~(PEN | AEN);
            AMS_REENABLE();

            AMS_SET_CAL_BINSRCH(ctx->proxDefaultConfigData->pBinSearch);
            ctx->shadowIntenabReg |= CIEN;
            ams_setField(ctx->portHndl, DEVREG_CALIB, MASK_LOW, MASK_OFFSET_CALIB | MASK_ELECT_CAL); // Ensure CAL disabled and NOT just electrical cal

            /* Trigger calibration sequence */
            /*Clear ALL Interrupts */
            ams_setByte(ctx->portHndl, DEVREG_STATUS, 0xFF);
            ams_setField(ctx->portHndl, DEVREG_CALIB, MASK_HIGH, MASK_OFFSET_CALIB);
            ccbCtx->state = PRX_STATE_HARDWARE_CALIBRATION;
        }
        else{
            AMS_DEV_INST_PRINTF(HIGH, this, "PRX_STATE_HARDWARE_CALIBRATION is false\n");
        }

#ifdef CONFIG_AMS_PROX_STATIC_MODE
        AMS_DEV_INST_PRINTF(HIGH, this, "CONFIG_AMS_PROX_STATIC_MODE\n");
        if (ccbCtx->initData.calibrate) {
            ccbCtx->state = PRX_STATE_FACTORY_CALIBRATION;

            ctx->shadowEnableReg &= ~(PEN);
            ams_setEnable(ctx);
            AMS_SET_PROX_THRS_LOW_HIGH(0xff, 0x00);
            ctx->shadowEnableReg |= (PEN);
            ams_setEnable(ctx);
            ccbCtx->nearBy = false;
        }
#endif

        if (ccbCtx->initData.dynamicMode == true){
            /* setup registers for HW calibration */
            ctx->shadowEnableReg &= ~(PEN | AEN);
            AMS_REENABLE();

            AMS_SET_CAL_BINSRCH(ctx->proxDefaultConfigData->pBinSearch);
            ctx->shadowIntenabReg |= CIEN;
            ams_setField(ctx->portHndl, DEVREG_CALIB, MASK_LOW, MASK_OFFSET_CALIB | MASK_ELECT_CAL); // Ensure CAL disabled and NOT just electrical cal

            /* Trigger calibration sequence */
            /*Clear ALL Interrupts */
            ams_setByte(ctx->portHndl, DEVREG_STATUS, 0xFF);
            ams_setField(ctx->portHndl, DEVREG_CALIB, MASK_HIGH, MASK_OFFSET_CALIB);
            ccbCtx->state = PRX_STATE_CAL;
        } else {
            if((ccbCtx->initData.calibrate == false) && (ccbCtx->initData.proxPoffsetCalibrate == false)){
                AMS_DEV_INST_PRINTF(HIGH, this, "No calibration, go to PRX_STATE_FIRST_MEASURE \n");
                ccbCtx->state = PRX_STATE_FIRST_MEASURE;
                ctx->shadowEnableReg &= ~(PEN);
                ams_setEnable(ctx);
                AMS_SET_PROX_THRS_LOW_HIGH(0xff, 0x00);
                ctx->shadowEnableReg |= (PEN);
                ams_setEnable(ctx);
                ccbCtx->nearBy = false;
            }
        }
        break;

    case PRX_STATE_HARDWARE_CALIBRATION:
            ccbCtx->initData.proxPoffsetCalibrate = false;
            AMS_DEV_INST_PRINTF(HIGH, this, "Entered PRX_STATE_HARDWARE_CALIBRATION state\n");
            if (proxData->statusReg & MASK_CINT){
                AMS_DEV_INST_PRINTF(HIGH, this, "PRX_STATE_HARDWARE_CALIBRATION - have CINT\n");
                uint8_t offset;
                uint8_t offsetSign;
                sns_port_vector port_vec[2];

                /* signal to DCB that calibration is completed */
                ctx->updateAvailable = AMS_CALIBRATION_DONE;
                port_vec[0].is_write = false;
                port_vec[0].reg_addr = deviceRegisterDefinition[DEVREG_POFFSET_L].address;
                port_vec[0].bytes    = 1;
                port_vec[0].buffer   = &offset;

                port_vec[1].is_write = false;
                port_vec[1].reg_addr = deviceRegisterDefinition[DEVREG_POFFSET_H].address;
                port_vec[1].bytes    = 1;
                port_vec[1].buffer   = &offsetSign;

                ams_rwVec(ctx->portHndl, port_vec, 2);

                AMS_DEV_INST_PRINTF(HIGH, this, "ccbCtx->proxOffset = %d", offset);
                ctx->systemCalibrationData->proxOffset = offset;
                ccbCtx->initData.proxOffset = offset;
                AMS_DEV_INST_PRINTF(HIGH, this, "ccbCtx->proxOffsetSign = %d", offsetSign);
                ctx->systemCalibrationData->proxOffsetSign = offsetSign;
                ccbCtx->initData.pOffsetSign = offsetSign;

                offsetSign &= 0x01;
                ams_setField(ctx->portHndl, DEVREG_CALIB, MASK_LOW, MASK_OFFSET_CALIB);
                ccbCtx->proxOffset = offset * ((offsetSign & 0x01) ? -1 : 1);
                AMS_DEV_INST_PRINTF(HIGH, this, "ccbCtx->proxOffset = %d", ccbCtx->proxOffset);

                /* Asymmetrical semi-dynamic mode, if HW supports it:
                * auto adjust to boost signal whenever PDATA goes too low.
                * No ability to adjust in the opposite direction, but if
                * calibration performed while FOV is empty, opposite
                * direction won't be needed.
                */
                AMS_SET_CAL_PROX_AUTO_OFFSET_ADJUST();
                AMS_ENABLE_PROX_INT();
                ctx->shadowIntenabReg &= ~CIEN;
                ctx->shadowEnableReg |= (PEN | PON);
                AMS_SET_PROX_THRS_LOW_HIGH(0xff, 0x00);
                ams_setEnable(ctx);
            }
            else {
                AMS_DEV_INST_PRINTF(HIGH, this, "PRX_STATE_HARDWARE_CALIBRATION - no CINT\n");
            }
        break;
    case PRX_STATE_CAL:
        if (proxData->statusReg & MASK_CINT){
            AMS_DEV_INST_PRINTF(HIGH, this, "PRX_STATE_CAL - have CINT\n");
            uint8_t offset;
            uint8_t offsetSign;
            sns_port_vector port_vec[2];

            port_vec[0].is_write = false;
            port_vec[0].reg_addr = deviceRegisterDefinition[DEVREG_POFFSET_L].address;
            port_vec[0].bytes    = 1;
            port_vec[0].buffer   = &offset;

            port_vec[1].is_write = false;
            port_vec[1].reg_addr = deviceRegisterDefinition[DEVREG_POFFSET_H].address;
            port_vec[1].bytes    = 1;
            port_vec[1].buffer   = &offsetSign;

            ams_rwVec(ctx->portHndl, port_vec, 2);

            offsetSign &= 0x01;
            ams_setField(ctx->portHndl, DEVREG_CALIB, MASK_LOW, MASK_OFFSET_CALIB);
            ccbCtx->proxOffset = offset * ((offsetSign & 0x01) ? -1 : 1);
            AMS_DEV_INST_PRINTF(HIGH, this, "ccbCtx->proxOffset = %d", ccbCtx->proxOffset);

            /* Asymmetrical semi-dynamic mode, if HW supports it:
            * auto adjust to boost signal whenever PDATA goes too low.
            * No ability to adjust in the opposite direction, but if
            * calibration performed while FOV is empty, opposite
            * direction won't be needed.
            */
            AMS_SET_CAL_PROX_AUTO_OFFSET_ADJUST();
            AMS_ENABLE_PROX_INT();
            ctx->shadowIntenabReg &= ~CIEN;
            ctx->shadowEnableReg |= (PEN | PON);

            if (ctx->mode & MODE_ALS_ALL){
                ctx->shadowEnableReg |= AEN;
            }

            if (ccbCtx->initData.calibrate) {
                AMS_DEV_INST_PRINTF(HIGH, this, "CALIB is TRUE");
                ccbCtx->state = PRX_STATE_FACTORY_CALIBRATION;
                ctx->shadowEnableReg &= ~(PEN);
                ams_setEnable(ctx);
                AMS_CLR_PROX_INT();
                AMS_SET_PROX_THRS_LOW_HIGH(0xff, 0x00);
                ctx->shadowEnableReg |= (PEN);
                ams_setEnable(ctx);
            } else {
                /* HW calibration completed */
                ccbCtx->state = PRX_STATE_FIRST_MEASURE;
                ctx->shadowEnableReg &= ~(PEN);
                ams_setEnable(ctx);
                AMS_SET_PROX_THRS_LOW_HIGH(
                   (1 << ccbCtx->initData.numberAdcBits) - 1,
                   1 << ccbCtx->initData.numberAdcBits);
                ctx->shadowEnableReg |= (PEN);
                ams_setEnable(ctx);
                ccbCtx->nearBy = false;
            }
        } else {
            AMS_DEV_INST_PRINTF(HIGH, this, "PRX_STATE_CAL - no CINT\n");
        }
        break;
    }

    if ((ccbCtx->state == PRX_STATE_STATIC_NO_DETECT) || (ccbCtx->state == PRX_STATE_STATIC_DETECT)){
        AMS_DEV_INST_PRINTF(HIGH, this, "pData: 0x%04x pDataNorm: %d nearBy: %d\n",
                                     ccbCtx->pData, ccbCtx->pDataNorm, ccbCtx->nearBy);
    }
    return false;
}

static void ccb_proximityGetResult(void *dcbCtx, ams_ccb_proximity_result_t *result){
    ams_ccb_proximity_ctx_t *ccbCtx = &((ams_deviceCtx_t *)dcbCtx)->ccbProxCtx;

    /* export proximity data */
    result->nearBy = ccbCtx->nearBy;
    result->proximity = ccbCtx->pData;
    result->proxOffset = ccbCtx->proxOffset;
    result->accuracy = ccbCtx->accuracy;
}

static void ccb_proximityInfo(ams_ccb_proximity_info_t* infoData){

    infoData->algName = "PROX";
    infoData->defaultCalibrationData.hardThreshold = AMS_PRX_DEFAULT_NEARBY_HARD_THRESHOLD;
}


#ifdef ENABLE_DEBUG
static void inst_exit_island(sns_sensor_instance *this)
{
    sns_service_manager *smgr = this->cb->get_service_manager(this);
    sns_island_service *island_service =
            (sns_island_service *)smgr->get_service(smgr, SNS_ISLAND_SERVICE);

    island_service->api->sensor_instance_island_exit(island_service, this);
}
#endif

void tmd2725_restore_enable_settings(ams_deviceCtx_t *devCtx)
{
  /* Restore the previous enable and intenab register settings */
  devCtx->shadowEnableReg = devCtx->shadowEnableRegPrevious;
  devCtx->shadowIntenabReg = devCtx->shadowIntenabRegPrevious;

  /* Self test is done so disable everything.
   * when the test client is removed the old
   * settings will take effect
   */
  ams_setByte(devCtx->portHndl, DEVREG_ENABLE, 0x0);
  ams_setByte(devCtx->portHndl, DEVREG_INTENAB, 0x0);

}
