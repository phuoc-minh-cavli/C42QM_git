/**************************************************************************************************************
 * Copyright © 2017-2019 Qualcomm Technologies, Inc.
 * @file  lwm2m_main_module_v2.c
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
#include "qcli.h"
#include "qcli_api.h"

#include "qapi_lwm2m.h"
#include "lwm2m_main_module_v2.h"
#include "sw_mgnt_stub_app_v2.h"
#include "devicecap_stub_app_v2.h"
#include "device_stub_app_v2.h"
#include "apn_info_app_v2.h"
#include "cell_con_app_v2.h"

#include "multi_app_test_module.h"

QCLI_Group_Handle_t qcli_m2m_stub_handle_v2;
extern TX_BYTE_POOL *byte_pool_qcli;

extern bool dam_thrd_running[NO_OF_APPS];

const QCLI_Command_t m2m_stub_cmd_list_v2[] = {
  {devicecap_app_start_v2, false, "devicecap_app_start", "usage: device capability application [-app <app id: 0/1>] start", "Start Device cap application"},
  {devicecap_create_instance_v2, false , "devicecap_create" , "usage : create new instance [-app <app id: 0/1>]\n " , "Create new instance\n"},
  {devicecap_delete_instance_v2, false , "devicecap_delete" , "usage : give [-app <app id: 0/1>] instance id\n " , "give proper instance id \n"},
  {devicecap_observe_v2, false , "devicecap_observe" , "usage : give [-app <app id: 0/1>] instance id\n " , "give proper instance id \n"},
  {devicecap_cancel_observe_v2, false , "devicecap_cancel_observe" , "usage : give [-app <app id: 0/1>] instance id\n " , "give proper instance id \n"},
  {devicecap_set_notify_v2, false, "set_notify", "usage: set_notify [-app <app id: 0/1>] <instance id> <notify value (0/1)>", "Set notify value for any device capability object instance"},
  {devicecap_app_stop_v2, false, "devicecap_app_stop", "usage: device capability application stopped [-app <app id: 0/1>]", "Stop Device cap application"},
  {sw_mgnt_app_start_v2, false, "sw_mgnt_app_start", "usage: fw application start [-app <app id: 0/1>]", "Start firmware application"},
  {sw_mgnt_update_result_v2, false , "sw_mgnt_update_result" , "usage : update result [-app <app id: 0/1>] [3 , 9 ,51,52,54,3,56 ]\n " , "update  result  with proper value \n"} ,
  {sw_mgnt_delete_instance_v2, false , "sw_mgnt_delete" , "usage : give [-app <app id: 0/1>] instance id\n " , "give proper instance id \n"} ,
  {device_app_start_v2, false , "device_app_start" , " [-app <app id: 0/1>]" , " Start device stub application"} ,
  {device_app_set_resource_v2, false , "device_app_set_resource" , "Usage : [-app <app id: 0/1>] <resource_id> [0,1,2,3,17,18,19] <resource value>", 
    " Give access to write resource value for device resource id 0,1,2,3,17,18,19"} ,  
  {sw_mgnt_get_v2, false , "sw_mgnt_get" , "Usage : [-app <app id: 0/1>] <object id> <instance id>(optional) <resource id>(optional)", " Get functionality"},
  {devicecap_get_v2, false , "devicecap_get" , "Usage : [-app <app id: 0/1>] <object id> <instance id>(optional) <resource id>(optional)", " Get functionality"},
  {cell_nw_app_start_v2, false, "cell_nw_app_start", "usage: cell con nw application [-app <app id: 0/1>] start", "Start cellular connectivity nw  application"},
  {cell_nw_app_stop_v2, false, "cell_nw_app_stop", "usage: cell con nw application stopped [-app <app id: 0/1>]", "Stop  cellular connectivity nw application"},
  {cell_nw_get_v2, false , "cell_nw_get" , "Usage : [-app <app id: 0/1>] <object id> <instance id>(optional) <resource id>(optional)", " Get functionality"},
  {cell_nw_create_instance_v2, false , "cell_nw_create" , "usage : create new instance [-app <app id: 0/1>] instance id\n " , "Create new instance\n"},
  {cell_nw_delete_instance_v2, false , "cell_nw_delete" , "usage : give [-app <app id: 0/1>] instance id\n " , "Delete an instance \n"},
  {apninfo_app_start_v2, false, "apninfo_app_start", "usage: apn conn info application [-app <app id: 0/1>] start", "Start APN conn info application"},
  {apninfo_app_stop_v2, false, "apninfo_app_stop", "usage: apn conn info application stopped [-app <app id: 0/1>]", "Stop  APN conn info application"},
  {apninfo_get_v2, false , "apninfo_get" , "Usage : [-app <app id: 0/1>] <object id> <instance id>(optional) <resource id>(optional)", " Get functionality"},
  {apninfo_create_instance_v2, false , "apninfo_create" , "usage : create new instance [-app <app id: 0/1>] instance id\n " , "Create new instance\n"},
  {apninfo_delete_instance_v2, false , "apninfo_delete" , "usage : give [-app <app id: 0/1>] instance id\n " , "Delete an instance \n"},
  
  
};

const QCLI_Command_Group_t m2m_stub_cmd_group_v2 = {
  "LWM2M_v2", /* Group_String: will display cmd prompt as "PSM> " */
  sizeof(m2m_stub_cmd_list_v2) / sizeof(m2m_stub_cmd_list_v2[0]), /* Command_Count */
  m2m_stub_cmd_list_v2                                   /* Command_List */
};

