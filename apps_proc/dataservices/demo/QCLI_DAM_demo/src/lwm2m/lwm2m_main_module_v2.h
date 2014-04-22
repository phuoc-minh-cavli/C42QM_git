/**************************************************************************************
 * Copyright © 2018-2019 Qualcomm Technologies, Inc.
 * \file    lwm2m_main_module_v2.h
 * \author  Qualcomm Technologies, Inc.
 * \brief   Interface File for LWM2M functions
 * 
 * This file implements a part of QCLI and stub applications.
 ****************************************************************************************/


#define MAX_RES_LEN 256

QCLI_Command_Status_t devicecap_app_start_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t sw_mgnt_update_result_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t sw_mgnt_app_start_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t sw_mgnt_delete_instance_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_delete_instance_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_create_instance_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_app_stop_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_cancel_observe_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_observe_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_set_notify_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t device_app_set_resource_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t device_app_start_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_get_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t sw_mgnt_get_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t apninfo_app_start_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t apninfo_app_stop_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t apninfo_get_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t apninfo_create_instance_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t apninfo_delete_instance_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t cell_nw_app_start_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t cell_nw_app_stop_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t cell_nw_get_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t cell_nw_create_instance_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t cell_nw_delete_instance_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);




