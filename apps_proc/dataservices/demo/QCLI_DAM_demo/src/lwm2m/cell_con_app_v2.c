/**************************************************************************************************************
 * Copyright © 2019  Qualcomm Technologies, Inc.
 * @file  cell_con_app_v2.c
 * @author  Qualcomm Technologies, Inc.
 * @brief File contains the implementation for cell nw conn object application
 *
 **************************************************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "qapi_fs.h"
#include "log_intf_module.h"
#include "qapi_lwm2m.h"
#include "cell_con_app_v2.h"
#include "utils_app_v2.h"
#include "multi_app_test_module.h"

#define PSM_TIMER        4200
#define ACTIVE_TIMER     1200
#define LWM2M_CELL_NW_CONN_INSTANCE_ID_0  0
#define LWM2M_APN_CONN_INSTANCE_ID_0  0


extern TX_BYTE_POOL *byte_pool_qcli;

//Application handler
qapi_Net_LWM2M_App_Handler_t cell_app_handler;

//Instance id list maintained by application.
std_obj_list_t *cell_inst_list;

/* Application CELL conn object persistence file */ 
#define LWM2M_CELL_CON_APP_PERSISTENCE_FILE      "/datatx/lwm2m/cell_con_app_obj_app_pr"

/**
 * @fn uint8_t store_cell_nw_list_persistent_info()
 * @brief This function is used to store the cell nw conn instance
 *        created by application into persistent storage.
 * 
 * @param id Instance id of object 11.
 * @param store To validate instance id 
 * 
 * @return return error code / SUCCESS 
 */
static int store_cell_nw_list_persistent_info(uint16_t id, bool store)
{
  int fd = 0, index = 0;
  std_obj_persist_list_t store_info;
  uint32_t nBytesWrote = 0;
  qapi_Status_t ret = QAPI_ERROR;
  qapi_FS_Offset_t actualOffset = 0;

  LOG_INFO("LWM2M_LOG: In store_cell_nw_list_persistent_info \n");

  if (NULL == cell_inst_list)
  {
    LOG_ERROR("LWM2M_LOG: null args passed\n");
    return -1;
  }
  ret = qapi_FS_Open( LWM2M_CELL_CON_APP_PERSISTENCE_FILE, QAPI_FS_O_CREAT_E | QAPI_FS_O_WRONLY_E , &fd);
  if (ret != QAPI_OK)
  {
    LOG_ERROR("LWM2M_LOG: failed to open cell nw conn app persistence file\n");
    return -1;
  }


  memset(&store_info, 0x00, sizeof(store_info));

  store_info.obj_id = QAPI_NET_LWM2M_CELL_NW_CONN_OBJECT_ID_E;
  store_info.id = id;
  store_info.isValid = store;

  /* lseek to reqd index and write the std_obj_persist_list_t structure in efs */
  ret = qapi_FS_Seek(fd, index * sizeof(std_obj_persist_list_t), QAPI_FS_SEEK_SET_E, &actualOffset);
  if (QAPI_OK != (qapi_FS_Write(fd, &store_info, sizeof(std_obj_persist_list_t), &nBytesWrote)))
  {
    LOG_ERROR("LWM2M_LOG: failed to store persistent value of cell conn app instance\n");
    qapi_FS_Close(fd);
    return -1;
  }

  qapi_FS_Close(fd);
  return 0;
}

/**
 * @fn uint8_t load_cell_con_list_persistent_info()
 * @brief This function is used to fetch the cell nw conn 
 *        instance list created by application from persistent storage
 * 
 * @param cell_inst_list pointer to cell conn object instance list.
 * 
 * @return return error code / SUCCESS 
 */