/**
 * @fn void lwm2m_register_cli(void)
 * @brief This function is used to register to QCLI interface
 * 
 * @returns void
 */
void Initialize_LwM2M_Stub_Demo_v2(void)
{
  // Attempt to reqister the Command Groups with the qcli framework.
  qcli_m2m_stub_handle_v2 = QCLI_Register_Command_Group(NULL, &m2m_stub_cmd_group_v2);
  if(qcli_m2m_stub_handle_v2)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "LWM2m_Stub registered\n");
  }

  return;
}

QCLI_Command_Status_t sw_mgnt_delete_instance_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_SW_MGNT_DELETE_INSTANCE_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 13;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_sw_mgnt_delete_instance_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int32_t ret_value= 0; 
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (Parameter_Count < 1 ) 
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Invalid!!");
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }
  ret_value = sw_mgnt_handle_delete_instance_v2(Parameter_List[0].Integer_Value);
  if (ret_value == -1)
  { 
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "registration update triggered \n");
  }
  else
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , " [SW_MGNT]INSTANCE DELETED SUCCESSFULLY \n");
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

QCLI_Command_Status_t devicecap_delete_instance_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_DEVICECAP_DELETE_INSTANCE_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 6;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_devicecap_delete_instance_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int32_t ret_value= 0;
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (Parameter_Count < 1 ) 
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Invalid!!");
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }
  ret_value = devicecap_handle_delete_instance_v2(Parameter_List[0].Integer_Value);
  if (ret_value == -1)
  { 
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "Error in deleting instance \n");
  }
  else
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , " [Device CAP]INSTANCE DELETED SUCCESSFULLY \n");
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

QCLI_Command_Status_t devicecap_app_start_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_DEVICECAP_APP_START_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 4;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_devicecap_app_start_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  devicecap_init_v2();
  QCLI_Printf (qcli_m2m_stub_handle_v2 , "Device capability app started\n");

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

QCLI_Command_Status_t devicecap_app_stop_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_DEVICECAP_APP_STOP_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 10;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_devicecap_app_stop_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  devicecap_app_deregister_v2();
  QCLI_Printf (qcli_m2m_stub_handle_v2 , "Device capability app stopped\n");

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

QCLI_Command_Status_t devicecap_create_instance_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_DEVICECAP_CREATE_INSTANCE_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 5;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_devicecap_create_instance_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int result = -1;
  int i;
  QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;

  result = devicecap_handle_create_instance_v2();
  if(result == -2)
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "Device capability app not registered");
  }
  else if (result == -1)
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "Error in creating instance");
  }
  else if (result == 0)
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "Instance created successfully.");
  }

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
  return status;
}

