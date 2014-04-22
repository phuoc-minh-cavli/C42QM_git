
/******************************************************************************
 * Copyright © 2020-2022 Qualcomm Technologies, Inc.
 * \file    qapi_coap_ext.c
 * \author  Qualcomm Technologies, Inc.
 *
 ****************************************************************************************/
#include "qapi_coap.h"

/*
 * @fn     void *qapi_coap_register_client() 
 * @brief  This function is used to send client registration request to coap
 * @param  coap_register_clientP - pointer to qapi coap client context  
 * @return on success            - void pointer of type coap handler 
 *         on failure            - NULL 
 */
qapi_Status_t qapi_Coap_Create_Session(qapi_Coap_Session_Hdl_t  *session, qapi_Coap_Session_Info_t * coap_session_config)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/*
 * @fn      void qapi_coap_deregister_client()
 * @brief   This function is used to close coap connection by invoking coap_close 
 * @param   coapHandle  - coap handle
 * @return   void 
 */
qapi_Status_t qapi_Coap_Destroy_Session(qapi_Coap_Session_Hdl_t  session)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/* 
 * @fn     void * qapi_coap_create_connection() 
 * @brief  This functionis used to send connection request to coap
 * @param  contextP   - pointer to  coap client context 
 *         host       - pointer to ip address string
 *         port       - pointer to port string 
 *         ssl_cfg    - pointer to certificate name
 *         cfg        - pointer to SSL configuration
 * @return on success - connection_t pointer 
 *         on failure - NULL
 */ 
qapi_Status_t qapi_Coap_Create_Connection(qapi_Coap_Session_Hdl_t sessionHandle,qapi_Coap_Connection_Cfg_t * coap_conn_config )
{
   return QAPI_ERR_NOT_SUPPORTED;
}


/**
 * @brief Reconnect DTLS connection
 * @param coap_handle CoAP handle
 * @param session CoAP connection
 * @param new_local_ip Local IP address if there is a change or pass NULL if want to use previous IP address
 * @return New Connection pointer
 */
qapi_Status_t qapi_Coap_Reconnect(qapi_Coap_Session_Hdl_t sessionhandle, char * new_local_ip, int session_resumption_time)
{
   return QAPI_ERR_NOT_SUPPORTED;
}


/*
 * @fn     void qapi_coap_close_connection()
 * @brief  This function is used to close the session by invoking coap_close_connection 
 * @param  coap_handle  - pointer to client context 
 * @return void 
 */
qapi_Status_t qapi_Coap_Close_Connection(qapi_Coap_Session_Hdl_t sessionHandle)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/* 
 * @fn      void *qapi_coap_create_message()
 * @brief   This function is used to create new request message
 * @param   coap_hanlde    - coap client context handle
 *          coap_msg_type  - qapi coap message type 
 *          msg_code       - request message code 
 * @return  on success     - pointer to coap_packet_t 
            on error       - NULL
 */ 

qapi_Status_t qapi_Coap_Init_Message(qapi_Coap_Session_Hdl_t sessionHandle , qapi_Coap_Packet_t ** message , qapi_Coap_Message_Type_t coap_msg_type ,uint8_t msg_code )
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/*
 * @fn     int qapi_coap_send_message()
 * @brief  This function is used to send the message to coap. Failures would internally
 *         release the allocated resources. 
 * @param  qapi_message - pointer to qapi coap message 
 * return  on success   -  0
 *         on error     - -1 
 */

qapi_Status_t qapi_Coap_Send_Message(qapi_Coap_Session_Hdl_t  sessionHandle , qapi_Coap_Packet_t * pkt, qapi_Coap_Message_Params_t * msg_conf)
{
   return QAPI_ERR_NOT_SUPPORTED;
}


