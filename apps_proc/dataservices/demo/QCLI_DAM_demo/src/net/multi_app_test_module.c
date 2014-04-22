/**************************************************************************************************************
 * Copyright © 2017-2021 Qualcomm Technologies, Inc.
 * @file  software_mgnt_app.c
 * @author  Qualcomm Technologies, Inc.
 * @brief File contains the utility api's for stub application
 *
 **************************************************************************************************************/

#include "multi_app_test_module.h"
#include "pal.h"

extern TX_BYTE_POOL *byte_pool_qcli;

extern bool dam_thrd_running[NO_OF_APPS];

bool dam_is_cmd_running[NO_OF_APPS] = {false};

#define CONSOLE_WRITE_STRING_LITERAL(__String__)    do { PAL_Console_Write(sizeof(__String__) - 1, (__String__)); } while(0)

/*===========================================================================

FUNCTION MULTI_APP_GET_CMD_BUFFER

DESCRIPTION
  This function is used to allocate a command buffer that the client can
  then enqueue into the command queue.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the command

SIDE EFFECTS
  None

=========================================================================*/
multi_app_cmd_t* multi_app_get_cmd_buffer(void)
{
  multi_app_cmd_t*                 cmd_ptr = NULL;

  LOG_INFO("multi_app_get_cmd_buffer \n ");

  tx_byte_allocate(byte_pool_qcli, (VOID **)&cmd_ptr, sizeof(multi_app_cmd_t), TX_NO_WAIT);

  if (cmd_ptr == NULL) 
  {
    LOG_INFO("Command buffer allocation failed");
  }

  return cmd_ptr;
}

