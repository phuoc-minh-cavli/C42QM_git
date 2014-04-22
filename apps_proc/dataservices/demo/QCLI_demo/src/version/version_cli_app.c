/*!
  @file
  version_cli_app_module.c

  @brief
  Version test application functionality 
*/

/*===========================================================================

  Copyright (c) 2019 Qualcomm Technologies, Inc. All Rights Reserved

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
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#include "qcli.h"
#include "qcli_api.h"
#include "version_cli_app.h"
#include "qapi_version.h"

#include "qurt_thread.h"
#include "qurt_error.h"
#include "multi_app_test.h"

extern bool thrd_running[NO_OF_APPS];

/* Forward declarations */
static QCLI_Command_Status_t version_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
QCLI_Command_Status_t get_version(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id);

const QCLI_Command_t version_cli_app_cmd_list[] =
{
    /* Command_Function, Start_Thread, Command_String, Usage_String, Description */
    {version_command_handler, false, "getversion","[-app <app_id: 0/1>] [-cat <category>]\n",
                              "get kernel versions ",NULL}
};

const QCLI_Command_Group_t version_cli_app_cmd_group =
{
    "version_cli_app",              /* Group_String: will display cmd prompt as "Net> " */
    sizeof(version_cli_app_cmd_list)/sizeof(version_cli_app_cmd_list[0]),   /* Command_Count */
    version_cli_app_cmd_list        /* Command_List */
};

QCLI_Group_Handle_t qcli_version_cli_app_handle;     /* Handle for Version Command Group. */

/*****************************************************************************
 * This function is used to register the Atfwd Command Group with QCLI.
 *****************************************************************************/
void Initialize_Version_Cli_App_Demo(void)
{
  /* Attempt to reqister the Command Groups with the qcli framework.*/
  qcli_version_cli_app_handle = QCLI_Register_Command_Group(NULL, &version_cli_app_cmd_group);

  if (qcli_version_cli_app_handle)
    QCLI_Printf(qcli_version_cli_app_handle, "Version_Cli_App Registered\n");

  return;
}

/*****************************************************************************
 *            [0]       
 * Version appversion
 * Version kernelversion
 *
 *****************************************************************************************/
static QCLI_Command_Status_t version_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_version_cli_app_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = VERSION_COMMAND_HANDLER_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&version_cli_app_cmd_list[0];
  cmd_parser_temp->Command_Index = 4;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_version_command_handler(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;
    int i;

    if (Parameter_Count == 0)
    {
      status = QCLI_STATUS_USAGE_E;
      goto end;
    }

    status = get_version(Parameter_Count, &Parameter_List[0], app_id);

end:
  if (Parameter_List != NULL)
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
  return status;
}

QCLI_Command_Status_t get_version(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
  QCLI_Command_Status_t result =  QCLI_STATUS_ERROR_E;
  qapi_Version_Object_t version;
  int                   ret = QAPI_ERROR, i = 0, p = 0;
  int                   cat = 0;   //default for cat DATA
  memset(&version, 0x00, sizeof(version));

  for (i = 0; i < Parameter_Count; i++)
  {
    if (strncmp((char*)Parameter_List[i].String_Value, "-cat", 4) == 0)
    {
      ++i;
      if(Parameter_List[i].Integer_Is_Valid)
      {
        cat = Parameter_List[i].Integer_Value;
        if(cat < 0 || cat >= (int)QAPI_VERSION_CATEGORY_CAT_MAX)
        {
          QCLI_Printf(qcli_version_cli_app_handle, "category %d is out of range [0~%d]\n", cat, QAPI_VERSION_CATEGORY_CAT_MAX-1);
          result = QCLI_STATUS_USAGE_E;
          goto end;
        }
      }
    }
  }

  ret = qapi_Version_Get_Kernel_Version((qapi_Version_Category_t)cat, &version);
  if(QAPI_OK == ret)
  {
    QCLI_Printf(qcli_version_cli_app_handle, "kernel version for category %d is: %d.%d.%d, version string is: %s\n",
                 cat, version.major_num, version.minor_num, version.patch_num, version.ver_str);
    result = QCLI_STATUS_SUCCESS_E;
  }
  else
  {
    QCLI_Printf(qcli_version_cli_app_handle, "get kernel version failed, return value is %d.\n", ret);
    result = QCLI_STATUS_ERROR_E;
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

