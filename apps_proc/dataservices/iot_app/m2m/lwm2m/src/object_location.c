/******************************************************************************

  @file    object_location.c

  ---------------------------------------------------------------------------

  Copyright (c) 2016-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

 *****************************************************************************/

/*******************************************************************************
 * 
 * Copyright (c) 2014 Bosch Software Innovations GmbH, Germany.
 *
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
 ******************************************************************************/
/*! \file
  LWM2M object "Location" implementation

  \author Joerg Hubschneider
  */

/*
 *  Object     |      | Multiple  |     | Description                   |
 *  Name       |  ID  | Instances |Mand.|                               |
 *-------------+------+-----------+-----+-------------------------------+
 *  Location   |   6  |    No     |  No |  see TS E.7 page 101          |
 *
 *  Resources:
 *  Name        | ID  | Oper.|Instances|Mand.|  Type   | Range | Units | Description                                                                      |
 * -------------+-----+------+---------+-----+---------+-------+-------+----------------------------------------------------------------------------------+
 *  Latitude    |  0  |  R   | Single  | Yes | String  |       |  Deg  | The decimal notation of latitude  e.g. -  45.5723  [Worlds Geodetic System 1984].|
 *  Longitude   |  1  |  R   | Single  | Yes | String  |       |  Deg  | The decimal notation of longitude e.g. - 153.21760 [Worlds Geodetic System 1984].|
 *  Altidude    |  2  |  R   | Single  | No  | String  |       |   m   | The decimal notation of altidude in meters above sea level.                      |
 *  Uncertainty |  3  |  R   | Single  | No  | String  |       |   m   | The accuracy of the position in meters.                                          |
 *  Velocity    |  4  |  R   | Single  | No  | Opaque  |       |   *   | The velocity of the device as defined in 3GPP 23.032 GAD specification(*).       |
 *              |     |      |         |     |         |       |       | This set of values may not be available if the device is static.                 |
 *              |     |      |         |     |         |       |       | opaque: see OMA_TS 6.3.2                                                         |
 *  Timestamp   |  5  |  R   | Single  | Yes | Time    |       |   s   | The timestamp when the location measurement was performed.                       |
 */

#include "liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "log_utils.h"
#include "object_location.h"
#include "ds_log.h"


#ifdef LWM2M_CLIENT_MODE


// ---- private "object location" specific defines ----
// Resource Id's:
#define RES_M_LATITUDE     0
#define RES_M_LONGITUDE    1
#define RES_O_ALTITUDE     2
#define RES_O_RADIUS       3
#define RES_O_VELOCITY     4
#define RES_M_TIMESTAMP    5
#define RES_O_SPEED        6

//-----  3GPP TS 23.032 V11.0.0(2012-09) ---------
#define HORIZONTAL_VELOCITY                  0  // for Octet-1 upper half(..<<4)
#define HORIZONTAL_VELOCITY_VERTICAL         1  // set vertical direction bit!
#define HORIZONTAL_VELOCITY_WITH_UNCERTAINTY 2

#define MILLI 1000

bool is_location_create = FALSE;
void free_object_location(lwm2m_object_t * object);
void location_setVelocity(lwm2m_object_t* locationObj, uint16_t bearing, uint16_t horizontalSpeed, uint8_t speedUncertainty);


/**
 * @fn static uint8_t prv_res2tlv()
 *
 * @brief This fucntion is used to populate the values of the resource
 *        to be provided to the server during the read operation on the object.
 * @param dataP data array for the resource 
 * @param locDataP Location object data
 * @return LWM2M response type
 */
