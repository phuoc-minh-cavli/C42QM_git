/**************************************************************************************************************
 * Copyright © 2017 Qualcomm Technologies, Inc.
 * @file	software_mgnt_app.c
 * @author	Qualcomm Technologies, Inc.
 * @brief	File contains the utility api's for stub applications
 *
 **************************************************************************************************************/

int32_t app_free_id_info (qapi_Net_LWM2M_Id_Info_t *id_infoP);

int32_t app_free_data(qapi_Net_LWM2M_Data_t *dataP);

int32_t app_free_instance_info  (qapi_Net_LWM2M_Instance_Info_t *instanceP);

int32_t app_free_resource_info (qapi_Net_LWM2M_Resource_Info_t *resourceP);

void app_display_obj_info(qapi_Net_LWM2M_Data_t *lwm2m_data);

