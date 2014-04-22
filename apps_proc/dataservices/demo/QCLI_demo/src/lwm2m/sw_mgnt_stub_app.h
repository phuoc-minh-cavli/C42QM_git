/******************************************************************************
 * Copyright © 2017 Qualcomm Technologies, Inc.
 * @file    sw_mgnt_stub_app.h
 * @author  Qualcomm Technologies, Inc.
 * @brief   Interface File Containing Interfaces and DS w.r.t carrier specific 
 * implementations
 *
 * This file declares the interfaces and data structures if any, used for 
 * software management stub application implementations
 *
 *****************************************************************************/



void software_mngt_init(void);

void sw_mgnt_handle_update_state_input(int32_t option);
 
int32_t sw_mgnt_handle_delete_instance(uint8_t instance_id ); 
 
static int32_t sw_mgnt_handle_create_instance(void);  

qapi_Status_t software_mgnt_app_callback( qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Data_t * lwm2m_data);

int32_t software_mngt_get(qapi_Net_LWM2M_Id_Info_t *id_info);


