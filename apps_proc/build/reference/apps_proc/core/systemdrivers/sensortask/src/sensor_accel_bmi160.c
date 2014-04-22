/*=============================================================================
  Copyright (c) 2016  by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

#include "sensor_accel_bmi160.h"
#include "sensorutil.h" 
//ALI:
//#include "sensortask.h" 


// driver configuration parameters (modify as needed)

#define CONFIG_ANYM_DUR   2                         // (1-4) # of slope data points that need to be 
                                                    // above threshold before interrupt triggered
#define CONFIG_LATCH      BMI160_LATCH_TEMP_320ms   // how long to keep interrupt line asserted after interrupt fires
#define CONFIG_RANGE      BMI160_RANGE_2G           // operation range (+/- 2g, +/- 4g, +/- 8g)


//for testing on MTP
//#define CONFIG_INTERRUPT_GPIO_NUM     74             // GPIO number to which interrupt line from bmi160 is connected

// for testing on thething dev board
#define CONFIG_INTERRUPT_GPIO_NUM       75             // GPIO number to which interrupt line from bmi160 is connected



// device properties (do NOT modify)

//for testing on MTP (with BMI160 dev kit) (when SDO is pulled to VDDIO, device address is 0x69; see BMI160 datasheet page 90)
//#define BMI160_SLAVE_ADDR     0x69

//for testing on thething dev board (when SDO is pulled to gnd, device address is 0x68; see BMI160 datasheet page 90)
#define BMI160_SLAVE_ADDR     0x68

// BMI160 register addresses
#define BMI160_REG_CHIP_ID      0x00

#define BMI160_REG_DATA_14      0x12
#define BMI160_REG_DATA_15      0x13
#define BMI160_REG_DATA_16      0x14
#define BMI160_REG_DATA_17      0x15
#define BMI160_REG_DATA_18      0x16
#define BMI160_REG_DATA_19      0x17

#define BMI160_REG_INT_STATUS_0 0x1C
#define BMI160_REG_INT_STATUS_1 0x1D
#define BMI160_REG_INT_STATUS_2 0x1E

#define BMI160_REG_ACC_RANGE    0x41

#define BMI160_REG_INT_EN_0     0x50
#define BMI160_REG_IN_OUT_CTRL  0x53
#define BMI160_REG_INT_LATCH    0x54
#define BMI160_REG_INT_MAP_0    0x55
#define BMI160_REG_INT_MAP_1    0x56
#define BMI160_REG_INT_MAP_2    0x57
#define BMI160_REG_INT_MOTION_0 0x5f
#define BMI160_REG_INT_MOTION_1 0x60

#define BMI160_REG_CMD          0x7E

// latch configurations

#define BMI160_LATCH_NON_LATCHED 0x0
#define BMI160_LATCH_TEMP_312us  0x1
#define BMI160_LATCH_TEMP_625us  0x2
#define BMI160_LATCH_TEMP_1ms    0x3
#define BMI160_LATCH_TEMP_2ms    0x4
#define BMI160_LATCH_TEMP_5ms    0x5
#define BMI160_LATCH_TEMP_10ms   0x6
#define BMI160_LATCH_TEMP_20ms   0x7
#define BMI160_LATCH_TEMP_40ms   0x8
#define BMI160_LATCH_TEMP_80ms   0x9
#define BMI160_LATCH_TEMP_160ms  0xA
#define BMI160_LATCH_TEMP_320ms  0xB
#define BMI160_LATCH_TEMP_640ms  0xC
#define BMI160_LATCH_TEMP_1s     0xD
#define BMI160_LATCH_TEMP_2s     0xE
#define BMI160_LATCH_LATCHED     0xF

#define BMI160_RANGE_2G         0x3
#define BMI160_RANGE_4G         0x5
#define BMI160_RANGE_8G         0x8


// Global variables

static void *i2c_handle;
static motion_event_handler_type g_user_motion_event_handler = 0;


// Private functions

void gpio_event_handler(void);
int convertRaw(int16 raw, float *val_in_g);
/*=============================================================================
                                    FUNCTIONS
=============================================================================*/
/**
    Start up accelerometer
 
    return:
    DAL_SUCCESS if successful
*/
int bmi160_init()
{
    int ret = DAL_SUCCESS;
    unsigned char chip_id = 0;
    unsigned char EXPECTED_CHIP_ID = 0xD1;

    ret = sensorutil_i2c_init_handle(&i2c_handle, BMI160_SLAVE_ADDR);
    if (ret != DAL_SUCCESS)
    {
        //ALI:ULogFront_RealTimePrintf(sensortask_ulog, 1, "sensorutil_i2c_init_handle failed with ret=%d", ret);
        //return ret;
    }

    // read chip_id and make sure it's correct
    ret = sensorutil_i2c_read(i2c_handle, BMI160_REG_CHIP_ID, &chip_id);
    if (ret != DAL_SUCCESS) return ret;

    if (chip_id != EXPECTED_CHIP_ID) // unexpected chip_id
    {
        //ALI:
        //ULogFront_RealTimePrintf(sensortask_ulog, 2, "chip_id read from device (0x%x) does not equal expected chip_id (0x%x)", chip_id, EXPECTED_CHIP_ID);
        return DAL_ERROR;
    }

    ret = sensorutil_i2c_write(i2c_handle, BMI160_REG_ACC_RANGE, CONFIG_RANGE);  // range (2g, 4g or 8g)
    if (ret != DAL_SUCCESS) return ret;

    ret = sensorutil_i2c_write(i2c_handle, BMI160_REG_INT_MOTION_0, (CONFIG_ANYM_DUR & 0x3));  // bits 1:0 => int_anym_dur
    if (ret != DAL_SUCCESS) return ret;

    // interrupt related config
    ret = sensorutil_i2c_write(i2c_handle, BMI160_REG_INT_EN_0, 0x7);  // enable anymotion x, y, and z interrupts
    if (ret != DAL_SUCCESS) return ret;

    ret = sensorutil_i2c_write(i2c_handle, BMI160_REG_INT_LATCH, 0xF & CONFIG_LATCH );  // latch period
    if (ret != DAL_SUCCESS) return ret;

    ret = sensorutil_i2c_write(i2c_handle, BMI160_REG_INT_MAP_0, 0xFF); // map all interrupts to INT1; none to INT2
    if (ret != DAL_SUCCESS) return ret;

    ret = sensorutil_i2c_write(i2c_handle, BMI160_REG_INT_MAP_1, 0xF0); // map all interrupts to INT1; none to INT2
    if (ret != DAL_SUCCESS) return ret;

    ret = sensorutil_i2c_write(i2c_handle, BMI160_REG_INT_MAP_2, 0x00); // map all interrupts to INT1; none to INT2
    if (ret != DAL_SUCCESS) return ret;


    // put bmi160 in normal mode
    ret = sensorutil_i2c_write(i2c_handle, BMI160_REG_CMD, 0x11); // set to NORMAL mode
    if (ret != DAL_SUCCESS) return ret;

    // register for gpio events from platform
    ret = sensorutil_register_gpio_interrupt(CONFIG_INTERRUPT_GPIO_NUM, GPIO_INT_TRIGGER_FALLING, gpio_event_handler); 
    if (ret != DAL_SUCCESS) return ret;

    return ret;
}

