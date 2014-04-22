#ifndef LWM2M_AT_HANDLER_H
#define LWM2M_AT_HANDLER_H
#ifdef LWM2M_AT_ENABLE
/*===========================================================================
                         LWM2M_AT_HANDLER.H

DESCRIPTION
  This header file contains definitions used internally by the lwm2m at handler  
  Module.

  Copyright (c)2018 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR FILE
when        who    what, where, why
--------    ---    -------------------------------------------------------------
06/14/2018   xianma     created file
==============================================================================*/

/*------------------------------------------------------------------------------
  MACRO declaration
------------------------------------------------------------------------------*/
#include "qapi_lwm2m.h"
#include "at_handler_main.h"
#include "at_handler_utility.h"
//AT Handler max parameter

#define LWM2M_AT_LWM2M_TYPE_ATTR QAPI_NET_LWM2M_TYPE_OBJECT_LINK+1 //input value type for lwm2m attributes
#define LWM2M_AT_LWM2M_ACK_MESSAGE_E 17 //Ack Message event type used to in event map

//LwM2M defines
#define LWM2M_AT_STR_MAX_LEN 50
#define LWM2M_AT_URL_INFO_SIZE  255
#define LWM2M_AT_PAYLOAD_MAX_LEN 50



/*------------------------------------------------------------------------------
  Structure declaration
------------------------------------------------------------------------------*/
typedef struct lwm2m_at_event_map_s
{/*Structure to save event map, which map event id with event string*/
  uint8 event_id;
  const char * event_string;
}lwm2m_at_event_map_t;

typedef struct lwm2m_at_req_map_s
{/*Structure to save request map, which map request id with request string*/
  uint8 req_id;
  const char * req_string;
} lwm2m_at_req_map_t;

typedef struct lwm2m_at_req_store_s
{/*Structure to store lwm2m server request information*/
  qapi_Net_LWM2M_DL_Msg_t msg_type;
  uint8_t msg_id_len;
  uint8_t msg_id[QAPI_MAX_LWM2M_MSG_ID_LENGTH];
  uint32_t observation_seq_num; /**< Observation sequence number. */
  uint16_t notification_id;
  uint8_t conf_msg;        /**< Confirmable (ACK) or nonconfirmable application response/notifications. */
  uint16_t conf_id;
  qapi_Net_LWM2M_Obj_Info_t * id_info;
  bool accept_is_valid; /**< Flag to check accept field is set or not. */
  qapi_Net_LWM2M_Content_Type_t accept; /**< Intended data payload content type. */
}lwm2m_at_req_store_t;

//LwM2M structure
typedef struct lwm2m_cb_ext_data_s
{/*Structure to save parameter provided by user_cb_fn()*/
  qapi_Net_LWM2M_App_Handler_t handler;
  qapi_Net_LWM2M_Server_Data_t *lwm2m_srv_data;
  void                         *data;
}lwm2m_cb_ext_data_t;

typedef struct lwm2m_cb_observe_data_s
{/*Structure to save parameter provided by observe_cb_fn()*/
  qapi_Net_LWM2M_App_Handler_t handler;
  char * oberve_string;
}lwm2m_cb_observe_data_t;

//Used to check if standard object, need to keep align with lwM2M client, and change it if new std object is added
#define LWM2M_AT_IS_STANDARD_OBJECT(object_id) ((((int32_t)object_id >= 0x00) && \
                                                (object_id <= 0x07)) || \
                                              (QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E == object_id) || \
                                              (QAPI_NET_LWM2M_CELL_NW_CONN_OBJECT_ID_E == object_id) || \
                                              (QAPI_NET_LWM2M_APN_CONN_OBJECT_ID_E == object_id) || \
                                              (QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E == object_id))


//M2MSETMSGDATA store structure
typedef struct lwm2m_at_M2MSETMSGDATA_s
{
  uint16 req_id;
  qapi_Net_LWM2M_Obj_Info_t obj_info;
  qapi_Net_LWM2M_Attributes_t * lwm2m_attr; //Attribute list
  qapi_Net_LWM2M_Flat_Data_t * lwm2m_flat_data; //Flat Data root
  qapi_Net_LWM2M_Content_Type_t enc_content_type;
  int last_more_value_tbc; //last inputed input_value_tbc
  int last_input_type; //last inputed value type
}lwm2m_at_M2MSETMSGDATA_t;

//M2MSET store structure
typedef struct lwm2m_at_M2MSET_s
{
  qapi_Net_LWM2M_Data_v2_t * obj_data_ptr;
  qapi_Net_LWM2M_Obj_Info_t id_info; //Last inputed obj_info
  int last_value_tbc; //last inputed value to be continue
  int last_type; //last inputed value type
}lwm2m_at_M2MSET_t;

//M2MIDMAPCFG store structure
typedef struct lwm2m_at_M2MIDMAPCFG_s
{
  uint8 * line_buf;
  size_t buf_len;
  int last_buf_tbc; //last inputed buf_to_be_come
  //int last_line_tbc; //last inputed line to be come
}lwm2m_at_M2MIDMAPCFG_t;

/*------------------------------------------------------------------------------
  Function declaration
------------------------------------------------------------------------------*/
/*Callback handle functions*/
atfwd_result_code_t lwm2m_cb_extended_evt_hdl(qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Server_Data_t op_data);
atfwd_result_code_t lwm2m_cb_extended_req_hdl(qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Server_Data_t op_data);
atfwd_result_code_t lwm2m_cb_observe_hdl(qapi_Net_LWM2M_App_Handler_t handler, char ** observe_string);

/*AT cmd table register*/
boolean lwm2m_at_cmdtbl_reg(void);
#else
#include "qapi_lwm2m.h"
#include "at_handler_main.h"

//LwM2M structure
typedef struct lwm2m_cb_ext_data_s
{/*Structure to save parameter provided by user_cb_fn()*/
  qapi_Net_LWM2M_App_Handler_t handler;
  qapi_Net_LWM2M_Server_Data_t *lwm2m_srv_data;
  void                         *data;
}lwm2m_cb_ext_data_t;

typedef struct lwm2m_cb_observe_data_s
{/*Structure to save parameter provided by observe_cb_fn()*/
  qapi_Net_LWM2M_App_Handler_t handler;
  char * oberve_string;
}lwm2m_cb_observe_data_t;

/*AT cmd table register*/
boolean lwm2m_at_cmdtbl_reg(void);
/*Callback handle functions*/
atfwd_result_code_t lwm2m_cb_extended_evt_hdl(qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Server_Data_t op_data);
atfwd_result_code_t lwm2m_cb_extended_req_hdl(qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Server_Data_t op_data);
atfwd_result_code_t lwm2m_cb_observe_hdl(qapi_Net_LWM2M_App_Handler_t handler, char ** observe_string);

#endif /*LWM2M_AT_ENABLE*/
#endif
