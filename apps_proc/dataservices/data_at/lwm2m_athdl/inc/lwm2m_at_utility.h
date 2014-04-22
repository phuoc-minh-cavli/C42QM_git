#ifndef LWM2M_AT_UTILITY_H
#define LWM2M_AT_UTILITY_H
#ifdef LWM2M_AT_ENABLE
/*===========================================================================
                         LWM2M_AT_UTILITY.H

DESCRIPTION
  This header file contains definitions used internally by the lwm2m at utility  
  Module.

  Copyright (c)2018-2020 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR FILE
when        who    what, where, why
--------    ---    -------------------------------------------------------------
06/14/2018   xianma     created file
==============================================================================*/
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "lwm2m_at_handler.h"

/*------------------------------------------------------------------------------
  MACRO declaration
------------------------------------------------------------------------------*/

typedef enum lwm2m_at_device_res_id_e
{
  LWM2M_AT_DEVICE_RES_MANUFACTURER   = 0,
  LWM2M_AT_DEVICE_RES_MODEL_NUMBER   = 1,
  LWM2M_AT_DEVICE_RES_SERIAL_NUMBER  = 2,
  LWM2M_AT_DEVICE_RES_FIRMEWARE_VER  = 3,
  LWM2M_AT_DEVICE_RES_DEVICE_TYPE    = 17,
  LWM2M_AT_DEVICE_RES_HARDWARE_VER   = 18,
  LWM2M_AT_DEVICE_RES_SOFTWARE_VER   = 19,
  LWM2M_AT_DEVICE_RES_DEVICE_INFO    = 22,
}lwm2m_at_device_res_id_t;
/*------------------------------------------------------------------------------
  Function declaration
------------------------------------------------------------------------------*/
/*Common utility functions*/
boolean lwm2m_util_buffer_extend(uint8 ** src_buffer,size_t * src_len, uint8 * add_buffer, size_t add_len, uint8 buf_type);
boolean lwm2m_util_pack_buffer(uint8 ** target_buf,size_t * target_len, uint8 * input_buf, size_t input_len, uint8 buf_type);
boolean lwm2m_util_unpack_buffer(uint8 ** target_buf,size_t * target_len, uint8 * input_buf, size_t input_len, uint8 buf_type);
boolean lwm2m_util_string_to_objlink(const uint8 *String_Buffer, objlink_t *objlink_Value);
boolean lwm2m_util_parse_value(uint8 * a_ptr, uint8 value_type, qapi_Net_LWM2M_Flat_Data_t * data);
boolean lwm2m_util_parse_uri_str(char * a_ptr, qapi_Net_LWM2M_Obj_Info_t * id_info);
boolean lwm2m_util_parse_attr(uint8 * a_ptr, qapi_Net_LWM2M_Attributes_t * attr_info);
boolean lwm2m_util_resp_code_check(int32 input_code);
boolean lwm2m_util_resource_type_check(qapi_Net_LWM2M_Obj_Info_t * id_info, uint8 type);
boolean lwm2m_util_obj_info_check(int level, qapi_Net_LWM2M_Obj_Info_t * standard_ptr, qapi_Net_LWM2M_Obj_Info_t * target_ptr);
void lwm2m_util_msgid_copy(uint8_t * src_msgid_array, uint8_t * dest_msgid_array, uint8_t msg_id_count);
boolean util_at_string_to_Double(const uint8 *buffer, double *dataP);
char * util_at_float_to_string(double input);



/*LWM2M_Flat_Data related utility functions*/
qapi_Net_LWM2M_Flat_Data_t * lwm2m_util_FlatData_array_find(qapi_Net_LWM2M_Flat_Data_t * array, unsigned int count, uint16_t id);
boolean lwm2m_uitl_FlatData_array_copy(qapi_Net_LWM2M_Flat_Data_t * src, qapi_Net_LWM2M_Flat_Data_t * dest, unsigned int count);
void lwm2m_util_FlatData_array_free(qapi_Net_LWM2M_Flat_Data_t ** array, unsigned int count);
boolean lwm2m_util_FlatData_array_extend(qapi_Net_LWM2M_Flat_Data_t ** array, size_t * array_len, qapi_Net_LWM2M_Flat_Data_t ** new_ep);
boolean lwm2m_util_FlatData_array_display(char * atcmd_name, char * prefix, int max_resp_len, qapi_Net_LWM2M_Flat_Data_t * lwm2m_data, size_t data_len);
atfwd_result_code_t lwm2m_util_FlatData_struct_extend(qapi_Net_LWM2M_Flat_Data_t * obj_root, qapi_Net_LWM2M_Obj_Info_t * id_info, qapi_Net_LWM2M_Flat_Data_t ** new_node);

/*LWM2M object structure related utility functions*/
atfwd_result_code_t lwm2m_util_ResInfo_list_extend(qapi_Net_LWM2M_Resource_Info_t ** root, qapi_Net_LWM2M_Obj_Info_t * id_info, qapi_Net_LWM2M_Flat_Data_t ** new_node);
atfwd_result_code_t lwm2m_util_InstInfo_list_extend(qapi_Net_LWM2M_Instance_Info_v2_t ** root, qapi_Net_LWM2M_Obj_Info_t * id_info, qapi_Net_LWM2M_Flat_Data_t ** new_node);
atfwd_result_code_t lwm2m_util_ObjInfo_list_extend(qapi_Net_LWM2M_Data_v2_t ** root, qapi_Net_LWM2M_Obj_Info_t * id_info, qapi_Net_LWM2M_Flat_Data_t ** new_node);
boolean lwm2m_util_ObjInfo_struct_display(char * atcmd_name, char ** resp_str, qapi_Net_LWM2M_Data_v2_t * lwm2m_data, int max_resp_len);

/*LWM2M attribute related utility functions*/
void lwm2m_util_attr_list_free(qapi_Net_LWM2M_Attributes_t *head);
boolean lwm2m_util_attr_list_add(qapi_Net_LWM2M_Attributes_t ** head, qapi_Net_LWM2M_Attributes_t ** new_node);
atfwd_result_code_t lwm2m_util_attr_list_display(char * atcmd_name, char * prefix, qapi_Net_LWM2M_Attributes_t * attr_info);

/*local struct related utility functions*/
boolean lwm2m_util_req_store_delete(qapi_Net_LWM2M_Obj_Info_t * target_id, qapi_Net_LWM2M_DL_Msg_t msg_type);
lwm2m_at_req_store_t * lwm2m_util_req_store_find_by_notifyId(uint16_t notification_id);
void lwm2m_util_m2mset_data_free(lwm2m_at_M2MSET_t ** m2mset_data_ptr);
void lwm2m_util_m2msetmsgdata_free(lwm2m_at_M2MSETMSGDATA_t ** input_ptr);
void lwm2m_util_req_store_free(util_at_list_t ** req_store_hdr);
void lwm2m_util_extreq_errhdl(qapi_Net_LWM2M_App_Handler_t hdl,uint16 req_id, atfwd_result_code_t input_result);
void lwm2m_util_send_error_response(qapi_Net_LWM2M_App_Handler_t hdl, qapi_Net_LWM2M_Response_Code_t status_code, uint8_t msg_id_len, uint8_t * msg_id, qapi_Net_LWM2M_Obj_Info_t * id_info);
#endif /*LWM2M_AT_ENABLE*/
#endif