static int load_cell_nw_list_persistent_info(std_obj_list_t **cell_inst_list)
{
  int fd = 0;
  uint32_t nBytesRead = 0;
  qapi_Status_t ret;

  if(cell_inst_list == NULL)
  {
    LOG_ERROR("LWM2M_LOG: null args passed\n");
    return -1;
  }
  ret = qapi_FS_Open( LWM2M_CELL_CON_APP_PERSISTENCE_FILE, QAPI_FS_O_CREAT_E | QAPI_FS_O_WRONLY_E , &fd);
  if (ret != QAPI_OK)
  {
    LOG_ERROR("LWM2M_LOG: failed to open cell conn app persistence file\n");
    return -1;
  }

  while(1)
  {
    std_obj_persist_list_t info;
    std_obj_list_t *list = NULL;

    memset(&info, 0, sizeof(info));
    ret = qapi_FS_Read(fd, &info, sizeof(std_obj_persist_list_t), &nBytesRead);
    if (ret != QAPI_OK)
    {
      break;
    }

    /*Check if the structure entry in file is valid*/
    if(FALSE == info.isValid)
    {
      continue;
    }
    tx_byte_allocate(byte_pool_qcli, (VOID **)&list, sizeof(std_obj_list_t), TX_NO_WAIT);
    if(list == NULL)
    {
      ret = -1;
      break;
    }

    memset(list, 0, sizeof(std_obj_list_t));
    LOG_INFO("id is %d.\n", info.id);
    list->id= info.id;

    *cell_inst_list = std_obj_list_add(*cell_inst_list, list);
  }
  if (ret < 0)
  {
    qapi_FS_Close(fd);
    return -1;
  }

  // Free the list in case of failure.
  qapi_FS_Close(fd);
  return ret;
}


/**
 * @fn cell_con_object_callback()
 *
 * @brief This callback fucntion is called to notify different LwM2M events.
 * @param handler LWM2M application handler
 * @param lwm2m_srv_data  Request message data
 * @param user_data    User data
 * @return QAPI_OK on success
 *         QAPI_ERROR on failure 
 */
qapi_Status_t cell_con_object_callback(qapi_Net_LWM2M_App_Handler_t handler, 
                                       qapi_Net_LWM2M_Server_Data_t *lwm2m_srv_data,
                                       void                         *user_data)
{
  std_obj_cb_data_t *cb_data = NULL;
  multi_app_cmd_t*        cmd_ptr = NULL;

  if(handler == NULL || lwm2m_srv_data == NULL)
  {
    return QAPI_ERROR;
  }
  tx_byte_allocate(byte_pool_qcli, (VOID **)&cb_data, sizeof(std_obj_cb_data_t), TX_NO_WAIT);
  if(cb_data == NULL)
  {
    return QAPI_ERROR;
  }

  memset(cb_data, 0x00, sizeof(std_obj_cb_data_t));

  cb_data->handler = handler;

  if(lwm2m_srv_data != NULL)
  {
    if(lwm2m_srv_data->msg_type == QAPI_NET_LWM2M_INTERNAL_CLIENT_IND_E)
    {
      if(lwm2m_srv_data->event == QAPI_NET_LWM2M_SLEEP_E)
      {
        cb_data->event = LWM2M_SLEEP_E;
      }
      else if(lwm2m_srv_data->event == QAPI_NET_LWM2M_WAKEUP_E)
      {
        cb_data->event = LWM2M_WAKEUP_E;
      }
      else if(lwm2m_srv_data->event == QAPI_NET_LWM2M_DEVICE_REBOOT_E)
      {
        cb_data->event = LWM2M_DEVICE_REBOOT_E;
      }
      else if(lwm2m_srv_data->event == QAPI_NET_LWM2M_DEVICE_FACTORY_RESET_E)
      {
        cb_data->event = LWM2M_DEVICE_FACTORY_RESET_E;
      }
      else if(lwm2m_srv_data->event == QAPI_NET_LWM2M_DEVICE_REBOOTSTRAPPING_E)
      {
        cb_data->event = LWM2M_DEVICE_REBOOTSTRAPPING_E;
      }
      else if(lwm2m_srv_data->event == QAPI_NET_LWM2M_CLIENT_RESET_E)
      {
        cb_data->event = LWM2M_CLIENT_RESET_E;
      }
      else
      {
        if(cb_data)
        {
          if(cb_data->srv_data)
          {
            tx_byte_release(cb_data->srv_data);
            cb_data->srv_data = NULL;
          }
          tx_byte_release(cb_data);
          cb_data = NULL;
        }
        return QAPI_OK;
      }
    }
    else if(lwm2m_srv_data->msg_type == QAPI_NET_LWM2M_DELETE_ALL_REQ_E)
    {
      //cb_data->event = LWM2M_DELETE_ALL_REQ_E;
    }
    else if(lwm2m_srv_data->msg_type == QAPI_NET_LWM2M_DELETE_REQ_E)
    {
      tx_byte_allocate(byte_pool_qcli, (VOID **)&(cb_data->srv_data), sizeof(qapi_Net_LWM2M_Server_Data_t), TX_NO_WAIT);
      if(cb_data->srv_data != NULL)
      {
        memset(cb_data->srv_data, 0x00, sizeof(qapi_Net_LWM2M_Server_Data_t));
        memcpy(cb_data->srv_data, lwm2m_srv_data, sizeof(qapi_Net_LWM2M_Server_Data_t));
        cb_data->event = LWM2M_DELETE_REQ_E;
      }
    }
    else
    {
      if(cb_data)
      {
        if(cb_data->srv_data)
        {
          tx_byte_release(cb_data->srv_data);
          cb_data->srv_data = NULL;
        }
        tx_byte_release(cb_data);
        cb_data = NULL;
      }
      return QAPI_OK;
    }
  }
  else
  {
    if(cb_data)
    {
      if(cb_data->srv_data)
      {
        tx_byte_release(cb_data->srv_data);
        cb_data->srv_data = NULL;
      }
      tx_byte_release(cb_data);
      cb_data = NULL;
    }
    return QAPI_OK;
  }

  LOG_INFO("Switching from callback context");

  if((cmd_ptr = multi_app_get_cmd_buffer()) == NULL)
  {
    LOG_INFO("Cannot obtain command buffer");
    return QAPI_ERR_NO_ENTRY;
  }
  else
  {
    cmd_ptr->cmd_data.cmd_id              = LWM2M_CELL_CON_CALL_BACK_EVT;
    cmd_ptr->cmd_data.data               = (void *)cb_data;

    LOG_INFO("Enqueue the command into the command queue. ");
    multi_app_enqueue_cmd_t1(cmd_ptr);
  }
  multi_app_set_signal_dam_t1();

  return QAPI_OK;
}

