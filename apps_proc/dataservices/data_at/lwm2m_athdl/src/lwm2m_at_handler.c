/******************************************************************************

                        Lwm2m_at_handler . C 

******************************************************************************/

/******************************************************************************

  @file    lwm2m_at_handler.c
  @brief   lwm2m at command handler

  DESCRIPTION
  AT Handler work as the handler of QCT native lwM2M AT commands

  ---------------------------------------------------------------------------
  Copyright (c) 2017, 2018, 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/
#ifdef LWM2M_AT_ENABLE
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "lwm2m_at_handler.h"
#include "lwm2m_at_utility.h"
#include "lwm2m_id_mapping.h"

/*===========================================================================

                           Global Variables

===========================================================================*/

/*LwM2M AT globals*/
qapi_Net_LWM2M_App_Handler_t lwm2m_at_handle = NULL;

int32 lwm2m_at_event_enable = 1;
int32 lwm2m_at_event_format = 0;
int32 lwm2m_at_req_enable = 1;
int32 lwm2m_at_req_format = 0;

lwm2m_at_M2MSET_t * lwm2m_at_m2mset_ptr = NULL;
lwm2m_at_M2MSETMSGDATA_t * lwm2m_at_m2msetmsgdata_ptr = NULL;
util_at_list_t * lwm2m_at_req_store_hdr = NULL;

/*LwM2M maps*/
const lwm2m_at_event_map_t lwm2m_at_event_map[] =
{
  {QAPI_NET_LWM2M_STATE_INITIAL_E,                "State Initial"},
  {QAPI_NET_LWM2M_BOOTSTRAP_REQUIRED_E,           "Bootstrap Required"},
  {QAPI_NET_LWM2M_BOOTSTRAP_COMPLETED_E,          "Bootstrap Completed"},
  {QAPI_NET_LWM2M_BOOTSTRAP_FAILED_E,             "Bootstrap Failed"},
  {QAPI_NET_LWM2M_REGISTERTION_COMPELTED_E,       "Registration Completed"},
  {QAPI_NET_LWM2M_REGISTRATION_FAILED_E,          "Registration Failed"},
  {QAPI_NET_LWM2M_DEVICE_REBOOT_E,                "Device Reboot"},
  {QAPI_NET_LWM2M_DEVICE_FACTORY_RESET_E,         "Device Factory Reset"},
  {QAPI_NET_LWM2M_DEVICE_REBOOTSTRAPPING_E,       "Device Rebootstrapping"},
  {QAPI_NET_LWM2M_TX_MESSAGE_MAX_RETRY_FAILURE_E, "TX Message Max Retry Failure"},
  {QAPI_NET_LWM2M_RX_MESSAGE_INTERNAL_FAILURE_E,  "RX Message Internal Failure"},
  {QAPI_NET_LWM2M_SLEEP_E,                        "Sleep"},
  {QAPI_NET_LWM2M_WAKEUP_E,                       "Wakeup"},
  {QAPI_NET_LWM2M_CLIENT_RESET_E,                 "Client Reset"},
  {QAPI_NET_LWM2M_LIFETIME_UPDATE_E,              "Lifetime Update"},
  {QAPI_NET_LWM2M_REGISTER_UPDATE_E,              "Register Update"},
  {QAPI_NET_LWM2M_BOOTSTRAP_START_E,              "Bootstrap Start"},
  {LWM2M_AT_LWM2M_ACK_MESSAGE_E,                  "Ack Message"},
  {QAPI_NET_LWM2M_BLOCKING_REGISTRATION_FAILURE_E,"Blocking Registration Failed"}
};

const lwm2m_at_req_map_t lwm2m_at_req_map[] =
{
  {QAPI_NET_LWM2M_READ_REQ_E,                    "Read"},
  {QAPI_NET_LWM2M_WRITE_REPLACE_REQ_E,           "Write Replace"},
  {QAPI_NET_LWM2M_WRITE_PARTIAL_UPDATE_REQ_E,    "Write Partial Update"},
  {QAPI_NET_LWM2M_WRITE_ATTR_REQ_E,              "Write Attribute"},
  {QAPI_NET_LWM2M_DISCOVER_REQ_E,                "Discover"},
  {QAPI_NET_LWM2M_EXECUTE_REQ_E,                 "Execute"},
  {QAPI_NET_LWM2M_DELETE_REQ_E,                  "Delete"},
  {QAPI_NET_LWM2M_OBSERVE_REQ_E,                 "Observe"},
  {QAPI_NET_LWM2M_CANCEL_OBSERVE_REQ_E,          "Cancel Observe"},
  {QAPI_NET_LWM2M_ACK_MSG_E,                     "ACK MSG"}, //Place ocuppy
  {QAPI_NET_LWM2M_INTERNAL_CLIENT_IND_E,         "Internal Event"}, //Place ocuppy 
  {QAPI_NET_LWM2M_CREATE_REQ_E,                  "Create"},
  {QAPI_NET_LWM2M_DELETE_ALL_REQ_E,              "Delete All"}
};
/*ID mapping globals*/
lwm2m_at_M2MIDMAPCFG_t * lwm2m_at_m2midmapcfg_ptr = NULL;

/*At response buffer*/
char at_resp_str[ATHDL_RESP_LEN_MAX] = {0};
/*===========================================================================

                           Gobal Functions

===========================================================================*/

/**
 * @fn void lwm2m_util_reset_gobals()
 * @brief reset gobals and free memory
 * @return N.A
 */
void lwm2m_util_reset_gobals()
{
  lwm2m_util_m2mset_data_free(&lwm2m_at_m2mset_ptr);
  lwm2m_util_m2msetmsgdata_free(&lwm2m_at_m2msetmsgdata_ptr);
  lwm2m_util_req_store_free(&lwm2m_at_req_store_hdr);

  lwm2m_at_event_enable = 1;
  lwm2m_at_event_format = 0;
  lwm2m_at_req_enable = 1;
  lwm2m_at_req_format = 0;

  lwm2m_at_handle = NULL;
}
/*===========================================================================

                           Callback Functions

===========================================================================*/
/**
 * @fn qapi_Status_t lwm2m_cb_extended_enq()
 * @LwM2M callback send msg to AT handler thread
 * @param
 *     @param[in]  handler  handler
 *     @param[in]  lwm2m_srv_data  lwm2m server data
 *     @param[in]  user_data  user data
 * @return qapi_Status_t
 */
qapi_Status_t lwm2m_cb_extended_enq(qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Server_Data_t *lwm2m_srv_data, void *user_data)
{
  lwm2m_cb_ext_data_t *ext_param = NULL;
  qapi_Status_t result = QAPI_ERR_NO_MEMORY;

  ATFWD_LOG_MSG_MED("cb_extended_enq() in", 0, 0, 0);

  if(lwm2m_srv_data == NULL)
  {
    ATFWD_LOG_MSG_LOW("lwm2m_srv_data empty", 0, 0, 0);
    result = QAPI_ERR_INVALID_PARAM;
    goto end;
  }

  if((ext_param = (lwm2m_cb_ext_data_t *)athdl_malloc(sizeof(lwm2m_cb_ext_data_t))) == NULL)
  {
    ATFWD_LOG_MSG_LOW("Cannot obtain ext_param buffer", 0, 0, 0);
    goto end;
  }

  if((ext_param->lwm2m_srv_data = (qapi_Net_LWM2M_Server_Data_t *) athdl_malloc(sizeof(qapi_Net_LWM2M_Server_Data_t))) == NULL)
  {
    ATFWD_LOG_MSG_LOW("Cannot obtain lwm2m_srv_data buffer", 0, 0, 0);
    goto end;
  }

  ext_param->handler = handler;
  memscpy(ext_param->lwm2m_srv_data,sizeof(qapi_Net_LWM2M_Server_Data_t),lwm2m_srv_data,sizeof(qapi_Net_LWM2M_Server_Data_t));
  if(lwm2m_srv_data->lwm2m_attr != NULL)
  {
    if((ext_param->lwm2m_srv_data->lwm2m_attr = (qapi_Net_LWM2M_Attributes_t *) athdl_malloc(sizeof(qapi_Net_LWM2M_Attributes_t))) == NULL)
    {
      ATFWD_LOG_MSG_LOW("Cannot obtain lwm2m_attr buffer", 0, 0, 0);
      goto end;
    }
    memscpy(ext_param->lwm2m_srv_data->lwm2m_attr,sizeof(qapi_Net_LWM2M_Attributes_t),lwm2m_srv_data->lwm2m_attr,sizeof(qapi_Net_LWM2M_Attributes_t));
  }

  if(lwm2m_srv_data->payload_len != 0)
  {
    if((ext_param->lwm2m_srv_data->payload = (uint8_t *)athdl_malloc((sizeof(uint8_t) * lwm2m_srv_data->payload_len))) == NULL)
    {
      ATFWD_LOG_MSG_LOW("Cannot obtain payload buffer", 0, 0, 0);
      goto end;
    }
    memscpy(ext_param->lwm2m_srv_data->payload, (sizeof(uint8_t) * lwm2m_srv_data->payload_len), lwm2m_srv_data->payload, (sizeof(uint8_t) * lwm2m_srv_data->payload_len));
  }

  ext_param->data = user_data;
  result = athdl_fwd_evt_enq(DATA_ATFWD_ATHDL_LWM2M_EVT,(void *)ext_param);
end:
  if(result != QAPI_OK && ext_param)
  {
    if(ext_param->lwm2m_srv_data)
    {
      athdl_free(ext_param->lwm2m_srv_data->lwm2m_attr);
      athdl_free(ext_param->lwm2m_srv_data->payload);
      athdl_free(ext_param->lwm2m_srv_data);
    }
    athdl_free(ext_param);
  }
  ATFWD_LOG_MSG_MED("cb_extended_enq() out, msg to ATHDL, hdl [0x%x],QAPI res %d", handler, (int)result, 0);
  return result;
}
/**
 * @fn void lwm2m_cb_extended_evt_hdl()
 * @LwM2M callback handle lwM2M event
 * @param 
 *     @param[in]  handler  handler
 *     @param[in]  qapi_Net_LWM2M_Server_Data_t  op_data
 * @return void
 */
atfwd_result_code_t lwm2m_cb_extended_evt_hdl(qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Server_Data_t op_data)
{
  char * at_urc = NULL;
  lwm2m_at_req_store_t * match_ep = NULL;
  int conf_id = 0;
  atfwd_result_code_t result = ATFWD_RET_GLOBAL_INVALID;

  ATFWD_LOG_MSG_MED("cb_extended_evt_hdl() in", 0, 0, 0);

  if(handler == NULL | handler != lwm2m_at_handle)
  {
    ATFWD_LOG_MSG_LOW("wrong handle", 0, 0, 0);
    goto end;
  }

  if(op_data.msg_type == QAPI_NET_LWM2M_ACK_MSG_E)
  {
    match_ep = lwm2m_util_req_store_find_by_notifyId(op_data.notification_id);
    if(match_ep == NULL || match_ep->conf_msg != 1)
    {
      ATFWD_LOG_MSG_LOW("ACK_MSG notifyId %d, no match", op_data.notification_id, 0, 0);
      goto end;
    }
    ATFWD_LOG_MSG_LOW("ACK_MSG notifyId %d, match", op_data.notification_id, 0, 0);
    conf_id = (int)(match_ep->conf_id);
  }

  if(op_data.event == QAPI_NET_LWM2M_CLIENT_RESET_E || op_data.event == QAPI_NET_LWM2M_DEVICE_FACTORY_RESET_E)
  {
    lwm2m_util_reset_gobals();
  }

  if(lwm2m_at_event_enable)
  {
    if((at_urc = athdl_malloc(ATHDL_URC_LEN_MAX)) == NULL)
    {
      ATFWD_LOG_MSG_LOW("memory allocation failure for at_urc", 0, 0, 0);
      result = ATFWD_RET_MEMORY_ERR;
      goto end;
    }

    if(lwm2m_at_event_format == 0)
    {
      if(op_data.msg_type == QAPI_NET_LWM2M_ACK_MSG_E)
      {
        snprintf(at_urc, ATHDL_URC_LEN_MAX, "%d, %d\n", lwm2m_at_event_map[LWM2M_AT_LWM2M_ACK_MESSAGE_E].event_id, conf_id);
      }
      else
      {
        snprintf(at_urc, ATHDL_URC_LEN_MAX, "%d\n", lwm2m_at_event_map[op_data.event].event_id);
      }
    }
    else
    {
      if(op_data.msg_type == QAPI_NET_LWM2M_ACK_MSG_E)
      {
        snprintf(at_urc, ATHDL_URC_LEN_MAX, "\"%s\", %d\n", lwm2m_at_event_map[LWM2M_AT_LWM2M_ACK_MESSAGE_E].event_string, conf_id);
      }
      else
      {
        snprintf(at_urc, ATHDL_URC_LEN_MAX, "\"%s\"\n", lwm2m_at_event_map[op_data.event].event_string);
      }
    }

    qapi_atfwd_send_urc_resp("+M2MEVENT", at_urc);
    result = ATFWD_RET_OK;
    athdl_free(at_urc);
    at_urc = NULL;
  }

end:
  ATFWD_LOG_MSG_MED("cb_extended_evt_hdl() out, hdl [0x%x], res %d", handler, (int)result, 0);
  return result;
}
/**
 * @fn void lwm2m_cb_extended_req_hdl()
 * @LwM2M callback handle lwM2M request
 * @param 
 *     @param[in]  handler  handler
 *     @param[in]  qapi_Net_LWM2M_Server_Data_t  op_data
 * @return void
 */
