/*!
  @file
  netcmd_module.c

  @brief
  Networking stack related utilities implementation.
*/
/*===========================================================================
  Copyright (c) 2017-2023  by Qualcomm Technologies, Inc.  All Rights Reserved.
 
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
 ======================================================================*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "qcli.h"
#include "pal.h"
#include "qcli_api.h"
#include "qapi_socket.h"
#include "qapi.h"
#include "qapi_netservices.h"
#include "bench.h"
#include "net_demo.h"
#include "qapi_netprofile.h"
#include "qapi_dnsc.h"
#include "qapi_timer.h"
#include "qapi_netstat.h"
#include "qapi_netfilter.h"



/* local declaration*/
#define QAPI_NET_DNS_SERVER_MAX 4
#define QAPI_NET_DNS_MAX_ADDRESS_LENGTH 48
#define PING_DEFAULT_V4_TTL_VAL 64
#define PING_DEFAULT_TIMEOUT 1000   


extern TX_BYTE_POOL* byte_pool_qcli;

extern QCLI_Command_Status_t httpc_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t ping(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t autoip(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t ifconfig(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t ping6(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t dnsc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t set_ipv6_router_prefix(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t cfg_netbuf(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t route6(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t route(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t mqtt_command_handler(uint32_t Parameter_Count,
                                           QCLI_Parameter_t *Parameter_List);
/* Device Info client handler */
QCLI_Command_Status_t device_info_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
                                           
static QCLI_Command_Status_t ping3(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t ping6_3(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t set_sock_opt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

QCLI_Command_Status_t coap_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

static QCLI_Command_Status_t server_abort(void);
static int test_for_quit_s(void);
static int validate_interface(char * interface);
static QCLI_Command_Status_t benchtoolconfig(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t get_nwstats(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t stack_filter_configure (uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

const QCLI_Command_t net_cmd_list[] =
{
    /* Command_Function, Start_Thread, Command_String, Usage_String, Description, Abort_function */
    {ping, false, "ping", "[-app <app id: 0/1>] <host> [-c <count>] [-s <size>] [-w <timeout (in ms)>] [-B <source ip address>]", "IPv4 ping",server_abort},
    {autoip, false, "autoipv4", "[-app <app id: 0/1>] <interface name>", "Get IPv4 link-local address",NULL},
    {ifconfig, false, "ifconfig", "[-app <app id: 0/1>] [<interface name>]\n"
                                  "            [-app <app id: 0/1>] ifconfig [{wlan0|wlan1} <ipv4addr> <subnetmask> <default gateway>]", "Display/configure a network interface",NULL},
    {ping6, false, "ping6", "[-app <app id: 0/1>] <host> [-c <count>] [-s <size>] [-I <interface name>] [-w <timeout (in ms)>] [-B <source ip address>] ", "IPv6 ping",server_abort},
    {set_ipv6_router_prefix, false, "prefix", "[-app <app id: 0/1>] <interface name> <ipv6addr> <prefixlen> <prefix_lifetime> <valid_lifetime>", "Set IPv6 router prefix",NULL},
    {dnsc, false, "dnsc", "[-app <app id: 0/1>] [start|stop]\n"
                          "   [-app <app id: 0/1>] [addsvr <server> <Interface Name> <id>]\n"
                          "   [-app <app id: 0/1>] [delsvr <Interface Name> <id>]\n"
                          "   [-app <app id: 0/1>] [get_svr_id <server> <Interface Name>]\n"
                          "   [-app <app id: 0/1>] [resolve <host> <Interface Name> [v4|v6]]\n"
                          "...[-app <app id: 0/1>] [hostbyname <host_name> <Interface name> [v4|v6]]" , "Perform DNS client operations",NULL},
    {cfg_netbuf, false, "profile", "[-app <app id: 0/1>] custom <pool_size> <buf_size> <num_buf> <buf_size> <num_buf>\n"
                                     "[-app <app id: 0/1>] set [1=performance, 2=best effort, 3=memory optimizied]\n"
                                     "[-app <app id>: 0/1] show\n", "Update netbuf pool",NULL},
#ifdef INCLUDE_SSLDEMO

	{cert_command_handler, false, "cert", "[-app <app id: 0/1>] <store|delete|list|get|convert>\n",
                                        "Perform certificate management operations\n"
                                        "Type command name to get more info on usage. For example \"cert store\".\n"},

    {ssl_command_handler, false, "ssl", "[-app <app id: 0/1>] <start|stop|config|cert>\n",
                                        "Perform SSL operations\n"
                                        "Type command name to get more info on usage. For example \"ssl start\".\n",NULL},
    {benchrx, false, "benchrx", "[-app <app id: 0/1>] {tcp|udp|ssl} <port> [v4|v6] [<multicast IP>]", "Run the receive traffic test",NULL},
    {benchtx, false, "benchtx", "[-app <app id: 0/1>] <Rx IP> <port> {tcp|udp|ssl} <msg size> {0|1} {<time to TX (sec)>|<num of msg to TX>} {<delay between msg (ms)>} [v4|v6]\n"
#else
    {benchrx, false, "benchrx", "[-app <app id: 0/1>] {tcp|udp} <port> [v4|v6] [<multicast IP>]\n", "Run the receive traffic test",NULL},
    {benchtx, false, "benchtx", "[-app <app id: 0/1>] <Rx IP> <port> {tcp|udp} <msg size> {0|1} {<time to TX (sec)>|<num of msg to TX>} {<delay between msg (ms)>} [v4|v6]\n"
#endif
                                "e.g. benchtx fe80::865d:d7ff:fe40:3498 wlan1 2390 udp 1400 1 100 0 v6",
                                "     benchtx 2001:db8:85a3:8d3:1319:8a2e:370:734 2390 udp 1400 0 30 5 v6\n"
                                "     benchtx 192.168.1.100 2390 tcp 500 0 30 0",NULL},
                                //" Run the transmit traffic test"},
#ifdef QCA4020
    {uapsdtest, false, "uapsdtest", "[-app <app id: 0/1>] {Remote IP} <port> <the number of packets> <time interval> <access category>\r\n", "Run the transmmit traffic test to send packets with specified ac",NULL},
#endif

    {httpc_command_handler, false, "httpc", "[-app <app id: 0/1>] [start|stop|connect|disconnect|get|post|put|patch]\n", "Perform HTTP Client operations\n Type command name to get more info on usage. For example \"httpc\".\n", NULL},
    {route,false, "route"," "," ",NULL},
    {route6,false,"route6"," "," ",NULL},
    {benchquit,false,"benchquit","[-app <app id: 0/1>] <rx|tx>","Quit benchrx/benchtx",NULL},
    {mqtt_command_handler, false, "mqtt", "[-app <app id: 0/1>] [connect|subscribe|unsubscribe|publish|disconnect|deserialize|destroy]\n",
                                "Perform MQTT Client operations\n Type command name to get more info on usage. For example \"mqtt\".\n", NULL},
    {benchtoolconfig,false, "0/1-Disable/Enable CAT4 Benchtool    2/3- Enable/disable us_delay\n"," "," ",NULL},
    {device_info_command_handler, false, "devinfo", "execute 'help' to get commands parameter list\n",
                                                    "Perform Device Info Client operations\n Type command name to get more info on usage. For example \"devinfo\".\n", NULL},
    {ping3, false, "ping3", "[-app <app id: 0/1>] <host> [-c <count>] [-s <size>] [-w <timeout (in ms)>] [-B <source ip address>] [-t <Time To Live>]", "IPv4 ping",server_abort},
    {ping6_3, false, "ping6_3", "[-app <app id: 0/1>] <host> [-c <count>] [-s <size>] [-I <interface name>] [-w <timeout (in ms)>] [-B <source ip address>] [-t <Time To Live>]", "IPv6 ping",server_abort},
    {set_sock_opt, false, "set_sock_opt", "[-app <app.id: 0/1] [-e<enable/disable sack>] [-i <irt>] [-m <max_rxt>] [-o <overall_timer>] [-s <mss>] [-k<keep_alive>] [-a <keep_idle>] [-t <keep_intvl>] [-c <keep_cnt>] [-r<rcv_buf>] [-b<snd_buf>] [-d<benchtx_sleep in ms>][-l<linger>][-f<reassm_timer>][-g<msl>][-p<max_persist>][-h<min_rto>][-j<max_rto>][-w<win_scale>][-n<enable_timestamp>][-q<max_sack_block>][-u<ip_ire>][-v<ip_maxfrags>]\n","set socket option", NULL},
    {coap_command_handler , false  , "coap" , "[-app <app id: 0/1>] [create_session|create_connection|init_message|set_header|send_message|disconnect|delete_session|lasterror]\n",
     "Perform COAP Client operations\n Type command name to get more info on usage. For example \"coap\".\n", NULL},
    {get_nwstats, false, "get_nwstats", " [-p <protocol>] [-i <interface_name>] [-o <operation>] "," ", NULL},
    {stack_filter_configure , false , "stack_filter_configure" , "[-IP4 <IPv4 ADDRESS>] [-IP6 <IPv6 Address] [-P<Port Number>] 1/0 Add Filter / Delete Filter	>", "",NULL}
};

const QCLI_Command_Group_t net_cmd_group =
{
    "Net",              /* Group_String: will display cmd prompt as "Net> " */
    sizeof(net_cmd_list)/sizeof(net_cmd_list[0]),   /* Command_Count */
    net_cmd_list        /* Command_List */
};

QCLI_Group_Handle_t qcli_net_handle;     /* Handle for Net Command Group. */

static char msg_invalid_id[] = "Invalid server id\n";
static char msg_invalid_iface[] = "Invalid Interface name\n";

static int server_quit;

struct sockaddr_in rclient[NO_OF_APPS];
char * rclient6[NO_OF_APPS] = {NULL};
int  ip_type[NO_OF_APPS] = {0};

extern TX_BYTE_POOL *byte_pool_qcli;

extern bool dam_thrd_running[NO_OF_APPS];

sock_opt option[NO_OF_APPS]={0};
bool enable_cat4 = false;
extern bool enable_us_delay;
/*****************************************************************************
 * This function is used to register the Net Command Group with QCLI.
 *****************************************************************************/
void Initialize_Net_Demo(void)
{
    /* Attempt to reqister the Command Groups with the qcli framework.*/
    qcli_net_handle = QCLI_Register_Command_Group(NULL, &net_cmd_group);
    if (qcli_net_handle)
    {
        QCLI_Printf(qcli_net_handle, "Net Registered\n");
    }

    return;
}

static QCLI_Command_Status_t route6(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = ROUTE6_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
  cmd_parser_temp->Command_Index = 17;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_route6(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{

    qapi_Net_Route_Command_t cmd;
    char ifname[24];
    ip6_addr  addr ,gw;
    uint32_t masklen;
    int e,i;
    qapi_Net_IPv6_Route_List_t routelist;
    char ip_str[130], gw_str[130];
    QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

    memset(&routelist, 0,sizeof(routelist));
    
    if(Parameter_Count ==0 )
    {
       QCLI_Printf(qcli_net_handle, "route [-app <app id: 0/1>] s/show/add/del command\n");
       result = QCLI_STATUS_USAGE_E;
       goto end;
    }

    if (strcmp((char*)Parameter_List[0].String_Value, "add") == 0)
    {
       cmd = QAPI_NET_ROUTE_ADD_E;
    }
     else if(strcmp((char*)Parameter_List[0].String_Value, "del") == 0)
    {
      cmd = QAPI_NET_ROUTE_DEL_E;
    }
    else
    {
      cmd = QAPI_NET_ROUTE_SHOW_E;
    }

    if(Parameter_Count >1 )
   {
     
      strlcpy(ifname ,(char*)Parameter_List[1].String_Value, sizeof(ifname));

      if(inet_pton(AF_INET6, (char*)Parameter_List[2].String_Value, &addr) != 0)
      {
        QCLI_Printf(qcli_net_handle, "Bad Input Address");

        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
 
      masklen = Parameter_List[3].Integer_Value;
      if(masklen <= 0 || masklen > 128)
      {
        QCLI_Printf(qcli_net_handle, "Invalid Mask Length\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
  
      if(inet_pton(AF_INET6, (char*)Parameter_List[4].String_Value, &gw) != 0)
      {
        QCLI_Printf(qcli_net_handle, "Invalid gateway address\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
   }
    e= qapi_Net_IPv6_Route(ifname,cmd,addr.s_addr,&masklen,gw.s_addr,&routelist);
    
    if(e == 0)
    {
      if(cmd == QAPI_NET_ROUTE_SHOW_E)
      {
        if(routelist.route_Count != 0)
        {
          QCLI_Printf(qcli_net_handle, " Interface              Destination              PrefixLen               NextHop             RouterIndex\n");
          QCLI_Printf(qcli_net_handle, "\n");
          QCLI_Printf(qcli_net_handle, "\n");
          for(i =0 ;i < routelist.route_Count; i++)
          { 
              inet_ntop(AF_INET6, &routelist.route[i].ipv6RouteDest, ip_str, sizeof(ip_str));
              inet_ntop(AF_INET6, &routelist.route[i].ipv6RouteNextHop, gw_str, sizeof(gw_str));
  
            QCLI_Printf(qcli_net_handle,"%s             %s             %d               %s             %d",routelist.route[i].ifName,
  ip_str,routelist.route[i].ipv6RoutePfxLength,gw_str,routelist.route[i].ipv6RouteIfIndex);
            QCLI_Printf(qcli_net_handle, "\n");
          }
        }
        else
        {
          QCLI_Printf(qcli_net_handle,"No Destination based route present\n"); 
        }
      }

      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }
    else
    {
      QCLI_Printf(qcli_net_handle, "Route Command UnSuccessful");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

 end:
  if (Parameter_List!= NULL)
  {
      for (i = 0; i<Parameter_Count; i++)
      {
        if (Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(Parameter_List[i].String_Value);
          Parameter_List[i].String_Value = NULL;
        }
      }
    }
  return result;
}

static QCLI_Command_Status_t route(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = ROUTE_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
  cmd_parser_temp->Command_Index = 16;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t 
proc_route(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{

    qapi_Net_Route_Command_t cmd;
    char ifname[24];
    uint32_t  addr ,mask,gw;
    int e,i;
    qapi_Net_IPv4_Route_List_t routelist;
    char ip_str[20], mask_str[20], gw_str[20];
    QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

    memset(&routelist, 0,sizeof(routelist));
    
    if(Parameter_Count ==0 )
    {
       QCLI_Printf(qcli_net_handle, "route [-app <app id: 0/1>] /show/add/del command\n");
       result = QCLI_STATUS_USAGE_E;
       goto end;
    }

    if (strcmp((char*)Parameter_List[0].String_Value, "add") == 0)
    {
       cmd = QAPI_NET_ROUTE_ADD_E;
    }
     else if(strcmp((char*)Parameter_List[0].String_Value, "del") == 0)
    {
      cmd = QAPI_NET_ROUTE_DEL_E;
    }
    else
    {
      cmd = QAPI_NET_ROUTE_SHOW_E;
    }

    if(Parameter_Count >1 )
   {
     
      strlcpy(ifname ,(char*)Parameter_List[1].String_Value, sizeof(ifname));

      if(inet_pton(AF_INET, (char*)Parameter_List[2].String_Value, &addr) != 0)
      {
        QCLI_Printf(qcli_net_handle, "Bad Input Address");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }

      if(inet_pton(AF_INET, (char*)Parameter_List[3].String_Value, &mask) != 0)
      {
        QCLI_Printf(qcli_net_handle, "Invalid subnet mask\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
      if(inet_pton(AF_INET, (char*)Parameter_List[4].String_Value, &gw) != 0)
      {
        QCLI_Printf(qcli_net_handle, "Invalid gateway address\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
      }
   }
    e= qapi_Net_IPv4_Route(ifname,cmd,&addr,&mask,&gw,&routelist);
    
    if(e == 0)
    {
      if(cmd == QAPI_NET_ROUTE_SHOW_E)
      {
        QCLI_Printf(qcli_net_handle, " Interface         Destination         SubnetMask         Next Hop\n");
        QCLI_Printf(qcli_net_handle, "\n");
        if(routelist.route_Count)
        {
          for(i =0 ;i < routelist.route_Count ; i++)
          { 
            inet_ntop(AF_INET, &routelist.route[i].ipRouteDest, ip_str, sizeof(ip_str));
            inet_ntop(AF_INET, &routelist.route[i].ipRouteMask, mask_str, sizeof(mask_str));
            inet_ntop(AF_INET, &routelist.route[i].ipRouteNextHop, gw_str, sizeof(gw_str));
          
            QCLI_Printf(qcli_net_handle,"%s      %s      %s      %s",routelist.route[i].ifName,
                                                                    ip_str,mask_str,gw_str);
            QCLI_Printf(qcli_net_handle, "\n");
         }
        }
         else
         {
           QCLI_Printf(qcli_net_handle," No Destiantion based route present\n");
         }
      }

      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }
    else
    {
      QCLI_Printf(qcli_net_handle, "Route Command UnSuccessful");
      result = QCLI_STATUS_ERROR_E;
      goto end;
    }

end:
  if (Parameter_List!= NULL)
  {
      for (i = 0; i<Parameter_Count; i++)
      {
        if (Parameter_List[i].String_Value != NULL)
        {
          tx_byte_release(Parameter_List[i].String_Value);
          Parameter_List[i].String_Value = NULL;
        }
      }
  }
  return result;
}


/*****************************************************************************
 *      [0]         [1]  [2]   [3] [4]     [5] [6]  [7] [8] 
 * Ping 192.168.1.1 [-c  100] [-s  1500]  [-w 1000] [-B 192.168.1.20]
 *****************************************************************************/
static QCLI_Command_Status_t ping(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  QCLI_Printf(qcli_net_handle, "Ping is removed, use ping3"); 
  return result;
}


/*****************************************************************************
 *      [0]         [1]  [2]   [3] [4]     [5] [6]  [7] [8] 
 * benchtoolconfig [0] for CATM1 [1] for CAT4
 *****************************************************************************/
static QCLI_Command_Status_t benchtoolconfig(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = BENCHTOOL_CONFIG_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
  cmd_parser_temp->Command_Index = 20;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_benchtoolconfig(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{

    int isCat4 = 0;

    if (Parameter_Count < 1 || Parameter_List == NULL)
    {
      return QCLI_STATUS_USAGE_E;
    }
    isCat4= Parameter_List[0].Integer_Value;

    if(isCat4 == 1)
    {
       enable_cat4 = true;
    }
    else if(isCat4 == 0)
    {
      enable_cat4 = false;
    }
    else if(isCat4 ==2)
    {
      enable_us_delay = true;
    }
    else if(isCat4 == 3)
    {
      enable_us_delay = false;
    }

    return QCLI_STATUS_SUCCESS_E;

}



/*****************************************************************************
 *          [0]
 * Autoipv4 wlan0
 *****************************************************************************/
static QCLI_Command_Status_t autoip(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = AUTOIP_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
  cmd_parser_temp->Command_Index = 5;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t 
proc_autoip(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    uint32_t addr = 0, mask = 0, gw = 0;
    char *interface_name;
    int i;
    QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

    if (Parameter_Count < 1 || Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    interface_name = (char*)Parameter_List[0].String_Value;

    if (qapi_Net_IPv4_Config(interface_name, QAPI_NET_IPV4CFG_AUTO_IP_E, &addr, &mask, &gw) != 0)
    {
        QCLI_Printf(qcli_net_handle, "Auto IPv4 failed\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

 end:
  if (Parameter_List!= NULL)
  {
        for (i = 0; i<Parameter_Count; i++)
        {
          if (Parameter_List[i].String_Value != NULL)
          {
            tx_byte_release(Parameter_List[i].String_Value);
            Parameter_List[i].String_Value = NULL;
          }
        }
    }
  return result;
}

/*****************************************************************************
 *****************************************************************************/
static int32_t display_interface_info(char *interface_name)
{
    ip6_addr v6Global, v6GlobalExtd, v6LinkLocal, v6DefGw;
    uint32_t LinkPrefix, GlobalPrefix, DefGwPrefix, GlobalPrefixExtd;
//    qapi_Net_DNS_Server_List_t svr_list;
    char msg_ipv4_target_error[] = "KF is not responding, operation failed";

    uint32_t addr=0, mask=0, gw=0;
    int i, err = -1;
    const uint8_t * physical_address = NULL;
    const uint8_t ** paddr;
    uint32_t physical_address_length = 0;

    QCLI_Printf(qcli_net_handle, "%s:\n", interface_name);

    /* MAC */
    tx_byte_allocate(byte_pool_qcli, (VOID **) &physical_address,
                     (sizeof(unsigned int)* QAPI_MAC_ADDR_LEN), TX_NO_WAIT);

    if(physical_address == NULL)
    {
       QCLI_Printf(qcli_net_handle, "Memory Allocation failure!! \n");
       return QCLI_STATUS_ERROR_E;
    }

    paddr = (const uint8_t **)&physical_address;
    if ((err = qapi_Net_Interface_Get_Physical_Address(
                    interface_name,
                    paddr,
                    &physical_address_length
                    )) == 0 && (physical_address != NULL))
    {
        QCLI_Printf(qcli_net_handle, " Phy Address: %02X", (*physical_address));
        for ( i = 1; i < physical_address_length; i++ )
        {
            QCLI_Printf(qcli_net_handle, ":%02X", (*( physical_address + i)));
        }
        QCLI_Printf(qcli_net_handle, "\n");
    }

    if(physical_address != NULL)
    {
      tx_byte_release((void *)physical_address);
    }
    /* IPv4 */
    if ((err = qapi_Net_IPv4_Config(interface_name, QAPI_NET_IPV4CFG_QUERY_E, &addr, &mask, &gw)) == 0)
    {
        char ip_str[20], mask_str[20], gw_str[20];

//        addr = htonl(addr);
//        mask = htonl(mask);
//        gw   = htonl(gw);
        QCLI_Printf(qcli_net_handle, " IPv4: %s  Subnet Mask: %s  Default Gateway: %s\n",
                    inet_ntop(AF_INET, &addr, ip_str, sizeof(ip_str)),
                    inet_ntop(AF_INET, &mask, mask_str, sizeof(mask_str)),
                    inet_ntop(AF_INET, &gw, gw_str, sizeof(gw_str)));
    }

    if (err == QAPI_NET_ERR_SOCKET_CMD_TIME_OUT)
    {
        QCLI_Printf(qcli_net_handle, "%s\n", msg_ipv4_target_error);
    }


    /* IPv6 */
    memset(&v6LinkLocal,0, sizeof(ip6_addr));
    memset(&v6Global,0, sizeof(ip6_addr));
    memset(&v6DefGw,0, sizeof(ip6_addr));
    memset(&v6GlobalExtd,0, sizeof(ip6_addr));
    LinkPrefix = GlobalPrefix = DefGwPrefix = GlobalPrefixExtd = 0;
    if (qapi_Net_IPv6_Get_Address(interface_name,
                (uint8_t *)&v6Global,
                (uint8_t *)&v6LinkLocal,
                (uint8_t *)&v6DefGw,
                (uint8_t *)&v6GlobalExtd,
                &LinkPrefix,
                &GlobalPrefix,
                &DefGwPrefix,
                &GlobalPrefixExtd) == 0)
    {
        char ip_str[48];

        if (v6LinkLocal.s_addr[0])
        {
            if (inet_ntop(AF_INET6, &v6LinkLocal, ip_str, sizeof(ip_str)) != NULL)
            {
                if (LinkPrefix)
                    QCLI_Printf(qcli_net_handle, " Link-local IPv6 Address ..... : %s/%d\n", ip_str, LinkPrefix);
                else
                    QCLI_Printf(qcli_net_handle, " Link-local IPv6 Address ..... : %s\n", ip_str);
            }

            if (inet_ntop(AF_INET6, &v6Global, ip_str, sizeof(ip_str)) != NULL)
            {
                if (GlobalPrefix)
                    QCLI_Printf(qcli_net_handle, " Global IPv6 Address ......... : %s/%d\n", ip_str, GlobalPrefix);
                else
                    QCLI_Printf(qcli_net_handle, " Global IPv6 Address ......... : %s\n", ip_str);
            }

            if (inet_ntop(AF_INET6, &v6DefGw, ip_str, sizeof(ip_str)) != NULL)
            {
                if (DefGwPrefix)
                    QCLI_Printf(qcli_net_handle, " Default Gateway  ............. : %s/%d\n", ip_str, DefGwPrefix);
                else
                    QCLI_Printf(qcli_net_handle, " Default Gateway  ............. : %s\n", ip_str);

            }

            if (inet_ntop(AF_INET6, &v6GlobalExtd, ip_str, sizeof(ip_str)) != NULL)
            {
                if (GlobalPrefixExtd)
                    QCLI_Printf(qcli_net_handle, " Global IPv6 Address 2 ......... : %s/%d\n", ip_str, GlobalPrefixExtd);
                else
                    QCLI_Printf(qcli_net_handle, " Global IPv6 Address 2 ......... : %s\n", ip_str);
            }
        }
    }

#if 0
    /* DNS */
    if (qapi_Net_DNSc_Get_Server_List(&svr_list) == 0)
    {
        int i;
        char ip_str[48];

        for (i = 0; i < QAPI_NET_DNS_SERVER_MAX; ++i)
        {
            if (svr_list.svr[i].type != AF_UNSPEC)
            {
                QCLI_Printf(qcli_net_handle, " DNS Server: %s\n",
                            inet_ntop(svr_list.svr[i].type, &svr_list.svr[i].a, ip_str, sizeof(ip_str)));
            }
        }
    }
#endif

    return 0;
}

/*****************************************************************************
 *          [0]   [1]           [2]         [3]
 * ifconfig
 * ifconfig lo0
 * ifconfig wlan0 192.168.1.100 255.255.255 192.168.1.1     //ifname must be wlan0 or wlan1
 *****************************************************************************/
static QCLI_Command_Status_t ifconfig(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = IFCONFIG_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
  cmd_parser_temp->Command_Index = 6;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t 
proc_ifconfig(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    int i, num, e;
    qapi_Net_Ifnameindex_t ifname_index[6];
    char * interface_name;
    uint32_t addr, mask, gw, *gw_addr = NULL;
    QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

    switch (Parameter_Count)
    {
        /* display all interfaces */
        case 0:
            memset(&ifname_index[0], 0, sizeof(ifname_index));
            num = qapi_Net_Get_All_Ifnames(ifname_index);
            for (i = 0; i < num; ++i)
            {
                display_interface_info(ifname_index[i].interface_Name);
            }

            /* If no interface, we want to display cmd syntax */
            if (num == 0)
            {
              result = QCLI_STATUS_USAGE_E;
              goto end;
            }
            break;

        /* display this interface */
        case 1:
            interface_name = (char*)Parameter_List[0].String_Value;
            if (qapi_Net_Interface_Exist(interface_name))
            {
                display_interface_info(interface_name);
            }
            else
            {
                QCLI_Printf(qcli_net_handle, "%s does not exist\n", interface_name);
                result = QCLI_STATUS_ERROR_E;
                goto end;
            }
            break;

        /* Set IPv4 address for WLAN interfaces */
        case 3:
        case 4:
            interface_name = (char*)Parameter_List[0].String_Value;

            /* interface's IP address */
            if (inet_pton(AF_INET, (char*)Parameter_List[1].String_Value, &addr) != 0)
            {
                QCLI_Printf(qcli_net_handle, "Invalid IP address\n");
                result = QCLI_STATUS_ERROR_E;
                goto end;
            }

            /* subnet mask */
            if (inet_pton(AF_INET, (char*)Parameter_List[2].String_Value, &mask) != 0)
            {
                QCLI_Printf(qcli_net_handle, "Invalid subnet mask\n");
                result = QCLI_STATUS_ERROR_E;
                goto end;
            }

            /* default gateway */
            if (Parameter_Count == 4)
            {
                if (inet_pton(AF_INET, (char*)Parameter_List[3].String_Value, &gw) != 0)
                {
                    QCLI_Printf(qcli_net_handle, "Invalid gateway address\n");
                    result = QCLI_STATUS_ERROR_E;
                    goto end;
                }

//                gw = ntohl(gw);
                gw_addr = &gw;
            }

//            addr = ntohl(addr);
//            mask = ntohl(mask);
            e = qapi_Net_IPv4_Config(interface_name, QAPI_NET_IPV4CFG_STATIC_IP_E, &addr, &mask, gw_addr);
            if (e != 0)
            {
                if (e == QAPI_NET_ERR_INVALID_IPADDR)
                {
                    QCLI_Printf(qcli_net_handle, "Invalid IP address\n");
                }
                else
                {
                    QCLI_Printf(qcli_net_handle, "Static IPv4 configure failed\n");
                }
                result = QCLI_STATUS_ERROR_E;
                goto end;
            }
            break;

        default:
            result = QCLI_STATUS_USAGE_E;
            goto end;
    }

 end:
  if (Parameter_List != NULL)
	{
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        tx_byte_release(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
	}
  return result;
}

/************************************************************************************************************
 *          [0]                    [1] [2]    [3] [4]     [5] [6]    [7] [8]   [9] [10]
 * Ping6 FE80::865D:D7FF:FE40:19C8 [-c  100] [-s  1500]  [-I  wlan0] [-w 1000] [-B FE80:::D7FF:19C8:8540:FEC8]
 ************************************************************************************************************/
static QCLI_Command_Status_t ping6(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  QCLI_Printf(qcli_net_handle, "ping6 is removed use ping6_3");
  return result;
}


/*****************************************************************************
 *       [0]     [1]                            [2]
 * Dnsc
 * Dnsc start
 * Dnsc stop
 * Dnsc disable
 * Dnsc resolve  sclcelinux1.qualcomm.com
 * Dnsc resolve  sclcelinux1.qualcomm.com       v4
 * Dnsc resolve  sclcelinux1.qualcomm.com       v6
 * Dnsc add      192.168.1.30
 * Dnsc add      192.168.1.300   1
 * Dnsc add      FE80::865D:D7FF:FE40:3498%wlan1
 * Dnsc del      1
 * Dnsc get_svr_id 10.4.27.7 rmnet_data0
 *****************************************************************************/
static QCLI_Command_Status_t dnsc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = DNSC_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
  cmd_parser_temp->Command_Index = 9;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t 
proc_dnsc(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    int32_t e = -1;
    uint32_t id;
	uint32_t settimeout;
    char *cmd, *svr, *interface;
    uint8_t index;
    int q;
    QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
    
    /* Dnsc */
    if (Parameter_Count == 0)
    {
        int32_t i;
        int32_t started;
        qapi_Net_DNS_Server_List_t svr_list;
        char ip_str[48];

        started = qapi_Net_DNSc_Is_Started();
        QCLI_Printf(qcli_net_handle, "DNS client is %s.\n", started ? "started" : "stopped");

        for( index = 0; index < QAPI_NET_DNS_SERVER_MAX; index++)
        {
           QCLI_Printf(qcli_net_handle, "rmnet_data%d:\n",index);
           e = qapi_Net_DNSc_Get_Server_List(&svr_list, index);
        if (e == 0)
        {
            for (i = 0; i < QAPI_NET_DNS_SERVER_MAX; ++i)
            {
                if (svr_list.svr[i].type == AF_UNSPEC)
                {
                    QCLI_Printf(qcli_net_handle, "%d: ****\n", i);
                }
                else
                {
                    QCLI_Printf(qcli_net_handle, "%d: %s\n", i,
                            inet_ntop(svr_list.svr[i].type, &svr_list.svr[i].a, ip_str, sizeof(ip_str)));
                }
            }
        }
        }
        /* If not started, we want to display cmd syntax */
        if (!started)
        {
          result = QCLI_STATUS_USAGE_E;
          goto end;
        }

      result = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

    cmd = (char*)Parameter_List[0].String_Value;

    /*      [0]   [1]           [2]
     * Dnsc add   192.168.1.30
     * Dnsc add   192.168.1.30   1
     */
    if (strncmp(cmd, "addsvr", 3) == 0)
    {
        if (Parameter_Count < 3)
        {
            result = QCLI_STATUS_USAGE_E;
            goto end;
        }

        svr = (char*)Parameter_List[1].String_Value;

        interface = (char*)Parameter_List[2].String_Value;

        e = validate_interface(interface);
        if (e == -1)
        {
          QCLI_Printf(qcli_net_handle, "%s\n", msg_invalid_iface);
          result = QCLI_STATUS_ERROR_E;
          goto end;
        }

        if (strlen(svr) > QAPI_NET_DNS_MAX_ADDRESS_LENGTH)
        {
            QCLI_Printf(qcli_net_handle, "address cannot be more then 48 bytes\n");
            result = QCLI_STATUS_ERROR_E;
            goto end;
        }

        if (Parameter_Count == 3)
        {
            id = QAPI_NET_DNS_ANY_SERVER_ID;
        }
        else if (Parameter_List[3].Integer_Is_Valid)
        {
            id = Parameter_List[3].Integer_Value;
        }
        else
        {
            QCLI_Printf(qcli_net_handle, "%s\n", msg_invalid_id);
            result = QCLI_STATUS_ERROR_E;
            goto end;
        }

        e = qapi_Net_DNSc_Add_Server_on_iface(svr, id, interface);
        if (e == QAPI_NET_ERR_INVALID_IPADDR)
        {
            QCLI_Printf(qcli_net_handle, "Invalid server address\n");
        }
        else if (e == QAPI_NET_ERR_CANNOT_GET_SCOPEID)
        {
            QCLI_Printf(qcli_net_handle, "Failed to get scope id\n");
        }
        else if (e == QAPI_NET_ERR_MAX_SERVER_REACHED)
        {
            QCLI_Printf(qcli_net_handle, "Max v4/v6 servers already present for this Interface\n");
            QCLI_Printf(qcli_net_handle, "Max 2 v4/v6 servers allowed per Interface\n");
        }
        else if(e == QAPI_ERR_EXISTS)
        {
            QCLI_Printf(qcli_net_handle, "Server is already added\n");
        }
    }
    /*
    * Dnsc get_svr_id 10.4.27.7 rmnet_data0
    */
    else if (strncmp(cmd, "get_svr_id", 10) == 0)
    {
        if (Parameter_Count != 3)
        {
            result = QCLI_STATUS_USAGE_E;
            goto end;
        }

        svr = (char*)Parameter_List[1].String_Value;

        interface = (char*)Parameter_List[2].String_Value;

        e = validate_interface(interface);
        if (e == QAPI_ERROR)
        {
          QCLI_Printf(qcli_net_handle, "%s\n", msg_invalid_iface);
          result = QCLI_STATUS_ERROR_E;
          goto end;
        }

        if (strlen(svr) > QAPI_NET_DNS_MAX_ADDRESS_LENGTH)
        {
            QCLI_Printf(qcli_net_handle, "address cannot be more then 48 bytes\n");
            result = QCLI_STATUS_ERROR_E;
            goto end;
        }
        id = QAPI_NET_DNS_ANY_SERVER_ID;
        e = qapi_Net_DNSc_Get_Server_Index(svr, &id, interface);
        if(e == QAPI_OK)
        {
            QCLI_Printf(qcli_net_handle, "\nServer is added at index : %d\n", id);
        }
        else if (e == QAPI_NET_ERR_INVALID_IPADDR)
        {
            QCLI_Printf(qcli_net_handle, "\nInvalid server address\n");
        }
        else if (e == QAPI_ERR_INVALID_PARAM)
        {
            QCLI_Printf(qcli_net_handle, "\nInvalid parameters passed\n");
        }
         else
        {
            QCLI_Printf(qcli_net_handle, "\nServer not present\n");
        }
    }

    /*
     * Dnsc del  1
     */
    else if (strncmp(cmd, "delsvr", 3) == 0)
    {
        if (Parameter_Count < 3)
        {
            result = QCLI_STATUS_USAGE_E;
            goto end;
        }

        interface = (char*)Parameter_List[1].String_Value;

        e = validate_interface(interface);
        if (e == -1)
        {
          QCLI_Printf(qcli_net_handle, "%s\n", msg_invalid_iface);
          result = QCLI_STATUS_ERROR_E;
          goto end;
        }

        if (Parameter_List[2].Integer_Is_Valid)
        {
            id = Parameter_List[2].Integer_Value;
        }
        else
        {
            QCLI_Printf(qcli_net_handle, "%s\n", msg_invalid_id);
            result = QCLI_STATUS_ERROR_E;
            goto end;
        }

        e = qapi_Net_DNSc_Del_Server_on_iface(id, interface);
    }

    /* Dnsc start */
    else if (strncmp(cmd, "start", 3) == 0)
    {
        e = qapi_Net_DNSc_Command(QAPI_NET_DNS_START_E);
		QCLI_Printf(qcli_net_handle, "setting timeout value = 2000ms\n");
		qapi_Net_DNSc_Set_Client_timeout(2000);
    }

    /* Dnsc stop */
    else if (strncmp(cmd, "stop", 3) == 0)
    {
        e = qapi_Net_DNSc_Command(QAPI_NET_DNS_STOP_E);
    }

    /* Dnsc settimeout */
	else if (strncmp(cmd, "settimeout", 7) == 0)
    {
        if(Parameter_Count == 2) 
        {
	       if (Parameter_List[1].Integer_Is_Valid)
           {
	          settimeout = Parameter_List[1].Integer_Value;
			  QCLI_Printf(qcli_net_handle, "new timeout value = %lu ms\n",settimeout );
		      e = qapi_Net_DNSc_Set_Client_timeout(settimeout);
           }
           else
           {
              QCLI_Printf(qcli_net_handle, "Invalid settimeout integer value\n");
              result = QCLI_STATUS_ERROR_E;
              goto end;
           }
        }
	    else
	    {
	       QCLI_Printf(qcli_net_handle, "setting default timeout = 2000ms\n");
		   result = QCLI_STATUS_SUCCESS_E;
           goto end;
	    }
	}

    /*      [0]     [1]          [2]                      [3]
     * Dnsc resolve <hostname>   <Interface Index>
     * Dnsc resolve <hostname>   <Interface Index>         v4
     * Dnsc resolve <hostname>   <Interface Index>         v6
     */
    else if (strncmp(cmd, "resolve", 3) == 0)
    {
        struct ip46addr ipaddr;
        char ip_str[48];

        if (Parameter_Count < 3)
        {
            result = QCLI_STATUS_USAGE_E;
            goto end;
        }

        if  (!qapi_Net_DNSc_Is_Started())
        {
            QCLI_Printf(qcli_net_handle, "DNS client is not started yet.\n");
            result = QCLI_STATUS_ERROR_E;
            goto end;
        }

        svr = (char*)Parameter_List[1].String_Value;

        interface = (char*)Parameter_List[2].String_Value;

        e = validate_interface(interface);
        if (e == -1)
        {
          QCLI_Printf(qcli_net_handle, "%s\n", msg_invalid_iface);
          result = QCLI_STATUS_ERROR_E;
          goto end;
        }

        memset(&ipaddr, 0, sizeof(ipaddr));
        if (Parameter_Count == 3 || strcmp((char*)Parameter_List[3].String_Value, "v4") == 0)
        {
            ipaddr.type = AF_INET;
        }
        else if (strcmp((char*)Parameter_List[3].String_Value, "v6") == 0)
        {
            ipaddr.type = AF_INET6;
        }
		
        e = qapi_Net_DNSc_Reshost_on_iface(svr, &ipaddr,interface);
        if (e)
        {
            QCLI_Printf(qcli_net_handle, "Unable to resolve %s\n", svr);
        }
        else
        {
            QCLI_Printf(qcli_net_handle, "\n%s --> %s\n", svr, inet_ntop(ipaddr.type, &ipaddr.a, ip_str, sizeof(ip_str)));
        }
    }
     /*      [0]     [1]          [2]                      [3]
     * Dnsc hostbyname <hostname>   <Interface Index>
     * Dnsc hostbyname <hostname>   <Interface Index>         v4
     * Dnsc hostbyname <hostname>   <Interface Index>         v6
     */
    else if (strncmp(cmd, "hostbyname", 3) == 0)
    {
        struct ip46addr ipaddr;
        struct qapi_hostent_s res;

        if (Parameter_Count < 3)
        {
            result = QCLI_STATUS_USAGE_E;
            goto end;
        }

        if  (!qapi_Net_DNSc_Is_Started())
        {
            QCLI_Printf(qcli_net_handle, "DNS client is not started yet.\n");
            result = QCLI_STATUS_ERROR_E;
            goto end;
        }

        svr = (char*)Parameter_List[1].String_Value;

        interface = (char*)Parameter_List[2].String_Value;

        e = validate_interface(interface);
        if (e == -1)
        {
          QCLI_Printf(qcli_net_handle, "%s\n", msg_invalid_iface);
          result = QCLI_STATUS_ERROR_E;
          goto end;
        }

        memset(&ipaddr, 0, sizeof(ipaddr));
        memset(&res, 0, sizeof(res));

        if (Parameter_Count == 3 || strcmp((char*)Parameter_List[3].String_Value, "v4") == 0)
        {
            ipaddr.type = AF_INET;
        }
        else if (strcmp((char*)Parameter_List[3].String_Value, "v6") == 0)
        {
            ipaddr.type = AF_INET6;
        }

        if (TX_SUCCESS != tx_byte_allocate(byte_pool_qcli, (VOID *)(&(res.h_name)), 31, TX_NO_WAIT)) 
        { 
          QCLI_Printf(qcli_net_handle,"tx_byte_allocate [h_name] failed!");
          result = QCLI_STATUS_ERROR_E;
          goto end;
        } 

        if (TX_SUCCESS != tx_byte_allocate(byte_pool_qcli, (VOID *)(&(res.h_addr_list)), (sizeof(char*)*5), TX_NO_WAIT)) 
        { 
            QCLI_Printf(qcli_net_handle,"tx_byte_allocate [h_addr_list] failed!"); 
            result = QCLI_STATUS_ERROR_E;
            goto error;
        } 

        for(index = 0; index < 5; index++) 
        {
          if(ipaddr.type == AF_INET)
          {
            if (TX_SUCCESS != tx_byte_allocate(byte_pool_qcli, (VOID *)(&(res.h_addr_list[index])), 16, TX_NO_WAIT)) 
            { 
              QCLI_Printf(qcli_net_handle,"tx_byte_allocate [h_addr_list[%d]] failed!", index); 
              result = QCLI_STATUS_ERROR_E;
              goto error;
            } 

            if((res.h_addr_list != NULL) && (res.h_addr_list[index] != NULL))
            {
              memset(res.h_addr_list[index], 0, 16);
            }
            else
            {
              result = QCLI_STATUS_ERROR_E;
              goto error;
            }
          }

          if(ipaddr.type == AF_INET6)
          {
            if (TX_SUCCESS != tx_byte_allocate(byte_pool_qcli, (VOID *)(&(res.h_addr_list[index])), 48, TX_NO_WAIT)) 
            { 
              QCLI_Printf(qcli_net_handle,"tx_byte_allocate [h_addr_list[%d]] failed!", index); 
              result = QCLI_STATUS_ERROR_E;
              goto error;
            } 

            if((res.h_addr_list != NULL) && (res.h_addr_list[index] != NULL))
            {
              memset(res.h_addr_list[index], 0, 48);
            }
            else
            {
              result = QCLI_STATUS_ERROR_E;
              goto error;
            }
          }
        } 

        if(ipaddr.type == AF_INET)
        {
          QCLI_Printf(qcli_net_handle, "qapi_Net_DNSc_Host_By_Name\n");
          e = qapi_Net_DNSc_Host_By_Name2(svr, AF_INET, &res);
          if (e)
          {
              QCLI_Printf(qcli_net_handle, "Unable to resolve %s\n", svr);
          }
          else
          {
              QCLI_Printf(qcli_net_handle, "h_len = %d \n ", res.h_length);
              QCLI_Printf(qcli_net_handle, "h_name = %s \n ", res.h_name);
              for(index = 0; index < 5; index++) 
              { 
                if(res.h_addr_list[index][0] != '\0') 
                  QCLI_Printf(qcli_net_handle, "h_addr_list[%d]:%s\n", index, res.h_addr_list[index]); 
              } 
          }
        }

        if((ipaddr.type == AF_INET) || (ipaddr.type == AF_INET6))
        {
          QCLI_Printf(qcli_net_handle, "qapi_Net_DNSc_Host_By_Name2\n");
          e = qapi_Net_DNSc_Host_By_Name2(svr,ipaddr.type, &res);
          if (e)
          {
              QCLI_Printf(qcli_net_handle, "Unable to resolve %s\n", svr);
          }
          else
          {
              QCLI_Printf(qcli_net_handle, "h_len = %d \n ", res.h_length);
              QCLI_Printf(qcli_net_handle, "h_name = %s \n ", res.h_name);
              for(index = 0; index < 5; index++) 
              { 
                if(res.h_addr_list[index][0] != '\0') 
                  QCLI_Printf(qcli_net_handle, "h_addr_list[%d]:%s\n", index, res.h_addr_list[index]); 
              } 
          }
        }

        error:
          if (res.h_name != NULL)
          {
             tx_byte_release(res.h_name);
          }

          if(res.h_addr_list != NULL)
          {
             for(index = 0; index < 5; index++) 
             {
               if(res.h_addr_list[index] != NULL)
               {
                  tx_byte_release(res.h_addr_list[index]);
               }
             }
            tx_byte_release(res.h_addr_list);
          }


    }
    else
    {
        QCLI_Printf(qcli_net_handle, "Invalid subcommand: %s\n", cmd);
        result = QCLI_STATUS_USAGE_E;
        goto end;
    }

    if (e)
    {
        QCLI_Printf(qcli_net_handle, "%s failed\n", cmd);
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }
    else
    {
        result = QCLI_STATUS_SUCCESS_E;
        goto end;
    }

end:
  if (Parameter_List!= NULL)
  {
    for (q = 0; q<Parameter_Count; q++)
    {
      if (Parameter_List[q].String_Value != NULL)
      {
        tx_byte_release(Parameter_List[q].String_Value);
        Parameter_List[q].String_Value = NULL;
      }
    }
  }
  return result;
}

int validate_interface(char *iface_name)
{
  int  e = 0;
  if(iface_name != NULL)
  {
     if (strncmp(iface_name, "rmnet_data0", strlen("rmnet_data0")) == 0)
       e = 0;
     else if (strncmp(iface_name, "rmnet_data1", strlen("rmnet_data1")) == 0)
       e = 0;
     else if (strncmp(iface_name, "rmnet_data2", strlen("rmnet_data2")) == 0)
       e = 0;
     else if (strncmp(iface_name, "rmnet_data3", strlen("rmnet_data3")) == 0)
       e = 0;
     else
     {
       e = -1;
     }
  }
  return e;
}

/*****************************************************************************
 *                     [0]             [1]         [2]        [3]               [4]
 * prefix <interfaceName> <v6addr> <prefixlen> <prefix_lifetime> <valid_lifetime>
 ****************************************************************************/
static QCLI_Command_Status_t set_ipv6_router_prefix(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = SET_IPV6_ROUTER_PREFIX_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
  cmd_parser_temp->Command_Index = 8;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t 
proc_set_ipv6_router_prefix(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    uint8_t v6addr[16];
    int retval;
    int prefixlen = 0;
    int prefix_lifetime = 0;
    int valid_lifetime = 0;
    char *interface_name;
    int i;
    QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

    if (Parameter_Count < 5)
    {
        result = QCLI_STATUS_USAGE_E;
        goto end;
    }

    retval = inet_pton(AF_INET6, (char*)Parameter_List[1].String_Value, v6addr);
    if (retval != 0)
    {
        QCLI_Printf(qcli_net_handle, "Invalid IPv6 prefix\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

    interface_name  = (char*)Parameter_List[0].String_Value;
    prefixlen       = Parameter_List[2].Integer_Value;
    prefix_lifetime = Parameter_List[3].Integer_Value;
    valid_lifetime  = Parameter_List[4].Integer_Value;
    retval = qapi_Net_IPv6_Config_Router_Prefix(interface_name, v6addr, prefixlen, prefix_lifetime, valid_lifetime);
    if (retval != 0)
    {
        QCLI_Printf(qcli_net_handle, "Setting router prefix failed\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

 end:
  if (Parameter_List!= NULL)
  {
        for (i = 0; i<Parameter_Count; i++)
        {
          if (Parameter_List[i].String_Value != NULL)
          {
            tx_byte_release(Parameter_List[i].String_Value);
            Parameter_List[i].String_Value = NULL;
          }
        }
    }
  return result;
}


/*****************************************************************************
 *                     [0]                           [1]              [2]             [3]             [4]....
 * cfg_netbuf   <custom> <pool_size> <buf_size> <num_buf> <buf_size> <num_buf>
 *                   <set> <profileID>
 *                   <get>
 ****************************************************************************/
static QCLI_Command_Status_t cfg_netbuf(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = CFG_NETBUF_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
  cmd_parser_temp->Command_Index = 10;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t 
proc_cfg_netbuf(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    qapi_Net_Profile_Custom_Pool_t *net_buf_pool = NULL;
    qapi_Net_Profile_Type_t profile = QAPI_NET_PROFILE_PREF_E;
    uint8_t pool_size = 0;
    QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;
    char *profileString[QAPI_NET_PROFILE_MAX] = { "PERFORMANCE",
                                                  "BEST EFFORT",
                                                  "MEMORY OPTIMIZIED",
                                                  "CUSTOM     " };
    uint32_t i = 0;
    char *cmd;
    int p;

    /* invalid options */
    if (Parameter_Count < 1 || Parameter_List == NULL)
    {
       status = QCLI_STATUS_USAGE_E;
        goto end;
    }

    cmd = (char*)Parameter_List[0].String_Value;

   if(strncmp(cmd, "custom", 3) == 0){
        /* validate the input */
        if( Parameter_Count < 2){
            status = QCLI_STATUS_USAGE_E;
             goto end;
        }

        /* read the no pool and validate size and count for each pool */
        pool_size = Parameter_List[1].Integer_Value;
        if( Parameter_Count < (2 + (pool_size * 2))){
            status = QCLI_STATUS_USAGE_E;
             goto end;
        }

        tx_byte_allocate(byte_pool_qcli, (VOID **) &net_buf_pool, sizeof(qapi_Net_Profile_Custom_Pool_t), TX_NO_WAIT);
        if( net_buf_pool == NULL){
            status = QCLI_STATUS_USAGE_E;
             goto end;
        }

        for(i= 2; i < Parameter_Count; i+=2){
           net_buf_pool->buf_Size= Parameter_List[i].Integer_Value;
           net_buf_pool->buf_Num= Parameter_List[i+1].Integer_Value;
           net_buf_pool++;
        }
        net_buf_pool -= (pool_size);
        if(0 != qapi_Net_Profile_Set_Custom(net_buf_pool, pool_size)){
            status = QCLI_STATUS_ERROR_E;
        }
        tx_byte_release(net_buf_pool);
    }
    else if(strncmp(cmd, "set", 2) == 0){
        /* validate the input */
        if( Parameter_Count < 2){
            status = QCLI_STATUS_USAGE_E;
             goto end;
        }

        profile = (qapi_Net_Profile_Type_t)Parameter_List[1].Integer_Value;
        if(0 != qapi_Net_Profile_Set_Active(profile))
        {
            status = QCLI_STATUS_ERROR_E;
            goto end;
        }

        QCLI_Printf(qcli_net_handle, "Active Profile : %s\n", profileString[profile - 1]);
    }
    /* wmiconfig --netbuf get_active_profile  */
    else if(strncmp(cmd, "get", 2) == 0){
        if(0 != qapi_Net_Profile_Get_Active(&net_buf_pool, &pool_size, &profile))
        {
            status = QCLI_STATUS_ERROR_E;
            goto end;
        }

        if(pool_size){
            QCLI_Printf(qcli_net_handle, " \n Profile : %s\n", profileString[profile - 1]);
            QCLI_Printf(qcli_net_handle, " BufSize     Count\n");
          do{
             QCLI_Printf(qcli_net_handle, "%8d  %8d\n", net_buf_pool->buf_Size, net_buf_pool->buf_Num);
             net_buf_pool++;
          }while(--pool_size);
       }
    }

 end:
  if (Parameter_List!= NULL)
  {
    for (p = 0; p<Parameter_Count; p++)
    {
      if (Parameter_List[p].String_Value != NULL)
      {
        tx_byte_release(Parameter_List[i].String_Value);
        Parameter_List[p].String_Value = NULL;
      }
    }
  }
  return status;
}


/*****************************************************************************
 *      [0]         [1]  [2]   [3] [4]     [5] [6]  [7] [8] 
 * Ping3 192.168.1.1 [-c  100] [-s  1500]  [-w 1000] [-B 192.168.1.20]
 *****************************************************************************/
static QCLI_Command_Status_t ping3(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = PING3_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
  cmd_parser_temp->Command_Index =22;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_ping3(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    char ip_str[16];
    uint32_t i, count = 1, size = 64, timeout = 1000, curr_count = 1;
    uint8_t ttl = 64;
    struct ip46addr addr;
    struct ip46addr addr1;
    char * host, *host1;
    int e;
    int p;
    QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
    qapi_Net_Ping_V4_R2_t *ping_buf = NULL;
    long tot_time, avg_time, max, min;
    qapi_Ping_Info_Resp_R2_t *ping_resp;
    int flag = false;
    server_quit = 0;
    tot_time = 0;
    avg_time = 0;
    max = 0;
    min = 2147483647;

    memset(&addr, 0x00, sizeof(struct ip46addr));
    memset(&addr1, 0x00, sizeof(struct ip46addr));
    if (Parameter_Count < 1 || Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    for (i = 1; i < Parameter_Count ; i++)
    {
        if (strcmp((char*)Parameter_List[i].String_Value, "-c") == 0)
        {
            ++i;
            if (Parameter_List[i].Integer_Is_Valid)
            {
                count = Parameter_List[i].Integer_Value;
            }
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-s") == 0)
        {
            ++i;
            if (Parameter_List[i].Integer_Is_Valid)
            {
                size = Parameter_List[i].Integer_Value;
                if (size > CFG_PACKET_SIZE_MAX_TX)
                {
                    QCLI_Printf(qcli_net_handle, "Size should be <= %d\n", CFG_PACKET_SIZE_MAX_TX);
                    result = QCLI_STATUS_ERROR_E;
                    goto end;
                }
            }
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-w") == 0)
        {
            ++i;
            flag = true;
            if (Parameter_List[i].Integer_Is_Valid)
            {
                timeout = Parameter_List[i].Integer_Value;
            }
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-B") == 0)
        {
            ++i;
            flag = true;
            host1 = (char *)Parameter_List[i].String_Value;
            if (inet_pton(AF_INET, host1, &addr1.a) != 0)
            {
              result = QCLI_STATUS_ERROR_E;
              goto end;
            }
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-t") == 0)
        {
             ++i;
            flag = true;
            if (Parameter_List[i].Integer_Is_Valid)
            {
              ttl = Parameter_List[i].Integer_Value;
            }
        }
    } /* for loop */

    host = (char *)Parameter_List[0].String_Value;
    if (inet_pton(AF_INET, host, &addr.a) != 0)   /* not in xxx.xxx.xxx.xxx format */
    {
        addr.type = AF_INET;
        if(qapi_Net_DNSc_Reshost(host, &addr) != 0)
        {
            QCLI_Printf(qcli_net_handle, "Cannot resolve %s\n", host);
            result = QCLI_STATUS_ERROR_E;
            goto end;
        }
        else
        {
            QCLI_Printf(qcli_net_handle, "Ping %s (%s):\n",
                    host,
                    inet_ntop(AF_INET, &addr.a, ip_str, sizeof(ip_str)));
        }
    }

    for (i = 0; i < count; i++)
    {
        uint32_t t1, t2, ms;

        t1 = app_get_time(NULL);

        if(test_for_quit_s())
        {
           break;
        }

      if(flag == false)
      {
         timeout = PING_DEFAULT_TIMEOUT ;
 		 ttl = PING_DEFAULT_V4_TTL_VAL ;
 		 addr1.a.addr4 = 0x00;
       }

        tx_byte_allocate(byte_pool_qcli, (VOID **)&ping_buf, sizeof(qapi_Net_Ping_V4_R2_t), TX_NO_WAIT);
        if(ping_buf == NULL)
        {
          QCLI_Printf(qcli_net_handle, "Failed to allocate memory\n");
          result = QCLI_STATUS_ERROR_E;
          goto end;
        }
        memset(ping_buf, 0x00, sizeof(qapi_Net_Ping_V4_R2_t));

        tx_byte_allocate(byte_pool_qcli, (VOID **)&ping_resp, sizeof(qapi_Ping_Info_Resp_R2_t), TX_NO_WAIT);
        if(ping_resp == NULL)
        {
          QCLI_Printf(qcli_net_handle, "Failed to allocate memory\n");
          result = QCLI_STATUS_ERROR_E;
          goto end;
        }
        memset(ping_resp, 0x00, sizeof(qapi_Ping_Info_Resp_R2_t));
        ping_buf->ipv4_addr = addr.a.addr4;
        ping_buf->bitmask |= QAPI_NET_PING_V4_DST_ADDR_MASK;
        ping_buf->ipv4_src = addr1.a.addr4;
        ping_buf->bitmask |= QAPI_NET_PING_V4_SRC_ADDR_MASK;
        ping_buf->size     = size;
        ping_buf->bitmask |= QAPI_NET_PING_V4_PKT_SIZE_MASK;
        ping_buf->timeout  = timeout;
        ping_buf->bitmask |= QAPI_NET_PING_V4_TIMEOUT_MASK;
        ping_buf->ttl      = ttl;
        ping_buf->bitmask |= QAPI_NET_PING_V4_TTL_MASK;

        e = qapi_Net_Ping_3(ping_buf, ping_resp);

        if(e == 0)
        {
            t2 = app_get_time(NULL);
            ms = t2 - t1;
            if(ms >= max)
            {
              max = ms;
            }
            if(ms <= min)
            {
              min = ms;
            }
            tot_time+=ms;
            curr_count++;
            QCLI_Printf(qcli_net_handle, "%d bytes from %s: seq=%d time=%u ms ttl=%d\n",
                    size, inet_ntop(AF_INET, &addr.a, ip_str, sizeof(ip_str)), i+1, ms, ping_resp->ttl);
        }
        else
        {
            if(ping_resp->ptype > 0)
            {
               QCLI_Printf(qcli_net_handle, "ICMP Error is %s\n",
                           ping_resp->perror);
            }
            else if(ping_resp->ptype == -2)
            {
                QCLI_Printf(qcli_net_handle, "Bind failed for provided src. \n");
            }
            else if(ping_resp->ptype == -1)
            {
                 QCLI_Printf(qcli_net_handle, "Request timed out\n");
            }
        } 

       if(ping_buf)
       {
         tx_byte_release(ping_buf);
         ping_buf = NULL;
       }

        if(ping_resp)
        {
          tx_byte_release(ping_resp);
          ping_resp = NULL;
        }

       if (i < count - 1)
       {
              qapi_Timer_Sleep(100,QAPI_TIMER_UNIT_MSEC,true);
       }
    }
    avg_time = (tot_time/(curr_count-1));
    QCLI_Printf(qcli_net_handle, "Min RTT %ld ms Avg RTT %ld ms Max RTT %ld ms\n",
                    min,avg_time,max);

end:
  if (Parameter_List!= NULL)
  {
    for (p = 0; p<Parameter_Count; p++)
    {
      if (Parameter_List[p].String_Value != NULL)
      {
        tx_byte_release(Parameter_List[i].String_Value);
        Parameter_List[p].String_Value = NULL;
      }
    }
  }
  return result;
}


/************************************************************************************************************
 *          [0]                    [1] [2]    [3] [4]     [5] [6]    [7] [8]   [9] [10]
 * Ping6_3 FE80::865D:D7FF:FE40:19C8 [-c  100] [-s  1500]  [-I  wlan0] [-w 1000] [-B FE80:::D7FF:19C8:8540:FEC8]
 ************************************************************************************************************/
static QCLI_Command_Status_t ping6_3(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = PING6_3_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
  cmd_parser_temp->Command_Index = 23;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t 
proc_ping6_3(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    uint8_t *ip6addr = NULL;
    uint8_t *ip6src = NULL;
    struct ip46addr addr, addr1;
    char ip_str[48];
    uint32_t i, count = 1, size = 64,timeout = 1000, curr_count = 1;
    uint8_t ttl = 255;
    char * ifname = NULL;
    char * host, *host1;
    uint32_t is_ifname_specification_needed = 0;
    int32_t e;
    int p;
    QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
    qapi_Net_Ping_V6_R2_t *ping6_buf = NULL;
    qapi_Ping_Info_Resp_R2_t *ping_resp;
    long tot_time, avg_time, max, min;
    int flag = false;
    server_quit = 0;
    tot_time = 0;
    avg_time = 0;
    max = 0;
    min = 2147483647;

    if (Parameter_Count < 1 || Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    memset(&addr1, 0x00, sizeof(struct ip46addr));

    for (i = 1; i < Parameter_Count ; i++)
    {
        if (strcmp((char*)Parameter_List[i].String_Value, "-c") == 0)
        {
            ++i;
            if (Parameter_List[i].Integer_Is_Valid)
            {
                count = Parameter_List[i].Integer_Value;
            }
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-s") == 0)
        {
            ++i;
            if (Parameter_List[i].Integer_Is_Valid)
            {
                size = Parameter_List[i].Integer_Value;
                if (size > CFG_PACKET_SIZE_MAX_TX)
                {
                    QCLI_Printf(qcli_net_handle, "Size should be <= %d\n", CFG_PACKET_SIZE_MAX_TX);
                    result = QCLI_STATUS_ERROR_E;
                    goto end;
                }
            }
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-I") == 0)
        {
            ++i;
            ifname = (char*)Parameter_List[i].String_Value;
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-w") == 0)
        {
            ++i;
            flag = true;
            if (Parameter_List[i].Integer_Is_Valid)
            {
                timeout = Parameter_List[i].Integer_Value;
            }
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-B") == 0)
        {
            ++i;
            flag = true;
            host1 = (char *)Parameter_List[i].String_Value;
            if (inet_pton(AF_INET6, host1, &addr1.a) != 0)    /* not IPv6 address */
            {
              result = QCLI_STATUS_ERROR_E;
              goto end;
            }
        }
        else if (strcmp((char*)Parameter_List[i].String_Value, "-t") == 0)
        {
           ++i;
           flag = true;
           if (Parameter_List[i].Integer_Is_Valid)
           {
            ttl = Parameter_List[i].Integer_Value;
           }
        }
    } /* for loop */

    host = (char *)Parameter_List[0].String_Value;
    if (inet_pton(AF_INET6, host, &addr.a) != 0)    /* not IPv6 address */
    {
        addr.type = AF_INET6;
        if (qapi_Net_DNSc_Reshost(host, &addr) != 0)
        {
            QCLI_Printf(qcli_net_handle, "Invalid IP address\n");
            result = QCLI_STATUS_ERROR_E;
            goto end;
        }
        else
        {
            QCLI_Printf(qcli_net_handle, "Ping %s (%s):\n",
                    host,
                    inet_ntop(AF_INET6, &addr.a, ip_str, sizeof(ip_str)));
        }
    }

    ip6addr = (uint8_t *)&addr.a;
    if (QAPI_IS_IPV6_MULTICAST(ip6addr) ||
        QAPI_IS_IPV6_LINK_LOCAL(ip6addr))
    {
        is_ifname_specification_needed = 1;
    }

    ip6src = (uint8_t *)&addr1.a;

    if (is_ifname_specification_needed && ifname == NULL)
    {
        QCLI_Printf(qcli_net_handle, "Error: The specified IPv6 address is either multicast or link-local, please specify source interface name using the -I option\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

    for (i = 0; i < count; i++)
    {
        uint32_t t1, t2, ms;

        t1 = app_get_time(NULL);

        if(test_for_quit_s())
        {
           break;
        }

        if(flag == false)
        {
          timeout = PING_DEFAULT_TIMEOUT ;
 		  ttl = PING_DEFAULT_V4_TTL_VAL ;
 		  ip6src  = 0x00;
        }

          tx_byte_allocate(byte_pool_qcli, (VOID **)&ping6_buf, sizeof(qapi_Net_Ping_V6_R2_t), TX_NO_WAIT);
          if(ping6_buf == NULL)
          {
            QCLI_Printf(qcli_net_handle, "Failed to allocate memory\n");
            result = QCLI_STATUS_ERROR_E;
            goto end;
          }
          memset(ping6_buf, 0x00, sizeof(qapi_Net_Ping_V6_R2_t));

          tx_byte_allocate(byte_pool_qcli, (VOID **)&ping_resp, sizeof(qapi_Ping_Info_Resp_R2_t), TX_NO_WAIT);
          if(ping_resp == NULL)
          {
            QCLI_Printf(qcli_net_handle, "Failed to allocate memory\n");
            result = QCLI_STATUS_ERROR_E;
            goto end;
          }
          memset(ping_resp, 0x00, sizeof(qapi_Ping_Info_Resp_R2_t));

          memcpy(ping6_buf->ipv6_addr,ip6addr, sizeof(struct in6_addr));
          ping6_buf->bitmask |= QAPI_NET_PING_V6_DST_ADDR_MASK;
		  if(ip6src)
           memcpy(ping6_buf->ipv6_src,ip6src, sizeof(struct in6_addr));
          ping6_buf->bitmask |= QAPI_NET_PING_V6_SRC_ADDR_MASK;
          ping6_buf->size = size;
          ping6_buf->bitmask |= QAPI_NET_PING_V6_PKT_SIZE_MASK;
          ping6_buf->timeout = timeout;
          ping6_buf->bitmask |= QAPI_NET_PING_V6_TIMEOUT_MASK;
          ping6_buf->ifname = ifname;
          ping6_buf->bitmask |= QAPI_NET_PING_V6_IF_NAME_MASK;
          ping6_buf->ttl = ttl;
          ping6_buf->bitmask |= QAPI_NET_PING_V6_TTL_MASK;

          e = qapi_Net_Ping6_3(ping6_buf, ping_resp);
          if(e == 0)
          {
             t2 = app_get_time(NULL);
              ms = t2 - t1;
              if(ms >= max)
              {
                max = ms;
              }
              if(ms <= min)
              {
                min = ms;
              }
              tot_time+=ms;
              curr_count++;
              QCLI_Printf(qcli_net_handle, "%d bytes from %s: seq=%d time=%u ms ttl=%d \n",
                      size, inet_ntop(AF_INET6, &addr.a, ip_str, sizeof(ip_str)), i+1, ms, ping_resp->ttl);
          }

          else
          {
            if(ping_resp->ptype > 0)
            {
               QCLI_Printf(qcli_net_handle, "ICMP Error is %s\n",
                           ping_resp->perror);
            }
            else if(ping_resp->ptype == -2)
            {
                QCLI_Printf(qcli_net_handle, "Bind failed for provided src. \n");
            }
            else if(ping_resp->ptype == -1)
            {
                 QCLI_Printf(qcli_net_handle, "Request timed out\n");
            }
          } 
 
          if(ping6_buf)
          {
            tx_byte_release(ping6_buf);
            ping6_buf = NULL;
          }
 
          if(ping_resp)
          {
            tx_byte_release(ping_resp);
            ping_resp = NULL;
          }

        #define PING6_DELAY_DURATION_IN_MS 1000

        if ( i < count - 1 ) {
             app_msec_delay(PING6_DELAY_DURATION_IN_MS);
        }
    }

    avg_time = (tot_time/(curr_count-1));
    QCLI_Printf(qcli_net_handle, "Min RTT %ld ms Avg RTT %ld ms Max RTT %ld ms\n",
                    min,avg_time,max);

 end:
  if (Parameter_List!= NULL)
  {
    for (p = 0; p<Parameter_Count; p++)
    {
      if (Parameter_List[p].String_Value != NULL)
      {
        tx_byte_release(Parameter_List[i].String_Value);
        Parameter_List[p].String_Value = NULL;
      }
    }
  }
  return result;
}

/**********************************************************************************************************************************************
 * set_sock_opt  [-e<enable/disable SACK>][-i <irt>] [-m <max_rxt>] [-t <overall_timer>] [-s <mss>] [-k<keepalive>] 
                 [-a<keep_idle>] [-t<keep_intvl>] [-c<keep_cnt>] [-r <rcv_buf>] [-b <snd_buf>] [-d <benchtx_sleep>]
                 [-f<reassm_timer>][-g<msl>][-p<max_persist>][-h<min_rto>][-j<max_rto>][-w<win_scale>][-n<enable_timestamp>][-q<max_sack_block>][-u<ip_ire>][-v<ip_maxfrags>]
 **********************************************************************************************************************************************/
static QCLI_Command_Status_t set_sock_opt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = SOCK_OPT_SET_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
  cmd_parser_temp->Command_Index =24;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t 
proc_set_sock_opt(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
   int i= 0, p=0;
   QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

   if (Parameter_Count < 1 || Parameter_List == NULL)
   {
     result = QCLI_STATUS_USAGE_E;
     goto end;
   }

   for (i = 0; i < Parameter_Count ; i++)
   {
      if (strcmp((char*)Parameter_List[i].String_Value, "-e") == 0)
      {
         ++i;
         if (Parameter_List[i].Integer_Is_Valid && (Parameter_List[i].Integer_Value == 1) || Parameter_List[i].Integer_Value == 0)
         {
            option[app_id].enable_sackopt = Parameter_List[i].Integer_Value;
         }
         else 
         {
            result = QCLI_STATUS_ERROR_E;
            goto end;
         }
      }

      else if (strcmp((char*)Parameter_List[i].String_Value, "-i") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].irt = Parameter_List[i].Integer_Value;
        }
      }

     else if (strcmp((char*)Parameter_List[i].String_Value, "-m") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].max_rxt = Parameter_List[i].Integer_Value;
        }
      }

      else if (strcmp((char*)Parameter_List[i].String_Value, "-o") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].overall_timer = Parameter_List[i].Integer_Value;
        }
      }

      else if(strcmp((char*)Parameter_List[i].String_Value, "-s") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].mss = Parameter_List[i].Integer_Value;
        }
      }

      else if(strcmp((char*)Parameter_List[i].String_Value, "-k") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].keepalive = Parameter_List[i].Integer_Value;
        }
      }

      else if(strcmp((char*)Parameter_List[i].String_Value, "-a") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].keep_idle = Parameter_List[i].Integer_Value;
        }
      }

      else if(strcmp((char*)Parameter_List[i].String_Value, "-t") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].keep_intvl = Parameter_List[i].Integer_Value;
        }
      }

      else if(strcmp((char*)Parameter_List[i].String_Value, "-c") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].keep_cnt = Parameter_List[i].Integer_Value;
        }
      }

      else if(strcmp((char*)Parameter_List[i].String_Value, "-r") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].rcv_buf = Parameter_List[i].Integer_Value;
        }
      }

      else if(strcmp((char*)Parameter_List[i].String_Value, "-b") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].snd_buf = Parameter_List[i].Integer_Value;
        }
      }

      else if(strcmp((char*)Parameter_List[i].String_Value, "-d") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].benchtx_sleep = Parameter_List[i].Integer_Value;
        }
      }

      else if(strcmp((char*)Parameter_List[i].String_Value, "-l") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           if (Parameter_List[i].Integer_Value == 0)
           {
           		option[app_id].linger_time_val0 = 1;
           }
           option[app_id].linger_time = Parameter_List[i].Integer_Value;
        }
      }

	  else if(strcmp((char*)Parameter_List[i].String_Value, "-f") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].reassm_timer = Parameter_List[i].Integer_Value;
        }
      }
	  else if(strcmp((char*)Parameter_List[i].String_Value, "-g") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].t_msl = Parameter_List[i].Integer_Value;
        }
      }
	  else if(strcmp((char*)Parameter_List[i].String_Value, "-p") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].persfails = Parameter_List[i].Integer_Value;
        }
      }
	  else if(strcmp((char*)Parameter_List[i].String_Value, "-h") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].rto_min = Parameter_List[i].Integer_Value;
        }
      }
	  else if(strcmp((char*)Parameter_List[i].String_Value, "-j") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].rto_max = Parameter_List[i].Integer_Value;
        }
      }
	  else if(strcmp((char*)Parameter_List[i].String_Value, "-w") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].win_scale = Parameter_List[i].Integer_Value;
        }
      }
	  else if (strcmp((char*)Parameter_List[i].String_Value, "-n") == 0)
      {
         ++i;
         if (Parameter_List[i].Integer_Is_Valid && (Parameter_List[i].Integer_Value == 1) || Parameter_List[i].Integer_Value == 0)
         {
            option[app_id].enable_timestamp = Parameter_List[i].Integer_Value;
         }
         else 
         {
            result = QCLI_STATUS_ERROR_E;
            goto end;
         }
      }
	  else if(strcmp((char*)Parameter_List[i].String_Value, "-q") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].max_sackblock = Parameter_List[i].Integer_Value;
        }
      }
	  else if(strcmp((char*)Parameter_List[i].String_Value, "-u") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].ip_ire = Parameter_List[i].Integer_Value;
        }
      }
	  else if(strcmp((char*)Parameter_List[i].String_Value, "-v") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
           option[app_id].ip_maxfrags = Parameter_List[i].Integer_Value;
        }
      }
    }

   QCLI_Printf(qcli_net_handle, "Going to set: \n  IRT = %d\n  MR = %d\n  OT = %d\n  MSS = %d\n ",option[app_id].irt,
         option[app_id].max_rxt, option[app_id].overall_timer, option[app_id].mss);
   QCLI_Printf(qcli_net_handle, " Reassm timer = %d\n MSL = %d\n MAX_PERSIST_FAILS = %d\n MIN_RTO = %d \n MAX_RTO = %d\n WIN_SCALE = %d\n",
   	option[app_id].reassm_timer, option[app_id].t_msl, option[app_id].persfails, option[app_id].rto_min,option[app_id].rto_max, option[app_id].win_scale);
   QCLI_Printf(qcli_net_handle, "TIMESTAMP = %d\n  MAX_SACKBLOCK = %d\n  IP_IRE = %d\n  IP_MAXFRAGS = %d \n",option[app_id].enable_timestamp,
         option[app_id].max_sackblock, option[app_id].ip_ire, option[app_id].ip_maxfrags);
   QCLI_Printf(qcli_net_handle, "SND_BUFF = %d\n  RCV_BUFF = %d \n",option[app_id].snd_buf,
         option[app_id].rcv_buf);

end:
  if (Parameter_List!= NULL)
  {
    for (p = 0; p<Parameter_Count; p++)
    {
      if (Parameter_List[p].String_Value != NULL)
      {
        tx_byte_release(Parameter_List[p].String_Value);
        Parameter_List[p].String_Value = NULL;
      }
    }
  }
  return result;
}

static QCLI_Command_Status_t get_nwstats(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
     QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
	 char* prot, *ifind;
	 int op = 0, i, error = -1;
	 prot = NULL;
	 ifind = NULL;

	 if (Parameter_Count < 3 || Parameter_List == NULL)
     {
         result = QCLI_STATUS_USAGE_E;
		 return result;
     }

	 for (i = 0; i < Parameter_Count ; i++)
     {

      if (strcmp((char*)Parameter_List[i].String_Value, "-p") == 0)
      {
        ++i;
		prot = (char *)Parameter_List[i].String_Value;
      }

     else if (strcmp((char*)Parameter_List[i].String_Value, "-i") == 0)
      {
        ++i;
        ifind = (char *)Parameter_List[i].String_Value;
      }

      else if (strcmp((char*)Parameter_List[i].String_Value, "-o") == 0)
      {
        ++i;
        if (Parameter_List[i].Integer_Is_Valid)
        {
          op = Parameter_List[i].Integer_Value;
        }
      }

     
    }

   if(prot && ifind && op)
    {
        nwstats_t* buf;
		qapi_nwstats_prot_t protocol;
		qapi_nwstats_op_t operation;

		buf = NULL;
		operation = QAPI_NWSTATS_GET;
		protocol = QAPI_NWSTATS_IPv4;

		QCLI_Printf(qcli_net_handle, "Nw statistics \n  Protocol = %s\n  Interface Name = %s\n  operation = %d\n  ",prot,
         ifind, op);
		
		if (TX_SUCCESS != tx_byte_allocate(byte_pool_qcli, (VOID **)&buf, sizeof(nwstats_t), TX_NO_WAIT))
        {
            QCLI_Printf(qcli_net_handle,"tx_byte_allocate [h_addr_list] failed!");
            result = QCLI_STATUS_ERROR_E;
        }

		if(op == 1)
				operation = QAPI_NWSTATS_GET;
		if(op == 2)
				operation = QAPI_NWSTATS_CLR;

        if(strncmp(prot,"IP4",3)==0)
        {
            if (TX_SUCCESS != tx_byte_allocate(byte_pool_qcli, (VOID **)&buf->u_prot_stats.ip, sizeof(qapi_ipv4_stats_t), TX_NO_WAIT))
        	{
            	QCLI_Printf(qcli_net_handle,"tx_byte_allocate [h_addr_list] failed!");
            	result = QCLI_STATUS_ERROR_E;
        	}

			protocol = QAPI_NWSTATS_IPv4;
			if(buf)
			error = qapi_getnwstats(ifind, protocol, operation, buf);

			if(error == 0)
			{
			    qapi_ipv4_stats_t* temp;
				if(buf){
				 temp = buf->u_prot_stats.ip;
			
			    QCLI_Printf(qcli_net_handle, "IPv4 Staitistics \n");
				QCLI_Printf(qcli_net_handle, "IP Forwarding: %lu \n IP DefaultTTL: %lu \n", temp->ipForwarding, temp->ipDefaultTTL);
				QCLI_Printf(qcli_net_handle, "IP InReceives: %lu \n IP InHdrErrors: %lu \n", temp->ipInReceives, temp->ipInHdrErrors);
				QCLI_Printf(qcli_net_handle, "IP InAddrErrors: %lu \n IP ForwDatagrams: %lu \n", temp->ipInAddrErrors, temp->ipForwDatagrams);
				QCLI_Printf(qcli_net_handle, "IP UnknownProtos: %lu \n IP InDiscards: %lu \n", temp->ipUnknownProtos, temp->ipInDiscards);
				QCLI_Printf(qcli_net_handle, "IP InDelivers: %lu \n IP OutRequests: %lu \n", temp->ipInDelivers, temp->ipOutRequests);
				QCLI_Printf(qcli_net_handle, "IP OutDiscards: %lu \n IP OutNoRoutes: %lu \n", temp->ipOutDiscards, temp->ipOutNoRoutes);
				QCLI_Printf(qcli_net_handle, "IP ReasmTimeout: %lu \n IP ReasmReqds: %lu \n", temp->ipReasmTimeout, temp->ipReasmReqds);
				QCLI_Printf(qcli_net_handle, "IP ReasmOKs: %lu \n IP ReasmFails: %lu \n", temp->ipReasmOKs, temp->ipReasmFails);
				QCLI_Printf(qcli_net_handle, "IP FragOKs: %lu \n IP FragFails: %lu \n", temp->ipFragOKs, temp->ipFragFails);
				QCLI_Printf(qcli_net_handle, "IP FragCreates: %lu \n IP RoutingDiscards: %lu \n", temp->ipFragCreates, temp->ipRoutingDiscards);
				QCLI_Printf(qcli_net_handle, "IP FilterDrop: %lu \n", temp->ipFilterDrop);
				}
			}
			else
            {
                QCLI_Printf(qcli_net_handle,"Error while fetching ipv4 network statistics\n");
			}
        }
		else if(strncmp(prot,"IP6",3)==0)
        {
            if (TX_SUCCESS != tx_byte_allocate(byte_pool_qcli, (VOID **)&buf->u_prot_stats.ip6, sizeof(qapi_ipv6_stats_t), TX_NO_WAIT))
        	{
            	QCLI_Printf(qcli_net_handle,"tx_byte_allocate [h_addr_list] failed!");
            	result = QCLI_STATUS_ERROR_E;
        	}

			protocol = QAPI_NWSTATS_IPv6;
			if(buf)
			error = qapi_getnwstats(ifind, protocol, operation, buf);

			if(error == 0)
			{
			    qapi_ipv6_stats_t* temp;
				if(buf){
				 temp = buf->u_prot_stats.ip6;
			
			    QCLI_Printf(qcli_net_handle, "IPv6 Staitistics \n");
				QCLI_Printf(qcli_net_handle, "ipv6IfStatsInReceives: %lu \n ipv6IfStatsInHdrErrors: %lu \n", temp->ipv6IfStatsInReceives, temp->ipv6IfStatsInHdrErrors);
				QCLI_Printf(qcli_net_handle, "ipv6IfStatsInTooBigErrors: %lu \n ipv6IfStatsInNoRoutes: %lu \n", temp->ipv6IfStatsInTooBigErrors, temp->ipv6IfStatsInNoRoutes);
				QCLI_Printf(qcli_net_handle, "ipv6IfStatsInAddrErrors: %lu \n ipv6IfStatsInUnknownProtos: %lu \n", temp->ipv6IfStatsInAddrErrors, temp->ipv6IfStatsInUnknownProtos);
				QCLI_Printf(qcli_net_handle, "ipv6IfStatsInTruncatedPkts: %lu \n ipv6IfStatsInDiscards: %lu \n", temp->ipv6IfStatsInTruncatedPkts, temp->ipv6IfStatsInDiscards);
				QCLI_Printf(qcli_net_handle, "ipv6IfStatsInDelivers: %lu \n ipv6IfStatsOutNoRoutes: %lu \n", temp->ipv6IfStatsInDelivers, temp->ipv6IfStatsOutNoRoutes);
				QCLI_Printf(qcli_net_handle, "ipv6IfStatsOutForwDatagrams: %lu \n ipv6IfStatsOutRequests: %lu \n", temp->ipv6IfStatsOutForwDatagrams, temp->ipv6IfStatsOutRequests);
				QCLI_Printf(qcli_net_handle, "ipv6IfStatsOutDiscards: %lu \n ipv6IfStatsOutFragOKs: %lu \n", temp->ipv6IfStatsOutDiscards, temp->ipv6IfStatsOutFragOKs);
				QCLI_Printf(qcli_net_handle, "ipv6IfStatsOutFragFails: %lu \n ipv6IfStatsOutFragCreates: %lu \n", temp->ipv6IfStatsOutFragFails, temp->ipv6IfStatsOutFragCreates);
				QCLI_Printf(qcli_net_handle, "ipv6IfStatsReasmReqds: %lu \n ipv6IfStatsReasmOKs: %lu \n", temp->ipv6IfStatsReasmReqds, temp->ipv6IfStatsReasmOKs);
				QCLI_Printf(qcli_net_handle, "ipv6IfStatsReasmFails: %lu \n ipv6IfStatsInMcastPkts: %lu \nipv6IfStatsOutMcastPkts: %d \n", temp->ipv6IfStatsReasmFails, temp->ipv6IfStatsInMcastPkts, temp->ipv6IfStatsOutMcastPkts);
				QCLI_Printf(qcli_net_handle, "IPv6FilterDrop: %lu \n", temp->ipv6FilterDrop);
				}
			}
			else
            {
                QCLI_Printf(qcli_net_handle,"Error while fetching ipv6 network statistics\n");
			}
        }
		else if(strncmp(prot,"TCP",3)==0)
        {
            if (TX_SUCCESS != tx_byte_allocate(byte_pool_qcli, (VOID **)&buf->u_prot_stats.tcp, sizeof(qapi_tcp_stats_t), TX_NO_WAIT))
        	{
            	QCLI_Printf(qcli_net_handle,"tx_byte_allocate [h_addr_list] failed!");
            	result = QCLI_STATUS_ERROR_E;
        	}

			protocol = QAPI_NWSTATS_TCP;
			if(buf)
			error = qapi_getnwstats(ifind, protocol, operation, buf);


			if(error == 0)
			{
			    qapi_tcp_stats_t* temp;
				if(buf){
				 temp = buf->u_prot_stats.tcp;
			
			    QCLI_Printf(qcli_net_handle, "TCP Staitistics \n");
				QCLI_Printf(qcli_net_handle, "tcpRtoAlgorithm: %lu \n tcpRtoMin: %lu \n", temp->tcpRtoAlgorithm, temp->tcpRtoMin);
				QCLI_Printf(qcli_net_handle, "tcpRtoMax: %lu \n tcpMaxConn: %ld \n", temp->tcpRtoMax, temp->tcpMaxConn);
				QCLI_Printf(qcli_net_handle, "tcpActiveOpens: %lu \n tcpPassiveOpens: %lu \n", temp->tcpActiveOpens, temp->tcpPassiveOpens);
				QCLI_Printf(qcli_net_handle, "tcpAttemptFails: %lu \n tcpEstabResets: %lu \n", temp->tcpAttemptFails, temp->tcpEstabResets);
				QCLI_Printf(qcli_net_handle, "tcpCurrEstab: %lu \n tcpInSegs: %lu \n", temp->tcpCurrEstab, temp->tcpInSegs);
				QCLI_Printf(qcli_net_handle, "tcpOutSegs: %lu \n tcpRetransSegs: %lu \n", temp->tcpOutSegs, temp->tcpRetransSegs);
				QCLI_Printf(qcli_net_handle, "tcpInErrs: %lu \n tcpOutRsts: %lu \n", temp->tcpInErrs, temp->tcpOutRsts);
				QCLI_Printf(qcli_net_handle, "tcpFilterDrop: %lu \n", temp->tcpFilterDrop);
				}
			}
			else
            {
                QCLI_Printf(qcli_net_handle,"Error while fetching TCP network statistics\n");
			}
        }
		else if(strncmp(prot,"UDP",3)==0)
        {
            if (TX_SUCCESS != tx_byte_allocate(byte_pool_qcli, (VOID **)&buf->u_prot_stats.udp, sizeof(qapi_udp_stats_t), TX_NO_WAIT))
        	{
            	QCLI_Printf(qcli_net_handle,"tx_byte_allocate [h_addr_list] failed!");
            	result = QCLI_STATUS_ERROR_E;
        	}

			protocol = QAPI_NWSTATS_UDP;
			if(buf)
			error = qapi_getnwstats(ifind, protocol, operation, buf);

			if(error == 0)
			{
			    qapi_udp_stats_t* temp;
				if(buf){
				 temp = buf->u_prot_stats.udp;
			
			    QCLI_Printf(qcli_net_handle, "UDP Staitistics \n");
				QCLI_Printf(qcli_net_handle, "udpInDatagrams: %lu \n udpNoPorts: %lu \n", temp->udpInDatagrams, temp->udpNoPorts);
				QCLI_Printf(qcli_net_handle, "udpInErrors: %lu \n udpOutDatagrams: %ld \n", temp->udpInErrors, temp->udpOutDatagrams);
				QCLI_Printf(qcli_net_handle, "udpOutErrors: %lu \n", temp->udpOutErrors);
				QCLI_Printf(qcli_net_handle, "udpFilterDrop: %lu \n", temp->udpFilterDrop);
				}
			}
			else
            {
                QCLI_Printf(qcli_net_handle,"Error while fetching UDP network statistics\n");
			}
        }
		else if(strncmp(prot,"ICMP4",5)==0)
        {
            if (TX_SUCCESS != tx_byte_allocate(byte_pool_qcli, (VOID **)&buf->u_prot_stats.icmp, sizeof(qapi_icmp_stats_t), TX_NO_WAIT))
        	{
            	QCLI_Printf(qcli_net_handle,"tx_byte_allocate [h_addr_list] failed!");
            	result = QCLI_STATUS_ERROR_E;
        	}

			protocol = QAPI_NWSTATS_ICMP;
			if(buf)
			error = qapi_getnwstats(ifind, protocol, operation, buf);

			if(error == 0)
			{
			    qapi_icmp_stats_t* temp;
				if(buf){
				 temp = buf->u_prot_stats.icmp;
			
			    QCLI_Printf(qcli_net_handle, "ICMPv4 Staitistics \n");
				QCLI_Printf(qcli_net_handle, "icmpInMsgs: %lu \n icmpInErrors: %lu \n", temp->icmpInMsgs, temp->icmpInErrors);
				QCLI_Printf(qcli_net_handle, "icmpInDestUnreachs: %lu \n IP icmpInTimeExcds: %lu \n", temp->icmpInDestUnreachs, temp->icmpInTimeExcds);
				QCLI_Printf(qcli_net_handle, "icmpInParmProbs: %lu \n icmpInSrcQuenchs: %lu \n", temp->icmpInParmProbs, temp->icmpInSrcQuenchs);
				QCLI_Printf(qcli_net_handle, "icmpInRedirects: %lu \n icmpInEchos: %lu \n", temp->icmpInRedirects, temp->icmpInEchos);
				QCLI_Printf(qcli_net_handle, "icmpInEchoReps: %lu \n icmpInTimestamps: %lu \n", temp->icmpInEchoReps, temp->icmpInTimestamps);
				QCLI_Printf(qcli_net_handle, "icmpInTimestampReps: %lu \n icmpInAddrMasks: %lu \n", temp->icmpInTimestampReps, temp->icmpInAddrMasks);
				QCLI_Printf(qcli_net_handle, "icmpInAddrMaskReps: %lu \n icmpOutMsgs: %lu \n", temp->icmpInAddrMaskReps, temp->icmpOutMsgs);
				QCLI_Printf(qcli_net_handle, "icmpOutErrors: %lu \n icmpOutDestUnreachs: %lu \n", temp->icmpOutErrors, temp->icmpOutDestUnreachs);
				QCLI_Printf(qcli_net_handle, "icmpOutTimeExcds: %lu \n icmpOutParmProbs: %lu \n", temp->icmpOutTimeExcds, temp->icmpOutParmProbs);
				QCLI_Printf(qcli_net_handle, "icmpOutSrcQuenchs: %lu \n icmpOutRedirects: %lu \n", temp->icmpOutSrcQuenchs, temp->icmpOutRedirects);
				QCLI_Printf(qcli_net_handle, "icmpOutEchos: %lu \n icmpOutEchoReps: %lu \n", temp->icmpOutEchos, temp->icmpOutEchoReps);
				QCLI_Printf(qcli_net_handle, "icmpOutTimestamps: %lu \n icmpOutTimestampReps: %lu \n", temp->icmpOutTimestamps, temp->icmpOutTimestampReps);
				QCLI_Printf(qcli_net_handle, "icmpOutAddrMasks: %lu \n icmpOutAddrMaskReps: %lu \n", temp->icmpOutAddrMasks, temp->icmpOutAddrMaskReps);
				}
			}
			else
            {
                QCLI_Printf(qcli_net_handle,"Error while fetching icmpv4 network statistics\n");
			}
        }
		else if(strncmp(prot,"ICMP6",5)==0)
        {
            if (TX_SUCCESS != tx_byte_allocate(byte_pool_qcli, (VOID **)&buf->u_prot_stats.icmp6, sizeof(qapi_icmp6_stats_t), TX_NO_WAIT))
        	{
            	QCLI_Printf(qcli_net_handle,"tx_byte_allocate [h_addr_list] failed!");
            	result = QCLI_STATUS_ERROR_E;
        	}

			protocol = QAPI_NWSTATS_ICMPv6;
			if(buf)
			error = qapi_getnwstats(ifind, protocol, operation, buf);
			if(error == 0)
			{
			    qapi_icmp6_stats_t* temp;
				if(buf){
				 temp = buf->u_prot_stats.icmp6;
			
			    QCLI_Printf(qcli_net_handle, "ICMPv6 Staitistics \n");
				QCLI_Printf(qcli_net_handle, "InMsgs: %lu \n InErrors: %lu \n", temp->InMsgs, temp->InErrors);
				QCLI_Printf(qcli_net_handle, "InDestUnreachs: %lu \n IP InAdminProhibs: %lu \n", temp->InDestUnreachs, temp->InAdminProhibs);
				QCLI_Printf(qcli_net_handle, "InTimeExcds: %lu \n InParmProblems: %lu \n", temp->InTimeExcds, temp->InParmProblems);
				QCLI_Printf(qcli_net_handle, "InPktTooBigs: %lu \n InEchos: %lu \n", temp->InPktTooBigs, temp->InEchos);
				QCLI_Printf(qcli_net_handle, "InEchoReplies: %lu \n InRouterSolicits: %lu \n", temp->InEchoReplies, temp->InRouterSolicits);
				QCLI_Printf(qcli_net_handle, "InRouterAdvertisements: %lu \n InNeighborSolicits: %lu \n", temp->InRouterAdvertisements, temp->InNeighborSolicits);
				QCLI_Printf(qcli_net_handle, "InNeighborAdvertisements: %lu \n InRedirects: %lu \n", temp->InNeighborAdvertisements, temp->InRedirects);
				QCLI_Printf(qcli_net_handle, "InGroupMembQueries: %lu \n InGroupMembResponses: %lu \n", temp->InGroupMembQueries, temp->InGroupMembResponses);
				QCLI_Printf(qcli_net_handle, "InGroupMembReductions: %lu \n OutMsgs: %lu \n", temp->InGroupMembReductions, temp->OutMsgs);
				QCLI_Printf(qcli_net_handle, "OutErrors: %lu \n OutDestUnreachs: %lu \n", temp->OutErrors, temp->OutDestUnreachs);
				QCLI_Printf(qcli_net_handle, "OutAdminProhibs: %lu \n OutTimeExcds: %lu \n", temp->OutAdminProhibs, temp->OutTimeExcds);
				QCLI_Printf(qcli_net_handle, "OutParmProblems: %lu \n OutPktTooBigs: %lu \n", temp->OutParmProblems, temp->OutPktTooBigs);
				QCLI_Printf(qcli_net_handle, "OutEchos: %lu \n OutEchoReplies: %lu \n", temp->OutEchos, temp->OutEchoReplies);
				QCLI_Printf(qcli_net_handle, "OutRouterSolicits: %lu \n OutRouterAdvertisements: %lu \n", temp->OutRouterSolicits, temp->OutRouterAdvertisements);
				QCLI_Printf(qcli_net_handle, "OutNeighborSolicits: %lu \n OutNeighborAdvertisements: %lu \n", temp->OutNeighborSolicits, temp->OutNeighborAdvertisements);
				QCLI_Printf(qcli_net_handle, "OutRedirects: %lu \n OutGroupMembQueries: %lu \n", temp->OutRedirects, temp->OutGroupMembQueries);
				QCLI_Printf(qcli_net_handle, "OutGroupMembResponses: %lu \n OutGroupMembReduction: %lu \n", temp->OutGroupMembResponses, temp->OutGroupMembReduction);
				}
			}
			else
            {
                QCLI_Printf(qcli_net_handle,"Error while fetching icmpv6 network statistics\n");
			}
        }   
    }		 
   return result;
	 
}

static QCLI_Command_Status_t stack_filter_configure (uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{

    QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;
   
    int retval = -1;
    
    int ip_v4_filter = 0;
    int ip_v6_filter = 0;
    int port_filter = 0;
   
    int add_filter = 1;
   
    filter_params_t filter_info = {0};
   
    char * filterip;
    filterip = NULL;
   
    if(Parameter_Count != 3)
    {
       QCLI_Printf(qcli_net_handle, "Incorrect Command Given\n");
       result = QCLI_STATUS_USAGE_E;
   
      return result;
   
    }
   
   if (strcmp((char*)Parameter_List[0].String_Value, "-IP4") == 0)
    {
       ip_v4_filter =1;
    }
   
    if (strcmp((char*)Parameter_List[0].String_Value, "-IP6") == 0)
    {
      ip_v6_filter = 1;
    }
   
   if (strcmp((char*)Parameter_List[0].String_Value, "-P") == 0)
   {
       port_filter = 1;
   }
   
   if(Parameter_List[2].Integer_Value == 1)
   {
        add_filter = 1;
   }
   else
   {
       add_filter = 0;
   }
   
   
   if(ip_v4_filter || ip_v6_filter)
   {
      filter_info.mask = FILTER_MASK_IP_DISALLOW;
   
       filterip = (char*)Parameter_List[1].String_Value;
   
      if(ip_v6_filter)
      {
         if ((retval = inet_pton(AF_INET6, filterip,(void *) &filter_info.ainfo.f.v6_addr)) != 0)
         {
            QCLI_Printf(qcli_net_handle, "Incorrect IPv6 address %s\n", filterip);
            result = QCLI_STATUS_USAGE_E;
          
         }
   	  filter_info.ainfo.mask = IPFILTER_MASK_V6;
   	 
      }
   
      if(ip_v4_filter)
      {
         if ((retval = inet_pton(AF_INET, filterip, (void *)&filter_info.ainfo.f.v4_addr)) != 0)
         {
            QCLI_Printf(qcli_net_handle, "Incorrect IPv4 address %s\n", filterip);
            result = QCLI_STATUS_USAGE_E;
          
         }
   	   filter_info.ainfo.mask = IPFILTER_MASK_V4;
   
      }
   
   }
   
   if(port_filter)
   {
       filter_info.mask = FILTER_MASK_PORT_DISALLOW;
   
      filter_info.pinfo.port = Parameter_List[1].Integer_Value;
   
   }
   
    if(add_filter == 1)
    {
      retval = qapi_add_stack_filter(&filter_info);
    }
   else
   {
         retval = qapi_remove_stack_filter(&filter_info);
   }
   
   if(retval != 0)
   {
       QCLI_Printf(qcli_net_handle, "Adding Stack Filter failed %s\n", filterip);
       result = QCLI_STATUS_ERROR_E;
	   return result;
   }
   
   result = QCLI_STATUS_SUCCESS_E;

   QCLI_Printf(qcli_net_handle, "stack_filter_configured with result = %d\n" , result);
   
   return result;
}


static QCLI_Command_Status_t server_abort(void)
{
    server_quit = 1;
    return QCLI_STATUS_SUCCESS_E;
}

static int test_for_quit_s(void)
{
    return server_quit;
}
