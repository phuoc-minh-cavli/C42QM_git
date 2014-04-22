/*=============================================================================
  Copyright (c) 2016  by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

#include "sensor_als_tmd2772.h"
#include "sensorutil.h"
//ALI:
//#include "sensortask.h"
#include "busywait.h"

// driver configuration parameters (modify as needed)

//for testing on MTP
//#define INT_GPIO          24

// for testing on thething dev board
#define INT_GPIO            74
#define EXT_LDO_ENABLE_GPIO 37  // GPIO to enable external 2.8 LDO on dev board

#define A_TIME_MS      (float)2.73
#define A_GAIN_X       2


// device properties (do NOT modify)

#define SLAVE_ADDR     0x39

// TMD2772 device registers
#define CMD_BASE       0x80
#define CMD_SPL_FN     0x60
#define REG_ENABLE     0x00
#define REG_AILTL      0x04
#define REG_AILTH      0x05
#define REG_AIHTL      0x06
#define REG_AIHTH      0x07
#define REG_PERS       0x0C
#define REG_GAIN       0x0F
#define REG_REVISION   0x11
#define REG_CHIPID     0x12
#define REG_STATUS     0x13
#define REG_C0DATA     0x14
#define REG_C0DATAH    0x15
#define REG_C1DATA     0x16
#define REG_C1DATAH    0x17

// Global variables
static void* i2c_handle;
static sensorutil_gpio_event_handler_type gpio_event_handler;

void tmd2772_light_event_handler(void);

/**
   Start up ambient light sensor 
 
   return: DAL_SUCCESS if successful
*/
int tmd2772_init() {
    int result = DAL_SUCCESS;
    uint8 chip_id, chip_rev;
    int i = 0;

    //turn on GPIO to enable external 2.8 V LDO
    result = sensorutil_gpio_set_value(EXT_LDO_ENABLE_GPIO, 1);
    if (result != DAL_SUCCESS) return result;

    // wait for sensor to power-up before perfomring i2c read/writes
    busywait(10*1000); // 10 msec delay

    //ULogFront_RealTimePrintf(sensortask_ulog, 0, "tmd2772: In tmd2772_init()");
    result = sensorutil_i2c_init_handle(&i2c_handle, SLAVE_ADDR);
    if (result != DAL_SUCCESS) return result;

    // This is very reliable for resetting, based on experience. Power off (0x0) doen't always work
    result = sensorutil_i2c_write(i2c_handle, CMD_BASE | REG_ENABLE, 0x13);

    // Power off
    result = sensorutil_i2c_write(i2c_handle, CMD_BASE | REG_ENABLE, 0x0);

    result = sensorutil_i2c_read(i2c_handle, CMD_BASE | REG_REVISION, &chip_rev);
    result = sensorutil_i2c_read(i2c_handle, CMD_BASE | REG_CHIPID, &chip_id);
    if (result != DAL_SUCCESS) return result;

    //ULogFront_RealTimePrintf(sensortask_ulog, 2, "tmd2772: CHIPID 0x%x REV 0x%x", chip_id, chip_rev);

    // Set Gain for ALS
    result = sensorutil_i2c_write(i2c_handle, CMD_BASE | REG_GAIN, 0x01);

    // Set Persistence for INT. Even though we don't enable INT yet, the status reg inexplicably gets set
    // and this repeats until appropriate ALS PERS is set
    result = sensorutil_i2c_write(i2c_handle, CMD_BASE | REG_PERS, 0x04);

    // Power on + ALS enable (AEN) + WAIT enable
    result = sensorutil_i2c_write(i2c_handle, CMD_BASE | REG_ENABLE, 0x0B);

    sensorutil_register_gpio_interrupt(INT_GPIO, GPIO_INT_TRIGGER_FALLING, (sensorutil_gpio_event_handler_type)tmd2772_light_event_handler);

    return result;
}

/**
   Shutdown ambient light sensor
 
   return: DAL_SUCCESS if successful
*/
int tmd2772_shutdown() {
    int result = DAL_SUCCESS;

    // This is very reliable for resetting, based on experience. Power off (0x0) doen't always work
    result = sensorutil_i2c_write(i2c_handle, CMD_BASE | REG_ENABLE, 0x13);

    // Power off
    result = sensorutil_i2c_write(i2c_handle, CMD_BASE | REG_ENABLE, 0x0); 

    return result;
}