atfwd_result_code_t lwm2m_cb_extended_req_hdl(qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Server_Data_t op_data)
{
  char * at_urc = NULL;
  char * urc_header = NULL;
  lwm2m_at_req_store_t * req_store_data = NULL;
  util_at_list_t * req_store_list_node = NULL;
  qapi_Net_LWM2M_Flat_Data_t * lwm2m_flat_data = NULL;
  size_t lwm2m_flat_data_len = 0;
  lwm2m_at_req_store_t * match_ep = NULL;
  uint16 req_id_random = 0;

  atfwd_result_code_t result = ATFWD_RET_MEMORY_ERR;

  ATFWD_LOG_MSG_MED("cb_extended_req_hdl() in", 0, 0, 0);

  if(handler == NULL | handler != lwm2m_at_handle)
  {
    ATFWD_LOG_MSG_LOW("wrong handle", 0, 0, 0);
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }

  if(lwm2m_at_req_enable == 0)
  {
    ATFWD_LOG_MSG_HIGH("Ext Req received, M2MREQ URC disabled", 0, 0, 0);
    if(!(op_data.msg_type == QAPI_NET_LWM2M_DELETE_ALL_REQ_E || (op_data.msg_type == QAPI_NET_LWM2M_CANCEL_OBSERVE_REQ_E && op_data.msg_id_len <= 0)))
    {
      lwm2m_util_send_error_response(lwm2m_at_handle, QAPI_NET_LWM2M_500_INTERNAL_SERVER_E, op_data.msg_id_len, op_data.msg_id, &(op_data.obj_info));
    }
    result = ATFWD_RET_OK;
    goto end;
  }

  do
  {
    req_id_random = util_at_rand16();
    ATFWD_LOG_MSG_LOW("util_at_rand16 generate id %d", req_id_random, 0, 0);
  }while(NULL != util_at_list_find(lwm2m_at_req_store_hdr, req_id_random));

  if(!(op_data.msg_type == QAPI_NET_LWM2M_DELETE_ALL_REQ_E || (op_data.msg_type == QAPI_NET_LWM2M_CANCEL_OBSERVE_REQ_E && op_data.msg_id_len <= 0)))
  {
  //Store LwM2M request
    if((req_store_list_node = athdl_malloc(sizeof(util_at_list_t))) == NULL)
    {
      ATFWD_LOG_MSG_LOW("memory allocation failure for req_store_list_node", 0, 0, 0);
      goto end;
    }
    req_store_list_node->next = NULL;

    if((req_store_data = athdl_malloc(sizeof(lwm2m_at_req_store_t))) == NULL)
    {
      ATFWD_LOG_MSG_LOW("memory allocation failure for req_store_data", 0, 0, 0);
      goto end;
    }
    req_store_data->msg_type = op_data.msg_type;
    req_store_data->msg_id_len = op_data.msg_id_len;
    lwm2m_util_msgid_copy(op_data.msg_id, req_store_data->msg_id, op_data.msg_id_len);

    req_store_data->accept_is_valid = op_data.accept_is_valid;
    req_store_data->accept = op_data.accept;
    ATFWD_LOG_MSG_HIGH("ext req, accept valid %d, accept type %d", op_data.accept_is_valid, op_data.accept,0);

    req_store_data->id_info = NULL;
    if((req_store_data->id_info = athdl_malloc(sizeof(qapi_Net_LWM2M_Obj_Info_t))) == NULL)
    {
      ATFWD_LOG_MSG_LOW("memory allocation failure for id_info", 0, 0, 0);
      goto end;
    }
    memscpy(req_store_data->id_info, sizeof(qapi_Net_LWM2M_Obj_Info_t), &(op_data.obj_info), sizeof(qapi_Net_LWM2M_Obj_Info_t));
    req_store_list_node->id = req_id_random;
    req_store_list_node->ptr = (void *)req_store_data;
    req_store_data = NULL;

    lwm2m_at_req_store_hdr = (util_at_list_t *)util_at_list_add(lwm2m_at_req_store_hdr, req_store_list_node);
    req_store_list_node = NULL;
  }
  //Build URC header
  if((urc_header = athdl_malloc(ATHDL_URC_LEN_MAX)) == NULL)
  {
    ATFWD_LOG_MSG_LOW("memory allocation failure for urc_header", 0, 0, 0);
    goto end;
  }
  if(lwm2m_at_req_format == 0)
  {
    snprintf(urc_header, ATHDL_URC_LEN_MAX, "%d, %d", lwm2m_at_req_map[op_data.msg_type - 1].req_id, (int)req_id_random);
  }
  else
  {
    snprintf(urc_header, ATHDL_URC_LEN_MAX, "\"%s\", %d", lwm2m_at_req_map[op_data.msg_type - 1].req_string, (int)req_id_random);
  }

  //Build URC
  if((at_urc = athdl_malloc(ATHDL_URC_LEN_MAX)) == NULL)
  {
    ATFWD_LOG_MSG_LOW("memory allocation failure for at_urc", 0, 0, 0);
    goto end;
  }
  switch(op_data.msg_type)
  {
    case QAPI_NET_LWM2M_READ_REQ_E:
    case QAPI_NET_LWM2M_DISCOVER_REQ_E:
    case QAPI_NET_LWM2M_OBSERVE_REQ_E:
    case QAPI_NET_LWM2M_CANCEL_OBSERVE_REQ_E:
    case QAPI_NET_LWM2M_DELETE_REQ_E:
    case QAPI_NET_LWM2M_CREATE_REQ_E:
    case QAPI_NET_LWM2M_WRITE_REPLACE_REQ_E:
    case QAPI_NET_LWM2M_WRITE_PARTIAL_UPDATE_REQ_E:
    case QAPI_NET_LWM2M_EXECUTE_REQ_E:
    {
      if((op_data.msg_type == QAPI_NET_LWM2M_WRITE_REPLACE_REQ_E || op_data.msg_type == QAPI_NET_LWM2M_WRITE_PARTIAL_UPDATE_REQ_E || op_data.msg_type == QAPI_NET_LWM2M_EXECUTE_REQ_E) &&
         (op_data.payload != NULL))
      {
        if(QAPI_OK != qapi_Net_LWM2M_Decode_Data(handler, &(op_data.obj_info), op_data.payload, op_data.payload_len, op_data.content_type, &lwm2m_flat_data, &lwm2m_flat_data_len))
        {
          ATFWD_LOG_MSG_ERROR("QAPI Decode fail ", 0, 0, 0);
          result= ATFWD_RET_QAPI_FAILED;
          goto end;
        }
        if((result = lwm2m_idmap_FlatData_map(lwm2m_flat_data, lwm2m_flat_data_len, op_data.obj_info.obj_id, LWM2M_IDMAP_MAP_TO_ORG)) != ATFWD_RET_OK)
        {
          ATFWD_LOG_MSG_LOW("FlatData map fail", 0, 0, 0);
          goto end;
        }
        if((result = lwm2m_idmap_ObjInfo_map(&(op_data.obj_info), LWM2M_IDMAP_MAP_TO_ORG)) != ATFWD_RET_OK)
        {
          ATFWD_LOG_MSG_LOW("ObjInfo map fail", 0, 0, 0);
          goto end;
        }
        snprintf(at_urc, ATHDL_URC_LEN_MAX, "%s, \"%d/%d/", urc_header, op_data.obj_info.obj_id, op_data.obj_info.obj_inst_id);
        if(TRUE != lwm2m_util_FlatData_array_display("+M2MREQ", at_urc, ATHDL_URC_LEN_MAX, lwm2m_flat_data, lwm2m_flat_data_len))
        {
          ATFWD_LOG_MSG_LOW("data_atfwd_athdl_display_flat_data  disp err ", 0, 0, 0);
          result= ATFWD_RET_FAILED;
          goto end;
        }
        athdl_free(at_urc);
        at_urc = NULL;
        break;
      }

      if(op_data.msg_type == QAPI_NET_LWM2M_CANCEL_OBSERVE_REQ_E)
      {
        if(op_data.msg_id_len > 0)
        {
          lwm2m_util_req_store_delete(&(op_data.obj_info), QAPI_NET_LWM2M_OBSERVE_REQ_E);
        }
        else
        {
          if((match_ep=lwm2m_util_req_store_find_by_notifyId(op_data.notification_id)) == NULL)
          {
            ATFWD_LOG_MSG_LOW("CANCEL_OBSERVE_REQ_E notifyId %d, no match", op_data.notification_id, 0, 0);
            result = ATFWD_RET_EP_NOT_FOUND;
            goto end;
          }
          ATFWD_LOG_MSG_LOW("CANCEL_OBSERVE_REQ notifyId %d, match", op_data.notification_id, 0, 0);
          if(match_ep->id_info == NULL)
          {
            ATFWD_LOG_MSG_LOW("no id_info", 0, 0, 0);
            result = ATFWD_RET_EP_INVALID;
            goto end;
          }
          memscpy(&(op_data.obj_info), sizeof(qapi_Net_LWM2M_Obj_Info_t), match_ep->id_info, sizeof(qapi_Net_LWM2M_Obj_Info_t));
          lwm2m_util_req_store_delete(match_ep->id_info, match_ep->msg_type);
          match_ep = NULL;
        }
      }

      if((result = lwm2m_idmap_ObjInfo_map(&(op_data.obj_info),LWM2M_IDMAP_MAP_TO_ORG)) != ATFWD_RET_OK)
      {
        ATFWD_LOG_MSG_LOW("ObjInfo map fail", 0, 0, 0);
        goto end;
      }
      if(op_data.obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E && op_data.obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E && op_data.obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E && op_data.obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E)
      {
        snprintf(at_urc, ATHDL_URC_LEN_MAX, "%s, \"%d/%d/%d/%d\"", urc_header, op_data.obj_info.obj_id, op_data.obj_info.obj_inst_id, op_data.obj_info.res_id, op_data.obj_info.res_inst_id);
      }
      else if(op_data.obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E && op_data.obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E && op_data.obj_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
      {
        snprintf(at_urc, ATHDL_URC_LEN_MAX, "%s, \"%d/%d/%d\"", urc_header, op_data.obj_info.obj_id, op_data.obj_info.obj_inst_id, op_data.obj_info.res_id);
      }
      else if(op_data.obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E && op_data.obj_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E)
      {
        snprintf(at_urc, ATHDL_URC_LEN_MAX, "%s, \"%d/%d\"", urc_header, op_data.obj_info.obj_id, op_data.obj_info.obj_inst_id);
      }
      else if(op_data.obj_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E)
      {
        snprintf(at_urc, ATHDL_URC_LEN_MAX, "%s, \"%d\"", urc_header, op_data.obj_info.obj_id);
      }
      else
      {
        ATFWD_LOG_MSG_LOW("obj_mask invalid", 0, 0, 0);
        result = ATFWD_RET_PARAM_ERR;
        goto end;
      }
      break;
    }
    case QAPI_NET_LWM2M_DELETE_ALL_REQ_E:
    {
      snprintf(at_urc, ATHDL_URC_LEN_MAX, "%s", urc_header);
      break;
    }
    case QAPI_NET_LWM2M_WRITE_ATTR_REQ_E:
    {
      if(op_data.lwm2m_attr == NULL)
      {
        ATFWD_LOG_MSG_LOW("qapi_Net_LWM2M_Decode_Data invalid attr ptr", 0, 0, 0);
        result= ATFWD_RET_PARAM_ERR;
        goto end;
      }

      if((result = lwm2m_util_attr_list_display("+M2MREQ", urc_header, op_data.lwm2m_attr)) != ATFWD_RET_OK)
      {
        ATFWD_LOG_MSG_LOW("data_atfwd_athdl_display_attr_list  display error ", 0, 0, 0);
        goto end;
      }
      athdl_free(at_urc);
      at_urc = NULL;
      break;
    }
    default:
    {
      ATFWD_LOG_MSG_LOW("msy type not support", 0, 0, 0);
      result = ATFWD_RET_UNSUPPORTIVE;
      goto end;
    }
  }

  //Send URC
  if(at_urc)
  {
    qapi_atfwd_send_urc_resp("+M2MREQ", at_urc);
  }
  result = ATFWD_RET_OK;

end:
  if(result != ATFWD_RET_OK)
  {
    lwm2m_util_extreq_errhdl(lwm2m_at_handle,req_id_random,result);
    athdl_free(req_store_data);
    athdl_free(req_store_list_node);
  }
  if(lwm2m_flat_data != NULL)
  {
    //free array memory
    lwm2m_util_FlatData_array_free(&lwm2m_flat_data, lwm2m_flat_data_len);
    ATFWD_LOG_MSG_LOW("free local lwm2m_flat_data", 0,0,0);
  }
  athdl_free(at_urc);
  athdl_free(urc_header);
  ATFWD_LOG_MSG_MED("cb_extended_req_hdl() out, res %d, hdl [0x%x], msg type %d", (int)result, handler, (int)op_data.msg_type);
  return result;
}

/**
 * @fn void lwm2m_cb_observe_enq()
 * @LwM2M observe callback send msg to AT handler thread
 * @param
 *     @param[in]  handler  handler
 *     @param[in]  qapi_Net_LWM2M_Data_v2_t *  lwm2m_data
 * @return qapi_Status_t
 */
void lwm2m_cb_observe_enq(qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Data_v2_t * lwm2m_data, qapi_Status_t * res)
{
  lwm2m_cb_observe_data_t * lwm2m_observe = NULL;
  qapi_Status_t result = QAPI_ERR_NO_MEMORY;

  ATFWD_LOG_MSG_MED("cb_observe_enq() in", 0, 0, 0);

  if(lwm2m_data == NULL)
  {
    ATFWD_LOG_MSG_LOW("ATHDL: invalid input", 0, 0, 0);
    result = QAPI_ERR_INVALID_PARAM;
    goto end;
  }

  if((lwm2m_observe = (lwm2m_cb_observe_data_t *)athdl_malloc(sizeof(lwm2m_cb_observe_data_t))) == NULL)
  {
    ATFWD_LOG_MSG_LOW("Cannot observe buffer for lwm2m_observe", 0, 0, 0);
    goto end; 
  }

  if(TRUE != lwm2m_util_ObjInfo_struct_display("+M2MOBJCHANGES", &(lwm2m_observe->oberve_string), lwm2m_data, ATHDL_URC_LEN_MAX))
  {
    ATFWD_LOG_MSG_LOW("observe encode fail", 0, 0, 0);
    result = QAPI_ERR_INVALID_PARAM;
    goto end;
  }

  ATFWD_LOG_MSG_LOW("observe encode success, oberve_string len is %d", strlen(lwm2m_observe->oberve_string),0,0);

  lwm2m_observe->handler = handler;
  result = athdl_fwd_evt_enq(DATA_ATFWD_ATHDL_LWM2M_OBSERVE_EVT,(void *)lwm2m_observe);

  //ToDo, check if need to free lwm2m_data
end:

  if(result != QAPI_OK && lwm2m_observe)
  {
    athdl_free(lwm2m_observe->oberve_string);
    athdl_free(lwm2m_observe);
  }
  ATFWD_LOG_MSG_MED("cb_observe_enq() out, msg to ATHDL, hdl [0x%x], QAPI res %d", handler, (int)result, 0);
  *res = result;
  return;
}
/**
 * @fn void lwm2m_cb_observe_hdl()
 * @LwM2M callback handle lwM2M observe
 * @param 
 *     @param[in]  handler  handler
 *     @param[in]  char *  observe_string
 * @return void
 */
atfwd_result_code_t lwm2m_cb_observe_hdl(qapi_Net_LWM2M_App_Handler_t handler, char ** observe_string)
{
  char * at_urc = NULL;
  atfwd_result_code_t result = ATFWD_RET_PARAM_INPUT_ERR;

  ATFWD_LOG_MSG_MED("cb_observe_hdl() in, hdl [0x%x]", (int)handler, 0, 0);

  if(handler == NULL | handler != lwm2m_at_handle || observe_string== NULL || *observe_string == NULL)
  {
    ATFWD_LOG_MSG_LOW("invalid input", 0, 0, 0);
    goto end;
  }

  at_urc = &((*observe_string)[strlen("+M2MOBJCHANGES:")]);

  qapi_atfwd_send_urc_resp("+M2MOBJCHANGES", at_urc);
  result = ATFWD_RET_OK;

end:
  if(observe_string != NULL)
  {
    athdl_free(*observe_string);
  }

  ATFWD_LOG_MSG_MED("cb_observe_hdl() out, res %d", (int)result, 0, 0);
  return result;
}

/*===========================================================================

                           AT Functions

===========================================================================*/
/**
 * @fn atfwd_result_code_t lwm2m_at_common_test_cmd()
 * @breif handle common test command
 * @param
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_common_test_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  ATFWD_LOG_MSG_MED("common_test_cmd() in", 0,0,0);

  qapi_atfwd_send_resp(atcmd_name, NULL, ATHDL_AT_RESULT_OK_V01);

  ATFWD_LOG_MSG_MED("common_test_cmd() out", 0,0,0);
  return ATFWD_RET_OK;
}

/**
 * @fn atfwd_result_code_t lwm2m_at_M2MINIT_exec_cmd()
 * @breif handle execute command +M2MINIT
 * @param
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MINIT_exec_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  atfwd_result_code_t result = ATFWD_RET_OK;
  qapi_Status_t lwm2m_ret = QAPI_OK;
  ATFWD_LOG_MSG_MED("M2MINIT_exec_cmd() in", 0,0,0);

  if(lwm2m_at_handle != NULL)
  {
    //LwM2M Handler already inialized, directly response OK
    ATFWD_LOG_MSG_ERROR("hdl already init", 0,0,0);
  }
  else
  {
    if((lwm2m_ret = qapi_Net_LWM2M_Register_App_Extended(&lwm2m_at_handle, NULL, lwm2m_cb_extended_enq)) != QAPI_OK)
    {
      ATFWD_LOG_MSG_ERROR("QAPI Register App fail, err %d", lwm2m_ret, 0, 0);
      result = ATFWD_RET_QAPI_FAILED;
      goto end;
    }
    ATFWD_LOG_MSG_MED("Hdl [0x%x] init success", (uint32)lwm2m_at_handle,0,0);
    }
  qapi_atfwd_send_resp(atcmd_name, NULL, ATHDL_AT_RESULT_OK_V01);

end:
  ATFWD_LOG_MSG_MED("M2MINIT_exec_cmd() out, res %d", (int)result,0,0);
  return result;
}
/**
 * @fn atfwd_result_code_t lwm2m_at_M2MINIT_set_cmd()
 * @breif handle set command +M2MINIT=<Enable_ID_Mapping>
 * @param
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MINIT_set_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  atfwd_result_code_t result = ATFWD_RET_OK;
  qapi_Status_t lwm2m_ret = QAPI_OK;
  int32 enable_ID_mapping = 0;;

  ATFWD_LOG_MSG_MED("M2MINIT_set_cmd() in", 0,0,0);

  if(tokens->args_found != 1)
  {
    result = ATFWD_RET_PARAM_COUNT_ERR;
    goto end;
  }

  //Save enable_ID_mapping
  if(FALSE == util_at_string_to_Integer(tokens->arg[0], &enable_ID_mapping) || enable_ID_mapping != 1)
  {
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }

  if(lwm2m_at_handle != NULL)
  {
    //LwM2M Handler already inialized, directly response OK
    ATFWD_LOG_MSG_ERROR("hdl already init", 0,0,0);
  }
  else
  {
    if((result = lwm2m_idmap_efs_read()) != ATFWD_RET_OK)
    {
      goto end;
    }

    if((lwm2m_ret = qapi_Net_LWM2M_Register_App_Extended(&lwm2m_at_handle, NULL, lwm2m_cb_extended_enq)) != QAPI_OK)
    {
      ATFWD_LOG_MSG_ERROR("QAPI Register App fail, err %d", lwm2m_ret, 0, 0);
      result = ATFWD_RET_QAPI_FAILED;
      goto end;
    }
    ATFWD_LOG_MSG_MED("Hdl [0x%x] init success", (uint32)lwm2m_at_handle,0,0);
  }
  qapi_atfwd_send_resp(atcmd_name, NULL, ATHDL_AT_RESULT_OK_V01);

end:
  ATFWD_LOG_MSG_MED("M2MINIT_set_cmd() out, res %d", (int)result,0,0);
  return result;
}

/**
 * @fn atfwd_result_code_t lwm2m_at_M2MDEINIT_exec_cmd()
 * @breif handler for execute command +M2MDEINIT
 * @param
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MDEINIT_exec_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  atfwd_result_code_t result = ATFWD_RET_OK;
  qapi_Status_t lwm2m_ret = QAPI_OK;
  ATFWD_LOG_MSG_MED("M2MDEINIT_exec_cmd() in", 0,0,0);

  if(lwm2m_at_handle == NULL)
  {
    //LwM2M Handler already de-inialized, directly response OK
    ATFWD_LOG_MSG_ERROR("hdl already deinit", 0,0,0);
  }
  else
  {
    if((lwm2m_ret=qapi_Net_LWM2M_DeRegister_App(lwm2m_at_handle)) != QAPI_OK)
    {
      ATFWD_LOG_MSG_ERROR("QAPI DeRegister App fail, err %d", lwm2m_ret, 0, 0);
      result = ATFWD_RET_QAPI_FAILED;
      goto end;
    }

    ATFWD_LOG_MSG_MED("Hdl [0x%x] de-init success.", (uint32)lwm2m_at_handle,0,0);
    lwm2m_util_reset_gobals();
  }
  qapi_atfwd_send_resp(atcmd_name, NULL, ATHDL_AT_RESULT_OK_V01);

end:
  ATFWD_LOG_MSG_MED("M2MDEINIT_exec_cmd() out, res %d", (int)result,0,0);
  return result;
}

/** 
 * @fn atfwd_result_code_t lwm2m_at_M2MCREATEOBJ_set_cmd()
 * @breif handler for set command +M2MCREATEOBJ, "AT+M2MCREATEOBJ=<objid>, <insid-1>, [<insid-2> [, ... <insid-n>]]"
 * @param 
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MCREATEOBJ_set_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  atfwd_result_code_t result = ATFWD_RET_PARAM_INPUT_ERR;
  qapi_Status_t lwm2m_ret = QAPI_OK;
  int used_buf = 0;
  int32 input_id = 0;
  uint16 obj_id = 0;
  int32 ins_id = 0;
  util_at_list_t * ins_list_hdr = NULL;
  util_at_list_t * ins_list_node = NULL;
  int i = 0;
  qapi_Net_LWM2M_Data_v2_t * lwm2m_data = NULL;

  ATFWD_LOG_MSG_MED("M2MCREATEOBJ_set_cmd() in", 0,0,0);

  if(lwm2m_at_handle == NULL)
  {
    //LwM2M Handler not inialized, directly response ERROR
    result = ATFWD_RET_GLOBAL_INVALID;
    goto end;
  }

  if(tokens->args_found <= 1)
  {
    result = ATFWD_RET_PARAM_COUNT_ERR;
    goto end;
  }

  //Save obj id
  if(FALSE == util_at_string_to_Integer(tokens->arg[0], &input_id) || input_id < 0)
  {
    ATFWD_LOG_MSG_LOW("<obj_id> input error", 0, 0, 0);
    goto end; //result as ATFWD_RET_PARAM_INPUT_ERR
  }
  obj_id = (uint16)input_id;


  if(LWM2M_AT_IS_STANDARD_OBJECT(obj_id))
  {
    if((obj_id != QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E) &&
       (obj_id != QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E) &&
       (obj_id != QAPI_NET_LWM2M_APN_CONN_OBJECT_ID_E) &&
       (obj_id != QAPI_NET_LWM2M_CELL_NW_CONN_OBJECT_ID_E))
    {
      ATFWD_LOG_MSG_LOW("std <obj_id> input error", 0, 0, 0);
      result = ATFWD_RET_QAPI_UNSUPPORTIVE;
      goto end;
    }
    if(tokens->args_found > 2)
    {
      ATFWD_LOG_MSG_LOW("std obj not allow multi-inst", 0, 0, 0);
      goto end;
    }
    if(lwm2m_at_m2mset_ptr == NULL || lwm2m_at_m2mset_ptr->obj_data_ptr == NULL || lwm2m_at_m2mset_ptr->obj_data_ptr->object_ID != obj_id)
    {
      ATFWD_LOG_MSG_LOW("resource not configed", 0, 0, 0);
      result = ATFWD_RET_GLOBAL_INVALID;
      goto end;
    }
    //ToDo check instance number only 1 allowed args_found > 2 error
    ATFWD_LOG_MSG_LOW("creating std obj %d", (int)obj_id,0,0);
  }
  else
  {
    if((result = lwm2m_idmap_ObjId_map(&obj_id, LWM2M_IDMAP_ORG_TO_MAP)) != ATFWD_RET_OK)
    {
      goto end;
    }
    ATFWD_LOG_MSG_LOW("creating ext obj %d", (int)obj_id,0,0);
  }
  //Save ins id
  for(i = 1; i < tokens->args_found; i++)
  {
    if(FALSE == util_at_string_to_Integer(tokens->arg[i], &ins_id) || ins_id < 0)
    {
      ATFWD_LOG_MSG_LOW("<ins_id> input error", 0, 0, 0);
      result = ATFWD_RET_PARAM_INPUT_ERR;
      goto end;
    }

    if((ins_list_node = (util_at_list_t *)athdl_malloc(sizeof(util_at_list_t))) == NULL)
    {
      result = ATFWD_RET_MEMORY_ERR;
      goto end;
    }
    ins_list_node->id = (uint16_t)ins_id;
    ATFWD_LOG_MSG_LOW("adding ins %d for object %d, i = %d", (int)ins_id, (int)obj_id, i);
    ins_list_hdr = (util_at_list_t *)util_at_list_add(ins_list_hdr, ins_list_node);
    ins_list_node = NULL;
  }

  memset(at_resp_str, 0x00, ATHDL_RESP_LEN_MAX);
  used_buf = 0;

  if((lwm2m_data = (qapi_Net_LWM2M_Data_v2_t *)athdl_malloc(sizeof(qapi_Net_LWM2M_Data_v2_t))) == NULL)
  {
    ATFWD_LOG_MSG_LOW("lwm2m_data malloc failed", 0, 0, 0);
    result = ATFWD_RET_MEMORY_ERR;
    goto end;
  }
  if((lwm2m_data->instance_info = (qapi_Net_LWM2M_Instance_Info_v2_t *)athdl_malloc(sizeof(qapi_Net_LWM2M_Instance_Info_v2_t))) == NULL)
  {
    ATFWD_LOG_MSG_LOW("instance_info malloc failed", 0, 0, 0);
    result = ATFWD_RET_MEMORY_ERR;
    goto end;
  }

  lwm2m_data->object_ID = (uint16_t)obj_id;
  lwm2m_data->no_instances = 1;

  i = 0;
  used_buf += snprintf(at_resp_str+used_buf, ATHDL_RESP_LEN_MAX-used_buf, "+M2MCREATEOBJ:");

  ins_list_node = ins_list_hdr;
  while(ins_list_node != NULL)
  {
    if(LWM2M_AT_IS_STANDARD_OBJECT(obj_id))
    {
      athdl_free(lwm2m_data->instance_info);
      lwm2m_data->instance_info = (qapi_Net_LWM2M_Instance_Info_v2_t *)util_at_list_find((util_at_list_t *)(lwm2m_at_m2mset_ptr->obj_data_ptr->instance_info), ins_list_node->id);
    }
    else
    {
      lwm2m_data->instance_info->instance_ID = ins_list_node->id;
    }
    lwm2m_ret = qapi_Net_LWM2M_Create_Object_Instance_v2(lwm2m_at_handle,lwm2m_data);
    if(lwm2m_ret != QAPI_OK)
    {
      ATFWD_LOG_MSG_ERROR("QAPI Create Object fail, for /%d/%d, err %d", obj_id, (int)ins_list_node->id, lwm2m_ret);
    }
    else
    {
      i++;
      ATFWD_LOG_MSG_MED("call QAPI succrss, for /%d/%d", obj_id, (int)ins_list_node->id, 0);
      if(i == 1)
      {
        used_buf += snprintf(at_resp_str+used_buf, ATHDL_RESP_LEN_MAX-used_buf, " %d", (int)ins_list_node->id);
      }
      else
      {
        used_buf += snprintf(at_resp_str+used_buf, ATHDL_RESP_LEN_MAX-used_buf, ", %d", (int)ins_list_node->id);
      }
      result = ATFWD_RET_OK;
    }
    ins_list_node = ins_list_node->next;
  }
  used_buf += snprintf(at_resp_str+used_buf, ATHDL_RESP_LEN_MAX-used_buf, "\r\n");
  if(result == ATFWD_RET_OK)
  {
    qapi_atfwd_send_resp(atcmd_name, at_resp_str, ATHDL_AT_RESULT_OK_V01);
  }

end:

  ATFWD_LOG_MSG_MED("M2MCREATEOBJ_set_cmd() out, res %d", (int)result,0,0);

  if(LWM2M_AT_IS_STANDARD_OBJECT(obj_id))
  {  //After using M2MCREATEOBJ for STD OBJ, we should clean stored object buffer
    if(lwm2m_data != NULL)
    {
      lwm2m_data->instance_info = NULL;
    }
    lwm2m_util_m2mset_data_free(&lwm2m_at_m2mset_ptr);
  }
  if(lwm2m_data)
  {
    athdl_free(lwm2m_data->instance_info);
    athdl_free(lwm2m_data);
  }
  if(ins_list_hdr)
  {
    util_at_list_free(ins_list_hdr);
  }
  return result;
}
/** 
 * @fn atfwd_result_code_t lwm2m_at_M2MDELETEOBJ_set_cmd()
 * @breif handler for set command +M2MDELETEOBJ, "AT+M2MDELETEOBJ=<objid>, <insid-1>, [<insid-2> [, ... <insid-n>]]"
 * @param 
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MDELETEOBJ_set_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  atfwd_result_code_t result = ATFWD_RET_PARAM_INPUT_ERR;
  qapi_Status_t lwm2m_ret = QAPI_OK;
  int used_buf = 0;
  int32 input_id = 0;
  uint16 obj_id = 0;
  int32 ins_id = 0;
  util_at_list_t * ins_list_hdr = NULL;
  util_at_list_t * ins_list_node = NULL;
  int i = 0;
  qapi_Net_LWM2M_Object_Info_v2_t * obj_info = NULL;

  ATFWD_LOG_MSG_MED("M2MDELETEOBJ_set_cmd() in", 0,0,0);

  if(lwm2m_at_handle == NULL)
  {
    //LwM2M Handler not inialized, directly response ERROR
    result = ATFWD_RET_GLOBAL_INVALID;
    goto end;
  }

  if(tokens->args_found <= 1)
  {
    result = ATFWD_RET_PARAM_COUNT_ERR;
    goto end;
  }

  //Save obj id
  if(FALSE == util_at_string_to_Integer(tokens->arg[0], &input_id) || input_id < 0)
  {
    ATFWD_LOG_MSG_LOW("<obj_id> input error", 0, 0, 0);
    goto end; //result as ATFWD_RET_PARAM_INPUT_ERR
  }
  obj_id = (uint16)input_id;

  if(LWM2M_AT_IS_STANDARD_OBJECT(obj_id))
  {
    if((obj_id != QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E) &&
       (obj_id != QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E) &&
       (obj_id != QAPI_NET_LWM2M_APN_CONN_OBJECT_ID_E) &&
       (obj_id != QAPI_NET_LWM2M_CELL_NW_CONN_OBJECT_ID_E))
    {
      result = ATFWD_RET_QAPI_UNSUPPORTIVE;
      goto end;
    }
    ATFWD_LOG_MSG_LOW("deleting std obj %d", (int)obj_id,0,0); 
  }
  else
  {
    if((result = lwm2m_idmap_ObjId_map(&obj_id, LWM2M_IDMAP_ORG_TO_MAP)) != ATFWD_RET_OK)
    {
      goto end;
    }
    ATFWD_LOG_MSG_LOW("deleting ext obj %d", (int)obj_id,0,0); 
  }
  //Save ins id
  for(i = 1; i < tokens->args_found; i++)
  {
    if(FALSE == util_at_string_to_Integer(tokens->arg[i], &ins_id) || ins_id < 0)
    {
      ATFWD_LOG_MSG_LOW("<ins_id> input error", 0, 0, 0);
      result = ATFWD_RET_PARAM_INPUT_ERR;
      goto end;
    }
    ins_list_node = (util_at_list_t *)athdl_malloc(sizeof(util_at_list_t));
    if(ins_list_node == NULL)
    {
      result = ATFWD_RET_MEMORY_ERR;
      goto end;
    }
    ins_list_node->id = (uint16_t)ins_id;
    ATFWD_LOG_MSG_LOW("adding ins %d for object %d, i = %d", (int)ins_id, (int)obj_id, i);
    ins_list_hdr = (util_at_list_t *)util_at_list_add(ins_list_hdr, ins_list_node);
  }

  memset(at_resp_str, 0x00, ATHDL_RESP_LEN_MAX);
  used_buf = 0;

  if((obj_info = (qapi_Net_LWM2M_Object_Info_v2_t *)athdl_malloc(sizeof(qapi_Net_LWM2M_Object_Info_v2_t))) == NULL)
  {
    ATFWD_LOG_MSG_LOW("obj_info malloc failed", 0, 0, 0);
    result = ATFWD_RET_MEMORY_ERR;
    goto end;
  }
  if((obj_info->id_info = (qapi_Net_LWM2M_Id_Info_v2_t *)athdl_malloc(sizeof(qapi_Net_LWM2M_Id_Info_v2_t))) == NULL)
  {
    ATFWD_LOG_MSG_LOW("id_info malloc failed", 0, 0, 0);
    result = ATFWD_RET_MEMORY_ERR;
    goto end;
  }
  obj_info->no_object_info = 1;
  obj_info->id_info->object_ID = obj_id;
  obj_info->id_info->id_set |= QAPI_NET_LWM2M_OBJECT_ID_E;
  obj_info->id_info->id_set |= QAPI_NET_LWM2M_INSTANCE_ID_E;

  i = 0;
  used_buf += snprintf(at_resp_str+used_buf, ATHDL_RESP_LEN_MAX-used_buf, "+M2MDELETEOBJ:");

  ins_list_node = ins_list_hdr;
  while(ins_list_node != NULL)
  {
    obj_info->id_info->instance_ID = ins_list_node->id;
    lwm2m_ret = qapi_Net_LWM2M_Delete_Object_Instance_v2(lwm2m_at_handle,obj_info);
    if(lwm2m_ret != QAPI_OK)
    {
      ATFWD_LOG_MSG_ERROR("QAPI Delete Object fail, for /%d/%d, err %d", obj_id, (int)ins_list_node->id, lwm2m_ret);
    }
    else
    {
      i++;
      ATFWD_LOG_MSG_MED("call QAPI succrss, for /%d/%d", obj_id, (int)ins_list_node->id, 0);
      if(i == 1)
      {
        used_buf += snprintf(at_resp_str+used_buf, ATHDL_RESP_LEN_MAX-used_buf, " %d", (int)ins_list_node->id);
      }
      else
      {
        used_buf += snprintf(at_resp_str+used_buf, ATHDL_RESP_LEN_MAX-used_buf, ", %d", (int)ins_list_node->id);
      }
      result = ATFWD_RET_OK;
    }
    ins_list_node = ins_list_node->next;
  }
  used_buf += snprintf(at_resp_str+used_buf, ATHDL_RESP_LEN_MAX-used_buf, "\r\n");

  if(result == ATFWD_RET_OK)
  {
    qapi_atfwd_send_resp(atcmd_name, at_resp_str, ATHDL_AT_RESULT_OK_V01);
  }

end:

  ATFWD_LOG_MSG_MED("M2MDELETEOBJ_set_cmd() out, res %d", (int)result,0,0);

  if(obj_info)
  {
    athdl_free(obj_info->id_info);
    athdl_free(obj_info);
  }
  if(ins_list_hdr)
  {
    util_at_list_free(ins_list_hdr);
  }
  return result;
}
/**
 * @fn atfwd_result_code_t lwm2m_at_M2MGETDEFATTR_set_cmd()
 * @breif handler for set command +M2MGETDEFATTR, "AT+M2MGETDEFATTR=<server_id>"
 * @param
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MGETDEFATTR_set_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  atfwd_result_code_t result = ATFWD_RET_OK;
  qapi_Status_t lwm2m_ret = QAPI_OK;
  int32 server_id = -1;
  uint32_t p_min = 0;
  uint32_t p_max = 0;

  ATFWD_LOG_MSG_MED("M2MGETDEFATTR_set_cmd() in", 0,0,0);

  if(tokens->args_found != 1)
  {
    result = ATFWD_RET_PARAM_COUNT_ERR;
    goto end;
  }

  //Save server_id
  if(FALSE == util_at_string_to_Integer(tokens->arg[0], &server_id) || (server_id <= 0 || server_id > 65535))
  {
    ATFWD_LOG_MSG_LOW("<server_id> input error", 0, 0, 0);
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }

  memset(at_resp_str, 0x00, ATHDL_RESP_LEN_MAX);

  if((lwm2m_ret = qapi_Net_LWM2M_Default_Attribute_Info(lwm2m_at_handle,(uint16_t)server_id, &p_min, &p_max)) != QAPI_OK)
  {
    ATFWD_LOG_MSG_ERROR("QAPI Attr fail, err %d", lwm2m_ret, 0, 0);
    result = ATFWD_RET_QAPI_FAILED;
    goto end;
  }

  snprintf(at_resp_str, ATHDL_RESP_LEN_MAX, "+M2MGETDEFATTR: %u, %u\r\n", p_min, p_max);
  qapi_atfwd_send_resp(atcmd_name, at_resp_str, ATHDL_AT_RESULT_OK_V01);

end:
  ATFWD_LOG_MSG_MED("M2MGETDEFATTR_set_cmd() out, res %d", (int)result,0,0);
  return result;
}
/** 
 * @fn atfwd_result_code_t lwm2m_at_M2MSETLIFETIME_set_cmd()
 * @breif handler for set command +M2MSETLIFETIME, "AT+M2MSETLIFETIME=<lifetime>[, <server_url>]"
 * @param 
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MSETLIFETIME_set_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  atfwd_result_code_t result = ATFWD_RET_PARAM_INPUT_ERR;
  qapi_Status_t lwm2m_ret = QAPI_OK;
  int32 lifetime = 0;
  char * server_url = NULL;

  ATFWD_LOG_MSG_MED("M2MSETLIFETIME_set_cmd() in", 0,0,0);

  if(lwm2m_at_handle == NULL)
  {
    //LwM2M Handler not inialized, directly response ERROR
    result = ATFWD_RET_GLOBAL_INVALID;
    goto end;
  }

  if((tokens->args_found != 1)&&(tokens->args_found != 2))
  {
    result = ATFWD_RET_PARAM_COUNT_ERR;
    goto end;
  }

  if(FALSE == util_at_string_to_Integer(tokens->arg[0], &lifetime) || lifetime < 0)
  {
    ATFWD_LOG_MSG_LOW("<lifetime> input error", 0, 0, 0);
    goto end;
  }

  if(tokens->args_found == 2)
  {
    if((server_url = util_at_save_str(tokens->arg[1])) == NULL)
    {
      ATFWD_LOG_MSG_LOW("save server_url failed", 0, 0, 0);
      goto end;
    }
  }

  if((lwm2m_ret = qapi_Net_LWM2M_Set_ServerLifeTime(lwm2m_at_handle, (uint8 *)server_url, (uint32_t)lifetime)
) != QAPI_OK)
  {
    ATFWD_LOG_MSG_ERROR("QAPI set lifetime fail, err %d", lwm2m_ret, 0, 0);
    result = ATFWD_RET_QAPI_FAILED;
    goto end;
  }
  qapi_atfwd_send_resp(atcmd_name, NULL, ATHDL_AT_RESULT_OK_V01);
  result = ATFWD_RET_OK;

end:

  ATFWD_LOG_MSG_MED("M2MSETLIFETIME_set_cmd() out, res %d", (int)result,0,0);
  athdl_free(server_url);
  return result;
}
/** 
 * @fn atfwd_result_code_t lwm2m_at_M2MGETLIFETIME_exec_cmd()
 * @breif handler for command +M2MGETLIFETIME
 * @param 
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MGETLIFETIME_exec_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  atfwd_result_code_t result = ATFWD_RET_OK;
  qapi_Status_t lwm2m_ret = QAPI_OK;
  uint32_t lifetime = 0;

  ATFWD_LOG_MSG_MED("M2MGETLIFETIME_exec_cmd() in", 0,0,0);

  memset(at_resp_str, 0x00, ATHDL_RESP_LEN_MAX);

  if((lwm2m_ret = qapi_Net_LWM2M_Get_ServerLifeTime(lwm2m_at_handle, NULL, &lifetime)) != QAPI_OK)
  {
    ATFWD_LOG_MSG_ERROR("QAPI get lifetime fail, err %d", lwm2m_ret, 0, 0);
    result = ATFWD_RET_QAPI_FAILED;
    goto end;
  }

  snprintf(at_resp_str, ATHDL_RESP_LEN_MAX, "+M2MGETLIFETIME: %u\r\n", lifetime);
  qapi_atfwd_send_resp(atcmd_name, at_resp_str, ATHDL_AT_RESULT_OK_V01);

end:
  ATFWD_LOG_MSG_MED("M2MGETLIFETIME_exec_cmd() out, res %d", (int)result,0,0);
  return result;
}

/** 
 * @fn atfwd_result_code_t lwm2m_at_M2MGETLIFETIME_set_cmd()
 * @breif handler for command +M2MGETLIFETIME, "AT+M2MGETLIFETIME[=<server_url>]"
 * @param 
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MGETLIFETIME_set_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  atfwd_result_code_t result = ATFWD_RET_PARAM_INPUT_ERR;
  qapi_Status_t lwm2m_ret = QAPI_OK;
  uint32_t lifetime = 0;
  char * server_url = NULL;

  ATFWD_LOG_MSG_MED("M2MGETLIFETIME_set_cmd() in", 0,0,0);

  memset(at_resp_str, 0x00, ATHDL_RESP_LEN_MAX);

  if(tokens->args_found != 1)
  {
    result = ATFWD_RET_PARAM_COUNT_ERR;
    goto end;
  }

  if((server_url = util_at_save_str(tokens->arg[0])) == NULL)
  {
    ATFWD_LOG_MSG_LOW("save server_url failed", 0, 0, 0);
    goto end; //result as ATFWD_RET_PARAM_INPUT_ERR
  }

  if((lwm2m_ret = qapi_Net_LWM2M_Get_ServerLifeTime(lwm2m_at_handle, (uint8 *)server_url, &lifetime)) != QAPI_OK)
  {
    ATFWD_LOG_MSG_ERROR("QAPI get lifetime fail, err %d", lwm2m_ret, 0, 0);
    result = ATFWD_RET_QAPI_FAILED;
    goto end;
  }

  snprintf(at_resp_str, ATHDL_RESP_LEN_MAX, "+M2MGETLIFETIME: %u\r\n", lifetime);
  qapi_atfwd_send_resp(atcmd_name, at_resp_str, ATHDL_AT_RESULT_OK_V01);
  result = ATFWD_RET_OK;

end:

  ATFWD_LOG_MSG_MED("M2MGETLIFETIME_set_cmd() out, res %d", (int)result,0,0);
  athdl_free(server_url);
  return result;
}
/** 
 * @fn atfwd_result_code_t lwm2m_at_M2MWAKEUP_exec_cmd()
 * @breif handler for execute command +M2MWAKEUP
 * @param 
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MWAKEUP_exec_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  atfwd_result_code_t result = ATFWD_RET_OK;
  qapi_Status_t lwm2m_ret = QAPI_OK;
  uint8_t msg_length = 1; //Not really useful, but for fill QAPI parameters only

  ATFWD_LOG_MSG_MED("M2MWAKEUP_exec_cmd() in", 0,0,0);

  if((lwm2m_ret = qapi_Net_LWM2M_Wakeup(lwm2m_at_handle, &msg_length, msg_length)) != QAPI_OK)
  {
    ATFWD_LOG_MSG_ERROR("QAPI wakeup fail, err %d", lwm2m_ret, 0, 0);
    result = ATFWD_RET_QAPI_FAILED;
    goto end;
  }

  qapi_atfwd_send_resp(atcmd_name, NULL, ATHDL_AT_RESULT_OK_V01);
end:

  ATFWD_LOG_MSG_MED("M2MWAKEUP_exec_cmd() out, res %d", (int)result,0,0);
  return result;

}
/**
 * @fn atfwd_result_code_t lwm2m_at_M2MEVENT_read_cmd()
 * @breif handler for read command +M2MEVENT
 * @param
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MEVENT_read_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  ATFWD_LOG_MSG_MED("M2MEVENT_read_cmd() in", 0,0,0);
  memset(at_resp_str, 0x00, ATHDL_RESP_LEN_MAX);
  snprintf(at_resp_str, ATHDL_RESP_LEN_MAX, "+M2MEVENT: %d, %d\r\n", (int)lwm2m_at_event_enable, (int)lwm2m_at_event_format);
  qapi_atfwd_send_resp(atcmd_name, at_resp_str, ATHDL_AT_RESULT_OK_V01);
  ATFWD_LOG_MSG_MED("M2MEVENT_read_cmd() out", 0,0,0);
  return ATFWD_RET_OK;
}
/**
 * @fn atfwd_result_code_t lwm2m_at_M2MEVENT_test_cmd()
 * @breif handler for test command +M2MEVENT
 * @param
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MEVENT_test_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  ATFWD_LOG_MSG_MED("M2MEVENT_test_cmd() in", 0,0,0);
  qapi_atfwd_send_resp(atcmd_name, "+M2MEVENT: (0,1), (0,1)\r\n", ATHDL_AT_RESULT_OK_V01);
  ATFWD_LOG_MSG_MED("M2MEVENT_test_cmd() out", 0,0,0);
  return ATFWD_RET_OK;
}

/**
 * @fn atfwd_result_code_t lwm2m_at_M2MEVENT_set_cmd()
 * @breif handler for set command +M2MEVENT, "AT+M2MEVENT=<n>[, <format>]"
 * @param
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MEVENT_set_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  atfwd_result_code_t result = ATFWD_RET_PARAM_INPUT_ERR;
  int32 event_enable = -1;
  int32 event_format = -1;

  ATFWD_LOG_MSG_MED("M2MEVENT_set_cmd() in", 0,0,0);

  if(tokens->args_found != 2 && tokens->args_found != 1)
  {
    result = ATFWD_RET_PARAM_COUNT_ERR;
    goto end;
  }

  //Save event_enable
  if(FALSE == util_at_string_to_Integer(tokens->arg[0], &event_enable) || (event_enable != 0 && event_enable != 1))
  {
    ATFWD_LOG_MSG_LOW("<n> input error", 0, 0, 0);
    goto end; //result as ATFWD_RET_PARAM_INPUT_ERR
  }

  if(tokens->args_found == 2)
  {
    //Save event_format
    if(FALSE == util_at_string_to_Integer(tokens->arg[1], &event_format) || (event_format != 0 && event_format != 1))
    {
      ATFWD_LOG_MSG_LOW("<format> input error", 0, 0, 0);
      goto end; //result as ATFWD_RET_PARAM_INPUT_ERR
    }
  }

  ATFWD_LOG_MSG_MED("new event_enable = %d, old event_enable = %d", event_enable, lwm2m_at_event_enable, 0);
  lwm2m_at_event_enable = event_enable;

  if(event_format != -1)
  {
    ATFWD_LOG_MSG_MED("new event_format = %d, old event_format = %d", event_format, lwm2m_at_event_format, 0);
    lwm2m_at_event_format = event_format;
  }
  qapi_atfwd_send_resp(atcmd_name, NULL, ATHDL_AT_RESULT_OK_V01);
  result = ATFWD_RET_OK;

end:

  ATFWD_LOG_MSG_MED("M2MEVENT_set_cmd() out, res %d", result,0,0);
  return result;
}
/**
 * @fn atfwd_result_code_t lwm2m_at_M2MGET_set_cmd()
 * @breif handler for set command +M2MGET, "AT+M2MGET=<uri_str>"
 * @param
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MGET_set_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  atfwd_result_code_t result = ATFWD_RET_PARAM_INPUT_ERR;
  qapi_Status_t lwm2m_ret = QAPI_OK;

  char * uri_str = NULL;
  char * resp_str = NULL;
  qapi_Net_LWM2M_Obj_Info_t id_info;
  qapi_Net_LWM2M_Object_Info_v2_t * lwm2m_info_req = NULL;
  qapi_Net_LWM2M_Data_v2_t * lwm2m_dataP = NULL;

  ATFWD_LOG_MSG_MED("M2MGET_set_cmd() in", 0,0,0);

  if(tokens->args_found != 1)
  {
    result = ATFWD_RET_PARAM_COUNT_ERR;
    goto end;
  }

  //Save uri_str
  uri_str = util_at_save_str(tokens->arg[0]);
  if(uri_str == NULL)
  {
    ATFWD_LOG_MSG_LOW("save uri_str failed", 0, 0, 0);
    goto end; //result as ATFWD_RET_PARAM_INPUT_ERR
  }

  ATFWD_LOG_MSG_LOW_SPRINT("uri_str = %s", uri_str, 0, 0);

  if(FALSE == lwm2m_util_parse_uri_str(uri_str,&id_info))
  {
    ATFWD_LOG_MSG_LOW("url decode error", 0, 0, 0);
    goto end; //result as ATFWD_RET_PARAM_INPUT_ERR
  }

  if(id_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E)
  {
    ATFWD_LOG_MSG_LOW("M2MGET not support res_inst_id", 0, 0, 0);
    result = ATFWD_RET_QAPI_UNSUPPORTIVE;
    goto end;
  }

  //build lwM2M_Object_info
  if((lwm2m_info_req = athdl_malloc(sizeof(qapi_Net_LWM2M_Object_Info_v2_t))) == NULL)
  {
    ATFWD_LOG_MSG_LOW("lwm2m_info_req malloc failed", 0, 0, 0);
    result = ATFWD_RET_MEMORY_ERR;
    goto end;
  }
  if((lwm2m_info_req->id_info = athdl_malloc(sizeof(qapi_Net_LWM2M_Id_Info_v2_t))) == NULL)
  {
    ATFWD_LOG_MSG_LOW("lwm2m_info_req->id_info malloc failed", 0, 0, 0);
    result = ATFWD_RET_MEMORY_ERR;
    goto end;
  }
  lwm2m_info_req->no_object_info = 0x01;
  lwm2m_info_req->id_info->id_set = 0;
  if(id_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E)
  {
    lwm2m_info_req->id_info->id_set |= QAPI_NET_LWM2M_OBJECT_ID_E;
    lwm2m_info_req->id_info->object_ID = id_info.obj_id;
  }

  if(id_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E)
  {
    lwm2m_info_req->id_info->id_set |= QAPI_NET_LWM2M_INSTANCE_ID_E;
    lwm2m_info_req->id_info->instance_ID = id_info.obj_inst_id;
  }

  if(id_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
  {
    lwm2m_info_req->id_info->id_set |= QAPI_NET_LWM2M_RESOURCE_ID_E;
    lwm2m_info_req->id_info->resource_ID = id_info.res_id;
  }

  //Call QAPI
  if((lwm2m_ret=qapi_Net_LWM2M_Get_v2(lwm2m_at_handle, lwm2m_info_req, &lwm2m_dataP)) != QAPI_OK)
  {
    ATFWD_LOG_MSG_ERROR("QAPI Get fail, err %d", lwm2m_ret, 0, 0);
    result = ATFWD_RET_QAPI_FAILED;
    goto end;
  }

  if(lwm2m_dataP == NULL || TRUE != lwm2m_util_ObjInfo_struct_display(atcmd_name, &resp_str, lwm2m_dataP, ATHDL_RESP_LEN_MAX))
  {
    ATFWD_LOG_MSG_LOW("qapi_Net_LWM2M_Get() fail, lwm2m_dataP invalid", 0, 0, 0);
    result = ATFWD_RET_INTERNAL_FAILED;
    goto end;
  }

  ATFWD_LOG_MSG_LOW("LwM2M get success", 0,0,0);
  qapi_atfwd_send_resp(atcmd_name, resp_str, ATHDL_AT_RESULT_OK_V01);
  result = ATFWD_RET_OK;

end:

  ATFWD_LOG_MSG_MED("M2MGET_set_cmd() out, res %d", (int)result,0,0);

  athdl_free(uri_str);
  if(lwm2m_info_req)
  {
    athdl_free(lwm2m_info_req->id_info);
    athdl_free(lwm2m_info_req);
  }
  athdl_free(lwm2m_dataP);
  athdl_free(resp_str);
  return result;
}
/**
 * @fn atfwd_result_code_t lwm2m_at_M2MSET_set_cmd()
 * @breif handler for set command +M2MSET, "AT+M2MSET=<res_uri_str>, <value_type>, <value> , <value-to-be-continue>[, <more-cmd-to-be-continue>]"
 * @param
 *     @param[in]  atcmd_name    at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MSET_set_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  atfwd_result_code_t result = ATFWD_RET_BASE;
  qapi_Status_t lwm2m_ret = QAPI_OK;

  qapi_Net_LWM2M_Obj_Info_t input_id_info;
  boolean input_more_cmd_tbc = FALSE;
  int input_value_tbc = 0;
  uint8 input_type = 0;
  qapi_Net_LWM2M_Flat_Data_t * input_data = NULL;


  int32 input_number = 0;
  char * input_string_ptr = NULL;

  ATFWD_LOG_MSG_MED("M2MSET_set_cmd() in", 0, 0, 0);

  if(lwm2m_at_handle == NULL)
  {
    //LwM2M Handler not inialized, directly response ERROR
    result = ATFWD_RET_GLOBAL_INVALID;
    goto end;
  }

  if((tokens->args_found != 4)&&(tokens->args_found != 5))
  {
    result = ATFWD_RET_PARAM_COUNT_ERR;
    goto end;
  }

  //Save <res_uri_str>
  if((input_string_ptr = util_at_save_str(tokens->arg[0])) == NULL)
  {
    ATFWD_LOG_MSG_LOW("save input_string_ptr failed", 0, 0, 0);
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }
  ATFWD_LOG_MSG_LOW_SPRINT("<res_uri_str> = %s", input_string_ptr, 0, 0);

  if(FALSE == lwm2m_util_parse_uri_str(input_string_ptr,&input_id_info))
  {
    ATFWD_LOG_MSG_LOW("<res_uri_str> decode error", 0, 0, 0);
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }
  if(!(input_id_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E) ||
     !(input_id_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E) ||
     !(input_id_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
    )
  {/*Validity check*/
    ATFWD_LOG_MSG_LOW("<res_uri_str> input error", 0, 0, 0);
    //<value> is the parameter of resource, so URI must reach res level at least
    result =ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }
  if(lwm2m_at_m2mset_ptr != NULL)
  {/*Dependency check*/
    if(lwm2m_at_m2mset_ptr->last_value_tbc != 0)
    {
      if(TRUE != lwm2m_util_obj_info_check(1, &(lwm2m_at_m2mset_ptr->id_info), &input_id_info))
      {
        ATFWD_LOG_MSG_LOW("<res_uri_str> dependecne err", 0, 0, 0);
        //Uri dependence check, if <value_to_be_continue> != 0, all uri need to be check
        result = ATFWD_RET_PARAM_DEPEND_ERR;
        goto end;
      }
    }
    else
    {
        ATFWD_LOG_MSG_HIGH("New Object value set", 0, 0, 0);
    }
  }
  athdl_free(input_string_ptr);

  //Save <value_type>
  if(FALSE == util_at_string_to_Integer(tokens->arg[1], &input_number) ||
    (input_number < QAPI_NET_LWM2M_TYPE_STRING_E || input_number > QAPI_NET_LWM2M_TYPE_OBJECT_LINK))
  {
    ATFWD_LOG_MSG_LOW("<value_type> input error", 0, 0, 0);
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }
  if(lwm2m_at_m2mset_ptr != NULL && lwm2m_at_m2mset_ptr->last_value_tbc != 0 && lwm2m_at_m2mset_ptr->last_type != input_number)
  {/*Dependence check*/
    //If <value_to_be_continue> != 0, current type must same as last type.
    ATFWD_LOG_MSG_LOW("<type> dependecne err", 0, 0, 0);
    result = ATFWD_RET_PARAM_DEPEND_ERR;
    goto end;
  }
  input_type = (uint8)input_number;
  input_number = 0;

  if(TRUE != lwm2m_util_resource_type_check(&input_id_info, input_type))
  {
    ATFWD_LOG_MSG_LOW("data_atfwd_athdl_resource_type_validity_check fail", 0, 0, 0);
    result = ATFWD_RET_QAPI_UNSUPPORTIVE;
    goto end;
  }

  //Save <value>
  if((input_data = athdl_malloc(sizeof(qapi_Net_LWM2M_Flat_Data_t))) == NULL)
  {
    ATFWD_LOG_MSG_LOW("input_data malloc failed", 0, 0, 0);
    result = ATFWD_RET_MEMORY_ERR;
    goto end;
  }
  if(FALSE == lwm2m_util_parse_value(tokens->arg[2], input_type, input_data))
  {
    ATFWD_LOG_MSG_LOW("<value> decode error", 0, 0, 0);
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }

  //Save <value_to_be_continue>
  if(FALSE == util_at_string_to_Integer(tokens->arg[3], &input_number) ||
    input_number < 0 ||
    (input_type != QAPI_NET_LWM2M_TYPE_STRING_E && input_type != QAPI_NET_LWM2M_TYPE_OPAQUE_E && input_number != 0))
  {
    ATFWD_LOG_MSG_LOW("<value_to_be_continue> input error", 0, 0, 0);
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }

  if(lwm2m_at_m2mset_ptr != NULL)
  {/*Dependency check*/
    if(lwm2m_at_m2mset_ptr->last_value_tbc != 0 && input_number != (lwm2m_at_m2mset_ptr->last_value_tbc - 1))
    {
      ATFWD_LOG_MSG_LOW("<value_to_be_continue> input err", 0, 0, 0);
      //Count down value must be last count--
      result = ATFWD_RET_PARAM_DEPEND_ERR;
      goto end;
    }
  }
  input_value_tbc = (int)input_number;
  input_number = 0;

  //Save <more_cmd_to_be_continue>
  if(tokens->args_found == 5)
  {
    if(FALSE == util_at_string_to_Integer(tokens->arg[4], &input_number) || (input_number != 1 && input_number != 0))
    {
      ATFWD_LOG_MSG_LOW("<more_cmd_to_be_continue> input err", 0, 0, 0);
      result = ATFWD_RET_PARAM_INPUT_ERR;
      goto end;
    }
    input_more_cmd_tbc = (input_number == 1? true: false);
    input_number = 0;
  }

  if(input_value_tbc != 0 && input_more_cmd_tbc == FALSE)
  {/*Dependecy Check*/
    ATFWD_LOG_MSG_LOW("input_value_tbc|input_more_cmd_tbc input err", 0, 0, 0);
    //value_tbc != 0, there must have more command to come, so current condition is wrong
    result = ATFWD_RET_PARAM_RELATE_ERR;
    goto end;
  }

  //Store to global structure
  if(lwm2m_at_m2mset_ptr == NULL)
  {/*malloc global structure*/
    ATFWD_LOG_MSG_HIGH("new struct", 0, 0, 0);
    if((lwm2m_at_m2mset_ptr = athdl_malloc(sizeof(lwm2m_at_M2MSET_t))) == NULL)
    {
      result = ATFWD_RET_MEMORY_ERR;
      goto end;
    }
    lwm2m_at_m2mset_ptr->obj_data_ptr = NULL;
    if(ATFWD_RET_OK != lwm2m_util_ObjInfo_list_extend(&(lwm2m_at_m2mset_ptr->obj_data_ptr), &input_id_info, &input_data))
    {
      ATFWD_LOG_MSG_LOW("ObjInfo_list_extend() for obj fail", 0, 0, 0);
      result = ATFWD_RET_EP_OBJ_MEMORY_ERR;
      goto end;
    }
    goto action;
  }

  /*extend current global structure*/
  if(lwm2m_at_m2mset_ptr->last_value_tbc != 0)
  {/*extend buffer*/
    qapi_Net_LWM2M_Data_v2_t * match_obj = NULL;
    qapi_Net_LWM2M_Instance_Info_v2_t * match_inst = NULL;
    qapi_Net_LWM2M_Resource_Info_t * match_res = NULL;

    ATFWD_LOG_MSG_HIGH("ext buf", 0, 0, 0);

    if((match_obj = (qapi_Net_LWM2M_Data_v2_t *)util_at_list_find((util_at_list_t *)(lwm2m_at_m2mset_ptr->obj_data_ptr), input_id_info.obj_id)) == NULL)
    {
      ATFWD_LOG_MSG_LOW("extend buf fail, no matched obj record", 0, 0, 0);
      result = ATFWD_RET_EP_OBJ_NOT_FOUND;
      goto end;
    }
    if((match_inst = (qapi_Net_LWM2M_Instance_Info_v2_t *)util_at_list_find((util_at_list_t *)(match_obj->instance_info), input_id_info.obj_inst_id)) == NULL)
    {
      ATFWD_LOG_MSG_LOW("extend buf fail, no matched inst record", 0, 0, 0);
      result = ATFWD_RET_EP_INST_NOT_FOUND;
      goto end;
    }
    if((match_res = (qapi_Net_LWM2M_Resource_Info_t *)util_at_list_find((util_at_list_t *)(match_inst->resource_info), input_id_info.res_id)) == NULL)
    {
      ATFWD_LOG_MSG_LOW("extend buf fail, no matched res record", 0, 0, 0);
      result = ATFWD_RET_EP_RES_NOT_FOUND;
      goto end;
    }

    if((input_id_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E) && (match_res->type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE))
    {
      qapi_Net_LWM2M_Flat_Data_t * match_res_inst = NULL;
      if((match_res_inst = lwm2m_util_FlatData_array_find(match_res->value.asChildren.array, match_res->value.asChildren.count, input_id_info.res_inst_id)) == NULL)
      {
        ATFWD_LOG_MSG_LOW("extend buf fail, no matched res_inst record", 0, 0, 0);
        result = ATFWD_RET_EP_RES_INST_NOT_FOUND;
        goto end;
      }
      /*Extend buffer of resource instance record*/
      if((match_res_inst->type != QAPI_NET_LWM2M_TYPE_STRING_E && match_res_inst->type != QAPI_NET_LWM2M_TYPE_OPAQUE_E) || match_res_inst->type != input_data->type)
      {
        ATFWD_LOG_MSG_LOW("extend buf fail, invalid res_inst record", 0, 0, 0);
        result = ATFWD_RET_EP_RES_INST_INVALID;
        goto end;
      }
      if(FALSE == lwm2m_util_buffer_extend(&(match_res_inst->value.asBuffer.buffer),
                                                 &(match_res_inst->value.asBuffer.length),
                                                 input_data->value.asBuffer.buffer,
                                                 input_data->value.asBuffer.length,
                                                 match_res_inst->type))
      {
        ATFWD_LOG_MSG_LOW("extend buf fail, extend res_inst buf fail", 0, 0, 0);
        result = ATFWD_RET_EP_RES_INST_MEMORY_ERR;
        goto end;
      }
    }
    else if(!(input_id_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E))
    {/*Extend buffer of resource record*/
      if((match_res->type != QAPI_NET_LWM2M_TYPE_STRING_E && match_res->type != QAPI_NET_LWM2M_TYPE_OPAQUE_E) || match_res->type != input_data->type)
      {
        ATFWD_LOG_MSG_LOW("extend buf fail, invalid res record", 0, 0, 0);
        result = ATFWD_RET_EP_RES_INVALID;
        goto end;
      }
      if(FALSE == lwm2m_util_buffer_extend(&(match_res->value.asBuffer.buffer),
                                                 &(match_res->value.asBuffer.length),
                                                 input_data->value.asBuffer.buffer,
                                                 input_data->value.asBuffer.length,
                                                 match_res->type))
      {
        ATFWD_LOG_MSG_LOW("extend buf fail, extend res buf fail", 0, 0, 0);
        result = ATFWD_RET_EP_RES_MEMORY_ERR;
        goto end;
      }
    }
    else
    {
      ATFWD_LOG_MSG_LOW("extend buf fail, invalid res record", 0, 0, 0);
      result = ATFWD_RET_EP_RES_INVALID;
      goto end;
    }
  }
  else
  {/*extend structure*/
    qapi_Net_LWM2M_Data_v2_t * tmp_obj = NULL;
    qapi_Net_LWM2M_Instance_Info_v2_t * tmp_inst = NULL;
    qapi_Net_LWM2M_Resource_Info_t * tmp_res = NULL;

    ATFWD_LOG_MSG_HIGH("ext struct", 0, 0, 0);

    if((tmp_obj = (qapi_Net_LWM2M_Data_v2_t *)util_at_list_find((util_at_list_t *)(lwm2m_at_m2mset_ptr->obj_data_ptr), input_id_info.obj_id)) == NULL)
    {
      if(ATFWD_RET_OK != lwm2m_util_ObjInfo_list_extend(&(lwm2m_at_m2mset_ptr->obj_data_ptr), &input_id_info, &input_data))
      {
        ATFWD_LOG_MSG_LOW("ObjInfo_list_extend() fail", 0, 0, 0);
        result = ATFWD_RET_EP_OBJ_MEMORY_ERR;
        goto end;
      }
      goto action;
    }

    if((tmp_inst = (qapi_Net_LWM2M_Instance_Info_v2_t *)util_at_list_find((util_at_list_t *)(tmp_obj->instance_info), input_id_info.obj_inst_id)) == NULL)
    {
      if(ATFWD_RET_OK != lwm2m_util_InstInfo_list_extend(&(tmp_obj->instance_info), &input_id_info, &input_data))
      {
        ATFWD_LOG_MSG_LOW("InstInfo_list_extend() fail", 0, 0, 0);
        result = ATFWD_RET_EP_INST_MEMORY_ERR;
        goto end;
      }
      tmp_obj->no_instances++;
      goto action;
    }

    if((tmp_res = (qapi_Net_LWM2M_Resource_Info_t *)util_at_list_find((util_at_list_t *)(tmp_inst->resource_info), input_id_info.res_id)) == NULL)
    {
      if(ATFWD_RET_OK != lwm2m_util_ResInfo_list_extend(&(tmp_inst->resource_info), &input_id_info, &input_data))
      {
        ATFWD_LOG_MSG_LOW("ResInfo_list_extend() fail", 0, 0, 0);
        result = ATFWD_RET_EP_RES_MEMORY_ERR;
        goto end;
      }
      tmp_inst->no_resources++;
      goto action;
    }

    if((input_id_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E) && (tmp_res->type == QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE))
    {
      if(lwm2m_util_FlatData_array_find(tmp_res->value.asChildren.array, tmp_res->value.asChildren.count, input_id_info.res_inst_id) != NULL)
      {
        ATFWD_LOG_MSG_LOW("extend struct fail, match to res_inst level", 0, 0, 0);
        result = ATFWD_RET_EP_RES_INST_EXIST;
        goto end;
      }
      /*Extend buffer of resource instance record*/
      input_data->id = input_id_info.res_inst_id;
      if(FALSE == lwm2m_util_FlatData_array_extend(&(tmp_res->value.asChildren.array),
                                                    &(tmp_res->value.asChildren.count),
                                                    &(input_data))) /*extend res_inst_array*/
      {
        ATFWD_LOG_MSG_LOW("extend res_inst_array error", 0, 0, 0);
        result = ATFWD_RET_EP_RES_INST_MEMORY_ERR;
        goto end;
      }
    }
    else if(!(input_id_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E))
    {
      ATFWD_LOG_MSG_LOW("extend struct fail, match to res level", 0, 0, 0);
      result = ATFWD_RET_EP_RES_EXIST;
      goto end;
    }
    else
    {
      ATFWD_LOG_MSG_LOW("extend struct fail, invalid res record", 0, 0, 0);
      result = ATFWD_RET_EP_RES_INVALID;
      goto end;
    }
  }