static uint8_t prv_res2tlv(lwm2m_data_t* dataP,
    location_data_t* locDataP)
{
  //-------------------------------------------------------------------- JH --
  uint8_t ret = LWM2M_205_CONTENT;  
  if( dataP == NULL || locDataP == NULL ) 
  { 
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  switch (dataP->id)     // location resourceId
  {
    case RES_M_LATITUDE:
      lwm2m_data_encode_float(locDataP->latitude, dataP);
      break;
    case RES_M_LONGITUDE:
      lwm2m_data_encode_float(locDataP->longitude, dataP);
      break;
    case RES_O_ALTITUDE:
      lwm2m_data_encode_float(locDataP->altitude, dataP);
      break;
    case RES_O_RADIUS:
      lwm2m_data_encode_float(locDataP->radius,dataP);
      break;
    case RES_O_VELOCITY:
      lwm2m_data_encode_opaque(locDataP->velocity,VELOCITY_OCTETS,dataP);
      break;
    case RES_M_TIMESTAMP:
      lwm2m_data_encode_int((locDataP->timestamp)/MILLI, dataP);
      break;
    case RES_O_SPEED:
      lwm2m_data_encode_float(locDataP->speed, dataP);
      break;
    default:
      ret = LWM2M_404_NOT_FOUND;
      break;
  }

  return ret;
}


/**
 * Implementation (callback-) function of reading object resources. For whole
 * object, single resources or a sequence of resources
 * see 3GPP TS 23.032 V11.0.0(2012-09) page 23,24.
 * implemented for: HORIZONTAL_VELOCITY_WITH_UNCERTAINT
 * @param objInstId    in,     instances ID of the location object to read
 * @param numDataP     in/out, pointer to the number of resource to read.
 *                             0 is the exception for all readable resource
 *                             of object instance
 * @param tlvArrayP    in/out, TLV data sequence with initialized resource
 *                             ID to read
 * @param objectP      in,     private location data structure
 */
static uint8_t prv_location_read(uint16_t objInstId,
    int*  numDataP,
    lwm2m_data_t** tlvArrayP,
    lwm2m_object_t*  objectP,
    uint16_t resInstId, lwm2m_server_t *serverP)
{   
  int     i = 0;
  uint8_t result = LWM2M_500_INTERNAL_SERVER_ERROR;
  location_data_t* locDataP = NULL;
  float loc_bearing =0;
  float loc_horUncertainity=0;
  uint16_t hor_speed = 0;
  uint8_t hor_Uncertainity = 0;
  
  if( tlvArrayP == NULL || objectP == NULL || numDataP == NULL) 
  {
    ds_iot_ulog_err("fun [prv_location_read] LWM2M_LOG: NULL Error");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
  locDataP = (location_data_t*)(objectP->userData);

  // defined as single instance object!
  if (objInstId != 0) return LWM2M_404_NOT_FOUND;

  if (*numDataP == 0)     // full object, readable resources!
  {
    uint16_t readResIds[] = {
      RES_M_LATITUDE,
      RES_M_LONGITUDE,
      RES_O_ALTITUDE,
      RES_O_RADIUS,
      RES_O_VELOCITY,
      RES_M_TIMESTAMP,
      RES_O_SPEED
    }; // readable resources!

    *numDataP  = sizeof(readResIds)/sizeof(uint16_t);
    *tlvArrayP = lwm2m_data_new(*numDataP);
    if (*tlvArrayP == NULL) return LWM2M_500_INTERNAL_SERVER_ERROR;

    // init readable resource id's
    for (i = 0 ; i < *numDataP ; i++)
    {
      (*tlvArrayP)[i].id = readResIds[i];
    }
  }

  // Update the GPS information 

  get_m2m_gps_data(locDataP, &loc_bearing, &loc_horUncertainity);
  hor_speed = ((locDataP->speed)*18)/5;
  hor_Uncertainity = (loc_horUncertainity*18)/5;
  location_setVelocity(objectP,loc_bearing,hor_speed,hor_Uncertainity);

  for (i = 0 ; i < *numDataP ; i++)
  {
    result = prv_res2tlv ((*tlvArrayP)+i, locDataP);
    if (result!=LWM2M_205_CONTENT) break;
  }

  return result;
}

/**
 * Convenience function to set the velocity attributes.
 * see 3GPP TS 23.032 V11.0.0(2012-09) page 23,24.
 * implemented for: HORIZONTAL_VELOCITY_WITH_UNCERTAINTY
 * @param locationObj location object reference (to be casted!)
 * @param bearing          [Deg]  0 - 359    resolution: 1 degree
 * @param horizontalSpeed  [km/h] 1 - s^16-1 resolution: 1 km/h steps
 * @param speedUncertainty [km/h] 1-254      resolution: 1 km/h (255=undefined!)
 */
void location_setVelocity(lwm2m_object_t* locationObj,
    uint16_t bearing,
    uint16_t horizontalSpeed,
    uint8_t speedUncertainty)
{
  location_data_t* pData = NULL;
  if( locationObj == NULL ) 
  {
    ds_iot_ulog_err("fun [location_setVelocity] LWM2M_LOG: NULL Error");
    return ;
  }

  pData = locationObj->userData;
  pData->velocity[0] = HORIZONTAL_VELOCITY_WITH_UNCERTAINTY << 4;
  pData->velocity[0] |= (bearing & 0x100) >> 8;
  pData->velocity[1] = (bearing & 0x0FF);
  pData->velocity[2] = horizontalSpeed >> 8;
  pData->velocity[3] = horizontalSpeed & 0xff;
  pData->velocity[4] = speedUncertainty;
}

/**
 * @fn     static uint8_t prv_location_discover()
 *
 * @brief  This fucntion implements the discover operation performed on the
 *         location object
 *
 * @param  instanceId Instance ID of the object to be read
 * @param  numDataP number of resources in the array
 * @param  dataArrayP resource value array
 * @param  objectP Object to be read
 *
 * @return LWM2M response type
 *
 */

static uint8_t prv_location_discover(uint16_t instanceId,
    int * numDataP,
    lwm2m_data_t ** dataArrayP,
    lwm2m_object_t * objectP, lwm2m_server_t *serverP)
{
  uint8_t result = 0;
  int i = 0;

  if (numDataP == NULL || dataArrayP == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);  
    return  LWM2M_400_BAD_REQUEST;    
  }
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
      RES_M_LATITUDE ,
      RES_M_LONGITUDE ,    
      RES_O_ALTITUDE ,
      RES_O_RADIUS,
      RES_O_VELOCITY ,     
      RES_M_TIMESTAMP,
      RES_O_SPEED
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
        case RES_M_LATITUDE :
        case RES_M_LONGITUDE:     
        case RES_O_ALTITUDE:
        case RES_O_RADIUS:
        case RES_O_VELOCITY:           
        case RES_M_TIMESTAMP:
        case RES_O_SPEED:
          break;
        default:
          result = LWM2M_404_NOT_FOUND;
      }
    }
  }

  return result;
}

