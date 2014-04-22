/******************************************************************************

  @file    object_connectivity_stat.c

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

/*******************************************************************************
 *
 * Copyright (c) 2015 Bosch Software Innovations GmbH Germany.
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
 *    
 *******************************************************************************/

/*
 * This connectivity statistics object is optional and single instance only
 * 
 *  Resources:
 *
 *          Name         | ID | Oper. | Inst. | Mand.|  Type   | Range | Units | Descripton |
 *  SMS Tx Counter       |  0 |   R   | Single|  No  | Integer |       |       |            |
 *  SMS Rx Counter       |  1 |   R   | Single|  No  | Integer |       |       |            |
 *  Tx Data              |  2 |   R   | Single|  No  | Integer |       | kByte |            |
 *  Rx Data              |  3 |   R   | Single|  No  | Integer |       | kByte |            |
 *  Max Message Size     |  4 |   R   | Single|  No  | Integer |       | Byte  |            |
 *  Average Message Size |  5 |   R   | Single|  No  | Integer |       | Byte  |            |
 *  StartOrReset         |  6 |   E   | Single|  Yes | Integer |       |       |            |
 */

#include "liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "qapi_dss.h"
#include "iotapp_dss_util.h"
#include "qapi_coap.h"
#include "ds_log.h"


#ifdef ENABLE_CONN_STAT_OBJ

// Resource Id's:
#define RES_O_SMS_TX_COUNTER            0
#define RES_O_SMS_RX_COUNTER            1
#define RES_O_TX_DATA                   2
#define RES_O_RX_DATA                   3
#define RES_O_MAX_MESSAGE_SIZE          4
#define RES_O_AVERAGE_MESSAGE_SIZE      5
#define RES_M_START                     6
#ifdef CONNECTIVITY_STATS_ENHANCEMENT
#define RES_M_STOP            7
#define RES_O_COLLECTION_PERIOD         8
#endif
extern qapi_DSS_Hndl_t m2m_call_hndl;
extern bool g_carrier_cfg;

typedef struct conn_s_data_
{
  uint16_t instanceId;
#if 0 //To be enabled when interface is available 
  int64_t   smsTxCounter;
  int64_t   smsRxCounter;
  int64_t   maxMessageSize;
#endif  
  int64_t   prvTxDataByte;
  int64_t   txDataByte;             // report in kByte!
  int64_t   prvRxDataByte;
  int64_t   rxDataByte;             // report in kByte!
  int64_t   avrMessageSize;
  int64_t   prvMessageCount;
  int64_t   messageCount;           // private for incremental average calc.
  time_t    collectionPeriod;
  bool      collectDataStarted;
  bool      collectDataStopped;
} conn_s_data_t;

typedef struct conn_stats_persistent_info_s
{
  time_t    collectionPeriod;
} conn_stats_persistent_info_t;

static timer_type collPeriodTimer;
static bool collectDataStopped;
static bool resetCollection;
bool conn_stat_create = FALSE;
extern timer_group_type lwm2mTimerGroup;
static bool create_operation = LWM2M_FALSE;
void free_object_conn_s(lwm2m_object_t * objectP);

/** 
 * @fn     store_conn_stats_persistent_info()
 * @brief  This function is used to dump resource value set for Connectivity stats
           into persistence file
 * @param  connDataP - pointer to connectivity stats object
 * @return on success  - ESUCCESS
           on error    - EFAILURE
 */ 
int store_conn_stats_persistent_info(conn_s_data_t* connDataP)
{
  int fd = 0;
  conn_stats_persistent_info_t info;

  if(NULL == connDataP)
  {
    LOG_DEBUG("%s",nullError);
    return EFAILURE;
  }
  ds_iot_log_strf_med("EFS_OPT: File open %s",CONN_STATS_PERSISTENCE_FILE);
  fd = efs_open(CONN_STATS_PERSISTENCE_FILE, O_CREAT | O_WRONLY, 00644);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open connectivity stats persistence file");
    return EFAILURE;
  }

  memset(&info, 0x00, sizeof(info));

  info.collectionPeriod = connDataP->collectionPeriod;
  ds_iot_log_strf_med("EFS_OPT: File write %s",CONN_STATS_PERSISTENCE_FILE);
  if (EFAILURE == efs_write(fd, &info, sizeof(info)))
  {
    ds_iot_ulog_err("failed to store persistent value of connectivity stats instance ");
    efs_close(fd);
    return EFAILURE;
  }
  efs_close(fd);
  return ESUCCESS;
}

