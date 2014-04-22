/*=============================================================================
  Copyright (c) 2016  by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

#ifndef SENSORUTIL_H
#define SENSORUTIL_H

typedef enum {
  GPIO_INT_TRIGGER_HIGH,
  GPIO_INT_TRIGGER_LOW,
  GPIO_INT_TRIGGER_RISING,
  GPIO_INT_TRIGGER_FALLING,
  GPIO_INT_TRIGGER_DUAL_EDGE,
} sensorutil_gpio_int_trigger_type;

typedef void (*sensorutil_gpio_event_handler_type)(void);

int sensorutil_register_gpio_interrupt(int gpio, sensorutil_gpio_int_trigger_type trigger, sensorutil_gpio_event_handler_type handler);
int sensorutil_i2c_init_handle(void **handle, unsigned char slave_addr);
int sensorutil_i2c_write_nodata(void *handle, unsigned char offset);
int sensorutil_i2c_write(void *handle, unsigned char offset, unsigned char data);
int sensorutil_i2c_read(void *handle, unsigned char offset, unsigned char *pData);
int sensorutil_gpio_set_value(int gpio_num, unsigned char value);

#endif // SENSORUTIL_H