action:
  //Save last value
  memscpy(&(lwm2m_at_m2mset_ptr->id_info), sizeof(qapi_Net_LWM2M_Obj_Info_t), &input_id_info, sizeof(qapi_Net_LWM2M_Obj_Info_t));
  lwm2m_at_m2mset_ptr->last_value_tbc = input_value_tbc;
  lwm2m_at_m2mset_ptr->last_type = input_type;

  //Take action
  if(!input_more_cmd_tbc)
  {	  
    if(!((lwm2m_at_m2mset_ptr->obj_data_ptr->object_ID == QAPI_NET_LWM2M_DEVICE_OBJECT_ID_E) ||
         (lwm2m_at_m2mset_ptr->obj_data_ptr->object_ID == QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E) ||
         (lwm2m_at_m2mset_ptr->obj_data_ptr->object_ID == QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E) ||
         (lwm2m_at_m2mset_ptr->obj_data_ptr->object_ID == QAPI_NET_LWM2M_APN_CONN_OBJECT_ID_E) ||
         (lwm2m_at_m2mset_ptr->obj_data_ptr->object_ID == QAPI_NET_LWM2M_CELL_NW_CONN_OBJECT_ID_E)))
      {
        ATFWD_LOG_MSG_ERROR("QAPI Set not support obj %d", lwm2m_at_m2mset_ptr->obj_data_ptr->object_ID, 0, 0);
        result = ATFWD_RET_QAPI_UNSUPPORTIVE;
        goto end;
      }

    lwm2m_ret = qapi_Net_LWM2M_Set_v2(lwm2m_at_handle, lwm2m_at_m2mset_ptr->obj_data_ptr);
    if(QAPI_OK != lwm2m_ret)
    {
      ATFWD_LOG_MSG_ERROR("QAPI Set fail, err %d", lwm2m_ret, 0, 0);
      result = ATFWD_RET_QAPI_FAILED;
      goto end;
    }
    ATFWD_LOG_MSG_LOW("qapi_Net_LWM2M_Set success", 0,0,0);
    result = ATFWD_RET_OK;
  }