/** 
 * @fn     load_conn_stats_persistent_info()
 * @brief  This function is used to restore resource values of Connectivity
           stats object from persistence file
 * @param  context - pointer to lwm2m context
 * @return on success  - ESUCCESS
           on error    - EFAILURE
 */ 
int load_conn_stats_persistent_info(conn_s_data_t* connDataP)
{
  int fd = 0, ret = 0;
  conn_stats_persistent_info_t info;

  if(NULL == connDataP)
  {
    LOG_DEBUG("%s",nullError);
    return EFAILURE;
  }
  ds_iot_log_strf_med("EFS_OPT: File open %s",CONN_STATS_PERSISTENCE_FILE);
  fd = efs_open(CONN_STATS_PERSISTENCE_FILE, 00);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open connectivity stats persistence file");
    return EFAILURE;
  }

  memset(&info, 0x00, sizeof(info));

  ret = efs_read(fd, &info, sizeof(info));
  if (ret <= 0)
  {
    efs_close(fd);
    return EFAILURE;
  }

  // Restoring the resource values into connectivity stats structure
  connDataP->collectionPeriod = info.collectionPeriod;

  efs_close(fd);
  return ESUCCESS;
}

/** 
* @fn static uint8_t prv_conn_s_create_write_check() 
*
* @brief This fucntion verifies and write the resource's into  
*        Connectivity statistics object 
*
* @param instanceId Instance of the Connectivity statistics to be create/write 
* @param numDataP number of resources in the array 
* @param dataArrayP resource value array 
* @param create_flag - if true creating instance else write on instance 
*
* @return LWM2M response type 
*
*/

/***************************************************************************************************

Under section 8.2.2 Operation on Object Instance of OMA 1.1

- For the "Write"operation on an Object Instance, the LwM2M Client MUST perform the operation, 
if all the Resources conveyed in the operation are allowed to perform the "Write" operation. 
If any Resource does not support the "Write" operation, the LwM2M Client MUST inform the LwM2M 
Server that the Object Instance cannot perform the requested "Write" operation in sending a 
"Operation is not supported" error code.

*************************************************************************************************************/     
static uint8_t prv_conn_s_create_write_check(int numData,
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
   
  do
  {
    switch(dataArray[i].id)
    {
      case RES_M_START:
      case RES_M_STOP:
      case RES_O_RX_DATA:
      case RES_O_TX_DATA:
      case RES_O_AVERAGE_MESSAGE_SIZE:
      if (!create_flag)
      {
         result = LWM2M_405_METHOD_NOT_ALLOWED;
         break;
      }
      case RES_O_COLLECTION_PERIOD:
         result = LWM2M_204_CHANGED;
         break;
      default:
         if(!create_flag)
		 {        
		    result = LWM2M_404_NOT_FOUND;        
			break;	   	 
		  }      
		  else      
		  {        
		     result = LWM2M_204_CHANGED;	  
		  }
    }
  }while(++i < numData && result == LWM2M_204_CHANGED);

  if(result != LWM2M_204_CHANGED)
  {
     ds_iot_ulog_high1("failed for resource id %d",dataArray[i-1].id);
     return result;
  }
   return result;
}




/**
 * @fn static uint8_t prv_set_tlv()
 *
 * @brief This fucntion is used to populate the values of the resource
 *        to be provided to the server during the read operation on the object.
 * @param dataP data array for the resource 
 * @param connStDataP connectivity statistics object data
 * @return LWM2M response type
 */
