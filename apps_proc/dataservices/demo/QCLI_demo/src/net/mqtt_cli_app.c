/*!
  @file
  mqtt_cli_app.c

  @brief
  Implementation of MQTT protocol Related functionality.
*/

/*===========================================================================

  Copyright (c) 2017-2021 Qualcomm Technologies, Inc. All Rights Reserved

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


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include "qurt_types.h"
#include "qcli.h"
#include "qcli_api.h"
#include "qapi_socket.h"
#include "qapi.h"
#include "qapi_netservices.h"
#include "mqtt_cli_app.h"
#include "bench.h"
#include "qurt/qurt_error.h"
#include "qurt/qurt_thread.h"
#include "qapi_socket.h"
#include "qapi_mqtt.h"
#include "qapi_net_status.h"

#define MAX_PUB_MSG_LEN 1000

extern qapi_Net_MQTT_Hndl_t app_mqttcli_ctx[NO_OF_APPS];
extern qapi_Net_MQTT_Config_t mqttdemo_cfg[NO_OF_APPS];
extern qapi_Net_Mqtt_Extended_Config_t mqttdemo_ext_cfg[NO_OF_APPS];

extern uint32_t port_server[NO_OF_APPS];
extern uint32_t allow_unsub_pub[NO_OF_APPS];
extern qapi_Net_Mqtt_Get_Extended_Config_Options_t get_ext_conf[NO_OF_APPS];

extern char *print_ipad(unsigned long);
UINT memscpy(void *dst, UINT dst_size, const void *src, UINT src_size);
extern char *print_ip6(ip6_addr * addr,char * str);
extern ip6_addr ip6unspecified;
extern QCLI_Group_Handle_t qcli_mqtt_handle;

int     user_data_mqtt[NO_OF_APPS] = {0};
char    pub_msg[MAX_PUB_MSG_LEN];
int32_t pub_msg_len = 0;

int deserialize;

void mqtt_cli_disconnect(int app_id);

void mqtt_cli_connect_callback(qapi_Net_MQTT_Hndl_t mqtt, int32_t reason)
{
  int i;
  int app_id = 0;

  for (i=0; i < NO_OF_APPS; i++)
  {
    if (app_mqttcli_ctx[i] != NULL && app_mqttcli_ctx[i] == mqtt)
    {
      app_id = i;
      break;
    }
  }

  MQTT_PRINTF("REASON = %d\n",reason);

  if(reason == QAPI_NET_MQTT_CONNECT_SUCCEEDED_E)
    MQTT_PRINTF("App %d : MQTT Connected\n", app_id);

  else if (reason == QAPI_NET_MQTT_CONNECT_PROTOCOL_VER_ERROR_E)

     MQTT_PRINTF("App %d : MQTT Connection Refused- Unacceptable proocol version\n", app_id);

  else if (reason == QAPI_NET_MQTT_CONNECT_IDENTIFIER_ERROR_E)
     MQTT_PRINTF("App %d : MQTT Connection Refused- Identifier rejected\n", app_id);

  else if (reason == QAPI_NET_MQTT_CONNECT_SERVER_UNAVAILABLE_E)
     MQTT_PRINTF("App %d : MQTT Connection Refused- Server unavailable\n", app_id);

  else if (reason == QAPI_NET_MQTT_CONNECT_MALFORMED_DATA_E)
     MQTT_PRINTF("App %d : MQTT Connection Refused- Malformed user name or password\n", app_id);

  else if (reason == QAPI_NET_MQTT_CONNECT_UNAUTHORIZED_CLIENT_E)
     MQTT_PRINTF("App %d : MQTT Connection Refused- Unauthorized client\n", app_id);

  else 
     MQTT_PRINTF("App %d : MQTT Connection failed\n", app_id);

}

void mqtt_cli_subscribe_callback(qapi_Net_MQTT_Hndl_t mqtt,
                                 int32_t reason,
                                 const uint8_t *topic,
                                 int32_t topic_length,
                                 int32_t qos,
                                 const void *sid)
{
  int i;
  int app_id = 0;

  for (i=0; i < NO_OF_APPS; i++)
  {
    if (app_mqttcli_ctx[i] != NULL && app_mqttcli_ctx[i] == mqtt)
    {
      app_id = i;
      break;
    }
  }

  MQTT_PRINTF("App %d : Subscribe Callback: topic %s reason %d\n", app_id, topic, reason);
}

void mqtt_cli_message_callback(qapi_Net_MQTT_Hndl_t mqtt,
                               int32_t reason,
                               const uint8_t *topic,
                               int32_t topic_length,
                               const uint8_t *msg,
                               int32_t msg_length,
                               int32_t qos,
                               const void *sid)
{
  int i;
  int app_id = 0;
  char *topic_str = NULL;

  for (i=0; i < NO_OF_APPS; i++)
  {
    if (app_mqttcli_ctx[i] != NULL && app_mqttcli_ctx[i] == mqtt)
    {
      app_id = i;
      break;
    }
  }

  MEMSET(pub_msg, 0, MAX_PUB_MSG_LEN);
  if(msg_length < MAX_PUB_MSG_LEN)
  {
    memscpy(pub_msg, msg_length, msg, msg_length);
    pub_msg[msg_length] = '\0';
    pub_msg_len = msg_length;
  }
  else
    MQTT_PRINTF("App %d : Message Callback: msg length %d exceeds the max length %d, won't do pub deserialize\n", app_id, msg_length, MAX_PUB_MSG_LEN);
  topic_str = (char *)malloc(topic_length + 1);
  if(topic_str == NULL)
  {
    MQTT_PRINTF("\n App %d : Message Callback: topic malloc failure\n", app_id);
    return;
  }
  memscpy(topic_str, topic_length, topic, topic_length);
  topic_str[topic_length] = '\0';

  MQTT_PRINTF("App %d : Message Callback: topic-%s\n reason %d  msg_length %d\n", app_id, topic_str, reason, msg_length);

  if(topic_str != NULL)
    free(topic_str);

  if(mqttdemo_ext_cfg[app_id].pass_msg_hdr == 1 && msg_length < MAX_PUB_MSG_LEN)
  {
    deserialize = 1;
  }
}

void mqtt_cli_publish_callback(qapi_Net_MQTT_Hndl_t mqtt,
                               enum QAPI_NET_MQTT_MSG_TYPES msgtype,
                               int32_t qos,
                               uint16_t msg_id)
{
  char msg[10] = {0};
  int i;
  int app_id = 0;

  for (i=0; i < NO_OF_APPS; i++)
  {
    if (app_mqttcli_ctx[i] != NULL && app_mqttcli_ctx[i] == mqtt)
    {
      app_id = i;
      break;
    }
  }

  if (msgtype == QAPI_NET_MQTT_PUBACK)
  {
    strlcpy(msg, "PUBACK", strlen("PUBACK")+1);
  }
  else if (msgtype == QAPI_NET_MQTT_PUBREC)
  {
    strlcpy(msg, "PUBREC", strlen("PUBREC")+1);
  }
  else if (msgtype == QAPI_NET_MQTT_PUBCOMP)
  {
    strlcpy(msg, "PUBCOMP", strlen("PUBCOMP")+1);
  }
  else
  {
    strlcpy(msg, "INVALID", strlen("INVALID")+1);
  }

  MQTT_PRINTF("App %d : Publish Callback type %s, QOS %d msg Id %d\n",
              app_id, msg, qos, msg_id);
}

void mqtt_cli_connect(int app_id)
{
  static uint32_t port_client = 3433;
  struct sockaddr_in *sin4;
#ifdef IP_V6
  struct sockaddr_in6 *sin6;
  char ip6addr[50];
#endif
  int count = 1;
  int ret;
  void *ext_val;
  int   ext_val_len;
  int i;
  ret = qapi_Net_MQTT_New(&app_mqttcli_ctx[app_id]);
  if(ret != QAPI_OK) {
    MQTT_PRINTF("Context not created\n");
    return;
  }

  for(i = 0; i <= QAPI_NET_MQTT_EXTENDED_CONFIG_MAX; ++i)
  {
    ext_val = NULL;
    ext_val_len = 0;
    if(QAPI_MQTT_IS_OPTION(&mqttdemo_ext_cfg[app_id], i))
    {
      switch(i)
      {
        case QAPI_NET_MQTT_EXTENDED_CONFIG_SEC_MODE:
          ext_val = (void *)(&(mqttdemo_ext_cfg[app_id].sec_mode));
          ext_val_len = sizeof(mqttdemo_ext_cfg[app_id].sec_mode);
          break;

        case QAPI_NET_MQTT_EXTENDED_CONFIG_PSK_TABLE_T:
#ifdef MQTT_CLIENT_SECURITY
          if(mqttdemo_ext_cfg[app_id].psk)
          {
            ext_val = (void *)(mqttdemo_ext_cfg[app_id].psk);
            ext_val_len = strlen(mqttdemo_ext_cfg[app_id].psk);
          }
          else
            continue;
#endif
          break;

	case QAPI_NET_MQTT_EXTENDED_CONFIG_PASS_MSG_HEADER:
	  ext_val = (void *)(&(mqttdemo_ext_cfg[app_id].pass_msg_hdr));
	  ext_val_len = sizeof(mqttdemo_ext_cfg[app_id].pass_msg_hdr);
	  break;
	case QAPI_NET_MQTT_EXTENDED_CONFIG_PUBACK_TIMEOUT:
       ext_val = (void *)(&(mqttdemo_ext_cfg[app_id].pub_ack_timeout));
       ext_val_len = sizeof(mqttdemo_ext_cfg[app_id].pub_ack_timeout);
       break;

        default:
          break;
      }
      ret = qapi_Net_MQTT_Set_Extended_Config_Option(app_mqttcli_ctx[app_id],
                                      (qapi_Net_Mqtt_Extended_Config_Options_t)i,
                                      ext_val, ext_val_len);
      if(ret != QAPI_OK)
      {
        MQTT_PRINTF("set option failure\n");
      }
      else
      {
        MQTT_PRINTF("set option %d succesfully\n", i);
      }
    }
  }

  qapi_Net_MQTT_Set_Connect_Callback(app_mqttcli_ctx[app_id],
                                     mqtt_cli_connect_callback);
                                     
  qapi_Net_MQTT_Set_Subscribe_Callback(app_mqttcli_ctx[app_id],
                                       mqtt_cli_subscribe_callback);
                                       
  qapi_Net_MQTT_Set_Message_Callback(app_mqttcli_ctx[app_id],
                                     mqtt_cli_message_callback);

  qapi_Net_MQTT_Set_Publish_Callback(app_mqttcli_ctx[app_id],
                                     mqtt_cli_publish_callback);

 if(allow_unsub_pub[app_id])
 {
    ret =  qapi_Net_MQTT_Allow_Unsub_Publish(app_mqttcli_ctx[app_id],TRUE);
    if (ret != QAPI_OK)
    {
      MQTT_PRINTF("qapi_Net_MQTT_Allow_Unsub_Publish returned error %d\n", ret);
      return;
    }
 }

  if(mqttdemo_cfg[app_id].remote.sa_family == AF_INET)
  {
  sin4 = (struct sockaddr_in *)&mqttdemo_cfg[app_id].local;
  sin4->sin_family = AF_INET;
  sin4->sin_port = (uint16_t)htons(port_client);
  sin4->sin_addr.s_addr = 0;
  }
  else
  {
#ifdef IP_V6
  sin6 = (struct sockaddr_in6 *)&mqttdemo_cfg[app_id].local;
  sin6->sin_family = AF_INET6;
  sin6->sin_port = (uint16_t)htons(port_client);
  memscpy(&sin6->sin_addr.s_addr,sizeof(struct in6_addr), &ip6unspecified, sizeof(struct in6_addr));
  #endif
  }

  if(mqttdemo_cfg[app_id].remote.sa_family == AF_INET)
  {
  sin4 = (struct sockaddr_in *)&mqttdemo_cfg[app_id].remote;
  sin4->sin_family = AF_INET;
  sin4->sin_port = (uint16_t)htons(port_server[app_id]);
  MQTT_PRINTF("IP address for remote : %s  Port: %d\n",
              print_ipad(*(long *)&(sin4->sin_addr)), port_server[app_id]);
  }
  else
  {
#ifdef IP_V6
    sin6 = (struct sockaddr_in6 *)&mqttdemo_cfg[app_id].remote;
    sin6->sin_family = AF_INET6;
    sin6->sin_port = (uint16_t)htons(port_server[app_id]);
    MQTT_PRINTF("IP address for remote : %s  Port: %d\n",
             print_ip6(&sin6->sin_addr,&ip6addr[0]), port_server[app_id]);
#endif
  }
  while(count--) {
    port_client += 1;
    MQTT_PRINTF("Connecting...\n");

    /*  Connect Command call */
    ret = qapi_Net_MQTT_Connect(app_mqttcli_ctx[app_id], &mqttdemo_cfg[app_id]);
    if(ret) {
      // MQTT_PRINTF("MQTT Connect State %d\n", app_mqttcli_ctx->conn_state);
      MQTT_PRINTF("MQTT Connect Error type %d\n", ret);
      MQTT_PRINTF("MQTT Connect Failed\n");

      sin4 = (struct sockaddr_in *)&mqttdemo_cfg[app_id].local;
      sin4->sin_port = (uint16_t)htons(port_client);
#ifdef IP_V6
      sin6 = (struct sockaddr_in6 *)&mqttdemo_cfg[app_id].local;
      sin6->sin_port = (uint16_t)htons(port_client);
#endif
      
    } else {
      MQTT_PRINTF("MQTT Connect Successfull\n");
      /*
      ping = 1;
      while(ping) {
              delay_time = curr_time = time_get_secs();
              while((curr_time + 20) > delay_time) {
                      delay_time = time_get_secs();
              }
              qapi_mqtt_ping(app_mqttcli_ctx);

              MQTT_PRINTF("Ping\n");
      } */
      break;
    }
    qurt_thread_sleep_ext(192000 * 5);
  }
}