end:
  ATFWD_LOG_MSG_MED("M2MSET_set_cmd() out, res %d", (int)result,0,0);

  athdl_free(input_string_ptr);

  if(input_data)
  {
    //free original array memory
    lwm2m_util_FlatData_array_free(&input_data, 1);
  }
  if(result != ATFWD_RET_BASE)
  {
    //Error happened or set triggered, release global vairable
    lwm2m_util_m2mset_data_free(&lwm2m_at_m2mset_ptr);
  }
  if(result == ATFWD_RET_BASE || result == ATFWD_RET_OK)
  {
    qapi_atfwd_send_resp(atcmd_name, NULL, ATHDL_AT_RESULT_OK_V01);
  }
  return result;
}
/**
 * @fn atfwd_result_code_t lwm2m_at_M2MSUBSCRIBE_set_cmd()
 * @breif handler for set command +M2MSUBSCRIBE, "AT+M2MSUBSCRIBE=<res_uri_str>,<observation>"
 * @param
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MSUBSCRIBE_set_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  atfwd_result_code_t result = ATFWD_RET_PARAM_INPUT_ERR;
  qapi_Status_t lwm2m_ret = QAPI_OK;

  char * uri_str = NULL;
  int32 observation = 0;

  qapi_Net_LWM2M_Obj_Info_t id_info;
  qapi_Net_LWM2M_Object_Info_v2_t * lwm2m_info_req = NULL;

  ATFWD_LOG_MSG_MED("M2MSUBSCRIBE_set_cmd() in", 0,0,0);

  if(tokens->args_found != 2)
  {
    result = ATFWD_RET_PARAM_COUNT_ERR;
    goto end;
  }

  //Save uri_str
  if((uri_str= util_at_save_str(tokens->arg[0])) == NULL)
  {
    ATFWD_LOG_MSG_LOW("save uri_str failed", 0, 0, 0);
    goto end;
  }

  ATFWD_LOG_MSG_LOW_SPRINT("uri_str = %s", uri_str, 0, 0);

  //Save observation
  if(FALSE == util_at_string_to_Integer(tokens->arg[1], &observation) || (observation != 0 && observation != 1))
  {
    ATFWD_LOG_MSG_LOW("ATHDL: <observation> input error", 0, 0, 0);
    goto end;
  }

  if(FALSE == lwm2m_util_parse_uri_str(uri_str,&id_info))
  {
    ATFWD_LOG_MSG_LOW("url decode error", 0, 0, 0);
    goto end;
  }

  if(id_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E)
  {
    ATFWD_LOG_MSG_LOW("M2MSUBSCRIBE not support res_inst_id", 0, 0, 0);
    result = ATFWD_RET_QAPI_UNSUPPORTIVE;
    goto end;
  }

  //build lwM2M_Object_info
  if((lwm2m_info_req = athdl_malloc(sizeof(qapi_Net_LWM2M_Object_Info_v2_t))) == NULL)
  {
    ATFWD_LOG_MSG_LOW("lwm2m_info_req malloc failed", 0, 0, 0);
    result = ATFWD_RET_MEMORY_ERR;
    goto end;
  }
  if((lwm2m_info_req->id_info = athdl_malloc(sizeof(qapi_Net_LWM2M_Id_Info_v2_t))) == NULL)
  {
    ATFWD_LOG_MSG_LOW("lwm2m_info_req->id_info malloc failed", 0, 0, 0);
    result = ATFWD_RET_MEMORY_ERR;
    goto end;
  }
  lwm2m_info_req->no_object_info = 0x01;
  if(id_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E)
  {
    lwm2m_info_req->id_info->id_set |= QAPI_NET_LWM2M_OBJECT_ID_E;
    lwm2m_info_req->id_info->object_ID = id_info.obj_id;
  }
  if(id_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E)
  {
    lwm2m_info_req->id_info->id_set |= QAPI_NET_LWM2M_INSTANCE_ID_E;
    lwm2m_info_req->id_info->instance_ID = id_info.obj_inst_id;
  }
  if(id_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E)
  {
    lwm2m_info_req->id_info->id_set |= QAPI_NET_LWM2M_RESOURCE_ID_E;
    lwm2m_info_req->id_info->resource_ID = id_info.res_id;
  }

  //Call QAPI
  if(lwm2m_info_req->id_info->object_ID != QAPI_NET_LWM2M_SOFTWARE_MGNT_OBJECT_ID_E && lwm2m_info_req->id_info->object_ID != QAPI_NET_LWM2M_DEVICE_CAP_OBJECT_ID_E)
  {
    ATFWD_LOG_MSG_LOW("opr on obj id %d unsupport by QAPI", lwm2m_info_req->id_info->object_ID, 0, 0);
    result = ATFWD_RET_QAPI_UNSUPPORTIVE;
    goto end;
  }
  if(observation == 1)
  {
    lwm2m_ret=qapi_Net_LWM2M_Observe_v2(lwm2m_at_handle, lwm2m_cb_observe_enq, lwm2m_info_req);
  }
  else
  {
    lwm2m_ret=qapi_Net_LWM2M_Cancel_Observe_v2(lwm2m_at_handle, lwm2m_info_req);
  }
  if(QAPI_OK != lwm2m_ret)
  {
    ATFWD_LOG_MSG_ERROR("QAPI Observe/Cancel fail, err %d", lwm2m_ret, 0, 0);
    result = ATFWD_RET_QAPI_FAILED;
    goto end;
  }

  ATFWD_LOG_MSG_LOW("LwM2M observer success", 0,0,0);
  qapi_atfwd_send_resp(atcmd_name, NULL, ATHDL_AT_RESULT_OK_V01); 
  result = ATFWD_RET_OK;

end:

  ATFWD_LOG_MSG_MED("M2MSUBSCRIBE_set_cmd() out, res %d", (int)result,0,0);

  athdl_free(uri_str);

  if(lwm2m_info_req)
  {
    athdl_free(lwm2m_info_req->id_info);
    athdl_free(lwm2m_info_req);
  }
  return result;
}
/** 
 * @fn atfwd_result_code_t lwm2m_at_M2MSETMSGDATA_set_cmd()
 * @breif handler for set command +M2MSETMSGDATA, "AT+M2MSETMSGDATA=< req-id>, <res_uri_str> [, <value-type>, <value>[, <value-to-be-continue>]]"
 * @param
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MSETMSGDATA_set_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  atfwd_result_code_t result = ATFWD_RET_BASE;

  qapi_Net_LWM2M_Obj_Info_t input_id_info;
  int input_value_tbc = 0;
  uint8 input_type = 0;
  qapi_Net_LWM2M_Flat_Data_t * input_data = NULL;
  uint16 input_req_id = 0;
  util_at_list_t * req_match_node = NULL;
  lwm2m_at_req_store_t * req_store_data = NULL;
  qapi_Net_LWM2M_Attributes_t * input_attr = NULL;

  int32 input_number = 0;
  char * input_string_ptr = NULL;

  ATFWD_LOG_MSG_MED("M2MSETMSGDATA_set_cmd() in", 0, 0, 0);

  if(lwm2m_at_handle == NULL)
  {
    //LwM2M Handler not inialized, directly response ERROR
    result = ATFWD_RET_GLOBAL_INVALID;
    goto end;
  }

  if((tokens->args_found != 2) && (tokens->args_found != 4) && (tokens->args_found != 5))
  {
    result = ATFWD_RET_PARAM_COUNT_ERR;
    goto end;
  }

/*Save parameters*/
  //Save <req_id>
  if(FALSE == util_at_string_to_Integer(tokens->arg[0], &input_number) || input_number < 0)
  {
    ATFWD_LOG_MSG_LOW("<req_id> input error", 0, 0, 0);
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }
  //--Dependency check
  if(lwm2m_at_m2msetmsgdata_ptr != NULL)
  {
    if(lwm2m_at_m2msetmsgdata_ptr->req_id != input_number)
    {
      //Before M2MSENDMSG, all M2MSETMSGDATA command should have same req_id, as they are configuring the same message
      ATFWD_LOG_MSG_LOW("<req_id> dependency err", 0, 0, 0);
      result = ATFWD_RET_PARAM_DEPEND_ERR;
      goto end;
    }
  }
  //--Save parameter
  input_req_id = (int)input_number;
  input_number = 0;

  //Find match req_id record
  if((req_match_node = util_at_list_find(lwm2m_at_req_store_hdr, input_req_id)) == NULL)
  {
    ATFWD_LOG_MSG_LOW("no matched req_id", 0, 0, 0);
    result = ATFWD_RET_EP_NOT_FOUND;
    goto end;
  }
  req_store_data = (lwm2m_at_req_store_t *)(req_match_node->ptr);
  if(req_store_data == NULL || req_store_data->id_info == NULL)
  {
    ATFWD_LOG_MSG_LOW("matched record invalid", 0, 0, 0);
    result = ATFWD_RET_EP_INVALID;
    goto end;
  }
  if(req_store_data->msg_type != QAPI_NET_LWM2M_READ_REQ_E &&
     req_store_data->msg_type != QAPI_NET_LWM2M_DISCOVER_REQ_E &&
     req_store_data->msg_type != QAPI_NET_LWM2M_OBSERVE_REQ_E)
  {
    ATFWD_LOG_MSG_LOW("only allowed for READ_RSP, DISCOVER_RSP and NOTIFY", 0, 0, 0);
    result = ATFWD_RET_LOCAL_UNSUPPORTIVE;
    goto end;
  }

  //Save <res_uri_str>
  if((input_string_ptr = util_at_save_str(tokens->arg[1])) == NULL)
  {
    ATFWD_LOG_MSG_LOW("save input_string_ptr failed", 0, 0, 0);
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }

  ATFWD_LOG_MSG_LOW_SPRINT("<res_uri_str> = %s", input_string_ptr, 0, 0);

  if(FALSE == lwm2m_util_parse_uri_str(input_string_ptr,&input_id_info))
  {
    ATFWD_LOG_MSG_LOW("<res_uri_str> decode error", 0, 0, 0);
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }
  if((result = lwm2m_idmap_ObjInfo_map(&input_id_info, LWM2M_IDMAP_ORG_TO_MAP)) != ATFWD_RET_OK)
  {
    goto end;
  }
  //--Validity check
  if(TRUE != lwm2m_util_obj_info_check(0, req_store_data->id_info, &input_id_info))
  {
    ATFWD_LOG_MSG_LOW("<res_uri_str> validity check error", 0, 0, 0); 
    //+M2MSETMSGDATA used to input value for the resource required by NW, so input_id_info need match with NW require
    result = ATFWD_RET_PARAM_RELATE_ERR;
    goto end;
  }
  //--Dependency check
  if(lwm2m_at_m2msetmsgdata_ptr != NULL)
  {
    if(lwm2m_at_m2msetmsgdata_ptr->last_more_value_tbc != 0)
    {
      if(TRUE != lwm2m_util_obj_info_check(1, &(lwm2m_at_m2msetmsgdata_ptr->obj_info), &input_id_info))
      {
        ATFWD_LOG_MSG_LOW("<res_uri_str> dependecne err", 0, 0, 0);
        //Uri dependence check, if <value_to_be_continue> != 0, all uri need to be check
        result = ATFWD_RET_PARAM_DEPEND_ERR;
        goto end;
      }
    }
  }

  if(tokens->args_found > 2)
  {
    //Save <value_type>
    if(FALSE == util_at_string_to_Integer(tokens->arg[2], &input_number) ||
      (input_number < QAPI_NET_LWM2M_TYPE_STRING_E || input_number > LWM2M_AT_LWM2M_TYPE_ATTR))
    {
      ATFWD_LOG_MSG_LOW("<value_type> input err", 0, 0, 0);
      result = ATFWD_RET_PARAM_INPUT_ERR;
      goto end;
    }
    //--Dependency check
    if(lwm2m_at_m2msetmsgdata_ptr != NULL)
    {
      if((lwm2m_at_m2msetmsgdata_ptr->last_more_value_tbc != 0 && lwm2m_at_m2msetmsgdata_ptr->last_input_type != input_number)||
         (lwm2m_at_m2msetmsgdata_ptr->last_more_value_tbc != 0 && input_number != QAPI_NET_LWM2M_TYPE_STRING_E && input_number != QAPI_NET_LWM2M_TYPE_OPAQUE_E)||
         (lwm2m_at_m2msetmsgdata_ptr->last_input_type == LWM2M_AT_LWM2M_TYPE_ATTR && input_number != LWM2M_AT_LWM2M_TYPE_ATTR))
      {
        ATFWD_LOG_MSG_LOW("<type> dependence err", 0, 0, 0);
        //Type dependence check, if <value_to_be_continue> != 0, current type must same as last type.
        result = ATFWD_RET_PARAM_DEPEND_ERR;
        goto end;
      }
    }
    input_type = (uint8)input_number;
    input_number = 0;

    //Save <value>
    if(input_type != LWM2M_AT_LWM2M_TYPE_ATTR)
    {
      if((input_data = athdl_malloc(sizeof(qapi_Net_LWM2M_Flat_Data_t))) == NULL)
      {
        ATFWD_LOG_MSG_LOW("input_data malloc failed", 0, 0, 0);
        result = ATFWD_RET_MEMORY_ERR;
        goto end;
      }
      if(FALSE == lwm2m_util_parse_value(tokens->arg[3], input_type, input_data))
      {
        ATFWD_LOG_MSG_LOW("<value> decode error", 0, 0, 0);
        result = ATFWD_RET_PARAM_INPUT_ERR;
        goto end;
      }
    }
    else
    {
      if((input_attr = athdl_malloc(sizeof(qapi_Net_LWM2M_Attributes_t))) == NULL)
      {
        ATFWD_LOG_MSG_LOW("input_attr malloc failed", 0, 0, 0);
        result = ATFWD_RET_MEMORY_ERR;
        goto end;
      }
      memscpy(&(input_attr->obj_info), sizeof(qapi_Net_LWM2M_Obj_Info_t), &input_id_info, sizeof(qapi_Net_LWM2M_Obj_Info_t));
      if(FALSE == lwm2m_util_parse_attr(tokens->arg[3], input_attr))
      {
        ATFWD_LOG_MSG_LOW("<attr_value> decode error", 0, 0, 0);
        result = ATFWD_RET_PARAM_INPUT_ERR;
        goto end;
      }
    }

    //Save <value_to_be_continue>
    if(tokens->args_found > 4)
    {
      if(FALSE == util_at_string_to_Integer(tokens->arg[4], &input_number)|| input_number < 0)
      {
        ATFWD_LOG_MSG_LOW("<value_to_be_continue> input error", 0, 0, 0);
        result = ATFWD_RET_PARAM_INPUT_ERR;
        goto end;
      }

      //--Dependency check
      if(lwm2m_at_m2msetmsgdata_ptr != NULL)
      {
        if(lwm2m_at_m2msetmsgdata_ptr->last_more_value_tbc != 0 && input_number != lwm2m_at_m2msetmsgdata_ptr->last_more_value_tbc - 1)
        {
          ATFWD_LOG_MSG_LOW("<value_to_be_continue> dependence err", 0, 0, 0);
          //value_to_be_continue must be count down
          result = ATFWD_RET_PARAM_DEPEND_ERR;
          goto end;
        }
      }
      input_value_tbc = input_number;
      input_number = 0;
    }
  }
  else
  {
    input_attr = NULL;
  }

