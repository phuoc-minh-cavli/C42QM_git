/**************************************************************************************
 * Copyright © 2017 Qualcomm Technologies, Inc.
 * \file    lwm2m_stub_common.h
 * \author  Qualcomm Technologies, Inc.
 * \brief   Interface File for LWM2M functions
 * 
 * This file implements a part of interfacing between LWM2M TO COAP.
 ****************************************************************************************/


#define MAX_RES_LEN 256
#define MAX_URI_LEN 256
#define MAX_APN_LEN 64

QCLI_Command_Status_t fw_update_result(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t fw_set_protocol(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t fw_set_delivery_method(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t fw_app_start(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);	
	
QCLI_Command_Status_t devicecap_app_start(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t sw_mgnt_app_start(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);
	
QCLI_Command_Status_t sw_mgnt_update_result(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t sw_mgnt_delete_instance(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_delete_instance(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_create_instance(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t location_app_start(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t location_app_set_radius(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_app_stop(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_cancel_observe(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_observe(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t devicecap_set_notify(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t device_app_set_resource(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t device_app_start(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t fw_set_pkg_version(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

QCLI_Command_Status_t devicecap_get(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

QCLI_Command_Status_t sw_mgnt_get(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List);

