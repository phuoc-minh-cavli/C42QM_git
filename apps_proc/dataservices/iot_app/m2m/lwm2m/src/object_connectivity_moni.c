/******************************************************************************

  @file    object_connectivity_moni.c

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

/*******************************************************************************
 *
 * Copyright (c) 2014 Bosch Software Innovations GmbH Germany. 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    
 *******************************************************************************/

/*
 *  This Connectivity Monitoring object is optional and has a single instance
 * 
 *  Resources:
 *
 *          Name             | ID | Oper. | Inst. | Mand.|  Type   | Range | Units |
 *  Network Bearer           |  0 |  R    | Single|  Yes | Integer |       |       |
 *  Available Network Bearer |  1 |  R    | Multi |  Yes | Integer |       |       |
 *  Radio Signal Strength    |  2 |  R    | Single|  Yes | Integer |       | dBm   |
 *  Link Quality             |  3 |  R    | Single|  No  | Integer | 0-100 |   %   |
 *  IP Addresses             |  4 |  R    | Multi |  Yes | String  |       |       |
 *  Router IP Addresses      |  5 |  R    | Multi |  No  | String  |       |       |
 *  Link Utilization         |  6 |  R    | Single|  No  | Integer | 0-100 |   %   |
 *  APN                      |  7 |  R    | Multi |  No  | String  |       |       |
 *  Cell ID                  |  8 |  R    | Single|  No  | Integer |       |       |
 *  SMNC                     |  9 |  R    | Single|  No  | Integer | 0-999 |   %   |
 *  SMCC                     | 10 |  R    | Single|  No  | Integer | 0-999 |       |
 *
 */

#include "liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "qapi_dss.h"
#include "iotapp_dss_util.h"
#include "iotapp_qmi_internal.h"
#include "obj_connectivity_moni.h"
#include "lwm2m_signals.h"
#include "qapi_coap.h"
#include "qapi_ns_utils.h"
#include "network_access_service_v01.h"
#include "internals.h"

#include "qapi_device_info.h"
#include "lwm2mclient.h"
#include "ds_log.h"



#ifdef ENABLE_CONN_MONITOR

extern qurt_signal_t lwm2m_signal_Handle;
extern bool g_carrier_cfg; 
// Resource Id's:
#define RES_M_NETWORK_BEARER            0
#define RES_M_AVL_NETWORK_BEARER        1
#define RES_M_RADIO_SIGNAL_STRENGTH     2
#define RES_O_LINK_QUALITY              3
#define RES_M_IP_ADDRESSES              4
#define RES_O_ROUTER_IP_ADDRESS         5
#define RES_O_LINK_UTILIZATION          6
#define RES_O_APN                       7
#define RES_O_CELL_ID                   8
#define RES_O_SMNC                      9
#define RES_O_SMCC                      10
#define RES_O_APN_30000                 30000
#define RES_O_APN_CLASS2                 0
#define RES_O_APN_CLASS3                 1
#define RES_O_APN_CLASS6                 2
#define RES_O_APN_CLASS7                 3


#define CLASS3_APN "VZWINTERNET"
#define CLASS6_APN "VZWCLASS6"
#define CLASS7_APN "VZWIOTTS"

extern char    *print_ipad(unsigned long); // 4.10/h/net.h
extern network_bearer_t srvc_type;

char class3_apn[CARRIER_APN_LEN];
char class6_apn[CARRIER_APN_LEN];
char class7_apn[CARRIER_APN_LEN];

char class3_apn_prev[CARRIER_APN_LEN];
char class6_apn_prev[CARRIER_APN_LEN];
char class7_apn_prev[CARRIER_APN_LEN];


typedef struct conn_mon_persistent_info_
{
  char class2_apn[CARRIER_APN_LEN];
  char class3_apn[CARRIER_APN_LEN];
  char class6_apn[CARRIER_APN_LEN];
  char class7_apn[CARRIER_APN_LEN];
} conn_mon_persistent_info_t;

bool conn_mon_created = FALSE;
static bool create_operation = LWM2M_FALSE;

void free_object_conn_m(lwm2m_object_t * objectP);

static uint8_t m2m_get_nw_bearer(conn_m_data_t * connDataP)
{
  qapi_Device_Info_t info;
  memset(&info, 0x00, sizeof(qapi_Device_Info_t));
  
  if(qapi_Device_Info_Get_v2(get_lwm2m_dev_info_hdl(),QAPI_DEVICE_INFO_NETWORK_BEARER_E, &info)!=QAPI_OK)
    return LWM2M_500_INTERNAL_SERVER_ERROR;

  connDataP->networkBearer = info.u.valueint;
  if(connDataP->networkBearer == IOTAPP_GSM)
    connDataP->networkBearer = GSM_NW_BEARER;
  else if(connDataP->networkBearer == IOTAPP_WCDMA)
    connDataP->networkBearer = WCDMA_NW_BEARER;

  if(srvc_type.network_type== IOTAPP_LTE)
  {
    if(iotapp_qmi_get_service_status(false) == TRUE)
    {
      if(srvc_type.ciot_mode == IOTAPP_CIOT_MODE_NB1)
      {
        connDataP->networkBearer = IOTAPP_LTE_NB_IOT;
      }
    }
  }
  return LWM2M_205_CONTENT;
}

/** 
 * @fn     load_conn_mon_persistent_info()
 * @brief  This function is used to restore APN values from persistence file
 * @param  connObjP - pointer to connectivity monitoring object
 * @return on success  - ESUCCESS
 on error    - EFAILURE
 */ 
int load_conn_mon_persistent_info(lwm2m_context_t * context )
{
  int fd = 0, ret = 0;
  conn_m_data_t *targetP = NULL;
  conn_mon_persistent_info_t info;
  lwm2m_object_t *connObjP = NULL;
  carrier_apn_cfg_t  *carrier_apn_cfg = NULL;
  bool val_change = false;

  if (NULL == context)
  {
    LOG_DEBUG("%s",nullError);
    return EFAILURE;
  }
  ds_iot_log_strf_med("EFS_OPT: File open %s",CONN_MON_PERSISTENCE_FILE);
  fd = efs_open(CONN_MON_PERSISTENCE_FILE, 00);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open connectivity monitoring persistence file");
    return EFAILURE;
  }

  connObjP = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_CONN_MONITOR_OBJECT_ID);
  if (connObjP == NULL) 
  {
    return EFAILURE;
  }
  memset(&info, 0x00, sizeof(info));
  targetP = NULL;

  ret = efs_read(fd, &info, sizeof(info));
  if (ret <= 0)
  {
    return EFAILURE;
  }

  if( NULL == connObjP->userData)
  {
    efs_close(fd);
    return EFAILURE;
  }

  // Restoring the APN values into connectivity monitoring structure
  targetP = (conn_m_data_t*)connObjP->userData;
  if (targetP != NULL) {
    prv_write_string_resource_inst_val(&(targetP->apnList),RES_O_APN_CLASS2, info.class2_apn, &val_change);
    prv_write_string_resource_inst_val(&(targetP->apnList),RES_O_APN_CLASS3, info.class3_apn, &val_change);
    prv_write_string_resource_inst_val(&(targetP->apnList),RES_O_APN_CLASS6, info.class6_apn, &val_change);
    prv_write_string_resource_inst_val(&(targetP->apnList),RES_O_APN_CLASS7, info.class7_apn, &val_change);
  }

  // Restoring the APN values into LWM2M context 
  carrier_apn_cfg = context->c_apn_cfg;
  if (carrier_apn_cfg != NULL) 
  {
    while (carrier_apn_cfg != NULL) 
    {
      if (carrier_apn_cfg->apn_class == LWM2M_APN_CLASS2) 
      {
        LWM2M_STRCPY(carrier_apn_cfg->apn_name, info.class2_apn, CARRIER_APN_LEN);
      } else if (carrier_apn_cfg->apn_class == LWM2M_APN_CLASS3) 
      {
        LWM2M_STRCPY(carrier_apn_cfg->apn_name, info.class3_apn, CARRIER_APN_LEN);
      }
      carrier_apn_cfg = carrier_apn_cfg->next ;
    }
  } else 
  {
    efs_close(fd);
    return EFAILURE;
  }

  efs_close(fd);
  return ESUCCESS;
}


/** 
* @fn static uint8_t prv_conn_m_create_write_check() 
*
* @brief This fucntion verifies and write the resource's into  
*        connectivity monitoring object 
*
* @param instanceId Instance of the connectivity monitoring to be create/write 
* @param numDataP number of resources in the array 
* @param dataArrayP resource value array 
* @param create_flag - if true creating instance else write on instance 
*
* @return LWM2M response type 
*
*/
/***************************************************************************************************

Under section 8.2.2 Operation on Object Instance of OMA 1.1

- For the �Write� operation on an Object Instance, the LwM2M Client MUST perform the operation, 
if all the Resources conveyed in the operation are allowed to perform the �Write� operation. 
If any Resource does not support the �Write� operation, the LwM2M Client MUST inform the LwM2M 
Server that the Object Instance cannot perform the requested �Write� operation in sending a 
�Operation is not supported� error code.

*************************************************************************************************************/  
static uint8_t prv_conn_m_create_write_check(int numData,
                                               lwm2m_data_t * dataArray,
                                               bool create_flag)
{ 
  int16_t i = 0; 
  uint8_t result = LWM2M_NO_ERROR; 
  if((NULL == dataArray))
  { 
    LOG_DEBUG("%s", nullError); 
    return LWM2M_500_INTERNAL_SERVER_ERROR; 
  }
  i = 0;
  do
  {
    switch(dataArray[i].id)
    {
       case RES_M_NETWORK_BEARER:
       case RES_M_AVL_NETWORK_BEARER:
       case RES_M_RADIO_SIGNAL_STRENGTH:
       case RES_O_LINK_QUALITY:
       case RES_M_IP_ADDRESSES:
       case RES_O_ROUTER_IP_ADDRESS:
       case RES_O_APN:
       case RES_O_CELL_ID:
       case RES_O_SMNC:
       case RES_O_SMCC:
        if (!create_flag)
        {
          result = LWM2M_405_METHOD_NOT_ALLOWED;
        }
        else
        {
         result = LWM2M_204_CHANGED;
        }
        break;
      case RES_O_APN_30000:
        if(!g_carrier_cfg)
        {
         result = LWM2M_405_METHOD_NOT_ALLOWED;
        }
        else
        {
          result = LWM2M_204_CHANGED;
        }
        break;
      default:
        if(!create_flag)
        {
          result = LWM2M_404_NOT_FOUND;
        }
        else
        {
          result = LWM2M_204_CHANGED;
        }
        break;
    }
  }while(++i < numData && result == LWM2M_204_CHANGED);

  if(result != LWM2M_204_CHANGED)
  {
     ds_iot_ulog_high1("failed for resource id %d",dataArray[i-1].id);
     return result;
  }
   return result;
}



