/*****************************************************************************
  Copyright (c) 2020 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#ifndef __CONTROLLER_THREAD_H__
#define __CONTROLLER_THREAD_H__

#include <stdint.h>
#include "qapi_location.h"

typedef enum
{
    LOCAL_CONTROLLER_MSG_ID_MIN,

    LOCAL_CONTROLLER_MSG_ID_INIT,
    LOCAL_CONTROLLER_MSG_ID_REPORT_LOCATION,

    LOCAL_CONTROLLER_MSG_ID_MAX
} LocalControllerMsgId;

typedef struct
{
    char* argName;
    char* argValue;

} LocalControllerCmdArg;

typedef struct
{
    uint32_t cmdIndex;
    char* cmdName;

    // Array of command arguments
    uint32_t cmdArgCount;
    LocalControllerCmdArg* cmdArgArray;

} LocalControllerCmd;

typedef struct
{
    uint32_t cmdCount;
    LocalControllerCmd* cmdArray;

} LocalControllerCmdArray;

void local_controller_task_entry(unsigned long args);
char* local_controller_get_task_name();
int local_controller_get_task_id();

// Send messages to this thread, which will be processed after a
// context switch to this thread.
void local_controller_send_msg_init();
void local_controller_send_msg_report_location(qapi_Location_t location);

#endif /* __CONTROLLER_THREAD_H__ */
