/*!
  @file
  multi_app_test.h
  @brief
  Multi App test application functionality to interact with underlying 
  data control plane. 
*/

 
/*===========================================================================
 
   Copyright (c) 2017-2019 Qualcomm Technologies, Inc. All Rights Reserved
 
   Qualcomm Technologies Proprietary
 
   Export of this technology or software is regulated by the U.S. Government.
   Diversion contrary to U.S. law prohibited.
 
   All ideas, data and information contained in or disclosed by
   this document are confidential and proprietary information of
   Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
   By accepting this material the recipient agrees that this material
   and the information contained therein are held in confidence and in
   trust and will not be used, copied, reproduced in whole or in part,
   nor its contents revealed in any manner to others without the express
   written permission of Qualcomm Technologies, Inc.
 
===========================================================================*/

#include "stdlib.h"
#include "qurt.h"
#include "qurt_signal.h"
#include "qurt_thread.h"
#include "qurt_error.h"
#include "qurt_mutex.h"
#include "err.h"
#include "rcinit.h"
#include "queue.h"
#include "qcli.h"
#include "qcli_api.h"
#include "msgcfg.h"

#include "log_intf.h"
#include "qapi_lwm2m.h"
#define NO_OF_APPS 2

#define MAXIMUM_NUMBER_OF_PARAM 20

/*------------------------------------------------------------------------------
  Typedef Declaration
------------------------------------------------------------------------------*/
typedef qurt_signal_t multi_app_signal_t;

/*------------------------------------------------------------------------------
  This is the command queue that will be used to enqueue commands from
  external entities
------------------------------------------------------------------------------*/
typedef q_type multi_app_task_cmd_q;

/* struct declarations in MultiApp */

