/*!
  @file
  mqtt_cmd.c

  @brief
  Implementation of MQTT command handler functionality.
*/

/*===========================================================================

  Copyright (c) 2021-2023 Qualcomm Technologies, Inc. All Rights Reserved

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
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "qurt_types.h"
#include "qcli.h"
#include "qcli_api.h"
#include "qapi_socket.h"
#include "qapi.h"
#include "qapi_netservices.h"
#include "qapi_mqtt.h"
#include "bench.h"
#include "mqtt_cli_app.h"
#include "net_demo.h"
#include "qurt_thread.h"
#include "timer.h"
#include "qapi_netprofile.h"
#include "qapi_ns_utils.h"
#include "time_svc.h"
#include "stringl/stringl.h"
#include "qapi_net_status.h"



extern uint16_t tick_rate_hz;
#define TICKS_PER_CTICKS       (tick_rate_hz/20)
#define VZW_LH_ROOT_CA_FILENAME                 "ts_sdk_list.bin"
#define VZW_LH_CERTIFICATE_FILENAME             "ts_sdk_crt.bin"

uint32_t port_server[NO_OF_APPS];
uint32_t ssl_enabled[NO_OF_APPS];
uint32_t allow_unsub_pub[NO_OF_APPS];
qapi_Net_Mqtt_Get_Extended_Config_Options_t get_ext_conf[NO_OF_APPS];

qapi_Net_MQTT_Hndl_t app_mqttcli_ctx[NO_OF_APPS] = {NULL};

extern bool thrd_running[NO_OF_APPS];

extern QCLI_Group_Handle_t qcli_net_handle; /* Handle for Net Command Group. */

extern QCLI_Command_t net_cmd_list[];
struct linger so_linger;
int tcpdelack = 0;
extern int deserialize;

extern void mqtt_cli_connect(int app_id);
extern void mqtt_cli_publish(uint8_t* topic, uint8_t* msg, int qos, int retain, int app_id);
extern void mqtt_cli_subscribe(uint8_t* topic, int qos, int app_id);
extern void mqtt_cli_unsubscribe(uint8_t* topic, int app_id);
extern void mqtt_cli_disconnect(int app_id);
extern void mqtt_cli_deserialize(int app_id);

extern char *print_ipad(unsigned long);
extern void mqtt_cli_connect_callback(qapi_Net_MQTT_Hndl_t mqtt, int32_t reason);
extern void mqtt_cli_message_callback(qapi_Net_MQTT_Hndl_t mqtt,
    int32_t reason,
    uint8_t* topic,
    int32_t topic_length,
    uint8_t* msg,
    int32_t msg_length,
    int32_t qos,
    const  void* sid);
extern void mqtt_cli_subscribe_callback(qapi_Net_MQTT_Hndl_t mqtt,
    int32_t reason,
    const uint8_t* topic,
    int32_t topic_length,
    int32_t qos,
    const void* sid);


qapi_Net_MQTT_Config_t mqttdemo_cfg[NO_OF_APPS];
qapi_Net_Mqtt_Extended_Config_t mqttdemo_ext_cfg[NO_OF_APPS];

static char psk_t[NO_OF_APPS][QCLI_PSK_TABLE_MAX_LEN];

static char username[NO_OF_APPS][MAX_UN];
static char password[NO_OF_APPS][MAX_PWD];
static char will_message[NO_OF_APPS][MAX_MSG];
static char will_topic[NO_OF_APPS][MAX_TOPIC_LEN];
static char client_id[NO_OF_APPS][CLI_MAX_CLIENT_ID_LEN] = {"mqtt_test_client 0", "mqtt_test_client 1"};


/*************************************************************************************************************
 *             [0]     [1]      [2]       [3]       [4]  [5]   [6]  [7]   [8] [9]  [10] [11]  [12] [13]
 * conn_mqtt  [-h  localhost]  [-A  bind_address]  [-p  1883]  [-k  60]   [-q  0]  [-r   1]   [-C    0]
                [14]        [15]        [16]            [17]    [18]  [19]       [20]  [21]    [22]  [23]
              [--wil-topic <topic>] [--will-message <message>] [-i <client_id>] [-u username] [-P password]
                [24]        [25]          [26]      [27]      [28]   [29]            [30]    [31]
              [-secmode <security_mode>] [-psk <psk_table>] [-time <time_validity>] [-header <pass_header>]
 **************************************************************************************************************/
 
