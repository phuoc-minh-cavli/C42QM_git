/******************************************************************************
 * Copyright © 2018 Qualcomm Technologies, Inc.
 * @file    device_stub_app_v2.h
 * @author  Qualcomm Technologies, Inc.
 * @brief   Contains the infomration needed for device app stub
 *****************************************************************************/

#define DEVICE_INSTANCE_ID_0  0

void device_stub_app_init_v2(void);

int set_device_resource_value_v2(uint16_t resource_id, char* buf, int len);

void device_app_deregister_v2(void);