void mqtt_cli_publish(uint8_t *topic, uint8_t *msg, int qos, int retain, int app_id)
{
  int ret = 0;
  uint32_t msg_len = 0;
  uint16_t msg_id = 0;

  /*    Publish Command call */
  if(msg) {
    msg_len = strlen((const char *)msg);
  }
  MQTT_PRINTF("Published trying...\n");
  ret = qapi_Net_MQTT_Publish_Get_Msg_Id(app_mqttcli_ctx[app_id], (uint8_t *)topic, (uint8_t *)msg,
                              msg_len, qos, retain, &msg_id);
  if(ret) {
    // MQTT_PRINTF("MQTT Publish State %d\n", app_mqttcli_ctx->conn_state);
    MQTT_PRINTF("MQTT Publish Error type %d\n", ret);
    MQTT_PRINTF("MQTT Publish Failed\n");
  } else {
    MQTT_PRINTF("Published Successfull Message_id %d\n", msg_id);
  }
  free(msg);
}

void mqtt_cli_subscribe(uint8_t *topic, int qos, int app_id)
{
  int ret;

  /*    Subscribe Command call */
  MQTT_PRINTF("subscribing...\n");
  ret = qapi_Net_MQTT_Subscribe(app_mqttcli_ctx[app_id], (uint8_t *)topic, qos);
  if(ret) {
    // MQTT_PRINTF("MQTT Subscribe State %d\n", app_mqttcli_ctx->conn_state);
    MQTT_PRINTF("MQTT Subscribe Error type %d\n", ret);
    MQTT_PRINTF("MQTT Subscribe Failed\n");
  } else {
    MQTT_PRINTF("Subscribe Successfull\n");
  }
  free(topic);
}

