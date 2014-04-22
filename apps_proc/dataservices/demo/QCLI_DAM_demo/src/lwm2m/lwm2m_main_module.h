/**************************************************************************************************************
 * Copyright © 2017 Qualcomm Technologies, Inc.
 * @file	software_mgnt_app.c
 * @author	Qualcomm Technologies, Inc.
 * @brief	File contains the utility api's for stub applications
 *
 **************************************************************************************************************/

#define MAX_RES_LEN 256

QCLI_Command_Status_t devicecap_app_start(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t sw_mgnt_update_result(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t sw_mgnt_app_start(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t sw_mgnt_delete_instance(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_delete_instance(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_create_instance(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_app_stop(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_cancel_observe(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_observe(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_set_notify(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t device_app_set_resource(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t device_app_start(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_get(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t sw_mgnt_get(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

