/******************************************************************************
 * Copyright © 2017 Qualcomm Technologies, Inc.
 * @file    fota_stub_app.h
 * @author  Qualcomm Technologies, Inc.
 * @brief   Interface File Containing Interfaces and DS w.r.t carrier specific 
 * implementations
 *
 * This file declares the interfaces and data structures if any, used for 
 * fota stub application implementations
 *
 *****************************************************************************/

void fota_init(void);

void handle_update_state_input(int32_t option );

void handle_fw_set_protocol_support(int32_t option);

void handle_fw_set_delivery_method(int32_t option);

int handle_fw_set_pkg_version(char* buf, int len);

qapi_Status_t app_callback( qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Data_t * lwm2m_data);

void dump_firmware_get_info(qapi_Net_LWM2M_Data_t *lwm2m_dataP);