typedef enum
{
  MULTI_APP_MIN_CMD_EVT,
  PING_EVT,
  AUTOIP_EVT,
  IFCONFIG_EVT,
  PING6_EVT,
  SET_IPV6_ROUTER_PREFIX_EVT,
  DNSC_EVT,
  CFG_NETBUF_EVT,
  CERT_COMMAND_HANDLER_EVT,
  SSL_COMMAND_HANDLER_EVT,
  BENCHRX_EVT,
  BENCHTX_EVT,
  UAPSDTEST_EVT,
  HTTPC_COMMAND_HANDLER_EVT,
  SERVER_EVT,
  SERVER6_EVT,
  UDP_SERVER_EVT,
  ROUTE_EVT,
  ROUTE6_EVT,
  MQTT_COMMAND_HANDLER_EVT,
  DSS_LIB_INIT_EVT,
  DSS_GET_CALL_HANDLE_EVT,
  DSS_RELEASE_CALL_HANDLE_EVT,
  DSS_TEST_SET_PARAM_EVT,
  DSS_TEST_NET_UP_EVT,
  DSS_TEST_NET_DOWN_EVT,
  DSS_TEST_LIST_HANDLE_EVT,
  DSS_TEST_RESET_PKT_STATS_EVT,
  DSS_TEST_GET_PKT_STATS_EVT,
  DSS_TEST_GET_CALL_END_REASON_EVT,
  DSS_TEST_GET_CALL_TECH_EVT,
  DSS_TEST_GET_CURRENT_DATA_BEARER_TECH_EVT,
  DSS_TEST_GET_DEVICE_NAME_EVT,
  DSS_TEST_GET_QMI_PORT_NAME_EVT,
  DSS_TEST_GET_IP_ADDR_COUNT_EVT,
  DSS_TEST_GET_IP_ADDR_EVT,
  DSS_TEST_GET_LINK_MTU_EVT,
  DSS_TEST_SEND_NIPD_DATA_EVT,
  DSS_TEST_ADD_FILTERS_EVT,
  DSS_TEST_REMOVE_FILTERS_EVT,
  EXT_OBJ_APP_ORIGINATED_EVT,
  EXT_OBJ_CALL_BACK_EVT,
  LWM2M_FW_APP_START_EVT,
  LWM2M_FW_UPDATE_RESULT_EVT,
  LWM2M_FW_SET_PROTOCOL_EVT,
  LWM2M_FW_SET_DELIVERY_METHOD_EVT,
  LWM2M_FW_SET_PKG_VERSION_EVT,
  LWM2M_DEVICECAP_APP_START_EVT,
  LWM2M_DEVICECAP_CREATE_INSTANCE_EVT,
  LWM2M_DEVICECAP_DELETE_INSTANCE_EVT,
  LWM2M_DEVICECAP_OBSERVE_EVT,
  LWM2M_DEVICECAP_CANCEL_OBSERVE_EVT,
  LWM2M_DEVICECAP_SET_NOTIFY_EVT,
  LWM2M_DEVICECAP_APP_STOP_EVT,
  LWM2M_SW_MGNT_APP_START_EVT,
  LWM2M_SW_MGNT_UPDATE_RESULT_EVT,
  LWM2M_SW_MGNT_DELETE_INSTANCE_EVT,
  LWM2M_LOCATION_APP_START_EVT,
  LWM2M_LOCATION_APP_SET_RADIUS_EVT,
  LWM2M_DEVICE_APP_START_EVT,
  LWM2M_DEVICE_APP_SET_RESOURCE_EVT,
  LWM2M_DECONFIG_EVT,
  LWM2M_SW_MGNT_GET_EVT,
  LWM2M_DEVICECAP_GET_EVT,
  ATFWD_EVT,
  EXT_OBJ_APP_ORIGINATED_EVT_v2,
  EXT_OBJ_CALL_BACK_EVT_v2,
  LWM2M_FW_APP_START_EVT_v2,
  LWM2M_FW_UPDATE_RESULT_EVT_v2,
  LWM2M_FW_SET_PROTOCOL_EVT_v2,
  LWM2M_FW_SET_DELIVERY_METHOD_EVT_v2,
  LWM2M_FW_SET_PKG_VERSION_EVT_v2,
  LWM2M_DEVICECAP_APP_START_EVT_v2,
  LWM2M_DEVICECAP_CREATE_INSTANCE_EVT_v2,
  LWM2M_DEVICECAP_DELETE_INSTANCE_EVT_v2,
  LWM2M_DEVICECAP_OBSERVE_EVT_v2,
  LWM2M_DEVICECAP_CANCEL_OBSERVE_EVT_v2,
  LWM2M_DEVICECAP_SET_NOTIFY_EVT_v2,
  LWM2M_DEVICECAP_APP_STOP_EVT_v2,
  LWM2M_SW_MGNT_APP_START_EVT_v2,
  LWM2M_SW_MGNT_UPDATE_RESULT_EVT_v2,
  LWM2M_SW_MGNT_DELETE_INSTANCE_EVT_v2,
  LWM2M_DEVICE_APP_START_EVT_v2,
  LWM2M_DEVICE_APP_SET_RESOURCE_EVT_v2,
  LWM2M_SW_MGNT_GET_EVT_v2,
  LWM2M_DEVICECAP_GET_EVT_v2,
  DEVICE_INFO_COMMAND_HANDLER_EVT,
  PING3_EVT,
  PING6_3_EVT,
  NIPD_DATA_TX_EVT,
  NIPD_DATA_RX_EVT,
  LWM2M_CELL_CON_CALL_BACK_EVT,
  LWM2M_CELL_CON_APP_START_EVT_v2,
  LWM2M_CELL_CON_APP_STOP_EVT_v2,
  LWM2M_CELL_CON_CREATE_INSTANCE_EVT_v2,
  LWM2M_CELL_CON_DELETE_INSTANCE_EVT_v2,
  LWM2M_CELL_CON_GET_EVT_v2,
  LWM2M_CELL_CON_SET_EVT_v2,
  LWM2M_APN_INFO_CALL_BACK_EVT,
  LWM2M_APN_INFO_APP_START_EVT_v2,
  LWM2M_APN_INFO_APP_STOP_EVT_v2,
  LWM2M_APN_INFO_CREATE_INSTANCE_EVT_v2,
  LWM2M_APN_INFO_DELETE_INSTANCE_EVT_v2,
  LWM2M_APN_INFO_GET_EVT_v2,
  LWM2M_APN_INFO_SET_EVT_v2,
  SOCK_OPT_SET_EVT,
  COAP_COMMAND_HANDLER_EVT,
  BENCHTOOL_CONFIG_EVT,
  DSS_TEST_SET_TRANSFER_STAT_EVT,
  VERSION_COMMAND_HANDLER_EVT,
  DSS_TEST_SET_TRANSFER_STAT_EVT_v2,
  MULTI_APP_MAX_CMD_EVT
}multi_app_cmd_id_e;

/*------------------------------------------------------------------------------
  Structure Declaration
------------------------------------------------------------------------------*/

