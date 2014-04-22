/*!
  @file
  atfwd_cli_app.c

  @brief
  ATFWD test application functionality 
*/

/*===========================================================================

  Copyright (c) 2017 - 2020 Qualcomm Technologies, Inc. All Rights Reserved

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
#include "atfwd_cli_app.h"

#include "qapi_atfwd.h"
#include "qapi_socket.h"

#include "qurt_thread.h"
#include "qurt_error.h"

#include "multi_app_test.h"

#define QCLI_AT_CMD_RESP_LEN_MAX_V01 100

#define QCLI_AT_URC_LEN_MAX_V01 64

#define QCLI_AT_RESULT_ERROR_V01 0 /**<  Result ERROR. 
                                         This is to be set in case of ERROR or CME ERROR. 
                                         The response buffer contains the entire details. */
#define QCLI_AT_RESULT_OK_V01 1    /**<  Result OK. This is to be set if the final response 
                                         must send an OK result code to the terminal. 
                                         The response buffer must not contain an OK.  */

#define QCLI_AT_MASK_EMPTY 0  /**<  Denotes a feed back mechanism for AT reg and de-reg */
#define QCLI_AT_MASK_NA_V01 1 /**<  Denotes presence of Name field  */
#define QCLI_AT_MASK_EQ_V01 2 /**<  Denotes presence of equal (=) operator  */
#define QCLI_AT_MASK_QU_V01 4 /**<  Denotes presence of question mark (?)  */
#define QCLI_AT_MASK_AR_V01 8 /**<  Denotes presence of trailing argument operator */

#define ATFWD_STACK_SIZE (1024*2)
#define ATFWD_PRIORITY 185

extern bool thrd_running[NO_OF_APPS];