bool store_prev_apn_before_update(resource_instance_string_list_t** rsList, uint16_t resId)
{
  bool ret = false;
  resource_instance_string_list_t *temp = *rsList;
  while(temp)
  {
    if(temp->resInstId == resId)
    {
      switch(resId)
      {
        case RES_O_APN_CLASS3:
          LWM2M_STRCPY( class3_apn_prev, temp->InstValue, CARRIER_APN_LEN);
          break;
        case RES_O_APN_CLASS6:
          LWM2M_STRCPY( class6_apn_prev, temp->InstValue, CARRIER_APN_LEN);
          break;
        case RES_O_APN_CLASS7:
          LWM2M_STRCPY( class7_apn_prev, temp->InstValue, CARRIER_APN_LEN);
          break;
        default:
           break;
            
      }
        return true;
    }
    temp = temp->next;
  }
  return ret;
}


/**
 * @fn static uint8_t prv_set_value()
 *
 * @brief This fucntion is used to populate the values of the resource
 *        to be provided to the server during the read operation on the object.
 * @param dataP data array for the resource 
 * @param connDataP connectivity monitoring object data
 * @return LWM2M response type
 */
static uint8_t prv_set_value(lwm2m_data_t * dataP,
    conn_m_data_t * connDataP, uint16_t resInstId)
{ 
  int mode_pref = 0;
  bool val_change = false;

  if (dataP == NULL || connDataP == NULL)
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  switch (dataP->id)
  {
    case RES_M_NETWORK_BEARER:
      { 
        m2m_get_nw_bearer(connDataP);
        lwm2m_data_encode_int(connDataP->networkBearer, dataP);	
      return LWM2M_205_CONTENT;
      }

    case RES_M_AVL_NETWORK_BEARER:
      {
        int i = 0;
        lwm2m_data_t * subTlvP = NULL;
        qapi_Device_Info_t info;
        memset(&info, 0x00, sizeof(qapi_Device_Info_t));

        m2m_get_nw_bearer(connDataP);

        if(qapi_Device_Info_Get_v2(get_lwm2m_dev_info_hdl(),QAPI_DEVICE_INFO_MODE_PREF_E, &info)!=QAPI_OK)	    
          return LWM2M_500_INTERNAL_SERVER_ERROR;
        mode_pref = info.u.valint.buf[0];
        if(mode_pref != 0)
        {
          connDataP->availNetworkBearerLength = 0;
          if(mode_pref & QMI_NAS_RAT_MODE_PREF_GSM_V01)
          {
            connDataP->availNetworkBearer[0] = 0;
            connDataP->availNetworkBearerLength++;
          }
          if(mode_pref & QMI_NAS_RAT_MODE_PREF_LTE_V01)
          {
            connDataP->availNetworkBearer[connDataP->availNetworkBearerLength] = srvc_type.lte_mode;
            connDataP->availNetworkBearerLength++;
          }
          if(mode_pref & QMI_NAS_RAT_MODE_PREF_UMTS_V01)
          {
            connDataP->availNetworkBearer[connDataP->availNetworkBearerLength] = 2;
            connDataP->availNetworkBearerLength++;
          }
        }

        if (0 == connDataP->availNetworkBearerLength)
        {
          if(resInstId == INVALID_RES_INST_ID)
          {
            lwm2m_data_t *subTlv = NULL;
            lwm2m_data_encode_instances(subTlv, 0, dataP);
            return LWM2M_205_CONTENT ;
          }
          else
           return LWM2M_404_NOT_FOUND;
        }

        if (resInstId != INVALID_RES_INST_ID)
        {
          if(resInstId >= connDataP->availNetworkBearerLength)
            return LWM2M_404_NOT_FOUND;

          subTlvP = lwm2m_data_new(1);
          if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;

          subTlvP->id = resInstId;
          lwm2m_data_encode_int(connDataP->availNetworkBearer[resInstId], subTlvP);
          lwm2m_data_encode_instances(subTlvP, 1, dataP);
        }
        else
        {
          subTlvP = lwm2m_data_new(connDataP->availNetworkBearerLength);
          LOG_DEBUG("length : %d", connDataP->availNetworkBearerLength);
          if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;

          for(i = 0; i < connDataP->availNetworkBearerLength; i++)
          {
            subTlvP[i].id = i;
            LOG_DEBUG("connDataP->availNetworkBearer[%d]: %d", i, connDataP->availNetworkBearer[i]);
            lwm2m_data_encode_int(connDataP->availNetworkBearer[i], &subTlvP[i]);
          }
          lwm2m_data_encode_instances(subTlvP, connDataP->availNetworkBearerLength, dataP);
        }
        return LWM2M_205_CONTENT ;        
      }
    case RES_M_RADIO_SIGNAL_STRENGTH: //s-int
      {
        connDataP->signalStrength = nas_get_signal_strength(PARAM_SIGNAL_STRENGTH); //gettin the value from qmi
        if(connDataP->signalStrength == EFAILURE)
        {
          return LWM2M_500_INTERNAL_SERVER_ERROR;
          //connDataP->signalStrength = 0;     // TODO: Need to check values
        }
        LOG_DEBUG("Signal Strength: %d", connDataP->signalStrength);

        lwm2m_data_encode_int(connDataP->signalStrength, dataP);
        return LWM2M_205_CONTENT;
      }
    case RES_O_LINK_QUALITY: //s-int
      {
		qapi_Device_Info_t info;
		memset(&info, 0x00, sizeof(qapi_Device_Info_t));
		if(qapi_Device_Info_Get_v2(get_lwm2m_dev_info_hdl(),QAPI_DEVICE_INFO_LINK_QUALITY_E, &info)!=QAPI_OK)	    
		{
		  return LWM2M_500_INTERNAL_SERVER_ERROR;
          //LOG_DEBUG("Link Quality hard coded to zero");
          //connDataP->linkQuality = 0;
        }
		else
			connDataP->linkQuality = info.u.valueint;
		
        lwm2m_data_encode_int(connDataP->linkQuality, dataP);
        return LWM2M_205_CONTENT ;
      }
    case RES_M_IP_ADDRESSES:
      {
        int ipCnt = 0;   // reduced to 1 instance to fit in one block size
        qapi_DSS_Addr_Info_t *ip_addr = NULL;
        qapi_DSS_Addr_Info_t *ip_addr_v6 = NULL;
        qapi_DSS_Addr_Info_t *ip_addr_non_ip = NULL;
        char *ip_addr_str = NULL;
        char ip_addr_str_v6[48];
        resource_instance_string_list_t* apnList = NULL; 

        int8_t ri = 0;
        int8_t ri_save = 0;
        resource_instance_string_list_t* ipListSave = connDataP->ipAddresses;
        connDataP->ipAddresses = NULL;

        if(g_carrier_cfg)
        {
          apnList = connDataP->apnList;
        }
        else 
        {
          apnList = connDataP->apn_name_list;
        }
        ri = 0;
        while(apnList)
        {
          ip_addr = iot_dss_ip_info_by_apn(apnList->InstValue, &ipCnt, AF_INET);
          ip_addr_v6 = iot_dss_ip_info_by_apn(apnList->InstValue, &ipCnt, AF_INET6);
          ipCnt = 0;
          ip_addr_non_ip = iot_dss_ip_info_by_apn(apnList->InstValue, &ipCnt, AF_NONIP);
          if (NULL == ip_addr)
          {
            ds_iot_log_strf_err("iot_dss_ip_info_by_apn failed for APN %s v4 family", apnList->InstValue);
            ri--;
          }
          else
          {
            lwm2m_free(ip_addr);
            ip_addr = NULL;
          }
          if (NULL == ip_addr_v6)
          {
            ds_iot_log_strf_err("iot_dss_ip_info_by_apn failed for APN %s v6 family", apnList->InstValue);
            ri--;
          }
          else
          {
            lwm2m_free(ip_addr_v6);
            ip_addr_v6 = NULL;
          }
          if (NULL == ip_addr_non_ip && ipCnt != 1)
          {
            ds_iot_log_strf_err("iot_dss_ip_info_by_apn failed for APN %s Non IP family", apnList->InstValue);
            ri--;
          }
          ri = ri+3;
          apnList = apnList->next;
        }
        if (ri==0)  // no values!
        {
          LWM2M_LIST_FREE(ipListSave);
          if(resInstId == INVALID_RES_INST_ID)
          {
            lwm2m_data_t *subTlv = NULL;
            lwm2m_data_encode_instances(subTlv, 0, dataP);
            return LWM2M_205_CONTENT ;
          }
          else
            return LWM2M_404_NOT_FOUND;
        }
        else
        {
          lwm2m_data_t* subTlvP = lwm2m_data_new(ri);
          
          if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;
          ds_iot_ulog_high1("No of IP add %d", ri);
          if(g_carrier_cfg)
          {
            apnList = connDataP->apnList;
          }
          else 
          {
            apnList = connDataP->apn_name_list;
          }
          ri = 0;
          ri_save = 0;
          while(apnList)
          {
            ip_addr = iot_dss_ip_info_by_apn(apnList->InstValue, &ipCnt, AF_INET);
            ip_addr_v6 = iot_dss_ip_info_by_apn(apnList->InstValue, &ipCnt, AF_INET6); 
            ipCnt = 0;
            ip_addr_non_ip = iot_dss_ip_info_by_apn(apnList->InstValue, &ipCnt, AF_NONIP);
            if (NULL == ip_addr && NULL == ip_addr_v6 && (NULL == ip_addr_non_ip && ipCnt != 1))
            {
              apnList = apnList->next;
              ri = ri+3;
              continue;
            }
            if(ip_addr != NULL )
            { 
              if(ip_addr[0].iface_addr_s.valid_addr) 
              {
                subTlvP[ri_save].id = ri;
                ip_addr_str = ((char *)print_ipad(ip_addr[0].iface_addr_s.addr.v4));
                lwm2m_data_encode_string(ip_addr_str, &subTlvP[ri_save]);
                prv_write_string_resource_inst_val(&(connDataP->ipAddresses),ri, ip_addr_str, &val_change);
                ri_save++;
                ds_iot_log_strf_high("Encoded IP  %s ri = %d", ip_addr_str, ri);
              }
              lwm2m_free(ip_addr);
              ip_addr = NULL;
            }
            ri++;
            if(ip_addr_v6 != NULL )
            { 
              if(ip_addr_v6[0].iface_addr_s.valid_addr) 
              {
                subTlvP[ri_save].id = ri;
                inet_ntop(AF_INET6, ip_addr_v6[0].iface_addr_s.addr.v6_addr8, ip_addr_str_v6, sizeof(ip_addr_str_v6));
                lwm2m_data_encode_string(ip_addr_str_v6, &subTlvP[ri_save]);
                prv_write_string_resource_inst_val(&(connDataP->ipAddresses),ri, ip_addr_str_v6, &val_change);
                ri_save++;
                ds_iot_log_strf_high("Encoded IP  %s ri = %d", ip_addr_str_v6, ri);
              }
              lwm2m_free(ip_addr_v6);
              ip_addr_v6 = NULL;
            }
            ri++;
            if(ip_addr_non_ip == NULL && ipCnt == 1)
            {
              subTlvP[ri_save].id = ri;
              memset(ip_addr_str_v6, 0x00, sizeof(ip_addr_str_v6));
              lwm2m_data_encode_string(ip_addr_str_v6, &subTlvP[ri_save]);
              prv_write_string_resource_inst_val(&(connDataP->ipAddresses),ri, ip_addr_str_v6, &val_change);
              ri_save++;
              ds_iot_log_strf_high("Encoded IP  %s ri = %d", ip_addr_str_v6, ri);
            }
            ri++;
            apnList = apnList->next;
          }

          if(resInstId == INVALID_RES_INST_ID)
          {
            lwm2m_data_encode_instances(subTlvP, ri_save, dataP);
            LWM2M_LIST_FREE(ipListSave);
            return LWM2M_205_CONTENT;
          }
          else
          {
            resource_instance_string_list_t *resInst = (resource_instance_string_list_t *)LWM2M_LIST_FIND(connDataP->ipAddresses,resInstId);
            if(resInst != NULL)
            {
              lwm2m_data_encode_string(resInst->InstValue, dataP);
              lwm2m_data_free(ri_save, subTlvP);
              LWM2M_LIST_FREE(ipListSave);
              return LWM2M_205_CONTENT;
            }
            else
            {
              lwm2m_data_free(ri_save, subTlvP);
              LWM2M_LIST_FREE(ipListSave);
              return LWM2M_404_NOT_FOUND;
            }
          }
        }
      }

    case RES_O_ROUTER_IP_ADDRESS:
      {
        int ipCnt = 0;   // reduced to 1 instance to fit in one block size
        qapi_DSS_Addr_Info_t *ip_addr = NULL;
        qapi_DSS_Addr_Info_t *ip_addr_v6 = NULL;
        qapi_DSS_Addr_Info_t *ip_addr_non_ip = NULL;
        char *ip_addr_str = NULL;
        char ip_addr_str_v6[48];
        resource_instance_string_list_t* apnList = NULL; 

        int8_t ri = 0;
        int8_t ri_save = 0;
        resource_instance_string_list_t* ipListSave = connDataP->routerIpAddresses;
        connDataP->routerIpAddresses = NULL;
        if(g_carrier_cfg)
        {
          apnList = connDataP->apnList;
        }
        else 
        {
          apnList = connDataP->apn_name_list;
        }
        ri = 0;
        while(apnList)
        {
          ip_addr = iot_dss_ip_info_by_apn(apnList->InstValue, &ipCnt, AF_INET);
          ip_addr_v6 = iot_dss_ip_info_by_apn(apnList->InstValue, &ipCnt, AF_INET6);
          ipCnt = 0;
          ip_addr_non_ip = iot_dss_ip_info_by_apn(apnList->InstValue, &ipCnt, AF_NONIP);
          if (NULL == ip_addr)
          {
            ds_iot_log_strf_err("iot_dss_ip_info_by_apn failed for APN %s v4 family", apnList->InstValue);
            ri--;
          }
          else
          {
            lwm2m_free(ip_addr);
            ip_addr = NULL;
          }
          if (NULL == ip_addr_v6)
          {
            ds_iot_log_strf_err("iot_dss_ip_info_by_apn failed for APN %s v6 family", apnList->InstValue);
            ri--;
          }
          else
          {
            lwm2m_free(ip_addr_v6);
            ip_addr_v6 = NULL;
          }
          if (NULL == ip_addr_non_ip && ipCnt != 1)
          {
            ds_iot_log_strf_err("iot_dss_ip_info_by_apn failed for APN %s Non IP family", apnList->InstValue);
            ri--;
          }
          ri = ri+3;
          apnList = apnList->next;
        }

        if (ri==0)  // no values!
        {
          LWM2M_LIST_FREE(ipListSave);
          if(resInstId == INVALID_RES_INST_ID)
          {
            lwm2m_data_t *subTlv = NULL;
            lwm2m_data_encode_instances(subTlv, 0, dataP);
            return LWM2M_205_CONTENT ;
          }
          else
            return LWM2M_404_NOT_FOUND;
        }
        else
        {
          lwm2m_data_t* subTlvP = lwm2m_data_new(ri);
          if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;
          ds_iot_ulog_high1("No of IP add %d", ri);
          if(g_carrier_cfg)
          {
            apnList = connDataP->apnList;
          }
          else 
          {
            apnList = connDataP->apn_name_list;
          }
          ri = 0;
          ri_save = 0;
          while(apnList)
          {
            ip_addr = iot_dss_ip_info_by_apn(apnList->InstValue, &ipCnt, AF_INET);
            ip_addr_v6 = iot_dss_ip_info_by_apn(apnList->InstValue, &ipCnt, AF_INET6); 
            ipCnt = 0;
            ip_addr_non_ip = iot_dss_ip_info_by_apn(apnList->InstValue, &ipCnt, AF_NONIP);
            if (NULL == ip_addr && NULL == ip_addr_v6 && (NULL == ip_addr_non_ip && ipCnt != 1))
            {
              apnList = apnList->next;
              ri = ri+3;
              continue;
            }
            if(ip_addr != NULL )
            { 
              if(ip_addr[0].gtwy_addr_s.valid_addr) 
              {
                subTlvP[ri_save].id = ri;
                ip_addr_str = ((char *)print_ipad(ip_addr[0].gtwy_addr_s.addr.v4));
                lwm2m_data_encode_string(ip_addr_str, &subTlvP[ri_save]);
                prv_write_string_resource_inst_val(&(connDataP->routerIpAddresses),ri, ip_addr_str, &val_change);
                ri_save++;
                ds_iot_log_strf_high("Encoded IP  %s ri = %d", ip_addr_str, ri);
              }
              lwm2m_free(ip_addr);
              ip_addr = NULL;
            }
            ri++;
            if(ip_addr_v6 != NULL )
            { 
              if(ip_addr_v6[0].gtwy_addr_s.valid_addr) 
              {
                subTlvP[ri_save].id = ri;
                inet_ntop(AF_INET6, ip_addr_v6[0].gtwy_addr_s.addr.v6_addr8, ip_addr_str_v6, sizeof(ip_addr_str_v6));
                lwm2m_data_encode_string(ip_addr_str_v6, &subTlvP[ri_save]);
                prv_write_string_resource_inst_val(&(connDataP->routerIpAddresses),ri, ip_addr_str_v6, &val_change);
                ri_save++;
                ds_iot_log_strf_high("Encoded IP  %s ri = %d", ip_addr_str_v6, ri);
              }
              lwm2m_free(ip_addr_v6);
              ip_addr_v6 = NULL;
            }
            ri++;
            if(ip_addr_non_ip == NULL && ipCnt == 1)
            {
              subTlvP[ri_save].id = ri;
              memset(ip_addr_str_v6, 0x00, sizeof(ip_addr_str_v6));
              lwm2m_data_encode_string(ip_addr_str_v6, &subTlvP[ri_save]);
              prv_write_string_resource_inst_val(&(connDataP->routerIpAddresses),ri, ip_addr_str_v6, &val_change);
              ri_save++;
              ds_iot_log_strf_high("Encoded IP  %s ri = %d", ip_addr_str_v6, ri);
            }
            ri++;
            apnList = apnList->next;
          }
          if(resInstId == INVALID_RES_INST_ID)
          {
            lwm2m_data_encode_instances(subTlvP, ri_save, dataP);
            LWM2M_LIST_FREE(ipListSave);
            return LWM2M_205_CONTENT;
          }
          else
          {
            resource_instance_string_list_t *resInst = (resource_instance_string_list_t *)LWM2M_LIST_FIND(connDataP->routerIpAddresses,resInstId);
            if(resInst != NULL)
            {
              lwm2m_data_encode_string(resInst->InstValue, dataP);
              lwm2m_data_free(ri_save, subTlvP);
              LWM2M_LIST_FREE(ipListSave);
              return LWM2M_205_CONTENT;
            }
            else
            {
              lwm2m_data_free(ri_save, subTlvP);
              LWM2M_LIST_FREE(ipListSave);
              return LWM2M_404_NOT_FOUND;
            }
          }
        }
      }
      // commented the break to address the dead code, as this case is currently not supported 
      //break;

      // As interface is not available , so disabling , once interface available need to enable 
#if 0
      //  VZ_REQ_OTADMLWM2M_41068 Requirement
    case RES_O_LINK_UTILIZATION:
      lwm2m_data_encode_int(connDataP->linkUtilization, dataP);
      return LWM2M_205_CONTENT;
#endif

    case RES_O_APN:
    case RES_O_APN_30000:
      {
        if((dataP->id == RES_O_APN_30000) && (!g_carrier_cfg))
        {
          return LWM2M_404_NOT_FOUND;
        }
        if (resInstId != INVALID_RES_INST_ID)
        {
          resource_instance_string_list_t *resInst = NULL;
          if(g_carrier_cfg)
          {
            resInst= (resource_instance_string_list_t *)LWM2M_LIST_FIND(connDataP->apnList,resInstId);
          }
          else
          {
            resInst= (resource_instance_string_list_t *)LWM2M_LIST_FIND(connDataP->apn_name_list,resInstId);
          }
          if(resInst != NULL)
          {
            if((resInst->resInstId == RES_O_APN_CLASS2) && (dataP->id == RES_O_APN_30000))
              return LWM2M_404_NOT_FOUND;
            lwm2m_data_encode_string(resInst->InstValue, dataP);
            return LWM2M_205_CONTENT;
          }
          else
            return LWM2M_404_NOT_FOUND;
        }
        else
        {
          int ri_apn_7 = 0, ri_apn_30000 = 0;
          resource_instance_string_list_t* apnList = NULL;
          if(!g_carrier_cfg)
          {
            for (apnList = connDataP->apn_name_list, ri_apn_7=0;
              apnList!=NULL;
              apnList= apnList->next, ri_apn_7++);
          }
          else
          {
            for (apnList = connDataP->apnList;apnList!=NULL;apnList = apnList->next)
            {
              if(apnList->resInstId == RES_O_APN_CLASS2)
                ri_apn_7++;
              else
                ri_apn_30000++;
            }
          }
          if ((ri_apn_7==0) && (ri_apn_30000 == 0))  // no values!
          {
            lwm2m_data_t *subTlv = NULL;
            lwm2m_data_encode_instances(subTlv, 0, dataP);
            return LWM2M_205_CONTENT ;
          }
          else
          {
            lwm2m_data_t* subTlvP;
            if(g_carrier_cfg)
            {
              if(dataP->id == RES_O_APN)
                subTlvP = lwm2m_data_new(ri_apn_7);
              else if(dataP->id == RES_O_APN_30000)
                subTlvP = lwm2m_data_new(ri_apn_30000);
            }
            else
            {
              subTlvP = lwm2m_data_new(ri_apn_7);
            }

            if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;
            ri_apn_7 = 0;
            ri_apn_30000 = 0;
            for (apnList = connDataP->apnList; apnList!= NULL;apnList = apnList->next)
            {
              if(g_carrier_cfg)
              {
                if((dataP->id == RES_O_APN) && (apnList->resInstId == RES_O_APN_CLASS2))
                {
                    subTlvP[ri_apn_7].id = (uint16_t)apnList->resInstId;
                    lwm2m_data_encode_string(apnList->InstValue, &subTlvP[ri_apn_7]);
                    ri_apn_7++;
                }
                else if ((dataP->id == RES_O_APN_30000) && (apnList->resInstId != RES_O_APN_CLASS2))
                {
                  subTlvP[ri_apn_30000].id = (uint16_t)apnList->resInstId;
                  lwm2m_data_encode_string(apnList->InstValue, &subTlvP[ri_apn_30000]);
                  ri_apn_30000++;
                }
              }
            }
            for (apnList = connDataP->apn_name_list; apnList!= NULL;apnList = apnList->next)
            {
              if(!g_carrier_cfg)
              {
                subTlvP[ri_apn_7].id = (uint16_t)apnList->resInstId;
                lwm2m_data_encode_string(apnList->InstValue, &subTlvP[ri_apn_7]);
                ri_apn_7++;
              }
            }
            if((g_carrier_cfg) && (dataP->id == RES_O_APN_30000))
              lwm2m_data_encode_instances(subTlvP, ri_apn_30000, dataP);
            else
              lwm2m_data_encode_instances(subTlvP, ri_apn_7, dataP);
            return LWM2M_205_CONTENT;
          }
        }
      }
      // commented the break to address the dead code, as this case is currently not supported 
      //break;
    case RES_O_CELL_ID:
      {
        connDataP->cellId = nas_get_ue_info(PARAM_CELL_ID, false, NULL); // getting the cell id value from qmi
        if(connDataP->cellId == EFAILURE)
        {
          return LWM2M_500_INTERNAL_SERVER_ERROR;
          //connDataP->cellId = 0;    // TODO: Need to check values
        }
        lwm2m_data_encode_int(connDataP->cellId, dataP);
        return LWM2M_205_CONTENT ;
      }
    case RES_O_SMNC:
      { 
        connDataP->mnc = nas_get_ue_info(PARAM_MNC, false, NULL);  // getting the mnc value from qmi
        if(connDataP->mnc == EFAILURE)
        {
          return LWM2M_500_INTERNAL_SERVER_ERROR;
          //connDataP->mnc = 0;    // TODO: Need to check values
        }
        lwm2m_data_encode_int(connDataP->mnc, dataP);
        return LWM2M_205_CONTENT ;
      }
    case RES_O_SMCC:
      {
        connDataP->mcc = nas_get_ue_info(PARAM_MCC, false, NULL);  // getting the mcc value from qmi
        if(connDataP->mcc == EFAILURE)
        {
          return LWM2M_500_INTERNAL_SERVER_ERROR;
          //connDataP->mcc = 0;    // TODO: Need to check values
        }
        lwm2m_data_encode_int(connDataP->mcc, dataP);
        return LWM2M_205_CONTENT ;
      }
    default:
      return LWM2M_404_NOT_FOUND ;
  }
}

