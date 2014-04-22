/**************************************************************************************************************
 * Copyright © 2017 Qualcomm Technologies, Inc.
 * @file  software_mgnt_app.c
 * @author  Qualcomm Technologies, Inc.
 * @brief File contains the utility api's for stub application
 *
 **************************************************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

#include "qapi_lwm2m.h"
#include "utils_app.h"
#include "log_intf_module.h"
#include "qcli.h"
#include "qcli_api.h"

#include "qapi_diag.h"

extern QCLI_Group_Handle_t qcli_m2m_stub_handle;

/**
 * @fn     int32_t app_free_id_info  ()
 * @brief  This API is used to Free the memory allocated for qapi lwm2m id info 
 * @param  id_infoP (IN) Pointer to memory allocated for id info to be freed 
 * @return  int32_t   0 on success
 *          -1 on failure
 */
int32_t app_free_id_info (qapi_Net_LWM2M_Id_Info_t *id_infoP)
{
  qapi_Net_LWM2M_Id_Info_t *idinfo_link = NULL; 
  if (id_infoP == NULL)
  {
    return -1;
  }
  while (id_infoP != NULL)
  {
    idinfo_link = id_infoP->next ;
    tx_byte_release (id_infoP);
    id_infoP = idinfo_link;
  }
  return 0;
}

/**
 * @fn     int32_t app_free_Data()  
 * @brief  This API is used to Free the memory allocated for qapi lwm2m data 
 * @param  dataP (IN)  Pointer to memory allocated for qapi lwm2m data  to be freed
 * @return  int32_t   0 on success
 *          -1 on failure
 */
int32_t app_free_data(qapi_Net_LWM2M_Data_t *dataP)
{
  if (dataP == NULL)
    return -1;
  
  if (dataP != NULL)
  {
    app_free_instance_info (dataP->instance_info);
    tx_byte_release (dataP);
  }

  return 0;
}


/**
 * @fn     int32_t app_free_instance_info()
 * @brief  This API is used to Free the memory allocated for qapi lwm2m instance 
 * @param  instanceP (IN) Pointer to memory allocated for qapi instance info to be freed 
 * @return  int32_t   0 on success
 *          -1 on failure
 */
int32_t app_free_instance_info(qapi_Net_LWM2M_Instance_Info_t *instanceP)
{
  if (instanceP == NULL)
   return -1;

  if (instanceP != NULL)
  {
    app_free_resource_info(instanceP->resource_info);
    tx_byte_release (instanceP);
  }

  return 0;
}


/**
 * @fn     int32_t  stub_app_free_resource_info ()
 * @brief  This API is used to Free the memory allocated for qapi lwm2m resource  
 * @param  resourceP (IN) Pointer to memory allocated for qapi  lwm2m resource info to be freed 
 * @return  int32_t   0 on success
 *          -1 on failure
 */
int32_t app_free_resource_info (qapi_Net_LWM2M_Resource_Info_t *resourceP)
{
  qapi_Net_LWM2M_Resource_Info_t *resource_linkP = NULL;

  if (resourceP == NULL)
    return -1;

  while (resourceP != NULL)
  {
    resource_linkP =   resourceP->next;
    if(resourceP->value.asBuffer.buffer)
    {
      tx_byte_release (resourceP->value.asBuffer.buffer);
    }
    tx_byte_release (resourceP);
    resourceP = resource_linkP;
  }

  return 0;
}


void app_display_obj_info(qapi_Net_LWM2M_Data_t *lwm2m_data)
{
  qapi_Net_LWM2M_Instance_Info_t *inst_info = NULL; 

  /* Validate the input parameters */
  if (!lwm2m_data)
    return;

  /* Object information */
  QCLI_Printf (qcli_m2m_stub_handle , "############# Object Information : %d ################\n", lwm2m_data->object_ID);
  QCLI_Printf (qcli_m2m_stub_handle , "Object Instances : %d ################\n", lwm2m_data->no_instances);
  
  /* Object instance information */
  inst_info = lwm2m_data->instance_info;
  while (inst_info)
  {
    qapi_Net_LWM2M_Resource_Info_t *res_info = NULL; 

    /* Object instance resource information */
    res_info = inst_info->resource_info;
    while (res_info)
    {
      switch (res_info->type)
      {
        case QAPI_NET_LWM2M_TYPE_STRING_E:           
        {
          uint8_t data_buf[128];

          memset(data_buf, 0x00, sizeof(data_buf));
          if(res_info->value.asBuffer.length)
          {
            strlcpy((void *)data_buf, (void *)res_info->value.asBuffer.buffer, res_info->value.asBuffer.length + 1);
          }
          else
          {
            res_info->value.asBuffer.buffer = NULL;
            res_info->value.asBuffer.buffer = 0;
          }
          QCLI_Printf (qcli_m2m_stub_handle , "Resource ID : %d,  Value : %s\n", res_info->resource_ID, data_buf);
          break;
        }  

        case QAPI_NET_LWM2M_TYPE_FLOAT_E:
        {
          QCLI_Printf (qcli_m2m_stub_handle , "Resource ID : %d,  Value : %ld\n", res_info->resource_ID, res_info->value.asFloat);
          break; 
        }

        case QAPI_NET_LWM2M_TYPE_BOOLEAN_E:
        {
          QCLI_Printf (qcli_m2m_stub_handle , "Resource ID : %d,  Value : %d\n", res_info->resource_ID, res_info->value.asBoolean);
          break;
        }

        case QAPI_NET_LWM2M_TYPE_INTEGER_E: 
        {
          QCLI_Printf (qcli_m2m_stub_handle , "Resource ID : %d,  Value : %lld\n", res_info->resource_ID, res_info->value.asInteger);
          break;
        }

        case QAPI_NET_LWM2M_TYPE_MULTIPLE_RESOURCE:
        {
          uint32_t cntr = 0;

          /* Only object link support */
          for (cntr = 0; cntr < res_info->value.asChildren.count; cntr++)
          {  
            if (QAPI_NET_LWM2M_TYPE_OBJECT_LINK == res_info->value.asChildren.array[cntr].type)
            {
              uint16_t obj_id = res_info->value.asChildren.array[cntr].value.asObjLink >> 16;
              uint16_t inst_id = res_info->value.asChildren.array[cntr].value.asObjLink & 0xFFFF;

              QCLI_Printf (qcli_m2m_stub_handle , "Resource ID : %d,  Value : [%d -> %d]\n", res_info->resource_ID, obj_id, inst_id);
            }
            else 
            {
              QCLI_Printf (qcli_m2m_stub_handle , "Resource ID : %d type(%d) not supported in test application(TO BE DONE)!\n", res_info->resource_ID, res_info->type);
            }
          }

          break;
        }

        default:
        {
          QCLI_Printf (qcli_m2m_stub_handle , "Resource ID : %d type(%d) not supported in test application(TO BE DONE)!\n", res_info->resource_ID, res_info->type);
          break; 
        }
      }
     
      res_info = res_info->next;   
    }
  
    inst_info = inst_info->next;
  } 
 
  return;  
}