/**
 * @fn static uint8_t prv_location_create()
 *
 * @brief This fucntion is used to create Location object
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

static uint8_t prv_location_create(uint16_t instanceId,
    int numData,
    lwm2m_data_t * dataArray,
    lwm2m_object_t * objectP, bool apiCall)
{
  uint8_t result = LWM2M_NO_ERROR;
  location_data_t * location_instance = NULL;
  float loc_bearing = 0;
  float loc_horUncertainity=0;
  uint16_t hor_speed = 0;
  uint8_t hor_Uncertainity = 0;
  uint16_t mandatory_resource[] = {RES_M_LATITUDE, RES_M_LONGITUDE, RES_M_TIMESTAMP};
  uint8_t size = sizeof(mandatory_resource)/sizeof(mandatory_resource[0]);

  if( dataArray == NULL || objectP == NULL ) 
  {
    LOG_DEBUG("%s", nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  /*check if already one instance is created */
  if(objectP->instanceList != NULL)
  {
    LOG_DEBUG("Location object instance exist");
    return LWM2M_400_BAD_REQUEST;
  }
  result = prv_mandatory_check(numData, dataArray, mandatory_resource, size);
  if(result != LWM2M_NO_ERROR)
    return result;
  location_instance = (location_data_t *)lwm2m_calloc(1, sizeof(location_data_t));
  if(NULL == location_instance) return LWM2M_500_INTERNAL_SERVER_ERROR;

  memset(location_instance, 0, sizeof(location_data_t));
  location_instance->instanceId = instanceId;
  objectP->userData = location_instance;

  get_m2m_gps_data(location_instance, &loc_bearing, &loc_horUncertainity);    //Get the GPS data
  hor_speed = ((location_instance->speed)*18)/5;
  hor_Uncertainity = (loc_horUncertainity*18)/5;
  location_setVelocity(objectP, loc_bearing, hor_speed, hor_Uncertainity);   // 255: speedUncertainty not supported!

  location_instance->timestamp   = lwm2m_gettime();
  objectP->instanceList = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));
  if (NULL != objectP->instanceList)
  {
    memset(objectP->instanceList, 0x00, sizeof(lwm2m_list_t));
  }

  efs_unlink(LWM2M_LOCATION_OBJ_PERSISTENCE_PATH);
  return LWM2M_201_CREATED;
}