//TODO add description and handle other scenarios
qapi_Status_t cell_con_handle_event(void *data)
{
  std_obj_cb_data_t *cb_data = NULL;

  if(data == NULL)
  {
    return QAPI_ERROR;
  }

  cb_data = (std_obj_cb_data_t *)data;

  switch(cb_data->event)
  {
    case LWM2M_SLEEP_E:
    case LWM2M_WAKEUP_E:
      break;
    case LWM2M_DEVICE_REBOOTSTRAPPING_E:
    case LWM2M_DEVICE_FACTORY_RESET_E:
    case LWM2M_CLIENT_RESET_E:
      cell_con_app_deregister_v2();
      qapi_FS_Unlink(LWM2M_CELL_CON_APP_PERSISTENCE_FILE);
      break;
    case LWM2M_DEVICE_REBOOT_E:
      break;
    case LWM2M_DELETE_ALL_REQ_E:
	case LWM2M_DELETE_REQ_E:
    {
      if(cell_inst_list)
      {
        cell_con_delete_instance_v2();
      }
    }
      break;
  }
  return QAPI_OK;
}


/**
 * @fn cell_con_init_v2()
 * @brief This function is used to initialise cell nw conn application.
 * @param void
 * @return void
 */
void cell_con_init_v2()
{
  void *user_data = NULL;

  if(cell_app_handler != NULL) 
  {
    LOG_INFO("cellular connectivity nw app already running.\n");
    return;
  }

  //Register cell con application with qapi lwm2m
  if(QAPI_OK == qapi_Net_LWM2M_Register_App_Extended(&cell_app_handler, user_data, cell_con_object_callback))
  {
    std_obj_list_t *inst_list = NULL;
	qapi_Net_LWM2M_Pass_Pool_Ptr(cell_app_handler,byte_pool_qcli);
    LOG_INFO("cellular connectivity nw app started\n ");

    if(load_cell_nw_list_persistent_info(&cell_inst_list) != 0)
      return;

    inst_list = cell_inst_list;

    if (inst_list != NULL)
    {
       cell_con_create_instance_v2();
    }
  }
  return;
}