/*Store to global variable data_atfwd_athdl_m2msetmsgdata_ptr*/
  if(input_type != LWM2M_AT_LWM2M_TYPE_ATTR && input_type != QAPI_NET_LWM2M_TYPE_UNDEFINED)
  {
    //--Redo Validity check for input_id_info, as for set value, obj/ins/res id is must ones
    if(!(input_id_info.obj_mask & QAPI_NET_LWM2M_OBJECT_ID_E) ||
       !(input_id_info.obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E) ||
       !(input_id_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_ID_E))
    {
      ATFWD_LOG_MSG_MED("<res_uri_str> valid err", 0, 0, 0);
      //+M2MSETMSGDATA used to input value for one resource, so URI must contain at least obj_id/ins_id/res_id
      result = ATFWD_RET_PARAM_INPUT_ERR;
      goto end;
    }

    if(lwm2m_at_m2msetmsgdata_ptr != NULL)
    {
      if(lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data->id != input_id_info.obj_id || lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data->value.asChildren.array == NULL)
      {
        ATFWD_LOG_MSG_LOW("m2msetmsgdata_ptr lost sync", 0, 0, 0);
        //Last inputed obj_id should be saved in lwm2m_flat_data->id, and should be same as current inputed obj_id
        result = ATFWD_RET_GLOBAL_SYNC_LOST;
        goto end;
      }
      if(lwm2m_at_m2msetmsgdata_ptr->last_more_value_tbc != 0 && input_data->value.asBuffer.buffer != NULL)
      {
        //Extend buffer
        qapi_Net_LWM2M_Flat_Data_t * inst_id_match_ep = NULL;
        qapi_Net_LWM2M_Flat_Data_t * res_id_match_ep = NULL;
        qapi_Net_LWM2M_Flat_Data_t * res_inst_id_match_ep = NULL;
        ATFWD_LOG_MSG_HIGH("ext buf", 0, 0, 0);

        inst_id_match_ep = lwm2m_util_FlatData_array_find(lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data->value.asChildren.array,
                                                          lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data->value.asChildren.count,
                                                          input_id_info.obj_inst_id);

        if(inst_id_match_ep == NULL || inst_id_match_ep->value.asChildren.array == NULL)
        {
          ATFWD_LOG_MSG_LOW("last_more_value_tbc != 0, but inst_id not found", 0, 0, 0);
          result = ATFWD_RET_EP_INST_NOT_FOUND;
          goto end;
        }

        res_id_match_ep = lwm2m_util_FlatData_array_find(inst_id_match_ep->value.asChildren.array,
                                                         inst_id_match_ep->value.asChildren.count,
                                                         input_id_info.res_id);
        if(res_id_match_ep == NULL)
        {
          ATFWD_LOG_MSG_LOW("last_more_value_tbc != 0, but res_id not found", 0, 0, 0);
          result = ATFWD_RET_EP_RES_NOT_FOUND;
          goto end;
        }

        if(input_id_info.obj_mask & QAPI_NET_LWM2M_RESOURCE_INSTANCE_ID_E  && res_id_match_ep->value.asChildren.array != NULL)
        {
          res_inst_id_match_ep = lwm2m_util_FlatData_array_find(res_id_match_ep->value.asChildren.array,
                                                                res_id_match_ep->value.asChildren.count,
                                                                input_id_info.res_inst_id);
          if(res_inst_id_match_ep == NULL || res_inst_id_match_ep->value.asBuffer.buffer == NULL)
          {
            ATFWD_LOG_MSG_LOW("last_more_value_tbc != 0, but res_inst_id not found", 0, 0, 0);
            result = ATFWD_RET_EP_RES_INST_NOT_FOUND;
            goto end;
          }
          if(FALSE == lwm2m_util_buffer_extend(&(res_inst_id_match_ep->value.asBuffer.buffer),
                                               &(res_inst_id_match_ep->value.asBuffer.length),
                                               input_data->value.asBuffer.buffer,
                                               input_data->value.asBuffer.length,
                                               res_inst_id_match_ep->type))
          {
            ATFWD_LOG_MSG_LOW("res_inst extend buf err", 0, 0, 0);
            result = ATFWD_RET_EP_RES_INST_MEMORY_ERR;
            goto end;
          }
        }
        else if(res_id_match_ep->value.asBuffer.buffer != NULL)
        {
          if(FALSE == lwm2m_util_buffer_extend(&(res_id_match_ep->value.asBuffer.buffer),
                                               &(res_id_match_ep->value.asBuffer.length),
                                               input_data->value.asBuffer.buffer,
                                               input_data->value.asBuffer.length,
                                               res_id_match_ep->type))
          {
            ATFWD_LOG_MSG_LOW("res extend buf err", 0, 0, 0);
            result = ATFWD_RET_EP_RES_MEMORY_ERR;
            goto end;
          }
        }
        else
        {
          ATFWD_LOG_MSG_LOW("last_more_value_tbc != 0, res_id found, but buf is NULL", 0, 0, 0);
          result = ATFWD_RET_EP_RES_INVALID;
          goto end;
        }
      }
      else if(lwm2m_at_m2msetmsgdata_ptr->last_more_value_tbc == 0)
      {
        //Extend array
        ATFWD_LOG_MSG_HIGH("ext arr", 0, 0, 0);

        if(ATFWD_RET_OK != lwm2m_util_FlatData_struct_extend(lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data, &input_id_info, &(input_data)))
        {
          ATFWD_LOG_MSG_LOW("FlatData_struct_extend() fail", 0, 0, 0);
          result = ATFWD_RET_INTERNAL_FAILED;
          goto end;
        }
      }
      else
      {
        ATFWD_LOG_MSG_ERROR("last more_value != 0, input_data's.buf = NULL", 0, 0, 0);
        result = ATFWD_RET_PARAM_INPUT_ERR;
        goto end;
      }
    }
    else
    {
      ATFWD_LOG_MSG_HIGH("new struct", 0, 0, 0);

      if((lwm2m_at_m2msetmsgdata_ptr = athdl_malloc(sizeof(lwm2m_at_M2MSETMSGDATA_t))) == NULL)
      {
        ATFWD_LOG_MSG_LOW("malloc m2msetmsgdata_ptr error, no memory", 0, 0, 0);
        result = ATFWD_RET_MEMORY_ERR;
        goto end;
      }
      lwm2m_at_m2msetmsgdata_ptr->req_id = input_req_id;
      lwm2m_at_m2msetmsgdata_ptr->enc_content_type = QAPI_NET_LWM2M_M2M_TLV;

      if((lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data = athdl_malloc(sizeof(qapi_Net_LWM2M_Flat_Data_t))) == NULL)
      {
        ATFWD_LOG_MSG_LOW("extend malloc lwm2m_flat_data error, no memory", 0, 0, 0);
        result = ATFWD_RET_MEMORY_ERR;
        goto end;
      }
      lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data->type = QAPI_NET_LWM2M_TYPE_OBJECT;
      lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data->id = input_id_info.obj_id;
      lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data->value.asChildren.count = 0;
      lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data->value.asChildren.array = NULL;
      if(ATFWD_RET_OK != lwm2m_util_FlatData_struct_extend(lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data, &input_id_info, &(input_data)))
      {
        ATFWD_LOG_MSG_LOW("lwm2m_util_FlatData_struct_extend() failed", 0, 0, 0);
        result = ATFWD_RET_INTERNAL_FAILED;
        goto end;
      } 
    }
  }
  else
  {/*For attribute*/
    ATFWD_LOG_MSG_HIGH("ext attr", 0, 0, 0);

    if(lwm2m_at_m2msetmsgdata_ptr != NULL)
    {/*!data_atfwd_athdl_m2msetmsgdata_ptr null*/
      result = lwm2m_util_FlatData_struct_extend(lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data, &input_id_info, &(input_data));
      if(result == ATFWD_RET_PARAM_INPUT_ERR || result == ATFWD_RET_MEMORY_ERR ||
         result == ATFWD_RET_EP_RES_MEMORY_ERR || result == ATFWD_RET_EP_INST_MEMORY_ERR ||
         result == ATFWD_RET_EP_RES_INST_MEMORY_ERR
        )
      {
        ATFWD_LOG_MSG_LOW("lwm2m_util_FlatData_struct_extend() failed", 0, 0, 0);
        goto end;
      }
      if(input_attr != NULL)
      {
        if(TRUE != lwm2m_util_attr_list_add(&(lwm2m_at_m2msetmsgdata_ptr->lwm2m_attr), &(input_attr)))
        {
          ATFWD_LOG_MSG_LOW("input_attr invalid", 0, 0, 0);
          result = ATFWD_RET_INTERNAL_FAILED;
          goto end;
        }
      }
    }
    else
    {/*data_atfwd_athdl_m2msetmsgdata_ptr null*/
      ATFWD_LOG_MSG_HIGH("new attr", 0, 0, 0);

      if((lwm2m_at_m2msetmsgdata_ptr = athdl_malloc(sizeof(lwm2m_at_M2MSETMSGDATA_t))) == NULL)
      {
        ATFWD_LOG_MSG_LOW("malloc lwm2m_at_m2msetmsgdata_ptr error, no memory", 0, 0, 0);
        result = ATFWD_RET_MEMORY_ERR;
        goto end;
      }
      lwm2m_at_m2msetmsgdata_ptr->req_id = input_req_id;
      lwm2m_at_m2msetmsgdata_ptr->enc_content_type = QAPI_NET_LWM2M_APPLICATION_LINK_FORMAT;
      lwm2m_at_m2msetmsgdata_ptr->lwm2m_attr = NULL;
      if((lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data = athdl_malloc(sizeof(qapi_Net_LWM2M_Flat_Data_t))) == NULL)
      {
        ATFWD_LOG_MSG_LOW("extend malloc lwm2m_flat_data error, no memory", 0, 0, 0);
        result = ATFWD_RET_MEMORY_ERR;
        goto end;
      }
      lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data->type = QAPI_NET_LWM2M_TYPE_OBJECT;
      lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data->id = input_id_info.obj_id;
      lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data->value.asChildren.count = 0;
      lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data->value.asChildren.array = NULL;
      result = lwm2m_util_FlatData_struct_extend(lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data, &input_id_info, &(input_data));
      if(result == ATFWD_RET_PARAM_INPUT_ERR || result == ATFWD_RET_MEMORY_ERR ||
         result == ATFWD_RET_EP_RES_MEMORY_ERR || result == ATFWD_RET_EP_INST_MEMORY_ERR ||
         result == ATFWD_RET_EP_RES_INST_MEMORY_ERR
        )
      {
        ATFWD_LOG_MSG_LOW("lwm2m_util_FlatData_struct_extend() failed", 0, 0, 0);
        goto end;
      }
      if(input_attr != NULL)
      {
        lwm2m_util_attr_list_add(&(lwm2m_at_m2msetmsgdata_ptr->lwm2m_attr), &(input_attr));
      }
    }
  }