/**
    Shutdown accelerometer
 
    return: DAL_SUCCESS if successful
*/
int bmi160_shutdown()
{
    int ret = DAL_SUCCESS;

    // put bmi160 into suspend mode
    ret = sensorutil_i2c_write(i2c_handle, BMI160_REG_CMD, 0x10); // set to SUSPEND mode

    return ret;
}

/**
    Set threshold for motion events

    Absolute value of threshold will depend on the range (2g,
    4g, 8g, 16g), which is internally configured by driver.\n
    Any motion duration (number of points above threshold
    required to trigger an interrupt) is also internally
    configured by the driver.

    param[in] threshold		value in range 0 to 255

    return: DAL_SUCCESS if successful
*/
int bmi160_set_motion_event_threshold(uint8 threshold)
{   
    int ret = DAL_SUCCESS;
    ret = sensorutil_i2c_write(i2c_handle, BMI160_REG_INT_MOTION_1, threshold);
    return ret;
}

/**
    Enable generation of motion events (when motion threshold is
    crossed)

    return: DAL_SUCCESS if successful
*/
int bmi160_enable_motion_events()
{
    int ret = DAL_SUCCESS;
    ret = sensorutil_i2c_write(i2c_handle, BMI160_REG_IN_OUT_CTRL, 0x08);  // enable interrupts on INT1
    return ret;
}

/**
    Disable motion events

    return: DAL_SUCCESS if successful
*/
int bmi160_disable_motion_events()
{
    int ret = DAL_SUCCESS;
    ret = sensorutil_i2c_write(i2c_handle, BMI160_REG_IN_OUT_CTRL, 0x00);  // disable interrupts on INT1
    return ret;
}

/**
    Attach callback to be invoked when a motion event is triggered

    param[in] handler		handler to be invoked to process
          motion event

    return: None
*/
void bmi160_attach_motion_event_handler(motion_event_handler_type handler)
{
    g_user_motion_event_handler = handler;
}