QCLI_Command_Status_t devicecap_get_v2(uint32_t Parameter_Count,
                                    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_DEVICECAP_GET_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 17;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_devicecap_get_v2(uint32_t Parameter_Count,
                                    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int result = -1;
  int inst_id = -1;
  int res_id  = -1;
  int obj_id = -1;
  qapi_Net_LWM2M_Id_Info_v2_t id_info;
  int i;
  QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;

  memset(&id_info, 0x00, sizeof(qapi_Net_LWM2M_Id_Info_v2_t));

  if (Parameter_Count < 1 ) 
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Invalid!!");
    status = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    obj_id = Parameter_List[0].Integer_Value; 
  }

  if ((Parameter_Count > 1) && Parameter_List[1].Integer_Is_Valid)
  {
    inst_id = Parameter_List[1].Integer_Value;
  }

  if ((Parameter_Count > 2) && Parameter_List[2].Integer_Is_Valid)
  {
    res_id = Parameter_List[2].Integer_Value;
  }

  if(obj_id != -1)
  {
    id_info.id_set |= QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info.object_ID = obj_id;
    if(inst_id != -1)
    {
      id_info.id_set |= QAPI_NET_LWM2M_INSTANCE_ID_E;
      id_info.instance_ID = inst_id;
      if( res_id != -1)
      {
        id_info.id_set |= QAPI_NET_LWM2M_RESOURCE_ID_E;
        id_info.resource_ID = res_id;
      }
    }
  }

  result = device_cap_get_v2(&id_info);
  if (result == -1)
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "Error in devcap get");
  }
  else
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "Get operation successful.");
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
  return status;
}

QCLI_Command_Status_t devicecap_observe_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_DEVICECAP_OBSERVE_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 7;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_devicecap_observe_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int result = -1;
  int i;
  QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;

  if (Parameter_Count < 1 ) 
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Invalid!!");
    status = QCLI_STATUS_USAGE_E;
    goto end;
  }
  result = devicecap_handle_observe_v2(Parameter_List[0].Integer_Value);

  if (result == -1)
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "Error in observation");
  }
  else
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "Observe operation successful.");
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
  return status;
}

QCLI_Command_Status_t devicecap_cancel_observe_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_DEVICECAP_CANCEL_OBSERVE_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 8;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_devicecap_cancel_observe_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int result = -1;
  int i;
  QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;

  if (Parameter_Count < 1 ) 
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Invalid!!");
    status = QCLI_STATUS_USAGE_E;
    goto end;
  }
  result = devicecap_handle_cancel_observe_v2(Parameter_List[0].Integer_Value);

  if (result == -1)
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "Error in cancel observation");
  }
  else
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "Cancel Observe operation successful.");
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
  return status;
}

QCLI_Command_Status_t devicecap_set_notify_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_DEVICECAP_SET_NOTIFY_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 9;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_devicecap_set_notify_v2(uint32_t Parameter_Count,QCLI_Parameter_t* Parameter_List, int app_id)
{
  int result = 0;
  int i;
  QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;

  if (Parameter_Count < 2 || Parameter_List == NULL)
  {
    status = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if(!Parameter_List[0].Integer_Is_Valid)
  {
    status= QCLI_STATUS_ERROR_E;
    goto end;
  }

  if((Parameter_List[1].Integer_Value != 0 ) && (Parameter_List[1].Integer_Value != 1 ))
  {
    status = QCLI_STATUS_ERROR_E;
    goto end;
  }

  result = set_notify_en_value_v2(Parameter_List[0].Integer_Value, Parameter_List[1].Integer_Value);

  if(result == 1)
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "Notify_en resource value is set \n");
    status = QCLI_STATUS_SUCCESS_E;
    goto end;
  }
  else
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "Unable to set Notify_en resource value \n");
    status = QCLI_STATUS_SUCCESS_E;
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
  return status;
}

QCLI_Command_Status_t sw_mgnt_update_result_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_SW_MGNT_UPDATE_RESULT_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 12;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_sw_mgnt_update_result_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (Parameter_Count < 1 ) 
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Invalid!!");
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }
  sw_mgnt_handle_update_state_input_v2(Parameter_List[0].Integer_Value);

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

