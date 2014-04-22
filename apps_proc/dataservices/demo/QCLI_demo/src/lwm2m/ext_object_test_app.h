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
#include "stdlib.h"
#include "qurt_signal.h"
#include "qurt_thread.h"
#include "qurt_error.h"
#include "err.h"
#include "qurt.h"
#include "msgcfg.h"
#include "rcinit.h"
#include "qapi_lwm2m.h"
#include "stdint.h"
#include "qurt_mutex.h"
#include "queue.h"
#include "qcli_api.h"
#include <time.h>
#include "time_types.h"
#include "time_svc.h"

#define MAX_STRING_LEN             256
#define RES_M_SENSOR_VALUE             0 /* Last or current measured sensor value. */
#define RES_O_SENSOR_UNITS             1 /* Measurement unit definition e.g. "Celcius" for tempwerature. */
#define RES_O_MIN_MEASURED_VAL         2 /* Minimum value measured since power on or reset */
#define RES_O_MAX_MEASURED_VAL         3 /* Maximum value measured since power on or reset */
#define RES_M_MIN_RANGE_VAL            4 /* Minimum threshold value. */
#define RES_M_MAX_RANGE_VAL            5 /* Maximum threshold value. */
#define RES_O_RESET_MEASURED_VAL       6 /* Reset minimum and maximum measured value. */
#define RES_M_SENSOR_STATE             7 /* Sensor state on/off. */
#define RES_O_MEAN_VAL                 8 /* Mean measured value */
#define RES_O_SNSR_VALUES              9

#define RES_M_DIG_INPUT_STATE                  0
#define RES_O_DIG_INPUT_COUNTER                1
#define RES_O_RESET_DIG_INPUT_COUNTER          2
#define RES_O_SENSOR_TYPE                      3
#define RES_O_BUSY_TO_CLEAR_DELAY              4
#define RES_O_CLEAR_TO_BUSY_DELAY              5
#define RES_O_SUP_SENSOR_TYPE                  6

#define LWM2M_GENERIC_SENSOR_OBJECT_ID 1001
#define LWM2M_PRESENCE_SENSOR_OBJECT_ID 1002
#define PRV_MAX_STRSIZE                255

#define URL_INFO_BUFFER_SIZE  255

/**
 * @brief Describes the Generic sensor object resources
 *
 */
typedef struct _ext_obj_t
{
  struct _ext_obj_t *next;        /**< corresponding to lwm2m_list_t::next */
  uint16_t           id;          /**< corresponding to lwm2m_list_t::id  */
  float sen_val;                  /**< Sensor value (Mandatory) */
  char sen_unit[PRV_MAX_STRSIZE]; /**< Sensor unit (Optional) */
  float min_measured_val;         /**< Min measured value (Optional) */
  float max_measured_val;         /**< Max measured value (Optional) */
  float min_range_val;            /**< Min threshold value (Mandatory) */
  float max_range_val;            /**< Max threshold value (Mandatory) */
  bool sen_state;                 /**< Sensor state (Mandatory) */
  int16_t mean_val;               /**< Mean calculated value (Optional) */
}ext_obj_t;

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

typedef enum
{
  URI_DEPTH_OBJECT,
  URI_DEPTH_OBJECT_INSTANCE,
  URI_DEPTH_RESOURCE,
  URI_DEPTH_RESOURCE_INSTANCE
}qapi_Net_LWM2M_Uri_Depth_t;

typedef struct qapi_Net_LWM2M_Uri_s
{
  uint32_t    flag;           // indicates which segments are set
  uint16_t    objectId;
  uint16_t    instanceId;
  uint16_t    resourceId;
  uint16_t    resourceInstId;
}qapi_Net_LWM2M_Uri_t;

typedef struct qapi_Net_LWM2M_Observe_info_s
{
  struct qapi_Net_LWM2M_Observe_info_s *next;
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
}qapi_Net_LWM2M_Observe_info_t;