/* Forward declarations */
static QCLI_Command_Status_t atfwd(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

const QCLI_Command_t atfwd_cli_app_cmd_list[] =
{
    /* Command_Function, Start_Thread, Command_String, Usage_String, Description */
    {atfwd, false, "atfwd", " [-app <app_id: 0/1>] reg <at_cmds>\n"
                            " [-app <app_id: 0/1>] dereg <at_cmds>\n"
                            " [-app <app_id: 0/1>] sendresp <atcmd_name> <response_buf> <result>\n"
                            " [-app <app_id: 0/1>] sendurc <atcmd_name> <response_buf>\n" , "Perform ATFWD operations",NULL}
};

const QCLI_Command_Group_t atfwd_cli_app_cmd_group =
{
    "atfwd_cli_app",              /* Group_String: will display cmd prompt as "Net> " */
    sizeof(atfwd_cli_app_cmd_list)/sizeof(atfwd_cli_app_cmd_list[0]),   /* Command_Count */
    atfwd_cli_app_cmd_list        /* Command_List */
};

QCLI_Group_Handle_t qcli_atfwd_cli_app_handle;     /* Handle for ATFWD Command Group. */

/*****************************************************************************
 * This function is used to register the Atfwd Command Group with QCLI.
 *****************************************************************************/
void Initialize_Atfwd_Cli_App_Demo(void)
{
  /* Attempt to reqister the Command Groups with the qcli framework.*/
  qcli_atfwd_cli_app_handle = QCLI_Register_Command_Group(NULL, &atfwd_cli_app_cmd_group);

  if (qcli_atfwd_cli_app_handle)
    QCLI_Printf(qcli_atfwd_cli_app_handle, "Atfwd_Cli_App Registered\n");

  return;
}

/*****************************************************************************
 * This function is used as a callback for extest command.
 *****************************************************************************/

void at_fwd_extest_callback(boolean is_reg, char *atcmd_name, uint8* at_fwd_params, uint8 mask, uint32 at_handle)
{
	QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
	qapi_atfwd_send_resp(atcmd_name, NULL, QCLI_AT_RESULT_OK_V01);
	return;
}

/*****************************************************************************
 * This function is used as a callback to the client.
 *****************************************************************************/

void at_fwd_callback(boolean is_reg, char *atcmd_name, uint8* at_fwd_params, uint8 mask, uint32 at_handle)
{
    int32_t e = -1;
    int i = -1;
    char *at_resp = NULL;
    static int total_cmd_reg = 0;

    QCLI_Printf(qcli_atfwd_cli_app_handle, "In at_fwd_callback\n");
    if (is_reg)
    {
      if (mask == QCLI_AT_MASK_EMPTY)
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Last registered AT cmd %s\n",atcmd_name);

        total_cmd_reg+=at_handle;

        QCLI_Printf(qcli_atfwd_cli_app_handle, "Total Registered AT cmds in ATFwD"
                    "with this callback %d \n",total_cmd_reg);
        return;
      }
    
    QCLI_Printf(qcli_atfwd_cli_app_handle, "Atcmd %s is registered\n",atcmd_name);

    if (!strncasecmp(atcmd_name, "+createsock", strlen(atcmd_name)))
    {
      if (mask == (QCLI_AT_MASK_NA_V01 | QCLI_AT_MASK_EQ_V01 | QCLI_AT_MASK_AR_V01))
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n", atcmd_name);
        QCLI_Printf(qcli_atfwd_cli_app_handle, "The parameters received are %s\n", at_fwd_params);

        if (!strncasecmp((char *)at_fwd_params, "0", strlen((char *)at_fwd_params)))
        {
          i = qapi_socket(AF_UNSPEC, SOCK_DGRAM, 0);
        }
        else if (!strncasecmp((char *)at_fwd_params, "2", strlen((char *)at_fwd_params)))
        {
          i = qapi_socket(AF_INET, SOCK_DGRAM, 0);
        }
        else if (!strncasecmp((char *)at_fwd_params, "3", strlen((char *)at_fwd_params)))
        {
          i = qapi_socket(AF_INET6, SOCK_DGRAM, 0);
        }
        else if (!strncasecmp((char *)at_fwd_params, "4", strlen((char *)at_fwd_params)))
        {
          i = qapi_socket(AF_INET_DUAL46, SOCK_DGRAM, 0);
        }
        else
        {
          QCLI_Printf(qcli_atfwd_cli_app_handle, "Received AT+CREATESOCK with invalid parameters\n");
          QCLI_Printf(qcli_atfwd_cli_app_handle, "Sending an Error Response...\n");
          e = qapi_atfwd_send_resp(atcmd_name, "", QCLI_AT_RESULT_ERROR_V01);
        }

        at_resp = (char*)malloc(QCLI_AT_CMD_RESP_LEN_MAX_V01);
        if (at_resp == NULL)
        {
          QCLI_Printf(qcli_atfwd_cli_app_handle, "Memory allocation failure for at_resp\n");
          return;
        }
        memset (at_resp, 0, QCLI_AT_CMD_RESP_LEN_MAX_V01);

        snprintf(at_resp, QCLI_AT_CMD_RESP_LEN_MAX_V01, "SOCKETFD = %d",i);

        e = qapi_atfwd_send_resp(atcmd_name , at_resp , QCLI_AT_RESULT_OK_V01);

        if (at_resp != NULL)
        {
          free(at_resp);
          at_resp = NULL;
        }
      }
      else
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Received AT+CREATESOCK without parameters which is not supported\n");
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Sending an Error Response...\n");
        e = qapi_atfwd_send_resp(atcmd_name, "", QCLI_AT_RESULT_ERROR_V01);
      }
    }
    else if (!strncasecmp(atcmd_name, "+ex1", strlen(atcmd_name)))
    {
      if (mask == (QCLI_AT_MASK_NA_V01))
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
        e = qapi_atfwd_send_resp(atcmd_name, "Received AT cmd +ex1 with Only Name", QCLI_AT_RESULT_OK_V01);
      }
      else if (mask == (QCLI_AT_MASK_NA_V01 | QCLI_AT_MASK_QU_V01))
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
        e = qapi_atfwd_send_resp(atcmd_name , "Received AT cmd +ex1 with ? " , QCLI_AT_RESULT_OK_V01);
      }
      else if (mask == (QCLI_AT_MASK_NA_V01 | QCLI_AT_MASK_EQ_V01| QCLI_AT_MASK_QU_V01))
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
        e = qapi_atfwd_send_resp(atcmd_name , "Received AT cmd +ex1 with =? " , QCLI_AT_RESULT_OK_V01);
      }
      else if (mask == (QCLI_AT_MASK_NA_V01 | QCLI_AT_MASK_EQ_V01| QCLI_AT_MASK_AR_V01))
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
        QCLI_Printf(qcli_atfwd_cli_app_handle, "The parameters received are %s\n",at_fwd_params);
        e = qapi_atfwd_send_resp(atcmd_name , "Received AT cmd +ex1 with Arguments" , QCLI_AT_RESULT_OK_V01);
      }
    }
    else if (!strncasecmp(atcmd_name, "+ex2", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      // Test the timer bail out scenario by not sending any response for the atcmd +ex2
      // e = qapi_atfwd_send_resp(atcmd_name, "buf less than max and response as complete", QCLI_AT_RESULT_OK_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex3", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      e = qapi_atfwd_send_resp(atcmd_name, "", QCLI_AT_RESULT_ERROR_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex4", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      e = qapi_atfwd_send_resp(atcmd_name, "", QCLI_AT_RESULT_OK_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex5", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      e = qapi_atfwd_send_resp(atcmd_name, "81-abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", QCLI_AT_RESULT_OK_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex6", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      e = qapi_atfwd_send_resp(atcmd_name, "97-abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnop", QCLI_AT_RESULT_OK_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex7", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      e = qapi_atfwd_send_resp(atcmd_name, "98-abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopq", QCLI_AT_RESULT_OK_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex8", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      e = qapi_atfwd_send_resp(atcmd_name, "108-abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", QCLI_AT_RESULT_OK_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex9", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      e = qapi_atfwd_send_resp(atcmd_name, "196-abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghij", QCLI_AT_RESULT_OK_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex10", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      e = qapi_atfwd_send_resp(atcmd_name, "197-abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk", QCLI_AT_RESULT_OK_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex11", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      e = qapi_atfwd_send_resp(atcmd_name, "212-abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", QCLI_AT_RESULT_OK_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex12", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      e = qapi_atfwd_send_resp(atcmd_name, "394-abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", QCLI_AT_RESULT_OK_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex13", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      e = qapi_atfwd_send_resp(atcmd_name, "at error with response as complete", QCLI_AT_RESULT_ERROR_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex14", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      e = qapi_atfwd_send_resp(atcmd_name, "498-abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", QCLI_AT_RESULT_OK_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex15", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      e = qapi_atfwd_send_resp(atcmd_name, "", QCLI_AT_RESULT_ERROR_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex16", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      e = qapi_atfwd_send_resp(atcmd_name, "", QCLI_AT_RESULT_OK_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex17", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      e = qapi_atfwd_send_resp(atcmd_name, "at error with response as complete", QCLI_AT_RESULT_ERROR_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex18", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      e = qapi_atfwd_send_resp(atcmd_name, "", QCLI_AT_RESULT_ERROR_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex19", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      e = qapi_atfwd_send_resp(atcmd_name, "at error with response as complete", QCLI_AT_RESULT_ERROR_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex20", strlen(atcmd_name)))
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
      e = qapi_atfwd_send_resp(atcmd_name, "", QCLI_AT_RESULT_ERROR_V01);
    }
    else if (!strncasecmp(atcmd_name, "+ex21", strlen(atcmd_name)))
    {
      /* ATFWD only support single DAM, and using callback function as identifiy of userspace memory.
       * So, when there is already command registered, for DAM usage, ATFWD should block DAM change callback function.
	   * Current case verify changing callback function while there is already command registered.
	   * For DAM usage, register should return error.
	   * For Kernel usage, register should success.
	   */
      uint32_t cmd_result = QCLI_AT_RESULT_ERROR_V01;
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully received callback of Atcmd %s\n",atcmd_name);
	  
	  e = qapi_atfwd_reg("+extest", at_fwd_extest_callback);
      if (e != QAPI_OK)
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Unable to Process +extext Registeration\n");
        cmd_result = QCLI_AT_RESULT_ERROR_V01;
      }
      else
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Completed Processing +extext Registeration\n");
		cmd_result = QCLI_AT_RESULT_OK_V01;
      }
      e = qapi_atfwd_send_resp(atcmd_name, "", cmd_result);
    }
    else 
    {
      e = QAPI_ERR_NO_ENTRY;
    }

    if (e == QAPI_OK)
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully sent response\n");
    }
    else if (e == QAPI_ERR_INVALID_PARAM)
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Failed to send response as the result type is unsupported\n");
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Please send the response with supported "
                                             "result type : Supported are 0, 1\n");
    }
    else if (e == QAPI_ERR_NOT_SUPPORTED)
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Failed to send response\n");
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Trying to send response for %s " 
                                              "when not expected\n", atcmd_name);
    }
    else if (e == QAPI_ERR_NO_ENTRY)
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Please send the response for "
                                             "the AT Cmd %s from the QCLI\n", atcmd_name);
    }
    else
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Failed to send response\n");
    }
  }
  else
  {
    if (mask != 0)
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Unsuccessful Registeration\n");
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Trying to register : More than limit OR Unsupported Cmds OR Already Registered Cmds\n");
  }
    else
    {
      total_cmd_reg--;
      QCLI_Printf(qcli_atfwd_cli_app_handle, "De-registered AT cmd\n");
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Remaining AT cmds reg with "
                                             "this callback %d\n", total_cmd_reg);
    }
  }
  return;
}