QCLI_Command_Status_t sw_mgnt_app_start_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_SW_MGNT_APP_START_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 11;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_sw_mgnt_app_start_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  software_mngt_init_v2(); 
  QCLI_Printf (qcli_m2m_stub_handle_v2 , "software init successful\n");

  if(Parameter_List != NULL)
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

QCLI_Command_Status_t sw_mgnt_get_v2(uint32_t Parameter_Count,
                                       QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_SW_MGNT_GET_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 16;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_sw_mgnt_get_v2(uint32_t Parameter_Count,
                                       QCLI_Parameter_t* Parameter_List, int app_id)
{
  int result = -1;
  int inst_id = -1;
  int res_id  = -1;
  int obj_id = -1;
  qapi_Net_LWM2M_Id_Info_v2_t id_info;
  int i;
  QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;

  memset(&id_info, 0x00, sizeof(qapi_Net_LWM2M_Id_Info_v2_t));

  if (Parameter_Count < 1 ) 
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Invalid!!");
    status = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    obj_id = Parameter_List[0].Integer_Value;
  }

  if ((Parameter_Count > 1) && Parameter_List[1].Integer_Is_Valid)
  {
    inst_id = Parameter_List[1].Integer_Value;
  }

  if ((Parameter_Count > 2) && Parameter_List[2].Integer_Is_Valid)
  {
    res_id = Parameter_List[2].Integer_Value;
  }

  if (obj_id != -1)
  {
    id_info.id_set |= QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info.object_ID = obj_id;
    if(inst_id != -1)
    {
      id_info.id_set |= QAPI_NET_LWM2M_INSTANCE_ID_E;
      id_info.instance_ID = inst_id;
      if( res_id != -1)
      {
        id_info.id_set |= QAPI_NET_LWM2M_RESOURCE_ID_E;
        id_info.resource_ID = res_id;
      }
    }
  }

  result = software_mngt_get_v2(&id_info);
  if (result == -1)
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "Error in software mngt get");
  }
  else
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "Get operation successful.");
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
  return status;
}

QCLI_Command_Status_t device_app_start_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_DEVICE_APP_START_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 14;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_device_app_start_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  device_stub_app_init_v2();

  QCLI_Printf (qcli_m2m_stub_handle_v2 , "Device stub application started\n");

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

QCLI_Command_Status_t device_app_set_resource_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_DEVICE_APP_SET_RESOURCE_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 15;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_device_app_set_resource_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int resource_id = -1;
  char *res_value = NULL;
  int res_len = -1;
  int result = -1;
  int i;
  QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;

  if(Parameter_Count < 2 || Parameter_List == NULL)
  {
    status = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if(Parameter_List[0].Integer_Is_Valid)
  {
    resource_id = Parameter_List[0].Integer_Value;
  }

  if(Parameter_List[1].String_Value == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Resource value passed is NULL");
    status = QCLI_STATUS_ERROR_E;
    goto end;
  }

  res_len = MAX_RES_LEN + 1;

  tx_byte_allocate(byte_pool_qcli, (VOID **)&res_value, res_len, TX_NO_WAIT);             

  if(res_value == NULL)
  {
    status = QCLI_STATUS_ERROR_E;
    goto end;
  }

  memset(res_value, 0, res_len);

  memcpy(res_value, (char*)Parameter_List[1].String_Value, res_len);

  QCLI_Printf(qcli_m2m_stub_handle_v2, "Setting resource value for %d \n", resource_id);

  result = set_device_resource_value_v2(resource_id, res_value, res_len);

  if(result == 0)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Value successfully set");
  }
  else if(result == -1)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Not able to set resource value.");
  }
  else if(result == -2)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Invalid parameters paased.");
  }
  else if(result == -3)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Memory allocation failure.");
  }
  else if(result == -8)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Not Supported.");
  }
  else
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Error happen.");
  }

  if(res_value)
  {
    tx_byte_release(res_value);
    res_value = NULL;
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
  return status;
}