/*Store last input*/
  memscpy(&(lwm2m_at_m2msetmsgdata_ptr->obj_info),sizeof(qapi_Net_LWM2M_Obj_Info_t),&input_id_info,sizeof(qapi_Net_LWM2M_Obj_Info_t));
  lwm2m_at_m2msetmsgdata_ptr->last_input_type = input_type;
  lwm2m_at_m2msetmsgdata_ptr->last_more_value_tbc = input_value_tbc;

  result = ATFWD_RET_OK;

end:

  ATFWD_LOG_MSG_MED("M2MSETMSGDATA_set_cmd() out res %d", (int)result,0,0);

  athdl_free(input_string_ptr);

  if(input_data)
  {
    //free original array memory
    lwm2m_util_FlatData_array_free(&input_data, 1);
  }

  athdl_free(input_attr);

  if(result == ATFWD_RET_OK)
  {
    qapi_atfwd_send_resp(atcmd_name, NULL, ATHDL_AT_RESULT_OK_V01);
  }
  else
  {
    //Free data_atfwd_athdl_m2msetmsgdata_ptr
    lwm2m_util_m2msetmsgdata_free(&lwm2m_at_m2msetmsgdata_ptr);
  }
  return result;
}
/**
 * @fn atfwd_result_code_t lwm2m_at_M2MSENDMSG_set_cmd()
 * @breif handler for set command +M2MSENDMSG, "AT+M2MSENDMSG=< req-id>, <resp-code>[, <conf-id>]"
 * @param
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MSENDMSG_set_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  atfwd_result_code_t result = ATFWD_RET_PARAM_INPUT_ERR;
  qapi_Status_t lwm2m_ret = QAPI_OK;

  int32 input_number = 0;
  uint16 input_req_id = 0;
  qapi_Net_LWM2M_Response_Code_t input_resp_code = QAPI_NET_LWM2M_IGNORE_E;
  
  uint32_t payload_len = 0;
  uint8_t * payload = NULL;
  qapi_Net_LWM2M_App_Ex_Obj_Data_t *lwm2m_app_data = NULL;
  util_at_list_t * req_match_node = NULL;
  lwm2m_at_req_store_t * req_store_data = NULL;

  ATFWD_LOG_MSG_MED("M2MSENDMSG_set_cmd() in", 0,0,0);

  if(lwm2m_at_handle == NULL)
  {
    //LwM2M Handler not inialized, directly response ERROR
    result = ATFWD_RET_GLOBAL_INVALID;
    goto end;
  }

  if(tokens->args_found != 2 && tokens->args_found != 3)
  {
    result = ATFWD_RET_PARAM_COUNT_ERR;
    goto end;
  }

/*Save parameters*/
  //Save <req_id>
  if(FALSE == util_at_string_to_Integer(tokens->arg[0], &input_number) || input_number < 0)
  {
    ATFWD_LOG_MSG_LOW("<req_id> input error", 0, 0, 0);
    goto end;
  }
  if(lwm2m_at_m2msetmsgdata_ptr != NULL)
  {
    if(lwm2m_at_m2msetmsgdata_ptr->req_id != input_number)
    {
      //M2MSENDMSG, should match with M2MSETMSGDATA command for req_id
      ATFWD_LOG_MSG_LOW("<req_id> dependency  err", 0, 0, 0);
      result = ATFWD_RET_PARAM_DEPEND_ERR;
      goto end;
    }
  }
  input_req_id = input_number;
  input_number = 0;

  //Find match req_id record
  if((req_match_node = util_at_list_find(lwm2m_at_req_store_hdr, input_req_id)) == NULL)
  {
    ATFWD_LOG_MSG_LOW("can't find matched req_id", 0, 0, 0);
    result = ATFWD_RET_EP_NOT_FOUND;
    goto end;
  }
  req_store_data = (lwm2m_at_req_store_t *)(req_match_node->ptr);
  if(req_store_data == NULL || req_store_data->id_info == NULL)
  {
    ATFWD_LOG_MSG_LOW("matchd record invalid", 0, 0, 0);
    result = ATFWD_RET_EP_INVALID;
    goto end;
  }

  //Save <resp-code>
  if(FALSE == util_at_string_to_Integer(tokens->arg[1], &input_number) ||
     TRUE != lwm2m_util_resp_code_check(input_number))
  {
    ATFWD_LOG_MSG_LOW("<resp-code> input error", 0, 0, 0);
    goto end;
  }
  input_resp_code = (qapi_Net_LWM2M_Response_Code_t)input_number;
  input_number = 0;

  //Save <conf-id>
  if(tokens->args_found == 3)
  {
    if(req_store_data->msg_type != QAPI_NET_LWM2M_OBSERVE_REQ_E)
    {
      ATFWD_LOG_MSG_LOW("only NOTIFY allow ack type", 0, 0, 0);
      result = ATFWD_RET_LOCAL_UNSUPPORTIVE;
      goto end;
    }
    if(FALSE == util_at_string_to_Integer(tokens->arg[2], &input_number) || input_number < 0)
    {
      ATFWD_LOG_MSG_LOW("<conf-id> input error", 0, 0, 0);
      goto end;
    }
    req_store_data->conf_id = input_number;
    req_store_data->conf_msg = 1;
    input_number = 0;
  }
  ATFWD_LOG_MSG_LOW("input param req_id %d, resp_code %d, conf_id %d", input_req_id, (int)input_resp_code, req_store_data->conf_id);