void mqtt_cli_unsubscribe(uint8_t *topic, int app_id)
{
  int ret;

  /*    Unsubscribe Command call */
  MQTT_PRINTF("Unsubscribing...\n");
  ret = qapi_Net_MQTT_Unsubscribe(app_mqttcli_ctx[app_id], (uint8_t *)topic);
  if(ret) {
    // MQTT_PRINTF("MQTT Unsubscribe State %d\n", app_mqttcli_ctx->conn_state);
    MQTT_PRINTF("MQTT Unsubscribe Error type %d\n", ret);
    MQTT_PRINTF("MQTT Unsubscribe Failed\n");
  } else {
    MQTT_PRINTF("Unsubscribe Successfull\n");
  }
  free(topic);
}

void mqtt_cli_disconnect(int app_id)
{
  int ret;

  /*    Disconnect Command call */
  ret = qapi_Net_MQTT_Disconnect(app_mqttcli_ctx[app_id]);
  if(ret) {
    MQTT_PRINTF("MQTT Disconnect Error type %d\n", ret);
    MQTT_PRINTF("MQTT Disconnect Failed\n");
  } else {
    MQTT_PRINTF("Disconnect Successfull\n");
  }
}

/*mqtt_cli_deserialize() is used to obtain the publish message along with the header. */
void mqtt_cli_deserialize(int app_id)
{
  int ret = QAPI_ERROR;
  int msg_len = 0;
  qapi_Net_MQTTString_t topic_n;
  qapi_Net_MQTT_Pub_Config_t pub;
  msg_len = pub_msg_len;

  MEMSET (&topic_n, 0,sizeof(qapi_Net_MQTTString_t));
  MEMSET (&pub, 0,sizeof(qapi_Net_MQTT_Pub_Config_t));

  if(deserialize == 1)
  {
    ret = qapi_Net_MQTT_Deserialize_Publish_Header(app_mqttcli_ctx[app_id], (unsigned char*)pub_msg, msg_len,&topic_n,&pub);
    if ( ret == QAPI_OK)
    {
       MQTT_PRINTF("\n DUP =%d  Retain = %d  qos =%d  ", pub.dup, pub.retained,pub.qos);
       MQTT_PRINTF("\n payload =%s  payload_len = %d  id =%d  ", pub.payload, pub.payload_length,pub.id);
    }
    else
       MQTT_PRINTF("\n qapi_Net_MQTT_Deserialize_Publish_Header() returned error");
  }

  deserialize = 0;
  MEMSET(pub_msg, 0, MAX_PUB_MSG_LEN);
  pub_msg_len = 0;
}

void mqtt_cli_get_last_error(int app_id)
{
  int ret = QAPI_ERROR;
  qapi_Net_Mqtt_Get_Extended_Config_Options_t ext_config = get_ext_conf[app_id];
  qapi_Net_Module_Last_Error_t *error_data;
  
  /*Get last error command call Command call */
  error_data = (qapi_Net_Module_Last_Error_t *)malloc(sizeof(qapi_Net_Module_Last_Error_t));
  if (error_data != NULL)
  {
    memset(error_data, 0, sizeof(qapi_Net_Module_Last_Error_t));
  	ret = qapi_Net_MQTT_Get_Extended_Config_Option(app_mqttcli_ctx[app_id],ext_config,error_data,sizeof(qapi_Net_Module_Last_Error_t));
  }
  if (ret) 
  {
    MQTT_PRINTF("MQTT Get last error code Failed\n");
  } 
  else 
  {
    MQTT_PRINTF("MQTT Get last error Successfull\n");
	MQTT_PRINTF("MQTT Error Category is %d The Error code is %d",error_data->err_cat, error_data->err_code);
  }

  if(error_data != NULL)
  	free(error_data);
}