QCLI_Command_Status_t conn_mqtt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  uint32_t i, retain = FALSE; //TODO: Read the default values from the file
  uint16_t k_alive = 60;
  uint32_t port_client = 3434;
  uint32_t flag = 0;
  struct ip46addr addr;
  uint8_t clean_session = TRUE;
  char* will_topic_ptr = NULL;
  uint8_t will_topic_len = 0;
  char* will_message_ptr = NULL;
  uint8_t will_message_len = 0;
  uint8_t will_qos = 1;
  char* username_ptr = NULL;
  uint8_t username_len = 0;
  char* password_ptr = NULL;
  uint8_t password_len = 0;
  uint8_t client_id_len = strlen((char *)(client_id[app_id]));
  uint8_t time_validity = 0;
  struct sockaddr_in *sin4;
  struct sockaddr_in6 *sin6;
  char buff[32];
  char *addr_ptr;
  uint8_t non_blocking = false;
  uint32_t connack_timed_out_sec = 60;
  int32_t linger_time = -1;
  int p;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  int   sec_mode = 0;
  char *psk = NULL;
  int socketoptioncount = 0;
  int ctr =0;
  int pass_header = 0;

  port_server[app_id] = CLI_MQTT_PORT;
  ssl_enabled[app_id] = 0;
  allow_unsub_pub[app_id] = 0;
  
  if((Parameter_Count < 2) || (Parameter_List == NULL))
  {
      MQTT_PRINTF("Usage: [-app <app_id>] connect [-h <ip>] [-p <port>] [-k <time>] [-q <Qos>] [-e <get extended config>] [-r <retain>] [-C <clean session>] [--will-topic <topic>]"
      "[--will-message <message>] [-i <Client_Id>] [-u <username>] [-P <password>] [-s <ssl_enabled 0/1>][-pubackt <publish ack timeout]");
      MQTT_PRINTF(" [-a <allow_unsub_pub 0/1>] [-nb <non-blocking 0/1>] [-ct <connack time out in sec.>] "
      "[-l <linger time in secs>] [-secmode <security_mode>] [-psk <psk_table>] [-time <time_validity 0/1>], [-tcpdelack <ms>] [-header<pass_hdr>]MQTT CLI Connect command\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }
  
  if(inet_pton(AF_INET, (char*)Parameter_List[1].String_Value, buff ) == 1 && inet_pton(AF_INET6, (char*)Parameter_List[1].String_Value, buff ) == 1)
  {
      MQTT_PRINTF("Invalid Ipv4/Ipv6 Address Usage : MQTT_CLI_Start ServerIP\n");
      result = QCLI_STATUS_SUCCESS_E;
      goto end;
  }

  MEMSET(&mqttdemo_cfg[app_id], 0, sizeof(mqttdemo_cfg[app_id]));
  MEMSET(&mqttdemo_ext_cfg[app_id], 0, sizeof(mqttdemo_ext_cfg[app_id]));
  MEMSET(psk_t[app_id], 0, QCLI_PSK_TABLE_MAX_LEN);
  MEMSET(will_topic[app_id], 0, MAX_TOPIC_LEN);
  MEMSET(will_message[app_id], 0, MAX_MSG);
  MEMSET(password[app_id], 0, MAX_PWD);
  MEMSET(username[app_id], 0, MAX_UN);
  
  
  for (i = 0; i < Parameter_Count; i++)
  {
    if (strcmp((char*)Parameter_List[i].String_Value, "-h") == 0)
    {
        ++i;
        HOST_SET;
        addr_ptr = (char*)Parameter_List[i].String_Value;
        if (inet_pton(AF_INET, addr_ptr, &addr.a) != 0)   
        {
             /* not xxx.xxx.xxx.xxx format */
            if (inet_pton(AF_INET6, addr_ptr, &addr.a) != 0)      
            {
                /* not IPv6 address */
                MQTT_PRINTF("Not an IPv4/6 address of host\n");
                result = QCLI_STATUS_USAGE_E;
                goto end;
            }
            else
            {
                sin6 = (struct sockaddr_in6 *)&mqttdemo_cfg[app_id].remote;
                sin6->sin_family = AF_INET6;
                sin6->sin_port = htons(port_server[app_id]);
                memscpy(&sin6->sin_addr.s_addr, sizeof(struct in6_addr), &addr.a,
                        sizeof(struct in6_addr));
            }
        } 
        else
        {
            sin4 = (struct sockaddr_in *)&mqttdemo_cfg[app_id].remote;
            sin4->sin_family = AF_INET;
            sin4->sin_port = htons(port_server[app_id]);
            memscpy(&sin4->sin_addr.s_addr, sizeof(struct in_addr), &addr.a, sizeof(struct in_addr));
        }
    }
    else if (strcmp((char*)Parameter_List[i].String_Value, "-A") == 0)
    {
        ++i;
        addr_ptr = (char*)Parameter_List[i].String_Value;
        if (inet_pton(AF_INET, addr_ptr, &addr.a) != 0)    
        {   
            /* not xxx.xxx.xxx.xxx format */
            if (inet_pton(AF_INET6, addr_ptr, &addr.a) != 0)       
            {
                /* not IPv6 address */
                MQTT_PRINTF("Not an IPv4/6 address for bind\n");
                result = QCLI_STATUS_USAGE_E;
                goto end;
            }
            else
            {
                sin6 = (struct sockaddr_in6 *)&mqttdemo_cfg[app_id].local;
                sin6->sin_family = AF_INET6;
                sin6->sin_port = htons(port_client);
      // memcpy(&sin6->sin_addr, &bind);    // What to copy ?
      memscpy(&sin6->sin_addr.s_addr, sizeof(struct in6_addr), &addr.a,
            sizeof(struct in6_addr));
            }
        } 
        else
        {
            sin4 = (struct sockaddr_in *)&mqttdemo_cfg[app_id].local;
            sin4->sin_family = AF_INET;
            sin4->sin_port = htons(port_client);
            memscpy(&sin4->sin_addr.s_addr, sizeof(struct in_addr), &addr.a, sizeof(struct in_addr));
        }
    }
    else if (strcmp((char*)Parameter_List[i].String_Value, "-p") == 0)
    {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
            port_server[app_id] = Parameter_List[i].Integer_Value;
        }
    }
    else if (strcmp((char*)Parameter_List[i].String_Value, "-k") == 0)
    {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
            k_alive = Parameter_List[i].Integer_Value;
            
        }
    }
    else if (strcmp((char*)Parameter_List[i].String_Value, "-q") == 0)
    {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
            will_qos = Parameter_List[i].Integer_Value;
            
        }
    }
	else if (strcmp((char*)Parameter_List[i].String_Value, "-e") == 0)
    {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
          	if(Parameter_List[i].Integer_Value == 1)
				get_ext_conf[app_id] = QAPI_NET_MQTT_GET_EXTENDED_CONFIG_LAST_ERR_CODE;
			else
				get_ext_conf[app_id] = QAPI_NET_MQTT_GET_EXTENDED_CONFIG_MAX;
            
        }
		else
			get_ext_conf[app_id] = QAPI_NET_MQTT_GET_EXTENDED_CONFIG_MAX;
    }
    else if (strcmp((char*)Parameter_List[i].String_Value, "-r") == 0)
    {
        ++i;
        if(Parameter_List[i].Integer_Is_Valid)
        {
            if(Parameter_List[i].Integer_Value == 1)
            {
                retain = TRUE;
            }
            else if(Parameter_List[i].Integer_Value > 1 || Parameter_List[i].Integer_Value < 0)
            {
                MQTT_PRINTF("Retain Can not be %d\n", Parameter_List[i].Integer_Value);
                result = QCLI_STATUS_USAGE_E;
                goto end;
            }
        }
        
    }
    else if (strcmp((char*)Parameter_List[i].String_Value, "-C") == 0)
    {
        ++i;
        if(Parameter_List[i].Integer_Is_Valid)
        {
            if(Parameter_List[i].Integer_Value == 0)
            {
                clean_session = FALSE;
                MQTT_PRINTF("Clean_session: %d\n", clean_session);
            }
            else if(Parameter_List[i].Integer_Value > 1 || Parameter_List[i].Integer_Value < 0)
            {
                MQTT_PRINTF("Clean Session Can not be %d\n", Parameter_List[i].Integer_Value );
                result = QCLI_STATUS_USAGE_E;
                goto end;
            }
        }
    }
    else if (strcmp((char*)Parameter_List[i].String_Value, "--will-topic") == 0)
    {
        ++i;
        if(Parameter_List[i].String_Value == NULL)
        {
            MQTT_PRINTF("Topic Name is too short\n");
            result = QCLI_STATUS_USAGE_E;
            goto end;
        }
        else
        {
            strlcpy(will_topic[app_id], (char*)Parameter_List[i].String_Value, MAX_TOPIC_LEN);
            will_topic_ptr = will_topic[app_id];
            if(strlen((char *)(will_topic[app_id])))
            {
                will_topic_len = strlen((char *)(will_topic[app_id]));
            }
        }
    }
    else if(strcmp((char*)Parameter_List[i].String_Value, "--will-message") == 0)
    {
        ++i;
        if(Parameter_List[i].String_Value == NULL)
        {
            MQTT_PRINTF("Will Message is too short\n");
            result = QCLI_STATUS_USAGE_E;
            goto end;
        }
        else
        {
            while(((char*)Parameter_List[i].String_Value) && (i < Parameter_Count)) {
                addr_ptr = (char*)Parameter_List[i].String_Value;
                if(addr_ptr[0] == '-')
                {
                    /*Decrement the i value so that the next option can be read*/
                    i--;
                    break;
                }
                strlcpy(will_message[app_id] + will_message_len, (char*)Parameter_List[i].String_Value, (MAX_MSG - will_message_len));
                will_message_ptr = will_message[app_id];
                if(strlen((char *)(will_message[app_id])))
                {
                    will_message_len = strlen((char *)(will_message[app_id]));
                    will_message[app_id][will_message_len++] = ' ';
                }
                i++;
            }
            MQTT_PRINTF("will_message: %s\n", will_message[app_id]);
        }
        
    }
    else if(strcmp((char*)Parameter_List[i].String_Value, "-i") == 0)
    {
        ++i;
        if(Parameter_List[i].String_Value == NULL)
        {
            /*Allow empty client Id*/
            client_id_len = 0;
        }
        else
        {
            strlcpy(client_id[app_id], (char*)Parameter_List[i].String_Value, CLI_MAX_CLIENT_ID_LEN);
            client_id_len = strlen((char *)(client_id[app_id]));
            if(client_id_len > CLI_MAX_CLIENT_ID_LEN)
            {
                MQTT_PRINTF("Client Id is too long\n");
                result = QCLI_STATUS_USAGE_E;
                goto end;
            }
        }
    }
    else if(strcmp((char*)Parameter_List[i].String_Value, "-u") == 0)
    {
        ++i;
        MQTT_PRINTF("-u is matched\n");
        strlcpy(username[app_id], (char*)Parameter_List[i].String_Value, MAX_UN);
        MQTT_PRINTF("Username is copied\n");
        username_ptr = username[app_id];
        if(username_ptr)
        {
            username_len = strlen((char *)(username[app_id]));
        }
    }
    else if(strcmp((char*)Parameter_List[i].String_Value, "-P") == 0)
    {
        ++i;
        strlcpy(password[app_id], (char*)Parameter_List[i].String_Value, MAX_PWD);
        password_ptr = password[app_id];
        if(password_ptr)
        {
            password_len = strlen((char *)(password[app_id]));
        }

    }
    else if(strcmp((char *)Parameter_List[i].String_Value, "-s") == 0)
    {
        ++i;
        if(Parameter_List[i].Integer_Is_Valid) 
        {
            ssl_enabled[app_id] = Parameter_List[i].Integer_Value;
        }
    }
    else if(strcmp((char *)Parameter_List[i].String_Value, "-a") == 0)
    {
        ++i;
        if(Parameter_List[i].Integer_Is_Valid) 
        {
            allow_unsub_pub[app_id] = Parameter_List[i].Integer_Value;
        }
    }
    else if(strcmp((char *)Parameter_List[i].String_Value, "-nb") == 0)
    {
        ++i;
        if(Parameter_List[i].Integer_Is_Valid) 
        {
            non_blocking = Parameter_List[i].Integer_Value;
        }
    }
    else if (strcmp((char*)Parameter_List[i].String_Value, "-ct") == 0)
    {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
            if (Parameter_List[i].Integer_Value > 0)
            {
                connack_timed_out_sec = Parameter_List[i].Integer_Value;
            }
            else
            {
                MQTT_PRINTF("Not a valide time out value\n");
                result = QCLI_STATUS_USAGE_E;
                goto end;
            }
        }
    }
    else if (strcmp((char*)Parameter_List[i].String_Value, "-l") == 0)
    {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
            if (Parameter_List[i].Integer_Value >= 0)
            {
                socketoptioncount++;
                linger_time = Parameter_List[i].Integer_Value;
            }
            else
            {
                MQTT_PRINTF("Not a valide linger timer value \n");
                result = QCLI_STATUS_USAGE_E;
                goto end;
            }
        }
    }
    else if(strcmp((char*)Parameter_List[i].String_Value, "-secmode") == 0)
    {
      ++i;
      if (Parameter_List[i].Integer_Is_Valid)
      {
        if (Parameter_List[i].Integer_Value >= 0)
        {
            sec_mode = Parameter_List[i].Integer_Value;
            QAPI_MQTT_SET_OPTION(&mqttdemo_ext_cfg[app_id], QAPI_NET_MQTT_EXTENDED_CONFIG_SEC_MODE);
        }
        else
        {
            MQTT_PRINTF("Not a valide security mode value \n");
            result = QCLI_STATUS_USAGE_E;
            goto end;
        }
      }
    }
    else if(strcmp((char*)Parameter_List[i].String_Value, "-psk") == 0)
    {
      ++i;
      if(Parameter_List[i].String_Value)
      {
        strlcpy(psk_t[app_id], (char*)Parameter_List[i].String_Value, QCLI_PSK_TABLE_MAX_LEN);
        psk = psk_t[app_id];
        QAPI_MQTT_SET_OPTION(&mqttdemo_ext_cfg[app_id], QAPI_NET_MQTT_EXTENDED_CONFIG_PSK_TABLE_T);
      }
    }
    else if (strcmp((char*)Parameter_List[i].String_Value, "-time") == 0)
    {
      ++i;
      if (Parameter_List[i].Integer_Is_Valid)
      {
        time_validity = Parameter_List[i].Integer_Value;
      }
    }
	else if (strcmp((char*)Parameter_List[i].String_Value, "-tcpdelack") == 0)
    {
      ++i;
      if (Parameter_List[i].Integer_Is_Valid)
      {
        socketoptioncount++;
        tcpdelack = Parameter_List[i].Integer_Value;
      }
    }
    else if (strcmp((char*)Parameter_List[i].String_Value, "-header") == 0)
    {
      ++i;
      if (Parameter_List[i].Integer_Is_Valid)
      {
        pass_header = Parameter_List[i].Integer_Value;
      }
    }
	else if(strcmp((char*)Parameter_List[i].String_Value, "-pubackt") == 0)
	{
	  ++i;
      if (Parameter_List[i].Integer_Is_Valid)
      {
        mqttdemo_ext_cfg[app_id].pub_ack_timeout = Parameter_List[i].Integer_Value;
		QAPI_MQTT_SET_OPTION(&mqttdemo_ext_cfg[app_id], QAPI_NET_MQTT_EXTENDED_CONFIG_PUBACK_TIMEOUT);
      }
	}
  }

  if(client_id_len > 0)
  {
    strlcpy((char*)(mqttdemo_cfg[app_id].client_id), (char*)(client_id[app_id]), CLI_MAX_CLIENT_ID_LEN);
    mqttdemo_cfg[app_id].client_id_len = client_id_len;
  }
  else
  {
    mqttdemo_cfg[app_id].client_id_len = 0;
  }

  if(!HOST_CHK)
  {
      result = QCLI_STATUS_USAGE_E;
      goto end;
  }
  if(((sec_mode != (int) QAPI_MQTT_PSK) && (QAPI_MQTT_IS_OPTION(&mqttdemo_ext_cfg[app_id],
       QAPI_NET_MQTT_EXTENDED_CONFIG_PSK_TABLE_T))) ||
     ((sec_mode == (int) QAPI_MQTT_PSK) && (!QAPI_MQTT_IS_OPTION(&mqttdemo_ext_cfg[app_id],
       QAPI_NET_MQTT_EXTENDED_CONFIG_PSK_TABLE_T))))
  {
    MQTT_PRINTF("need to set both security mode and psk correctly\n");
    result = QCLI_STATUS_USAGE_E;
      goto end;
  }
