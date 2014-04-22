/* Copyright (c) 2017-2020.  Qualcomm Atheros, Inc.
All rights reserved.

Qualcomm Atheros Confidential and Proprietary.

*/

#ifndef _NET_UTILS_EXT_H_
#define _NET_UTILS_EXT_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct cb_params_uspace_ptr
{
   void * cb_param[8];
   int cb_count;
   void *usr_pool_ptr;
}data_cb_params_uspace_ptr;

typedef enum{

   APP_DSS = 0,
   APP_MQTT,
   APP_LWM2M,
   APP_HTTP,
   APP_SSL,
   APP_NWSTACK,
   APP_NIPD,
   APP_ATFWD,
   APP_DEVICE_INFO,
   APP_CLI,
   APP_COAP,
#ifdef TRACKER_REF_APP
   APP_SENSOR_MGR,
#endif
   LOC_SSL,
   APP_MAX
}data_app_type;


typedef struct {
  void * handle;
  data_cb_params_uspace_ptr * user_space_address;
  data_app_type app_type;
  unsigned int addr_len;
}data_module_vmem_info;


/* Utility function to be used for accessing shared memory changes for DAM Support on TX */
data_module_vmem_info *data_get_uspace_address(void * handle , data_app_type app_type);
char* extractIPAddrFromUrl(const char* host_url, char* identi_name);
bool data_is_user_space_module(void);
int data_get_module_info(uint32_t *module_uid, uint32_t *module_instance);
int data_validate_module_address(uint32_t module_addr, uint32_t module_addr_size);
int data_validate_module_id_info(uint32_t module_uid);
int data_copy_from_user_ext(uint32_t module_uid, uint32_t module_instance, void **kspace_addr, uint32_t *kspace_buf_size, const void *uspace_addr, uint32_t uspace_buf_size);
int data_copy_from_user(uint32_t module_uid, uint32_t module_instance, void *kspace_addr, uint32_t kspace_buf_size, const void *uspace_addr, uint32_t uspace_buf_size);
int data_copy_to_user_ext(uint32_t module_uid, uint32_t module_instance, void **uspace_addr, uint32_t *uspace_buf_size, const void *kspace_addr, uint32_t kspace_buf_size, const void *user_memory_pool);
int data_copy_to_user(uint32_t module_uid, uint32_t module_instance, void *uspace_addr, uint32_t uspace_buf_size, const void *kspace_addr, uint32_t kspace_buf_size);



#endif /* _NET_UTILS_EXT_H_ */