/*
 * @fn     int32_t cell_con_create_instance_v2() 
 * @brief  This function is used to create the cell nw conn object instance 
 * @param  void
 * @return on success - 0
 *         on failure - -1
 */

int32_t cell_con_create_instance_v2() 
{
  qapi_Net_LWM2M_Data_v2_t lwm2m_data;
  qapi_Net_LWM2M_Resource_Info_t *resource_info = NULL;
  qapi_Net_LWM2M_Resource_Info_t *resource_info1 = NULL;
  qapi_Status_t result = QAPI_OK;

  if(cell_app_handler == NULL)
  {
    LOG_INFO("LWM2M_LOG : Create is not allowed as application is not started \n"); 
    return -2;
  }

  memset(&lwm2m_data , 0x00 , sizeof(qapi_Net_LWM2M_Data_v2_t));

  lwm2m_data.object_ID    = QAPI_NET_LWM2M_CELL_NW_CONN_OBJECT_ID_E;

  lwm2m_data.no_instances++;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_data.instance_info), sizeof(qapi_Net_LWM2M_Instance_Info_v2_t), TX_NO_WAIT);
  if (lwm2m_data.instance_info == NULL) 
  {
    LOG_INFO("LWM2M_LOG:MALLOC FAILURE\n");
    goto CREATE_ERROR;
  }
  memset(lwm2m_data.instance_info, 0x00, sizeof(qapi_Net_LWM2M_Instance_Info_v2_t));

  lwm2m_data.instance_info->instance_ID = LWM2M_CELL_NW_CONN_INSTANCE_ID_0;
  lwm2m_data.instance_info->no_resources++;
  tx_byte_allocate(byte_pool_qcli, (VOID **)&(lwm2m_data.instance_info->resource_info), sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);
  if(lwm2m_data.instance_info->resource_info == NULL)
  {
    LOG_INFO("LWM2M_LOG : MALLOC FAILURE \n");
    goto CREATE_ERROR;
  }
  memset(lwm2m_data.instance_info->resource_info, 0x00 , sizeof(qapi_Net_LWM2M_Resource_Info_t));

  lwm2m_data.instance_info->resource_info->type  = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  lwm2m_data.instance_info->resource_info->resource_ID = QAPI_NET_LWM2M_NW_CONN_O_PSM_TIMER_E;
  lwm2m_data.instance_info->resource_info->value.asInteger = PSM_TIMER;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&resource_info , sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);
  if(resource_info == NULL)
  {
    LOG_INFO("LWM2M_LOG : MALLOC FAILURE \n");
    goto CREATE_ERROR;
  }
  memset (resource_info, 0 , sizeof (qapi_Net_LWM2M_Resource_Info_t));

  lwm2m_data.instance_info->no_resources++;
  lwm2m_data.instance_info->resource_info->next = resource_info;
  resource_info->type  = QAPI_NET_LWM2M_TYPE_INTEGER_E;
  resource_info->resource_ID = QAPI_NET_LWM2M_NW_CONN_O_ACTIVE_TIMER_E;
  resource_info->value.asInteger = ACTIVE_TIMER;
  
  tx_byte_allocate(byte_pool_qcli, (VOID **)&resource_info1 , sizeof(qapi_Net_LWM2M_Resource_Info_t), TX_NO_WAIT);
  if(resource_info1 == NULL)
  {
    LOG_INFO("LWM2M_LOG : MALLOC FAILURE \n");
    goto CREATE_ERROR;
  }
  memset(resource_info1, 0 , sizeof(qapi_Net_LWM2M_Resource_Info_t));

  lwm2m_data.instance_info->no_resources++;
  resource_info->next = resource_info1;
  resource_info1->type  = QAPI_NET_LWM2M_TYPE_OBJECT_LINK;
  resource_info1->resource_ID = QAPI_NET_LWM2M_NW_CONN_M_ACTIVATED_PROF_NAME_E;
  resource_info1->value.asObjLink = (QAPI_NET_LWM2M_APN_CONN_OBJECT_ID_E << 16 | LWM2M_APN_CONN_INSTANCE_ID_0);

  LOG_INFO ("LWM2M_LOG : INVOKING THE OBJECT CREATE INSTANCE  \n");

  result = qapi_Net_LWM2M_Create_Object_Instance_v2(cell_app_handler, &lwm2m_data);
  if (result == QAPI_ERROR)
  {
    LOG_INFO("LWM2M_LOG : Create object instance got failed \n");
  }
  else if (result == QAPI_ERR_EXISTS)
  {
    LOG_INFO("LWM2M_LOG : Object instance already exists. \n");
    result = QAPI_ERR_EXISTS;
  }
  else
  {
    std_obj_list_t *new_entry = NULL;

	new_entry = std_obj_list_find(cell_inst_list, LWM2M_CELL_NW_CONN_INSTANCE_ID_0);

    if(new_entry == NULL)
    {

	  tx_byte_allocate(byte_pool_qcli, (VOID **)&new_entry, sizeof (std_obj_list_t), TX_NO_WAIT);
      if(new_entry == NULL)
      {
        cell_con_delete_instance_v2();
        goto CREATE_ERROR;
      }
      memset(new_entry, 0x00, sizeof(std_obj_list_t));
  
      new_entry->id = LWM2M_CELL_NW_CONN_INSTANCE_ID_0;
      cell_inst_list = std_obj_list_add(cell_inst_list, new_entry);
  
      /* Store the instance created in persistent file */
      store_cell_nw_list_persistent_info(LWM2M_CELL_NW_CONN_INSTANCE_ID_0, TRUE);
      LOG_INFO("LWM2M_LOG : Object instance [%d] created \n", LWM2M_CELL_NW_CONN_INSTANCE_ID_0);
    }
  }

  app_free_instance_info_v2(lwm2m_data.instance_info);
  return result;

