/******************************************************************************
 * Copyright © 2018 Qualcomm Technologies, Inc.
 * @file    fota_stub_app_v2.h
 * @author  Qualcomm Technologies, Inc.
 * @brief   Interface File Containing Interfaces and DS w.r.t carrier specific 
 * implementations
 *
 * This file declares the interfaces and data structures if any, used for 
 * fota stub application implementations
 *
 *****************************************************************************/

void fota_init_v2(void);

void handle_update_state_input_v2(int32_t option );

void handle_fw_set_protocol_support_v2(int32_t option);

void handle_fw_set_delivery_method_v2(int32_t option);

int handle_fw_set_pkg_version_v2(char* buf, int len);

void  app_callback_v2( qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Data_v2_t * lwm2m_data, qapi_Status_t *result);

void dump_firmware_get_info_v2(qapi_Net_LWM2M_Data_v2_t *lwm2m_dataP);

