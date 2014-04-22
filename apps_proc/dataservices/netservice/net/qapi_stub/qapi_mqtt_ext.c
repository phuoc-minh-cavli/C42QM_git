/******************************************************************************
 * Copyright © 2020 Qualcomm Technologies, Inc.
 * \file    qapi_mqtt_ext.c
 * \author  Qualcomm Technologies, Inc.
 *
 ****************************************************************************************/
#include <stdbool.h>
#include "qapi_mqtt.h"



qapi_Status_t qapi_Net_MQTT_New(qapi_Net_MQTT_Hndl_t *hndl)
{
   return QAPI_ERR_NOT_SUPPORTED;
}


/**
 * @brief Destroy MQTT Context
 * @param mqtt MQTT Context to be destroyed
 * @return QAPI_OK on Success
 */
qapi_Status_t qapi_Net_MQTT_Destroy(qapi_Net_MQTT_Hndl_t hndl)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Connect to MQTT Broker
 * @param hndl MQTT Handle
 * @param config MQTT Client Configuration
 * @return QAPI_OK on Success or < 0 on Failure
 */
qapi_Status_t qapi_Net_MQTT_Connect(qapi_Net_MQTT_Hndl_t hndl, const qapi_Net_MQTT_Config_t* config)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Disconnect from MQTT Broker
 * @param  hdnl MQTT Handle
 * @return QAPI_OK on Success or < 0 on Failure
 */
qapi_Status_t qapi_Net_MQTT_Disconnect(qapi_Net_MQTT_Hndl_t hndl)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Publish message to a particular topic
 * @param hdnl MQTT handle
 * @param topic MQTT Topic
 * @param msg MQTT Payload
 * @param msg_len MQTT Payload length
 * @param qos QOS to be used for message
 * @param retain Retain flag
 * @return QAPI_OK on Success or <0 on Failure
 */
qapi_Status_t qapi_Net_MQTT_Publish(qapi_Net_MQTT_Hndl_t hndl, const uint8_t* topic, 
                                    const uint8_t* msg, int32_t msg_len, int32_t qos, bool retain)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Publish message to a particular topic
 * @param hdnl MQTT handle
 * @param topic MQTT Topic
 * @param msg MQTT Payload
 * @param msg_len MQTT Payload length
 * @param qos QOS to be used for message
 * @param retain Retain flag
 * @param[out] msg_id of the Mqtt publish msg
 * @return QAPI_OK on Success or <0 on Failure
 */
qapi_Status_t qapi_Net_MQTT_Publish_Get_Msg_Id(qapi_Net_MQTT_Hndl_t hndl,
                                                            const uint8_t* topic, 
                                                            const uint8_t* msg,
                                                            int32_t msg_len,
                                                            int32_t qos,
                                                            bool retain,
                                                            uint16_t *msg_id)

{
   return QAPI_ERR_NOT_SUPPORTED;
}


/**
 * @brief Subscribe to a topic
 * @param mqtt MQTT Handle
 * @param topic Subscription topic
 * @param qos QOS to be used
 * @return QAPI_OK on Success or < 0 on Failure
 */
qapi_Status_t qapi_Net_MQTT_Subscribe(qapi_Net_MQTT_Hndl_t hndl, const uint8_t* topic, int32_t qos)

