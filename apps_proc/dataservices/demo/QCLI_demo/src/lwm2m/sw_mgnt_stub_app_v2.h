/******************************************************************************
 * Copyright © 2018 Qualcomm Technologies, Inc.
 * @file    sw_mgnt_stub_app_v2.h
 * @author  Qualcomm Technologies, Inc.
 * @brief   Interface File Containing Interfaces and DS w.r.t carrier specific 
 * implementations
 *
 * This file declares the interfaces and data structures if any, used for 
 * software management stub application implementations
 *
 *****************************************************************************/



void software_mngt_init_v2(void);

void sw_mgnt_handle_update_state_input_v2(int32_t option);
 
int32_t sw_mgnt_handle_delete_instance_v2(uint16_t instance_id ); 
 
static int32_t sw_mgnt_handle_create_instance_v2(void);  

void software_mgnt_app_callback_v2( qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Data_v2_t * lwm2m_data, qapi_Status_t *result);

int32_t software_mngt_get_v2(qapi_Net_LWM2M_Id_Info_v2_t *id_info);


