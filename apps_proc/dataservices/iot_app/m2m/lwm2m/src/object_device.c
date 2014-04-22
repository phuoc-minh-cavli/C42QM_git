/******************************************************************************

  @file    object_device.c

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

/*******************************************************************************
 *
 * Copyright (c) 2013, 2014 Intel Corporation and others.
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
 *    David Navarro, Intel Corporation - initial API and implementation
 *    domedambrosio - Please refer to git log
 *    Fabien Fleutot - Please refer to git log
 *    Axel Lorente - Please refer to git log
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    
 *******************************************************************************/

/*
   Copyright (c) 2013, 2014 Intel Corporation

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 * Neither the name of Intel Corporation nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 THE POSSIBILITY OF SUCH DAMAGE.

 David Navarro <david.navarro@intel.com>

*/

/*
 * This object is single instance only, and is mandatory to all LWM2M device as it describe the object such as its
 * manufacturer, model, etc...
 */
 
#ifdef MOB_TX
#include "time_stub.h"
#endif


#include "liblwm2m.h"
#include "lwm2mclient.h"
#include "log_utils.h"
#include "lwm2m_signals.h"
#include "qurt/qurt.h"

#include "iotapp_qmi_internal.h"
#include "time_types.h"
#include "memheap.h"
#include "qapi_pmapp_vbatt.h"
#include "iotapp_qmi_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "time_svc.h"
#include "time_jul.h"
#include "qapi_coap.h"
#include "ds_log.h"

#define PRV_MANUFACTURER      "Open Mobile Alliance"
#define PRV_MODEL_NUMBER      "Lightweight M2M Client"
#define PRV_SERIAL_NUMBER     "345000123"
#define PRV_FIRMWARE_VERSION  "1.0"
#define PRV_POWER_SOURCE_1    1
#define PRV_POWER_SOURCE_2    5
#define PRV_POWER_VOLTAGE_1   3800
#define PRV_POWER_VOLTAGE_2   5000
#define PRV_POWER_CURRENT_1   125
#define PRV_POWER_CURRENT_2   900
#define PRV_BATTERY_LEVEL     100
#define PRV_MEMORY_FREE       15
#define PRV_ERROR_CODE        0
#define PRV_ERR_CODE_LOWBATTERY 1
#define PRV_ERR_CODE_LOWSIG   4
#define PRV_ERR_CODE_LOWMEM   5
#define PRV_TIME_ZONE         " "
#define PRV_BINDING_MODE_1_0  "UQS"
#define PRV_BINDING_MODE_1_1  "UN"
#define PRV_EMPTY         " "

#define PRV_OFFSET_MAXLEN   7 // +HH:MM\0 at max

#define PRV_TIMEZONE_MAXLEN  25
#define PRV_TLV_BUFFER_SIZE 128
#define M2M_DEVICE_TYPE_STR   "Smart Device"
#define GPS_TO_UNIX_TIME_OFFSET   315964800

void device_obj_release_object_link(uint16_t obj_id, uint16_t inst_id); 
extern qurt_signal_t lwm2m_signal_Handle;

typedef struct 
{
  const char* time_offset;
  const char* time_zone;
} time_zone_offset_t;

/* External device information */
typedef struct _ext_device_info_s
{
  struct _ext_device_info_s *next; 
  uint32_t link_info; 
} ext_device_info_t;

extern mem_heap_type amss_mem_heap; //To fetch memory related data

bool obj_created = FALSE;
typedef struct _device_data_t
{
  struct _device_data_t * next;   // matches lwm2m_list_t::next
  uint16_t    instanceId;            // matches lwm2m_list_t::id
  int64_t free_memory;
  int64_t total_memory; 
  int64_t error;
  int64_t time;
  uint8_t battery_level;
  uint8_t battery_status;
  uint8_t *serial_no;
  uint8_t *hw_version;
  uint8_t *sw_version;
  uint8_t *fm_version;
  uint8_t *mnfctre_code;
  uint8_t *device_type;
  uint8_t *model_no;
  ext_device_info_t *ext_dev_info;   
  bool    isFromApp_serial_no;  /*serial_no is setting from application(else from default) */
  bool    isFromApp_hw_version; /*hw_version is setting from application(else from default) */
  bool    isFromApp_sw_version; /*sw_version is setting from application(else from default) */
  bool    isFromApp_fm_version; /*fm_version is setting from application(else from default) */
  bool    isFromApp_mnfctre_code; /*mnfctre_code is setting from application(else from default) */
  bool    isFromApp_device_type;  /*device_type is setting from application(else from default) */
  bool    isFromApp_model_no;     /*model_no is setting from application(else from default) */
  char time_offset[PRV_OFFSET_MAXLEN];
  char time_zone[PRV_TIMEZONE_MAXLEN];
} device_data_t;

time_zone_offset_t zone_offset[] ={
  {"-11:00", "Pacific/Midwayk"},
  {"-10:00", "America/Adak"},
  {"-09:30", "Pacific/Marquesas"},
  {"-09:00", "America/Anchorage"},
  {"-08:00", "America/Los_Angeles"},
  {"-07:00", "America/Mazatlan"},
  {"-06:00", "America/Mexico_city"},
  {"-05:00", "America/New_York"},
  {"-04:00", "America/Montserrat"},
  {"-03:30", "America/St_Johns"},
  {"-03:00", "America/Montevideo"},
  {"-02:00", "America/Noronha"},
  {"-01:00", "America/Scoresbysund"},
  {"+00:00", "Africa/Abidjan"},
  {"+01:00", "Africa/Algiers"},
  {"+02:00", "Africa/cairo"},
  {"+03:00", "Africa/Addis_Ababa"},
  {"+03:30", "Asia/Tehran"},
  {"+04:00", "Asia/Dubai"},
  {"+04:30", "Asia/Kathmandu"},
  {"+05:00", "Asia/Karachi"},
  {"+05:30" ,"Asia/kolkata"},
  {"+05:45", "Asia/Kathmandu"},
  {"+06:00", "Asia/Kashgar"},
  {"+06:30" ,"Asia/Rangoon"},
  {"+07:00", "Asia/Bangkok"},
  {"+08:00", "Asia/Macao"},
  {"+08:30", "Asia/Pyongyang"},
  {"+08:45", "Australia/Eucla"},
  {"+09:00", "Asia/chaita"},
  {"+09:30", "Australia/Adelaide"},
  {"+10:00", "Australia/Hobart"},
  {"+10:30", "Australia/Lord_Howe"},
  {"+11:00", "Antartica/Casey"},
  {"+12:00", "Antarctica/McMurdo"},
  {"+12:45", "Pacific/Chatham"},
  {"+13:00", "Pacific/Apia"},
  {"+14:00", "Pacific/Kiritimati"}
};

time_zone_offset_t zone_offset_dlt_saving[] ={
  {"-11:00", "Pacific/Midwayk"},
  {"-10:00", "Pacific/Honolulu"},
  {"-09:30", "Pacific/Marquesas"},
  {"-09:00", "America/Adak"},
  {"-08:00", "America/Anchorage"},
  {"-07:00", "America/Los_Angeles"},
  {"-06:00", "America/Mazatlan"},
  {"-05:00", "America/New_York"},
  {"-04:00", "America/Montserrat"},
  {"-03:00", "America/Montevideo"},
  {"-02:30", "America/St_Johns"},
  {"-02:00", "America/Noronha"},
  {"-01:00", "America/Scoresbysund"},
  {"+00:00", "Africa/Abidjan"},
  {"+01:00", "Africa/Algiers"},
  {"+02:00", "Africa/cairo"},
  {"+03:00", "Africa/Addis_Ababa"},
  {"+04:00", "Asia/Dubai"},
  {"+04:30", "Asia/Kathmandu"},
  {"+05:00", "Asia/Karachi"},
  {"+05:30" ,"Asia/kolkata"},
  {"+05:45", "Asia/Kathmandu"},
  {"+06:00", "Asia/Kashgar"},
  {"+06:30" ,"Asia/Rangoon"},
  {"+07:00", "Asia/Bangkok"},
  {"+08:00", "Asia/Macao"},
  {"+08:30", "Asia/Pyongyang"},
  {"+08:45", "Australia/Eucla"},
  {"+09:00", "Asia/chita"},
  {"+10:00", "Australia/Brisbane"},
  {"+10:30", "Australia/Adelaide"},
  {"+11:00", "Australia/Hobart"},
  {"+12:00", "Pacific/Tarawa"},
  {"+13:00", "Antarctica/McMurdo"},
  {"+13:45", "Pacific/Chatham"},
  {"+14:00", "Pacific/Apia"}
};

void get_utc_time_zone(device_data_t *instanceP, bool daylight_save);
bool factory_reset_req =false;
// basic check that the time offset value is at ISO 8601 format
// bug: +12:30 is considered a valid value by this function

extern int gSignalThreshold;
extern uint32_t gMemThreshold;

void prv_device_get_utc_offset_n_time_zone(device_data_t *device_instance)
{
  int16_t time_offset = 0;
  bool daylight_save = 0;

  if(NULL == device_instance)
    return;

  /** Fetch the time offset value  */
  time_offset = qmi_nas_get_network_time(&daylight_save);
  if(time_offset != EFAILURE) {
    if(time_offset < 0)
      snprintf(device_instance->time_offset, sizeof(device_instance->time_offset), "-%02d:%02d",
          (-1 * (time_offset/60)), ((-1 * (time_offset%60))));
    else
      snprintf(device_instance->time_offset, sizeof(device_instance->time_offset), "+%02d:%02d",
          (time_offset / 60), (time_offset%60));          
  } else {
    snprintf(device_instance->time_offset, sizeof(device_instance->time_offset), " "); 
  }

  get_utc_time_zone(device_instance, daylight_save); 

  return;  
}
/**
 * @fn get_utc_time_zone()
 * @brief  stub implementation for fetching time zone
 * @param instanceP handle to device_data_t 
 * @return void
 *
 * */
