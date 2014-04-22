/*!
  @file
  qapi_lwm2m_ext.c
*/

/*===========================================================================

  Copyright (c) 2020 Qualcomm Technologies, Inc. All Rights Reserved

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
#include "qapi_lwm2m.h"
/**
* @brief  Registers an application with an LWM2M client. The application gets a handle  
*         on successful registration with the LWM2M client.
*
* @param[in,out] handle  Handle that is provided to the application on successful registration.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Register_App 
(
  qapi_Net_LWM2M_App_Handler_t *handle
)
{
  return QAPI_ERR_NOT_SUPPORTED;
}


/**
* @brief  Registers an application with an LWM2M client along with a callback handle. The application
*         gets a handle on successful registration with the LWM2M client and must use this handle for
*         subsequent calls to the LWM2M client in the APIs.
*
* @param[in,out] handle  Handle that is provided to the application on successful registration.
* @param[in] user_data  Transparent user data payload (to be returned in the user callback).
* @param[in] user_cb_fn  User client callback handle to forward data to the application.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Register_App_Extended
(
  qapi_Net_LWM2M_App_Handler_t *handle, 
  void *user_data, 
  qapi_Net_LWM2M_App_Extended_CB_t user_cb_fn
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}


/**
* @brief  Deregisters an application. Any object instances associated with the handle are 
*         automatically cleaned up as a result of deregistration.
*
* @param[in] handle  Handle that was provided to the application on successful registration.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_DeRegister_App
(
  qapi_Net_LWM2M_App_Handler_t handle
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}


/**
* @brief  Used by the application to indicate to the LWM2M client the object/instance/resource that 
*         the application is interested in observing. Only allowed for standard objects.
*
* @param[in] handle  Handle received after successful application registration.
* @param[in] observe_cb_fn  Application callback to be invoked on a value change.
* @param[in] observe_info  Object/instance/resource information that the application is interested
*                          in monitoring on on the LWM2M client.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Observe 
(
  qapi_Net_LWM2M_App_Handler_t handle,  
  qapi_Net_LWM2M_App_CB_t observe_cb_fn, 
  qapi_Net_LWM2M_Object_Info_t *observe_info
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
* @brief  Used by the application to indicate to the LWM2M client the object/instance/resource that 
*         the application is interested in observing. Only allowed for standard objects.
*         Version 2
*
* @param[in] handle  Handle received after successful application registration.
* @param[in] observe_cb_fn  New application callback to be invoked on a value change.
* @param[in] observe_info  Object/instance/resource information that the application is interested
*                          in monitoring on on the LWM2M client.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Observe_v2 
(
  qapi_Net_LWM2M_App_Handler_t handle,  
  qapi_Net_LWM2M_App_CB_v2_t observe_cb_fn, 
  qapi_Net_LWM2M_Object_Info_v2_t *observe_info
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
* @brief  Used by the application to cancel the observation.
*
* @param[in] handle  Handle received after successful application registration.
* @param[in] observe_info  Object/instance/resource information for which the application
*                          is to cancel the observation.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Cancel_Observe 
(
  qapi_Net_LWM2M_App_Handler_t handle,  
  qapi_Net_LWM2M_Object_Info_t *observe_info
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
* @brief  Used by the application to cancel the observation.
*         Version 2
*
* @param[in] handle  Handle received after successful application registration.
* @param[in] observe_info  Object/instance/resource information for which the application
*                          is to cancel the observation.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Cancel_Observe_v2 
(
  qapi_Net_LWM2M_App_Handler_t handle,  
  qapi_Net_LWM2M_Object_Info_v2_t *observe_info
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}


/**
* @brief  Creates an LWM2M standard/custom object instances from the application. Only creation of 
*         an object instance is allowed at a time. Applications are allowed to create instances of 
*         standard objects at any time and can pass the information associated with the instance. 
*         However, custom/extensible object instances can only be created by the application within 
*         the "bootstrap window" during the bootstrap phase. In case, the application missed the 
*         "bootstrap window" internally "re-bootstrapping" would be set to force the device perform 
*         re-bootstrapping on the next reboot and the application would be allowed to create the 
*         new object instance. It is not required by the application to pass the information of the
*         custom object instance. 
*
* @param[in] handle  Handle received after successful application registration.
* @param[in] lwm2m_data  LWM2M object instance and its resource information.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Create_Object_Instance 
(
  qapi_Net_LWM2M_App_Handler_t handle,   
  qapi_Net_LWM2M_Data_t *lwm2m_data
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
* @brief  Creates an LWM2M standard/custom object instances from the application. Only creation of 
*         an object instance is allowed at a time. Applications are allowed to create instances of 
*         standard objects at any time and can pass the information associated with the instance. 
*         However, custom/extensible object instances can only be created by the application within 
*         the "bootstrap window" during the bootstrap phase. In case, the application missed the 
*         "bootstrap window" internally "re-bootstrapping" would be set to force the device perform 
*         re-bootstrapping on the next reboot and the application would be allowed to create the 
*         new object instance. It is not required by the application to pass the information of the
*         custom object instance.
*         Version 2
*
* @param[in] handle  Handle received after successful application registration.
* @param[in] lwm2m_data  LWM2M object instance and its resource information.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Create_Object_Instance_v2 
(
  qapi_Net_LWM2M_App_Handler_t handle,   
  qapi_Net_LWM2M_Data_v2_t *lwm2m_data
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}


/**
* @brief  Deletes an LWM2M object instance from the application. Only deletion of an object instance 
*         is allowed at a time.
*
* @param[in] handle  Handle received after successful application registration.
* @param[in] instance_info  LWM2M object instance and its resource information.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Delete_Object_Instance 
(
  qapi_Net_LWM2M_App_Handler_t handle,  
  qapi_Net_LWM2M_Object_Info_t *instance_info
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
* @brief  Deletes an LWM2M object instance from the application. Only deletion of an object instance 
*         is allowed at a time.
*         Version 2
*
* @param[in] handle  Handle received after successful application registration.
* @param[in] instance_info  LWM2M object instance and its resource information.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Delete_Object_Instance_v2 
(
  qapi_Net_LWM2M_App_Handler_t handle,  
  qapi_Net_LWM2M_Object_Info_v2_t *instance_info
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
* @brief  Gets the value of the LWM2M object/instance/resource from the application. Only quering of 
*         an object instance is allowed at a time.
*
* @param[in] handle  Handle received after successful application registration.
* @param[in] lwm2m_info_req  Object/instance/resource information requested from the application.
* @param[out] lwm2m_data  Value of the LWM2M object/instance/resource information.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Get
(
  qapi_Net_LWM2M_App_Handler_t handle, 
  qapi_Net_LWM2M_Object_Info_t *lwm2m_info_req, 
  qapi_Net_LWM2M_Data_t **lwm2m_data
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
* @brief  Gets the value of the LWM2M object/instance/resource from the application. Only quering of 
*         an object instance is allowed at a time.
*         Version 2
*
* @param[in] handle  Handle received after successful application registration.
* @param[in] lwm2m_info_req  Object/instance/resource information requested from the application.
* @param[out] lwm2m_data  Value of the LWM2M object/instance/resource information.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Get_v2
(
  qapi_Net_LWM2M_App_Handler_t handle, 
  qapi_Net_LWM2M_Object_Info_v2_t *lwm2m_info_req, 
  qapi_Net_LWM2M_Data_v2_t **lwm2m_data
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
* @brief  Sets the value of LWM2M resources. Only setting of an object instance is allowed at a time.
*
* @param[in] handle  Handle received after successful application registration.
* @param[in] lwm2m_data  Value of the LWM2M resource to be set.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Set 
(
  qapi_Net_LWM2M_App_Handler_t handle, 
  qapi_Net_LWM2M_Data_t *lwm2m_data
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
* @brief  Sets the value of LWM2M resources. Only setting of an object instance is allowed at a time.
*         Version 2
*
* @param[in] handle  Handle received after successful application registration.
* @param[in] lwm2m_data  Value of the LWM2M resource to be set.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Set_v2 
(
  qapi_Net_LWM2M_App_Handler_t handle, 
  qapi_Net_LWM2M_Data_v2_t *lwm2m_data
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}


/**
* @brief  Sends application data, which can be responses or notification events, to the server. For 
*         notifications, notification ID is returned by the LWM2M client and it is the application's
*         responsiblity to store this notification ID. If there is a observation cancellation, LWM2M
*         client will send this notification ID through the registered callback. Applications should 
*         encode the data payload either using the provided qapi_Net_LWM2M_Encode_App_Payload QAPI or
*         can use their own encode functions.  
*
* @param[in] handle  Handle received after successful application registration.
* @param[in/out] lwm2m_app_data  Value of the LWM2M extended/custom object information to be sent.
*                                Application is responsible for releasing any allocated resources.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Send_Message 
(
  qapi_Net_LWM2M_App_Handler_t handle,
  qapi_Net_LWM2M_App_Ex_Obj_Data_t *lwm2m_app_data
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}


/**
* @brief  Utility function to encode application responses/notification data before sending them to 
*         the server. If applications have their own encoding functions, they are free to use those 
*         functions to encode the data payload.  
*
* @param[in] obj_info  Object/URI information.
* @param[in] in_dec_data  Input data that is to be encoded.
* @param[in] in_dec_data_size  Input data size (in buffers).
* @param[in] write_attr  Write attribute information.
* @param[in] enc_content_type  Encoding format of the data.
* @param[out] out_enc_data  Output data buffer in encoded format. Resources are allocated internally.
*                           Application is responsible for releasing any allocated resources.  
* @param[out] out_enc_data_len  Output encoded data buffer length.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Encode_App_Payload 
(
  qapi_Net_LWM2M_Obj_Info_t *obj_info,
  qapi_Net_LWM2M_Flat_Data_t *in_dec_data,
  size_t in_dec_data_size,
  qapi_Net_LWM2M_Attributes_t *write_attr,  
  qapi_Net_LWM2M_Content_Type_t enc_content_type,
  uint8_t **out_enc_data,
  uint32_t *out_enc_data_len
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
* @brief  Utility function to encode application responses/notification data before sending them to 
*         the server. If applications have their own encoding functions, they are free to use those 
*         functions to encode the data payload.  
*
* @param[in] handle  Handle received after successful application registration.
* @param[in] obj_info  Object/URI information.
* @param[in] in_dec_data  Input data that is to be encoded.
* @param[in] in_dec_data_size  Input data size (in buffers).
* @param[in] write_attr  Write attribute information.
* @param[in] enc_content_type  Encoding format of the data.
* @param[out] out_enc_data  Output data buffer in encoded format. Resources are allocated internally.
*                           Application is responsible for releasing any allocated resources.  
* @param[out] out_enc_data_len  Output encoded data buffer length.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Encode_Data 
(
  qapi_Net_LWM2M_App_Handler_t handle, 
  qapi_Net_LWM2M_Obj_Info_t *obj_info,
  qapi_Net_LWM2M_Flat_Data_t *in_dec_data,
  size_t in_dec_data_size,
  qapi_Net_LWM2M_Attributes_t *write_attr,  
  qapi_Net_LWM2M_Content_Type_t enc_content_type,
  uint8_t **out_enc_data,
  uint32_t *out_enc_data_len
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}


/**
* @brief  Utility function to decode the server request data received through the registered application 
*         callback. If applications have their own decoding functions, they are free to use those functions 
*         to decode the data payload.
*
* @param[in] obj_info  Object/URI information.
* @param[in] in_enc_data  Input data that is to be decoded.
* @param[in] in_enc_data_len  Input data length.
* @param[in] dec_content_type  Decoding format of the input data.
* @param[out] out_dec_data  Output data buffer in decoded format. Resources are allocated internally.
*                           Application is responsible for releasing any allocated resources.  
* @param[out] out_dec_data_size  Output decoded data size (in buffers).
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Decode_App_Payload 
(
  qapi_Net_LWM2M_Obj_Info_t *obj_info,
  uint8_t *in_enc_data,
  uint32_t in_enc_data_len,
  qapi_Net_LWM2M_Content_Type_t dec_content_type,
  qapi_Net_LWM2M_Flat_Data_t **out_dec_data,
  size_t *out_dec_data_size
)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/**
* @brief  Utility function to decode the server request data received through the registered application 
*         callback. If applications have their own decoding functions, they are free to use those functions 
*         to decode the data payload.
*
* @param[in] handle  Handle received after successful application registration.
* @param[in] obj_info  Object/URI information.
* @param[in] in_enc_data  Input data that is to be decoded.
* @param[in] in_enc_data_len  Input data length.
* @param[in] dec_content_type  Decoding format of the input data.
* @param[out] out_dec_data  Output data buffer in decoded format. Resources are allocated internally.
*                           Application is responsible for releasing any allocated resources.  
* @param[out] out_dec_data_size  Output decoded data size (in buffers).
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Decode_Data 
(
  qapi_Net_LWM2M_App_Handler_t handle, 
  qapi_Net_LWM2M_Obj_Info_t *obj_info,
  uint8_t *in_enc_data,
  uint32_t in_enc_data_len,
  qapi_Net_LWM2M_Content_Type_t dec_content_type,
  qapi_Net_LWM2M_Flat_Data_t **out_dec_data,
  size_t *out_dec_data_size
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}


/**
* @brief  Wakes up the LWM2M client module to send notifications to the server. Wakeup and sleep states of 
*         the LWM2M client are indicated to the application using the registered callback. Application is 
*         responsible for tracking the states of the LWM2M client.     
*               
*
* @param[in] handle  Handle received after successful application registration.
* @param[in] msg_id  Message ID information associated with the request.
* @param[in] msg_id_len  Message ID information length.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Wakeup 
(
  qapi_Net_LWM2M_App_Handler_t handle,
  uint8_t *msg_id,
  uint8_t  msg_id_len
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}


/**
* @brief   Gets the value of the default Pmin and Pmax information for a specific server.
*               
*
* @param[in] handle  Handle received after successful application registration.
* @param[in] server_id  Server ID information (use QAPI_LWM2M_SERVER_ID_INFO macro).   
* @param[out] p_min  Default "p_min" server attribute value. 
* @param[out] p_max  Default "p_max" server attribute value. 

*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Default_Attribute_Info 
(
  qapi_Net_LWM2M_App_Handler_t handle,
  uint16_t server_id,
  uint32_t *p_min,
  uint32_t *p_max
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}


/**
* @brief  Initialization configuration parameters to LWM2M client module.
*
*
* @param[in] handle       Handler received after successful registration of the application. 
* @param[in] config_data  Config parameter associated with the request. 
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK(0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_ConfigClient
(
  qapi_Net_LWM2M_App_Handler_t handle,
  qapi_Net_LWM2M_Config_Data_t* config_data
)
{
   return QAPI_ERR_NOT_SUPPORTED;
}


/**
* @brief  Configure the server life time information in the LWM2M client from the application. 
*         If the device is connected to only a single server then it is optional to pass the 
*         URL information.  
*
* @param[in] handle  Handle received after successful application registration.
* @param[in] url_info  URL information of the server.  
* @param[in] life_time  Server life time information to be configured.  
* 
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK (0) is returned. Other value on error.
*/
extern qapi_Status_t qapi_Net_LWM2M_Set_ServerLifeTime (qapi_Net_LWM2M_App_Handler_t handle, uint8_t *url_info, uint32_t life_time)
{
   return QAPI_ERR_NOT_SUPPORTED;
}


/**
* @brief  Retrieve the server life time information from the LWM2M client to the application. 
*         If the device is connected to only a single server then it is optional to pass the 
*         URL information.  
*
* @param[in] handle  Handle received after successful application registration.
* @param[in] url_info  URL information of the server.  
* @param[out] life_time  Server life time information that is configured.  
* 
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK (0) is  returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Get_ServerLifeTime (qapi_Net_LWM2M_App_Handler_t handle, uint8_t *url_info, uint32_t *life_time)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
* @brief    This function which is used to set extended.
 *          configuration parameters. It is expected that the user will call this API
 *          before these parameters are being used in another API. 
*
* @param[in] handle  Handle received after successful application registration.
* @param   option  Option type to be set.
* @param   val     Values to be set.
* @param   len     Option length.

* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK (0) is returned. Other value on error.
*/
qapi_Status_t qapi_Net_LWM2M_Set_Extended_Config_Option (qapi_Net_LWM2M_App_Handler_t handle, qapi_LWM2M_Extended_Config_Options_t option,
                                                                                                       void * val , int32_t len)
{
   return QAPI_ERR_NOT_SUPPORTED;
}