static uint8_t prv_set_tlv(lwm2m_data_t * dataP, conn_s_data_t * connStDataP)
{
  qapi_DSS_Data_Pkt_Stats_t  packet_stats;
  int64_t txDataByte = 0, rxDataByte = 0;
  memset(&packet_stats, 0, sizeof(packet_stats));

  if(dataP->id != RES_O_COLLECTION_PERIOD)
  {
    // Do not fetch value and send until collection is started by server
    if (connStDataP->collectDataStarted) 
    {
      if(EFAILURE == iotapp_dss_get_packetstats(&packet_stats))
      {
        ds_iot_ulog_err("Failed to get Packet Stats");
        return LWM2M_500_INTERNAL_SERVER_ERROR;
      }
      // Do not fetch value and send after collection is stopped
      if (!collectDataStopped)
      {
        txDataByte = packet_stats.bytes_tx - connStDataP->prvTxDataByte;
        connStDataP->txDataByte = txDataByte;
  
        ds_iot_ulog_high2("txDataByte: %lld,  bytes_tx: %lld",
            connStDataP->txDataByte, packet_stats.bytes_tx );
  
        rxDataByte = packet_stats.bytes_rx - connStDataP->prvRxDataByte;
        connStDataP->rxDataByte = rxDataByte;
  
        ds_iot_ulog_high2("rxDataByte: %lld   bytes_rx: %lld",
            connStDataP->rxDataByte, packet_stats.bytes_rx);
  
        connStDataP->messageCount = packet_stats.pkts_rx + packet_stats.pkts_tx ;
  
        ds_iot_ulog_high4("messageCount : %lld pkts_rx :%u  pkts_tx: %u  prvMessageCount: %lld",
            connStDataP->messageCount, packet_stats.pkts_rx, packet_stats.pkts_tx, 
            connStDataP->prvMessageCount);
  
        if (connStDataP->messageCount)
        {
          connStDataP->avrMessageSize = (txDataByte + rxDataByte) / (connStDataP->messageCount - connStDataP->prvMessageCount);
          ds_iot_ulog_high1("avrMessageSize: %lld ", connStDataP->avrMessageSize);
        }
      }
      else{
        ds_iot_ulog_err("collectDataStopped is true");
      }
    }
    else{
      ds_iot_ulog_err("collectDataStarted is false");
    }
  }

  switch (dataP->id) 
  {
    // As interface is not available , so disabling , once interface available need to enable 
#if 0
    //  VZ_REQ_OTADMLWM2M_41068 Requirement   
    case RES_O_SMS_TX_COUNTER:
      lwm2m_data_encode_int(connStDataP->smsTxCounter, dataP);
      return LWM2M_205_CONTENT;
      //commented the break to address the dead code, as this case is currently not supported 
      //break; 
    case RES_O_SMS_RX_COUNTER:
      lwm2m_data_encode_int(connStDataP->smsRxCounter, dataP);
      return LWM2M_205_CONTENT;
      // commented the break to address the dead code, as this case is currently not supported 
      //break;
#endif 
    case RES_O_TX_DATA:
      lwm2m_data_encode_int(connStDataP->txDataByte/1024, dataP);
      return LWM2M_205_CONTENT;
      // commented the break to address the dead code, as this case is currently not supported 
      //break;
    case RES_O_RX_DATA:
      lwm2m_data_encode_int(connStDataP->rxDataByte/1024, dataP);
      return LWM2M_205_CONTENT;
      // commented the break to address the dead code, as this case is currently not supported 
      // break
      // As interface is not available , so disabling , once interface available need to enable
    case RES_O_AVERAGE_MESSAGE_SIZE:
      lwm2m_data_encode_int(connStDataP->avrMessageSize, dataP);
      return LWM2M_205_CONTENT;
      // commented the break to address the dead code, as this case is currently not supported 
      // break;
    case RES_O_COLLECTION_PERIOD:
      lwm2m_data_encode_int(connStDataP->collectionPeriod, dataP);
      return LWM2M_205_CONTENT;
   case RES_M_START :
   case RES_M_STOP  :
      return LWM2M_405_METHOD_NOT_ALLOWED;
    default:
      return LWM2M_404_NOT_FOUND ;
  }
}

/**
 * @fn static void prv_resetCounter()
 *
 * @brief This function is used to reset counter
 *
 * @param objectP handle to connectivity statistics Object
 * @param start bool to indicate start/stop data collection
 *
 * @return void
 */