/*Encode payload*/
  if(lwm2m_at_m2msetmsgdata_ptr != NULL)
  {
    qapi_Net_LWM2M_Flat_Data_t * flat_data_ptr = NULL;
    size_t flat_data_size = 0;

    if(lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data == NULL)
    {
      ATFWD_LOG_MSG_LOW("qapi_Net_LWM2M_Encode_Data() invalid glabol", 0, 0, 0);
      result = ATFWD_RET_GLOBAL_SYNC_LOST;
      goto end;
    }
    if(!(req_store_data->id_info->obj_mask & QAPI_NET_LWM2M_INSTANCE_ID_E))
    {
      flat_data_ptr = lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data->value.asChildren.array;
      flat_data_size = lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data->value.asChildren.count;
    }
    else
    {
      flat_data_ptr = lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data->value.asChildren.array[0].value.asChildren.array;
      flat_data_size = lwm2m_at_m2msetmsgdata_ptr->lwm2m_flat_data->value.asChildren.array[0].value.asChildren.count;
    }

    if(req_store_data->accept_is_valid)
    {
      lwm2m_at_m2msetmsgdata_ptr->enc_content_type = req_store_data->accept;
    }
    ATFWD_LOG_MSG_HIGH("enc_content_type=%d", lwm2m_at_m2msetmsgdata_ptr->enc_content_type, 0, 0);

    lwm2m_ret = qapi_Net_LWM2M_Encode_Data(lwm2m_at_handle,
                                          req_store_data->id_info,
                                          flat_data_ptr, 
                                          flat_data_size, 
                                          lwm2m_at_m2msetmsgdata_ptr->lwm2m_attr,
                                          lwm2m_at_m2msetmsgdata_ptr->enc_content_type,
                                          &payload, &payload_len);
    if(lwm2m_ret != QAPI_OK || payload == NULL)
    {
      ATFWD_LOG_MSG_ERROR("QAPI Encode fail, err %d", (int)lwm2m_ret, 0, 0);
      result = ATFWD_RET_QAPI_FAILED;
      goto end;
    }

    ATFWD_LOG_MSG_LOW("qapi_Net_LWM2M_Encode_Data success, payload ptr %d, payload len %d", payload, payload_len, 0);
  }
/*Send message*/
  //Malloc lwm2m_app_data
  if((lwm2m_app_data = athdl_malloc(sizeof(qapi_Net_LWM2M_App_Ex_Obj_Data_t))) == NULL)
  {
    ATFWD_LOG_MSG_LOW("lwm2m_app_data malloc failed", 0, 0, 0);
    result = ATFWD_RET_MEMORY_ERR;
    goto end;
  }

  //Fill lwm2m_app_data;
  if(req_store_data->msg_type == QAPI_NET_LWM2M_OBSERVE_REQ_E)
  {
    lwm2m_app_data->msg_type = QAPI_NET_LWM2M_NOTIFY_MSG_E;
  }
  else if(req_store_data->msg_type == QAPI_NET_LWM2M_CREATE_REQ_E)
  {
    lwm2m_app_data->msg_type = QAPI_NET_LWM2M_CREATE_RESPONSE_MSG_E;
  }
  else
  {
    lwm2m_app_data->msg_type = QAPI_NET_LWM2M_RESPONSE_MSG_E;
  }

  lwm2m_app_data->status_code = input_resp_code;
  lwm2m_app_data->conf_msg = req_store_data->conf_msg;
  lwm2m_app_data->observation_seq_num = req_store_data->observation_seq_num;
  lwm2m_app_data->msg_id_len = req_store_data->msg_id_len;
  lwm2m_util_msgid_copy(req_store_data->msg_id, lwm2m_app_data->msg_id, req_store_data->msg_id_len);
  memscpy(&(lwm2m_app_data->obj_info), sizeof(qapi_Net_LWM2M_Obj_Info_t), req_store_data->id_info, sizeof(qapi_Net_LWM2M_Obj_Info_t));
  if(lwm2m_at_m2msetmsgdata_ptr != NULL)
  {
    lwm2m_app_data->content_type = lwm2m_at_m2msetmsgdata_ptr->enc_content_type;
    lwm2m_app_data->payload_len = payload_len;
    lwm2m_app_data->payload = payload;
    payload = NULL;
  }

  //Call QAPIqapi_Net_LWM2M_Send_Message
  lwm2m_ret = qapi_Net_LWM2M_Send_Message(lwm2m_at_handle, lwm2m_app_data);
  if(lwm2m_ret != QAPI_OK)
  {
    ATFWD_LOG_MSG_ERROR("QAPI Send fail, err %d", (int)lwm2m_ret, 0, 0);
    result = ATFWD_RET_QAPI_FAILED;
    goto end;
  }
  ATFWD_LOG_MSG_LOW("qapi_Net_LWM2M_Send_Message success ", 0, 0, 0);
  if(req_store_data->msg_type != QAPI_NET_LWM2M_OBSERVE_REQ_E)
  {
    lwm2m_util_req_store_delete(req_store_data->id_info, req_store_data->msg_type);
  }
  else
  {
    req_store_data->observation_seq_num++;
    req_store_data->notification_id = lwm2m_app_data->notification_id;
  }
/*Send OK*/
  qapi_atfwd_send_resp(atcmd_name, NULL, ATHDL_AT_RESULT_OK_V01);
  result = ATFWD_RET_OK;

