/*!
  @file
  qapi_httpc_ext.c
*/

/*===========================================================================

  Copyright (c) 2020-2023 Qualcomm Technologies, Inc. All Rights Reserved

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
=========================================================================== */



#include "qapi_httpc.h" 


/**
 * @brief Start/Re-start HTTP client module
 *
 * @details Function is invoked to start or restart the HTTP client after it is
 *          stopped via call to qapi_Net_HTTPc_Stop().
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Start(void)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Stop HTTP client module
 *
 * @details Function is invoked to stop the HTTP client after it was started via 
            call to qapi_Net_HTTPc_Start().
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Stop(void)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Creates new HTTP client session 
 *
 * @details In order to create a client session the caller needs to invoke this function 
 *          and the handle to the newly created context is returned if success. As part 
 *          of the function call, an user callback function is registered with the HTTP 
 *          client module that gets invoked for that particular session if there is some
 *          response data from the HTTP server. Passing in the SSL context information
 *          would ensure that a secure session would get created.           
 *
 * @param[in] timeout   Timeout(ms) on session method (zero is not recommended)
 * @param[in] ssl_ctx   SSL context for HTTPs connect (zero for no HTTPs session support)
 * @param[in] callback  Register callback function, NULL for not support callback
 * @param[in] arg       User data payload to be returned by the callback function
 * @param[in] httpc_max_body_length    Max body length on this session
 * @param[in] httpc_max_header_length  Max header length on this session
 *
 * @return On success, qapi_Net_HTTPc_handle_t is returned. NULL otherwise.
 */
qapi_Net_HTTPc_handle_t qapi_Net_HTTPc_New_sess(uint32_t timeout, uint32_t ssl_ctx, qapi_HTTPc_CB_t callback, void* arg, uint32_t httpc_max_body_length, uint32_t httpc_max_header_length)
{
  return NULL;
}

/**
 * @brief Release HTTP client session
 *
 * @details HTTP client session that is connected to the HTTP server is disconnected prior 
            to releasing the resources associated with that session. 
 *
 * @param[in] handle  Handle to the HTTP client session
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Free_sess(qapi_Net_HTTPc_handle_t handle)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Connect HTTP client session to the HTTP server
 *
 * @details HTTP client session is connected to the HTTP server in non-blocking mode. 
 *
 * @param[in] handle  Handle to the HTTP client session
 * @param[in] URL     Server URL informtion  
 * @param[in] port    Server port information 
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Connect(qapi_Net_HTTPc_handle_t handle, const char *URL, uint16_t port)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Connect HTTP client session to the HTTP proxy server
 *
 * @details HTTP client session is connected to the HTTP server via the proxy in non-blocking mode. 
 *
 * @param[in] handle        Handle to the HTTP client session
 * @param[in] URL           Server URL informtion  
 * @param[in] port          Server port information 
 * @param[in] secure_proxy  Secure proxy connection
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Proxy_Connect(qapi_Net_HTTPc_handle_t handle, const char *URL, uint16_t port, uint8_t secure_proxy)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Disconnect HTTP client session from the HTTP server
 *
 * @details HTTP client session that is connected to the HTTP server is disconnected 
            from the HTTP server. 
 *
 * @param[in] handle  Handle to the HTTP client session
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Disconnect(qapi_Net_HTTPc_handle_t handle)
{ 
  return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Process the HTTP client session requests
 *
 * @details HTTP client session requests are processed and sent to the HTTP server. 
 *
 * @param[in] handle   Handle to the HTTP client session
 * @param[in] cmd      HTTP request method information 
 * @param[in] URL      Server URL information
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Request(qapi_Net_HTTPc_handle_t handle, qapi_Net_HTTPc_Method_e cmd, const char *URL)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Set HTTP client session body
 *
 * @details Multiple invocations of this function would result in overwriting the 
            internal buffer with content of the last call. Setting the body length 
            to zero would clear the HTTP body.
 *
 * @param[in] handle    Handle to the HTTP client session
 * @param[in] body      HTTP body related information buffer
 * @param[in] bodylen   HTTP body buffer length
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Set_Body(qapi_Net_HTTPc_handle_t handle, const char *body, uint32_t bodylen)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Set HTTP client session parameter
 *
 * @details Multiple invocations of this function would result in appending the 
            parameter key-value pair information to the internal buffer. 
 *
 * @param[in] handle    Handle to the HTTP client session
 * @param[in] key       HTTP key related inforamtion 
 * @param[in] value     HTTP value associated with the key 
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Set_Param(qapi_Net_HTTPc_handle_t handle, const char *key, const char *value)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Add HTTP client session header field
 *
 * @details Multiple invocations of this function would result in appending the 
            header type-value pair information to the internal buffer. 
 *
 * @param[in] handle    Handle to the HTTP client session
 * @param[in] type      HTTP header type related inforamtion 
 * @param[in] value     HTTP value associated with the header type 
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Add_Header_Field(qapi_Net_HTTPc_handle_t handle, const char *type, const char *value)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Clear HTTP client session header
 *
 * @details Clears the entire content associated with the internal header buffer. 
 *
 * @param[in] handle    Handle to the HTTP client session
 *
 * @return On success, 0 is returned. Other value on error.
 */ 
qapi_Status_t qapi_Net_HTTPc_Clear_Header(qapi_Net_HTTPc_handle_t handle)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Configure HTTP client session securely
 *
 * @details Configure the HTTP client session securely based on the application requirement.  
 *
 * @param[in] handle    Handle to the HTTP client session
 * @param[in] ssl_Cfg   SSL configuration information 
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Configure_SSL(qapi_Net_HTTPc_handle_t handle, qapi_Net_SSL_Config_t *ssl_Cfg)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Configure HTTP client session
 *
 * @details Configure the HTTP client session based on the application requirement.  
 *
 * @param[in] handle      Handle to the HTTP client session
 * @param[in] httpc_Cfg   HTTP client configuration information 
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Configure(qapi_Net_HTTPc_handle_t handle, qapi_Net_HTTPc_Config_t *httpc_Cfg)
{
  return QAPI_ERR_NOT_SUPPORTED;
}


/**
 *  Extended configuration options for HTTP client session based on the application requirement.
 *
 * @param[in] handle        Handle to the HTTP client session.
 * @param[in] option        HTTP client extended configuration option. 
 * @param[in] option_value  HTTP client extended configuration option info.
 * @param[in] option_size   HTTP client extended configuration option size. 
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Extended_Config_Options(qapi_Net_HTTPc_handle_t handle, 
                                                     qapi_Net_HTTPc_Extended_Config_Options_e option, 
                                                     void *option_value , 
                                                     uint32_t option_size)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/**
 *  Get Extended configuration options for HTTP client session based on the application requirement.
 *
 * @param[in] handle        Handle to the HTTP client session.
 * @param[in] option        HTTP client Get extended configuration option. 
 * @param[out] option_value HTTP client Get extended configuration option info.
 * @param[in] option_size   HTTP client extended configuration option size. 
 *
 * @return On success, 0 is returned. Other value on error.
 */
qapi_Status_t qapi_Net_HTTPc_Get_Extended_Config_Options(qapi_Net_HTTPc_handle_t handle,
                                                qapi_Net_HTTPc_Get_Extended_Config_Options_t ext_option,
                                                void* val, uint32_t val_len)
{
  return QAPI_ERR_NOT_SUPPORTED;
}