typedef struct
{
  multi_app_signal_t              multi_app_signal;
  qurt_mutex_t                  multi_app_mutex;
  q_type*                       cmd_q_ptr;
}multi_app_info_t;


typedef struct{
  q_link_type                     link;
}multi_app_cmd_hdr_t;

typedef struct{
  int app_id;
  uint32_t Parameter_Count;
  QCLI_Parameter_t Parameter_List[MAXIMUM_NUMBER_OF_PARAM];
  void * data;
  multi_app_cmd_id_e cmd_id;
  QCLI_Command_t *Command;
  uint32_t Command_Index;
}multi_app_cmd_data_t;


typedef struct{
  multi_app_cmd_hdr_t               cmd_hdr;
  multi_app_cmd_data_t              cmd_data;
} multi_app_cmd_t;

typedef struct{
  multi_app_cmd_data_t app_data1;
  multi_app_cmd_data_t app_data2;
  multi_app_cmd_id_e cmd_id;
  QCLI_Command_t *Command;
  uint32_t Command_Index;
}multi_app_cmd_parser_t;

/**
* @brief  LWM2M event information.passed to stub applications
*/
typedef enum Std_Obj_LWM2M_Event_e
{
  LWM2M_DEVICE_REBOOT_E                 ,   /**< Device reboot event. */
  LWM2M_DEVICE_FACTORY_RESET_E          ,   /**< Device factory reset event. */
  LWM2M_DEVICE_REBOOTSTRAPPING_E        ,   /**< Device rebootstrapping event. */
  LWM2M_DELETE_ALL_REQ_E                ,   /**< Delete all event. */
  LWM2M_SLEEP_E                         ,  /**< Sleep event. */
  LWM2M_WAKEUP_E                        ,  /**< Wake-up event. */
  LWM2M_CLIENT_RESET_E                  ,  /**< Reset event. */
  LWM2M_DELETE_REQ_E                    ,  /**< Delete request event. */
} Std_Obj_LWM2M_Event_t;

typedef struct{
  void *handler;
  Std_Obj_LWM2M_Event_t event;
  qapi_Net_LWM2M_Server_Data_t *srv_data;
}std_obj_cb_data_t;

/*------------------------------------------------------------------------------
  Signal Related Macros
------------------------------------------------------------------------------*/
#define MULTI_APP_SIGNAL_MASK       0x00000001

#define MULTI_APP_TASK_IS_SIG_SET(signal, signal_mask)  ((signal & signal_mask) != 0)

#define MULTI_APP_TASK_CLEAR_SIG(signal, signal_mask) \
                                    qurt_signal_clear(&signal,signal_mask)

#define MULTI_APP_TASK_SET_SIG(signal, signal_mask) \
                                    qurt_signal_set(&signal, signal_mask)

/*------------------------------------------------------------------------------
  Command Queue Related Macros
------------------------------------------------------------------------------*/

#define MULTI_APP_CMD_ID_RANGE_CHECK(cmd_id) \
                                       ( (cmd_id > MULTI_APP_MIN_CMD_EVT) && \
                                         (cmd_id < MULTI_APP_MAX_CMD_EVT) )

/* Thread 1 */

void multi_app_set_signal_t1(void);

void multi_app_mutex_init_t1(void);

void multi_app_signal_init_t1(void);

void multi_app_signal_wait_t1(void);

void multi_app_task_init_t1(void);

void multi_app_task_entry_t1(void *thread_ctxt);

void multi_app_cmdq_init_t1 (multi_app_info_t * multi_app_state);

void multi_app_enqueue_cmd_t1(multi_app_cmd_t* cmd_ptr);

multi_app_cmd_t* multi_app_dequeue_cmd_t1( void );

void multi_app_process_commands_t1(void);

/* Thread 2 */

void multi_app_set_signal_t2(void);

void multi_app_mutex_init_t2(void);

void multi_app_signal_init_t2(void);

void multi_app_signal_wait_t2(void);

void multi_app_task_init_t2(void);

void multi_app_task_entry_t2(void *thread_ctxt);

void multi_app_cmdq_init_t2(multi_app_info_t * multi_app_state);

void multi_app_enqueue_cmd_t2(multi_app_cmd_t* cmd_ptr);

multi_app_cmd_t* multi_app_dequeue_cmd_t2( void );

multi_app_cmd_t* multi_app_get_cmd_buffer(void);