multi_app_cmd_parser_t* 
multi_app_cmd_parser_func(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, boolean is_lwm2m_ext_cmd)
{
  multi_app_cmd_parser_t* cmd_parser = NULL;

  uint32_t Parameter_Count1 = 0;
  uint32_t Parameter_Count2 = 0;

  int app_id[NO_OF_APPS] = {0};
  int app_id_index[NO_OF_APPS] = {0};

  int i, j, k = 0;
  int offset = 0;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&cmd_parser, sizeof(multi_app_cmd_parser_t), TX_NO_WAIT);

  if(cmd_parser == NULL)
  {
    return cmd_parser;
  }

  memset(cmd_parser, 0, sizeof(multi_app_cmd_parser_t));

  for (i = 0; i < Parameter_Count; i++)
  {
    if (strcmp((char*)Parameter_List[i].String_Value, "-app") == 0)
    {
      if (i+1 < Parameter_Count)
      {
      if (Parameter_List[i+1].Integer_Is_Valid)
      {
        if (Parameter_List[i+1].Integer_Value == 0 || Parameter_List[i+1].Integer_Value  == 1)
        {
          app_id[k] = Parameter_List[i+1].Integer_Value + 1;
          app_id_index[k] = i;
          k++;
        }
        else
        {
          LOG_ERROR("App ID is not valid!! Valid Ids are: 0/1.");

          CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
          CONSOLE_WRITE_STRING_LITERAL("ERROR : App ID is not valid!! Valid Ids are: 0/1");
          CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);

          tx_byte_release(cmd_parser);
          return NULL;
        }
      }
      else
      {
        LOG_ERROR("-app option should be followed by a valid integer specifying the App ID.");
  
          CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
          CONSOLE_WRITE_STRING_LITERAL("ERROR : -app option should be followed by a valid integer specifying the App ID");
          CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);

          tx_byte_release(cmd_parser);
          return NULL;
        }
      }
      else
      {
        LOG_ERROR("-app option should be followed by a valid integer specifying the App ID.");
  
        CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
        CONSOLE_WRITE_STRING_LITERAL("ERROR : -app option should be followed by a valid integer specifying the App ID");
        CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);

        tx_byte_release(cmd_parser);
        return NULL;
      }
    }
  }

  offset = 2;

  if (k == 2)  /* Command given to be run on both Threads*/
  {
    Parameter_Count1 = app_id_index[1] - app_id_index[0] - offset;

    i = 0;
    for (j = (app_id_index[0] + offset); j < app_id_index[1]; j++)
    {
      tx_byte_allocate(byte_pool_qcli, (VOID **)&(cmd_parser->app_data1.Parameter_List[i].String_Value), (strlen((char *)Parameter_List[j].String_Value) + 1), TX_NO_WAIT);
      strlcpy((char *)cmd_parser->app_data1.Parameter_List[i].String_Value, (char *)Parameter_List[j].String_Value, strlen((char *)Parameter_List[j].String_Value) + 1);
      cmd_parser->app_data1.Parameter_List[i].Integer_Is_Valid = Parameter_List[j].Integer_Is_Valid;
      cmd_parser->app_data1.Parameter_List[i].Integer_Value = Parameter_List[j].Integer_Value;
      i++;
    }

    Parameter_Count2 = Parameter_Count - app_id_index[1] - offset;

    i = 0;
    for (j = (app_id_index[1] + offset); j < Parameter_Count; j++)
    {
      tx_byte_allocate(byte_pool_qcli, (VOID **)&(cmd_parser->app_data2.Parameter_List[i].String_Value), (strlen((char *)Parameter_List[j].String_Value) + 1), TX_NO_WAIT);
      strlcpy((char *)cmd_parser->app_data2.Parameter_List[i].String_Value, (char *)Parameter_List[j].String_Value, strlen((char *)Parameter_List[j].String_Value) + 1);
      cmd_parser->app_data2.Parameter_List[i].Integer_Is_Valid = Parameter_List[j].Integer_Is_Valid;
      cmd_parser->app_data2.Parameter_List[i].Integer_Value = Parameter_List[j].Integer_Value;
      i++;
    }
  }
  else if (k == 1) /* Command given to be run on one specific Thread */
  {
    Parameter_Count1 = Parameter_Count - app_id_index[0] - offset;

    i = 0;
    for (j = (app_id_index[0] + offset); j < Parameter_Count; j++)
    {
      tx_byte_allocate(byte_pool_qcli, (VOID **)&(cmd_parser->app_data1.Parameter_List[i].String_Value), (strlen((char *)Parameter_List[j].String_Value) + 1), TX_NO_WAIT);
      strlcpy((char *)cmd_parser->app_data1.Parameter_List[i].String_Value, (char *)Parameter_List[j].String_Value, strlen((char *)Parameter_List[j].String_Value) + 1);
      cmd_parser->app_data1.Parameter_List[i].Integer_Is_Valid = Parameter_List[j].Integer_Is_Valid;
      cmd_parser->app_data1.Parameter_List[i].Integer_Value = Parameter_List[j].Integer_Value;
      i++;
    }
  }
  else if (k == 0) /* Not specified app id. By default run on Thread 0 */
  {
    app_id[0] = 1;
    Parameter_Count1 = Parameter_Count;

    i = 0;
    for (j = 0; j < Parameter_Count; j++)
    {
      tx_byte_allocate(byte_pool_qcli, (VOID **)&(cmd_parser->app_data1.Parameter_List[i].String_Value), (strlen((char *)Parameter_List[j].String_Value) + 1), TX_NO_WAIT);
      strlcpy((char *)cmd_parser->app_data1.Parameter_List[i].String_Value, (char *)Parameter_List[j].String_Value, strlen((char *)Parameter_List[j].String_Value) + 1);
      cmd_parser->app_data1.Parameter_List[i].Integer_Is_Valid = Parameter_List[j].Integer_Is_Valid;
      cmd_parser->app_data1.Parameter_List[i].Integer_Value = Parameter_List[j].Integer_Value;
        i++;
    }
  }
  else
  {
    LOG_ERROR("Can give -app option atmost TWICE.");

    CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
    CONSOLE_WRITE_STRING_LITERAL("ERROR : Can give -app option atmost TWICE");
    CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);

    tx_byte_release(cmd_parser);
    return NULL;
  }

  cmd_parser->app_data1.app_id = app_id[0];
  cmd_parser->app_data2.app_id = app_id[1];
  cmd_parser->app_data1.Parameter_Count = Parameter_Count1;
  cmd_parser->app_data2.Parameter_Count = Parameter_Count2;

  return cmd_parser;
}