typedef struct qapi_Net_LWM2M_Observed_s
{
  struct qapi_Net_LWM2M_Observed_s *next;
  qapi_Net_LWM2M_Uri_t uri;
  qapi_Net_LWM2M_Observe_info_t *observe_info;
}qapi_Net_LWM2M_Observed_t;

typedef struct qapi_Net_LWM2M_Pending_Observed_s
{
  struct qapi_Net_LWM2M_Pending_Observed_s *next;
  qapi_Net_LWM2M_Observe_info_t *observe_info;
  qapi_Net_LWM2M_Uri_t uri;
  void *message;
}qapi_Net_LWM2M_Pending_Observed_t;

typedef struct ext_resource_instance_int_list_s
{
  struct ext_resource_instance_int_list_s*   next; 
  uint16_t                resInstId;  
  uint16_t                InstValue;
} ext_resource_instance_int_list_t;

typedef struct ext_resource_instance_string_list_s
{
  struct ext_resource_instance_string_list_s*   next; 
  uint16_t                resInstId;  
  char                    InstValue[MAX_STRING_LEN];
} ext_resource_instance_string_list_t;

typedef struct gen_multi_res_s
{
  struct gen_multi_res_s *next;
  uint16_t instanceId;
  ext_resource_instance_int_list_t*  sensors_val;
}gen_multi_res_t;

typedef struct pre_multi_res_s
{
  struct pre_multi_res_s *next;
  uint16_t instanceId;
  ext_resource_instance_string_list_t*  sensor_types;
}pre_multi_res_t;

typedef enum qapi_Net_LWM2M_Ext_App_Msg_e
{
  QAPI_NET_LWM2M_EXT_REGISTER_APP_E,
  QAPI_NET_LWM2M_EXT_DEREGISTER_APP_E,
  QAPI_NET_LWM2M_EXT_CONFIG_CLIENT_E,
  QAPI_NET_LWM2M_EXT_CREATE_OBJ_E,
  QAPI_NET_LWM2M_EXT_CREATE_OBJ_INSTANCE_E,
  QAPI_NET_LWM2M_EXT_DELETE_OBJ_INSTANCE_E,
  QAPI_NET_LWM2M_EXT_NOTIFY_E,
  QAPI_NET_LWM2M_EXT_SET_OBJ_LINK_E,
  QAPI_NET_LWM2M_EXT_GET_OBJ_LINK_E,
  QAPI_NET_LWM2M_EXT_SET_SRV_LIFETIME_E,
  QAPI_NET_LWM2M_EXT_GET_SRV_LIFETIME_E,
}qapi_Net_LWM2M_Ext_App_Msg_t;

typedef struct qapi_Net_LWM2M_Ext_Obj_s
{
  qapi_Net_LWM2M_App_Handler_t handler;
  qapi_Net_LWM2M_Server_Data_t *lwm2m_srv_data;
  void                         *data;
}qapi_Net_LWM2M_Ext_Obj_t;

typedef enum {
  QAPI_COAP_GET = 1,
  QAPI_COAP_POST,
  QAPI_COAP_PUT,
  QAPI_COAP_DELETE
}qapi_coap_method_t ;

typedef struct qapi_Net_LWM2M_Ext_App_s
{
  uint8_t id;
  qapi_Net_LWM2M_Ext_App_Msg_t msg_type;
  qapi_Net_LWM2M_Obj_Info_t obj_info;
  uint8_t url_info[URL_INFO_BUFFER_SIZE];
  uint32_t lifetime; 
}qapi_Net_LWM2M_Ext_App_t;

typedef struct qapi_Net_LWM2M_Ext_s
{
  union
  {
    qapi_Net_LWM2M_Ext_Obj_t dl_op;
    qapi_Net_LWM2M_Ext_App_t ul_op;
  }app_data;
}qapi_Net_LWM2M_Ext_t;

