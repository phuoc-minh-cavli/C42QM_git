
/******************************************************************************
 * Copyright © 2017 Qualcomm Technologies, Inc.
 * @file    devicecap_stub_app.h
 * @author  Qualcomm Technologies, Inc.
 * @brief   Contains the infomration needed for device capability app stub
 *****************************************************************************/

void devicecap_init(void);
qapi_Status_t devicecap_app_callback( qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Data_t * lwm2m_data);

int32_t devicecap_handle_create_instance(void); 

int32_t devicecap_handle_delete_instance(uint8_t instance_id);

void devicecap_app_deregister(void);

int32_t devicecap_handle_observe(uint8_t instance_id);

int32_t devicecap_handle_cancel_observe(uint8_t instance_id);

int set_notify_en_value(uint8_t instance_id,bool notifyen);

int device_cap_get(qapi_Net_LWM2M_Id_Info_t *id_info);