/**
 * @fn static uint8_t prv_location_delete()
 *
 * @brief This fucntion is used to delete Location object
 *
 * @param id Instance ID of the object to be deleted
 * @param objectP Object to be deleted
 *
 * @return LWM2M response type
 */
static uint8_t prv_location_delete(uint16_t id,
    lwm2m_object_t * objectP,
    bool persist_file)
{
  int fd = 0;
  location_data_t * location_instance = NULL;

  if( objectP == NULL ) 
  {
    LOG_DEBUG("%s",nullError);
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }

  objectP->instanceList = lwm2m_list_remove(objectP->instanceList, id, (lwm2m_list_t **)&location_instance);
  if (NULL == location_instance) return LWM2M_404_NOT_FOUND;

  lwm2m_free(location_instance);
  location_instance = NULL;

  if( NULL != objectP->userData)
  {
    lwm2m_free(objectP->userData);
    objectP->userData = NULL;
  }

  /*  Update single instance object persistence */

  if(persist_file)
  {
  ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_LOCATION_OBJ_PERSISTENCE_PATH);
  fd = efs_open( LWM2M_LOCATION_OBJ_PERSISTENCE_PATH, O_CREAT, 00744);
  
  if( EFAILURE == fd)
  {
    LOG_DEBUG("Failed to open single_instance_obj_pr");
    return LWM2M_500_INTERNAL_SERVER_ERROR;
  }
  efs_close(fd);
   }
  
  return LWM2M_202_DELETED;
}
#if 0
/**
 * @fn static uint8_t prv_location_execute()
 *
 * @brief This fucntion implements the execute operation performed on
 *        Location object
 *
 * @param instanceId (IN) Instance ID of the object to be read
 * @param resourceId (IN) Resource ID of the resource to be executed
 * @param buffer (IN) message payload from server fro execute operation
 * @param length (IN) message payload length
 * @param objectP Object on which the operation needs to be executed
 * @return LWM2M response type
 */
static uint8_t prv_location_execute(uint16_t instanceId,
                                    uint16_t resourceId,
                                    uint8_t * buffer,
                                    int length,
                                    lwm2m_object_t * objectP)
{
  location_data_t *instanceP = NULL;

  if ( objectP == NULL )
  {
    LOG_ERROR("%s", nullError);
    return LWM2M_400_BAD_REQUEST;
  }

  instanceP = (location_data_t *)LWM2M_LIST_FIND(objectP->instanceList, instanceId);

  if (NULL == instanceP)
  {
    return LWM2M_404_NOT_FOUND;
  }

  switch(resourceId)
  {
    case RES_M_LATITUDE:
    case RES_M_LONGITUDE:
    case RES_O_ALTITUDE:
    case RES_O_RADIUS:
    case RES_O_VELOCITY:
    case RES_M_TIMESTAMP:
    case RES_O_SPEED:
      return LWM2M_405_METHOD_NOT_ALLOWED;
    default:
      return LWM2M_404_NOT_FOUND;
  }
}
#endif

/*
 * @fn      static int32_t prv_location_setvalue ()
 * @brief   This function is used to set the particular resource value from appllication
 * @param   lwm2m_data - pointer to lwm2m object information
 targetP    - pointer to lwm2m object 
 * @return  on success - 1
 on error   - 0    
 */