/* Clean-up in case of failure */
CREATE_ERROR:
  if(lwm2m_data.instance_info!= NULL && lwm2m_data.instance_info->resource_info != NULL)
  {
    app_free_resource_info_v2(lwm2m_data.instance_info->resource_info);
  }
  if(lwm2m_data.instance_info != NULL)
  {
    app_free_instance_info_v2(lwm2m_data.instance_info);
  }

  return result;
}

/*
 * @fn     int32_t cell_con_delete_instance_v2() 
 * @brief  This function is used to delete the cell nw conn object instance 
 * @param  void
 * @return on success - 0
 *         on failure - -1
 */

int32_t cell_con_delete_instance_v2()
{
  qapi_Status_t  result = QAPI_ERROR;
  qapi_Net_LWM2M_Object_Info_v2_t object_info;
  qapi_Net_LWM2M_Id_Info_v2_t *id_info = NULL;

  if(cell_app_handler == NULL) 
  {
    LOG_INFO("LWM2M_LOG : Delete is not allowed as application is not started \n");  
    return -1;
  }

  tx_byte_allocate(byte_pool_qcli, (VOID **)&id_info, sizeof (qapi_Net_LWM2M_Id_Info_v2_t), TX_NO_WAIT);
  if(id_info == NULL)
  {
    LOG_INFO("LWM2M : [CELL CON APPS] MALLOC FALIURE \n");
    goto DELETE_ERROR;
  }
  else
  {
    memset(&object_info , 0x00, sizeof(qapi_Net_LWM2M_Object_Info_v2_t));
    object_info.no_object_info++;
    memset(id_info , 0x00 , sizeof (qapi_Net_LWM2M_Id_Info_v2_t));
    id_info->id_set = id_info->id_set | QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info->id_set = id_info->id_set | QAPI_NET_LWM2M_INSTANCE_ID_E;
    id_info->object_ID = QAPI_NET_LWM2M_CELL_NW_CONN_OBJECT_ID_E;
    id_info->instance_ID = LWM2M_CELL_NW_CONN_INSTANCE_ID_0;

    id_info->next = NULL;
    object_info.id_info = id_info;
    object_info.id_info->next = NULL;

    result = qapi_Net_LWM2M_Delete_Object_Instance_v2(cell_app_handler, &object_info);

    if (result != QAPI_OK)
    {
      LOG_INFO("LWM2M_LOG : DELETE INSTANCE GOT FAILED \n");
      
      goto DELETE_ERROR;
    }
    else
    {
      std_obj_list_t *old_entry = NULL;

      /* Remove the instance delete in persistent file */
      store_cell_nw_list_persistent_info(LWM2M_CELL_NW_CONN_INSTANCE_ID_0, FALSE);

      cell_inst_list = std_obj_list_remove(cell_inst_list, LWM2M_CELL_NW_CONN_INSTANCE_ID_0, &old_entry);

      if(old_entry == NULL)
      {
        goto DELETE_ERROR;
      }

      tx_byte_release(old_entry);
      old_entry = NULL;
      LOG_INFO("LWM2M_LOG : Instance id [%d] deleted \n", LWM2M_CELL_NW_CONN_INSTANCE_ID_0);
    }
  }

/* Clean-up in case of failure */
DELETE_ERROR:
  if(id_info != NULL)
  {
    app_free_id_info_v2(id_info);
  }
  return result;
}