#if 0

  MQTT_PRINTF("Client ID: %s\n", client_id);
  if(username_ptr)
      MQTT_PRINTF("username_ptr: %s username_len: %d\n", username_ptr, username_len);
  if(password_ptr)
      MQTT_PRINTF("password_ptr: %s password_len: %d\n", password_ptr, password_len);
  MQTT_PRINTF("k_alive: %d\n", k_alive);
  
#endif

  mqttdemo_cfg[app_id].nonblocking_connect = non_blocking;
  mqttdemo_cfg[app_id].keepalive_duration = k_alive; //qurt_timer_convert_time_to_ticks(k_alive, QURT_TIME_SEC);
  mqttdemo_cfg[app_id].clean_session = clean_session;
  mqttdemo_cfg[app_id].will_topic = (uint8_t *)will_topic_ptr;
  mqttdemo_cfg[app_id].will_topic_len = will_topic_len;
  mqttdemo_cfg[app_id].will_message = (uint8_t *)will_message_ptr;
  mqttdemo_cfg[app_id].will_message_len = will_message_len;
  mqttdemo_cfg[app_id].will_retained = retain;
  mqttdemo_cfg[app_id].will_qos = will_qos;
  mqttdemo_cfg[app_id].username = (uint8_t *)username_ptr;
  mqttdemo_cfg[app_id].username_len = username_len;
  mqttdemo_cfg[app_id].password = (uint8_t *)password_ptr;
  mqttdemo_cfg[app_id].password_len = password_len;
  mqttdemo_cfg[app_id].connack_timed_out_sec = connack_timed_out_sec;

  if(!QAPI_MQTT_IS_OPTION(&mqttdemo_ext_cfg[app_id],QAPI_NET_MQTT_EXTENDED_CONFIG_SEC_MODE))
  {
    /*if  no security mode is set, regard it as cert as the default value*/
    mqttdemo_ext_cfg[app_id].sec_mode = QAPI_MQTT_CERTIFICATE;
    QAPI_MQTT_SET_OPTION(&mqttdemo_ext_cfg[app_id],QAPI_NET_MQTT_EXTENDED_CONFIG_SEC_MODE);
    
  }
  else
  {
    mqttdemo_ext_cfg[app_id].sec_mode = (qapi_Net_Mqtt_Sec_Mode)sec_mode;
  }

  if(socketoptioncount)
  {
    if ((mqttdemo_cfg[app_id].sock_options = malloc(sizeof(qapi_Net_MQTT_Sock_Opts_t) * socketoptioncount)) == NULL)
      {
          result = QCLI_STATUS_ERROR_E;
          goto end;
      }
  }

  if(pass_header == 1)
  {
     mqttdemo_ext_cfg[app_id].pass_msg_hdr = 1;
     QAPI_MQTT_SET_OPTION(&mqttdemo_ext_cfg[app_id],QAPI_NET_MQTT_EXTENDED_CONFIG_PASS_MSG_HEADER);
  }
  
  if (linger_time >= 0)
  {
      so_linger.l_onoff = 1;
      so_linger.l_linger = linger_time;

      mqttdemo_cfg[app_id].sock_options[ctr].level = SOL_SOCKET;
      mqttdemo_cfg[app_id].sock_options[ctr].opt_name = SO_LINGER;
      mqttdemo_cfg[app_id].sock_options[ctr].opt_len = sizeof(struct linger);
      mqttdemo_cfg[app_id].sock_options[ctr].opt_value = &so_linger;
      mqttdemo_cfg[app_id].sock_options_cnt = socketoptioncount;
      ctr++;
  }


    if (tcpdelack > 0)
    {

        MQTT_PRINTF("Settin TCP DEL ACK with value %d\n",tcpdelack);
        mqttdemo_cfg[app_id].sock_options[ctr].level = SOL_SOCKET;
        mqttdemo_cfg[app_id].sock_options[ctr].opt_name = SO_TCP_ACKDELAYTIME;
        mqttdemo_cfg[app_id].sock_options[ctr].opt_len = sizeof(tcpdelack);
        mqttdemo_cfg[app_id].sock_options[ctr].opt_value = &tcpdelack;
        mqttdemo_cfg[app_id].sock_options_cnt = socketoptioncount;
        ctr++;
  }
      