static int32_t prv_location_setvalue (lwm2m_object_data_t * lwm2m_data, lwm2m_object_t * targetP)
{
  int result = 0;

  if (lwm2m_data == NULL || targetP == NULL || lwm2m_data->instance_info == NULL || lwm2m_data->instance_info->resource_info == NULL || targetP->userData == NULL)
  {
    ds_iot_ulog_err("LWM2M_LOG: NULL Error");
    return 0;
  }

  switch (lwm2m_data->instance_info->resource_info->resource_ID) 
  {
    case RES_O_RADIUS:
      {
        ((location_data_t*)targetP->userData)->radius = lwm2m_data->instance_info->resource_info->value.asFloat;
        ds_iot_ulog_high2("radius: %d.%d",(int)((location_data_t*)targetP->userData)->radius, ((int)((location_data_t*)targetP->userData)->radius*1000 )%1000 );
        result = 1;
      }
      break;
    default:
      break;
  }

  return result;
}

/**
 * @fn lwm2m_object_t * get_object_location()
 *
 * @brief This function is used to get the information regarding the Location
 *        object during client registartion
 *
 * @param void
 *
 * @return Location object  
 *
 */
lwm2m_object_t * get_object_location(void)
{
  lwm2m_object_t * locationObj = NULL;
  float loc_bearing = 0;
  int fd = 0;
  
  float loc_horUncertainity=0;
  uint16_t hor_speed = 0;
  uint8_t hor_Uncertainity = 0;

  locationObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
  if (NULL != locationObj)
  {
    memset(locationObj, 0, sizeof(lwm2m_object_t));

    /* It assigns its unique ID
     * The 6 is the standard ID for the optional object "Location".
     */
    locationObj->objID = LWM2M_LOCATION_OBJECT_ID;

    /* And the private function that will access the object.
     * Those function will be called when a read query is made by the server.
     * In fact the library don't need to know the resources of the object, only the server does.
     */
    locationObj->readFunc    = prv_location_read;
    locationObj->discoverFunc = prv_location_discover;
    locationObj->createFunc  = prv_location_create;
    locationObj->deleteFunc  = prv_location_delete;
    locationObj->executeFunc  = NULL;
    locationObj->setValueFunc = prv_location_setvalue;

    /*  Check if location_obj_single_instance_pr file exist */
    ds_iot_log_strf_med("EFS_OPT: File open %s",LWM2M_LOCATION_OBJ_PERSISTENCE_PATH);
    fd = efs_open(LWM2M_LOCATION_OBJ_PERSISTENCE_PATH, O_RDONLY);
    if(fd != EFAILURE)
    {
      locationObj->userData = NULL;
      efs_close(fd);
      return locationObj;
    }
    // and its unique instance
    locationObj->instanceList = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));
    if (NULL != locationObj->instanceList)
    {
      memset(locationObj->instanceList, 0, sizeof(lwm2m_list_t));
    }
    else
    {
      lwm2m_free(locationObj);
      return NULL;
    }

    locationObj->userData    = lwm2m_malloc(sizeof(location_data_t));

    // initialize private data structure containing the needed variables
    if (NULL != locationObj->userData)
    {
      location_data_t* data = (location_data_t*)locationObj->userData;
      get_m2m_gps_data(data, &loc_bearing, &loc_horUncertainity);   //Get the GPS data
      hor_speed = ((data->speed)*18)/5;
      hor_Uncertainity = (loc_horUncertainity*18)/5;
      location_setVelocity(locationObj, loc_bearing, hor_speed, hor_Uncertainity); // 255: speedUncertainty not supported!

      data->timestamp   = lwm2m_gettime();
    }
    else
    {
      lwm2m_free(locationObj);
      locationObj = NULL;
    }
  }

  return locationObj;
}

/**
 * @fn void free_object_location()
 *
 * @brief This fucntion frees the Host Device object allocated
 * @param ObjectP Object to free
 *
 * @return void
 */
void free_object_location(lwm2m_object_t * object)
{

  lwm2m_list_t* objlist ;

  if( object == NULL ) 
  {
    LOG_DEBUG("fun [%s] %s",__func__, nullError);
    return ;
  }
  objlist = object->instanceList;

  while(objlist)
  {
    prv_location_delete(objlist->id,object,false);
    objlist = object->instanceList;
  }

  lwm2m_free(object);
}

#endif  //LWM2M_CLIENT_MODE
