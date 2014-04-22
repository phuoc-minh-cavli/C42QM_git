/**************************************************************************************************************
 * Copyright © 2018 Qualcomm Technologies, Inc.
 * @file	utils_app_v2.h
 * @author	Qualcomm Technologies, Inc.
 * @brief	File contains the utility api's for stub applications
 *
 **************************************************************************************************************/

/*
 * Utility functions for sorted linked list
 */

typedef struct _std_obj_list_t
{
  struct _std_obj_list_t *next;
  uint16_t    id;
} std_obj_list_t;

typedef struct _std_obj_persist_list_t
{
  uint16_t    obj_id;
  uint16_t    id;
  bool isValid;
} std_obj_persist_list_t;


// Add 'node' to the list 'head' and return the new list
std_obj_list_t * std_obj_list_add(std_obj_list_t * head, std_obj_list_t * node);
// Return the node with ID 'id' from the list 'head' or NULL if not found
std_obj_list_t * std_obj_list_find(std_obj_list_t * head, uint16_t id);
// Remove the node with ID 'id' from the list 'head' and return the new list
std_obj_list_t * std_obj_list_remove(std_obj_list_t * head, uint16_t id, std_obj_list_t ** nodeP);

void std_obj_utils_copyValue(void * dst, const void * src, size_t len);

int std_obj_utils_plainTextToInt64(uint8_t * buffer,
    int length,
    int64_t * dataP);

int32_t app_free_id_info_v2 (qapi_Net_LWM2M_Id_Info_v2_t *id_infoP);

int32_t app_free_data_v2(qapi_Net_LWM2M_Data_v2_t *dataP);

int32_t app_free_instance_info_v2  (qapi_Net_LWM2M_Instance_Info_v2_t *instanceP);

int32_t app_free_resource_info_v2 (qapi_Net_LWM2M_Resource_Info_t *resourceP);

void app_display_obj_info_v2(qapi_Net_LWM2M_Data_v2_t *lwm2m_data);