QCLI_Command_Status_t multi_app_cmd_queue_func(multi_app_cmd_t* cmd_ptr)
{
  if(!cmd_ptr)
  return QCLI_STATUS_ERROR_E;

  if (cmd_ptr->cmd_data.app_id == 0)
  {
    if(dam_thrd_running[0] == false)
    {
      LOG_INFO("Application 0 not registerd ");
      return QCLI_STATUS_SUCCESS_E;
    }

    if (dam_is_cmd_running[0] == false)
    {
    LOG_INFO("Enqueue the command into the command queue. ");
    multi_app_enqueue_cmd_t1(cmd_ptr);
    multi_app_set_signal_dam_t1();
  }
    else
    {
      LOG_ERROR("Cannot run the current command on App 0 as the previous command is in process\n");

      CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
      CONSOLE_WRITE_STRING_LITERAL("ERROR : Cannot run the current command on App 0 as the previous command is in process");
      CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);

      tx_byte_release(cmd_ptr);
      return QCLI_STATUS_ERROR_E;
    }

  }
  else if (cmd_ptr->cmd_data.app_id == 1)
  {
    if(dam_thrd_running[1] == false)
    {
       LOG_INFO("Application 1 not registerd ");
      return QCLI_STATUS_SUCCESS_E;
    }

    if (dam_is_cmd_running[1] == false)
    {
    LOG_INFO("Enqueue the command into the command queue. ");
    multi_app_enqueue_cmd_t2(cmd_ptr);
    multi_app_set_signal_dam_t2();
  }
  else
  {
      LOG_ERROR("Cannot run the current command on App 1 as the previous command is in process\n");

      CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);
      CONSOLE_WRITE_STRING_LITERAL("ERROR : Cannot run the current command on App 1 as the previous command is in process");
      CONSOLE_WRITE_STRING_LITERAL(PAL_OUTPUT_END_OF_LINE_STRING);

      tx_byte_release(cmd_ptr);
      return QCLI_STATUS_ERROR_E;
    }

  }
  else
  {
    LOG_INFO("Application id is not valid!! \n");
    return QCLI_STATUS_ERROR_E;
  }

  return QCLI_STATUS_SUCCESS_E;
}

QCLI_Command_Status_t multi_app_cmd_fill_func(  multi_app_cmd_parser_t *cmd_parser_temp)
{
  multi_app_cmd_t* cmd_ptr = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;
  int i;

  if(cmd_parser_temp->app_data1.app_id != 0) 
  {
    cmd_ptr = multi_app_get_cmd_buffer();
    if(cmd_ptr)
    {
      cmd_ptr->cmd_data.cmd_id = cmd_parser_temp->cmd_id;
      cmd_ptr->cmd_data.Command = cmd_parser_temp->Command;
      cmd_ptr->cmd_data.Command_Index = cmd_parser_temp->Command_Index;
      cmd_ptr->cmd_data.app_id = cmd_parser_temp->app_data1.app_id - 1;
      cmd_ptr->cmd_data.Parameter_Count = cmd_parser_temp->app_data1.Parameter_Count;
      memcpy(cmd_ptr->cmd_data.Parameter_List , cmd_parser_temp->app_data1.Parameter_List,sizeof(cmd_parser_temp->app_data1.Parameter_List));
      result = multi_app_cmd_queue_func(cmd_ptr);
    }
    else
    {
      result =  QCLI_STATUS_ERROR_E;
    }
  }

  if(cmd_parser_temp->app_data2.app_id != 0) 
  {
    cmd_ptr = multi_app_get_cmd_buffer();
    if(cmd_ptr)
    {
      cmd_ptr->cmd_data.cmd_id = cmd_parser_temp->cmd_id;
      cmd_ptr->cmd_data.Command = cmd_parser_temp->Command;
      cmd_ptr->cmd_data.Command_Index = cmd_parser_temp->Command_Index;
      cmd_ptr->cmd_data.app_id = cmd_parser_temp->app_data2.app_id - 1;
      cmd_ptr->cmd_data.Parameter_Count = cmd_parser_temp->app_data2.Parameter_Count;
      memcpy(cmd_ptr->cmd_data.Parameter_List , cmd_parser_temp->app_data2.Parameter_List,sizeof(cmd_parser_temp->app_data2.Parameter_List));
      result = multi_app_cmd_queue_func(cmd_ptr);
    }
    else
    {
      result =  QCLI_STATUS_ERROR_E;
    }
  }

  if (result != QCLI_STATUS_SUCCESS_E)
  {
    if (cmd_parser_temp->app_data1.app_id != 0)
    {
      if (cmd_parser_temp->app_data1.Parameter_List != NULL)
      {
        for (i = 0; i< cmd_parser_temp->app_data1.Parameter_Count; i++)
        {
          if (cmd_parser_temp->app_data1.Parameter_List[i].String_Value != NULL)
          {
            tx_byte_release(cmd_parser_temp->app_data1.Parameter_List[i].String_Value);
            cmd_parser_temp->app_data1.Parameter_List[i].String_Value = NULL;
          }
        }
      }
    }

    if (cmd_parser_temp->app_data2.app_id != 0)
    {
      if (cmd_parser_temp->app_data2.Parameter_List != NULL)
      {
        for (i = 0; i< cmd_parser_temp->app_data2.Parameter_Count; i++)
        {
          if (cmd_parser_temp->app_data2.Parameter_List[i].String_Value != NULL)
          {
            tx_byte_release(cmd_parser_temp->app_data2.Parameter_List[i].String_Value);
            cmd_parser_temp->app_data2.Parameter_List[i].String_Value = NULL;
          }
        }
      }
    }
  }

  tx_byte_release(cmd_parser_temp);

  return result;
}

