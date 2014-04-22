/******************************************************************************
 * Copyright © 2019 Qualcomm Technologies, Inc.
 * @file    apn_info_app_v2.h
 * @author  Qualcomm Technologies, Inc.
 * @brief   Contains the infomration needed for apn connection profile app stub
 *****************************************************************************/

void apn_info_init_v2(void);

int32_t apn_info_create_instance_v2(uint16_t instanceID);

int32_t apn_info_delete_instance_v2(uint16_t instanceID);

qapi_Status_t apn_info_get_v2(qapi_Net_LWM2M_Id_Info_v2_t *id_info);

void apn_info_app_deregister_v2(void);