void get_utc_time_zone(device_data_t *instanceP, bool daylight_save)
{
  int i = 0;

  if(!instanceP)
    return;

  strlcpy(instanceP->time_zone, PRV_TIME_ZONE, PRV_TIMEZONE_MAXLEN);
  if(daylight_save == 1) {
    for(i = 0; i < sizeof(zone_offset_dlt_saving)/ sizeof(zone_offset_dlt_saving[0]) ; i++)
    {
      if(!strcmp(instanceP->time_offset, zone_offset_dlt_saving[i].time_offset))
      {
        strlcpy(instanceP->time_zone, zone_offset_dlt_saving[i].time_zone, PRV_TIMEZONE_MAXLEN);
        break;
      }
    }
  }
  else {
    for(i = 0; i < sizeof(zone_offset)/ sizeof(zone_offset[0]) ; i++)
    {
      if(!strcmp(instanceP->time_offset, zone_offset[i].time_offset))
      {
        strlcpy(instanceP->time_zone, zone_offset[i].time_zone, PRV_TIMEZONE_MAXLEN);
        break;
      }
    }
  }

  return;  
}

#if 0
/**
 * @fn static int prv_check_time_offset()
 *
 * @brief  this function is used to check time offset
 *
 * @param buffer time offset buffer
 * @param length length of buffer
 *
 * @return 1 on success, 0 on faiure
 *
 **/
static int prv_check_time_offset(char * buffer,
    int length)
{
  int min_index;

  if (length != 3 && length != 5 && length != 6) return 0;
  if (buffer[0] != '-' && buffer[0] != '+') return 0;
  switch (buffer[1])
  {
    case '0':
      if (buffer[2] < '0' || buffer[2] > '9') return 0;
      break;
    case '1':
      if (buffer[2] < '0' || buffer[2] > '2') return 0;
      break;
    default:
      return 0;
  }
  switch (length)
  {
    case 3:
      return 1;
    case 5:
      min_index = 3;
      break;
    case 6:
      if (buffer[3] != ':') return 0;
      min_index = 4;
      break;
    default:
      // never happen
      return 0;
  }
  if (buffer[min_index] < '0' || buffer[min_index] > '5') return 0;
  if (buffer[min_index+1] < '0' || buffer[min_index+1] > '9') return 0;

  return 1;
}
#endif

/** 
 * @fn     store_device_persistent_info()
 * @brief  This function is used to dump resource value set from App into persistence file
 * @param  devDataP - pointer to Dev object
 * @return on success  - LWM2M_TRUE
           on error    - LWM2M_FALSE
 */ 
bool store_device_persistent_info(device_data_t* devDataP)
{
  int fd = 0;
  bool res = LWM2M_FALSE;

  if (NULL == devDataP)
  {
    LOG_DEBUG("%s",nullError);
    return res;
  }
  ds_iot_log_strf_med("EFS_OPT: File open %s",DEVICE_PERSISTENCE_FILE);
  fd = efs_open(DEVICE_PERSISTENCE_FILE, O_CREAT | O_WRONLY | O_TRUNC, 00644);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open device-object persistence file");
    return res;
  }

  if(devDataP->isFromApp_serial_no && !write_Tlv_to_file(fd, RES_O_SERIAL_NUMBER, strlen((char*)devDataP->serial_no), devDataP->serial_no ))
  {
    goto write_error;
  }
  if(devDataP->isFromApp_mnfctre_code && !write_Tlv_to_file(fd, RES_O_MANUFACTURER, strlen((char*)devDataP->mnfctre_code), devDataP->mnfctre_code ))
  {
    goto write_error;
  }
  if(devDataP->isFromApp_model_no && !write_Tlv_to_file(fd, RES_O_MODEL_NUMBER, strlen((char*)devDataP->model_no), devDataP->model_no))
  {
    goto write_error;  
  }
  if(devDataP->isFromApp_hw_version && !write_Tlv_to_file(fd, RES_O_HARDWARE_VERSION, strlen((char*)devDataP->hw_version), devDataP->hw_version ))
  {
    goto write_error;
  }
  if(devDataP->isFromApp_sw_version && !write_Tlv_to_file(fd, RES_O_SOFTWARE_VERSION, strlen((char*)devDataP->sw_version), devDataP->sw_version ))
  {
    goto write_error;
  }
  if(devDataP->isFromApp_fm_version && !write_Tlv_to_file(fd, RES_O_FIRMWARE_VERSION, strlen((char*)devDataP->fm_version), devDataP->fm_version))
  {
	goto write_error;  
  }
  if(devDataP->isFromApp_device_type && !write_Tlv_to_file(fd, RES_O_DEVICE_TYPE, strlen((char*)devDataP->device_type), devDataP->device_type))
  {
    goto write_error;  
  }

  res = LWM2M_TRUE;

  write_error:
	efs_close(fd);
    if(res == LWM2M_FALSE)
    {//if any error happen when delete the file
      efs_unlink(DEVICE_PERSISTENCE_FILE);
	  ds_iot_ulog_high("save device-object persistence file failed");	
    }
	else
	{
	  ds_iot_ulog_high("save device-object persistence file success");	
	}
    return res;	
}

/** 
 * @fn     load_device_persistent_info()
 * @brief  This function is used to load resource value set from App from persistence file
 * @param  devDataP - pointer to Dev object
 * @return on success  - LWM2M_TRUE
           on error    - LWM2M_FALSE
 */ 
bool load_device_persistent_info(device_data_t* devDataP)
{
  int fd = 0;
  uint16    resource_id   = 0;
  uint8* resource_data = 0;
  uint8  resource_length = 0;
  int   res = 0;

  if (NULL == devDataP)
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_FALSE;
  }
  ds_iot_log_strf_med("EFS_OPT: File open %s",DEVICE_PERSISTENCE_FILE);
  fd = efs_open(DEVICE_PERSISTENCE_FILE, 00);
  if (EFAILURE == fd)
  {
    ds_iot_ulog_err("failed to open device-object persistence file");
    return LWM2M_FALSE;
  }

  res = read_Tlv_from_file(fd, &resource_id, &resource_length, &resource_data);
  while(res == 1)
  {
    switch(resource_id)
    {
	  case RES_O_MANUFACTURER:
		devDataP->mnfctre_code = resource_data; 
		devDataP->isFromApp_mnfctre_code = LWM2M_TRUE;
		break;
      case RES_O_SERIAL_NUMBER:
	  	devDataP->serial_no = resource_data;
		devDataP->isFromApp_serial_no = LWM2M_TRUE;
		break;
	  case RES_O_MODEL_NUMBER:
	  	devDataP->model_no = resource_data; 
		devDataP->isFromApp_model_no = LWM2M_TRUE;
		break;
	  case RES_O_HARDWARE_VERSION:
	  	devDataP->hw_version = resource_data; 
		devDataP->isFromApp_hw_version = LWM2M_TRUE;
		break;
	  case RES_O_SOFTWARE_VERSION:
	  	devDataP->sw_version = resource_data; 
		devDataP->isFromApp_sw_version = LWM2M_TRUE;
		break;
	  case RES_O_FIRMWARE_VERSION:
		devDataP->fm_version = resource_data; 
		devDataP->isFromApp_fm_version = LWM2M_TRUE;
		break;
	  case RES_O_DEVICE_TYPE:
	  	devDataP->device_type = resource_data; 
		devDataP->isFromApp_device_type = LWM2M_TRUE;
		break;
	  default:
	  	break;
    }		
	res = read_Tlv_from_file(fd, &resource_id, &resource_length, &resource_data);
  }

  efs_close(fd);

  if(res == EFAILURE)
  {//clean device persistent data if meet failure during load
    efs_unlink(DEVICE_PERSISTENCE_FILE);
    if(devDataP->isFromApp_mnfctre_code == LWM2M_TRUE)
    {
      lwm2m_free(devDataP->mnfctre_code);
	  devDataP->mnfctre_code           = NULL;
	  devDataP->isFromApp_mnfctre_code = LWM2M_FALSE;
    }
    if(devDataP->isFromApp_serial_no  == LWM2M_TRUE)
    {
      lwm2m_free(devDataP->serial_no );
	  devDataP->serial_no           = NULL;
	  devDataP->isFromApp_serial_no = LWM2M_FALSE;
    }
    if(devDataP->isFromApp_model_no == LWM2M_TRUE)
    {
      lwm2m_free(devDataP->model_no);
	  devDataP->model_no           = NULL;
	  devDataP->isFromApp_model_no = LWM2M_FALSE;
    }
    if(devDataP->isFromApp_hw_version == LWM2M_TRUE)
    {
      lwm2m_free(devDataP->hw_version);
	  devDataP->hw_version           = NULL;
	  devDataP->isFromApp_hw_version = LWM2M_FALSE;
    }
    if(devDataP->isFromApp_fm_version == LWM2M_TRUE)
    {
      lwm2m_free(devDataP->fm_version);
	  devDataP->fm_version           = NULL;
	  devDataP->isFromApp_fm_version = LWM2M_FALSE;
    }
    if(devDataP->isFromApp_sw_version == LWM2M_TRUE)
    {
      lwm2m_free(devDataP->sw_version);
	  devDataP->sw_version           = NULL;
	  devDataP->isFromApp_sw_version = LWM2M_FALSE;
    }
    if(devDataP->isFromApp_device_type == LWM2M_TRUE)
    {
      lwm2m_free(devDataP->device_type);
      devDataP->device_type           = NULL;
	  devDataP->isFromApp_device_type = LWM2M_FALSE;
    }  
  }

  return LWM2M_TRUE;
}

