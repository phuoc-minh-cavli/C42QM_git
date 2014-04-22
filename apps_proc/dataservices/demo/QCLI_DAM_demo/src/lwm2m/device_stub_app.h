/******************************************************************************
 * Copyright © 2017 Qualcomm Technologies, Inc.
 * @file    device_stub_app.h
 * @author  Qualcomm Technologies, Inc.
 * @brief   Contains the infomration needed for device app stub
 *****************************************************************************/

#define DEVICE_INSTANCE_ID_0  0

void device_stub_app_init(void);

int set_device_resource_value(int resource_id, char* buf, int len);

void device_app_deregister(void);