/**
 * @fn static uint8_t prv_conn_mon_read()
 *
 * @brief This function implements the read operation performed on Connectivity
 * monitoring object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numDataP number of resources in the array
 * @param dataArrayP resource value array
 * @param objectP Object to be read
 *
 * @return LWm2M response type
 */
static uint8_t prv_conn_mon_read(uint16_t instanceId,
    int * numDataP,
    lwm2m_data_t ** dataArrayP,
    lwm2m_object_t * objectP,
    uint16_t resInstId, lwm2m_server_t *serverP)
{
  uint8_t result = 0;
  int i = 0,  ri_apn_7 = 0, ri_apn_30000 = 0;
  resource_instance_string_list_t *apn;
  conn_m_data_t * targetP;

  if (numDataP == NULL || dataArrayP == NULL || objectP == NULL)
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  // this is a single instance object
  if (instanceId != 0)
  {
    return LWM2M_404_NOT_FOUND ;
  }

  // is the server asking for the full object ?
  if (*numDataP == 0)
  {
    uint16_t resList[] = {
      RES_M_NETWORK_BEARER,
      RES_M_AVL_NETWORK_BEARER,
      RES_M_RADIO_SIGNAL_STRENGTH,
      RES_O_LINK_QUALITY,
      RES_M_IP_ADDRESSES,
      RES_O_ROUTER_IP_ADDRESS,
      // As interface is not available , so disabling , once interface available need to enable 
#if 0  
      // VZ_REQ_OTADMLWM2M_41068 Requirement   
      RES_O_LINK_UTILIZATION,
#endif                
      RES_O_CELL_ID,
      RES_O_SMNC,
      RES_O_SMCC
    };
    int nbRes = sizeof(resList) / sizeof(uint16_t);
    int temp = nbRes;
    targetP = (conn_m_data_t*) (objectP->userData);
    if(targetP != NULL)
    {
      if(!g_carrier_cfg)
      {
        for (apn = targetP->apn_name_list, ri_apn_7=0;
              apn!=NULL;
              apn = apn->next, ri_apn_7++);
      }
      else
      {
        for (apn = targetP->apnList;apn!=NULL;apn = apn->next)
        {
          if(apn->resInstId == RES_O_APN_CLASS2)
            ri_apn_7++;
          else
            ri_apn_30000++;
        }
      }
    }

    if (ri_apn_7 != 0)
      nbRes++;
    if((g_carrier_cfg ) && (ri_apn_30000 != 0 ))
      nbRes++;

    *dataArrayP = lwm2m_data_new(nbRes);
    if (*dataArrayP == NULL)
      return LWM2M_500_INTERNAL_SERVER_ERROR ;

    *numDataP = nbRes;

    for (i = 0; i < temp; i++)
    {
      (*dataArrayP)[i].id = resList[i];
    }
    if (ri_apn_7 != 0)
    {
      (*dataArrayP)[temp].id = RES_O_APN ;
    }
    if ((g_carrier_cfg ) && (ri_apn_30000 != 0 ))
    {
      (*dataArrayP)[temp+1].id = RES_O_APN_30000 ;
    }
  }

  i = 0;
  do
  {
    result = prv_set_value((*dataArrayP) + i, (conn_m_data_t*) (objectP->userData), resInstId);
    i++;
  } while (i < *numDataP && result == LWM2M_205_CONTENT );

  return result;
}