/******************************************************************************************
 *       [0]             [1]          [2]             [3]
 * Atfwd reg          <at_cmds>      
 * Atfwd dereg        <at_cmds>      
 * Atfwd sendresp    <atcmd_name>  <response_buf>   <result>      
 * Atfwd sendurc     <atcmd_name>  <response_buf>
 *
 *****************************************************************************************/
static QCLI_Command_Status_t atfwd(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_atfwd_cli_app_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = ATFWD_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&atfwd_cli_app_cmd_list[0];
  cmd_parser_temp->Command_Index = 4;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_atfwd(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    int32_t e = -1;
    char *cmd;
    char *at_cmd_name_list, *buf, *at_urc;
    char *atcmd_name;
    uint32_t result = 0;
    int i;
    QCLI_Command_Status_t status = QCLI_STATUS_SUCCESS_E;

    if (Parameter_Count == 0)
    {
      goto parm_error;
    }

    cmd = (char*)Parameter_List[0].String_Value;

    /*
     * atfwd -app 0 reg +ex1;+ex2
     */
    if (strncmp(cmd, "reg", 3) == 0)
    {
      if (Parameter_Count != 2)
      {
          goto parm_error;
      }

      at_cmd_name_list = (char*)Parameter_List[1].String_Value;

      QCLI_Printf(qcli_atfwd_cli_app_handle, "Received registeration request\n");

      e = qapi_atfwd_reg(at_cmd_name_list, at_fwd_callback);

      if (e != QAPI_OK)
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Unable to Process the received Registeration Request\n");
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }
      else
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Completed Processing the received Registeration Request\n");
      }
    }

    /*
     * atfwd -app 0 dereg +ex1;+ex2
     */
    else if (strncmp(cmd, "dereg", 5) == 0)
    {
      if (Parameter_Count != 2)
      {
          goto parm_error;
      }

      at_cmd_name_list = (char*)Parameter_List[1].String_Value;

      QCLI_Printf(qcli_atfwd_cli_app_handle, "Received De-registeration request\n");

      e = qapi_atfwd_dereg(at_cmd_name_list);

      if (e != QAPI_OK)
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Unable to Process the received De-Registeration Request\n");
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }
      else
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Completed Processing the received De-Registeration Request\n");
      }
    }

    /*
     * atfwd -app 0 sendresponse +ex1 testresp 0
     */
    else if (strncmp(cmd, "sendresp", 8) == 0)
    {
      if (Parameter_Count < 3 || Parameter_Count > 4)
      {
        goto parm_error;
      }

      atcmd_name = (char*)Parameter_List[1].String_Value;

      if (Parameter_Count == 3)
      {
        if (Parameter_List[2].Integer_Is_Valid)
        {
          result = Parameter_List[2].Integer_Value;
        }
        else
        {
          goto parm_error;
        }
        buf = "";
      }
      else if(Parameter_Count == 4)
      {
        buf = (char*)Parameter_List[2].String_Value;

        if (Parameter_List[3].Integer_Is_Valid)
        {
          result = Parameter_List[3].Integer_Value;
        }
      }

      e = qapi_atfwd_send_resp(atcmd_name, buf, result);

      if (e == QAPI_OK)
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully sent response\n");
      }
      else if (e == QAPI_ERR_INVALID_PARAM)
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Failed to send response as the result type is unsupported\n");
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Please send the response with supported "
                                                "result type : Supported are 0, 1\n");
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }
      else if (e == QAPI_ERR_NOT_SUPPORTED)
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Failed to send response\n");
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Trying to send response for %s " 
                                                "when not expected\n", atcmd_name);
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }
      else
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Failed to send response\n");
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }

    }

    /*
    * atfwd -app 0 sendurc testurcresp 0
    */
    else if (strncmp(cmd, "sendurc", 7) == 0)
    {
      if (Parameter_Count != 3)
      {
        goto parm_error;
      }

      atcmd_name = (char*)Parameter_List[1].String_Value;

      at_urc = (char*)Parameter_List[2].String_Value;

      e = qapi_atfwd_send_urc_resp(atcmd_name, at_urc);

      if (e == QAPI_OK)
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Successfully sent URC\n");
      }
      else
      {
        QCLI_Printf(qcli_atfwd_cli_app_handle, "Failed to send URC\n");
        status = QCLI_STATUS_ERROR_E;
        goto end;
      }
    }
    else
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "Invalid subcommand: %s\n", cmd);
      goto parm_error;
    }

    if (e)
    {
      QCLI_Printf(qcli_atfwd_cli_app_handle, "%s failed\n", cmd);
      status = QCLI_STATUS_ERROR_E;
      goto end;
    }
    else
    {
      status = QCLI_STATUS_SUCCESS_E;
      goto end;
    }

parm_error:
    status = QCLI_STATUS_USAGE_E;
    goto end;

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