/** 
   Configure threshold for light events.\n
   Two luminance values (in Lux) are configured here (threshold_low, threshold_high).
   When the ambient light level (measured in Lux) falls below 
   threshold_low or goes above threshold_high, a light event will be triggered. \n
   The driver controls (via APERS) how many consecutive data points need 
   to be outside of this range before an event will be triggered.
   
   return: DAL_SUCCESS if successful
*/
int tmd2772_set_light_event_threshold_c0data(uint16 threshold_low, uint16 threshold_high) {
    int result = DAL_SUCCESS;

    //ULogFront_RealTimePrintf(sensortask_ulog, 0, "tmd2772: In tmd2772_set_light_event_threshold_c0data()");

    result = sensorutil_i2c_write(i2c_handle, CMD_BASE | REG_AILTL, threshold_low & 0xFF);
    if (result != DAL_SUCCESS) return result;

    result = sensorutil_i2c_write(i2c_handle, CMD_BASE | REG_AILTH, (threshold_low >> 8) & 0xFF);
    if (result != DAL_SUCCESS) return result;

    // Set ALS High Threshold to 0 to render it inactive
    result = sensorutil_i2c_write(i2c_handle, CMD_BASE | REG_AIHTL, threshold_high & 0xFF);
    if (result != DAL_SUCCESS) return result;

    result = sensorutil_i2c_write(i2c_handle, CMD_BASE | REG_AIHTH, (threshold_high >> 8) & 0xFF);
    if (result != DAL_SUCCESS) return result;

    //ULogFront_RealTimePrintf(sensortask_ulog, 2, "tmd2772: thresholds (0x%x, 0x%x) set correctly", threshold_low, threshold_high);

    return result;
}

/** 
   Configure threshold for light events.\n
   Two luminance values (in Lux) are configured here (threshold_low, threshold_high).
   When the ambient light level (measured in Lux) falls below 
   threshold_low or goes above threshold_high, a light event will be triggered. \n
   The driver controls (via APERS) how many consecutive data points need 
   to be outside of this range before an event will be triggered.
   
   return: DAL_SUCCESS if successful
*/
int tmd2772_set_light_event_threshold(float threshold_low, float threshold_high) {
    float cpl;
    float lux = 0.0;
    uint16 c0data_low, c0data_high;

    //ULogFront_RealTimePrintf(sensortask_ulog, 0, "tmd2772: In tmd2772_set_light_event_threshold()");

    // Calculate c0data from Illuminance
    cpl = (A_TIME_MS * A_GAIN_X)/20;
    c0data_low = (uint16)(threshold_low * cpl);
    c0data_high = (uint16)(threshold_high * cpl);

    tmd2772_set_light_event_threshold_c0data(c0data_low, c0data_high);
    return DAL_SUCCESS;
}

/**
    Enable generation of light events.
    Events are generated when ambient light level goes below threshold_low or above
    threshold_high as configured in tmd2772_set_light_event_threshold().

    return: DAL_SUCCESS if successful
*/
int tmd2772_enable_light_events() {
    int result = DAL_SUCCESS;
    uint8 reg_val;

    //ULogFront_RealTimePrintf(sensortask_ulog, 0, "tmd2772: In tmd2772_enable_light_events()");

    // Clear any stale interrupt flag
    result = sensorutil_i2c_write_nodata(i2c_handle, CMD_BASE | CMD_SPL_FN | 0x6);

    result = sensorutil_i2c_read(i2c_handle, CMD_BASE | REG_ENABLE, &reg_val);
    // Set ALS Int enable (AIEN)
    reg_val |= 0x10;
    result = sensorutil_i2c_write(i2c_handle, CMD_BASE | REG_ENABLE, reg_val);

    return result;
}

/**
    Disable light events.

    return: DAL_SUCCESS if successful
*/
int tmd2772_disable_light_events() {
    int result = DAL_SUCCESS;
    uint8 reg_val;

    //ULogFront_RealTimePrintf(sensortask_ulog, 0, "tmd2772: In tmd2772_disable_light_events()");

    result = sensorutil_i2c_read(i2c_handle, CMD_BASE | REG_ENABLE, &reg_val);
    reg_val &= 0xEF;
    result = sensorutil_i2c_write(i2c_handle, CMD_BASE | REG_ENABLE, reg_val);

    return result;
}