/**
 * @fn      cell_con_get_v2()
 *
 * @brief   This function is used to get resource value or resource values of instance
 *
 * @param   id_info Structure to indicate the object/instance/resource ID for which the 
 *                   application is interested in getting the value.

 * @return  QAPI_OK   on Success, 
 *          QAPI_ERR* on Failure
 */
qapi_Status_t cell_con_get_v2(qapi_Net_LWM2M_Id_Info_v2_t *id_info)
{
  qapi_Net_LWM2M_Data_v2_t *lwm2m_dataP = NULL;
  qapi_Status_t result = QAPI_ERROR;
  qapi_Net_LWM2M_Object_Info_v2_t lwm2m_info_req;

  if(cell_app_handler == NULL ) 
  {
    LOG_INFO( "LWM2M_LOG : Application not started \n");
    return QAPI_ERROR;
  }

  if(id_info == NULL)
  {
    LOG_INFO("Null parameter passed.");
    return QAPI_ERR_INVALID_PARAM;
  }
  if(id_info->object_ID != QAPI_NET_LWM2M_CELL_NW_CONN_OBJECT_ID_E)
  {
    LOG_INFO( "LWM2M_LOG : invalid object id  \n");
    return QAPI_ERROR;
  }
  memset (&lwm2m_info_req, 0x00, sizeof (qapi_Net_LWM2M_Object_Info_v2_t));
  lwm2m_info_req.no_object_info++;
  lwm2m_info_req.id_info = id_info; 

  result = qapi_Net_LWM2M_Get_v2(cell_app_handler, &lwm2m_info_req, &lwm2m_dataP);
  if (result == QAPI_ERROR)
  {
    LOG_INFO ("LWM2M_LOG : Get failed for cell nw conn!\n");
  }
  else 
  {
    app_display_obj_info_v2(lwm2m_dataP);
  }

  app_free_data_v2(lwm2m_dataP);
  return result;
}

/**
 * @fn cell_con_app_deregister_v2()
 * @brief This function is used to deregister cell nw conn application.
 * @return void
 */
void cell_con_app_deregister_v2(void)
{
  qapi_Status_t result = QAPI_OK;

  if(cell_app_handler)
  {
    std_obj_list_t *inst_list = cell_inst_list;
    while(inst_list)
    {
      std_obj_list_t *old_entry = NULL;
      /* Remove the instance delete in persistent file */
      store_cell_nw_list_persistent_info(inst_list->id, FALSE);

      cell_inst_list = std_obj_list_remove(cell_inst_list, inst_list->id, &old_entry);

      if(old_entry == NULL)
      {
        break;
      }

      tx_byte_release(old_entry);
      old_entry = NULL;

      inst_list = inst_list->next;
    }

    result = qapi_Net_LWM2M_DeRegister_App(cell_app_handler);
    if (result != QAPI_OK)
    {
      LOG_INFO ("LWM2M_LOG : cell nw conn handler is not valid \n");
    }
    else
    {
      cell_inst_list = NULL;
    }
    cell_app_handler = NULL;
  }
  else
  {
    LOG_INFO("Handler is not valid\n");
  }
  return;
}


