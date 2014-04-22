/*===========================================================================
                         EXT_OBJECT_TEST_APP.H

DESCRIPTION
   This header file contains definitions used internally by the Extensible object test
   Module. 
  
   Copyright (c) 2017 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
#include "txm_module.h"
#include "stdlib.h"
#include "stdint.h"
#include <stdarg.h>
#include "qurt_error.h"
#include "qurt_memory.h"
#include "qurt_mutex.h"
#include "qurt_signal.h"
#include "qurt_system.h"
#include "qurt_thread.h"
#include "qurt_types.h"
#include "qapi_lwm2m.h"
#include "qcli_api.h"
#include "log_intf_module.h"
#include "qapi_uart.h"
#include "qapi_timer.h"
#include "time.h"

#define MAX_STRING_LEN             256

#define PRINT_SIZE 256

static char m2m_log_buffer[PRINT_SIZE];

#define RES_M_SENSOR_VALUE_v2             0 /* Last or current measured sensor value. */
#define RES_O_SENSOR_UNITS_v2             1 /* Measurement unit definition e.g. "Celcius" for tempwerature. */
#define RES_O_MIN_MEASURED_VAL_v2         2 /* Minimum value measured since power on or reset */
#define RES_O_MAX_MEASURED_VAL_v2         3 /* Maximum value measured since power on or reset */
#define RES_M_MIN_RANGE_VAL_v2            4 /* Minimum threshold value. */
#define RES_M_MAX_RANGE_VAL_v2            5 /* Maximum threshold value. */
#define RES_O_RESET_MEASURED_VAL_v2       6 /* Reset minimum and maximum measured value. */
#define RES_M_SENSOR_STATE_v2             7 /* Sensor state on/off. */
#define RES_O_MEAN_VAL_v2                 8 /* Mean measured value */
#define RES_O_SNSR_VALUES_v2              9

#define RES_M_DIG_INPUT_STATE_v2                  65528
#define RES_O_DIG_INPUT_COUNTER_v2                65529
#define RES_O_RESET_DIG_INPUT_COUNTER_v2          65530
#define RES_O_SENSOR_TYPE_v2                      65531
#define RES_O_BUSY_TO_CLEAR_DELAY_v2              65532
#define RES_O_CLEAR_TO_BUSY_DELAY_v2              65533
#define RES_O_SUP_SENSOR_TYPE_v2                  65534

#define LWM2M_GENERIC_SENSOR_OBJECT_ID_v2 65533
#define LWM2M_PRESENCE_SENSOR_OBJECT_ID_v2 65534

#define BIT64_BUFFER_SIZE              8

#define URL_INFO_BUFFER_SIZE  255

#define ERANGE  34 /* Math result not representable */
#define LWM2M_ATTR_FLAG_NUMERIC (QAPI_NET_LWM2M_LESS_THAN_E| QAPI_NET_LWM2M_GREATER_THAN_E| QAPI_NET_LWM2M_STEP_E)

#define LWM2M_ATTR_SERVER_ID_STR       "ep="
#define LWM2M_ATTR_SERVER_ID_LEN       3
#define LWM2M_ATTR_MIN_PERIOD_STR      "pmin="
#define LWM2M_ATTR_MIN_PERIOD_LEN      5
#define LWM2M_ATTR_MAX_PERIOD_STR      "pmax="
#define LWM2M_ATTR_MAX_PERIOD_LEN      5
#define LWM2M_ATTR_GREATER_THAN_STR    "gt="
#define LWM2M_ATTR_GREATER_THAN_LEN    3
#define LWM2M_ATTR_LESS_THAN_STR       "lt="
#define LWM2M_ATTR_LESS_THAN_LEN       3
#define LWM2M_ATTR_STEP_STR            "st="
#define LWM2M_ATTR_STEP_LEN            3
#define LWM2M_ATTR_DIMENSION_STR       "dim="
#define LWM2M_ATTR_DIMENSION_LEN       4

#define LWM2M_URI_MAX_STRING_LEN    18      // /65535/65535/65535
#define _PRV_64BIT_BUFFER_SIZE 8