#if 0
/**
 * @fn static uint8_t prv_conn_mon_execute()
 *
 * @brief This fucntion implements the execute operation performed on
 *        Connectivity monitoring object
 *
 * @param instanceId (IN) Instance ID of the object to be read
 * @param resourceId (IN) Resource ID of the resource to be executed
 * @param buffer (IN) message payload from server fro execute operation
 * @param length (IN) message payload length
 * @param objectP Object on which the operation needs to be executed
 * @return LWM2M response type
 */
static uint8_t prv_conn_mon_execute(uint16_t instanceId,
                                    uint16_t resourceId,
                                    uint8_t * buffer,
                                    int length,
                                    lwm2m_object_t * objectP)
{
  conn_m_data_t *instanceP = NULL;

  if ( objectP == NULL )
  {
    LOG_ERROR("%s",nullError);
    return LWM2M_400_BAD_REQUEST;
  }

  instanceP = (conn_m_data_t *)LWM2M_LIST_FIND(objectP->instanceList, instanceId);

  if (NULL == instanceP)
  {
    return LWM2M_404_NOT_FOUND;
  }

  switch(resourceId)
  {
    case RES_M_NETWORK_BEARER:
    case RES_M_AVL_NETWORK_BEARER:
    case RES_M_RADIO_SIGNAL_STRENGTH:
    case RES_O_LINK_QUALITY:
    case RES_M_IP_ADDRESSES:
    case RES_O_ROUTER_IP_ADDRESS:
    case RES_O_LINK_UTILIZATION:
    case RES_O_APN:
    case RES_O_CELL_ID:
    case RES_O_SMNC:
    case RES_O_SMCC:
    case RES_O_APN_30000:
      return LWM2M_405_METHOD_NOT_ALLOWED;
    default:
      return LWM2M_404_NOT_FOUND;
  }
}
#endif

/** 
 * @fn     store_conn_mon_persistent_info()
 * @brief  This function is used to dump APN values into persistence file
 * @param  connDataP - pointer to connectivity monitoring object
 * @return on success  - ESUCCESS
 on error    - EFAILURE
 */ 
int store_conn_mon_persistent_info(conn_m_data_t * connDataP)
{
  int fd = 0;
  conn_mon_persistent_info_t info;
  resource_instance_string_list_t* temp;

  if (NULL == connDataP)
  {
    LOG_DEBUG("%s",nullError);
    return EFAILURE;
  }
  temp = connDataP->apnList;
  ds_iot_log_strf_med("EFS_OPT: File open %s",CONN_MON_PERSISTENCE_FILE);
  fd = efs_open(CONN_MON_PERSISTENCE_FILE, O_CREAT | O_WRONLY, 00644);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open connectivity monitoring persistence file");
    return EFAILURE;
  }

  memset(&info, 0x00, sizeof(info));
  if(temp != NULL)
  {
    LWM2M_STRCPY(info.class2_apn, temp->InstValue, CARRIER_APN_LEN);
    temp = temp->next;
  }
  if(temp != NULL)
  {
    LWM2M_STRCPY(info.class3_apn, temp->InstValue, CARRIER_APN_LEN);
    temp = temp->next;
  }
  if(temp != NULL)
  {
    LWM2M_STRCPY(info.class6_apn, temp->InstValue, CARRIER_APN_LEN);
    temp = temp->next;
  }
  if(temp != NULL)
    LWM2M_STRCPY(info.class7_apn, temp->InstValue, CARRIER_APN_LEN);

  //efs_lseek(fd, sizeof(info), SEEK_SET);
  ds_iot_log_strf_med("EFS_OPT: File write %s",CONN_MON_PERSISTENCE_FILE);
  if (EFAILURE == efs_write(fd, &info, sizeof(info)))
  {
    ds_iot_ulog_err("failed to store persistent value of connectivity monitoring instance ");
    efs_close(fd);
    return EFAILURE;
  }
  efs_close(fd);
  return ESUCCESS;
}

/* @fn      void conn_monitoring_update_ip_addresses()
 * @brief   This function is used to ferch updated ip addresses
 * @param   connDataP - pointer to conn_m_data_t 
 *
 * @return  None
 */ 