#ifdef MQTT_CLIENT_SECURITY

  memset(&(mqttdemo_cfg[app_id].ssl_cfg), 0, sizeof(mqttdemo_cfg[app_id].ssl_cfg)); 

   if(ssl_enabled[app_id] == 1 || port_server[app_id] == CLI_MQTT_SECURE_PORT)
   {
      mqttdemo_cfg[app_id].ca_list = 
        ((qapi_Net_SSL_Cert_File_Exists(VZW_LH_ROOT_CA_FILENAME, QAPI_NET_SSL_CA_LIST_E) == 
        QAPI_OK) ? VZW_LH_ROOT_CA_FILENAME : NULL);
      mqttdemo_cfg[app_id].cert = 
        ((qapi_Net_SSL_Cert_File_Exists(VZW_LH_CERTIFICATE_FILENAME, QAPI_NET_SSL_CERTIFICATE_E) == 
        QAPI_OK) ? VZW_LH_CERTIFICATE_FILENAME : NULL);
      mqttdemo_ext_cfg[app_id].psk = 
        ((qapi_Net_SSL_Cert_File_Exists(psk, QAPI_NET_SSL_PSK_TABLE_E) == QAPI_OK) ? psk : NULL);
  
      mqttdemo_cfg[app_id].ssl_cfg.protocol = QAPI_NET_SSL_PROTOCOL_TLS_1_2;

      if(mqttdemo_ext_cfg[app_id].sec_mode == QAPI_MQTT_CERTIFICATE)
      {
        mqttdemo_cfg[app_id].ssl_cfg.cipher[0] = QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384;
        mqttdemo_cfg[app_id].ssl_cfg.cipher[1] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384;
        mqttdemo_cfg[app_id].ssl_cfg.cipher[2] = QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256;
        mqttdemo_cfg[app_id].ssl_cfg.cipher[3] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256;
        mqttdemo_cfg[app_id].ssl_cfg.cipher[4] = QAPI_NET_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA;
        mqttdemo_cfg[app_id].ssl_cfg.cipher[5] = QAPI_NET_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA;
        mqttdemo_cfg[app_id].ssl_cfg.cipher[6] = QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384;
        mqttdemo_cfg[app_id].ssl_cfg.cipher[7] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384;
      }
      else if(mqttdemo_ext_cfg[app_id].sec_mode == QAPI_MQTT_PSK)
      {
        mqttdemo_cfg[app_id].ssl_cfg.cipher[0] = QAPI_NET_TLS_PSK_WITH_AES_128_CBC_SHA;
        mqttdemo_cfg[app_id].ssl_cfg.cipher[1] = QAPI_NET_TLS_PSK_WITH_AES_256_CBC_SHA;
        mqttdemo_cfg[app_id].ssl_cfg.cipher[2] = QAPI_NET_TLS_PSK_WITH_AES_128_GCM_SHA256;
        mqttdemo_cfg[app_id].ssl_cfg.cipher[3] = QAPI_NET_TLS_PSK_WITH_AES_256_GCM_SHA384;
        mqttdemo_cfg[app_id].ssl_cfg.cipher[4] = QAPI_NET_TLS_PSK_WITH_AES_128_CBC_SHA256;
        mqttdemo_cfg[app_id].ssl_cfg.cipher[5] = QAPI_NET_TLS_PSK_WITH_AES_256_CBC_SHA384;
      }
      mqttdemo_cfg[app_id].ssl_cfg.verify.domain = 0;
      mqttdemo_cfg[app_id].ssl_cfg.verify.time_Validity = time_validity;
      mqttdemo_cfg[app_id].ssl_cfg.verify.send_Alert = 0;
      mqttdemo_cfg[app_id].ssl_cfg.verify.match_Name[0] = '\0';
      /* Frangment value will be taken only in 512 , 1024 , 2048 , 4096   */
      mqttdemo_cfg[app_id].ssl_cfg.max_Frag_Len = 4096;
      mqttdemo_cfg[app_id].ssl_cfg.max_Frag_Len_Neg_Disable = 0;
   }