void tmd2772_check_status(void) {
    uint8 reg_val, status, enable;
    uint16 c0data, threshold_low, threshold_high;

    //ULogFront_RealTimePrintf(sensortask_ulog, 0, "tmd2772: In tmd2772_check_status()");

    sensorutil_i2c_read(i2c_handle, CMD_BASE | REG_STATUS, &status);

    sensorutil_i2c_read(i2c_handle, CMD_BASE | REG_C0DATA, &reg_val);
    c0data = reg_val;
    sensorutil_i2c_read(i2c_handle, CMD_BASE | REG_C0DATAH, &reg_val);
    c0data |= reg_val << 8;

    sensorutil_i2c_read(i2c_handle, CMD_BASE | REG_ENABLE, &enable);

    sensorutil_i2c_read(i2c_handle, CMD_BASE | REG_AILTL, &reg_val);
    threshold_low = reg_val;
    sensorutil_i2c_read(i2c_handle, CMD_BASE | REG_AILTH, &reg_val);
    threshold_low |= reg_val << 8;
    sensorutil_i2c_read(i2c_handle, CMD_BASE | REG_AIHTL, &reg_val);
    threshold_high = reg_val;
    sensorutil_i2c_read(i2c_handle, CMD_BASE | REG_AIHTH, &reg_val);
    threshold_high |= reg_val << 8;

    //ALI:
    //ULogFront_RealTimePrintf(sensortask_ulog, 5, "tmd2772: STATUS 0x%x, ENABLE 0x%x, C0DATA 0x%x, AILT 0x%x, AIHT 0x%x", 
    //                         status, enable, c0data, threshold_low, threshold_high);
}

/**
    Callback to be invoked when a light event is triggered

    param[in]: None

    return: None
*/
void tmd2772_light_event_handler(void) {

    //ULogFront_RealTimePrintf(sensortask_ulog, 0, "tmd2772: In tmd2772_light_event_handler()");

    // For debugging
    //tmd2772_check_status();

    if (gpio_event_handler) {
        gpio_event_handler();
    }
    
    // Clear interrupts so we can start getting these again
    sensorutil_i2c_write_nodata(i2c_handle, CMD_BASE | CMD_SPL_FN | 0x6);
}

/**
    Attach callback to be invoked when a light event is
    triggered

    param[in] handler		handler to be invoked to process
          light event

    return: None
*/
void tmd2772_attach_light_event_handler(light_event_handler_type handler) {
    gpio_event_handler = handler;
}

/**
    Get Luminance (in units of Lux)

    This function reads the raw sensor data (C0Data, C1Data) and uses parameter values 
    configured by the driver [integration time (ATIME) and ALS gain (AGAIN)] with a formula 
    specified by the TMD2772 data sheet to compute the luminance value in units of Lux.

    return: Luminance value in units of lux (floating point
    value)
*/
float tmd2772_get_lux() {
    int result = DAL_SUCCESS;
    uint8 reg_val;
    uint16 c0data;//, c1data;
    float cpl;
    float lux = 0.0;

    //ULogFront_RealTimePrintf(sensortask_ulog, 0, "tmd2772: In tmd2772_get_lux()");

    result = sensorutil_i2c_read(i2c_handle, CMD_BASE | REG_C0DATA, &reg_val);
    c0data = reg_val;
    result = sensorutil_i2c_read(i2c_handle, CMD_BASE | REG_C0DATAH, &reg_val);
    c0data |= reg_val << 8;
    if (result != DAL_SUCCESS) return 0.0;
    //ULogFront_RealTimePrintf(sensortask_ulog, 1, "tmd2772: C0DATA 0x%x", c0data);

    // Calculate Illuminance
    cpl = (A_TIME_MS * A_GAIN_X)/20;
    lux = (float)c0data/cpl;

    //ULogFront_RealTimePrintf(sensortask_ulog, 1, "tmd2772: Illuminance is %d lux", (int)lux);

    // For debugging
    //tmd2772_check_status();

    return lux;
}