void conn_monitoring_update_ip_addresses(conn_m_data_t* connDataP)
{
  int ri = 0;
  resource_instance_string_list_t *apnLst;
  qapi_DSS_Addr_Info_t *ip_addr = NULL;
  qapi_DSS_Addr_Info_t *ip_addr_v6 = NULL;
  qapi_DSS_Addr_Info_t *ip_addr_non_ip = NULL;
  char *ip_addr_str = NULL, *router_ip_addr_str = NULL;
  char ip_addr_str_v6[48], router_ip_addr_str_v6[48];
  int ipCnt = 0;
  resource_instance_string_list_t* ipListSave = NULL;
  resource_instance_string_list_t* routerListSave = NULL;
  int mode_pref = 0;  
  qapi_Device_Info_t info ;
  bool val_change = false;
  memset(&info, 0x00, sizeof(qapi_Device_Info_t));

  if (NULL != connDataP)
  {
    if(!g_carrier_cfg)
    {
      apnLst = connDataP->apn_name_list;
    }
    else 
    {
      apnLst = connDataP->apnList;
    }
    ipListSave = connDataP->ipAddresses;
    connDataP->ipAddresses = NULL;
    routerListSave = connDataP->routerIpAddresses;
    connDataP->routerIpAddresses = NULL;
    while (apnLst!= NULL) 
    {
      ip_addr = iot_dss_ip_info_by_apn(apnLst->InstValue, &ipCnt, AF_INET);
      ip_addr_v6 = iot_dss_ip_info_by_apn(apnLst->InstValue, &ipCnt, AF_INET6);
      ipCnt = 0;
      ip_addr_non_ip = iot_dss_ip_info_by_apn(apnLst->InstValue, &ipCnt, AF_NONIP);
      if( ip_addr != NULL )
      {
        if(ip_addr[0].iface_addr_s.valid_addr )
        { 
          ip_addr_str = ((char *)print_ipad(ip_addr[0].iface_addr_s.addr.v4));
          prv_write_string_resource_inst_val(&(connDataP->ipAddresses),ri, ip_addr_str, &val_change);
        }
        if( ip_addr[0].iface_addr_s.valid_addr )
        { 
          router_ip_addr_str = ((char *)print_ipad(ip_addr[0].gtwy_addr_s.addr.v4));
          prv_write_string_resource_inst_val(&(connDataP->routerIpAddresses),ri, router_ip_addr_str, &val_change);
        }
        ri++;
        lwm2m_free(ip_addr);
        ip_addr = NULL;
      }
      if( ip_addr_v6 != NULL )
      {
        if(ip_addr_v6[0].iface_addr_s.valid_addr )
        { 
          inet_ntop(AF_INET6, ip_addr_v6[0].iface_addr_s.addr.v6_addr8, ip_addr_str_v6, sizeof(ip_addr_str_v6));
          prv_write_string_resource_inst_val(&(connDataP->ipAddresses),ri, ip_addr_str_v6, &val_change);
        }
        if( ip_addr_v6[0].iface_addr_s.valid_addr )
        { 
          inet_ntop(AF_INET6, ip_addr_v6[0].gtwy_addr_s.addr.v6_addr8, router_ip_addr_str_v6, sizeof(router_ip_addr_str_v6));
          prv_write_string_resource_inst_val(&(connDataP->routerIpAddresses),ri, router_ip_addr_str_v6, &val_change);
        }
        ri++;
        lwm2m_free(ip_addr_v6);
        ip_addr_v6 = NULL;
      }
      if( ip_addr_non_ip == NULL && ipCnt == 1)
      {
        memset(ip_addr_str_v6, 0x00, sizeof(ip_addr_str_v6));
        prv_write_string_resource_inst_val(&(connDataP->ipAddresses),ri, ip_addr_str_v6, &val_change);
        memset(ip_addr_str_v6, 0x00, sizeof(ip_addr_str_v6));
        prv_write_string_resource_inst_val(&(connDataP->routerIpAddresses),ri, router_ip_addr_str_v6, &val_change);
        ri++;
      }
      apnLst = apnLst->next ;
    }
    LWM2M_LIST_FREE(ipListSave);
    LWM2M_LIST_FREE(routerListSave);

    m2m_get_nw_bearer(connDataP);
    if(qapi_Device_Info_Get_v2(get_lwm2m_dev_info_hdl(),QAPI_DEVICE_INFO_MODE_PREF_E, &info)!=QAPI_OK)	    
       return;
    mode_pref = info.u.valint.buf[0];

    if(mode_pref != 0)
    {
      connDataP->availNetworkBearerLength = 0;
      if(mode_pref & QMI_NAS_RAT_MODE_PREF_GSM_V01)
      {
        connDataP->availNetworkBearer[0] = 0;
        connDataP->availNetworkBearerLength++;
      }
      if(mode_pref & QMI_NAS_RAT_MODE_PREF_LTE_V01)
      {
        connDataP->availNetworkBearer[connDataP->availNetworkBearerLength] = srvc_type.lte_mode;
        connDataP->availNetworkBearerLength++;
      }
      if(mode_pref & QMI_NAS_RAT_MODE_PREF_UMTS_V01)
      {
        connDataP->availNetworkBearer[connDataP->availNetworkBearerLength] = 2;
        connDataP->availNetworkBearerLength++;
      }
    }
  }
}

/* @fn      static uint8_t prv_conn_mon_discover()
 * @brief   This function is used to discover resources of a connectivity monitoring object
 * @param   instanceId - instance id of object 
 *          numdataP   - pointer to store number of data array elements 
 dataArrayP - pointer to store data 
 objectP    - pointer to lwm2m_object 
 * @return  on success - LWM2M_404_NOT_FOUND
 on error   - LWM2M_404_NOT_FOUND
 */   