end:

  ATFWD_LOG_MSG_MED("M2MSENDMSG_set_cmd() out, res %d", (int)result,0,0);
  /*Free data_atfwd_athdl_m2msetmsgdata_ptr*/
  lwm2m_util_m2msetmsgdata_free(&lwm2m_at_m2msetmsgdata_ptr);

  if(lwm2m_app_data)
  {
    athdl_free(lwm2m_app_data->payload);
    athdl_free(lwm2m_app_data);
  }
  athdl_free(payload);
  return result;
}
/** 
 * @fn atfwd_result_code_t lwm2m_at_M2MREQ_read_cmd()
 * @breif handler for read command +M2MREQ
 * @param 
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MREQ_read_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  ATFWD_LOG_MSG_MED("M2MREQ_read_cmd() in", 0,0,0);
  memset(at_resp_str, 0x00, ATHDL_RESP_LEN_MAX);
  snprintf(at_resp_str, ATHDL_RESP_LEN_MAX, "+M2MREQ: %d, %d\r\n", (int)lwm2m_at_req_enable, (int)lwm2m_at_req_format);
  qapi_atfwd_send_resp(atcmd_name, at_resp_str, ATHDL_AT_RESULT_OK_V01);
  ATFWD_LOG_MSG_MED("M2MREQ_read_cmd() out", 0,0,0);
  return ATFWD_RET_OK;
}
/** 
 * @fn atfwd_result_code_t lwm2m_at_M2MREQ_test_cmd()
 * @breif handler for test command +M2MREQ
 * @param 
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MREQ_test_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  ATFWD_LOG_MSG_MED("M2MREQ_test_cmd() in", 0,0,0);
  qapi_atfwd_send_resp(atcmd_name, "+M2MREQ: (0,1), (0,1)\r\n", ATHDL_AT_RESULT_OK_V01);
  ATFWD_LOG_MSG_MED("M2MREQ_test_cmd() out", 0,0,0);
  return ATFWD_RET_OK;
}

/** 
 * @fn atfwd_result_code_t lwm2m_at_M2MREQ_set_cmd()
 * @breif handler for set command +M2MREQ, "AT+M2MREQ=<n>[, <format>]"
 * @param 
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MREQ_set_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  atfwd_result_code_t result = ATFWD_RET_OK;
  int32 req_enable = -1;
  int32 req_format = -1;

  ATFWD_LOG_MSG_MED("M2MREQ_set_cmd() in", 0,0,0);

  if(tokens->args_found != 2 && tokens->args_found != 1)
  {
    result = ATFWD_RET_PARAM_COUNT_ERR;
    goto end;
  }

  //Save req_enable
  if(FALSE == util_at_string_to_Integer(tokens->arg[0], &req_enable) || (req_enable != 0 && req_enable != 1))
  {
    ATFWD_LOG_MSG_LOW("<n> input error", 0, 0, 0);
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }

  if(tokens->args_found == 2)
  {
    //Save req_format
    if(FALSE == util_at_string_to_Integer(tokens->arg[1], &req_format) || (req_format != 0 && req_format != 1))
    {
      ATFWD_LOG_MSG_LOW("<format> input error", 0, 0, 0);
      result = ATFWD_RET_PARAM_INPUT_ERR;
      goto end;
    }
  }

  ATFWD_LOG_MSG_MED("new req_enable = %d, old req_enable = %d", req_enable, lwm2m_at_req_enable, 0);
  lwm2m_at_req_enable = req_enable;

  if(req_format != -1)
  {
    ATFWD_LOG_MSG_MED("new req_format = %d, old req_format = %d", req_format, lwm2m_at_req_format, 0);
    lwm2m_at_req_format = req_format;
  }
  qapi_atfwd_send_resp(atcmd_name, NULL, ATHDL_AT_RESULT_OK_V01);

end:

  ATFWD_LOG_MSG_MED("M2MREQ_set_cmd() out, res %d", (int)result,0,0);
  return result;
}
/** 
 * @fn atfwd_result_code_t lwm2m_at_M2MIDMAPCFG_set_cmd()
 * @breif handler for set command +M2MIDMAPCFG, "AT+M2MIDMAPCFG = <line_buf>, <buf_to_be_come>[, <line_to_be_come>]"
   @ M2MIDMAPCFG didn't use lwM2M QAPI, independence with M2MINIT.
 * @param 
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MIDMAPCFG_set_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  char * input_buf = NULL;
  size_t input_buf_len = 0;
  int32 input_number = 0;
  uint16 input_buf_tbc = 0;
  boolean input_line_tbc = FALSE;

  atfwd_result_code_t result = ATFWD_RET_BASE;

  ATFWD_LOG_MSG_MED("M2MIDMAPCFG_set_cmd() in", 0,0,0);

  if((tokens->args_found != 2)&&(tokens->args_found != 3))
  {
    result = ATFWD_RET_PARAM_COUNT_ERR;
    goto end;
  }

  //Save input buffer
  input_buf = util_at_save_str(tokens->arg[0]);
  if(input_buf == NULL)
  {
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }
  input_buf_len = strlen(input_buf);

  //Save input <buffer_to_be_come>
  if(FALSE == util_at_string_to_Integer(tokens->arg[1], &input_number) || input_number < 0)
  {
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }
  if(lwm2m_at_m2midmapcfg_ptr != NULL)
  {/*Dependency check*/
    if(lwm2m_at_m2midmapcfg_ptr->last_buf_tbc != 0 && input_number != (lwm2m_at_m2midmapcfg_ptr->last_buf_tbc - 1))
    {
      ATFWD_LOG_MSG_LOW("<buf_to_be_continue> input err", 0, 0, 0);
      //Count down value must be last count--
      result = ATFWD_RET_PARAM_DEPEND_ERR;
      goto end;
    }
  }
  input_buf_tbc = (uint16)input_number;
  input_number = 0;

  //Save input <line_to_be_come>
  if(tokens->args_found == 3)
  {
    if(FALSE == util_at_string_to_Integer(tokens->arg[2], &input_number) || (input_number != 1 && input_number != 0))
    {
      result = ATFWD_RET_PARAM_INPUT_ERR;
      goto end;
    }
    input_line_tbc = (input_number == 1? TRUE: FALSE);
    input_number = 0;
  }

  if(input_buf_tbc != 0 && input_line_tbc == FALSE)
  {/*Dependecy Check*/
    ATFWD_LOG_MSG_LOW("input_buf_tbc|input_line_tbc input err", 0, 0, 0);
    //input_buf_tbc != 0, there must have more line to come, so current condition is wrong
    result = ATFWD_RET_PARAM_RELATE_ERR;
    goto end;
  }

  //extend buffer
  if(lwm2m_at_m2midmapcfg_ptr != NULL)
  {
    if(lwm2m_at_m2midmapcfg_ptr->last_buf_tbc != 0)
    {
      if(TRUE != lwm2m_util_buffer_extend(&(lwm2m_at_m2midmapcfg_ptr->line_buf),
                                          &(lwm2m_at_m2midmapcfg_ptr->buf_len),
                                          (uint8 *)input_buf,
                                          input_buf_len,
                                          QAPI_NET_LWM2M_TYPE_STRING_E))
      {
        result = ATFWD_RET_INTERNAL_FAILED;
        goto end;
      }
    }
    else
    {
      result = ATFWD_RET_GLOBAL_SYNC_LOST;
      goto end;
    }
  }
  else
  {
    if((lwm2m_at_m2midmapcfg_ptr = athdl_malloc(sizeof(lwm2m_at_M2MIDMAPCFG_t))) == NULL)
    {
      result = ATFWD_RET_MEMORY_ERR;
      goto end;
    }

    lwm2m_at_m2midmapcfg_ptr->line_buf = (uint8 *)input_buf;
    lwm2m_at_m2midmapcfg_ptr->buf_len = strlen(input_buf);
    input_buf = NULL;
  }

  //Write buffer
  if(input_buf_tbc == 0)
  {
    if(lwm2m_at_m2midmapcfg_ptr->buf_len > LWM2M_IDMAP_MAX_LINE)
    {
      result = ATFWD_RET_LOCAL_UNSUPPORTIVE;
      goto end;
    }
    
    if((result = lwm2m_idmap_efs_writeline(lwm2m_at_m2midmapcfg_ptr->line_buf, lwm2m_at_m2midmapcfg_ptr->buf_len, !input_line_tbc)) != ATFWD_RET_OK)
    {
      goto end;
    }
  }

  //save last tbc
  lwm2m_at_m2midmapcfg_ptr->last_buf_tbc = input_buf_tbc;

end:

  athdl_free(input_buf);
  if(result == ATFWD_RET_BASE || result == ATFWD_RET_OK)
  {
    qapi_atfwd_send_resp(atcmd_name, NULL, ATHDL_AT_RESULT_OK_V01);
  }
  if(result != ATFWD_RET_BASE && lwm2m_at_m2midmapcfg_ptr != NULL)
  {
    athdl_free(lwm2m_at_m2midmapcfg_ptr->line_buf);
    athdl_free(lwm2m_at_m2midmapcfg_ptr);
  }
  ATFWD_LOG_MSG_MED("M2MIDMAPCFG_set_cmd() out, res %d", (int)result,0,0);
  return result;
}

atfwd_result_code_t lwm2m_at_M2MSTART_exec_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{  
  atfwd_result_code_t result = ATFWD_RET_BASE;
  qapi_Status_t lwm2m_ret = QAPI_OK;
  qapi_Net_LWM2M_Config_Data_t clientconfig_data;

  if(atcmd_name == NULL || tokens == NULL)
  {
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }

  if(lwm2m_at_handle == NULL)
  {
    result = ATFWD_RET_GLOBAL_INVALID;
    goto end;
  }

  if(lwm2m_at_m2mset_ptr)
  {
    clientconfig_data.config_type = QAPI_NET_LWM2M_CONFIG_BOOTSTRAP_INFO;
    clientconfig_data.next = NULL;
    clientconfig_data.value.lwm2m_data = lwm2m_at_m2mset_ptr->obj_data_ptr;
	lwm2m_ret = qapi_Net_LWM2M_ConfigClient(lwm2m_at_handle, &clientconfig_data);  

	lwm2m_util_m2mset_data_free(&lwm2m_at_m2mset_ptr);
	lwm2m_at_m2mset_ptr = NULL;
	
	if(QAPI_OK != lwm2m_ret)
	{
      result = ATFWD_RET_QAPI_FAILED;
	  goto end;
	}
  }

  clientconfig_data.config_type      = QAPI_NET_LWM2M_START;
  clientconfig_data.value.lwm2m_data = NULL;
  lwm2m_ret = qapi_Net_LWM2M_ConfigClient(lwm2m_at_handle, &clientconfig_data);  
  
  if(QAPI_OK != lwm2m_ret)
  {
    result = ATFWD_RET_QAPI_FAILED;
  }
  else
  {
    qapi_atfwd_send_resp(atcmd_name, NULL, ATHDL_AT_RESULT_OK_V01);
    result = ATFWD_RET_OK;
  }  

end:
	
  return result;
}
/** 
 * @fn athdl_result_code_t lwm2m_at_M2MSTOP_set_cmd()
 * @breif handler for set command +M2MSTOP, "AT+M2MSTOP=<n>]"
 * @param 
 *     @param[in]  atcmd_name  at command name
 *     @param[in]  athdl_tokens_t * tokens
 * @return ATHDL result
 */
atfwd_result_code_t lwm2m_at_M2MSTOP_set_cmd(char *atcmd_name, athdl_tokens_t * tokens)
{
  atfwd_result_code_t result = ATFWD_RET_BASE;
  qapi_Status_t lwm2m_ret = QAPI_OK;
  int32 n_stop_clean = 0;
  qapi_Net_LWM2M_Config_Data_t clientconfig_data;
  
  if(atcmd_name == NULL || tokens == NULL)
  {
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }

  if(tokens->args_found != 1)
  {
    result = ATFWD_RET_PARAM_COUNT_ERR;
    goto end;  
  }
  
  if(lwm2m_at_handle == NULL)
  {
    result = ATFWD_RET_GLOBAL_INVALID;
    goto end;
  }
  
  if((FALSE == util_at_string_to_Integer(tokens->arg[0], &n_stop_clean)) || 
  	 (n_stop_clean != 0 && n_stop_clean != 1 && n_stop_clean != 2))
  {
    result = ATFWD_RET_PARAM_INPUT_ERR;
    goto end;
  }

  if(n_stop_clean == 0)
    clientconfig_data.config_type = QAPI_NET_LWM2M_STOP;
  else if(n_stop_clean == 1)
    clientconfig_data.config_type = QAPI_NET_LWM2M_DEREGISTER_AND_STOP;
  else if(n_stop_clean == 2)
    clientconfig_data.config_type = QAPI_NET_LWM2M_FACTORY_RESET;
  
  clientconfig_data.next = NULL;
  
  lwm2m_ret = qapi_Net_LWM2M_ConfigClient(lwm2m_at_handle, &clientconfig_data);  
  if(QAPI_OK != lwm2m_ret)
  {
    result = ATFWD_RET_QAPI_FAILED;
  }
  else
  {
    qapi_atfwd_send_resp(atcmd_name, NULL, ATHDL_AT_RESULT_OK_V01);
    result = ATFWD_RET_OK;
  }  

end:
  return result;
}


/*===========================================================================

                           Command Table related

===========================================================================*/
athdl_cmd_table_t lwm2m_at_cmd_table[]=
{
/* AT Cmd Name,      Test handle function,       Set handle function,             Exec handle function,             Read handle function*/
  {"+M2MINIT",       lwm2m_at_common_test_cmd,   lwm2m_at_M2MINIT_set_cmd,        lwm2m_at_M2MINIT_exec_cmd,        NULL},
  {"+M2MDEINIT",     lwm2m_at_common_test_cmd,   NULL,                            lwm2m_at_M2MDEINIT_exec_cmd,      NULL},
  {"+M2MCREATEOBJ",  lwm2m_at_common_test_cmd,   lwm2m_at_M2MCREATEOBJ_set_cmd,   NULL,                             NULL},
  {"+M2MDELETEOBJ",  lwm2m_at_common_test_cmd,   lwm2m_at_M2MDELETEOBJ_set_cmd,   NULL,                             NULL},
  {"+M2MGETDEFATTR", lwm2m_at_common_test_cmd,   lwm2m_at_M2MGETDEFATTR_set_cmd,  NULL,                             NULL},
  {"+M2MSETLIFETIME",lwm2m_at_common_test_cmd,   lwm2m_at_M2MSETLIFETIME_set_cmd, NULL,                             NULL},
  {"+M2MGETLIFETIME",lwm2m_at_common_test_cmd,   lwm2m_at_M2MGETLIFETIME_set_cmd, lwm2m_at_M2MGETLIFETIME_exec_cmd, NULL},
  {"+M2MWAKEUP",     lwm2m_at_common_test_cmd,   NULL,                            lwm2m_at_M2MWAKEUP_exec_cmd,      NULL},
  {"+M2MEVENT",      lwm2m_at_M2MEVENT_test_cmd, lwm2m_at_M2MEVENT_set_cmd,       NULL,                             lwm2m_at_M2MEVENT_read_cmd},
  {"+M2MGET",        lwm2m_at_common_test_cmd,   lwm2m_at_M2MGET_set_cmd,         NULL,                             NULL},
  {"+M2MSET",        lwm2m_at_common_test_cmd,   lwm2m_at_M2MSET_set_cmd,         NULL,                             NULL},
  {"+M2MSUBSCRIBE",  lwm2m_at_common_test_cmd,   lwm2m_at_M2MSUBSCRIBE_set_cmd,   NULL,                             NULL},
  {"+M2MSETMSGDATA", lwm2m_at_common_test_cmd,   lwm2m_at_M2MSETMSGDATA_set_cmd,  NULL,                             NULL},
  {"+M2MSENDMSG",    lwm2m_at_common_test_cmd,   lwm2m_at_M2MSENDMSG_set_cmd,     NULL,                             NULL},
  {"+M2MREQ",        lwm2m_at_M2MREQ_test_cmd,   lwm2m_at_M2MREQ_set_cmd,         NULL,                             lwm2m_at_M2MREQ_read_cmd},
  {"+M2MIDMAPCFG",   lwm2m_at_common_test_cmd,   lwm2m_at_M2MIDMAPCFG_set_cmd,    NULL,                             NULL},
  {"+M2MSTART", 	 lwm2m_at_common_test_cmd,   NULL,                            lwm2m_at_M2MSTART_exec_cmd,       NULL},
  {"+M2MSTOP", 		 lwm2m_at_common_test_cmd,   lwm2m_at_M2MSTOP_set_cmd,        NULL,                             NULL},  
};
/**
 * @fn boolean lwm2m_at_cmdtbl_reg()
 * @brief register lwm2m AT command table to ATHDL's table list
 * @param N.A
 * @return TRUE/FALSE
 */
boolean lwm2m_at_cmdtbl_reg()
{
  athdl_cmdtbl_list_t * tmp_list;
  boolean result = FALSE;
  ATFWD_LOG_MSG_MED("cmdtbl_reg() in", 0, 0, 0);

  if((tmp_list = athdl_malloc(sizeof(athdl_cmdtbl_list_t))) == NULL)
  {
    ATFWD_LOG_MSG_LOW("memory error", 0, 0, 0);
    goto end;
  }

  tmp_list->count = sizeof(lwm2m_at_cmd_table)/sizeof(athdl_cmd_table_t);
  tmp_list->cmd_table = lwm2m_at_cmd_table;
  tmp_list->next = NULL;
  result = athdl_cmd_list_extend(&tmp_list);

end:
  ATFWD_LOG_MSG_MED("cmdtbl_reg() out, bool res %d", (int)result, 0, 0);
  return result;
}
#else
#include "lwm2m_at_handler.h"
/**
 * @fn boolean lwm2m_at_cmdtbl_reg()
 * @brief register lwm2m AT command table to ATHDL's table list
 * @param N.A
 * @return TRUE/FALSE
 */
boolean lwm2m_at_cmdtbl_reg()
{
  return TRUE;
}
/**
 * @fn void lwm2m_cb_extended_evt_hdl()
 * @LwM2M callback handle lwM2M event
 * @param 
 *     @param[in]  handler  handler
 *     @param[in]  qapi_Net_LWM2M_Server_Data_t  op_data
 * @return void
 */
atfwd_result_code_t lwm2m_cb_extended_evt_hdl(qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Server_Data_t op_data)
{
  return ATFWD_RET_OK;
}
/**
 * @fn void lwm2m_cb_extended_req_hdl()
 * @LwM2M callback handle lwM2M request
 * @param 
 *     @param[in]  handler  handler
 *     @param[in]  qapi_Net_LWM2M_Server_Data_t  op_data
 * @return void
 */
atfwd_result_code_t lwm2m_cb_extended_req_hdl(qapi_Net_LWM2M_App_Handler_t handler, qapi_Net_LWM2M_Server_Data_t op_data)
{
  return ATFWD_RET_OK;
}
/**
 * @fn void lwm2m_cb_observe_hdl()
 * @LwM2M callback handle lwM2M observe
 * @param 
 *     @param[in]  handler  handler
 *     @param[in]  char *  observe_string
 * @return void
 */
atfwd_result_code_t lwm2m_cb_observe_hdl(qapi_Net_LWM2M_App_Handler_t handler, char ** observe_string)
{
  return ATFWD_RET_OK;
}

#endif /*LWM2M_AT_ENABLE*/