#define LWM2M_LINK_ITEM_START             "<"
#define LWM2M_LINK_ITEM_START_SIZE        1
#define LWM2M_LINK_ITEM_END               ">,"
#define LWM2M_LINK_ITEM_END_SIZE          2
#define LWM2M_LINK_ITEM_DIM_START         ">;dim="
#define LWM2M_LINK_ITEM_DIM_START_SIZE    6
#define LWM2M_LINK_ITEM_ATTR_END          ","
#define LWM2M_LINK_ITEM_ATTR_END_SIZE     1
#define LWM2M_LINK_URI_SEPARATOR          "/"
#define LWM2M_LINK_URI_SEPARATOR_SIZE     1
#define LWM2M_LINK_ATTR_SEPARATOR         ";"
#define LWM2M_LINK_ATTR_SEPARATOR_SIZE    1

typedef struct qapi_Net_LWM2M_Uri_v2_s
{
  uint32_t    flag;           // indicates which segments are set
  uint16_t    objectId;
  uint16_t    instanceId;
  uint16_t    resourceId;
  uint16_t    resourceInstId;
}qapi_Net_LWM2M_Uri_v2_t;

typedef struct qapi_Net_LWM2M_Observe_info_v2_s
{
  struct qapi_Net_LWM2M_Observe_info_v2_s *next;
  bool active;
  bool update;
  qapi_Net_LWM2M_Attributes_t *attributes;
  time_t last_time;
  uint32_t seq_num;
  union
  {
     int64_t asInteger;
     double  asFloat;
  }lastvalue;
  uint8_t msg_id_len;
  uint8_t msg_id[QAPI_MAX_LWM2M_MSG_ID_LENGTH];
  uint16_t not_id;
  qapi_Net_LWM2M_Content_Type_t format;
}qapi_Net_LWM2M_Observe_info_v2_t;

typedef struct qapi_Net_LWM2M_Observed_v2_s
{
  struct qapi_Net_LWM2M_Observed_v2_s *next;
  qapi_Net_LWM2M_Uri_v2_t uri;
  qapi_Net_LWM2M_Observe_info_v2_t *observe_info;
}qapi_Net_LWM2M_Observed_v2_t;

typedef struct qapi_Net_LWM2M_Pending_Observed_v2_s
{
  struct qapi_Net_LWM2M_Pending_Observed_v2_s *next;
  qapi_Net_LWM2M_Observe_info_v2_t *observe_info;
  qapi_Net_LWM2M_Uri_v2_t uri;
  void *message;
}qapi_Net_LWM2M_Pending_Observed_v2_t;

typedef struct ext_resource_instance_int_list_v2_s
{
  struct ext_resource_instance_int_list_v2_s*   next; 
  uint16_t                resInstId;  
  uint16_t                InstValue;
} ext_resource_instance_int_list_v2_t;

typedef struct ext_resource_instance_string_list_v2_s
{
  struct ext_resource_instance_string_list_v2_s*   next; 
  uint16_t                resInstId;  
  char                    InstValue[MAX_STRING_LEN];
} ext_resource_instance_string_list_v2_t;

typedef struct gen_multi_res_v2_s
{
  struct gen_multi_res_v2_s *next;
  uint16_t instanceId;
  ext_resource_instance_int_list_v2_t*  sensors_val;
}gen_multi_res_v2_t;

typedef struct pre_multi_res_v2_s
{
  struct pre_multi_res_v2_s *next;
  uint16_t instanceId;
  ext_resource_instance_string_list_v2_t*  sensor_types;
}pre_multi_res_v2_t;

typedef enum qapi_Net_LWM2M_Ext_App_Msg_v2_e
{
  QAPI_NET_LWM2M_EXT_REGISTER_APP_v2_E,
  QAPI_NET_LWM2M_EXT_DEREGISTER_APP_v2_E,
  QAPI_NET_LWM2M_EXT_CREATE_OBJ_v2_E,
  QAPI_NET_LWM2M_EXT_CREATE_OBJ_INSTANCE_v2_E,
  QAPI_NET_LWM2M_EXT_DELETE_OBJ_INSTANCE_v2_E,
  QAPI_NET_LWM2M_EXT_NOTIFY_v2_E,
  QAPI_NET_LWM2M_EXT_SET_OBJ_LINK_v2_E,
  QAPI_NET_LWM2M_EXT_GET_OBJ_LINK_v2_E,
  QAPI_NET_LWM2M_EXT_SET_SRV_LIFETIME_v2_E,
  QAPI_NET_LWM2M_EXT_GET_SRV_LIFETIME_v2_E,
  QAPI_NET_LWM2M_EXT_CONFIG_BOOTSTRAP_INFO_E,
  QAPI_NET_LWM2M_EXT_START_LWM2M,
  QAPI_NET_LWM2M_EXT_STOP_LWM2M,
  QAPI_NET_LWM2M_EXT_DEREGISTER_AND_STOP_LWM2M,
  QAPI_NET_LWM2M_EXT_FACTORY_RESET_LWM2M,
}qapi_Net_LWM2M_Ext_App_Msg_v2_t;