static void prv_resetCounter(lwm2m_object_t* objectP, bool start)
{
  conn_s_data_t *myData = (conn_s_data_t*) objectP->userData;
  qapi_DSS_Data_Pkt_Stats_t local_stats;

  if (NULL == myData)
  {
    LOG_DEBUG("%s",nullError);
    return ;
  }

  ds_iot_ulog_high("Resetting counters");

  memset(&local_stats, 0x00 , sizeof(qapi_DSS_Data_Pkt_Stats_t));

  if(EFAILURE == iotapp_dss_get_packetstats(&local_stats))
  {
    LOG_DEBUG("Failed to get Packet Stats");
    return ;
  }

  if(start) //When start is executed
  {
    ds_iot_ulog_high("Started to collect pkt stats");
#if 0    //To be enabled when interafce is available
    myData->smsTxCounter        = 0;
    myData->smsRxCounter        = 0;
    myData->maxMessageSize      = 0;
#endif    
    myData->prvTxDataByte       = local_stats.bytes_tx;
    myData->txDataByte          = 0;
    myData->prvRxDataByte       = local_stats.bytes_rx;
    myData->rxDataByte          = 0;
    myData->avrMessageSize      = 0;
    myData->prvMessageCount     = local_stats.pkts_tx + local_stats.pkts_rx;
    myData->messageCount        = 0;
    myData->collectDataStarted  = start;
    resetCollection = LWM2M_FALSE;
  }
  else // When reset is executed
  {
    ds_iot_ulog_high("Stop to collect pkt stats and reset");
#if 0   // To be enabled when interface is available 
    myData->smsTxCounter        = 0;
    myData->smsRxCounter        = 0;
    myData->maxMessageSize      = 0;
#endif    
    myData->prvTxDataByte       = 0;
    myData->txDataByte          = 0;
    myData->prvRxDataByte       = 0;
    myData->rxDataByte          = 0;
    myData->avrMessageSize      = 0;
    myData->prvMessageCount     = 0;
    myData->messageCount        = 0;  
    myData->collectDataStarted  = start;
  }


}

/**
 * @fn static uint8_t prv_conn_s_read()
 *
 * @brief This function implements the read operation performed on Connectivity
 * statistics object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numDataP number of resources in the array
 * @param dataArrayP resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */
static uint8_t prv_conn_s_read(uint16_t instanceId, int * numDataP, 
    lwm2m_data_t** dataArrayP, lwm2m_object_t * objectP, uint16_t resInstId, lwm2m_server_t *serverP)
{
  uint8_t result = 0;
  int i = 0;

  if( numDataP == NULL || dataArrayP == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  // this is a single instance object
  if (instanceId != 0)
  {
    return LWM2M_404_NOT_FOUND ;
  }

  if (resetCollection)
    prv_resetCounter(objectP, ((conn_s_data_t *)objectP->userData)->collectDataStarted);

  // is the server asking for the full object ?
  if (*numDataP == 0)
  {
    uint16_t resList[] = {
      RES_O_TX_DATA,
      RES_O_RX_DATA,
      RES_O_AVERAGE_MESSAGE_SIZE,
      // As interface is not available , so disabling , once interface available need to enable 
#if 0
      //  VZ_REQ_OTADMLWM2M_41068 Requirement            
      RES_O_SMS_TX_COUNTER,
      RES_O_SMS_RX_COUNTER,
      //  VZ_REQ_OTADMLWM2M_41068 Requirement 
      RES_O_MAX_MESSAGE_SIZE,
#endif
      RES_O_COLLECTION_PERIOD,
    };
    int nbRes = sizeof(resList) / sizeof(uint16_t);
    *dataArrayP = lwm2m_data_new(nbRes);
    if (*dataArrayP == NULL)
      return LWM2M_500_INTERNAL_SERVER_ERROR ;
    *numDataP = nbRes;
    for (i = 0; i < nbRes; i++)
    {
      (*dataArrayP)[i].id = resList[i];
    }
  }

  i = 0;
  do
  {
    result = prv_set_tlv((*dataArrayP) + i, (conn_s_data_t*) (objectP->userData));
    i++;
  } while (i < *numDataP && result == LWM2M_205_CONTENT );

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
static uint8_t prv_conn_s_write_resources(lwm2m_object_t * objectP,
                                            int numData,
                                            lwm2m_data_t *dataArray,
                                            uint16_t resInstId, uint8_t write_method)
{
  conn_s_data_t *connStDataP = NULL;
  int i = 0;
  int64_t val = 0;
  bool val_change = false;

  if (NULL == dataArray || NULL == objectP)
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  connStDataP = (conn_s_data_t *)objectP->userData;
  if (NULL == connStDataP) return LWM2M_404_NOT_FOUND ;

  for (i = 0; i < numData; i++)
  {
    switch (dataArray[i].id)
    {
      case RES_O_COLLECTION_PERIOD:
        if (1 == lwm2m_data_decode_int(dataArray + i, &val))
        {
          if (val >= 0 && val <= 0xFFFFFFFF)
          {
            if(connStDataP->collectionPeriod != val)
            {
              val_change = true;
            }
            connStDataP->collectionPeriod = (time_t)val;
            /* Persist the collection period timer value */
            if(val_change == TRUE || create_operation == TRUE)
            {
              store_conn_stats_persistent_info(connStDataP);
            }
            break;
          }
          else
          {
            return LWM2M_400_BAD_REQUEST;
          }
        }
        else
        {
          return LWM2M_400_BAD_REQUEST;
        }

      case RES_O_RX_DATA:
      case RES_O_TX_DATA:
      case RES_O_AVERAGE_MESSAGE_SIZE:
      case RES_M_START:
      case RES_M_STOP:
        /*If create/write have one more one resource to write we write on reablable resource*/
        if(numData > 1)
          break;
        else
          return LWM2M_405_METHOD_NOT_ALLOWED;
      default:
        /*If create/write have one more one resource to write we write on unknown/notsupported resource*/
        if(numData > 1)
          break;
        else
          return LWM2M_404_NOT_FOUND;
    }
  }
  return LWM2M_204_CHANGED;
  
}



/**
 * @fn static uint8_t prv_conn_s_write()
 *
 * @brief This fucntion implements the write operation performed on Connectivity
 *        Statistics object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numData number of resources in the array
 * @param dataArray resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */
static uint8_t prv_conn_s_write (uint16_t instanceId, int numData,
    lwm2m_data_t * dataArray, lwm2m_object_t * objectP, uint16_t resInstId, uint8_t write_method, bool bs_true)
{

  uint8_t result = LWM2M_205_CONTENT;

  if (NULL == dataArray || NULL == objectP)
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  // this is a single instance object
  if (instanceId != 0)
  {
    return LWM2M_404_NOT_FOUND ;
  }

  result = prv_conn_s_create_write_check(numData, dataArray,bs_true);
  if (result != LWM2M_204_CHANGED )
  {
    ds_iot_ulog_err("Write operation failed due to write on Readable resource. ");
    return result;
        }

  result = prv_conn_s_write_resources(objectP, numData, dataArray,
                                        resInstId, write_method);
  return result;

}

#ifdef CONNECTIVITY_STATS_ENHANCEMENT
/**
 * @fn static uint8_t prv_conn_s_discover()
 *
 * @brief This fucntion implements the discover operation performed on the
 *     Connectivity Statistics object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numDataP number of resources in the array
 * @param dataArrayP resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 *
 */
static uint8_t prv_conn_s_discover(uint16_t instanceId,
    int * numDataP,
    lwm2m_data_t ** dataArrayP,
    lwm2m_object_t * objectP, lwm2m_server_t *serverP)
{
  uint8_t result;
  int i;

  // this is a single instance object
  if (instanceId != 0)
  {
    return LWM2M_404_NOT_FOUND;
  }

  result = LWM2M_205_CONTENT;

  // is the server asking for the full object ?
  if (*numDataP == 0)
  {
    uint16_t resList[] = {
      // As interface is not available , so disabling , once interface available need to enable
#if 0
      //  VZ_REQ_OTADMLWM2M_41068 Requirement             
      RES_O_SMS_TX_COUNTER,
      RES_O_SMS_RX_COUNTER,
#endif            
      RES_O_TX_DATA,
      RES_O_RX_DATA,
      //RES_O_MAX_MESSAGE_SIZE is not supported in TX
      // As interface is not available , so disabling , once interface available need to enable
#if 0    
      //  VZ_REQ_OTADMLWM2M_41068 Requirement
      RES_O_MAX_MESSAGE_SIZE,
#endif
      RES_O_AVERAGE_MESSAGE_SIZE,
      RES_M_START,
      RES_M_STOP,
      RES_O_COLLECTION_PERIOD
    };
    int nbRes = sizeof(resList) / sizeof(uint16_t);

    *dataArrayP = lwm2m_data_new(nbRes);
    if (*dataArrayP == NULL) return LWM2M_500_INTERNAL_SERVER_ERROR;
    *numDataP = nbRes;
    for (i = 0; i < nbRes; i++)
    {
      (*dataArrayP)[i].id = resList[i];
    }
  }
  else
  {
    for (i = 0; i < *numDataP && result == LWM2M_205_CONTENT; i++)
    {
      switch ((*dataArrayP)[i].id)
      {
        // As interface is not available , so disabling , once interface available need to enable
#if 0  
        //  VZ_REQ_OTADMLWM2M_41068 Requirement
        case RES_O_SMS_TX_COUNTER:
        case RES_O_SMS_RX_COUNTER:
#endif            
        case RES_O_TX_DATA:
        case RES_O_RX_DATA:
          // As interface is not available , so disabling , once interface available need to enable
#if 0
          //  VZ_REQ_OTADMLWM2M_41068 Requirement
        case RES_O_MAX_MESSAGE_SIZE:
#endif            
        case RES_O_AVERAGE_MESSAGE_SIZE:
        case RES_M_START:
        case RES_M_STOP:
        case RES_O_COLLECTION_PERIOD:
          break;
        default:
          result = LWM2M_404_NOT_FOUND;
      }
    }
  }

  return result;
}
#endif

/**
 * @fn static uint8_t prv_conn_s_execute()
 *
 * @brief This fucntion implements the execute operation performed on Connectivity
 *        Statistics object
 * @param instanceId Instance ID of the object to be read
 * @param resourceId Resource ID of the resource to be executed
 * @param buffer message payload from server fro execute operation
 * @param length message payload length
 * @param objectP Object on which the operation needs to be executed
 *
 * @return LWM2M response type
 *
 */
static uint8_t prv_conn_s_execute(uint16_t instanceId, uint16_t resourceId,
    uint8_t * buffer, int length, lwm2m_object_t * objectP)
{
  // this is a single instance object
  if( objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  if (instanceId != 0)
  {
    return LWM2M_404_NOT_FOUND;
  }

  switch (resourceId)
  {
    case RES_M_START:
      {
        conn_s_data_t *dataP = (conn_s_data_t *)objectP->userData;
       prv_resetCounter(objectP, LWM2M_TRUE);
       collectDataStopped = LWM2M_FALSE;

        if (0 != dataP->collectionPeriod)
          timer_set(&collPeriodTimer, dataP->collectionPeriod, 0, T_SEC);

        resetCollection = LWM2M_FALSE;
        return LWM2M_204_CHANGED;
      }
    case RES_M_STOP:

      collectDataStopped = LWM2M_TRUE;
      timer_clr(&collPeriodTimer, T_SEC);
      return LWM2M_204_CHANGED;

    case RES_O_COLLECTION_PERIOD:
    case RES_O_RX_DATA:
    case RES_O_TX_DATA:
    case RES_O_AVERAGE_MESSAGE_SIZE:
      return LWM2M_405_METHOD_NOT_ALLOWED;

    default:
      return LWM2M_404_NOT_FOUND;
  }
}
#if 0
/**
 * @fn void conn_s_updateTxStatistic()
 *
 * @brief This function is used to update tx statistics
 *
 * @param objectP Object on which the operation needs to be executed
 * @param txDataByte tx data
 * @param smsBased flag to check for updating sms counter
 *
 * @return void
 *
 */
void conn_s_updateTxStatistic(lwm2m_object_t * objectP, uint16_t txDataByte,
    bool smsBased)
{
  conn_s_data_t* myData = NULL;
  if( objectP == NULL ) 
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL",__func__,__FILE__);
    return ;
  }

  myData = (conn_s_data_t*) (objectP->userData);
  if (myData->collectDataStarted)
  {
    myData->txDataByte += txDataByte;
    myData->messageCount++;
    myData->avrMessageSize = (myData->txDataByte+myData->rxDataByte) /
      myData->messageCount;
    if (txDataByte > myData->maxMessageSize)
      myData->maxMessageSize = txDataByte;
    if (smsBased) myData->smsTxCounter++;
  }
}

/**
 * @fn void conn_s_updateRxStatistic()
 *
 * @brief This function is used to update rx statistics
 *
 * @param objectP Object on which the operation needs to be executed
 * @param rxDataByte rx data
 * @param smsBased flag to check for updating sms counter
 *
 * @return void
 *
 */
void conn_s_updateRxStatistic(lwm2m_object_t * objectP, uint16_t rxDataByte,
    bool smsBased)
{
  conn_s_data_t* myData = NULL;
  if( objectP == NULL )  
  {
    LOG_INFO("fun [%s] file [%s]  Passed  Argument is NULL",__func__,__FILE__);
    return;
  }

  myData = (conn_s_data_t*) (objectP->userData);
  if (myData->collectDataStarted)
  {
    myData->rxDataByte += rxDataByte;
    myData->messageCount++;
    myData->avrMessageSize = (myData->txDataByte+myData->rxDataByte) /
      myData->messageCount;
    if (rxDataByte > myData->maxMessageSize)
      myData->maxMessageSize = rxDataByte;
    myData->txDataByte += rxDataByte;
    if (smsBased) myData->smsRxCounter++;
  }
}
#endif
/**
 * @fn static void prvCollPeriodTmrCB()
 *
 * @brief This function is used to stop collection period
 *
 * @param time_ms time in ms
 * @param userData handle to callback
 *
 * @return void
 *
 */
static void prvCollPeriodTmrCB(int32 time_ms, timer_cb_data_type userData )
{
  LOG_DEBUG("Colletion Period Timed out!!!!");
  collectDataStopped = LWM2M_TRUE;
}


/**
 * @fn static uint8_t prv_conn_s_create()
 *
 * @brief This fucntion is used to create Connectivity Statistics object
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

- If the "Create" operation is permitted, the LwM2M Client MUST perform the instantiation on the 
Object only if all the mandatory Resources are present in the "New Value" parameter (see Section 5). 
If all the mandatory Resources are not present, the LwM2M Client MUST send a "Bad Request" error code 
to the LwM2M Server


*************************************************************************************************************/

static uint8_t prv_conn_s_create(uint16_t instanceId,
    int numData,
    lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP, bool apiCall)
{
  conn_s_data_t *connObj = NULL;
  uint8_t result = 0;

  if( dataArray == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  /*check if already one instance is created */
  if(objectP->instanceList != NULL)
  {
    ds_iot_ulog_err("Connectivity Statistics object instance exist");
    return LWM2M_400_BAD_REQUEST;
  }
  result = prv_conn_s_create_write_check(numData, dataArray, TRUE);
  if (result != LWM2M_204_CHANGED )
  {
    ds_iot_ulog_err("Creating instance failed ");
    return result;
  }

  connObj = (conn_s_data_t*) lwm2m_calloc(1, sizeof(conn_s_data_t));
  if( NULL == connObj)
  {
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  connObj->instanceId = instanceId;
  prv_resetCounter(objectP, false);
  timer_def_osal(&collPeriodTimer, &lwm2mTimerGroup, TIMER_FUNC1_CB_TYPE,
      prvCollPeriodTmrCB, (timer_cb_data_type)0);

  objectP->instanceList = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));
  if (NULL != objectP->instanceList)
  {
    memset(objectP->instanceList, 0x00, sizeof(lwm2m_list_t));
  }

  objectP->userData = connObj;
  create_operation = LWM2M_TRUE;
  result = prv_conn_s_write_resources(objectP, numData, dataArray, 
                                          INVALID_RES_INST_ID, QAPI_COAP_POST);
  create_operation = LWM2M_FALSE;
  if (result != LWM2M_204_CHANGED)
  {
    // Issue in creating object instanace so retuning Bad request error.
    result = LWM2M_400_BAD_REQUEST;
    lwm2m_free(objectP->instanceList);
    objectP->instanceList = NULL;
    lwm2m_free(connObj);
    objectP->userData = NULL;
    return result;
  }

  /* Restore collection period timer value from persistence file.*/
  load_conn_stats_persistent_info(connObj);
  
  efs_unlink(LWM2M_CONN_STAT_OBJ_PERSISTENCE_PATH);
  return LWM2M_201_CREATED;
}
/**
 * @fn static uint8_t prv_conn_s_delete()
 *
 * @brief This fucntion is used to delete Connectivity Statistics object
 *
 * @param id Instance ID of the object to be deleted
 * @param objectP Object to be deleted
 *
 * @return LWM2M response type
 */
static uint8_t prv_conn_s_delete(uint16_t id,
    lwm2m_object_t * objectP,
    bool persist_file)
{
  int fd = 0;
  conn_s_data_t * connObj = NULL;

  if( objectP == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }


  objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&connObj);
  if (NULL == connObj) return LWM2M_404_NOT_FOUND;

  /*  Update single instance object persistence */
  
  if(NULL != objectP->userData)
  {
    lwm2m_free(objectP->userData);
    objectP->userData = NULL;
  }

  lwm2m_free(connObj);
  connObj = NULL;

  if(persist_file)
  {
  ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_CONN_STAT_OBJ_PERSISTENCE_PATH);
  fd = efs_open( LWM2M_CONN_STAT_OBJ_PERSISTENCE_PATH, O_CREAT, 00744);
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
 * @fn lwm2m_object_t * get_object_conn_s()
 *
 * @brief This function is used to get the information about the Connectivity
 *        Statistics object during client registartion
 *
 * @param void
 *
 * @return Connectivity Statistics object  
 *
 */
lwm2m_object_t * get_object_conn_s(void)
{
  /*
   * The get_object_conn_s() function create the object itself and return
   * a pointer to the structure that represent it.
   */
  lwm2m_object_t * connObj = NULL;
  int fd = 0;

  connObj = (lwm2m_object_t *) lwm2m_malloc(sizeof(lwm2m_object_t));

  if (NULL != connObj)
  {
    memset(connObj, 0, sizeof(lwm2m_object_t));

    /*
     * It assign his unique ID
     * The 7 is the standard ID for the optional object "Connectivity Statistics".
     */
    connObj->objID = LWM2M_CONN_STATS_OBJECT_ID;

    /*
     * And the private function that will access the object.
     * Those function will be called when a read/execute/close
     * query is made by the server or core. In fact the library don't need
     * to know the resources of the object, only the server does.
     */
    connObj->readFunc     = prv_conn_s_read;
    connObj->writeFunc    = prv_conn_s_write;
    connObj->executeFunc  = prv_conn_s_execute;
    connObj->createFunc   = prv_conn_s_create;
    connObj->deleteFunc   = prv_conn_s_delete;
#ifdef CONNECTIVITY_STATS_ENHANCEMENT
    connObj->discoverFunc = prv_conn_s_discover;
#endif

    /*  Check if host_device_obj_single_instance_pr file exist */
    ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_CONN_STAT_OBJ_PERSISTENCE_PATH);
    fd = efs_open(LWM2M_CONN_STAT_OBJ_PERSISTENCE_PATH, O_RDONLY);
    if(fd != EFAILURE)
    {
      efs_close(fd);
      return connObj;
    }

    connObj->instanceList = lwm2m_malloc(sizeof(lwm2m_list_t));
    if (NULL != connObj->instanceList)
    {
      memset(connObj->instanceList, 0, sizeof(lwm2m_list_t));
    }
    else 
    {
      lwm2m_free(connObj);
      return NULL;
    }

    connObj->userData     = lwm2m_malloc(sizeof(conn_s_data_t));

    /*
     * Also some user data can be stored in the object with a private
     * structure containing the needed variables.
     */
    if (NULL != connObj->userData)
    {
      memset(connObj->userData, 0, sizeof(conn_s_data_t));
      prv_resetCounter(connObj, false);
      timer_def_osal(&collPeriodTimer, &lwm2mTimerGroup, TIMER_FUNC1_CB_TYPE,
      prvCollPeriodTmrCB, (timer_cb_data_type)0);
      /* Restore collection period timer value from persistence file.*/
      load_conn_stats_persistent_info((conn_s_data_t*)connObj->userData);
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
 * @fn void free_object_conn_s()
 *
 * @brief This fucntion frees the Connectivity Statistics object allocated
 * @param ObjectP Object to free
 *
 * @return void
 */
void free_object_conn_s(lwm2m_object_t * objectP)
{
  lwm2m_list_t* objlist;

  if (objectP == NULL) return;

  objlist = objectP->instanceList;
  while(objlist)
  {
    prv_conn_s_delete(objlist->id,objectP,false);
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
uint8_t connectivity_stat_change(lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP)
{
  uint8_t result = 0;
  //conn_m_data_t * data = NULL;

  if (dataArray == NULL || objectP == NULL)
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  switch (dataArray->id)
  {
    case RES_O_TX_DATA:
    case RES_O_RX_DATA:
    case RES_O_AVERAGE_MESSAGE_SIZE:
      result = LWM2M_204_CHANGED;
      break;
    default:
      result = LWM2M_404_NOT_FOUND;
      break;
  }
  return result;
}
#endif