#endif

  mqtt_cli_connect(app_id);
  
  if(mqttdemo_cfg[app_id].sock_options != NULL)
  {
      free(mqttdemo_cfg[app_id].sock_options);
  }

end:
  if (Parameter_List != NULL)
  {
    for (p = 0; p<Parameter_Count; p++)
    {
      if (Parameter_List[p].String_Value != NULL)
      {
        free(Parameter_List[p].String_Value);
        Parameter_List[p].String_Value = NULL;
      }
    }
  }
return result;
}

/*****************************************************************************
 *               [0]   [1]    [2]   [3]      [4]  [5]   [6]   [7]
 * publish_mqtt [-t <topic>] [-m <message>] [-q <QoS>] [-r <state>] 
 *****************************************************************************/
 
QCLI_Command_Status_t pub_mqtt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    int i, p;
    char topic[MAX_TOPIC_LEN];
    char *topic_ptr = NULL;
    char *msg_ptr = NULL;
    char *msg = NULL;
    int qos = 0;
    int retain = 0;
    QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  if(NULL == app_mqttcli_ctx[app_id])
  {
    MQTT_PRINTF("No MQTT Connection, Please do MQTT connection first \n");
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  if((Parameter_Count <= 4) || (Parameter_List == NULL)) {
    MQTT_PRINTF(
        "Usage: [-app <app_id>] publish [-t <topic>] [-m <message>] [-q <QoS>] [-r <state>], MQTT CLI Publish command\n");
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }

  msg = (char *) malloc(MAX_SUB_LEN);
    if(msg == NULL)
  {
    MQTT_PRINTF("malloc failed while allocating 65535 bytes for the msg\n");
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }  
    MEMSET(msg, 0, MAX_SUB_LEN);
    MEMSET(topic, 0, MAX_TOPIC_LEN);
    for (i = 0; i < Parameter_Count; i++)
    {
        if(strcmp((char*)Parameter_List[i].String_Value, "-t") == 0)
        {
            ++i;
            if(i > Parameter_Count)
            {
                MQTT_PRINTF("Topic is required to publish\n");
                result = QCLI_STATUS_USAGE_E;
                goto end;
            }
            else
            {
                
                strlcpy(topic, (char*)Parameter_List[i].String_Value, MAX_TOPIC_LEN);
                topic_ptr = topic;
            }
        }
        else if(strcmp((char*)Parameter_List[i].String_Value, "-m") == 0)
        {
        ++i;
        if(i > Parameter_Count - 1) {
          strlcpy(msg, "", MAX_SUB_LEN);
        } else {
          strlcpy(msg, (char*)Parameter_List[i].String_Value, MAX_SUB_LEN);
        }
        msg_ptr = msg;
        }
        else if(strcmp((char*)Parameter_List[i].String_Value, "-q") == 0)
        {
            ++i;
            if(i > Parameter_Count)
            {
                MQTT_PRINTF("Default QoS will be considered\n");
            }
            else if(Parameter_List[i].Integer_Is_Valid)
            {
                qos = Parameter_List[i].Integer_Value;
            }
			else
			{
				MQTT_PRINTF("Invalid value passed for qos, expected value is 32 bit signed integer");
				goto end;
			}
        }
        else if(strcmp((char*)Parameter_List[i].String_Value, "-r") == 0)
        {
            ++i;
            if(i > Parameter_Count)
            {
                MQTT_PRINTF("Retain value is required \n");
                result = QCLI_STATUS_USAGE_E;
                goto end;
            }
            else if(Parameter_List[i].Integer_Is_Valid)
            {
                if(Parameter_List[i].Integer_Value > 1)
                {
                    MQTT_PRINTF("Value of Retain can not be more than 1\n");
                    result = QCLI_STATUS_SUCCESS_E;
                    goto end;
                }
                else
                {
                    retain = Parameter_List[i].Integer_Value;
                }
            }
        }
    } /* for loop */
    
    mqtt_cli_publish((uint8_t*)topic_ptr, (uint8_t*)msg_ptr, qos, retain, app_id);

end:
  if (Parameter_List != NULL)
  {
    for (p = 0; p<Parameter_Count; p++)
    {
      if (Parameter_List[p].String_Value != NULL)
      {
        free(Parameter_List[p].String_Value);
        Parameter_List[p].String_Value = NULL;
      }
    }
  }
  return result;
}