/** 
* @fn static uint8_t prv_device_create_write_check() 
*
* @brief This fucntion verifies and write the resource's into  
*        Device object 
*
* @param instanceId Instance of the device object to be create/write 
* @param numDataP number of resources in the array 
* @param dataArrayP resource value array 
* @param create_flag - if true creating instance else write on instance 
*
* @return LWM2M response type 
*
*/
/***************************************************************************************************

Under section 8.2.2 Operation on Object Instance of OMA 1.1

- For the "Write" operation on an Object Instance, the LwM2M Client MUST perform the operation, 
if all the Resources conveyed in the operation are allowed to perform the "Write" operation. 
If any Resource does not support the "Write" operation, the LwM2M Client MUST inform the LwM2M 
Server that the Object Instance cannot perform the requested "Write" operation in sending a 
"Operation is not supported" error code.

*************************************************************************************************************/   
static uint8_t prv_device_create_write_check(int numData,
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
      case RES_M_REBOOT:
      case RES_O_FACTORY_RESET:
	  case RES_O_RESET_ERROR_CODE:
	  case RES_O_MANUFACTURER:
      case RES_O_MODEL_NUMBER:
      case RES_O_SERIAL_NUMBER:
      case RES_O_FIRMWARE_VERSION:
      case RES_O_BATTERY_LEVEL:
      case RES_O_MEMORY_FREE:
	  case RES_M_ERROR_CODE:
      case RES_M_BINDING_MODES:
      case RES_O_MEMORY_TOTAL:
      case RES_O_BATTERY_STATUS:
      case RES_O_HARDWARE_VERSION:
      case RES_O_SOFTWARE_VERSION:
      case RES_O_DEVICE_TYPE:
      case RES_O_EXT_DEVICE_INFO:
      if (!create_flag)
      {
         result = LWM2M_405_METHOD_NOT_ALLOWED;
         break;
      }
      case RES_O_CURRENT_TIME:
	  case RES_O_UTC_OFFSET:
      case RES_O_TIMEZONE:
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
 * @fn static uint8_t prv_set_value()
 *
 * @brief This fucntion is used to populate the values of the resource
 *        to be provided to the server during the read operation on the object.
 * @param dataP data array for the resource 
 * @param devDataP device object data
 * @param resInstId Resource instance ID
 *
 * @return LWM2M response type
 */
static uint8_t prv_set_value(lwm2m_data_t * dataP,
    device_data_t * devDataP, uint16_t resInstId, lwm2m_server_t *serverP)
{
  mem_heap_totals_type amss_mem_heap_stats = {0};
  uint8_t battery_level = 0;
  uint8_t battery_threshold = 0;
  qapi_Status_t batt_status = QAPI_OK;
  
  // a simple switch structure is used to respond at the specified resource asked
  if( dataP == NULL || devDataP == NULL ) 
  {
    LOG_DEBUG("fun [%s] file [%s]  %s",__func__,__FILE__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;   
  }

  switch (dataP->id)
  {
    case RES_O_MANUFACTURER:
      lwm2m_data_encode_string((const char *)devDataP->mnfctre_code, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_MODEL_NUMBER:
      lwm2m_data_encode_string((const char *)devDataP->model_no, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_SERIAL_NUMBER:
      lwm2m_data_encode_string((const char *)devDataP->serial_no, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_FIRMWARE_VERSION:
      lwm2m_data_encode_string((const char *)devDataP->fm_version, dataP);
      return LWM2M_205_CONTENT;

    case RES_M_REBOOT:
      return LWM2M_405_METHOD_NOT_ALLOWED;

    case RES_O_FACTORY_RESET:
      return LWM2M_405_METHOD_NOT_ALLOWED;
      // As interface is not available , so disabling , once interface available need to enable 
#if 0
      //  VZ_REQ_OTADMLWM2M_41068 Requirement
    case RES_O_AVL_POWER_SOURCES: 
      {
        lwm2m_data_t * subTlvP;

        subTlvP = lwm2m_data_new(AVL_POWER_SOURCES);
        if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;
        subTlvP[0].id = 0;
        lwm2m_data_encode_int(PRV_POWER_SOURCE_1, subTlvP);
        subTlvP[1].id = 1;
        lwm2m_data_encode_int(PRV_POWER_SOURCE_2, subTlvP + 1);

        lwm2m_data_encode_instances(subTlvP, AVL_POWER_SOURCES, dataP);

        return LWM2M_205_CONTENT;
      }

    case RES_O_POWER_SOURCE_VOLTAGE:
      {
        lwm2m_data_t * subTlvP;

        subTlvP = lwm2m_data_new(AVL_POWER_SOURCES);
        if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;
        subTlvP[0].id = 0;
        lwm2m_data_encode_int(PRV_POWER_VOLTAGE_1, subTlvP);
        subTlvP[1].id = 1;
        lwm2m_data_encode_int(PRV_POWER_VOLTAGE_2, subTlvP + 1);

        lwm2m_data_encode_instances(subTlvP, AVL_POWER_SOURCES, dataP);

        return LWM2M_205_CONTENT;
      }

    case RES_O_POWER_SOURCE_CURRENT:
      {
        lwm2m_data_t * subTlvP;

        subTlvP = lwm2m_data_new(AVL_POWER_SOURCES);
        if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;
        subTlvP[0].id = 0;
        lwm2m_data_encode_int(PRV_POWER_CURRENT_1, &subTlvP[0]);
        subTlvP[1].id = 1;
        lwm2m_data_encode_int(PRV_POWER_CURRENT_2, &subTlvP[1]);

        lwm2m_data_encode_instances(subTlvP, AVL_POWER_SOURCES, dataP);

        return LWM2M_205_CONTENT;
      }
#endif
    case RES_O_BATTERY_LEVEL:
      /* Get Device Battery Info */

      batt_status = qapi_Pmapp_Vbatt_Get_Battery_Status( &devDataP->battery_level);
      if(( QAPI_OK != batt_status ) && ( QAPI_ERR_BATT_ABSENT != batt_status ))
      {
        ds_iot_ulog_err1("battery_level fetch failed : %d err",batt_status);
        return LWM2M_500_INTERNAL_SERVER_ERROR;
      }
      else if( QAPI_ERR_BATT_ABSENT == batt_status )
      {
        devDataP->battery_level = 0;
      }

      LOG_DEBUG("read battery_level: %d", devDataP->battery_level);
      lwm2m_data_encode_int(devDataP->battery_level, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_MEMORY_FREE:
      mem_heap_get_totals(&amss_mem_heap, &amss_mem_heap_stats);
      devDataP->free_memory = (amss_mem_heap_stats.free_bytes / 1024);      // To convert into KBs

      lwm2m_data_encode_int(devDataP->free_memory, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_MEMORY_TOTAL:
      mem_heap_get_totals(&amss_mem_heap, &amss_mem_heap_stats);
      devDataP->total_memory = (amss_mem_heap_stats.total_physical_bytes / 1024);     // To convert into KBs

      lwm2m_data_encode_int(devDataP->total_memory, dataP);
      return LWM2M_205_CONTENT; 

    case RES_M_ERROR_CODE:
      {
        lwm2m_data_t * subTlvP;
        int count = 0, index = 0;
        bool lowMemError = LWM2M_FALSE, lowSignal = LWM2M_FALSE;
        bool lowBatteryLevel = LWM2M_FALSE;
        uint8_t battery_threshold = 0;
        if (devDataP->error != 0)
          count++;
        mem_heap_get_totals(&amss_mem_heap, &amss_mem_heap_stats);
        if (amss_mem_heap_stats.free_bytes < gMemThreshold)
        {
          lowMemError = LWM2M_TRUE;
          count++;
        }
        if (nas_get_signal_strength(PARAM_SIGNAL_STRENGTH) < gSignalThreshold)
        {
          lowSignal = LWM2M_TRUE;
          count++;
        }

        batt_status = qapi_Pmapp_Vbatt_Get_Battery_Status( &devDataP->battery_level);

        if(( QAPI_OK != batt_status) && ( QAPI_ERR_BATT_ABSENT != batt_status ))
        {
          ds_iot_ulog_err("battery_level fetch failed");
          return LWM2M_500_INTERNAL_SERVER_ERROR;
        }
        else
        {
          if( QAPI_ERR_BATT_ABSENT == batt_status )devDataP->battery_level = 0;
          battery_threshold = get_battery_level_threshold();
          LOG_DEBUG("error code battery_level: %d", devDataP->battery_level);
          LOG_DEBUG("battery_threshold: %d", battery_threshold);
          if( devDataP->battery_level < battery_threshold )
          {
            lowBatteryLevel = LWM2M_TRUE;
            count++;            
          }
        }


        if (count == 0)
        {
          subTlvP = lwm2m_data_new(1);
          if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;
          subTlvP[0].id = 0;
          lwm2m_data_encode_int(0, &subTlvP[0]);
          lwm2m_data_encode_instances(subTlvP, 1, dataP);
          return LWM2M_205_CONTENT;
        }
        subTlvP = lwm2m_data_new(count);
        if (NULL == subTlvP) return LWM2M_500_INTERNAL_SERVER_ERROR;
        if (devDataP->error != 0)
        {
          subTlvP[index].id = index;
          lwm2m_data_encode_int(devDataP->error, &subTlvP[index]);
          index++;
        }
        if (lowMemError)
        {
          subTlvP[index].id = index;
          lwm2m_data_encode_int(PRV_ERR_CODE_LOWMEM, &subTlvP[index]);
          index++;
        }
        if (lowSignal)
        {
          subTlvP[index].id = index;
          lwm2m_data_encode_int(PRV_ERR_CODE_LOWSIG, &subTlvP[index]);
          index++;
        }
        if (lowBatteryLevel)
        {
          subTlvP[index].id = index;
          lwm2m_data_encode_int(PRV_ERR_CODE_LOWBATTERY, &subTlvP[index]);
          index++;
        }

        lwm2m_data_encode_instances(subTlvP, count, dataP);

        return LWM2M_205_CONTENT;
      }        
    case RES_O_RESET_ERROR_CODE:
      return LWM2M_405_METHOD_NOT_ALLOWED;

    case RES_O_CURRENT_TIME:
      lwm2m_data_encode_int(lwm2m_gettime()+GPS_TO_UNIX_TIME_OFFSET, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_UTC_OFFSET:
      prv_device_get_utc_offset_n_time_zone(devDataP);
      lwm2m_data_encode_string(devDataP->time_offset, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_TIMEZONE:
      prv_device_get_utc_offset_n_time_zone(devDataP);
      lwm2m_data_encode_string(devDataP->time_zone, dataP);
      return LWM2M_205_CONTENT;

    case RES_M_BINDING_MODES:
    {
      
      if (NULL == serverP)
        return LWM2M_500_INTERNAL_SERVER_ERROR;

      /* LwM2Mv1.0 client supports U/Q/S transport bindings. On the other hand, LwM2Mv1.1 client supports U/N bindings.
       * These bindings can't be changed by the server and are internal to the client (hard-coded). Device bindings are 
       * sent as part of the "Register Update" in LwM2Mv1.1 only if they change. Since we are hard-coding these device 
       * bindings they never change, so we don't send them at all.   
       */ 
      if ((float)LWM2M_V_1_0 == serverP->version)
        lwm2m_data_encode_string(PRV_BINDING_MODE_1_0, dataP);
      else  
        lwm2m_data_encode_string(PRV_BINDING_MODE_1_1, dataP);

      return LWM2M_205_CONTENT;
    }

    case RES_O_HARDWARE_VERSION :
      lwm2m_data_encode_string((const char *)devDataP->hw_version, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_SOFTWARE_VERSION :
      lwm2m_data_encode_string((const char *)devDataP->sw_version, dataP);
      return LWM2M_205_CONTENT; 

    case RES_O_BATTERY_STATUS :
      batt_status = qapi_Pmapp_Vbatt_Get_Battery_Status(&battery_level);
      if(( QAPI_OK !=  batt_status) && (QAPI_ERR_BATT_ABSENT != batt_status))
      {
        ds_iot_ulog_err("battery_level fetch failed");
        return LWM2M_500_INTERNAL_SERVER_ERROR;
      }
      else if (QAPI_ERR_BATT_ABSENT == batt_status)
      {
        ds_iot_ulog_err("battery not present ");
        battery_level = 0;
        devDataP->battery_status = BATTERY_STATUS_NOT_PRESENT;
      }
      if(battery_level > 0)
      {
        battery_threshold = get_battery_level_threshold();
        if( battery_level < battery_threshold )
        {
          devDataP->battery_status = BATTERY_STATUS_LOW;
        }
        else
        {
          devDataP->battery_status = BATTERY_STATUS_NORMAL;
        }
      }
      else
      {
        devDataP->battery_status = BATTERY_STATUS_NOT_PRESENT;
      }
      lwm2m_data_encode_int(devDataP->battery_status, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_DEVICE_TYPE:
      lwm2m_data_encode_string((const char *)devDataP->device_type, dataP);
      return LWM2M_205_CONTENT;

    case RES_O_EXT_DEVICE_INFO:
    {
      uint32_t                   result = LWM2M_NO_ERROR;
      lwm2m_data_t            *res_inst = NULL;
      uint32_t                     cntr = 0x00;
      ext_device_info_t  *curr_dev_info = devDataP->ext_dev_info;
    
      qurt_mutex_lock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);

      if (NULL == curr_dev_info)
      {
        /* Encode NULL link as there are no object links in the context */
        if (resInstId == INVALID_RES_INST_ID)
        {
          res_inst = lwm2m_data_new(0x01);
          if (NULL == res_inst) 
          {
            result = LWM2M_500_INTERNAL_SERVER_ERROR;
            goto READ_EXT_DEV_ERROR;
          }
          res_inst->id = cntr++;
          lwm2m_data_encode_objLink(0xFFFFFFFF, res_inst);
        }
        else
        {
          result = LWM2M_404_NOT_FOUND;
          goto READ_EXT_DEV_ERROR;
        }
      }
      else
      {
        if (resInstId != INVALID_RES_INST_ID)
        {
          cntr = 0;
          /* find the resource instance */
          while (curr_dev_info)
          {
            if(resInstId == cntr)
            {
              res_inst = lwm2m_data_new(0x01);
              if (NULL == res_inst)
              {
                result = LWM2M_500_INTERNAL_SERVER_ERROR;
                goto READ_EXT_DEV_ERROR;
              }
              res_inst->id = resInstId;
              lwm2m_data_encode_objLink(curr_dev_info->link_info, res_inst);
              break;
            }
            cntr++;
            curr_dev_info = curr_dev_info->next;
          }
          if(resInstId != cntr)
          {
            result = LWM2M_404_NOT_FOUND;
            goto READ_EXT_DEV_ERROR;
          }
          else
            cntr = 1;
        }
        else
        {
          /* Retrieve the object link count */ 
          while (curr_dev_info)
          {
            cntr++;
            curr_dev_info = curr_dev_info->next;
          }
          
          /* Allocate the resource instances */ 
          res_inst = lwm2m_data_new(cntr);
          if (NULL == res_inst) 
          {
            result = LWM2M_500_INTERNAL_SERVER_ERROR;
            goto READ_EXT_DEV_ERROR;
          }
          
          cntr = 0x00;
          curr_dev_info = devDataP->ext_dev_info;
          while (curr_dev_info)
          {
            res_inst[cntr].id = cntr;
            lwm2m_data_encode_objLink(curr_dev_info->link_info, &res_inst[cntr]);
          
            curr_dev_info = curr_dev_info->next;
            cntr++;
          }
        }
      }
      
      lwm2m_data_encode_instances(res_inst, cntr, dataP);
      result = LWM2M_205_CONTENT;

READ_EXT_DEV_ERROR:
      qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);
      return result;      
    }
  
    default:
      return LWM2M_404_NOT_FOUND;
  }
}

/**
 * @fn static uint8_t prv_device_read()
 *
 * @brief This function implements the read operation performed on device object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numDataP number of resources in the array
 * @param dataArrayP resource value array
 * @param objectP Object to be read
 * @param resInstId Resource instance ID
 *
 * @return LWM2M response type
 */
static uint8_t prv_device_read(uint16_t instanceId,
    int * numDataP,
    lwm2m_data_t ** dataArrayP,
    lwm2m_object_t * objectP,
    uint16_t resInstId, lwm2m_server_t *serverP)
{
  uint8_t result = 0;
  int i = 0;
  if( numDataP == NULL || dataArrayP == NULL || objectP == NULL ) 
  {
    ds_iot_ulog_err("fun [prv_device_read] LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  // this is a single instance object
  if (instanceId != 0)
  {
    return LWM2M_404_NOT_FOUND;
  }

  // is the server asking for the full object ?
  if (*numDataP == 0)
  {
    uint16_t resList[] = {
      RES_O_MANUFACTURER,
      RES_O_MODEL_NUMBER,
      RES_O_SERIAL_NUMBER,
      RES_O_FIRMWARE_VERSION,
      //E: RES_M_REBOOT,
      //E: RES_O_FACTORY_RESET,
      // As interface is not available , so disabling , once interface available need to enable 
#if 0  
      //  VZ_REQ_OTADMLWM2M_41068 Requirement             
      RES_O_AVL_POWER_SOURCES,
      RES_O_POWER_SOURCE_VOLTAGE,
      RES_O_POWER_SOURCE_CURRENT,
#endif              
      RES_O_BATTERY_LEVEL,
      RES_O_MEMORY_FREE,
      RES_M_ERROR_CODE,
      //E: RES_O_RESET_ERROR_CODE,
      RES_O_CURRENT_TIME,
      RES_O_UTC_OFFSET,
      RES_O_TIMEZONE,
      RES_M_BINDING_MODES,
      RES_O_HARDWARE_VERSION,
      RES_O_SOFTWARE_VERSION,
      RES_O_BATTERY_STATUS,
      RES_O_DEVICE_TYPE,
      RES_O_MEMORY_TOTAL,
      RES_O_EXT_DEVICE_INFO
    };

    int nbRes = sizeof(resList)/sizeof(uint16_t);

    *dataArrayP = lwm2m_data_new(nbRes);
    if (*dataArrayP == NULL) return LWM2M_500_INTERNAL_SERVER_ERROR;
    *numDataP = nbRes;
    for (i = 0 ; i < nbRes ; i++)
    {
      (*dataArrayP)[i].id = resList[i];
    }
  }

  i = 0;
  do
  {
    result = prv_set_value((*dataArrayP) + i, (device_data_t*)(objectP->userData), resInstId, serverP);
    i++;
  } while (i < *numDataP && result == LWM2M_205_CONTENT);

  return result;
}

/**
 * @fn static uint8_t prv_device_discover()
 *
 * @brief This fucntion implements the discover operation performed on the
 *     device object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numDataP number of resources in the array
 * @param dataArrayP resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 *
 */
static uint8_t prv_device_discover(uint16_t instanceId,
    int * numDataP,
    lwm2m_data_t ** dataArrayP,
    lwm2m_object_t * objectP, lwm2m_server_t *serverP)
{
  uint8_t result = 0;
  int i = 0;

  if ( numDataP == NULL || dataArrayP == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  // this is a single instance object
  if (instanceId != 0)
    return LWM2M_404_NOT_FOUND;
  
  result = LWM2M_205_CONTENT;

  // is the server asking for the full object ?
  if (*numDataP == 0)
  {
    uint16_t resList[] = {
      RES_O_MANUFACTURER,
      RES_O_MODEL_NUMBER,
      RES_O_SERIAL_NUMBER,
      RES_O_FIRMWARE_VERSION,
      RES_M_REBOOT,
      RES_O_FACTORY_RESET,
      // As interface is not available , so disabling , once interface available need to enable
#if 0 
      //  VZ_REQ_OTADMLWM2M_41068 Requirement
      RES_O_AVL_POWER_SOURCES,
      RES_O_POWER_SOURCE_VOLTAGE,
      RES_O_POWER_SOURCE_CURRENT,
#endif            
      RES_O_BATTERY_LEVEL,
      RES_O_MEMORY_FREE,
      RES_M_ERROR_CODE,
      RES_O_RESET_ERROR_CODE,
      RES_O_CURRENT_TIME,
      RES_O_UTC_OFFSET,
      RES_O_TIMEZONE,
      RES_M_BINDING_MODES,
      RES_O_HARDWARE_VERSION,
      RES_O_SOFTWARE_VERSION,
      RES_O_BATTERY_STATUS,
      RES_O_DEVICE_TYPE,
      RES_O_MEMORY_TOTAL,
      RES_O_EXT_DEVICE_INFO
    };
  
    int nbRes = sizeof(resList) / sizeof(uint16_t);

   *dataArrayP = lwm2m_data_new(nbRes);
    if (*dataArrayP == NULL) 
      return LWM2M_500_INTERNAL_SERVER_ERROR;

   *numDataP = nbRes;

    for (i = 0; i < nbRes; i++)
    {
      (*dataArrayP)[i].id = resList[i];
      // As interface is not available , so disabling , once interface available need to enable
#if 0  
      //  VZ_REQ_OTADMLWM2M_41068 Requirement     
      if((*dataArrayP)[i].id == RES_O_AVL_POWER_SOURCES)
      {
        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;           
        (*dataArrayP)[i].value.asChildren.count = AVL_POWER_SOURCES;    
      }
      else if ((*dataArrayP)[i].id == RES_O_POWER_SOURCE_VOLTAGE)
      {
        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;           
        (*dataArrayP)[i].value.asChildren.count = AVL_POWER_SOURCES;    
      }
      else if((*dataArrayP)[i].id == RES_O_POWER_SOURCE_CURRENT)
      {
        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;           
        (*dataArrayP)[i].value.asChildren.count = AVL_POWER_SOURCES;    
      }
      else {}
#endif         
      if ((*dataArrayP)[i].id == RES_M_ERROR_CODE)
      {
        lwm2m_data_t *data = NULL;

        data = lwm2m_data_new(1);
        if (data == NULL)
        {
          return LWM2M_500_INTERNAL_SERVER_ERROR;
        }
        data->id = RES_M_ERROR_CODE;

        result = prv_set_value(data, (device_data_t*)(objectP->userData), INVALID_RES_INST_ID, serverP);
        if(result != LWM2M_205_CONTENT)
        {
          lwm2m_data_free(1, data);
          data = NULL;
          return result;
        }

        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
        (*dataArrayP)[i].value.asChildren.count = data->value.asChildren.count;

        lwm2m_data_free(data->value.asChildren.count, data);
        data = NULL;
      }
      else if((*dataArrayP)[i].id == RES_O_EXT_DEVICE_INFO)
      {
        uint32_t cntr = 0;
        ext_device_info_t *ext_dev_info = ((device_data_t *)(objectP->userData))->ext_dev_info;

        while (ext_dev_info)
        {
          ext_dev_info = ext_dev_info->next;
          cntr++;
        }

        (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
        (*dataArrayP)[i].value.asChildren.count = cntr;
      }
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
        case RES_O_AVL_POWER_SOURCES:
          (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;           
          (*dataArrayP)[i].value.asChildren.count = AVL_POWER_SOURCES;    
        case RES_O_POWER_SOURCE_VOLTAGE:
          (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;           
          (*dataArrayP)[i].value.asChildren.count = AVL_POWER_SOURCES;
        case RES_O_POWER_SOURCE_CURRENT:
          (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;           
          (*dataArrayP)[i].value.asChildren.count = AVL_POWER_SOURCES;
#endif   
        case RES_M_ERROR_CODE:
        {
          lwm2m_data_t *data = NULL;
  
          data = lwm2m_data_new(1);
          if (data == NULL)
          {
            return LWM2M_500_INTERNAL_SERVER_ERROR;
          }
          data->id = RES_M_ERROR_CODE;
  
          result = prv_set_value(data, (device_data_t*)(objectP->userData), INVALID_RES_INST_ID, serverP);
          if(result != LWM2M_205_CONTENT)
          {
            lwm2m_data_free(1, data);
            data = NULL;
            return result;
          }
  
          (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
          (*dataArrayP)[i].value.asChildren.count = data->value.asChildren.count;
  
          lwm2m_data_free(data->value.asChildren.count, data);
          data = NULL;
          break;
        }         

        case RES_O_EXT_DEVICE_INFO:
        {
          uint32_t cntr = 0;
          ext_device_info_t *ext_dev_info = ((device_data_t *)(objectP->userData))->ext_dev_info;
          
          while (ext_dev_info)
          {
            ext_dev_info = ext_dev_info->next;
            cntr++;
          }
          
          (*dataArrayP)[i].type = LWM2M_TYPE_MULTIPLE_RESOURCE;
          (*dataArrayP)[i].value.asChildren.count = cntr;

          break;
        }   

        case RES_O_MANUFACTURER:
        case RES_O_MODEL_NUMBER:
        case RES_O_SERIAL_NUMBER:
        case RES_O_FIRMWARE_VERSION:
        case RES_M_REBOOT:
        case RES_O_FACTORY_RESET:
        case RES_O_BATTERY_LEVEL:
        case RES_O_MEMORY_FREE:
        case RES_O_RESET_ERROR_CODE:
        case RES_O_CURRENT_TIME:
        case RES_O_UTC_OFFSET:
        case RES_O_TIMEZONE:
        case RES_M_BINDING_MODES:
        case RES_O_MEMORY_TOTAL:
        case RES_O_BATTERY_STATUS:
          break;
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
static uint8_t prv_device_write_resources(lwm2m_object_t * objectP,
    int numData,
                                            lwm2m_data_t *dataArray,
    uint16_t resInstId, uint8_t write_method)
{
  int i = 0;
  uint8_t result = LWM2M_205_CONTENT;
  time_julian_type julian ;
  int64_t value = 0;
  if( dataArray == NULL || objectP == NULL )
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  
  i = 0;

  do
  {
    switch (dataArray[i].id)
    {
      case RES_O_CURRENT_TIME:
        if (1 == lwm2m_data_decode_int(dataArray + i, &(value)))
        {
          if (value < GPS_TO_UNIX_TIME_OFFSET)
          {  
            ds_iot_ulog_err("Given time value is less than standard gps to unix offset value");
            return LWM2M_400_BAD_REQUEST;   
          }            
          (((device_data_t*)(objectP->userData))->time)  = value;
          memset(&julian, 0 , sizeof(time_julian_type));
          time_jul_from_secs(((((device_data_t*)(objectP->userData))->time) - GPS_TO_UNIX_TIME_OFFSET) , &julian);
          time_set_julian(&julian);

          ((device_data_t*)(objectP->userData))->time = (int64_t)time_get_secs();
          result = LWM2M_204_CHANGED;
        }
        else
        {
          result = LWM2M_400_BAD_REQUEST;
        }
        break;

      case RES_O_MANUFACTURER:
      case RES_O_MODEL_NUMBER:
      case RES_O_SERIAL_NUMBER:
      case RES_O_FIRMWARE_VERSION:
      case RES_M_REBOOT:
      case RES_O_FACTORY_RESET:
      case RES_O_BATTERY_LEVEL:
      case RES_O_MEMORY_FREE:
      case RES_O_RESET_ERROR_CODE:
      case RES_O_UTC_OFFSET:
      case RES_O_TIMEZONE:
      case RES_M_BINDING_MODES:
      case RES_O_MEMORY_TOTAL:
      case RES_O_BATTERY_STATUS:
      case RES_O_HARDWARE_VERSION:
      case RES_O_SOFTWARE_VERSION:
      case RES_O_DEVICE_TYPE:
      case RES_O_EXT_DEVICE_INFO:
          /*If create/write have one more one resource to write we write on reablable resource*/
          if(numData > 1)
            result = LWM2M_204_CHANGED;
          else
            result = LWM2M_405_METHOD_NOT_ALLOWED;
        break;
      default:
        /*If create/write have one more one resource to write we write on unknown/notsupported resource*/
        if(numData > 1)
          result = LWM2M_204_CHANGED;
        else
          result = LWM2M_404_NOT_FOUND;
    }

    i++;
  } while (i < numData && result == LWM2M_204_CHANGED);
  
	return result;
  
}


/**
 * @fn static uint8_t prv_device_write()
 *
 * @brief This fucntion implements the write operation performed 
 *        on Device object
 *
 * @param instanceId Instance ID of the object to be read
 * @param numData number of resources in the array
 * @param dataArray resource value array
 * @param objectP Object to be read
 *
 * @return LWM2M response type
 */
static uint8_t prv_device_write(uint16_t instanceId,
    int numData,
    lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP,
    uint16_t resInstId, uint8_t write_method, bool bs_true)
{
  uint8_t result = LWM2M_205_CONTENT;
  if( dataArray == NULL || objectP == NULL ) 
  {
    ds_iot_ulog_err("fun [prv_device_write] LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  // this is a single instance object
  if (instanceId != 0)
  {
    return LWM2M_404_NOT_FOUND;
  }
  result = prv_device_create_write_check(numData, dataArray, bs_true);
  if (result != LWM2M_204_CHANGED )
  {
    ds_iot_ulog_err ("Write operation failed due to write on Readable resource.");
    return result;
        }

  result = prv_device_write_resources(objectP, numData, dataArray,
                                        resInstId, write_method);
  return result;
}

/**
 * @fn static uint8_t prv_device_execute()
 *
 * @brief This fucntion implements the execute operation performed 
 *        on Device object
 *
 * @param instanceId Instance ID of the object to be read
 * @param resourceId Resource ID of the resource to be executed
 * @param buffer message payload from server fro execute operation
 * @param length message payload length
 * @param objectP Object on which the operation needs to be executed
 *
 * @return LWM2M response type
 *
 */
static uint8_t prv_device_execute(uint16_t instanceId,
    uint16_t resourceId,
    uint8_t * buffer,
    int length,
    lwm2m_object_t * objectP)
{
  // this is a single instance object
  if( objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  if (instanceId != 0)
  {
    return LWM2M_404_NOT_FOUND;
  }

  switch (resourceId)
  {
    case RES_M_REBOOT:
      ds_iot_ulog_high("REBOOT");
      qurt_signal_set(&lwm2m_signal_Handle, LWM2M_REBOOT_SET);
      //return LWM2M_NO_ERROR;
      return LWM2M_204_CHANGED;
    case RES_O_FACTORY_RESET:
      qurt_signal_set(&lwm2m_signal_Handle, LWM2M_FACTORY_RESET_SET);
      ds_iot_ulog_high("FACTORY RESET");
      return LWM2M_204_CHANGED;
    case RES_O_RESET_ERROR_CODE:
      ds_iot_ulog_high("RESET ERROR CODE");
      ((device_data_t*)(objectP->userData))->error = 0;
      return LWM2M_204_CHANGED;

    case RES_O_MANUFACTURER:
    case RES_O_MODEL_NUMBER:
    case RES_O_SERIAL_NUMBER:
    case RES_O_FIRMWARE_VERSION:
    case RES_O_CURRENT_TIME:
    case RES_O_BATTERY_LEVEL:
    case RES_O_MEMORY_FREE:
    case RES_O_UTC_OFFSET:
    case RES_O_TIMEZONE:
    case RES_M_BINDING_MODES:
    case RES_O_MEMORY_TOTAL:
    case RES_O_BATTERY_STATUS:
    case RES_O_HARDWARE_VERSION:
    case RES_O_SOFTWARE_VERSION:
    case RES_O_DEVICE_TYPE:
    case RES_O_EXT_DEVICE_INFO:
      return LWM2M_405_METHOD_NOT_ALLOWED;
    default:
      return LWM2M_404_NOT_FOUND;
  }
}

/*
 * @fn      static int prv_update_value()
 * @brief   This function is used to update the particular resource value received from application
 * @param   lwm2m_data - pointer to lwm2m object information
 targetP    - pointer to lwm2m object
 resourceID - resource number
 * @return  on success - 1
 on error   - 0
 */
static int prv_update_value(lwm2m_object_data_t * lwm2m_data, lwm2m_object_t * targetP, uint8_t resourceID)
{
  int res = 0;
  bool val_changed = false;

  if (lwm2m_data->instance_info->resource_info->type == LWM2M_TYPE_STRING)
  { 
    uint8_t *res_value = NULL;

    switch (resourceID)
    {
      case RES_O_MANUFACTURER:
        res_value = ((device_data_t *)targetP->userData)->mnfctre_code;
        break;
  
      case RES_O_MODEL_NUMBER:
        res_value = ((device_data_t *)targetP->userData)->model_no;
        break;
  
      case RES_O_SERIAL_NUMBER:
        res_value = ((device_data_t *)targetP->userData)->serial_no;
        break;
  
      case RES_O_FIRMWARE_VERSION:
        res_value = ((device_data_t *)targetP->userData)->fm_version;
        break;
  
      case RES_O_HARDWARE_VERSION:
        res_value = ((device_data_t *)targetP->userData)->hw_version;
        break;
  
      case RES_O_SOFTWARE_VERSION:
        res_value = ((device_data_t *)targetP->userData)->sw_version;
        break;
   
      case RES_O_DEVICE_TYPE:
        res_value = ((device_data_t *)targetP->userData)->device_type;
        break;
    }
  
    if(lwm2m_strcmp((char *)res_value, (char *)lwm2m_data->instance_info->resource_info->value.asBuffer.buffer) != 0)
    {
      val_changed = true;
    }
    if ((NULL!= res_value) && strcmp((const char *)res_value, PRV_EMPTY))
      lwm2m_free(res_value);
    
    res_value = lwm2m_malloc((sizeof(uint8_t)*lwm2m_data->instance_info->resource_info->value.asBuffer.length));
  
    if (res_value == NULL)
    {
      ds_iot_ulog_high("LWM2M_LOG: Memory Allocation FAILED");
      return res;
    }

    memset(res_value, 0x00, (sizeof(uint8_t)*lwm2m_data->instance_info->resource_info->value.asBuffer.length));
    memcpy(res_value, (const char *)lwm2m_data->instance_info->resource_info->value.asBuffer.buffer,
           lwm2m_data->instance_info->resource_info->value.asBuffer.length);

    switch (resourceID)
    {
      case RES_O_MANUFACTURER:
        ((device_data_t *)targetP->userData)->mnfctre_code = res_value;
		((device_data_t *)targetP->userData)->isFromApp_mnfctre_code = LWM2M_TRUE;
        break;

      case RES_O_MODEL_NUMBER:
        ((device_data_t *)targetP->userData)->model_no = res_value;
		((device_data_t *)targetP->userData)->isFromApp_model_no = LWM2M_TRUE;
        break;

      case RES_O_SERIAL_NUMBER:
        ((device_data_t *)targetP->userData)->serial_no = res_value;
		((device_data_t *)targetP->userData)->isFromApp_serial_no = LWM2M_TRUE;
        break;

      case RES_O_FIRMWARE_VERSION:
        ((device_data_t *)targetP->userData)->fm_version = res_value;
		((device_data_t *)targetP->userData)->isFromApp_fm_version = LWM2M_TRUE;
        break;

      case RES_O_HARDWARE_VERSION:
        ((device_data_t *)targetP->userData)->hw_version = res_value;
		((device_data_t *)targetP->userData)->isFromApp_hw_version = LWM2M_TRUE;
        break;

      case RES_O_SOFTWARE_VERSION:
        ((device_data_t *)targetP->userData)->sw_version = res_value;
		((device_data_t *)targetP->userData)->isFromApp_sw_version = LWM2M_TRUE;
        break;

      case RES_O_DEVICE_TYPE:
        ((device_data_t *)targetP->userData)->device_type = res_value;
		((device_data_t *)targetP->userData)->isFromApp_device_type = LWM2M_TRUE;
        break;
    }  
    if(val_changed == true)
    {
      store_device_persistent_info((device_data_t *)targetP->userData);
      resource_value_change(lwm2m_data);
    }
    res = 1;
  }
  else if ((LWM2M_TYPE_OBJECT_LINK == lwm2m_data->instance_info->resource_info->type) &&
           (RES_O_EXT_DEVICE_INFO == resourceID))
  {
    ext_device_info_t *new_dev_info = NULL;
    ext_device_info_t **head_dev_info = &(((device_data_t *)targetP->userData)->ext_dev_info);
    ext_device_info_t *curr_dev_info = *head_dev_info; 

    /* Skip duplicate links */    
    while (curr_dev_info)
    {
      if (curr_dev_info->link_info == lwm2m_data->instance_info->resource_info->value.asObjLink)
        return 1;      

      curr_dev_info = curr_dev_info->next;
    }

    curr_dev_info = *head_dev_info;
    
    new_dev_info = lwm2m_malloc(sizeof(ext_device_info_t));  
    if (!new_dev_info)
    {
      ds_iot_ulog_err("LWM2M_LOG: Memory Allocation FAILED for external device information resource");
      return res;
    }

    memset(new_dev_info, 0x00, sizeof(ext_device_info_t));
    new_dev_info->link_info = lwm2m_data->instance_info->resource_info->value.asObjLink;

    if (!curr_dev_info)
    {
      *head_dev_info = new_dev_info;
    }
    else 
    {
      ext_device_info_t *prev_dev_info = NULL;

      while (curr_dev_info)
      {
        prev_dev_info = curr_dev_info;
        curr_dev_info = curr_dev_info->next;
      }

      if (prev_dev_info)
        prev_dev_info->next = new_dev_info;
    }
    res = 1; 
  }
  else
  {
    return res;
  }

  return res;
}

/*
 * @fn      static int32_t prv_device_setvalue ()
 * @brief   This function is used to set the particular resource value from application
 * @param   lwm2m_data - pointer to lwm2m object information
 targetP    - pointer to lwm2m object 
 * @return  on success - 1
 on error   - 0
 */
static int32_t prv_device_setvalue(lwm2m_object_data_t * lwm2m_data, lwm2m_object_t * targetP)
{
  int result = 0;

  if(lwm2m_data == NULL || targetP == NULL || lwm2m_data->no_instances != 1 ||
      lwm2m_data->instance_info == NULL || lwm2m_data->instance_info->resource_info == NULL)
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return 0;
  }

  switch(lwm2m_data->instance_info->resource_info->resource_ID) 
  {
  /*
    case RES_O_MANUFACTURER:
      result = prv_update_value(lwm2m_data, targetP, RES_O_MANUFACTURER);
      break;

    case RES_O_MODEL_NUMBER:
      result = prv_update_value(lwm2m_data, targetP, RES_O_MODEL_NUMBER);
      break;

    case RES_O_SERIAL_NUMBER:
      result = prv_update_value(lwm2m_data, targetP, RES_O_SERIAL_NUMBER);
      break;

    case RES_O_FIRMWARE_VERSION:
      result = prv_update_value(lwm2m_data, targetP, RES_O_FIRMWARE_VERSION);
      break;

    case RES_O_HARDWARE_VERSION:
      result = prv_update_value(lwm2m_data, targetP, RES_O_HARDWARE_VERSION);
      break;

    case RES_O_SOFTWARE_VERSION:
      result = prv_update_value(lwm2m_data, targetP, RES_O_SOFTWARE_VERSION);
      break;
  
    case RES_O_DEVICE_TYPE:
      result = prv_update_value(lwm2m_data, targetP, RES_O_DEVICE_TYPE);
      break;

    case RES_O_EXT_DEVICE_INFO:
      result = prv_update_value(lwm2m_data, targetP, RES_O_EXT_DEVICE_INFO);
      break;
	*/
	case RES_O_MANUFACTURER:
	case RES_O_MODEL_NUMBER:
  	case RES_O_SERIAL_NUMBER:
	case RES_O_FIRMWARE_VERSION:
	case RES_O_HARDWARE_VERSION:
	case RES_O_SOFTWARE_VERSION:
	case RES_O_DEVICE_TYPE:
	case RES_O_EXT_DEVICE_INFO:	
		result = prv_update_value(lwm2m_data, targetP,(uint8_t) lwm2m_data->instance_info->resource_info->resource_ID);
      	break;
    default:
      ds_iot_ulog_err("Write not supported for resource in Device Object");
      result = 0;
      break;
  }
  return result;
}

/*
 * @fn      static int32_t prv_device_getvalue ()
 * @brief   Function is used to get the particular resource value and pass it to the application.
 * @param   lwm2m_data - pointer to lwm2m object information
 targetP    - pointer to lwm2m object 
 * @return  on success - 1
 on error   - 0
 */
int32_t prv_device_getvalue(lwm2m_id_info_t id_info, lwm2m_object_data_t **lwm2m_dataP, lwm2m_object_t *targetP
                                     , lwm2m_resource_info_t **resource_info)
{
  int8_t result = -1;
  lwm2m_data_t * dataArrayP = NULL;
  //lwm2m_resource_info_t *resource_info = NULL;
  int numData = 0;
  uint16_t id_number = 0;

  if(lwm2m_dataP == NULL)
    return -1;

  if((result = prv_init_getvalue(id_info,*lwm2m_dataP, resource_info, &dataArrayP, &numData))==-1)
  	return result;
  id_number = id_info.instance_ID;
	result = prv_device_read(id_number, &numData, &dataArrayP, targetP,INVALID_RES_INST_ID, NULL );

  if (result != LWM2M_205_CONTENT)
  {
    result = -1;
    goto GET_ERROR;
  }

  if((result = prv_getvalue_resource_populate(lwm2m_dataP, dataArrayP, resource_info, numData))==-1)
     goto GET_ERROR;

  result = 0;

GET_ERROR:
  /* Resources allocated for lwm2m_dataP are cleaned-up by the caller */
  if (dataArrayP)
    lwm2m_free(dataArrayP);

  return result;
}

/**
 * @fn static uint8_t prv_device_create()
 *
 * @brief This fucntion is used to create Device object
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

static uint8_t prv_device_create(uint16_t instanceId,
    int numData,
    lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP, bool apiCall)
{
  device_data_t *device_instance = NULL;
  // int8_t hr = 0, min = 0;
  uint8_t result = 0;
  uint8_t battery_threshold = 0;
  mem_heap_totals_type amss_mem_heap_stats = {0};
  qapi_Status_t batt_status = QAPI_OK;
  uint16_t mandatory_resource[] = {RES_M_ERROR_CODE, RES_M_BINDING_MODES};
  uint8_t size = sizeof(mandatory_resource)/sizeof(mandatory_resource[0]);
  if( dataArray == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  /*check if already one instance is created */
  if(objectP->instanceList != NULL)
  {
    ds_iot_ulog_err("Device object instance already exist");
    return LWM2M_400_BAD_REQUEST;
  }
  if(apiCall == TRUE)
  {
     goto skip_check;
  }
  result = prv_mandatory_check(numData, dataArray, mandatory_resource, size);
  if(result != LWM2M_NO_ERROR)
		return result;
  result = prv_device_create_write_check(numData, dataArray, TRUE);
  if (result != LWM2M_204_CHANGED )
  {
    ds_iot_ulog_err ("Creating instance failed ");
    return result;
  }
skip_check:
  device_instance = (device_data_t *)lwm2m_calloc(1, sizeof(device_data_t));
  if (NULL == device_instance) return LWM2M_500_INTERNAL_SERVER_ERROR;

  memset(device_instance, 0, sizeof(device_data_t));
  device_instance->instanceId = instanceId;

  device_instance->battery_status = 0;
  LOG_DEBUG("battery status :%d",device_instance->battery_status);

  mem_heap_get_totals(&amss_mem_heap, &amss_mem_heap_stats);
  device_instance->free_memory   = (amss_mem_heap_stats.free_bytes / 1024) ;           // To convert into KBs
  device_instance->total_memory   = (amss_mem_heap_stats.total_physical_bytes / 1024); // To convert into KBs

  device_instance->error = PRV_ERROR_CODE;

  device_instance->time = (int64_t)time_get_secs();   
  LOG_DEBUG(" time in secs :%d ",device_instance->time);
  device_instance->serial_no = get_device_serial_number();
  if(NULL == device_instance->serial_no)
    device_instance->serial_no = PRV_EMPTY;
  LOG_DEBUG("serial number :%s ",device_instance->serial_no);

  /** fetch the device manufacturer */
  device_instance->mnfctre_code = get_device_manufacturer();
  if(NULL == device_instance->mnfctre_code)
    device_instance->mnfctre_code = PRV_EMPTY;
  LOG_DEBUG("manufacture code :%s ",device_instance->mnfctre_code);

  /** Fetch the hardware version */
  device_instance->hw_version = get_device_hw_version();
  if(NULL == device_instance->hw_version)
    device_instance->hw_version = PRV_EMPTY;
  LOG_DEBUG(" Hardware version :%s",device_instance->hw_version);

  /** Fetch the software version */
  device_instance->sw_version = get_device_sw_info(QAPI_DEVICE_INFO_OS_VERSION_E);
  if(NULL == device_instance->sw_version)
    device_instance->sw_version = PRV_EMPTY;
  LOG_DEBUG(" software version :%s",device_instance->sw_version);

  /** Fetch the firmware version */
  device_instance->fm_version = get_device_fw_version();
  if(NULL == device_instance->fm_version)
    device_instance->fm_version = PRV_EMPTY;
  LOG_DEBUG("fimware version :%s ",device_instance->fm_version);
  /** Fetch the model number of the device */
  device_instance->model_no =  get_device_model_number();
  if(NULL == device_instance->model_no)
    device_instance->model_no = PRV_EMPTY;
  LOG_DEBUG(" Model Number :%s ",device_instance->model_no);

  /** Fetch the time offset value  */
  prv_device_get_utc_offset_n_time_zone(device_instance);

  batt_status = qapi_Pmapp_Vbatt_Get_Battery_Status(&device_instance->battery_level);

  /* Battery Level changes */
  if(( QAPI_OK != batt_status ) && ( QAPI_ERR_BATT_ABSENT != batt_status))
  {
    ds_iot_ulog_err("battery_level fetch failed");
  }
  else
  {
    if(QAPI_ERR_BATT_ABSENT == batt_status) 
    {
      device_instance->battery_level = 0;
      device_instance->battery_status = BATTERY_STATUS_NOT_PRESENT;
    }
    else
    {
      battery_threshold = get_battery_level_threshold();
      LOG_DEBUG("battery_threshold: %d", battery_threshold);
      if( device_instance->battery_level < battery_threshold )
      {
        device_instance->battery_status = BATTERY_STATUS_LOW;
      }
      else
      {
        device_instance->battery_status = BATTERY_STATUS_NORMAL;
      }
    }
  }
  
  /** Fetch the device type */
  device_instance->device_type = (uint8_t *)lwm2m_malloc(strlen(M2M_DEVICE_TYPE_STR) + 1);
  if(device_instance->device_type == NULL)
  {
    device_instance->device_type = PRV_EMPTY;
  }
  else
  {
    memset(device_instance->device_type, 0x00, strlen(M2M_DEVICE_TYPE_STR) + 1);
    memcpy(device_instance->device_type, M2M_DEVICE_TYPE_STR, strlen(M2M_DEVICE_TYPE_STR) + 1);
  }
  objectP->instanceList = LWM2M_LIST_ADD(objectP->instanceList, device_instance);  
  objectP->userData = device_instance;

  result = prv_device_write_resources(objectP, numData, dataArray, 
                                          INVALID_RES_INST_ID, QAPI_COAP_POST);
  if (result != LWM2M_204_CHANGED)
  {
    // Issue in creating object instanace so retuning Bad request error.
    result = LWM2M_400_BAD_REQUEST;
    lwm2m_free(device_instance);
    objectP->instanceList = NULL;
    objectP->userData = NULL;
    return result;
  }

  efs_unlink(LWM2M_DEVICE_OBJ_PERSISTENCE_PATH);
  return LWM2M_201_CREATED;
}

#if 0
/**
 * @fn static uint8_t prv_device_delete()
 *
 * @brief This fucntion is used to delete Device object
 *
 * @param id Instance ID of the object to be deleted
 * @param objectP Object to be deleted
 *
 * @return LWM2M response type
 */
static uint8_t prv_device_delete(uint16_t id,
    lwm2m_object_t * objectP)
{
  int fd = 0;
  device_data_t *device_instance = NULL;

  if( objectP == NULL ) 
  {
    LOG_ERROR("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }


  objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&device_instance);
  if (NULL == device_instance) return LWM2M_404_NOT_FOUND;

  /*  Update single instance object persistence */

  fd = efs_open(LWM2M_DEVICE_OBJ_PERSISTENCE_PATH, O_CREAT, 00744);
  if( EFAILURE == fd)
  {
    LOG_DEBUG("Failed to open single_instance_obj_pr");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
  efs_close(fd);
  obj_created = TRUE;
  free_object_device(objectP);
  return LWM2M_202_DELETED;
}
#endif


/**
 * @fn device_obj_release_object_link
 *
 * @brief Clean-up the object link associated object instance. 
 *
 * @param obj_id  Object ID 
 *        inst_id Instance ID 
 *
 * @return None
 *
 */
void device_obj_release_object_link(uint16_t obj_id, uint16_t inst_id)
{
  lwm2m_object_t * targetP = NULL;
  lwm2m_context_t * contextP = NULL;
  uint32_t obj_link = (((uint32_t)obj_id << 16) | inst_id);  
  ext_device_info_t *prev_dev_info = NULL;
  ext_device_info_t **head_dev_info = NULL;
  ext_device_info_t *curr_dev_info = NULL; 

  /* Validate the input parameters */ 
  if ((0xFFFF == obj_id) && (0xFFFF == inst_id))
  {
    ds_iot_ulog_err2("Invalid input paramters - obj_id[%d], inst_id[%d]", obj_id, inst_id);
    return;
  }

  contextP = data.lwm2mH;
  if(contextP == NULL)
  {
    return;
  }

  targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, LWM2M_DEVICE_OBJECT_ID);
  if (NULL == targetP)
  {
    LOG_DEBUG("Object information[%d] is not valid!", LWM2M_DEVICE_OBJECT_ID);
    return;
  }

  qurt_mutex_lock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);

  head_dev_info = &(((device_data_t *)targetP->userData)->ext_dev_info);
  curr_dev_info = *head_dev_info; 

  /* Clean-up the object link associated with the object instance */   
  if ((curr_dev_info) &&
      (curr_dev_info->link_info == obj_link))
  {
   *head_dev_info = (*head_dev_info)->next;    
  }
  else
  {
    while (curr_dev_info)
    {
      if (curr_dev_info->link_info == obj_link)
      {
        prev_dev_info->next = curr_dev_info->next;
        break; 
      }
 
      prev_dev_info = curr_dev_info;      
      curr_dev_info = curr_dev_info->next;
    }    
  } 

  if (!curr_dev_info)
  {
    qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);
    return;
  }
    
  lwm2m_free(curr_dev_info); 
  qurt_mutex_unlock(&data.glwm2m_app_ctx.lwm2m_app_global_mutex);

  return;
}


/**
 * @fn lwm2m_object_t * get_object_device()
 *
 * @brief This function is used to get the information regarding the Device
 *        object during client registartion
 *
 * @param void
 *
 * @return Device object 
 *
 */
lwm2m_object_t * get_object_device()
{
  /*
   * The get_object_device function create the object itself and return a pointer to the structure that represent it.
   */
  lwm2m_object_t * deviceObj = NULL;
  int fd = 0;
  uint8_t battery_threshold = 0;
  qapi_Status_t batt_status = QAPI_OK;
  
  mem_heap_totals_type amss_mem_heap_stats = {0};
  deviceObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));


  if (NULL != deviceObj)
  {
    device_data_t *instanceP = NULL;
    memset(deviceObj, 0, sizeof(lwm2m_object_t));

    /*
     * It assigns his unique ID
     * The 3 is the standard ID for the mandatory object "Object device".
     */
    deviceObj->objID = LWM2M_DEVICE_OBJECT_ID;

    /*
     * And the private function that will access the object.
     * Those function will be called when a read/write/execute query is made by the server. In fact the library don't need to
     * know the resources of the object, only the server does.
     */
    deviceObj->readFunc       = prv_device_read;
    deviceObj->discoverFunc   = prv_device_discover;
    deviceObj->writeFunc      = prv_device_write;
    deviceObj->executeFunc    = prv_device_execute;
    deviceObj->createFunc     = prv_device_create;
    //deviceObj->deleteFunc   = prv_device_delete;
    deviceObj->setValueFunc   = prv_device_setvalue;
    deviceObj->get_Value_Func = prv_device_getvalue;

    /*  Check if device_obj_single_instance_pr file exist */
    ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_DEVICE_OBJ_PERSISTENCE_PATH);
    fd = efs_open(LWM2M_DEVICE_OBJ_PERSISTENCE_PATH, O_RDONLY);
    if(fd != EFAILURE)
    {
      deviceObj->userData = NULL;
      efs_close(fd);
      return deviceObj;
    }

    /*
     * and its unique instance
     *
     */
    instanceP = (device_data_t *)lwm2m_malloc(sizeof(device_data_t));
    if (NULL != instanceP)
    {
      memset(instanceP, 0, sizeof(device_data_t));
    }
    else
    {
      lwm2m_free(deviceObj);
      return NULL;
    }
    deviceObj->userData   = instanceP;//lwm2m_malloc(sizeof(device_data_t));

    //load data from persistent file
	load_device_persistent_info(instanceP);

    /*
     * Also some user data can be stored in the object with a private structure containing the needed variables 
     */
    //if (NULL != deviceObj->userData)
    {
      mem_heap_get_totals(&amss_mem_heap, &amss_mem_heap_stats);
      instanceP->free_memory   = (amss_mem_heap_stats.free_bytes / 1024) ;           // To convert into KBs
      instanceP->total_memory   = (amss_mem_heap_stats.total_physical_bytes / 1024); // To convert into KBs

      instanceP->error = PRV_ERROR_CODE;

      //     instanceP->time  = 1367491215;
      instanceP->time = (int64_t)time_get_secs();   
      LOG_DEBUG(" time in secs :%d ",instanceP->time);

	  if(!instanceP->isFromApp_serial_no)
        instanceP->serial_no = get_device_serial_number();
      if(NULL == instanceP->serial_no)
        instanceP->serial_no = PRV_EMPTY;
      LOG_DEBUG("serial number :%s ",instanceP->serial_no);

      /** fetch the device manufacturer */
	  if(!instanceP->isFromApp_mnfctre_code)
        instanceP->mnfctre_code = get_device_manufacturer();
      if(NULL == instanceP->mnfctre_code)
        instanceP->mnfctre_code = PRV_EMPTY;
      LOG_DEBUG("manufacture code :%s ",instanceP->mnfctre_code);

      /** Fetch the hardware version */
	  if(!instanceP->isFromApp_hw_version)
        instanceP->hw_version = get_device_hw_version();
      if(NULL == instanceP->hw_version)
        instanceP->hw_version = PRV_EMPTY;
      LOG_DEBUG(" Hardware version :%s",instanceP->hw_version);

      /** Fetch the software version */
	  if(!instanceP->isFromApp_sw_version)
        instanceP->sw_version = get_device_sw_info(QAPI_DEVICE_INFO_OS_VERSION_E);
      if(NULL == instanceP->sw_version)
        instanceP->sw_version = PRV_EMPTY;
      LOG_DEBUG(" software version :%s",instanceP->sw_version);

      /** Fetch the firmware version */
	  if(!instanceP->isFromApp_fm_version)
        instanceP->fm_version = get_device_fw_version();
      if(NULL == instanceP->fm_version)
        instanceP->fm_version = PRV_EMPTY;
      LOG_DEBUG("fimware version :%s ",instanceP->fm_version);
      /** Fetch the model number of the device */
	  if(!instanceP->isFromApp_model_no)
        instanceP->model_no =  get_device_model_number();
      if(NULL == instanceP->model_no)
        instanceP->model_no = PRV_EMPTY;
      LOG_DEBUG(" Model Number :%s ",instanceP->model_no);

      /** Fetch the time offset value  */
      prv_device_get_utc_offset_n_time_zone(instanceP);

      /* Battery Level changes */
      instanceP->battery_status = 0;
      batt_status = qapi_Pmapp_Vbatt_Get_Battery_Status(&instanceP->battery_level);

      if(( QAPI_OK != batt_status ) && ( QAPI_ERR_BATT_ABSENT != batt_status ))
      {
        ds_iot_ulog_err1("battery_level fetch failed %d err ", batt_status);
      }
      else{
        if(QAPI_ERR_BATT_ABSENT == batt_status)
        {
          instanceP->battery_status = BATTERY_STATUS_NOT_PRESENT;
        }
        else
        {
          battery_threshold = get_battery_level_threshold();
          LOG_DEBUG("Battery_level: %d", instanceP->battery_level);
          LOG_DEBUG("battery_threshold: %d", battery_threshold);
          if( instanceP->battery_level < battery_threshold )
          {
            instanceP->battery_status = BATTERY_STATUS_LOW;
          }
          else
          {
            instanceP->battery_status = BATTERY_STATUS_NORMAL;
          }
        }
      }
	  /** Fetch the device type */	  
	  if(!instanceP->isFromApp_device_type)
	    instanceP->device_type = (uint8_t *)lwm2m_malloc(strlen(M2M_DEVICE_TYPE_STR) + 1);
	  if(instanceP->device_type == NULL)
      {
        instanceP->device_type = PRV_EMPTY;
      }
      else if(!instanceP->isFromApp_device_type)
	  {
	    memset(instanceP->device_type, 0x00, strlen(M2M_DEVICE_TYPE_STR) + 1);
        memcpy(instanceP->device_type, M2M_DEVICE_TYPE_STR, strlen(M2M_DEVICE_TYPE_STR) + 1);
	  }
    }
    deviceObj->instanceList = LWM2M_LIST_ADD(deviceObj->instanceList, instanceP);
#if 0
    else
    {
      lwm2m_free(deviceObj->instanceList);
      lwm2m_free(deviceObj);
      deviceObj = NULL;
    }
#endif
  }

  return deviceObj;
}

/**
 * @fn void free_object_device()
 *
 * @brief This fucntion frees the Device object allocated
 * @param ObjectP Object to free
 *
 * @return void
 */
void free_object_device(lwm2m_object_t * objectP)
{
  device_data_t *instanceP =  NULL;
  if( objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return ;
  }

  if (NULL != objectP->userData)
  {
    instanceP = objectP->userData;  
    if(NULL != instanceP->serial_no 
        && strcmp((const char *)instanceP->serial_no, PRV_EMPTY))
    {
      lwm2m_free(instanceP->serial_no);
    }
    if(NULL != instanceP->hw_version 
        && strcmp((const char *)instanceP->hw_version, PRV_EMPTY))
    {
      lwm2m_free(instanceP->hw_version);
    }
    if(NULL != instanceP->sw_version 
        && strcmp((const char *)instanceP->sw_version, PRV_EMPTY))
    {
      lwm2m_free(instanceP->sw_version);
    }
    if(NULL != instanceP->fm_version 
        && strcmp((const char *)instanceP->fm_version, PRV_EMPTY))
    {
      lwm2m_free(instanceP->fm_version);
    }
    if(NULL != instanceP->mnfctre_code 
        && strcmp((const char *)instanceP->mnfctre_code, PRV_EMPTY))
    {
      lwm2m_free(instanceP->mnfctre_code);
    }
    if(NULL != instanceP->model_no 
        && strcmp((const char *)instanceP->model_no, PRV_EMPTY))
    {
      lwm2m_free(instanceP->model_no);
    }
	if(NULL != instanceP->device_type 
        && strcmp((const char *)instanceP->device_type, PRV_EMPTY))
    {
      lwm2m_free(instanceP->device_type);
    }
    //lwm2m_free(objectP->userData);  
    objectP->userData = NULL;
  }
  if (NULL != objectP->instanceList)
  {
    lwm2m_free(objectP->instanceList);
    objectP->instanceList = NULL;
  }

  if( !obj_created )
    lwm2m_free(objectP);
}

/**
 * @fn uint8_t device_change()
 *
 * @brief This fucntion is used to populate the values of the resource
 *        to be provided to the server during the read operation on the object.
 * @param dataArray data array for the resource 
 * @param objectP handle to device object 
 * @return LWM2M response type
 */
uint8_t device_change(lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP)
{
  uint8_t result = LWM2M_NO_ERROR;
  int64_t value = 0;
  int64_t free_memory = 0;
  int64_t total_memory = 0;
  if( dataArray == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__,nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  switch (dataArray->id)
  {
    case RES_O_BATTERY_LEVEL:
      {

        if (1 == lwm2m_data_decode_int(dataArray, &value))
        {
          if ((0 <= value) && (100 >= value) && (value != ((device_data_t*)(objectP->userData))->battery_level) )
          {
            ((device_data_t*)(objectP->userData))->battery_level = value;
            ds_iot_ulog_high("Battery Value changed");
            result = LWM2M_204_CHANGED;
          }
          else
          {
            result = LWM2M_400_BAD_REQUEST;
          }
        }
        else
        {
          result = LWM2M_400_BAD_REQUEST;
        }
      }
      break;
    case RES_M_ERROR_CODE:
      if (1 == lwm2m_data_decode_int(dataArray, &((device_data_t*)(objectP->userData))->error))
      {
        result = LWM2M_204_CHANGED;
      }
      else
      {
        result = LWM2M_400_BAD_REQUEST;
      }
      break;
    case RES_O_MEMORY_FREE:
	  free_memory = ((device_data_t*)(objectP->userData))->free_memory;
	  if ( 1 == lwm2m_data_decode_int(dataArray, &((device_data_t*)(objectP->userData))->free_memory))
	  {
	    if (free_memory != ((device_data_t*)(objectP->userData))->free_memory)
	    {
          ds_iot_ulog_high(" memory free Value changed");
          result = LWM2M_204_CHANGED;
        }
	    else
	    {
          result = LWM2M_400_BAD_REQUEST;
        }
	  }
      else
      {
        result = LWM2M_400_BAD_REQUEST;
      }
      break;
	case RES_O_MEMORY_TOTAL:
	  total_memory = ((device_data_t*)(objectP->userData))->total_memory;
	  if (1 == lwm2m_data_decode_int(dataArray, &((device_data_t*)(objectP->userData))->total_memory))
	  {
	    if (total_memory != ((device_data_t*)(objectP->userData))->total_memory)
		{
	      ds_iot_ulog_high("Total free Value changed");
		  result = LWM2M_204_CHANGED;
	    }
		else
	    {
	      result = LWM2M_400_BAD_REQUEST;
		}
	  }
	  else
      {
        result = LWM2M_400_BAD_REQUEST;
      }
      break;
	default:
      result = LWM2M_405_METHOD_NOT_ALLOWED;
      break;
  }

  return result;
 }