static uint8_t prv_conn_mon_discover(uint16_t instanceId,
    int * numDataP,
    lwm2m_data_t ** dataArrayP,
    lwm2m_object_t * objectP, lwm2m_server_t *serverP)
{
  uint8_t result = 0;
  int i = 0, ri_apn_7 = 0, ri_apn_30000 = 0, ri_ip = 0, ri_rip = 0;
  resource_instance_string_list_t *apn, *ip, *router_ip;
  conn_m_data_t * targetP;

  // this is a single instance object
  if( numDataP == NULL || dataArrayP == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
  if (instanceId != 0)
  {
    return LWM2M_404_NOT_FOUND;
  }

  result = LWM2M_205_CONTENT;
  targetP = (conn_m_data_t*) (objectP->userData);
  conn_monitoring_update_ip_addresses(targetP);
  if(targetP != NULL)
  {
    if(!g_carrier_cfg)
    {
      for (apn = targetP->apn_name_list, ri_apn_7=0;
            apn!=NULL;
            apn = apn->next, ri_apn_7++);
    }
    else
    {
      for (apn = targetP->apnList;apn!=NULL;apn = apn->next)
      {
        if(apn->resInstId == RES_O_APN_CLASS2)
          ri_apn_7++;
        else
          ri_apn_30000++;
      }
    }

    for (ip = targetP->ipAddresses, ri_ip=0;
          ip!=NULL;
          ip = ip->next, ri_ip++);

    for (router_ip= targetP->routerIpAddresses, ri_rip=0;
          router_ip!=NULL;
          router_ip = router_ip->next, ri_rip++);
  }
  // is the server asking for the full object ?
  if (*numDataP == 0)
  {
    uint16_t resList[] = {
      RES_M_NETWORK_BEARER,
      RES_M_AVL_NETWORK_BEARER,
      RES_M_RADIO_SIGNAL_STRENGTH,
      RES_O_LINK_QUALITY,
      RES_M_IP_ADDRESSES,
      RES_O_ROUTER_IP_ADDRESS,
      // As interface is not available , so disabling , once interface available need to enable 
#if 0
      // VZ_REQ_OTADMLWM2M_41068 Requirement 
      RES_O_LINK_UTILIZATION,
#endif            
      RES_O_CELL_ID,
      RES_O_SMNC,
      RES_O_SMCC ,

    };
    int nbRes = sizeof(resList) / sizeof(uint16_t);
    int temp = nbRes;

    if (ri_apn_7 != 0)
      nbRes++;
    if((g_carrier_cfg ) && (ri_apn_30000 != 0 ))
      nbRes++;

    *dataArrayP = lwm2m_data_new(nbRes);
    if (*dataArrayP == NULL) return LWM2M_500_INTERNAL_SERVER_ERROR;

    *numDataP = nbRes;

    for (i = 0; i < temp; i++)
    {
      (*dataArrayP)[i].id = resList[i];
      if((*dataArrayP)[i].id == RES_M_AVL_NETWORK_BEARER )
      {
        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
        if (targetP != NULL)
        {
          (*dataArrayP)[i].value.asChildren.count = targetP->availNetworkBearerLength;
        }
      }
      else if ((*dataArrayP)[i].id == RES_M_IP_ADDRESSES )
      {
        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
        (*dataArrayP)[i].value.asChildren.count = ri_ip;
      }
      else if((*dataArrayP)[i].id == RES_O_ROUTER_IP_ADDRESS)
      {
        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
        (*dataArrayP)[i].value.asChildren.count = ri_rip;
      } 
      else {} 
    }

    if (ri_apn_7 != 0)
    {
      (*dataArrayP)[temp].id = RES_O_APN ;
      (*dataArrayP)[temp].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
      (*dataArrayP)[i].value.asChildren.count = ri_apn_7;
    }
    if ((g_carrier_cfg ) && (ri_apn_30000 != 0 ))
    {
      (*dataArrayP)[temp+1].id = RES_O_APN_30000 ;
      (*dataArrayP)[temp+1].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
      (*dataArrayP)[temp+1].value.asChildren.count = ri_apn_30000;
    }
  }
  else
  {
    for (i = 0; i < *numDataP && result == LWM2M_205_CONTENT; i++)
    {
      switch ((*dataArrayP)[i].id)
      {                
        case RES_M_AVL_NETWORK_BEARER:
        {
          (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
          if (targetP != NULL)
          {
            (*dataArrayP)[i].value.asChildren.count = targetP->availNetworkBearerLength;
          }
          break;
        }
        case RES_M_IP_ADDRESSES:
        {
          (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
          (*dataArrayP)[i].value.asChildren.count = ri_ip;
          break;
        }
        case RES_O_ROUTER_IP_ADDRESS:
        {
          (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
          (*dataArrayP)[i].value.asChildren.count = ri_rip;
          break;
        }
        case RES_O_APN:
          {
            (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
            (*dataArrayP)[i].value.asChildren.count = ri_apn_7;
          break;
          }
        case RES_O_APN_30000:
          {
            if((!g_carrier_cfg) && ((*dataArrayP)[i].id == RES_O_APN_30000)) {
              result = LWM2M_404_NOT_FOUND;
              break;
            }
          (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
          (*dataArrayP)[i].value.asChildren.count = ri_apn_30000;
          break;
          }
        case RES_M_NETWORK_BEARER:
        case RES_M_RADIO_SIGNAL_STRENGTH:
        case RES_O_LINK_QUALITY:             
          // As interface is not available , so disabling , once interface available need to enable 
#if 0  
          //  VZ_REQ_OTADMLWM2M_41068 Requirement           
        case RES_O_LINK_UTILIZATION:
#endif
        case RES_O_CELL_ID:
        case RES_O_SMNC:
        case RES_O_SMCC:break;

        default:
          result = LWM2M_404_NOT_FOUND;
      }
    }
  }

  return result;
}

/**
 * @brief This function is used to write the resources into object instance
 *        while creating the object instance.or got write request from server.

 * @param   objectP (IN)   handle to object
 * @param   instanceP (IN/OUT)   handle to object instance
 * @param   numData (IN)        number of resources to be written
 * @param   dataArray (IN)      array of resource values
 * @param resInstId Resource instance id or INVALID_RES_INST_ID 
 *                  if resource instance id is not passed.
 * @param write_method To distinguish between partial write or replace.
 * @return LWM2M response type
 */

static uint8_t prv_conn_m_write_resources(lwm2m_object_t * objectP,
    int numData,
                                            lwm2m_data_t *dataArray,
    uint16_t resInstId, uint8_t write_method)
{
  int i = 0;
  uint8_t result = 0;
  bool val_change = false;

  if( dataArray == NULL || objectP == NULL ) 
  {
    ds_iot_ulog_err("fun [prv_conn_m_write_resources] LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
  i = 0;

  do
  {
    switch (dataArray[i].id)
    {
      case RES_O_APN_30000:
        {
          if(g_carrier_cfg)
          {
            int ri = 0;
            lwm2m_data_t* subTlvArray = NULL;
            char apn_name[CARRIER_APN_LEN];
            if(resInstId != INVALID_RES_INST_ID)
            {
              ds_iot_ulog_high1("Write on  %d Resource Instance ID", resInstId);
              strlcpy(apn_name, (char*)dataArray[i].value.asBuffer.buffer, dataArray[i].value.asBuffer.length + 1);
              if(resInstId == RES_O_APN_CLASS2)
              {
                return LWM2M_404_NOT_FOUND;
              }
              if (!store_prev_apn_before_update(&(((conn_m_data_t*)(objectP->userData))->apnList),resInstId))
              {
                result = LWM2M_500_INTERNAL_SERVER_ERROR;
                break;
              }
              if (!prv_write_string_resource_inst_val(&(((conn_m_data_t*)(objectP->userData))->apnList),resInstId, apn_name, &val_change))
              {
                result = LWM2M_500_INTERNAL_SERVER_ERROR;
                break;
              }
              else
              {
                if(resInstId == RES_O_APN_CLASS3)
                {
                  ds_iot_ulog_high("Class 3 APN name is changed");
                  //LWM2M_STRCPY( class3_apn, (const char *)dataArray[i].value.asBuffer.buffer, CARRIER_APN_LEN);
                  LWM2M_STRCPY( class3_apn, apn_name, CARRIER_APN_LEN);
                  qurt_signal_set(&lwm2m_signal_Handle, LWM2M_CONN_MONI_SCND_APN_CHANGE);
                }
                if(resInstId == RES_O_APN_CLASS6)
                {
                  ds_iot_ulog_high("Class 6 APN name is changed");
                  //LWM2M_STRCPY( class6_apn, (const char *)dataArray[i].value.asBuffer.buffer, CARRIER_APN_LEN);
                  LWM2M_STRCPY( class6_apn, apn_name, CARRIER_APN_LEN);
                  qurt_signal_set(&lwm2m_signal_Handle, LWM2M_CONN_MONI_CLASS6_APN_CHANGE);
                }
                if(resInstId == RES_O_APN_CLASS7)
                {
                  ds_iot_ulog_high("Class 7 APN name is changed");
                  //LWM2M_STRCPY( class7_apn, (const char *)dataArray[i].value.asBuffer.buffer, CARRIER_APN_LEN);
                  LWM2M_STRCPY( class7_apn, apn_name, CARRIER_APN_LEN);
                  qurt_signal_set(&lwm2m_signal_Handle, LWM2M_CONN_MONI_CLASS7_APN_CHANGE);
                }

                result = LWM2M_204_CHANGED;

              }
            }
            else
            {
              resource_instance_string_list_t* apnListSave = ((conn_m_data_t*)(objectP->userData))->apnList;
              if(dataArray[i].type != QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE)
                return LWM2M_400_BAD_REQUEST;

              if(write_method == QAPI_COAP_PUT)
                ((conn_m_data_t*)(objectP->userData))->apnList = NULL;

              subTlvArray = dataArray[i].value.asChildren.array;

              if (dataArray[i].value.asChildren.count == 0)
              {
                result = LWM2M_400_BAD_REQUEST;
              }
              else if (subTlvArray==NULL)
              {
                result = LWM2M_400_BAD_REQUEST;
              }
              else
              {
                for (ri=0; ri < dataArray[i].value.asChildren.count; ri++)
                {
                  strlcpy(apn_name, (char *)subTlvArray[ri].value.asBuffer.buffer, subTlvArray[ri].value.asBuffer.length + 1);
                  if(subTlvArray[ri].id == RES_O_APN_CLASS2)
                  {
                    return LWM2M_405_METHOD_NOT_ALLOWED;
                  }
                  if (!store_prev_apn_before_update(&(((conn_m_data_t*)(objectP->userData))->apnList),subTlvArray[ri].id))
                  {
                    result = LWM2M_500_INTERNAL_SERVER_ERROR;
                    break;
                  }
                  if (!prv_write_string_resource_inst_val(&(((conn_m_data_t*)(objectP->userData))->apnList),subTlvArray[ri].id, apn_name, &val_change))
                  {
                    result = LWM2M_500_INTERNAL_SERVER_ERROR;
                    break;
                  }
                  else
                  {
                    if(subTlvArray[ri].id == RES_O_APN_CLASS3)
                    {
                   //   LWM2M_STRCPY( class3_apn, (const char *)subTlvArray[ri].value.asBuffer.buffer, CARRIER_APN_LEN);
                      LWM2M_STRCPY( class3_apn, apn_name, CARRIER_APN_LEN);
                      qurt_signal_set(&lwm2m_signal_Handle, LWM2M_CONN_MONI_SCND_APN_CHANGE);
                    }
                    if(subTlvArray[ri].id == RES_O_APN_CLASS6)
                    {
                   //   LWM2M_STRCPY( class6_apn, (const char *)subTlvArray[ri].value.asBuffer.buffer, CARRIER_APN_LEN);
                      LWM2M_STRCPY( class6_apn, apn_name, CARRIER_APN_LEN);
                      qurt_signal_set(&lwm2m_signal_Handle, LWM2M_CONN_MONI_CLASS6_APN_CHANGE);
                    }
                    if(subTlvArray[ri].id == RES_O_APN_CLASS7)
                    {
                   //   LWM2M_STRCPY( class7_apn, (const char *)subTlvArray[ri].value.asBuffer.buffer, CARRIER_APN_LEN);
                      LWM2M_STRCPY( class7_apn, apn_name, CARRIER_APN_LEN);
                      qurt_signal_set(&lwm2m_signal_Handle, LWM2M_CONN_MONI_CLASS7_APN_CHANGE);
                    }
                    val_change = true;
                    result = LWM2M_204_CHANGED;
                  }
                }
              }

              if (result != LWM2M_204_CHANGED)
              {
                // free pot. partial created new ones
                if(write_method == QAPI_COAP_PUT)
                  LWM2M_LIST_FREE(((conn_m_data_t*)(objectP->userData))->apnList);
                // restore old values:
                ((conn_m_data_t*)(objectP->userData))->apnList = apnListSave;
              }
              else
              {
                // final free saved value list
                if(write_method == QAPI_COAP_PUT)
                  LWM2M_LIST_FREE(apnListSave);
              }
            }
          }
          else
            result = LWM2M_405_METHOD_NOT_ALLOWED;
        }
        break;
        case RES_M_NETWORK_BEARER:
        case RES_M_AVL_NETWORK_BEARER:
        case RES_M_RADIO_SIGNAL_STRENGTH:
        case RES_O_LINK_QUALITY:
        case RES_M_IP_ADDRESSES:
        case RES_O_CELL_ID:
        case RES_O_SMNC:
        case RES_O_SMCC:
        case RES_O_APN:
        case RES_O_ROUTER_IP_ADDRESS:
        {
          /*If create/write have one more one resource to write we write on reablable resource*/
          if(numData > 1)
            result = LWM2M_204_CHANGED;
          else
            result = LWM2M_405_METHOD_NOT_ALLOWED;
          break;
        }
      default:
        /*If create/write have one more one resource to write we write on unknown/notsupported resource*/
        if(numData > 1)
          result = LWM2M_204_CHANGED;
        else
          result = LWM2M_404_NOT_FOUND;
    }
    i++;
  } while (i < numData && result == LWM2M_204_CHANGED);

  if ((g_carrier_cfg) && (LWM2M_204_CHANGED == result) && (create_operation == TRUE || val_change == TRUE))
    store_conn_mon_persistent_info(((conn_m_data_t*)(objectP->userData)));
  return result;

}


/**
 * @fn static uint8_t prv_write_conn_m()
 *
 * @brief This fucntion implements the write operation performed 
 *        on Connectivity Monitoring object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numData number of resources in the array
 * @param dataArray resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */
static uint8_t prv_write_conn_m(uint16_t instanceId,
    int numData,
    lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP,
    uint16_t resInstId, uint8_t write_method, bool bs_true)
{
  uint8_t result = LWM2M_205_CONTENT;
  if( dataArray == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("LWM2M_LOG: fun [%s] %s",__func__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  // this is a single instance object
  if (instanceId != 0)
  {
    return LWM2M_404_NOT_FOUND;
  }
  result = prv_conn_m_create_write_check(numData, dataArray, bs_true);
  if (result != LWM2M_204_CHANGED )
  {
    ds_iot_ulog_err ("Write operation failed due to write on Readable resource. ");
    return result;
  }

  result = prv_conn_m_write_resources(objectP, numData, dataArray,
                                        resInstId, write_method);
  return result;
}

/**
 * @fn static uint8_t prv_conn_mon_create()
 *
 * @brief This fucntion is used to create Connectivity Monitoring object
 *
 * @param instanceId Instance ID of the object to be created
 * @param numData number of resources in the array
 * @param dataArray resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */

/***************************************************************************************************

 Under section 8.2.3 Operation on Object of OMA 1.1

- If the �Create� operation is permitted, the LwM2M Client MUST perform the instantiation on the 
Object only if all the mandatory Resources are present in the �New Value� parameter (see Section 5). 
If all the mandatory Resources are not present, the LwM2M Client MUST send a �Bad Request� error code 
to the LwM2M Server


*************************************************************************************************************/

static uint8_t prv_conn_mon_create(uint16_t instanceId,
    int numData,
    lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP, bool apiCall)
{
  conn_m_data_t *connObj = NULL;
  uint8_t result = 0;
  uint16_t mandatory_resource[] = {RES_M_NETWORK_BEARER, RES_M_AVL_NETWORK_BEARER, RES_M_RADIO_SIGNAL_STRENGTH, RES_M_IP_ADDRESSES};
  qapi_Device_Info_t info;
  int64_t temp = 0;
  bool val_change = false;
  uint8_t size = sizeof(mandatory_resource)/sizeof(mandatory_resource[0]);
  if( dataArray == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  /*check if already one instance is created */
  if(objectP->instanceList != NULL)
  {
    ds_iot_ulog_err("Connectivity Monitoring object instance exist");
    return LWM2M_400_BAD_REQUEST;
  }
  if(apiCall == TRUE)
  {
     goto skip_check;
  }

  result = prv_mandatory_check(numData, dataArray, mandatory_resource, size);
  if(result != LWM2M_NO_ERROR)
    return result;
  result = prv_conn_m_create_write_check(numData, dataArray, TRUE);
  if (result != LWM2M_204_CHANGED )
  {
    ds_iot_ulog_err ("Creating instance failed");
    return result;
  }
skip_check:

  connObj = (conn_m_data_t*) lwm2m_calloc(1, sizeof(conn_m_data_t));
  if( NULL == connObj)
  {
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  memset(connObj, 0, sizeof(conn_m_data_t));
  objectP->userData = connObj;


  memset(&info, 0x00, sizeof(qapi_Device_Info_t));
  if(qapi_Device_Info_Get_v2(get_lwm2m_dev_info_hdl(),QAPI_DEVICE_INFO_4G_SIG_STRENGTH_E, &info)!=QAPI_OK)	    
  {
    LOG_DEBUG("Signal Strength hard coded to zero");
    connObj->signalStrength  = 0;
  }
  else
  	connObj->signalStrength = info.u.valueint;

  
  memset(&info, 0x00, sizeof(qapi_Device_Info_t));
  if(qapi_Device_Info_Get_v2(get_lwm2m_dev_info_hdl(),QAPI_DEVICE_INFO_LINK_QUALITY_E, &info)!=QAPI_OK)	    
  {
    LOG_DEBUG("Link Quality hard coded to zero");
    connObj->linkQuality     = 0;
  }
  else
  	connObj->linkQuality = info.u.valueint;


  
  memset(&info, 0x00, sizeof(qapi_Device_Info_t));
  if(qapi_Device_Info_Get_v2(get_lwm2m_dev_info_hdl(),QAPI_DEVICE_INFO_CELL_ID_E, &info)!=QAPI_OK)	
  {     
    LOG_DEBUG("Cell Id hard coded to zero");
    connObj->cellId = 0;
  }
  else
	connObj->cellId = info.u.valueint;

  memset(&info, 0x00, sizeof(qapi_Device_Info_t));
  temp =0;
  if((qapi_Device_Info_Get_v2(get_lwm2m_dev_info_hdl(),QAPI_DEVICE_INFO_MCC_E, &info)!=QAPI_OK)|| 	
	!utils_plainTextToInt64((uint8_t*)info.u.valuebuf.buf, info.u.valuebuf.len, &temp))
  {
    LOG_DEBUG("Country code hard coded to zero");
    connObj->mcc = 0;
  }
  else
  	connObj->mcc = (int32_t)temp;
  
  
  memset(&info, 0x00, sizeof(qapi_Device_Info_t));
  temp =0;

  if((qapi_Device_Info_Get_v2(get_lwm2m_dev_info_hdl(),QAPI_DEVICE_INFO_MNC_E, &info)!=QAPI_OK)|| 	
	!utils_plainTextToInt64((uint8_t*)info.u.valuebuf.buf, info.u.valuebuf.len, &temp))
  {
    LOG_DEBUG("Network code hard coded to zero");
    connObj->mnc = 0;
  } 
  else
  	connObj->mnc = (int32_t)temp;

  if(g_carrier_cfg)
  {
    prv_write_string_resource_inst_val(&(connObj->apnList),RES_O_APN_CLASS3, CLASS3_APN, &val_change);
    prv_write_string_resource_inst_val(&(connObj->apnList),RES_O_APN_CLASS6, CLASS6_APN, &val_change);
	prv_write_string_resource_inst_val(&(connObj->apnList),RES_O_APN_CLASS7, CLASS7_APN, &val_change);
    create_operation = LWM2M_TRUE;
    result = prv_conn_m_write_resources(objectP, numData, dataArray, 
                                          INVALID_RES_INST_ID, QAPI_COAP_POST);
    create_operation = LWM2M_FALSE;
    if (result != LWM2M_204_CHANGED)
    {
      // Issue in creating object instanace so retuning Bad request error.
      result = LWM2M_400_BAD_REQUEST;
      objectP->userData = NULL;
      lwm2m_free(connObj);
      return result;
    }
  }

  objectP->instanceList = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));
  if (NULL != objectP->instanceList)
  {
    memset(objectP->instanceList, 0x00, sizeof(lwm2m_list_t));
  }

  efs_unlink(LWM2M_CONN_MON_OBJ_PERSISTENCE_PATH);
  return LWM2M_201_CREATED;

}
/**
 * @fn static uint8_t prv_conn_mon_delete()
 *
 * @brief This fucntion is used to delete Connectivity Monitoring object
 *
 * @param id Instance ID of the object to be deleted
 * @param objectP Object to be deleted
 *
 * @return LWM2M response type
 */
static uint8_t prv_conn_mon_delete(uint16_t id,
    lwm2m_object_t * objectP,
    bool persist_file)
{
  int fd = 0;
  conn_m_data_t * connObj = NULL;

  if( objectP == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&connObj);
  if (NULL == connObj) return LWM2M_404_NOT_FOUND;

 /*  Update single instance object persistence */

  lwm2m_free(connObj);
  connObj = NULL;

  if(NULL != objectP->userData)
  {
    connObj = (conn_m_data_t*)objectP->userData;
    
    LWM2M_LIST_FREE(connObj->ipAddresses);
    LWM2M_LIST_FREE(connObj->routerIpAddresses);
    LWM2M_LIST_FREE(connObj->apnList);
    LWM2M_LIST_FREE(connObj->apn_name_list);

    lwm2m_free(objectP->userData);
    objectP->userData = NULL;
  }

    if(persist_file){
    ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_CONN_MON_OBJ_PERSISTENCE_PATH);
	fd = efs_open( LWM2M_CONN_MON_OBJ_PERSISTENCE_PATH, O_CREAT, 00744);

  	if( EFAILURE == fd)
  	{
    	LOG_DEBUG("Failed to open single_instance_obj_pr");
    	return LWM2M_500_INTERNAL_SERVER_ERROR;
  	}
  	efs_close(fd);
  }

  return LWM2M_202_DELETED;

}

/**
 * @fn lwm2m_object_t * get_object_conn_m()
 *
 * @brief This function is used to get the information about the Connectivity
 * monitoring object during client registartion
 *
 * @param void
 *
 * @return Connectivity monitoring object  
 *
 */
lwm2m_object_t * get_object_conn_m(void)
{
  /*
   * The get_object_conn_m() function create the object itself and return a pointer to the structure that represent it.
   */
  lwm2m_object_t * connObj = NULL;
  int fd = 0;
  bool val_change = false;
  connObj = (lwm2m_object_t *) lwm2m_malloc(sizeof(lwm2m_object_t));
  if (NULL != connObj)
  {
    memset(connObj, 0, sizeof(lwm2m_object_t));

    /*
     * It assigns his unique ID
     */
    connObj->objID = LWM2M_CONN_MONITOR_OBJECT_ID;

    /*
     * And the private function that will access the object.
     * Those function will be called when a read/write/execute query is made by the server. In fact the library don't need to
     * know the resources of the object, only the server does.
     */
    connObj->readFunc     = prv_conn_mon_read;
    connObj->discoverFunc = prv_conn_mon_discover;
    connObj->executeFunc  = NULL;
    connObj->createFunc   = prv_conn_mon_create;
    connObj->deleteFunc   = prv_conn_mon_delete;
    if(g_carrier_cfg)
      connObj->writeFunc = prv_write_conn_m;

    /*  Check if host_device_obj_single_instance_pr file exist */
    ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_CONN_MON_OBJ_PERSISTENCE_PATH);
    fd = efs_open(LWM2M_CONN_MON_OBJ_PERSISTENCE_PATH, O_RDONLY);
    if(fd != EFAILURE)
    {
      efs_close(fd);
      return connObj;
    }

    /*
     * and its unique instance
     *
     */
    connObj->instanceList = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));
    if (NULL != connObj->instanceList)
    {
      memset(connObj->instanceList, 0, sizeof(lwm2m_list_t));
    }
    else
    {
      lwm2m_free(connObj);
      return NULL;
    }


    connObj->userData = lwm2m_malloc(sizeof(conn_m_data_t));
    if (connObj->userData == NULL)
      return NULL;
    memset(connObj->userData, 0, sizeof(conn_m_data_t));

    /*
     * Also some user data can be stored in the object with a private structure containing the needed variables
     */
    if (NULL != connObj->userData)
    {
      conn_m_data_t *myData = (conn_m_data_t*) connObj->userData;
      
	  qapi_Device_Info_t info;
	  int64_t temp = 0;
	  
		memset(&info, 0x00, sizeof(qapi_Device_Info_t));
		if(qapi_Device_Info_Get_v2(get_lwm2m_dev_info_hdl(),QAPI_DEVICE_INFO_4G_SIG_STRENGTH_E, &info)!=QAPI_OK)	  
      {
        LOG_DEBUG("Signal Strength hard coded to zero");
        myData->signalStrength  = 0;
      }
		else
		  myData->signalStrength = info.u.valueint;
	  
		
		memset(&info, 0x00, sizeof(qapi_Device_Info_t));
		if(qapi_Device_Info_Get_v2(get_lwm2m_dev_info_hdl(),QAPI_DEVICE_INFO_LINK_QUALITY_E, &info)!=QAPI_OK)	  
      {
        LOG_DEBUG("Link Quality hard coded to zero");
        myData->linkQuality     = 0;
      }
		else
		  myData->linkQuality = info.u.valueint;
	  
	  
		
		memset(&info, 0x00, sizeof(qapi_Device_Info_t));
		if(qapi_Device_Info_Get_v2(get_lwm2m_dev_info_hdl(),QAPI_DEVICE_INFO_CELL_ID_E, &info)!=QAPI_OK)  
      {     
        LOG_DEBUG("Cell Id hard coded to zero");
        myData->cellId = 0;
      }
		else
		  myData->cellId = info.u.valueint;
	  
		memset(&info, 0x00, sizeof(qapi_Device_Info_t));
		temp =0;
		if((qapi_Device_Info_Get_v2(get_lwm2m_dev_info_hdl(),QAPI_DEVICE_INFO_MCC_E, &info)!=QAPI_OK)||   
		  !utils_plainTextToInt64((uint8_t*)info.u.valuebuf.buf, info.u.valuebuf.len, &temp))
      {
        LOG_DEBUG("Country code hard coded to zero");
        myData->mcc = 0;
      }
		else
		  myData->mcc = (int32_t)temp;
		
		
		memset(&info, 0x00, sizeof(qapi_Device_Info_t));
		temp =0;
	  
		if((qapi_Device_Info_Get_v2(get_lwm2m_dev_info_hdl(),QAPI_DEVICE_INFO_MNC_E, &info)!=QAPI_OK)||   
		  !utils_plainTextToInt64((uint8_t*)info.u.valuebuf.buf, info.u.valuebuf.len, &temp))
      {
        LOG_DEBUG("Network code hard coded to zero");
        myData->mnc = 0;
      } 
		else
		  myData->mnc = (int32_t)temp;
      
      if(g_carrier_cfg)
      {
        prv_write_string_resource_inst_val(&(myData->apnList),RES_O_APN_CLASS3, CLASS3_APN, &val_change);
		prv_write_string_resource_inst_val(&(myData->apnList),RES_O_APN_CLASS6, CLASS6_APN, &val_change);
		prv_write_string_resource_inst_val(&(myData->apnList),RES_O_APN_CLASS7, CLASS7_APN, &val_change);
      }
    }
    else
    {
      lwm2m_free(connObj);
      connObj = NULL;
    }
  }
  return connObj;
}

/**
 * @fn void free_object_conn_m()
 *
 * @brief This fucntion frees the Connectivity monitoring object allocated
 * @param ObjectP Object to free
 *
 * @return void
 */
void free_object_conn_m(lwm2m_object_t * objectP)
{
  lwm2m_list_t* objlist;

  if (objectP == NULL) return;

  objlist = objectP->instanceList;
  while(objlist)
  {
    prv_conn_mon_delete(objlist->id,objectP,false);
    objlist = objectP->instanceList;
  }

    lwm2m_free(objectP);
}

/**
 * @fn uint8_t connectivity_moni_change()
 *
 * @brief This fucntion changes the Connectivity monitoring object 
 *        resource values
 * @param dataArray resource value array
 * @param ObjectP Object to change
 *
 * @return void
 */
uint8_t connectivity_moni_change(lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP)
{
  int64_t value = 0;
  uint8_t result = 0;
  conn_m_data_t * data = NULL;

  if (dataArray == NULL || objectP == NULL)
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  data = (conn_m_data_t*) (objectP->userData);

  switch (dataArray->id)
  {
    case RES_M_RADIO_SIGNAL_STRENGTH:
      if (1 == lwm2m_data_decode_int(dataArray, &value))
      {
        data->signalStrength = value;
        result = LWM2M_204_CHANGED;
      }
      else
      {
        result = LWM2M_400_BAD_REQUEST;
      }
      break;

    case RES_O_LINK_QUALITY:
      if (1 == lwm2m_data_decode_int(dataArray, &value))
      {
        data->linkQuality = value;
        result = LWM2M_204_CHANGED;
      }
      else
      {
        result = LWM2M_400_BAD_REQUEST;
      }
      break;

    case RES_O_CELL_ID:
      if (1 == lwm2m_data_decode_int(dataArray, &value))
      {
        data->cellId = value;
        result = LWM2M_204_CHANGED;
      }
      else
      {
        result = LWM2M_400_BAD_REQUEST;
      }
      break;

    default:
      result = LWM2M_405_METHOD_NOT_ALLOWED;
  }

  return result;
}

int util_carrier_apn_info_to_conn_m_obj(lwm2m_context_t * context, bool update)
{
  lwm2m_object_t * targetP = NULL;
  carrier_apn_cfg_t  *carrier_apn_cfg = NULL;
  carrier_apn_cfg_t  *bs_apn_cfg = NULL;
  conn_m_data_t *myData = NULL;
  bool val_change = false;

  if (context == NULL) {
    return EFAILURE;
  }
  targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_CONN_MONITOR_OBJECT_ID);
  if (targetP == NULL) 
  {
    return EFAILURE;
  }
  if (NULL != targetP->userData)
  {
    myData = (conn_m_data_t*) targetP->userData;

    if(update == FALSE)
    {
      carrier_apn_cfg = context->c_apn_cfg;
    }
    else
    {
      carrier_apn_cfg = context->apn_cfg;
    LWM2M_LIST_FREE(myData->apn_name_list);
    myData->apn_name_list = NULL;
    bs_apn_cfg = context->c_apn_cfg;
    while(bs_apn_cfg)
    {
      if(bs_apn_cfg->bootstrap_apn == TRUE)
      {
        if(g_carrier_cfg)
        {
          if (bs_apn_cfg->apn_class == LWM2M_APN_CLASS2)
          {
              prv_write_string_resource_inst_val(&(myData->apnList),RES_O_APN_CLASS2, bs_apn_cfg->apn_name, &val_change);
          } 
          else if (bs_apn_cfg->apn_class == LWM2M_APN_CLASS3) 
          {
              prv_write_string_resource_inst_val(&(myData->apnList),RES_O_APN_CLASS3, bs_apn_cfg->apn_name, &val_change);
          }
          else if (bs_apn_cfg->apn_class == LWM2M_APN_CLASS6) 
          {
              prv_write_string_resource_inst_val(&(myData->apnList),RES_O_APN_CLASS6, bs_apn_cfg->apn_name, &val_change);
          }
          else if (bs_apn_cfg->apn_class == LWM2M_APN_CLASS7) 
          {
              prv_write_string_resource_inst_val(&(myData->apnList),RES_O_APN_CLASS7, bs_apn_cfg->apn_name, &val_change);
          }
        }
        else
        {
          prv_write_string_resource_val(&(myData->apn_name_list), bs_apn_cfg->apn_name);
        }
        break;
      }
      bs_apn_cfg = bs_apn_cfg->next;
    }
    }

    if (carrier_apn_cfg != NULL) 
    {
       ds_iot_ulog_err("carrier_apn Not NULL");
       while (carrier_apn_cfg != NULL) 
       {
        if(g_carrier_cfg)
        {
          ds_iot_log_strf_err("apn name %s apn_class %d", carrier_apn_cfg->apn_name, carrier_apn_cfg->apn_class);
          if (carrier_apn_cfg->apn_class == LWM2M_APN_CLASS2)
          {
            prv_write_string_resource_inst_val(&(myData->apnList),RES_O_APN_CLASS2, carrier_apn_cfg->apn_name, &val_change);
          } 
          else if (carrier_apn_cfg->apn_class == LWM2M_APN_CLASS3) 
          {
            prv_write_string_resource_inst_val(&(myData->apnList),RES_O_APN_CLASS3, carrier_apn_cfg->apn_name, &val_change);
          }
          else if (carrier_apn_cfg->apn_class == LWM2M_APN_CLASS6) 
          {
            prv_write_string_resource_inst_val(&(myData->apnList),RES_O_APN_CLASS6, carrier_apn_cfg->apn_name, &val_change);
          }
          else if (carrier_apn_cfg->apn_class == LWM2M_APN_CLASS7) 
          {
            prv_write_string_resource_inst_val(&(myData->apnList),RES_O_APN_CLASS7, carrier_apn_cfg->apn_name, &val_change);
          }
        }
        else
        {
          /* For non Verizon cases store all the APN's */
          prv_write_string_resource_val(&(myData->apn_name_list), carrier_apn_cfg->apn_name);
        }
        carrier_apn_cfg = carrier_apn_cfg->next ;
       }
      } 
      else 
      {
        return EFAILURE;
      }
    } else {
    return EFAILURE;
  }
  return ESUCCESS;
}

/** 
 * @fn     int update_carrier_apn_info_to_conn_m_obj()
 * @brief  This function is used to update the active apn  into connectivity moni objcet 
 * @param  context    - lwm2m context handle 
 * @return on success - 0
 *         on error   - -1
 */
int update_carrier_apn_info_to_conn_m_obj(lwm2m_context_t * context)
{
  return util_carrier_apn_info_to_conn_m_obj(context, TRUE);
}

/** 
 * @fn     int populate_carrier_apn_info_to_conn_m_obj()
 * @brief  This function is used to populate the carrier apn config into connectivity moni objcet 
 * @param  context    - lwm2m context handle 
 * @return on success - 0
 *         on error   - -1
 */
int populate_carrier_apn_info_to_conn_m_obj(lwm2m_context_t * context)
{
  return util_carrier_apn_info_to_conn_m_obj(context, FALSE);
}

/** 
 * @fn     m2m_set_network_bearer_to_conn_m_obj()
 * @brief  This function is used to fetch network bearer and, available network bearer
 *         values from QMI
 * @param  context    - lwm2m context handle 
 * @return on success - ESUCCESS
 *         on error   - EFAILURE
 */
int m2m_set_network_bearer_to_conn_m_obj(lwm2m_context_t * context)
{
  lwm2m_object_t * targetP = NULL; 
  int result = EFAILURE;
  if (context == NULL)
  {
    return EFAILURE;
  }
  targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(context->objectList, LWM2M_CONN_MONITOR_OBJECT_ID);
  if (targetP == NULL) 
  {
    return EFAILURE;
  }
  if (NULL != targetP->userData)
  {
    #if 0
	qapi_Device_Info_t info ;
	memset(&info, 0x00, sizeof(qapi_Device_Info_t));
	if(qapi_Device_Info_Get_v2(get_lwm2m_dev_info_hdl(),QAPI_DEVICE_INFO_NETWORK_BEARER_E, &info)!=QAPI_OK)		
		return LWM2M_500_INTERNAL_SERVER_ERROR;
			
	((conn_m_data_t*) targetP->userData)->networkBearer = info.u.valueint;
	
	if(((conn_m_data_t*) (targetP->userData))->networkBearer == IOTAPP_GSM)
		((conn_m_data_t*) (targetP->userData))->networkBearer = GSM_NW_BEARER;
	else if(((conn_m_data_t*) (targetP->userData))->networkBearer == IOTAPP_WCDMA)
		((conn_m_data_t*) (targetP->userData))->networkBearer = WCDMA_NW_BEARER;
	
    if(srvc_type.network_type== IOTAPP_LTE)
    {
      if(iotapp_qmi_get_service_status(false) == TRUE)
      {
        if(srvc_type.lte_mode == IOTAPP_LTE_NB_IOT)
        {
          ((conn_m_data_t*) (targetP->userData))->networkBearer = IOTAPP_LTE_NB_IOT;
        }
      }
    }
    #endif
    result = m2m_get_nw_bearer((conn_m_data_t*) (targetP->userData));
    if(result != LWM2M_205_CONTENT)
      return EFAILURE;
  }
  return ESUCCESS;
}
#else

int load_conn_mon_persistent_info(lwm2m_context_t * context )
{
    return EFAILURE;
}
int populate_carrier_apn_info_to_conn_m_obj(lwm2m_context_t * context)
{
		return EFAILURE;
}
int update_carrier_apn_info_to_conn_m_obj(lwm2m_context_t * context)
{
	return EFAILURE;
}

int m2m_set_network_bearer_to_conn_m_obj(lwm2m_context_t * context)
{
	return EFAILURE;
}

#endif