/***********************************************
 *                 [0]  [1]      [2]   [3]     
 * subscribe_mqtt [-t <topic> ] [-q  <qos> ] 
 ***********************************************/
 
QCLI_Command_Status_t sub_mqtt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    
    int i, p;
    //char topic[MAX_TOPIC_LEN];
    char *topic = NULL;
    int qos = 0;
    QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  if(NULL == app_mqttcli_ctx[app_id])
  {
    MQTT_PRINTF("No MQTT Connection, Please do MQTT connection first \n");
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }
  
  if((Parameter_Count <= 2) || (Parameter_List == NULL)) {
    MQTT_PRINTF(
        "Usage: [-app <app_id>] subscribe [-t <topic>] [-q <QoS>],  MQTT CLI Subscribe command\n");
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }
  
    for (i = 0; i < Parameter_Count ; i++)
    {
        if(strcmp((char*)Parameter_List[i].String_Value, "-t") == 0)
        {
          ++i;
          if(i > Parameter_Count)
          {
              MQTT_PRINTF("Topic is required\n");
              result = QCLI_STATUS_USAGE_E;
              goto end;
          }
          else
          {
            topic = (char *) malloc(MAX_SUB_LEN);
            if(topic == NULL)
            {
              MQTT_PRINTF("malloc failed while allocating MAX_SUB_LEN bytes for the topic\n");
              result = QCLI_STATUS_SUCCESS_E;
              goto end;
            }
            strlcpy(topic, (char*)Parameter_List[i].String_Value, MAX_SUB_LEN);
          }
        }
        else if(strcmp((char*)Parameter_List[i].String_Value, "-q") == 0)
        {
            ++i;
            if(i > Parameter_Count)
            {
                MQTT_PRINTF("Default QoS will be considered\n");
            }
            else if(Parameter_List[i].Integer_Is_Valid)
            {
                qos = Parameter_List[i].Integer_Value;
            }
			else
			{
				MQTT_PRINTF("Invalid value passed for qos, expected value is 32 bit signed integer");
				goto end;
			}
        }
    }   /* for loop*/
    
    mqtt_cli_subscribe((uint8_t*)topic, qos, app_id);

