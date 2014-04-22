
/******************************************************************************
 * Copyright © 2018 Qualcomm Technologies, Inc.
 * @file    devicecap_stub_app_v2.h
 * @author  Qualcomm Technologies, Inc.
 * @brief   Contains the infomration needed for device capability app stub
 *****************************************************************************/

void devicecap_init_v2(void);
void devicecap_app_callback_v2( qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Data_v2_t * lwm2m_data, qapi_Status_t *result);

int32_t devicecap_handle_create_instance_v2(void); 

int32_t devicecap_handle_delete_instance_v2(uint16_t instance_id);

void devicecap_app_deregister_v2(void);

int32_t devicecap_handle_observe_v2(uint16_t instance_id);

int32_t devicecap_handle_cancel_observe_v2(uint16_t instance_id);

int set_notify_en_value_v2(uint16_t instance_id,bool notifyen);

int device_cap_get_v2(qapi_Net_LWM2M_Id_Info_v2_t *id_info);