typedef struct qapi_Net_LWM2M_Ext_Obj_v2_s
{
  qapi_Net_LWM2M_App_Handler_t handler;
  qapi_Net_LWM2M_Server_Data_t *lwm2m_srv_data;
  void                         *data;
}qapi_Net_LWM2M_Ext_Obj_v2_t;

typedef struct qapi_Net_LWM2M_Ext_App_v2_s
{
  uint16_t id;
  qapi_Net_LWM2M_Ext_App_Msg_v2_t msg_type;
  qapi_Net_LWM2M_Obj_Info_t obj_info;
  uint8_t url_info[URL_INFO_BUFFER_SIZE];
  uint32_t lifetime; 
}qapi_Net_LWM2M_Ext_App_v2_t;

typedef struct qapi_Net_LWM2M_Ext_v2_s
{
  union
  {
    qapi_Net_LWM2M_Ext_Obj_v2_t dl_op;
    qapi_Net_LWM2M_Ext_App_v2_t ul_op;
  }app_data;
}qapi_Net_LWM2M_Ext_v2_t;

QCLI_Command_Status_t init_ext_obj_v2(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t create_ext_obj_v2(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t create_ext_obj_inst_v2(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t delete_ext_obj_inst_v2(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t set_obj_link_v2(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t get_obj_link_v2(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t set_server_lifetime_v2 (uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t get_server_lifetime_v2 (uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t ext_read_v2(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t ext_write_v2(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t ext_exec_v2(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t display_template_v2(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t deinit_ext_obj_v2(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t res_inst_create_v2(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t res_inst_delete_v2(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qcli_cmd_config_bootstrap_info(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qcli_cmd_start_lwm2m(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t qcli_cmd_stop_lwm2m(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);


qapi_Status_t create_object_instance_app_v2(qapi_Net_LWM2M_Ext_v2_t *obj_buffer);

qapi_Status_t delete_object_instance_app_v2(qapi_Net_LWM2M_Ext_v2_t *obj_buffer);

qapi_Status_t set_obj_link_app_v2(qapi_Net_LWM2M_Ext_v2_t *obj_buffer);

qapi_Status_t get_obj_link_app_v2(qapi_Net_LWM2M_Ext_v2_t *obj_buffer);

qapi_Status_t set_server_lifetime_app_v2(qapi_Net_LWM2M_Ext_v2_t *obj_buffer);

qapi_Status_t get_server_lifetime_app_v2(qapi_Net_LWM2M_Ext_v2_t *obj_buffer);

qapi_Status_t create_object_v2(int id);

qapi_Status_t ext_object_register_v2(int id);

qapi_Status_t ext_object_deregister_v2(int id);

qapi_Net_LWM2M_Instance_Info_v2_t *find_object_instance_v2(uint16_t object_id, uint16_t instance_id);

qapi_Net_LWM2M_Resource_Info_t *find_resource_info_v2(uint16_t object_id, uint16_t instance_id,
                                                   uint16_t resource_id);

qapi_Status_t create_object_instance_v2(qapi_Net_LWM2M_App_Handler_t handler, 
                                     qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t delete_object_instance_v2(qapi_Net_LWM2M_App_Handler_t handler, 
                                     qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t read_ext_obj_v2(qapi_Net_LWM2M_App_Handler_t handler, 
                           qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t write_ext_obj_v2(qapi_Net_LWM2M_App_Handler_t handler, 
                            qapi_Net_LWM2M_Server_Data_t op_data,
                            uint8_t msg_code);

qapi_Status_t exec_ext_obj_v2(qapi_Net_LWM2M_App_Handler_t handler, 
                           qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t disc_ext_obj_v2(qapi_Net_LWM2M_App_Handler_t handler, 
                           qapi_Net_LWM2M_Server_Data_t op_data);

void send_response_message_v2(qapi_Net_LWM2M_App_Handler_t handler,
                           qapi_Net_LWM2M_Server_Data_t op_data,
                           qapi_Net_LWM2M_Response_Code_t status);

qapi_Status_t ext_obj_handle_event_v2(qapi_Net_LWM2M_App_Handler_t handler, 
                                   qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t ext_obj_delete_all_v2(qapi_Net_LWM2M_App_Handler_t handler, 
                                 qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t start_app_v2(int id);

qapi_Status_t read_data_ext_v2(uint16_t instanceid, int *size, qapi_Net_LWM2M_Flat_Data_t **data,
                            qapi_Net_LWM2M_Data_v2_t *object, uint16_t res_inst_id);

qapi_Status_t observe_cancel_ext_obj_v2(qapi_Net_LWM2M_App_Handler_t handler, 
                                     qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t ext_object_write_attr_v2(qapi_Net_LWM2M_App_Handler_t handler, 
                                    qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t observe_ext_obj_v2(qapi_Net_LWM2M_App_Handler_t handler, 
                              qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t ext_object_configclient_v2(int id);
qapi_Status_t configclient_start(int id);
qapi_Status_t configclient_stop(int id, qapi_Net_LWM2M_Config_Type_t type);
qapi_Status_t configclient_bootstrap_info(int id);

uint8_t ext_object_set_observe_param_v2(qapi_Net_LWM2M_App_Handler_t handler,
                                     qapi_Net_LWM2M_Server_Data_t op_data);

void ext_object_notify_v2(int id,
                       time_t current_time,
                       time_t *timeout);

void stop_ext_obs_notify_timer_v2(qapi_Net_LWM2M_App_Handler_t handler);

int ext_obj_discover_serialize_v2(qapi_Net_LWM2M_App_Handler_t handler,
                               qapi_Net_LWM2M_Server_Data_t op_data,
                               qapi_Net_LWM2M_Uri_v2_t *uri,
                               int size,
                               qapi_Net_LWM2M_Flat_Data_t *data,
                               uint8_t **buffer);

void copy_value(void * dst,
                const void *src,
                size_t len);

size_t int_to_text(int64_t data,
                   uint8_t *string,
                   size_t length);

size_t float_to_text(double data,
                     uint8_t *string,
                     size_t length);

size_t int64_to_text(int64_t data,
                     uint8_t **buffer);

size_t float64_to_text(double data,
                       uint8_t **buffer);

size_t bool_to_text(bool data,
                    uint8_t **buffer);

size_t serialize_text(qapi_Net_LWM2M_Flat_Data_t *data,
                      uint8_t **buffer);

int set_buffer(qapi_Net_LWM2M_Flat_Data_t *data,
               uint8_t *buffer,
               size_t buffer_len);

size_t encode_int(int64_t data,
                  uint8_t data_buffer[BIT64_BUFFER_SIZE]);

size_t encode_float(double data,
                    uint8_t data_buffer[BIT64_BUFFER_SIZE]);

void encode_opaque_data(uint8_t * buffer,
                        size_t length,
                        qapi_Net_LWM2M_Flat_Data_t *data);

uint8_t get_header_type(qapi_Net_LWM2M_Value_Type_t type);

qapi_Net_LWM2M_Value_Type_t get_data_type(uint8_t type);

int get_header_length(uint16_t id,
                      size_t data_len);

int create_header(uint8_t *header,
                  bool is_instance,
                  qapi_Net_LWM2M_Value_Type_t type,
                  uint16_t id,
                  size_t data_len);

int tlv_decode(uint8_t *buffer,
               size_t buffer_len,
               qapi_Net_LWM2M_Value_Type_t *type,
               uint16_t *id,
               size_t *data_index,
               size_t *data_len);

int tlv_parser(uint8_t *buffer,
               size_t buffer_len,
               qapi_Net_LWM2M_Flat_Data_t **data);

size_t get_item_length(int size,
                       qapi_Net_LWM2M_Flat_Data_t *data);

size_t tlv_encode(bool is_resource_instance, 
                  int size,
                  qapi_Net_LWM2M_Flat_Data_t *data,
                  uint8_t ** buffer);

void observe_delete_ext_handle_req_v2(qapi_Net_LWM2M_App_Handler_t handler,
                                   qapi_Net_LWM2M_Uri_v2_t *uri);

void flush_pending_notifications(qapi_Net_LWM2M_App_Handler_t handler, uint16_t ssid, time_t current_time);;