end:
	  
  if (Parameter_List != NULL)
  {
    for (p = 0; p<Parameter_Count; p++)
    {
      if (Parameter_List[p].String_Value != NULL)
      {
        free(Parameter_List[p].String_Value);
        Parameter_List[p].String_Value = NULL;
      }
    }
  }
  return result;
}


QCLI_Command_Status_t unsub_mqtt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    
    int i, p;
    //char topic[MAX_TOPIC_LEN];
    char *topic = NULL;
    QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  if(NULL == app_mqttcli_ctx[app_id])
  {
    MQTT_PRINTF("No MQTT Connection, Please do MQTT connection first \n");
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }
  
  if((Parameter_Count < 2) || (Parameter_List == NULL)) {
    MQTT_PRINTF(
        "Usage: [-app <app_id>] subscribe [-t <topic>],  MQTT CLI Un-Subscribe command\n");
    result = QCLI_STATUS_SUCCESS_E;
    goto end;
  }
  
    for (i = 0; i < Parameter_Count ; i++)
    {
        if(strcmp((char*)Parameter_List[i].String_Value, "-t") == 0)
        {
            ++i;
            if(i > Parameter_Count)
            {
                MQTT_PRINTF("Topic is required\n");
                result = QCLI_STATUS_USAGE_E;
                goto end;
            }
            else
            {
                topic = (char *) malloc(MAX_SUB_LEN);
        if(topic == NULL)
        {
          MQTT_PRINTF("malloc failed while allocating MAX_SUB_LEN bytes for the topic\n");
          result = QCLI_STATUS_SUCCESS_E;
          goto end;
        }
                strlcpy(topic, (char*)Parameter_List[i].String_Value, MAX_SUB_LEN);
            }
        }
    }   /* for loop*/

    mqtt_cli_unsubscribe((uint8_t*)topic, app_id);

end:

  if (Parameter_List != NULL)
  {
    for (p = 0; p<Parameter_Count; p++)
    {
      if (Parameter_List[p].String_Value != NULL)
      {
        free(Parameter_List[p].String_Value);
        Parameter_List[p].String_Value = NULL;
      }
    }
  }
  return result;
}