QCLI_Command_Status_t cell_nw_app_start_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_CELL_CON_APP_START_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 18;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_cell_nw_app_start_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  cell_con_init_v2();
  QCLI_Printf (qcli_m2m_stub_handle_v2 , "cellular connectivity nw app started\n");

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

QCLI_Command_Status_t cell_nw_app_stop_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_CELL_CON_APP_STOP_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 19;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t proc_cell_nw_app_stop_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  cell_con_app_deregister_v2();
  QCLI_Printf (qcli_m2m_stub_handle_v2 , "cell nw conn connection profile app stopped\n");

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


QCLI_Command_Status_t cell_nw_get_v2(uint32_t Parameter_Count,
                                    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_CELL_CON_GET_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 20;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t proc_cell_nw_get_v2(uint32_t Parameter_Count,
                                    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int result = -1;
  int inst_id = -1;
  int res_id  = -1;
  int obj_id = -1;
  qapi_Net_LWM2M_Id_Info_v2_t id_info;
  int i;
  QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;

  memset(&id_info, 0x00, sizeof(qapi_Net_LWM2M_Id_Info_v2_t));

  if (Parameter_Count < 1 ) 
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Invalid!!");
    status = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    obj_id = Parameter_List[0].Integer_Value; 
  }

  if ((Parameter_Count > 1) && Parameter_List[1].Integer_Is_Valid)
  {
    inst_id = Parameter_List[1].Integer_Value;
  }

  if ((Parameter_Count > 2) && Parameter_List[2].Integer_Is_Valid)
  {
    res_id = Parameter_List[2].Integer_Value;
  }

  if(obj_id != -1)
  {
    id_info.id_set |= QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info.object_ID = obj_id;
    if(inst_id != -1)
    {
      id_info.id_set |= QAPI_NET_LWM2M_INSTANCE_ID_E;
      id_info.instance_ID = inst_id;
      if( res_id != -1)
      {
        id_info.id_set |= QAPI_NET_LWM2M_RESOURCE_ID_E;
        id_info.resource_ID = res_id;
      }
    }
  }

  result = cell_con_get_v2(&id_info);
  if (result == -1)
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "Error in cell nw conn get");
  }
  else
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "Get operation successful.");
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
  return status;
}

QCLI_Command_Status_t cell_nw_create_instance_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_CELL_CON_CREATE_INSTANCE_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 21;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t proc_cell_nw_create_instance_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int32_t ret_value= 0;
  int i;
  QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;

  ret_value = cell_con_create_instance_v2();
  
  if (ret_value == 0)
  { 
	QCLI_Printf (qcli_m2m_stub_handle_v2 , " [cell NW]INSTANCE CREATED SUCCESSFULLY \n");
  }
  else if(ret_value == -12)
  { 
	QCLI_Printf (qcli_m2m_stub_handle_v2 , "Instance already exists.\n");
  }
  else
  {
	QCLI_Printf (qcli_m2m_stub_handle_v2 , "Error in creating instance \n");
  } 
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
  return status;
}

QCLI_Command_Status_t cell_nw_delete_instance_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_CELL_CON_DELETE_INSTANCE_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 22;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t proc_cell_nw_delete_instance_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int32_t ret_value= 0;
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  ret_value = cell_con_delete_instance_v2();
  
  if (ret_value == 0)
  { 
	 QCLI_Printf (qcli_m2m_stub_handle_v2 , " [CELL NW]INSTANCE DELETED SUCCESSFULLY \n");
  }
  else
  {
	 QCLI_Printf (qcli_m2m_stub_handle_v2 , "Error in deleting instance \n");
  }
  

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


QCLI_Command_Status_t apninfo_app_start_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_APN_INFO_APP_START_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 23;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_apninfo_app_start_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  apn_info_init_v2();
  QCLI_Printf (qcli_m2m_stub_handle_v2 , "APN connection profile app started\n");

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

QCLI_Command_Status_t apninfo_app_stop_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_APN_INFO_APP_STOP_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 24;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t proc_apninfo_app_stop_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  apn_info_app_deregister_v2();
  QCLI_Printf (qcli_m2m_stub_handle_v2 , "APN connection profile app stopped\n");

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


