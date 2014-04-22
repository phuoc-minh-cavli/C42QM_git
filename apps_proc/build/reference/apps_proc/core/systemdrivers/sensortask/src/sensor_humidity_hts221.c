/*=============================================================================
  Copyright (c) 2016  by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

#include "sensor_humidity_hts221.h"
#include "sensorutil.h"

// device properties (do NOT modify)

#define SLAVE_ADDR         0x5F

// HTS221 register addresses
#define REG_WHO_AM_I       0x0F
#define REG_CTRL_REG1      0x20
#define REG_HUMIDITY_OUT_L 0x28
#define REG_HUMIDITY_OUT_H 0x29
#define REG_TEMP_OUT_L     0x2A
#define REG_TEMP_OUT_H     0x2B
#define REG_T0_DEGC_X8     0x32
#define REG_T1_DEGC_X8     0x33
#define REG_T1_T0_MSB      0x35
#define REG_T0_OUT_L       0x3C
#define REG_T0_OUT_H       0x3D
#define REG_T1_OUT_L       0x3E
#define REG_T1_OUT_H       0x3F

#define REG_WHO_AM_I_VAL   0xBC

// Global variables
static void *i2c_handle;
static int t_slope, t_b;
static int t_t0, t_t1, cal_t0, cal_t1;
static uint16 cal_t0a, cal_t0b, cal_t1a, cal_t1b;

/**
   Start up HTS221 humidity + temp sensor
 
   return: DAL_SUCCESS if successful 
*/
int hts221_init() {
    uint8 reg_val;

    //ULogFront_RealTimePrintf(sensortask_ulog, 0, "In hts221_init()");
    sensorutil_i2c_init_handle(&i2c_handle, SLAVE_ADDR);

    sensorutil_i2c_read(i2c_handle, REG_WHO_AM_I, &reg_val);
    //ULogFront_RealTimePrintf(sensortask_ulog, 2, "hts221: WHOAMI (0x%x) returned 0x%x", REG_WHO_AM_I, reg_val);

    if (reg_val != REG_WHO_AM_I_VAL) {
        //ALI:
        //ULogFront_RealTimePrintf(sensortask_ulog, 1, "hts221: unexpected WHOAMI 0x%x", reg_val);
        return DAL_ERROR;
    }

    sensorutil_i2c_read(i2c_handle, REG_T0_OUT_L, &reg_val);
    t_t0 = reg_val;
    sensorutil_i2c_read(i2c_handle, REG_T0_OUT_H, &reg_val);
    t_t0 |= reg_val << 8;

    sensorutil_i2c_read(i2c_handle, REG_T1_OUT_L, &reg_val);
    t_t1 = reg_val;
    sensorutil_i2c_read(i2c_handle, REG_T1_OUT_H, &reg_val);
    t_t1 |= reg_val << 8;

    sensorutil_i2c_read(i2c_handle, REG_T0_DEGC_X8, &reg_val);
    cal_t0a = reg_val;

    sensorutil_i2c_read(i2c_handle, REG_T1_DEGC_X8, &reg_val);
    cal_t1a = reg_val;

    sensorutil_i2c_read(i2c_handle, REG_T1_T0_MSB, &reg_val);
    cal_t0b = reg_val & 0x3;
    cal_t1b = reg_val & 0xC;
    cal_t1b = cal_t1b >> 2;

    cal_t0 = (cal_t0b << 8) | cal_t0a;
    cal_t1 = (cal_t1b << 8) | cal_t1a;

    t_slope = ((cal_t1 - cal_t0) * 8000) / (t_t1 - t_t0);
    t_b = (((t_t1 * cal_t0) - (t_t0 * cal_t1)) * 1000) / (t_t1 - t_t0);
    t_b = t_b * 8;

    sensorutil_i2c_write(i2c_handle, REG_CTRL_REG1, 0x0); // Power off
    sensorutil_i2c_write(i2c_handle, REG_CTRL_REG1, 0x80 | 0x1); // ODR @ 1Hz

    return DAL_SUCCESS;
}

/**
   Shutdown HTS221 humidity + temp sensor
 
   return: DAL_SUCCESS if successful 
*/
int hts221_shtudown() {
    //ULogFront_RealTimePrintf(sensortask_ulog, 0, "In hts221_shutdown()");

    sensorutil_i2c_write(i2c_handle, REG_CTRL_REG1, 0x0); // Power off

    return DAL_SUCCESS;
}

/**
   Read current temperature value (in degrees Celcius)
 
   return: Temperature in degrees Celcius (floating point) 
*/
float hts221_get_temp() {
    uint8 reg_val;
    uint16 temp;
    float temp_f;

    //ULogFront_RealTimePrintf(sensortask_ulog, 0, "In hts221_get_temp()");

    sensorutil_i2c_read(i2c_handle, REG_TEMP_OUT_L, &reg_val);
    temp = reg_val;
    sensorutil_i2c_read(i2c_handle, REG_TEMP_OUT_H, &reg_val);
    temp |= reg_val << 8;

    temp = ((t_slope * temp) + t_b) >> 6;
    temp_f = (float)temp/1000;

    return temp_f;
}

/**
   Read relative humidity (\%rH value between 0.0 and 100.0)

   This function is optional and may not be supported in the 
   initial driver version. Please check driver release notes 
   prior to using. 
 
   return: Relative humidity (\%rH value between 0.0 and 100.0) 
*/
float hts221_get_humidity() { // optional
    uint8 reg_val;
    uint16 hum;

    //ULogFront_RealTimePrintf(sensortask_ulog, 0, "In hts221_get_humidity()");

    /*
    sensorutil_i2c_read(i2c_handle, REG_HUMIDITY_OUT_L, &reg_val);
    hum = reg_val;
    sensorutil_i2c_read(i2c_handle, REG_HUMIDITY_OUT_H, &reg_val);
    hum |= reg_val << 8;

    hum = ((h_slope * hum) + h_b) >>6;

    ULogFront_RealTimePrintf(sensortask_ulog, 1, "hts221: humidity is 0x%x units", hum);
    */

    //ALI:
    //ULogFront_RealTimePrintf(sensortask_ulog, 0, "hts221: humidity not supported");

    return 0;
}