{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Unsubscribe from a topic
 * @param mqtt MQTT Handle
 * @param topic Topic to unsubscribed
 * @return QAPI_OK on Success or < 0 on Failure
 */
qapi_Status_t qapi_Net_MQTT_Unsubscribe(qapi_Net_MQTT_Hndl_t hndl, const uint8_t* topic)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Set conncet callback, will be invoked when connect is successful
 * @param mqtt MQTT handle
 * @param callback Callback to be invoked
 * @return Success or Failure
 */
qapi_Status_t qapi_Net_MQTT_Set_Connect_Callback(qapi_Net_MQTT_Hndl_t hndl, qapi_Net_MQTT_Connect_CB_t callback)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Set subscribe callback, will be invoked when subscription is granted or denied
 * @param mqtt MQTT handle
 * @param callback Callback to be invoked
 * @return QAPI_OK on Success or < 0 on Failure
 */
qapi_Status_t qapi_Net_MQTT_Set_Subscribe_Callback(qapi_Net_MQTT_Hndl_t hndl, qapi_Net_MQTT_Subscribe_CB_t callback)
{
  return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Set message callback, will be invoked when message is received for a subscribed topic
 * @param mqtt MQTT handle
 * @param callback Callback to be invoked
 * @return QAPI_OK on Success or < 0 on Failure
 */
qapi_Status_t qapi_Net_MQTT_Set_Message_Callback(qapi_Net_MQTT_Hndl_t hndl, qapi_Net_MQTT_Message_CB_t callback)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Set Publish callback, will be invoked when PUBACK(QOS1)/PUBREC,PUBCOMP(QOS2)
 * @param mqtt MQTT handle
 * @param callback Callback to be invoked
 * @return QAPI_OK on Success or < 0 on Failure
 */
qapi_Status_t qapi_Net_MQTT_Set_Publish_Callback(qapi_Net_MQTT_Hndl_t hndl, qapi_Net_MQTT_Publish_CB_t callback)
{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @brief Set allow_unsub_pub, will allow  messages to be received for a unsubscribed topic
 * @param mqtt MQTT handle
 * @param bool  allow_unsub_pub
 * @return QAPI_OK on Success or < 0 on Failure
 */
qapi_Status_t qapi_Net_MQTT_Allow_Unsub_Publish(qapi_Net_MQTT_Hndl_t hndl, bool  allow_unsub_pub)
{
    return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @fn      ini qapi_Net_MQTT_Set_Extended_Config_Option()
 * @brief   This function which is used to set Extended
 *          Config Parameters.It is expected that the user
 *          will call this API before these params are being used
 *          in another API's
 * @param[in] hndl MQTT handle.
 * @param[in] ext_option Option Type which we need to set.
 * @param[in] val values to be set.
 * @param[in] val_len val's length.
 *
 * @return QAPI_OK on success or < 0 on failure.
 */
qapi_Status_t qapi_Net_MQTT_Set_Extended_Config_Option(qapi_Net_MQTT_Hndl_t hndl,
                                                qapi_Net_Mqtt_Extended_Config_Options_t ext_option,
                                                void* val,
                                                int32_t val_len)
{
    return QAPI_ERR_NOT_SUPPORTED;
}

/**
 * @fn      qapi_Net_MQTT_Deserialize_Publish_Header()
 *          This function is used to retrive the PUBLSH message 
 *          along with the header.
 * @param[in] hndl - MQTT handle.
 * @param[in] buf - The raw buffer data.
 * @param[in] buf_len -  The length in bytes of the data in the supplied buffer.
 * @param[out] topic - returned MQTTString, the MQTT topic in the publish.
 * @param[out] pub - .Returned value containing the DUP flag, QOS value, retain flag,
 *                    Packet identifier, publish payload, length of the publish payload.
 *
 * @return QAPI_OK on success or QAPI_ERR_NOT_SUPPORTED on failure.
 */
qapi_Status_t qapi_Net_MQTT_Deserialize_Publish_Header (qapi_Net_MQTT_Hndl_t hndl, unsigned char* buf, int buf_len,
                                          qapi_Net_MQTTString_t* topic_n,qapi_Net_MQTT_Pub_Config_t* pub)

{
   return QAPI_ERR_NOT_SUPPORTED;
}

/**
 *  @func qapi_Net_MQTT_Get_Extended_Config_Option 
 *   Gets the extended configuration parameters.
 * @param[in] hndl MQTT handle.
 * @param[in] ext_option  Option type to get.
 * @param[in] val         Values to be returned.
 * @param[in] val_len         Values to be returned.
 *
 * @return QAPI_OK on success or QAPI_ERR_NO_ENTRY on failure in retrieving the handle
 *								 QAPI_ERR_INVALID_PARAM if the pointer passed is null for value.
 *								 QAPI_ERR_NOT_SUPPORTED for unsupported extended config options
 *								 QAPI_ERROR for all other failures
 */
qapi_Status_t qapi_Net_MQTT_Get_Extended_Config_Option(qapi_Net_MQTT_Hndl_t hndl,
                                                qapi_Net_Mqtt_Get_Extended_Config_Options_t ext_option,
                                                void* val, uint32_t val_len)

{
    return QAPI_ERR_NOT_SUPPORTED;
}