/**
    @brief
    Get acceleration on x,y,z axis (scaled to units of 'g')

    param[out] x		acceleration (scaled to units of 'g') on
    x-axis
    
    param[out] y		acceleration (scaled to units of ts of
    'g') on y-axis
    
    param[out] z		acceleration (scaled to ed to units of
    'g') on z-axis

    return: DAL_SUCCESS if successful
*/
int bmi160_get_accel(float *x, float *y, float *z)
{
    int ret = DAL_SUCCESS;
    unsigned char x_lsb;
    unsigned char x_msb;
    unsigned char y_lsb;
    unsigned char y_msb;
    unsigned char z_lsb;
    unsigned char z_msb;
    int16 x_raw;
    int16 y_raw;
    int16 z_raw;

    ret = sensorutil_i2c_read(i2c_handle, BMI160_REG_DATA_14, &x_lsb);
    if (ret != DAL_SUCCESS) return ret;

    ret = sensorutil_i2c_read(i2c_handle, BMI160_REG_DATA_15, &x_msb);
    if (ret != DAL_SUCCESS) return ret;

    ret = sensorutil_i2c_read(i2c_handle, BMI160_REG_DATA_16, &y_lsb);
    if (ret != DAL_SUCCESS) return ret;

    ret = sensorutil_i2c_read(i2c_handle, BMI160_REG_DATA_17, &y_msb);
    if (ret != DAL_SUCCESS) return ret;

    ret = sensorutil_i2c_read(i2c_handle, BMI160_REG_DATA_18, &z_lsb);
    if (ret != DAL_SUCCESS) return ret;

    ret = sensorutil_i2c_read(i2c_handle, BMI160_REG_DATA_19, &z_msb);
    if (ret != DAL_SUCCESS) return ret;

    x_raw = x_msb << 8 | x_lsb;
    y_raw = y_msb << 8 | y_lsb;
    z_raw = z_msb << 8 | z_lsb;

    ret = convertRaw(x_raw,x);
    if (ret != DAL_SUCCESS) return ret;

    ret = convertRaw(y_raw,y);
    if (ret != DAL_SUCCESS) return ret;

    ret = convertRaw(z_raw,z);
    if (ret != DAL_SUCCESS) return ret;

    return ret;
}

void gpio_event_handler()
{
    if (g_user_motion_event_handler != 0)
    {
        unsigned char int_status_2 = 0;
        int negative;
        int x_dir;
        int y_dir;
        int z_dir;
        int out_dir = -1;

        // read interrupt status data; notify user
        sensorutil_i2c_read(i2c_handle, BMI160_REG_INT_STATUS_2, &int_status_2);

        negative = (int_status_2 & 0x08) != 0;
        x_dir = (int_status_2 & 0x1) != 0;
        y_dir = (int_status_2 & 0x2) != 0;
        z_dir = (int_status_2 & 0x4) != 0;

        if (negative)
        {
            if (x_dir)
            {
                out_dir = MOTION_X_NEGATIVE;
            }
            else if (y_dir)
            {
                out_dir = MOTION_Y_NEGATIVE;
            }
            else if (z_dir)
            {
                out_dir = MOTION_Z_NEGATIVE;
            }
        }
        else
        {
            if (x_dir)
            {
                out_dir = MOTION_X_POSITIVE;
            }
            else if (y_dir)
            {
                out_dir = MOTION_Y_POSITIVE;
            }
            else if (z_dir)
            {
                out_dir = MOTION_Z_POSITIVE;
            }
        }

        g_user_motion_event_handler(out_dir);
    }
}

int convertRaw(int16 raw, float *val_in_g)
{
    float slope;
    float val;
    float range_abs;

    if (!val_in_g){
        //ALI:
        //ULogFront_RealTimePrintf(sensortask_ulog, 0, "convertRaw: val_in_g is NULL");
        return DAL_ERROR;
    }

    switch (CONFIG_RANGE) 
    {
    case BMI160_RANGE_2G:
        range_abs = 2.0f;
        break;
    case BMI160_RANGE_4G:
        range_abs = 4.0f;
        break;
    case BMI160_RANGE_8G:
        range_abs = 8.0f;
        break;
    default:
        //ALI:
        //ULogFront_RealTimePrintf(sensortask_ulog, 1, "convertRaw: CONFIG_RANGE=0x%x is invalid",CONFIG_RANGE);
        return DAL_ERROR;
    }

    /* Input range will be -32768 to 32767
     * Output range must be -range_abs to range_abs */
    val = (float)raw;
    slope = (range_abs * 2.0f) / 65535.0f;
    *val_in_g = -(range_abs)+slope * (val + 32768.0f);

    return DAL_SUCCESS;
}