QCLI_Command_Status_t apninfo_get_v2(uint32_t Parameter_Count,
                                    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_APN_INFO_GET_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 25;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t proc_apninfo_get_v2(uint32_t Parameter_Count,
                                    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int result = -1;
  int inst_id = -1;
  int res_id  = -1;
  int obj_id = -1;
  qapi_Net_LWM2M_Id_Info_v2_t id_info;
  int i;
  QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;

  memset(&id_info, 0x00, sizeof(qapi_Net_LWM2M_Id_Info_v2_t));

  if (Parameter_Count < 1 ) 
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Invalid!!");
    status = QCLI_STATUS_USAGE_E;
    goto end;
  }

  if (Parameter_List[0].Integer_Is_Valid)
  {
    obj_id = Parameter_List[0].Integer_Value; 
  }

  if ((Parameter_Count > 1) && Parameter_List[1].Integer_Is_Valid)
  {
    inst_id = Parameter_List[1].Integer_Value;
  }

  if ((Parameter_Count > 2) && Parameter_List[2].Integer_Is_Valid)
  {
    res_id = Parameter_List[2].Integer_Value;
  }

  if(obj_id != -1)
  {
    id_info.id_set |= QAPI_NET_LWM2M_OBJECT_ID_E;
    id_info.object_ID = obj_id;
    if(inst_id != -1)
    {
      id_info.id_set |= QAPI_NET_LWM2M_INSTANCE_ID_E;
      id_info.instance_ID = inst_id;
      if( res_id != -1)
      {
        id_info.id_set |= QAPI_NET_LWM2M_RESOURCE_ID_E;
        id_info.resource_ID = res_id;
      }
    }
  }

  result = apn_info_get_v2(&id_info);
  if (result == -1)
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "Error in apn connection profile get");
  }
  else
  {
    QCLI_Printf (qcli_m2m_stub_handle_v2 , "Get operation successful.");
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
  return status;
}

QCLI_Command_Status_t apninfo_create_instance_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_APN_INFO_CREATE_INSTANCE_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 26;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t proc_apninfo_create_instance_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int32_t ret_value= 0;
  int i;
  QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;

  if (Parameter_Count < 1 ) 
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Invalid!!");
    ret_value = QCLI_STATUS_USAGE_E;
    goto end;
  }

  ret_value = apn_info_create_instance_v2(Parameter_List[0].Integer_Value);
  
  if (ret_value == 0)
  { 
	QCLI_Printf (qcli_m2m_stub_handle_v2 , " [APN CONN]INSTANCE CREATED SUCCESSFULLY \n");
  }
  else if(ret_value == -12)
  { 
	QCLI_Printf (qcli_m2m_stub_handle_v2 , "Instance already exists.\n");
  }
  else
  {
	QCLI_Printf (qcli_m2m_stub_handle_v2 , "Error in creating instance \n");
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
  return status;
}

QCLI_Command_Status_t apninfo_delete_instance_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = LWM2M_APN_INFO_DELETE_INSTANCE_EVT_v2;
  cmd_parser_temp->Command = (QCLI_Command_t *)&m2m_stub_cmd_list_v2[0];
  cmd_parser_temp->Command_Index = 27;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}

QCLI_Command_Status_t proc_apninfo_delete_instance_v2(uint32_t Parameter_Count,
    QCLI_Parameter_t* Parameter_List, int app_id)
{
  int32_t ret_value= 0;
  int i;
  QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

  if (Parameter_Count < 1 ) 
  {
    QCLI_Printf(qcli_m2m_stub_handle_v2, "Invalid!!");
    result = QCLI_STATUS_USAGE_E;
    goto end;
  }
  ret_value = apn_info_delete_instance_v2(Parameter_List[0].Integer_Value);
  
  if (ret_value == 0)
  { 
	 QCLI_Printf (qcli_m2m_stub_handle_v2 , " [APN CONN]INSTANCE DELETED SUCCESSFULLY \n");
  }
  else
  {
	 QCLI_Printf (qcli_m2m_stub_handle_v2 , "Error in deleting instance \n");
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