/**
 * @versiontable{2.0,2.45,
 * Data\_Services 1.2.0  &  Introduced. @tblendline
 * }
 *         This function is used to send the message to coap in Blocks . Failures would internally
 *   release the allocated resources.
 *  
 * @param sessionHandle    CoAP Session Handle returned by Create Session .
 * @param message          Pointer to qapi CoAP message.
 * @param msg_conf         Pointer to CoAP modified Message config structure
 * @param blocktype........Blockwise option
 * @param blocksize........Size of each block
 * return  on success   -  0
 *         on error     - -1 
 *
 */
qapi_Status_t qapi_Coap_Send_Message_v2(qapi_Coap_Session_Hdl_t  sessionHandle , qapi_Coap_Packet_t * pkt, qapi_Coap_Message_Params_t * msg_conf , qapi_Coap_Block_Wise_Options_t blocktype , uint16_t blocksize)
{
    return QAPI_ERR_NOT_SUPPORTED;
}


/* 
 * @fn      void qapi_coap_free_message()
 * @brief   This function is used to free the coap message 
 * @param   messageP - pointer to qapi coap message 
 * @retrun  void
 */   
qapi_Status_t qapi_Coap_Free_Message(qapi_Coap_Session_Hdl_t sessionhandle ,qapi_Coap_Packet_t *messageP)
{
   return QAPI_ERR_NOT_SUPPORTED;
}
/*
 * @fn      int qapi_coap_set_payload()
 * @brief   This function is used to set coap payload
 * @param   packet     - pointer to qapi coap packet
 *          payload    - pointer to payload
 *          length     - length of payload 
 * @return  on success - 1 
 *          on error   - 0
 */       

qapi_Status_t qapi_Coap_Set_Payload(qapi_Coap_Session_Hdl_t sessionhandle , qapi_Coap_Packet_t *packet, const void *payload, size_t length)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/* @fn     int qapi_coap_set_header()
 * @brief  This function is used to set header parameters 
 * @param  message      - pointer to coap message header to be fill
 *         header_type  - type of header field to be fill
 *         header_val   - pointer to header value 
 *         val_len      - length of header value 
 * @return int value 
 */
qapi_Status_t qapi_Coap_Set_Header(qapi_Coap_Session_Hdl_t sessionhandle ,qapi_Coap_Packet_t *message , qapi_Coap_Header_type header_type , const void *header_val , size_t val_len)
{
   return QAPI_ERR_NOT_SUPPORTED;
}
/* @fn    int qapi_coap_get_header()
 * @brief This function is used to get header parameters 
 * @param  message      - pointer to coap message header to be get
 *         header_type  - type of header field to be get
 *         header_val   - pointer to header value to store 
 *         val_len      - length of header value  
 */
qapi_Status_t qapi_Coap_Get_Header(qapi_Coap_Session_Hdl_t sessionhandle ,qapi_Coap_Packet_t * message , qapi_Coap_Header_type header_type , void **header_val , size_t * val_len)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/* @fn    int qapi_Coap_Get_Exchange_Lifetime()
 * @brief This function is used to get Exchange Lifetime
 * @param  session      -       Session handle
 *         exchange_lifetime  - Pointer to the exchange life time variable which
                                will be filled by the API
 */

qapi_Status_t qapi_Coap_Get_Exchange_Lifetime(qapi_Coap_Session_Hdl_t  session  ,uint32_t *exchange_lifetime)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/* @fn    int qapi_coap_parse_message()
 * @brief This function is used to Parse coap message
 * @param  packet      - pointer to coap message 
 *         data         - Pointer to buffer need to parse
 *         data_len     - Length of buffer 
 *         umem         - indicate user sapce 
 */


qapi_Status_t qapi_Coap_Parse_Message(qapi_Coap_Session_Hdl_t  sessionhandle, qapi_Coap_Packet_t **packet, uint8_t *data, uint16_t data_len)

{
   return QAPI_ERR_NOT_SUPPORTED;
}