QCLI_Command_Status_t init_ext_obj(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t config_client_ext_obj(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t create_ext_obj(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t create_ext_obj_inst(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t delete_ext_obj_inst(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t set_obj_link(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t get_obj_link(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t set_server_lifetime (uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t get_server_lifetime (uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t ext_read(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t ext_write(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t ext_exec(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t display_template(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t deinit_ext_obj(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t res_inst_create(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t res_inst_delete(uint32_t Parameter_Count,QCLI_Parameter_t *Parameter_List);


qapi_Status_t create_object_instance_app(qapi_Net_LWM2M_Ext_t *obj_buffer);

qapi_Status_t delete_object_instance_app(qapi_Net_LWM2M_Ext_t *obj_buffer);

qapi_Status_t set_obj_link_app(qapi_Net_LWM2M_Ext_t *obj_buffer);

qapi_Status_t get_obj_link_app(qapi_Net_LWM2M_Ext_t *obj_buffer);

qapi_Status_t set_server_lifetime_app(qapi_Net_LWM2M_Ext_t *obj_buffer);

qapi_Status_t get_server_lifetime_app(qapi_Net_LWM2M_Ext_t *obj_buffer);

qapi_Status_t create_object(int id);

qapi_Status_t ext_object_register(int id);

qapi_Status_t ext_object_deregister(int id);

qapi_Net_LWM2M_Instance_Info_t *find_object_instance(uint16_t object_id, uint8_t instance_id);

qapi_Net_LWM2M_Resource_Info_t *find_resource_info(uint16_t object_id, uint8_t instance_id,
                                                   uint8_t resource_id);

qapi_Status_t create_object_instance(qapi_Net_LWM2M_App_Handler_t handler, 
                                     qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t delete_object_instance(qapi_Net_LWM2M_App_Handler_t handler, 
                                     qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t read_ext_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                           qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t write_ext_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                            qapi_Net_LWM2M_Server_Data_t op_data,
                            uint8_t msg_code);

qapi_Status_t exec_ext_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                           qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t disc_ext_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                           qapi_Net_LWM2M_Server_Data_t op_data);

void send_response_message(qapi_Net_LWM2M_App_Handler_t handler,
                           qapi_Net_LWM2M_Server_Data_t op_data,
                           qapi_Net_LWM2M_Response_Code_t status);

qapi_Status_t ext_obj_handle_event(qapi_Net_LWM2M_App_Handler_t handler, 
                                   qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t ext_obj_delete_all(qapi_Net_LWM2M_App_Handler_t handler, 
                                 qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t start_app(int id);

qapi_Status_t read_data_ext(uint16_t instanceid, int *size, qapi_Net_LWM2M_Flat_Data_t **data,
                            qapi_Net_LWM2M_Data_t *object, uint16_t res_inst_id);

qapi_Status_t observe_cancel_ext_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                                     qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t ext_object_write_attr(qapi_Net_LWM2M_App_Handler_t handler, 
                                    qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t observe_ext_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                              qapi_Net_LWM2M_Server_Data_t op_data);

uint8_t ext_object_set_observe_param(qapi_Net_LWM2M_App_Handler_t handler,
                                     qapi_Net_LWM2M_Server_Data_t op_data);

void ext_object_notify(int id,
                       time_t current_time,
                       time_t *timeout);

qapi_Status_t ext_object_configclient(int id);

void stop_ext_obs_notify_timer(qapi_Net_LWM2M_App_Handler_t handler);

int ext_obj_discover_serialize(qapi_Net_LWM2M_App_Handler_t handler,
                               qapi_Net_LWM2M_Server_Data_t op_data,
                               qapi_Net_LWM2M_Uri_t *uri,
                               int size,
                               qapi_Net_LWM2M_Flat_Data_t *data,
                               uint8_t **buffer);

void observe_delete_ext_handle_req(qapi_Net_LWM2M_App_Handler_t handler,
                                   qapi_Net_LWM2M_Uri_t *uri);