QCLI_Command_Status_t disconn_mqtt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int p;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  if(NULL == app_mqttcli_ctx[app_id])
  {
    MQTT_PRINTF("No MQTT Connection, Please do MQTT connection first \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }
  
  mqtt_cli_disconnect(app_id);
    
  if(app_mqttcli_ctx[app_id] != NULL)
  {
    if (qapi_Net_MQTT_Destroy(app_mqttcli_ctx[app_id]) == QAPI_OK)
    {
      app_mqttcli_ctx[app_id] = NULL;
    }
  }

end:
  if (Parameter_List != NULL)
  {
    for (p = 0; p<Parameter_Count; p++)
    {
      if (Parameter_List[p].String_Value != NULL)
      {
        free(Parameter_List[p].String_Value);
        Parameter_List[p].String_Value = NULL;
      }
    }
  }
  return result;
}

/* deserialize mqtt() is used to retrive the publish message along with the header */
QCLI_Command_Status_t deserialize_mqtt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int p;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  if(NULL == app_mqttcli_ctx[app_id]) {
    MQTT_PRINTF("No MQTT Connection, Please do MQTT connection first \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

  if(deserialize == 1)
    mqtt_cli_deserialize(app_id);

end:
  if (Parameter_List != NULL)
  {
     for (p = 0; p<Parameter_Count; p++)
    {
      if (Parameter_List[p].String_Value != NULL)
      {
        free(Parameter_List[p].String_Value);
        Parameter_List[p].String_Value = NULL;
      }
    }
  }
  return result;
}

/* destroy_mqtt() is used to disconnect from MQTT broker and destroy the MQTT context */
QCLI_Command_Status_t destroy_mqtt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  int p;
  int ret = QAPI_ERROR;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  if(NULL == app_mqttcli_ctx[app_id]) {
    MQTT_PRINTF("No MQTT Connection, Please do MQTT connection first \n");
    result = QCLI_STATUS_ERROR_E;
    goto end;
  }

    ret = qapi_Net_MQTT_Disconnect(app_mqttcli_ctx[app_id]);
    if(ret != QAPI_OK)
      MQTT_PRINTF("MQTT Disconnect Failed\n");
    else
      MQTT_PRINTF("Disconnect Successfull\n");

    if(qapi_Net_MQTT_Destroy(app_mqttcli_ctx[app_id]) == QAPI_OK)
    {
      app_mqttcli_ctx[app_id] = NULL;
    }

end:
  if (Parameter_List != NULL)
  {
     for (p = 0; p<Parameter_Count; p++)
    {
      if (Parameter_List[p].String_Value != NULL)
      {
        free(Parameter_List[p].String_Value);
        Parameter_List[p].String_Value = NULL;
      }
    }
  }
  return result;
}

QCLI_Command_Status_t get_last_error_mqtt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
	int p;
  	int ret = QAPI_ERROR;
  	QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
	qapi_Net_Mqtt_Get_Extended_Config_Options_t ext_config = get_ext_conf[app_id];
	qapi_Net_Module_Last_Error_t *error_data;

	
  	if(NULL == app_mqttcli_ctx[app_id]) {
   		MQTT_PRINTF("No MQTT Handle, Please do MQTT connection first \n");
    	result = QCLI_STATUS_ERROR_E;
    	goto end;
  	}

	if ((Parameter_Count > 2) || (Parameter_List == NULL)) {
        MQTT_PRINTF("Mqtt_last_error : Invalid Params or More than necessary Params passed\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
  
    error_data = (qapi_Net_Module_Last_Error_t *)malloc(sizeof(qapi_Net_Module_Last_Error_t));

	if (error_data != NULL)
    {
    	memset(error_data, 0, sizeof(qapi_Net_Module_Last_Error_t));
  		ret = qapi_Net_MQTT_Get_Extended_Config_Option(app_mqttcli_ctx[app_id],ext_config,error_data, sizeof(qapi_Net_Module_Last_Error_t));
  	}
    
    if(ret != QAPI_OK)
      MQTT_PRINTF("MQTT Get Last error Code failed Failed\n");
    else
    {
      MQTT_PRINTF("The Last error category is %d Last error code is %d", error_data->err_cat, error_data->err_code);
    }
	
	if(error_data != NULL)
  		free(error_data);

	end:
  	if (Parameter_List != NULL)
  	{
    	for (p = 0; p<Parameter_Count; p++)
    	{
      		if (Parameter_List[p].String_Value != NULL)
      		{
        		free(Parameter_List[p].String_Value);
        		Parameter_List[p].String_Value = NULL;
      		}
    	}
  	}
  	return result;
}

QCLI_Command_Status_t mqtt_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = MQTT_COMMAND_HANDLER_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
  cmd_parser_temp->Command_Index = 22;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t 
proc_mqtt_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    int i;
	char *command = NULL;
    QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

    if(Parameter_Count < 1)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

	command = (char*)Parameter_List[0].String_Value;

    if (0 == strcmp(command, "connect")) {
        result = conn_mqtt(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else if (0 == strcmp(command, "publish")) {
        result = pub_mqtt(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else if (0 == strcmp(command, "subscribe")) {
         result = sub_mqtt(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else if (0 == strcmp(command, "unsubscribe")) {
         result = unsub_mqtt(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else if (0 == strcmp(command, "disconnect")) {
         result = disconn_mqtt(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else if (0 == strcmp(command, "deserialize")) {
         result = deserialize_mqtt(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else if (0 == strcmp(command, "destroy")) {
         result = destroy_mqtt(Parameter_Count , &Parameter_List[1], app_id);
    }
	else if(0 == strcmp(command, "lasterror")) {
    	 result = get_last_error_mqtt(Parameter_Count - 1, &Parameter_List[1], app_id);
    }
    else
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

end:
  if (Parameter_List != NULL && result == QCLI_STATUS_USAGE_E)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }

  if(command)
  {
    command = NULL;
  }

  return result;
}