qapi_Status_t qapi_Coap_Set_Extended_Config_Option(qapi_Coap_Session_Hdl_t session , qapi_Coap_Extended_Config_Options_t option,
                                                                                                       void * val , int32_t len)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/** 
 * @fn  qapi_Status_t qapi_Coap_Set_Header_Ext
 *
 * @brief  This function is used to set extended header parameters.
 *  
 * @param session      CoAP Session Handle returned by Create Session .
 * @param message      Pointer to coap message header to be filled.
 * @param header_type  Option number of header field to be filled.
 * @param header_val   Pointer to header value.
 * @param val_len      Length of header value.
 *
 * @return  on success - QAPI_OK 
 *          on error   - QAPI_ERROR
 */
 qapi_Status_t qapi_Coap_Set_Header_Ext(qapi_Coap_Session_Hdl_t sessionhandle ,qapi_Coap_Packet_t *message , uint16_t header_type , const void *header_val , size_t val_len)
 {
   return QAPI_ERR_NOT_SUPPORTED;
 }

/** 
 * @fn  qapi_Status_t qapi_Coap_Get_Header_Ext
 *
 * @brief  This function is used to get extended header parameters. 
 *
 *
 * @param session      CoAP Session Handle returned by Create Session .
 * @param message      Pointer to coap message header to get.
 * @param option       Output option in length and value format. Resources are allocated internally.
 *                     The application is responsible for releasing any allocated resources .
 * @param opt_num      Option number value to be fetched.
 * @param all_options  1 -> all extended options to be fetched.
 *                     0 -> single extended option to be fetched.
 * 
 * @return  on success - QAPI_OK 
 *          on error   - QAPI_ERROR.
 *
 */
qapi_Status_t qapi_Coap_Get_Header_Ext(qapi_Coap_Session_Hdl_t sessionhandle ,qapi_Coap_Packet_t * message , qapi_Coap_Ext_Msg_Option_t **option, 
                                                  uint16_t opt_num, uint8_t all_options)
 {
   return QAPI_ERR_NOT_SUPPORTED;
 }

/**
 * @versiontable{2.0,2.45,
 * Data\_Services 1.2.0  &  Introduced. @tblendline
 * }
 *         This function is used to query the current snapshot for ongoing uplink .
 *         blockwise transfer. 
 * @param sessionHandle    CoAP Session Handle returned by Create Session .
 * @param block_info       Pointer to blockwise transfer info.
 * 
 * return  on success   -  QAPI_OK
 *         on error     - QAPI_ERROR
 *
 */
qapi_Status_t qapi_Coap_Get_Block_Transfer_Info(qapi_Coap_Session_Hdl_t sessionhandle,  qapi_Coap_Block_Transfer_Info_t **block_info )
{
   return QAPI_ERR_NOT_SUPPORTED;
}


qapi_Status_t  qapi_Coap_Register_Event(qapi_Coap_Session_Hdl_t hdl , qapi_Coap_Event_Callback_t event_cb_ptr)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @fn  qapi_Status_t qapi_Coap_Get_Header_Ext
 *
 * @brief This is function is used to get the extended configuration parameters.
 * @param hndl COAP handle.
 * @param ext_option Option Type which we need to get.
 * @param val values to be get.
 * @param val_len val's length.
 *
 * @return QAPI_OK on success or QAPI_ERR_NO_ENTRY on failure in retrieving the handle
 *								 QAPI_ERR_INVALID_PARAM if the pointer passed is null for value.
 *								 QAPI_ERR_NOT_SUPPORTED for unsupported extended config options
 *								 QAPI_ERROR for all other failures
 */
qapi_Status_t qapi_Net_COAP_Get_Extended_Config_Option(qapi_Coap_Session_Hdl_t sessionHandle,
                                                qapi_Net_Coap_Get_Extended_Config_Options_t ext_option,
                                                void* val, uint32_t val_len)
{
	return QAPI_ERR_NOT_SUPPORTED;
}