void multi_app_process_commands_t2(void);

multi_app_cmd_parser_t* multi_app_cmd_parser_func(uint32_t Parameter_Count, 
                                            QCLI_Parameter_t *Parameter_List, 
                                            boolean is_lwm2m_ext_cmd);
                                            /* is_lwm2m_ext_cmd is true when the command being 
                                             * processed is related to LWM2M_Ext and false otherwise*/

QCLI_Command_Status_t multi_app_cmd_fill_func(multi_app_cmd_parser_t *cmd_parser_temp);

QCLI_Command_Status_t multi_app_cmd_queue_func(multi_app_cmd_t* cmd_ptr);

/* dss_netapp */

QCLI_Command_Status_t proc_dss_lib_init(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_get_call_handle(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_release_call_handle(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_test_set_param(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_test_net_up(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_test_net_down(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_test_list_handle(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_test_reset_pkt_stats(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_test_get_pkt_stats(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_test_get_call_end_reason(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_test_get_call_tech(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_test_get_current_data_bearer_tech(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_test_get_device_name(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_test_get_qmi_port_name(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_test_get_ip_addr_count(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_test_get_ip_addr(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_test_get_link_mtu(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_test_send_nipd_data(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_test_add_filters(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_test_remove_filters(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_nipd_data_tx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);
QCLI_Command_Status_t proc_nipd_data_rx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dss_set_data_transfer_status(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);
QCLI_Command_Status_t proc_dss_set_data_transfer_status_v2(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);


/* Net_Cmd */

QCLI_Command_Status_t proc_server(   uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_route6(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_route(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_server6(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_ping(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_autoip(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_ifconfig(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_ping6(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_dnsc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_set_ipv6_router_prefix(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_cfg_netbuf(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_udp_server(    uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_httpc_command_handler(    uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_benchtx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_benchrx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_cert_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);
QCLI_Command_Status_t proc_ssl_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_mqtt_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_coap_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);


QCLI_Command_Status_t proc_device_info_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_ping3(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_ping6_3(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_set_sock_opt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_benchtoolconfig(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

/* lwm2m */

QCLI_Command_Status_t proc_fw_update_result(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_fw_set_protocol(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_fw_set_delivery_method(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_fw_app_start(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);	

QCLI_Command_Status_t proc_devicecap_app_start(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_sw_mgnt_app_start(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_sw_mgnt_update_result(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_sw_mgnt_app_start(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_sw_mgnt_delete_instance(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_devicecap_delete_instance(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_devicecap_create_instance(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_location_app_start(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_location_app_set_radius(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_devicecap_app_stop(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_devicecap_cancel_observe(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_devicecap_observe(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_devicecap_set_notify(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_device_app_set_resource(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_device_app_start(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_fw_set_pkg_version(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_devicecap_get(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_sw_mgnt_get(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_fw_update_result_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_fw_set_protocol_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_fw_set_delivery_method_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_fw_app_start_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);	

QCLI_Command_Status_t proc_devicecap_app_start_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_sw_mgnt_app_start_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_sw_mgnt_update_result_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_sw_mgnt_app_start_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_sw_mgnt_delete_instance_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_devicecap_delete_instance_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_devicecap_create_instance_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_devicecap_app_stop_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_devicecap_cancel_observe_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_devicecap_observe_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_devicecap_set_notify_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_device_app_set_resource_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_device_app_start_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_fw_set_pkg_version_v2(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

QCLI_Command_Status_t proc_devicecap_get_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_sw_mgnt_get_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_cell_nw_app_start_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_cell_nw_app_stop_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_cell_nw_get_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_cell_nw_create_instance_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_cell_nw_delete_instance_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_cell_nw_set_resource_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);


qapi_Status_t cell_con_handle_event(void *data);


/* atfwd */

QCLI_Command_Status_t proc_atfwd(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);
QCLI_Command_Status_t proc_version_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);


/* APN connection profile */
#ifdef LWM2M_ENABLE_APN_OBJ

QCLI_Command_Status_t proc_apninfo_app_start_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_apninfo_app_stop_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_apninfo_get_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_apninfo_create_instance_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_apninfo_delete_instance_v2(uint32_t Parameter_Count, QCLI_Parameter_t* Parameter_List, int app_id);

QCLI_Command_Status_t proc_apninfo_set_resource_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id);

qapi_Status_t apn_info_handle_event(void *data);

#endif

